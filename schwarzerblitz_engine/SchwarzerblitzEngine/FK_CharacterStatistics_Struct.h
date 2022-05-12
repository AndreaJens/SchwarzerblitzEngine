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

#ifndef FK_CHARASTATS_STRUCT
#define FK_CHARASTATS_STRUCT

#include<irrlicht.h>
#include<string>
#include<map>
#include<fstream>

using namespace irr;

namespace fk_engine{

	struct FK_CharacterVSStatistics;

	struct FK_CharacterStatistics{
		std::string CharacterFileStartTag = "#character";
		std::string CharacterFileEndTag = "#character_end";
		std::string CharacterDirTag = "#name";
		std::string CharacterUsageTag = "#usage";
		std::string CharacterArcadeTag = "#arcade";
		std::string CharacterStoryTag = "#story";
		std::string CharacterDamageDoneTag = "#dmg_done";
		std::string CharacterDamageReceivedTag = "#dmg_received";
		std::string CharacterTimeOutWinsTag = "#time_out_wins";
		std::string CharacterTimeOutLossesTag = "#time_out_losses";
		std::string CharacterMatchWinsTag = "#wins";
		std::string CharacterMatchLostTag = "#losses";
		std::string CharacterRoundsWonTag = "#rounds_won";
		std::string CharacterRoundsLostTag = "#rounds_lost";
		std::string CharacterPerfectWinsTag = "#perfect_wins";
		std::string CharacterPerfectLossesTag = "#perfect_losses";
		std::string CharacterRingoutWinsTag = "#ringout_wins";
		std::string CharacterRingoutLossesTag = "#ringout_losses";
		std::string CharacterDrawRoundTag = "#draws";
		std::string CharacterContinueTag = "#continue";
		std::string VSCharactersTableStartTag = "#VS_statistics";
		std::string VSCharactersTableEndTag = "#VS_statistics_end";
		std::string characterName = std::string();
		u32 numberOfMatchesWon = 0;
		u32 numberOfMatchesLost = 0;
		u32 numberOfContinues = 0;
		u32 numberOfRoundsWon = 0;
		u32 numberOfRoundsLost = 0;
		u32 numberOfPerfectWins = 0;
		u32 numberOfPerfectLosses = 0;
		u32 numberOfRingoutPerformed = 0;
		u32 numberOfRingoutSuffered = 0;
		u32 numberOfTimeoutWins = 0;
		u32 numberOfTimeoutLosses = 0;
		double totalDamageDone = 0;
		double totalDamageSuffered = 0;
		u32 numberOfBeatenArcadeMode = 0;
		u32 numberOfBeatenStoryMode = 0;
		u32 numberOfFreeMatchesPlayed = 0;
		u32 numberOfDraws = 0;
		std::map<std::string, FK_CharacterVSStatistics> VsRecords;
	};
	struct FK_CharacterVSStatistics{
		std::string CharacterFileStartTag = "#vs_start";
		std::string CharacterFileEndTag = "#vs_end";
		std::string CharacterDirTag = "#vs";
		std::string CharacterDamageDoneTag = "#dmg_done";
		std::string CharacterDamageReceivedTag = "#dmg_received";
		std::string CharacterTimeOutWinsTag = "#time_out_wins";
		std::string CharacterTimeOutLossesTag = "#time_out_losses";
		std::string CharacterMatchWinsTag = "#wins";
		std::string CharacterMatchLostTag = "#losses";
		std::string CharacterRoundsWonTag = "#rounds_won";
		std::string CharacterRoundsLostTag = "#rounds_lost";
		std::string CharacterPerfectWinsTag = "#perfect_wins";
		std::string CharacterPerfectLossesTag = "#perfect_losses";
		std::string CharacterRingoutWinsTag = "#ringout_wins";
		std::string CharacterRingoutLossesTag = "#ringout_losses";
		std::string CharacterDrawRoundTag = "#draws";
		std::string characterName = std::string();
		u32 numberOfMatchesWon = 0;
		u32 numberOfMatchesLost = 0;
		u32 numberOfRoundsWon = 0;
		u32 numberOfRoundsLost = 0;
		u32 numberOfPerfectWins = 0;
		u32 numberOfPerfectLosses = 0;
		u32 numberOfRingoutPerformed = 0;
		u32 numberOfRingoutSuffered = 0;
		u32 numberOfTimeoutWins = 0;
		u32 numberOfTimeoutLosses = 0;
		double totalDamageDone = 0;
		double totalDamageSuffered = 0;
		u32 numberOfDraws = 0;
	};
	namespace fk_saveFileAddons{
		// write to file
		void writeCharacterStatisticsToFile(FK_CharacterStatistics &stats, std::ofstream &outputFile);
		// write VS stats to file
		void writeCharacterVSStatisticsToFile(FK_CharacterVSStatistics &stats, std::ofstream &outputFile, std::string indentation = "\t");
		// read from file
		bool readCharacterStatisticsFromFile(FK_CharacterStatistics &stats, std::ifstream &inputFile);
		// read from file
		bool readCharacterVSStatisticsFromFile(FK_CharacterVSStatistics &stats, std::ifstream &inputFile);
		// const tags for creating the files
		const std::string GlobalStatisticsForSaveFileTag = "#GLOBAL";
		const std::string CollectedTagsForSaveFileTags = "#EXTRA";
		const std::string StoryEpisodesClearedForSaveFileTag = "#STORY_EPISODES_CLEAR";
		const std::string ChallengeClearedForSaveFileTag = "#CHALLENGES_CLEAR";
		const std::string CharacterStatisticsForSaveFileTag = "#CHARACTERS";
		const std::string AILevelTagForArcade = "#AI_LEVEL";
		const std::string AILevelTagForSurvivalArcade = "#AI_LEVEL_SURVIVAL";
		const std::string AILevelTagForTimeAttackArcade = "#AI_LEVEL_TIME_ATTACK";
		const std::string TutorialForSaveFileTag = "#CONFIG_PROMPTS";
		const std::string UnlockAllContentsTag = "iwantitalliwantitalliwantitnow";
		const std::string UnlockAllCharactersTag = "everyoneisherethefeastcansoonbegin";
		const std::string UnlockAllStagesTag = "allaroundtheworld";
		const std::string UnlockExtraCostumesTag = "emperorsnewclothes";
	};
}

#endif