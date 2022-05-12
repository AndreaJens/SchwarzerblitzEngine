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
		enum RewardType : s32{
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