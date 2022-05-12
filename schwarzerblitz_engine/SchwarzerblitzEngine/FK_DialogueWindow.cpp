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

#include "FK_DialogueWindow.h"
#include"ExternalAddons.h"
#include"FK_Database.h"
#include<sstream>
#include<map>

using namespace irr;

namespace fk_engine{
	FK_DialogueWindow::FK_DialogueWindow(IrrlichtDevice *new_device, core::rect<s32> newFrame,
		std::string new_skinName, std::string new_path, f32 scaleFactor, std::string newMugshot,
		std::vector<std::string> textLines,
		FK_DialogueWindow::DialogueWindowAlignment newAlignment,
		FK_DialogueWindow::DialogueTextMode newTextMode) : FK_Window(new_device, newFrame, new_skinName, new_path){
		textWidth = newFrame.getWidth() - 2 * FK_DialogueWindow::DialogueOffsetX;
		device = new_device;
		driver = new_device->getVideoDriver();
		arrowTexture = driver->getTexture((resourcesPath + "common_menu_items\\arrowD.png").c_str());
		reset(scaleFactor, newMugshot, textLines, newAlignment, newTextMode);
	}
	void FK_DialogueWindow::reset(f32 scaleFactor, std::string newMugshot, std::vector<std::string> textLines,
		DialogueWindowAlignment newAlignment, DialogueTextMode newTextMode){
		alignment = newAlignment;
		textMode = newTextMode;
		allTextDisplayedFlag = false;
		timeElapsed = 0;
		windowText.clear();
		textWidth = getWidth() - 2 * FK_DialogueWindow::DialogueOffsetX;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		if (!newMugshot.empty()){
			mugshotTexture = driver->getTexture(newMugshot.c_str());
			if (mugshotTexture != NULL){
				textWidth -= (u32)std::ceil(DialogueMugshotSizeRatio * (f32)getHeight());
				if (alignment == DialogueWindowAlignment::Left){
					textWidth -= DialogueOffsetFromMugshot;
				}
			}
		}
		oldCharacterIndex = -2;
		currentCharacterIndex = -1;
		currentLineIndex = 0;
		letterSoundFlag = false;
		characterDelayMS = FK_DialogueWindow::BaseCharacterDelayMS;
		currentIncompleteLine = std::wstring();
		processTextLines(textLines, scaleFactor);
	}
	void FK_DialogueWindow::update(u32 delta_t_ms){
		FK_Window::update(delta_t_ms);
		if (isActive() && isOpen()){
			timeElapsed += delta_t_ms;
			timeElapsedFromLastCharacter += delta_t_ms;
			if (textMode == DialogueTextMode::CharByChar){
				if (timeElapsedFromLastCharacter > characterDelayMS){
					currentCharacterIndex += 1;
					timeElapsedFromLastCharacter = 0;
				}
			}
		}
	}

	// check if sound can be played
	bool FK_DialogueWindow::canPlaySound(){
		return letterSoundFlag;
	}

	// reset the "play sound" variable
	void FK_DialogueWindow::flagSoundAsPlayed(){
		letterSoundFlag = false;
	}

	// check if the whole text is displayed
	bool FK_DialogueWindow::isAllTextDisplayed(){
		if (textMode == DialogueTextMode::AllTogether){
			return true;
		}
		else{
			return allTextDisplayedFlag;
		}
	}

	std::wstring FK_DialogueWindow::removeFormattingHintsFromLine(std::wstring& lineToProcess) {
		std::wstring temp = lineToProcess;
		std::map<std::wstring, std::wstring> formattingTags{
			{ L".~", L"."},
			{ L"!~", L"!" },
			{ L"?~", L"?" },
			{ L";~", L";" },
			{ L":~", L":" },
		};
		for (auto it = formattingTags.begin(); it != formattingTags.end(); ++it) {
			std::wstring key = (*it).first;
			std::wstring replacement = (*it).second;

			std::string::size_type n = 0;
			while ((n = temp.find(key, n)) != std::wstring::npos)
			{
				temp.replace(n, key.size(), replacement);
				n += replacement.size();
			}
		}
		return temp;
	}

	// draw text
	void FK_DialogueWindow::drawText(f32 scaleX, f32 scaleY){
		s32 textX = (s32)std::floor((f32)(getTopLeftCornerPosition().X + FK_DialogueWindow::DialogueOffsetX) * scaleX);
		s32 textY = (s32)std::floor((f32)(getTopLeftCornerPosition().Y + FK_DialogueWindow::DialogueOffsetY) * scaleY);
		if (mugshotTexture != NULL && alignment == DialogueWindowAlignment::Left){
			textX += (u32)std::ceil((DialogueOffsetFromMugshot + DialogueMugshotSizeRatio * (f32)getHeight()) * scaleX);
		}
		if (textMode == DialogueTextMode::CharByChar){
			if (currentLineIndex < windowText.size()){
				if (currentCharacterIndex < (s32)windowText[currentLineIndex].size()){
					if (oldCharacterIndex != currentCharacterIndex){
						std::wstring lastAddedCharacter = std::wstring();
						if (currentCharacterIndex >= 0){
							std::wstring characterToAdd = std::wstring();
							std::wstring backupCharacter = std::wstring();
							characterToAdd += windowText[currentLineIndex][currentCharacterIndex];
							if (currentCharacterIndex + 1 < (s32)windowText[currentLineIndex].size()){
								backupCharacter += windowText[currentLineIndex][currentCharacterIndex + 1];
							}
							currentIncompleteLine += characterToAdd;
							if (characterToAdd == L"." || characterToAdd == L";" || characterToAdd == L":" ||
								characterToAdd == L"!" || characterToAdd == L"?"){
								if (!backupCharacter.empty() && backupCharacter != L" "){
									characterDelayMS = FK_DialogueWindow::BaseCharacterDelayMS;
									if (backupCharacter == L"~") {
										currentCharacterIndex += 1;
									}
								}
								else if (backupCharacter == L"."){
									characterDelayMS = FK_DialogueWindow::WeakStopDelayMS;
								}
								else{
									characterDelayMS = FK_DialogueWindow::StrongStopDelayMS;
								}
							}
							else if (characterToAdd == L","){
								characterDelayMS = FK_DialogueWindow::WeakStopDelayMS;
							}
							else{
								characterDelayMS = FK_DialogueWindow::BaseCharacterDelayMS;
							}
							lastAddedCharacter = characterToAdd;
						}
						oldCharacterIndex = currentCharacterIndex;
						if (lastAddedCharacter != L" " && !lastAddedCharacter.empty()) {
							letterSoundFlag = true;
						}
					}
				}
				else{
					currentCharacterIndex = -1;
					oldCharacterIndex = -2;
					windowText[currentLineIndex] = currentIncompleteLine;
					currentIncompleteLine = std::wstring();
					currentLineIndex += 1;
				}
			}
			else{
				allTextDisplayedFlag = true;
			}
			for (u32 i = 0; i < currentLineIndex; ++i){
				u32 lineIndex = i;
				//if (currentLineIndex > MaxNumberOfLines - 1){
				//	lineIndex += (currentLineIndex - MaxNumberOfLines + 1);
				//}
				if (lineIndex < windowText.size()){
					std::wstring line = windowText[lineIndex];
					line = removeFormattingHintsFromLine(line);
					core::dimension2d<u32> dimension = font->getDimension(line.c_str());
					u32 sentenceWidth = dimension.Width;
					u32 sentenceHeight = dimension.Height;
					core::rect<s32> destRect(textX, textY, textX + (s32)sentenceWidth, textY + (s32)sentenceHeight);
					fk_addons::drawBorderedText(font, line, destRect, video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
					textY += (s32)sentenceHeight;
				}
			}
			if (currentLineIndex <= windowText.size()){
				core::dimension2d<u32> dimension = font->getDimension(currentIncompleteLine.c_str());
				u32 sentenceWidth = dimension.Width;
				u32 sentenceHeight = dimension.Height;
				core::rect<s32> destRect(textX, textY, textX + (s32)sentenceWidth, textY + (s32)sentenceHeight);
				fk_addons::drawBorderedText(font, currentIncompleteLine, destRect,
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
				textY += (s32)sentenceHeight;
			}
		}
		else if (textMode == DialogueTextMode::AllTogether){
			for each (std::wstring line in windowText){
				line = removeFormattingHintsFromLine(line);
				core::dimension2d<u32> dimension = font->getDimension(line.c_str());
				u32 sentenceWidth = dimension.Width;
				u32 sentenceHeight = dimension.Height;
				core::rect<s32> destRect(textX, textY, textX + (s32)sentenceWidth, textY + (s32)sentenceHeight);
				fk_addons::drawBorderedText(font, line, destRect, video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
				textY += (s32)sentenceHeight;
			}
		}
	}

	// draw mugshot
	void FK_DialogueWindow::drawMugshot(core::dimension2d<u32> screenSize, f32 scaleX, f32 scaleY){
		// draw mugshot
		u32 mughsotSize = (u32)std::ceil(DialogueMugshotSizeRatio * getHeight());
		if (mugshotTexture != NULL){
			s32 mugshotX = getTopLeftCornerPosition().X + FK_DialogueWindow::DialogueOffsetX;
			s32 mugshotY = getTopLeftCornerPosition().Y + (s32)std::floor((f32)(getHeight() - (s32)mughsotSize) / 2.f);
			if (alignment == DialogueWindowAlignment::Right){
				mugshotX += textWidth;
			}
			mugshotX = std::round((f32)mugshotX * scaleX);
			mugshotY = std::round((f32)mugshotY * scaleY);
			f32 scaleMug = DialogueMugshotSizeRatio * (f32)getHeight() * scaleX / (f32)mugshotTexture->getSize().Height;
			fk_addons::draw2DImage(driver, mugshotTexture,
				core::rect<s32>(0, 0, mugshotTexture->getSize().Width, mugshotTexture->getSize().Height),
				core::vector2d<s32>(mugshotX, mugshotY), core::vector2d<s32>(0, 0), 0, core::vector2df(scaleMug, scaleMug),
				false, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}

	// draw arrow
	void FK_DialogueWindow::drawArrow(f32 scale_factorX, f32 scale_factorY){
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
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowTexture->getSize().Width, arrowTexture->getSize().Height);
		// create color array for loading screen
		video::SColor color = video::SColor(255, 255, 255, 255);
		if (!isActive()){
			color = video::SColor(255, 128, 128, 128);
		}
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(arrowTexture, destinationRect, sourceRect, 0, vertexColors_background, true);
	}

	// draw window
	void FK_DialogueWindow::draw(core::dimension2d<u32> screenSize, f32 scaleX, f32 scaleY, bool drawArrow_flag){
		if (!isVisible() || isClosed()){
			return;
		}
		// draw window
		if (isBackgroundVisible()){
			FK_Window::draw(scaleX, scaleY);
		}
		if (isAnimating()){
			return;
		}
		// draw mugshot
		drawMugshot(screenSize, scaleX, scaleY);
		// draw window text
		drawText(scaleX, scaleY);
		// if it's not active, draw an additional shadow
		if (!isActive()){
			video::SColor color(128, 0, 0, 0);
			driver->draw2DRectangle(core::rect<s32>(
				getTopLeftCornerPosition().X * scaleX,
				getTopLeftCornerPosition().Y * scaleY,
				(getTopLeftCornerPosition().X + getWidth()) * scaleX,
				(getTopLeftCornerPosition().Y + getHeight()) * scaleY),
				color, color, color, color);
		}
		// draw arrow (if all text displayed)
		if (isAllTextDisplayed() && isActive() && drawArrow_flag){
			drawArrow(scaleX, scaleY);
		}
	}

	// set text mode
	void FK_DialogueWindow::setTextMode(DialogueTextMode newTextMode){
		textMode = newTextMode;
	}

	// split text lines
	void FK_DialogueWindow::processTextLines(std::vector<std::string> textLines, f32 scaleFactor){
		for each(std::string line in textLines){
			std::istringstream lineReader(line);
			std::string tag;
			std::string lineToDraw = std::string();
			bool title = false;
			bool section = false;
			bool firstElementChanged = false;
			u32 textSize = 0;
			u32 textH = 0;
			while (!lineReader.eof()){
				lineReader >> tag;
				std::string wtag = std::string(tag.begin(), tag.end());
				std::string newLine;
				if (lineToDraw.length() == 0){
					newLine = wtag;
				}
				else{
					newLine = lineToDraw + " " + wtag;
				}
				std::wstring tempLine(newLine.begin(), newLine.end());
				textSize = font->getDimension(tempLine.c_str()).Width;
				textH = font->getDimension(tempLine.c_str()).Height;
				u32 threshold = (s32)std::ceil(((f32)textWidth * scaleFactor));
				if (textSize >= threshold){
					windowText.push_back(fk_addons::convertNameToNonASCIIWstring(lineToDraw));
					lineToDraw = wtag;
				}
				else{
					lineToDraw = newLine;
				}
			}
			windowText.push_back(fk_addons::convertNameToNonASCIIWstring(lineToDraw));
		}
	}
}