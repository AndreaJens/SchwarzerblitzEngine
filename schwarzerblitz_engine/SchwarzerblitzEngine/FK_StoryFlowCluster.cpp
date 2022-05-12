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

#include"FK_StoryFlowCluster.h"
#include<fstream>

using namespace irr;

namespace fk_engine{

	// in-match dialogue
	// read character-specific arcade settings file
	void FK_InMatchDialogue::readFromFile(std::string filename){
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		while (inputFile >> temp){
			if (temp == FK_InMatchDialogue::dialogueBeginTag){
				FK_MatchDialogue dialogue;
				inputFile >> temp;
				u32 previousTime = 0;
				while (temp != FK_InMatchDialogue::dialogueEndTag){
					std::replace(temp.begin(), temp.end(), '_', ' ');
					dialogue.speakerName = temp;
					u32 t1, t2;
					inputFile >> t1 >> t2;
					dialogue.timeStart = t1 + previousTime;
					dialogue.timeEnd = dialogue.timeStart + t2;
					std::string dialogueLine = std::string();
					while (dialogueLine.empty() || dialogueLine == " "){
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

	// basic constructor
	FK_StoryItem::FK_StoryItem(){
		playCredits = false;
		saveStoryCompletion = false;
		returnToStoryMenu = false;
	}

	//define the only known method for story item
	FK_StoryItem::ItemType FK_StoryItem::getType(){
		return type;
	}

	//define methods for Story Match
	FK_StoryMatch::FK_StoryMatch(){
		type = ItemType::Match;
		additionalOptions.numberOfRounds = 3;
		additionalOptions.showRoundWinScreen = false;
		stagePath = "TheWalkingNight\\";
		playerPath = "chara_dummy\\";
		opponentPath = "chara_dummy\\";
		opponentOutfitName = std::string();
		playerOutfitName = std::string();
		playerOutfitId = 0;
		opponentOutfitId = 0;
		AI_level = 5;
		winCondition.first = MatchWinCondition::Normal;
		winCondition.second = 0.f;
	}
	void FK_StoryMatch::setup(std::string filename, std::string episodeDirectory, std::string episodeRelativePath){
		storyDirectory = episodeDirectory;
		configFileName = filename;
		episodePath = episodeRelativePath;
		std::ifstream inputFile((episodeDirectory + configFileName).c_str());
		std::string temp;
		std::map<MatchWinCondition, std::string> winConditionMap;
		winConditionMap[MatchWinCondition::Normal] = "Normal";
		winConditionMap[MatchWinCondition::Survive] = "Survive";
		winConditionMap[MatchWinCondition::Percentage] = "Percentage";
		winConditionMap[MatchWinCondition::WinBeforeTimer] = "Before_Time_Out";
		winConditionMap[MatchWinCondition::Poison] = "Poison";
		winConditionMap[MatchWinCondition::Ringout] = "Ring_Out";
		while (inputFile >> temp){
			if (temp == NumberOfRoundsFileKey){
				inputFile >> additionalOptions.numberOfRounds;
			}
			else if (temp == NumberOfRoundsPlayerFileKey){
				inputFile >> additionalOptions.numberOfRoundsPlayer1MustWin;
			}
			else if (temp == NumberOfRoundsOpponentFileKey){
				inputFile >> additionalOptions.numberOfRoundsPlayer2MustWin;
			}
			else if (temp == RingoutFileKey){
				additionalOptions.allowRingout = false;
			}
			else if (temp == TimerFileKey){
				inputFile >> additionalOptions.roundTimerInSeconds;
			}
			else if (temp == PlayerTriggerRegenFileKey){
				inputFile >> additionalOptions.triggerRegenerationTimeMSPlayer1;
			}
			else if (temp == OpponentTriggerRegenFileKey){
				inputFile >> additionalOptions.triggerRegenerationTimeMSPlayer2;
			}
			else if (temp == PlayerLifeMultiplierFileKey){
				inputFile >> additionalOptions.lifeMultiplierPlayer1;
			}
			else if (temp == OpponentLifeMultiplierFileKey){
				inputFile >> additionalOptions.lifeMultiplierPlayer2;
			}
			else if (temp == PlayerLifePoisonKey) {
				inputFile >> additionalOptions.continuousDamagePlayer1perSecond;
			}
			else if (temp == OpponentLifePosionKey) {
				inputFile >> additionalOptions.continuousDamagePlayer2perSecond;
			}
			else if (temp == OpponentPhysDamageMultiplierTag){
				inputFile >> additionalOptions.receivedPhysicalDamageMultiplierPlayer2;
			}
			else if (temp == OpponentBulletDamageMultiplierTag){
				inputFile >> additionalOptions.receivedBulletDamageMultiplierPlayer2;
			}
			else if (temp == PlayerPhysDamageMultiplierTag){
				inputFile >> additionalOptions.receivedPhysicalDamageMultiplierPlayer1;
			}
			else if (temp == PlayerBulletDamageMultiplierTag){
				inputFile >> additionalOptions.receivedBulletDamageMultiplierPlayer1;
			}
			else if (temp == WinConditionFileKey) {
				inputFile >> temp;
				if (temp == winConditionMap[MatchWinCondition::Normal]) {
					winCondition.first = MatchWinCondition::Normal;
					winCondition.second = 0.f;
				} 
				else if (temp == winConditionMap[MatchWinCondition::Survive]) {
					winCondition.first = MatchWinCondition::Survive;
					winCondition.second = 0.f;
				}
				else if (temp == winConditionMap[MatchWinCondition::WinBeforeTimer]) {
					winCondition.first = MatchWinCondition::WinBeforeTimer;
					winCondition.second = 0.f;
				}
				else if (temp == winConditionMap[MatchWinCondition::Poison]) {
					winCondition.first = MatchWinCondition::Poison;
					winCondition.second = 0.f;
				}
				else if (temp == winConditionMap[MatchWinCondition::Ringout]) {
					winCondition.first = MatchWinCondition::Ringout;
					winCondition.second = 0.f;
				}
				else if (temp == winConditionMap[MatchWinCondition::Percentage]) {
					f32 percentageVal = 0.f;
					inputFile >> percentageVal;
					winCondition.first = MatchWinCondition::Percentage;
					winCondition.second = percentageVal;
				}
			}
			else if (temp == PlayerRelativePathKey){
				inputFile >> playerPath;
				s32 pos = 0;
				std::string tempStr = playerPath;
				// find mesh path
				pos = tempStr.find('\\');
				if (pos != std::string::npos){
					playerPath = tempStr.substr(0, pos);
					playerOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				playerPath += "\\";
			}
			else if (temp == OpponentRelativePathKey){
				inputFile >> opponentPath;
				std::string tempStr = opponentPath;
				// find mesh path
				s32 pos = tempStr.find('\\');
				if (pos != std::string::npos){
					opponentPath = tempStr.substr(0, pos);
					opponentOutfitName = tempStr.substr(pos + 1, tempStr.length()) + "\\";
				}
				opponentPath += "\\";
			}
			else if (temp == PlayerOutfitIdFileKey){
				inputFile >> playerOutfitId;
			}
			else if (temp == OpponentOutfitIdFileKey){
				inputFile >> opponentOutfitId;
			}
			else if (temp == DialogueInBattleTag){
				inputFile >> dialogueInBattleFilename;
			}
			else if (temp == AILevelFileKey){
				inputFile >> AI_level;
			}
			else if (temp == PlayerIntroQuoteFileKey){
				playerIntroQuote = std::string();
				while (playerIntroQuote.empty()){
					std::getline(inputFile, playerIntroQuote);
				}
			}
			else if (temp == PlayerWinQuoteFileKey){
				playerWinQuote = std::string();
				while (playerWinQuote.empty()){
					std::getline(inputFile, playerWinQuote);
				}
			}
			else if (temp == OpponentIntroQuoteFileKey){
				opponentIntroQuote = std::string();
				while (opponentIntroQuote.empty()){
					std::getline(inputFile, opponentIntroQuote);
				}
			}
			else if (temp == OpponentWinQuoteFileKey){
				opponentWinQuote = std::string();
				while (opponentWinQuote.empty()){
					std::getline(inputFile, opponentWinQuote);
				}
			}
			else if (temp == StageFileKey){
				inputFile >> stagePath;
				stagePath += "\\";
			}
		}
	}

	// define story dialogue
	FK_StoryEvent::FK_StoryEvent(){
		type = ItemType::Cutscene;
		saveWhenSkipped = false;
		allowMenu = true;
		fadeWhenEnding = true;

	}

	void FK_StoryEvent::setup(std::string filename, std::string episodeDirectory, std::string episodeRelativePath){
		configFileName = filename;
		episodePath = episodeRelativePath;
		storyDirectory = episodeDirectory;
	}

	// story flow
	FK_StoryFlowCluster::FK_StoryFlowCluster(){
		reset();
	}
	FK_StoryFlowCluster::~FK_StoryFlowCluster(){
		reset();
	}
	FK_StoryFlowCluster::FK_StoryFlowCluster(const FK_StoryFlowCluster& newEpisode){
		setupFromEpisode(newEpisode);
		episodeComplete = newEpisode.episodeComplete;
		storyIndex = newEpisode.storyIndex;
	}

	FK_StoryFlowCluster& FK_StoryFlowCluster::operator=(const FK_StoryFlowCluster& newEpisode){
		setupFromEpisode(newEpisode);
		episodeComplete = newEpisode.episodeComplete;
		storyIndex = newEpisode.storyIndex;
		return (*this);
	}

	void FK_StoryFlowCluster::setup(std::string new_episodeDirectory, std::string new_episodeRelativePath, std::string configurationFilename){
		episodeDirectory = new_episodeDirectory;
		configFileName = configurationFilename;
		// open flow file
		std::string flowFileName = episodeDirectory + new_episodeRelativePath + configurationFilename;
		episodeRelativePath = new_episodeRelativePath;
		episodeDescription.clear();
		episodeLockedDescription.clear();
		std::ifstream inputFile(flowFileName.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		while (inputFile >> temp){
			if (temp == FK_StoryFlowCluster::EpisodeNameTag){
				std::string name = std::string();
				while (name.empty()){
					std::getline(inputFile, name);
				}
				episodeName = name;
			}
			if (temp == FK_StoryFlowCluster::EpisodeDescriptionTag){
				std::string line = std::string();
				while (line.empty()){
					std::getline(inputFile, line);
					if (!line.empty()){
						if (line == FK_StoryFlowCluster::EpisodeDescriptionEndTag){
							break;
						}
						else{
							episodeDescription.push_back(line);
							line = std::string();
						}
					}
				}
			}
			if (temp == FK_StoryFlowCluster::EpisodeLockedDescriptionTag) {
				std::string line = std::string();
				while (line.empty()) {
					std::getline(inputFile, line);
					if (!line.empty()) {
						if (line == FK_StoryFlowCluster::EpisodeLockedDescriptionEndTag) {
							break;
						}
						else {
							episodeLockedDescription.push_back(line);
							line = std::string();
						}
					}
				}
			}
			if (temp == FK_StoryFlowCluster::MatchEventTag){
				inputFile >> temp;
				temp += ".txt";
				FK_StoryMatch* match = new FK_StoryMatch();
				match->setup(temp, episodeDirectory + new_episodeRelativePath, new_episodeRelativePath);
				storyEvents.push_back(match);
				match = NULL;
			}
			else if (temp == FK_StoryFlowCluster::DialogueEventTag){
				inputFile >> temp;
				temp += ".txt";
				FK_StoryEvent* dial_event = new FK_StoryEvent();
				dial_event->setup(temp, episodeDirectory + new_episodeRelativePath, new_episodeRelativePath);
				storyEvents.push_back(dial_event);
				dial_event = NULL;
			}
			else if (temp == FK_StoryFlowCluster::CreditsTag) {
				inputFile >> temp;
				temp += ".txt";
				FK_StoryCredits* credits_event = new FK_StoryCredits();
				credits_event->setup(temp, episodeDirectory + new_episodeRelativePath, new_episodeRelativePath);
				storyEvents.push_back(credits_event);
				credits_event = NULL;
			}
		}
		inputFile.close();
		if (storyEvents.size() >= (u32)1){
			storyEvents[storyEvents.size() - 1]->returnToStoryMenu = true;
			storyEvents[storyEvents.size() - 1]->saveStoryCompletion = true;
			if (storyEvents.size() > 1 && storyEvents[storyEvents.size() - 1]->getType() == FK_StoryEvent::ItemType::Credits) {
				storyEvents[storyEvents.size() - 2]->saveStoryCompletion = true;
			}
		}
	}
	void FK_StoryFlowCluster::reset(){
		episodeComplete = false;
		episodeName = std::string();
		episodeDescription.clear();
		episodeLockedDescription.clear();
		episodeRelativePath = std::string();
		configFileName = std::string();
		episodeDirectory = std::string();
		storyIndex = -1;
		for (u32 i = 0; i < storyEvents.size(); ++i){
			delete storyEvents[i];
			storyEvents[i] = NULL;
		}
		storyEvents.clear();
	}

	void FK_StoryFlowCluster::setupFromEpisode(const FK_StoryFlowCluster &newEpisode){
		reset();
		setup(newEpisode.episodeDirectory, newEpisode.episodeRelativePath, newEpisode.configFileName);
	}

	FK_StoryItem* FK_StoryFlowCluster::getNextEvent(){
		saveProgress();
		if (storyIndex < (s32)storyEvents.size()){
			storyIndex += 1;
			return storyEvents[storyIndex];
		}
		else{
			return NULL;
		}
	}
	std::string FK_StoryFlowCluster::getEpisodeName(){
		return episodeName;
	}
	std::string FK_StoryFlowCluster::getEpisodeRelativePath(){
		return episodeRelativePath;
	}

	std::string FK_StoryFlowCluster::getEpisodeFullPath() {
		return episodeDirectory + episodeRelativePath;
	}

	std::vector<std::string>& FK_StoryFlowCluster::getEpisodeDescription(){
		return episodeDescription;
	}

	std::vector<std::string>& FK_StoryFlowCluster::getEpisodeLockedDescription() {
		return episodeLockedDescription;
	}

	void FK_StoryFlowCluster::resetProgress() {
		storyIndex = -1;
		std::ofstream ofile(episodeDirectory + episodeRelativePath + SavefileName);
		ofile << "";
		ofile.close();
	}

	void FK_StoryFlowCluster::saveProgress(){
		std::ofstream ofile(episodeDirectory + episodeRelativePath + SavefileName);
		if (storyIndex >= 0){
			ofile << storyEvents[storyIndex]->configFileName;
		}
		ofile.close();
	}

	void FK_StoryFlowCluster::loadProgress(){
		std::ifstream ifile(episodeDirectory + episodeRelativePath + SavefileName);
		std::string lastPartPlayed = std::string();
		if (ifile.good()){
			while (lastPartPlayed.empty() && ifile.good()){
				std::getline(ifile, lastPartPlayed);
			}
			ifile.close();
			bool saveFileFound = false;
			for (s32 i = 0; i < (s32)storyEvents.size() - 1; ++i){
				//std::cout << storyEvents[i]->configFileName << " " << lastPartPlayed << std::endl;
				if (storyEvents[i]->configFileName == lastPartPlayed){
					storyIndex = i;
					saveFileFound = true;
					episodeComplete = false;
					while (storyIndex >= -1 && storyEvents[storyIndex + 1]->getType() == FK_StoryItem::ItemType::Match){
						storyIndex -= 1;
					}
				}
			}
			if (!storyEvents.empty()){
				if (storyEvents[storyEvents.size() - 1]->configFileName == lastPartPlayed){
					episodeComplete = true;
					storyIndex = -1;
				}
			}
			if (!saveFileFound){
				storyIndex = -1;
			}
		}
	}

	f32 FK_StoryFlowCluster::getCompletionPercentage(){
		if (episodeComplete){
			return 100.f;
		}
		if (storyEvents.empty() || storyIndex < 0){
			return 0.f;
		}
		u32 size = storyEvents.size();
		if (storyEvents[size - 1]->getType() == FK_StoryItem::ItemType::Credits) {
			size -= 1;
		}
		if (size == 0) {
			return 0.f;
		}
		return 100.f * (f32)(storyIndex + 1) / size;
	}

	bool FK_StoryFlowCluster::isLastEvent(){
		return storyIndex >= (s32)storyEvents.size() + 1;
	}

	FK_StoryCredits::FK_StoryCredits()
	{
		type = FK_StoryItem::ItemType::Credits;
	}

	void FK_StoryCredits::setup(std::string filename, std::string episodeDirectory, std::string episodeRelativePath)
	{
		configFileName = filename;
		episodePath = episodeRelativePath;
		storyDirectory = episodeDirectory;
	}

}