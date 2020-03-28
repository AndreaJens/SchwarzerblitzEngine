#include"FK_Reward.h"
#include"FK_Database.h"

using namespace irr;

namespace fk_engine{

	FK_RewardCondition::FK_RewardCondition(){
		RewardConditionNumericParamsTags = std::map<std::string, RewardCondition>{
			{ "##number_of_arcade_beaten", FK_RewardCondition::RewardCondition::NoOfArcadeBeaten },
			{ "##arcade_beaten_at_AI_level", FK_RewardCondition::RewardCondition::ArcadeBeatenAtSpecificLevel },
			{ "##total_number_of_continue", FK_RewardCondition::RewardCondition::NoOfContinueInArcadeMode },
			{ "##total_number_of_free_matches", FK_RewardCondition::RewardCondition::NoOfFreeMatchPlayed },
			{ "##total_number_of_perfect_wins", FK_RewardCondition::RewardCondition::NoOfPerfectWins },
			{ "##total_number_of_ringout_wins", FK_RewardCondition::RewardCondition::NoOfRingoutWins },
			{ "##arcade_beaten_by", FK_RewardCondition::RewardCondition::ArcadeBeatenWithSpecificCharacter },
		};
		RewardConditionStringParamsTags = std::map<std::string, RewardCondition>{
			{ "##arcade_beaten_by", FK_RewardCondition::RewardCondition::ArcadeBeatenWithSpecificCharacter },
			{ "##story_episode_beaten", FK_RewardCondition::RewardCondition::SpecificStoryEpisodeBeaten},
			{ "##challenge_beaten", FK_RewardCondition::RewardCondition::SpecificChallengeBeaten },
			{ "##character_unlocked", FK_RewardCondition::RewardCondition::CharacterUnlocked },
			{ "##special_arcade_tag_unlocked", FK_RewardCondition::RewardCondition::SpecialArcadeTagUnlocked},
		};
		RewardConditionNoParamsTags = std::map<std::string, RewardCondition>{
			{"##tutorial_cleared", FK_RewardCondition::RewardCondition::TutorialCompleted}
		};
	}

	FK_RewardCondition& FK_RewardCondition::operator = (FK_RewardCondition &reward){
		condition = reward.condition;
		additionalNumericParameter = reward.additionalNumericParameter;
		additionalStringParameter = reward.additionalStringParameter;
		return *this;
	}

	bool FK_RewardCondition::isFulfilled(FK_RewardUnlockStruct &unlockingParameters){
		switch (condition){
		case RewardCondition::ArcadeBeatenAtSpecificLevel:
			if ((s32)unlockingParameters.arcadeWins.count(additionalNumericParameter) > 0 && 
				unlockingParameters.arcadeWins.at(additionalNumericParameter) > 0){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::NoOfArcadeBeaten:
			if ((s32)unlockingParameters.characterStats.at(fk_constants::GlobalStatsIdentifier).numberOfBeatenArcadeMode >= 
				additionalNumericParameter){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::SpecificStoryEpisodeBeaten:
			if (std::find(unlockingParameters.storyEpisodesCleared.begin(), unlockingParameters.storyEpisodesCleared.end(), 
				additionalStringParameter + "\\") != unlockingParameters.storyEpisodesCleared.end()){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::SpecialArcadeTagUnlocked:
			if (std::find(unlockingParameters.specialArcadeTags.begin(), unlockingParameters.specialArcadeTags.end(),
				additionalStringParameter) != unlockingParameters.specialArcadeTags.end()) {
				return true;
			}
			else {
				return false;
			}
			break;
		case RewardCondition::SpecificChallengeBeaten:
			if (std::find(unlockingParameters.challengesCleared.begin(), unlockingParameters.challengesCleared.end(),
				additionalStringParameter + "\\") != unlockingParameters.challengesCleared.end()){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::ArcadeBeatenWithSpecificCharacter:
			if ((s32)unlockingParameters.characterStats.count(additionalStringParameter) > 0 &&
				(s32)unlockingParameters.characterStats.at(additionalStringParameter).numberOfBeatenArcadeMode >= 
				additionalNumericParameter){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::NoOfContinueInArcadeMode:
			if ((s32)unlockingParameters.characterStats.at(fk_constants::GlobalStatsIdentifier).numberOfContinues >= 
				additionalNumericParameter){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::NoOfFreeMatchPlayed:
			if ((s32)unlockingParameters.characterStats.at(fk_constants::GlobalStatsIdentifier).numberOfFreeMatchesPlayed >= 
				additionalNumericParameter){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::NoOfPerfectWins:
			if ((s32)unlockingParameters.characterStats.at(fk_constants::GlobalStatsIdentifier).numberOfPerfectWins >= 
				additionalNumericParameter){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::NoOfRingoutWins:
			if ((s32)unlockingParameters.characterStats.at(fk_constants::GlobalStatsIdentifier).numberOfRingoutPerformed >= 
				additionalNumericParameter){
				return true;
			}
			else{
				return false;
			}
			break;
		case RewardCondition::TutorialCompleted:
			if (unlockingParameters.isTutorialCleared){
				return true;
			}
			break;
		case RewardCondition::CharacterUnlocked:
			if (std::find(unlockingParameters.unlockedCharacters.begin(), unlockingParameters.unlockedCharacters.end(), 
				additionalStringParameter) != unlockingParameters.unlockedCharacters.end()){
				return true;
			}
			else{
				return false;
			}
			break;
		default:
			return false;
			break;
		}
		return false;
	}

	bool FK_RewardCondition::readFromFile(std::ifstream& inputFile){
		std::string temp;
		while (inputFile >> temp){
			if (temp == ConditionEndTag){
				return true;
			}
			if (RewardConditionStringParamsTags.count(temp) > 0){
				condition = RewardConditionStringParamsTags.at(temp);
				inputFile >> additionalStringParameter;
				std::string param = additionalStringParameter;
			}
			if (RewardConditionNumericParamsTags.count(temp) > 0){
				condition = RewardConditionNumericParamsTags.at(temp);
				inputFile >> additionalNumericParameter;
			}
			if (RewardConditionNoParamsTags.count(temp) > 0){
				condition = RewardConditionNoParamsTags.at(temp);
			}
		};
		return false;
	};

	FK_Reward::FK_Reward(){
		RewardTypeTags = std::map<std::string, RewardType>{
			{ "##character", FK_Reward::RewardType::Character },
			{ "##stage", FK_Reward::RewardType::Stage },
			{ "##game_mode", FK_Reward::RewardType::GameMode },
			{ "##music", FK_Reward::RewardType::Music },
			{ "##picture", FK_Reward::RewardType::Illustration },
			{ "##start_background", FK_Reward::RewardType::StartScreenBackground },
			{ "##costume", FK_Reward::RewardType::Costume },
			{ "##story_episode", FK_Reward::RewardType::StoryEpisode },
			{ "##challenge", FK_Reward::RewardType::Challenge },
			{ "##diorama", FK_Reward::RewardType::Diorama },
		};
		rewardConditions.clear();
	}
	FK_Reward& FK_Reward::operator = (FK_Reward &reward){
		type = reward.type;
		rewardConditions = reward.rewardConditions;
		rewardKey = reward.rewardKey;
		return *this;
	}
	bool FK_Reward::readFromFile(std::ifstream& inputFile){
		std::string temp;
		while (inputFile >> temp){
			if (temp == RewardFileEndTag){
				return true;
			}
			else if (RewardTypeTags.count(temp) > 0){
				type = RewardTypeTags.at(temp);
				inputFile >> rewardKey;
			}
			else if (temp == ConditionStartTag){
				FK_RewardCondition condition;
				if (condition.readFromFile(inputFile)){
					rewardConditions.push_back(condition);
				}
			}
		};
		return false;
	};
	bool FK_Reward::rewardIsUnlocked(FK_RewardUnlockStruct &unlockingParameters){
		for each (FK_RewardCondition condition in rewardConditions){
			if (!(condition.isFulfilled(unlockingParameters))){
				return false;
			}
		}
		return true;
	}
}