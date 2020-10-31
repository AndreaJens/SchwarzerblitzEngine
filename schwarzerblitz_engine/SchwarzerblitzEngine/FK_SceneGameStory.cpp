#include"FK_SceneGameStory.h"

namespace fk_engine{
	// constructor (lazily based on the basic class constructor) 
	FK_SceneGameStory::FK_SceneGameStory() : FK_SceneGame(){
		winConditionDialogueWindow = NULL;
	};

	// setup
	void FK_SceneGameStory::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		FK_StoryMatch* newMatchSettings){
		matchSettings = newMatchSettings;
		u32 roundsPerMatch = matchSettings->additionalOptions.numberOfRounds;
		u32 timerSeconds = matchSettings->additionalOptions.roundTimerInSeconds;
		dialogueInMatch = !matchSettings->dialogueInBattleFilename.empty();
		//load outfits
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		charactersPath = mediaPath + "characters\\";
		std::string new_player1path = matchSettings->playerPath;
		std::string new_player2path = matchSettings->opponentPath;
		if (new_player1path == new_player2path && matchSettings->playerOutfitId == matchSettings->opponentOutfitId){
			matchSettings->playerOutfitId = 0;
			matchSettings->opponentOutfitId = 1;
		}
		FK_Character* tempCharacter = new FK_Character;
		std::string character1Path = charactersPath + new_player1path;
		if (!isValidCharacterPath(character1Path) && isValidCharacterPath(new_player1path)) {
			character1Path = new_player1path;
		}
		if (matchSettings->playerOutfitName.empty()){
			tempCharacter->loadBasicVariables(
				databaseAccessor,
				"character.txt", character1Path,
				commonResourcesPath,
				smgr, matchSettings->playerOutfitId);
		}
		else{
			tempCharacter->loadBasicVariables(
				databaseAccessor,
				"character.txt", character1Path,
				commonResourcesPath,
				smgr, matchSettings->playerOutfitName);
		}
		FK_Character::FK_CharacterOutfit outfitPlayer = tempCharacter->getOutfit();
		delete tempCharacter;
		tempCharacter = new FK_Character;
		std::string character2Path = charactersPath + new_player2path;
		if (!isValidCharacterPath(character2Path) && isValidCharacterPath(new_player2path)) {
			character2Path = new_player2path;
		}
		if (matchSettings->opponentOutfitName.empty()){
			tempCharacter->loadBasicVariables(
				databaseAccessor,
				"character.txt", character2Path,
				commonResourcesPath,
				smgr, matchSettings->opponentOutfitId);
		}
		else{
			tempCharacter->loadBasicVariables(
				databaseAccessor,
				"character.txt", character2Path,
				commonResourcesPath,
				smgr, matchSettings->opponentOutfitName);
		}
		FK_Character::FK_CharacterOutfit outfitOpponent = tempCharacter->getOutfit();
		delete tempCharacter;
		s32 AIlevel = matchSettings->AI_level;
		f32 standardAIlevel = 5.f;
		AIlevel = (s32)std::round((f32)AIlevel * max(1.f, (f32)gameOptions->getAILevel()) / standardAIlevel);
		FK_SceneGame::setup(newDevice, new_joypadInfo, newOptions, new_player1path, new_player2path,
			outfitPlayer, outfitOpponent, matchSettings->stagePath, FK_Scene::FK_SceneType::SceneStoryModeMatch,
			matchSettings->additionalOptions, false, true, AIlevel, AIlevel);
	}

	// setup training variables
	void FK_SceneGameStory::setupAdditionalVariables(){
		
	}

	// dispose
	void FK_SceneGameStory::dispose() {
		if (winConditionDialogueWindow != NULL) {
			delete winConditionDialogueWindow;
		}
		FK_SceneGame::dispose();
	}

	/* get pause menu items */
	std::vector<std::string> FK_SceneGameStory::getPauseMenuItems(){
		std::vector<std::string> menuItems = FK_SceneGame::getPauseMenuItems();
		menuItems[4] = "To episode selection";
		return menuItems;
	}

	// setup players intro and win quotes for climatic battle (if any)
	void FK_SceneGameStory::setupAdditionalParametersAfterLoading(){
		FK_Character* activePlayer = player1;
		FK_Character* opponent = player2;
		if (!matchSettings->playerIntroQuote.empty()){
			std::vector<std::wstring> quotes;
			std::string line = matchSettings->playerIntroQuote;
			std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
			quotes.push_back(tempLine);
			activePlayer->overrideIntroQuotes(quotes, opponent->getName());
		}
		if (!matchSettings->opponentIntroQuote.empty()){
			std::vector<std::wstring> quotes;
			std::string line = matchSettings->opponentIntroQuote;
			std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
			quotes.push_back(tempLine);
			opponent->overrideIntroQuotes(quotes, activePlayer->getName());
		}
		if (!matchSettings->playerWinQuote.empty()){
			std::vector<std::wstring> quotes;
			std::string line = matchSettings->playerWinQuote;
			std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
			quotes.push_back(tempLine);
			activePlayer->overrideMatchWinQuotes(quotes, opponent->getName());
			activePlayer->overrideRoundWinQuotes(quotes, opponent->getName());
		}
		if (!matchSettings->opponentWinQuote.empty()){
			std::vector<std::wstring> quotes;
			std::string line = matchSettings->opponentWinQuote;
			std::wstring tempLine = fk_addons::convertNameToNonASCIIWstring(line);
			quotes.push_back(tempLine);
			opponent->overrideMatchWinQuotes(quotes, activePlayer->getName());
			opponent->overrideRoundWinQuotes(quotes, activePlayer->getName());
		}
		activePlayer = NULL;
		opponent = NULL;
		if (dialogueInMatch){
			std::string path = matchSettings->storyDirectory + matchSettings->dialogueInBattleFilename;
			dialogue.readFromFile(path);
		}
		if (matchSettings->winCondition.first != FK_StoryMatch::MatchWinCondition::Normal) {
			setupMatchWinConditionWindow();
		}
	}

	// update AI input
	bool FK_SceneGameStory::updateAIInput(u32 delta_t_ms, u32 playerID) {
		s32 tempRoundTimer = roundTimerMs;
		if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Survive) {
			roundTimerMs = 100;
		}
		bool exitFlag = FK_SceneGame::updateAIInput(delta_t_ms, playerID);
		roundTimerMs = tempRoundTimer;
		return exitFlag;
	}


	// update scene training logic
	void FK_SceneGameStory::updateAdditionalSceneLogic(u32 delta_t_ms){
		if (winConditionDialogueWindow != NULL) {
			winConditionDialogueWindow->update(delta_t_ms);
		}
	}

	// draw additional HUD items
	void FK_SceneGameStory::drawAdditionalHUDItems(f32 delta_t_s){
		// draw match number for arcade mode 
		//TODO!
		u32 timeElapsed = getElapsedTimeMS();
		if (showHUD()){
			if (dialogueInMatch && !(player1->isKO() || player2->isKO())){
				if (!dialogue.dialogueLines.empty()){
					for each(auto dialogue in dialogue.dialogueLines){
						if (timeElapsed >= dialogue.timeStart && timeElapsed < dialogue.timeEnd){
							hudManager->drawMatchSentence(dialogue.speakerName, dialogue.sentence);
							break;
						}
					}
				}
			}
		}
		if (processingIntroStage && winConditionDialogueWindow != NULL) {
			core::dimension2d<u32> screenSize = driver->getScreenSize();
			if (gameOptions->getBorderlessWindowMode()) {
				screenSize = borderlessWindowRenderTarget->getSize();
			}
			winConditionDialogueWindow->draw(screenSize, 1.f, 1.f, false);
		}
	}

	// check if match was canceled
	bool FK_SceneGameStory::hasCanceledMatch(){
		return winnerId == 0;
	}

	// impact effect (every time the boss hits you, obviously)
	void FK_SceneGameStory::setupImpactEffect(FK_Reaction_Type reaction, s32 playerId){
		FK_SceneGame::setupImpactEffect(reaction);
	}

	/* get scene name for loading screen */
	std::wstring FK_SceneGameStory::getSceneNameForLoadingScreen(){
		return L"Story battle";
	}

	// draw loading screen
	void FK_SceneGameStory::drawLoadingScreen(f32 completionPercentage){
		FK_SceneGame::drawLoadingScreen(completionPercentage);
	}

	//! load loading textures
	void FK_SceneGameStory::loadLoadingTextures(){
		FK_SceneGame::loadLoadingTextures();
	}

	/* setup "continue" if needed */
	void FK_SceneGameStory::processMatchEnd(){
		if ((winnerId == 1 && player2isAI) ||
			(winnerId == -1 && player1isAI)
			){
			setupContinue();
		}
		else{
			processingSceneEnd = true;
			FK_SceneGameStory::updateSaveFileData();
			if (matchSettings->playCredits){
				setNextScene(FK_Scene::FK_SceneType::SceneCredits);
			}
			else if (matchSettings->returnToStoryMenu){
				setNextScene(FK_Scene::FK_SceneType::SceneStoryModeSelection);
			}
			else{
				setNextScene(nameId());
			}
			abortMatch = true;
		}
	}

	/* change conditions for win text showing */
	bool FK_SceneGameStory::showWinText(){
		bool continueFlag = FK_SceneGame::showContinueText();
		bool winFlag = FK_SceneGame::showWinText();
		return (!continueFlag && (winFlag || processingSceneEnd));
	}

	/* update save file data */
	void FK_SceneGameStory::updateSaveFileData(){
		if (matchSettings->saveStoryCompletion){
			// and add 1 even to the character which won the arcade
			//std::string key1 = player1->getOriginalName();
			//std::replace(key1.begin(), key1.end(), ' ', '_');
			//characterStatisticsStructure[key1].numberOfBeatenStoryMode += 1;
			if (std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), matchSettings->episodePath) ==
				storyEpisodesCleared.end()){
				storyEpisodesCleared.push_back(matchSettings->episodePath);
			}
			writeSaveFile();
		}
	}

	/* get round winner (for survive missions) */
	int FK_SceneGameStory::getRoundWinner(bool updateWins) {
		if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Normal ||
			matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Poison) {
			return FK_SceneGame::getRoundWinner(updateWins);
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Percentage) {
			if (player1->getLifePercentage() > 0.f && 
				player2->getLifePercentage() <= matchSettings->winCondition.second) {
				numberOfPlayer1Wins += 1;
				return -1;
			}else{
				numberOfPlayer2Wins += 1;
				return 1;
			}
			
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Survive) {
			if (player1->getLifePercentage() > 0.f) {
				numberOfPlayer1Wins += 1;
				return -1;
			}
			else {
				numberOfPlayer2Wins += 1;
				return 1;
			}

		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::WinBeforeTimer) {
			if (player2->getLifePercentage() <= 0.f && player1->getLifePercentage() > 0.f) {
				numberOfPlayer1Wins += 1;
				return -1;
			}
			else {
				numberOfPlayer2Wins += 1;
				return 1;
			}
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Ringout){
			if (isProcessingRingout() && player2->getLifePercentage() <= 0.f && player1->getLifePercentage() > 0.f) {
				numberOfPlayer1Wins += 1;
				return -1;
			}
			else {
				numberOfPlayer2Wins += 1;
				return 1;
			}
		}
		return -99;
	}

	/* game logic (win, lose, et cetera)*/
	bool FK_SceneGameStory::isEndOfRound() {
		if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Percentage) {
			return (player1->isKO() || player2->getLifePercentage() <= matchSettings->winCondition.second);
		}
		return FK_SceneGame::isEndOfRound();
	}

	/* setup match win condition window */
	void FK_SceneGameStory::setupMatchWinConditionWindow()
	{
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		
		//f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		//core::rect<s32> frame(50, 140, 590, 220);
		if (winConditionDialogueWindow != NULL) {
			delete winConditionDialogueWindow;
			winConditionDialogueWindow = NULL;
		}
		s32 width = screenSize.Width * 1 / 3;
		s32 x = (screenSize.Width - width) / 2;
		s32 height = screenSize.Height / 16;
		std::vector<std::string> content;
		if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Survive) {
			content.push_back("Survive until the timer runs out!");
			height = height * 2;
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::WinBeforeTimer) {
			std::string textToAdd = "Defeat your opponent before the timer runs out!";
			content.push_back(textToAdd);
			height = height * 3;
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Poison) {
			std::string textToAdd = "Your energy goes down with time!";
			content.push_back(textToAdd);
			height = height * 2;
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Percentage) {
			std::string textToAdd = "Reduce the opponent's health below ";
			textToAdd += std::to_string((u32)(std::round(matchSettings->winCondition.second))) + "%!";
			content.push_back(textToAdd);
			height = height * 2;
		}
		else if (matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Ringout) {
			std::string textToAdd = "Win by sending the opponent out of the ring!";
			content.push_back(textToAdd);
			height = height * 3;
		}
		if (content.empty()) {
			winConditionDialogueWindow = NULL;
			return;
		}
		s32 y = (screenSize.Height - height) / 2;
		core::rect<s32> frame(x, y, x + width, y + height);
		winConditionDialogueWindow = new FK_DialogueWindow(device, frame, "windowskin.png", commonResourcesPath,
			1.f,
			std::string(), content, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
		winConditionDialogueWindow->setAlpha(160);
		winConditionDialogueWindow->open(400);
		winConditionDialogueWindow->setVisibility(true);
		winConditionDialogueWindow->setActive(true);
	}

	/* show round diamonds indicators */
	bool FK_SceneGameStory::showRoundIndicatorsHUD() {
		return numberOfRounds > 1 || 
			(matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Normal &&
				roundTimerMs <= fk_constants::FK_MaxRoundTimerSeconds * 1000);
	}

	/* check if player can receive input */
	bool FK_SceneGameStory::canUpdateInput() {
		if (isEndOfRound() && matchSettings->winCondition.first == FK_StoryMatch::MatchWinCondition::Percentage) {
			return false;
		}
		return FK_SceneGame::canUpdateInput();
	}

	/* produce proper win text line */
	std::string FK_SceneGameStory::getEndOfRoundSoundIdentifier(const s32& matchWinnerId, const bool& perfectFlag) {
		std::string soundIdentifier = perfectFlag ? "perfect" : "ko";
		bool successFlag = matchWinnerId == -1 && matchSettings->winCondition.first != FK_StoryMatch::MatchWinCondition::Normal;
		if (successFlag) {
			soundIdentifier = "success";
		}
		return soundIdentifier;
	}

	/* draw K.O. text */
	void FK_SceneGameStory::drawKOtext(const s32& matchWinnerId, const bool&perfectFlag) {
		if (matchWinnerId == -1 && matchSettings->winCondition.first != FK_StoryMatch::MatchWinCondition::Normal) {
			hudManager->drawSuccessText();
		}else{
			FK_SceneGame::drawKOtext(matchWinnerId, perfectFlag);
		}
	}

}