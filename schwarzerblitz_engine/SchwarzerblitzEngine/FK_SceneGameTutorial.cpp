#include"FK_SceneGameTutorial.h"

using namespace irr;

namespace fk_engine{
	FK_SceneGameTutorial::FK_SceneGameTutorial() : FK_SceneGameTraining(){
		phaseIndex = -1;
		numberOfPhases = -1;
		messageIndex = -1;
		canSkipDialogue = false;
		switchDialogue = false;
		oldPlayerStance = FK_Stance_Type::NoStance;
		crouchingTimer = 0;
		runningTimer = 0;
		numberOfJumps = 0;
		playerJumpStatus = false;
		previousOpponentLife = -1;
		selection_timer_ms = 0;
		playerSpecificMoveCount = 0;
		opponentOutfitName = std::string();
		playerOutfitName = std::string();
		tutorialId = std::string();
		tutorialDirectory = std::string();
		lastMoveName = std::string();
		lastMoveId = 0;
		tutorialPhasesSize = 0;
		numberOfMovesToPerform = 0;
		tutorialPhases.clear();
		collectiblesCreated = false;
		collectibles.clear();
	}

	void FK_SceneGameTutorial::dispose(){
		delete messageWindow;
		disposeCollectibles();
		FK_SceneGame::dispose();
	}

	void FK_SceneGameTutorial::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		std::string tutorialFileName,
		std::string new_tutorialId,
		FK_Scene::FK_SceneTrainingType newChallengeType,
		u32 new_challengeIndex){
		FK_SceneGame::FK_AdditionalSceneGameOptions newAdditionalOptions;
		newAdditionalOptions.roundTimerInSeconds = 100;
		newAdditionalOptions.numberOfRounds = 9;
		tutorialId = new_tutorialId;
		challengeIndex = new_challengeIndex;
		challengeType = newChallengeType;
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		loadTutorialFlowFile(tutorialFileName);
		setupResourcesPaths(playerPath, opponentPath, stagePath);
		FK_Character* character = new FK_Character();
		if (playerOutfitName.empty()){
			character->loadBasicVariables(
				databaseAccessor,
				"character.txt", charactersPath + playerPath, commonResourcesPath, smgr, playerOutfitId);
		}
		else{
			character->loadBasicVariables(
				databaseAccessor,
				"character.txt", charactersPath + playerPath, commonResourcesPath, smgr, playerOutfitName);
		}
		FK_Character::FK_CharacterOutfit new_outfitPlayer1 = character->getOutfit();
		delete character;
		character = new FK_Character();
		if (opponentOutfitName.empty()){
			character->loadBasicVariables(
				databaseAccessor,
				"character.txt", charactersPath + opponentPath, commonResourcesPath, smgr, opponentOutfitId);
		}
		else{
			character->loadBasicVariables(
				databaseAccessor,
				"character.txt", charactersPath + opponentPath, commonResourcesPath, smgr, opponentOutfitName);
		}
		FK_Character::FK_CharacterOutfit new_outfitPlayer2 = character->getOutfit();
		delete character;
		FK_SceneGame::setup(newDevice, new_joypadInfo, newOptions, playerPath, opponentPath,
			new_outfitPlayer1, new_outfitPlayer2, stagePath, FK_Scene::FK_SceneType::SceneGameFreeMatch,
			newAdditionalOptions, false, false, 0, 0);
		storeMoveIdsInTrainingSets();
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		setNextScene(FK_Scene::FK_SceneType::SceneGameTutorial);
		setPreviousScene(FK_Scene::FK_SceneType::SceneGameTutorial);
	}

	void FK_SceneGameTutorial::setupAdditionalVariables(){
		FK_SceneGameTraining::setupAdditionalVariables();
		dummyRecoveryOptions = FK_RecoveryOptionsDummy::TutorialRecovery;
		createMessageWindow();
	}

	bool FK_SceneGameTutorial::isRunning(){
		bool specificPhaseRunning = messageWindow->isAnimating() || (tutorialPhasesSize == 0) || phaseIndex < (s32)tutorialPhasesSize;
		return FK_SceneGame::isRunning();// && specificPhaseRunning;
	}

	FK_SceneGameTutorial::FK_TutorialTrainingSet FK_SceneGameTutorial::getCurrentPhase(){
		if (phaseIndex < 0 || (tutorialPhasesSize == 0) || phaseIndex >=(s32)tutorialPhasesSize){
			return FK_TutorialTrainingSet();
		}
		return tutorialPhases.at(phaseIndex);
	}

	void FK_SceneGameTutorial::resumeGame(){
		FK_SceneGame::resumeGame();
	}

	void FK_SceneGameTutorial::storeMoveIdsInTrainingSets() {
		if (player1 == NULL) {
			return;
		}
		// add move IDs after player character is loaded to decrease processing time
		u32 size = tutorialPhases.size();
		u32 movesetSize = player1->getMovesCollection().size();
		for (u32 i = 0; i < size; ++i) {
			if (tutorialPhases[i].phase == FK_TutorialPhase::FlowCombo ||
				tutorialPhases[i].phase == FK_TutorialPhase::Trigger ||
				(tutorialPhases[i].phase == FK_TutorialPhase::TriggerRunCancel && !tutorialPhases[i].additionalStringParameters.empty())) {
				auto originalArray = tutorialPhases[i].additionalNumericParameters;
				tutorialPhases[i].additionalNumericParameters.clear();
				for each (std::string moveName in tutorialPhases[i].additionalStringParameters) {
					for (u32 k = 0; k < movesetSize; ++k) {
						if (moveName == player1->getMovesCollection()[k].getName()) {
							tutorialPhases[i].additionalNumericParameters.push_back(
								player1->getMovesCollection()[k].getMoveId());
							break;
						}
					}
				}
				for (auto entry : originalArray) {
					tutorialPhases[i].additionalNumericParameters.push_back(entry);
				}
			}
			tutorialPhases[i].numberOfStringParameters = tutorialPhases[i].additionalStringParameters.size();
			tutorialPhases[i].numberOfNumericParameters = tutorialPhases[i].additionalNumericParameters.size();
		}
		
	}

	void FK_SceneGameTutorial::loadTutorialFlowFile(std::string new_filename){
		tutorialDirectory = new_filename;
		std::map<std::string, FK_Input_Buttons> inputMap;
		std::string comment = "###";
		/* create input map */
		inputMap["0"] = FK_Input_Buttons::None; // for throw escape only
		inputMap["*"] = FK_Input_Buttons::Neutral_Input;
		inputMap["2"] = FK_Input_Buttons::Down_Direction;
		inputMap["3"] = FK_Input_Buttons::DownRight_Direction;
		inputMap["1"] = FK_Input_Buttons::DownLeft_Direction;
		inputMap["4"] = FK_Input_Buttons::Left_Direction;
		inputMap["7"] = FK_Input_Buttons::UpLeft_Direction;
		inputMap["8"] = FK_Input_Buttons::Up_Direction;
		inputMap["9"] = FK_Input_Buttons::UpRight_Direction;
		inputMap["6"] = FK_Input_Buttons::Right_Direction;
		inputMap["H2"] = FK_Input_Buttons::HeldDown_Direction;
		inputMap["H3"] = FK_Input_Buttons::HeldDownRight_Direction;
		inputMap["H1"] = FK_Input_Buttons::HeldDownLeft_Direction;
		inputMap["H4"] = FK_Input_Buttons::HeldLeft_Direction;
		inputMap["H7"] = FK_Input_Buttons::HeldUpLeft_Direction;
		inputMap["H8"] = FK_Input_Buttons::HeldUp_Direction;
		inputMap["H9"] = FK_Input_Buttons::HeldUpRight_Direction;
		inputMap["H6"] = FK_Input_Buttons::HeldRight_Direction;
		inputMap["G"] = FK_Input_Buttons::Guard_Button;
		inputMap["T"] = FK_Input_Buttons::Tech_Button;
		inputMap["K"] = FK_Input_Buttons::Kick_Button;
		inputMap["P"] = FK_Input_Buttons::Punch_Button;
		inputMap["Tg"] = FK_Input_Buttons::Trigger_Button;
		// deptecated - only for retro compatibility/testing - to be deleted soon!!!
		inputMap["A"] = FK_Input_Buttons::Guard_Button; //deprecated
		inputMap["B"] = FK_Input_Buttons::Tech_Button; //deprecated
		inputMap["X"] = FK_Input_Buttons::Kick_Button; //deprecated
		inputMap["Y"] = FK_Input_Buttons::Punch_Button; //deprecated
		inputMap["R"] = FK_Input_Buttons::Trigger_Button; //deprecated
		inputMap["v"] = FK_Input_Buttons::Down_Direction;
		inputMap["v>"] = FK_Input_Buttons::DownRight_Direction;
		inputMap["<v"] = FK_Input_Buttons::DownLeft_Direction;
		inputMap["<"] = FK_Input_Buttons::Left_Direction;
		inputMap["<^"] = FK_Input_Buttons::UpLeft_Direction;
		inputMap["^"] = FK_Input_Buttons::Up_Direction;
		inputMap["^>"] = FK_Input_Buttons::UpRight_Direction;
		inputMap[">"] = FK_Input_Buttons::Right_Direction;
		inputMap["Hv"] = FK_Input_Buttons::HeldDown_Direction;
		inputMap["Hv>"] = FK_Input_Buttons::HeldDownRight_Direction;
		inputMap["H<v"] = FK_Input_Buttons::HeldDownLeft_Direction;
		inputMap["H<"] = FK_Input_Buttons::HeldLeft_Direction;
		inputMap["H<^"] = FK_Input_Buttons::HeldUpLeft_Direction;
		inputMap["H^"] = FK_Input_Buttons::HeldUp_Direction;
		inputMap["H^>"] = FK_Input_Buttons::HeldUpRight_Direction;
		inputMap["H>"] = FK_Input_Buttons::HeldRight_Direction;
		/* define the input combination identifier as "+" */
		std::string combinationIdentifier = "+";
		std::map<FK_TutorialKeywords, std::string> parsingMap;
		parsingMap[FK_TutorialKeywords::PhaseBegin] = "#PHASE";
		parsingMap[FK_TutorialKeywords::PhaseEnd] = "#PHASE_END";
		parsingMap[FK_TutorialKeywords::MessageBegin] = "#MESSAGE_BEGIN";
		parsingMap[FK_TutorialKeywords::MessageEnd] = "#MESSAGE_END";
		parsingMap[FK_TutorialKeywords::PhaseTypeBegin] = "#TYPE";
		parsingMap[FK_TutorialKeywords::PhaseTypeEnd] = "#TYPE_END";
		parsingMap[FK_TutorialKeywords::PlayerPath] = "#CHARACTER";
		parsingMap[FK_TutorialKeywords::OpponentPath] = "#OPPONENT";
		parsingMap[FK_TutorialKeywords::StagePath] = "#STAGE";
		parsingMap[FK_TutorialKeywords::FileEnd] = "#END";
		parsingMap[FK_TutorialKeywords::Description] = "#HINT";
		parsingMap[FK_TutorialKeywords::InputBegin] = "#INPUT";
		parsingMap[FK_TutorialKeywords::InputEnd] = "#INPUT_END";

		std::map<std::string, FK_TutorialPhase> phaseParsingMap;
		phaseParsingMap["movementFWD"] = FK_TutorialPhase::MovementForward;
		phaseParsingMap["movementBKW"] = FK_TutorialPhase::MovementBackwards;
		phaseParsingMap["text"] = FK_TutorialPhase::GenericText;
		phaseParsingMap["movementSide"] = FK_TutorialPhase::MovementSidewards;
		phaseParsingMap["crouch"] = FK_TutorialPhase::Crouching;
		phaseParsingMap["jump"] = FK_TutorialPhase::Jumping;
		phaseParsingMap["run"] = FK_TutorialPhase::Run;
		phaseParsingMap["backstep"] = FK_TutorialPhase::Backstep;
		phaseParsingMap["ukemi"] = FK_TutorialPhase::Ukemi;
		phaseParsingMap["punch"] = FK_TutorialPhase::Punch;
		phaseParsingMap["specific"] = FK_TutorialPhase::SpecificMove;
		phaseParsingMap["trigger"] = FK_TutorialPhase::Trigger;
		phaseParsingMap["triggerGuard"] = FK_TutorialPhase::TriggerGuard;
		phaseParsingMap["triggerRunCancel"] = FK_TutorialPhase::TriggerRunCancel;
		phaseParsingMap["triggerReversal"] = FK_TutorialPhase::TriggerReversal;
		phaseParsingMap["triggerSpecialMove"] = FK_TutorialPhase::TriggerSpecialMove;
		phaseParsingMap["throw"] = FK_TutorialPhase::TechThrow;
		phaseParsingMap["combo"] = FK_TutorialPhase::FlowCombo;
		phaseParsingMap["kick"] = FK_TutorialPhase::Kick;
		phaseParsingMap["guard"] = FK_TutorialPhase::StandingGuard;
		phaseParsingMap["ringout"] = FK_TutorialPhase::Ringout;
		phaseParsingMap["collect"] = FK_TutorialPhase::CoinCollect;
		phaseParsingMap["collectCrouch"] = FK_TutorialPhase::CoinCollectCrouch;
		phaseParsingMap["trueCombo"] = FK_TutorialPhase::TrueCombo;

		std::string numericParameterTag = "N";
		std::string stringParameterTag = "S";
		tutorialResourcesPath = mediaPath + "scenes\\tutorial\\";
		std::string filename = tutorialResourcesPath + FK_SceneGameTutorial::TutorialInputFileName;
		if (!new_filename.empty()){
			tutorialResourcesPath = mediaPath + "scenes\\tutorial\\challenges\\" + new_filename + "\\";
			filename = tutorialResourcesPath + FK_SceneGameTutorial::TutorialInputFileName;
		}		
		std::ifstream configFile(filename.c_str());
		if (!configFile){
			std::cout << "Warning: there are issues with the tutorial config file!" << std::endl;
			return;
		}
		std::string temp;
		TutorialDialogue dialogue = TutorialDialogue();
		FK_TutorialTrainingSet tempSet = FK_TutorialTrainingSet();
		tutorialPhasesSize = 0;
		tutorialPhases.clear();
		while (configFile >> temp){
			if (temp == parsingMap[FK_TutorialKeywords::FileEnd]){
				break;
			}
			else if (temp == comment){
				while (configFile >> temp){
					if (temp == comment){
						break;
					}
				}
			}
			else if (temp == parsingMap[FK_TutorialKeywords::PlayerPath]){
				configFile >> playerPath;
				configFile >> playerOutfitId;
				std::string tempStr = playerPath;
				s32 pos = tempStr.find('\\');
				if (pos != std::string::npos){
					playerPath = tempStr.substr(0, pos);
					playerOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				playerPath += "\\";
			}
			else if (temp == parsingMap[FK_TutorialKeywords::OpponentPath]){
				configFile >> opponentPath;
				configFile >> opponentOutfitId;
				std::string tempStr = opponentPath;
				s32 pos = tempStr.find('\\');
				if (pos != std::string::npos){
					opponentPath = tempStr.substr(0, pos);
					opponentOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				opponentPath += "\\";
			}
			else if (temp == parsingMap[FK_TutorialKeywords::StagePath]){
				configFile >> stagePath;
				stagePath += "\\";
			}
			else if (temp == parsingMap[FK_TutorialKeywords::PhaseBegin]){
				tempSet = FK_TutorialTrainingSet();
				while (configFile >> temp){
					if (temp == parsingMap[FK_TutorialKeywords::PhaseEnd]){
						tutorialPhases.push_back(tempSet);
						tutorialPhasesSize += 1;
						FK_TutorialTrainingSet tempSet = FK_TutorialTrainingSet();
						break;
					}
					else if (temp == parsingMap[FK_TutorialKeywords::MessageBegin]){
						dialogue = TutorialDialogue();
						while (configFile){
							std::getline(configFile, temp);
							if (temp == parsingMap[FK_TutorialKeywords::MessageEnd]){
								tempSet.messages.push_back(dialogue);
								dialogue = TutorialDialogue();
								break;
							}
							if (!(temp.empty())){
								dialogue.dialogueText.push_back(temp);
							}
						}
					}
					else if (temp == parsingMap[FK_TutorialKeywords::Description]){
						temp = std::string();
						while (temp.empty()){
							std::getline(configFile, temp);
						}
						tempSet.description = temp;
						tempSet.inputAndHintData.setDisplayName(tempSet.description);
					}
					/* next, parse the INPUT required for the move */
					else if (temp == parsingMap[FK_TutorialKeywords::InputBegin]){
						u32 lastButton = 0;
						u32 button = 0;
						tempSet.inputArray.clear();
						std::deque<u32> rightInputArray = std::deque<u32>();
						bool combination = false;
						while (!configFile.eof()){
							lastButton = button;
							std::string temp;
							configFile >> temp;
							/* if the INPUT end tag is found, break cycle*/
							if (temp == parsingMap[FK_TutorialKeywords::InputEnd]){
								break;
							}
							else{
								if (strcmp(temp.c_str(), combinationIdentifier.c_str()) == 0){
									combination = true;
									tempSet.inputArray.pop_back();
									rightInputArray.pop_back();
								}
								else{
									button = inputMap[temp];
									if (combination){
										combination = false;
										button = button | lastButton;
									}
									u32 buttonRight = button;
									if (button & FK_Input_Buttons::Left_Direction){
										buttonRight = button - FK_Input_Buttons::Left_Direction + FK_Input_Buttons::Right_Direction;
									}
									else if (button & FK_Input_Buttons::Right_Direction){
										buttonRight = button - FK_Input_Buttons::Right_Direction + FK_Input_Buttons::Left_Direction;
									}
									else if (button & FK_Input_Buttons::HeldLeft_Direction){
										buttonRight = button - FK_Input_Buttons::HeldLeft_Direction + FK_Input_Buttons::HeldRight_Direction;
									}
									else if (button & FK_Input_Buttons::HeldRight_Direction){
										buttonRight = button - FK_Input_Buttons::HeldRight_Direction + FK_Input_Buttons::HeldLeft_Direction;
									}
									tempSet.inputArray.push_back(button);
									rightInputArray.push_back(buttonRight);
								}
							}
						}
						/* store input string*/
						tempSet.inputAndHintData.setInputStringLeft(tempSet.inputArray);
						tempSet.inputAndHintData.setInputStringRight(rightInputArray);
					}
					else if (temp == parsingMap[FK_TutorialKeywords::PhaseTypeBegin]){
						configFile >> temp;
						if (phaseParsingMap.count(temp) > 0){
							tempSet.phase = phaseParsingMap[temp];
							while (configFile){
								configFile >> temp;
								if (temp == parsingMap[FK_TutorialKeywords::PhaseTypeEnd]){
									break;
								}
								else if (temp == numericParameterTag){
									f32 tempNumeric;
									configFile >> tempNumeric;
									tempSet.additionalNumericParameters.push_back(tempNumeric);
								}
								else if (temp == stringParameterTag){
									configFile >> temp;
									tempSet.additionalStringParameters.push_back(temp);
								}
							}
						}
					}
				}
			}
		}
	}

	/* workaround fix to prevent issues in Trial mode*/
	void FK_SceneGameTutorial::drawHUDThrowName(FK_Character* performer) {
		
	}

	void FK_SceneGameTutorial::createMessageWindow(){
		f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		core::rect<s32> frameTop(50, 10, 590, 90);
		//core::rect<s32> frameBottom(50, 270, 590, 350);
		//core::rect<s32> frameCenter(50, 140, 590, 220);
		messageWindow = new FK_DialogueWindow(device, frameTop, "windowskin.png", commonResourcesPath, scaleFactor,
			std::string(), std::vector<std::string>(), FK_DialogueWindow::DialogueWindowAlignment::Right,
			FK_DialogueWindow::DialogueTextMode::CharByChar);
		messageWindow->setAlpha(220);
	}

	void FK_SceneGameTutorial::drawAdditionalHUDItems(f32 delta_t_s){
		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());
		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
		driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());
		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

		if (!isPaused()){
			hudManager->drawMoveHitFlags();
		}
		f32 scaleFactorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		f32 scaleFactorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		if (isRunning()){
			if (messageWindow->isOpen() && messageWindow->isActive() && !messageWindow->isAllTextDisplayed()){
				if (typingSoundTimerMS > FK_SceneGameTutorial::TypingSoundInterval){
					if (messageWindow->canPlaySound()){
						typingSoundTimerMS = 0;
						getSoundManager()->playSound("cursor", 30.0 * gameOptions->getSFXVolume(), 0.6f);
						messageWindow->flagSoundAsPlayed();
					}
				}
			}
			if (messageWindow->isVisible()){
				messageWindow->draw(screenSize, scaleFactorX, scaleFactorY);
			}
		}
		if (messageIndex < 0 && !isPaused() && !isEndOfRound()){
			hudManager->drawInputBuffer(player1input);
			FK_Move tempMove = getCurrentPhase().inputAndHintData;
			if (getCurrentPhase().phase == FK_TutorialPhase::Backstep || 
				getCurrentPhase().phase == FK_TutorialPhase::Ukemi ||
				getCurrentPhase().phase == FK_TutorialPhase::TechThrow) {
				u32 quantityToUse = numberOfEvadedThrows;
				u32 paramIndex = 2;
				if (getCurrentPhase().phase == FK_TutorialPhase::Backstep || 
					getCurrentPhase().phase == FK_TutorialPhase::Ukemi) {
					quantityToUse = playerSpecificMoveCount;
				}
				if (getCurrentPhase().phase == FK_TutorialPhase::Backstep ||
					getCurrentPhase().phase == FK_TutorialPhase::TriggerSpecialMove) {
					paramIndex = 0;
				}
				std::wstring moveDisplayName = tempMove.getWDisplayName();
				if (getCurrentPhase().additionalNumericParameters[paramIndex] > 1) {
					moveDisplayName += L" [" +
						std::to_wstring(quantityToUse) + L" / " +
						std::to_wstring((u32)std::round(getCurrentPhase().additionalNumericParameters[paramIndex])) +
						L"]";
				}
				tempMove.setWDisplayName(moveDisplayName);
			}
			hudManager->drawInputFromMove(tempMove, player1->isOnLeftSide(),
				getCurrentPhase().phase == FK_TutorialPhase::Trigger ||
				getCurrentPhase().phase == FK_TutorialPhase::TriggerGuard ||
				getCurrentPhase().phase == FK_TutorialPhase::TriggerRunCancel);
		}
		// Restore projection, world, and view matrices
		driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
		driver->setTransform(irr::video::ETS_WORLD, oldWorldMat);
		if (blackScreenResetCounter > 0) {
			core::dimension2d<u32> screenSize = driver->getScreenSize();
			if (gameOptions->getBorderlessWindowMode()) {
				screenSize = borderlessWindowRenderTarget->getSize();
			}
			video::SColor bcolor(blackScreenResetCounter, 0, 0, 0);
			driver->draw2DRectangle(bcolor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}

	bool FK_SceneGameTutorial::canUpdateInput(){
		return FK_SceneGame::canUpdateInput() && (!messageWindow->isAnimating() && messageIndex < 0) &&
			getRoundWinner() != -1;
	}

	bool FK_SceneGameTutorial::canPauseGame(){
		return FK_SceneGame::canPauseGame() && (!messageWindow->isAnimating() && messageIndex < 0);
	}

	// pause menu
	void FK_SceneGameTutorial::drawPauseMenu(){
		FK_SceneGame::drawPauseMenu();
	}

	void FK_SceneGameTutorial::updatePauseMenu(){
		FK_SceneGame::updatePauseMenu();
	}
	
	void FK_SceneGameTutorial::activatePauseMenu(){
		FK_SceneGame::activatePauseMenu();
	}

	void FK_SceneGameTutorial::cancelThrow(){
		FK_SceneGame::cancelThrow();
		numberOfEvadedThrows += 1;
	}

	void FK_SceneGameTutorial::updateCharactersPressedButtons(u32 &pressedButtonsPlayer1, u32 &pressedButtonsPlayer2){
		FK_SceneGame::updateCharactersPressedButtons(pressedButtonsPlayer1, pressedButtonsPlayer2);
		if (opponentHasToAct()){
			if (canUpdateInput() && opponentMoveTimer < 0 && opponentAttackIndex == 0){
				f32 distance = player1->getAnimatedMesh()->getAbsolutePosition().getDistanceFrom(
					player2->getAnimatedMesh()->getAbsolutePosition());
				if (distance > getCurrentPhase().additionalNumericParameters[0]){
					pressedButtonsPlayer2 = player2->isOnLeftSide() ?
						FK_Input_Buttons::Right_Direction : FK_Input_Buttons::Left_Direction;
				}
			}
		}
		if (getCurrentPhase().phase == FK_TutorialPhase::TriggerGuard && getCurrentPhase().additionalNumericParameters[3] > 0){
			pressedButtonsPlayer2 |= FK_Input_Buttons::Trigger_Button;
		}
	}

	void FK_SceneGameTutorial::updateTriggerComboEffect(u32 frameDelta, bool newMovePlayer1, bool newMovePlayer2)
	{
		bool player1NewMoveCheck = player1->getCurrentMove() != NULL && newMovePlayer1 &&
			player1MoveToCheck != player1->getCurrentMove()->getName();
		/* specific move counter */
		if (player1NewMoveCheck) {
			if (getCurrentPhase().phase == FK_TutorialPhase::Backstep ||
				getCurrentPhase().phase == FK_TutorialPhase::TriggerSpecialMove) {
				if (newMovePlayer1 && player1->getCurrentMove()->getName() == getCurrentPhase().additionalStringParameters[0]) {
					playerSpecificMoveCount += 1;
					getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
				}
			}
			player1NewMoveCheck = false;
		}
		FK_SceneGame::updateTriggerComboEffect(frameDelta, newMovePlayer1, newMovePlayer2);
	};

	// frame advantage is useless in Tutorial mode
	bool FK_SceneGameTutorial::calculateFrameAdvantage(s32&, s32&, FK_Hitbox *, FK_Character *, FK_Character *, bool, bool, bool)
	{
		return false;
	}

	void FK_SceneGameTutorial::updatePlayer1Input(u32 delta_t_ms, bool& newMove){
		if (messageIndex >= 0){
			newMove = false;
		}
		else{
			bool playerIsPerformingMove = player1->getCurrentMove() != NULL;
			FK_SceneGame::updatePlayer1Input(delta_t_ms, newMove);
			if (newMove) {
				player1MoveToCheck = player1->getCurrentMove()->getName();
			}
			//if (newMove || playerIsPerformingMove) {
			//	std::cout << player1->getCurrentMove()->getName() << std::endl;
			//}
			/* specific move counter */
			if (getCurrentPhase().phase == FK_TutorialPhase::Backstep ||
				getCurrentPhase().phase == FK_TutorialPhase::TriggerSpecialMove){
				if (newMove && player1->getCurrentMove()->getName() == getCurrentPhase().additionalStringParameters[0]){
					playerSpecificMoveCount += 1;
					getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
				}
			}
			else if (getCurrentPhase().phase == FK_TutorialPhase::Ukemi || 
				getCurrentPhase().phase == TriggerReversal){
				if (newMove && player1->getCurrentMove()->getName() == getCurrentPhase().additionalStringParameters[
					getCurrentPhase().numberOfStringParameters - 1]){
					playerSpecificMoveCount += 1;
					getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
				}
			}
		}
	}

	void FK_SceneGameTutorial::updatePlayer2Input(u32 delta_t_ms, bool& newMove){
		if (messageIndex >= 0){
			newMove = false;
			return;
		}
		bool ukemiFlag = 
			getCurrentPhase().phase == FK_TutorialPhase::Ukemi && !player1->isGrounded();
		ukemiFlag |= getCurrentPhase().phase == FK_TutorialPhase::TriggerReversal && !player1->isGrounded();
		s32 moveArraySize = (s32)getCurrentPhase().numberOfStringParameters;
		if (ukemiFlag){
			moveArraySize = (s32)std::floor(getCurrentPhase().additionalNumericParameters[3]);
		}
		if (opponentHasToAct()){
			f32 distance = player1->getAnimatedMesh()->getAbsolutePosition().getDistanceFrom(
				player2->getAnimatedMesh()->getAbsolutePosition());
			opponentMoveTimer -= (s32)delta_t_ms;
			if (!player2->isHitStun() && (distance <= getCurrentPhase().additionalNumericParameters[0] || opponentAttackIndex > 0)){
				if (opponentMoveTimer < 0){
					if (player2->getCurrentMove() == NULL){
						currentOpponentMove = std::string();
						if (opponentAttackIndex >= moveArraySize){
							opponentAttackIndex = 0;
							opponentMoveTimer = (s32)getCurrentPhase().additionalNumericParameters[1];
						}
						else{
							if (opponentAttackIndex == 0){
								parriedMoves.clear();
								for (s32 i = 0; i < moveArraySize; ++i){
									parriedMoves.push_back(std::string());
								}
							}
							auto moves = player2->getMovesCollection();
							for (s32 i = 0; i < (s32)moves.size(); ++i){
								if (moves[i].getName() == getCurrentPhase().additionalStringParameters[opponentAttackIndex]){
									currentOpponentMove = getCurrentPhase().additionalStringParameters[opponentAttackIndex];
									player2->performMove(&player2->getMovesCollection()[i], true);
									opponentAttackIndex += 1;
									newMove = true;
									return;
								}
							}
						}
					}
					newMove = false;
				}
			}
			else{
				newMove = false;
			}
		}
		else{
			newMove = false;
		}
	}

	bool FK_SceneGameTutorial::opponentHasToAct(){
		FK_TutorialPhase phase = getCurrentPhase().phase;
		bool phaseFlag = (phase == FK_TutorialPhase::CrouchedGuard ||
			phase == FK_TutorialPhase::StandingGuard ||
			phase == FK_TutorialPhase::TechThrow ||
			phase == FK_TutorialPhase::TriggerGuard);
		bool ukemiFlag = 
			(phase == FK_TutorialPhase::Ukemi ||
				phase == FK_TutorialPhase::TriggerReversal) && !player1->isGrounded();
		return (phaseFlag || ukemiFlag);
	}

	void FK_SceneGameTutorial::setAdditionalLogicTriggers(){
		crouchingTimer = 0;
		numberOfJumps = 0;
		runningTimer = 0;
		numberOfHitsOpponent = 0;
		playerSpecificMoveCount = 0;
		currentOpponentMove = std::string();
		oldOpponentMove = std::string();
		opponentAttackIndex = 0;
		numberOfHitsGuarded = 0;
		opponentAttackIndex = 0;
		numberOfEvadedThrows = 0;
		opponentMoveTimer = 0;
		parriedMoves.clear();
		if (opponentHasToAct()){
			opponentMoveTimer = (s32)getCurrentPhase().additionalNumericParameters[1];
			for (u32 i = 0; i < (s32)getCurrentPhase().numberOfStringParameters; ++i){
				parriedMoves.push_back(std::string());
			}
		}
		numberOfMovesToPerform = getCurrentPhase().numberOfNumericParameters;
	}

	void FK_SceneGameTutorial::updateLogicTriggers(u32 delta_t_ms){
		/* collectibles */
		updateCollectibles();
		/* check crouching*/
		if (player1->getStance() == FK_Stance_Type::CrouchedStance){
			crouchingTimer += delta_t_ms;
		}
		else{
			crouchingTimer = 0;
		}
		/* jump */
		FK_TutorialTrainingSet trainingSet = getCurrentPhase();
		if (player1->isJumping() && !playerJumpStatus){
			playerJumpStatus = true;
			u32 button = player1input->getPressedButtons() & FK_Input_Buttons::Any_Direction;
			if (trainingSet.phase == FK_TutorialPhase::Jumping){
				if (trainingSet.additionalStringParameters[0] == "stand" &&
					button == FK_Input_Buttons::Up_Direction){
					numberOfJumps += 1;
				}
				else if (trainingSet.additionalStringParameters[0] == "forward" &&
					((player1->isOnLeftSide() && (button & FK_Input_Buttons::Right_Direction) != 0) ||
					(!player1->isOnLeftSide() && (button & FK_Input_Buttons::Left_Direction) != 0))){
					numberOfJumps += 1;
				}
				else if (trainingSet.additionalStringParameters[0] == "back" &&
					((!player1->isOnLeftSide() && (button & FK_Input_Buttons::Right_Direction) != 0) ||
					(player1->isOnLeftSide() && (button & FK_Input_Buttons::Left_Direction) != 0))){
					numberOfJumps += 1;
				}
			}
			
		}
		else if(playerJumpStatus && !player1->isJumping()){
			playerJumpStatus = false;
		}
		/* run */
		if (player1->getStance() == FK_Stance_Type::RunningStance){
			runningTimer += delta_t_ms;
		}
		else{
			runningTimer = 0;
		}
		/* hit */
		if (!player1->isAirborne() && player1->getCurrentMove() == NULL && player1->getNextMove() == NULL && !isProcessingThrow()){
			if (trainingSet.phase == FK_TutorialPhase::FlowCombo || trainingSet.phase == FK_TutorialPhase::Trigger){
				if (player1->getBulletsCollection().empty()){
					if (playerSpecificMoveCount > 0 && !getCurrentPhase().phaseComplete) {
						getSoundManager()->playSound("wrong", 100.0 * gameOptions->getSFXVolume());
					}
					playerSpecificMoveCount = 0;
					player2->setLifeToMax();
				}
			}
			if (trainingSet.phase == FK_TutorialPhase::Trigger){
				if (!player1->isTriggerModeActive() && player1->getBulletsCollection().empty()){
					if (playerSpecificMoveCount > 0 && !getCurrentPhase().phaseComplete) {
						getSoundManager()->playSound("wrong", 100.0 * gameOptions->getSFXVolume());
					}
					playerSpecificMoveCount = 0;
					player2->setLifeToMax();
				}
			}
		}
		// trigger guard
		if (trainingSet.phase == FK_TutorialPhase::TriggerGuard){
			if (!previousOpponentHitStun && player2->isHitStun() && player1->isGuarding()){
				numberOfHitsOpponent += 1;
			}
		}
		if (player2->getLife() < previousOpponentLife && previousOpponentLife >= 0){
			opponentAttackIndex = 0;
			if (trainingSet.phase == FK_TutorialPhase::Punch){
				if (player1->getCurrentMove() != NULL){
					u32 button = player1->getCurrentMove()->getInputStringLeft()[
						player1->getCurrentMove()->getInputStringLeft().size() - 1];
					if ((button & FK_Input_Buttons::Any_NonDirectionButton) == FK_Input_Buttons::Punch_Button){
						numberOfHitsOpponent += 1;
					}
				}
			}
			else if (trainingSet.phase == FK_TutorialPhase::Kick){
				if (player1->getCurrentMove() != NULL){
					u32 button = player1->getCurrentMove()->getInputStringLeft()[
						player1->getCurrentMove()->getInputStringLeft().size() - 1];
					if ((button & FK_Input_Buttons::Any_NonDirectionButton) == FK_Input_Buttons::Kick_Button){
						numberOfHitsOpponent += 1;
					}
				}
			}
			else if (trainingSet.phase == FK_TutorialPhase::SpecificMove){
				if (player1->getCurrentMove() != NULL){
					std::string name = player1->getCurrentMove()->getName();
					if (name == trainingSet.additionalStringParameters[0]){
						playerSpecificMoveCount += 1;
						getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
					}
				}
			}
			else if (trainingSet.phase == FK_TutorialPhase::FlowCombo){
				if (player1->getCurrentMove() != NULL){
					//std::string name = player1->getCurrentMove()->getName();
					u32 moveId = player2->getLastDamagingMoveId();
					if (playerSpecificMoveCount < numberOfMovesToPerform){
						//std::cout << name << " " << trainingSet.additionalStringParameters[playerSpecificMoveCount] << std::endl;
					}
					if (playerSpecificMoveCount >= numberOfMovesToPerform ||
						moveId == trainingSet.additionalNumericParameters[playerSpecificMoveCount]){
						//name == trainingSet.additionalStringParameters[playerSpecificMoveCount]){
						playerSpecificMoveCount += 1;
						getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
						//std::cout << name << " ok!" << std::endl;
					}
					else if (playerSpecificMoveCount > 0 && 
						moveId == trainingSet.additionalNumericParameters[playerSpecificMoveCount - 1]){
						//getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
						//std::cout << name << " repeated but ok!" << std::endl;
					}
					else{
						if (playerSpecificMoveCount >= 0) {
							getSoundManager()->playSound("wrong", 100.0 * gameOptions->getSFXVolume());
						}
						playerSpecificMoveCount = 0;
					}
				}
				else if (player1->getNextMove() == NULL){
					if ((s32)playerSpecificMoveCount < (s32)(numberOfMovesToPerform - 1)) {
						getSoundManager()->playSound("wrong", 100.0 * gameOptions->getSFXVolume());
					}
					//playerSpecificMoveCount = 0;
					//std::cout << "player1 - no available move" << std::endl;
					if (playerSpecificMoveCount < trainingSet.numberOfNumericParameters){
						//std::cout << lastMoveName << " " << trainingSet.additionalStringParameters[playerSpecificMoveCount] << std::endl;
					}
					if (playerSpecificMoveCount >= numberOfMovesToPerform ||
						lastMoveId == trainingSet.additionalNumericParameters[playerSpecificMoveCount]) {
						//lastMoveName == trainingSet.additionalStringParameters[playerSpecificMoveCount]){
						playerSpecificMoveCount += 1;
						getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
						//std::cout << lastMoveName << " ok!" << std::endl;
					}
					else{
						if ((s32)playerSpecificMoveCount < (s32)(numberOfMovesToPerform - 1)) {
							getSoundManager()->playSound("wrong", 100.0 * gameOptions->getSFXVolume());
						}
						playerSpecificMoveCount = 0;
					}
				}
			}
			else if (trainingSet.phase == FK_TutorialPhase::Trigger || (trainingSet.phase == FK_TutorialPhase::TriggerRunCancel &&
				!trainingSet.additionalStringParameters.empty())){
				if (player1->getCurrentMove() != NULL){
					bool correctModeOn = (trainingSet.phase == FK_TutorialPhase::Trigger && player1->isTriggerModeActive()) ||
						(trainingSet.phase == FK_TutorialPhase::TriggerRunCancel && player1->isInImpactCancelState());
					std::string name = player1->getCurrentMove()->getName();
					if (playerSpecificMoveCount < numberOfMovesToPerform){
						//std::cout << name << " " << trainingSet.additionalStringParameters[playerSpecificMoveCount] << std::endl;
					}
					if ((playerSpecificMoveCount >= numberOfMovesToPerform ||
						name == trainingSet.additionalStringParameters[playerSpecificMoveCount]) &&
						correctModeOn){
						playerSpecificMoveCount += 1;
						getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
						//std::cout << name << " ok!" << std::endl;
					}
					else if (correctModeOn && (playerSpecificMoveCount > 0 &&
						name == trainingSet.additionalStringParameters[playerSpecificMoveCount - 1])){
						//std::cout << name << " repeated but ok!" << std::endl;
						getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
					}
					else{
						if (playerSpecificMoveCount >= 0) {
							if (!(playerSpecificMoveCount == 0 && 
								trainingSet.phase == FK_TutorialPhase::TriggerRunCancel && 
								!player1->isInImpactCancelState())) {
								getSoundManager()->playSound("wrong", 100.0 * gameOptions->getSFXVolume());
							}
						}
						playerSpecificMoveCount = 0;
					}
				}
				else{
					if (playerSpecificMoveCount < numberOfMovesToPerform){
						std::cout << lastMoveName << " " << trainingSet.additionalStringParameters[playerSpecificMoveCount] << std::endl;
					}
					if (playerSpecificMoveCount >= numberOfMovesToPerform ||
						lastMoveId == trainingSet.additionalNumericParameters[playerSpecificMoveCount]){
						playerSpecificMoveCount += 1;
						getSoundManager()->playSound("correct", 100.0 * gameOptions->getSFXVolume());
						//std::cout << lastMoveName << " ok!" << std::endl;
					}
					else{
						playerSpecificMoveCount = 0;
					}
				}
			}
		}
		/* guard */
		if (previousPlayerHitStun == false && player1->isHitStun()){
			if (player1->isGuarding() && previousPlayerLife == player1->getLife()){
				numberOfHitsGuarded += 1;
				parriedMoves.pop_front();
				parriedMoves.push_back(currentOpponentMove);
			}
			else{
				numberOfHitsGuarded = 0;
			}
		}
		/* throw escape */
		oldPlayerStance = player1->getStance();
		previousOpponentLife = player2->getLife();
		previousOpponentHitStun = player2->isHitStun();
		previousPlayerHitStun = player1->isHitStun();
		previousPlayerLife = player1->getLife();
	}

	void FK_SceneGameTutorial::updateMessageInput(u32 delta_t_ms){
		u32 inputButtons = 0;
		u32 now = device->getTimer()->getTime();
		if (joystickInfo.size() > 0){
			player1input->update(now, inputReceiver->JoypadStatus(0), false);
		}
		else{
			player1input->update(now, inputReceiver->KeyboardStatus(), false);
		}
		inputButtons = player1input->getPressedButtons();
		inputButtons &= ~FK_Input_Buttons::Any_Direction;
		if (selection_timer_ms > 0){
			selection_timer_ms -= (s32)delta_t_ms;
		}else{
			if ((inputButtons & FK_Input_Buttons::Selection_Button) != 0){
				inputReceiver->reset();
				selection_timer_ms = FK_SceneGameTutorial::SelectionTimerBufferMS;
				if (!switchDialogue && (messageWindow->isAllTextDisplayed() && messageWindow->isActive())){
					getSoundManager()->playSound("cursor", 100.f * gameOptions->getSFXVolume());
					switchDialogue = true;
					canSkipDialogue = true;
					messageIndex += 1;
					if (messageIndex >= (s32)getCurrentPhase().messages.size()){
						if (!(getCurrentPhase().phase == FK_TutorialPhase::GenericText &&
							(phaseIndex < (s32)tutorialPhasesSize - 1))){
							messageWindow->close(300);
						}
						player1input->getBuffer().clear();
						messageIndex = -1;
						switchDialogue = false;
					}
					return;
				}
				else if (!messageWindow->isAllTextDisplayed() &&
					messageWindow->isActive() && messageWindow->isOpen()){
					canSkipDialogue = false;
					//soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
					messageWindow->setTextMode(FK_DialogueWindow::DialogueTextMode::AllTogether);
					return;
				}
			}
		}
	}

	void FK_SceneGameTutorial::updateAdditionalSceneLogic(u32 delta_t_ms){
		FK_SceneGameTraining::updateAdditionalSceneLogic(delta_t_ms);
		updateLogicTriggers(delta_t_ms);
		typingSoundTimerMS += delta_t_ms;
		if (messageWindow->isVisible()){
			messageWindow->update(delta_t_ms);
		}
		// store last performed move
		//if (player1->getCurrentMove() != NULL && player1->getCurrentMove()->getName() != lastMoveName){
		//	lastMoveName = player1->getCurrentMove()->getName();
		//}
		if (player1->getCurrentMove() != NULL && player1->getCurrentMove()->getMoveId() != lastMoveId){
			lastMoveId = player1->getCurrentMove()->getMoveId();
		}
		// remove triggers if not needed
		FK_TutorialTrainingSet tutorialPhase = getCurrentPhase();
		if (tutorialPhase.phase != FK_TutorialPhase::TriggerGuard && 
			tutorialPhase.phase != FK_TutorialPhase::FlowCombo){
			player2->useTriggerCounters(databaseAccessor.getMaxTriggers());
			player1->useTriggerCounters(databaseAccessor.getMaxTriggers());
			player1NumberOfBulletCounters = player1->getTriggerCounters();
			player2NumberOfBulletCounters = player2->getTriggerCounters();
		}
		if (tutorialPhase.phase == FK_TutorialPhase::Trigger ||
			tutorialPhase.phase == FK_TutorialPhase::TriggerRunCancel ||
			tutorialPhase.phase == FK_TutorialPhase::TriggerReversal ||
			tutorialPhase.phase == FK_TutorialPhase::TriggerSpecialMove){
			if (player1->getTriggerCounters() == 0 && (!player1->isTriggerModeActive() || player1->getCurrentMove() == NULL)){
				player1->addTriggerCounters(1);
				player1NumberOfBulletCounters = player1->getTriggerCounters();
				player2NumberOfBulletCounters = player2->getTriggerCounters();
			}
		}
		else if (tutorialPhase.phase == FK_TutorialPhase::FlowCombo) {
			player2->useTriggerCounters(databaseAccessor.getMaxTriggers());
			if (tutorialPhase.numberOfNumericParameters >
				tutorialPhase.numberOfStringParameters) {
				int numberOfTriggers = tutorialPhase.additionalNumericParameters[
					tutorialPhase.numberOfStringParameters];
				if (player1->getTriggerCounters() > numberOfTriggers) {
					player1->setTriggerCounters(numberOfTriggers);
				}
				else if (player1->getTriggerCounters() < numberOfTriggers && (!player1->isTriggerModeActive() &&
					player1->getCurrentMove() == NULL)) {
					player1->setTriggerCounters(numberOfTriggers);
				}
			}
			else {
				player1->useTriggerCounters(databaseAccessor.getMaxTriggers());
			}
			player1NumberOfBulletCounters = player1->getTriggerCounters();
			player2NumberOfBulletCounters = player2->getTriggerCounters();
		}
		if (tutorialPhase.phase != FK_TutorialPhase::Ringout && stage->allowsRingout()){
			stage->disallowRingout();
		}
		else if (tutorialPhase.phase == FK_TutorialPhase::Ringout && !stage->allowsRingout()){
			stage->allowRingout(true, true, true, true);
		}
		// check if new phase is open
		if (canPauseGame() && (tutorialPhase.phaseComplete || phaseIndex < 0) && 
			!player1->isTriggerModeActive() &&
			!player2->isTriggerModeActive() &&
			!player2->isBeingThrown() &&
			!player1->isBeingThrown() &&
			!player1->isAirborne() &&
			!player1->isRunning() &&
			player1->getCurrentMove() == NULL){
			phaseIndex += 1;
			tutorialPhase = getCurrentPhase();
			if (tutorialPhase.messages.size() > 0){
				messageIndex = 0;
				switchDialogue = true;
				canSkipDialogue = true;
			}
			if (tutorialPhase.phase == FK_TutorialPhase::CoinCollect ||
				tutorialPhase.phase == FK_TutorialPhase::CoinCollectCrouch) {
				createCollectibles();
			}
			setAdditionalLogicTriggers();
			if (phaseIndex >= (s32)tutorialPhasesSize){
				if (std::find(tutorialChallengesCleared.begin(), tutorialChallengesCleared.end(), tutorialDirectory) ==
					tutorialChallengesCleared.end()){
					tutorialChallengesCleared.push_back(tutorialDirectory);
				}
				writeSaveFile();
			}
		}
		if (messageIndex >= 0 && messageWindow->isActive()){
			updateMessageInput(delta_t_ms);
		}
		if (switchDialogue){
			if (messageWindow->isAnimating()){
				return;
			}
			if (messageIndex >= 0){
				f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
				std::string mugshotPath = charactersPath + playerPath;
				mugshotPath += "FightPreview.png";
				switchDialogue = false;
				messageWindow->setActive(true);
				messageWindow->setVisibility(true);
				messageWindow->reset(scaleFactor, mugshotPath, tutorialPhase.messages[messageIndex].dialogueText,
					FK_DialogueWindow::DialogueWindowAlignment::Left,
					FK_DialogueWindow::DialogueTextMode::CharByChar);
				if (messageWindow->isClosed()) messageWindow->open(100);
			}
		}
		if (messageIndex < 0){
			checkForPhaseCompletion();
		}
	}

	/* pause menu update */
	/* get pause menu items */
	std::vector<std::string> FK_SceneGameTutorial::getPauseMenuItems(){
		std::vector<std::string> menuItems = FK_SceneGame::getPauseMenuItems();
		if (challengeType == FK_SceneTrainingType::TrainingTrial) {
			menuItems[4] = "to Challenge Selection";
		}else{
			menuItems[4] = "to Tutorial Selection";
		}
		return menuItems;
	}

	void FK_SceneGameTutorial::markCurrentPhaseAsComplete(bool playSound){
		if (playSound && !tutorialPhases[phaseIndex].phaseComplete){
			getSoundManager()->playSound("confirm", 100.f * gameOptions->getSFXVolume(), 1.f);
		}
		tutorialPhases[phaseIndex].phaseComplete = true;
		lastMoveName = std::string();
		lastMoveId = 0;
	}

	void FK_SceneGameTutorial::checkForPhaseCompletion(){
		FK_TutorialTrainingSet trainingSet = getCurrentPhase();
		f32 distance = player1->getAnimatedMesh()->getAbsolutePosition().getDistanceFrom(
			player2->getAnimatedMesh()->getAbsolutePosition());
		if (trainingSet.phase == FK_TutorialPhase::GenericText){
			markCurrentPhaseAsComplete(false);
		}
		else if (trainingSet.phase == FK_TutorialPhase::MovementForward){
			if (distance <= tutorialPhases[phaseIndex].additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::MovementBackwards){
			if (distance >= tutorialPhases[phaseIndex].additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::MovementSidewards){
			f32 temp_ang = player2->getRotation().Y;
			core::vector2df normal1 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
			temp_ang = player1->getRotation().Y;
			core::vector2df normal2 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
			f32 mutualDirection = normal1.dotProduct(normal2);
			f32 targetDirection = trainingSet.additionalNumericParameters[0];
			f32 targetUncertainty = trainingSet.additionalNumericParameters[1];
			if (abs(mutualDirection - targetDirection) <= targetUncertainty){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Crouching){
			if ((f32)crouchingTimer >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//crouchingTimer = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Jumping){
			if ((f32)numberOfJumps >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//numberOfJumps = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Run){
			if ((f32)runningTimer >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//runningTimer = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::TriggerRunCancel) {
			if (player1->isInImpactCancelState() && trainingSet.additionalStringParameters.empty() &&
				(f32)runningTimer >= trainingSet.additionalNumericParameters[0]) {
				markCurrentPhaseAsComplete();
				//runningTimer = 0;
			}
			if (player1->isInImpactCancelState() && !trainingSet.additionalStringParameters.empty()) {
				if ((f32)playerSpecificMoveCount >= (f32)trainingSet.additionalStringParameters.size()) {
					markCurrentPhaseAsComplete();
				}
				//runningTimer = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Backstep){
			if ((f32)playerSpecificMoveCount >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//playerSpecificMoveCount = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Ukemi || 
			trainingSet.phase == FK_TutorialPhase::TriggerReversal){
			if ((f32)playerSpecificMoveCount >= trainingSet.additionalNumericParameters[2]){
				markCurrentPhaseAsComplete();
				//playerSpecificMoveCount = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Punch){
			if ((f32)numberOfHitsOpponent >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//numberOfHitsOpponent = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Kick){
			if ((f32)numberOfHitsOpponent >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//numberOfHitsOpponent = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::TriggerGuard){
			if ((f32)numberOfHitsOpponent >= trainingSet.additionalNumericParameters[2]){
				markCurrentPhaseAsComplete();
				//numberOfHitsOpponent = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::SpecificMove ||
			trainingSet.phase == FK_TutorialPhase::TriggerSpecialMove){
			if ((f32)playerSpecificMoveCount >= trainingSet.additionalNumericParameters[0]){
				markCurrentPhaseAsComplete();
				//playerSpecificMoveCount = 0;
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::FlowCombo){
			if ((f32)playerSpecificMoveCount >= (f32)trainingSet.additionalStringParameters.size()){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Trigger){
			if ((f32)playerSpecificMoveCount >= (f32)trainingSet.additionalStringParameters.size()){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::TechThrow){
			if ((f32)numberOfEvadedThrows >= (f32)trainingSet.additionalNumericParameters[2]){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::Ringout){
			if (player2->getRingoutFlag() && player2->isKO() && canPauseGame()){
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::CoinCollect ||
			getCurrentPhase().phase == FK_TutorialPhase::CoinCollectCrouch) {
			if (collectibles.empty() && collectiblesCreated) {
				collectiblesCreated = false;
				disposeCollectibles();
				markCurrentPhaseAsComplete();
			}
		}
		else if (trainingSet.phase == FK_TutorialPhase::StandingGuard || trainingSet.phase == FK_TutorialPhase::CrouchedGuard){
			bool equalBuffers = true;
			for (u32 i = 0; i < trainingSet.additionalStringParameters.size(); ++i){
				if (parriedMoves[i] != trainingSet.additionalStringParameters[i]){
					equalBuffers = false;
					break;
				}
			}
			if (equalBuffers){
				markCurrentPhaseAsComplete();
			}
		}
	}

	/* get scene name for loading screen */
	std::wstring FK_SceneGameTutorial::getSceneNameForLoadingScreen(){
		if (tutorialId.empty()){
			return L"Tutorial";
		}
		else{
			return fk_addons::convertNameToNonASCIIWstring(tutorialId);
		}
	}

	/* get challenge index */
	u32 FK_SceneGameTutorial::getChallengeIndex(){
		return challengeIndex;
	}
	FK_Scene::FK_SceneTrainingType FK_SceneGameTutorial::getChallengeType()
	{
		return challengeType;
	}

	/* collectibles */

	// create collectibles
	void FK_SceneGameTutorial::createCollectibles() {
		if (getCurrentPhase().phase != FK_TutorialPhase::CoinCollect &&
			getCurrentPhase().phase != FK_TutorialPhase::CoinCollectCrouch) {
			return;
		}
		
		for each(auto filename in getCurrentPhase().additionalStringParameters) {
			FK_Collectible* newCollectible = new FK_Collectible;
			newCollectible->meshFileName = filename;
			auto mesh = smgr->getMesh((tutorialResourcesPath + filename).c_str());
			newCollectible->node = smgr->addAnimatedMeshSceneNode(mesh);
			newCollectible->canBePickedByPlayer1 = true;
			newCollectible->canBePickedByPlayer2 = false;
			collectibles.push_back(newCollectible);
			newCollectible = NULL;
		}
		u32 csize = collectibles.size();
		for (u32 i = 0; i < csize; ++i) {
			f32 x = getCurrentPhase().additionalNumericParameters[3 * i];
			f32 y = getCurrentPhase().additionalNumericParameters[3 * i + 1];
			f32 z = getCurrentPhase().additionalNumericParameters[3 * i + 2];
			collectibles[i]->position = core::vector3df(x, y, z);
			collectibles[i]->node->setPosition(collectibles[i]->position);
			//collectibles[i]->node->setScale(core::vector3df(20.f, 20.f, 20.f));
			collectibles[i]->node->setMaterialFlag(video::EMF_LIGHTING, false);
			collectibles[i]->node->setJointMode(irr::scene::EJUOR_CONTROL);
			collectibles[i]->node->setAnimationSpeed(databaseAccessor.getAnimationFramerateFPS());
			int mcount = collectibles[i]->node->getMaterialCount();
			for (int i = 0; i < mcount; i++) {
				collectibles[i]->node->getMaterial(i).BackfaceCulling = false;
				collectibles[i]->node->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
				collectibles[i]->node->getMaterial(i).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			}
			lightManager->addShadowToNode(collectibles[i]->node, shadowFilterType, shadowModeCharacters);
			player1->activateParticleEffect(collectibles[i]->position);
		}
		collectiblesCreated = true;
	}

	// update collectibles
	void FK_SceneGameTutorial::updateCollectibles() {
		s32 collisionIndex1 = -1;
		s32 collisionIndex2 = -1;
		s32 csize = (s32)collectibles.size();
		for (s32 i = 0; i < csize; ++i) {
			FK_Collectible* collectible = collectibles[i];
			collectible->node->animateJoints();
			collectible->node->updateAbsolutePosition();
			if (getCurrentPhase().phase == FK_TutorialPhase::CoinCollectCrouch &&
				player1->getStance() != FK_Stance_Type::CrouchedStance) {
				continue;
			}
			if (collisionIndex1 < 0 && collectible->canBePickedByPlayer1) {
				if (checkNodeCollision(player1->getAnimatedMesh(), collectible->node)) {
					collisionIndex1 = i;
				}
			}
			if (collisionIndex2 < 0 && collisionIndex1 != i && collectible->canBePickedByPlayer2) {
				if (checkNodeCollision(player1->getAnimatedMesh(), collectible->node)) {
					collisionIndex2 = i;
				}
			}
		}
		// perform sound and animation
		if (collisionIndex1 >= 0) {
			getSoundManager()->playSound("collect", 100.0 * gameOptions->getSFXVolume());
			player1->activateParticleEffect(collectibles[collisionIndex1]->node->getPosition());
			lightManager->removeShadowFromNode(collectibles[collisionIndex1]->node);
			collectibles[collisionIndex1]->node->remove();
			delete collectibles[collisionIndex1];
			collectibles.erase(collectibles.begin() + collisionIndex1);
		}

		// perform sound and animation
		if (collisionIndex2 >= 0) {
			getSoundManager()->playSound("collect", 100.0 * gameOptions->getSFXVolume());
			player2->activateParticleEffect(collectibles[collisionIndex1]->node->getPosition());
			lightManager->removeShadowFromNode(collectibles[collisionIndex1]->node);
			collectibles[collisionIndex2]->node->remove();
			delete collectibles[collisionIndex2];
			collectibles.erase(collectibles.begin() + collisionIndex2);
		}
	}

	// dispose collectibles
	void FK_SceneGameTutorial::disposeCollectibles() {
		for each (auto collectible in collectibles) {
			lightManager->removeShadowFromNode(collectible->node);
			collectible->node->remove();
			delete collectible;
		}
		collectibles.clear();
	}

	/* produce proper win text line */
	std::string FK_SceneGameTutorial::getEndOfRoundSoundIdentifier(const s32& matchWinnerId, const bool& perfectFlag) {
		std::string soundIdentifier = "success";
		return soundIdentifier;
	}

	/* draw K.O. text */
	void FK_SceneGameTutorial::drawKOtext(const s32& matchWinnerId, const bool&perfectFlag) {
		hudManager->drawSuccessText();
	}

	///* always reset round in case of accidental K.O.*/
	void FK_SceneGameTutorial::processRoundEnd() {
		if (isProcessingRingout()) {
			FK_SceneGameTraining::processRoundEnd();
		}
		else {
			FK_SceneGame::processRoundEnd();
		}
	}

	/* always reset round in case of accidental K.O.*/
	void FK_SceneGameTutorial::processMatchEnd() {
		FK_SceneGame::processMatchEnd();
	}

	/* get round winner*/
	int FK_SceneGameTutorial::getRoundWinner(bool updateWins) {
		if (phaseIndex >= (s32)tutorialPhasesSize) {
			return -1;
		}
		else return FK_SceneGame::getRoundWinner(updateWins);
	}

	/* get match winner*/
	int FK_SceneGameTutorial::getMatchWinner() {
		if (phaseIndex >= (s32)tutorialPhasesSize) {
			return -1;
		}
		else return -99;
	}

	/* game logic (win, lose, et cetera)*/
	bool FK_SceneGameTutorial::isEndOfRound() {
		if (isProcessingRingout()) {
			return FK_SceneGame::isEndOfRound();
		}
		return !messageWindow->isAnimating() && phaseIndex >= (s32)tutorialPhasesSize;
	}

	/* end of round */
	bool FK_SceneGameTutorial::isEndOfMatch() {
		return isEndOfRound();
	}
}