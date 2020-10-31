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
		for (int i = 0; i < unlockInputSequence.size(); i++) {
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
