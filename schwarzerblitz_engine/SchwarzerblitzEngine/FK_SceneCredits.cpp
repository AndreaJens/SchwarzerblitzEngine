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

#include"FK_SceneCredits.h"
#include"ExternalAddons.h"
#include<iostream>
#include<fstream>
#include<sstream>

using namespace gui;

namespace fk_engine{
	FK_SceneCredits::FK_SceneCredits() : FK_SceneWithInput(){
		fadeToBlackFlag = true;
		showPicturesSlideshow = false;
		configurationFileName = std::string();
		pictureDurationCounter = -1;
		currentPictureDurationMs = 0;
		pictureIndex = 0;
	};
	FK_SceneCredits::FK_SceneCredits(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options* newOptions, bool fadeToBlack, bool showPictures, std::string newConfigFileName) : FK_SceneCredits(){
		FK_SceneWithInput::setup(newDevice, joypadInfo, newOptions);
		fadeToBlackFlag = fadeToBlack;
		showPicturesSlideshow = showPictures;
		configurationFileName = newConfigFileName;
		pictureDurationCounter = -1;
		FK_SceneCredits::initialize();
	};

	/* update input*/
	void FK_SceneCredits::updateInput(){
	if (joystickInfo.size() > 0){
		player1input->update(now, inputReceiver->JoypadStatus(0), false);
	}
	else{
		player1input->update(now, inputReceiver->KeyboardStatus(), false);
	}
	if (joystickInfo.size() > 1){
		player2input->update(now, inputReceiver->JoypadStatus(1), false);
	}
	else{
		player2input->update(now, inputReceiver->KeyboardStatus(), false);
	}
	u32 currentButtonPlayer1ForPausePurpose = player1input->getPressedButtons() & FK_Input_Buttons::Confirmation_Button;
	u32 currentButtonPlayer2ForPausePurpose = player2input->getPressedButtons() & FK_Input_Buttons::Confirmation_Button;
		if (currentButtonPlayer1ForPausePurpose | currentButtonPlayer2ForPausePurpose){
			terminateScene = true;
			inputReceiver->reset();
		}
	}

	/* update*/
	void FK_SceneCredits::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		f32 targetOpacity = 96.0f;
		// if this is the first update cycle, play "select character" voice and the BGM
		if (cycleId == 0){
			cycleId = 1;
			credits_bgm.play();
			delta_t_ms = 0;
		}
		else{
			if (backgroundOpacity > targetOpacity){
				backgroundOpacity -= (255 - targetOpacity) * (f32)delta_t_ms / 1500.0;
				backgroundOpacity = core::clamp(backgroundOpacity, 1.f, 255.f);
			}
			if (terminateScene){
				blackFadingOpacity += 255 * (f32)delta_t_ms / 1500.0;
			}
			currentTextY -= FK_SceneCredits::AdvancementSpeed * basicSize * (f32)delta_t_ms / 1000.0;
		}
		if (lastElementPositionY < 0){
			terminateScene = true;
		}
		if (!terminateScene){
			updateInput();
		}
		if (terminateScene && fadeToBlackFlag) {
			f32 ratio = 1 - blackFadingOpacity / 255.f;
			if (ratio < 0) {
				ratio = 0.f;
			}
			credits_bgm.setVolume(creditsBGMvolume * gameOptions->getMusicVolume() * ratio);
		}
		if (showPicturesSlideshow) {
			if (pictureDurationCounter < 0) {
				if (pictureIndex < pictures.size()) {
					std::string picturePath = mediaPath + pictures[pictureIndex].path;
					if (pictures[pictureIndex].duration < 0) {
						pictureDurationCounter = defaultPictureDurationMs;
						currentPictureDurationMs = defaultPictureDurationMs;
					}
					else {
						pictureDurationCounter = pictures[pictureIndex].duration;
						currentPictureDurationMs = pictureDurationCounter;
					}
					lastFrame = driver->getTexture(picturePath.c_str());
					pictureIndex += 1;
				}
			}
			else {
				pictureDurationCounter -= delta_t_ms;
			}
		}
		// draw scene
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		drawAll();
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}

	/* name Id*/
	FK_Scene::FK_SceneType FK_SceneCredits::nameId(){
		return FK_Scene::FK_SceneType::SceneCredits;
	}

	void FK_SceneCredits::initialize(){
		cycleId = 0;
		terminateScene = false;
		backgroundOpacity = 255;
		blackFadingOpacity = 0;
		firstElementIndex = 0;
		referenceScreenShift = 0;
		firstElementY = -(s32)screenSize.Height;
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper();
		setup();
	}

	/* setup scene*/
	void FK_SceneCredits::setup(){
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		currentTextY = screenSize.Height;
		lastElementPositionY = currentTextY;
		normalFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		sectionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		titleFont = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		core::dimension2d<u32> fsize = normalFont->getDimension(L"A");
		basicSize = (f32)fsize.Height;
		setNextScene(FK_SceneType::SceneMainMenu);
		readLinesFromFile();
		readPicturesFromFile();
		setupBGM();
		video::IImage* image = driver->createScreenShot();
		lastFrame = fk_addons::ImageTexture(driver, image, "lastScreenshot");
		image->drop();
		processLines();
	};

	void FK_SceneCredits::drawLine(s32 x, s32 y, u32 index){
		std::wstring lineToDraw = linesToDraw[index];
		u32 textSize = 0;
		u32 textH = 0;
		gui::IGUIFont *font = normalFont;
		video::SColor color = FK_SceneCredits::NormalTextColor;
		bool title = false;
		if (linesToDrawStyle[index] == FK_CreditsStyle::TitleText){
			font = titleFont;
			color = FK_SceneCredits::TitleColor;
			title = true;
		}
		else if (linesToDrawStyle[index] == FK_CreditsStyle::SectionText){
			font = sectionFont;
			color = FK_SceneCredits::SectionColor;
		}
		
		u32 textW = font->getDimension(lineToDraw.c_str()).Width;
		s32 x1 = x;
		s32 x2 = x1 + (s32)textW;
		s32 y1 = y;
		s32 y2 = y1 + (s32)textH;
		core::rect<s32> rectD(x1, y1, x2, y2);
		if (title){
			rectD = core::rect<s32>(0, y1, screenSize.Width, y2);
		}
		if (y1 >= - (s32) screenSize.Height / 10){
			fk_addons::drawBorderedText(font, lineToDraw.c_str(), rectD, color, FK_SceneCredits::BorderColor, title);
		}
		linesDrawn += 1;
	}

	/* is running?*/
	bool FK_SceneCredits::isRunning(){
		return !(terminateScene && ((fadeToBlackFlag && blackFadingOpacity >= 255.0f) || !fadeToBlackFlag));
	}

	// process lines
	void FK_SceneCredits::processLines(){
		s32 y = 0;
		s32 x = screenSize.Width / 32;
		for (u32 i = 0; i < lines.size(); ++i){
			y = processLine(x, y, lines[i]);
		}
	}

	s32 FK_SceneCredits::processLine(s32 x, s32 y, std::string line){
		std::istringstream lineReader(line);
		std::string tag;
		std::wstring lineToDraw = std::wstring();
		bool title = false;
		bool section = false;
		bool firstElementChanged = false;
		u32 textSize = 0;
		u32 textH = 0;
		gui::IGUIFont *font = normalFont;
		video::SColor color = FK_SceneCredits::NormalTextColor;
		FK_CreditsStyle style = FK_CreditsStyle::NormalText;
		while (!lineReader.eof()){
			lineReader >> tag;
			if (tag == FK_SceneCredits::TitleMarker){
				title = true;
				font = titleFont;
				color = FK_SceneCredits::TitleColor;
				style = FK_CreditsStyle::TitleText;
				continue;
			}
			else if (tag == FK_SceneCredits::SectionMarker){
				section = true;
				font = sectionFont;
				color = FK_SceneCredits::SectionColor;
				style = FK_CreditsStyle::SectionText;
				continue;
			}
			else if (tag == FK_SceneCredits::EmptyLineMarker){
				y += (s32)floor(basicSize);
				return y;
			}
			std::wstring wtag = std::wstring(tag.begin(), tag.end());
			std::wstring newLine;
			if (lineToDraw.length() == 0){
				newLine = wtag;
			}
			else{
				newLine = lineToDraw + L" " + wtag;
			}
			textSize = font->getDimension(newLine.c_str()).Width;
			textH = font->getDimension(newLine.c_str()).Height;
			if (textSize >= (screenSize.Width - 2 * x)){
				linesToDraw.push_back(lineToDraw);
				linesToDrawY.push_back(y);
				linesToDrawStyle.push_back(style);
				lineToDraw = wtag;
				y += (s32)(floor((f32)textH *1.2f));
			}
			else{
				lineToDraw = newLine;
			}
		}
		linesToDraw.push_back(lineToDraw);
		linesToDrawY.push_back(y);
		linesToDrawStyle.push_back(style);
		y += (s32)(floor((f32)textH *1.5f));
		return y;
	}

	// draw
	void FK_SceneCredits::drawLines(){
		s32 basicY = floor(currentTextY);
		s32 backupFirstElementY = firstElementY;
		s32 x = screenSize.Width / 32;
		lastElementPositionY = basicY + linesToDrawY[linesToDrawY.size()-1];
		linesDrawn = 0;
		for (u32 i = 0; i < linesToDraw.size(); ++i){
			s32 y = basicY + linesToDrawY[i];
			drawLine(x, y, i);
			if (y > (s32)screenSize.Height){
				break;
			}
		}
		//std::cout << linesDrawn << std::endl;
	}

	// draw
	void FK_SceneCredits::drawBackground(){
		video::SColor const color = video::SColor(ceil(backgroundOpacity), 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, lastFrame->getSize().Width, lastFrame->getSize().Height);
		driver->draw2DImage(lastFrame, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height), sourceRect, 0, vertexColors);
		if (showPicturesSlideshow) {
			core::rect<s32> destRect(0, 0, screenSize.Width, screenSize.Height);
			s32 alpha = 1;
			if (pictureDurationCounter <= defaultFadeInTimeMsSlideshow) {
				alpha = 255 * (defaultFadeInTimeMsSlideshow - pictureDurationCounter) / defaultFadeInTimeMsSlideshow;
			}
			else if (pictureDurationCounter >= (currentPictureDurationMs - defaultFadeInTimeMsSlideshow)) {
				s32 delta = currentPictureDurationMs - defaultFadeInTimeMsSlideshow;
				alpha = 255 * (pictureDurationCounter - delta) / defaultFadeInTimeMsSlideshow;
			}
			alpha = core::clamp(alpha, 1, 255);
			if (pictureIndex > pictures.size()) {
				alpha = 255;
			}
			if (alpha > 1) {
				video::SColor const color = video::SColor(alpha, 0, 0, 0);
				driver->draw2DRectangle(color, destRect);
			}
		}
	}

	// draw
	void FK_SceneCredits::drawAll(){
		drawBackground();
		drawLines();
		if (terminateScene && fadeToBlackFlag){
			video::SColor const color = video::SColor(core::clamp((s32)ceil(blackFadingOpacity), 1, 255), 0, 0, 0);
			driver->draw2DRectangle(color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}

	void FK_SceneCredits::readLinesFromFile(){
		std::string filename = mediaPath + CreditsResourcesDirectory + FK_SceneCredits::CreditsFileName;
		if (!configurationFileName.empty()) {
			filename = mediaPath + CreditsResourcesDirectory + configurationFileName;
		}
		std::ifstream inputFile(filename.c_str());
		std::string line;
		inputFile >> creditsBGMName >> creditsBGMvolume >> creditsBGMpitch;
		std::string endTag = "#END";
		while (!inputFile.eof()){
			std::getline(inputFile, line);
			if (line.empty()) continue;
			if (line == endTag){
				break;
			}
			else{
				lines.push_back(line);
			}
		};
	}

	void FK_SceneCredits::readPicturesFromFile() {
		std::string filename = mediaPath + CreditsResourcesDirectory + FK_SceneCredits::PicturesFileName;
		std::ifstream inputFile(filename.c_str());
		std::string line;
		while (!inputFile.eof()) {
			FK_CreditPicture pic;
			inputFile >> pic.path >> pic.duration;
			if (!pic.path.empty()) {
				pictures.push_back(pic);
			}
		};
	}

	// setup BGM
	void FK_SceneCredits::setupBGM(){
		creditsResourcesPath = mediaPath + FK_SceneCredits::CreditsResourcesDirectory;
		std::string filePath = creditsResourcesPath + creditsBGMName;
		if (!credits_bgm.openFromFile(filePath.c_str()))
			return;
		credits_bgm.setLoop(true);
		credits_bgm.setVolume(creditsBGMvolume * gameOptions->getMusicVolume());
		credits_bgm.setPitch(creditsBGMpitch);
	}

	/* check if it is running */
	void FK_SceneCredits::dispose(){
		credits_bgm.stop();
		terminateScene = false;
		// clear the scene of all scene nodes
		smgr->clear();
		// remove all meshes from the mesh cache
		scene::IMeshCache* cache = smgr->getMeshCache();
		while (cache->getMeshCount())
			cache->removeMesh(cache->getMeshByIndex(0));
		// remove all textures
		smgr->getVideoDriver()->removeAllTextures();
	}
}