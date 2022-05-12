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

#include "FK_Window.h"
#include <string>

using namespace irr;

namespace fk_engine{
	/* basic constructor, the window has no dimension or skin*/
	FK_Window::FK_Window(IrrlichtDevice *new_device){
		device = new_device;
		driver = device->getVideoDriver();
		closingTimeCounterMs = 0;
		openingTimeCounterMs = 0;
		openFlag = false;
		activeFlag = false;
		visibilityFlag = false;
		backgroundVisible = true;
		stretched = true;
		windowFrame = core::rect<s32>(0, 0, 0, 0);
		currentFrame = core::rect<s32>(0, 0, 0, 0);
		skinName = std::string();
		skinPath = std::string();
		alpha = 255;
		windowColor = video::SColor(alpha, 255, 255, 255);
	};
	/* constructor which takes as arguments even the position and the dimensions of the window, plus the skin specifics*/
	FK_Window::FK_Window(IrrlichtDevice *new_device, s32 x, s32 y, s32 width, s32 height,
		std::string new_skinName, std::string new_path) : FK_Window(new_device){
		// set position
		windowFrame = core::rect<s32>(x, y, x+width, y+height);
		// set skin
		setSkin(new_skinName, new_path);
	};
	/* constructor which takes as arguments even the position and the dimensions of the window, plus the skin specifics (2)*/
	FK_Window::FK_Window(IrrlichtDevice *new_device, core::rect<s32> newFrame,
		std::string new_skinName, std::string new_path) : FK_Window(new_device){
		// set position
		windowFrame = newFrame;
		// save resource path
		resourcesPath = new_path;
		// set skin
		setSkin(new_skinName, new_path);
	};
	/* constructor which takes as arguments even the position and the dimensions of the window, plus the skin specifics (3)*/
	FK_Window::FK_Window(IrrlichtDevice *new_device, core::vector2d<s32> newPosition, s32 width, s32 height,
		std::string new_skinName, std::string new_path) : FK_Window(new_device, newPosition.X, newPosition.Y, width, height,
		new_skinName, new_path){
	};
	/* update the window */
	void FK_Window::update(u32 delta_t_ms){
		// update timer
		actionTimeCounterMs += delta_t_ms;
		// if it's fully open, stop animation
		if (openingTimeCounterMs > 0 && !isOpening()){
			openFlag = true;
			openingTimeCounterMs = 0;
		}
		// if it's fully closed, stop animation
		if (closingTimeCounterMs > 0 && !isClosing()){
			openFlag = false;
			closingTimeCounterMs = 0;
		}
	};
	/* force the window open */
	void FK_Window::forceOpen(){
		openFlag = true;
	}
	/* draw the window*/
	void FK_Window::draw(f32 scaleFactorX, f32 scaleFactorY){
		drawTile(scaleFactorX, scaleFactorY);
		drawBorders(scaleFactorX, scaleFactorY);
	};
	/* draw the window background*/
	void FK_Window::drawTile(f32 scaleFactorX, f32 scaleFactorY){
		s32 currentWidth = windowFrame.getWidth();
		s32 baseHeight = windowFrame.getHeight();
		s32 currentHeight = baseHeight;
		if (isOpening()){
			currentHeight = (float)actionTimeCounterMs / openingTimeCounterMs * baseHeight;
		}
		else if (isClosing()){
			currentHeight = baseHeight - ((float)actionTimeCounterMs / closingTimeCounterMs * baseHeight);
		}
		if (currentHeight > 0){
			s32 currentX = windowFrame.UpperLeftCorner.X;
			s32 currentY = (windowFrame.UpperLeftCorner.Y + (baseHeight - currentHeight) / 2);
			currentFrame = core::rect<s32>(currentX, currentY, currentX + currentWidth, currentY + currentHeight);
			core::rect<s32> drawFrame(currentX*scaleFactorX, currentY*scaleFactorY,
				(currentX + currentWidth)*scaleFactorX, (currentY + currentHeight)*scaleFactorY);
			video::SColor const vertexColors[4] = {
				windowColor,
				windowColor,
				windowColor,
				windowColor
			};
			driver->draw2DImage(skin,
				drawFrame,
				SKIN_TILECORE,
				0, vertexColors, true);
		}
	};

	core::rect<s32> FK_Window::getClippingRectangle(f32 scaleFactorX, f32 scaleFactorY){
		s32 x1 = currentFrame.UpperLeftCorner.X;
		s32 y1 = currentFrame.UpperLeftCorner.Y;
		s32 x2 = currentFrame.LowerRightCorner.X;
		s32 y2 = currentFrame.LowerRightCorner.Y;
		return core::rect<s32>(x1*scaleFactorX, y1*scaleFactorY,
			x2*scaleFactorX, y2*scaleFactorY);
	}

	/* draw window borders*/
	void FK_Window::drawBorders(f32 scaleFactorX, f32 scaleFactorY){
		s32 x1 = currentFrame.UpperLeftCorner.X;
		s32 y1 = currentFrame.UpperLeftCorner.Y;
		s32 x2 = currentFrame.LowerRightCorner.X;
		s32 y2 = currentFrame.LowerRightCorner.Y;
		core::rect<s32> drawFrame(x1*scaleFactorX, y1*scaleFactorY,
			x2*scaleFactorX,y2*scaleFactorY);
		// create new destination rectangle
		core::rect<s32> destinationRect;
		// fill top row with border
		u32 noOfTiles = std::ceil(currentFrame.getWidth() / SKIN_TILESIZE + 1);
		for (u32 i = 0; i < noOfTiles; i++){
			s32 tx1 = (x1 + i * SKIN_TILESIZE) * scaleFactorX;
			s32 tx2 = (x1 + (i + 1) * SKIN_TILESIZE) * scaleFactorX;
			destinationRect = core::rect<s32>(tx1, y1 * scaleFactorY, tx2, (y1 + SKIN_TILESIZE)*scaleFactorY);
			driver->draw2DImage(skin,
				destinationRect,
				SKIN_BORDER_TOPSTRAIGHT,
				&drawFrame, 0, true);
		}
		// fill bottom row with border
		for (u32 i = 0; i < noOfTiles; i++){
			s32 tx1 = (x1 + i * SKIN_TILESIZE) * scaleFactorX;
			s32 tx2 = (x1 + (i + 1) * SKIN_TILESIZE) * scaleFactorX;
			destinationRect = core::rect<s32>(tx1, (y2 - SKIN_TILESIZE)* scaleFactorY, tx2, y2 * scaleFactorY);
			driver->draw2DImage(skin,
				destinationRect,
				SKIN_BORDER_DOWNSTRAIGHT,
				&drawFrame, 0, true);
		}
		// fill left row with border
		noOfTiles = ceil(currentFrame.getHeight() / SKIN_TILESIZE);
		for (u32 i = 0; i < noOfTiles; i++){
			s32 ty1 = (y1 + i * SKIN_TILESIZE) * scaleFactorY;
			s32 ty2 = (y1 + (i + 1) * SKIN_TILESIZE) * scaleFactorY;
			destinationRect = core::rect<s32>(x1 * scaleFactorX, ty1, (x1 + SKIN_TILESIZE)*scaleFactorX, ty2);
			driver->draw2DImage(skin,
				destinationRect,
				SKIN_BORDER_LEFTSTRAIGHT,
				&drawFrame, 0, true);
		}
		// fill right row with border
		for (u32 i = 0; i < noOfTiles; i++){
			s32 ty1 = (y1 + i * SKIN_TILESIZE) * scaleFactorY;
			s32 ty2 = (y1 + (i + 1) * SKIN_TILESIZE) * scaleFactorY;
			destinationRect = core::rect<s32>((x2 - SKIN_TILESIZE)*scaleFactorX, ty1, x2*scaleFactorX, ty2);
			driver->draw2DImage(skin,
				destinationRect,
				SKIN_BORDER_RIGHTSTRAIGHT,
				&drawFrame, 0, true);
		}
		// draw top-left frame
		destinationRect = core::rect<s32>(x1 * scaleFactorX, y1 * scaleFactorY, 
			(x1 + SKIN_TILESIZE) * scaleFactorX, (y1 + SKIN_TILESIZE) * scaleFactorY);
		driver->draw2DImage(skin,
			destinationRect,
			SKIN_BORDER_TOPLEFT,
			&drawFrame, 0, true);
		//draw top-right frame
		destinationRect = core::rect<s32>((x2 - SKIN_TILESIZE)*scaleFactorX, y1 * scaleFactorY, 
			x2 * scaleFactorX, (y1 + SKIN_TILESIZE) * scaleFactorY);
		driver->draw2DImage(skin,
			destinationRect,
			SKIN_BORDER_TOPRIGHT,
			&drawFrame, 0, true);
		//draw bottom-left frame
		destinationRect = core::rect<s32>(x1*scaleFactorX, (y2 - SKIN_TILESIZE)*scaleFactorY, 
			(x1 + SKIN_TILESIZE)*scaleFactorX, y2*scaleFactorY);
		driver->draw2DImage(skin,
			destinationRect,
			SKIN_BORDER_DOWNLEFT,
			&drawFrame, 0, true);
		//draw bottom-right frame
		destinationRect = core::rect<s32>((x2 - SKIN_TILESIZE)*scaleFactorX, (y2 - SKIN_TILESIZE)*scaleFactorY,
			x2*scaleFactorX, y2*scaleFactorY);
		driver->draw2DImage(skin,
			destinationRect,
			SKIN_BORDER_DOWNRIGHT,
			&drawFrame, 0, true);
	}
	// draw selector
	void FK_Window::drawSelector(s32 x, s32 y, s32 width, s32 height, s32 opacity, f32 scaleFactorX, f32 scaleFactorY){
		s32 x1 = currentFrame.UpperLeftCorner.X;
		s32 y1 = currentFrame.UpperLeftCorner.Y;
		s32 x2 = currentFrame.LowerRightCorner.X;
		s32 y2 = currentFrame.LowerRightCorner.Y;
		core::rect<s32> drawFrame(x1*scaleFactorX, y1*scaleFactorY,
			x2*scaleFactorX, y2*scaleFactorY);
		core::rect<s32> destinationRect = core::rect<s32>(x*scaleFactorX, 
			y * scaleFactorY,
			(x + width)*scaleFactorY,
			(y+height)*scaleFactorY);
		video::SColor const color = video::SColor(opacity, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(skin,
			destinationRect,
			SKIN_SELECTOR,
			&drawFrame, vertexColors, true);
	}
	/* set window position */
	void FK_Window::setPosition(core::vector2d<s32> newPosition){
		windowFrame = core::rect<s32>(newPosition.X, newPosition.Y,
			newPosition.X + windowFrame.getWidth(), newPosition.Y + windowFrame.getHeight());
	};
	/* check if window is active (i.e. inputable) */
	bool FK_Window::isActive(){
		return activeFlag;
	};
	/* set new activity for the window */
	void FK_Window::setActive(bool activity){
		activeFlag = activity;
	}
	/* check if window is opening*/
	bool FK_Window::isOpening(){
		return actionTimeCounterMs < openingTimeCounterMs;
	};
	/* check if window is closing */
	bool FK_Window::isClosing(){
		return actionTimeCounterMs < closingTimeCounterMs;
	};
	/* check if window is animated */
	bool FK_Window::isAnimating(){
		return (isOpening() || isClosing());
	};
	/* check if window is completely open */
	bool FK_Window::isOpen(){
		return openFlag;
	};
	/* check if window is completely close*/
	bool FK_Window::isClosed(){
		return (!isOpen() && !isOpening() && !isClosing());
	}
	/* if the window is closed open window in a certain amount of ms*/
	void FK_Window::open(u32 delta_t_ms){
		if (isClosed()){
			openingTimeCounterMs = delta_t_ms;
			// if the time interval is set to zero, show as already open
			if (openingTimeCounterMs == 0){
				openFlag = true;
			}
			else{
				// reset action timer, so that the opening process can start
				actionTimeCounterMs = 0;
			}
		}
	};
	/* if the window is open, close it in a certain amount of time*/
	void FK_Window::close(u32 delta_t_ms){
		if (isOpen()){
			closingTimeCounterMs = delta_t_ms;
			openFlag = false;
			// if the time interval is set to zero, show as already close
			if (closingTimeCounterMs != 0){
				// reset action timer, so that the opening process can start
				actionTimeCounterMs = 0;
			}
		}
	};
	/* get normal width */
	s32 FK_Window::getWidth(){
		return windowFrame.getWidth();
	};
	/* get normal height */
	s32 FK_Window::getHeight(){
		return windowFrame.getHeight();
	};
	/* get position of the center */
	core::vector2d<s32> FK_Window::getCenterPosition(){
		return windowFrame.getCenter();
	};
	/* get top left corner position */
	core::vector2d<s32> FK_Window::getTopLeftCornerPosition(){
		return windowFrame.UpperLeftCorner;
	};
	/* check visibility */
	bool FK_Window::isVisible(){
		return visibilityFlag;
	};
	/* set visibility */
	void FK_Window::setVisibility(bool isVisible){
		visibilityFlag = isVisible;
	};
	/* check background visibility */
	bool FK_Window::isBackgroundVisible(){
		return backgroundVisible;
	}
	/* set background visibility */
	void FK_Window::setBackgroundVisibility(bool isVisible){
		backgroundVisible = isVisible;
	}
	/* set alpha */
	void FK_Window::setAlpha(s32 newAlpha){
		alpha = newAlpha;
		windowColor = video::SColor(alpha, 255, 255, 255);
	}
	/* set skin */
	void FK_Window::setSkin(std::string new_skinName, std::string new_path){
		skinName = new_skinName;
		skinPath = new_path;
		if(skin && skin->getReferenceCount() > 0){
			skin->drop();
		}
		skin = driver->getTexture((skinPath + skinName).c_str());
	};
};
