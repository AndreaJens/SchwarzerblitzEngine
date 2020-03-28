#ifndef FK_REWARD_STRUCT
#define FK_REWARD_STRUCT

#include<irrlicht.h>
#include<string>
#include<map>
#include<fstream>
#include<vector>
#include"FK_CharacterStatistics_struct.h"

using namespace irr;

namespace fk_engine{

	struct FK_RewardUnlockStruct{
	public:
		std::map<std::string, FK_CharacterStatistics> characterStats;
		std::map<u32, u32> arcadeWins;
		std::map<u32, std::pair<std::string, u32>> survivalRecords;
		std::map<u32, std::pair<std::string, u32>> timeAttackRecords;
		std::vector<std::string> storyEpisodesCleared;
		std::vector<std::string> challengesCleared;
		std::vector<std::string> specialArcadeTags;
		bool isTutorialCleared = false;
		std::vector<std::string> unlockedCharacters;
	};

	struct FK_RewardCondition{
	public:
		enum RewardCondition : s32{
			NoRewardCondition = -1,
			NoOfArcadeBeaten = 0,
			NoOfFreeMatchPlayed = 1,
			NoOfTimeoutWins = 2,
			NoOfRingoutWins = 3,
			NoOfPerfectWins = 4,
			ArcadeBeatenWithSpecificCharacter = 5,
			ArcadeBeatenAtSpecificLevel = 6,
			SpecificStoryEpisodeBeaten = 8,
			SpecificChallengeBeaten = 11,
			NoOfContinueInArcadeMode = 7,
			TutorialCompleted = 9,
			CharacterUnlocked = 10,
			SpecialArcadeTagUnlocked = 12,
		};
		const std::string ConditionStartTag = "#condition";
		const std::string ConditionEndTag = "#condition_end";
	private:
		std::map<std::string, RewardCondition> RewardConditionStringParamsTags;
		std::map<std::string, RewardCondition> RewardConditionNumericParamsTags;
		std::map<std::string, RewardCondition> RewardConditionNoParamsTags;
		std::string additionalStringParameter = std::string();
		s32 additionalNumericParameter = 0;
		RewardCondition condition = RewardCondition::NoRewardCondition;
	public:
		bool readFromFile(std::ifstream& inputFile);
		bool isFulfilled(FK_RewardUnlockStruct &unlockingParameters);
		FK_RewardCondition& operator=(FK_RewardCondition &rewardCondition);
		FK_RewardCondition();
	};

	struct FK_Reward{
	public:
		static enum RewardType : s32{
			NoRewardType = -1,
			Character = 0,
			Stage = 1,
			Music = 2,
			Illustration = 3,
			Costume = 4,
			GameMode = 5,
			StartScreenBackground = 6,
			StoryEpisode = 7,
			Challenge = 8,
			Diorama = 9,
			Ending = 10,
		};
	private:
		const std::string RewardFileStartTag = "#reward";
		const std::string RewardFileEndTag = "#reward_end";
		const std::string ConditionStartTag = "#condition";
		std::map<std::string, RewardType> RewardTypeTags;
	public:
		RewardType type = RewardType::NoRewardType;
		std::vector<FK_RewardCondition> rewardConditions;
		std::string rewardKey = std::string();
		FK_Reward& operator=(FK_Reward &reward);
		FK_Reward();
		bool readFromFile(std::ifstream& inputFile);
		bool rewardIsUnlocked(FK_RewardUnlockStruct &unlockingParameters);
	};
}

#endif