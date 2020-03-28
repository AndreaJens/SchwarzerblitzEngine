#ifndef FK_DIALOGUE_WINDOW_CLASS
#define FK_DIALOGUE_WINDOW_CLASS

#include <irrlicht.h>
#include "FK_Window.h"
#include<vector>

using namespace irr;

namespace fk_engine{
	class FK_DialogueWindow : public FK_Window{
	public:
		enum DialogueWindowAlignment : s32{
			Left = -1,
			Right = 1,
		};
		enum DialogueTextMode : s32{
			CharByChar = 0,
			LineByLine = 1,
			AllTogether = -1,
		};
	private:
		const u32 DialogueOffsetX = 10;
		const u32 DialogueOffsetY = 10;
		const u32 DialogueOffsetFromMugshot = 5;
		const f32 DialogueMugshotSizeRatio = 0.8f;
		const u32 MaxNumberOfLines = 4;
		const u32 BaseCharacterDelayMS = 16;
		const u32 StrongStopDelayMS = 300;
		const u32 WeakStopDelayMS = 60;
	public:
		FK_DialogueWindow(IrrlichtDevice *new_device, core::rect<s32> newFrame,
			std::string new_skinName, std::string new_path, 
			f32 scaleFactor,
			std::string newMugshot, std::vector<std::string> textLines,
			DialogueWindowAlignment newAlignment, DialogueTextMode newTextMode);
		void reset(f32 scaleFactor, std::string newMugshot, std::vector<std::string> textLines,
			DialogueWindowAlignment newAlignment, DialogueTextMode newTextMode);
		virtual void update(u32 delta_t_ms);
		void drawText(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void drawMugshot(core::dimension2d<u32> screenSize, f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void drawArrow(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void draw(core::dimension2d<u32> screenSize, f32 scaleX = 1.0f, f32 scaleY = 1.0f, bool drawArrow_flag = true);
		bool isAllTextDisplayed();
		void setTextMode(DialogueTextMode textMode);
		bool canPlaySound();
		void flagSoundAsPlayed();
	private:
		void processTextLines(std::vector<std::string> textLines, f32 scaleFactor);
		std::wstring FK_DialogueWindow::removeFormattingHintsFromLine(std::wstring& lineToProcess);
	private:
		IrrlichtDevice *device;
		video::IVideoDriver *driver;
		video::ITexture* mugshotTexture;
		video::ITexture* arrowTexture;
		std::vector <std::wstring> windowText;
		gui::IGUIFont* font;
		DialogueTextMode textMode;
		DialogueWindowAlignment alignment;
		u32 timeElapsedFromLastCharacter;
		u32 characterDelayMS;
		u32 lineDelayMS;
		u32 textWidth;
		s32 currentLineX;
		s32 currentLineY;
		s32 currentCharacterIndex;
		s32 oldCharacterIndex;
		u32 currentLineIndex;
		u32 timeElapsed;
		std::wstring currentIncompleteLine;
		bool allTextDisplayedFlag;
		bool letterSoundFlag;
	};
}

#endif