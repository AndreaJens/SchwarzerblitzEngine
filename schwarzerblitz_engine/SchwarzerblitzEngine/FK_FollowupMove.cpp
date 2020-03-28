#include "FK_FollowupMove.h"

namespace fk_engine{

	FK_FollowupMove::FK_FollowupMove()
	{
		moveName = std::string();
		inputWindowStartingFrame = 0;
		inputWindowEndingFrame = 0;
		moveId = 0;
	}

	irr::u32 FK_FollowupMove::getMoveId()
	{
		return moveId;
	}

	void FK_FollowupMove::setMoveId(irr::u32 newId)
	{
		moveId = newId;
	}

	int FK_FollowupMove::getInputWindowStartingFrame()
	{
		return inputWindowStartingFrame;
	}

	int FK_FollowupMove::getInputWindowEndingFrame()
	{
		return inputWindowEndingFrame;
	}

	std::string FK_FollowupMove::getName()
	{
		return moveName;
	}

	void FK_FollowupMove::setInputWindowStartingFrame(int new_frame)
	{
		inputWindowStartingFrame = new_frame;
	}

	void FK_FollowupMove::setInputWindowEndingFrame(int new_frame)
	{
		inputWindowEndingFrame = new_frame;
	}

	void FK_FollowupMove::setName(std::string new_name)
	{
		moveName = new_name;
	}

}
