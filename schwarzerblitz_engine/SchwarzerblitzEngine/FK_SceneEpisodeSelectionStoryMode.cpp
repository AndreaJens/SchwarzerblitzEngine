#include "FK_SceneEpisodeSelectionStoryMode.h"
#include "ExternalAddons.h"
#include "FK_AchievementManager.h"
#include <filesystem>

using namespace irr;

namespace fk_engine{
	FK_SceneEpisodeSelectionStoryMode::FK_SceneEpisodeSelectionStoryMode(){
		initialize();
	}
	void FK_SceneEpisodeSelectionStoryMode::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, 
		FK_Options* newOptions,	s32 currentEpisodeIndex){
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		episodeDirectory = applicationPath + FK_SceneEpisodeSelectionStoryMode::EpisodeMainDirectory;
		storyCommonMediaPath = mediaPath + "scenes\\story\\common\\system\\";
		initialize();
		episodeIndex = currentEpisodeIndex;
		setupIrrlichtDevice();
		FK_SceneWithInput::readSaveFile();
		setupJoypad();
		setupInputMapper();
		setupSoundManager();
		setupBGM();
		loadEpisodeList();
		setupGraphics();
		setupChapterMenu();
		setupSafeContentMenu();
	}
	void FK_SceneEpisodeSelectionStoryMode::initialize(){
		now = 0;
		then = 0;
		delta_t_ms = 0;
		cycleId = 0;
		lastInputTime = 0;
		backToMenu = false;
		episodeSelected = false;
	}
	void FK_SceneEpisodeSelectionStoryMode::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		if (cycleId == 0){
			cycleId = 1;
			delta_t_ms = 0;
			bgm.play();
		}
		else{
			if (chapterSelectMenu->isActive()) {
				updateChapterMenu();
			}else if (safeContentMenu->isActive() || safeContentInformation->isAnimating()) {
				updateSafeContentMenu();
			}
			else {
				updateInput();
			}
		}
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		drawAll();
		if (hasToSetupJoypad()){
			drawJoypadSetup(now);
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}
	void FK_SceneEpisodeSelectionStoryMode::dispose(){
		// stop bgm
		bgm.stop();
		// clear episodes
		episodes.clear();
		// delete sound manager
		delete soundManager;
		// clear the scene of all scene nodes
		FK_SceneWithInput::dispose();
	}
	FK_Scene::FK_SceneType FK_SceneEpisodeSelectionStoryMode::nameId(){
		return FK_Scene::FK_SceneType::SceneStoryModeSelection;
	}
	bool FK_SceneEpisodeSelectionStoryMode::isRunning(){
		return ((!backToMenu && !episodeSelected) || safeContentInformation->isAnimating());
	}
	void FK_SceneEpisodeSelectionStoryMode::setupGraphics(){
		// basic
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		// fonts
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		mediumFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		captionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		// textures
		background = driver->getTexture((commonResourcesPath + "common_menu_items\\menuBackground_base.png").c_str());
		lockedEpisodeIcon = driver->getTexture((storyCommonMediaPath + "lockedEpisode.png").c_str());
		for each(auto episode in episodes){
			//std::string path = episodeDirectory + episode.getEpisodeRelativePath() + "\\banner.png";
			std::string path = episode.getEpisodeFullPath() + "\\banner.png";
			episodeIcons.push_back(
				driver->getTexture(path.c_str())
				);
		}
		completionBoxTex = driver->getTexture((storyCommonMediaPath + "completionBox.png").c_str());
		// preview
		previewFrame = driver->getTexture((storyCommonMediaPath + "previewFrame.png").c_str());
		lockedEpisodePreview = driver->getTexture((storyCommonMediaPath + "lockedPreview.jpg").c_str());
		// arrows
		arrowUp_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowU.png").c_str());
		arrowDown_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowD.png").c_str());
		arrowFrequency = 500;
		// episode complete tick
		episodeCompleteTick = driver->getTexture((storyCommonMediaPath + "episodeCompleteTick.png").c_str());
	}
	void FK_SceneEpisodeSelectionStoryMode::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}
	void FK_SceneEpisodeSelectionStoryMode::setupBGM(){
		systemBGMPath = commonResourcesPath + "system_bgm\\";
		bgmName = "menu(astral)_cut2.ogg";
		bgmVolume = 45.0f;
		bgmPitch = 1.0f;
		std::string filePath = systemBGMPath + bgmName;
		if (!bgm.openFromFile(filePath.c_str()))
			return;
		bgm.setLoop(true);
		bgm.setVolume(bgmVolume * gameOptions->getMusicVolume());
		bgm.setPitch(bgmPitch);
	}

	/* setup chapter select menu */
	void FK_SceneEpisodeSelectionStoryMode::setupChapterMenu() {
		std::vector<std::string> menuItems;
		if (episodes[episodeIndex].getCompletionPercentage() >= 100.0f ||
			(episodes[episodeIndex].getCompletionPercentage() <= 0.0f &&
			completedEpisodes[episodeIndex])) {
			menuItems = {
				" Restart episode ",
				"Cancel",
			};
		}
		else if (episodes[episodeIndex].getCompletionPercentage() > 0.0f) {
			menuItems = {
				"Continue",
				" Restart episode ",
				"Cancel",
			};
		}else{
			menuItems = {
				" Play episode ",
				"Cancel",
			};
		}
		chapterSelectMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		chapterSelectMenu->setViewport(core::position2di(-1, -1), screenSize);
	}

	/* activate/deactivate chapter menu */
	void FK_SceneEpisodeSelectionStoryMode::activateChapterMenu() {
		delete chapterSelectMenu;
		setupChapterMenu();
		chapterSelectMenu->setActive(true);
		chapterSelectMenu->setVisible(true);
		chapterSelectMenu->reset();
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		Sleep(200);
	}
	void FK_SceneEpisodeSelectionStoryMode::deactivateChapterMenu() {
		chapterSelectMenu->setActive(false);
		chapterSelectMenu->setVisible(false);
		inputReceiver->reset();
		if (!safeContentMenu->isActive()) {
			Sleep(200);
		}
	}

	/* update chapter menu */
	void FK_SceneEpisodeSelectionStoryMode::updateChapterMenu() {
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0));
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus());
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1));
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus());
		}
		u32 inputButtonsForMenu = player1input->getPressedButtons();
		inputButtonsForMenu |= player2input->getPressedButtons();
		if ((inputButtonsForMenu & FK_Input_Buttons::Cancel_Button) != 0) {
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			deactivateChapterMenu();
			return;
		}
		else {
			chapterSelectMenu->update(inputButtonsForMenu, false);
			if (chapterSelectMenu->itemIsSelected()) {
				s32 chapterMenuIndex = chapterSelectMenu->getIndex();
				chapterSelectMenu->resetSelection();
				switch (chapterMenuIndex) {
				case 0:
					soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
					if (episodes[episodeIndex].getCompletionPercentage() > 0.0f &&
						episodes[episodeIndex].getCompletionPercentage() < 100.0f) {
						episodeSelected = true;
					}else{
						//if (fk_addons::fileExists(
						//	(episodeDirectory + episodes[episodeIndex].getEpisodeRelativePath() + "\\" +
						//		FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName).c_str())) {
						//	deactivateChapterMenu();
						//	activateSafeContentMenu();
						if (fk_addons::fileExists(
							(episodes[episodeIndex].getEpisodeFullPath() + "\\" +
								FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName).c_str())) {
							deactivateChapterMenu();
							activateSafeContentMenu();
						}else{
							episodes[episodeIndex].resetProgress();
							episodeSelected = true;
						}
					}
					break;
				case 1:
					if (episodes[episodeIndex].getCompletionPercentage() > 0.0f &&
						episodes[episodeIndex].getCompletionPercentage() < 100.0f) {
						soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						if (fk_addons::fileExists(
							(episodes[episodeIndex].getEpisodeFullPath() + "\\" +
								FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName).c_str())) {
							deactivateChapterMenu();
							activateSafeContentMenu();
						}
						else {
							episodes[episodeIndex].resetProgress();
							episodeSelected = true;
						}
					}else{
						episodeSelected = false;
						soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					}
					break;
				case 2:
					episodeSelected = false;
					soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					break;
				default:
					break;
				}
				deactivateChapterMenu();
			}
		}
	}

	/* setup chapter select menu */
	void FK_SceneEpisodeSelectionStoryMode::setupSafeContentMenu() {
		std::vector<std::string> safeContentPromptMessage = { 
			"This chapter contains scenes that could be mildly unsettling and thus unsuitable for a stream or for a small part of the audience.",
			"Would you like to play it in its original form?",
			"* There are no penalties for playing the edited version.",
			"* This choice can be undone by restarting the chapter." };
		std::vector<std::string> menuItems;
		menuItems = {
			" Original version ",
			" Edited version ",
			"Cancel",
		};
		safeContentMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		safeContentMenu->setViewport(core::position2di(-1, -1), screenSize);
		s32 frameHeight = safeContentMenu->getFrame().getHeight();
		safeContentMenu->setPosition(safeContentMenu->getFrame().UpperLeftCorner.X, screenSize.Height / 2);
		f32 scaleFactorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		s32 width = screenSize.Width * 3 / 4;
		s32 x = (screenSize.Width - width) / 2;
		s32 height = screenSize.Height / 16 * (s32)safeContentPromptMessage.size();
		s32 y = safeContentMenu->getFrame().UpperLeftCorner.Y - height;
		core::rect<s32> frame(x, y, x + width, y + height);
		//f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		//core::rect<s32> frame(50, 140, 590, 220);
		safeContentInformation = new FK_DialogueWindow(device, frame, "windowskin.png", commonResourcesPath,
			1.f,
			std::string(), safeContentPromptMessage, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
	}

	/* activate/deactivate chapter menu */
	void FK_SceneEpisodeSelectionStoryMode::activateSafeContentMenu() {
		delete safeContentMenu;
		delete safeContentInformation;
		setupSafeContentMenu();
		safeContentMenu->setActive(true);
		safeContentMenu->setVisible(true);
		safeContentInformation->setVisibility(true);
		safeContentInformation->setActive(true);
		safeContentInformation->open(300);
		//safeContentInformation->close();
		safeContentMenu->reset();
		//soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		//Sleep(200);
	}
	void FK_SceneEpisodeSelectionStoryMode::deactivateSafeContentMenu() {
		safeContentMenu->setActive(false);
		safeContentMenu->setVisible(false);
		safeContentInformation->close(200);
		inputReceiver->reset();
		//Sleep(200);
	}

	/* update chapter menu */
	void FK_SceneEpisodeSelectionStoryMode::updateSafeContentMenu() {
		safeContentInformation->update(delta_t_ms);
		if (safeContentInformation->isAnimating()) {
			return;
		}
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0));
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus());
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1));
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus());
		}
		u32 inputButtonsForMenu = player1input->getPressedButtons();
		inputButtonsForMenu |= player2input->getPressedButtons();
		if ((inputButtonsForMenu & FK_Input_Buttons::Cancel_Button) != 0) {
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			deactivateSafeContentMenu();
			return;
		}
		else {
			safeContentMenu->update(inputButtonsForMenu, false);
			if (safeContentMenu->itemIsSelected()) {
				s32 safeContentSelectionIndex = safeContentMenu->getIndex();
				safeContentMenu->resetSelection();
				std::string censorFileName = 
					(episodes[episodeIndex].getEpisodeFullPath() + "\\" +
				FK_SceneEpisodeSelectionStoryMode::EpisodeEditedMarkdownFileName);
				std::ofstream censorFile(censorFileName.c_str());
				censorFile << "";
				censorFile.close();
				std::string episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeFlowFileName;
				std::string episodeCensorConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName;
				switch (safeContentSelectionIndex) {
				case 0:
					// remove censor marker
					remove(censorFileName.c_str());
					episodes[episodeIndex].setup(
						episodes[episodeIndex].getEpisodeFullPath() + "\\..\\", episodes[episodeIndex].getEpisodeRelativePath(), episodeConfigFilename);
					// check if there exist a censored version of the episode (if so, a certain file will be present)
					soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
					episodes[episodeIndex].resetProgress();
					episodeSelected = true;
					break;
				case 1:
					episodes[episodeIndex].setup(
						episodes[episodeIndex].getEpisodeFullPath() + "\\..\\", episodes[episodeIndex].getEpisodeRelativePath(), episodeCensorConfigFilename);
					soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
					episodes[episodeIndex].resetProgress();
					episodeSelected = true;
					break;
				case 2:
					// remove censor marker
					remove(censorFileName.c_str());
					episodeSelected = false;
					soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					break;
				default:
					break;
				}
				deactivateSafeContentMenu();
			}
		}
	}

	// back to main menu
	void FK_SceneEpisodeSelectionStoryMode::goBackToMainMenu(){
		setNextScene(FK_Scene::FK_SceneType::SceneMainMenu);
		backToMenu = true;
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		Sleep(300);
		return;
	}

	// menu index operations
	void FK_SceneEpisodeSelectionStoryMode::increaseMenuIndex(s32 increase){
		lastInputTime = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		episodeIndex += increase;
		if (increase > 1 && episodeIndex >= episodes.size()){
			episodeIndex = episodes.size() - 1;
		}
		else{
			episodeIndex %= episodes.size();
		}
		inputReceiver->reset();
	}
	void FK_SceneEpisodeSelectionStoryMode::decreaseMenuIndex(s32 decrease){
		lastInputTime = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		episodeIndex -= decrease;
		if (decrease > 1 && episodeIndex < 0){
			episodeIndex = 0;
		}
		else{
			episodeIndex += (s32)episodes.size();
			episodeIndex %= episodes.size();
		}
		inputReceiver->reset();
	}
	void FK_SceneEpisodeSelectionStoryMode::cancelSelection(){
		lastInputTime = now;
		inputReceiver->reset();
		goBackToMainMenu();
	}
	void FK_SceneEpisodeSelectionStoryMode::selectItem(){
		if (episodeSelected){
			return;
		}
		if (isEpisodeAvailable(episodeIndex)){
			lastInputTime = now;
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			//episodeSelected = true;
			inputReceiver->reset();
			setNextScene(FK_Scene::FK_SceneType::SceneStoryModeDialogue);
			activateChapterMenu();
			//Sleep(300);
			return;
		}
		else{
			lastInputTime = now;
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			inputReceiver->reset();
			return;
		}
	}

	void FK_SceneEpisodeSelectionStoryMode::updateInput(){
		u32 lastInputTimePlayer1 = lastInputTime;
		u32 lastInputTimePlayer2 = lastInputTime;
		// update input
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0), false);
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus(), false);
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1), false);
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus(), false);
		}

		u32 inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		updateMenuInputTimeThresholdMS(inputButtons, delta_t_ms);
		u32 inputThreshold = getMenuInputTimeThresholdMS();

		// check input for player 1
		if (now - lastInputTimePlayer1 > inputThreshold){
			u32 directionPlayer1 = player1input->getLastDirection();
			u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button){
				cancelSelection();
				return;
			}
			else{
				if (directionPlayer1 == FK_Input_Buttons::Down_Direction){
					increaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer1 == FK_Input_Buttons::ScrollRight_Button){
					increaseMenuIndex(5);
					return;
				}
				else if (directionPlayer1 == FK_Input_Buttons::Up_Direction){
					decreaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer1 == FK_Input_Buttons::ScrollLeft_Button){
					decreaseMenuIndex(5);
					return;
				}
				else{
					// select item
					if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button){
						selectItem();
						return;
					}
				}
			}
		}
		// check input for player 2
		if (now - lastInputTimePlayer2 > inputThreshold){
			u32 directionPlayer2 = player2input->getLastDirection();
			u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button){
				cancelSelection();
				return;
			}
			else{
				if (directionPlayer2 == FK_Input_Buttons::Down_Direction){
					increaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer2 == FK_Input_Buttons::ScrollRight_Button){
					increaseMenuIndex(5);
					return;
				}
				else if (directionPlayer2 == FK_Input_Buttons::Up_Direction){
					decreaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer2 == FK_Input_Buttons::ScrollLeft_Button){
					decreaseMenuIndex(5);
					return;
				}
				else{
					// select item
					if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button){
						selectItem();
						return;
					}
				}
			}
		}
	}

	// load all available episodes
	void FK_SceneEpisodeSelectionStoryMode::loadEpisodeList(){
		std::ifstream ifile((episodeDirectory + FK_SceneEpisodeSelectionStoryMode::EpisodeListFileName).c_str());
		u32 index = 0;
		while (ifile){
			std::string episodeName;
			s32 availability;
			ifile >> episodeName >> availability;
			if (episodeName.empty()){
				break;
			}
			if (availability == 0){
				availableEpisodes.push_back(index);
			}
			else{
				if (
					std::find(unlockedStoryEpisodes.begin(), unlockedStoryEpisodes.end(), episodeName) !=
					unlockedStoryEpisodes.end()
					){
					availableEpisodes.push_back(index);
				}
			}
			episodeName += "\\";
			if (std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), episodeName) != 
				storyEpisodesCleared.end()){
				completedEpisodes.push_back(true);
			}
			else{
				completedEpisodes.push_back(false);
			}
			FK_StoryFlowCluster newEpisode;
			episodes.push_back(newEpisode);
			std::string episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeFlowFileName;
			if (fk_addons::fileExists(
				(episodeDirectory + episodeName + "\\" + 
					FK_SceneEpisodeSelectionStoryMode::EpisodeEditedMarkdownFileName).c_str())){
				episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName;
				editedEpisodes.push_back(true);
			}
			else {
				editedEpisodes.push_back(false);
			}			
			// check if there exist a censored version of the episode (if so, a certain file will be present)
			episodes[episodes.size() - 1].setup(
				episodeDirectory, episodeName, episodeConfigFilename);
			episodes[episodes.size() - 1].loadProgress();
			episodeNames.push_back(episodes[episodes.size() - 1].getEpisodeName());
			index += 1;
		}
		ifile.close();

		//add workshop and dlc stages
		for (auto item : enabledWorkshopItems) {
			if (item.type == FK_WorkshopContentManager::WorkshopItemType::StoryEpisode &&
				item.enabled) {
				std::string episodeName;
				std::string episodePath;
				episodePath = episodeDirectory;
				episodeName = "..\\..\\..\\" + fk_constants::FK_WorkshopFolder + fk_constants::FK_WorkshopStoryEpisodesFolder + item.path;
				availableEpisodes.push_back(index);
				if (std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), episodeName) !=
					storyEpisodesCleared.end()) {
					completedEpisodes.push_back(true);
				}
				else {
					completedEpisodes.push_back(false);
				}
				FK_StoryFlowCluster newEpisode;
				episodes.push_back(newEpisode);
				std::string episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeFlowFileName;
				if (fk_addons::fileExists(
					(episodePath + episodeName + "\\" +
						FK_SceneEpisodeSelectionStoryMode::EpisodeEditedMarkdownFileName).c_str())) {
					episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName;
					editedEpisodes.push_back(true);
				}
				else {
					editedEpisodes.push_back(false);
				}
				// check if there exist a censored version of the episode (if so, a certain file will be present)
				episodes[episodes.size() - 1].setup(
					episodePath, episodeName, episodeConfigFilename);
				episodes[episodes.size() - 1].loadProgress();
				episodeNames.push_back(episodes[episodes.size() - 1].getEpisodeName());
				index += 1;
			}
		};
		//add workshop and dlc stages
		for (auto item : enabledDLCItems) {
			if (item.type == FK_WorkshopContentManager::WorkshopItemType::StoryEpisode &&
				item.enabled) {
				std::string episodeName;
				episodeName = "..\\..\\..\\" + fk_constants::FK_DLCFolder + fk_constants::FK_WorkshopStoryEpisodesFolder + item.path;
				availableEpisodes.push_back(index);
				if (std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), episodeName) !=
					storyEpisodesCleared.end()) {
					completedEpisodes.push_back(true);
				}
				else {
					completedEpisodes.push_back(false);
				}
				FK_StoryFlowCluster newEpisode;
				episodes.push_back(newEpisode);
				std::string episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeFlowFileName;
				if (fk_addons::fileExists(
					(episodeDirectory + episodeName + "\\" +
						FK_SceneEpisodeSelectionStoryMode::EpisodeEditedMarkdownFileName).c_str())) {
					episodeConfigFilename = FK_SceneEpisodeSelectionStoryMode::EpisodeEditedFlowFileName;
					editedEpisodes.push_back(true);
				}
				else {
					editedEpisodes.push_back(false);
				}
				// check if there exist a censored version of the episode (if so, a certain file will be present)
				episodes[episodes.size() - 1].setup(
					episodeDirectory, episodeName, episodeConfigFilename);
				episodes[episodes.size() - 1].loadProgress();
				episodeNames.push_back(episodes[episodes.size() - 1].getEpisodeName());
				index += 1;
			}
		};
	}
	// draw background
	void FK_SceneEpisodeSelectionStoryMode::drawBackground(){
		core::dimension2d<u32> backgroundSize = background->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, backgroundSize.Width, backgroundSize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(background, destinationRect, sourceRect, 0, vertexColors_background);
	}

	void FK_SceneEpisodeSelectionStoryMode::drawTitle(){
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Episodes";
		s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
		x -= sentenceWidth / 2;
		y -= sentenceHeight / 2;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + scale_factorY);
		fk_addons::drawBorderedText(font, itemString, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	//draw episode icon and completion percentage
	void FK_SceneEpisodeSelectionStoryMode::drawEpisode(u32 index){
		// draw episode icon
		s32 offsetY = 66;
		s32 offsetX = 4;
		s32 additionalY = 5;
		
		video::ITexture* tex = isEpisodeAvailable(index) ? episodeIcons[index] : lockedEpisodeIcon;
		core::dimension2d<u32> texSize = tex->getSize();
		s32 bannerWidth = 128;
		f32 bannerScaleFactor = (f32)((f32)bannerWidth / (f32)texSize.Width);
		core::vector2d<f32> scale(scale_factorX * bannerScaleFactor,
			scale_factorY * bannerScaleFactor);

		s32 fixedOffsetY = (s32)std::ceil((f32)offsetY * scale_factorY);
		s32 fixedSpacingY = (s32)std::ceil((f32)additionalY * scale_factorY + (f32)texSize.Height *
			scale_factorY * bannerScaleFactor);
		// the plus one serves to avoid a bug which caused the last episode not to be displayed correctly with certain screen resolutions
		f32 additionaSpacing = std::max(1.1f, scale_factorY);
		s32 thresholdValue = (s32)std::ceil(((f32)episodes.size()) * fixedSpacingY + additionaSpacing);
		// add offset
		if (episodeIndex > 3 && episodes.size() > 6){
			s32 bufferY = (s32)std::ceil((f32)additionalY * scale_factorY);
			thresholdValue = screenSize.Height - (fixedOffsetY + bufferY) - thresholdValue;
			fixedOffsetY -= (fixedSpacingY * (episodeIndex - 3));
			while (fixedOffsetY < thresholdValue){
				fixedOffsetY += fixedSpacingY;
			}
		}

		s32 x = (s32)std::ceil((f32)offsetX * scale_factorX);
		s32 y = fixedOffsetY + index * fixedSpacingY;

		if (y < offsetY * scale_factorY || (y + fixedSpacingY) > screenSize.Height - offsetY * scale_factorY){
			return;
		}

		video::SColor color(255, 128, 128, 128);
		video::SColor color2(128, 32, 32, 32);
		if (index == episodeIndex){
			color = video::SColor(255, 255, 255, 255);
			f32 magnificationPeriodMs = 1000.0;
			f32 phase = 2 * core::PI * ((f32)now / magnificationPeriodMs);
			phase = fmod(phase, 2 * core::PI);
			f32 magnificationAmplitude = 48;
			//f32 triangleWave = 2 * abs(2 * (phase / 2 - floor(phase / 2 + core::PI))) - 1;
			s32 addon = magnificationAmplitude / 2 + (s32)std::floor(magnificationAmplitude * sin(phase));
			color = video::SColor(255, 160 + addon, 160 + addon, 160 + addon);
		}
		core::vector2d<s32> drawingPosition(x, y);
		fk_addons::draw2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		// draw event number in a corner
		std::wstring eventId = std::to_wstring(index + 1);
		core::dimension2d<u32> textSize = captionFont->getDimension(eventId.c_str());
		s32 textX = x + 4;
		fk_addons::drawBorderedText(captionFont, eventId, core::rect<s32>(textX, y, textX + textSize.Width, y + textSize.Height),
			color);
		// if the episode is EDITED, write a small E in the bottom left corner
		if (editedEpisodes[index]) {
			std::wstring editedFlag = L"E";
			textSize = captionFont->getDimension(editedFlag.c_str());
			s32 editedMarkY = (s32)std::floor((f32)y + scale.Y * (f32)texSize.Height - (f32)textSize.Height);
			fk_addons::drawBorderedText(captionFont, editedFlag, core::rect<s32>(textX, editedMarkY, textX + textSize.Width, y + textSize.Height),
				color);
		}
		// draw completion box
		core::dimension2d<u32> boxSize = completionBoxTex->getSize();
		s32 boxX = x + (s32)std::ceil((f32)bannerWidth * scale_factorX);
		drawingPosition = core::vector2d<s32>(boxX, y);
		fk_addons::draw2DImage(driver, completionBoxTex,
			core::rect<s32>(0, 0, boxSize.Width, boxSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		// draw completion percentage
		f32 percent = episodes[index].getCompletionPercentage();
		std::wstring percentage = std::to_wstring((s32)std::round(episodes[index].getCompletionPercentage()));
		percentage += L"%";
		textSize = captionFont->getDimension(percentage.c_str());
		fk_addons::drawBorderedText(captionFont, percentage, core::rect<s32>(
			drawingPosition.X, 
			drawingPosition.Y, 
			drawingPosition.X + (s32)(boxSize.Width * scale.X),
			drawingPosition.Y + (s32)(boxSize.Height * scale.Y)),
			color, color2, true, true);
		// draw tick if episode was completed
		if (index == episodeIndex){
			color = video::SColor(255, 255, 255, 255);
		}
		if (completedEpisodes[index] && isEpisodeAvailable(index)){
			core::dimension2d<u32> tickSize = episodeCompleteTick->getSize();
			s32 tickX = boxX + (s32)(boxSize.Width * scale.X);
			drawingPosition = core::vector2d<s32>(tickX, y);
			fk_addons::draw2DImage(driver, episodeCompleteTick,
				core::rect<s32>(0, 0, tickSize.Width, tickSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}
	// draw episode banners
	void FK_SceneEpisodeSelectionStoryMode::drawEpisodeList(){
		for (u32 i = 0; i < episodes.size(); ++i){
			drawEpisode(i);
		}
	}

	// draw episode preview
	void FK_SceneEpisodeSelectionStoryMode::drawCurrentEpisodePreview(){
		// draw selected episode preview
		s32 offsetY = 66;
		s32 offsetX = 260;
		s32 frameSizeX = 320;
		s32 frameSizeY = 180;
		video::ITexture* tex;
		if (isEpisodeAvailable(episodeIndex)){
			//std::string path = episodeDirectory + episodes[episodeIndex].getEpisodeRelativePath() + "\\preview.jpg";
			std::string path = episodes[episodeIndex].getEpisodeFullPath() + "\\preview.jpg";
			tex = driver->getTexture(path.c_str());
		}
		else{
			tex = lockedEpisodePreview;
		}
		core::dimension2d<u32> texSize = tex->getSize();
		core::vector2d<f32> scale(scale_factorX * (f32)frameSizeX / (f32)texSize.Width,
			scale_factorY * (f32)frameSizeY / (f32)texSize.Height);
		core::vector2d<s32> drawingPosition(offsetX * scale_factorX, offsetY * scale_factorY);
		fk_addons::draw2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		// draw frame
		tex = previewFrame;
		texSize = tex->getSize();
		scale = core::vector2d<f32>(scale_factorX * (f32)frameSizeX / (f32)texSize.Width,
			scale_factorY * (f32)frameSizeY / (f32)texSize.Height);
		fk_addons::draw2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		// draw episode name and description
		video::SColor color(255, 255, 255, 255);
		video::SColor color2(128, 32, 32, 32);
		s32 textX = drawingPosition.X;
		s32 textY = drawingPosition.Y + frameSizeY * scale_factorY;
		std::string temp = "[TOP SECRET]";
		if (isEpisodeAvailable(episodeIndex)){
			temp = episodes[episodeIndex].getEpisodeName();
		}
		std::wstring episodeName = std::wstring(temp.begin(), temp.end());
		core::dimension2d<u32> nameSize = mediumFont->getDimension(episodeName.c_str());
		fk_addons::drawBorderedText(mediumFont, episodeName, core::rect<s32>(
			textX,
			textY,
			textX + nameSize.Width,
			textY + nameSize.Height),
			color, color2);
		// draw description
		textY = textY + nameSize.Height;
		if (isEpisodeAvailable(episodeIndex)){
			for (int i = 0; i < episodes[episodeIndex].getEpisodeDescription().size(); ++i){
				temp = episodes[episodeIndex].getEpisodeDescription().at(i);
				std::wstring line = std::wstring(temp.begin(), temp.end());
				nameSize = captionFont->getDimension(line.c_str());
				fk_addons::drawBorderedText(captionFont, line, core::rect<s32>(
					textX,
					textY,
					textX + nameSize.Width,
					textY + nameSize.Height),
					color, color2);
				textY = textY + nameSize.Height;
			}
		}
		else{
			temp = "???";
			std::wstring line = std::wstring(temp.begin(), temp.end());
			nameSize = captionFont->getDimension(line.c_str());
			fk_addons::drawBorderedText(captionFont, line, core::rect<s32>(
				textX,
				textY,
				textX + nameSize.Width,
				textY + nameSize.Height),
				color, color2);
		}
	}

	// draw arrows
	void FK_SceneEpisodeSelectionStoryMode::drawArrows(){
		f32 phase = 2 * core::PI * (f32)(now % arrowFrequency) / arrowFrequency;
		s32 addonY = floor(10 * asin(cos(phase)) / core::PI);
		u32 arrowSize = 16;
		s32 baseArrowY = 42;
		s32 baseArrowX = 68;
		if (episodeIndex > 3 && episodes.size() > 6){
			s32 x1 = baseArrowX * scale_factorX - arrowSize * scale_factorX / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y1 = (baseArrowY + addonY) * scale_factorY;
			s32 y2 = y1 + arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowUp_tex->getSize().Width, arrowUp_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowUp_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
		if (episodeIndex < episodes.size() - 3 && episodes.size() > 6){
			s32 x1 = baseArrowX * scale_factorX - arrowSize * scale_factorX / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y2 = screenSize.Height - (baseArrowY + addonY) * scale_factorY;
			s32 y1 = y2 - arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowDown_tex->getSize().Width, arrowDown_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowDown_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
	}

	// draw everything
	void FK_SceneEpisodeSelectionStoryMode::drawAll(){
		drawBackground();
		drawEpisodeList();
		drawCurrentEpisodePreview();
		drawArrows();
		drawTitle();
		if (chapterSelectMenu->isVisible() || 
			(safeContentMenu->isVisible() || safeContentInformation->isAnimating()) ||episodeSelected) {
			driver->draw2DRectangle(SColor(128, 0, 0, 0),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			drawEpisode(episodeIndex);
			if (!episodeSelected && chapterSelectMenu->isVisible()) {
				chapterSelectMenu->draw();
			}
			else if (safeContentInformation->isAnimating()) {
				safeContentInformation->draw(screenSize, 1.0f, 1.0f, false);
			}
			else if (!episodeSelected && 
				(safeContentMenu->isVisible() || safeContentInformation->isAnimating())) {
				if (safeContentInformation->isVisible() || safeContentInformation->isAnimating()) {
					safeContentInformation->draw(screenSize, 1.0f, 1.0f, false);
				}
				if (safeContentMenu->isVisible()) {
					safeContentMenu->draw();
				}
			}
		}
	}
	// check if episode is available
	bool FK_SceneEpisodeSelectionStoryMode::isEpisodeAvailable(u32 index){
		if (index >= episodes.size() || index < 0){
			return false;
		}
		return std::find(availableEpisodes.begin(), availableEpisodes.end(), index) != availableEpisodes.end();
	}
	// check selected episode
	FK_StoryFlowCluster FK_SceneEpisodeSelectionStoryMode::getSelectedEpisode(){
		return episodes[episodeIndex];
	}
	// get current episode index
	s32 FK_SceneEpisodeSelectionStoryMode::getCurrentIndex(){
		return episodeIndex;
	}
}