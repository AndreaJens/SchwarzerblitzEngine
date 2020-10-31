#ifndef FK_ARCADECLUSTER_CLASS
#define FK_ARCADECLUSTER_CLASS

#include<irrlicht.h>
#include<string>
#include"FK_Character.h"

using namespace irr;

namespace fk_engine{

	struct FK_ArcadeProgress {
	public:
		u32 numberOfPerfectRoundsWon = 0;
		u32 numberOfRoundsLost = 0;
		u32 numberOfContinues = 0;
		u32 numberOfTriggersUsed = 0;
		u32 numberOfRingouts = 0;
		u32 numberOfTimeouts = 0;
		u32 totalPlayTimeMs = 0;
		f32 playerEnergyAtEndOfMatch = 1.0f;
		std::vector<std::string> specialTagsUnlocked = std::vector<std::string>();
	};

	struct FK_ArcadeFlow{
	public:
		struct FK_ArcadeDialogue{
		public:
			std::string speakerName = std::string();
			std::string sentence = std::string();
			u32 timeStart = 0;
			u32 timeEnd = 0;
		};
	private:
		std::string climaticBattleStageTag = "#CLIMATIC_BATTLE_STAGE";
		std::string climaticBattleOpponentTag = "#CLIMATIC_BATTLE_OPPONENT";
		std::string climaticBattleIntroQuoteTag = "#CLIMATIC_BATTLE_INTRO_QUOTE";
		std::string climaticBattleIntroQuoteOpponentTag = "#CLIMATIC_BATTLE_INTRO_QUOTE_OPPONENT";
		std::string climaticBattleWinQuoteTag = "#CLIMATIC_BATTLE_WIN_QUOTE";
		std::string climaticBattleWinQuoteOpponentTag = "#CLIMATIC_BATTLE_WIN_QUOTE_OPPONENT";
		std::string climaticBattleDialogueTag = "#CLIMATIC_BATTLE_DIALOGUE";
		std::string climaticBattleDialogueEndTag = "#CLIMATIC_BATTLE_DIALOGUE_END";
		std::string arcadeBossOpponentTag = "#ARCADE_BOSS";
		std::string arcadeBossStageTag = "#ARCADE_BOSS_STAGE";
	public:
		void readFromFile(std::string filename);
		bool hasClimaticBattle = false;
		std::string climaticBattleOpponentPath = std::string();
		u32 climaticBattleOpponentOutfitId = 0;
		std::string climaticBattleOpponentOutfitName = std::string();
		std::string climaticBattleStage = std::string();
		std::vector<FK_ArcadeDialogue> dialogueLines;
		std::string climaticBattleIntroQuote = std::string();
		std::string climaticBattleIntroQuoteOpponent = std::string();
		std::string climaticBattleWinQuote = std::string();
		std::string climaticBattleWinQuoteOpponent = std::string();
		std::string arcadeBossOpponentPath = std::string();
		std::string arcadeBossStagePath = std::string();
		std::string arcadeBossOpponentOutfitName = std::string();
		u32 arcadeBossOpponentOutfitId = 0;
	};

	struct FK_ArcadeCondition {
	private:
		std::string greaterThanSign = ">";
		std::string smallerThanSign = "<";
		std::string numberOfRoundsLostTag = "#rounds_lost";
		std::string numberOfPerfectWinsTag = "#perfect_rounds";
		std::string numberOfContinuesTag = "#continues";
		std::string numberOfRingoutsTag = "#ring_out_wins";
		std::string numberOfTimeoutsTag = "#time_out_wins";
		std::string AIdifficultyTag = "#AI_level";
		std::string specialTagsBegin = "#tags";
		std::string specialTagsEnd = "#tags_end";
		std::string onlyWithCharacterTag = "#only_with_character";
		std::string notWithCharacterTag = "#not_with_character";
	public:
		bool conditionIsFulfilled(FK_ArcadeProgress& currentProgress, u32 AI_level, std::string playerCharacterPath);
		void readFromFile(std::ifstream& inputFile, std::string conditionEndTag = "#CONDITION_END");
	private:
		std::map<std::string, std::pair<u32, bool>> conditionsToFulfill;
		std::vector<std::pair<std::string, bool>> tagsToClear;
		std::vector<std::string> notFoundWithSpecificCharacter;
		std::vector<std::string> onlyFoundWithSpecificCharacter;
	};


	class FK_ArcadeCluster{
	public:
		FK_ArcadeCluster();
		FK_ArcadeCluster(std::string mediaDirectoryPath, std::string new_playerPath, u32 new_outfitId,
			std::string newOutfitName,
			std::string new_stagePath, s32 new_AIlevel);
		std::string getPlayerPath();
		std::string getStagePath();
		std::string getOutfitName();
		u32 getPlayerOutfitId();
		FK_Character::FK_CharacterOutfit getPlayerOutfit();
		s32 getAILevel();
	private:
		std::string playerPath;
		s32 AIlevel;
		u32 outfitId;
		std::string outfitName;
		std::string stagePath;
		FK_Character::FK_CharacterOutfit outfit;
		void parseCharacterMeshFiles(std::string mediaDirectoryPath);
	};

	struct FK_ArcadeSpecialMatch {
	private:
		std::string OpponentOutfitIdFileKey = "#OPPONENT_OUTFIT_ID";
		std::string StageFileKey = "#STAGE";
		std::string OpponentRelativePathKey = "#OPPONENT";
		std::string AILevelFileKey = "#AI_LEVEL_MULTIPLIER";
		std::string RingoutFileKey = "#NO_RINGOUT";
		std::string MatchConditionFileKey = "#CONDITION";
		std::string MatchConditionFileKeyEnd = "#CONDITION_END";
		std::string MatchNumberKey = "#MATCH_NUMBER";
		std::string MatchLoadingTagKey = "#TAGLINE";
		std::string SplashcreenFilenameKey = "#SPLASHSCREEN";
		std::string TagGainedOnWinKey = "#MATCH_ID";
	public:
		void setup(std::ifstream& inputFile, std::string mediaPath, std::string specialMatchEndTag = "#SPECIAL_MATCH_END");
	public:
		FK_ArcadeCluster matchConditions;
		FK_ArcadeCondition condition;
		u32 matchNumber = 3;
		std::string matchTag = std::string();
		std::string matchPreviewSplashScreenName = std::string();
		std::string tagGainedOnWin = std::string();
	};

	struct FK_ArcadeSettings {
	private:
		std::string numberOfMatchesFileTag = "#NUMBER_OF_MATCHES";
		std::string bossCharacter1PathTag = "#ARCADE_BOSS_1";
		std::string bossCharacter1StageTag = "#ARCADE_BOSS_STAGE_1";
		std::string bossCharacter1LoadingScreenFilenameTag = "#ARCADE_BOSS_LOADING_SCREEN_1";
		std::string bossCharacter1LoadingScreenTaglineTag = "#ARCADE_BOSS_LOADING_SCREEN_TAGLINE_1";
		std::string bossCharacter1LifeMultiplierTag = "#ARCADE_BOSS_1_LIFE_MULTIPLIER";
		std::string climaticBattleDefaultTaglineTag = "#CLIMATIC_BATTLE_TAGLINE";
		std::string climaticBattleDefaultStageTag = "#CLIMATIC_BATTLE_DEFAULT_STAGE";
		std::string climaticBattleActivatedTag = "#ALLOW_CLIMATIC_BATTLES";
		std::string specialMatchesActivatedTag = "#ALLOW_SPECIAL_MATCHES";
		std::string climaticBattleLifeMultiplierTag = "#CLIMATIC_BATTLE_LIFE_MULTIPLIER";
		std::string climaticBattleNumberOfRoundsTag = "#CLIMATIC_BATTLE_NUMBER_OF_ROUNDS";
		std::string climaticBattleTimerTag = "#CLIMATIC_BATTLE_TIMER";
		std::string specialMatchTag = "#SPECIAL_MATCH";
		std::string specialMatchEndTag = "#SPECIAL_MATCH_END";
	public:
		u32 numberOfMatches;
		std::string bossCharacter1Path;
		std::string bossCharacter1Stage;
		std::string bossCharacter1LoadingScreenFilename;
		std::string bossCharacter1LoadingScreenTagline;
		std::string climaticBattleDefaultTagline;
		std::string climaticBattleDefaultStage;
		f32 bossCharacter1LifeMultiplier = 1.5f;
		f32 climaticBattleLifeMultiplier = 3.0f;
		s32 climaticBattleNumberOfRounds = -1;
		s32 climaticBattleTimer = -1;
		bool allowClimaticBattle = true;
		bool allowSpecialMatches = false;
		std::vector<FK_ArcadeSpecialMatch> specialMatches;
	public:
		void readFromFile(std::string filename, std::string mediaPath);
	};
}

#endif