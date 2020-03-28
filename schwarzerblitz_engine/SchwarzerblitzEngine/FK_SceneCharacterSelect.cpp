#include "FK_SceneCharacterSelect.h"
#include "FK_Database.h"
#include "ExternalAddons.h"
#include <fstream>
#include <iostream>
#include <Windows.h>

namespace fk_engine{
	FK_SceneCharacterSelect::FK_SceneCharacterSelect() : FK_SceneCharacterSelect_Base(){
		backToMenu = false;
	};

	FK_SceneCharacterSelect::~FK_SceneCharacterSelect(){
		
	};
	/* setup scene*/
	void FK_SceneCharacterSelect::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions,
		FK_SceneFreeMatchType newfreeMatchType,
		int inputPlayer1Index, int inputPlayer2Index, 
		u32 inputPlayer1OutfitId, u32 inputPlayer2OutfitId,
		bool player1IsCPUFlag, bool player2IsCPUFlag){
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		freeMatchType = newfreeMatchType;
		charactersDirectory = mediaPath + fk_constants::FK_CharactersFileFolder;
		stagesResourcePath = mediaPath + fk_constants::FK_StagesFileFolder;
		voiceClipsPath = mediaPath + fk_constants::FK_VoiceoverEffectsFileFolder;
		soundPath = commonResourcesPath + fk_constants::FK_SoundEffectsFileFolder;
		systemBGMPath = commonResourcesPath + fk_constants::FK_SystemBGMFileFolder;
		currentStageName = std::wstring();
		offset = core::vector2d<f32>(0.f, 0.f);
		stageIndex = -1;
		oldStageIndex = 0;
		player1Index = inputPlayer1Index;
		player2Index = inputPlayer2Index;
		player1Character = NULL;
		player2Character = NULL;
		oldIndexPlayer1 = -1;
		oldIndexPlayer2 = -1;
		player1OutfitId = inputPlayer1OutfitId;
		player2OutfitId = inputPlayer2OutfitId;
		player1OldOutfitId = inputPlayer1OutfitId;
		player2OldOutfitId = inputPlayer2OutfitId;
		//lastTimeInputButtonsPlayer1 = 0;
		//lastTimeInputButtonsPlayer2 = 0;
		firstSceneRender = false;
		//picturesPerRow = 3;
		//numberOfRows = 4;
		then = 0;
		now = 0;
		cycleId = 0;
		lastInputTimePlayer1 = 0;
		lastInputTimePlayer2 = 0;
		transitionCounter = 0;
		player1IsCPU = player1IsCPUFlag;
		player2IsCPU = player2IsCPUFlag;
		stageSelectionAnnounced = false;
		backToMenu = false;
		availableCharacters = std::vector<u32>();
		player1SelectionMS = minimumTimeToLoadCharacterMS;
		player2SelectionMS = minimumTimeToLoadCharacterMS;
		player1MovesetTag = std::string();
		player2MovesetTag = std::string();
		selectionMap = std::map<FK_Input_Buttons, SelectionDirection>{
			{ FK_Input_Buttons::Down_Direction, SelectionDirection::Down },
			{ FK_Input_Buttons::Up_Direction, SelectionDirection::Up },
			{ FK_Input_Buttons::Left_Direction, SelectionDirection::Left },
			{ FK_Input_Buttons::Right_Direction, SelectionDirection::Right },
		};
		// set fonts
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		capfont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		hintfont = device->getGUIEnvironment()->getFont(fk_constants::FK_SmallNoteFontIdentifier.c_str());
		// read save file
		FK_SceneWithInput::readSaveFile();
		forcePlayer1Selection = false;
		forcePlayer2Selection = false;
		allowRingoutFlag = true;
		modifierButtonTexture = NULL;
		player1UsesJoypad = false;
		player2UsesJoypad = false;
		initialize();
	}
	void FK_SceneCharacterSelect::initialize(){
		player1CharacterLoaded.store(false);
		player2CharacterLoaded.store(false);
		player1ThreadProcessing.store(false);
		player2ThreadProcessing.store(false);
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
		loadStageList();
		setupTextures();
		setupCamera();
		setIconSizeAndOffset();
		if (player1Index < 0){
			if (sceneResources.startingPlayerIndexes.first >= 0) {
				player1Index = sceneResources.startingPlayerIndexes.first;
			}
			else {
				player1Index = 0;
			}
		}
		if (player2Index < 0){
			if (sceneResources.startingPlayerIndexes.second >= 0) {
				player2Index = sceneResources.startingPlayerIndexes.second;
			}
			else {
				player2Index = maxColumns - 1;
			}
		}
		setupCharacters();
	};

	void FK_SceneCharacterSelect::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		updateInput();
		updateCharacters();
		if (canSelectStage()){
			if (!stageSelectionAnnounced){
				announcer->playSound("select_arena", 100.0 * gameOptions->getSFXVolume());
				stageSelectionAnnounced = true;
			}
			updateStageSelection();
		}
		else{
			updateCharacterSelection();
		}
		// if this is the first update cycle, play "select character" voice and the BGM
		if (cycleId == 0){
			cycleId = 1;
			announcer->playSound("select_character", 100.0 * gameOptions->getSFXVolume());
			character_selection_bgm.play();
		}
		// update transition counter, if everything is ready to go
		if (player1Ready && player2Ready){
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
				if (player1Character != NULL) {
					player1Character->setPosition(getIdleCharacterPosition(true));
				}
				if (player2Character != NULL) {
					player2Character->setPosition(getIdleCharacterPosition(false));
				}
			}
		}
	};
	void FK_SceneCharacterSelect::dispose(){
		stageNames.clear();
		stagePaths.clear();
		characterPaths.clear();
		character_selection_bgm.stop();
		disposeEmbeddedSceneOptions();
		// clear character infos
		for (std::map<std::string, FK_Character*>::iterator itr = characterInfos.begin(); itr != characterInfos.end(); itr++)
		{
			delete itr->second;
		}
		delete player1Character;
		delete player2Character;
		delete soundManager;
		delete announcer;
		characterInfos.clear();
		// clear the scene of all scene nodes
		FK_SceneWithInput::dispose();
	};
	FK_Scene::FK_SceneType FK_SceneCharacterSelect::nameId(){
		return FK_SceneType::SceneCharacterSelection;
	};
	bool FK_SceneCharacterSelect::isRunning(){
		u32 sceneTransitionTime = 2000;
		if (backToMenu){
			return false;
		}
		return (!(isPlayer1Ready() && isPlayer2Ready() && isStageSelected() && transitionCounter > sceneTransitionTime));
	};

	// input update
	void FK_SceneCharacterSelect::updateInput(){
		if (backToMenu || !firstSceneRender){
			return;
		}
		if (isEmbeddedSceneOptionsActive) {
			updateEmbeddedSceneOptions();
		}
		else {
			if (canSelectStage()) {
				if (!isStageSelected()) {
					updateInputStageSelection();
				}
			}
			else if (!(isPlayer1Ready() && isPlayer2Ready())) {
				updateInputPlayerSelection();
			}
		}
	}

	// setup icons for keyboard and joypad input
	void FK_SceneCharacterSelect::setupSystemButtonIcons() {
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		if (joystickInfo.size() > 0) {
			player1UsesJoypad = true;
			mapId = 1;
			currentJoypadName = std::string(joystickInfo[0].Name.c_str());
		}
		else {
			player1UsesJoypad = false;
			if (!player2CanSelect()) {
				mapId = 0;
			}
			else {
				mapId = 1;
			}
		}
		setIconsBasedOnCurrentInputMap(buttonInputLayoutPlayer1, keyboardKeyLayoutPlayer1,
			mapId, player1UsesJoypad, currentJoypadName
		);
		mapId = -1;
		currentJoypadName = std::string();
		if (joystickInfo.size() > 1) {
			player2UsesJoypad = true;
			mapId = 2;
			currentJoypadName = std::string(joystickInfo[1].Name.c_str());
		}
		else {
			player2UsesJoypad = false;
			if (!player1CanSelect() || joystickInfo.size() > 0) {
				mapId = 0;
			}
			else {
				mapId = 2;
			}
		}
		setIconsBasedOnCurrentInputMap(buttonInputLayoutPlayer2, keyboardKeyLayoutPlayer2,
			mapId, player2UsesJoypad, currentJoypadName
		);
	}

	// check if character can be selected
	bool FK_SceneCharacterSelect::isCharacterSelectable(FK_Character* character, bool player1Selection) {
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			if (player1Selection) {
				return player1Preview.selectable;
			}else{
				return player2Preview.selectable;
			}
		}else{
			return FK_SceneCharacterSelect_Base::isCharacterSelectable(character);
		}
		return true;
	}

	// make a basic setup for player input
	void FK_SceneCharacterSelect::setupInputForPlayers(){
		// in case there are no joypads available, split keyboard
		if (!(player1IsCPU || player2IsCPU) && joystickInfo.size() <= 0){
			player1input = new FK_InputBuffer(1, inputMapper->getKeyboardMap(1), 200, 50, 60);
			player2input = new FK_InputBuffer(2, inputMapper->getKeyboardMap(2), 200, 50, 60);
		}
		else if (player1IsCPU && !player2IsCPU && joystickInfo.size() <= 0){
			player1input = new FK_InputBuffer(1, inputMapper->getKeyboardMap(-1), 200, 50, 60);
			player2input = new FK_InputBuffer(2, inputMapper->getKeyboardMap(0), 200, 50, 60);
		}
		/*else if (player1IsCPU && joystickInfo.size() == 1){
			player2input = new FK_InputBuffer(1, inputMapper->getKeyboardMap(-1), 200, 50, 60);
			player1input = new FK_InputBuffer(2, inputMapper->getJoypadMap(1), 200, 70, 40);
		}*/
		else{
			FK_SceneWithInput::setupInputForPlayers();
		}
		setupSystemButtonIcons();
	}

	void FK_SceneCharacterSelect::updateInputStageSelection(){
		//u32 lastInputTimePlayer1 = player1input->getLastInputTime();
		//u32 lastInputTimePlayer2 = player2input->getLastInputTime();
		// check input for player 1
		if (player1CanSelect()){
			if (now - lastInputTimePlayer1 > FK_SceneCharacterSelect::InputThresholdStageSelectionMs){
				if (joystickInfo.size() > 0){
					player1input->update(now, inputReceiver->JoypadStatus(0), false);
				}
				else{
					player1input->update(now, inputReceiver->KeyboardStatus(), false);
				}
				u32 directionPlayer1 = player1input->getLastDirection();
				u32 rawInputPlayer1 = player1input->getPressedButtons();
				u32 lastButtonPlayer1 = rawInputPlayer1 & FK_Input_Buttons::Any_MenuButton;
				if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button){
					cancelStageSelection();
					lastInputTimePlayer1 = now;
					if (!player2CanSelect()){
						cancelPlayer2CharacterSelection();
					}
					else{
						cancelPlayer1CharacterSelection();
					}
					return;
				}
				else {
					if (directionPlayer1 == FK_Input_Buttons::Right_Direction) {
						soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						lastInputTimePlayer1 = now;
						stageIndex += 1;
						if (stageIndex == stagePaths.size()) {
							stageIndex = -1;
						}
					}
					else if (directionPlayer1 == FK_Input_Buttons::Left_Direction) {
						soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						lastInputTimePlayer1 = now;
						if (stageIndex == -1) {
							stageIndex = stagePaths.size() - 1;
						}
						else {
							stageIndex -= 1;
						}
					}
					else /*if (now - lastTimeInputButtonsPlayer1 > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs) {
						lastTimeInputButtonsPlayer1 = now;*/
						if ((rawInputPlayer1 & (FK_Input_Buttons::Modifier_Button/* | FK_Input_Buttons::Kick_Button*/)) != 0) {
							if (databaseAccessor.ringoutAllowed() && sceneResources.allowRingoutToggle && sceneResources.drawStageBorderBox) {
								toggleRingoutFlag();
								return;
							}
						}
						else {
							// select stage
							if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button) {
								selectStage();
								return;
							}
						}
					//}
				}
			}
		}
		// check input for player 2
		if (player2CanSelect()){
			if (now - lastInputTimePlayer2 > FK_SceneCharacterSelect::InputThresholdStageSelectionMs){
				if (joystickInfo.size() > 1){
					player2input->update(now, inputReceiver->JoypadStatus(1),false);
				}
				else{
					player2input->update(now, inputReceiver->KeyboardStatus(),false);
				}
				u32 directionPlayer2 = player2input->getLastDirection();
				u32 rawInputPlayer2 = player2input->getPressedButtons();
				u32 lastButtonPlayer2 = rawInputPlayer2 & FK_Input_Buttons::Any_MenuButton;
				if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button){
					cancelStageSelection();
					if (!player1CanSelect()){
						cancelPlayer1CharacterSelection();
					}
					else{
						cancelPlayer2CharacterSelection();
					}
					return;
				}
				else {
					if (directionPlayer2 == FK_Input_Buttons::Right_Direction) {
						soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						stageIndex += 1;
						if (stageIndex == stagePaths.size()) {
							stageIndex = -1;
						}
						lastInputTimePlayer2 = now;
					}
					else if (directionPlayer2 == FK_Input_Buttons::Left_Direction) {
						soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						if (stageIndex == -1) {
							stageIndex = stagePaths.size() - 1;
						}
						else {
							stageIndex -= 1;
						}
						lastInputTimePlayer2 = now;
					}
					else /*if (now - lastTimeInputButtonsPlayer2 > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs) {
						lastTimeInputButtonsPlayer2 = now;*/
						if ((rawInputPlayer2 & (FK_Input_Buttons::Modifier_Button/* | FK_Input_Buttons::Kick_Button*/)) != 0) {
							toggleRingoutFlag();
							lastInputTimePlayer2 = now;
							return;
						}
						else {
							// select stage
							if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button) {
								selectStage();
								lastInputTimePlayer2 = now;
								return;
							}
						}
					//}
				}
			}
		}
	}

	// go back to main menu
	void FK_SceneCharacterSelect::goBackToMainMenu(){
		soundManager->playSound("cancel" ,100.0 * gameOptions->getSFXVolume());
		setNextScene(FK_Scene::FK_SceneType::SceneMainMenu);
		backToMenu = true;
	};

	// input update during character selection
	void FK_SceneCharacterSelect::updateInputPlayerSelection(){
		/*u32 lastInputTimePlayer1 = player1input->getLastInputTime();
		u32 lastInputTimePlayer2 = player2input->getLastInputTime();*/
		player2SelectionMS += delta_t_ms;
		player1SelectionMS += delta_t_ms;
		// check input for player 1
		if (player1CanSelect() || isPlayer2Ready()){
			if (now - (player1CanSelect() ? lastInputTimePlayer1 : lastInputTimePlayer2) > 
				FK_SceneCharacterSelect_Base::InputThresholdMs){
				if (joystickInfo.size() > 0){
					player1input->update(now, inputReceiver->JoypadStatus(0), false);
				}
				else{
					player1input->update(now, inputReceiver->KeyboardStatus(), false);
				}
				u32 directionPlayer1 = player1input->getLastDirection();
				u32 lastButtonPlayer1 = player1input->getPressedButtons();// &FK_Input_Buttons::Any_MenuButton;
				if (player2CanSelect() && !player1CanSelect()){
					if (joystickInfo.size() > 1){
						player2input->update(now, inputReceiver->JoypadStatus(1),false);
					}
					else{
						player2input->update(now, inputReceiver->KeyboardStatus(),false);
					}
					directionPlayer1 = player2input->getLastDirection();
					lastButtonPlayer1 = player2input->getPressedButtons();// &FK_Input_Buttons::Any_MenuButton;
				}
				if (isPlayer1Ready() && !isPlayer2Ready()){
					if (!forcePlayer1Selection && (lastButtonPlayer1 & FK_Input_Buttons::Cancel_Button)!= 0){
						cancelPlayer1CharacterSelection();
						if (!player2CanSelect()){
							return;
						}
					}
				}
				else{
					if (!forcePlayer1Selection && (lastButtonPlayer1 & FK_Input_Buttons::Cancel_Button) != 0){
						if (!player2Ready){
							goBackToMainMenu();
							return;
						}
					}
					if (!forcePlayer1Selection && (directionPlayer1 & FK_Input_Buttons::Any_Direction) != 0){
						if (selectionMap.count((FK_Input_Buttons)directionPlayer1) > 0){
							soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
							player1Index = getNearestNeighbourIndex(player1Index, selectionMap[(FK_Input_Buttons)directionPlayer1]);
							player1SelectionMS = 0;
							(player1CanSelect() ? lastInputTimePlayer1 : lastInputTimePlayer2) = now;
						}
					}
					else if (player1Index <= characterPaths.size()){
						if (/*player1ThreadProcessing.load() || */player1SelectionMS < minimumTimeToLoadCharacterMS){
							return;
						}
						// update outfits
						if (!forcePlayer1Selection && /*now - lastTimeInputButtonsPlayer1 > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs &&*/
							((lastButtonPlayer1 & FK_Input_Buttons::Modifier_Button) != 0/* ||
							(lastButtonPlayer1 & FK_Input_Buttons::Kick_Button) != 0*/) && !isPlayer1Ready() &&
							isCharacterAvailable(player1Index)){
							soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
							inputReceiver->reset();
							(player1CanSelect() ? lastInputTimePlayer1 : lastInputTimePlayer2) = now;
							player1OutfitId += 1;
							player1OutfitId %= player1Character->getNumberOfOutfits();
							//inputReceiver->reset();
						}
						// select character
						else if (forcePlayer1Selection || 
							(/*now - lastTimeInputButtonsPlayer1 > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs && */
								(lastButtonPlayer1 & FK_Input_Buttons::Selection_Button) != 0)){
							//if (player1CharacterLoaded.load()){
								(player1CanSelect() ? lastInputTimePlayer1 : lastInputTimePlayer2) = now;
								if (isCharacterSelectable(player1Character, true)){
									if (player1Index >= characterPaths.size()){
										if (!randomCanBeSelected(lastButtonPlayer1)){
											f32 randomIndex = (float)rand() / RAND_MAX;
											u32 charaIndex = (u32)(floor(randomIndex * characterPaths.size()));
											while (!isCharacterAvailable(charaIndex)){
												randomIndex = (float)rand() / RAND_MAX;
												charaIndex = (u32)(floor(randomIndex * characterPaths.size()));
											}
											player1Index = charaIndex;
											forcePlayer1Selection = true;
										}
										else{
											if (isSimpleModeSelected(lastButtonPlayer1)){
												player1MovesetTag = player1Character->SimpleModeMoveFileTag;
											}
											else{
												player1MovesetTag = std::string();
											}
											selectPlayer1Character();
										}
									}
									else{
										if (isSimpleModeSelected(lastButtonPlayer1)){
											player1MovesetTag = player1Character->SimpleModeMoveFileTag;
										}
										else{
											player1MovesetTag = std::string();
										}
										selectPlayer1Character();
										if (!player2CanSelect() || !player1CanSelect()){
											return;
										}
									}
								}
								else{
									soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
									inputReceiver->reset();
									return;
								}
							//}
						}
					}
				}
			}
		}
		if (player2CanSelect() || isPlayer1Ready()){
			// check input for player 2
			if (now - (player2CanSelect() ? lastInputTimePlayer2 : lastInputTimePlayer1) > FK_SceneCharacterSelect_Base::InputThresholdMs){
				if (joystickInfo.size() > 1){
					player2input->update(now, inputReceiver->JoypadStatus(1),false);
				}
				else{
					player2input->update(now, inputReceiver->KeyboardStatus(),false);
				}
				u32 directionPlayer2 = player2input->getLastDirection();
				u32 lastButtonPlayer2 = player2input->getPressedButtons();
				if (!player2CanSelect() && player1CanSelect()){
					if (joystickInfo.size() > 0){
						player1input->update(now, inputReceiver->JoypadStatus(0),false);
					}
					else{
						player1input->update(now, inputReceiver->KeyboardStatus(),false);
					}
					directionPlayer2 = player1input->getLastDirection();
					lastButtonPlayer2 = player1input->getPressedButtons();
				}
				if (isPlayer2Ready() && !isPlayer1Ready()){
					if (!forcePlayer2Selection && (lastButtonPlayer2 & FK_Input_Buttons::Cancel_Button) != 0){
						cancelPlayer2CharacterSelection();
						return;
					}
				}
				else{
					if (!forcePlayer2Selection && (lastButtonPlayer2 & FK_Input_Buttons::Cancel_Button) != 0){
						if (!player1Ready){
							goBackToMainMenu();
							return;
						}
					}
					if (!forcePlayer2Selection && (directionPlayer2 & FK_Input_Buttons::Any_Direction) != 0){
						if (selectionMap.count((FK_Input_Buttons)directionPlayer2) > 0){
							soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
							player2Index = getNearestNeighbourIndex(player2Index, selectionMap[(FK_Input_Buttons)directionPlayer2]);
							player2SelectionMS = 0;
							(player2CanSelect() ? lastInputTimePlayer2 : lastInputTimePlayer1) = now;
						}
					}
					else if (player2Index <= characterPaths.size()){
						if (/*player2ThreadProcessing.load() || */player2SelectionMS < minimumTimeToLoadCharacterMS){
							return;
						}
						// update outfits
						if (!forcePlayer2Selection && /*now - lastTimeInputButtonsPlayer2 > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs &&*/
							((lastButtonPlayer2 & FK_Input_Buttons::Modifier_Button) != 0/* ||
							(lastButtonPlayer2 & FK_Input_Buttons::Kick_Button) != 0*/) && !isPlayer2Ready() &&
							isCharacterAvailable(player2Index)){
							//inputReceiver->reset();
							soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
							player2OutfitId += 1;
							player2OutfitId %= player2Character->getNumberOfOutfits();
							player2SelectionMS = 0;
							(player2CanSelect() ? lastInputTimePlayer2 : lastInputTimePlayer1) = now;
							//inputReceiver->reset();
						}
						// select character
						else if (forcePlayer2Selection || 
							(/*now - lastTimeInputButtonsPlayer2 > FK_SceneCharacterSelect_Base::NonDirectionInputThresholdMs &&*/
							(lastButtonPlayer2 & FK_Input_Buttons::Selection_Button) != 0)){
							//if (player2CharacterLoaded.load()){
								(player2CanSelect() ? lastInputTimePlayer2 : lastInputTimePlayer1) = now;
								if (isCharacterSelectable(player2Character, false)){
									if (player2Index >= characterPaths.size()){
										if (!randomCanBeSelected(lastButtonPlayer2)){
											f32 randomIndex = (float)rand() / RAND_MAX;
											u32 charaIndex = (u32)(floor(randomIndex * characterPaths.size()));
											while (!isCharacterAvailable(charaIndex)){
												randomIndex = (float)rand() / RAND_MAX;
												charaIndex = (u32)(floor(randomIndex * characterPaths.size()));
											}
											player2Index = charaIndex;
											forcePlayer2Selection = true;
										}
										else{
											if (isSimpleModeSelected(lastButtonPlayer2)){
												player2MovesetTag = player2Character->SimpleModeMoveFileTag;
											}
											else{
												player2MovesetTag = std::string();
											}
											selectPlayer2Character();
										}
									}
									else{
										if (isSimpleModeSelected(lastButtonPlayer2)){
											player2MovesetTag = player2Character->SimpleModeMoveFileTag;
										}
										else{
											player2MovesetTag = std::string();
										}
										selectPlayer2Character();
										if (!player1CanSelect()){
											return;
										}
									}
								}
								else{
									soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
									inputReceiver->reset();
									return;
								}
							//}
						}
					}
				}
			}
		}

		// update outfit IDs if player select the same character, giving priority to the player who was formerly there
		if (player1Index == player2Index/* && 
			!(player1ThreadProcessing.load() || player2ThreadProcessing.load())*/){
			//u32 maxOutfitsPlayer1 = player1Character->getNumberOfOutfits();
			//u32 maxOutfitsPlayer2 = player2Character->getNumberOfOutfits();
			u32 testOutfitPlayer1 = player1OutfitId;
			u32 testOutfitPlayer2 = player2OutfitId;
			if (oldIndexPlayer1 != player1Index && oldIndexPlayer2 == player2Index){
				/*if (player2OutfitId == maxOutfitsPlayer2 - 1 && player1OutfitId == maxOutfitsPlayer1 - 1) {
					if (maxOutfitsPlayer2 > maxOutfitsPlayer1) {
						testOutfitPlayer2 = maxOutfitsPlayer1 - 1;
					}
					else if (maxOutfitsPlayer2 < maxOutfitsPlayer1) {
						testOutfitPlayer1 = maxOutfitsPlayer2 - 1;
					}
				}*/
				if (testOutfitPlayer1 == testOutfitPlayer2){
					if (player1Character != NULL && player2Character != NULL){
						player1OutfitId += 1;
						player1OutfitId %= player1Character->getNumberOfOutfits();
					}
				}
			}
			else if (oldIndexPlayer1 == player1Index && oldIndexPlayer2 != player2Index){
				if (testOutfitPlayer1 == testOutfitPlayer2){
					if (player1Character != NULL && player2Character != NULL){
						player2OutfitId += 1;
						player2OutfitId %= player2Character->getNumberOfOutfits();
					}
				}
			}
			// check if one player changed the outfit id and give prioirty to the former player
			else if (player1OldOutfitId != player1OutfitId && player2OldOutfitId == player2OutfitId){
				if (testOutfitPlayer1 == testOutfitPlayer2){
					player1OutfitId += 1;
					player1OutfitId %= player1Character->getNumberOfOutfits();
				}
			}
			else if (player1OldOutfitId == player1OutfitId && player2OldOutfitId != player2OutfitId){
				if (testOutfitPlayer1 == testOutfitPlayer2){
					player2OutfitId += 1;
					player2OutfitId %= player2Character->getNumberOfOutfits();
				}
			}
			// in the extremely random case of contemorary movement, give priority to player 1
			else{
				if (testOutfitPlayer1 == testOutfitPlayer2){
					if (player1Character != NULL && player2Character != NULL){
						player2OutfitId += 1;
						player2OutfitId %= player2Character->getNumberOfOutfits();
					}
					/*
					if (player1OutfitId == 0){
						player2OutfitId = 1;
					}
					else{
						player2OutfitId += 1;
						player2OutfitId %= player2Character->getNumberOfOutfits();
					}
					*/
				}
			}
		};

		u32 inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		if (!isEmbeddedSceneOptionsActive && (inputButtons & Help_Button)) {
			callEmbeddedSceneOptions();
		}
	}

	//! check if stage selection can be started
	bool FK_SceneCharacterSelect::canSelectStage(){
		u32 transitionToStageMs = 2000;
		return (isPlayer1Ready() && isPlayer2Ready() && transitionCounter > transitionToStageMs);
	}

	//! setup characters
	void FK_SceneCharacterSelect::setupCharacters(){
		player1Path = std::string();
		player2Path = std::string();
		updateCharacterSelection();
	}

	//! update character selection
	void FK_SceneCharacterSelect::updateCharacterSelection(bool force_update){
		// break if thread is loading
		if (force_update || oldIndexPlayer1 != player1Index || player1OldOutfitId != player1OutfitId){
			//if (!(player1ThreadProcessing.load())){
			//std::cout << "loading Player1" << std::endl;
			//bool thread1Flag = player1ThreadProcessing.load();
			//bool thread2Flag = player2ThreadProcessing.load();
			//bool threadFlag = thread1Flag || thread2Flag;
			bool threadFlag = false;
			if (player1Character != NULL){
				std::exception_ptr eptr;
				if (player1Character->getAnimatedMesh() != NULL){
					try{
						player1Character->getAnimatedMesh()->setVisible(false);
					}
					catch (...){
						eptr = std::current_exception();
						std::cout << "An exception occurred during mesh setup. Trying to recover..." << std::endl;
					}
					fk_addons::handle_eptr(eptr);
				}
			}
			if (player1SelectionMS < minimumTimeToLoadCharacterMS){
				return;
			}
			/*if (player1ThreadProcessing.load()){
				return;
			}
			if (player2ThreadProcessing.load()){
				return;
			}*/
			if (!threadFlag){
				core::vector3df player1Position = getIdleCharacterPosition(true);
				f32 playerAngle = sceneResources.player1CharacterYRotationAngle;
				bool changeOutfitOnly = false;
				if (oldIndexPlayer1 == player1Index && player1OldOutfitId != player1OutfitId){
					changeOutfitOnly = true;
				}
				oldIndexPlayer1 = player1Index;
				player1OldOutfitId = player1OutfitId;
				if (player1Index < characterPaths.size()){
					player1Path = characterPaths[player1Index];
				}
				else{
					player1Path = dummyCharacterDirectory;
				}
				if (player1Character != NULL) {
					removeCharacterNodeFromCache(player1Character);
					//if (player1Index != player2Index || player1OutfitId != player2OutfitId) {
					//	removeCharacterNodeFromCache(player1Character);
					//}
					delete player1Character;
					player1Character = NULL;
				}
				loadPlayer1Character(charactersDirectory + player1Path, player1Position,
					player1OutfitId, playerAngle, changeOutfitOnly);
				oldIndexPlayer1 = player1Index;
				player1OldOutfitId = player1OutfitId;
				//player1Character->setPosition(getIdleCharacterPosition(true));
			}
		}
		if (force_update || oldIndexPlayer2 != player2Index || player2OldOutfitId != player2OutfitId){
			//if (!(player2ThreadProcessing.load())){
			//bool thread1Flag = player1ThreadProcessing.load();
			//bool thread2Flag = player2ThreadProcessing.load();
			//bool threadFlag = thread1Flag || thread2Flag;
			bool threadFlag = false;
			//std::cout << "loading Player2" << std::endl;
			if (player2Character != NULL){
				std::exception_ptr eptr;
				if (player2Character->getAnimatedMesh() != NULL){
					try{
						player2Character->getAnimatedMesh()->setVisible(false);
					}
					catch (...){
						eptr = std::current_exception();
						std::cout << "An exception occurred during mesh setup. Trying to recover..." << std::endl;
					}
					fk_addons::handle_eptr(eptr);
				}
			}
			// again, break if threads are loading
			if (player2SelectionMS < minimumTimeToLoadCharacterMS){
				return;
			}
			/*if (player1ThreadProcessing.load()){
				return;
			}
			if (player2ThreadProcessing.load()){
				return;
			}*/
			if (!threadFlag){
				core::vector3df player2Position = getIdleCharacterPosition(false);
				f32 playerAngle = sceneResources.player2CharacterYRotationAngle;
				bool changeOutfitOnly = false;
				if (oldIndexPlayer2 == player2Index && player2OldOutfitId != player2OutfitId){
					changeOutfitOnly = true;
				}
				oldIndexPlayer2 = player2Index;
				player2OldOutfitId = player2OutfitId;
				if (player2Index < characterPaths.size()){
					player2Path = characterPaths[player2Index];
				}
				else{
					player2Path = dummyCharacterDirectory;
				}
				if (player2Character != NULL) {
					removeCharacterNodeFromCache(player2Character);
					//if (player1Index != player2Index || player1OutfitId != player2OutfitId) {
					//	removeCharacterNodeFromCache(player2Character);
					//}
					delete player2Character;
					player2Character = NULL;
				}
				//loadPlayer2CharacterThread = std::thread(
				//&FK_SceneCharacterSelect::loadPlayer2Character, this, charactersDirectory + player2Path,
				//player2Position, player2OutfitId, playerAngle);
				loadPlayer2Character(charactersDirectory + player2Path, player2Position,
					player2OutfitId, playerAngle, changeOutfitOnly);
				oldIndexPlayer2 = player2Index;
				player2OldOutfitId = player2OutfitId;
			}
		}
	}
	//! load character for player 1
	void FK_SceneCharacterSelect::loadPlayer1Character(std::string characterPath,
		core::vector3df position, int &outfit_id, f32 playerAngle, bool sameOutfit){
		//player1ThreadProcessing.store(true);
		bool lighting = false;
		if (!isCharacterAvailable(player1Index)){
			lighting = true;
		}
		// check same character / same costume issues
		if (player2Character != NULL && player1Index == player2Index) {
			u32 maxNumberOfOutfits = player2Character->getNumberOfOutfits();
			if (player1OutfitId >= maxNumberOfOutfits) {
				player1OutfitId = maxNumberOfOutfits - 1;
				if (player1OutfitId == player2OutfitId) {
					player1OutfitId = 0;
				}
			}
			else if (player2OutfitId == player1OutfitId) {
				player1OutfitId += 1;
				player1OutfitId %= player2Character->getNumberOfOutfits();
			}
		}
		outfit_id = player1OutfitId;
		player1OldOutfitId = player1OutfitId;
		player1Character = loadCharacter(characterPath,
			core::vector3df(-999,-999,-999), playerAngle, outfit_id, lighting, 
			!sameOutfit, true);
		if (player1Character != NULL){
			player1OutfitId = (u32)core::clamp<s32>((s32)player1OutfitId, 0, player1Character->getNumberOfOutfits() - 1);
			player1OldOutfitId = player1OutfitId;
			if (player1Character->getAnimatedMesh() != NULL) {
				player1Character->setPosition(position);
			}
		}
	}

	//! load character for player 2
	void FK_SceneCharacterSelect::loadPlayer2Character(std::string characterPath,
		core::vector3df position, int &outfit_id, f32 playerAngle, bool sameOutfit){
		//player2ThreadProcessing.store(true);
		bool lighting = false;
		if (!isCharacterAvailable(player2Index)){
			lighting = true;
		}
		if (player1Character != NULL && player1Index == player2Index) {
			u32 maxNumberOfOutfits = player1Character->getNumberOfOutfits();
			if (player2OutfitId >= maxNumberOfOutfits) {
				player2OutfitId = maxNumberOfOutfits - 1;
				if (player2OutfitId == player1OutfitId) {
					player2OutfitId = 0;
				}
			}
			else if (player2OutfitId == player1OutfitId) {
				player2OutfitId += 1;
				player2OutfitId %= player1Character->getNumberOfOutfits();
			}
		}
		outfit_id = player2OutfitId;
		player2Character = loadCharacter(characterPath,
			core::vector3df(999,999,999), playerAngle, outfit_id, lighting, !sameOutfit, false);
		if (player2Character != NULL){
			player2OutfitId = (u32)core::clamp<s32>((s32)player2OutfitId, 0, player2Character->getNumberOfOutfits() - 1);
			player2OldOutfitId = player2OutfitId;
			if (player2Character->getAnimatedMesh() != NULL) {
				player2Character->setPosition(position);
			}
		}
	}

	//! update stage selection
	void FK_SceneCharacterSelect::updateStageSelection(bool force_update){
		if (stageIndex != oldStageIndex || force_update){
			oldStageIndex = stageIndex;
			if (stageIndex == -1){
				currentStageName = L"Random";
			}else{
				currentStageName = stageNames[stageIndex];
			}
		}
	}

	//! update characters
	void FK_SceneCharacterSelect::updateCharacters(){
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			return;
		}
		//if (!player1ThreadProcessing.load() && player1CharacterLoaded.load()){
			/* update player 1 character */
			if (player1Character != NULL){
				bool movePerformed = false;
				player1Character->update(0, 0, movePerformed);
			}
		//}
		//if (!player2ThreadProcessing.load() && player2CharacterLoaded.load()){
			/* update player 2 character */
			if (player2Character != NULL){
				bool movePerformed = false;
				player2Character->update(0, 0, movePerformed);
			}
		//}
	}

	// select character
	void FK_SceneCharacterSelect::selectPlayer1Character(){
		(player1CanSelect() ? lastInputTimePlayer1 : lastInputTimePlayer2) = now;
		forcePlayer1Selection = false;
		//inputReceiver->reset();
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		soundManager->playSound(player1Character->getName(), 100.0 * gameOptions->getSFXVolume());
		if (!sceneResources.useImagePreviewsInsteadOfMesh) {
			player1Character->setStance(FK_Stance_Type::WinStance);
			player1Character->setBasicAnimation(FK_BasicPose_Type::WinAnimation, true);
			core::vector3df temp_position = getSelectedCharacterPosition(true);
			player1Character->setPosition(temp_position);
		}
		player1Ready = true;
	};
	void FK_SceneCharacterSelect::selectPlayer2Character(){
		(player2CanSelect() ? lastInputTimePlayer2 : lastInputTimePlayer1) = now;
		forcePlayer2Selection = false;
		//inputReceiver->reset();
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		soundManager->playSound(player2Character->getName(), 100.0 * gameOptions->getSFXVolume());
		if (!sceneResources.useImagePreviewsInsteadOfMesh) {
			player2Character->setStance(FK_Stance_Type::WinStance);
			player2Character->setBasicAnimation(FK_BasicPose_Type::WinAnimation, true);
			core::vector3df temp_position = getSelectedCharacterPosition(false);
			player2Character->setPosition(temp_position);
		}
		player2Ready = true;
	};

	// deselect character
	// select character
	void FK_SceneCharacterSelect::cancelPlayer1CharacterSelection(){
		(player1CanSelect() ? lastInputTimePlayer1 : lastInputTimePlayer2) = now;
		//inputReceiver->reset();
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		if (!sceneResources.useImagePreviewsInsteadOfMesh) {
			player1Character->setStance(FK_Stance_Type::GroundStance);
			core::vector3df temp_position = getIdleCharacterPosition(true);
			player1Character->setPosition(temp_position);
		}
		player1Ready = false;
	};
	void FK_SceneCharacterSelect::cancelPlayer2CharacterSelection(){
		(player2CanSelect() ? lastInputTimePlayer2 : lastInputTimePlayer1) = now;
		//inputReceiver->reset();
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		if (!sceneResources.useImagePreviewsInsteadOfMesh) {
			player2Character->setStance(FK_Stance_Type::GroundStance);
			core::vector3df temp_position = getIdleCharacterPosition(false);
			player2Character->setPosition(temp_position);
		}
		player2Ready = false;
	};


	// get if player 1 is ready
	bool FK_SceneCharacterSelect::isPlayer1Ready(){
		return player1Ready;
	}

	// get if player 2 is ready
	bool FK_SceneCharacterSelect::isPlayer2Ready(){
		return player2Ready;
	}

	// check if stage is selected
	bool FK_SceneCharacterSelect::isStageSelected(){
		return stageReady;
	}

	// select stage
	void FK_SceneCharacterSelect::selectStage(){
		lastInputTimePlayer1 = now;
		lastInputTimePlayer2 = now;
		inputReceiver->reset();
		if (stageIndex < 0){
			f32 randomIndex = (float)rand() / RAND_MAX;
			//std::cout << randomIndex << std::endl;
			stageIndex = (int)(floor(randomIndex * stagePaths.size()));
			currentStageName = stageNames[stageIndex];
		}
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		soundManager->playSound(stagePaths[stageIndex], 100.0 * gameOptions->getSFXVolume());
		transitionCounter = 0;
		stageReady = true;
	};

	// cancel stage selection
	void FK_SceneCharacterSelect::cancelStageSelection(){
		lastInputTimePlayer1 = now;
		lastInputTimePlayer2 = now;
		inputReceiver->reset();
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		stageReady = false;
		stageSelectionAnnounced = false;
	}

	//check if it is a match vs CPU or not (change control scheme for selection)
	bool FK_SceneCharacterSelect::isVsCPU(){
		return player1IsCPU || player2IsCPU;
	}

	bool FK_SceneCharacterSelect::isP1vsP2(){
		return !(player1IsCPU || player2IsCPU);
	}
	bool FK_SceneCharacterSelect::isP1vsCPU(){
		return (player2IsCPU && !player1IsCPU);
	}
	bool FK_SceneCharacterSelect::isCPUvsP2(){
		return (!player2IsCPU && player1IsCPU);
	}
	bool FK_SceneCharacterSelect::isCPUvsCPU(){
		return player1IsCPU && player2IsCPU;
	}
	bool FK_SceneCharacterSelect::player1CanSelect(){
		if (getFreeMatchType() == FK_Scene::FK_SceneFreeMatchType::FreeMatchTraining){
			return true;
		}
		if (getFreeMatchType() == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer2Selects) {
			return false;
		}
		return (isP1vsP2() || isP1vsCPU() || isCPUvsCPU());
	}
	bool FK_SceneCharacterSelect::player2CanSelect(){
		if (getFreeMatchType() == FK_Scene::FK_SceneFreeMatchType::FreeMatchTraining){
			return false;
		}
		if (getFreeMatchType() == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer2Selects) {
			return true;
		}
		return (isP1vsP2() || isCPUvsP2());
	}

	// set and get "allow ringout" flag
	void FK_SceneCharacterSelect::toggleRingoutFlag(){
		lastInputTimePlayer1 = now;
		lastInputTimePlayer2 = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		//inputReceiver->reset();
		allowRingoutFlag = !allowRingoutFlag;
	}
	bool FK_SceneCharacterSelect::getRingoutAllowanceFlag(){
		return allowRingoutFlag;
	}

	FK_Scene::FK_SceneFreeMatchType FK_SceneCharacterSelect::getFreeMatchType(){
		return freeMatchType;
	}

	// get all the information out of the scene
	std::string FK_SceneCharacterSelect::getPlayer1Path(){
		return player1Path;
	}
	std::string FK_SceneCharacterSelect::getPlayer2Path(){
		return player2Path;
	}
	int FK_SceneCharacterSelect::getPlayer1OutfitId(){
		return player1OutfitId;
	}
	int FK_SceneCharacterSelect::getPlayer2OutfitId(){
		return player2OutfitId;
	};
	FK_Character::FK_CharacterOutfit FK_SceneCharacterSelect::getPlayer1Outfit(){
		return player1Character->getOutfit();
	}
	FK_Character::FK_CharacterOutfit FK_SceneCharacterSelect::getPlayer2Outfit(){
		return player2Character->getOutfit();
	}
	int FK_SceneCharacterSelect::getPlayer1Index(){
		return player1Index;
	}
	int FK_SceneCharacterSelect::getPlayer2Index(){
		return player2Index;
	}
	std::string FK_SceneCharacterSelect::getPlayer1MovesetTag(){
		return player1MovesetTag;
	}
	std::string FK_SceneCharacterSelect::getPlayer2MovesetTag(){
		return player2MovesetTag;
	}

	// get stage information
	std::string FK_SceneCharacterSelect::getStagePath(){
		if (stageIndex >= 0 && stageIndex < stagePaths.size()) {
			return stagePaths[stageIndex];
		}
		else {
			return std::string();
		}
	}

	/* draw background*/
	void FK_SceneCharacterSelect::drawBackground(){
		if (canSelectStage() || stageReady){
			driver->draw2DRectangle(video::SColor(128, 0, 0, 0), core::rect<s32>(0, 0,
				(s32)screenSize.Width, (s32)screenSize.Height));
			std::string previewPictureFilename = mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath + "random_stage_preview.jpg";
			if (stageIndex >= 0){
				previewPictureFilename = stagesResourcePath + stagePaths[stageIndex] + "preview.jpg";
			}
			video::ITexture* tex = driver->getTexture(previewPictureFilename.c_str());
			core::dimension2d<u32> texSize = tex->getSize();
			// draw preview (fullscreen)
			s32 dest_width = screenSize.Width;
			s32 dest_height = screenSize.Height;
			s32 x = 0;
			s32 y = 0;
			core::rect<s32> destinationRect = core::rect<s32>(0, 0,
				dest_width,
				dest_height);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			driver->draw2DImage(tex, destinationRect, sourceRect);
		}
		else{
			FK_SceneCharacterSelect_Base::drawBackground();
		}
	};

	/* draw player names */
	void FK_SceneCharacterSelect::drawPlayerNames(){
		std::wstring player1Name = characterNames[player1Index];
		std::wstring player2Name = characterNames[player2Index];
		if (isCharacterAvailable(player1Index)){
			if (player1Character != NULL && !player1ThreadProcessing){
				if (isCharacterSelectable(player1Character, true)){
					player1Name = player1Character->getWName();
				}
				else{
					player1Name = player1Character->getWOriginalName();
				}
			}
		}
		else{
			player1Name = L"???";
		}
		if (isCharacterAvailable(player2Index)){
			if (player2Character != NULL && !player2ThreadProcessing){
				if (isCharacterSelectable(player2Character, false)){
					player2Name = player2Character->getWName();
				}
				else{
					player2Name = player2Character->getWOriginalName();
				}
			}
		}
		else{
			player2Name = L"???";
		}
		// convert names to wide strings format to write them (irrlicht accepts only wide strings here)
		std::wstring pl1Name = std::wstring(player1Name.begin(), player1Name.end());
		std::wstring pl2Name = std::wstring(player2Name.begin(), player2Name.end());
		/* get width and height of the string*/
		s32 sentenceWidth = font->getDimension(pl1Name.c_str()).Width;
		s32 sentenceHeight = font->getDimension(pl1Name.c_str()).Height;
		s32 sentenceOffsetX = sceneResources.characterNameOffset.X; // distance from screen center
		s32 sentenceOffsetY = sceneResources.characterNameOffset.Y; // distance from the top of the screen
		// draw player 1 name
		fk_addons::drawBorderedText(font, pl1Name.c_str(),
			core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), false, false);
		// draw player 2 name
		sentenceWidth = font->getDimension(pl2Name.c_str()).Width;
		sentenceHeight = font->getDimension(pl2Name.c_str()).Height;
		fk_addons::drawBorderedText(font, pl2Name.c_str(),
			core::rect<s32>(screenSize.Width / 2 + sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 + sentenceWidth + sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), false, false);
		// draw costume names
		drawCostumeNames(sentenceOffsetX);
		// draw costumeIndices
		std::wstring costumeString = L"Costume:";
		s32 costumeSentenceWidth = font->getDimension(costumeString.c_str()).Width;
		s32 costumeDotSentenceOffsetX = (sentenceOffsetX + 5) * scale_factorX + costumeSentenceWidth;
		drawCostumeIndex(costumeDotSentenceOffsetX);
		// draw kick icons
		drawButtonIcons();
		// draw locked costumes signs
		drawLockedCostumeSigns();
	}

	void FK_SceneCharacterSelect::drawCostumeNames(s32 sentenceOffsetX){
		// draw player 1 costume name
		s32 sentenceOffsetY = sceneResources.outfitSelectTextOffset.Y;
		std::wstring costumeString = L"Costume:";
		s32 sentenceWidth = font->getDimension(costumeString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(costumeString.c_str()).Height;
		fk_addons::drawBorderedText(font, costumeString.c_str(),
			core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY),
			video::SColor(255, 50, 150, 255), video::SColor(128, 0, 0, 0), false, false);
		sentenceOffsetY += 18;
		if (!player1ThreadProcessing){
			std::wstring soutfitname;
			if (isCharacterSelectable(player1Character, true)){
				soutfitname = player1Character->getOutfit().outfitWName;
				u32 inputButtons = player1input->getPressedButtons();
				if (player2CanSelect() && !player1CanSelect()){
					inputButtons = player2input->getPressedButtons();
				}
				if (player1Index == characterPaths.size() && !randomCanBeSelected(inputButtons) && !player1Ready){
					std::wstring names[3]{L"Default", L"Alt. 1", L"Alt. 2"};
					soutfitname = names[player1OutfitId];
				}
			}else{
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
		}
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
		if (!player2ThreadProcessing){
			std::wstring soutfitname;
			if (isCharacterSelectable(player2Character, false)){
				soutfitname = player2Character->getOutfit().outfitWName;
				u32 inputButtons = player2input->getPressedButtons();
				if (!player2CanSelect() && player1CanSelect()){
					inputButtons = player1input->getPressedButtons();
				}
				if (player2Index == characterPaths.size() && !randomCanBeSelected(inputButtons) && !player2Ready){
					std::wstring names[3]{L"Default", L"Alt. 1", L"Alt. 2"};
					soutfitname = names[player2OutfitId];
				}
			}
			else{
				soutfitname = L"???";
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
	}

	void FK_SceneCharacterSelect::drawCostumeIndex(s32 scaledSentenceOffsetX){
		// draw player 1 costume index
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
		video::SColor standardColor(255, 255, 255, 255);
		video::SColor unavailableColor(255, 255, 64, 64);
		if (player1Character != NULL){
			s32 outfitSize = player1Character->getNumberOfOutfits();
			f32 scaleFactor = 1.f;
			if (outfitSize > sceneResources.maximumNumberOfFullSizeOutfitDots){
				scale.X = baseScale.X * (f32)sceneResources.maximumNumberOfFullSizeOutfitDots / (f32)outfitSize;
				scaleFactor = (f32)sceneResources.maximumNumberOfFullSizeOutfitDots / (f32)outfitSize;
			}
			else{
				scale.X = baseScale.X;
			}
			for (int i = 0; i < outfitSize; ++i){
				s32 x = baseX + (i - outfitSize) * (s32)std::floor(baseOutfitDotSize * scaleFactor);
				s32 y = sentenceOffsetY * scale_factorY;
				core::position2d<s32> position(x, y);
				fk_addons::draw2DImage(driver, player1OutfitId == i ? currentCostumeDotTex : costumeDotTex, 
					sourceRect, position, core::vector2d<s32>(0, 0), 0, scale, true,
					(player1Index == player2Index && i == player2OutfitId) ? unavailableColor : standardColor,
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
		}
		if (player2Character != NULL){
			s32 outfitSize = player2Character->getNumberOfOutfits();
			f32 scaleFactor = 1.f;
			if (outfitSize > sceneResources.maximumNumberOfFullSizeOutfitDots) {
				scale.X = baseScale.X * (f32)sceneResources.maximumNumberOfFullSizeOutfitDots / (f32)outfitSize;
				scaleFactor = (f32)sceneResources.maximumNumberOfFullSizeOutfitDots / (f32)outfitSize;
			}
			else {
				scale.X = baseScale.X;
			}
			baseX = screenSize.Width / 2 + scaledSentenceOffsetX + outfitSize * sourceRect.getWidth() * scale.X;
			for (int i = 0; i < outfitSize; ++i){
				s32 x = baseX + (i - outfitSize) * (s32)std::floor(baseOutfitDotSize * scaleFactor);
				s32 y = sentenceOffsetY * scale_factorY;
				core::position2d<s32> position(x, y);
				fk_addons::draw2DImage(driver, player2OutfitId == i ? currentCostumeDotTex : costumeDotTex,
					sourceRect, position, core::vector2d<s32>(0, 0), 0, scale, true,
					(player1Index == player2Index && i == player1OutfitId) ? unavailableColor : standardColor,
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
		}
	};

	/* draw button icons */
	//void FK_SceneCharacterSelect::drawButtonIcons() {
	//	// draw kick icon near costume
	//	//std::string modifierButtonIconPath = "movelist\\joypad\\Xbutton.png";
	//	f32 magnificationAmplitude = 0.05;
	//	f32 magnificationOffset = 0.95;
	//	f32 magnificationPeriodMs = 300.0;
	//	f32 phase = 2 * core::PI * ((f32)now / magnificationPeriodMs);
	//	phase = fmod(phase, 2 * core::PI);
	//	f32 magnificationFactor = (magnificationOffset + magnificationAmplitude * cos(phase));
	//	if (isPlayer1Ready()) {
	//		magnificationFactor = 1.0f;
	//	}
	//	f32 baseWidth = 22 * scale_factorX;
	//	f32 baseHeight = 22 * scale_factorY;
	//	f32 width = baseWidth * magnificationFactor;
	//	f32 height = baseHeight * magnificationFactor;
	//	s32 x = (s32)((16.0f - (width - baseWidth) / 2) * scale_factorX);
	//	s32 y = (s32)((332.0f - (height - baseHeight) / 2) * scale_factorY);
	//	/*if (modifierButtonTexture == NULL) {
	//	modifierButtonTexture = device->getVideoDriver()->getTexture((mediaPath + modifierButtonIconPath).c_str());
	//	}*/
	//	if (player1CanSelect()) {
	//		if (player1UsesJoypad) {
	//			modifierButtonTexture = joypadButtonsTexturesMap[
	//				(FK_JoypadInput)buttonInputLayoutPlayer1[FK_Input_Buttons::Modifier_Button]];
	//		}
	//		else {
	//			modifierButtonTexture = keyboardButtonTexture;
	//		}
	//	}
	//	else {
	//		if (player2UsesJoypad) {
	//			modifierButtonTexture = joypadButtonsTexturesMap[
	//				(FK_JoypadInput)buttonInputLayoutPlayer2[FK_Input_Buttons::Modifier_Button]];
	//		}
	//		else {
	//			modifierButtonTexture = keyboardButtonTexture;
	//		}
	//	}
	//	core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
	//	fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
	//		core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
	//		core::vector2d<f32>((f32)width / iconSize.Width, (f32)height / iconSize.Height),
	//		true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
	//	if (!player2CanSelect()) {
	//		if (player1UsesJoypad) {
	//			modifierButtonTexture = joypadButtonsTexturesMap[
	//				(FK_JoypadInput)buttonInputLayoutPlayer1[FK_Input_Buttons::Modifier_Button]];
	//		}
	//		else {
	//			modifierButtonTexture = keyboardButtonTexture;
	//		}
	//	}
	//	else {
	//		if (player2UsesJoypad) {
	//			modifierButtonTexture = joypadButtonsTexturesMap[
	//				(FK_JoypadInput)buttonInputLayoutPlayer2[FK_Input_Buttons::Modifier_Button]];
	//		}
	//		else {
	//			modifierButtonTexture = keyboardButtonTexture;
	//		}
	//	}
	//	magnificationFactor = (magnificationOffset + magnificationAmplitude * cos(phase));
	//	if (isPlayer2Ready() || (!player2CanSelect() && !isPlayer1Ready())) {
	//		magnificationFactor = 1.0f;
	//	}
	//	width = baseWidth * magnificationFactor;
	//	height = baseHeight * magnificationFactor;
	//	x = (s32)((16.0f - (width - baseWidth) / 2) * scale_factorX);
	//	y = (s32)((332.0f - (height - baseHeight) / 2) * scale_factorY);
	//	x = screenSize.Width - x - width;
	//	fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
	//		core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
	//		core::vector2d<f32>((f32)width / iconSize.Width, (f32)height / iconSize.Height),
	//		true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
	//}

	/* draw button icons */
	void FK_SceneCharacterSelect::drawButtonIcons(){
		// draw kick icon near costume
		//std::string modifierButtonIconPath = "movelist\\joypad\\Xbutton.png";

		std::wstring costumeSelectionText = L"change costume";
		std::wstring characterSelectionText = L"select character";
		std::wstring cancelSelectionText = L"cancel selection";
		std::wstring backToMenuText = L"back to menu";

		// player 1
		core::dimension2du offset = capfont->getDimension(L"A");
		core::dimension2du textSize = hintfont->getDimension(L"A");
		u32 iconScreenSize = textSize.Height;
		s32 baseWidth = (s32)iconScreenSize;
		s32 baseHeight = (s32)iconScreenSize;
		s32 iconWidth = baseWidth;
		s32 x = (s32)std::floor(16.f * scale_factorX);
		s32 y = (s32)std::floor(sceneResources.characterNameOffset.Y * scale_factorY - offset.Height);
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		video::SColor normalTextColor = video::SColor(255, 196, 196, 196);
		video::SColor normalBorderColor = video::SColor(64, 0, 0, 0);
		video::SColor inactiveTextColor = video::SColor(255, 128, 128, 128);

		// player 1
		if (player1CanSelect()) {
			if (player1UsesJoypad) {
				modifierButtonTexture = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayoutPlayer1[FK_Input_Buttons::Modifier_Button]];
			}
			else {
				modifierButtonTexture = keyboardButtonTexture;
			}

			// change text color accordingly
			video::SColor tcolor = normalTextColor;
			if (!isCharacterSelectable(player1Character, true)) {
				tcolor = inactiveTextColor;
			}

			// draw different icon and text, depending on the keyboard/joypad notation
			// start by drawing the modifier/change costume button
			core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
			iconWidth = baseWidth;
			if (player1UsesJoypad) {
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				std::wstring itemToDraw = keyboardKeyLayoutPlayer1[FK_Input_Buttons::Modifier_Button];
				textSize = hintfont->getDimension(itemToDraw.c_str());
				u32 baseIconSize = baseWidth;
				while (iconWidth < textSize.Width) {
					iconWidth += baseIconSize;
				}
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				core::rect<s32> destRect = core::rect<s32>(
					x,
					y,
					x + iconWidth,
					y + baseIconSize);
				fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}

			std::wstring itemToDraw = L": " + costumeSelectionText;
			textSize = hintfont->getDimension(itemToDraw.c_str());
			core::rect<s32> destRect = core::rect<s32>(
				x + iconWidth,
				y,
				x + iconWidth + textSize.Width,
				y + textSize.Height);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				tcolor, normalBorderColor);

			// do the same for selection button
			// check what icon has to be used

			// offset y
			y -= textSize.Height;

			if (player1UsesJoypad) {
				modifierButtonTexture = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayoutPlayer1[FK_Input_Buttons::Selection_Button]];
			}
			else {
				modifierButtonTexture = keyboardButtonTexture;
			}

			iconSize = modifierButtonTexture->getSize();
			iconWidth = baseWidth;
			if (player1UsesJoypad) {
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				std::wstring itemToDraw = keyboardKeyLayoutPlayer1[FK_Input_Buttons::Selection_Button];
				textSize = hintfont->getDimension(itemToDraw.c_str());
				u32 baseIconSize = baseWidth;
				while (iconWidth < textSize.Width) {
					iconWidth += baseIconSize;
				}
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				core::rect<s32> destRect = core::rect<s32>(
					x,
					y,
					x + iconWidth,
					y + baseIconSize);
				fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}

			itemToDraw = L": " + characterSelectionText;
			textSize = hintfont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				x + iconWidth,
				y,
				x + iconWidth + textSize.Width,
				y + textSize.Height);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				tcolor, video::SColor(128, 0, 0, 0));

			// cancel selection button
			y -= textSize.Height;

			if (player1UsesJoypad) {
				modifierButtonTexture = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayoutPlayer1[FK_Input_Buttons::Cancel_Button]];
			}
			else {
				modifierButtonTexture = keyboardButtonTexture;
			}

			iconSize = modifierButtonTexture->getSize();
			iconWidth = baseWidth;
			if (player1UsesJoypad) {
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				std::wstring itemToDraw = keyboardKeyLayoutPlayer1[FK_Input_Buttons::Cancel_Button];
				textSize = hintfont->getDimension(itemToDraw.c_str());
				u32 baseIconSize = baseWidth;
				while (iconWidth < textSize.Width) {
					iconWidth += baseIconSize;
				}
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				core::rect<s32> destRect = core::rect<s32>(
					x,
					y,
					x + iconWidth,
					y + baseIconSize);
				fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}
			tcolor = normalTextColor;
			if (!isPlayer1Ready() && isPlayer2Ready()) {
				tcolor = inactiveTextColor;
			}
			if (player1Ready) {
				itemToDraw = L": " + cancelSelectionText;
			}
			else {
				itemToDraw = L": " + backToMenuText;
			}
			textSize = hintfont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				x + iconWidth,
				y,
				x + iconWidth + textSize.Width,
				y + textSize.Height);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				tcolor, video::SColor(128, 0, 0, 0));
		}

		// go to player 2
		y = (s32)std::floor(sceneResources.characterNameOffset.Y * scale_factorY - offset.Height);

		if (player2CanSelect()) {
			if (player2UsesJoypad) {
				modifierButtonTexture = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayoutPlayer2[FK_Input_Buttons::Modifier_Button]];
			}
			else {
				modifierButtonTexture = keyboardButtonTexture;
			}

			// change text color accordingly
			video::SColor tcolor = normalTextColor;
			if (!isCharacterSelectable(player2Character, false)) {
				tcolor = inactiveTextColor;
			}

			// draw different icon and text, depending on the keyboard/joypad notation
			// start by drawing the modifier/change costume button
			std::wstring itemToDraw = costumeSelectionText + L": ";
			textSize = hintfont->getDimension(itemToDraw.c_str());
			s32 additionalWidth = textSize.Width;
			core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
			s32 x1 = 0;
			iconWidth = baseWidth;
			if (player2UsesJoypad) {
				x1 = screenSize.Width - x - baseWidth;
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				std::wstring letterToDraw = keyboardKeyLayoutPlayer2[FK_Input_Buttons::Modifier_Button];
				textSize = hintfont->getDimension(letterToDraw.c_str());
				u32 baseIconSize = baseWidth;
				while (iconWidth < textSize.Width) {
					iconWidth += baseIconSize;
				}
				x1 = screenSize.Width - x - iconWidth;
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				core::rect<s32> destRect = core::rect<s32>(
					x1,
					y,
					x1 + iconWidth,
					y + baseIconSize);
				fk_addons::drawBorderedText(hintfont, letterToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}

			
			textSize = hintfont->getDimension(itemToDraw.c_str());
			core::rect<s32> destRect = core::rect<s32>(
				x1 - additionalWidth,
				y,
				x1 - additionalWidth + textSize.Width,
				y + textSize.Height);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				tcolor, normalBorderColor);

			// do the same for selection button
			// check what icon has to be used

			// offset y
			y -= textSize.Height;

			if (player2UsesJoypad) {
				modifierButtonTexture = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayoutPlayer2[FK_Input_Buttons::Selection_Button]];
			}
			else {
				modifierButtonTexture = keyboardButtonTexture;
			}

			itemToDraw = characterSelectionText + L": ";
			textSize = hintfont->getDimension(itemToDraw.c_str());
			additionalWidth = textSize.Width;
			iconSize = modifierButtonTexture->getSize();
			iconWidth = baseWidth;
			if (player2UsesJoypad) {
				x1 = screenSize.Width - x - baseWidth;
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				std::wstring letterToDraw = keyboardKeyLayoutPlayer2[FK_Input_Buttons::Selection_Button];
				textSize = hintfont->getDimension(letterToDraw.c_str());
				u32 baseIconSize = baseWidth;
				while (iconWidth < textSize.Width) {
					iconWidth += baseIconSize;
				}
				x1 = screenSize.Width - x - iconWidth;
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				core::rect<s32> destRect = core::rect<s32>(
					x1,
					y,
					x1 + iconWidth,
					y + baseIconSize);
				fk_addons::drawBorderedText(hintfont, letterToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}

			textSize = hintfont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				x1 - additionalWidth,
				y,
				x1 - additionalWidth + textSize.Width,
				y + textSize.Height);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				tcolor, normalBorderColor);

			// cancel selection
			// offset y
			y -= textSize.Height;

			if (player2UsesJoypad) {
				modifierButtonTexture = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayoutPlayer2[FK_Input_Buttons::Cancel_Button]];
			}
			else {
				modifierButtonTexture = keyboardButtonTexture;
			}

			if (player2Ready) {
				itemToDraw = cancelSelectionText + L": ";
			}
			else {
				itemToDraw = backToMenuText + L": ";
			}
			textSize = hintfont->getDimension(itemToDraw.c_str());
			additionalWidth = textSize.Width;
			iconSize = modifierButtonTexture->getSize();
			iconWidth = baseWidth;
			if (player2UsesJoypad) {
				x1 = screenSize.Width - x - baseWidth;
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				std::wstring letterToDraw = keyboardKeyLayoutPlayer2[FK_Input_Buttons::Cancel_Button];
				textSize = hintfont->getDimension(letterToDraw.c_str());
				u32 baseIconSize = baseWidth;
				while (iconWidth < textSize.Width) {
					iconWidth += baseIconSize;
				}
				x1 = screenSize.Width - x - iconWidth;
				fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1, y), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
					true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				core::rect<s32> destRect = core::rect<s32>(
					x1,
					y,
					x1 + iconWidth,
					y + baseIconSize);
				fk_addons::drawBorderedText(hintfont, letterToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}

			tcolor = normalTextColor;
			if (!isPlayer2Ready() && isPlayer1Ready()) {
				tcolor = inactiveTextColor;
			}

			textSize = hintfont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				x1 - additionalWidth,
				y,
				x1 - additionalWidth + textSize.Width,
				y + textSize.Height);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				tcolor, normalBorderColor);
		}
	}

	/* draw stage selection */
	void FK_SceneCharacterSelect::drawStageScreen(){
		driver->draw2DRectangle(video::SColor(128, 0, 0, 0), core::rect<s32>(0, 0,
			(s32)screenSize.Width, (s32)screenSize.Height));
		std::string previewPictureFilename = mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath + "random_stage_preview.jpg";
		if (stageIndex >= 0){
			previewPictureFilename = stagesResourcePath + stagePaths[stageIndex] + "preview.jpg";
			currentStageName = stageNames[stageIndex];
		}
		video::ITexture* tex = driver->getTexture(previewPictureFilename.c_str());
		core::dimension2d<u32> texSize = tex->getSize();
		// draw preview (reduce size by 50% in order to achieve better resolution when upscaled)
		s32 dest_width = texSize.Width / 2;
		s32 dest_height = texSize.Height / 2;
		s32 x = screenSize.Width / 2 - dest_width / 2 * scale_factorX;
		s32 y = screenSize.Height / 2 - dest_height / 2 * scale_factorY;
		s32 previewX = x;
		s32 previewY = y + dest_height * scale_factorY;
		core::rect<s32> destinationRect = core::rect<s32>(x, y,
			x + dest_width * scale_factorX,
			y + dest_height * scale_factorY);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
		driver->draw2DImage(tex, destinationRect, sourceRect);
		// draw preview frame
		tex = driver->getTexture((mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath + "previewFrame.png").c_str());
		texSize = tex->getSize();
		// draw preview
		dest_width = texSize.Width;
		dest_height = texSize.Height;
		x = screenSize.Width / 2 - texSize.Width / 2 * scale_factorX;
		y = screenSize.Height / 2 - texSize.Height / 2 * scale_factorY;
		destinationRect = core::rect<s32>(x, y,
			x + dest_width * scale_factorX,
			y + dest_height * scale_factorY);
		sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
		driver->draw2DImage(tex, destinationRect, sourceRect, 0, 0, true);
		// draw stage number
		std::wstring stageNumber = std::to_wstring(stageIndex + 2);
		std::wstring stageSize = std::to_wstring(stagePaths.size() + 1);
		std::wstring separator = L"/";
		u32 numberWidth = font->getDimension(stageNumber.c_str()).Width;
		u32 numberHeight = font->getDimension(stageNumber.c_str()).Height;
		u32 separationWidth = font->getDimension(L"00").Width;
		s32 fx, fy;
		fx = previewX;
		fy = previewY + 4 * scale_factorY;
		fk_addons::drawBorderedText(font, stageNumber.c_str(),
			core::rect<s32>(fx,
			fy,
			fx + numberWidth,
			fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		fx += separationWidth;
		numberWidth = font->getDimension(separator.c_str()).Width;
		fk_addons::drawBorderedText(font, separator.c_str(),
			core::rect<s32>(fx,
			fy,
			fx + numberWidth,
			fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		fx += separationWidth / 2;
		numberWidth = font->getDimension(stageSize.c_str()).Width;
		fk_addons::drawBorderedText(font, stageSize.c_str(),
			core::rect<s32>(fx,
			fy,
			fx + numberWidth,
			fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		// draw stage name
		std::wstring currentStageNameW(currentStageName.begin(), currentStageName.end());
		u32 sentenceWidth = font->getDimension(currentStageNameW.c_str()).Width;
		u32 sentenceHeight = font->getDimension(currentStageNameW.c_str()).Height;
		u32 sentenceOffsetY = screenSize.Height / 2 - tex->getSize().Height * scale_factorY / 2 - sentenceHeight;
		fk_addons::drawBorderedText(font, currentStageNameW.c_str(),
			core::rect<s32>(0,
			sentenceOffsetY,
			screenSize.Width,
			sentenceHeight + sentenceOffsetY),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		// draw ring shape
		if (databaseAccessor.ringoutAllowed() && sceneResources.drawStageBorderBox) {
			drawStageWallbox(stageIndex);
		}
	}

	void FK_SceneCharacterSelect::drawProportionalStageWallbox(s32 stageIndex){
		if (stageIndex < 0){
			return;
		}
		s32 stageWidth = std::ceil(stageWallboxes[stageIndex].getExtent().Z);
		s32 stageHeight = std::ceil(stageWallboxes[stageIndex].getExtent().X);
		s32 lineWidth = 4;
		s32 standardWidth = 80;
		s32 standardHeight = 80;
		s32 baseWidth = 500;
		s32 baseHeight = 500;
		s32 rectWidth = (s32)std::ceil((f32)stageWidth / baseWidth * standardWidth * scale_factorX);
		s32 rectHeight = (s32)std::ceil((f32)stageHeight / baseHeight * standardHeight * scale_factorY);
		s32 offsetY = 300;
		video::SColor borderColor(255, 255, 255, 255);
		video::SColor ringoutColor(255, 255, 100, 50);
		s32 x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		s32 y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		s32 x2 = x1 + lineWidth;
		s32 y2 = y1 + rectHeight;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width + rectWidth) / 2) - lineWidth;
		y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		x2 = x1 + lineWidth;
		y2 = y1 + rectHeight;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		x2 = x1 + rectWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		y1 = std::ceil(offsetY * scale_factorY + rectHeight / 2) - lineWidth;
		x2 = x1 + rectWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
	}

	void FK_SceneCharacterSelect::drawStageWallbox(s32 stageIndex){
		// draw stage box
		s32 lineWidth = (s32)std::ceil(4 * scale_factorX);
		s32 rectWidth = (s32)std::ceil(120 * scale_factorX);
		s32 rectHeight = (s32)(std::ceil(54 * scale_factorY));
		if (rectHeight % 2 != 0){
			rectHeight += 1;
		}
		if (rectWidth % 2 != 0){
			rectWidth += 1;
		}
		s32 offsetY = 310;
		f32 period = 700.0f;
		s32 ringoutAlpha = 128 + 127 * cos(2 * core::PI * ((f32)now / period));
		bool allowRingout = stageIndex >= 0 && allowRingoutFlag && stageRingoutFlags[stageIndex] > 0;
		//if (isStageSelected()){
		//	ringoutAlpha = 255;
		//}
		video::SColor backgroundColor(96, 128, 128, 128);
		video::SColor borderColor(255, 255, 255, 255);
		video::SColor ringoutColor(ringoutAlpha, 255, 25, 25);
		video::SColor ringoutFontColor(255, 255, 255, 255);
		video::SColor noRingoutFontColor(128, 255, 255, 255);
		video::SColor ringoutFontBorderColor(255, 0, 0, 0);
		video::SColor noRingoutFontBorderColor(64, 0, 0, 0);
		video::SColor ringoutDisabledColor(255, 96, 96, 96);
		if (!allowRingoutFlag){
			borderColor = ringoutDisabledColor;
			ringoutFontColor = ringoutDisabledColor;
			ringoutColor = ringoutDisabledColor;
		}
		// draw basic black box
		s32 x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		s32 y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		s32 x2 = x1 + rectWidth;
		s32 y2 = y1 + rectHeight;
		driver->draw2DRectangle(backgroundColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		x2 = x1 + lineWidth;
		y2 = y1 + rectHeight;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width + rectWidth) / 2 - lineWidth);
		y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		x2 = x1 + lineWidth;
		y2 = y1 + rectHeight;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
		x2 = x1 + rectWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = std::ceil((screenSize.Width - rectWidth) / 2);
		y1 = std::ceil(offsetY * scale_factorY + rectHeight / 2 - lineWidth);
		x2 = x1 + rectWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		// draw ringout text
		std::wstring ringoutString = L"RING OUT";
		if (!allowRingoutFlag){
			ringoutString = L"DISABLED";
		}
		s32 fwidth = font->getDimension(ringoutString.c_str()).Width;
		s32 fheight = font->getDimension(ringoutString.c_str()).Height;
		x1 = std::ceil((screenSize.Width - fwidth) / 2);
		y1 = std::ceil(offsetY * scale_factorY - fheight / 2);
		x2 = x1 + fwidth;
		y2 = y1 + fheight;
		fk_addons::drawBorderedText(font, ringoutString.c_str(),
			core::rect<s32>(x1,y1,x2,y2), 
			allowRingout ? ringoutFontColor : noRingoutFontColor,
			allowRingout ? ringoutFontBorderColor : noRingoutFontBorderColor);
		if (stageIndex >= 0){
			// draw ringout borders
			if (allowRingoutFlag && ((stageRingoutFlags[stageIndex] & FK_PlaneDirections::PlaneWest) != 0)){
				x1 = std::ceil((screenSize.Width - rectWidth) / 2);
				y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
				x2 = x1 + lineWidth;
				y2 = y1 + rectHeight;
				driver->draw2DRectangle(ringoutColor, core::rect<s32>(x1, y1, x2, y2));
			}
			if (allowRingoutFlag && ((stageRingoutFlags[stageIndex] & FK_PlaneDirections::PlaneEast) != 0)){
				x1 = std::ceil((screenSize.Width + rectWidth) / 2) - lineWidth;
				y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
				x2 = x1 + lineWidth;
				y2 = y1 + rectHeight;
				driver->draw2DRectangle(ringoutColor, core::rect<s32>(x1, y1, x2, y2));
			}
			if (allowRingoutFlag && ((stageRingoutFlags[stageIndex] & FK_PlaneDirections::PlaneNorth) != 0)){
				x1 = std::ceil((screenSize.Width - rectWidth) / 2);
				y1 = std::ceil(offsetY * scale_factorY - rectHeight / 2);
				x2 = x1 + rectWidth;
				y2 = y1 + lineWidth;
				driver->draw2DRectangle(ringoutColor, core::rect<s32>(x1, y1, x2, y2));
			}
			if (allowRingoutFlag && ((stageRingoutFlags[stageIndex] & FK_PlaneDirections::PlaneSouth) != 0)){
				x1 = std::ceil((screenSize.Width - rectWidth) / 2);
				y1 = std::ceil(offsetY * scale_factorY + rectHeight / 2) - lineWidth;
				x2 = x1 + rectWidth;
				y2 = y1 + lineWidth;
				driver->draw2DRectangle(ringoutColor, core::rect<s32>(x1, y1, x2, y2));
			}
		}
		//// draw ringout toggle input
		//if (sceneResources.allowRingoutToggle) {
		//	std::string modifierButtonIconPath = "movelist\\joypad\\Xbutton.png";
		//	std::wstring allowRingoutText = L": enable ring out";
		//	std::wstring disableRingoutText = L": disable ring out";
		//	u32 sentenceWidth = font->getDimension(allowRingoutText.c_str()).Width;
		//	u32 sentenceWidthTemp = font->getDimension(allowRingoutText.c_str()).Width;
		//	sentenceWidth = std::max(sentenceWidth, sentenceWidthTemp);
		//	sentenceWidth = (u32)std::ceil((f32)sentenceWidth *1.05f);
		//	u32 sentenceHeight = font->getDimension(allowRingoutText.c_str()).Height;
		//	if (allowRingoutFlag) {
		//		allowRingoutText = disableRingoutText;
		//	}
		//	f32 iconwidth = std::ceil(16 * scale_factorX);
		//	f32 iconheight = std::ceil(16 * scale_factorY);
		//	offsetY = (s32)std::ceil(1.f * sentenceHeight);
		//	s32 offsetX = (sentenceWidth + iconwidth) / 2;
		//	s32 x = (s32)std::ceil((screenSize.Width / 2 - offsetX));
		//	s32 y = (s32)std::ceil((screenSize.Height - offsetY));
		//	s32 iconOffsetY = 5 * scale_factorY;

		//	// find proper icon to draw

		//	if (modifierButtonTexture == NULL) {
		//		modifierButtonTexture = device->getVideoDriver()->getTexture((mediaPath + modifierButtonIconPath).c_str());
		//	}
		//	core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
		//	fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
		//		core::position2d<s32>(x, y + iconOffsetY), core::vector2d<s32>(0, 0), 0,
		//		core::vector2d<f32>((f32)iconwidth / iconSize.Width, (f32)iconheight / iconSize.Height),
		//		true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		//	x1 = x + iconwidth;
		//	x2 = x1 + sentenceWidth;
		//	y1 = y;
		//	y2 = y1 + font->getDimension(allowRingoutText.c_str()).Height;
		//	fk_addons::drawBorderedText(font, allowRingoutText.c_str(),
		//		core::rect<s32>(x1, y1, x2, y2), video::SColor(220, 255, 255, 255), video::SColor(128, 0, 0, 0));
		//}
		// draw ringout toggle input
		if (sceneResources.allowRingoutToggle) {
			std::string modifierButtonIconPath = "movelist\\joypad\\Xbutton.png";
			std::wstring allowRingoutText = L": enable ring out";
			std::wstring disableRingoutText = L": disable ring out";
			u32 sentenceWidth = font->getDimension(allowRingoutText.c_str()).Width;
			u32 sentenceWidthTemp = font->getDimension(allowRingoutText.c_str()).Width;
			sentenceWidth = std::max(sentenceWidth, sentenceWidthTemp);
			sentenceWidth = (u32)std::ceil((f32)sentenceWidth *1.05f);
			u32 sentenceHeight = font->getDimension(allowRingoutText.c_str()).Height;
			if (allowRingoutFlag) {
				allowRingoutText = disableRingoutText;
			}

			std::wstring separator = L"/";
			u32 separatorWidth = font->getDimension(separator.c_str()).Width;

			s32 iconwidth = (s32)std::ceil(16 * scale_factorX);
			s32 iconheight = (s32)std::ceil(16 * scale_factorY);
			offsetY = (s32)std::ceil(1.f * sentenceHeight);
			s32 offsetX = (sentenceWidth + iconwidth) / 2;
			if (player1CanSelect() && player2CanSelect()) {
				offsetX += (iconwidth + separatorWidth) / 2;
			}
			s32 x = (s32)std::ceil((screenSize.Width / 2 - offsetX));
			s32 y = (s32)std::ceil((screenSize.Height - offsetY));
			s32 iconOffsetY = (s32)std::floor(5.f * scale_factorY);
			
			// find proper icon to draw
			video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
			video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
			if (player1CanSelect()) {
				if (player1UsesJoypad) {
					modifierButtonTexture = joypadButtonsTexturesMap[
						(FK_JoypadInput)buttonInputLayoutPlayer1[FK_Input_Buttons::Modifier_Button]];
				}
				else {
					modifierButtonTexture = keyboardButtonTexture;
				}

				// draw different icon and text, depending on the keyboard/joypad notation
				core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
				u32 iconSizeWidth = (u32)iconwidth;
				if (player1UsesJoypad) {
					fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
						core::position2d<s32>(x, y + iconOffsetY), core::vector2d<s32>(0, 0), 0,
						core::vector2d<f32>((f32)iconwidth / iconSize.Width, (f32)iconheight / iconSize.Height),
						true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
				else {
					std::wstring itemToDraw = keyboardKeyLayoutPlayer1[FK_Input_Buttons::Modifier_Button];
					core::dimension2du textSize = hintfont->getDimension(itemToDraw.c_str());
					u32 baseIconSize = iconwidth;
					while (iconSizeWidth < textSize.Width) {
						iconSizeWidth += baseIconSize;
					}
					fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
						core::position2d<s32>(x, y + iconOffsetY), core::vector2d<s32>(0, 0), 0,
						core::vector2d<f32>((f32)iconSizeWidth / iconSize.Width, (f32)iconheight / iconSize.Height),
						true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
					core::rect<s32> destRect = core::rect<s32>(
						x,
						y + iconOffsetY,
						x + iconSizeWidth,
						y + iconOffsetY + baseIconSize);
					fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
						keyboardLetterColor, keyboardLetterBorder,
						true, true);
				}
				x += iconSizeWidth;
			}

			// player 2 (if needed)
			if (player2CanSelect()) {
				// add small separator
				if (player1CanSelect()) {
					x1 = x;
					x2 = x1 + separatorWidth;
					y1 = y;
					y2 = y1 + font->getDimension(separator.c_str()).Height;
					fk_addons::drawBorderedText(font, separator.c_str(),
						core::rect<s32>(x1, y1, x2, y2), video::SColor(220, 255, 255, 255), video::SColor(128, 0, 0, 0));
					x += separatorWidth;
				}
				if (player2UsesJoypad) {
					modifierButtonTexture = joypadButtonsTexturesMap[
						(FK_JoypadInput)buttonInputLayoutPlayer2[FK_Input_Buttons::Modifier_Button]];
				}
				else {
					modifierButtonTexture = keyboardButtonTexture;
				}

				// draw different icon and text, depending on the keyboard/joypad notation
				core::dimension2d<u32> iconSize = modifierButtonTexture->getSize();
				u32 iconSizeWidth = (u32)iconwidth;
				if (player2UsesJoypad) {
					fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
						core::position2d<s32>(x, y + iconOffsetY), core::vector2d<s32>(0, 0), 0,
						core::vector2d<f32>((f32)iconwidth / iconSize.Width, (f32)iconheight / iconSize.Height),
						true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
				else {
					std::wstring itemToDraw = keyboardKeyLayoutPlayer2[FK_Input_Buttons::Modifier_Button];
					core::dimension2du textSize = hintfont->getDimension(itemToDraw.c_str());
					u32 baseIconSize = iconwidth;
					while (iconSizeWidth < textSize.Width) {
						iconSizeWidth += baseIconSize;
					}
					fk_addons::draw2DImage(device->getVideoDriver(), modifierButtonTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
						core::position2d<s32>(x, y + iconOffsetY), core::vector2d<s32>(0, 0), 0,
						core::vector2d<f32>((f32)iconSizeWidth / iconSize.Width, (f32)iconheight / iconSize.Height),
						true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
					core::rect<s32> destRect = core::rect<s32>(
						x,
						y + iconOffsetY,
						x + iconSizeWidth,
						y + iconOffsetY + baseIconSize);
					fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
						keyboardLetterColor, keyboardLetterBorder,
						true, true);
				}
				x += iconSizeWidth;
			}

			x1 = x;
			x2 = x1 + sentenceWidth;
			y1 = y;
			y2 = y1 + font->getDimension(allowRingoutText.c_str()).Height;
			fk_addons::drawBorderedText(font, allowRingoutText.c_str(),
				core::rect<s32>(x1, y1, x2, y2), video::SColor(220, 255, 255, 255), video::SColor(128, 0, 0, 0));
		}
	}

	void FK_SceneCharacterSelect::drawLockedCostumeSigns(){
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
		if (!isCharacterSelectable(player1Character, true)){
			fk_addons::draw2DImage(driver, lockedCostumeSign,
				core::rect<s32>(0, 0, size.Width, size.Height),
				core::vector2d<s32>(x1, y1),
				core::vector2d<s32>(0, 0), 0, scale,
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		// player 2
		centerX1 = screenSize.Width  - centerX1;
		x1 = (s32)std::floor(centerX1 - width / 2);
		if (!isCharacterSelectable(player2Character, false)){
			fk_addons::draw2DImage(driver, lockedCostumeSign,
				core::rect<s32>(0, 0, size.Width, size.Height),
				core::vector2d<s32>(x1, y1),
				core::vector2d<s32>(0, 0), 0, scale,
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	};

	/* draw all*/
	void FK_SceneCharacterSelect::drawAll(){
		/* draw screen overlay */
		drawScreenOverlay();
		/*draw character screen*/
		drawCharacterScreen();
		//drawStageScreen();
		drawPlayerSelector(player1Index, isPlayer1Ready(), 
			sceneResources.characterSelectorOscillationPhases.first, sceneResources.characterSelectorColors.first);
		drawPlayerSelector(player2Index, isPlayer2Ready(), 
			sceneResources.characterSelectorOscillationPhases.second, sceneResources.characterSelectorColors.second);
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			drawCharacterPreview(true, player1Preview);
			drawCharacterPreview(false, player2Preview);
		}
		drawPlayerNames();
		// draw semitransparent rect if both players have been chosen
		if (canSelectStage() || stageReady){
			// draw stage selection
			drawStageScreen();
		}
		if (isEmbeddedSceneOptionsActive) {
			embeddedSceneOptions->drawAll();
		}
		//// sleep a bit if other threads need to be processed in the meanwhile
		//if (player1ThreadProcessing.load() || player2ThreadProcessing.load()){
		//	Sleep(20);
		//}
	};


	/* load character files */
	void FK_SceneCharacterSelect::loadCharacterList(){
		drawLoadingScreen(0);
		player1Ready = false;
		player2Ready = false;
		availableCharacters.clear();
		characterPaths.clear();
		characterNames.clear();
		for (std::map<std::string, FK_Character*>::iterator itr = characterInfos.begin(); itr != characterInfos.end(); itr++)
		{
			delete itr->second;
		}
		characterInfos.clear();
		dummyCharacterDirectory = "chara_dummy\\";
		std::string characterFileName = charactersDirectory + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName.c_str());
		u32 characterId = 0;
		std::string charaPath = std::string();
		while (!characterFile.eof() && characterFile >> charaPath){
			std::string key = charaPath;
			charaPath += "\\";
			int availabilityIdentifier;
			characterFile >> availabilityIdentifier;
			if (availabilityIdentifier == CharacterUnlockKeys::Character_AvailableFromStart ||
				availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_AvailableFromStart){
				availableCharacters.push_back(characterId);
				characterPaths.push_back(charaPath);
				characterId += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_UnlockableAndShown ||
				availabilityIdentifier == CharacterUnlockKeys::Character_UnlockableAndShown ||
				availabilityIdentifier == Character_Unlockable_NoArcadeOpponent){
				if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()){
					availableCharacters.push_back(characterId);
				}
				characterPaths.push_back(charaPath);
				characterId += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_Hidden_NoArcadeOpponent){
				if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()){
					availableCharacters.push_back(characterId);
					characterPaths.push_back(charaPath);
					characterId += 1;
				}
			}
		};
		availableCharacters.push_back(characterId);
		f32 totalProgress = (f32)characterPaths.size() + 1;
		f32 currentProgress = 0;
		for each(std::string chara_path in characterPaths){
			std::string temp;
			loadSingleCharacterFile(charactersDirectory + chara_path, temp);
			std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
			characterNames.push_back(wtemp);
			currentProgress += 1.f;
			drawLoadingScreen(100.f * currentProgress / totalProgress);
		}
		std::string temp;
		loadSingleCharacterFile(charactersDirectory + dummyCharacterDirectory, temp);
		std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
		characterNames.push_back(wtemp);
		drawLoadingScreen(100.f);
	};

	/* load character */
	FK_Character* FK_SceneCharacterSelect::loadCharacter(std::string characterPath,
		core::vector3df position, float rotationAngleY, int &outfit_id, bool lighting, 
		bool onlyValidOutfit, bool player1IsSelecting){
		u32 characterIndex = player1IsSelecting ? player1Index : player2Index;
		if (!isCharacterAvailable(characterIndex)){
			return NULL;
		}
		FK_Character* character = new FK_Character;
		std::string additionalPath = player1IsSelecting ? std::string() : "\\..\\";
		character->setCharacterSelectionVariablesFromCharacter(*characterInfos[characterPath]);
		outfit_id = core::clamp(outfit_id, 0, character->getNumberOfOutfits() - 1);
		character->setOutfitId(outfit_id);
		bool basicLighting = lighting;
		bool validOutfit = false;
		if (!basicLighting){
			while (validOutfit == false){
				if (!character->getOutfit().isAvailableFromBeginning){
					std::string outfitKey = character->getOriginalName() + "\\" + character->getOutfit().outfitName;
					std::replace(outfitKey.begin(), outfitKey.end(), ' ', '_');
					if (std::find(unlockedOutfits.begin(), unlockedOutfits.end(), outfitKey) == unlockedOutfits.end()){
						lighting = true;
						validOutfit = onlyValidOutfit ? false : true;
					}
					else{
						lighting = basicLighting;
						validOutfit = true;
					}
					if (validOutfit == false){
						outfit_id += 1;
						outfit_id %= character->getNumberOfOutfits();
						character->setOutfitId(outfit_id);
					}
					if (player1IsSelecting) {
						if (player2Character != NULL && player2Index == player1Index) {
							if (outfit_id == player2Character->getOutfitId()) {
								outfit_id += 1;
								outfit_id %= character->getNumberOfOutfits();
							}
						}
					}
					else {
						if (player1Character != NULL && player2Index == player1Index) {
							if (outfit_id == player1Character->getOutfitId()) {
								outfit_id += 1;
								outfit_id %= character->getNumberOfOutfits();
							}
						}
					}
				}
				else{
					lighting = basicLighting;
					validOutfit = true;
				}
			}
		}
		else{
			outfit_id = 0;
			validOutfit = true;
			character->setOutfitId(outfit_id);
			//character->loadBasicVariables(
			//	databaseAccessor,
			//	"character.txt", characterPath,
			//	commonResourcesPath,
			//	smgr, outfit_id);
		}
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			if (player1IsSelecting) {
				player1Preview.filename = characterPath + character->getOutfitPath() + "FightPreview.png";
				if (fk_addons::fileExists(player1Preview.filename.c_str())) {
					player1Preview.texture = driver->getTexture((characterPath +
						character->getOutfitPath() + "FightPreview.png").c_str());
				}else{
					player1Preview.texture = driver->getTexture((characterPath + "FightPreview.png").c_str());
				}
				player1Preview.selectable = !lighting;
			}else{
				player2Preview.filename = characterPath + character->getOutfitPath() + "FightPreview.png";
				if (fk_addons::fileExists(player2Preview.filename.c_str())) {
					player2Preview.texture = driver->getTexture((characterPath +
						character->getOutfitPath() + "FightPreview.png").c_str());
				}else{
					player2Preview.texture = driver->getTexture((characterPath + "FightPreview.png").c_str());
				}
				player2Preview.selectable = !lighting;
			}
		}
		else {
			character->loadMeshsAndEffects(position,
				core::vector3df(0, rotationAngleY, 0),
				std::string(),
				false, false, false);
			character->toggleLighting(lighting);
		}
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

	/* load stage files */
	void FK_SceneCharacterSelect::loadStageList(){
		stagePaths.clear();
		stageReady = false;
		std::string stageFileName = stagesResourcePath + fk_constants::FK_AvailableStagesFileName;
		std::ifstream stageFile(stageFileName.c_str());
		while (!stageFile.eof()){
			std::string stagePath;
			s32 unlockId;
			stageFile >> stagePath >> unlockId;
			if (unlockId == StageUnlockKeys::Stage_AvailableFromStart || unlockId == Stage_OnlyFreeMatch_AvailableFromStart ||
				std::find(unlockedStages.begin(), unlockedStages.end(), stagePath) != unlockedStages.end()){
				stagePath += "\\";
				stagePaths.push_back(stagePath);
			}
		};
		stageNames.clear();
		// load stage names & sound clips
		for each (std::string stagePath in stagePaths)
		{
			// load stage clips
			soundManager->addSoundFullPath(stagePath, stagesResourcePath + stagePath + fk_constants::FK_VoiceoverEffectsFileFolder + "name.wav");
			// load stage name
			std::ifstream stageFile((stagesResourcePath + stagePath + "config.txt").c_str());
			std::string stageFileKeyName = "#NAME";
			std::string wallboxKeyName = "#WALLBOX";
			std::string ringoutKeyName = "#ALLOW_RINGOUT";
			if (!stageFile){
				std::cout << "WARNING:: File " << stagePath + "config.txt" << "does not exist" << std::endl;
				continue;
			}
			bool ringoutFlagFound = false;
			bool wallboxFlagFound = false;
			while (!stageFile.eof()){
				std::string temp;
				stageFile >> temp;
				/* check if temp matches one of the keys*/
				/* name */
				if (temp == stageFileKeyName){
					stageFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					stageNames.push_back(fk_addons::convertNameToNonASCIIWstring(temp));
				}
				if (temp == ringoutKeyName){
					ringoutFlagFound = true;
					u32 north, east, south, west;
					u32 ringoutAllowedDirections;
					stageFile >> north >> east >> south >> west;
					ringoutAllowedDirections =
						east * (u32)FK_PlaneDirections::PlaneEast +
						west *(u32)FK_PlaneDirections::PlaneWest +
						north * (u32)FK_PlaneDirections::PlaneNorth +
						south * (u32)FK_PlaneDirections::PlaneSouth;
					stageRingoutFlags.push_back(ringoutAllowedDirections);
				}
				if (temp == wallboxKeyName){
					wallboxFlagFound = true;
					f32 xMin, yMin, zMin, xMax, yMax, zMax;
					stageFile >> xMin >> yMin >> zMin >> xMax >> yMax >> zMax;
					stageWallboxes.push_back(core::aabbox3d<f32>(xMin, yMin, zMin, xMax, yMax, zMax));
				}
			}
			if (!ringoutFlagFound){
				stageRingoutFlags.push_back(0);
			}
			if (!wallboxFlagFound){
				stageWallboxes.push_back(core::aabbox3d<f32>(0, 0, 0, 1, 1, 1));
			}
			stageFile.close();
		}
	};
}