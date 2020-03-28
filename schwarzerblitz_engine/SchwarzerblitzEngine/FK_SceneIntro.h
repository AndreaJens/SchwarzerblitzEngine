#ifndef FK_SCENEINTRO_CLASS
#define FK_SCENEINTRO_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include"FK_SceneWithInput.h"
#include"Irrlicht_TMovie.h"

using namespace irr;

namespace fk_engine{

	class FK_SceneIntro : public FK_SceneWithInput{
	protected:
		struct introFrame{
			video::ITexture* frameTexture;
			u32 duration_ms;
			u32 fadeInDuration_ms;
			u32 fadeOutDuration_ms;
			bool canBeSkipped;
			u32 fps;
			bool isVideoFrame;
			TMovie* videoFile;
			std::string resourceFileName;
		};
		enum FK_SceneIntroFramePhase : s32{
			IFP_fadein = -1,
			IFP_show = 0,
			IFP_fadeout = 1,
			IFP_hidden = -2,
		};
	protected:
		const std::string NewVideoFrameKey = "#VIDEO_FRAME";
		const std::string VideoFrameFPS = "#FPS";
		const std::string NewFrameFileKey = "#FRAME";
		const std::string FadeInTimeMSFileKey = "#FADEIN";
		const std::string FadeOutTimeMSFileKey = "#FADEOUT";
		const std::string DurationTimeMSFileKey = "#DURATION";
		const std::string EndFrameFileKey = "#END";
		const std::string SkippableFileKey = "#SKIPPABLE";
		const std::string IntroFilesDirectory = "scenes\\intro\\";
		const std::string IntroConfigFileName = "config.ini";
	public:
		FK_SceneIntro();
		FK_SceneIntro(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void initialize();
		void update();
		void draw();
		void dispose();
		bool isRunning();
		FK_SceneType nameId();
	protected:
		void updateInput();
		void readFramesFromFile();
		std::vector<introFrame> frames;
		u32 now;
		u32 then;
		u32 delta_t_ms;
		u32 framesCounter;
		u32 currentFrameIndex;
		u32 currentFrameTime;
		f32 currentFrameOpacity;
		FK_SceneIntroFramePhase currentFramePhase;
		std::string resourcesPath;
		std::string configFileName;
		core::dimension2d<u32> screenSize;
		u32 cycleId;
	};
}
#endif