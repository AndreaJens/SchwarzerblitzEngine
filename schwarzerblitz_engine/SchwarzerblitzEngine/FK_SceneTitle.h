#ifndef FK_SCENETITLE_CLASS
#define FK_SCENETITLE_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include"FK_SceneWithInput.h"
#include"FK_SoundManager.h"

#include <SFML\Audio.hpp>

using namespace irr;

namespace fk_engine{

	class FK_SceneTitle : public FK_SceneWithInput{
	protected:
		const std::string TitleFilesDirectory = "scenes\\title\\";
		const std::string TitleConfigFileName = "config.ini";
		const std::string BackgroundPictureFileKey = "#BACKGROUND_PICTURE";
		const std::string BGMFileKey = "#BGM";
		const std::string CaptionTextFileKey = "#CAPTION_TEXT";
		const std::string CaptionColorFileKey = "#CAPTION_COLOR";
		const std::string CaptionPositionFileKey = "#CAPTION_POSITION";
		const std::string CaptionPeriodFileKey = "#CAPTION_PERIOD";
		const std::string AttractModeFileKey = "#ACTIVATE_DEMO_MODE";
		const u32 TransitionTimeMS = 1000;
		const u32 FlashTimeMs = 200;
	public:
		FK_SceneTitle();
		FK_SceneTitle(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void initialize();
		void update();
		void draw();
		void dispose();
		bool isRunning();
		FK_SceneType nameId();
	protected:
		void readConfigFile();
		void updateInput();
		void setupInputForPlayers();
		void setupSoundManager();
		void setupBGM();
		void drawBackground();
		void drawCaption();
		void drawOverlay();
		u32 now;
		u32 then;
		u32 delta_t_ms;
		std::string titleCaption;
		std::string resourcesPath;
		std::string configFileName;
		core::dimension2d<u32> screenSize;
		u32 cycleId;
		u32 transitionCounter;
		u32 flashCounter;
		std::string soundPath;
		FK_SoundManager* soundManager;
		/* BGM */
		std::string titleBGMName;
		f32 titleBGMvolume;
		f32 titleBGMpitch;
		sf::Music titleBGM;
		/* fonts and graphical elements */
		gui::IGUIFont *font;
		video::ITexture* backgroundTex;
		bool anyKeyPressed;
		bool cheatFlashEffect;
		u32 captionTimeMS;
		core::position2df captionPosition;
		std::wstring captionText;
		u32 captionPeriodDurationMS;
		video::SColor captionColor;
		u32 demoMatchTimerMs;
		u32 timeUntilDemoMatchMs;
		bool allowAttractMode;
	};
}
#endif