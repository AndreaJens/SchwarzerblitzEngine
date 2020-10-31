#include "FK_SceneCharacterSelectArcade.h"
#include "FK_Database.h"
#include "ExternalAddons.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <filesystem>

namespace fk_engine{
	FK_SceneCharacterSelectArcade::FK_SceneCharacterSelectArcade() : FK_SceneCharacterSelect_Base(){};
	/* setup scene*/
	void FK_SceneCharacterSelectArcade::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, 
		FK_Options* newOptions,
		FK_Scene::FK_SceneArcadeType newArcadeType,
		bool player1IsActiveFlag,
		int inputPlayerIndex,
		u32 inputPlayerOutfitId){
		arcadeType = newArcadeType;
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		charactersDirectory = mediaPath + fk_constants::FK_CharactersFileFolder;
		stagesResourcePath = mediaPath + fk_constants::FK_StagesFileFolder;
		voiceClipsPath = mediaPath + fk_constants::FK_VoiceoverEffectsFileFolder;
		soundPath = commonResourcesPath + fk_constants::FK_SoundEffectsFileFolder;
		systemBGMPath = commonResourcesPath + fk_constants::FK_SystemBGMFileFolder;
		offset = core::vector2d<f32>(0.f, 0.f);
		firstSceneRender = false;
		//if (inputPlayerIndex > 0){
		//	playerIndex = inputPlayerIndex;
		//	oldPlayerIndex = inputPlayerIndex;
		//}
		//else{
		//	playerIndex = 0;
		//	oldPlayerIndex = 0;
		//}
		playerIndex = inputPlayerIndex;
		oldPlayerIndex = inputPlayerIndex;
		lastInputTime = 0;
		playerOutfitId = inputPlayerOutfitId;
		playerOldOutfitId = inputPlayerOutfitId;
		//picturesPerRow = 3;
		//numberOfRows = 4;
		then = 0;
		now = 0;
		cycleId = 0;
		transitionCounter = 0;
		player1IsActive = player1IsActiveFlag;
		backToMenu = false;
		forcePlayerSelection = false;
		availableCharacters = std::vector<u32>();
		playerUsesJoypad = false;
		characterAvailableCostumes.clear();
		selectionMap = std::map<FK_Input_Buttons, SelectionDirection>{
			{ FK_Input_Buttons::Down_Direction, SelectionDirection::Down },
			{ FK_Input_Buttons::Up_Direction, SelectionDirection::Up },
			{ FK_Input_Buttons::Left_Direction, SelectionDirection::Left },
			{ FK_Input_Buttons::Right_Direction, SelectionDirection::Right },
		};
		// read save file
		FK_SceneWithInput::readSaveFile();
		initialize();
	}
	void FK_SceneCharacterSelectArcade::initialize(){
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper();
		setupBasicResources();
		loadConfigurationFile();
		setupSystemButtonIcons();
		setupBGM();
		setupSoundManager();
		setupAnnouncer();
		loadCharacterList();
		setupTextures();
		setupCamera();
		setIconSizeAndOffset();
		if (playerIndex < 0) {
			if (sceneResources.startingPlayerIndexes.first >= 0) {
				playerIndex = sceneResources.startingPlayerIndexes.first;
				oldPlayerIndex = playerIndex;
			}
			else {
				playerIndex = 0;
				oldPlayerIndex = 0;
			}
		}
		setupCharacters();
	};
	void FK_SceneCharacterSelectArcade::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		updateInput();
		updateCharacter();
		updateCharacterSelection();
		// if this is the first update cycle, play "select character" voice and the BGM
		if (cycleId == 0){
			cycleId = 1;
			announcer->playSound("select_character", 100.0 * gameOptions->getSFXVolume());
			character_selection_bgm.play();
		}
		// update transition counter, if everything is ready to go
		/*
		if (player1Ready && player2Ready){
		stageReady = true;
		}
		*/
		if (playerReady){
			transitionCounter += delta_t_ms;
		}
		else{
			transitionCounter = 0;
		}
		// draw scene
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, sceneResources.backgroundColor);
		FK_SceneWithInput::executePreRenderingRoutine();
		drawBackground();
		smgr->drawAll();
		drawAll();
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
		if (!firstSceneRender) {
			firstSceneRender = true;
			if (sceneResources.use2DCoordinates) {
				calculateRaycastedCoordinates();
			}
			if (sceneResources.use2DCoordinates && !sceneResources.useImagePreviewsInsteadOfMesh) {
				if (playerCharacter != NULL) {
					playerCharacter->setPosition(getIdleCharacterPosition(player1IsActive));
				}
			}
		}
	};
	void FK_SceneCharacterSelectArcade::dispose(){
		characterPaths.clear();
		character_selection_bgm.stop();
		disposeEmbeddedSceneOptions();
		// clear character infos
		for (std::map<std::string, FK_Character*>::iterator itr = characterInfos.begin(); itr != characterInfos.end(); itr++)
		{
			delete itr->second;
		}
		delete playerCharacter;
		delete soundManager;
		delete announcer;
		characterInfos.clear();
		// dispose all graphics
		FK_SceneWithInput::dispose();
	};
	FK_Scene::FK_SceneType FK_SceneCharacterSelectArcade::nameId(){
		return FK_SceneType::SceneCharacterSelectionArcade;
	};
	bool FK_SceneCharacterSelectArcade::isRunning(){
		u32 sceneTransitionTime = 4000;
		if (backToMenu){
			return false;
		}
		return (!(isPlayerReady() && transitionCounter > sceneTransitionTime));
	};

	// input update
	void FK_SceneCharacterSelectArcade::updateInput(){
		if (backToMenu || !firstSceneRender) {
			return;
		}
		else if (isEmbeddedSceneOptionsActive) {
			updateEmbeddedSceneOptions();
		}
		else if(!(isPlayerReady())){
			updateInputPlayerSelection();
		}
	}

	// go back to main menu
	void FK_SceneCharacterSelectArcade::goBackToMainMenu(){
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		setNextScene(FK_Scene::FK_SceneType::SceneMainMenu);
		backToMenu = true;
	};

	// update input buffer
	void FK_SceneCharacterSelectArcade::updateInputBuffer(){
		if (player1IsActive){
			if (joystickInfo.size() > 0){
				player1input->update(now, inputReceiver->JoypadStatus(0), false);
			}
			else{
				player1input->update(now, inputReceiver->KeyboardStatus(), false);
			}
		}
		else{
			if (joystickInfo.size() > 1){
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			else{
				player2input->update(now, inputReceiver->KeyboardStatus(), false);
			}
		}
	}

	// get active buffer
	FK_InputBuffer* FK_SceneCharacterSelectArcade::getCurrentInputBuffer(){
		if (player1IsActive){
			return player1input;
		}
		else{
			return player2input;
		}
	}

	// input update during character selection
	void FK_SceneCharacterSelectArcade::updateInputPlayerSelection(){
		u32 lastInputTimePlayer = lastInputTime;
		// check input for player
		if (!isPlayerReady()){
			if ((now - lastInputTimePlayer) > FK_SceneCharacterSelect_Base::InputThresholdMs){
				updateInputBuffer();
				u32 directionPlayer = getCurrentInputBuffer()->getLastDirection();
				u32 lastButtonPlayer = getCurrentInputBuffer()->getPressedButtons();// &FK_Input_Buttons::Any_MenuButton;
				if (!forcePlayerSelection && (lastButtonPlayer &  FK_Input_Buttons::Cancel_Button) != 0){
					lastInputTime = now;
					goBackToMainMenu();
					return;
				}
				if (!forcePlayerSelection && selectionMap.count((FK_Input_Buttons)directionPlayer) > 0){
					lastInputTime = now;
					soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
					playerIndex = getNearestNeighbourIndex(playerIndex, selectionMap.at((FK_Input_Buttons)directionPlayer));
				}
				else if (playerIndex <= characterPaths.size() && isCharacterAvailable(playerIndex)){
					// update outfits
					if (!forcePlayerSelection && /*now - lastTimeInputButtonsPlayer > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs &&*/
						(lastButtonPlayer & FK_Input_Buttons::Modifier_Button) != 0){
						lastInputTime = now;
						soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						playerOutfitId += 1;
						playerOutfitId %= (s32)characterAvailableCostumes[playerIndex].size();
						inputReceiver->reset();
					}
					// select character
					else if (forcePlayerSelection || 
						(/*now - lastTimeInputButtonsPlayer > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs && */
						(lastButtonPlayer & FK_Input_Buttons::Selection_Button) != 0)){
						lastInputTime = now;
						if (isCharacterSelectable(playerCharacter)){
							if (playerIndex >= characterPaths.size() && !randomCanBeSelected(lastButtonPlayer)){
								if (availableCharacters.size() > 1) {
									f32 randomIndex = (float)rand() / RAND_MAX;
									u32 charaIndex = (u32)(floor(randomIndex * characterPaths.size()));
									while (!isCharacterAvailable(charaIndex)) {
										randomIndex = (float)rand() / RAND_MAX;
										charaIndex = (u32)(floor(randomIndex * characterPaths.size()));
									}
									playerIndex = charaIndex;
									forcePlayerSelection = true;
								}else{
									soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
									inputReceiver->reset();
									return;
								}
							}
							else{
								selectPlayerCharacter();
								return;
							}
						}
						else{
							soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
							inputReceiver->reset();
							return;
						}
					}
				}
			}
		}
		u32 inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		if (!isEmbeddedSceneOptionsActive && (inputButtons & Help_Button)) {
			callEmbeddedSceneOptions();
		}
	}

	//! setup characters
	void FK_SceneCharacterSelectArcade::setupCharacters(){
		playerPath = std::string();
		updateCharacterSelection(true);
	}

	// check if character can be selected
	bool FK_SceneCharacterSelectArcade::isCharacterSelectable(FK_Character* character) {
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			playerPreview.selectable;
		}
		else {
			return FK_SceneCharacterSelect_Base::isCharacterSelectable(character);
		}
		return true;
	}

	//! update character selection
	void FK_SceneCharacterSelectArcade::updateCharacterSelection(bool force_update){
		f32 playerAngle = player1IsActive ?
			sceneResources.player1CharacterYRotationAngle : sceneResources.player2CharacterYRotationAngle;
		core::vector3df playerPosition = getIdleCharacterPosition(player1IsActive);
		if (force_update || oldPlayerIndex != playerIndex || playerOldOutfitId != playerOutfitId){
			bool changeOnlyOutfit = false;
			if (oldPlayerIndex == playerIndex && playerOldOutfitId != playerOutfitId){
				changeOnlyOutfit = true;
			}
			oldPlayerIndex = playerIndex;
			playerOldOutfitId = playerOutfitId;
			
			if (playerIndex < characterPaths.size()){
				playerPath = characterPaths[playerIndex];
			}
			else{
				playerPath = dummyCharacterDirectory;
			}
			if (playerCharacter != NULL){
				removeCharacterNodeFromCache(playerCharacter);
				delete playerCharacter;
			}
			std::string fullPath = charactersDirectory + playerPath;
			if (!isValidCharacterPath(fullPath) && isValidCharacterPath(playerPath)) {
				fullPath = playerPath;
			}
			loadPlayerCharacter(fullPath, playerPosition, playerOutfitId, playerAngle,
				changeOnlyOutfit);
		}
	}

	/*void FK_SceneCharacterSelectArcade::updateCharacterSelection(bool force_update) {
		if (force_update || oldPlayerIndex != playerIndex || playerOldOutfitId != playerOutfitId) {

			f32 playerAngle = sceneResources.player1CharacterYRotationAngle;
			core::vector3df playerPosition;
			if (player1IsActive) {
				playerAngle = -sceneResources.player1CharacterYRotationAngle;
				playerPosition = sceneResources.player1CharacterPositionIdle;
			}
			else {
				playerAngle = -sceneResources.player2CharacterYRotationAngle;
				playerPosition = sceneResources.player2CharacterPositionIdle;
			}
			f32 defaultScreenAspectRatio = (f32)sceneResources.referenceScreenSize.Width / sceneResources.referenceScreenSize.Height;
			f32 currentScreenAspectRatio = (f32)screenSize.Width / screenSize.Height;
			
			bool changeOnlyOutfit = false;
			if (oldPlayerIndex == playerIndex && playerOldOutfitId != playerOutfitId) {
				changeOnlyOutfit = true;
			}
			oldPlayerIndex = playerIndex;
			playerOldOutfitId = playerOutfitId;

			if (playerIndex < characterPaths.size()) {
				playerPath = characterPaths[playerIndex];
			}
			else {
				playerPath = dummyCharacterDirectory;
			}
			if (playerCharacter != NULL) {
				removeCharacterNodeFromCache(playerCharacter);
				delete playerCharacter;
			}
			loadPlayerCharacter(charactersDirectory + playerPath, playerPosition, playerOutfitId, playerAngle,
				changeOnlyOutfit);
		}
	}*/

	//! load character for player 1
	void FK_SceneCharacterSelectArcade::loadPlayerCharacter(std::string characterPath,
		core::vector3df position, int &outfit_id, f32 playerAngle, bool changeOnlyOutfit){
		bool lighting = false;
		if (!isCharacterAvailable(playerIndex)){
			lighting = true;
		}
		outfit_id = core::clamp(outfit_id, 0, (s32)characterAvailableCostumes[playerIndex].size() - 1);
		s32 tempOutfitId = outfit_id;
		s32 backupOutfitId = tempOutfitId;
		playerCharacter = loadCharacter(playerIndex, characterPath,
			position, playerAngle, tempOutfitId, lighting, !changeOnlyOutfit);// , player1Index >= characterPaths.size());
		if (playerCharacter != NULL && tempOutfitId != backupOutfitId){
			outfit_id = tempOutfitId;
		}
	}

	//! update characters
	void FK_SceneCharacterSelectArcade::updateCharacter(){
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			return;
		}
		if (playerCharacter != NULL){
			bool movePerformed = false;
			playerCharacter->update(0, 0, movePerformed);
		}
	}

	// select character
	void FK_SceneCharacterSelectArcade::selectPlayerCharacter(){
		lastInputTime = now;
		forcePlayerSelection = false;
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		soundManager->playSound(playerCharacter->getName(), 100.0 * gameOptions->getSFXVolume());
		if (!sceneResources.useImagePreviewsInsteadOfMesh) {
			playerCharacter->setStance(FK_Stance_Type::WinStance);
			playerCharacter->setBasicAnimation(FK_BasicPose_Type::WinAnimation, true);
			core::vector3df temp_position = getSelectedCharacterPosition(player1IsActive);
			playerCharacter->setPosition(temp_position);
		}
		playerReady = true;
	};
/*
	void FK_SceneCharacterSelectArcade::selectPlayerCharacter() {
		forcePlayerSelection = false;
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		soundManager->playSound(playerCharacter->getName(), 100.0 * gameOptions->getSFXVolume());
		playerCharacter->setStance(FK_Stance_Type::WinStance);
		playerCharacter->setBasicAnimation(FK_BasicPose_Type::WinAnimation, true);
		core::vector3df temp_position = playerCharacter->getPosition();
		if (!player1IsActive) {
			temp_position.X = sceneResources.player2CharacterPositionSelect.X;
			temp_position.Z = sceneResources.player2CharacterPositionSelect.Z;
			temp_position.Y = sceneResources.player2CharacterPositionSelect.Y;
		}
		else {
			temp_position.X = sceneResources.player1CharacterPositionSelect.X;
			temp_position.Z = sceneResources.player1CharacterPositionSelect.Z;
			temp_position.Y = sceneResources.player1CharacterPositionSelect.Y;
		}
		playerCharacter->setPosition(temp_position);
		playerReady = true;
	};*/

	// get if player is ready
	bool FK_SceneCharacterSelectArcade::isPlayerReady(){
		return playerReady;
	}

	// get all the information out of the scene
	std::string FK_SceneCharacterSelectArcade::getPlayerPath(){
		return playerPath;
	}

	int FK_SceneCharacterSelectArcade::getPlayerOutfitId(){
		return playerOutfitId;
	}

	FK_Character::FK_CharacterOutfit FK_SceneCharacterSelectArcade::getPlayerOutfit(){
		return playerCharacter->getOutfit();
	}

	int FK_SceneCharacterSelectArcade::getPlayerIndex(){
		return playerIndex;
	}

	/* draw player names */
	void FK_SceneCharacterSelectArcade::drawPlayerNames(){
		std::wstring playerName = L"???";
		if (playerIndex >= 0 && isCharacterAvailable(playerIndex)){
			playerName = characterNames[playerIndex];
		}
		if (playerCharacter != NULL && isCharacterAvailable(playerIndex)){
			if (isCharacterSelectable(playerCharacter)){
				playerName = playerCharacter->getWName();
			}
			else{
				playerName = playerCharacter->getWOriginalName();
			}
		}
		// convert names to wide strings format to write them (irrlicht accepts only wide strings here)
		std::wstring plName = playerName;// std::wstring(playerName.begin(), playerName.end());
		/* get width and height of the string*/
		auto font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		auto capfont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		s32 sentenceWidth = font->getDimension(plName.c_str()).Width;
		s32 sentenceHeight = font->getDimension(plName.c_str()).Height;
		s32 sentenceOffsetX = sceneResources.characterNameOffset.X; // distance from screen center
		s32 sentenceOffsetY = sceneResources.characterNameOffset.Y; // distance from the top of the screen
		std::wstring costumeString = L"Costume:";
		s32 costumeStringWidth = font->getDimension(costumeString.c_str()).Width;
		if (player1IsActive){
			// draw player 1 name near the lifebar
			fk_addons::drawBorderedText(font, plName.c_str(),
				core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), false, false);
			// draw player 1 costume name
			sentenceOffsetY = sceneResources.outfitSelectTextOffset.Y;
			sentenceWidth = font->getDimension(costumeString.c_str()).Width;
			sentenceHeight = font->getDimension(costumeString.c_str()).Height;
			fk_addons::drawBorderedText(font, costumeString.c_str(),
				core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY),
				video::SColor(255, 50, 150, 255), video::SColor(128, 0, 0, 0), false, false);
			sentenceOffsetY += 18;
			std::wstring soutfitname;
			if (isCharacterSelectable(playerCharacter)){
				soutfitname = playerCharacter->getOutfit().outfitWName;
				u32 inputButtons = player1input->getPressedButtons();
				if (playerIndex == characterPaths.size() && !randomCanBeSelected(inputButtons) && !playerReady){
					std::wstring names[3]{L"Default", L"Alt. 1", L"Alt. 2"};
					soutfitname = names[playerOutfitId];
				}
			}
			else{
				soutfitname = L"???";
			}
			std::wstring outfitName(soutfitname.begin(), soutfitname.end());
			sentenceWidth = capfont->getDimension(outfitName.c_str()).Width;
			sentenceHeight = capfont->getDimension(outfitName.c_str()).Height;
			fk_addons::drawBorderedText(capfont, outfitName.c_str(),
				core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), false, false);
		}else{
			// draw player 2 name near the lifebar
			sentenceWidth = font->getDimension(plName.c_str()).Width;
			sentenceHeight = font->getDimension(plName.c_str()).Height;
			fk_addons::drawBorderedText(font, plName.c_str(),
				core::rect<s32>(screenSize.Width / 2 + sentenceOffsetX * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 + sentenceWidth + sentenceOffsetX * scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), false, false);
			// draw player 2 costume name
			sentenceOffsetY = sceneResources.outfitSelectTextOffset.Y;
			sentenceWidth = font->getDimension(costumeString.c_str()).Width;
			sentenceHeight = font->getDimension(costumeString.c_str()).Height;
			fk_addons::drawBorderedText(font, costumeString.c_str(),
				core::rect<s32>(screenSize.Width / 2 + sentenceOffsetX * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 + sentenceWidth + sentenceOffsetX * scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY),
				video::SColor(255, 50, 150, 255), video::SColor(128, 0, 0, 0), false, false);
			sentenceOffsetY += 18;
			std::string soutfitname;
			if (isCharacterSelectable(playerCharacter)){
				soutfitname = playerCharacter->getOutfit().outfitName;
				u32 inputButtons = player2input->getPressedButtons();
				if (playerIndex == characterPaths.size() && !randomCanBeSelected(inputButtons) && !playerReady){
					std::string names[3]{"Default", "Alt. 1", "Alt. 2"};
					soutfitname = names[playerOutfitId];
				}
			}
			else{
				soutfitname = "???";
			}
			std::wstring outfitName(soutfitname.begin(), soutfitname.end());
			sentenceWidth = capfont->getDimension(outfitName.c_str()).Width;
			sentenceHeight = capfont->getDimension(outfitName.c_str()).Height;
			fk_addons::drawBorderedText(capfont, outfitName.c_str(),
				core::rect<s32>(screenSize.Width / 2 + sentenceOffsetX * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 + sentenceWidth + sentenceOffsetX * scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), false, false);
		}
		drawButtonIcon();
		drawLockedCostumeSigns();
		sentenceOffsetX += 5;
		s32 costumeDotSentenceOffsetX = sentenceOffsetX * scale_factorX + costumeStringWidth;
		drawCostumeIndex(costumeDotSentenceOffsetX);
	}

	/* draw button icon */
	void FK_SceneCharacterSelectArcade::drawButtonIcon() {
		// draw kick icon near costume
		//std::string modifierButtonIconPath = "movelist\\joypad\\Xbutton.png";

		std::wstring costumeSelectionText = L"change costume";
		std::wstring characterSelectionText = L"select character";
		std::wstring cancelSelectionText = L"cancel selection";
		std::wstring backToMenuText = L"back to menu";

		// player 1
		video::ITexture* modifierButtonTexture = NULL;
		gui::IGUIFont* capfont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		gui::IGUIFont* hintfont = device->getGUIEnvironment()->getFont(fk_constants::FK_SmallNoteFontIdentifier.c_str());

		core::dimension2du offset = capfont->getDimension(L"A");
		core::dimension2du textSize = hintfont->getDimension(L"A");
		u32 iconScreenSize = textSize.Height;
		s32 baseWidth = (s32)iconScreenSize;
		s32 baseHeight = (s32)iconScreenSize;
		s32 iconWidth = baseWidth;
		s32 x = (s32)std::floor(16.f * scale_factorX);
		s32 y = (s32)std::floor(sceneResources.characterNameOffset.Y * scale_factorY - offset.Height);
		s32 x1 = x;
		s32 additionalWidth = 0;
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		video::SColor normalTextColor = video::SColor(255, 196, 196, 196);
		video::SColor normalBorderColor = video::SColor(64, 0, 0, 0);
		video::SColor inactiveTextColor = video::SColor(255, 128, 128, 128);

		if (playerUsesJoypad) {
			modifierButtonTexture = joypadButtonsTexturesMap[
				(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::Modifier_Button]];
		}
		else {
			modifierButtonTexture = keyboardButtonTexture;
		}

		// change text color accordingly
		video::SColor tcolor = normalTextColor;
		//if (!isCharacterSelectable(playerCharacter)) {
		//	tcolor = inactiveTextColor;
		//}

		// draw different icon and text, depending on the keyboard/joypad notation
		// start by drawing the modifier/change costume button
		core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
		std::wstring itemToDraw = L": " + costumeSelectionText;
		if (!isPlayer1Active()) {
			itemToDraw = costumeSelectionText + L": ";
		}
		textSize = hintfont->getDimension(itemToDraw.c_str());
		additionalWidth = textSize.Width;
		iconWidth = baseWidth;
		if (playerUsesJoypad) {
			if (isPlayer1Active()) {
				x1 = x;
			}else{
				x1 = screenSize.Width - x - baseWidth;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			std::wstring itemToDraw = keyboardKeyLayout[FK_Input_Buttons::Modifier_Button];
			core::dimension2du letterSize = hintfont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = baseWidth;
			while (iconWidth < letterSize.Width) {
				iconWidth += baseIconSize;
			}
			if (isPlayer1Active()) {
				x1 = x;
			}
			else {
				x1 = screenSize.Width - x - iconWidth;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				x1,
				y,
				x1 + iconWidth,
				y + baseIconSize);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		if (isPlayer1Active()) {
			x1 = x1 + iconWidth;
		}
		else{
			x1 = x1 - additionalWidth;
		}
		core::rect<s32> destRect = core::rect<s32>(
			x1,
			y,
			x1 + textSize.Width,
			y + textSize.Height);
		fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
			tcolor, normalBorderColor);

		// do the same for selection button
		// check what icon has to be used
		if (!isCharacterSelectable(playerCharacter)) {
			tcolor = inactiveTextColor;
		}
		// offset y
		y -= textSize.Height;

		if (playerUsesJoypad) {
			modifierButtonTexture = joypadButtonsTexturesMap[
				(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::Selection_Button]];
		}
		else {
			modifierButtonTexture = keyboardButtonTexture;
		}

		iconSize = modifierButtonTexture->getSize();
		itemToDraw = L": " + characterSelectionText;
		if (!isPlayer1Active()) {
			itemToDraw = characterSelectionText + L": ";
		}
		textSize = hintfont->getDimension(itemToDraw.c_str());
		additionalWidth = textSize.Width;
		iconWidth = baseWidth;
		if (playerUsesJoypad) {
			if (isPlayer1Active()) {
				x1 = x;
			}
			else {
				x1 = screenSize.Width - x - baseWidth;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			std::wstring itemToDraw = keyboardKeyLayout[FK_Input_Buttons::Selection_Button];
			core::dimension2du letterSize = hintfont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = baseWidth;
			while (iconWidth < letterSize.Width) {
				iconWidth += baseIconSize;
			}
			if (isPlayer1Active()) {
				x1 = x;
			}
			else {
				x1 = screenSize.Width - x - iconWidth;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				x1,
				y,
				x1 + iconWidth,
				y + baseIconSize);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		if (isPlayer1Active()) {
			x1 = x1 + iconWidth;
		}
		else {
			x1 = x1 - additionalWidth;
		}
		destRect = core::rect<s32>(
			x1,
			y,
			x1 + textSize.Width,
			y + textSize.Height);
		fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
			tcolor, normalBorderColor);

		// cancel selection button
		y -= textSize.Height;
		tcolor = normalTextColor;
		if (playerUsesJoypad) {
			modifierButtonTexture = joypadButtonsTexturesMap[
				(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::Cancel_Button]];
		}
		else {
			modifierButtonTexture = keyboardButtonTexture;
		}

		iconSize = modifierButtonTexture->getSize(); 
		itemToDraw = L": " + backToMenuText;
		if (!isPlayer1Active()) {
			itemToDraw = backToMenuText + L": ";
		}
		textSize = hintfont->getDimension(itemToDraw.c_str());
		additionalWidth = textSize.Width;
		iconWidth = baseWidth;
		if (playerUsesJoypad) {
			if (isPlayer1Active()) {
				x1 = x;
			}
			else {
				x1 = screenSize.Width - x - baseWidth;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			std::wstring itemToDraw = keyboardKeyLayout[FK_Input_Buttons::Cancel_Button];
			core::dimension2du letterSize = hintfont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = baseWidth;
			while (iconWidth < letterSize.Width) {
				iconWidth += baseIconSize;
			}
			if (isPlayer1Active()) {
				x1 = x;
			}
			else {
				x1 = screenSize.Width - x - iconWidth;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				x1,
				y,
				x1 + iconWidth,
				y + baseIconSize);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		if (isPlayer1Active()) {
			x1 = x1 + iconWidth;
		}
		else {
			x1 = x1 - additionalWidth;
		}
		tcolor = normalTextColor;
		destRect = core::rect<s32>(
			x1,
			y,
			x1 + textSize.Width,
			y + textSize.Height);
		fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
			tcolor, normalBorderColor);
	}

	///* draw button icon*/
	//void FK_SceneCharacterSelectArcade::drawButtonIcon(){
	//	// draw kick icon near costume
	//	std::string modifierButtonIconPath = "movelist\\joypad\\Xbutton.png";
	//	f32 magnificationAmplitude = 0.05;
	//	f32 magnificationOffset = 0.95;
	//	f32 magnificationPeriodMs = 300.0;
	//	f32 phase = 2 * core::PI * ((f32)now / magnificationPeriodMs);
	//	phase = fmod(phase, 2 * core::PI);
	//	f32 magnificationFactor = (magnificationOffset + magnificationAmplitude * cos(phase));
	//	if (isPlayerReady()){
	//		magnificationFactor = 1.0f;
	//	}
	//	f32 baseWidth = 22 * scale_factorX;
	//	f32 baseHeight = 22 * scale_factorY;
	//	f32 width = baseWidth * magnificationFactor;
	//	f32 height = baseHeight * magnificationFactor;
	//	s32 x = (s32)((16.0f - (width - baseWidth) / 2) * scale_factorX);
	//	s32 y = (s32)((332.0f - (height - baseHeight) / 2) * scale_factorY);
	//	if (!player1IsActive){
	//		x = screenSize.Width - x - width;
	//	}
	//	auto modifierButtonTexture = device->getVideoDriver()->getTexture((mediaPath + modifierButtonIconPath).c_str());
	//	core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
	//	fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
	//		core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
	//		core::vector2d<f32>((f32)width / iconSize.Width, (f32)height / iconSize.Height), true,
	//		video::SColor(255, 255, 255, 255),
	//		core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
	//}

	/* draw disclaimer for no arcade ending */
	void FK_SceneCharacterSelectArcade::drawNoEndingDisclaimer() {
		if (!characterHasAvailableEndingMap[getPlayerIndex()]) {
			s32 x1, y1;
			f32 width, height;
			width = screenSize.Width / 4;
			s32 centerX1 = screenSize.Width / 6;
			core::dimension2d<u32> size = noArcadeEndingSign->getSize();
			f32 scaleFactor = width / size.Width;
			irr::core::vector2df scale(scaleFactor, scaleFactor);
			height = size.Height * scaleFactor;
			// player 1
			y1 = (s32)std::floor(screenSize.Height / 2 - height / 2);
			x1 = (s32)std::floor(centerX1 - width / 2);
			if (!player1IsActive) {
				fk_addons::draw2DImage(driver, noArcadeEndingSign,
					core::rect<s32>(0, 0, size.Width, size.Height),
					core::vector2d<s32>(x1, y1),
					core::vector2d<s32>(0, 0), 0, scale,
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			// player 2
			centerX1 = screenSize.Width - centerX1;
			x1 = (s32)std::floor(centerX1 - width / 2);
			if (player1IsActive) {
				fk_addons::draw2DImage(driver, noArcadeEndingSign,
					core::rect<s32>(0, 0, size.Width, size.Height),
					core::vector2d<s32>(x1, y1),
					core::vector2d<s32>(0, 0), 0, scale,
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
		}
	};

	// draw survival record if needed
	void FK_SceneCharacterSelectArcade::drawSurvivalRecord() {
		if (arcadeType == FK_SceneArcadeType::ArcadeSurvival) {
			if (survivalRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = 0;
			}

			s32 basicOffsetX = 60;
			s32 basicOffsetY = 190;
			core::dimension2d<u32> targetPreviewSize(96, 96);

			// load record preview
			video::ITexture* recordPreviewTex;
			std::string playerPreviewPath = survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].first;
			if (!playerPreviewPath.empty()) {
				s32 pos = 0;
				std::string tempStr = playerPreviewPath;
				// find mesh path
				std::string recordCharacterPath;
				std::string recordCharacterOutfit;
				pos = tempStr.find('\\');
				if (pos != std::string::npos) {
					recordCharacterPath = tempStr.substr(0, pos);
					recordCharacterOutfit = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				recordCharacterPath += "\\";
				std::string characterPath = charactersDirectory + recordCharacterPath + recordCharacterOutfit;
				std::string filename = characterPath + "FightPreview.png";
				if (fk_addons::fileExists(filename.c_str())) {
					recordPreviewTex = driver->getTexture(filename.c_str());
				}
				else {
					recordPreviewTex = driver->getTexture((charactersDirectory + recordCharacterPath + "FightPreview.png").c_str());
				}

				if (recordPreviewTex) {
					auto miniatureSize = recordPreviewTex->getSize();
					s32 x1, x2, y1, y2;
					y1 = screenSize.Height - basicOffsetY * scale_factorY;
					y2 = y1 + (s32)targetPreviewSize.Height * scale_factorY;
					if (player1IsActive) {
						x1 = screenSize.Width -
							basicOffsetX * scale_factorX - (s32)targetPreviewSize.Width * scale_factorX;
					}
					else {
						x1 = basicOffsetX * scale_factorX;
					}
					x2 = x1 + (s32)targetPreviewSize.Width * scale_factorX;
					driver->draw2DImage(recordPreviewTex,
						core::rect<s32>(x1, y1,
							x2,
							y2),
						core::rect<s32>(0, 0, miniatureSize.Width, miniatureSize.Height));
				}
			}

			u32 recordNumberOfWins = survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second;
			std::wstring winMatchName = L"wins";
			std::wstring recordWins = std::to_wstring(recordNumberOfWins) + L" " + winMatchName;
			auto font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
			s32 sentenceWidth = font->getDimension(recordWins.c_str()).Width;
			s32 separatorWidth = font->getDimension(L":").Width;
			s32 sentenceHeight = font->getDimension(L"5").Height;
			s32 sentenceOffsetX = basicOffsetX * scale_factorX; // distance from screen center
			s32 sentenceOffsetY = basicOffsetY * scale_factorY + sentenceHeight;
			video::SColor recordColor(255, 250, 250, 0);
			video::SColor normalColor(255, 250, 255, 255);

			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordWins, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordWins, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
					false, false);
			}
			std::wstring recordString = L"Record:";
			sentenceWidth = font->getDimension(recordString.c_str()).Width;
			sentenceOffsetY += sentenceHeight;
			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}

			recordString = std::to_wstring(gameOptions->getAILevel());
			sentenceWidth = font->getDimension(recordString.c_str()).Width;
			sentenceOffsetY += sentenceHeight;
			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					normalColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					normalColor, video::SColor(128, 0, 0, 0),
					false, false);
			}

			recordString = L"AI level:";
			sentenceWidth = font->getDimension(recordString.c_str()).Width;
			sentenceOffsetY += sentenceHeight;
			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
		}
	}

	// draw time attack record if needed
	void FK_SceneCharacterSelectArcade::drawTimeAttackRecord() {
		if (arcadeType == FK_SceneArcadeType::ArcadeTimeAttack) {
			if (timeAttackRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].second = (356400000 + 3540000 + 59000 + 999);
			}

			s32 basicOffsetX = 60;
			s32 basicOffsetY = 190;
			core::dimension2d<u32> targetPreviewSize(96, 96);

			// load record preview
			video::ITexture* recordPreviewTex;
			std::string playerPreviewPath = timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].first;
			if (!playerPreviewPath.empty()) {
				s32 pos = 0;
				std::string tempStr = playerPreviewPath;
				// find mesh path
				std::string recordCharacterPath;
				std::string recordCharacterOutfit;
				pos = tempStr.find('\\');
				if (pos != std::string::npos) {
					recordCharacterPath = tempStr.substr(0, pos);
					recordCharacterOutfit = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				recordCharacterPath += "\\";
				std::string characterPath = charactersDirectory + recordCharacterPath + recordCharacterOutfit;
				std::string filename = characterPath + "FightPreview.png";
				if (fk_addons::fileExists(filename.c_str())) {
					recordPreviewTex = driver->getTexture(filename.c_str());
				}
				else {
					recordPreviewTex = driver->getTexture((charactersDirectory + recordCharacterPath + "FightPreview.png").c_str());
				}

				if (recordPreviewTex) {
					auto miniatureSize = recordPreviewTex->getSize();
					s32 x1, x2, y1, y2;
					y1 = screenSize.Height - basicOffsetY * scale_factorY;
					y2 = y1 + (s32)targetPreviewSize.Height * scale_factorY;
					if (player1IsActive) {
						x1 = screenSize.Width -
							basicOffsetX * scale_factorX - (s32)targetPreviewSize.Width * scale_factorX;
					}
					else {
						x1 = basicOffsetX * scale_factorX;
					}
					x2 = x1 + (s32)targetPreviewSize.Width * scale_factorX;
					driver->draw2DImage(recordPreviewTex,
						core::rect<s32>(x1, y1,
							x2,
							y2),
						core::rect<s32>(0, 0, miniatureSize.Width, miniatureSize.Height));
				}
			}

			auto font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
			u32 timeMS = timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].second;
			u32 timeS = timeMS / 1000;
			u32 hours = timeS / 3600;
			u32 minutes = (timeS - hours * 3600) / 60;
			u32 seconds = (timeS - hours * 3600 - minutes * 60);
			u32 centiseconds = (timeMS - (hours * 3600 + minutes * 60 + seconds) * 1000) / 10;
			if (hours > 99) {
				timeMS = 356400000 + 3540000 + 59000 + 999;
				timeS = timeMS / 1000;
				hours = timeS / 3600;
				minutes = (timeS - hours * 3600) / 60;
				seconds = (timeS - hours * 3600 - minutes * 60);
				centiseconds = (timeMS - (hours * 3600 + minutes * 60 + seconds) * 1000) / 10;
			}
			u32 digits[8] = {
				centiseconds - (centiseconds / 10) * 10,
				centiseconds / 10,
				seconds - (seconds / 10) * 10,
				seconds / 10,
				minutes - (minutes / 10) * 10,
				minutes / 10,
				hours - (hours / 10) * 10,
				hours / 10
			};
			if (!player1IsActive) {
				digits[0] = hours / 10;
				digits[1] = hours - (hours / 10) * 10;
				digits[2] = minutes / 10;
				digits[3] = minutes - (minutes / 10) * 10;
				digits[4] = seconds / 10;
				digits[5] = seconds - (seconds / 10) * 10;
				digits[6] = centiseconds / 10;
				digits[7] = centiseconds - (centiseconds / 10) * 10;
			}
			s32 characterWidth = font->getDimension(L"5").Width;
			s32 separatorWidth = font->getDimension(L":").Width;
			s32 sentenceHeight = font->getDimension(L"5").Height;
			s32 sentenceOffsetX = basicOffsetX * scale_factorX; // distance from screen center
			s32 sentenceOffsetY = basicOffsetY * scale_factorY + sentenceHeight; // distance from the bottom of the screen
			for (u32 i = 0; i < 8; ++i) {
				if (player1IsActive) {
					fk_addons::drawBorderedText(font, std::to_wstring(digits[i]), core::rect<s32>(
						screenSize.Width - sentenceOffsetX - characterWidth,
						screenSize.Height - sentenceHeight - sentenceOffsetY,
						screenSize.Width - sentenceOffsetX,
						screenSize.Height - sentenceOffsetY),
						video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
						true, false);
					sentenceOffsetX += characterWidth;
					if (i == 3 || i == 5 || i == 1) {
						fk_addons::drawBorderedText(font, L":", core::rect<s32>(
							screenSize.Width - sentenceOffsetX - separatorWidth,
							screenSize.Height - sentenceHeight - sentenceOffsetY,
							screenSize.Width - sentenceOffsetX,
							screenSize.Height - sentenceOffsetY),
							video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
							true, false);
						sentenceOffsetX += separatorWidth;
					}
				}
				else {
					fk_addons::drawBorderedText(font, std::to_wstring(digits[i]), core::rect<s32>(
						sentenceOffsetX,
						screenSize.Height - sentenceHeight - sentenceOffsetY,
						sentenceOffsetX + characterWidth,
						screenSize.Height - sentenceOffsetY),
						video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
						true, false);
					sentenceOffsetX += characterWidth;
					if (i == 3 || i == 5 || i == 1) {
						fk_addons::drawBorderedText(font, L":", core::rect<s32>(
							sentenceOffsetX,
							screenSize.Height - sentenceHeight - sentenceOffsetY,
							sentenceOffsetX + separatorWidth,
							screenSize.Height - sentenceOffsetY),
							video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
							true, false);
						sentenceOffsetX += separatorWidth;
					}
				}
			}
			video::SColor recordColor(255, 250, 250, 0);
			video::SColor normalColor(255, 255, 255, 255);
			sentenceOffsetX = basicOffsetX * scale_factorX;
			std::wstring recordString = L"Record:";
			u32 sentenceWidth = font->getDimension(recordString.c_str()).Width;
			sentenceOffsetY += sentenceHeight;
			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}

			recordString = std::to_wstring(gameOptions->getAILevel());
			sentenceWidth = font->getDimension(recordString.c_str()).Width;
			sentenceOffsetY += sentenceHeight;
			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					normalColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					normalColor, video::SColor(128, 0, 0, 0),
					false, false);
			}

			recordString = L"AI level:";
			sentenceWidth = font->getDimension(recordString.c_str()).Width;
			sentenceOffsetY += sentenceHeight;
			if (player1IsActive) {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					screenSize.Width - sentenceOffsetX - sentenceWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
			else {
				fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
					sentenceOffsetX,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					sentenceOffsetX + sentenceWidth,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					false, false);
			}
		}
	}

	void FK_SceneCharacterSelectArcade::drawLockedCostumeSigns(){
		s32 x1, y1;
		f32 width, height;
		width = screenSize.Width / 4;
		s32 centerX1 = screenSize.Width / 6;
		core::dimension2d<u32> size = lockedCostumeSign->getSize();
		f32 scaleFactor = width / size.Width;
		irr::core::vector2df scale(scaleFactor, scaleFactor);
		height = size.Height * scaleFactor;
		// player 1
		y1 = (s32)std::floor(screenSize.Height / 2 - height / 2);
		x1 = (s32)std::floor(centerX1 - width / 2);
		if (!isCharacterSelectable(playerCharacter)){
			if (player1IsActive){
				fk_addons::draw2DImage(driver, lockedCostumeSign,
					core::rect<s32>(0, 0, size.Width, size.Height),
					core::vector2d<s32>(x1, y1),
					core::vector2d<s32>(0, 0), 0, scale, true,
					video::SColor(255, 255, 255, 255),
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else{
				// player 2
				centerX1 = screenSize.Width - centerX1;
				x1 = (s32)std::floor(centerX1 - width / 2);
				fk_addons::draw2DImage(driver, lockedCostumeSign,
					core::rect<s32>(0, 0, size.Width, size.Height),
					core::vector2d<s32>(x1, y1),
					core::vector2d<s32>(0, 0), 0, scale, true,
					video::SColor(255, 255, 255, 255),
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
		}
	};

	/* draw all*/
	void FK_SceneCharacterSelectArcade::drawAll(){
		/* draw screen overlay */
		drawScreenOverlay();
		/*draw character screen*/
		drawCharacterScreen();
		//drawStageScreen();
		if (player1IsActive){
			drawPlayerSelector(playerIndex, isPlayerReady(), 
				sceneResources.characterSelectorOscillationPhases.first, 
				sceneResources.characterSelectorColors.first);
		}
		else{
			drawPlayerSelector(playerIndex, isPlayerReady(), 
				sceneResources.characterSelectorOscillationPhases.second, 
				sceneResources.characterSelectorColors.second);
		}
		if (sceneResources.showArcadeModeCompletedLogo) {
			if (arcadeType != FK_SceneArcadeType::ArcadeSurvival && arcadeType != FK_SceneArcadeType::ArcadeTimeAttack) {
				drawCompletedArcadeIcon();
			}
		}
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			drawCharacterPreview(player1IsActive, playerPreview);
		}
		if (arcadeType != FK_SceneArcadeType::ArcadeSurvival &&
			arcadeType != FK_SceneArcadeType::ArcadeTimeAttack) {
			drawNoEndingDisclaimer();
		}
		drawSurvivalRecord();
		drawTimeAttackRecord();
		drawPlayerNames();
		if (isEmbeddedSceneOptionsActive) {
			embeddedSceneOptions->drawAll();
		}
	};

	void FK_SceneCharacterSelectArcade::drawCostumeIndex(s32 scaledSentenceOffsetX){
		s32 baseX = screenSize.Width / 2 - scaledSentenceOffsetX;
		s32 baseOutfitDotSize = std::ceil(12 * scale_factorX);
		core::dimension2d<u32> dotSize = costumeDotTex->getSize();
		core::rect<s32> sourceRect(0, 0, dotSize.Width, dotSize.Height);
		if (baseOutfitDotSize % 2 != 0){
			baseOutfitDotSize += 1;
		}
		core::vector2df baseScale((f32)baseOutfitDotSize / dotSize.Width, (f32)baseOutfitDotSize / dotSize.Width);
		core::vector2df scale = baseScale;
		s32 sentenceOffsetY = sceneResources.costumeDotsVerticalPosition;
		if (player1IsActive){
			if (playerCharacter != NULL){
				s32 outfitSize = characterAvailableCostumes[playerIndex].size();
				f32 scaleFactor = 1.f;
				if (outfitSize > sceneResources.maximumNumberOfFullSizeOutfitDots) {
					scale.X = baseScale.X * (f32)sceneResources.maximumNumberOfFullSizeOutfitDots / (f32)outfitSize;
					scaleFactor = (f32)sceneResources.maximumNumberOfFullSizeOutfitDots / (f32)outfitSize;
				}
				else {
					scale.X = baseScale.X;
				}
				for (int i = 0; i < outfitSize; ++i){
					s32 x = baseX + (i - outfitSize) * (s32)std::floor(baseOutfitDotSize * scaleFactor);
					s32 y = sentenceOffsetY * scale_factorY;
					core::position2d<s32> position(x, y);
					fk_addons::draw2DImage(driver, playerOutfitId == i ? currentCostumeDotTex : costumeDotTex,
						sourceRect, position, core::vector2d<s32>(0, 0), 0, scale, true,
						video::SColor(255, 255, 255, 255),
						core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
			}
		}
		else{
			if (playerCharacter != NULL){
				s32 outfitSize = characterAvailableCostumes[playerIndex].size();
				f32 scaleFactor = 1.f;
				if (outfitSize > 5){
					scale.X = baseScale.X * 5.f / (f32)outfitSize;
					scaleFactor = 5.f / (f32)outfitSize;
				}
				else{
					scale.X = baseScale.X;
				}
				baseX = screenSize.Width / 2 + scaledSentenceOffsetX + outfitSize * sourceRect.getWidth() * scale.X;
				for (int i = 0; i < outfitSize; ++i){
					s32 x = baseX + (i - outfitSize) * (s32)std::floor(baseOutfitDotSize * scaleFactor);
					s32 y = sentenceOffsetY * scale_factorY;
					core::position2d<s32> position(x, y);
					fk_addons::draw2DImage(driver, playerOutfitId == i ? currentCostumeDotTex : costumeDotTex,
						sourceRect, position, core::vector2d<s32>(0, 0), 0, scale, true,
						video::SColor(255, 255, 255, 255),
						core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
			}
		}
	}

	/* check if ending is available to play */
	bool FK_SceneCharacterSelectArcade::checkIfEndingIsAvailable(std::string& characterPath) {
		std::string charactersPath = FK_SceneCharacterSelectArcade::charactersDirectory;
		std::string endingFilePath =
			charactersPath + characterPath +
			fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeEndingFileName;
		std::ifstream inputFile(endingFilePath.c_str());
		if (!inputFile) {
			inputFile.clear();
			inputFile.close();
			return false;
		}
		return true;
	}

	/* load character files */
	void FK_SceneCharacterSelectArcade::loadCharacterList(){
		availableCharactersArcade.clear();
		characterPaths.clear();
		characterPathsArcade.clear();
		availableCharacters.clear();
		characterNames.clear();
		characterHasAvailableEndingMap.clear();
		drawLoadingScreen(0);
		dummyCharacterDirectory = "chara_dummy\\";
		std::string characterFileName = charactersDirectory + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName.c_str());
		u32 characterId = 0;
		u32 characterIdArcadeOpponent = 0;
		std::string charaPath = std::string();
		while (!characterFile.eof() && characterFile >> charaPath){
			std::string key = charaPath;
			charaPath += "\\";
			int availabilityIdentifier;
			characterFile >> availabilityIdentifier;
			if (availabilityIdentifier == CharacterUnlockKeys::Character_AvailableFromStart){
				availableCharacters.push_back(characterId);
				characterPaths.push_back(charaPath);
				characterPathsArcade.push_back(charaPath);
				availableCharactersArcade.push_back(characterIdArcadeOpponent);
				characterHasAvailableEndingMap[characterId] = checkIfEndingIsAvailable(charaPath);
				characterId += 1;
				characterIdArcadeOpponent += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_UnlockableAndShown ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_NoArcadeOpponent){
				characterPaths.push_back(charaPath);
				characterPathsArcade.push_back(charaPath);
				characterHasAvailableEndingMap[characterId] = checkIfEndingIsAvailable(charaPath);
				if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()){
					if (availabilityIdentifier != CharacterUnlockKeys::Character_Unlockable_NoArcadeOpponent){
						availableCharactersArcade.push_back(characterIdArcadeOpponent);
					}
					availableCharacters.push_back(characterId);
				}
				else if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSurvival) {
					if (availabilityIdentifier != CharacterUnlockKeys::Character_Unlockable_NoArcadeOpponent) {
						availableCharactersArcade.push_back(characterIdArcadeOpponent);
					}
				}
				//if (availabilityIdentifier != CharacterUnlockKeys::Character_Unlockable_NoArcadeOpponent){
				//	characterIdArcadeOpponent += 1;
				//}
				characterIdArcadeOpponent += 1;
				characterId += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_Hidden_NoArcadeOpponent){
				if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()){
					characterPathsArcade.push_back(charaPath);
					if (availabilityIdentifier != CharacterUnlockKeys::Character_Unlockable_Hidden_NoArcadeOpponent) {
						availableCharactersArcade.push_back(characterIdArcadeOpponent);
						//characterIdArcadeOpponent += 1;
					}
					availableCharacters.push_back(characterId);
					characterPaths.push_back(charaPath);
					characterHasAvailableEndingMap[characterId] = checkIfEndingIsAvailable(charaPath);
					characterIdArcadeOpponent += 1;
					characterId += 1;
				}
			}
		};
		characterFile.close();
		// load workshop characters
		for (auto item : enabledWorkshopItems) {
			if (item.type == FK_WorkshopContentManager::WorkshopItemType::Character &&
				item.enabled) {
				charaPath = "..\\" + fk_constants::FK_WorkshopFolder + fk_constants::FK_CharactersFileFolder + item.path;
				availableCharacters.push_back(characterId);
				if (!charaPath.empty()) {
					availableCharacters.push_back(characterId);
					characterPaths.push_back(charaPath);
				}
				characterHasAvailableEndingMap[characterId] = checkIfEndingIsAvailable(charaPath);
				characterId += 1;
			}
		};
		// load dlc characters
		for (auto item : enabledDLCItems) {
			if (item.type == FK_WorkshopContentManager::WorkshopItemType::Character &&
				item.enabled) {
				charaPath = "..\\" + fk_constants::FK_DLCFolder + fk_constants::FK_CharactersFileFolder + item.path;
				availableCharacters.push_back(characterId);
				characterPaths.push_back(charaPath);
				characterPathsArcade.push_back(charaPath);
				characterHasAvailableEndingMap[characterId] = checkIfEndingIsAvailable(charaPath);
				characterId += 1;
			}
		};
		characterHasAvailableEndingMap[characterId] = true;
		availableCharacters.push_back(characterId);
		finalizeCharacterList();
	};

	/* finalize character list (costumes, character infos, et cetera)*/
	void FK_SceneCharacterSelectArcade::finalizeCharacterList() {
		f32 totalProgress = (f32)characterPaths.size() + 1;
		f32 currentProgress = 0;
		u32 selectableCharacterIndex = 0;
		for (std::map<std::string, FK_Character*>::iterator itr = characterInfos.begin(); itr != characterInfos.end(); itr++)
		{
			delete itr->second;
		}
		characterInfos.clear();
		for each(std::string chara_path in characterPaths) {
			std::string temp;
			std::string fullPath = charactersDirectory + chara_path;
			if (isValidCharacterPath(charactersDirectory + chara_path)) {
				loadSingleCharacterFile(charactersDirectory + chara_path, temp);
			}
			else if (isValidCharacterPath(chara_path)) {
				fullPath = chara_path;
				loadSingleCharacterFile(chara_path, temp);
			}
			else {
				temp = std::string();
			}
			std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
			characterNames.push_back(wtemp);
			loadSingleCharacterOutfitList(selectableCharacterIndex, characterInfos[fullPath]);
			selectableCharacterIndex += 1;
			currentProgress += 1.f;
			drawLoadingScreen(100.f * currentProgress / totalProgress);
			std::string key = temp;
			std::replace(key.begin(), key.end(), ' ', '_');
			bool arcadeCompleted = characterStatisticsStructure[key].numberOfBeatenArcadeMode > 0;
			characterHasCompletedArcade.push_back(arcadeCompleted);
		}
		std::string temp;
		loadSingleCharacterFile(charactersDirectory + dummyCharacterDirectory, temp);
		loadSingleCharacterOutfitList(selectableCharacterIndex, characterInfos[charactersDirectory + dummyCharacterDirectory]);
		std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
		std::string key = temp;
		std::replace(key.begin(), key.end(), ' ', '_');
		bool arcadeCompleted = characterStatisticsStructure[key].numberOfBeatenArcadeMode > 0;
		characterHasCompletedArcade.push_back(arcadeCompleted);
		characterNames.push_back(wtemp);
		drawLoadingScreen(100.f);
	}

	void FK_SceneCharacterSelectArcade::loadSingleCharacterOutfitList(u32 characterId, FK_Character* character) {
		u32 maxNumberOfOutfits = character->getNumberOfOutfits();
		for (u32 i = 0; i < maxNumberOfOutfits; ++i) {
			FK_Character::FK_CharacterOutfit outfit = character->getOutfit(i);
			if (outfit.isAvailableFromBeginning) {
				characterAvailableCostumes[characterId].push_back(std::pair<u32, bool>(i, true));
			}else{
				std::string outfitKey = character->getOriginalName() + "\\" + outfit.outfitName;
				std::replace(outfitKey.begin(), outfitKey.end(), ' ', '_');
				bool unlockFlag = std::find(unlockedOutfits.begin(), unlockedOutfits.end(), outfitKey) != unlockedOutfits.end();
				characterAvailableCostumes[characterId].push_back(std::pair<u32, bool>(i, unlockFlag));
			}
		}
	}

	FK_Character* FK_SceneCharacterSelectArcade::loadCharacter(u32 characterIndex, std::string characterPath,
		core::vector3df position, float rotationAngleY, int &outfit_id, bool lighting,
		bool onlyValidOutfit){
		if (!isCharacterAvailable(playerIndex)){
			return NULL;
		}
		bool basicLighting = lighting;
		/*FK_Character* character = new FK_Character;
		character->loadBasicVariables(
			databaseAccessor,
			"character.txt", characterPath,
			commonResourcesPath,
			smgr, outfit_id);
		bool validOutfit = false;*/
		FK_Character* character = new FK_Character;
		character->setCharacterSelectionVariablesFromCharacter(*characterInfos[characterPath]);
		u32 costumeListSize = characterAvailableCostumes[characterIndex].size();
		outfit_id = core::clamp(outfit_id, 0, (s32)costumeListSize - 1);
		character->setOutfitId(characterAvailableCostumes[characterIndex][outfit_id].first);
		bool validOutfit = false;
		if (!basicLighting){
			while (validOutfit == false){
				if (!characterAvailableCostumes[characterIndex][outfit_id].second) {
					lighting = true;
					validOutfit = onlyValidOutfit ? false : true;
				}
				else {
					lighting = basicLighting;
					validOutfit = true;
				}
				if (validOutfit == false) {
					outfit_id += 1;
					outfit_id %= (s32)costumeListSize;
					character->setOutfitId(characterAvailableCostumes[characterIndex][outfit_id].first);
				}
			}
		}
		else{
			outfit_id = 0;
			validOutfit = true;
			character->loadBasicVariables(
				databaseAccessor,
				"character.txt", characterPath,
				commonResourcesPath,
				smgr, outfit_id);
		}
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			playerPreview.filename = characterPath + character->getOutfitPath() + "FightPreview.png";
			if (fk_addons::fileExists(playerPreview.filename.c_str())) {
				playerPreview.texture = driver->getTexture((characterPath +
					character->getOutfitPath() + "FightPreview.png").c_str());
			}
			else {
				playerPreview.texture = driver->getTexture((characterPath + "FightPreview.png").c_str());
			}
			playerPreview.selectable = !lighting;
		}
		else {
			character->loadMeshsAndEffects(core::vector3df(-999, -999, -999),
				core::vector3df(0, rotationAngleY, 0), 
				std::string(),
				false, false, false);
			//character->setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
			character->toggleLighting(lighting);
			if (character != NULL) {
				if (character->getAnimatedMesh() != NULL) {
					character->setPosition(position);
				}
			}
		}
		// check if the outfit has an original voiceover clip
		std::string voiceName = characterPath + character->getOutfitPath() + fk_constants::FK_VoiceoverEffectsFileFolder + "name.wav";
		std::ifstream testFile(voiceName.c_str());
		if (testFile.good()){
			testFile.clear();
			testFile.close();
			soundManager->addSoundFullPath(character->getName(), voiceName.c_str());
		}
		else{
			testFile.clear();
			testFile.close();
			soundManager->addSoundFullPath(character->getName(), characterPath + fk_constants::FK_VoiceoverEffectsFileFolder + "name.wav");
		}
		return character;
	}

	bool FK_SceneCharacterSelectArcade::isPlayer1Active(){
		return player1IsActive;
	}

	// setup icons for keyboard and joypad input
	void FK_SceneCharacterSelectArcade::setupSystemButtonIcons() {
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		if (isPlayer1Active()) {
			if (joystickInfo.size() > 0) {
				playerUsesJoypad = true;
				mapId = 1;
				currentJoypadName = std::string(joystickInfo[0].Name.c_str());
			}
			else {
				playerUsesJoypad = false;
				mapId = 0;
			}
		}else{
			if (joystickInfo.size() > 1) {
				playerUsesJoypad = true;
				mapId = 2;
				currentJoypadName = std::string(joystickInfo[1].Name.c_str());
			}
			else {
				playerUsesJoypad = false;
				mapId = 0;
			}
		}
		setIconsBasedOnCurrentInputMap(buttonInputLayout, keyboardKeyLayout,
			mapId, playerUsesJoypad, currentJoypadName
		);
	}

	/* load stage files */
	std::vector<std::string> FK_SceneCharacterSelectArcade::loadStageList(){
		std::vector<std::string> stagePaths;
		bool stageReady = false;
		std::string stageFileName = stagesResourcePath + fk_constants::FK_AvailableStagesFileName;
		std::ifstream stageFile(stageFileName.c_str());
		while (!stageFile.eof()){
			std::string stagePath;
			s32 unlockId;
			stageFile >> stagePath >> unlockId;
			if (stagePath.empty()){
				continue;
			}
			if (unlockId == StageUnlockKeys::Stage_AvailableFromStart || 
				(unlockId == StageUnlockKeys::Stage_Unlockable && 
					std::find(unlockedStages.begin(), unlockedStages.end(), stagePath) != unlockedStages.end())){
				stagePath += "\\";
				stagePaths.push_back(stagePath);
			}
		};
		stageFile.close();
		//add workshop and dlc stages
		for (auto item : enabledWorkshopItems) {
			if (item.type == FK_WorkshopContentManager::WorkshopItemType::Stage &&
				item.enabled) {
				std::string stagePath;
				stagePath = "..\\" + fk_constants::FK_WorkshopFolder + fk_constants::FK_StagesFileFolder + item.path;
				if (!stagePath.empty()) {
					stagePaths.push_back(stagePath);
				}
			}
		};
		//add workshop and dlc stages
		for (auto item : enabledDLCItems) {
			if (item.type == FK_WorkshopContentManager::WorkshopItemType::Stage &&
				item.enabled) {
				std::string stagePath;
				stagePath = "..\\" + fk_constants::FK_DLCFolder + fk_constants::FK_StagesFileFolder + item.path;
				stagePaths.push_back(stagePath);
			}
		};
		return stagePaths;
	}

	void FK_SceneCharacterSelectArcade::drawCompletedArcadeIcon()
	{
		if (completedArcadeModeLogoTexture != NULL) {
			core::dimension2d<u32> texSize = completedArcadeModeLogoTexture->getSize();
			core::rect<s32> sourceRect(0, 0, texSize.Width, texSize.Height);
			core::rect<f32> logoPosition = sceneResources.arcadeLogoPosition;
			int size = characterIcons.size();
			core::rect<s32> iconRect = getCharacterIconPosition(0);
			f32 scaleX = logoPosition.LowerRightCorner.X * (f32)iconRect.getWidth() / (f32)texSize.Width;
			f32 scaleY = logoPosition.LowerRightCorner.Y * (f32)iconRect.getHeight() / (f32)texSize.Height;
			core::vector2df scale(scaleX, scaleY);
			// all character icons must have the same width and height
			for (int i = 0; i < size; i++) {
				if (characterHasCompletedArcade[i] && isCharacterAvailable(i)) {
					iconRect = getCharacterIconPosition(i);
					s32 x = iconRect.UpperLeftCorner.X +
						(s32)std::ceil(logoPosition.UpperLeftCorner.X * iconRect.getWidth());
					s32 y = iconRect.UpperLeftCorner.Y +
						(s32)std::ceil(logoPosition.UpperLeftCorner.Y * iconRect.getHeight());
					core::position2d<s32> position(x, y);
					fk_addons::draw2DImage(driver, completedArcadeModeLogoTexture,
						sourceRect, position, core::vector2d<s32>(0, 0), 0, scale, true,
						video::SColor(255, 255, 255, 255),
						core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
			}
		}
	}

	std::vector<FK_ArcadeCluster> FK_SceneCharacterSelectArcade::buildArcadeArray() {
		if (arcadeType == FK_SceneArcadeType::ArcadeSurvival) {
			return buildSurvivalArcadeArray();
		}
		else {
			return buildClassicArcadeArray();
		}
		return std::vector<FK_ArcadeCluster>();
	}

	std::vector<FK_ArcadeCluster> FK_SceneCharacterSelectArcade::buildClassicArcadeArray(){
		drawArcadeLadderLoadingScreen(0.f);
		f32 loadingProgress = 0.f;
		std::vector<FK_ArcadeCluster> new_arcadeArray;
		int numberOfMatches = 7;
		std::string arcadeBossCharacterPath = "chara_donner\\";
		std::string stageBossPath = "theLoathing";
		/* open arcade configuration file, if it exists */
		FK_ArcadeSettings arcadeSettings;
		arcadeSettings.readFromFile(configurationFilesPath + fk_constants::FK_ArcadeConfigFileName, charactersDirectory);
		numberOfMatches = arcadeSettings.numberOfMatches;
		arcadeBossCharacterPath = arcadeSettings.bossCharacter1Path;
		stageBossPath = arcadeSettings.bossCharacter1Stage;
		std::string climaticBattleStagePath = arcadeSettings.climaticBattleDefaultStage;
		std::vector<u32> randomCharacters = availableCharactersArcade;
		bool hasClimaticBattle = arcadeSettings.allowClimaticBattle;
		if (getArcadeType() == FK_Scene::FK_SceneArcadeType::ArcadeSpecial2 || 
			getArcadeType() == FK_Scene::FK_SceneArcadeType::ArcadeSpecial3 ||
			getArcadeType() == FK_Scene::FK_SceneArcadeType::ArcadeSurvival){
			hasClimaticBattle = false;
		}
		FK_ArcadeFlow characterArcadeFlow;
		std::string meshFolder = playerCharacter->getOutfit().outfitDirectory;
		std::ifstream flowFile(charactersDirectory + playerPath + meshFolder + fk_constants::FK_CharacterArcadeFolder +
			fk_constants::FK_CharacterArcadeFlowFileName);
		if (flowFile.good()){
			flowFile.clear();
			flowFile.close();
			characterArcadeFlow.readFromFile(charactersDirectory + playerPath + meshFolder + 
				fk_constants::FK_CharacterArcadeFolder +
				fk_constants::FK_CharacterArcadeFlowFileName);
		}
		else{
			flowFile.clear();
			flowFile.close();
			characterArcadeFlow.readFromFile(charactersDirectory + playerPath + fk_constants::FK_CharacterArcadeFolder +
				fk_constants::FK_CharacterArcadeFlowFileName);
		}
		hasClimaticBattle = characterArcadeFlow.hasClimaticBattle && hasClimaticBattle;
		if (hasClimaticBattle){
			if (!characterArcadeFlow.climaticBattleStage.empty()){
				climaticBattleStagePath = characterArcadeFlow.climaticBattleStage;
			}
		}
		// remove random character dummy (not necessary anymore)
		//randomCharacters.pop_back();
		// remove the player index to avoid mirror matches
		if (playerIndex >= characterPaths.size()){
			characterPaths.push_back(playerPath);
		}
		s32 arcadeIndex = -1;
		s32 arcadeIndexClimaticOpponent = -1;
		for (u32 i = 0; i < randomCharacters.size(); ++i){
			if (characterPaths[playerIndex] == characterPathsArcade[randomCharacters[i]]){
				arcadeIndex = (s32)randomCharacters[i];
			}
			if (hasClimaticBattle){
				if (characterArcadeFlow.climaticBattleOpponentPath == characterPathsArcade[randomCharacters[i]]){
					arcadeIndexClimaticOpponent = (s32)randomCharacters[i];
				}
			}
		}

		loadingProgress += 5.0f;
		drawArcadeLadderLoadingScreen(loadingProgress);

		// set arcade boss
		if (!characterArcadeFlow.arcadeBossOpponentPath.empty()) {
			arcadeBossCharacterPath = characterArcadeFlow.arcadeBossOpponentPath;
		}
		if (!characterArcadeFlow.arcadeBossStagePath.empty()) {
			stageBossPath = characterArcadeFlow.arcadeBossStagePath;
		}
		if (arcadeIndex >= 0){
			randomCharacters.erase(std::remove(randomCharacters.begin(), randomCharacters.end(), (u32)arcadeIndex), randomCharacters.end());
		}
		if (arcadeIndexClimaticOpponent >= 0){
			randomCharacters.erase(std::remove(randomCharacters.begin(), randomCharacters.end(), (u32)arcadeIndexClimaticOpponent), randomCharacters.end());
		}
		// remove the boss character from the vector
		s32 bossIndex = -1;
		auto it = std::find(characterPathsArcade.begin(), characterPathsArcade.end(), arcadeBossCharacterPath);
		if (it != characterPathsArcade.end())
		{
			bossIndex = std::distance(characterPathsArcade.begin(), it);
		}
		if (bossIndex >= 0){
			randomCharacters.erase(std::remove(randomCharacters.begin(), randomCharacters.end(), bossIndex), randomCharacters.end());
		}

		loadingProgress += 5.0f;
		drawArcadeLadderLoadingScreen(loadingProgress);

		// using built-in random generator to shuffle vector
		std::random_device rd;
		std::mt19937 randomGenerator(rd());
		std::shuffle(randomCharacters.begin(), randomCharacters.end(), randomGenerator);
		// build player paths
		std::vector<std::string> availablePlayerPaths = std::vector<std::string>();
		for (int i = 0; i < randomCharacters.size(); ++i){
			availablePlayerPaths.push_back(characterPathsArcade[randomCharacters[i]]);
		}
		// do the same for stages
		std::vector<std::string> availableStages = loadStageList();
		// remove the boss stage
		availableStages.erase(std::remove(availableStages.begin(), availableStages.end(), stageBossPath), availableStages.end());
		// remove climatic battle stage
		if (hasClimaticBattle){
			availableStages.erase(std::remove(availableStages.begin(), availableStages.end(), climaticBattleStagePath), availableStages.end());
		}
		std::shuffle(availableStages.begin(), availableStages.end(), randomGenerator);
		// prepare AI array

		f32 loadingProgressAdvancePerCharacter = (100.0f - loadingProgress) / (numberOfMatches + 1);
		s32 baseAIlevel = (s32)gameOptions->getAILevel();
		std::vector<s32> AI_levels = buildAILevelArray(baseAIlevel, numberOfMatches + 1);
		for (int i = 0; i < numberOfMatches; ++i){
			std::string stagePath = std::string();
			std::string AIPlayerPath = std::string();
			u32 AIoutfitId = 0;
			std::string AIoutfitName = std::string();
			if (hasClimaticBattle && i == numberOfMatches - 1){
				stagePath = climaticBattleStagePath;
				AIPlayerPath = characterArcadeFlow.climaticBattleOpponentPath;
				AIoutfitId = characterArcadeFlow.climaticBattleOpponentOutfitId;
				AIoutfitName = characterArcadeFlow.climaticBattleOpponentOutfitName;
			}
			else{
				if (i < availablePlayerPaths.size() && i < availableStages.size()){
					AIPlayerPath = availablePlayerPaths[i];
					AIoutfitId = 0;
					if (characterHasCompletedArcade[playerIndex]) {
						AIoutfitId = rand() * (s32)characterAvailableCostumes[randomCharacters[i]].size() / RAND_MAX;
					}
					stagePath = availableStages[i];
				}
				else{
					break;
				}
			}
			FK_ArcadeCluster cluster(charactersDirectory, AIPlayerPath, AIoutfitId, AIoutfitName,
				stagePath, AI_levels[i]);
			new_arcadeArray.push_back(cluster);
			loadingProgress += loadingProgressAdvancePerCharacter;
			drawArcadeLadderLoadingScreen(loadingProgress);
		}
		u32 bossOutfitId = 0;
		if (arcadeIndex == bossIndex){
			bossOutfitId = 1;
		}

		std::string bossOutfitName = std::string();
		if (!characterArcadeFlow.arcadeBossOpponentOutfitName.empty()) {
			bossOutfitName = characterArcadeFlow.arcadeBossOpponentOutfitName;
		}
		FK_ArcadeCluster cluster(charactersDirectory, arcadeBossCharacterPath, bossOutfitId, bossOutfitName,
			stageBossPath, AI_levels[AI_levels.size() - 1]);
		new_arcadeArray.push_back(cluster);
		drawArcadeLadderLoadingScreen(100.f);
		return new_arcadeArray;
	}

	// build survival array
	std::vector<FK_ArcadeCluster> FK_SceneCharacterSelectArcade::buildSurvivalArcadeArray() {
		f32 loadingProgress = 0.f;
		std::vector<FK_ArcadeCluster> new_arcadeArray;
		int numberOfMatches = 7;
		std::string arcadeBossCharacterPath = "chara_donner\\";
		std::string stageBossPath = "theLoathing";
		/* open arcade configuration file, if it exists */
		FK_ArcadeSettings arcadeSettings;
		arcadeSettings.readFromFile(configurationFilesPath + fk_constants::FK_ArcadeConfigFileName, charactersDirectory);
		numberOfMatches = arcadeSettings.numberOfMatches;
		numberOfMatches = numberOfMatches * 2 + 1;
		if (numberOfMatches > (s32)(availableCharactersArcade.size()) - 2) {
			numberOfMatches = (s32)(availableCharactersArcade.size()) - 2;
		}
		arcadeBossCharacterPath = arcadeSettings.bossCharacter1Path;
		stageBossPath = arcadeSettings.bossCharacter1Stage;

		std::vector<u32> randomCharacters = availableCharactersArcade;
		
		FK_ArcadeFlow characterArcadeFlow;
		std::string meshFolder = playerCharacter->getOutfit().outfitDirectory;
		std::ifstream flowFile(charactersDirectory + playerPath + meshFolder + fk_constants::FK_CharacterArcadeFolder +
			fk_constants::FK_CharacterArcadeFlowFileName);
		if (flowFile.good()) {
			flowFile.clear();
			flowFile.close();
			characterArcadeFlow.readFromFile(charactersDirectory + playerPath + meshFolder +
				fk_constants::FK_CharacterArcadeFolder +
				fk_constants::FK_CharacterArcadeFlowFileName);
		}
		else {
			flowFile.clear();
			flowFile.close();
			characterArcadeFlow.readFromFile(charactersDirectory + playerPath + fk_constants::FK_CharacterArcadeFolder +
				fk_constants::FK_CharacterArcadeFlowFileName);
		}
		
		// remove random character dummy (not necessary anymore)
		//randomCharacters.pop_back();
		// remove the player index to avoid mirror matches
		if (playerIndex >= characterPaths.size()) {
			characterPaths.push_back(playerPath);
		}
		s32 arcadeIndex = -1;
		s32 arcadeIndexClimaticOpponent = -1;
		for (u32 i = 0; i < randomCharacters.size(); ++i) {
			if (characterPaths[playerIndex] == characterPathsArcade[i]) {
				arcadeIndex = (s32)i;
			}
		}

		loadingProgress += 5.0f;
		drawArcadeLadderLoadingScreen(loadingProgress);

		// set arcade boss
		if (!characterArcadeFlow.arcadeBossOpponentPath.empty()) {
			arcadeBossCharacterPath = characterArcadeFlow.arcadeBossOpponentPath;
		}
		if (!characterArcadeFlow.arcadeBossStagePath.empty()) {
			stageBossPath = characterArcadeFlow.arcadeBossStagePath;
		}
		if (arcadeIndex >= 0) {
			randomCharacters.erase(std::remove(randomCharacters.begin(), randomCharacters.end(), (u32)arcadeIndex), randomCharacters.end());
		}
		// remove the boss character from the vector
		s32 bossIndex = -1;
		auto it = std::find(characterPathsArcade.begin(), characterPathsArcade.end(), arcadeBossCharacterPath);
		if (it != characterPathsArcade.end())
		{
			bossIndex = std::distance(characterPathsArcade.begin(), it);
		}
		if (bossIndex >= 0) {
			randomCharacters.erase(std::remove(randomCharacters.begin(), randomCharacters.end(), bossIndex), randomCharacters.end());
		}

		// remove the boss character from the vector
		bossIndex = -1;

		loadingProgress += 5.0f;
		drawArcadeLadderLoadingScreen(loadingProgress);
		std::random_device rd;
		std::mt19937 randomGenerator(rd());
		// using built-in random generator to shuffle vector
		std::shuffle(randomCharacters.begin(), randomCharacters.end(), randomGenerator);
		// build player paths
		std::vector<std::string> availablePlayerPaths = std::vector<std::string>();
		for (int i = 0; i < randomCharacters.size(); ++i) {
			availablePlayerPaths.push_back(characterPathsArcade[randomCharacters[i]]);
		}
		// do the same for stages
		std::vector<std::string> availableStages = loadStageList();
		// remove the boss stage
		availableStages.erase(std::remove(availableStages.begin(), availableStages.end(), stageBossPath), availableStages.end());
		std::shuffle(availableStages.begin(), availableStages.end(), randomGenerator);
		// prepare AI array

		f32 loadingProgressAdvancePerCharacter = (100.0f - loadingProgress) / (numberOfMatches + 1);
		s32 baseAIlevel = (s32)gameOptions->getAILevel();
		std::vector<s32> AI_levels = buildAILevelArray(baseAIlevel, numberOfMatches + 1, 0.5f, 2.0f);
		for (int i = 0; i < numberOfMatches; ++i) {
			std::string stagePath = std::string();
			std::string AIPlayerPath = std::string();
			u32 AIoutfitId = 0;
			std::string AIoutfitName = std::string();
			if (i < availablePlayerPaths.size() && i < availableStages.size()) {
				AIPlayerPath = availablePlayerPaths[i];
				AIoutfitId = rand() * (s32)characterAvailableCostumes[randomCharacters[i]].size() / RAND_MAX;
				stagePath = availableStages[i];
			}
			else {
				break;
			}
			FK_ArcadeCluster cluster(charactersDirectory, AIPlayerPath, AIoutfitId, AIoutfitName,
				stagePath, AI_levels[i]);
			new_arcadeArray.push_back(cluster);
			loadingProgress += loadingProgressAdvancePerCharacter;
			drawArcadeLadderLoadingScreen(loadingProgress);
		}
		u32 bossOutfitId = 0;
		if (arcadeIndex == bossIndex) {
			bossOutfitId = 1;
		}

		std::string bossOutfitName = std::string();
		if (!characterArcadeFlow.arcadeBossOpponentOutfitName.empty()) {
			bossOutfitName = characterArcadeFlow.arcadeBossOpponentOutfitName;
		}
		FK_ArcadeCluster cluster(charactersDirectory, arcadeBossCharacterPath, bossOutfitId, bossOutfitName,
			stageBossPath, AI_levels[AI_levels.size() - 1]);
		new_arcadeArray.push_back(cluster);

		drawArcadeLadderLoadingScreen(100.f);
		return new_arcadeArray;
	}

	std::vector<s32> FK_SceneCharacterSelectArcade::buildAILevelArray(
		s32 basicLevel, s32 numberOfMatches, f32 lowVal, f32 highVal){
		std::vector<s32> levels;
		s32 maxAI = (s32)ceil((f32)basicLevel * highVal);
		if (maxAI <= 5){
			maxAI = 5;
		}
		s32 minAI = (s32)floor((f32)basicLevel * lowVal);
		if (minAI <= 0){
			minAI = 0;
		}
		f32 increment = (f32)(maxAI - minAI) / (f32)numberOfMatches;
		for (int i = 0; i < numberOfMatches; ++i){
			s32 currentAI = (s32)ceil((f32)minAI + i * increment);
			levels.push_back(currentAI);
		}
		return levels;
	}

	FK_Scene::FK_SceneArcadeType FK_SceneCharacterSelectArcade::getArcadeType(){
		return arcadeType;
	}

	// setup input for players
	void FK_SceneCharacterSelectArcade::setupInputForPlayers()
	{
		FK_SceneWithInput::setupInputForPlayers();
		setupSystemButtonIcons();
	}

	//! draw loading screen
	void FK_SceneCharacterSelectArcade::drawArcadeLadderLoadingScreen(f32 completionPercentage) {
		if (!sceneResources.drawLoadingScreen) {
			return;
		}
		// get arena background for loading screen
		std::string resourcePath = mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath +
			sceneResources.arcadeLadderloadingScreenTexture;
		video::ITexture* loadingTexture = NULL;
		if (!sceneResources.arcadeLadderloadingScreenTexture.empty()) {
			loadingTexture = driver->getTexture(resourcePath.c_str());
		}
		if (!loadingTexture) {
			sceneResources.arcadeLadderloadingScreenTexture = std::string();
		}

		// create color array for background
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));

		FK_SceneWithInput::executePreRenderingRoutine();
		// draw stage picture
		if (loadingTexture) {
			core::dimension2d<u32> texSize = loadingTexture->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			core::vector2df scale((f32)((f32)screenSize.Width / (f32)texSize.Width),
				(f32)((f32)screenSize.Height / (f32)texSize.Height));
			video::SColor color(255, 255, 255, 255);
			fk_addons::draw2DImage(driver, loadingTexture, sourceRect, core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			video::SColor const color_bck = video::SColor(255, 0, 0, 0);
			driver->draw2DRectangle(color_bck, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		// draw overlay
		drawLoadingScreenProgressBar(completionPercentage);
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}
}