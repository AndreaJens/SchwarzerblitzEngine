#include"FK_SceneGameTraining.h"

namespace fk_engine{

	// constructor (lazily based on the basic class constructor) 
	FK_SceneGameTraining::FK_SceneGameTraining() : FK_SceneGameFreeMatch(){

	};

	// setup
	void FK_SceneGameTraining::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		std::string new_player1path,
		std::string new_player2path,
		FK_Character::FK_CharacterOutfit new_outfitPlayer1,
		FK_Character::FK_CharacterOutfit new_outfitPlayer2,
		std::string new_arenaPath,
		FK_SceneGame::FK_AdditionalSceneGameOptions newAdditionalOptions){
		newAdditionalOptions.roundTimerInSeconds = 100;
		newAdditionalOptions.numberOfRounds = 9;
		FK_SceneGame::setup(newDevice, new_joypadInfo, newOptions, new_player1path, new_player2path,
			new_outfitPlayer1, new_outfitPlayer2, new_arenaPath, FK_Scene::FK_SceneType::SceneGameFreeMatch,
			newAdditionalOptions, false, false, 0, 0);
		setFreeMacthType(FK_SceneFreeMatchType::FreeMatchTraining);
	}

	// make a basic setup for player input
	void FK_SceneGameTraining::setupInputForPlayers(){
		// in case there are no joypads available, split keyboard
		s32 maxNumberOfRecordedButtonInInputBuffer = 10;
		s32 timeWindowBetweenInputBeforeClearing_ms = 200;
		s32 mergeTimeForInputAtSameFrame_ms = 20;
		s32 timeToRegisterHeldButton_ms = 125; //it was originally 40.
		s32 timeToRegisterNullInput_ms = 200;
		FK_SceneWithInput::setupInputForPlayers(timeWindowBetweenInputBeforeClearing_ms,
			mergeTimeForInputAtSameFrame_ms, timeToRegisterHeldButton_ms,
			timeToRegisterNullInput_ms, maxNumberOfRecordedButtonInInputBuffer);
		if (hudManager != NULL) {
			setupHUDManagerInputIcons();
		}
	}

	/* setup additional (i.e. class specific) variables after resources are loaded */
	void FK_SceneGameTraining::setupAdditionalParametersAfterLoading() {
		std::string movelistResourcePath = mediaPath + "movelist\\";
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		demoMoveList = new FK_MoveListPanel(device, getSoundManager(), gameOptions, screenSize,
			player2, movelistResourcePath, "windowskin.png", true);
	}

	// setup training variables
	void FK_SceneGameTraining::setupAdditionalVariables(){
		trainingCounterMS = 0;
		groundedDurationMS = 0;
		updateGroundedTimer = false;
		skipIntro = true;
		dummyMovementOptions = FK_TrainingOptionsDummy::StandingDummy;
		dummyGuardOptions = FK_GuardOptionsDummy::NoGuard;
		dummyRecoveryOptions = FK_RecoveryOptionsDummy::NoRecovery;
		dummyUkemiOptions = FK_UkemiOptionsDummy::NoUkemi;
		dummyAfterGuardOptions = FK_AfterGuardOptionsDummy::NoAfterGuardAction;
		dummyWasHitOnceInCurrentCombo = false;
		hasBlockedAnAttack = false;
		dummyHitOnceMsCounter = 0;
		isRecordingDummyActions = false;
		isControllingPlayer2ForRecording = false;
		isReplayingDummyActions = false;
		recordBuffer.clear();
		recordTimerMs = 0;
		dummyRecoveryOptionsToCheck = dummyRecoveryOptions;
		guardDurationMS = 400;
		guardDurationCounterMS = 0;
		currentPressedButtons = 0;
		updateGuardDuration = false;
		trainingMenu = new FK_TrainingOptionsMenu;
		trainingMenu->setup(device);
		showMoveListForDemoMode = false;
	}

	/* setup hud manager*/
	void FK_SceneGameTraining::setupHUD() {
		FK_SceneGame::setupHUD();
		setupHUDManagerInputIcons();
	}

	void FK_SceneGameTraining::setupHUDManagerInputIcons() {
		// setup HUD input maps for training mode
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		bool player1UsesJoypad = false;
		if (joystickInfo.size() > 0) {
			player1UsesJoypad = true;
			mapId = 1;
			currentJoypadName = std::string(joystickInfo[0].Name.c_str());
		}
		else {
			player1UsesJoypad = false;
			mapId = 0;
		}

		std::map<FK_Input_Buttons, u32> buttonInputLayoutPlayer1;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayoutPlayer1;
		setIconsBasedOnCurrentInputMap(buttonInputLayoutPlayer1, keyboardKeyLayoutPlayer1,
			mapId, player1UsesJoypad, currentJoypadName
		);

		hudManager->setInputButtonTextures(buttonInputLayoutPlayer1, keyboardKeyLayoutPlayer1, player1UsesJoypad);
	}

	void FK_SceneGameTraining::initialize(){
		FK_SceneGame::initialize();
		if (gameOptions->getBorderlessWindowMode()){
			trainingMenu->setViewport(borderlessWindowRenderTarget->getSize());
		}
	}

	///* load stage files */
	//void FK_SceneGameTraining::loadStageList() {
	//	bool stageReady = false;
	//	std::string stageFileName = stagesPath + fk_constants::FK_AvailableStagesFileName;
	//	std::ifstream stageFile(stageFileName.c_str());
	//	while (!stageFile.eof()) {
	//		std::string stagePath;
	//		s32 unlockId;
	//		stageFile >> stagePath >> unlockId;
	//		if (unlockId == StageUnlockKeys::Stage_AvailableFromStart || unlockId == Stage_OnlyFreeMatch_AvailableFromStart ||
	//			std::find(unlockedStages.begin(), unlockedStages.end(), stagePath) != unlockedStages.end()) {
	//			stagePath += "\\";
	//			stagePaths.push_back(stagePath);
	//		}
	//	};
	//	stageNames.clear();
	//	// load stage names & sound clips
	//	for each (std::string stagePath in stagePaths)
	//	{
	//		// load stage name
	//		std::ifstream stageFile((stagesPath + stagePath + "config.txt").c_str());
	//		std::string stageFileKeyName = "#NAME";
	//		if (!stageFile) {
	//			std::cout << "WARNING:: File " << stagePath + "config.txt" << "does not exist" << std::endl;
	//			continue;
	//		}
	//		bool ringoutFlagFound = false;
	//		bool wallboxFlagFound = false;
	//		while (!stageFile.eof()) {
	//			std::string temp;
	//			stageFile >> temp;
	//			/* check if temp matches one of the keys*/
	//			/* name */
	//			if (temp == stageFileKeyName) {
	//				stageFile >> temp;
	//				std::replace(temp.begin(), temp.end(), '_', ' ');
	//				stageNames.push_back(fk_addons::convertNameToNonASCIIWstring(temp));
	//			}
	//		}
	//		stageFile.close();
	//	}
	//};

	///* load character files */
	//void FK_SceneGameTraining::loadCharacterList() {
	//	bool player1Ready = false;
	//	bool player2Ready = false;
	//	std::string characterFileName = charactersPath + fk_constants::FK_CharacterRosterFileName;
	//	std::ifstream characterFile(characterFileName.c_str());
	//	u32 characterId = 0;
	//	std::string charaPath = std::string();
	//	while (!characterFile.eof() && characterFile >> charaPath) {
	//		std::string key = charaPath;
	//		charaPath += "\\";
	//		int availabilityIdentifier;
	//		characterFile >> availabilityIdentifier;
	//		if (availabilityIdentifier == CharacterUnlockKeys::Character_AvailableFromStart ||
	//			availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_AvailableFromStart) {
	//			characterPaths.push_back(charaPath);
	//			characterId += 1;
	//		}
	//		else if (availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_UnlockableAndShown ||
	//			availabilityIdentifier == CharacterUnlockKeys::Character_UnlockableAndShown ||
	//			availabilityIdentifier == Character_Unlockable_NoArcadeOpponent) {
	//			if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()) {
	//				characterPaths.push_back(charaPath);
	//			}
	//		}
	//		else if (availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_Hidden ||
	//			availabilityIdentifier == CharacterUnlockKeys::Character_Hidden) {
	//			if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()) {
	//				characterPaths.push_back(charaPath);
	//			}
	//		}
	//	};
	//	for each(std::string chara_path in characterPaths) {
	//		std::string temp;
	//		loadSingleCharacterFile(charactersPath + chara_path, temp);
	//		std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
	//		characterNames.push_back(wtemp);
	//	}
	//};

	///* load single character file */
	//void FK_SceneGameTraining::loadSingleCharacterFile(std::string characterPath, std::string &playerName) {
	//	std::ifstream characterFile((characterPath + "character.txt").c_str());
	//	std::string characterFileKeyName = "#NAME";
	//	while (!characterFile.eof()) {
	//		std::string temp;
	//		characterFile >> temp;
	//		/* check if temp matches one of the keys*/
	//		/* name */
	//		if (strcmp(temp.c_str(), characterFileKeyName.c_str()) == 0) {
	//			characterFile >> temp;
	//			std::replace(temp.begin(), temp.end(), '_', ' ');
	//			playerName = temp;
	//			break;
	//		}
	//	}
	//	characterFile.close();
	//};
	
	// avoid K.O. for characters
	void FK_SceneGameTraining::applyDamageToPlayer(FK_Character* targetPlayer, f32 damage, bool affectsObjects){
		FK_SceneGame::applyDamageToPlayer(targetPlayer, damage, affectsObjects);
		if (targetPlayer->isKO()){
			targetPlayer->setLife(0.1f);
		}
	}


	// update input for Record & Replay mode
	void FK_SceneGameTraining::updateInputForRecordAndReplayMode(const u32& inputButtons, u32 delta_t_ms) {
		if (!isControllingPlayer2ForRecording && !isReplayingDummyActions) {
			if ((inputButtons & FK_Input_Buttons::Selection_Button) != 0) {
				isControllingPlayer2ForRecording = true;
				getSoundManager()->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
			}
			else if (!isReplayingDummyActions && 
				recordBufferSize > 0 && 
				(inputButtons & FK_Input_Buttons::Modifier_Button) != 0) {
				isReplayingDummyActions = true;
				isControllingPlayer2ForRecording = false;
				recordTimerMs = 0;
				recordReplayInputIndexMs = 0;
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
				getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			}
		}
		else if (!isRecordingDummyActions && !isReplayingDummyActions) {
			if ((inputButtons & FK_Input_Buttons::Cancel_Button) != 0) {
				isControllingPlayer2ForRecording = false;
				getSoundManager()->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
			}
			else if ((inputButtons & FK_Input_Buttons::Selection_Button) != 0) {
				isRecordingDummyActions = true;
				getSoundManager()->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
				recordTimerMs = 0;
				recordBuffer.clear();
				recordBufferSize = 0;
				recordedInputBuffer.clear();
			}
			else if (!isReplayingDummyActions &&
				recordBufferSize > 0 &&
				(inputButtons & FK_Input_Buttons::Modifier_Button) != 0) {
				isReplayingDummyActions = true;
				recordTimerMs = 0;
				recordReplayInputIndexMs = 0;
				inputReceiver->reset();
				player1input->clearBuffer();
				isControllingPlayer2ForRecording = false;
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
				getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			}
		}
		else if (isRecordingDummyActions) {
			if ((inputButtons & FK_Input_Buttons::Cancel_Button) != 0) {
				getSoundManager()->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				isRecordingDummyActions = false;
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
				recordBuffer.clear();
				recordBufferSize = 0;
				recordedInputBuffer.clear();
			}
			else if ((inputButtons & FK_Input_Buttons::Selection_Button) != 0) {
				isRecordingDummyActions = false;
				getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
				recordReplayInputIndexMs = 0;
			}
		}
		else if (isReplayingDummyActions) {
			if ((inputButtons & (FK_Input_Buttons::Cancel_Button | 
				FK_Input_Buttons::Modifier_Button |
				FK_Input_Buttons::Selection_Button)) != 0) {
				getSoundManager()->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				isRecordingDummyActions = false;
				isReplayingDummyActions = false;
				recordTimerMs = 0;
				recordReplayInputIndexMs = 0;
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
			}
		}
	}

	void FK_SceneGameTraining::updateRecordAndReplayFlow(u32 delta_t_ms)
	{
		// manage record timer
		if (isRecordingDummyActions) {
			FK_TrainingInputRecordUnit recordUnit;
			recordUnit.buffer = player1input->getBuffer();
			recordUnit.pressedButtons = player1input->getPressedButtons();
			recordUnit.time = recordTimerMs;
			recordBuffer.push_back(recordUnit);
			recordBufferSize += 1;
			if (recordTimerMs >= MaxRecordTimeMs) {
				recordTimerMs = 0;
				isRecordingDummyActions = false;
				getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
				player1input->clearBuffer();
				inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
				recordReplayInputIndexMs = 0;
			}
			else {
				recordTimerMs += (s32)delta_t_ms;
			}
		}

		// manage record timer
		if (isReplayingDummyActions) {
			recordTimerMs += (s32)delta_t_ms;
			if (recordReplayInputIndexMs >= (s32)recordBufferSize) {
				recordTimerMs = 0;
				recordReplayInputIndexMs = 0;
			}
			while (recordTimerMs > recordBuffer[recordReplayInputIndexMs].time) {
				recordReplayInputIndexMs += 1;
				if (recordReplayInputIndexMs >= (s32)recordBufferSize) {
					break;
				}
			}
		}
	}

	// load system icons
	void FK_SceneGameTraining::loadSystemIcons() {
		// keyboard button texture
		std::string resourcePath = mediaPath + "movelist\\";
		keyboardButtonTexture = device->getVideoDriver()->getTexture((resourcePath + "keyboard\\empty2.png").c_str());
		// joypad buttons textures
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_A_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Abutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_X_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Xbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Y_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Ybutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_B_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Bbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L1_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L1button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L2_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L2button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L3_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L3button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R1_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R1button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R2_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R2button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R3_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R3button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Select_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\backbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Start_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\startbutton.png").c_str());
	}

	// update demo move list
	void FK_SceneGameTraining::updateDemoMoveList()
	{
		u32 inputButtons = player1input->getPressedButtons();
		demoMoveList->update(inputButtons, false);
		if (demoMoveList->hasSelectedRootMove()) {
			afterGuardMove = demoMoveList->getSelectedMove();
			Sleep(200);
		}
		if (demoMoveList->hasToBeClosed()) {
			demoMoveList->setActive(false);
			demoMoveList->setVisible(false);
			Sleep(200);
		}
	}

	// update scene training logic
	void FK_SceneGameTraining::updateAdditionalSceneLogic(u32 delta_t_ms) {
		// update reset counter
		if (blackScreenResetCounter > 0) {
			blackScreenResetCounter -= 10;
		}
		// clear record flags
		if (dummyMovementOptions != FK_TrainingOptionsDummy::RecordDummy) {
			isRecordingDummyActions = false;
			isControllingPlayer2ForRecording = false;
			isReplayingDummyActions = false;
		}
		if (!isPaused() && canPauseGame()) {
			updateRecordAndReplayFlow(delta_t_ms);
		}

		// update flags for "after 1st hit guard"
		if (dummyHitOnceMsCounter > 0) {
			dummyHitOnceMsCounter -= delta_t_ms;
			if (dummyHitOnceMsCounter <= 0 && player1->getCurrentMove() == NULL) {
				dummyWasHitOnceInCurrentCombo = false;
				dummyHitOnceMsCounter = 0;
			}
			else if(player1->getCurrentMove() != NULL){
				dummyHitOnceMsCounter = 1;
			}
		}

		if (inputCooldownForReset > 0) {
			inputCooldownForReset -= (s32)delta_t_ms;
		}
		else {
			u32 inputButtons = player1input->getPressedButtons();
			if (!isPaused() && canPauseGame() && (inputButtons & FK_Input_Buttons::Help_Button) != 0) {
				if (!isRecordingDummyActions) {
					bool player1cornered = false;
					bool player2cornered = false;
					bool resetPerformed = false;
					if ((inputButtons & FK_Input_Buttons::Any_Direction) == FK_Input_Buttons::Right_Direction){
						stage->getRightPlayerAtWallPosition(player1startingPosition, player2startingPosition);
						player2cornered = true;
						resetPerformed = true;
					}
					else if ((inputButtons & FK_Input_Buttons::Any_Direction) == FK_Input_Buttons::Left_Direction){
						stage->getLeftPlayerAtWallPosition(player1startingPosition, player2startingPosition);
						player1cornered = true;
						resetPerformed = true;
					}
					else if ((inputButtons & FK_Input_Buttons::Any_Direction) == FK_Input_Buttons::Down_Direction){
						stage->getCenterStartingPosition(player1startingPosition, player2startingPosition);
						resetPerformed = true;
					}
					else if (dummyMovementOptions != FK_TrainingOptionsDummy::RecordDummy) {
						resetPerformed = true;
					}
					if (resetPerformed) {
						inputReceiver->reset();
						blackScreenResetCounter = 255;
						resetRound();
						player1->setCorneredFlag(player1cornered);
						player2->setCorneredFlag(player2cornered);
						player1->setRingWallCollisionFlag(player1cornered);
						player2->setRingWallCollisionFlag(player2cornered);
						inputCooldownForReset = FK_SceneGameTraining::InputCooldownForResetMs;
					}
					else if (dummyMovementOptions == FK_TrainingOptionsDummy::RecordDummy) {
						updateInputForRecordAndReplayMode(inputButtons, delta_t_ms);
					}
				}
				else {
					updateInputForRecordAndReplayMode(inputButtons, delta_t_ms);
				}
			}
		}
		// add some debug options
		if (inputReceiver->IsKeyDown(EKEY_CODE::KEY_F4)) {
			player1->toggleHitboxVisibility(!player1->checkHitboxVisibility());
			inputReceiver->reset();
		}else if (inputReceiver->IsKeyDown(EKEY_CODE::KEY_F5)) {
			player2->toggleHitboxVisibility(!player2->checkHitboxVisibility());
			inputReceiver->reset();
		}
		else if (inputReceiver->IsKeyDown(EKEY_CODE::KEY_F6)) {
			player1->toggleHurtboxVisibility(!player1->checkHurtboxVisibility());
			inputReceiver->reset();
		}
		else if (inputReceiver->IsKeyDown(EKEY_CODE::KEY_F7)) {
			player2->toggleHurtboxVisibility(!player2->checkHurtboxVisibility());
			inputReceiver->reset();
		}

		if (!(isProcessingRingout())){
			if (player1->getCurrentMove() == NULL && !player2->isHitStun()){
				trainingCounterMS += delta_t_ms;
			}
			else{
				trainingCounterMS = 0;
			}
			if (trainingCounterMS >= damageLastingTimeTrainingMS){
				player1->setLifeToMax();
				player2->setLifeToMax();
				player1->addTriggerCounters(databaseAccessor.getMaxTriggers());
				player2->addTriggerCounters(databaseAccessor.getMaxTriggers());
				player1NumberOfBulletCounters = player1->getTriggerCounters();
				player2NumberOfBulletCounters = player2->getTriggerCounters();
				trainingCounterMS = 0;
			}
			if (player2->isGrounded()){
				if (!updateGroundedTimer){
					groundedCounterMS = 0;
					updateGroundedTimer = true;
				}
				else{
					groundedCounterMS += delta_t_ms;
					if (groundedCounterMS > groundedDurationMS && 
						dummyRecoveryOptions == FK_RecoveryOptionsDummy::TutorialRecovery){
						cameraManager->alignCharacters(player2);
						player2->recoverFromGrounding();
						groundedCounterMS = 0;
						updateGroundedTimer = false;
					}
				}
			}
			else{
				groundedCounterMS = 0;
				updateGroundedTimer = true;
			}
			if (player2->isGuarding()){
				guardDurationCounterMS += delta_t_ms;
			}
			else{
				updateGuardDuration = false;
			}
		}
	}

	void FK_SceneGameTraining::drawHUDThrowName(FK_Character* performer) {
		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());
		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
		driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());
		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
		// quick check for seeing which player performed the throw
		if (player1->isOnLeftSide() == performer->isOnLeftSide()) {
			hudManager->drawMoveName(performer);
		}
		// Restore projection, world, and view matrices
		driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
		driver->setTransform(irr::video::ETS_WORLD, oldWorldMat);
	}

	// add move hitflag does nothing outside training mode
	void FK_SceneGameTraining::addMoveHitFlag(ISceneNode * hitboxNode, 
		FK_Hitbox * hitboxData, s32 frameAdvantage, s32 cancelAdvantage, bool showframeAdvantage)
	{
		hudManager->addMoveHitFlag(
			hitboxNode, hitboxData, showframeAdvantage, frameAdvantage, cancelAdvantage);
	}

	// calculate frame advatnage (nothing outside training mode)
	bool FK_SceneGameTraining::calculateFrameAdvantage(
		s32& frameAdvantage,
		s32& cancelAdvantage,
		FK_Hitbox * hitbox, 
		f32 hitstunMultiplier,
		FK_Character * attacker, 
		FK_Character * defender,
		bool guardBreak,
		bool counterAttack, 
		bool defenderHasArmor)
	{
		s32 delayAfterMove = databaseAccessor.getCharacterMoveCooldownDurationMs();
		if (!attacker->getCurrentMove()->getDelayAfterMoveFlag()) {
			delayAfterMove = 0;
		}
		else if (attacker->getCurrentMove()->getDelayAfterMoveTime() > 0) {
			delayAfterMove = attacker->getCurrentMove()->getDelayAfterMoveTime();
		}
		f32 remainingFrameTime = (f32)attacker->getCurrentMove()->getEndingFrame() - 
			attacker->getAnimatedMesh()->getFrameNr();
		f32 frameDuration = 1000.f / databaseAccessor.getAnimationFramerateFPS();
		f32 gameFrameRate = 1000.f / 60;
		f32 remainingDurationMs = delayAfterMove + frameDuration * remainingFrameTime;
		if (defender->isGuarding()) {
			remainingDurationMs -= databaseAccessor.getGuardHitstunDurationMs();
		}
		else {
			FK_Reaction_Type reaction = hitbox->getReaction();
			if (guardBreak &&
				(hitbox->getReaction() == FK_Reaction_Type::StrongFlight ||
				hitbox->getReaction() == FK_Reaction_Type::ReverseStrongFlight ||
				hitbox->getReaction() == FK_Reaction_Type::WeakFlight ||
				hitbox->getReaction() == FK_Reaction_Type::StandingFlight)) {
				reaction = FK_Reaction_Type::StrongMedium;
			}
			else {
				if (defender->isAirborne() ||
					(defender->isHitStun() && defender->getVelocityPerSecond().Z != 0) ||
					hitbox->getReaction() == FK_Reaction_Type::StrongFlight ||
					hitbox->getReaction() == FK_Reaction_Type::ReverseStrongFlight ||
					hitbox->getReaction() == FK_Reaction_Type::WeakFlight ||
					hitbox->getReaction() == FK_Reaction_Type::StandingFlight ||
					hitbox->getReaction() == FK_Reaction_Type::SmackdownLanding
					) {
					return false;
				}
			}
			f32 hitstunMs = 0;
			if (isStrongReaction(reaction)) {
				hitstunMs = (f32)databaseAccessor.getStrongHitstunDurationMs();
			}
			else {
				hitstunMs = (f32)databaseAccessor.getWeakHitstunDurationMs();
			
			}
			hitstunMs *= hitstunMultiplier;
			//if (attacker->isTriggerModeActive()) {
			//	hitstunMs *= databaseAccessor.getTriggerComboHitstunMultiplier();
			//}
			if (counterAttack) {
				//hitstunMs *= databaseAccessor.getCounterAttackHitstunMultiplier();
			}
			if (defenderHasArmor) {
				hitstunMs = 0;
			}

			hitstunMs = core::clamp(hitstunMs, 0.f, (f32)databaseAccessor.getMaxHitstunDurationMs());
			remainingDurationMs -= hitstunMs;
		}
		s32 numberOfFrames = -(s32)std::round(remainingDurationMs / gameFrameRate);
		frameAdvantage = numberOfFrames;
		cancelAdvantage = numberOfFrames;
		// now, calculate cancel advantage
		bool cancelFlag = false;
		if (!attacker->getCurrentMove()->getCancelIntoMovesSet().empty()) {
			s32 minCancelFrame = attacker->getCurrentMove()->getEndingFrame();
			for (auto cmove : attacker->getCurrentMove()->getCancelIntoMovesSet()) {
				if (cmove.getInputWindowEndingFrame() > attacker->getCurrentFrame()) {
					if (cmove.getInputWindowStartingFrame() < minCancelFrame) {
						minCancelFrame = cmove.getInputWindowStartingFrame();
						cancelFlag = true;
					}
				}
			}
			if (cancelFlag) {
				minCancelFrame = core::clamp(minCancelFrame,
					attacker->getCurrentFrame(),
					attacker->getCurrentMove()->getEndingFrame());
				remainingDurationMs -= delayAfterMove;
				remainingDurationMs -= (attacker->getCurrentMove()->getEndingFrame() - minCancelFrame) * frameDuration;
				numberOfFrames = -(s32)std::round(remainingDurationMs / gameFrameRate);
				cancelAdvantage = numberOfFrames;
			}
		}
		if (!cancelFlag && !attacker->getCurrentMove()->getFollowupMovesSet().empty()) {
			u32 size = attacker->getCurrentMove()->getFollowupMovesSet().size();
			u32 overlap = attacker->getCurrentMove()->hasThrowAttacks();
			if (size == 1 && overlap > 0) {
				cancelAdvantage = frameAdvantage;
			}
			else {
				remainingDurationMs -= delayAfterMove;
				numberOfFrames = -(s32)std::round(remainingDurationMs / gameFrameRate);
				cancelAdvantage = numberOfFrames;
			}
		}
		return true;
	}



	// draw additional HUD items
	void FK_SceneGameTraining::drawAdditionalHUDItems(f32 delta_t_s){
		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());
		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
		driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());
		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
		// draw input buffer if in Training Mode
		if (!isPaused()){
			hudManager->drawMoveHitFlags();
			hudManager->drawInputBuffer(player1input);
			hudManager->drawMoveName(player1);
			if (isRecordingDummyActions || isReplayingDummyActions) {
				s32 maxTime = FK_SceneGameTraining::MaxRecordTimeMs;
				FK_HUDManager_font::TrainingRecordModePhase recordPhase = FK_HUDManager_font::TrainingRecordModePhase::Standby;
				if (isReplayingDummyActions) {
					maxTime = recordBuffer[recordBufferSize - 1].time;
					recordPhase = FK_HUDManager_font::TrainingRecordModePhase::Replay;
				}
				else if (isRecordingDummyActions) {
					recordPhase = FK_HUDManager_font::TrainingRecordModePhase::Record;
				}
				hudManager->drawInputRecordTimeBar(recordTimerMs, maxTime, recordPhase);
			}
			if (dummyMovementOptions == FK_TrainingOptionsDummy::RecordDummy) {
				if (isReplayingDummyActions) {
					hudManager->drawHUDAdditionalInputRecordInputReplayPhase(delta_t_s);
				}
				else if (isRecordingDummyActions) {
					hudManager->drawHUDAdditionalInputRecordInputRecordPhase(delta_t_s);
				}
				else if (isControllingPlayer2ForRecording) {
					hudManager->drawHUDAdditionalInputRecordInputControlPhase(delta_t_s);
				}
				else {
					hudManager->drawHUDAdditionalInputRecordInputDummyPhase(delta_t_s);
				}
			}
			else {
				// draw reset buttons
				hudManager->drawHUDAdditionalResetInput();
			}
		}

		// Restore projection, world, and view matrices
		driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
		driver->setTransform(irr::video::ETS_WORLD, oldWorldMat);
		// if it is transitioning for a reset, draw a black rectangle on screen
		if (blackScreenResetCounter > 0) {
			core::dimension2d<u32> screenSize = driver->getScreenSize();
			if (gameOptions->getBorderlessWindowMode()) {
				screenSize = borderlessWindowRenderTarget->getSize();
			}
			video::SColor bcolor(blackScreenResetCounter, 0, 0, 0);
			driver->draw2DRectangle(bcolor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}
	
	/* get scene name for loading screen */
	std::wstring FK_SceneGameTraining::getSceneNameForLoadingScreen(){
		return L"Training";
	}

	/* change the "round text at beginning" flag for training */
	void FK_SceneGameTraining::processNewRoundStart(){
		FK_SceneGame::processNewRoundStart();
		processingRoundText = false;
	}

	/* always reset round in case of accidental K.O.*/
	void FK_SceneGameTraining::processRoundEnd(){
		processingKOText = false;
		newRoundFlag = true;
	}

	/* update pause menu */
	void FK_SceneGameTraining::drawPauseMenu(){
		trainingMenu->draw();
		if (demoMoveList->isVisible()) {
			demoMoveList->draw(false);
		}
		/*if (pauseMenu->isActive()){
			FK_SceneGame::drawPauseMenu();
		}*/
	}

	void FK_SceneGameTraining::resumeGame(){
		dummyMovementOptions =
			(FK_TrainingOptionsDummy)trainingMenu->getParameterValue(
			FK_TrainingOptionsMenu::FK_TrainingOption::DummyAction);
		dummyGuardOptions =
			(FK_GuardOptionsDummy)trainingMenu->getParameterValue(
			FK_TrainingOptionsMenu::FK_TrainingOption::DummyGuard);
		dummyRecoveryOptions =
			(FK_RecoveryOptionsDummy)trainingMenu->getParameterValue(
			FK_TrainingOptionsMenu::FK_TrainingOption::DummyRecovery);
		dummyUkemiOptions = (FK_UkemiOptionsDummy)trainingMenu->getParameterValue(
			FK_TrainingOptionsMenu::FK_TrainingOption::DummyUkemi);
		dummyAfterGuardOptions = (FK_AfterGuardOptionsDummy)trainingMenu->getParameterValue(
			FK_TrainingOptionsMenu::FK_TrainingOption::DummyAfterGuard);
		player2AIManager->setAILevel(trainingMenu->getParameterValue(
			FK_TrainingOptionsMenu::FK_TrainingOption::DummyAILevel));
		player2isAI = dummyMovementOptions == FK_TrainingOptionsDummy::AIControlledDummy;
		FK_SceneGame::resumeGame();
	}

	/* input time for pause menu */
	void FK_SceneGameTraining::resetInputTimeForPauseMenu() {
		trainingMenu->setCurrentTimeAsInputTime();
	}

	void FK_SceneGameTraining::updatePauseMenu(){
		if (demoMoveList->isActive()) {
			updateDemoMoveList();
		}
		else
		{
			// AI level
			auto tempAfterGuardValue = (FK_AfterGuardOptionsDummy)trainingMenu->getParameterValue(
				FK_TrainingOptionsMenu::FK_TrainingOption::DummyAfterGuard);
			auto testParameter = (FK_TrainingOptionsDummy)trainingMenu->getParameterValue(
				FK_TrainingOptionsMenu::FK_TrainingOption::DummyAction);
			if (testParameter == FK_TrainingOptionsDummy::AIControlledDummy) {
				trainingMenu->toggleItemActivity(FK_TrainingOptionsMenu::DummyAILevel, true);
			}
			else {
				trainingMenu->toggleItemActivity(FK_TrainingOptionsMenu::DummyAILevel, false);
			}
			// action after guard
			auto guardParameter = (FK_GuardOptionsDummy)trainingMenu->getParameterValue(
				FK_TrainingOptionsMenu::FK_TrainingOption::DummyGuard);
			if (guardParameter != FK_GuardOptionsDummy::NoGuard) {
				trainingMenu->toggleItemActivity(FK_TrainingOptionsMenu::DummyAfterGuard, true);
			}
			else {
				trainingMenu->toggleItemActivity(FK_TrainingOptionsMenu::DummyAfterGuard, false);
			}

			if (trainingMenu->canAcceptInput()) {
				u32 inputButtonsForPauseMenu = player1input->getPressedButtons();
				trainingMenu->update(inputButtonsForPauseMenu);
				FK_TrainingOptionsMenu::FK_TrainingOption currentItem = trainingMenu->getCurrentItem();
				// selection button
				if ((inputButtonsForPauseMenu & FK_Input_Buttons::Selection_Button) != 0) {
					switch (currentItem) {
					case FK_TrainingOptionsMenu::FK_TrainingOption::Resume:
						getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						Sleep(300);
						resumeGame();
						break;
					case FK_TrainingOptionsMenu::FK_TrainingOption::MoveList:
						getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						showMoveList(true);
						break;
					case FK_TrainingOptionsMenu::FK_TrainingOption::CharacterSelect:
						getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						abortMatch = true;
						setNextScene(getPreviousScene());
						device->getTimer()->setSpeed(fk_constants::FK_GameTimerNormalVelocity);
						winnerId = 0;
						//FK_SceneGame::updateSaveFileData();
						Sleep(300);
						break;
					case FK_TrainingOptionsMenu::FK_TrainingOption::FreeCameraMode:
						getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						activateFreeCameraMode();
						//FK_SceneGame::updateSaveFileData();
						Sleep(300);
						break;
					case FK_TrainingOptionsMenu::FK_TrainingOption::DummyAfterGuard:
						if (tempAfterGuardValue == FK_AfterGuardOptionsDummy::MoveFromListAfterGuard) {
							getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
							demoMoveList->setVisible(true);
							demoMoveList->setActive(true);
							demoMoveList->reset(false);
							Sleep(150);
						}
						break;
					case FK_TrainingOptionsMenu::FK_TrainingOption::ReturnToMenu:
						getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						abortMatch = true;
						winnerId = 0;
						//FK_SceneGame::updateSaveFileData();
						setNextScene(FK_SceneType::SceneMainMenu);
						device->getTimer()->setSpeed(fk_constants::FK_GameTimerNormalVelocity);
						Sleep(500);
						break;
					default:
						/*bool success = trainingMenu->increaseCurrentItem();
						if (success){
							getSoundManager()->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
						}*/
						break;
					}

				}
				else if ((inputButtonsForPauseMenu & FK_Input_Buttons::Cancel_Button) != 0) {
					getSoundManager()->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					resumeGame();
				}
				// left direction
				else if ((inputButtonsForPauseMenu & FK_Input_Buttons::Left_Direction) != 0) {
					bool success = trainingMenu->decreaseCurrentItem();
					if (success) {
						getSoundManager()->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
					}
				}
				// right direction
				else if ((inputButtonsForPauseMenu & FK_Input_Buttons::Right_Direction) != 0) {
					bool success = trainingMenu->increaseCurrentItem();
					if (success) {
						getSoundManager()->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
					}
				}
				// up direction
				else if ((inputButtonsForPauseMenu & FK_Input_Buttons::Up_Direction) != 0) {
					getSoundManager()->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
					trainingMenu->decreaseIndex();
				}
				// down direction
				else if ((inputButtonsForPauseMenu & FK_Input_Buttons::Down_Direction) != 0) {
					getSoundManager()->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
					trainingMenu->increaseIndex();
				}
			}
		}
	}

	void FK_SceneGameTraining::disposeAdditionalVariables(){
		delete demoMoveList;
		delete trainingMenu;
	}

	void FK_SceneGameTraining::activatePauseMenu(){
		// this method intentionally left blank
	}

	/* don't show round diamonds indicators */
	bool FK_SceneGameTraining::showRoundIndicatorsHUD(){
		return false;
	}

	/* update player 1 input while not recording for player 2*/
	void FK_SceneGameTraining::updatePlayer1Input(u32 delta_t_ms, bool& newMove) {
		if (!isControllingPlayer2ForRecording) {
			FK_SceneGame::updatePlayer1Input(delta_t_ms, newMove);
		}
	}

	/* update player 2*/
	void FK_SceneGameTraining::updatePlayer2Input(u32 delta_t_ms, bool& newMove){
		newMove = false;
		// Player 2 controlled dummy works only if there is at least one joypad connected
		if (dummyMovementOptions == FK_TrainingOptionsDummy::Player2ControlledDummy) {
			if (joystickInfo.size() < 1) {
				return;
			}
			FK_SceneGame::updatePlayer2Input(delta_t_ms, newMove);
		}
		else if (dummyMovementOptions == FK_TrainingOptionsDummy::RecordDummy) {
			if (isControllingPlayer2ForRecording) {
				u32 inputButtons = player1input->getPressedButtons();
				if ((inputButtons & FK_Input_Buttons::Help_Button) == 0) {
					FK_SceneGame::updateGenericPlayerInput(
						player2, player1input, 0, delta_t_ms, newMove);
				}
				else {
					FK_SceneGame::updateGenericPlayerInput(
						NULL, player1input, 0, delta_t_ms, newMove);
				}
			}
			else if (isReplayingDummyActions) {
				if (recordReplayInputIndexMs < recordBufferSize) {
					player2input->setBuffer(recordBuffer[recordReplayInputIndexMs].buffer, true);
					bool success = player2->performMove(player2input->readBuffer(player2));
				}
			}
		}
		else if (player2isAI){
			FK_SceneGame::updatePlayer2Input(delta_t_ms, newMove);
			//newMove = updateAIInput(delta_t_ms, 2);
		}
		else {
			FK_UkemiOptionsDummy dummyUkemiOptionToCheck = dummyUkemiOptions;
			if (dummyUkemiOptions == FK_UkemiOptionsDummy::RandomUkemi) {
				u32 switchingVariable = (u32)std::floor((double)(FK_UkemiOptionsDummy::RandomUkemi + 1) * (double)rand() / RAND_MAX);
				dummyUkemiOptionToCheck = (FK_UkemiOptionsDummy)(switchingVariable);
			}
			if (dummyUkemiOptionToCheck == FK_UkemiOptionsDummy::FGUkemi) {
				if (player2->getStance() == FK_Stance_Type::LandingStance &&
					player2->getCurrentMove() == NULL) {
					std::deque<u32> buffer;
					buffer.push_back(FK_Input_Buttons::Tech_Button);
					player2input->setBuffer(buffer);
					player2->performMove(player2input->readBuffer(player2));
				}
			}
			else if (dummyUkemiOptionToCheck == FK_UkemiOptionsDummy::BGUkemi) {
				if (player2->getStance() == FK_Stance_Type::LandingStance &&
					player2->getCurrentMove() == NULL) {
					std::deque<u32> buffer;
					buffer.push_back(FK_Input_Buttons::Tech_Button | FK_Input_Buttons::Up_Direction);
					player2input->setBuffer(buffer);
					player2->performMove(player2input->readBuffer(player2));
				}
			}
			else if (dummyUkemiOptionToCheck == FK_UkemiOptionsDummy::FrontRollUkemi) {
				if (player2->getStance() == FK_Stance_Type::LandingStance &&
					player2->getCurrentMove() == NULL) {
					std::deque<u32> buffer;
					buffer.push_back(FK_Input_Buttons::Kick_Button);
					player2input->setBuffer(buffer);
					player2->performMove(player2input->readBuffer(player2));
				}
			}
			else if (dummyUkemiOptionToCheck == FK_UkemiOptionsDummy::BackRollUkemi) {
				if (player2->getStance() == FK_Stance_Type::LandingStance &&
					player2->getCurrentMove() == NULL) {
					std::deque<u32> buffer;
					buffer.push_back(FK_Input_Buttons::Punch_Button);
					player2input->setBuffer(buffer);
					player2->performMove(player2input->readBuffer(player2));
				}
			}
		}
	}

	// update input
	void FK_SceneGameTraining::updateCharactersPressedButtons(u32 &pressedButtonsPlayer1, u32 &pressedButtonsPlayer2){
		FK_SceneGame::updateCharactersPressedButtons(pressedButtonsPlayer1, pressedButtonsPlayer2);
		// Player 2 controlled dummy works only if there is at least one joypad connected
		if (dummyMovementOptions == FK_TrainingOptionsDummy::Player2ControlledDummy) {
			if (joystickInfo.size() < 1) {
				pressedButtonsPlayer2 = 0;
				return;
			}
		}
		else if (dummyMovementOptions == FK_TrainingOptionsDummy::RecordDummy) {
			if (isControllingPlayer2ForRecording) {
				pressedButtonsPlayer2 = player1input->getPressedButtons();
				pressedButtonsPlayer1 = 0;
			}
			else if (isReplayingDummyActions) {
				if (recordReplayInputIndexMs < recordBufferSize) {
					pressedButtonsPlayer2 = recordBuffer[recordReplayInputIndexMs].pressedButtons;
				}
			}
		}
		else{
			if (!player2isAI){
				if (player1->getCurrentMove() == NULL) {
					if (dummyWasHitOnceInCurrentCombo && dummyHitOnceMsCounter <= 0){
						dummyHitOnceMsCounter = (s32)FK_SceneGameTraining::MaxTimeBeforeComboBecomesInvalidMs;
					}
				}
				if (player2->isHitStun() && !player2->isGuarding()) {
					dummyWasHitOnceInCurrentCombo = true;
					dummyHitOnceMsCounter = 0;
				}
				if (!hasBlockedAnAttack && player2->isHitStun() && player2->isGuarding()) {
					hasBlockedAnAttack = true;
				}
				pressedButtonsPlayer2 = 0;
				// grounded
				if (player2->isGrounded()){
					if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::RandomRecovery){
						u32 switchingVariable = (u32)std::floor((double)(FK_RecoveryOptionsDummy::RandomRecovery + 1) * (double)rand() / RAND_MAX);
						dummyRecoveryOptionsToCheck = (FK_RecoveryOptionsDummy)(switchingVariable);
					}
					if (groundedCounterMS > groundedDurationMS || player1->getCurrentMove() != NULL){
						if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::NoRecovery ||
							dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::RandomRecovery){
							pressedButtonsPlayer2 = FK_Input_Buttons::Guard_Button;
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::ForegroundRotation){
							pressedButtonsPlayer2 = FK_Input_Buttons::Down_Direction;
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::BackgroundRotation){
							pressedButtonsPlayer2 = FK_Input_Buttons::Up_Direction;
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::AdvanceRecovery){
							pressedButtonsPlayer2 = player2->isOnLeftSide() ? 
								FK_Input_Buttons::Right_Direction : FK_Input_Buttons::Left_Direction;
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::BackwardsRecovery){
							pressedButtonsPlayer2 = player2->isOnLeftSide() ?
								FK_Input_Buttons::Left_Direction : FK_Input_Buttons::Right_Direction;
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::CrouchingRecovery){
							pressedButtonsPlayer2 = FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Down_Direction;
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::FrontRollRecovery) {
							std::deque<u32> buffer;
							buffer.push_back(FK_Input_Buttons::Kick_Button);
							player2input->setBuffer(buffer);
							bool success = player2->performMove(player2input->readBuffer(player2));
							if (!success) {
								pressedButtonsPlayer2 = FK_Input_Buttons::Guard_Button;
							}
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::BackRollRecovery) {
							std::deque<u32> buffer;
							buffer.push_back(FK_Input_Buttons::Punch_Button);
							player2input->setBuffer(buffer);
							bool success = player2->performMove(player2input->readBuffer(player2));
							if (!success) {
								pressedButtonsPlayer2 = FK_Input_Buttons::Guard_Button;
							}
						}
						else if (dummyRecoveryOptionsToCheck == FK_RecoveryOptionsDummy::Reversal) {
							std::deque<u32> buffer;
							buffer.push_back(FK_Input_Buttons::Punch_plus_Kick_Button);
							player2input->setBuffer(buffer);
							bool success = player2->performMove(player2input->readBuffer(player2));
							if (!success) {
								pressedButtonsPlayer2 = FK_Input_Buttons::Guard_Button;
							}
						}
					}
					currentPressedButtons = pressedButtonsPlayer2;
					return;
				}
				else {
					updateGroundedTimer = false;
					if (groundedCounterMS > 0) {
						groundedCounterMS = 0;
					}
					dummyRecoveryOptionsToCheck = dummyRecoveryOptions;
					// after guard option
					if (hasBlockedAnAttack && player2->canAcceptInput() && !player2->isGuarding()) {
						hasBlockedAnAttack = false;
						if (dummyAfterGuardOptions == FK_AfterGuardOptionsDummy::BackstepAfterGuard) {
							std::deque<u32> buffer;
							u32 direction = FK_Input_Buttons::Left_Direction;
							if (!player2->isOnLeftSide()) {
								direction = FK_Input_Buttons::Right_Direction;
							}
							buffer.push_back(direction);
							buffer.push_back(direction);
							player2input->setBuffer(buffer);
							if (player2->performMove(player2input->readBuffer(player2))) {
								return;
							}
						}
						else if(dummyAfterGuardOptions == FK_AfterGuardOptionsDummy::SidestepBackgroundAfterGuard) {
							std::deque<u32> buffer;
							u32 direction = FK_Input_Buttons::Up_Direction;
							buffer.push_back(direction);
							buffer.push_back(direction);
							player2input->setBuffer(buffer);
							if (player2->performMove(player2input->readBuffer(player2))) {
								return;
							}
						}
						else if (dummyAfterGuardOptions == FK_AfterGuardOptionsDummy::SidestepForegroundAfterGuard) {
							std::deque<u32> buffer;
							u32 direction = FK_Input_Buttons::Down_Direction;
							buffer.push_back(direction);
							buffer.push_back(direction);
							player2input->setBuffer(buffer);
							if (player2->performMove(player2input->readBuffer(player2))) {
								return;
							}
						}
						else if (dummyAfterGuardOptions == FK_AfterGuardOptionsDummy::MoveFromListAfterGuard) {
							if (!afterGuardMove.getInputStringLeft().empty()) {
								std::deque<u32> buffer = afterGuardMove.getInputString(player2->isOnLeftSide());
								player2->setStance(afterGuardMove.getStance());
								if (afterGuardMove.getStance() == FK_Stance_Type::CrouchedStance) {
									buffer.pop_front();
									buffer.pop_front();
								}
								player2input->setBuffer(buffer);
								if (player2->performMove(player2input->readBuffer(player2))) {
									return;
								}
							}
						}
					}
					// guard options
					if (updateGuardDuration && guardDurationCounterMS > guardDurationMS) {
						updateGuardDuration = false;
					}
					if (updateGuardDuration && guardDurationCounterMS <= guardDurationMS &&
						(dummyGuardOptions == FK_GuardOptionsDummy::RandomGuard)) {
						pressedButtonsPlayer2 = currentPressedButtons;
					}
					else {
						bool proximityLowBullet = false;
						bool proximityHighBullet = false;
						for each(FK_Bullet* bullet in player1->getBulletsCollection()) {
							if (!bullet->isDisposeable() && bullet->getNode()->isVisible()) {
								f32 distance = bullet->getNode()->getAbsolutePosition().getDistanceFrom(player2->getPosition());
								f32 maxRadius = max(bullet->getHitboxRadius().X, max(bullet->getHitboxRadius().Y, bullet->getHitboxRadius().Z));
								f32 delta_t_frame = 1 / 60.f;
								f32 bulletDistanceTravelledInOneFrame = bullet->getVelocity().X * delta_t_frame;
								f32 characterHitboxOffset = 50.0f;
								f32 distanceToCheck = 2 * maxRadius + 2 * abs(bulletDistanceTravelledInOneFrame) + characterHitboxOffset;
								if (distance <= distanceToCheck && !bullet->getHitbox().hasHit()) {
									proximityLowBullet = bullet->getHitbox().canBeCrouchGuarded();
									proximityHighBullet = bullet->getHitbox().canBeStandGuarded();
								}
							}
						}
						if (!(proximityHighBullet || proximityLowBullet)) {
							if (player1->getCurrentMove() != NULL && !player1->getCurrentMove()->getBulletCollection().empty()) {
								for each(FK_Bullet bullet in player1->getCurrentMove()->getBulletCollection()) {
									bool frameCondition =
										std::abs(player1->getCurrentFrame() - bullet.getHitbox().getStartingFrame()) < 5;
									//player1->getCurrentFrame() > bullet.getHitbox().getStartingFrame();
									f32 distance = player1->getPosition().getDistanceFrom(player2->getPosition());
									bool distanceCondition = distance <= 100.f; // ad hoc condition for nearby bullets
									if (frameCondition && distanceCondition) {
										proximityLowBullet = bullet.getHitbox().canBeCrouchGuarded();
										proximityHighBullet = bullet.getHitbox().canBeStandGuarded();
									}
								}
							}
						}
						if ((player1->getCurrentMove() != NULL && player1->getCurrentMove()->canBeTriggered()) ||
							proximityLowBullet ||
							proximityHighBullet) {
							if (!hasBlockedAnAttack || dummyAfterGuardOptions == FK_AfterGuardOptionsDummy::NoAfterGuardAction) {
								if (dummyGuardOptions == FK_GuardOptionsDummy::OnlyStandingGuard) {
									pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button;
								}
								else if (dummyGuardOptions == FK_GuardOptionsDummy::OnlyCrouchingGuard) {
									pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Down_Direction;
									if (player2->isOnLeftSide()) {
										pressedButtonsPlayer2 |= FK_Input_Buttons::Right_Direction;
									}
									else {
										pressedButtonsPlayer2 |= FK_Input_Buttons::Left_Direction;
									}
								}
								else if (dummyGuardOptions == FK_GuardOptionsDummy::AlwaysGuard) {
									pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button;
									if ((!proximityHighBullet && proximityLowBullet) ||
										(player1->getCurrentAttackType() & FK_Attack_Type::LowAtks) != 0) {
										pressedButtonsPlayer2 |= FK_Input_Buttons::Down_Direction;
										if (player2->isOnLeftSide()) {
											pressedButtonsPlayer2 |= FK_Input_Buttons::Right_Direction;
										}
										else {
											pressedButtonsPlayer2 |= FK_Input_Buttons::Left_Direction;
										}
									}
								}
								else if (dummyGuardOptions == FK_GuardOptionsDummy::GuardAfterFirstHit) {
									if (dummyWasHitOnceInCurrentCombo) {
										pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button;
										if ((!proximityHighBullet && proximityLowBullet) ||
											(player1->getCurrentAttackType() & FK_Attack_Type::LowAtks) != 0) {
											pressedButtonsPlayer2 |= FK_Input_Buttons::Down_Direction;
											if (player2->isOnLeftSide()) {
												pressedButtonsPlayer2 |= FK_Input_Buttons::Right_Direction;
											}
											else {
												pressedButtonsPlayer2 |= FK_Input_Buttons::Left_Direction;
											}
										}
									}
								}
								else if (dummyGuardOptions == FK_GuardOptionsDummy::RandomGuard) {
									double rdn = (double)rand() / RAND_MAX;
									if (rdn >= 0.5) {
										pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button;
										if ((!proximityHighBullet && proximityLowBullet) ||
											(player1->getCurrentAttackType() & FK_Attack_Type::LowAtks) != 0) {
											pressedButtonsPlayer2 |= FK_Input_Buttons::Down_Direction;
											if (player2->isOnLeftSide()) {
												pressedButtonsPlayer2 |= FK_Input_Buttons::Right_Direction;
											}
											else {
												pressedButtonsPlayer2 |= FK_Input_Buttons::Left_Direction;
											}
										}
										updateGuardDuration = true;
										guardDurationCounterMS = 0;
									}
								}
							}
						}

						if (pressedButtonsPlayer2 != 0) {
							return;
						}
						// movement
						if (dummyMovementOptions == FK_TrainingOptionsDummy::StandingDummy) {

						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::WalkingDummy) {
							if (player2->getPosition().getDistanceFrom(player1->getPosition()) > 50.f) {
								pressedButtonsPlayer2 |= player2->isOnLeftSide() ?
									FK_Input_Buttons::Right_Direction : FK_Input_Buttons::Left_Direction;
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::WalkBackwardDummy) {
							if (player2->getPosition().getDistanceFrom(player1->getPosition()) < 200.f) {
								pressedButtonsPlayer2 |= player2->isOnLeftSide() ?
									FK_Input_Buttons::Left_Direction : FK_Input_Buttons::Right_Direction;
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::ForegroundSidestepDummy) {
							if (player2->getStance() != FK_Stance_Type::CrouchedStance) {
								pressedButtonsPlayer2 |= FK_Input_Buttons::Down_Direction;
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::BackgroundSidestepDummy) {
							pressedButtonsPlayer2 |= FK_Input_Buttons::Up_Direction;
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::CrouchingDummy) {
							if (player2->getStance() != FK_Stance_Type::CrouchedStance) {
								pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Down_Direction;
							}
							else {
								pressedButtonsPlayer2 |= FK_Input_Buttons::Down_Direction;
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::JumpingDummy) {
							if (player2->canJump()) {
								////pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Up_Direction;
								//player2->performJump(FK_Character::FK_JumpDirection::JumpUp);
								//jumpCounterMS = 0;
								pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Up_Direction;
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::JumpingForwardDummy) {
							if (player2->canJump()) {
								pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Up_Direction;
								pressedButtonsPlayer2 |= player2->isOnLeftSide() ?
									FK_Input_Buttons::Right_Direction : FK_Input_Buttons::Left_Direction;
								//player2->performJump(FK_Character::FK_JumpDirection::JumpForward);
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::JumpingBackwardDummy) {
							if (player2->canJump()) {
								pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Up_Direction;
								pressedButtonsPlayer2 |= player2->isOnLeftSide() ?
									FK_Input_Buttons::Left_Direction : FK_Input_Buttons::Right_Direction;
							}
						}
						else if (dummyMovementOptions == FK_TrainingOptionsDummy::RandomJump) {
							if (player2->canJump()) {
								pressedButtonsPlayer2 |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Up_Direction;
								s32 direction = (s32)std::floor((f32)(3 * rand()) / RAND_MAX - 1);
								if (direction == FK_Character::FK_JumpDirection::JumpBackward) {
									pressedButtonsPlayer2 |= player2->isOnLeftSide() ?
										FK_Input_Buttons::Left_Direction : FK_Input_Buttons::Right_Direction;
								}
								else if (direction == FK_Character::FK_JumpDirection::JumpForward) {
									pressedButtonsPlayer2 |= player2->isOnLeftSide() ?
										FK_Input_Buttons::Right_Direction : FK_Input_Buttons::Left_Direction;
								}
							}
						}
						currentPressedButtons = pressedButtonsPlayer2;
					}
				}
			}
		}
	}
}