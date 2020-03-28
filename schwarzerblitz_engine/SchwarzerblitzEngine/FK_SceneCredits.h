#ifndef FK_SCENECREDITS_CLASS
#define FK_SCENECREDITS_CLASS

#include "FK_SceneWithInput.h"
#include <SFML\Audio.hpp>

using namespace irr;

namespace fk_engine{
	class FK_SceneCredits : public FK_SceneWithInput {
	public:
		const std::string CreditsResourcesDirectory = "scenes\\credits\\";
		const std::string TitleMarker = "**";
		const std::string SectionMarker = "--";
		const std::string EmptyLineMarker = "__";
		const std::string CreditsFileName = "credits.txt";
		const video::SColor BackgroundColor = video::SColor(255, 0, 0, 0);
		const video::SColor NormalTextColor = video::SColor(255, 255, 255, 255);
		const video::SColor TitleColor = video::SColor(255, 200, 200, 50);
		const video::SColor SectionColor = video::SColor(255, 200, 200, 50);
		const video::SColor BorderColor = video::SColor(128, 0, 0, 0);
		const f32 AdvancementSpeed = 3.0f;
		const enum FK_CreditsStyle : u32{
			NormalText = 0,
			SectionText = 1,
			TitleText = 2
		};
	public:
		FK_SceneCredits();
		FK_SceneCredits(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions, bool fadeToBlack);
		void initialize();
		void update();
		void updateInput();
		void dispose();
		bool isRunning();
		FK_Scene::FK_SceneType nameId();
	protected:
		void setup();
		void drawAll();
		void drawLines();
		void drawBackground();
		void readLinesFromFile();
		void drawLine(s32 x, s32 y, u32 index);
		void processLines();
		s32 processLine(s32 x, s32 y, std::string line);
		void setupBGM();
	private:
		/* lines to draw */
		std::vector<std::string> lines;
		std::vector<std::wstring> linesToDraw;
		std::vector<s32> linesToDrawY;
		std::vector<FK_CreditsStyle> linesToDrawStyle;
		/* screen */
		core::dimension2d<u32> screenSize;
		/* fonts */
		gui::IGUIFont *normalFont;
		gui::IGUIFont *sectionFont;
		gui::IGUIFont *titleFont;
		/* logic */
		bool terminateScene;
		u32 cycleId;
		/* coordinates*/
		f32 currentTextY;
		f32 lastElementPositionY;
		f32 basicSize;
		/* BGM */
		std::string creditsResourcesPath;
		std::string creditsBGMName;
		f32 creditsBGMvolume;
		f32 creditsBGMpitch;
		sf::Music credits_bgm;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 lastInputUpdateTime;
		video::ITexture* lastFrame;
		/* background opacity */
		f32 backgroundOpacity;
		f32 blackFadingOpacity;
		bool fadeToBlackFlag;
		u32 linesDrawn;
		s32 firstElementY;
		s32 referenceScreenShift;
		u32 firstElementIndex;
	};
}

#endif