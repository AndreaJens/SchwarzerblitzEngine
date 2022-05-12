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

#include "FK_SceneMainMenu.h"
#include "ExternalAddons.h"
#include<iostream>
#include<Windows.h>
#include <shellapi.h>
#include "FK_AchievementManager.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{

	// get blinking color according to time
	video::SColor FK_SceneMainMenu::FK_BlinkingItemColor::getCurrentColor(u32 now) {
		f32 phase = 2 * core::PI * ((f32)now / colorTransitionPeriodMs);
		phase = fmod(phase, 2 * core::PI);
		f32 magnificationAmplitudeA = (f32)(endingColor.getAlpha() - startingColor.getAlpha());
		f32 magnificationAmplitudeR = (f32)(endingColor.getRed() - startingColor.getRed());
		f32 magnificationAmplitudeG = (f32)(endingColor.getGreen() - startingColor.getGreen());
		f32 magnificationAmplitudeB = (f32)(endingColor.getBlue() - startingColor.getBlue());
		s32 addonA = (s32)std::floor(magnificationAmplitudeA / 2 * ( 1 + sin(phase)));
		s32 addonR = (s32)std::floor(magnificationAmplitudeR / 2 * (1 + sin(phase)));
		s32 addonG = (s32)std::floor(magnificationAmplitudeG / 2 * (1 + sin(phase)));
		s32 addonB = (s32)std::floor(magnificationAmplitudeB / 2 * (1 + sin(phase)));
		return video::SColor(
			std::min(255, (s32)startingColor.getAlpha() + addonA),
			std::min(255, (s32)startingColor.getRed() + addonR),
			std::min(255, (s32)startingColor.getGreen() + addonG),
			std::min(255, (s32)startingColor.getBlue() + addonB)
			);
	}

	FK_SceneMainMenu::FK_SceneMainMenu() : FK_SceneWithInput(){
		initialize();
	};
	FK_SceneMainMenu::FK_SceneMainMenu(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions) :
		FK_SceneWithInput(newDevice, joypadInfo, newOptions){
		initialize();
	};
	void FK_SceneMainMenu::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions, s32 newMenuIndex){
		FK_SceneWithInput::setup(newDevice, joypadInfo, newOptions);
		initialize();
		setupIrrlichtDevice();
		FK_SceneWithInput::readSaveFile();
		setupJoypad();
		setupInputMapper();
		setupDefaultResources();
		loadConfigurationFile();
		setUnavailableMenuItems();
		menuIndex = newMenuIndex;
		if (menuIndex >= menuOptionsStrings.size()) {
			menuIndex = 0;
		}
		setupSoundManager();
		setupBGM();
		setupGraphics();
		setupTutorialMenu();
		setupUnlockablesMenu();
	};

	FK_SceneMainMenu::~FK_SceneMainMenu() {

	}

	FK_Scene::FK_SceneType FK_SceneMainMenu::nameId(){
		return FK_Scene::FK_SceneType::SceneMainMenu;
	}

	bool FK_SceneMainMenu::isRunning(){
		return (itemSelected == false);
	}

	FK_Scene::FK_SceneFreeMatchType FK_SceneMainMenu::getFreeMatchType(){
		if (subMenuType == FK_MenuSubMenuType::Training) {
			return FK_SceneFreeMatchType::FreeMatchTraining;
		}
		return freeMatchMenuOptionsScenes[subMenuIndex];
	}

	FK_Scene::FK_SceneArcadeType FK_SceneMainMenu::getArcadeType(){
		return arcadeMenuOptionsScenes[subMenuIndex];
	}

	FK_Scene::FK_SceneTrainingType FK_SceneMainMenu::getTrainingType() {
		return trainingMenuOptionsScenes[subMenuIndex];
	}

	FK_Scene::FK_SceneExtraType FK_SceneMainMenu::getExtraType() {
		return extraMenuOptionsScenes[subMenuIndex];
	}

	void FK_SceneMainMenu::drawBackground(){
		core::dimension2d<u32> backgroundSize = menuBackgroundTexture->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, backgroundSize.Width, backgroundSize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		//video::SColor const vertexColors_background[4] = {
		//	color,
		//	color,
		//	color,
		//	color
		//};
		irr::core::vector2df scale((f32)((f32)screenSize.Width / (f32)backgroundSize.Width),
			(f32)((f32)screenSize.Height / (f32)backgroundSize.Height));
		fk_addons::draw2DImage(driver, menuBackgroundTexture, sourceRect, core::vector2d<s32>(0, 0),
			core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		//driver->draw2DImage(menuBackgroundTexture, destinationRect, sourceRect, 0, vertexColors_background);
	}

	void FK_SceneMainMenu::drawCaptions(){
		core::dimension2d<u32> backgroundSize = menuCaptionOverlay->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, backgroundSize.Width, backgroundSize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(menuCaptionOverlay, destinationRect, sourceRect, 0, vertexColors_background, true);
		// draw caption text
		s32 fixedOffsetY = 320;
		s32 fixedOffsetX = 16 * scale_factorX;
		/* get width and height of the string*/
		std::wstring itemString(menuOptionsCaptions[menuIndex].begin(), menuOptionsCaptions[menuIndex].end());
		if (isSelectingSubMenu) {
			if (subMenuType == FK_MenuSubMenuType::FreeMatch) {
				itemString = std::wstring(
					freeMatchMenuOptionsCaptions[subMenuIndex].begin(), freeMatchMenuOptionsCaptions[subMenuIndex].end());
			}
			else if (subMenuType == FK_MenuSubMenuType::Arcade) {
				itemString = std::wstring(
					arcadeMenuOptionsCaptions[subMenuIndex].begin(), arcadeMenuOptionsCaptions[subMenuIndex].end());
			}
			else if (subMenuType == FK_MenuSubMenuType::Training) {
				itemString = std::wstring(
					trainingMenuOptionsCaptions[subMenuIndex].begin(), trainingMenuOptionsCaptions[subMenuIndex].end());
			}
			else if (subMenuType == FK_MenuSubMenuType::Extra) {
				itemString = std::wstring(
					extraMenuOptionsCaptions[subMenuIndex].begin(), extraMenuOptionsCaptions[subMenuIndex].end());
			}
		}
		std::wstring captionRepetitionSeparator = L"     *     ";
		s32 sentenceWidth = captionFont->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = captionFont->getDimension(itemString.c_str()).Height;
		s32 separatorSize = captionFont->getDimension(captionRepetitionSeparator.c_str()).Width;
		fixedOffsetX -= ((s32)(captionMovementTimeCounterMs / 10 * scale_factorX)) % (sentenceWidth + separatorSize);
		std::wstring sentenceToDraw = itemString + captionRepetitionSeparator;
		sentenceWidth = captionFont->getDimension(sentenceToDraw.c_str()).Width;
		while (fixedOffsetX + sentenceWidth < screenSize.Width){
			sentenceToDraw += sentenceToDraw;
			sentenceWidth = captionFont->getDimension(sentenceToDraw.c_str()).Width;
		}
		s32 sentenceX = fixedOffsetX; // distance from screen center
		s32 sentenceY = fixedOffsetY; // distance from the top of the screen
		// draw player 1 name near the lifebar
		destinationRect = core::rect<s32>(sentenceX,
			sentenceY * scale_factorY,
			sentenceX * scale_factorX + sentenceWidth,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(captionFont, sentenceToDraw, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	void FK_SceneMainMenu::drawItemBar(s32 index, bool selected){
		s32 fixedOffsetY = itemMainViewport.UpperLeftCorner.Y;
		s32 fixedSpacingX = itemMainSize.UpperLeftCorner.X;
		s32 fixedSpacingY = itemMainSize.UpperLeftCorner.Y;
		s32 fixedOffsetX = itemMainViewport.UpperLeftCorner.X;
		s32 basicY = index * fixedSpacingY + fixedOffsetY;
		s32 basicX = index * fixedSpacingX + fixedOffsetX;
		core::dimension2d<u32> basicSize((u32)itemMainSize.LowerRightCorner.X, (u32)itemMainSize.LowerRightCorner.Y);
		core::dimension2d<u32> textureSize = itemBarTexture->getSize();
		core::rect<s32> destinationRect = core::rect<s32>(
			basicX * scale_factorX,
			basicY * scale_factorY,
			(basicX + basicSize.Width) * scale_factorX,
			(basicY + basicSize.Height) * scale_factorY);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, textureSize.Width, textureSize.Height);
		video::SColor color = itemBlinkingColor.endingColor;
		if (selected && !isSubMenuActive()) {
			color = itemBlinkingColor.getCurrentColor(now);
		}
		video::SColor const vertexColors_item[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(selected ? itemBarSelectedTexture : itemBarTexture,
			destinationRect, sourceRect, 0, vertexColors_item, true);
	}

	void FK_SceneMainMenu::drawItemBars(){
		s32 menuItemsArraySize = menuOptionsStrings.size();
		for (s32 i = 0; i < menuItemsArraySize; ++i){
			/* draw item bars */
			if (i != menuIndex){
				drawItemBar(i, false);
			}
		}
		drawItemBar(menuIndex, true);
	}

	void FK_SceneMainMenu::drawSubMenuItemBars(u32 number, s32 index, u32 width){
		s32 menuItemsArraySize = number;
		for (s32 i = 0; i < menuItemsArraySize; ++i){
			/* draw item bars */
			if (i != index){
				drawSubMenuItemBar(i, false, width);
			}
		}
		drawSubMenuItemBar(index, true, width);
	}

	void FK_SceneMainMenu::drawFreeMatchItemBars(){
		s32 barWidth = itemSubSize.LowerRightCorner.X;
		drawSubMenuItemBars(freeMatchMenuOptionsStrings.size(), subMenuIndex, barWidth);
	}

	void FK_SceneMainMenu::drawArcadeItemBars(){
		s32 barWidth = itemSubSize.LowerRightCorner.X;
		drawSubMenuItemBars(arcadeMenuOptionsStrings.size(), subMenuIndex, barWidth);
	}

	void FK_SceneMainMenu::drawTrainingItemBars() {
		s32 barWidth = itemSubSize.LowerRightCorner.X;
		drawSubMenuItemBars(trainingMenuOptionsStrings.size(), subMenuIndex, barWidth);
	}

	void FK_SceneMainMenu::drawExtraItemBars() {
		s32 barWidth = itemSubSize.LowerRightCorner.X;
		drawSubMenuItemBars(extraMenuOptionsStrings.size(), subMenuIndex, barWidth);
	}

	void FK_SceneMainMenu::drawItems(){
		s32 menuItemsArraySize = menuOptionsStrings.size();
		s32 fixedOffsetY = itemMainTextOffset.UpperLeftCorner.Y + itemMainViewport.UpperLeftCorner.Y;
		s32 fixedSpacingY = itemMainSize.UpperLeftCorner.Y;
		s32 fixedSpacingX = itemMainSize.UpperLeftCorner.X;
		s32 fixedOffsetX = itemMainViewport.UpperLeftCorner.X;
		s32 textOffsetX = itemMainTextOffset.UpperLeftCorner.X;
		s32 selectedItemAdditionalOffsetX = itemMainTextOffset.LowerRightCorner.X;
		s32 selectedItemAdditionalOffsetY = itemMainTextOffset.LowerRightCorner.Y;
		for (s32 i = 0; i < menuItemsArraySize; ++i){
			/* get width and height of the string*/
			std::wstring itemString(menuOptionsStrings[i].begin(), menuOptionsStrings[i].end());
			s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
			s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
			s32 sentenceX = fixedOffsetX + textOffsetX + i * fixedSpacingX; // distance from screen center
			s32 sentenceY = fixedOffsetY + i * fixedSpacingY; // distance from the top of the screen
			if (i == menuIndex) {
				sentenceX += selectedItemAdditionalOffsetX;
				sentenceY += selectedItemAdditionalOffsetY;
			}
			s32 drawingX = (s32)std::round(sentenceX * scale_factorX);
			s32 drawingY = (s32)std::round(sentenceY * scale_factorY);
			if (itemMainAlignment == FK_MenuTextAlignment::Center) {
				drawingX = (s32)std::round(
					(fixedOffsetX + i * fixedSpacingX +
						itemMainSize.LowerRightCorner.X / 2) * scale_factorX) -
					sentenceWidth / 2;
				if (i == menuIndex) {
					drawingY += selectedItemAdditionalOffsetY * scale_factorY;
				}
			}
			else if (itemMainAlignment == FK_MenuTextAlignment::Right) {
				drawingX = (s32)std::round(
					(fixedOffsetX - textOffsetX + i * fixedSpacingX +
						itemMainSize.LowerRightCorner.X) * scale_factorX) -
					sentenceWidth;
				if (i == menuIndex) {
					drawingX -= selectedItemAdditionalOffsetX * scale_factorY;
					drawingY += selectedItemAdditionalOffsetY * scale_factorX;
				}
			}
			else {
				if (i == menuIndex) {
					drawingX += selectedItemAdditionalOffsetX * scale_factorX;
					drawingY += selectedItemAdditionalOffsetY * scale_factorY;
				}
			}
			core::rect<s32> destinationRect = core::rect<s32>(
				drawingX,
				drawingY,
				drawingX + sentenceWidth,
				sentenceHeight + drawingY);
			fk_addons::drawBorderedText(font, itemString, destinationRect,
				irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		}
	}

	void FK_SceneMainMenu::drawSubMenuItemBar(s32 index, bool selected, u32 width){
		s32 fixedOffsetY = itemSubViewport.UpperLeftCorner.Y;
		s32 fixedOffsetX = itemSubViewport.UpperLeftCorner.X;
		s32 fixedSpacingY = itemSubSize.UpperLeftCorner.Y;
		s32 fixedSpacingX = itemSubSize.UpperLeftCorner.X;
		s32 basicY = index * fixedSpacingY + fixedOffsetY;
		s32 basicX = index * fixedSpacingX + fixedOffsetX;
		core::dimension2d<u32> basicSize(width, itemSubSize.LowerRightCorner.Y);
		core::dimension2d<u32> textureSize = itemBarTexture->getSize();
		core::rect<s32> destinationRect = core::rect<s32>(
			basicX * scale_factorX,
			basicY * scale_factorY,
			(basicX + basicSize.Width) * scale_factorX,
			(basicY + basicSize.Height) * scale_factorY);
		core::rect<s32> sourceRect = core::rect<s32>(textureSize.Width, 0, 0, textureSize.Height);
		video::SColor color = itemBlinkingColor.endingColor;
		if (selected) {
			color = itemBlinkingColor.getCurrentColor(now);
		}
		video::SColor const vertexColors_item[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(selected ? itemBarSelectedTexture : itemBarTexture,
			destinationRect, sourceRect, 0, vertexColors_item, true);
	}

	void FK_SceneMainMenu::drawSubMenuItems(std::vector<std::string> optionStrings, s32 index){
		s32 menuItemsArraySize = optionStrings.size();
		s32 fixedOffsetY = itemSubViewport.UpperLeftCorner.Y + itemSubTextOffset.UpperLeftCorner.Y;
		s32 fixedSpacingY = itemSubSize.UpperLeftCorner.Y;
		s32 fixedSpacingX = itemSubSize.UpperLeftCorner.X;
		s32 fixedOffsetX = itemSubViewport.UpperLeftCorner.X;
		s32 textOffsetX = itemSubTextOffset.UpperLeftCorner.X;
		s32 selectedItemAdditionalOffsetX = itemSubTextOffset.LowerRightCorner.X;
		s32 selectedItemAdditionalOffsetY = itemSubTextOffset.LowerRightCorner.Y;
		for (s32 i = 0; i < menuItemsArraySize; ++i){
			/* get width and height of the string*/
			std::wstring itemString(optionStrings[i].begin(), optionStrings[i].end());
			s32 sentenceWidth = subMenufont->getDimension(itemString.c_str()).Width;
			s32 sentenceHeight = subMenufont->getDimension(itemString.c_str()).Height;
			s32 sentenceX = textOffsetX + fixedOffsetX + i * fixedSpacingX; // distance from screen center
			s32 sentenceY = fixedOffsetY + i * fixedSpacingY; // distance from the top of the screen
			s32 drawingX = (s32)std::round(sentenceX * scale_factorX);
			s32 drawingY = (s32)std::round(sentenceY * scale_factorY);
			if (itemSubAlignment == FK_MenuTextAlignment::Center) {
				drawingX = (s32)std::round(
					(fixedOffsetX + i * fixedSpacingX +
					itemSubSize.LowerRightCorner.X / 2) * scale_factorX) - 
					sentenceWidth / 2;
				if (i == index) {
					drawingY += selectedItemAdditionalOffsetY * scale_factorY;
				}
			}
			else if (itemSubAlignment == FK_MenuTextAlignment::Right) {
				drawingX = (s32)std::round(
					(fixedOffsetX - textOffsetX + i * fixedSpacingX +
					itemSubSize.LowerRightCorner.X) * scale_factorX) -
					sentenceWidth;
				if (i == index) {
					drawingX -= selectedItemAdditionalOffsetX * scale_factorX;
					drawingX += selectedItemAdditionalOffsetY * scale_factorY;
				}
			}
			else{
				if (i == index) {
					drawingX += selectedItemAdditionalOffsetX * scale_factorX;
					drawingX += selectedItemAdditionalOffsetY * scale_factorY;
				}
			}
			// draw player 1 name near the lifebar
			core::rect<s32> destinationRect = core::rect<s32>(
				drawingX,
				drawingY,
				drawingX + sentenceWidth,
				drawingY + sentenceHeight);
			fk_addons::drawBorderedText(subMenufont, itemString, destinationRect,
				irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		}
	}

	void FK_SceneMainMenu::drawFreeMatchItems(){
		drawSubMenuItems(freeMatchMenuOptionsStrings, subMenuIndex);
	}

	void FK_SceneMainMenu::drawArcadeItems(){
		drawSubMenuItems(arcadeMenuOptionsStrings, subMenuIndex);
	}

	void FK_SceneMainMenu::drawTrainingItems() {
		drawSubMenuItems(trainingMenuOptionsStrings, subMenuIndex);
	}

	void FK_SceneMainMenu::drawExtraItems() {
		drawSubMenuItems(extraMenuOptionsStrings, subMenuIndex);
	}	

	void FK_SceneMainMenu::drawAll(){
		drawBackground();
		if (!isSubMenuActive() || showMenuWithSubMenu) {
			drawItemBars();
			drawItems();
		}
		if (isSelectingSubMenu) {
			if (subMenuType == FK_MenuSubMenuType::FreeMatch) {
				drawFreeMatchItemBars();
				drawFreeMatchItems();
			}
			else if (subMenuType == FK_MenuSubMenuType::Arcade) {
				drawArcadeItemBars();
				drawArcadeItems();
			}
			else if (subMenuType == FK_MenuSubMenuType::Training) {
				drawTrainingItemBars();
				drawTrainingItems();
			}
			else if (subMenuType == FK_MenuSubMenuType::Extra) {
				drawExtraItemBars();
				drawExtraItems();
			}
		}
		if (itemShowCaptions) {
			drawCaptions();
		}
	}

	void FK_SceneMainMenu::initialize(){
		itemSelected = false;
		lastSelectionFromPlayer1 = false;
		menuIndex = 0;
		/* time */
		then = 0;
		now = 0;
		delta_t_ms = 0;
		cycleId = 0;
		lastInputTime = 0;
		/* path */
		menuBGMName = std::string();
		/* caption movement */
		captionStillPeriodThresholdMs = 1000;
		tutorialIntroMaxTime = 500;
		/* unlockables window (declared NULL)*/
		unlockableContentWindow = NULL;
		/* cleanup menu variables */
		clearMenuVariables();
	}

	void FK_SceneMainMenu::clearMenuVariables() {
		// clear all vectors
		menuOptionsStrings.clear();
		menuOptionsScenes.clear();
		menuActiveItems.clear();
		menuOptionsCaptions.clear();
		freeMatchMenuOptionsStrings.clear();
		freeMatchMenuOptionsScenes.clear();
		freeMatchMenuOptionsCaptions.clear();
		freeMatchMenuActiveItems.clear();
		extraMenuActiveItems.clear();
		extraMenuOptionsCaptions.clear();
		extraMenuOptionsScenes.clear();
		extraMenuOptionsStrings.clear();
		arcadeMenuActiveItems.clear();
		arcadeMenuOptionsCaptions.clear();
		arcadeMenuOptionsScenes.clear();
		arcadeMenuOptionsStrings.clear();
		trainingMenuOptionsStrings.clear();
		trainingMenuActiveItems.clear();
		trainingMenuOptionsScenes.clear();
		trainingMenuOptionsCaptions.clear();

		// main menu
		menuOptionsStrings = std::vector<std::string>();
		menuOptionsScenes = std::vector<FK_Scene::FK_SceneType>();
		menuOptionsCaptions = std::vector<std::string>();
		menuActiveItems = std::vector<bool>();

		//submenu
		subMenuIndex = 0;
		subMenuSize = 0;
		subMenuType = FK_MenuSubMenuType::NoSubmenu;
		isSelectingSubMenu = false;

		// free match
		freeMatchMenuOptionsStrings = std::vector<std::string>();
		freeMatchMenuOptionsScenes = std::vector<FK_Scene::FK_SceneFreeMatchType>();
		freeMatchMenuOptionsCaptions = std::vector<std::string>();
		freeMatchMenuActiveItems = std::vector<bool>();

		// arcade
		arcadeMenuOptionsStrings = std::vector<std::string>();
		arcadeMenuOptionsScenes = std::vector<FK_Scene::FK_SceneArcadeType>();
		arcadeMenuOptionsCaptions = std::vector<std::string>();
		arcadeMenuActiveItems = std::vector<bool>();

		// subMenu extra
		extraMenuOptionsStrings = std::vector<std::string>();
		extraMenuOptionsScenes = std::vector<FK_Scene::FK_SceneExtraType>();
		extraMenuOptionsCaptions = std::vector<std::string>();
		extraMenuActiveItems = std::vector<bool>();

		// subMenu training
		trainingMenuOptionsStrings = std::vector<std::string>();
		trainingMenuOptionsScenes = std::vector<FK_Scene::FK_SceneTrainingType>();
		trainingMenuOptionsCaptions = std::vector<std::string>();
		trainingMenuActiveItems = std::vector<bool>();
	}

	void FK_SceneMainMenu::dispose(){
		itemSelected = false;
		menu_bgm.stop();
		menuIndex = 0;
		clearMenuVariables();
		delete tutorialMenu;
		delete tutorialTextWindow;
		delete unlockablesTextWindow;
		delete unlockablesMenu;
		if (unlockableContentWindow != NULL) {
			delete unlockableContentWindow;
		}
		delete soundManager;
		// clear the scene of all scene nodes
		FK_SceneWithInput::dispose();
	}
	void FK_SceneMainMenu::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		// if this is the first update cycle, play "select character" voice and the BGM
		if (cycleId == 0){
			cycleId = 1;
			menu_bgm.play();
			delta_t_ms = 0;
		}
		if (hasToSetupJoypad()){
			updateJoypadSetup(delta_t_ms);
		}
		else{
			captionStillPeriodCounterMs += delta_t_ms;
			if (captionStillPeriodCounterMs > captionStillPeriodThresholdMs){
				captionMovementTimeCounterMs += delta_t_ms;
			}
			if (!(canProcessUnlockedContent || unlockableContentWindow != NULL)) {
				if (showTutorialPrompt && !tutorialPresented &&
					!(tutorialMenu->isActive() || tutorialTextWindow->isOpening())) {
					if (beforeTutorialTimer >= tutorialIntroMaxTime) {
						activateTutorialMenu();
						beforeTutorialTimer = 0;
					}
					else {
						beforeTutorialTimer += delta_t_ms;
					}
				}
				else if (tutorialPresented) {
					if (showUnlockablesPrompt && !unlockablesChoicePresented &&
						!(unlockablesMenu->isActive() || unlockablesTextWindow->isOpening())) {
						if (beforeTutorialTimer >= tutorialIntroMaxTime) {
							activateUnlockablesMenu();
							beforeTutorialTimer = 0;
						}
						else {
							beforeTutorialTimer += delta_t_ms;
						}
					}
				}
			}

			tutorialTextWindow->update(delta_t_ms);
			unlockablesTextWindow->update(delta_t_ms);

			if (tutorialTextWindow->isOpen() && !tutorialMenu->isActive()){
				tutorialMenu->setActive(true);
				tutorialMenu->setVisible(true);
			}
			else if (unlockablesTextWindow->isOpen() && !unlockablesMenu->isActive()) {
				unlockablesMenu->setActive(true);
				unlockablesMenu->setVisible(true);
			}
			if (beforeTutorialTimer == 0 && !tutorialTextWindow->isAnimating() && 
				!unlockablesTextWindow->isAnimating()){
				updateInput();
			}
		}
		// draw scene
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		drawAll();
		if (hasToSetupJoypad()){
			drawJoypadSetup(now);
		}
		else if (beforeTutorialTimer > 0){
			s32 alpha = std::floor((128 * beforeTutorialTimer) / (f32)(tutorialIntroMaxTime) + 1);
			driver->draw2DRectangle(SColor(alpha, 0, 0, 0),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else if (tutorialMenu->isActive() || tutorialTextWindow->isAnimating()) {
			driver->draw2DRectangle(SColor(128, 0, 0, 0),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			tutorialTextWindow->draw(screenSize, 1.f, 1.f, false);
			tutorialMenu->draw();
		}
		else if (unlockablesMenu->isActive() || unlockablesTextWindow->isAnimating()) {
			driver->draw2DRectangle(SColor(128, 0, 0, 0),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			unlockablesTextWindow->draw(screenSize, 1.f, 1.f, false);
			unlockablesMenu->draw();
		}else if(unlockableContentWindow != NULL){
			driver->draw2DRectangle(SColor(128, 0, 0, 0),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			unlockableContentWindow->draw(screenSize, 1.f, 1.f, false);
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}

	// setup default resources
	void FK_SceneMainMenu::setupDefaultResources(){
		// menu BGM name
		menuBGMName = "menu(astral)_cut2.ogg";
		menuBGMvolume = 60.0f;
		menuBGMpitch = 1.0f;
		menuBackgroundTextureName = "menuBackground.png";
		menuCaptionBackgroundTextureName = "menu_CaptionOverlay.png";
		menuItemBarTextureName = "menuItemBarSmall.png";
		menuSelectedItemBarTextureName = "menuItemBarSelected.png";
		itemMainViewport = core::rect<s32>(0, 64, 640, 296);
		itemMainSize = core::rect<s32>(0, 48, 320, 48);
		itemMainTextOffset = core::rect<s32>(32, 0, 15, 0);
		itemSubSize = core::rect<s32>(0, 32, 280, 28);
		itemSubViewport = core::rect<s32>(360, 64, 280, 28);
		itemSubTextOffset = core::rect<s32>(12, 0, 15, 0);
		itemMainAlignment = FK_MenuTextAlignment::Left;
		itemSubAlignment = FK_MenuTextAlignment::Right;
		secretModeDefaultCaption = "This mode is still [TOP SECRET]. Play more to unlock it!";
		itemShowCaptions = true;
		backToIntroWhenCancelIsPressed = true;
		showMenuWithSubMenu = true;
		setupMenuItems();
		setupFreeMatchMenuItems();
		setupArcadeMenuItems();
		// tutorial related variables
		showTutorialPrompt = true;
		showUnlockablesPrompt = true;
		tutorialPromptMessage = { "Would you like to access the game tutorial?",
			"You can also access it later by selecting Fight Lab > Tutorial."
			"(Select menu items with C key / A button, cancel with X key / B button)" };
		unlockablesPromptMessage = { "Some characters and stages are unlocked through Arcade and Story.",
			"Do you wish to have them all from the beginning instead?",
			"(this choice cannot be undone)" };
		characterUnlockedMessage = "You have unlocked a new character:";
		characterOutfitUnlockedMessage = "You have unlocked a new outfit for %chara_name:";
		stageUnlockedMessage = "You have unlocked a new stage:";
		characterDioramaUnlockedMessage = "You have unlocked a new profile:";
		gameModeUnlockedMessage = "You have unlocked a new game mode:";
		trialUnlockedMessage = "You have unlocked a new trial:";
		storyEpisodeUnlockedMessage = "You have unlocked a new story episode:";
		pictureUnlockedMessage = "You have unlocked new pictures in the gallery:";
	}

	// append new item to  menu
	void FK_SceneMainMenu::appendNewMenuItem(std::ifstream& inputFile, FK_Scene::FK_SceneType sceneType) {
		std::string temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		menuOptionsStrings.push_back(temp);
		temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		menuOptionsCaptions.push_back(temp);
		menuOptionsScenes.push_back(sceneType);
		s32 sceneAvailable = 1;
		inputFile >> sceneAvailable;
		bool sceneIsAvailable = sceneAvailable > 0;
		menuActiveItems.push_back(sceneIsAvailable);
	}

	void FK_SceneMainMenu::appendNewMenuItemFreeMatch(std::ifstream & inputFile, FK_SceneFreeMatchType sceneType) {
		std::string temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		freeMatchMenuOptionsStrings.push_back(temp);
		temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		freeMatchMenuOptionsCaptions.push_back(temp);
		freeMatchMenuOptionsScenes.push_back(sceneType);
		s32 sceneAvailable = 1;
		inputFile >> sceneAvailable;
		bool sceneIsAvailable = sceneAvailable > 0;
		freeMatchMenuActiveItems.push_back(sceneIsAvailable);
	}

	void FK_SceneMainMenu::appendNewMenuItemArcade(std::ifstream & inputFile, FK_SceneArcadeType sceneType) {
		std::string temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		arcadeMenuOptionsStrings.push_back(temp);
		temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		arcadeMenuOptionsCaptions.push_back(temp);
		arcadeMenuOptionsScenes.push_back(sceneType);
		s32 sceneAvailable = 1;
		inputFile >> sceneAvailable;
		bool sceneIsAvailable = sceneAvailable > 0;
		arcadeMenuActiveItems.push_back(sceneIsAvailable);
	}

	void FK_SceneMainMenu::appendNewMenuItemTraining(std::ifstream & inputFile, FK_SceneTrainingType sceneType) {
		std::string temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		trainingMenuOptionsStrings.push_back(temp);
		temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		trainingMenuOptionsCaptions.push_back(temp);
		trainingMenuOptionsScenes.push_back(sceneType);
		s32 sceneAvailable = 1;
		inputFile >> sceneAvailable;
		bool sceneIsAvailable = sceneAvailable > 0;
		trainingMenuActiveItems.push_back(sceneIsAvailable);
	}

	void FK_SceneMainMenu::appendNewMenuItemExtra(std::ifstream & inputFile, FK_SceneExtraType sceneType) {
		std::string temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		extraMenuOptionsStrings.push_back(temp);
		temp = std::string();
		while (temp.empty()) {
			std::getline(inputFile, temp);
		}
		extraMenuOptionsCaptions.push_back(temp);
		extraMenuOptionsScenes.push_back(sceneType);
		s32 sceneAvailable = 1;
		inputFile >> sceneAvailable;
		bool sceneIsAvailable = sceneAvailable > 0;
		extraMenuActiveItems.push_back(sceneIsAvailable);
	}

	//load configuration from file
	bool FK_SceneMainMenu::loadConfigurationFile(){
		std::string resourcePath = mediaPath + FK_SceneMainMenu::MenuResourcesPath;
		std::string filename = resourcePath + "config.txt";
		if (gameOptions->getTourneyMode()) {
			filename = resourcePath + "tourney_config.txt";
		}
		std::ifstream inputFile(filename.c_str());
		if (!inputFile) {
			if (gameOptions->getTourneyMode()) {
				filename = resourcePath + "config.txt";
				inputFile.clear();
				inputFile.open(filename);
				if (!inputFile) {
					return false;
				}
			}
			else {
				return false;
			}
		}
		std::map<FK_MainMenuConfigFileKeys, std::string> fileKeys;
		fileKeys[FK_MainMenuConfigFileKeys::MenuBGM] = "#bgm";
		fileKeys[FK_MainMenuConfigFileKeys::BackgroundPicture] = "#background_picture";
		fileKeys[FK_MainMenuConfigFileKeys::CaptionBackground] = "#caption_background_texture";
		fileKeys[FK_MainMenuConfigFileKeys::ItemBackground] = "#item_bar_texture";
		fileKeys[FK_MainMenuConfigFileKeys::SelectedItemBackground] = "#selected_item_bar_texture";
		fileKeys[FK_MainMenuConfigFileKeys::MainItemViewport] = "#main_item_viewport";
		fileKeys[FK_MainMenuConfigFileKeys::MainItemSize] = "#main_item_size";
		fileKeys[FK_MainMenuConfigFileKeys::MainItemTextOffset] = "#main_item_text_offset";
		fileKeys[FK_MainMenuConfigFileKeys::SubItemViewport] = "#sub_item_viewport";
		fileKeys[FK_MainMenuConfigFileKeys::SubItemSize] = "#sub_item_size";
		fileKeys[FK_MainMenuConfigFileKeys::SubItemTextOffset] = "#sub_item_text_offset";
		fileKeys[FK_MainMenuConfigFileKeys::MainItemTextCentered] = "#main_item_text_alignment";
		fileKeys[FK_MainMenuConfigFileKeys::SubItemTextCentered] = "#sub_item_text_alignment";
		fileKeys[FK_MainMenuConfigFileKeys::ItemBlinkingColor] = "#selected_item_blinking_color";
		fileKeys[FK_MainMenuConfigFileKeys::MainMenuItems] = "#main_menu_items";
		fileKeys[FK_MainMenuConfigFileKeys::MainMenuItemsEnd] = "#main_menu_items_end";
		fileKeys[FK_MainMenuConfigFileKeys::FreeMatchItems] = "#free_match_menu_items";
		fileKeys[FK_MainMenuConfigFileKeys::FreeMatchItemsEnd] = "#free_match_menu_items_end";
		fileKeys[FK_MainMenuConfigFileKeys::ArcadeItems] = "#arcade_menu_items";
		fileKeys[FK_MainMenuConfigFileKeys::ArcadeItemsEnd] = "#arcade_menu_items_end";
		fileKeys[FK_MainMenuConfigFileKeys::ExtraItems] = "#extra_menu_items";
		fileKeys[FK_MainMenuConfigFileKeys::ExtraItemsEnd] = "#extra_menu_items_end";
		fileKeys[FK_MainMenuConfigFileKeys::TrainingItems] = "#training_menu_items";
		fileKeys[FK_MainMenuConfigFileKeys::TrainingItemsEnd] = "#training_menu_items_end";
		fileKeys[FK_MainMenuConfigFileKeys::SecretModeDefaultCaption] = "#secret_modes_menu_caption";
		fileKeys[FK_MainMenuConfigFileKeys::ShowCaptions] = "#show_captions";
		fileKeys[FK_MainMenuConfigFileKeys::CancelBackToIntro] = "#back_to_title_CANCEL_button";
		fileKeys[FK_MainMenuConfigFileKeys::ShowMenuWithSubMenu] = "#show_menu_and_subMenu";
		fileKeys[FK_MainMenuConfigFileKeys::ShowTutorialPrompt] = "#show_tutorial_prompt";
		fileKeys[FK_MainMenuConfigFileKeys::TutorialMessage] = "#tutorial_prompt_message";
		fileKeys[FK_MainMenuConfigFileKeys::TutorialMessageEnd] = "#tutorial_prompt_message_end";
		fileKeys[FK_MainMenuConfigFileKeys::ShowUnlockablesPrompt] = "#show_unlockables_prompt";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockablesMessage] = "#unlockables_prompt_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockablesMessageEnd] = "#unlockables_prompt_message_end";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedCharacterMessage] = "#character_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedOutfitMessage] = "#character_outfit_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedStageMessage] = "#stage_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedDioramaMessage] = "#diorama_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedGameModeMessage] = "#game_mode_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedStoryEpisodeMessage] = "#story_episode_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedTrialMessage] = "#challenge_unlocked_message";
		fileKeys[FK_MainMenuConfigFileKeys::UnlockedPictureMessage] = "#picture_unlocked_message";

		std::map<std::string, FK_SceneType> menuItemFileKeys;
		menuItemFileKeys["##STORY"] = FK_SceneType::SceneStoryModeSelection;
		menuItemFileKeys["##ARCADE"] = FK_SceneType::SceneCharacterSelectionArcade;
		menuItemFileKeys["##FREE_MATCH"] = FK_SceneType::SceneFreeMatchSelection;
		menuItemFileKeys["##QUIT"] = FK_SceneType::SceneQuitGame;
		menuItemFileKeys["##TRAINING"] = FK_SceneType::SceneTrainingSelection;
		menuItemFileKeys["##CREDITS"] = FK_SceneType::SceneCredits;
		menuItemFileKeys["##EXTRA"] = FK_SceneType::SceneExtra;
		menuItemFileKeys["##OPTIONS"] = FK_SceneType::SceneOptions;

		std::map<std::string, FK_SceneFreeMatchType> freeMatchItemFileKeys;
		freeMatchItemFileKeys["##P_V_P"] = FK_SceneFreeMatchType::FreeMatchMultiplayer;
		freeMatchItemFileKeys["##P_V_CPU"] = FK_SceneFreeMatchType::FreeMatchHumanPlayerVsCPU;
		freeMatchItemFileKeys["##P1_V_CPU"] = FK_SceneFreeMatchType::FreeMatchPlayer1VsCPU;
		freeMatchItemFileKeys["##CPU_V_P1"] = FK_SceneFreeMatchType::FreeMatchCPUVsPlayer2;
		freeMatchItemFileKeys["##CPU_V_CPU"] = FK_SceneFreeMatchType::FreeMatchCPUVsCPU;
		freeMatchItemFileKeys["##TRAINING"] = FK_SceneFreeMatchType::FreeMatchTraining;
		freeMatchItemFileKeys["##TRIALS"] = FK_SceneFreeMatchType::FreeMatchTutorial;

		std::map<std::string, FK_SceneArcadeType> arcadeItemFileKeys;
		arcadeItemFileKeys["##CLASSIC"] = FK_SceneArcadeType::ArcadeClassic;
		arcadeItemFileKeys["##FREE_TRIGGER"] = FK_SceneArcadeType::ArcadeSpecial1;
		arcadeItemFileKeys["##SAVAGE"] = FK_SceneArcadeType::ArcadeSpecial2;
		arcadeItemFileKeys["##FIRST_HIT"] = FK_SceneArcadeType::ArcadeSpecial3;
		arcadeItemFileKeys["##EXTRA4"] = FK_SceneArcadeType::ArcadeSpecial4;
		arcadeItemFileKeys["##SURVIVAL"] = FK_SceneArcadeType::ArcadeSurvival;
		arcadeItemFileKeys["##TIME_ATTACK"] = FK_SceneArcadeType::ArcadeTimeAttack;

		std::map<std::string, FK_SceneExtraType> extraItemFileKeys;
		extraItemFileKeys["##GALLERY"] = FK_SceneExtraType::ExtraGallery;
		extraItemFileKeys["##SOUND_PLAYER"] = FK_SceneExtraType::ExtraSoundPlayer;
		extraItemFileKeys["##PROFILES"] = FK_SceneExtraType::ExtraProfiles;
		extraItemFileKeys["##ARCADE_ENDINGS"] = FK_SceneExtraType::ExtraArcadeEndingGallery;
		extraItemFileKeys["##VIEWER"] = FK_SceneExtraType::ExtraStages;
		extraItemFileKeys["##DIORAMA"] = FK_SceneExtraType::ExtraDiorama;
		extraItemFileKeys["##MUSIC"] = FK_SceneExtraType::ExtraMusicPlayer;
		extraItemFileKeys["##GACHA"] = FK_SceneExtraType::ExtraGachablitz;
		extraItemFileKeys["##DISCORD"] = FK_SceneExtraType::ExtraDiscordLink;

		std::map<std::string, FK_SceneTrainingType> trainingItemFileKeys;
		trainingItemFileKeys["##TRAINING"] = FK_SceneTrainingType::TrainingNormal;
		trainingItemFileKeys["##TRIALS"] = FK_SceneTrainingType::TrainingTrial;
		trainingItemFileKeys["##TUTORIAL"] = FK_SceneTrainingType::TrainingTutorial;

		std::string temp;
		while (inputFile >> temp) {
			if (!inputFile) {
				break;
			}
			if (temp == fileKeys[FK_MainMenuConfigFileKeys::SecretModeDefaultCaption]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				secretModeDefaultCaption = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ShowTutorialPrompt]) {
				s32 flag;
				inputFile >> flag;
				showTutorialPrompt = flag > 0;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ShowUnlockablesPrompt]) {
				s32 flag;
				inputFile >> flag;
				showUnlockablesPrompt = flag > 0;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::TutorialMessage]) {
				temp = std::string();
				tutorialPromptMessage.clear();
				while (inputFile) {
					while (temp.empty() || temp == " " || temp == "\t") {
						std::getline(inputFile, temp);
					}
					if (temp == fileKeys[FK_MainMenuConfigFileKeys::TutorialMessageEnd]) {
						break;
					}
					else {
						tutorialPromptMessage.push_back(temp);
					}
					temp = std::string();
				}
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockablesMessage]) {
				temp = std::string();
				unlockablesPromptMessage.clear();
				while (inputFile) {
					while (temp.empty() || temp == " " || temp == "\t") {
						std::getline(inputFile, temp);
					}
					if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockablesMessageEnd]) {
						break;
					}
					else {
						unlockablesPromptMessage.push_back(temp);
					}
					temp = std::string();
				}
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedCharacterMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				characterUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedOutfitMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				characterOutfitUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedStageMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				stageUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedDioramaMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				characterDioramaUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedGameModeMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				gameModeUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedStoryEpisodeMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				storyEpisodeUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedTrialMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				trialUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::UnlockedPictureMessage]) {
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				pictureUnlockedMessage = temp;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::MenuBGM]) {
				inputFile >> menuBGMName >> menuBGMvolume >> menuBGMpitch;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ShowCaptions]) {
				s32 flag;
				inputFile >> flag;
				itemShowCaptions = flag > 0;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::CancelBackToIntro]) {
				s32 flag;
				inputFile >> flag;
				backToIntroWhenCancelIsPressed = flag > 0;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ShowMenuWithSubMenu]) {
				s32 flag;
				inputFile >> flag;
				showMenuWithSubMenu = flag > 0;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::BackgroundPicture]) {
				inputFile >> menuBackgroundTextureName;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ItemBackground]) {
				inputFile >> menuItemBarTextureName;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::SelectedItemBackground]) {
				inputFile >> menuSelectedItemBarTextureName;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::CaptionBackground]) {
				inputFile >> menuCaptionBackgroundTextureName;
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::MainItemViewport]) {
				s32 offsetX, offsetY, sizeX, sizeY;
				inputFile >> offsetX >> offsetY >> sizeX >> sizeY;
				itemMainViewport = core::rect<s32>(offsetX, offsetY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::MainItemSize]) {
				s32 spacingX, spacingY, sizeX, sizeY;
				inputFile >> spacingX >> spacingY >> sizeX >> sizeY;
				itemMainSize = core::rect<s32>(spacingX, spacingY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::MainItemTextOffset]) {
				s32 offsetX, offsetY, offsetWhenSelectedX, offsetWhenSelectedY;
				inputFile >> offsetX >> offsetY >> offsetWhenSelectedX >> offsetWhenSelectedY;
				itemMainTextOffset = core::rect<s32>(offsetX, offsetY, offsetWhenSelectedX, offsetWhenSelectedY);
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::SubItemViewport]) {
				s32 offsetX, offsetY, sizeX, sizeY;
				inputFile >> offsetX >> offsetY >> sizeX >> sizeY;
				itemSubViewport = core::rect<s32>(offsetX, offsetY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::SubItemSize]) {
				s32 spacingX, spacingY, sizeX, sizeY;
				inputFile >> spacingX >> spacingY >> sizeX >> sizeY;
				itemSubSize = core::rect<s32>(spacingX, spacingY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::SubItemTextOffset]) {
				s32 offsetX, offsetY, offsetWhenSelectedX, offsetWhenSelectedY;
				inputFile >> offsetX >> offsetY >> offsetWhenSelectedX >> offsetWhenSelectedY;
				itemSubTextOffset = core::rect<s32>(offsetX, offsetY, offsetWhenSelectedX, offsetWhenSelectedY);
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::MainItemTextCentered]) {
				s32 flag;
				inputFile >> flag;
				if (flag == 0) {
					itemMainAlignment = FK_MenuTextAlignment::Center;
				}
				else if (flag > 0) {
					itemMainAlignment = FK_MenuTextAlignment::Right;
				}
				else {
					itemMainAlignment = FK_MenuTextAlignment::Left;
				}
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::SubItemTextCentered]) {
				s32 flag;
				inputFile >> flag;
				if (flag == 0) {
					itemSubAlignment = FK_MenuTextAlignment::Center;
				}
				else if (flag > 0) {
					itemSubAlignment = FK_MenuTextAlignment::Right;
				}
				else {
					itemSubAlignment = FK_MenuTextAlignment::Left;
				}
			}
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ItemBlinkingColor]) {
				f32 period;
				u32 a, r, g, b;
				inputFile >> period;
				itemBlinkingColor.colorTransitionPeriodMs = period;
				inputFile >> a >> r >> g >> b;
				itemBlinkingColor.startingColor = video::SColor(a, r, g, b);
				inputFile >> a >> r >> g >> b;
				itemBlinkingColor.endingColor = video::SColor(a, r, g, b);
			}
			// set which element is in the main menu and erase default values
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::MainMenuItems]) {
				menuOptionsStrings.clear();
				menuActiveItems.clear();
				menuOptionsScenes.clear();
				menuOptionsCaptions.clear();
				while (inputFile >> temp && temp != fileKeys[FK_MainMenuConfigFileKeys::MainMenuItemsEnd]) {
					if (menuItemFileKeys.count(temp) > 0) {
						appendNewMenuItem(inputFile, menuItemFileKeys[temp]);
					}
				}
			}
			// set which element is in the free match menu and erase default values
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::FreeMatchItems]) {
				freeMatchMenuOptionsStrings.clear();
				freeMatchMenuActiveItems.clear();
				freeMatchMenuOptionsScenes.clear();
				freeMatchMenuOptionsCaptions.clear();
				while (inputFile >> temp && temp != fileKeys[FK_MainMenuConfigFileKeys::FreeMatchItemsEnd]) {
					if (freeMatchItemFileKeys.count(temp) > 0) {
						appendNewMenuItemFreeMatch(inputFile, freeMatchItemFileKeys[temp]);
					}
				}
			}
			// set which element is in the arcade menu and erase default values
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ArcadeItems]) {
				arcadeMenuOptionsStrings.clear();
				arcadeMenuActiveItems.clear();
				arcadeMenuOptionsScenes.clear();
				arcadeMenuOptionsCaptions.clear();
				while (inputFile >> temp && temp != fileKeys[FK_MainMenuConfigFileKeys::ArcadeItemsEnd]) {
					if (arcadeItemFileKeys.count(temp) > 0) {
						appendNewMenuItemArcade(inputFile, arcadeItemFileKeys[temp]);
					}
				}
			}
			// set which element is in the extra menu and erase default values
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::ExtraItems]) {
				extraMenuOptionsStrings.clear();
				extraMenuActiveItems.clear();
				extraMenuOptionsScenes.clear();
				extraMenuOptionsCaptions.clear();
				while (inputFile >> temp && temp != fileKeys[FK_MainMenuConfigFileKeys::ExtraItemsEnd]) {
					if (extraItemFileKeys.count(temp) > 0) {
						appendNewMenuItemExtra(inputFile, extraItemFileKeys[temp]);
					}
				}
			}
			// set which element is in the training menu and erase default values
			else if (temp == fileKeys[FK_MainMenuConfigFileKeys::TrainingItems]) {
				trainingMenuOptionsStrings.clear();
				trainingMenuActiveItems.clear();
				trainingMenuOptionsScenes.clear();
				trainingMenuOptionsCaptions.clear();
				while (inputFile >> temp && temp != fileKeys[FK_MainMenuConfigFileKeys::TrainingItemsEnd]) {
					if (trainingItemFileKeys.count(temp) > 0) {
						appendNewMenuItemTraining(inputFile, trainingItemFileKeys[temp]);
					}
				}
			}
		}
		return true;
	}

	// set unavailable menu items
	void FK_SceneMainMenu::setUnavailableMenuItems() {
		for (int i = 0; i < menuActiveItems.size(); ++i) {
			if (!menuActiveItems[i]) {
				std::string tagString = menuOptionsStrings[i];
				std::replace(tagString.begin(), tagString.end(), ' ', '_');
				bool unlockedMode = std::find(unlockedGameModes.begin(), unlockedGameModes.end(), tagString) != 
					unlockedGameModes.end();
				if (unlockedMode) {
					menuActiveItems[i] = true;
				}else{
					menuOptionsStrings[i] = "???";
					menuOptionsCaptions[i] = secretModeDefaultCaption;
				}
			}
		}
		// arcade subMenu
		for (int i = 0; i < arcadeMenuActiveItems.size(); ++i) {
			if (!arcadeMenuActiveItems[i]) {
				std::string tagString = "Arcade\\" + arcadeMenuOptionsStrings[i];
				std::replace(tagString.begin(), tagString.end(), ' ', '_');
				bool unlockedMode = std::find(unlockedGameModes.begin(), unlockedGameModes.end(), tagString) !=
					unlockedGameModes.end();
				if (unlockedMode) {
					arcadeMenuActiveItems[i] = true;
				}
				else {
					arcadeMenuOptionsStrings[i] = "???";
					arcadeMenuOptionsCaptions[i] = secretModeDefaultCaption;
				}
			}
		}
		// Free Match subMenu
		for (int i = 0; i < freeMatchMenuActiveItems.size(); ++i) {
			if (!freeMatchMenuActiveItems[i]) {
				std::string tagString = "FreeMatch\\" + freeMatchMenuOptionsStrings[i];
				std::replace(tagString.begin(), tagString.end(), ' ', '_');
				bool unlockedMode = std::find(unlockedGameModes.begin(), unlockedGameModes.end(), tagString) !=
					unlockedGameModes.end();
				if (unlockedMode) {
					freeMatchMenuActiveItems[i] = true;
				}
				else {
					freeMatchMenuOptionsStrings[i] = "???";
					freeMatchMenuOptionsCaptions[i] = secretModeDefaultCaption;
				}
			}
		}
		// Extra subMenu
		for (int i = 0; i < extraMenuActiveItems.size(); ++i) {
			if (!extraMenuActiveItems[i]) {
				std::string tagString = "Extra\\" + extraMenuOptionsStrings[i];
				std::replace(tagString.begin(), tagString.end(), ' ', '_');
				bool unlockedMode = std::find(unlockedGameModes.begin(), unlockedGameModes.end(), tagString) !=
					unlockedGameModes.end();
				if (unlockedMode) {
					extraMenuActiveItems[i] = true;
				}
				else {
					extraMenuOptionsStrings[i] = "???";
					extraMenuOptionsCaptions[i] = secretModeDefaultCaption;
				}
			}
		}
		// Training subMenu
		for (int i = 0; i < trainingMenuActiveItems.size(); ++i) {
			if (!trainingMenuActiveItems[i]) {
				std::string tagString = "Training\\" + trainingMenuOptionsStrings[i];
				std::replace(tagString.begin(), tagString.end(), ' ', '_');
				bool unlockedMode = std::find(unlockedGameModes.begin(), unlockedGameModes.end(), tagString) !=
					unlockedGameModes.end();
				if (unlockedMode) {
					trainingMenuActiveItems[i] = true;
				}
				else {
					trainingMenuOptionsStrings[i] = "???";
					trainingMenuOptionsCaptions[i] = secretModeDefaultCaption;
				}
			}
		}
	}

	// setup BGM
	void FK_SceneMainMenu::setupBGM(){
		systemBGMPath = commonResourcesPath + "system_bgm\\";
		std::string filePath = systemBGMPath + menuBGMName;
		if (!menu_bgm.openFromFile(filePath.c_str()))
			return;
		menu_bgm.setLoop(true);
		menu_bgm.setVolume(menuBGMvolume * gameOptions->getMusicVolume());
		menu_bgm.setPitch(menuBGMpitch);
	}

	// setup sound manager
	void FK_SceneMainMenu::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
		soundManager->addSoundFromDefaultPath("unlock", "success.ogg");
	}

	void FK_SceneMainMenu::setupGraphics(){
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		subMenufont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		captionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		/* textures */
		std::string resourcePath = mediaPath + FK_SceneMainMenu::MenuResourcesPath;
		menuBackgroundTexture = driver->getTexture((resourcePath + menuBackgroundTextureName).c_str());
		menuCaptionOverlay = driver->getTexture((resourcePath + menuCaptionBackgroundTextureName).c_str());
		itemBarTexture = driver->getTexture((resourcePath + menuItemBarTextureName).c_str());
		itemBarSelectedTexture = driver->getTexture((resourcePath + menuSelectedItemBarTextureName).c_str());
		// load joypad settings textures
		loadJoypadSetupTextures();
	}

	// setup menu items
	void FK_SceneMainMenu::setupMenuItems(){
		menuOptionsStrings.push_back("Story Mode");
		menuOptionsStrings.push_back("Arcade Mode");
		menuOptionsStrings.push_back("Free Match");
		//menuOptionsStrings.push_back("Training");
		menuOptionsStrings.push_back("Options");
		//menuOptionsStrings.push_back("Credits");
		menuOptionsStrings.push_back("Quit");
		//
		menuOptionsCaptions.push_back("Discover the story behind Schwarzerblitz, following each individual character's path");
		menuOptionsCaptions.push_back("Fight in a series of matches of increasing difficulty. You can also take the challenge of the various special modes");
		menuOptionsCaptions.push_back("Fight against another player or a computer-controlled opponent. You can also watch a computer vs. computer match");
		menuOptionsCaptions.push_back("Modify various options and settings, such as difficulty, video and audio options");
		//menuOptionsCaptions.push_back("Find out who contributed to build Schwarzerblitz");
		menuOptionsCaptions.push_back("Quit game and return to Windows");
		//
		menuOptionsScenes.push_back(FK_Scene::FK_SceneType::SceneStoryModeSelection);
		menuOptionsScenes.push_back(FK_Scene::FK_SceneType::SceneCharacterSelectionArcade);
		menuOptionsScenes.push_back(FK_Scene::FK_SceneType::SceneFreeMatchSelection);
		menuOptionsScenes.push_back(FK_Scene::FK_SceneType::SceneOptions);
		//menuOptionsScenes.push_back(FK_Scene::FK_SceneType::SceneCredits);
		menuOptionsScenes.push_back(FK_Scene::FK_SceneType::SceneQuitGame);
		//
		for (s32 i = 0; i < menuOptionsStrings.size(); ++i){
			menuActiveItems.push_back(true);
		}
		// only for debug reasons!!!
		//menuActiveItems[1] = false;
	}

	// setup free match itmes
	void FK_SceneMainMenu::setupFreeMatchMenuItems(){
		freeMatchMenuOptionsStrings.push_back("Player vs Player");
		freeMatchMenuOptionsStrings.push_back("Player vs CPU");
		//freeMatchMenuOptionsStrings.push_back("CPU vs Player 2");
		freeMatchMenuOptionsStrings.push_back("CPU vs CPU");
		freeMatchMenuOptionsStrings.push_back("Training");
		freeMatchMenuOptionsStrings.push_back("Challenge");
		//
		freeMatchMenuOptionsCaptions.push_back("Fight against a friend in a player vs player match (if no joypad is connected, the keyboard layout is split for the two players - see KEYBOARD SETTINGS for more information)");
		freeMatchMenuOptionsCaptions.push_back("Test your skill against a computer-controlled opponent (player 1). The AI level can be set in the Options menu");
		//freeMatchMenuOptionsCaptions.push_back("Test your skill against a computer-controlled opponent (player 2). The AI level can be set in the Options menu");
		freeMatchMenuOptionsCaptions.push_back("Watch an exhibition match performed by two computer-controlled players. The AI level can be set in the Options menu");
		freeMatchMenuOptionsCaptions.push_back("Improve your skills and try out a character against a dummy, learning how to manage every possible game situation");
		freeMatchMenuOptionsCaptions.push_back("Learn the game basics through in-depth tutorials and challenge yourself in learning every character's combo");
		//
		freeMatchMenuOptionsScenes.push_back(FK_Scene::FK_SceneFreeMatchType::FreeMatchMultiplayer);
		freeMatchMenuOptionsScenes.push_back(FK_Scene::FK_SceneFreeMatchType::FreeMatchPlayer1VsCPU);
		//freeMatchMenuOptionsScenes.push_back(FK_Scene::FK_SceneType::FreeMatchCPUVsPlayer2);
		freeMatchMenuOptionsScenes.push_back(FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPU);
		freeMatchMenuOptionsScenes.push_back(FK_Scene::FK_SceneFreeMatchType::FreeMatchTraining);
		freeMatchMenuOptionsScenes.push_back(FK_Scene::FK_SceneFreeMatchType::FreeMatchTutorial);
		//
		for (s32 i = 0; i < freeMatchMenuOptionsStrings.size(); ++i){
			freeMatchMenuActiveItems.push_back(true);
		}
		// only for debug reasons!!!
		//freeMatchMenuActiveItems[4] = false;
	}

	// setup free match itmes
	void FK_SceneMainMenu::setupArcadeMenuItems(){
		arcadeMenuOptionsStrings.push_back("Classic");
		arcadeMenuOptionsStrings.push_back("Trigger Happy");
		arcadeMenuOptionsStrings.push_back("Insane");
		arcadeMenuOptionsStrings.push_back("Quicksilver");
		//
		arcadeMenuOptionsCaptions.push_back("Fight your way through opponents of increasing difficulty level and discover more about Schwarzerblitz story");
		arcadeMenuOptionsCaptions.push_back("A classic arcade mode - with a twist: Bullet Counters regenerate over time");
		arcadeMenuOptionsCaptions.push_back("The odds are against you: your opponent needs to win only one round, and your health doesn't regenerate... but your Bullet Counters do! Only for hardcore gamers - note: this mode has no Climatic Battle before the boss");
		arcadeMenuOptionsCaptions.push_back("Who's the quickest to draw their gun? Try it out in a one-hit-one-kill route with increasing amount of round! - note: this mode has no Climatic Battle before the boss");
		//
		arcadeMenuOptionsScenes.push_back(FK_Scene::FK_SceneArcadeType::ArcadeClassic);
		arcadeMenuOptionsScenes.push_back(FK_Scene::FK_SceneArcadeType::ArcadeSpecial1);
		arcadeMenuOptionsScenes.push_back(FK_Scene::FK_SceneArcadeType::ArcadeSpecial2);
		arcadeMenuOptionsScenes.push_back(FK_Scene::FK_SceneArcadeType::ArcadeSpecial3);
		//
		for (s32 i = 0; i < arcadeMenuOptionsScenes.size(); ++i){
			std::string tagString = "Arcade\\" + arcadeMenuOptionsStrings[i];
			std::replace(tagString.begin(), tagString.end(), ' ', '_');
			bool unlockedMode = std::find(unlockedGameModes.begin(), unlockedGameModes.end(), tagString) != unlockedGameModes.end();
			if (unlockedMode || i == 0){
				arcadeMenuActiveItems.push_back(true);
			}
			else{
				arcadeMenuActiveItems.push_back(false);
			}
		}
		for (s32 i = 0; i < arcadeMenuOptionsStrings.size(); ++i){
			if (!arcadeMenuActiveItems[i]){
				arcadeMenuOptionsStrings[i] = "???";
				arcadeMenuOptionsCaptions[i] = secretModeDefaultCaption;
			}
		}
	}

	void FK_SceneMainMenu::selectItem(){
		lastInputTime = now;
		inputReceiver->reset();
		if (isSelectingSubMenu) {
			if (subMenuType == FK_MenuSubMenuType::Arcade) {
				if (arcadeMenuActiveItems[subMenuIndex]) {
					soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
					itemSelected = true;
					setNextScene(FK_SceneType::SceneCharacterSelectionArcade);
				}
				else {
					soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
					return;
				}
			}
			else if (subMenuType == FK_MenuSubMenuType::FreeMatch) {
				if (freeMatchMenuActiveItems[subMenuIndex]) {
					soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
					itemSelected = true;
					if (freeMatchMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneFreeMatchType::FreeMatchTutorial) {
						setNextScene(FK_SceneType::SceneGameTutorial);
					}
					else {
						setNextScene(FK_SceneType::SceneCharacterSelection);
					}
				}
				else {
					soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
					return;
				}
			}
			else if (subMenuType == FK_MenuSubMenuType::Training) {
				if (trainingMenuActiveItems[subMenuIndex]) {
					soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
					itemSelected = true;
					if (trainingMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneTrainingType::TrainingTutorial ||
						trainingMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneTrainingType::TrainingTrial) {
						setNextScene(FK_SceneType::SceneGameTutorial);
					}
					else {
						setNextScene(FK_SceneType::SceneCharacterSelection);
					}
				}
				else {
					soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
					return;
				}
			}
			else if (subMenuType == FK_MenuSubMenuType::Extra) {
				if (extraMenuActiveItems[subMenuIndex]) {
					soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
					itemSelected = true;
					if (extraMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneExtraType::ExtraDiscordLink) {
						// call Discord link
						ShellExecute(0, 0, "https://discord.gg/vXKrvpV", 0, 0, SW_SHOW);
						itemSelected = false;
						Sleep(500);
					}
					else if (extraMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneExtraType::ExtraMusicPlayer) {
						setNextScene(FK_SceneType::SceneMusicPlayer);
					}
					else if (extraMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneExtraType::ExtraDiorama ||
						extraMenuOptionsScenes[subMenuIndex] == FK_Scene::FK_SceneExtraType::ExtraArcadeEndingGallery) {
						setNextScene(FK_SceneType::SceneCharacterSelectionExtra);
					}
					else {
						setNextScene(FK_SceneType::SceneExtra);
					}
				}
				else {
					soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
					return;
				}
			}
		}
		else{
			if (menuActiveItems[menuIndex]){
				soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
				if (getCurrentSubMenuType() != FK_MenuSubMenuType::NoSubmenu) {
					startSubMenuSelection();
				}
				else{
					itemSelected = true;
					setNextScene(menuOptionsScenes[menuIndex]);
					return;
				}
			}
			else{
				soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
				return;
			}
		}
	}

	bool FK_SceneMainMenu::isSubMenuActive() {
		return isSelectingSubMenu;
	}

	void FK_SceneMainMenu::increaseMenuIndex(){
		if (isSelectingSubMenu){
			increaseSubMenuIndex();
		}
		else{
			increaseMainMenuIndex();
		}
	}

	void FK_SceneMainMenu::decreaseMenuIndex(){
		if (isSelectingSubMenu) {
			decreaseSubMenuIndex();
		}
		else {
			decreaseMainMenuIndex();
		}
	}

	void FK_SceneMainMenu::cancelSubMenuSelection(){
		lastInputTime = now;
		inputReceiver->reset();
		soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
		isSelectingSubMenu = false;
		captionMovementTimeCounterMs = 0;
		captionStillPeriodCounterMs = 0;
	}

	void FK_SceneMainMenu::startSubMenuSelection(){
		lastInputTime = now;
		subMenuIndex = 0;
		isSelectingSubMenu = true;
		captionMovementTimeCounterMs = 0;
		captionStillPeriodCounterMs = 0;
		subMenuType = getCurrentSubMenuType();
		if (subMenuType == FK_MenuSubMenuType::FreeMatch) {
			subMenuSize = freeMatchMenuOptionsStrings.size();
		}
		else if (subMenuType == FK_MenuSubMenuType::Arcade) {
			subMenuSize = arcadeMenuOptionsStrings.size();
		}
		else if (subMenuType == FK_MenuSubMenuType::Training) {
			subMenuSize = trainingMenuOptionsStrings.size();
		}
		else if (subMenuType == FK_MenuSubMenuType::Extra) {
			subMenuSize = extraMenuOptionsStrings.size();
		}
	}

	// get submenu type
	FK_SceneMainMenu::FK_MenuSubMenuType FK_SceneMainMenu::getCurrentSubMenuType()
	{
		FK_SceneType menuType = menuOptionsScenes[menuIndex];
		if (menuType == FK_SceneType::SceneFreeMatchSelection) {
			return FK_MenuSubMenuType::FreeMatch;
		}else if (menuType == FK_SceneType::SceneCharacterSelectionArcade){
			return FK_MenuSubMenuType::Arcade;
		}
		else if (menuType == FK_SceneType::SceneTrainingSelection) {
			return FK_MenuSubMenuType::Training;
		}
		else if (menuType == FK_SceneType::SceneExtra) {
			return FK_MenuSubMenuType::Extra;
		}
		return FK_MenuSubMenuType::NoSubmenu;
	}

	void FK_SceneMainMenu::increaseMainMenuIndex(){
		lastInputTime = now;
		soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		menuIndex += 1;
		menuIndex %= menuOptionsStrings.size();
		captionStillPeriodCounterMs = 0;
		captionMovementTimeCounterMs = 0;
	}

	void FK_SceneMainMenu::decreaseMainMenuIndex(){
		lastInputTime = now;
		soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		if (menuIndex == 0){
			menuIndex = menuOptionsStrings.size() - 1;
		}
		else{
			menuIndex -= 1;
		}
		captionStillPeriodCounterMs = 0;
		captionMovementTimeCounterMs = 0;
	}

	// general subMenu
	void FK_SceneMainMenu::increaseSubMenuIndex() {
		lastInputTime = now;
		soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		subMenuIndex += 1;
		subMenuIndex %= subMenuSize;
		captionStillPeriodCounterMs = 0;
		captionMovementTimeCounterMs = 0;
	}
	void FK_SceneMainMenu::decreaseSubMenuIndex() {
		lastInputTime = now;
		soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		if (subMenuIndex == 0) {
			subMenuIndex = subMenuSize - 1;
		}
		else {
			subMenuIndex -= 1;
		}
		captionStillPeriodCounterMs = 0;
		captionMovementTimeCounterMs = 0;
	}

	bool FK_SceneMainMenu::player1HasSelected(){
		return lastSelectionFromPlayer1;
	}

	s32 FK_SceneMainMenu::getCurrentMenuIndex() {
		return menuIndex;
	}

	// unlockable comparison
	void FK_SceneMainMenu::compareUnlockables() {
		canProcessUnlockedContent = false;
		unlockablesToProcess = FK_SceneWithInput::FK_UnlockedContent();
		if (!gameOptions->getTourneyMode()) {
			// characters
			std::vector<std::string> unlockedCharasToCheck = previouslyUnlockedContent.unlockedCharacters;
			for each (auto characterPath in currentlyUnlockedContent.unlockedCharacters) {
				if (std::find(unlockedCharasToCheck.begin(), unlockedCharasToCheck.end(), characterPath) == unlockedCharasToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedCharacters.begin(),
						unlockablesToProcess.unlockedCharacters.end(), characterPath) ==
						unlockablesToProcess.unlockedCharacters.end()) {
						unlockablesToProcess.unlockedCharacters.push_back(characterPath);
						canProcessUnlockedContent = true;
					}
				}
			}
			// costumes
			std::vector<std::string> unlockedCostumesToCheck = previouslyUnlockedContent.unlockedOutfits;
			for each (auto outfit in currentlyUnlockedContent.unlockedOutfits) {
				if (std::find(unlockedCostumesToCheck.begin(), unlockedCostumesToCheck.end(), outfit) == unlockedCostumesToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedOutfits.begin(),
						unlockablesToProcess.unlockedOutfits.end(), outfit) ==
						unlockablesToProcess.unlockedOutfits.end()) {
						unlockablesToProcess.unlockedOutfits.push_back(outfit);
						canProcessUnlockedContent = true;
					}
				}
			}
			// stages
			std::vector<std::string> unlockedStagesToCheck = previouslyUnlockedContent.unlockedStages;
			for each (auto stage in currentlyUnlockedContent.unlockedStages) {
				if (std::find(unlockedStagesToCheck.begin(), unlockedStagesToCheck.end(), stage) == unlockedStagesToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedStages.begin(),
						unlockablesToProcess.unlockedStages.end(), stage) ==
						unlockablesToProcess.unlockedStages.end()) {
						unlockablesToProcess.unlockedStages.push_back(stage);
						canProcessUnlockedContent = true;
					}
				}
			}
			// dioramas
			std::vector<std::string> unlockedDioramasToCheck = previouslyUnlockedContent.unlockedCharacterDioramas;
			for each (auto diorama in currentlyUnlockedContent.unlockedCharacterDioramas) {
				if (std::find(unlockedDioramasToCheck.begin(), unlockedDioramasToCheck.end(), diorama) == unlockedDioramasToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedCharacterDioramas.begin(),
						unlockablesToProcess.unlockedCharacterDioramas.end(), diorama) ==
						unlockablesToProcess.unlockedCharacterDioramas.end()) {
						unlockablesToProcess.unlockedCharacterDioramas.push_back(diorama);
						canProcessUnlockedContent = true;
					}
				}
			}
			// game modes
			std::vector<std::string> unlockedGameModesToCheck = previouslyUnlockedContent.unlockedGameModes;
			for each (auto mode in currentlyUnlockedContent.unlockedGameModes) {
				if (std::find(unlockedGameModesToCheck.begin(), unlockedGameModesToCheck.end(), mode) ==
					unlockedGameModesToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedGameModes.begin(),
						unlockablesToProcess.unlockedGameModes.end(), mode) ==
						unlockablesToProcess.unlockedGameModes.end()) {
						unlockablesToProcess.unlockedGameModes.push_back(mode);
						canProcessUnlockedContent = true;
					}
				}
			}
			// story episodes
			std::vector<std::string> unlockedStoryEpisodesToCheck = previouslyUnlockedContent.unlockedStoryEpisodes;
			for each (auto episode in currentlyUnlockedContent.unlockedStoryEpisodes) {
				if (std::find(unlockedStoryEpisodesToCheck.begin(), unlockedStoryEpisodesToCheck.end(), episode) ==
					unlockedStoryEpisodesToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedStoryEpisodes.begin(),
						unlockablesToProcess.unlockedStoryEpisodes.end(), episode) ==
						unlockablesToProcess.unlockedStoryEpisodes.end()) {
						unlockablesToProcess.unlockedStoryEpisodes.push_back(episode);
						canProcessUnlockedContent = true;
					}
				}
			}
			// trials and tutorials
			std::vector<std::string> unlockedTutorialChallengesToCheck = previouslyUnlockedContent.unlockedTutorialChallenges;
			for each (auto challenge in currentlyUnlockedContent.unlockedTutorialChallenges) {
				if (std::find(unlockedTutorialChallengesToCheck.begin(), unlockedTutorialChallengesToCheck.end(), challenge) ==
					unlockedTutorialChallengesToCheck.end()) {
					if (std::find(unlockablesToProcess.unlockedTutorialChallenges.begin(),
						unlockablesToProcess.unlockedTutorialChallenges.end(), challenge) ==
						unlockablesToProcess.unlockedTutorialChallenges.end()) {
						unlockablesToProcess.unlockedTutorialChallenges.push_back(challenge);
						canProcessUnlockedContent = true;
					}
				}
			}
			// pictures (only if Gallery is unlocked)
			bool galleryAvailable = false;
			for (u32 i = 0; i < extraMenuActiveItems.size(); ++i) {
				if (extraMenuOptionsScenes[i] == FK_Scene::FK_SceneExtraType::ExtraGallery && extraMenuActiveItems[i]) {
					galleryAvailable = true;
				}
			}
			if (galleryAvailable) {
				std::deque<u32> unlockedPicturesToCheck = previouslyUnlockedContent.unlockedGalleryPictures;
				for each (auto picture in currentlyUnlockedContent.unlockedGalleryPictures) {
					if (std::find(unlockedPicturesToCheck.begin(), unlockedPicturesToCheck.end(), picture) ==
						unlockedPicturesToCheck.end()) {
						if (std::find(unlockablesToProcess.unlockedGalleryPictures.begin(),
							unlockablesToProcess.unlockedGalleryPictures.end(), picture) ==
							unlockablesToProcess.unlockedGalleryPictures.end()) {
							unlockablesToProcess.unlockedGalleryPictures.push_back(picture);
							canProcessUnlockedContent = true;
						}
					}
				}
			}
		}
	}

	// unlockable window
	void FK_SceneMainMenu::setupUnlockableWindow(std::vector<std::string> content) {
		s32 width = screenSize.Width * 3 / 4;
		s32 x = (screenSize.Width - width) / 2;
		s32 height = screenSize.Height / 8;
		s32 y = (screenSize.Height - height) / 2;
		core::rect<s32> frame(x, y, x + width, y + height);
		//f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		//core::rect<s32> frame(50, 140, 590, 220);
		if (unlockableContentWindow != NULL) {
			delete unlockableContentWindow;
			unlockableContentWindow = NULL;
		}
		unlockableContentWindow = new FK_DialogueWindow(device, frame, "windowskin.png", commonResourcesPath,
			1.f,
			std::string(), content, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
		unlockableWindowTimer = 0;
	}

	// unlockable display
	std::vector<std::string> FK_SceneMainMenu::processUnlockableContent() {
		std::string unlockable;
		std::string flavourText;
		std::vector<std::string> content;
		std::string charactersPath = mediaPath + "characters\\";
		std::string stagesPath = mediaPath + "stages\\";
		std::string storyEpisodePath = mediaPath + "scenes\\story\\episodes\\";
		std::string challengesPath = mediaPath + "scenes\\tutorial\\challenges\\";

		std::string successfulCheatMessage = "Cheat code successfully inputed:";

		if (!unlockablesToProcess.unlockedGameModes.empty()) {
			flavourText = gameModeUnlockedMessage;
			unlockable = unlockablesToProcess.unlockedGameModes[unlockablesToProcess.unlockedGameModes.size() - 1];
			unlockablesToProcess.unlockedGameModes.pop_back();
			std::string nameToReturn = unlockable;
			std::replace(nameToReturn.begin(), nameToReturn.end(), '_', ' ');
			u32 nameIndex = std::find(nameToReturn.begin(), nameToReturn.end(), '\\') - nameToReturn.begin();
			std::string sceneType = nameToReturn.substr(0, nameIndex);
			std::string unlockedScene = nameToReturn.substr(nameIndex + 1);
			for (int i = 0; i < menuOptionsScenes.size(); ++i) {
				if (menuOptionsScenes[i] == FK_Scene::FK_SceneType::SceneExtra && sceneType == "Extra") {
					sceneType = menuOptionsStrings[i];
				}else if (menuOptionsScenes[i] == FK_Scene::FK_SceneType::SceneTrainingSelection && sceneType == "Training") {
					sceneType = menuOptionsStrings[i];
				}
				else if (menuOptionsScenes[i] == FK_Scene::FK_SceneType::SceneCharacterSelectionArcade && sceneType == "Arcade") {
					sceneType = menuOptionsStrings[i];
				}
				else if (menuOptionsScenes[i] == FK_Scene::FK_SceneType::SceneFreeMatchSelection && sceneType == "FreeMatch") {
					sceneType = menuOptionsStrings[i];
				}
			}
			content.push_back(flavourText);
			content.push_back(sceneType + " - " + unlockedScene);
		}
		else if (!unlockablesToProcess.unlockedStoryEpisodes.empty()) {
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatUnlockAllStoryEpisodes) != 0) {
				unlockablesToProcess.unlockedStoryEpisodes.clear();
				content.push_back(successfulCheatMessage);
				content.push_back("All story episodes unlocked!");
			}
			else {
				flavourText = storyEpisodeUnlockedMessage;
				unlockable = unlockablesToProcess.unlockedStoryEpisodes[unlockablesToProcess.unlockedStoryEpisodes.size() - 1];
				unlockablesToProcess.unlockedStoryEpisodes.pop_back();
				// load character name
				std::string keyIdentifier = "#EPISODE_NAME";
				std::string filename = storyEpisodePath + unlockable + "\\episodeConfig.txt";
				std::string nameToReturn = "";
				/* open file and start parsing it */
				std::ifstream inputFile(filename.c_str());
				while (!inputFile.eof()) {
					std::string temp;
					inputFile >> temp;
					/* check if temp matches one of the keys*/
					/* name */
					if (temp == keyIdentifier) {
						std::string name = std::string();
						while (name.empty()) {
							std::getline(inputFile, name);
						}
						nameToReturn = name;
						break;
					}
				}
				content.push_back(flavourText);
				content.push_back(nameToReturn);
			}
		}
		else if (!unlockablesToProcess.unlockedCharacters.empty()) {
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatUnlockAllCharacters) != 0) {
				unlockablesToProcess.unlockedCharacters.clear();
				content.push_back(successfulCheatMessage);
				content.push_back("All characters unlocked!");
			}
			else {
				flavourText = characterUnlockedMessage;
				unlockable = unlockablesToProcess.unlockedCharacters[unlockablesToProcess.unlockedCharacters.size() - 1];
				unlockablesToProcess.unlockedCharacters.pop_back();
				// load character name
				std::string keyIdentifier = "#NAME";
				std::string keyIdentifier2 = "#UNLOCKNAME";
				std::string filename = charactersPath + unlockable + "\\character.txt";
				std::string nameToReturn = "";
				/* open file and start parsing it */
				std::ifstream inputFile(filename.c_str());
				while (!inputFile.eof()) {
					std::string temp;
					inputFile >> temp;
					/* check if temp matches one of the keys*/
					/* name */
					if (temp == keyIdentifier2) {
						inputFile >> temp;
						nameToReturn = temp;
						break;
					}
					if (temp == keyIdentifier) {
						inputFile >> temp;
						nameToReturn = temp;
						break;
					}
				}
				std::replace(nameToReturn.begin(), nameToReturn.end(), '_', ' ');
				content.push_back(flavourText);
				content.push_back(nameToReturn);
			}
		}
		else if (!unlockablesToProcess.unlockedOutfits.empty()) {
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatUnlockAllCostumes) != 0) {
				unlockablesToProcess.unlockedOutfits.clear();
				content.push_back(successfulCheatMessage);
				content.push_back("All costumes unlocked!");
			}
			else {
				flavourText = characterOutfitUnlockedMessage;
				unlockable = unlockablesToProcess.unlockedOutfits[unlockablesToProcess.unlockedOutfits.size() - 1];
				u32 nameIndex = std::find(unlockable.begin(), unlockable.end(), '\\') - unlockable.begin();
				std::string charaName = unlockable.substr(0, nameIndex);
				std::string outfitName = unlockable.substr(nameIndex + 1);
				std::replace(charaName.begin(), charaName.end(), '_', ' ');
				std::replace(outfitName.begin(), outfitName.end(), '_', ' ');
				//std::string temp = flavourText;
				bool success = fk_addons::replaceSubstring(flavourText, "%chara_name", charaName);
				unlockable = outfitName;
				unlockablesToProcess.unlockedOutfits.pop_back();
				content.push_back(flavourText);
				content.push_back(unlockable);
			}
		}
		else if (!unlockablesToProcess.unlockedStages.empty()) {
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatUnlockAllStages) != 0) {
				unlockablesToProcess.unlockedStages.clear();
				content.push_back(successfulCheatMessage);
				content.push_back("All stages unlocked!");
			}
			else {
				flavourText = stageUnlockedMessage;
				unlockable = unlockablesToProcess.unlockedStages[unlockablesToProcess.unlockedStages.size() - 1];
				unlockablesToProcess.unlockedStages.pop_back();
				// load character name
				std::string keyIdentifier = "#NAME";
				std::string filename = stagesPath + unlockable + "\\config.txt";
				std::string nameToReturn = "";
				/* open file and start parsing it */
				std::ifstream inputFile(filename.c_str());
				while (!inputFile.eof()) {
					std::string temp;
					inputFile >> temp;
					/* check if temp matches one of the keys*/
					/* name */
					if (temp == keyIdentifier) {
						inputFile >> temp;
						nameToReturn = temp;
						break;
					}
				}
				std::replace(nameToReturn.begin(), nameToReturn.end(), '_', ' ');
				content.push_back(flavourText);
				content.push_back(nameToReturn);
			}
		}
		else if (!unlockablesToProcess.unlockedCharacterDioramas.empty()) {
			flavourText = characterDioramaUnlockedMessage;
			unlockable = unlockablesToProcess.unlockedCharacterDioramas[unlockablesToProcess.unlockedCharacterDioramas.size() - 1];
			unlockablesToProcess.unlockedCharacterDioramas.pop_back();
			// load character name
			std::string keyIdentifier = "#NAME";
			std::string filename = charactersPath + unlockable + "\\character.txt";
			std::string nameToReturn = "";
			/* open file and start parsing it */
			std::ifstream inputFile(filename.c_str());
			while (!inputFile.eof()) {
				std::string temp;
				inputFile >> temp;
				/* check if temp matches one of the keys*/
				/* name */
				if (strcmp(temp.c_str(), keyIdentifier.c_str()) == 0) {
					inputFile >> temp;
					nameToReturn = temp;
				}
			}
			std::replace(nameToReturn.begin(), nameToReturn.end(), '_', ' ');
			content.push_back(flavourText);
			content.push_back(nameToReturn);
		}
		else if (!unlockablesToProcess.unlockedTutorialChallenges.empty()) {
			flavourText = trialUnlockedMessage;
			unlockable = unlockablesToProcess.unlockedTutorialChallenges[unlockablesToProcess.unlockedTutorialChallenges.size() - 1];
			unlockablesToProcess.unlockedTutorialChallenges.pop_back();
			// load character name
			std::string keyIdentifier = "#NAME";
			std::string filename = challengesPath + unlockable + "\\config.txt";
			std::string nameToReturn = "";
			/* open file and start parsing it */
			std::ifstream inputFile(filename.c_str());
			while (!inputFile.eof()) {
				std::string temp;
				inputFile >> temp;
				/* check if temp matches one of the keys*/
				/* name */
				if (temp == keyIdentifier) {
					std::string name = std::string();
					while (name.empty()) {
						std::getline(inputFile, name);
					}
					nameToReturn = name;
				}
			}
			content.push_back(flavourText);
			content.push_back(nameToReturn);
		}
		else if (!unlockablesToProcess.unlockedGalleryPictures.empty()) {
			flavourText = pictureUnlockedMessage;
			u32 size = unlockablesToProcess.unlockedGalleryPictures.size();
			u32 firstPictureId = unlockablesToProcess.unlockedGalleryPictures[0];
			u32 nextPictureId = firstPictureId;
			u32 numberOfElementsToPop = 1;
			if (size > 1) {
				for (u32 k = 1; k < size; ++k) {
					if (unlockablesToProcess.unlockedGalleryPictures[k] == nextPictureId + 1) {
						nextPictureId += 1;
						numberOfElementsToPop += 1;
					}
					else {
						break;
					}
				}
			}
			unlockable = std::string();
			if (nextPictureId != firstPictureId) {
				unlockable = "#" + std::to_string(firstPictureId) + " - #" + std::to_string(nextPictureId);
			}
			else {
				unlockable = "#" + std::to_string(firstPictureId);
			}
			// pop the elements used for this 
			for (u32 k = 0; k < numberOfElementsToPop; ++k) {
				unlockablesToProcess.unlockedGalleryPictures.pop_front();
			}
			content.push_back(flavourText);
			content.push_back(unlockable);
		}
		else {
			canProcessUnlockedContent = false;
		}
		return content;
	}

	void FK_SceneMainMenu::updateUnlockableDisplay(u32 pressedButton) {
		if (unlockableContentWindow != NULL) {
			unlockableContentWindow->update(delta_t_ms);
			if (unlockableContentWindow->isClosed() && !canProcessUnlockedContent) {
				delete unlockableContentWindow;
				unlockableContentWindow = NULL;
			}
		}
		if (unlockableContentWindow != NULL && unlockableContentWindow->isAnimating()) {
			return;
		}
		if (canProcessUnlockedContent && 
			(pressedButton & FK_Input_Buttons::Any_MenuButton) == FK_Input_Buttons::Confirmation_Button) {
			if (unlockableContentWindow != NULL) {
				deactivateUnlockableWindow();
				unlockablesToProcess.clear();
				canProcessUnlockedContent = false;
				inputReceiver->reset();
				return;
			}
		}
		else if ((pressedButton & FK_Input_Buttons::Any_MenuButton) != 0){
			deactivateUnlockableWindow();
			inputReceiver->reset();
			return;
		}
		if (canProcessUnlockedContent && (unlockableContentWindow == NULL ||
			(unlockableContentWindow != NULL && !unlockableContentWindow->isOpen()))) {
			std::vector<std::string> text = processUnlockableContent();
			if (!text.empty()) {
				soundManager->playSound("unlock", 100.f * gameOptions->getSFXVolume());
				setupUnlockableWindow(text);
				activateUnlockableWindow();
				text.clear();
			}
			return;
		}
	}

	void FK_SceneMainMenu::activateUnlockableWindow() {
		unlockableContentWindow->open(200);
		unlockableWindowTimer = 0;
		unlockableContentWindow->setVisibility(true);
		unlockableContentWindow->setActive(true);
	}

	void FK_SceneMainMenu::deactivateUnlockableWindow() {
		unlockableContentWindow->close(400);
		unlockableWindowTimer = 0;
	}


	// tutorial menu
	void FK_SceneMainMenu::setupTutorialMenu(){
		std::vector<std::string> menuItems = {
			"  Yes  ",
			"  No  ",
		};
		tutorialMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		f32 scaleFactorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		s32 width = screenSize.Width * 3 / 4;
		s32 x = (screenSize.Width - width) / 2;
		s32 height = screenSize.Height / 16 * (s32)tutorialPromptMessage.size();
		s32 y = tutorialMenu->getFrame().UpperLeftCorner.Y - height;
		core::rect<s32> frame(x, y, x + width, y + height);
		//f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		//core::rect<s32> frame(50, 140, 590, 220);
		tutorialTextWindow = new FK_DialogueWindow(device, frame, "windowskin.png", commonResourcesPath, 
			1.f,
			std::string(), tutorialPromptMessage, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
		deactivateTutorialMenu();
		beforeTutorialTimer = 0;
	}

	void FK_SceneMainMenu::activateTutorialMenu(){
		/*tutorialMenu->setActive(true);
		tutorialMenu->setVisible(true);*/
		tutorialTextWindow->setVisibility(true);
		tutorialTextWindow->setActive(true);
		tutorialTextWindow->open(200);
		//Sleep(200);
	}

	void FK_SceneMainMenu::deactivateTutorialMenu(){
		tutorialMenu->setActive(false);
		tutorialMenu->setVisible(false);
		//tutorialMenu->close(500);
		tutorialMenu->reset();
		//tutorialTextWindow->setVisibility(false);
		//tutorialTextWindow->setActive(false);
		tutorialTextWindow->close(400);
	}

	// tutorial menu
	void FK_SceneMainMenu::setupUnlockablesMenu() {
		std::vector<std::string> menuItems = {
			"  Yes  ",
			"  No  ",
		};
		unlockablesMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		f32 scaleFactorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		s32 width = screenSize.Width * 3 / 4;
		s32 x = (screenSize.Width - width) / 2;
		s32 height = screenSize.Height / 16 * (s32)unlockablesPromptMessage.size();
		s32 y = tutorialMenu->getFrame().UpperLeftCorner.Y - height;
		core::rect<s32> frame(x, y, x + width, y + height);
		unlockablesTextWindow = new FK_DialogueWindow(device, frame, "windowskin.png", commonResourcesPath,
			1.f,
			std::string(), unlockablesPromptMessage, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
		deactivateTutorialMenu();
		beforeTutorialTimer = 0;
	}

	void FK_SceneMainMenu::activateUnlockablesMenu() {
		unlockablesTextWindow->setVisibility(true);
		unlockablesTextWindow->setActive(true);
		unlockablesTextWindow->open(200);
	}

	void FK_SceneMainMenu::deactivateUnlockablesMenu() {
		unlockablesMenu->setActive(false);
		unlockablesMenu->setVisible(false);
		unlockablesMenu->reset();
		unlockablesTextWindow->close(400);
	}

	// joypad assignment update
	void FK_SceneMainMenu::playAssignedButtonSound() {
		soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
	}

	void FK_SceneMainMenu::updateTutorialMenu(u32 pressedButton){
		tutorialMenu->update(pressedButton);
		if (tutorialMenu->itemIsSelected()){
			tutorialPresented = true;
			writeSaveFile();
			if (tutorialMenu->getIndex() == 0){
				bool tutorialSceneFound = false;
				FK_Scene::FK_SceneTrainingType tutorialScene = FK_Scene::FK_SceneTrainingType::TrainingTutorial;
				if (std::find(trainingMenuOptionsScenes.begin(), trainingMenuOptionsScenes.end(), tutorialScene) !=
					trainingMenuOptionsScenes.end()) {
					subMenuIndex = std::find(trainingMenuOptionsScenes.begin(), trainingMenuOptionsScenes.end(), tutorialScene) -
						trainingMenuOptionsScenes.begin();
					tutorialSceneFound = true;
				}
				if (tutorialSceneFound) {
					menuIndex = 0;
					FK_SceneType sceneToCheck = FK_SceneType::SceneTrainingSelection;
					if (std::find(menuOptionsScenes.begin(), menuOptionsScenes.end(), sceneToCheck) != menuOptionsScenes.end()) {
						menuIndex = std::find(menuOptionsScenes.begin(), menuOptionsScenes.end(), sceneToCheck) - menuOptionsScenes.begin();
					}
				}
				else {
					subMenuIndex = 0;
					FK_Scene::FK_SceneFreeMatchType freeMatchTutorial = FK_Scene::FK_SceneFreeMatchType::FreeMatchTutorial;
					if (std::find(freeMatchMenuOptionsScenes.begin(), 
						freeMatchMenuOptionsScenes.end(), freeMatchTutorial) !=
						freeMatchMenuOptionsScenes.end()) {
						subMenuIndex = std::find(freeMatchMenuOptionsScenes.begin(), 
							freeMatchMenuOptionsScenes.end(), freeMatchTutorial) -
							freeMatchMenuOptionsScenes.begin();
						tutorialSceneFound = true;
					}
					if (tutorialSceneFound) {
						menuIndex = 0;
						FK_SceneType sceneToCheck = FK_SceneType::SceneFreeMatchSelection;
						if (std::find(menuOptionsScenes.begin(), menuOptionsScenes.end(), sceneToCheck) != menuOptionsScenes.end()) {
							menuIndex = std::find(menuOptionsScenes.begin(), menuOptionsScenes.end(), sceneToCheck) - menuOptionsScenes.begin();
						}
					}
				}
				showUnlockablesPrompt = false;
				setNextScene(FK_SceneType::SceneGameTutorial);
				itemSelected = true;
				Sleep(200);
				return;
			}
			else{
				deactivateTutorialMenu();
				return;
			}
		}
	}

	void FK_SceneMainMenu::updateUnlockablesMenu(u32 pressedButton) {
		unlockablesMenu->update(pressedButton);
		if (unlockablesMenu->itemIsSelected()) {
			unlockablesChoicePresented = true;
			if (unlockablesMenu->getIndex() == 0) {
				unlockAllCharactersFlag = true;
				unlockAllStagesFlag = true;
				writeSaveFile();
				readRewardFile();
				setPreviouslyUnlockedContent(currentlyUnlockedContent);
				deactivateUnlockablesMenu();
				return;
			}
			else {
				writeSaveFile();
				deactivateUnlockablesMenu();
				return;
			}
		}
	}

	void FK_SceneMainMenu::updateInput(){
		u32 lastInputTimePlayer1 = lastInputTime;
		u32 lastInputTimePlayer2 = lastInputTime;
		// update input
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0), false);
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus(), false);
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1), false);
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus(), false);
		}

		u32 inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		updateMenuInputTimeThresholdMS(inputButtons, delta_t_ms);
		u32 inputThreshold = getMenuInputTimeThresholdMS();

		if (canProcessUnlockedContent || (unlockableContentWindow != NULL)) {
			updateUnlockableDisplay(inputButtons);
		}
		else if (tutorialMenu->isActive()){
			if ((inputButtons & FK_Input_Buttons::Any_MenuButton) == FK_Input_Buttons::Cancel_Button){
				deactivateTutorialMenu();
				tutorialPresented = true;
				writeSaveFile();
				return;
			}
			else{
				updateTutorialMenu(inputButtons);
				return;
			}
		}
		else if (unlockablesMenu->isActive()) {
			if ((inputButtons & FK_Input_Buttons::Any_MenuButton) == FK_Input_Buttons::Cancel_Button) {
				deactivateUnlockablesMenu();
				unlockablesChoicePresented = true;
				writeSaveFile();
				return;
			}
			else {
				updateUnlockablesMenu(inputButtons);
				return;
			}
		}
		else{
			// check input for player 1
			if (now - lastInputTimePlayer1 > inputThreshold){
				u32 directionPlayer1 = player1input->getLastDirection();
				u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
				if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button){
					if (isSelectingSubMenu) {
						cancelSubMenuSelection();
					}
					else if (backToIntroWhenCancelIsPressed){
						setNextScene(FK_Scene::SceneTitle);
						itemSelected = true;
						soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
						Sleep(300);
						return;
					}
				}
				else{
					if (directionPlayer1 == FK_Input_Buttons::Down_Direction){
						increaseMenuIndex();
					}
					else if (directionPlayer1 == FK_Input_Buttons::Up_Direction){
						decreaseMenuIndex();
					}
					else{
						// select item
						if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button){
							selectItem();
							lastSelectionFromPlayer1 = true;
							return;
						}
					}
				}
			}
			// check input for player 2
			if (now - lastInputTimePlayer2 > inputThreshold){
				u32 directionPlayer2 = player2input->getLastDirection();
				u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
				if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button){
					if (isSelectingSubMenu){
						cancelSubMenuSelection();
						return;
					}
					else if (backToIntroWhenCancelIsPressed) {
						setNextScene(FK_Scene::SceneTitle);
						itemSelected = true;
						soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
						Sleep(300);
						return;
					}
				}
				else{
					if (directionPlayer2 == FK_Input_Buttons::Down_Direction){
						increaseMenuIndex();
					}
					else if (directionPlayer2 == FK_Input_Buttons::Up_Direction){
						decreaseMenuIndex();
					}
					else{
						// select item
						if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button){
							selectItem();
							lastSelectionFromPlayer1 = false;
							return;
						}
					}
				}
			}
		}
	}
}
