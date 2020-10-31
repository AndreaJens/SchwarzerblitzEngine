#include "FK_SceneChallengeSelectionTutorial.h"
#include "ExternalAddons.h"
#include "FK_AchievementManager.h"

using namespace irr;

namespace fk_engine{

	void FK_SceneChallengeSelectionTutorial::FK_TutorialCluster::setup(std::string challengeDirectory,
		std::string challengeRelativePath, std::string configurationFilename){
		parentDirectory = challengeDirectory;
		configFileName = configurationFilename;
		// open flow file
		std::string flowFileName = parentDirectory + challengeRelativePath + configurationFilename;
		relativePath = challengeRelativePath;
		std::ifstream inputFile(flowFileName.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		std::string ChallengeNameTag = "#NAME";
		std::string ChallengeIDTag = "#ID";
		std::string ChallengeDescriptionTag = "#DESCRIPTION";
		std::string ChallengeDescriptionEndTag = "#DESCRIPTION_END";
		while (inputFile >> temp){
			if (temp == ChallengeNameTag){
				std::string name = std::string();
				while (name.empty()){
					std::getline(inputFile, name);
				}
				challengeStoreName = name;
				challengeName = fk_addons::convertNameToNonASCIIWstring(name);
			}
			if (temp == ChallengeIDTag){
				std::string name = std::string();
				while (name.empty()){
					std::getline(inputFile, name);
				}
				challengeId = fk_addons::convertNameToNonASCIIWstring(name);
			}
			if (temp == ChallengeDescriptionTag){
				std::string line = std::string();
				while (line.empty()){
					std::getline(inputFile, line);
					if (!line.empty()){
						if (line == ChallengeDescriptionEndTag){
							break;
						}
						else{
							description.push_back(line);
							line = std::string();
						}
					}
				}
			}
		}
	}

	FK_SceneChallengeSelectionTutorial::FK_SceneChallengeSelectionTutorial(){
		initialize();
	}

	void FK_SceneChallengeSelectionTutorial::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions, FK_Scene::FK_SceneTrainingType newChallengeSelectionType, s32 currentChallengeIndex){
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		challengeDirectory = applicationPath + FK_SceneChallengeSelectionTutorial::ChallengeMainDirectory;
		challengeCommonMediaPath = mediaPath + "scenes\\tutorial\\common\\system\\";
		challengeSelectionType = newChallengeSelectionType;
		initialize();
		challengeIndex = currentChallengeIndex;
		setupIrrlichtDevice();
		FK_SceneWithInput::readSaveFile();
		setupJoypad();
		setupInputMapper();
		setupSoundManager();
		setupBGM();
		loadChallengeList();
		setupGraphics();
	}
	void FK_SceneChallengeSelectionTutorial::initialize(){
		now = 0;
		then = 0;
		delta_t_ms = 0;
		cycleId = 0;
		backToMenu = false;
		challengeSelected = false;
		lastInputTime = 0;
	}
	void FK_SceneChallengeSelectionTutorial::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		if (cycleId == 0){
			cycleId = 1;
			delta_t_ms = 0;
			bgm.play();
		}
		else{
			updateInput();
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
	void FK_SceneChallengeSelectionTutorial::dispose(){
		// stop bgm
		bgm.stop();
		// clear challenges
		challenges.clear();
		// delete sound manager
		delete soundManager;
		// clear the scene of all scene nodes
		FK_SceneWithInput::dispose();
	}
	FK_Scene::FK_SceneType FK_SceneChallengeSelectionTutorial::nameId(){
		return FK_Scene::FK_SceneType::SceneGameTutorial;
	}
	bool FK_SceneChallengeSelectionTutorial::isRunning(){
		return (!backToMenu && !challengeSelected);
	}
	void FK_SceneChallengeSelectionTutorial::setupGraphics(){
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
		lockedChallengeIcon = driver->getTexture((challengeCommonMediaPath + "lockedChallenge.png").c_str());
		defaultChallengeIcon = driver->getTexture((challengeCommonMediaPath + "default.png").c_str());
		for each(auto challenge in challenges){
			std::string path = challengeDirectory + challenge.relativePath + "\\banner.png";
			video::ITexture* tex = driver->getTexture(path.c_str());
			challengeIcons.push_back(
				tex == NULL ? defaultChallengeIcon : tex
				);
		}
		completionBoxTex = driver->getTexture((challengeCommonMediaPath + "completionBox.png").c_str());
		// preview
		previewFrame = driver->getTexture((challengeCommonMediaPath + "previewFrame.png").c_str());
		lockedChallengePreview = driver->getTexture((challengeCommonMediaPath + "lockedPreview.jpg").c_str());
		// arrows
		arrowUp_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowU.png").c_str());
		arrowDown_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowD.png").c_str());
		arrowFrequency = 500;
		// challenge complete tick
		challengeCompleteTick = driver->getTexture((challengeCommonMediaPath + "challengeCompleteTick.png").c_str());
	}
	void FK_SceneChallengeSelectionTutorial::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}
	void FK_SceneChallengeSelectionTutorial::setupBGM(){
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

	// back to main menu
	void FK_SceneChallengeSelectionTutorial::goBackToMainMenu(){
		setNextScene(FK_Scene::FK_SceneType::SceneMainMenu);
		backToMenu = true;
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		Sleep(300);
		return;
	}

	// menu index operations
	void FK_SceneChallengeSelectionTutorial::increaseMenuIndex(s32 increase){
		lastInputTime = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		challengeIndex += increase;
		if (increase > 1 && challengeIndex >= challenges.size()){
			challengeIndex = challenges.size() - 1;
		}
		else{
			challengeIndex %= challenges.size();
		}
		inputReceiver->reset();
	}
	void FK_SceneChallengeSelectionTutorial::decreaseMenuIndex(s32 decrease){
		lastInputTime = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		challengeIndex -= decrease;
		if (decrease > 1 && challengeIndex < 0){
			challengeIndex = 0;
		}
		else{
			challengeIndex += (s32)challenges.size();
			challengeIndex %= challenges.size();
		}
		inputReceiver->reset();
	}
	void FK_SceneChallengeSelectionTutorial::cancelSelection(){
		lastInputTime = now;
		inputReceiver->reset();
		goBackToMainMenu();
	}
	void FK_SceneChallengeSelectionTutorial::selectItem(){
		lastInputTime = now;
		if (challengeSelected){
			return;
		}
		if (isChallengeAvailable(challengeIndex)){
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			challengeSelected = true;
			inputReceiver->reset();
			setNextScene(FK_Scene::FK_SceneType::SceneGameFreeMatch);
			Sleep(300);
			return;
		}
		else{
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			inputReceiver->reset();
			return;
		}
	}

	void FK_SceneChallengeSelectionTutorial::updateInput(){
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

	// load all available challenges
	void FK_SceneChallengeSelectionTutorial::loadChallengeList(){
		std::string filePath = challengeDirectory + FK_SceneChallengeSelectionTutorial::ChallengeListFileName;
		if (challengeSelectionType == FK_Scene::FK_SceneTrainingType::TrainingTutorial) {
			filePath = challengeDirectory + FK_SceneChallengeSelectionTutorial::TutorialListFileName;
		}
		std::ifstream ifile(filePath.c_str());
		u32 index = 0;
		while (ifile){
			std::string challengeName;
			s32 availability;
			ifile >> challengeName >> availability;
			if (challengeName.empty()){
				break;
			}
			if (availability == 0){
				availableChallenges.push_back(index);
			}
			else{
				if (
					std::find(unlockedTutorialChallenges.begin(), unlockedTutorialChallenges.end(), challengeName) !=
					unlockedTutorialChallenges.end()
					){
					availableChallenges.push_back(index);
				}
			}
			challengeName += "\\";
			if (std::find(tutorialChallengesCleared.begin(), tutorialChallengesCleared.end(), challengeName) !=
				tutorialChallengesCleared.end()){
				completedChallenges.push_back(true);
			}
			else{
				completedChallenges.push_back(false);
			}
			FK_TutorialCluster newChallenge;
			challenges.push_back(newChallenge);
			challenges[challenges.size() - 1].setup(
				challengeDirectory, challengeName, 
				FK_SceneChallengeSelectionTutorial::ChallengeConfigurationFileName);
			challengeNames.push_back(challenges[challenges.size() - 1].challengeStoreName);
			index += 1;
		}
		ifile.close();
	}
	// draw background
	void FK_SceneChallengeSelectionTutorial::drawBackground(){
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

	void FK_SceneChallengeSelectionTutorial::drawTitle(){
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Trials";
		if (challengeSelectionType == FK_Scene::FK_SceneTrainingType::TrainingTutorial) {
			itemString = L"Tutorial";
		}
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

	//draw challenge icon and completion percentage
	void FK_SceneChallengeSelectionTutorial::drawChallenge(u32 index){
		// draw challenge icon
		s32 offsetY = 66;
		s32 offsetX = 4;
		s32 additionalY = 5;

		video::ITexture* tex = isChallengeAvailable(index) ? challengeIcons[index] : lockedChallengeIcon;
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
		s32 thresholdValue = (s32)std::ceil(((f32)challenges.size()) * fixedSpacingY + additionaSpacing);
		//s32 thresholdValue = (s32)std::ceil(((f32)challenges.size()) * (fixedSpacingY + 1));
		// add offset
		if (challengeIndex > 3 && challenges.size() > 6){
			s32 bufferY = (s32)std::ceil((f32)additionalY * scale_factorY);
			thresholdValue = screenSize.Height - (fixedOffsetY + bufferY) - thresholdValue;
			fixedOffsetY -= (fixedSpacingY * (challengeIndex - 3));
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
		if (index == challengeIndex){
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
		if (isChallengeAvailable(index)){
			// draw event name
			textX = x;
			textSize = core::dimension2d<u32>((u32)std::floor(bannerWidth * scale_factorX), fixedSpacingY);
			std::wstring currentChallengeId = challenges[index].challengeId;
			std::wstring challengeTag = std::wstring(currentChallengeId.begin(), currentChallengeId.end());
			fk_addons::drawBorderedText(captionFont, challengeTag, core::rect<s32>(textX, y, textX + textSize.Width, y + textSize.Height),
				color, video::SColor(255, 0, 0, 0), true, true);
		}
		// draw completion box
		s32 boxX = x + (s32)std::ceil((f32)bannerWidth * scale_factorX);
		// draw tick if challenge was completed
		if (index == challengeIndex){
			color = video::SColor(255, 255, 255, 255);
		}
		if (completedChallenges[index] && isChallengeAvailable(index)){
			core::dimension2d<u32> tickSize = challengeCompleteTick->getSize();
			drawingPosition = core::vector2d<s32>(boxX, y);
			fk_addons::draw2DImage(driver, challengeCompleteTick,
				core::rect<s32>(0, 0, tickSize.Width, tickSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}
	// draw challenge banners
	void FK_SceneChallengeSelectionTutorial::drawChallengeList(){
		for (u32 i = 0; i < challenges.size(); ++i){
			drawChallenge(i);
		}
	}

	// draw challenge preview
	void FK_SceneChallengeSelectionTutorial::drawCurrentChallengePreview(){
		// draw selected challenge preview
		s32 offsetY = 66;
		s32 offsetX = 260;
		s32 frameSizeX = 320;
		s32 frameSizeY = 180;
		video::ITexture* tex;
		if (isChallengeAvailable(challengeIndex)){
			std::string path = challengeDirectory + challenges[challengeIndex].relativePath + "\\preview.jpg";
			tex = driver->getTexture(path.c_str());
		}
		else{
			tex = lockedChallengePreview;
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
		// draw challenge name and description
		video::SColor color(255, 255, 255, 255);
		video::SColor color2(128, 32, 32, 32);
		s32 textX = drawingPosition.X;
		s32 textY = drawingPosition.Y + frameSizeY * scale_factorY;
		std::wstring temp = L"[TOP SECRET]";
		if (isChallengeAvailable(challengeIndex)){
			temp = challenges[challengeIndex].challengeName;
		}
		std::wstring challengeName = std::wstring(temp.begin(), temp.end());
		core::dimension2d<u32> nameSize = mediumFont->getDimension(challengeName.c_str());
		fk_addons::drawBorderedText(mediumFont, challengeName, core::rect<s32>(
			textX,
			textY,
			textX + nameSize.Width,
			textY + nameSize.Height),
			color, color2);
		// draw description
		textY = textY + nameSize.Height;
		if (isChallengeAvailable(challengeIndex)){
			for (int i = 0; i < challenges[challengeIndex].description.size(); ++i){
				std::string temp1 = challenges[challengeIndex].description.at(i);
				std::wstring line = std::wstring(temp1.begin(), temp1.end());
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
			temp = L"???";
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
	void FK_SceneChallengeSelectionTutorial::drawArrows(){
		f32 phase = 2 * core::PI * (f32)(now % arrowFrequency) / arrowFrequency;
		s32 addonY = floor(10 * asin(cos(phase)) / core::PI);
		u32 arrowSize = 16;
		s32 baseArrowY = 42;
		s32 baseArrowX = 68;
		if (challengeIndex > 3 && challenges.size() > 6){
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
		if (challengeIndex < challenges.size() - 3 && challenges.size() > 6){
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
	void FK_SceneChallengeSelectionTutorial::drawAll(){
		drawBackground();
		drawChallengeList();
		drawCurrentChallengePreview();
		drawArrows();
		drawTitle();
	}
	// check if challenge is available
	bool FK_SceneChallengeSelectionTutorial::isChallengeAvailable(u32 index){
		if (index >= challenges.size() || index < 0){
			return false;
		}
		return std::find(availableChallenges.begin(), availableChallenges.end(), index) != availableChallenges.end();
	}
	// check selected challenge
	FK_SceneChallengeSelectionTutorial::FK_TutorialCluster FK_SceneChallengeSelectionTutorial::getSelectedChallenge(){
		return challenges[challengeIndex];
	}
	// get current challenge index
	s32 FK_SceneChallengeSelectionTutorial::getCurrentIndex(){
		return challengeIndex;
	}
	FK_Scene::FK_SceneTrainingType FK_SceneChallengeSelectionTutorial::getChallengeType()
	{
		return challengeSelectionType;
	}
}