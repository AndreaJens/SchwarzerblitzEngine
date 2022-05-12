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

#ifndef FK_WINDOW_CLASS
#define FK_WINDOW_CLASS

#include <irrlicht.h>
#include <string>

using namespace irr;

namespace fk_engine{
	class FK_Window{
	public:
		FK_Window(IrrlichtDevice *new_device);
		FK_Window(IrrlichtDevice *new_device, s32 x, s32 y, s32 width, s32 height,
			std::string new_skinName, std::string new_path);
		FK_Window(IrrlichtDevice *new_device, core::rect<s32> newFrame,
			std::string new_skinName, std::string new_path);
		FK_Window(IrrlichtDevice *new_device, core::vector2d<s32> newPosition, s32 width, s32 height,
			std::string new_skinName, std::string new_path);
		virtual void update(u32 delta_t_ms);
		void draw(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void drawSelector(s32 x, s32 y, s32 width, s32 height, s32 opacity, f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void setPosition(core::vector2d<s32> new_position);
		bool isBackgroundVisible();
		bool isOpening();
		bool isClosing();
		void forceOpen();
		void open(u32 delta_t_ms);
		void close(u32 delta_t_ms);
		bool isVisible();
		void setVisibility(bool isVisible);
		void setBackgroundVisibility(bool isVisible);
		void setAlpha(s32 alpha);
		bool isOpen();
		bool isClosed();
		bool isAnimating();
		bool isActive();
		void setActive(bool activity);
		s32 getWidth();
		s32 getHeight();
		core::vector2d<s32> getCenterPosition();
		core::vector2d<s32> getTopLeftCornerPosition();
		const u32 SKIN_TILESIZE = 32;
		const core::rect<s32> SKIN_BORDER_TOPLEFT = core::rect<s32>(0, 0, SKIN_TILESIZE, SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_TOPRIGHT = core::rect<s32>(SKIN_TILESIZE, 0, 2 * SKIN_TILESIZE, SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_DOWNRIGHT = core::rect<s32>(SKIN_TILESIZE, SKIN_TILESIZE, 
			2 * SKIN_TILESIZE, 2 * SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_DOWNLEFT = core::rect<s32>(0, SKIN_TILESIZE, SKIN_TILESIZE, 2*SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_TOPSTRAIGHT = core::rect<s32>(SKIN_TILESIZE / 2, 0, SKIN_TILESIZE * 3 / 2, SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_DOWNSTRAIGHT = core::rect<s32>(SKIN_TILESIZE/2, SKIN_TILESIZE, SKIN_TILESIZE * 3 / 2, 2 * SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_LEFTSTRAIGHT = core::rect<s32>(0, SKIN_TILESIZE / 2, SKIN_TILESIZE, SKIN_TILESIZE * 3 / 2);
		const core::rect<s32> SKIN_BORDER_RIGHTSTRAIGHT = core::rect<s32>(SKIN_TILESIZE, SKIN_TILESIZE / 2, 2*SKIN_TILESIZE, SKIN_TILESIZE * 3 / 2);
		const core::rect<s32> SKIN_TILECORE = core::rect<s32>(2 * SKIN_TILESIZE, 0, 4 * SKIN_TILESIZE, 2 * SKIN_TILESIZE);
		const core::rect<s32> SKIN_SELECTOR = core::rect<s32>(0, 2 * SKIN_TILESIZE, 3 * SKIN_TILESIZE, 3 * SKIN_TILESIZE);
	protected:
		void setSkin(std::string new_skinName, std::string new_path);
		void drawBorders(f32 scaleX, f32 scaleY);
		void drawTile(f32 scaleX, f32 scaleY);
		core::rect<s32> getClippingRectangle(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		std::string resourcesPath;
	private:
		IrrlichtDevice *device;
		video::IVideoDriver *driver;
		gui::IGUIFont *font;
		core::rect<s32> windowFrame;
		core::rect<s32> currentFrame;
		bool stretched;
		bool visibilityFlag;
		bool openFlag;
		bool activeFlag;
		bool backgroundVisible;
		video::ITexture* skin;
		std::string skinName;
		std::string skinPath;
		u32 closingTimeCounterMs;
		u32 openingTimeCounterMs;
		u32 actionTimeCounterMs;
		s32 alpha;
		video::SColor windowColor;
	};
}

#endif