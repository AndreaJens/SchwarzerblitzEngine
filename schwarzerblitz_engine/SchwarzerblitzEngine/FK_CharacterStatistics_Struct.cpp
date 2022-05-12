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

#include"FK_CharacterStatistics_Struct.h"

namespace fk_engine{
	namespace fk_saveFileAddons{
		// write to file
		void writeCharacterStatisticsToFile(FK_CharacterStatistics &stats, std::ofstream &outputFile){
			outputFile << stats.CharacterFileStartTag << std::endl;
			outputFile << stats.CharacterDirTag << "\t" << stats.characterName << std::endl;
			outputFile << stats.CharacterUsageTag << "\t" << stats.numberOfFreeMatchesPlayed << std::endl;
			outputFile << stats.CharacterArcadeTag << "\t" << stats.numberOfBeatenArcadeMode << std::endl;
			outputFile << stats.CharacterStoryTag << "\t" << stats.numberOfBeatenStoryMode << std::endl;
			outputFile << stats.CharacterContinueTag << "\t" << stats.numberOfContinues << std::endl;
			outputFile << stats.CharacterDamageDoneTag << "\t" << stats.totalDamageDone << std::endl;
			outputFile << stats.CharacterDamageReceivedTag << "\t" << stats.totalDamageSuffered << std::endl;
			outputFile << stats.CharacterMatchWinsTag << "\t" << stats.numberOfMatchesWon << std::endl;
			outputFile << stats.CharacterMatchLostTag << "\t" << stats.numberOfMatchesLost << std::endl;
			outputFile << stats.CharacterRoundsWonTag << "\t" << stats.numberOfRoundsWon << std::endl;
			outputFile << stats.CharacterRoundsLostTag << "\t" << stats.numberOfRoundsLost << std::endl;
			outputFile << stats.CharacterTimeOutWinsTag << "\t" << stats.numberOfTimeoutWins << std::endl;
			outputFile << stats.CharacterTimeOutLossesTag << "\t" << stats.numberOfTimeoutLosses << std::endl;
			outputFile << stats.CharacterPerfectWinsTag << "\t" << stats.numberOfPerfectWins << std::endl;
			outputFile << stats.CharacterPerfectLossesTag << "\t" << stats.numberOfPerfectLosses << std::endl;
			outputFile << stats.CharacterRingoutWinsTag << "\t" << stats.numberOfRingoutPerformed << std::endl;
			outputFile << stats.CharacterRingoutLossesTag << "\t" << stats.numberOfRingoutSuffered << std::endl;
			outputFile << stats.CharacterDrawRoundTag << "\t" << stats.numberOfDraws << std::endl;
			/*outputFile << stats.VSCharactersTableStartTag << std::endl;
			for each (std::pair<std::string, FK_CharacterVSStatistics> entry in stats.VsRecords){
				writeCharacterVSStatisticsToFile(entry.second, outputFile);
			}
			outputFile << stats.VSCharactersTableEndTag << std::endl << std::endl;*/
			outputFile << stats.CharacterFileEndTag << std::endl << std::endl;
		}
		// write to file
		void writeCharacterVSStatisticsToFile(FK_CharacterVSStatistics &stats, std::ofstream &outputFile, std::string indentation){
			outputFile << indentation << stats.CharacterFileStartTag << std::endl;
			outputFile << indentation << stats.CharacterDirTag << "\t" << stats.characterName << std::endl;
			outputFile << indentation << stats.CharacterDamageDoneTag << "\t" << stats.totalDamageDone << std::endl;
			outputFile << indentation << stats.CharacterDamageReceivedTag << "\t" << stats.totalDamageSuffered << std::endl;
			outputFile << indentation << stats.CharacterMatchWinsTag << "\t" << stats.numberOfMatchesWon << std::endl;
			outputFile << indentation << stats.CharacterMatchLostTag << "\t" << stats.numberOfMatchesLost << std::endl;
			outputFile << indentation << stats.CharacterRoundsWonTag << "\t" << stats.numberOfRoundsWon << std::endl;
			outputFile << indentation << stats.CharacterRoundsLostTag << "\t" << stats.numberOfRoundsLost << std::endl;
			outputFile << indentation << stats.CharacterTimeOutWinsTag << "\t" << stats.numberOfTimeoutWins << std::endl;
			outputFile << indentation << stats.CharacterTimeOutLossesTag << "\t" << stats.numberOfTimeoutLosses << std::endl;
			outputFile << indentation << stats.CharacterPerfectWinsTag << "\t" << stats.numberOfPerfectWins << std::endl;
			outputFile << indentation << stats.CharacterPerfectLossesTag << "\t" << stats.numberOfPerfectLosses << std::endl;
			outputFile << indentation << stats.CharacterRingoutWinsTag << "\t" << stats.numberOfRingoutPerformed << std::endl;
			outputFile << indentation << stats.CharacterRingoutLossesTag << "\t" << stats.numberOfRingoutSuffered << std::endl;
			outputFile << indentation << stats.CharacterDrawRoundTag << "\t" << stats.numberOfDraws << std::endl;
			outputFile << indentation << stats.CharacterFileEndTag << std::endl << std::endl;
		}
		// read from file
		bool readCharacterStatisticsFromFile(FK_CharacterStatistics &stats, std::ifstream &inputFile){
			std::string temp;
			while (inputFile >> temp){
				if (temp == stats.CharacterFileEndTag){
					return true;
				}
				else if (temp == stats.CharacterDirTag){
					inputFile >> stats.characterName;
				}
				else if (temp == stats.CharacterUsageTag){
					inputFile >> stats.numberOfFreeMatchesPlayed;
				}
				else if (temp == stats.CharacterArcadeTag){
					inputFile >> stats.numberOfBeatenArcadeMode;
				}
				else if (temp == stats.CharacterStoryTag){
					inputFile >> stats.numberOfBeatenStoryMode;
				}
				else if (temp == stats.CharacterContinueTag){
					inputFile >> stats.numberOfContinues;
				}
				else if (temp == stats.CharacterDamageDoneTag){
					inputFile >> stats.totalDamageDone;
				}
				else if (temp == stats.CharacterDamageReceivedTag){
					inputFile >> stats.totalDamageSuffered;
				}
				else if (temp == stats.CharacterMatchWinsTag){
					inputFile >> stats.numberOfMatchesWon;
				}
				else if (temp == stats.CharacterMatchLostTag){
					inputFile >> stats.numberOfMatchesLost;
				}
				else if (temp == stats.CharacterRoundsWonTag){
					inputFile >> stats.numberOfRoundsWon;
				}
				else if (temp == stats.CharacterRoundsLostTag){
					inputFile >> stats.numberOfRoundsLost;
				}
				else if (temp == stats.CharacterDrawRoundTag){
					inputFile >> stats.numberOfDraws;
				}
				else if (temp == stats.CharacterTimeOutWinsTag){
					inputFile >> stats.numberOfTimeoutWins;
				}
				else if (temp == stats.CharacterPerfectWinsTag){
					inputFile >> stats.numberOfPerfectWins;
				}
				else if (temp == stats.CharacterRingoutWinsTag){
					inputFile >> stats.numberOfRingoutPerformed;
				}
				else if (temp == stats.CharacterTimeOutLossesTag){
					inputFile >> stats.numberOfTimeoutLosses;
				}
				else if (temp == stats.CharacterPerfectLossesTag){
					inputFile >> stats.numberOfPerfectLosses;
				}
				else if (temp == stats.CharacterRingoutLossesTag){
					inputFile >> stats.numberOfRingoutSuffered;
				}
				else if (temp == stats.VSCharactersTableStartTag){
					while (inputFile >> temp){
						if (!inputFile){
							return true;
						}
						if (temp == stats.VSCharactersTableEndTag){
							break;
						}
						FK_CharacterVSStatistics tempStat;
						bool successFlag = readCharacterVSStatisticsFromFile(tempStat, inputFile);
						stats.VsRecords[tempStat.characterName] = tempStat;
						if (!successFlag){
							return false;
						}
					}
				}
				if (!inputFile){
					return false;
				}
			}
			return false;
		}
		// read VS from file
		bool readCharacterVSStatisticsFromFile(FK_CharacterVSStatistics &stats, std::ifstream &inputFile){
			std::string temp;
			while (inputFile >> temp){
				if (temp == stats.CharacterFileEndTag){
					return true;
				}
				else if (temp == stats.CharacterDirTag){
					inputFile >> stats.characterName;
				}
				else if (temp == stats.CharacterDamageDoneTag){
					inputFile >> stats.totalDamageDone;
				}
				else if (temp == stats.CharacterDamageReceivedTag){
					inputFile >> stats.totalDamageSuffered;
				}
				else if (temp == stats.CharacterMatchWinsTag){
					inputFile >> stats.numberOfMatchesWon;
				}
				else if (temp == stats.CharacterMatchLostTag){
					inputFile >> stats.numberOfMatchesLost;
				}
				else if (temp == stats.CharacterRoundsWonTag){
					inputFile >> stats.numberOfRoundsWon;
				}
				else if (temp == stats.CharacterRoundsLostTag){
					inputFile >> stats.numberOfRoundsLost;
				}
				else if (temp == stats.CharacterDrawRoundTag){
					inputFile >> stats.numberOfDraws;
				}
				else if (temp == stats.CharacterTimeOutWinsTag){
					inputFile >> stats.numberOfTimeoutWins;
				}
				else if (temp == stats.CharacterPerfectWinsTag){
					inputFile >> stats.numberOfPerfectWins;
				}
				else if (temp == stats.CharacterRingoutWinsTag){
					inputFile >> stats.numberOfRingoutPerformed;
				}
				else if (temp == stats.CharacterTimeOutLossesTag){
					inputFile >> stats.numberOfTimeoutLosses;
				}
				else if (temp == stats.CharacterPerfectLossesTag){
					inputFile >> stats.numberOfPerfectLosses;
				}
				else if (temp == stats.CharacterRingoutLossesTag){
					inputFile >> stats.numberOfRingoutSuffered;
				}
				if (!inputFile){
					return false;
				}
			}
			return false;
		}
	}
}