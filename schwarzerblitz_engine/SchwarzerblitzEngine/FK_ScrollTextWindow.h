#ifndef FK_SCROLLTEXT_WINDOW_CLASS
#define FK_SCROLLTEXT_WINDOW_CLASS

#include <irrlicht.h>
#include "FK_Window.h"
#include<vector>

using namespace irr;

namespace fk_engine {
	class FK_ScrollTextWindow : public FK_Window {
	private:
		const u32 DialogueOffsetX = 10;
		const u32 DialogueOffsetY = 10;
		const f32 ScrollSpeedPerSecond = 60.f;
	public:
		FK_ScrollTextWindow(IrrlichtDevice *new_device, core::rect<s32> newFrame,
			std::string new_skinName, std::string new_path,
			f32 scaleFactor,
			std::vector<std::string> textLines);
		void reset(f32 scaleFactor, std::vector<std::string> textLines);
		void resetTextPosition();
		virtual void update(u32 pressedButtons, u32 delta_t_ms);
		void drawText(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void drawArrows(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void draw(core::dimension2d<u32> screenSize, f32 scaleX = 1.0f, f32 scaleY = 1.0f, bool drawArrow_flag = true);
	private:
		void processTextLines(std::vector<std::string> textLines, f32 scaleFactor);
	private:
		IrrlichtDevice *device;
		video::IVideoDriver *driver;
		video::ITexture* mugshotTexture;
		video::ITexture* arrowTextureDown;
		video::ITexture* arrowTextureUp;
		std::vector <std::wstring> windowText;
		gui::IGUIFont* font;
		u32 textWidth;
		s32 currentLineX;
		s32 currentLineY;
		u32 currentLineIndex;
		u32 timeElapsed;
		f32 textPosition;
		bool lineBefore;
		bool lineAfter;
	};
}

#endif
