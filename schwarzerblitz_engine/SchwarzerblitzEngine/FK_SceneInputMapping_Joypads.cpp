#include"FK_SceneInputMapping_Joypads.h"
#include"ExternalAddons.h"
#include<Windows.h>
#include<iostream>

namespace fk_engine{
	FK_SceneInputMapping_Joypads::FK_SceneInputMapping_Joypads() : FK_SceneInputMapping(){

	};
	FK_SceneInputMapping_Joypads::FK_SceneInputMapping_Joypads(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options *newOptions) : FK_SceneInputMapping_Joypads(){
		setup(newDevice, joypadInfo, newOptions);
	};
	void FK_SceneInputMapping_Joypads::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options* newOptions){
		FK_SceneInputMapping::setup(newDevice, joypadInfo, newOptions);
		initializeJoypadButtonsMap();
		modifiedInputMap = {
			std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(1)),
			std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(2)),
		};
		// translate modified input maps
		u32 buttonMask = (u32)(FK_Input_Buttons::Any_NonDirectionButton | FK_Input_Buttons::Any_MenuButton | FK_Input_Buttons::Any_SystemButton);
		std::string joypadName = std::string(joystickInfo[0].Name.c_str());
		for (auto iter = modifiedInputMap[0].begin(); iter != modifiedInputMap[0].end(); ++iter){
			if ((iter->first & buttonMask) != 0){
				modifiedInputMap[0][iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
					joypadName, (FK_JoypadInputNative)iter->second);
			}
		}
		if (joypadInfo.size() > 1){
			joypadName = std::string(joystickInfo[1].Name.c_str());
		}
		for (auto iter = modifiedInputMap[1].begin(); iter != modifiedInputMap[1].end(); ++iter){
			if ((iter->first & buttonMask) != 0){
				modifiedInputMap[1][iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
					joypadName, (FK_JoypadInputNative)iter->second);
			}
		}
		menuIndex = directionButtons.size();
	};

	void FK_SceneInputMapping_Joypads::saveSettings(){
		inputMapper->setJoypadInputMap(1, modifiedInputMap[0]);
		inputMapper->setJoypadInputMap(2, modifiedInputMap[1]);
		inputMapper->saveMapToFile(configurationFilesPath + fk_constants::FK_InputMapFileName);
	};

	void FK_SceneInputMapping_Joypads::initialize(){
		FK_SceneInputMapping::initialize();
		playerIndex = 0;
	}

	// draw item
	void FK_SceneInputMapping_Joypads::drawItem(u32 itemIndex){
		// draw KEY DESCRIPTION
		std::string unassignedKeyString = "N/A";
		std::string keyIdentifier = menuOptionsStrings[itemIndex];
		std::string assignedKey1;
		std::string assignedKey2;
		s32 fixedOffsetY = 64;
		s32 baseOffsetY = fixedOffsetY;
		s32 fixedSpacingY = 18;
		u32 tempIndex = itemIndex;

		// add offset
		if (menuIndex > 5){
			fixedOffsetY -= (fixedSpacingY * (menuIndex - 5));
			s32 thresholdValue = (menuOptionsStrings.size()) * fixedSpacingY + baseOffsetY + 40;
			thresholdValue = screenSize.Height / scale_factorY - thresholdValue;
			if (fixedOffsetY < thresholdValue){
				fixedOffsetY = thresholdValue;
			}
		}

		if (itemIndex >= directionButtons.size() + menuButtons.size()){
			tempIndex -= directionButtons.size() + menuButtons.size();
			if (modifiedInputMap[0][gameButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native){
				assignedKey1 = joypadKeys[(FK_JoypadInput)modifiedInputMap[0][gameButtons[tempIndex]]];
			}
			else{
				assignedKey1 = unassignedKeyString;
			}
			if (modifiedInputMap[1][gameButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native){
				assignedKey2 = joypadKeys[(FK_JoypadInput)modifiedInputMap[1][gameButtons[tempIndex]]];
			}
			else{
				assignedKey2 = unassignedKeyString;
			}
			fixedOffsetY += 40;
		}
		else if (itemIndex >= directionButtons.size()){
			tempIndex -= directionButtons.size();
			if (modifiedInputMap[0][menuButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native){
				assignedKey1 = joypadKeys[(FK_JoypadInput)modifiedInputMap[0][menuButtons[tempIndex]]];
			}
			else{
				assignedKey1 = unassignedKeyString;
			}
			if (modifiedInputMap[1][menuButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native){
				assignedKey2 = joypadKeys[(FK_JoypadInput)modifiedInputMap[1][menuButtons[tempIndex]]];
			}
			else{
				assignedKey2 = unassignedKeyString;
			}
			fixedOffsetY += 20;
		}
		else{
			if (modifiedInputMap[0][directionButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native){
				assignedKey1 = joypadKeys[(FK_JoypadInput)modifiedInputMap[0][directionButtons[tempIndex]]];
			}
			else{
				assignedKey1 = unassignedKeyString;
			}
			if (modifiedInputMap[1][directionButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native){
				assignedKey2 = joypadKeys[(FK_JoypadInput)modifiedInputMap[1][directionButtons[tempIndex]]];
			}
			else{
				assignedKey2 = unassignedKeyString;
			}
		}

		// don't draw if outside of screen
		s32 tempY = fixedOffsetY + itemIndex * fixedSpacingY;
		if (tempY < baseOffsetY){
			return;
		}
		if (tempY > screenSize.Height / scale_factorY - (baseOffsetY + fixedSpacingY)){
			return;
		}

		s32 fixedOffsetX = 32;
		s32 selectedItemAdditionalOffsetX = 15;

		irr::video::SColor itemColor = irr::video::SColor(255, 255, 255, 255);
		std::wstring itemString(keyIdentifier.begin(), keyIdentifier.end());
		s32 sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		s32 sentenceX = fixedOffsetX;
		if (itemIndex == menuIndex){
			sentenceX += selectedItemAdditionalOffsetX;
			itemColor = irr::video::SColor(255, 200, 200, 20);
		}
		// direction can't be modified for joypad
		if (itemIndex < directionButtons.size()){
			itemColor = irr::video::SColor(255, 96, 96, 96);
		}
		s32 sentenceY = fixedOffsetY + itemIndex * fixedSpacingY; // distance from the top of the screen
		core::rect<s32> destinationRect = core::rect<s32>(sentenceX * scale_factorX,
			sentenceY * scale_factorY,
			sentenceX * scale_factorX + sentenceWidth,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			itemColor, irr::video::SColor(128, 0, 0, 0));

		// draw KEY (Player 1)

		s32 selectedItemXL = 0;
		s32 selectedItemXR = 0;
		s32 selectedItemY = 0;
		s32 selectedItemDimension = 0;
		fixedOffsetX = 192;
		/* get width and height of the string*/
		itemColor = irr::video::SColor(255, 20, 225, 120);
		itemString = std::wstring(assignedKey1.begin(), assignedKey1.end());
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		sentenceX = fixedOffsetX; // distance from screen center
		sentenceY = fixedOffsetY + itemIndex * fixedSpacingY; // distance from the top of the screen
		if (itemIndex == menuIndex && playerIndex == 0){
			//sentenceX += selectedItemAdditionalOffsetX;
			itemColor = getSelectedKeyColor();// irr::video::SColor(255, 20, 225, 220);
			selectedItemXL = screenSize.Width - (sentenceX * scale_factorX + sentenceWidth);
			selectedItemXR = screenSize.Width - sentenceX * scale_factorX;
			selectedItemY = (sentenceY + 5) * scale_factorY;
			selectedItemDimension = sentenceHeight * 3 / 4;
		}
		// direction can't be modified for joypad
		if (itemIndex < directionButtons.size()){
			itemColor = irr::video::SColor(255, 96, 96, 96);
		}
		destinationRect = core::rect<s32>(
			screenSize.Width - (sentenceX * scale_factorX + sentenceWidth),
			sentenceY * scale_factorY,
			screenSize.Width - sentenceX * scale_factorX,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			itemColor, irr::video::SColor(128, 0, 0, 0));

		//draw KEY(Player2)

		fixedOffsetX = 32;
		/* get width and height of the string*/
		itemColor = irr::video::SColor(255, 20, 225, 120);
		itemString = std::wstring(assignedKey2.begin(), assignedKey2.end());
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		sentenceX = fixedOffsetX; // distance from screen center
		sentenceY = fixedOffsetY + itemIndex * fixedSpacingY; // distance from the top of the screen
		if (itemIndex == menuIndex && playerIndex == 1){
			//sentenceX += selectedItemAdditionalOffsetX;
			itemColor = getSelectedKeyColor();// irr::video::SColor(255, 20, 225, 220);
			selectedItemXL = screenSize.Width - (sentenceX * scale_factorX + sentenceWidth);
			selectedItemXR = screenSize.Width - sentenceX * scale_factorX;
			selectedItemY = (sentenceY + 5) * scale_factorY;
			selectedItemDimension = sentenceHeight * 3 / 4;
		}
		// direction can't be modified for joypad
		if (itemIndex < directionButtons.size()){
			itemColor = irr::video::SColor(255, 96, 96, 96);
		}
		destinationRect = core::rect<s32>(
			screenSize.Width - (sentenceX * scale_factorX + sentenceWidth),
			sentenceY * scale_factorY,
			screenSize.Width - sentenceX * scale_factorX,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			itemColor, irr::video::SColor(128, 0, 0, 0));
	}

	void FK_SceneInputMapping_Joypads::drawTitle(){
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Joypad Settings";
		s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
		x -= sentenceWidth / 2;
		y -= sentenceHeight / 2;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + scale_factorY);
		fk_addons::drawBorderedText(font, itemString, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	// draw tags
	void FK_SceneInputMapping_Joypads::drawPlayerTags(){
		// draw tag (Player1)
		f32 phase = (f32)(now % 500) / 500 * 2.0f * 3.1415926f;
		/* get width and height of the string*/
		video::SColor itemColor(255, 200, 200, 200);
		if (playerIndex == 0){
			u32 blinkAddon = std::round(60 * std::cos(phase));
			itemColor = irr::video::SColor(255, 220, 160 + blinkAddon, 60 + blinkAddon);
		}
		std::wstring itemString = L"Player 1";
		u32 sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		u32 sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		s32 sentenceX = 192; // distance from screen center
		s32 sentenceY = 36;
		core::rect<s32> destinationRect(
			screenSize.Width - (sentenceX * scale_factorX + sentenceWidth),
			sentenceY * scale_factorY,
			screenSize.Width - sentenceX * scale_factorX,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			itemColor, irr::video::SColor(128, 0, 0, 0));

		//draw tag (Player2)

		/* get width and height of the string*/
		itemString = L"Player 2";
		itemColor = irr::video::SColor(255, 200, 200, 200);
		if (playerIndex == 1) {
			u32 blinkAddon = std::round(60 * std::cos(phase));
			itemColor = irr::video::SColor(255, 220, 160 + blinkAddon, 60 + blinkAddon);
		}
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		sentenceX = 32; // distance from screen center
		destinationRect = core::rect<s32>(
			screenSize.Width - (sentenceX * scale_factorX + sentenceWidth),
			sentenceY * scale_factorY,
			screenSize.Width - sentenceX * scale_factorX,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			itemColor, irr::video::SColor(128, 0, 0, 0));
	}

	// draw all
	void FK_SceneInputMapping_Joypads::drawAll(bool showBackground){
		if (showBackground) {
			drawBackground();
		}
		drawItems();
		drawCaptions();
		drawArrows();
		drawPlayerTags();
		drawTitle();
	}

	// toggle player
	void FK_SceneInputMapping_Joypads::togglePlayer(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		playerIndex = !playerIndex;
	}

	void FK_SceneInputMapping_Joypads::swapSameInputButtons(FK_Input_Buttons lastModifiedButton, u32 oldKeyCode){
		u32 inputKey = modifiedInputMap[playerIndex][lastModifiedButton];
		std::cout << joypadKeys[(FK_JoypadInput)oldKeyCode] << " " << lastModifiedButton << std::endl;
		if (isDirection(lastModifiedButton)){
			for each (auto key in modifiedInputMap[playerIndex]){
				if (key.second == inputKey && key.first != lastModifiedButton){
					modifiedInputMap[playerIndex][key.first] = oldKeyCode;
				}
			}
		}
		else if (isMenuButton(lastModifiedButton)){
			for each (auto key in menuButtons){
				if (modifiedInputMap[playerIndex][key] == inputKey  && key != lastModifiedButton){
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
			for each (auto key in directionButtons){
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton){
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
		}
		else if (isGameButton(lastModifiedButton)){
			for each (auto key in gameButtons){
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton){
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
			for each (auto key in directionButtons){
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton){
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
		}
	}

	// get last pressed button
	u32 FK_SceneInputMapping_Joypads::getLastPressedButton(){
		bool *keyArray = inputReceiver->JoypadStatus(0);
		u32 currentInput = (u32)FK_JoypadInputNative::JoypadInput_NoButton_Native;
		u32 numberOfNativeButtons = FK_JoypadInputNative::JoypadInput_NumberOfButtons;
		for (u32 i = 0; i < numberOfNativeButtons; ++i){
			if (keyArray[i]){
				currentInput = i;
				u32 currentButton = inputMapper->getTranslatedButtonFromJoypadMap(
					std::string(joystickInfo[0].Name.c_str()), (FK_JoypadInputNative)currentInput);
				//std::cout << "current input = " << currentInput << " " << currentButton << " " << joypadKeys[(FK_JoypadInput)currentButton] << std::endl;
				currentInput = currentButton;
				if (currentButton != FK_JoypadInput::JoypadInput_NoButton) {
					return currentInput;
				}
			}
		}
		if (joystickInfo.size() > 1) {
			keyArray = inputReceiver->JoypadStatus(1);
			for (u32 i = 0; i < numberOfNativeButtons; ++i) {
				if (keyArray[i]) {
					currentInput = i;
					u32 currentButton = inputMapper->getTranslatedButtonFromJoypadMap(
						std::string(joystickInfo[1].Name.c_str()), (FK_JoypadInputNative)currentInput);
					//std::cout << "current input = " << currentInput << " " << currentButton << " " << joypadKeys[(FK_JoypadInput)currentButton] << std::endl;
					currentInput = currentButton;
					if (currentButton != FK_JoypadInput::JoypadInput_NoButton) {
						return currentInput;
					}
				}
			}
		}
		return currentInput;
	}

	// update selection
	void FK_SceneInputMapping_Joypads::updateSelection(){
		u32 inputThreshold = 200;
		if ((now - lastInputTime) > inputThreshold){
			u32 newInput = FK_SceneInputMapping_Joypads::getLastPressedButton();
			if (newInput != oldInputButton && newInput < FK_JoypadInputNative::JoypadInput_NumberOfButtons){
				lastInputTime = now;
				oldInputButton = newInput;
				itemSelected = false;
				Sleep(200);
				assignNewInput(getButtonFromIndex(menuIndex), newInput);
				soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			}
		}
	}

	// assign new input
	void FK_SceneInputMapping_Joypads::assignNewInput(FK_Input_Buttons buttonToReplace, u32 newKeyCode){
		if (modifiedInputMap[playerIndex][buttonToReplace] != newKeyCode){
			u32 oldKeyCode = modifiedInputMap[playerIndex][buttonToReplace];
			modifiedInputMap[playerIndex][buttonToReplace] = newKeyCode;
			swapSameInputButtons(buttonToReplace, oldKeyCode);
		}
		else if (isOptionalButton(buttonToReplace)){
			modifiedInputMap[playerIndex][buttonToReplace] = (u32)FK_JoypadInputNative::JoypadInput_NoButton_Native;
		}
	}

	void FK_SceneInputMapping_Joypads::increaseMenuIndex(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		menuIndex += 1;
		menuIndex %= menuOptionsStrings.size();
		if (menuIndex < directionButtons.size()){
			menuIndex = directionButtons.size();
		}
	}

	void FK_SceneInputMapping_Joypads::decreaseMenuIndex(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		if (menuIndex == directionButtons.size()){
			menuIndex = menuOptionsStrings.size() - 1;
		}
		else{
			menuIndex -= 1;
		}
		//captionStillPeriodCounterMs = 0;
		//captionMovementTimeCounterMs = 0;
	}

	// update input
	void FK_SceneInputMapping_Joypads::updateInput(){
		//u32 lastInputTimePlayer1 = player1input->getLastInputTime();
		u32 inputThreshold = 200;
		// check input for player 1
		if (now - lastInputTime > inputThreshold){
			if (joystickInfo.size() > 0){
				player1input->update(now, inputReceiver->JoypadStatus(0), false);
			}
			if (joystickInfo.size() > 1) {
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			u32 directionPlayer = player1input->getLastDirection() | player2input->getLastDirection();
			u32 lastButtonPlayer = (player1input->getPressedButtons() |
									player2input->getPressedButtons()) & 
									FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer == FK_Input_Buttons::Confirmation_Button){
				lastInputTime = now;
				saveAndExit();
				return;
			}
			if (lastButtonPlayer == FK_Input_Buttons::Cancel_Button){
				lastInputTime = now;
				activateExitMenu();
				return;
			}
			else{
				if (directionPlayer == FK_Input_Buttons::Down_Direction){
					lastInputTime = now;
					increaseMenuIndex();
				}
				else if (directionPlayer == FK_Input_Buttons::Up_Direction){
					lastInputTime = now;
					decreaseMenuIndex();
				}
				else if (directionPlayer == FK_Input_Buttons::Left_Direction ||
					directionPlayer == FK_Input_Buttons::Right_Direction){
					lastInputTime = now;
					togglePlayer();
				}
				else if (lastButtonPlayer == FK_Input_Buttons::ScrollLeft_Button ||
					lastButtonPlayer == FK_Input_Buttons::ScrollRight_Button) {
					lastInputTime = now;
					togglePlayer();
					inputReceiver->reset();
				}
				else{
					if (lastButtonPlayer == FK_Input_Buttons::Selection_Button){
						if (menuIndex < directionButtons.size()){
							soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
						}
						else{
							selectItem();
						}
						lastInputTime = now;
						Sleep(200);
						return;
					}
				}
			}
		}
	}
}