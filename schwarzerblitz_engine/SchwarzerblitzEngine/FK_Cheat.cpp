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

#include "FK_Cheat.h"
#include <map>
#include<string>
#include "FK_Database.h"

using namespace irr;

namespace fk_engine {

	FK_Cheat::FK_Cheat()
	{
		cheatType = FK_CheatType::NoCheat;
		unlockInputSequence.clear();
	}

	FK_Cheat::FK_Cheat(FK_CheatType type, const std::deque<irr::u32>& inputSequence)
	{
		cheatType = type;
		setInputSequence(inputSequence);
	}

	FK_Cheat::FK_CheatType FK_Cheat::getCheatType() {
		return cheatType;
	}

	void FK_Cheat::setCheatType(FK_CheatType newType) {
		cheatType = newType;
	}

	/* set new buffer */
	void FK_Cheat::setInputSequence(const std::deque<irr::u32>& newBuffer) {
		unlockInputSequence.clear();
		for (u32 i = 0; i < unlockInputSequence.size(); ++i) {
			unlockInputSequence.push_back(newBuffer[i]);
		}
	}

	const std::deque<irr::u32> & FK_Cheat::getInputSequence() {
		return unlockInputSequence;
	}

	/* read from file */
	bool FK_Cheat::readFromFile(std::ifstream& ifile) {
		std::map<std::string, u32> inputMap;

		/* create input map */
		inputMap["2"] = FK_Input_Buttons::Down_Direction;
		inputMap["4"] = FK_Input_Buttons::Left_Direction;
		inputMap["8"] = FK_Input_Buttons::Up_Direction;
		inputMap["6"] = FK_Input_Buttons::Right_Direction;
		inputMap["v"] = FK_Input_Buttons::Down_Direction;
		inputMap["<"] = FK_Input_Buttons::Left_Direction;
		inputMap["^"] = FK_Input_Buttons::Up_Direction;
		inputMap[">"] = FK_Input_Buttons::Right_Direction;
		inputMap["G"] = FK_Input_Buttons::Guard_Button;
		inputMap["T"] = FK_Input_Buttons::Tech_Button;
		inputMap["K"] = FK_Input_Buttons::Kick_Button;
		inputMap["P"] = FK_Input_Buttons::Punch_Button;

		std::string cheatTypeKey = "##type";
		std::string cheatInputBeginKey = "##input";
		std::string cheatInputEndKey = "##input_end";
		std::string cheatEndTagKey = "#end";
		auto cheatTypeMap = std::map<std::string, FK_Cheat::FK_CheatType>{
			{ "characters", FK_CheatType::CheatUnlockAllCharacters },
			{ "stages", FK_CheatType::CheatUnlockAllStages },
			{ "story_episodes", FK_CheatType::CheatUnlockAllStoryEpisodes },
			{ "costumes", FK_CheatType::CheatUnlockAllCostumes },
			{ "clear", FK_CheatType::NoCheat },
			{ "bigheads", FK_CheatType::CheatBobbleHead },
			{ "moongravity", FK_CheatType::CheatMoonGravity },
			{ "wireframe", FK_CheatType::CheatWireframe },
			{ "trigger_party", FK_CheatType::CheatTriggerFest },
			{ "extra_costumes", FK_CheatType::CheatUnlockExtraCostumes },
		};
		std::string stringToParse = std::string();
		while(ifile >> stringToParse){
			if (stringToParse == cheatEndTagKey) {
				return true;
			}
			else if (stringToParse == cheatTypeKey) {
				ifile >> stringToParse;
				if (cheatTypeMap.count(stringToParse) > 0) {
					cheatType = cheatTypeMap.at(stringToParse);
				}
			}
			/* next, parse the INPUT required for the cheat */
			else if (stringToParse == cheatInputBeginKey) {
				unlockInputSequence.clear();
				u32 lastButton = 0;
				u32 button = 0;
				bool combination = false;
				while (!ifile.eof()) {
					lastButton = button;
					std::string temp;
					ifile >> temp;
					/* if the INPUT end tag is found, break cycle*/
					if (temp == cheatInputEndKey) {
						break;
					}
					else {
						if (inputMap.count(temp) > 0) {
							button = inputMap[temp];
							unlockInputSequence.push_back(button);
						}
					}
				}
			}
		}
		return false;
	}
}
