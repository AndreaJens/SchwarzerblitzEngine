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

#include "FK_ScrollTextWindow.h"
#include"ExternalAddons.h"
#include"FK_Database.h"
#include<sstream>

using namespace irr;

namespace fk_engine {
	FK_ScrollTextWindow::FK_ScrollTextWindow(IrrlichtDevice *new_device, core::rect<s32> newFrame,
		std::string new_skinName, std::string new_path, f32 scaleFactor,
		std::vector<std::string> textLines) : FK_Window(new_device, newFrame, new_skinName, new_path) {
		textWidth = newFrame.getWidth() - 2 * FK_ScrollTextWindow::DialogueOffsetX;
		device = new_device;
		driver = new_device->getVideoDriver();
		arrowTextureDown = driver->getTexture((resourcesPath + "common_menu_items\\arrowD.png").c_str());
		arrowTextureUp = driver->getTexture((resourcesPath + "common_menu_items\\arrowU.png").c_str());
		reset(scaleFactor, textLines);
		setAlpha(196);
	}
	void FK_ScrollTextWindow::reset(f32 scaleFactor, std::vector<std::string> textLines) {
		windowText.clear();
		textWidth = getWidth() - 2 * FK_ScrollTextWindow::DialogueOffsetX;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		currentLineIndex = 0;
		timeElapsed = 0;
		textPosition = 0;
		processTextLines(textLines, scaleFactor);
	}
	void FK_ScrollTextWindow::resetTextPosition() {
		timeElapsed = 0;
		textPosition = 0;
	}
	void FK_ScrollTextWindow::update(u32 pressedButtons, u32 delta_t_ms) {
		FK_Window::update(delta_t_ms);
		timeElapsed += delta_t_ms;
		f32 delta_t_s = (f32)delta_t_ms / 1000.f;
		if (isActive() && isOpen()) {
			if ((pressedButtons & FK_Input_Buttons::Down_Direction) != 0) {
				textPosition -= ScrollSpeedPerSecond * delta_t_s;
			}else if ((pressedButtons & FK_Input_Buttons::Up_Direction) != 0){
				textPosition += ScrollSpeedPerSecond * delta_t_s;
			}
		}
	}

	// draw text
	void FK_ScrollTextWindow::drawText(f32 scaleX, f32 scaleY) {
		core::dimension2d<u32> dimensionSample = font->getDimension(L"A");
		lineBefore = false;
		lineAfter = false;
		s32 textX = (s32)std::floor((f32)(getTopLeftCornerPosition().X + FK_ScrollTextWindow::DialogueOffsetX) * scaleX);
		s32 textY = (s32)std::floor((f32)(getTopLeftCornerPosition().Y + FK_ScrollTextWindow::DialogueOffsetY) * scaleY);
		s32 baseTextY = textY;
		core::rect<s32> *clipRect = new core::rect<s32>(
			textX,
			baseTextY,
			textX + (s32)std::floor((getWidth() - 2 * FK_ScrollTextWindow::DialogueOffsetX) * scaleX),
			baseTextY + (s32)std::floor((getHeight() - 2 * FK_ScrollTextWindow::DialogueOffsetY) * scaleY)
			);
		f32 textWindowHeight = (f32)clipRect->getHeight();
		f32 textSizeH = (f32)dimensionSample.Height;
		f32 totalTextSize = textSizeH * (f32)windowText.size();
		f32 minimumOffset = textWindowHeight - totalTextSize;
		if (minimumOffset > 0.f) {
			minimumOffset = 0.f;
		}
		if (textPosition < minimumOffset / scaleY) {
			textPosition = minimumOffset / scaleY;
		}
		if (textPosition > 0.f) {
			textPosition = 0.f;
		}

		textY = (s32)std::floor((f32)textY + textPosition * scaleY);
		for each (std::wstring line in windowText) {
			core::dimension2d<u32> dimension = font->getDimension(line.c_str());
			u32 sentenceWidth = dimension.Width;
			u32 sentenceHeight = dimension.Height;
			if (textY < (s32)(clipRect->UpperLeftCorner.Y)) {
				lineBefore = true;
			}
			if (textY + sentenceHeight < (s32)(clipRect->UpperLeftCorner.Y)) {
				textY += (s32)sentenceHeight;
				lineBefore = true;
				continue;
			}
			core::rect<s32> destRect(textX, textY, textX + (s32)sentenceWidth, textY + (s32)sentenceHeight);
			fk_addons::drawBorderedText(
				font, line, destRect, video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
				false, false, clipRect);
			textY += (s32)sentenceHeight;
			if (textY > (s32)clipRect->UpperLeftCorner.Y + (s32)clipRect->getHeight()) {
				lineAfter = true;
				break;
			}
		}
		delete clipRect;
	}

	// draw arrow
	void FK_ScrollTextWindow::drawArrows(f32 scale_factorX, f32 scale_factorY) {
		u32 arrowFrequency = 500;
		f32 phase = 2 * core::PI * (f32)(timeElapsed % arrowFrequency) / arrowFrequency;
		s32 addonY = floor(10 * asin(cos(phase)) / core::PI);
		u32 arrowSize = 16;
		s32 baseArrowY = getTopLeftCornerPosition().Y + getHeight() - arrowSize / 2;
		s32 baseArrowX = getCenterPosition().X;
		s32 x1 = (baseArrowX - arrowSize / 2) * scale_factorX;
		s32 x2 = x1 + arrowSize * scale_factorX;
		s32 y1 = baseArrowY  * scale_factorY + addonY * scale_factorY;
		s32 y2 = y1 + arrowSize * scale_factorY;
		// draw arrow on selected item
		core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowTextureDown->getSize().Width, arrowTextureDown->getSize().Height);
		// create color array for loading screen
		video::SColor color = video::SColor(255, 255, 255, 255);
		if (!isActive()) {
			color = video::SColor(255, 128, 128, 128);
		}
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		if (lineAfter) {
			driver->draw2DImage(arrowTextureDown, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
		// create up arrow
		y1 = (getTopLeftCornerPosition().Y - arrowSize / 2) * scale_factorY - addonY * scale_factorY;
		y2 = y1 + arrowSize * scale_factorY;
		destinationRect = core::rect<s32>(x1, y1, x2, y2);
		sourceRect = core::rect<s32>(0, 0, arrowTextureUp->getSize().Width, arrowTextureUp->getSize().Height);
		if (lineBefore) {
			driver->draw2DImage(arrowTextureUp, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
	}

	// draw window
	void FK_ScrollTextWindow::draw(core::dimension2d<u32> screenSize, f32 scaleX, f32 scaleY, bool drawArrow_flag) {
		if (!isVisible() || isClosed()) {
			return;
		}
		// draw window
		if (isBackgroundVisible()) {
			FK_Window::draw(scaleX, scaleY);
		}
		if (isAnimating()) {
			return;
		}
		// draw window text
		drawText(scaleX, scaleY);
		// if it's not active, draw an additional shadow
		if (!isActive()) {
			video::SColor color(128, 0, 0, 0);
			driver->draw2DRectangle(core::rect<s32>(
				getTopLeftCornerPosition().X * scaleX,
				getTopLeftCornerPosition().Y * scaleY,
				(getTopLeftCornerPosition().X + getWidth()) * scaleX,
				(getTopLeftCornerPosition().Y + getHeight()) * scaleY),
				color, color, color, color);
		}
		// draw arrow (if all text displayed)
		if (isActive() && drawArrow_flag) {
			drawArrows(scaleX, scaleY);
		}
	}

	// split text lines
	void FK_ScrollTextWindow::processTextLines(std::vector<std::string> textLines, f32 scaleFactor) {
		for each(std::string line in textLines) {
			std::istringstream lineReader(line);
			std::string tag;
			std::string lineToDraw = std::string();
			bool title = false;
			bool section = false;
			bool firstElementChanged = false;
			u32 textSize = 0;
			u32 textH = 0;
			while (!lineReader.eof()) {
				lineReader >> tag;
				std::string wtag = std::string(tag.begin(), tag.end());
				std::string newLine;
				if (lineToDraw.length() == 0) {
					newLine = wtag;
				}
				else {
					newLine = lineToDraw + " " + wtag;
				}
				std::wstring tempLine(newLine.begin(), newLine.end());
				textSize = font->getDimension(tempLine.c_str()).Width;
				textH = font->getDimension(tempLine.c_str()).Height;
				u32 threshold = (s32)std::ceil(((f32)textWidth * scaleFactor));
				if (textSize >= threshold) {
					windowText.push_back(fk_addons::convertNameToNonASCIIWstring(lineToDraw));
					lineToDraw = wtag;
				}
				else {
					lineToDraw = newLine;
				}
			}
			windowText.push_back(fk_addons::convertNameToNonASCIIWstring(lineToDraw));
		}
	}
}