/*
	*** Schwarzerblitz 3D Fighting Game Engine  ***

	=================== Source Code ===================
	Copyright (C) 2016-2022 Andrea Demetrio

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation and/or
	   other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may be
	   used to endorse or promote products derived from  this software without specific
	   prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	=============== Additional Components ==============
	Please refer to the license/irrlicht/ and license/SFML/ folder for the license
	indications concerning those components. The irrlicht-schwarzerlicht engine and
	the SFML code and binaries are subject to their own licenses, see the relevant
	folders for more information.

	=============== Assets and resources ================
	Unless specificed otherwise in the Credits file, the assets and resources
	bundled with this engine are to be considered "all rights reserved" and
	cannot be redistributed without the owner's consent. This includes but it is
	not limited to the characters concepts / designs, the 3D models, the music,
	the sound effects, 2D and 3D illustrations, stages, icons, menu art.

	Tutorial Man, Evil Tutor, and Random:
	Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

#include"FK_SceneGameArcade.h"

namespace fk_engine{
	// constructor (lazily based on the basic class constructor) 
	FK_SceneGameArcade::FK_SceneGameArcade() : FK_SceneGame(){
		arcadeRoundNumber = 0;
		arcadeLastMatch_flag = false;
		climaticBattle_flag = false;
		interfaceScrewRandomCounter = 0;
	};

	// setup
	void FK_SceneGameArcade::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		FK_Scene::FK_SceneArcadeType newArcadeType,
		std::string new_player1path,
		std::string new_player2path,
		FK_Character::FK_CharacterOutfit new_outfitPlayer1,
		FK_Character::FK_CharacterOutfit new_outfitPlayer2,
		std::string new_arenaPath,
		bool player1isPlaying,
		u32 AI_level,
		u32 matchNumber,
		FK_SceneGame::FK_AdditionalSceneGameOptions newAdditionalOptions,
		FK_ArcadeProgress currentArcadeProgress){
		arcadeType = newArcadeType;
		
		u32 roundsPerMatch = newOptions->getNumberOfRoundsArcade();
		u32 timerSeconds = newOptions->getTimerArcade();
		if (newAdditionalOptions.overrideTimer) {
			timerSeconds = newAdditionalOptions.roundTimerInSeconds;
		}
		resetRoundsForClimatic = false;
		if (newAdditionalOptions.numberOfRounds > 0){
			roundsPerMatch = (u32)newAdditionalOptions.numberOfRounds;
		}
		else{
			resetRoundsForClimatic = true;
		}
		arcadeRoundNumber = matchNumber;
		player1IsPlaying_flag = player1isPlaying;
		arcadeLastMatch_flag = false;
		climaticBattle_flag = false;
		forceNewRecordMessage = false;
		newAdditionalOptions.numberOfRounds = (s32)roundsPerMatch;
		newAdditionalOptions.roundTimerInSeconds = timerSeconds;
		arcadeProgress = currentArcadeProgress;
		//loadArcadeConfigFile();
		FK_SceneGame::setup(newDevice, new_joypadInfo, newOptions, new_player1path, new_player2path,
			new_outfitPlayer1, new_outfitPlayer2, new_arenaPath, FK_Scene::FK_SceneType::SceneGameArcade,
			newAdditionalOptions, !player1isPlaying, player1isPlaying, AI_level, AI_level);
		setupAdditionalParametersAfterLoading();
	}

	// load config file
	void FK_SceneGameArcade::loadArcadeConfigFile(){
		arcadeSettings.readFromFile(configurationFilesPath + fk_constants::FK_ArcadeConfigFileName, charactersPath);
		std::string playerPath = (player1isActivePlayer() ? player1_configPath : player2_configPath);
		std::string meshFolder = (player1isActivePlayer() ? player1Outfit.outfitDirectory : player2Outfit.outfitDirectory);
		std::ifstream flowFile(charactersPath + playerPath + meshFolder + fk_constants::FK_CharacterArcadeFolder +
			fk_constants::FK_CharacterArcadeFlowFileName);
		if (flowFile.good()){
			flowFile.clear();
			flowFile.close();
			characterArcadeFlow.readFromFile(charactersPath + playerPath + meshFolder +
				fk_constants::FK_CharacterArcadeFolder +
				fk_constants::FK_CharacterArcadeFlowFileName);
		}
		else{
			flowFile.clear();
			flowFile.close();
			characterArcadeFlow.readFromFile(charactersPath + playerPath + fk_constants::FK_CharacterArcadeFolder +
				fk_constants::FK_CharacterArcadeFlowFileName);
		}
	}

	// setup training variables
	void FK_SceneGameArcade::setupAdditionalVariables(){
		arcadeAddonsPath = commonResourcesPath + "arcade_addons\\";
		loadArcadeConfigFile();
		arcadeMatchTagline = std::wstring();
		if (!arcadeSettings.specialMatches.empty() &&
			arcadeSettings.allowSpecialMatches && 
			additionalOptions.allowArcadeSpecialMatches) {
			for each (FK_ArcadeSpecialMatch match in arcadeSettings.specialMatches) {
				if (arcadeRoundNumber == match.matchNumber && 
					match.condition.conditionIsFulfilled(
						arcadeProgress, 
						gameOptions->getAILevel(), 
						player1isAI ? player2_configPath : player1_configPath
					)) {
					currentSpecialMatch = match;
					if (player2isAI) {
						if (!match.matchConditions.getPlayerPath().empty()) {
							player2_configPath = match.matchConditions.getPlayerPath();
							player2Outfit = match.matchConditions.getPlayerOutfit();
						}
						player2AILevel = player2AILevel * match.matchConditions.getAILevel() / 100;
					}
					else if (player1isAI) {
						if (!match.matchConditions.getPlayerPath().empty()) {
							player1_configPath = match.matchConditions.getPlayerPath();
							player1Outfit = match.matchConditions.getPlayerOutfit();
						}
						player1AILevel = player2AILevel * match.matchConditions.getAILevel() / 100;
					}
					if (!match.matchConditions.getStagePath().empty()) {
						arena_configPath = match.matchConditions.getStagePath();
					}
					if (!match.matchTag.empty()) {
						arcadeMatchTagline = fk_addons::convertNameToNonASCIIWstring(match.matchTag);
					}
					if (!match.tagGainedOnWin.empty()) {
						arcadeProgress.specialTagsUnlocked.push_back(match.tagGainedOnWin);
					}
					if (!match.matchPreviewSplashScreenName.empty()) {
						arcadeSpecialMatchSplashscreenTexture = match.matchPreviewSplashScreenName;
					}
					break;
				}
			}
		}
		if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSurvival) {
			arcadeLastMatch_flag = arcadeRoundNumber > 2 * arcadeSettings.numberOfMatches + 1;
		}
		else {
			arcadeLastMatch_flag = arcadeRoundNumber > arcadeSettings.numberOfMatches;
		}
		climaticBattle_flag = characterArcadeFlow.hasClimaticBattle && arcadeSettings.allowClimaticBattle && 
			arcadeRoundNumber == arcadeSettings.numberOfMatches;
		if (/*arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSpecial2 ||
			arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSpecial3*/
			!additionalOptions.allowArcadeClimaticBattle){
			climaticBattle_flag = false;
		}
		//u32 minimumAILevelArcadeBoss = 6;
		if (arcadeLastMatch_flag){
			if (player1isActivePlayer()){
				player2LifeMultiplier *= arcadeSettings.bossCharacter1LifeMultiplier;
			}
			else{
				player1LifeMultiplier *= arcadeSettings.bossCharacter1LifeMultiplier;
			}
		}
		if (climaticBattle_flag){
			if (arcadeSettings.climaticBattleNumberOfRounds > 0){
				if (resetRoundsForClimatic){
					numberOfRounds = arcadeSettings.climaticBattleNumberOfRounds;
					numberOfRoundsPlayer1 = std::ceil((numberOfRounds + 1) / 2);
					numberOfRoundsPlayer2 = std::ceil((numberOfRounds + 1) / 2);
				}
			}
			if (arcadeSettings.climaticBattleTimer > 0){
				startRoundTimerMs = arcadeSettings.climaticBattleTimer * 1000;
			}
			roundTimerMs = startRoundTimerMs;
			player1LifeMultiplier *= arcadeSettings.climaticBattleLifeMultiplier;
			player2LifeMultiplier *= arcadeSettings.climaticBattleLifeMultiplier;
		}
	}

	// setup players intro and win quotes for climatic battle (if any)
	void FK_SceneGameArcade::setupAdditionalParametersAfterLoading(){
		if (climaticBattle_flag){
			stage->disallowRingout();
			FK_Character* activePlayer = player1isActivePlayer() ? player1 : player2;
			FK_Character* opponent = player1isActivePlayer() ? player2 : player1;
			if (!characterArcadeFlow.climaticBattleIntroQuote.empty()){
				std::vector<std::wstring> quotes;
				std::string line = characterArcadeFlow.climaticBattleIntroQuote;
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				quotes.push_back(tempLine);
				activePlayer->overrideIntroQuotes(quotes, opponent->getName());
			}
			if (!characterArcadeFlow.climaticBattleIntroQuoteOpponent.empty()){
				std::vector<std::wstring> quotes;
				std::string line = characterArcadeFlow.climaticBattleIntroQuoteOpponent;
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				quotes.push_back(tempLine);
				opponent->overrideIntroQuotes(quotes, activePlayer->getName());
			}
			if (!characterArcadeFlow.climaticBattleWinQuote.empty()){
				std::vector<std::wstring> quotes;
				std::string line = characterArcadeFlow.climaticBattleWinQuote;
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				quotes.push_back(tempLine);
				activePlayer->overrideMatchWinQuotes(quotes, opponent->getName());
				activePlayer->overrideRoundWinQuotes(quotes, opponent->getName());
			}
			if (!characterArcadeFlow.climaticBattleWinQuoteOpponent.empty()){
				std::vector<std::wstring> quotes;
				std::string line = characterArcadeFlow.climaticBattleWinQuoteOpponent;
				std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
				quotes.push_back(tempLine);
				opponent->overrideMatchWinQuotes(quotes, activePlayer->getName());
				opponent->overrideRoundWinQuotes(quotes, activePlayer->getName());
			}
			activePlayer = NULL;
			opponent = NULL;
		}
	}

	// update scene arcade logic
	void FK_SceneGameArcade::updateAdditionalSceneLogic(u32 delta_t_ms){
		updateVisualEffectsLastMatch(delta_t_ms);
		// add some debug options
#ifdef _DEBUG
		if (inputReceiver->IsKeyDown(EKEY_CODE::KEY_F4)) {
			FK_Character* testSubject = (player2isAI ? player2 : player1);
			testSubject->receiveDamage(2500, true);
			testSubject->setReaction(FK_Reaction_Type::StrongFlight, false, -1);
		}
		else if (inputReceiver->IsKeyDown(EKEY_CODE::KEY_F5)) {
			FK_Character* testSubject = (player2isAI ? player2 : player1);
			ISceneNodeAnimator* pAnim2 = *(testSubject->getAnimatedMesh()->getAnimators().begin());
			if (pAnim2->getType() == ESNAT_COLLISION_RESPONSE)
			{
				core::vector3df startingPosition = core::vector3df(0.f, -120.f, 0.f);
				testSubject->getAnimatedMesh()->removeAnimator(pAnim2);
				testSubject->setPosition(startingPosition);
				testSubject->getAnimatedMesh()->updateAbsolutePosition();
				setupCharacterArenaInteraction();
			}
		}
#endif // DEBUG
	}

	// update interface screw effect on last arcade match
	void FK_SceneGameArcade::updateVisualEffectsLastMatch(u32 delta_t_ms){
		if (arcadeLastMatch_flag){
			if (!isUpdatingImpactEffect() && canPauseGame()){
				interfaceScrewRandomCounter += delta_t_ms;
				u32 interfaceScrewMinimumPeriod = 5000;
				u32 interfaceScrewMaximumPeriod = 10000;
				if (interfaceScrewRandomCounter > interfaceScrewMinimumPeriod){
					double random = (double)interfaceScrewMaximumPeriod * (double)rand() / RAND_MAX;
					if (random < interfaceScrewRandomCounter){
						interfaceScrewRandomCounter = 0;
						FK_SceneGame::setupImpactEffect(FK_Reaction_Type::StrongFlight);
					}
				}
			}
		}
	}

	void FK_SceneGameArcade::updateRoundTimer(u32 delta_t_ms)
	{
		FK_SceneGame::updateRoundTimer(delta_t_ms);
		arcadeProgress.totalPlayTimeMs += delta_t_ms;
	}

	/* save game on game over for survival mode*/
	void FK_SceneGameArcade::processAdditionalGameOverRoutine()
	{
		// update save file data for survival mode
		if (arcadeType == FK_SceneArcadeType::ArcadeSurvival) {
			if (survivalRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = 0;
			}
			if (getArcadeMatchNumber() - 1 > survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second) {
				survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = getArcadeMatchNumber() - 1;
				std::string playerPath = player1isActivePlayer() ?
					player1_configPath + player1Outfit.outfitDirectory :
					player2_configPath + player2Outfit.outfitDirectory;
				survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].first = playerPath;
				forceNewRecordMessage = true;
			}
		}
		writeSaveFile();
	}


	// draw additional HUD items
	void FK_SceneGameArcade::drawAdditionalHUDItems(f32 delta_t_s){
		// draw match number for arcade mode 
		//TODO!
		u32 timeElapsed = getElapsedTimeMS();
		if (!isProcessingIntro() && arcadeType == FK_SceneArcadeType::ArcadeTimeAttack) {
			if (timeAttackRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()] =
					std::pair<std::string, u32> (std::string(), (356400000 + 3540000 + 59000 + 999));
			}
			hudManager->drawTimeElapsed(arcadeProgress.totalPlayTimeMs, timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].second);
		}
		else if (!isProcessingIntro() && arcadeType == FK_SceneArcadeType::ArcadeSurvival) {
			if (survivalRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = 0;
			}
			u32 survivalRecord = survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second;
			u32 score = getArcadeMatchNumber() - 1;
			if (isEndOfMatch() && (player1isAI && winnerId == 1) || (player2isAI && winnerId == -1)) {
				score = getArcadeMatchNumber();
			}
			if (score > survivalRecord) {
				survivalRecord = score;
			}
			hudManager->drawSurvivalRecord(score, survivalRecord);
		}
		if (showHUD()){
			if (climaticBattle_flag && !(player1->isKO() || player2->isKO())){
				if (!characterArcadeFlow.dialogueLines.empty()){
					for each(auto dialogue in characterArcadeFlow.dialogueLines){
						if (timeElapsed >= dialogue.timeStart && timeElapsed < dialogue.timeEnd){
							hudManager->drawMatchSentence(dialogue.speakerName, dialogue.sentence);
							break;
						}
					}
				}
			}
		}
	}

	// impact effect (every time the boss hits you, obviously)
	void FK_SceneGameArcade::setupImpactEffect(FK_Reaction_Type reaction, s32 playerId){
		if (arcadeLastMatch_flag && (
			(player1IsPlaying_flag && playerId == 1) || (!player1IsPlaying_flag && playerId == 2))){
				FK_SceneGame::setupImpactEffect(FK_Reaction_Type::StrongFlight);
		}
		else{
			FK_SceneGame::setupImpactEffect(reaction);
		}
	}

	/* get scene name for loading screen */
	std::wstring FK_SceneGameArcade::getSceneNameForLoadingScreen(){
		if (!arcadeMatchTagline.empty()) {
			return arcadeMatchTagline;
		}
		else if (arcadeLastMatch_flag){
			std::string arcadeLastMatchTagline = arcadeSettings.bossCharacter1LoadingScreenTagline;
			return std::wstring(arcadeLastMatchTagline.begin(), arcadeLastMatchTagline.end());
		}
		else if (climaticBattle_flag){
			std::string matchTagline = arcadeSettings.climaticBattleDefaultTagline;
			return std::wstring(matchTagline.begin(), matchTagline.end());
		}else{
			return L"Match #" + std::to_wstring(arcadeRoundNumber);
		}
	}

	// draw loading screen
	void FK_SceneGameArcade::drawLoadingScreen(f32 completionPercentage){
		if (arcadeLastMatch_flag || !arcadeSpecialMatchSplashscreenTexture.empty()){
			drawFinalMatchLoadingScreen(completionPercentage);
		}
		else{
			FK_SceneGame::drawLoadingScreen(completionPercentage);
		}
	}

	//! load loading textures
	void FK_SceneGameArcade::loadLoadingTextures(){
		FK_SceneGame::loadLoadingTextures();
		if (!arcadeSpecialMatchSplashscreenTexture.empty()) {
			lastRoundLoadingTex = driver->getTexture((arcadeAddonsPath + arcadeSpecialMatchSplashscreenTexture).c_str());
			if (!lastRoundLoadingTex) {
				arcadeSpecialMatchSplashscreenTexture = std::string();
			}
		}
		if (arcadeLastMatch_flag && !lastRoundLoadingTex) {
			std::string arcadeLastMatchSplashScreenFileName = arcadeSettings.bossCharacter1LoadingScreenFilename;
			std::string path = player1IsPlaying_flag ? player1_configPath : player2_configPath;
			std::string outfitPath = player1IsPlaying_flag ? player1Outfit.outfitDirectory : player2Outfit.outfitDirectory;
			std::string pathToCheck = charactersPath + "\\" + path + "\\" + outfitPath + fk_constants::FK_CharacterArcadeFolder;
			lastRoundLoadingTex = driver->getTexture((pathToCheck + arcadeLastMatchSplashScreenFileName).c_str());
			if (!lastRoundLoadingTex) {
				pathToCheck = charactersPath + "\\" + path + fk_constants::FK_CharacterArcadeFolder;
				lastRoundLoadingTex = driver->getTexture((pathToCheck + arcadeLastMatchSplashScreenFileName).c_str());
			}
			if (!lastRoundLoadingTex) {
				lastRoundLoadingTex = driver->getTexture((arcadeAddonsPath + arcadeLastMatchSplashScreenFileName).c_str());
			}
		}
	}

	//! draw loading screen
	void FK_SceneGameArcade::drawFinalMatchLoadingScreen(f32 completionPercentage){
		// get arena background for loading screen
		core::dimension2d<u32> texSize = lastRoundLoadingTex->getSize();
		// draw preview (fullscreen)
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// create color array for background
		video::SColor const color_bck = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors_bck[4] = {
			color_bck,
			color_bck,
			color_bck,
			color_bck
		};
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		// draw stage picture
		driver->draw2DImage(lastRoundLoadingTex, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height), sourceRect, 0, vertexColors_bck);
		// draw overlay
		FK_SceneGame::drawLoadingScreenProgressBar(completionPercentage);
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}

	/* fill the "check for new record" method */
	bool FK_SceneGameArcade::checkForNewRecord() {
		if (!isEndOfMatch()) {
			return false;
		}
		if (forceNewRecordMessage) {
			forceNewRecordMessage = false;
			return true;
		}
		if ((((player1isAI && winnerId == 1) || (player2isAI && winnerId == -1) &&
			arcadeLastMatch_flag) || isGameOver) && 
			arcadeType == FK_SceneArcadeType::ArcadeSurvival) {
			if (survivalRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = 0;
			}
			u32 score = isGameOver ? (getArcadeMatchNumber() - 1) : getArcadeMatchNumber();
			if (score > survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second) {
				return true;
			}
		}
		if (((player1isAI && winnerId == 1) || (player2isAI && winnerId == -1)) &&
			arcadeLastMatch_flag &&
			arcadeType == FK_SceneArcadeType::ArcadeTimeAttack) {
			if (timeAttackRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
				timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()] =
					std::pair<std::string, u32>(std::string(), (356400000 + 3540000 + 59000 + 999));
			}
			if (arcadeProgress.totalPlayTimeMs < timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].second) {
				return true;
			}
		}
		return false;
	}

	/* setup "continue" if needed */
	void FK_SceneGameArcade::processMatchEnd(){
		//arcadeProgress.totalPlayTimeMs += totalMatchTimeMs;
		processCharacterStats();
		arcadeProgress.playerEnergyAtEndOfMatch = player1isAI ? player2->getLifeRatio() : player1->getLifeRatio();
		if ((winnerId == 1 && player2isAI) ||
			(winnerId == -1 && player1isAI)
			){
			setupContinue();
			arcadeProgress.numberOfContinues += 1;
		}
		else{
			if (player2isAI) {
				arcadeProgress.numberOfRoundsLost += matchStatistics.getNumberOfRoundWins(1);
				arcadeProgress.numberOfPerfectRoundsWon += matchStatistics.getNumberOfPerfectVictories(-1);
				arcadeProgress.numberOfRingouts += matchStatistics.getNumberOfRingoutVictories(-1);
			}
			else {
				arcadeProgress.numberOfRoundsLost += matchStatistics.getNumberOfRoundWins(-1);
				arcadeProgress.numberOfPerfectRoundsWon += matchStatistics.getNumberOfPerfectVictories(1);
				arcadeProgress.numberOfRingouts += matchStatistics.getNumberOfRingoutVictories(1);
			}
			processingSceneEnd = true;
			FK_SceneGameArcade::updateSaveFileData();
			if (arcadeLastMatch_flag){
				u32 extraEndingInput = player1input->getPressedButtons();
				std::string player_configPath = player1_configPath;
				auto playerOutfit = player1Outfit;
				if (player1isAI) {
					extraEndingInput = player2input->getPressedButtons();
					player_configPath = player2_configPath;
					playerOutfit = player2Outfit;
				}
				extraEndingInput &= FK_Input_Buttons::Trigger_Button;
				// play extra ending if Trigger input is pressed before the scene
				bool validEndingFile = false;
				std::string endingFilePath;
				std::string relativePath;
				std::string arcadeEndingConfigFileName;
				if (additionalOptions.playArcadeEnding) {
					std::string characterFullPath = charactersPath + player_configPath;
					bool isWorkshopCharacter = false;
					if (!isValidCharacterPath(characterFullPath) && isValidCharacterPath(player_configPath)) {
						characterFullPath = player_configPath;
					}
					if (extraEndingInput != 0) {
						endingFilePath =
							characterFullPath + playerOutfit.outfitDirectory +
							fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeAlternateEndingFileName;
						std::ifstream inputFile(endingFilePath.c_str());
						relativePath = player_configPath + playerOutfit.outfitDirectory;
						if (!inputFile) {
							inputFile.clear();
							inputFile.close();
							endingFilePath =
								characterFullPath +
								fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeAlternateEndingFileName;
							relativePath = player_configPath;
							inputFile = std::ifstream(endingFilePath.c_str());
						}
						if (inputFile.good()) {
							validEndingFile = true;
							arcadeEndingConfigFileName = fk_constants::FK_CharacterArcadeAlternateEndingFileName;
						}
					}
					if (!validEndingFile) {
						endingFilePath =
							characterFullPath + playerOutfit.outfitDirectory +
							fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeEndingFileName;
						std::ifstream inputFile(endingFilePath.c_str());
						relativePath = player_configPath + playerOutfit.outfitDirectory;
						if (!inputFile) {
							inputFile.clear();
							inputFile.close();
							endingFilePath =
								characterFullPath +
								fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeEndingFileName;
							relativePath = player_configPath;
							inputFile = std::ifstream(endingFilePath.c_str());
						}
						if (inputFile.good()) {
							validEndingFile = true;
							arcadeEndingConfigFileName = fk_constants::FK_CharacterArcadeEndingFileName;
						}
					}
				}
				if (!validEndingFile) {
					setNextScene(FK_Scene::FK_SceneType::SceneCredits);
				}else{
					//inputFile.close();
					//inputFile.clear();
					arcadeEnding.setup(arcadeEndingConfigFileName,
						charactersPath + relativePath + fk_constants::FK_CharacterArcadeFolder, relativePath);
					arcadeEnding.playCredits = true;
					arcadeEnding.allowMenu = true;
					arcadeEnding.saveStoryCompletion = true;
					arcadeEnding.saveWhenSkipped = true;
					arcadeEnding.fadeWhenEnding = false;
					setNextScene(FK_Scene::FK_SceneType::SceneArcadeCutscene);
				}
			}
			else{
				setNextScene(nameId());
			}
			abortMatch = true;
		}
	}

	/* set arcade match index */
	void FK_SceneGameArcade::setArcadeMatchNumber(u32 matchIndex){
		arcadeRoundNumber = matchIndex;
	}

	/* get arcade match index */
	u32 FK_SceneGameArcade::getArcadeMatchNumber(){
		return arcadeRoundNumber;
	}

	/* set match as arcade last match */
	void FK_SceneGameArcade::setArcadeLastMatch(){
		arcadeLastMatch_flag = true;
	}

	// check if player 1 is the active player
	bool FK_SceneGameArcade::player1isActivePlayer(){
		return player1IsPlaying_flag;
	}

	/* change conditions for win text showing */
	bool FK_SceneGameArcade::showWinText(){
		bool continueFlag = FK_SceneGame::showContinueText();
		bool winFlag = FK_SceneGame::showWinText();
		return (!continueFlag && (winFlag || processingSceneEnd));
	}

	/* update save file data */
	void FK_SceneGameArcade::updateSaveFileData(){
		if (arcadeLastMatch_flag){
			for each (std::string tag in arcadeProgress.specialTagsUnlocked) {
				if (std::find(specialTagsAcquired.begin(), specialTagsAcquired.end(), tag) ==
					specialTagsAcquired.end()) {
					specialTagsAcquired.push_back(tag);
				}
			}
			// update arcade mode achievements
			processArcadeAchievements(arcadeType);
			// update save file data (that is, add 1 at the number of beaten arcade modes, in this case)
			if (arcadeType == FK_SceneArcadeType::ArcadeSurvival) {
				if (survivalRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
					survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = 0;
				}
				if (getArcadeMatchNumber() > survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second) {
					survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].second = getArcadeMatchNumber();
					std::string playerPath = player1isActivePlayer() ?
						player1_configPath + player1Outfit.outfitDirectory :
						player2_configPath + player2Outfit.outfitDirectory;
					survivalRecordPerDifficultyLevel[gameOptions->getAILevel()].first = playerPath;
				}
			}
			else if (arcadeType == FK_SceneArcadeType::ArcadeTimeAttack) {
				if (timeAttackRecordPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
					timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()] =
						std::pair<std::string, u32>(std::string(), (356400000 + 3540000 + 59000 + 999));
				}
				if (arcadeProgress.totalPlayTimeMs < 
					timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].second) {
					timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].second = 
						arcadeProgress.totalPlayTimeMs;
					std::string playerPath = player1isActivePlayer() ?
						player1_configPath + player1Outfit.outfitDirectory :
						player2_configPath + player2Outfit.outfitDirectory;
					timeAttackRecordPerDifficultyLevel[gameOptions->getAILevel()].first = playerPath;
				}
			}
			else if (arcadeType == FK_SceneArcadeType::ArcadeClassic) {
				if (numberOfArcadeBeatenPerDifficultyLevel.count(gameOptions->getAILevel()) == 0) {
					numberOfArcadeBeatenPerDifficultyLevel[gameOptions->getAILevel()] = 0;
				}
				numberOfArcadeBeatenPerDifficultyLevel[gameOptions->getAILevel()] += 1;
				// and add 1 even to the character which won the arcade
				std::string key1 = player1->getOriginalName();
				std::replace(key1.begin(), key1.end(), ' ', '_');
				std::string key2 = player2->getOriginalName();
				std::replace(key2.begin(), key2.end(), ' ', '_');
				if (player1isAI) {
					characterStatisticsStructure[key2].numberOfBeatenArcadeMode += 1;
				}
				else {
					characterStatisticsStructure[key1].numberOfBeatenArcadeMode += 1;
				}
			}
			writeSaveFile();
		}
	}

	FK_Scene::FK_SceneArcadeType FK_SceneGameArcade::getArcadeType(){
		return arcadeType;
	}

	FK_StoryEvent FK_SceneGameArcade::getArcadeEnding()
	{
		return arcadeEnding;
	}

	FK_ArcadeProgress FK_SceneGameArcade::getCurrentArcadeProgress()
	{
		return arcadeProgress;
	}
}