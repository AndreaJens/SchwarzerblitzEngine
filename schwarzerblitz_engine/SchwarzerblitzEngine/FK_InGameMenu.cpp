#include"FK_InGameMenu.h"
#include"FK_Database.h"
#include"FK_InputReceiver.h"

using namespace irr;

namespace fk_engine{
	FK_InGameMenu::FK_InGameMenu(){
		reset();
	};
	FK_InGameMenu::FK_InGameMenu(IrrlichtDevice *new_device, FK_SoundManager* scene_soundManager, 
		FK_Options* scene_options, std::vector<std::string> items, std::string skinName, 
		std::string skinPath, f32 x, f32 y) : FK_InGameMenu(){
		setup(new_device, scene_soundManager, scene_options, items, skinName, skinPath, x, y);
	};
	void FK_InGameMenu::setup(IrrlichtDevice *new_device, FK_SoundManager* scene_soundManager,
		FK_Options* scene_options, std::vector<std::string> items, std::string skinName, 
		std::string skinPath, f32 x, f32 y){
		device = new_device;
		gameOptions = scene_options;
		menuItems = items;
		soundManager = scene_soundManager;
		menuWindow = NULL;
		setViewport(core::position2di(-1, -1), device->getVideoDriver()->getScreenSize());
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		int size = menuItems.size();
		s32 maxWidth = 0;
		s32 maxHeight = 0;
		f32 offset = 0.1;
		for (int i = 0; i < size; ++i){
			std::wstring itemToDraw(menuItems[i].begin(), menuItems[i].end());
			s32 tempH = font->getDimension(itemToDraw.c_str()).Height;
			s32 tempW = font->getDimension(itemToDraw.c_str()).Width;
			if (tempW > maxWidth){
				maxWidth = tempW;
			}
			if (tempH > maxHeight){
				maxHeight = tempH;
			}
		}
		maxHeight = (s32)(floor((f32)(maxHeight)*(1 + offset) * size));
		maxWidth = (s32)(floor((f32)(maxWidth)*(1 + offset)));
		if (x == -1){
			x = (screenSize.Width - maxWidth) / 2;
		}
		if (y == -1){
			y = (screenSize.Height - maxHeight) / 2;
		}
		menuWindow = new FK_Window(device, x, y, maxWidth, maxHeight, skinName, skinPath);
		menuWindow->setVisibility(false);
		menuWindow->setActive(false);
		menuWindow->forceOpen();
		now = device->getTimer()->getRealTime();
		then = now;
	};

	/* set position */
	void FK_InGameMenu::setPosition(s32 x, s32 y) {
		s32 maxHeight = getFrame().getHeight();
		s32 maxWidth = getFrame().getWidth();
		if (x == -1) {
			x = (screenSize.Width - maxWidth) / 2;
		}
		if (y == -1) {
			y = (screenSize.Height - maxHeight) / 2;
		}
		menuWindow->setPosition(core::vector2d<s32>(x, y));
	}

	void FK_InGameMenu::setViewport(core::position2di position, core::dimension2d<u32> newViewportSize){
		screenSize = newViewportSize;
		scaleFactorX = screenSize.Width;
		scaleFactorY = screenSize.Height;
		scaleFactorX /= (f32)fk_constants::FK_DefaultResolution.Width;
		scaleFactorY /= (f32)fk_constants::FK_DefaultResolution.Height;

		if (menuWindow != NULL){
			if (position.X * position.Y == 1){
				int size = menuItems.size();
				s32 maxWidth = 0;
				s32 maxHeight = 0;
				f32 offset = 0.1;
				for (int i = 0; i < size; ++i){
					std::wstring itemToDraw(menuItems[i].begin(), menuItems[i].end());
					s32 tempH = font->getDimension(itemToDraw.c_str()).Height;
					s32 tempW = font->getDimension(itemToDraw.c_str()).Width;
					if (tempW > maxWidth){
						maxWidth = tempW;
					}
					if (tempH > maxHeight){
						maxHeight = tempH;
					}
				}
				maxHeight = (s32)(floor((f32)(maxHeight)*(1 + offset) * size));
				maxWidth = (s32)(floor((f32)(maxWidth)*(1 + offset)));
				position.X = (screenSize.Width - maxWidth) / 2;
				position.Y = (screenSize.Height - maxHeight) / 2;
			}
			menuWindow->setPosition(position);
		}
	}

	void FK_InGameMenu::reset(){
		selectionFlag = false;
		menuIndex = 0;
	};
	void FK_InGameMenu::selectItem(bool playSoundOnItemSelect){
		((FK_InputReceiver*)device->getEventReceiver())->reset();
		inputDelayMS = 0;
		if (playSoundOnItemSelect) {
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		}
		selectionFlag = true;
	};
	void FK_InGameMenu::cancelSelection(bool playSoundOnItemSelect){
		((FK_InputReceiver*)device->getEventReceiver())->reset();
		inputDelayMS = 0;
		if (playSoundOnItemSelect) {
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		}
		selectionFlag = false;
	};
	void FK_InGameMenu::resetSelection(){
		inputDelayMS = 0;
		selectionFlag = false;
	};
	s32 FK_InGameMenu::getIndex(){
		return menuIndex;
	};
	bool FK_InGameMenu::isActive(){
		return menuWindow->isActive();
	};
	bool FK_InGameMenu::isVisible(){
		return menuWindow->isVisible();
	};
	bool FK_InGameMenu::itemIsSelected(){
		return selectionFlag;
	};
	void FK_InGameMenu::setActive(bool new_active_flag){
		menuWindow->setActive(new_active_flag);
	};
	void FK_InGameMenu::setVisible(bool new_visibility_flag){
		menuWindow->setVisibility(new_visibility_flag);
	};
	void FK_InGameMenu::increaseCursor(){
		inputDelayMS = 0;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		menuIndex += 1;
		menuIndex %= menuItems.size();
	};
	void FK_InGameMenu::decreaseCursor(){
		inputDelayMS = 0;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		menuIndex += menuItems.size() - 1;
		menuIndex %= menuItems.size();
	};
	void FK_InGameMenu::update(u32 pressedButton, bool playSoundOnItemSelect){
		now = device->getTimer()->getRealTime();
		u32 delta_t_ms = now - then;
		//u32 timeDelayForInputMs = 200;
		inputDelayMS += delta_t_ms;
		then = now;
		menuWindow->update(delta_t_ms);
		if (menuWindow->isActive() && (pressedButton & FK_Input_Buttons::Selection_Button)) {
			selectItem(playSoundOnItemSelect);
			return;
		}
		else {
			if (canAcceptInput()) {
				if (pressedButton & FK_Input_Buttons::Down_Direction) {
					increaseCursor();
					return;
				}
				if (pressedButton & FK_Input_Buttons::Up_Direction) {
					decreaseCursor();
					return;
				}
				if (pressedButton & FK_Input_Buttons::Cancel_Button) {
					cancelSelection(playSoundOnItemSelect);
					return;
				}
			}
		}
	};

	void FK_InGameMenu::resetLastInputTime() {
		inputDelayMS = 0;
	}

	bool FK_InGameMenu::canAcceptInput() {
		u32 timeDelayForInputMs = 200;
		return inputDelayMS > timeDelayForInputMs && menuWindow->isActive();
	}

	core::rect<s32> FK_InGameMenu::getFrame(){
		core::vector2d<s32> rightCorner = menuWindow->getTopLeftCornerPosition() +
			core::vector2d<s32>(menuWindow->getWidth(), menuWindow->getHeight());
		return core::rect<s32>(menuWindow->getTopLeftCornerPosition(), rightCorner);
	}

	void FK_InGameMenu::draw(){
		if (menuWindow->isVisible()){
			menuWindow->draw(1.0f, 1.0f);
			int size = menuItems.size();
			s32 maxWidth = 0;
			s32 maxHeight = 0;
			f32 offset = 0.1;
			s32 selectorY = 0;
			s32 selectorHeight = 0;
			s32 selectorWidth = (s32)floor(menuWindow->getWidth() / (1.0f + offset / 2));
			s32 selectorX = menuWindow->getTopLeftCornerPosition().X + (menuWindow->getWidth() - selectorWidth) / 2;
			for (int i = 0; i < size; ++i){
				std::wstring itemToDraw(menuItems[i].begin(), menuItems[i].end());
				s32 tempH = font->getDimension(itemToDraw.c_str()).Height;
				s32 tempW = font->getDimension(itemToDraw.c_str()).Width;
				s32 tx = menuWindow->getCenterPosition().X - tempW / 2;
				s32 ty = floor(menuWindow->getTopLeftCornerPosition().Y + ((f32)tempH * i * (1.f + offset)));
				core::rect<s32> destRect(tx, ty, tx + tempW, ty + tempH);
				fk_addons::drawBorderedText(font, itemToDraw, destRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
				if (menuIndex == i){
					selectorY = ty - (s32)floor(((f32)tempH * offset)) / 2;
					selectorHeight = (s32)(floor((f32)tempH*(1.0f + offset)));
				}
			}
			// draw selector
			u32 oscillationPeriod = 1000.0;
			f32 phase = (f32)(now % oscillationPeriod) / oscillationPeriod * 2.0f * 3.1415926f;
			s32 opacity = 180 + 75 * std::cos(phase);
			menuWindow->drawSelector(selectorX, selectorY, selectorWidth, selectorHeight, opacity);
		}
	}
}