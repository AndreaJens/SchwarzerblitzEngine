#include"FK_SceneInputMapping_SingleplayerKeyboard.h"
#include"ExternalAddons.h"
#include<Windows.h>
#include<iostream>

namespace fk_engine{
	FK_SceneInputMapping_SingleplayerKeyboard::FK_SceneInputMapping_SingleplayerKeyboard() : FK_SceneInputMapping(){
		initialize();
	};
	FK_SceneInputMapping_SingleplayerKeyboard::FK_SceneInputMapping_SingleplayerKeyboard(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options *newOptions) : FK_SceneInputMapping(){
		setup(newDevice, joypadInfo, newOptions);
	};
	void FK_SceneInputMapping_SingleplayerKeyboard::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options* newOptions){
		FK_SceneInputMapping::setup(newDevice, joypadInfo, newOptions);
		modifiedInputMap.push_back(std::map<FK_Input_Buttons, u32>(inputMapper->getKeyboardMap(0)));
	};

	void FK_SceneInputMapping_SingleplayerKeyboard::saveSettings(){
		inputMapper->setKeyboardInputMap(0, modifiedInputMap[0]);
		// save correct joypad settings, if needed
		if (!joystickInfo.empty()) {
			std::map<FK_Input_Buttons, u32> joypad1Map = std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(1));
			// translate modified input maps
			u32 buttonMask = (u32)(FK_Input_Buttons::Any_NonDirectionButton | FK_Input_Buttons::Any_MenuButton | FK_Input_Buttons::Any_SystemButton);
			std::string joypadName = std::string(joystickInfo[0].Name.c_str());
			for (auto iter = joypad1Map.begin(); iter != joypad1Map.end(); ++iter) {
				if ((iter->first & buttonMask) != 0) {
					joypad1Map[iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
						joypadName, (FK_JoypadInputNative)iter->second);
				}
			}
			inputMapper->setJoypadInputMap(1, joypad1Map);
			if (joystickInfo.size() > 1) {
				joypadName = std::string(joystickInfo[1].Name.c_str());
				joypad1Map = std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(2));
				for (auto iter = joypad1Map.begin(); iter != joypad1Map.end(); ++iter) {
					if ((iter->first & buttonMask) != 0) {
						joypad1Map[iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
							joypadName, (FK_JoypadInputNative)iter->second);
					}
				}
			}
			inputMapper->setJoypadInputMap(2, joypad1Map);
		}
		// save to file
		inputMapper->saveMapToFile(configurationFilesPath + fk_constants::FK_InputMapFileName);
	};

	// draw item
	void FK_SceneInputMapping_SingleplayerKeyboard::drawItem(u32 itemIndex){

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
			if (modifiedInputMap[0][gameButtons[tempIndex]] != KEY_KEY_CODES_COUNT){
				assignedKey1 = acceptedKeys[(EKEY_CODE)modifiedInputMap[0][gameButtons[tempIndex]]];
			}
			else{
				assignedKey1 = unassignedKeyString;
			}
			fixedOffsetY += 40;
		}
		else if (itemIndex >= directionButtons.size()){
			tempIndex -= directionButtons.size();
			if (modifiedInputMap[0][menuButtons[tempIndex]] != KEY_KEY_CODES_COUNT){
				assignedKey1 = acceptedKeys[(EKEY_CODE)modifiedInputMap[0][menuButtons[tempIndex]]];
			}
			else{
				assignedKey1 = unassignedKeyString;
			}
			fixedOffsetY += 20;
		}
		else{
			if (modifiedInputMap[0][directionButtons[tempIndex]] != KEY_KEY_CODES_COUNT){
				assignedKey1 = acceptedKeys[(EKEY_CODE)modifiedInputMap[0][directionButtons[tempIndex]]];
			}
			else{
				assignedKey1 = unassignedKeyString;
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
		fixedOffsetX = 32;
		/* get width and height of the string*/
		itemColor = irr::video::SColor(255, 20, 225, 120);
		itemString = std::wstring(assignedKey1.begin(), assignedKey1.end());
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		sentenceX = fixedOffsetX; // distance from screen center
		sentenceY = fixedOffsetY + itemIndex * fixedSpacingY; // distance from the top of the screen
		if (itemIndex == menuIndex){
			//sentenceX += selectedItemAdditionalOffsetX;
			itemColor = getSelectedKeyColor();// irr::video::SColor(255, 20, 225, 220);
			selectedItemXL = screenSize.Width - (sentenceX * scale_factorX + sentenceWidth);
			selectedItemXR = screenSize.Width - sentenceX * scale_factorX;
			selectedItemY = (sentenceY + 5) * scale_factorY;
			selectedItemDimension = sentenceHeight * 3 / 4;
		}
		destinationRect = core::rect<s32>(
			screenSize.Width - (sentenceX * scale_factorX + sentenceWidth),
			sentenceY * scale_factorY,
			screenSize.Width - sentenceX * scale_factorX,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			itemColor, irr::video::SColor(128, 0, 0, 0));
	}

		void FK_SceneInputMapping_SingleplayerKeyboard::drawTitle(){
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Keyboard Settings";
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

	// draw all
	void FK_SceneInputMapping_SingleplayerKeyboard::drawAll(bool showBackground){
		if (showBackground) {
			drawBackground();
		}
		drawItems();
		drawCaptions();
		drawArrows();
		drawTitle();
	}
	
	
	void FK_SceneInputMapping_SingleplayerKeyboard::swapSameInputButtons(FK_Input_Buttons lastModifiedButton, u32 oldKeyCode){
		u32 inputKey = modifiedInputMap[0][lastModifiedButton];
		std::cout << acceptedKeys[(EKEY_CODE)oldKeyCode] << " " << lastModifiedButton << std::endl;
		if (isDirection(lastModifiedButton)){
			for each (auto key in modifiedInputMap[0]){
				if (key.second == inputKey && key.first != lastModifiedButton){
					modifiedInputMap[0][key.first] = oldKeyCode;
				}
			}
		}
		else if (isMenuButton(lastModifiedButton)){
			for each (auto key in menuButtons){
				if (modifiedInputMap[0][key] == inputKey  && key != lastModifiedButton){
					modifiedInputMap[0][key] = oldKeyCode;
					return;
				}
			}
			for each (auto key in directionButtons){
				if (modifiedInputMap[0][key] == inputKey && key != lastModifiedButton){
					modifiedInputMap[0][key] = oldKeyCode;
					return;
				}
			}

		}
		else if (isGameButton(lastModifiedButton)){
			for each (auto key in gameButtons){
				if (modifiedInputMap[0][key] == inputKey && key != lastModifiedButton){
					modifiedInputMap[0][key] = oldKeyCode;
					return;
				}
			}
			for each (auto key in directionButtons){
				if (modifiedInputMap[0][key] == inputKey && key != lastModifiedButton){
					modifiedInputMap[0][key] = oldKeyCode;
					return;
				}
			}
		}
	}

	// assign new input
	void FK_SceneInputMapping_SingleplayerKeyboard::assignNewInput(FK_Input_Buttons buttonToReplace, u32 newKeyCode){
		if (modifiedInputMap[0][buttonToReplace] != newKeyCode){
			u32 oldKeyCode = modifiedInputMap[0][buttonToReplace];
			modifiedInputMap[0][buttonToReplace] = newKeyCode;
			swapSameInputButtons(buttonToReplace, oldKeyCode);
		}
		else if (isOptionalButton(buttonToReplace)){
			modifiedInputMap[0][buttonToReplace] = KEY_KEY_CODES_COUNT;
		}
	}

	// update input
	void FK_SceneInputMapping_SingleplayerKeyboard::updateInput(){
		u32 lastInputTimePlayer1 = lastInputTime;
		u32 lastInputTimePlayer2 = lastInputTime;
		u32 inputThreshold = 200;
		// check input for player 1
		if (now - lastInputTimePlayer1 > inputThreshold){
			if (joystickInfo.size() > 0){
				player1input->update(now, inputReceiver->JoypadStatus(0), false);
			}
			else{
				player1input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer1 = player1input->getLastDirection();
			u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer1 == FK_Input_Buttons::Confirmation_Button){
				saveAndExit();
				lastInputTime = now;
				return;
			}
			if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button){
				lastInputTime = now;
				std::vector < std::map<FK_Input_Buttons, u32>> originalInputMap = {
					std::map<FK_Input_Buttons, u32>(inputMapper->getKeyboardMap(0))
				};
				bool needsSaving = false;
				for (auto key : originalInputMap[0]) {
					if (originalInputMap[0][key.first] !=
						modifiedInputMap[0][key.first]) {
						needsSaving = true;
						break;
					}
				}
				if (needsSaving) {
					activateExitMenu();
				}
				else {
					exitScene(false);
				}
				return;
			}
			else{
				if (directionPlayer1 == FK_Input_Buttons::Down_Direction){
					increaseMenuIndex();
					lastInputTime = now;
				}
				else if (directionPlayer1 == FK_Input_Buttons::Up_Direction){
					decreaseMenuIndex();
					lastInputTime = now;
				}
				else{
					if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button){
						selectItem();
						lastInputTime = now;
						Sleep(200);
						return;
					}
				}
			}
		}
		// check input for player 2
		if (now - lastInputTimePlayer2 > inputThreshold){
			if (joystickInfo.size() > 1){
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			else{
				player2input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer2 = player2input->getLastDirection();
			u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer2 == FK_Input_Buttons::Confirmation_Button){
				saveAndExit();
				lastInputTime = now;
				return;
			}
			if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button){
				lastInputTime = now;
				std::vector < std::map<FK_Input_Buttons, u32>> originalInputMap = {
					std::map<FK_Input_Buttons, u32>(inputMapper->getKeyboardMap(0))
				};
				bool needsSaving = false;
				for (auto key : originalInputMap[0]) {
					if (originalInputMap[0][key.first] !=
						modifiedInputMap[0][key.first]) {
						needsSaving = true;
						break;
					}
				}
				if (needsSaving) {
					activateExitMenu();
				}
				else {
					exitScene(false);
				}
				return;
			}
			else{
				if (directionPlayer2 == FK_Input_Buttons::Down_Direction){
					increaseMenuIndex();
					lastInputTime = now;
				}
				else if (directionPlayer2 == FK_Input_Buttons::Up_Direction){
					decreaseMenuIndex();
					lastInputTime = now;
				}
				else{
					if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button){
						selectItem();
						lastInputTime = now;
						Sleep(200);
						return;
					}
				}
			}
		}
	}
}