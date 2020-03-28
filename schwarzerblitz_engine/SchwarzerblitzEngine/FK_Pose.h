#pragma once
#include<string>

namespace fk_engine{

	class FK_Pose
	{
	public:
		FK_Pose(std::string newAnimationName = std::string(), int startFrame = 0, int endFrame = 0, bool looping = false);
		int getStartingFrame();
		int getEndingFrame();
		void setStartingFrame(int frame);
		void setEndingFrame(int frame);
		bool isLooping();
		void setLoop(bool loopFlag);
		void setAnimationName(std::string name);
		std::string getAnimationName();
	private:
		int startingFrame;
		int endingFrame;
		bool poseLoopFlag;
		std::string animationName;
	};

}

