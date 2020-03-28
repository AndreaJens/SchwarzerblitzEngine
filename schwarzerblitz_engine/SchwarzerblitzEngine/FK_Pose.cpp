#include "FK_Pose.h"

namespace fk_engine{
	FK_Pose::FK_Pose(std::string newAnimationName, int new_startingFrame, int new_endingFrame, bool looping)
	{
		animationName = newAnimationName;
		startingFrame = new_startingFrame;
		endingFrame = new_endingFrame;
		poseLoopFlag = looping;
	}

	int FK_Pose::getStartingFrame()
	{
		return startingFrame;
	}


	int FK_Pose::getEndingFrame()
	{
		return endingFrame;
	}


	void FK_Pose::setStartingFrame(int frame)
	{
		startingFrame = frame;
	}


	void FK_Pose::setEndingFrame(int frame)
	{
		endingFrame = frame;
	}


	bool FK_Pose::isLooping()
	{
		return poseLoopFlag;
	}


	void FK_Pose::setLoop(bool loopFlag)
	{
		poseLoopFlag = loopFlag;
	}

	void FK_Pose::setAnimationName(std::string name){
		animationName = name;
	}

	std::string FK_Pose::getAnimationName(){
		return animationName;
	}

}