#include"FK_ArcadeCluster.h"
#include<algorithm>
#include<iostream>
#include<fstream>

namespace fk_engine{

	// read character-specific arcade settings file
	void FK_ArcadeFlow::readFromFile(std::string filename){
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		while (inputFile >> temp){
			if (temp == arcadeBossOpponentTag) {
				inputFile >> arcadeBossOpponentPath >> arcadeBossOpponentOutfitId;
				s32 pos = 0;
				std::string tempStr = arcadeBossOpponentPath;
				// find mesh path
				pos = tempStr.find('\\');
				if (pos != std::string::npos) {
					arcadeBossOpponentPath = tempStr.substr(0, pos);
					arcadeBossOpponentOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				arcadeBossOpponentPath += "\\";
			}
			else if (temp == arcadeBossStageTag) {
				inputFile >> arcadeBossStagePath;
				arcadeBossStagePath += "\\";
			}
			else if (temp == climaticBattleOpponentTag){
				inputFile >> climaticBattleOpponentPath >> climaticBattleOpponentOutfitId;
				s32 pos = 0;
				std::string tempStr = climaticBattleOpponentPath;
				// find mesh path
				pos = tempStr.find('\\');
				if (pos != std::string::npos){
					climaticBattleOpponentPath = tempStr.substr(0, pos);
					climaticBattleOpponentOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				climaticBattleOpponentPath += "\\";
				hasClimaticBattle = true;
			}
			else if (temp == climaticBattleStageTag){
				inputFile >> climaticBattleStage;
				climaticBattleStage += "\\";
			}
			else if (temp == climaticBattleIntroQuoteTag){
				std::string dialogueLine = std::string();
				while (dialogueLine.empty()){
					std::getline(inputFile, dialogueLine);
				}
				climaticBattleIntroQuote = dialogueLine;
			}
			else if (temp == climaticBattleIntroQuoteOpponentTag){
				std::string dialogueLine = std::string();
				while (dialogueLine.empty()){
					std::getline(inputFile, dialogueLine);
				}
				climaticBattleIntroQuoteOpponent = dialogueLine;
			}
			else if (temp == climaticBattleWinQuoteTag){
				std::string dialogueLine = std::string();
				while (dialogueLine.empty()){
					std::getline(inputFile, dialogueLine);
				}
				climaticBattleWinQuote = dialogueLine;
			}
			else if (temp == climaticBattleWinQuoteOpponentTag){
				std::string dialogueLine = std::string();
				while (dialogueLine.empty()){
					std::getline(inputFile, dialogueLine);
				}
				climaticBattleWinQuoteOpponent = dialogueLine;
			}
			else if (temp == climaticBattleDialogueTag){
				FK_ArcadeDialogue dialogue;
				inputFile >> temp;
				u32 previousTime = 0;
				while (temp != climaticBattleDialogueEndTag){
					std::replace(temp.begin(), temp.end(), '_', ' ');
					dialogue.speakerName = temp;
					u32 t1, t2;
					inputFile >> t1 >> t2;
					dialogue.timeStart = t1 + previousTime;
					dialogue.timeEnd = dialogue.timeStart + t2;
					std::string dialogueLine = std::string();
					while (dialogueLine.empty()){
						std::getline(inputFile, dialogueLine);
					}
					dialogue.sentence = dialogueLine;
					dialogueLines.push_back(dialogue);
					inputFile >> temp;
					previousTime = dialogue.timeEnd;
				}
			}
		}
		inputFile.clear();
		inputFile.close();
	}

	// read arcade settings file
	void FK_ArcadeSettings::readFromFile(std::string filename, std::string mediaPath){
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		while (inputFile >> temp){
			if (temp == numberOfMatchesFileTag){
				inputFile >> numberOfMatches;
			}
			else if (temp == bossCharacter1PathTag){
				inputFile >> bossCharacter1Path;
				bossCharacter1Path += "\\";
			}
			else if (temp == bossCharacter1LoadingScreenFilenameTag){
				inputFile >> bossCharacter1LoadingScreenFilename;
			}
			else if (temp == bossCharacter1StageTag){
				inputFile >> bossCharacter1Stage;
				bossCharacter1Stage += "\\";
			}
			else if (temp == bossCharacter1LoadingScreenTaglineTag){
				bossCharacter1LoadingScreenTagline = std::string();
				while (bossCharacter1LoadingScreenTagline.empty()){
					std::getline(inputFile, bossCharacter1LoadingScreenTagline);
				}
			}
			else if (temp == bossCharacter1LifeMultiplierTag){
				inputFile >> bossCharacter1LifeMultiplier;
				bossCharacter1LifeMultiplier = std::max(0.f, bossCharacter1LifeMultiplier);
			}
			else if (temp == climaticBattleActivatedTag){
				inputFile >> allowClimaticBattle;
			}
			else if (temp == specialMatchesActivatedTag) {
				inputFile >> allowSpecialMatches;
			}
			else if (temp == climaticBattleDefaultStageTag){
				inputFile >> climaticBattleDefaultStage;
				climaticBattleDefaultStage += "\\";
			}
			else if (temp == climaticBattleDefaultTaglineTag){
				climaticBattleDefaultTagline = std::string();
				while (climaticBattleDefaultTagline.empty()){
					std::getline(inputFile, climaticBattleDefaultTagline);
				}
			}
			else if (temp == climaticBattleLifeMultiplierTag){
				inputFile >> climaticBattleLifeMultiplier;
				climaticBattleLifeMultiplier = std::max(0.f, climaticBattleLifeMultiplier);
			}
			else if (temp == climaticBattleNumberOfRoundsTag){
				inputFile >> climaticBattleNumberOfRounds;
				climaticBattleNumberOfRounds = std::max(1, climaticBattleNumberOfRounds);
			}
			else if (temp == climaticBattleTimerTag){
				inputFile >> climaticBattleTimer;
				climaticBattleTimer = std::max(15, climaticBattleTimer);
			}
			else if (temp == specialMatchTag) {
				FK_ArcadeSpecialMatch match;
				match.setup(inputFile, mediaPath,specialMatchEndTag);
				specialMatches.push_back(match);
			}
		}
		inputFile.clear();
		inputFile.close();
	}

	// prepare arcade cluster
	FK_ArcadeCluster::FK_ArcadeCluster(){
		playerPath = std::string();
		AIlevel = -1;
		outfitId = 0;
		outfitName = std::string();
		stagePath = std::string();
		outfit = FK_Character::FK_CharacterOutfit();
	};
	FK_ArcadeCluster::FK_ArcadeCluster(std::string mediaDirectoryPath, std::string new_playerPath, u32 new_outfitId, std::string newOutfitName, std::string new_stagePath, s32 new_AIlevel) : 
		FK_ArcadeCluster(){
		playerPath = new_playerPath;
		AIlevel = new_AIlevel;
		outfitId = new_outfitId;
		outfitName = newOutfitName;
		stagePath = new_stagePath;
		if (!playerPath.empty()) {
			parseCharacterMeshFiles(mediaDirectoryPath);
		}
	};
	std::string FK_ArcadeCluster::getPlayerPath(){
		return playerPath;
	}
	std::string FK_ArcadeCluster::getStagePath(){
		return stagePath;
	}
	std::string FK_ArcadeCluster::getOutfitName()
	{
		return outfitName;
	}
	u32 FK_ArcadeCluster::getPlayerOutfitId(){
		return outfitId;
	}
	FK_Character::FK_CharacterOutfit FK_ArcadeCluster::getPlayerOutfit(){
		return outfit;
	}
	s32 FK_ArcadeCluster::getAILevel(){
		return AIlevel;
	}
	void FK_ArcadeCluster::parseCharacterMeshFiles(std::string mediaDirectoryPath){
		u32 maximumNumberOfMeshes = 100;
		std::string additionalMeshConfigFilename = "config.txt";
		std::map<FK_Character::FK_OutfitFileKeys, std::string> outfitKeysMap;
		outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitFilename] = "#MESH_FILENAME";
		outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitName] = "#OUTFIT_NAME";
		outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAvailable] = "#AVAILABLE_FROM_START";
		outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeCharacterName] = "#CHARACTER_NAME";
		outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeDisplayName] = "#CHARACTER_SHORTNAME";
		std::vector<FK_Character::FK_CharacterOutfit> availableCharacterOutfits;
		std::cout << playerPath << std::endl;
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i){
			std::string subfolder = fk_constants::AdditionalCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			std::string path = mediaDirectoryPath + playerPath + subfolder;
			std::ifstream configurationFile((path + additionalMeshConfigFilename).c_str());
			if (!configurationFile){
				continue;
			}
			std::string temp;
			FK_Character::FK_CharacterOutfit newOutfit;
			while (configurationFile >> temp){
				if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitFilename]){
					configurationFile >> temp;
					newOutfit.outfitFilename = temp;
					newOutfit.outfitDirectory = subfolder;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitName]){
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitName = temp;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAvailable]){
					u32 availabilityFlag;
					configurationFile >> availabilityFlag;
					newOutfit.isAvailableFromBeginning = (bool)availabilityFlag;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeCharacterName]){
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitCharacterName = temp;
					newOutfit.outfitDisplayName = temp;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeDisplayName]){
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitDisplayName = temp;
				}
			}
			newOutfit.outfitId = (s32)availableCharacterOutfits.size();
			availableCharacterOutfits.push_back(newOutfit);
			configurationFile.clear();
			configurationFile.close();
		}
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i){
			std::string subfolder = fk_constants::DLCCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			std::string path = mediaDirectoryPath + playerPath + subfolder;
			std::ifstream configurationFile((path + additionalMeshConfigFilename).c_str());
			if (!configurationFile){
				continue;
			}
			std::string temp;
			FK_Character::FK_CharacterOutfit newOutfit;
			while (configurationFile >> temp){
				if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitFilename]){
					configurationFile >> temp;
					newOutfit.outfitFilename = temp;
					newOutfit.outfitDirectory = subfolder;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitName]){
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitName = temp;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAvailable]){
					u32 availabilityFlag;
					configurationFile >> availabilityFlag;
					newOutfit.isAvailableFromBeginning = availabilityFlag;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeCharacterName]){
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitCharacterName = temp;
					newOutfit.outfitDisplayName = temp;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeDisplayName]){
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitDisplayName = temp;
				}
			}
			newOutfit.outfitId = (s32)availableCharacterOutfits.size();
			availableCharacterOutfits.push_back(newOutfit);
			configurationFile.clear();
			configurationFile.close();
		}
		for (u32 i = 1; i < maximumNumberOfMeshes; ++i) {
			std::string subfolder = fk_constants::ExtraCharacterMeshSubfolderRoot + std::to_string(i) + "\\";
			std::string path = mediaDirectoryPath + playerPath + subfolder;
			std::ifstream configurationFile((path + additionalMeshConfigFilename).c_str());
			if (!configurationFile) {
				continue;
			}
			std::string temp;
			FK_Character::FK_CharacterOutfit newOutfit;
			while (configurationFile >> temp) {
				if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitFilename]) {
					configurationFile >> temp;
					newOutfit.outfitFilename = temp;
					newOutfit.outfitDirectory = subfolder;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitName]) {
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitName = temp;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAvailable]) {
					u32 availabilityFlag;
					configurationFile >> availabilityFlag;
					newOutfit.isAvailableFromBeginning = availabilityFlag;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeCharacterName]) {
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitCharacterName = temp;
					newOutfit.outfitDisplayName = temp;
				}
				else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeDisplayName]) {
					configurationFile >> temp;
					std::replace(temp.begin(), temp.end(), '_', ' ');
					newOutfit.outfitDisplayName = temp;
				}
			}
			newOutfit.outfitId = (s32)availableCharacterOutfits.size();
			availableCharacterOutfits.push_back(newOutfit);
			configurationFile.clear();
			configurationFile.close();
		}
		if (!outfitName.empty()) {
			std::string subfolder = outfitName;
			std::string path = mediaDirectoryPath + playerPath + subfolder;
			std::ifstream configurationFile((path + additionalMeshConfigFilename).c_str());
			if (configurationFile) {
				std::string temp;
				FK_Character::FK_CharacterOutfit newOutfit;
				while (configurationFile >> temp) {
					if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitFilename]) {
						configurationFile >> temp;
						newOutfit.outfitFilename = temp;
						newOutfit.outfitDirectory = subfolder;
					}
					else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitName]) {
						configurationFile >> temp;
						std::replace(temp.begin(), temp.end(), '_', ' ');
						newOutfit.outfitName = temp;
					}
					else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAvailable]) {
						u32 availabilityFlag;
						configurationFile >> availabilityFlag;
						newOutfit.isAvailableFromBeginning = (bool)availabilityFlag;
					}
					else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeCharacterName]) {
						configurationFile >> temp;
						std::replace(temp.begin(), temp.end(), '_', ' ');
						newOutfit.outfitCharacterName = temp;
						newOutfit.outfitDisplayName = temp;
					}
					else if (temp == outfitKeysMap[FK_Character::FK_OutfitFileKeys::OutfitAlternativeDisplayName]) {
						configurationFile >> temp;
						std::replace(temp.begin(), temp.end(), '_', ' ');
						newOutfit.outfitDisplayName = temp;
					}
				}
				newOutfit.outfitId = (s32)availableCharacterOutfits.size();
				availableCharacterOutfits.push_back(newOutfit);
				configurationFile.clear();
				configurationFile.close();
			}
		}
		if (outfitName.empty()){
			outfit = availableCharacterOutfits[outfitId];
		}
		else{
			for each(FK_Character::FK_CharacterOutfit newOutfit in availableCharacterOutfits){
				if (outfitName == newOutfit.outfitDirectory){
					outfit = newOutfit;
					break;
				}
			}
		}
		availableCharacterOutfits.clear();
	}

	bool FK_ArcadeCondition::conditionIsFulfilled(FK_ArcadeProgress & currentProgress,
		u32 AI_level, std::string playerCharacterPath)	{
		bool successFlag = true;
		if (conditionsToFulfill.empty() && tagsToClear.empty() &&
			onlyFoundWithSpecificCharacter.empty() &&
			notFoundWithSpecificCharacter.empty()) {
			return true;
		}

		if (!onlyFoundWithSpecificCharacter.empty()) {
			successFlag &= std::find(
				onlyFoundWithSpecificCharacter.begin(),
				onlyFoundWithSpecificCharacter.end(),
				playerCharacterPath) != onlyFoundWithSpecificCharacter.end();
		}

		if (!notFoundWithSpecificCharacter.empty()) {
			successFlag &= std::find(
				notFoundWithSpecificCharacter.begin(),
				notFoundWithSpecificCharacter.end(),
				playerCharacterPath) == notFoundWithSpecificCharacter.end();
		}

		if (conditionsToFulfill.count(AIdifficultyTag) > 0) {
			if (conditionsToFulfill[AIdifficultyTag].second) {
				successFlag &= AI_level > conditionsToFulfill[AIdifficultyTag].first;
			}
			else {
				successFlag &= AI_level < conditionsToFulfill[AIdifficultyTag].first;
			}
		}
		if (conditionsToFulfill.count(numberOfContinuesTag) > 0) {
			if (conditionsToFulfill[numberOfContinuesTag].second) {
				successFlag &= currentProgress.numberOfContinues > conditionsToFulfill[numberOfContinuesTag].first;
			}
			else {
				successFlag &= currentProgress.numberOfContinues < conditionsToFulfill[numberOfContinuesTag].first;
			}
		}
		if (conditionsToFulfill.count(numberOfPerfectWinsTag) > 0) {
			if (conditionsToFulfill[numberOfPerfectWinsTag].second) {
				successFlag &= currentProgress.numberOfPerfectRoundsWon > conditionsToFulfill[numberOfPerfectWinsTag].first;
			}
			else {
				successFlag &= currentProgress.numberOfPerfectRoundsWon < conditionsToFulfill[numberOfPerfectWinsTag].first;
			}
		}
		if (conditionsToFulfill.count(numberOfRingoutsTag) > 0) {
			if (conditionsToFulfill[numberOfRingoutsTag].second) {
				successFlag &= currentProgress.numberOfRingouts > conditionsToFulfill[numberOfRingoutsTag].first;
			}
			else {
				successFlag &= currentProgress.numberOfRingouts < conditionsToFulfill[numberOfRingoutsTag].first;
			}
		}
		if (conditionsToFulfill.count(numberOfTimeoutsTag) > 0) {
			if (conditionsToFulfill[numberOfTimeoutsTag].second) {
				successFlag &= currentProgress.numberOfTimeouts > conditionsToFulfill[numberOfTimeoutsTag].first;
			}
			else {
				successFlag &= currentProgress.numberOfTimeouts < conditionsToFulfill[numberOfTimeoutsTag].first;
			}
		}
		if (conditionsToFulfill.count(numberOfRoundsLostTag) > 0) {
			if (conditionsToFulfill[numberOfRoundsLostTag].second) {
				successFlag &= currentProgress.numberOfRoundsLost > conditionsToFulfill[numberOfRoundsLostTag].first;
			}
			else {
				successFlag &= currentProgress.numberOfRoundsLost < conditionsToFulfill[numberOfRoundsLostTag].first;
			}
		}
		if (!tagsToClear.empty()) {
			for each (std::pair<std::string, bool> tag in tagsToClear) {
				if (tag.second) {
					successFlag &= std::find(currentProgress.specialTagsUnlocked.begin(),
						currentProgress.specialTagsUnlocked.end(), tag.first) !=
						currentProgress.specialTagsUnlocked.end();
				}
				else {
					successFlag &= std::find(currentProgress.specialTagsUnlocked.begin(),
						currentProgress.specialTagsUnlocked.end(), tag.first) ==
						currentProgress.specialTagsUnlocked.end();
				}
			}
		}
		return successFlag;
	}

	void FK_ArcadeCondition::readFromFile(std::ifstream& inputFile, std::string ConditionEndTag)
	{
		tagsToClear.clear();
		conditionsToFulfill.clear();
		std::string temp;
		while (inputFile >> temp) {
			if (temp == ConditionEndTag) {
				return;
			}
			if (temp == specialTagsBegin) {
				while (inputFile >> temp) {
					if (temp == specialTagsEnd) {
						break;
					}
					else {
						s32 flag;
						inputFile >> flag;
						tagsToClear.push_back(std::pair<std::string, bool>(temp, flag > 0));
					}
				}
			}
			else if (temp == notWithCharacterTag) {
				inputFile >> temp;
				notFoundWithSpecificCharacter.push_back(temp + "\\");
			}
			else if (temp == onlyWithCharacterTag) {
				inputFile >> temp;
				onlyFoundWithSpecificCharacter.push_back(temp + "\\");
			}
			else {
				std::string tag = temp;
				u32 value;
				bool comparisonGreaterThan = true;
				inputFile >> temp >> value;
				if (temp == greaterThanSign) {
					comparisonGreaterThan = true;
				}
				else if (temp == smallerThanSign) {
					comparisonGreaterThan = false;
				}
				conditionsToFulfill[tag] = std::pair<u32, bool>(value, comparisonGreaterThan);
			}
		}
	}

	/* setup a special match for arcade mode*/
	void FK_ArcadeSpecialMatch::setup(std::ifstream & inputFile, std::string mediaPath, std::string specialMatchEndTag)
	{
		std::string temp;
		std::string opponentPath;
		std::string stagePath;
		std::string opponentOutfitName;
		u32 AILevel = 100;
		u32 opponentOutfitId = 0;

		while (inputFile >> temp) {
			if (temp == specialMatchEndTag) {
				matchConditions = FK_ArcadeCluster(
					mediaPath, opponentPath, opponentOutfitId,
					opponentOutfitName, stagePath, AILevel);
				return;
			}
			else if (temp == MatchConditionFileKey) {
				condition = FK_ArcadeCondition();
				condition.readFromFile(inputFile, MatchConditionFileKeyEnd);
			}
			else if (temp == MatchLoadingTagKey) {
				matchTag = std::string();
				while (matchTag.empty()) {
					std::getline(inputFile, matchTag);
				}
			}
			else if (temp == SplashcreenFilenameKey) {
				inputFile >> matchPreviewSplashScreenName;
			}
			else if (temp == TagGainedOnWinKey) {
				inputFile >> tagGainedOnWin;
			}
			else if (temp == AILevelFileKey) {
				f32 tempVal = 1.0f;
				inputFile >> tempVal;
				AILevel = (u32)std::round(tempVal * 100);
			}
			else if (temp == MatchNumberKey) {
				inputFile >> matchNumber;
			}
			else if (temp == OpponentRelativePathKey) {
				inputFile >> opponentPath;
				std::string tempStr = opponentPath;
				// find mesh path
				s32 pos = tempStr.find('\\');
				if (pos != std::string::npos) {
					opponentPath = tempStr.substr(0, pos);
					opponentOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				opponentPath += "\\";
			}
			else if (temp == OpponentOutfitIdFileKey) {
				inputFile >> opponentOutfitId;
			}
			else if (temp == StageFileKey) {
				inputFile >> stagePath;
				stagePath += "\\";
			}
		}
	}
}