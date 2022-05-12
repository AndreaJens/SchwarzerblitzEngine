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

#include"FK_SceneInputMapping_JoypadPlayer.h"
#include"ExternalAddons.h"
#include<Windows.h>
#include<iostream>

namespace fk_engine {
	FK_SceneInputMapping_JoypadPlayer::FK_SceneInputMapping_JoypadPlayer(u32 playerId) : FK_SceneInputMapping() {
		playerIndex = playerId;
	};
	FK_SceneInputMapping_JoypadPlayer::FK_SceneInputMapping_JoypadPlayer(u32 playerId, IrrlichtDevice* newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options* newOptions) : FK_SceneInputMapping_JoypadPlayer(playerId) {
		setup(newDevice, joypadInfo, newOptions);
	};
	void FK_SceneInputMapping_JoypadPlayer::setup(IrrlichtDevice* newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options* newOptions) {
		FK_SceneInputMapping::setup(newDevice, joypadInfo, newOptions);
		initializeJoypadButtonsMap();
		modifiedInputMap = {
			std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(1)),
			std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(2)),
		};
		// translate modified input maps
		u32 buttonMask = (u32)(FK_Input_Buttons::Any_NonDirectionButton | FK_Input_Buttons::Any_MenuButton | FK_Input_Buttons::Any_SystemButton);
		std::string joypadName = std::string(joystickInfo[0].Name.c_str());
		for (auto iter = modifiedInputMap[0].begin(); iter != modifiedInputMap[0].end(); ++iter) {
			if ((iter->first & buttonMask) != 0) {
				modifiedInputMap[0][iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
					joypadName, (FK_JoypadInputNative)iter->second);
			}
		}
		if (joypadInfo.size() > 1) {
			joypadName = std::string(joystickInfo[1].Name.c_str());
		}
		for (auto iter = modifiedInputMap[1].begin(); iter != modifiedInputMap[1].end(); ++iter) {
			if ((iter->first & buttonMask) != 0) {
				modifiedInputMap[1][iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
					joypadName, (FK_JoypadInputNative)iter->second);
			}
		}
		originalInputMap = {
					std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(1)),
					std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(2)),
		};
		originalInputMapTranslated = {
					std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(1)),
					std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(2)),
		};
		joypadName = std::string(joystickInfo[0].Name.c_str());
		for (auto iter = originalInputMapTranslated[0].begin(); iter != originalInputMapTranslated[0].end(); ++iter) {
			if ((iter->first & buttonMask) != 0) {
				originalInputMapTranslated[0][iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
					joypadName, (FK_JoypadInputNative)iter->second);
			}
		}
		if (joystickInfo.size() > 1) {
			joypadName = std::string(joystickInfo[1].Name.c_str());
		}
		for (auto iter = originalInputMapTranslated[1].begin(); iter != originalInputMapTranslated[1].end(); ++iter) {
			if ((iter->first & buttonMask) != 0) {
				originalInputMapTranslated[1][iter->first] = inputMapper->getTranslatedButtonFromJoypadMap(
					joypadName, (FK_JoypadInputNative)iter->second);
			}
		}
		menuIndex = directionButtons.size();
		selectItem();
	};

	void FK_SceneInputMapping_JoypadPlayer::saveSettings() {
		inputMapper->setJoypadInputMap(1, modifiedInputMap[0]);
		inputMapper->setJoypadInputMap(2, modifiedInputMap[1]);
		inputMapper->saveMapToFile(configurationFilesPath + fk_constants::FK_InputMapFileName);
	};

	void FK_SceneInputMapping_JoypadPlayer::initialize() {
		FK_SceneInputMapping::initialize();
		playerIndex = 0;
	}

	// draw item
	void FK_SceneInputMapping_JoypadPlayer::drawItem(u32 itemIndex) {
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
		if (menuIndex > 5) {
			fixedOffsetY -= (fixedSpacingY * (menuIndex - 5));
			s32 thresholdValue = (menuOptionsStrings.size()) * fixedSpacingY + baseOffsetY + 40;
			thresholdValue = screenSize.Height / scale_factorY - thresholdValue;
			if (fixedOffsetY < thresholdValue) {
				fixedOffsetY = thresholdValue;
			}
		}

		if (itemIndex >= directionButtons.size() + menuButtons.size()) {
			tempIndex -= directionButtons.size() + menuButtons.size();
			if (modifiedInputMap[playerIndex][gameButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native) {
				assignedKey1 = joypadKeys[(FK_JoypadInput)modifiedInputMap[playerIndex][gameButtons[tempIndex]]];
			}
			else {
				assignedKey1 = unassignedKeyString;
			}
			fixedOffsetY += 40;
		}
		else if (itemIndex >= directionButtons.size()) {
			tempIndex -= directionButtons.size();
			if (modifiedInputMap[playerIndex][menuButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native) {
				assignedKey1 = joypadKeys[(FK_JoypadInput)modifiedInputMap[playerIndex][menuButtons[tempIndex]]];
			}
			else {
				assignedKey1 = unassignedKeyString;
			}
			fixedOffsetY += 20;
		}
		else {
			if (modifiedInputMap[playerIndex][directionButtons[tempIndex]] != FK_JoypadInputNative::JoypadInput_NoButton_Native) {
				assignedKey1 = joypadKeys[(FK_JoypadInput)modifiedInputMap[playerIndex][directionButtons[tempIndex]]];
			}
			else {
				assignedKey1 = unassignedKeyString;
			}
		}

		// don't draw if outside of screen
		s32 tempY = fixedOffsetY + itemIndex * fixedSpacingY;
		if (tempY < baseOffsetY) {
			return;
		}
		if (tempY > screenSize.Height / scale_factorY - (baseOffsetY + fixedSpacingY)) {
			return;
		}

		s32 fixedOffsetX = 32;
		s32 selectedItemAdditionalOffsetX = 15;

		irr::video::SColor itemColor = irr::video::SColor(255, 255, 255, 255);
		std::wstring itemString(keyIdentifier.begin(), keyIdentifier.end());
		s32 sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		s32 sentenceX = fixedOffsetX;
		if (itemIndex == menuIndex) {
			sentenceX += selectedItemAdditionalOffsetX;
			itemColor = irr::video::SColor(255, 200, 200, 20);
		}
		// direction can't be modified for joypad
		if (itemIndex < directionButtons.size() || menuIndex == directionButtons.size() && itemSelected && itemIndex != menuIndex) {
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
		fixedOffsetX = 32;
		/* get width and height of the string*/
		itemColor = irr::video::SColor(255, 20, 225, 120);
		itemString = std::wstring(assignedKey1.begin(), assignedKey1.end());
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		sentenceX = fixedOffsetX; // distance from screen center
		sentenceY = fixedOffsetY + itemIndex * fixedSpacingY; // distance from the top of the screen
		if (menuIndex == directionButtons.size() && itemSelected && itemIndex != menuIndex) {
			itemColor = irr::video::SColor(255, 96, 96, 96);
		}
		if (itemIndex == menuIndex) {
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

	void FK_SceneInputMapping_JoypadPlayer::drawTitle() {
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Player 1 Joypad Settings";
		std::string joypadName = std::string(joystickInfo[0].Name.c_str());
		if (playerIndex > 0) {
			itemString = L"Player 2 Joypad Settings";
			joypadName = std::string(joystickInfo[1].Name.c_str());
		}
		std::wstring joypadNameWstr = std::wstring(joypadName.begin(), joypadName.end());
		s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
		x -= sentenceWidth / 2;
		y -= sentenceHeight / 2;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(font, itemString, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));

		y += sentenceHeight;
		sentenceWidth = captionFont->getDimension(joypadNameWstr.c_str()).Width;
		sentenceHeight = captionFont->getDimension(joypadNameWstr.c_str()).Height;
		x = screenSize.Width / 2;
		x -= sentenceWidth / 2;
		
		destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(captionFont, joypadNameWstr, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	// draw all
	void FK_SceneInputMapping_JoypadPlayer::drawAll(bool showBackground) {
		if (showBackground) {
			drawBackground();
		}
		drawItems();
		drawCaptions();
		drawArrows();
		drawTitle();
	}

	void FK_SceneInputMapping_JoypadPlayer::swapSameInputButtons(FK_Input_Buttons lastModifiedButton, u32 oldKeyCode) {
		u32 inputKey = modifiedInputMap[playerIndex][lastModifiedButton];
		std::cout << joypadKeys[(FK_JoypadInput)oldKeyCode] << " " << lastModifiedButton << std::endl;
		if (isDirection(lastModifiedButton)) {
			for each (auto key in modifiedInputMap[playerIndex]) {
				if (key.second == inputKey && key.first != lastModifiedButton) {
					modifiedInputMap[playerIndex][key.first] = oldKeyCode;
				}
			}
		}
		else if (isMenuButton(lastModifiedButton)) {
			for each (auto key in menuButtons) {
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton) {
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
			for each (auto key in directionButtons) {
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton) {
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
		}
		else if (isGameButton(lastModifiedButton)) {
			for each (auto key in gameButtons) {
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton) {
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
			for each (auto key in directionButtons) {
				if (modifiedInputMap[playerIndex][key] == inputKey && key != lastModifiedButton) {
					modifiedInputMap[playerIndex][key] = oldKeyCode;
					return;
				}
			}
		}
	}

	void FK_SceneInputMapping_JoypadPlayer::restoreOriginalSaveSettings()
	{
		inputMapper->setJoypadInputMap(1, originalInputMapTranslated[0]);
		inputMapper->setJoypadInputMap(2, originalInputMapTranslated[1]);
		inputMapper->saveMapToFile(configurationFilesPath + fk_constants::FK_InputMapFileName);
	}

	// get last pressed button
	u32 FK_SceneInputMapping_JoypadPlayer::getLastPressedButton() {
		u32 joypadIndex = playerIndex;
		bool* keyArray = inputReceiver->JoypadStatus(joypadIndex);
		u32 currentInput = (u32)FK_JoypadInputNative::JoypadInput_NoButton_Native;
		u32 numberOfNativeButtons = FK_JoypadInputNative::JoypadInput_NumberOfButtons;
		for (u32 i = 0; i < numberOfNativeButtons; ++i) {
			if (keyArray[i]) {
				currentInput = i;
				u32 currentButton = inputMapper->getTranslatedButtonFromJoypadMap(
					std::string(joystickInfo[joypadIndex].Name.c_str()), (FK_JoypadInputNative)currentInput);
				//std::cout << "current input = " << currentInput << " " << currentButton << " " << joypadKeys[(FK_JoypadInput)currentButton] << std::endl;
				currentInput = currentButton;
				if (currentButton != FK_JoypadInput::JoypadInput_NoButton) {
					return currentInput;
				}
			}
		}
		return currentInput;
	}

	// update selection
	void FK_SceneInputMapping_JoypadPlayer::updateSelection() {
		u32 inputThreshold = 200;
		if ((now - lastInputTime) > inputThreshold) {
			u32 lastSelectionButton = modifiedInputMap[playerIndex][FK_Input_Buttons::Selection_Button];
			u32 newInput = FK_SceneInputMapping_JoypadPlayer::getLastPressedButton();
			if (newInput != oldInputButton && newInput < FK_JoypadInputNative::JoypadInput_NumberOfButtons) {
				lastInputTime = now;
				oldInputButton = newInput;
				itemSelected = false;
				Sleep(200);
				assignNewInput(getButtonFromIndex(menuIndex), newInput);
				soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
				// switch selection button if needed
				u32 newSelectionButton = modifiedInputMap[playerIndex][FK_Input_Buttons::Selection_Button];
				if (newSelectionButton != lastSelectionButton) {
					saveSettings();
					delete inputMapper;
					FK_SceneWithInput::setupInputMapper();
				}
			}
		}
	}

	// assign new input
	void FK_SceneInputMapping_JoypadPlayer::assignNewInput(FK_Input_Buttons buttonToReplace, u32 newKeyCode) {
		if (modifiedInputMap[playerIndex][buttonToReplace] != newKeyCode) {
			u32 oldKeyCode = modifiedInputMap[playerIndex][buttonToReplace];
			modifiedInputMap[playerIndex][buttonToReplace] = newKeyCode;
			swapSameInputButtons(buttonToReplace, oldKeyCode);
		}
		else if (isOptionalButton(buttonToReplace)) {
			modifiedInputMap[playerIndex][buttonToReplace] = (u32)FK_JoypadInputNative::JoypadInput_NoButton_Native;
		}
	}

	void FK_SceneInputMapping_JoypadPlayer::increaseMenuIndex() {
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		menuIndex += 1;
		menuIndex %= menuOptionsStrings.size();
		if (menuIndex < directionButtons.size()) {
			menuIndex = directionButtons.size();
		}
	}

	void FK_SceneInputMapping_JoypadPlayer::decreaseMenuIndex() {
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		if (menuIndex == directionButtons.size()) {
			menuIndex = menuOptionsStrings.size() - 1;
		}
		else {
			menuIndex -= 1;
		}
		//captionStillPeriodCounterMs = 0;
		//captionMovementTimeCounterMs = 0;
	}

	// update input
	void FK_SceneInputMapping_JoypadPlayer::updateInput() {
		if (joystickInfo.size() < playerIndex + 1) {
			restoreOriginalSaveSettings();
			exitScene(true);
			return;
		}
		//u32 lastInputTimePlayer1 = player1input->getLastInputTime();
		u32 inputThreshold = 200;
		// check input for player 1
		if (now - lastInputTime > inputThreshold) {
			if (playerIndex == 0 && joystickInfo.size() > 0) {
				player1input->update(now, inputReceiver->JoypadStatus(0), false);
			}
			else if (playerIndex == 1 && joystickInfo.size() > 1) {
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			u32 directionPlayer = player1input->getLastDirection() | player2input->getLastDirection();
			u32 lastButtonPlayer = (player1input->getPressedButtons() |
				player2input->getPressedButtons()) &
				FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer == FK_Input_Buttons::Confirmation_Button) {
				lastInputTime = now;
				saveAndExit();
				return;
			}
			if (lastButtonPlayer == FK_Input_Buttons::Cancel_Button) {
				lastInputTime = now;
				
				bool needsSaving = false;
				if (playerIndex == 0) {
					for (auto key : originalInputMapTranslated[0]) {
						if (originalInputMapTranslated[0][key.first] !=
							modifiedInputMap[0][key.first]) {
							needsSaving = true;
							break;
						}
					}
				}
				if (playerIndex == 1) {
					for (auto key : originalInputMapTranslated[1]) {
						if (originalInputMapTranslated[1][key.first] !=
							modifiedInputMap[1][key.first]) {
							needsSaving = true;
							break;
						}
					}
				}
				if (needsSaving) {
					activateExitMenu();
				}
				else {
					restoreOriginalSaveSettings();
					exitScene(true);
				}
				return;
			}
			else {
				if (directionPlayer == FK_Input_Buttons::Down_Direction) {
					lastInputTime = now;
					increaseMenuIndex();
				}
				else if (directionPlayer == FK_Input_Buttons::Up_Direction) {
					lastInputTime = now;
					decreaseMenuIndex();
				}
				else {
					if (lastButtonPlayer == FK_Input_Buttons::Selection_Button) {
						if (menuIndex < directionButtons.size()) {
							soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
						}
						else {
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

	void FK_SceneInputMapping_JoypadPlayer::exitSceneFromExitMenu(bool playSound)
	{
		restoreOriginalSaveSettings();
		FK_SceneInputMapping::exitSceneFromExitMenu(playSound);
	}
}