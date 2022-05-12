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

#ifndef FK_STORYFLOWCLUSTER_CLASS
#define FK_STORYFLOWCLUSTER_CLASS

#include<irrlicht.h>
#include<string>
#include"FK_Character.h"
#include"FK_SceneGame.h"

using namespace irr;

namespace fk_engine{

	class FK_StoryItem{
	public:
		enum ItemType{
			NoType,
			Cutscene,
			Match,
			Credits,
		};
	public:
		FK_StoryItem();
		virtual void setup(std::string filename, std::string episodeDirectory, std::string episodeRelativePath) = 0;
		virtual ItemType getType();
	public:
		ItemType type;
		std::string storyDirectory;
		std::string configFileName;
		std::string episodePath;
		bool playCredits;
		bool returnToStoryMenu;
		bool saveStoryCompletion;
	};

	class FK_StoryMatch : public FK_StoryItem{
	public:
		enum MatchWinCondition {
			Normal,
			Survive,
			Percentage,
			WinBeforeTimer,
			Poison,
			Ringout,
		};
	protected:
		const std::string NumberOfRoundsFileKey = "#NO_OF_ROUNDS";
		const std::string NumberOfRoundsPlayerFileKey = "#NO_OF_ROUNDS_PLAYER";
		const std::string NumberOfRoundsOpponentFileKey = "#NO_OF_ROUNDS_OPPONENT";
		const std::string PlayerWinQuoteFileKey = "#PLAYER_WIN_QUOTE";
		const std::string OpponentWinQuoteFileKey = "#OPPONENT_WIN_QUOTE";
		const std::string PlayerIntroQuoteFileKey = "#PLAYER_INTRO_QUOTE";
		const std::string OpponentIntroQuoteFileKey = "#OPPONENT_INTRO_QUOTE";
		const std::string PlayerOutfitIdFileKey = "#PLAYER_OUTFIT_ID";
		const std::string OpponentOutfitIdFileKey = "#OPPONENT_OUTFIT_ID";
		const std::string PlayerPhysDamageMultiplierTag = "#PLAYER_R_DAMAGE_MULTIPLIER";
		const std::string PlayerBulletDamageMultiplierTag = "#PLAYER_R_B_DAMAGE_MULTIPLIER";
		const std::string OpponentPhysDamageMultiplierTag = "#OPPONENT_R_DAMAGE_MULTIPLIER";
		const std::string OpponentBulletDamageMultiplierTag = "#OPPONENT_R_B_DAMAGE_MULTIPLIER";
		const std::string StageFileKey = "#STAGE";
		const std::string OpponentRelativePathKey = "#OPPONENT";
		const std::string PlayerRelativePathKey = "#PLAYER";
		const std::string AILevelFileKey = "#AI_LEVEL";
		const std::string OpponentLifeMultiplierFileKey = "#OPPONENT_LIFE_MULTIPLIER";
		const std::string PlayerLifeMultiplierFileKey = "#PLAYER_LIFE_MULTIPLIER";
		const std::string OpponentTriggerRegenFileKey = "#OPPONENT_TRIGGER_REGEN_TIME";
		const std::string PlayerTriggerRegenFileKey = "#PLAYER_TRIGGER_REGEN_MULTIPLIER";
		const std::string OpponentLifePosionKey = "#OPPONENT_LIFE_POISON";
		const std::string PlayerLifePoisonKey = "#PLAYER_LIFE_POISON";
		const std::string DialogueInBattleTag = "#DIALOGUE_IN_BATTLE";
		const std::string TimerFileKey = "#TIMER";
		const std::string RingoutFileKey = "#NO_RINGOUT";
		const std::string WinConditionFileKey = "#WIN_CONDITION";
	public:
		FK_StoryMatch();
		virtual void setup(std::string filename, std::string episodeDirectory, std::string episodeRelativePath);
	public:
		FK_SceneGame::FK_AdditionalSceneGameOptions additionalOptions;
		std::string stagePath;
		std::string playerPath;
		std::string opponentPath;
		std::string playerIntroQuote;
		std::string opponentIntroQuote;
		std::string playerWinQuote;
		std::string opponentWinQuote;
		std::string dialogueInBattleFilename;
		std::string playerOutfitName;
		std::string opponentOutfitName;
		u32 playerOutfitId;
		u32 opponentOutfitId;
		s32 AI_level;
		std::pair<MatchWinCondition, f32> winCondition;
	};

	class FK_StoryEvent : public FK_StoryItem{
	public:
		FK_StoryEvent();
		virtual void setup(std::string filename, std::string episodeDirectory, std::string episodeRelativePath);
		bool allowMenu = true;
		bool saveWhenSkipped = false;
		bool fadeWhenEnding = true;
	};

	class FK_StoryCredits : public FK_StoryItem {
	public:
		FK_StoryCredits();
		virtual void setup(std::string creditsConfigFileName, std::string episodeDirectory, std::string episodeRelativePath);
	};

	// in-match dialogue
	struct FK_InMatchDialogue{
	public:
		struct FK_MatchDialogue{
		public:
			std::string speakerName = std::string();
			std::string sentence = std::string();
			u32 timeStart = 0;
			u32 timeEnd = 0;
		};
	private:
		std::string dialogueBeginTag = "#BEGIN";
		std::string dialogueEndTag = "#END";
	public:
		void readFromFile(std::string filename);
		std::vector<FK_MatchDialogue> dialogueLines;
	};

	class FK_StoryFlowCluster{
	private:
		const std::string SavefileName = "status.txt";
		const std::string EpisodeNameTag = "#EPISODE_NAME";
		const std::string EpisodeDescriptionTag = "#DESCRIPTION";
		const std::string EpisodeDescriptionEndTag = "#DESCRIPTION_END";
		const std::string EpisodeLockedDescriptionTag = "#LOCKED_DESCRIPTION";
		const std::string EpisodeLockedDescriptionEndTag = "#LOCKED_DESCRIPTION_END";
		const std::string MatchEventTag = "#MATCH";
		const std::string DialogueEventTag = "#EVENT";
		const std::string CreditsTag = "#CREDITS";
	public:
		FK_StoryFlowCluster();
		FK_StoryFlowCluster(const FK_StoryFlowCluster&);
		~FK_StoryFlowCluster();
		FK_StoryFlowCluster& operator= (const FK_StoryFlowCluster&);
		void setupFromEpisode(const FK_StoryFlowCluster &);
		void setup(std::string new_episodeDirectory, std::string new_episodeRelativePath, std::string configurationFilename);
		void reset();
		void loadProgress();
		void resetProgress();
		void saveProgress();
		f32 getCompletionPercentage();
		FK_StoryItem* getNextEvent();
		std::string getEpisodeName();
		std::string getEpisodeRelativePath();
		std::string getEpisodeFullPath();
		std::vector<std::string>& getEpisodeDescription();
		std::vector<std::string>& getEpisodeLockedDescription();
		bool isLastEvent();
	private:
		std::vector<FK_StoryItem*> storyEvents;
		s32 storyIndex;
		std::string episodeRelativePath;
		std::string episodeDirectory;
		std::string configFileName;
		std::string episodeName;
		std::vector<std::string> episodeDescription;
		std::vector<std::string> episodeLockedDescription;
		bool episodeComplete;
	};
}

#endif