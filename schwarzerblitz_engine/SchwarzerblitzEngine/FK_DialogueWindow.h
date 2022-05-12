
/*
	*** Schwarzerblitz 3D Fighting Game Engine  ***

	=================== Source Code ===================
	Copyright (C) 2016-2022 Andrea Demetrio

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation and/or
	   other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may be
	   used to endorse or promote products derived from  this software without specific
	   prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	=============== Additional Components ==============
	Please refer to the license/irrlicht/ and license/SFML/ folder for the license
	indications concerning those components. The irrlicht-schwarzerlicht engine and
	the SFML code and binaries are subject to their own licenses, see the relevant
	folders for more information.

	=============== Assets and resources ================
	Unless specificed otherwise in the Credits file, the assets and resources
	bundled with this engine are to be considered "all rights reserved" and
	cannot be redistributed without the owner's consent. This includes but it is
	not limited to the characters concepts / designs, the 3D models, the music,
	the sound effects, 2D and 3D illustrations, stages, icons, menu art.

	Tutorial Man, Evil Tutor, and Random:
	Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

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