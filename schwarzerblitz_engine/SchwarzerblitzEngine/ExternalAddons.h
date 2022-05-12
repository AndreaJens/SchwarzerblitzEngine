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

#pragma once

#include <irrlicht.h>
#include <string>
#include <Windows.h>

using namespace irr;

namespace fk_addons{

	// check if directory exists
	BOOL directoryExists(LPCTSTR absolutePath);

	// check if file exists
	BOOL fileExists(LPCTSTR szPath);

	//draw bordered text
	void drawBorderedText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
		video::SColor textColor = video::SColor(255, 255, 255, 255),
		video::SColor borderColor = video::SColor(255, 0, 0, 0),
		bool horizontalCentering = false, bool verticalCentering = false,
		core::rect<s32> *clipRect = 0);

	// draw image with rotation
	void draw2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0,0,0,0));

	// draw batch 2D image (no need to expose children functions)
	void drawBatch2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	void drawRotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	void drawStraight2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	void drawUnrotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	// draw tiled image
	void draw2DTiledImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32>& sourceRect, 
		irr::core::position2d<irr::s32> offsetPosition = irr::core::position2d<irr::s32>(0, 0),
		irr::core::vector2d<irr::f32> scalingFactor = core::vector2d<irr::f32>(1.0, 1.0));

	//Convert a texture to an image. Remember to drop the returned pointer
	video::IImage* TextureImage(video::IVideoDriver* driver, video::ITexture* texture);

	//Convert an image to a texture. Remember to drop the returned pointer
	video::ITexture* ImageTexture(video::IVideoDriver* driver, video::IImage* image,
		core::stringc name = core::stringc(""));

	//Convert string into wstring, replacing special characters for non-ASCII
	std::wstring convertNameToNonASCIIWstring(std::string nameToConvert);

	// replace substring 
	bool replaceSubstring(std::string& stringToActOn, std::string substringToFind, std::string substringToReplace);

	// handle game breaking exceptions
	void handle_eptr(std::exception_ptr eptr);

}