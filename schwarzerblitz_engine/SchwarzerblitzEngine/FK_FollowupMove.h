#ifndef FK_FOLLOWUPMOVE_CLASS
#define FK_FOLLOWUPMOVE_CLASS

#include<irrlicht.h>
#include<string>

namespace fk_engine{
	class FK_FollowupMove
	{
	public:
		FK_FollowupMove();
		int getInputWindowStartingFrame();
		int getInputWindowEndingFrame();
		std::string getName();
		void setInputWindowStartingFrame(int new_frame);
		void setInputWindowEndingFrame(int new_frame);
		void setName(std::string new_name);
		irr::u32 getMoveId();
		void setMoveId(irr::u32 newId);
	private:
		std::string moveName;
		irr::u32 moveId;
		int inputWindowStartingFrame;
		int inputWindowEndingFrame;
	};
}

#endif

