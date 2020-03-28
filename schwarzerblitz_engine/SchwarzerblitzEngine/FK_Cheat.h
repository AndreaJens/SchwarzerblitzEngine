#pragma once
#include<irrlicht.h>
#include<deque>
#include<fstream>

namespace fk_engine {
	class FK_Cheat {
	public:
		enum FK_CheatType : irr::u32 {
			NoCheat = 0,
			CheatUnlockAllCharacters = 1,
			CheatUnlockAllStages = 1 << 1,
			CheatUnlockAllStoryEpisodes = 1 << 2,
			CheatUnlockAllCostumes = 1 << 3,
			CheatTriggerFest = 1 << 28,
			CheatWireframe = 1 << 29,
			CheatMoonGravity = 1 << 30,
			CheatBobbleHead = 1 << 27,
		};
	public:
		FK_Cheat();
		FK_Cheat(FK_CheatType type, const std::deque<irr::u32>& inputSequence);
		const std::deque<irr::u32> & getInputSequence();
		void setInputSequence(const std::deque<irr::u32>& newSequence);
		FK_CheatType getCheatType();
		void setCheatType(FK_CheatType);
		bool readFromFile(std::ifstream& ifile);
	private:
		FK_CheatType cheatType;
		std::deque<irr::u32> unlockInputSequence;
	};
}