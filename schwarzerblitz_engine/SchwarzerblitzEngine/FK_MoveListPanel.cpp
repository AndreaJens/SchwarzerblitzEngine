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

#include"FK_MoveListPanel.h"
#include"ExternalAddons.h"
#include<iostream>

using namespace irr;

namespace fk_engine{
	// default constructor
	FK_MoveListPanel::FK_MoveListPanel(){
		clear();
		reset();
		skinName = "windowskin.png";
		skinName_fwp = "windowskin_fwp.png";
		skinName_trg = "windowskin_trg.png";
		then = 0;
		now = 0;
		inputDelayMS = 0;
		moveWindows = std::vector<FK_Window*>();
		visible_flag = false;
		active_flag = false;
		characterName = std::wstring();
		demoMove = FK_Move();
		showRootMovesOnly = true;
		rootMoveSelectedIndex = -1;
		nextRootMoveSelectedIndex = -1;
		buttonInputLayout[FK_Input_Buttons::Selection_Button] = FK_JoypadInput::JoypadInput_A_Button;
		buttonInputLayout[FK_Input_Buttons::Modifier_Button] = FK_JoypadInput::JoypadInput_X_Button;
		buttonInputLayout[FK_Input_Buttons::ScrollRight_Button] = FK_JoypadInput::JoypadInput_R1_Button;
		buttonInputLayout[FK_Input_Buttons::ScrollLeft_Button] = FK_JoypadInput::JoypadInput_L1_Button;
		keyboardKeyLayout[FK_Input_Buttons::Selection_Button] = L"C";
		keyboardKeyLayout[FK_Input_Buttons::Modifier_Button] = L"A";
		keyboardKeyLayout[FK_Input_Buttons::ScrollRight_Button] = L"E";
		keyboardKeyLayout[FK_Input_Buttons::ScrollLeft_Button] = L"Q";
		showJoypadInputs = true;
	};
	FK_MoveListPanel::~FK_MoveListPanel(){
		clear();
	};
	// basic constructor
	FK_MoveListPanel::FK_MoveListPanel(IrrlichtDevice* new_device, FK_SoundManager* new_soundManager, 
		FK_Options* scene_options, core::dimension2d<u32> viewportSize, bool forDemoPurposes) : FK_MoveListPanel(){
		device = new_device;
		soundManager = new_soundManager;
		gameOptions = scene_options;
		now = device->getTimer()->getRealTime();
		then = now;
		screenSize = viewportSize;
		panelWidth = (s32)screenSize.Width * 3 / 4;
		topLeftCornerX = (s32)(screenSize.Width - panelWidth) / 2;
		panelHeight = (s32)screenSize.Height * 2 / 3;
		topLeftCornerY = (s32)(screenSize.Height - panelHeight) / 2;
		windowSizeY = panelHeight / 4;
		mainFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		notesFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		demoOnly = forDemoPurposes;
	}
	// advanced constructor
	FK_MoveListPanel::FK_MoveListPanel(IrrlichtDevice* new_device, FK_SoundManager* new_soundManager, 
		FK_Options* scene_options, core::dimension2d<u32> viewportSize, FK_Character* referenceCharacter,
		std::string resourcesPath, std::string skinName, bool forDemoPurposes) :
		FK_MoveListPanel(new_device, new_soundManager, scene_options, viewportSize, forDemoPurposes){
		setSkin(skinName);
		setup(referenceCharacter, resourcesPath);
		loadTextures();
	}
	// setup variables and read file
	void FK_MoveListPanel::setup(FK_Character* referenceCharacter, std::string new_resourcesPath){
		std::map<FK_Stance_Type, std::string> specialStanceDictionary;
		if (characterName != referenceCharacter->getWName()){
			movelistCharacterReference = referenceCharacter;
			characterName = referenceCharacter->getWName();
			resourcePath = new_resourcesPath;
			specialStanceDictionary = referenceCharacter->getSpecialStanceDictionary();
			parseMoves(referenceCharacter);
		}
		setupWindows();
		setupStanceMap(specialStanceDictionary);
	}

	// setup windows
	void FK_MoveListPanel::setupWindows(){
		for each (FK_Move move in moveListTree){
			std::string temp_skinName = skinName;
			s32 tempWidth = panelWidth;
			if (move.isFollowupOnly()){
				temp_skinName = skinName_fwp;
				tempWidth = 0.9f * panelWidth;
			}
			if (move.getNumberOfRequiredBulletCounters() > 0) {
				temp_skinName = skinName_trg;
			}
			FK_Window* temp_window = new FK_Window(device, core::vector2d<s32>(0, 0), tempWidth,
				windowSizeY, temp_skinName, resourcePath);
			moveWindows.push_back(temp_window);
		}
		for each(FK_Window* window in moveWindows){
			window->forceOpen();
		}
	}

	// setup stance map
	void FK_MoveListPanel::setupStanceMap(std::map<FK_Stance_Type, std::string> specialStanceDictionary){
		stanceMapForMoveList[FK_Stance_Type::GroundStance] = L"while standing";
		stanceMapForMoveList[FK_Stance_Type::CrouchedStance] = L"while crouching";
		stanceMapForMoveList[FK_Stance_Type::RunningStance] = L"while running";
		stanceMapForMoveList[FK_Stance_Type::AirStance] = L"while jumping";
		stanceMapForMoveList[FK_Stance_Type::LandingStance] = L"while landing";
		stanceMapForMoveList[FK_Stance_Type::SupineStance] = L"while lying on the ground";
		stanceMapForMoveList[FK_Stance_Type::ProneStance] = L"while lying on the ground";
		std::vector<FK_Stance_Type> specialStanceVector = {
			FK_Stance_Type::SpecialStance1,
			FK_Stance_Type::SpecialStance2,
			FK_Stance_Type::SpecialStance3,
			FK_Stance_Type::SpecialStance4,
			FK_Stance_Type::SpecialStance5
		};
		for (u32 i = 0; i < specialStanceVector.size(); ++i){
			FK_Stance_Type tempStance = specialStanceVector[i];
			if (specialStanceDictionary.count(tempStance) > 0){
				std::string stanceName = specialStanceDictionary[tempStance];
				stanceMapForMoveList[tempStance] = L"while in " + std::wstring(stanceName.begin(), stanceName.end());
			}
			else{
				stanceMapForMoveList[tempStance] = L"while in special stance";
			}
		}
	}

	// clear all
	void FK_MoveListPanel::clear(){
		for each(FK_MoveListMove* tempMove in moveList){
			delete tempMove;
			tempMove = NULL;
		}
		stanceMapForMoveList.clear();
		rootMovesIndex.clear();
		rootMovesBranches.clear();
		moveList.clear();
		moveMap.clear();
		moveListTree.clear();
		clearWindows();
	}
	// set new skin
	void FK_MoveListPanel::setSkin(std::string new_skinName){
		skinName = new_skinName;
	}
	// update
	void FK_MoveListPanel::update(u32 pressedButton, bool updateActionButtons){
		now = device->getTimer()->getRealTime();
		u32 delta_t_ms = now - then;
		u32 timeDelayForInputMs = 200;
		inputDelayMS += delta_t_ms;
		then = now;
		for each(FK_Window* window in moveWindows){
			window->update(delta_t_ms);
		}
		if (inputDelayMS > timeDelayForInputMs && isActive()){
			if (pressedButton & FK_Input_Buttons::Down_Direction){
				u32 testIndex = moveIndex + 1;
				testIndex %= moveListTree.size();
				bool followupSafeCondition = moveListTree[testIndex].isFollowupOnly();
				followupSafeCondition &= (s32)testIndex > rootMoveSelectedIndex &&
					(nextRootMoveSelectedIndex == 0 || (s32)testIndex < nextRootMoveSelectedIndex);
				if (showRootMovesOnly && !followupSafeCondition){
					toNextRootMove();
				}
				else{
					increaseCursor();
				}
				return;
			}
			if (pressedButton & FK_Input_Buttons::Up_Direction){
				u32 testIndex = moveIndex;
				if (testIndex == 0) {
					testIndex += moveListTree.size();
				}
				testIndex -= 1;
				bool followupSafeCondition = moveListTree[testIndex].isFollowupOnly();
				followupSafeCondition &= (s32)testIndex > rootMoveSelectedIndex && 
					(nextRootMoveSelectedIndex == 0 || (s32)testIndex < nextRootMoveSelectedIndex);
				if (showRootMovesOnly && !followupSafeCondition){
					toPreviousRootMove();
				}
				else{
					decreaseCursor();
				}
				return;
			}
			if (pressedButton & (FK_Input_Buttons::ScrollRight_Button | FK_Input_Buttons::Right_Direction)) {
				toNextRootMove();
				return;
			}
			if (pressedButton & (FK_Input_Buttons::ScrollLeft_Button | FK_Input_Buttons::Left_Direction)) {
				toPreviousRootMove();
				return;
			}
			if (updateActionButtons) {
				if ((pressedButton & FK_Input_Buttons::Selection_Button)/* | (pressedButton & FK_Input_Buttons::Guard_Button)*/) {
					if (showRootMovesOnly && !moveListTree[moveIndex].isFollowupOnly()) {
						selectItem();
						return;
					}
				}
				if (pressedButton & FK_Input_Buttons::Cancel_Button) {
					cancelSelection();
					return;
				}
				if ((pressedButton & FK_Input_Buttons::Modifier_Button)/* | (pressedButton & FK_Input_Buttons::Kick_Button)*/) {
					toggleBranchMovesVisibility();
					return;
				}
			}
			else {
				if (pressedButton & FK_Input_Buttons::Selection_Button) {
					selectMoveForDemo();
					return;
				}
				if (pressedButton & FK_Input_Buttons::Cancel_Button) {
					demoMove = FK_Move();
					selectionFlag = false;
					cancelSelection();
					return;
				}
			}
		}
	}

	/* set correct icons based on input maps*/
	void FK_MoveListPanel::setIconsBasedOnCurrentInputMap(FK_InputMapper* mapper, s32 mapIndex, 
		bool isJoypad, std::string joypadName) {
		if (isJoypad && joypadName.empty()) {
			return;
		}
		std::map<FK_Input_Buttons, u32> mapToCheck;
		if (isJoypad) {
			showJoypadInputs = true;
			mapToCheck = std::map<FK_Input_Buttons, u32> (mapper->getJoypadMap(mapIndex));
			
			buttonInputLayout[FK_Input_Buttons::Selection_Button] = 
				mapper->getTranslatedButtonFromJoypadMap(joypadName, 
				(FK_JoypadInputNative) mapToCheck[FK_Input_Buttons::Selection_Button]);
			buttonInputLayout[FK_Input_Buttons::Modifier_Button] = mapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative) mapToCheck[FK_Input_Buttons::Modifier_Button]);
			buttonInputLayout[FK_Input_Buttons::ScrollRight_Button] = mapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative) mapToCheck[FK_Input_Buttons::ScrollRight_Button]);
			buttonInputLayout[FK_Input_Buttons::ScrollLeft_Button] = mapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative) mapToCheck[FK_Input_Buttons::ScrollLeft_Button]);
		}
		else {
			showJoypadInputs = false;
			mapToCheck = std::map<FK_Input_Buttons, u32>(mapper->getKeyboardMap(mapIndex));
			// create map
			std::map <EKEY_CODE, std::wstring> acceptedKeys;
			// directions
			acceptedKeys[EKEY_CODE::KEY_UP] = L"Up";
			acceptedKeys[EKEY_CODE::KEY_DOWN] = L"Down";
			acceptedKeys[EKEY_CODE::KEY_LEFT] = L"Left";
			acceptedKeys[EKEY_CODE::KEY_RIGHT] = L"Right";

			// letters and numbers
			std::vector <std::wstring> letters = {
				L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J",
				L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T",
				L"U", L"V", L"W", L"X", L"Y", L"Z"
			};
			std::vector <std::wstring> numbers = {
				L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9",
			};
			for (u32 i = (u32)(EKEY_CODE::KEY_KEY_A); i <= (u32)(EKEY_CODE::KEY_KEY_Z); ++i) {
				acceptedKeys[(EKEY_CODE)i] = letters[i - (u32)(EKEY_CODE::KEY_KEY_A)];
			}
			for (u32 i = (u32)(EKEY_CODE::KEY_KEY_0); i <= (u32)(EKEY_CODE::KEY_KEY_9); ++i) {
				acceptedKeys[(EKEY_CODE)i] = numbers[i - (u32)(EKEY_CODE::KEY_KEY_0)];
			}

			// additional signs
			acceptedKeys[EKEY_CODE::KEY_PLUS] = L"+";  // PLUS key
			acceptedKeys[EKEY_CODE::KEY_MINUS] = L"-";  // MINUS key
			acceptedKeys[EKEY_CODE::KEY_COMMA] = L",";  // COMMA key
			acceptedKeys[EKEY_CODE::KEY_PERIOD] = L".";  // PERIOD key

			// function buttons
			acceptedKeys[EKEY_CODE::KEY_BACK] = L"BCK";  // BACKSPACE key
			acceptedKeys[EKEY_CODE::KEY_TAB] = L"TAB";  // TAB key
			acceptedKeys[EKEY_CODE::KEY_CLEAR] = L"CLR";  // CLEAR key
			acceptedKeys[EKEY_CODE::KEY_RETURN] = L"ENTER";  // ENTER key
			acceptedKeys[EKEY_CODE::KEY_LSHIFT] = L"SHIFT";  // SHIFT key
			acceptedKeys[EKEY_CODE::KEY_LCONTROL] = L"L CTRL";  // CTRL key
			acceptedKeys[EKEY_CODE::KEY_LMENU] = L"L ALT";  // ALT key
			acceptedKeys[EKEY_CODE::KEY_RSHIFT] = L"R SHIFT";  // SHIFT key
			acceptedKeys[EKEY_CODE::KEY_RCONTROL] = L"R CTRL";  // CTRL key
			acceptedKeys[EKEY_CODE::KEY_RMENU] = L"R ALT";  // ALT key
			acceptedKeys[EKEY_CODE::KEY_PAUSE] = L"PAUSE";  // PAUSE key
			acceptedKeys[EKEY_CODE::KEY_INSERT] = L"INS";  // INSERT key
			acceptedKeys[EKEY_CODE::KEY_DELETE] = L"DEL";  // DELETE key
			acceptedKeys[EKEY_CODE::KEY_SPACE] = L"SPACE";  // SPACE key
			acceptedKeys[EKEY_CODE::KEY_PRIOR] = L"Pg-";  // PG UP key
			acceptedKeys[EKEY_CODE::KEY_NEXT] = L"Pg+";  // PG DOWN key
			acceptedKeys[EKEY_CODE::KEY_END] = L"END";  // END key
			acceptedKeys[EKEY_CODE::KEY_HOME] = L"HOME";  // HOME key
			acceptedKeys[EKEY_CODE::KEY_CAPITAL] = L"CAPS"; // CAPS LOCK key
			acceptedKeys[EKEY_CODE::KEY_ESCAPE] = L"ESC"; //ESCAPE key

														 // numpad
			std::vector <std::wstring> numpad = {
				L"N0",L"N1",L"N2",L"N3",L"N4",L"N5",L"N6",L"N7",L"N8",L"N9",
				L"N*", L"N+", L"N.", L"N-", L"N,", L"N/"
			};
			for (u32 i = (u32)(EKEY_CODE::KEY_NUMPAD0); i <= (u32)(EKEY_CODE::KEY_DIVIDE); ++i) {
				acceptedKeys[(EKEY_CODE)i] = numpad[i - (u32)(EKEY_CODE::KEY_NUMPAD0)];
			}

			//OEM keys
			acceptedKeys[EKEY_CODE::KEY_OEM_1] = L"OEM1";
			acceptedKeys[EKEY_CODE::KEY_OEM_2] = L"OEM2";
			acceptedKeys[EKEY_CODE::KEY_OEM_3] = L"OEM3";
			acceptedKeys[EKEY_CODE::KEY_OEM_4] = L"OEM4";
			acceptedKeys[EKEY_CODE::KEY_OEM_5] = L"OEM5";
			acceptedKeys[EKEY_CODE::KEY_OEM_6] = L"OEM6";
			acceptedKeys[EKEY_CODE::KEY_OEM_7] = L"OEM7";
			acceptedKeys[EKEY_CODE::KEY_OEM_8] = L"OEM8";
			acceptedKeys[EKEY_CODE::KEY_OEM_102] = L"OEM9";

			//F keys
			acceptedKeys[EKEY_CODE::KEY_F1] = L"F1";
			acceptedKeys[EKEY_CODE::KEY_F2] = L"F2";
			acceptedKeys[EKEY_CODE::KEY_F3] = L"F3";
			acceptedKeys[EKEY_CODE::KEY_F4] = L"F4";
			acceptedKeys[EKEY_CODE::KEY_F5] = L"F5";
			acceptedKeys[EKEY_CODE::KEY_F6] = L"F6";
			acceptedKeys[EKEY_CODE::KEY_F7] = L"F7";
			acceptedKeys[EKEY_CODE::KEY_F8] = L"F8";
			acceptedKeys[EKEY_CODE::KEY_F9] = L"F9";
			acceptedKeys[EKEY_CODE::KEY_F10] = L"F10";
			acceptedKeys[EKEY_CODE::KEY_F11] = L"F11";
			acceptedKeys[EKEY_CODE::KEY_F12] = L"F12";
			acceptedKeys[EKEY_CODE::KEY_F13] = L"F13";
			acceptedKeys[EKEY_CODE::KEY_F14] = L"F14";
			acceptedKeys[EKEY_CODE::KEY_F15] = L"F15";
			acceptedKeys[EKEY_CODE::KEY_F16] = L"F16";
			acceptedKeys[EKEY_CODE::KEY_F17] = L"F17";
			acceptedKeys[EKEY_CODE::KEY_F18] = L"F18";
			acceptedKeys[EKEY_CODE::KEY_F19] = L"F19";
			acceptedKeys[EKEY_CODE::KEY_F20] = L"F20";
			acceptedKeys[EKEY_CODE::KEY_F21] = L"F21";
			acceptedKeys[EKEY_CODE::KEY_F22] = L"F22";
			acceptedKeys[EKEY_CODE::KEY_F23] = L"F23";
			acceptedKeys[EKEY_CODE::KEY_F24] = L"F24";

			// read real input
			keyboardKeyLayout[FK_Input_Buttons::Selection_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::Selection_Button]];
			keyboardKeyLayout[FK_Input_Buttons::Modifier_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::Modifier_Button]];
			keyboardKeyLayout[FK_Input_Buttons::ScrollRight_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::ScrollRight_Button]];
			keyboardKeyLayout[FK_Input_Buttons::ScrollLeft_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::ScrollLeft_Button]];
		}
	}

	// draw all
	void FK_MoveListPanel::draw(bool drawHelp){

		// draw move list windows
		std::wstring itemToDraw(characterName.begin(), characterName.end());
		core::dimension2d<u32> textSize = mainFont->getDimension(itemToDraw.c_str());
		core::rect<s32> destRect = core::rect<s32>(topLeftCornerX, topLeftCornerY - textSize.Height,
			topLeftCornerX + textSize.Width, topLeftCornerY);
		fk_addons::drawBorderedText(mainFont, itemToDraw, destRect, 
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
		itemToDraw = L"Move list";
		textSize = mainFont->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(topLeftCornerX + panelWidth - textSize.Width, topLeftCornerY - textSize.Height,
			topLeftCornerX + panelWidth, topLeftCornerY);
		fk_addons::drawBorderedText(mainFont, itemToDraw, destRect,
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
		// draw windows
		s32 wsize = moveWindows.size();
		for (int i = 0; i < wsize; ++i){
			if (!showRootMovesOnly || 
				(showRootMovesOnly && !moveListTree[i].isFollowupOnly()) ||
				(showRootMovesOnly && moveListTree[i].isFollowupOnly() &&
					i > rootMoveSelectedIndex && 
					(nextRootMoveSelectedIndex == 0 || i < nextRootMoveSelectedIndex))
				){
				if (isWindowVisible(i)){
					drawItem(i);
				}
			}
		}
		// draw selector
		u32 oscillationPeriod = 1000;
		f32 phase = (f32)(now % oscillationPeriod) / oscillationPeriod * 2.0f * 3.1415926f;
		s32 opacity = (s32)std::floor(180 + 75 * std::cos(phase));
		moveWindows[moveIndex]->drawSelector(
			moveWindows[moveIndex]->getTopLeftCornerPosition().X, 
			moveWindows[moveIndex]->getTopLeftCornerPosition().Y,
			panelWidth, windowSizeY, opacity);
		// draw number of moves for scrolling purpose
		itemToDraw = std::to_wstring(moveIndex + 1) + L"/" + std::to_wstring(moveListTree.size());
		if (showRootMovesOnly){
			u32 rootSize = rootMovesIndex.size();
			if (moveIndex > rootMoveSelectedIndex &&
				(nextRootMoveSelectedIndex == 0 || moveIndex < nextRootMoveSelectedIndex)) {
				itemToDraw = std::to_wstring(rootMovesIndex[rootMoveSelectedIndex] + 1);
				itemToDraw += L"." + std::to_wstring(moveIndex - rootMoveSelectedIndex);
				itemToDraw += L"/" + std::to_wstring(rootSize);
			}
			else {
				itemToDraw = std::to_wstring(rootMovesIndex[moveIndex] + 1) + L"/" + std::to_wstring(rootSize);
			}
		}
		textSize = mainFont->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(
			topLeftCornerX + panelWidth - textSize.Width, 
			topLeftCornerY + panelHeight,
			topLeftCornerX + panelWidth, 
			topLeftCornerY + panelHeight + textSize.Height);
		fk_addons::drawBorderedText(mainFont, itemToDraw, destRect,
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));

		if (drawHelp) {
			// draw controls (single move)
			video::ITexture* buttonTex;
			video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
			video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
			if (showJoypadInputs) {
				buttonTex = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::Selection_Button]];
			}
			else {
				buttonTex = keyboardButtonTexture;
			}
			textSize = notesFont->getDimension(L"A");
			u32 iconSize = textSize.Height;
			u32 y1 = topLeftCornerY + panelHeight + textSize.Height - iconSize;
			if (showJoypadInputs) {
				drawIcon(buttonTex, topLeftCornerX, y1, iconSize, iconSize, true);
			}
			else {
				itemToDraw = keyboardKeyLayout[FK_Input_Buttons::Selection_Button];
				textSize = notesFont->getDimension(itemToDraw.c_str());
				u32 baseIconSize = iconSize;
				while (iconSize < textSize.Width) {
					iconSize += baseIconSize;
				}
				drawIcon(buttonTex, topLeftCornerX, y1, iconSize, baseIconSize, true);
				destRect = core::rect<s32>(
					topLeftCornerX,
					topLeftCornerY + panelHeight,
					topLeftCornerX + iconSize,
					topLeftCornerY + panelHeight + baseIconSize);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}

			itemToDraw = L": show chains for selected move";
			video::SColor tcolor = video::SColor(255, 255, 255, 255);
			if (rootMoveSelectedIndex >= 0 &&
				moveIndex >= rootMoveSelectedIndex && (nextRootMoveSelectedIndex == 0 || moveIndex < nextRootMoveSelectedIndex)
				) {
				itemToDraw = L": hide chains for selected move";
			}
			if (moveIndex > rootMoveSelectedIndex && (nextRootMoveSelectedIndex == 0 || moveIndex < nextRootMoveSelectedIndex)) {
				tcolor = video::SColor(255, 128, 128, 128);
			}
			textSize = notesFont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				topLeftCornerX + iconSize,
				topLeftCornerY + panelHeight,
				topLeftCornerX + iconSize + panelWidth + textSize.Width,
				topLeftCornerY + panelHeight + textSize.Height);
			fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
				tcolor, video::SColor(128, 0, 0, 0));
			// draw controls
			if (showJoypadInputs) {
				buttonTex = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::Modifier_Button]];
			}
			else {
				buttonTex = keyboardButtonTexture;
			}
			u32 offsetY = iconSize;
			iconSize = textSize.Height;
			y1 = topLeftCornerY + panelHeight + textSize.Height - iconSize + offsetY;
			if (showJoypadInputs) {
				drawIcon(buttonTex, topLeftCornerX, y1, iconSize, iconSize, true);
			}
			else {
				itemToDraw = keyboardKeyLayout[FK_Input_Buttons::Modifier_Button];
				textSize = notesFont->getDimension(itemToDraw.c_str());
				u32 baseIconSize = iconSize;
				while (iconSize < textSize.Width) {
					iconSize += baseIconSize;
				}
				drawIcon(buttonTex, topLeftCornerX, y1, iconSize, baseIconSize, true);
				destRect = core::rect<s32>(
					topLeftCornerX,
					topLeftCornerY + panelHeight + offsetY,
					topLeftCornerX + iconSize,
					topLeftCornerY + panelHeight + offsetY + baseIconSize);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
			}
			itemToDraw = L": hide all branch moves";
			if (showRootMovesOnly) {
				itemToDraw = L": show all branch moves";
			}
			textSize = notesFont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				topLeftCornerX + iconSize,
				topLeftCornerY + panelHeight + offsetY,
				topLeftCornerX + iconSize + panelWidth + textSize.Width,
				topLeftCornerY + panelHeight + textSize.Height + offsetY);
			fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
			// draw additional controls
			// draw controls

			iconSize = textSize.Height;
			offsetY = 2 * iconSize;
			y1 = topLeftCornerY + panelHeight + textSize.Height - iconSize + offsetY;
			s32 additionalOffsetX = 0;
			if (showJoypadInputs) {
				buttonTex = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::ScrollLeft_Button]];
				drawIcon(buttonTex, topLeftCornerX, y1, iconSize, iconSize, true);
				buttonTex = joypadButtonsTexturesMap[
					(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::ScrollRight_Button]];
				drawIcon(buttonTex, topLeftCornerX + iconSize, y1, iconSize, iconSize, true);
				additionalOffsetX = 2 * iconSize;
			}
			else {
				buttonTex = keyboardButtonTexture;
				itemToDraw = keyboardKeyLayout[FK_Input_Buttons::ScrollLeft_Button];
				textSize = notesFont->getDimension(itemToDraw.c_str());
				u32 baseIconSize = iconSize;
				while (iconSize < textSize.Width) {
					iconSize += baseIconSize;
				}
				drawIcon(buttonTex, topLeftCornerX, y1, iconSize, baseIconSize, true);
				destRect = core::rect<s32>(
					topLeftCornerX,
					topLeftCornerY + panelHeight + offsetY,
					topLeftCornerX + iconSize,
					topLeftCornerY + panelHeight + offsetY + baseIconSize);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
				itemToDraw = keyboardKeyLayout[FK_Input_Buttons::ScrollRight_Button];
				textSize = notesFont->getDimension(itemToDraw.c_str());
				s32 offsetX = iconSize;
				iconSize = baseIconSize;
				while (iconSize < textSize.Width) {
					iconSize += baseIconSize;
				}
				drawIcon(buttonTex, topLeftCornerX + offsetX, y1, iconSize, baseIconSize, true);
				destRect = core::rect<s32>(
					topLeftCornerX + offsetX,
					topLeftCornerY + panelHeight + offsetY,
					topLeftCornerX + offsetX + iconSize,
					topLeftCornerY + panelHeight + offsetY + baseIconSize);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
					keyboardLetterColor, keyboardLetterBorder,
					true, true);
				additionalOffsetX = offsetX + iconSize;
			}

			/*if (!showJoypadInputs) {
				itemToDraw = keyboardKeyLayout[FK_Input_Buttons::ScrollLeft_Button];
				textSize = notesFont->getDimension(itemToDraw.c_str());
				destRect = core::rect<s32>(
					topLeftCornerX,
					topLeftCornerY + panelHeight + offsetY,
					topLeftCornerX + iconSize,
					topLeftCornerY + panelHeight + offsetY + iconSize);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
					video::SColor(255, 0, 0, 0), video::SColor(128, 0, 0, 0),
					true, true);
				itemToDraw = keyboardKeyLayout[FK_Input_Buttons::ScrollRight_Button];
				textSize = notesFont->getDimension(itemToDraw.c_str());
				destRect = core::rect<s32>(
					topLeftCornerX + iconSize,
					topLeftCornerY + panelHeight + offsetY,
					topLeftCornerX + 2*iconSize,
					topLeftCornerY + panelHeight + offsetY + iconSize);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
					video::SColor(255, 0, 0, 0), video::SColor(128, 0, 0, 0),
					true, true);
			}*/
			itemToDraw = L": go to previous/next root move";
			textSize = notesFont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(
				topLeftCornerX + additionalOffsetX,
				topLeftCornerY + panelHeight + offsetY,
				topLeftCornerX + additionalOffsetX + panelWidth + textSize.Width,
				topLeftCornerY + panelHeight + textSize.Height + offsetY);
			fk_addons::drawBorderedText(notesFont, itemToDraw, destRect,
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0));
		}
	};

	// draw single item
	void FK_MoveListPanel::drawItem(s32 windowIndex){
		s32 windowY = getWindowY(windowIndex);
		s32 windowX = topLeftCornerX;
		if (moveListTree[windowIndex].isFollowupOnly()){
			windowX = topLeftCornerX + (panelWidth - moveWindows[windowIndex]->getWidth());
		}
		core::position2d<s32> tempPosition(windowX, windowY);
		moveWindows[windowIndex]->setPosition(tempPosition);
		moveWindows[windowIndex]->draw();
		std::wstring itemToDraw = moveListTree[windowIndex].getWDisplayName();
		std::replace(itemToDraw.begin(), itemToDraw.end(), L'_', L' ');
		// draw move name
		core::dimension2d<u32> textSize = mainFont->getDimension(itemToDraw.c_str());
		core::dimension2d<u32> nameSize = textSize;
		s32 offsetY = (s32)std::floor(0.2 * textSize.Height);
		s32 offsetX = offsetY;
		core::rect<s32> destRect = core::rect<s32>(windowX + offsetX, windowY + offsetY,
			windowX + offsetX + textSize.Width, windowY + offsetY + textSize.Height);
		fk_addons::drawBorderedText(mainFont, itemToDraw, destRect);
		// draw additional icons (e.g. impact)
		s32 standardXOffset = notesFont->getDimension(L"A").Width / 2;
		s32 iconSize = destRect.getHeight();
		s32 x1 = windowX + moveWindows[windowIndex]->getWidth() - standardXOffset - iconSize;
		s32 y1 = destRect.LowerRightCorner.Y - iconSize;
		
		/*if (moveListTree[windowIndex].getTotalBulletDamage() > 0) {
			drawIcon(projectileIconTex, x1, y1, iconSize, iconSize);
			x1 -= iconSize + standardXOffset;
		}
		if (moveListTree[windowIndex].isImpactAttack()) {
			drawIcon(impactIconTex, x1, y1, iconSize, iconSize);
			x1 -= iconSize + standardXOffset;
		}*/
		
		// draw stance requirement
		itemToDraw = L"";
		if (stanceMapForMoveList.count(moveListTree[windowIndex].getStance()) > 0){
			itemToDraw = stanceMapForMoveList[moveListTree[windowIndex].getStance()];
		}
		textSize = notesFont->getDimension(itemToDraw.c_str());
		offsetY = (s32)std::floor(0.2 * textSize.Height);
		offsetX = offsetY;
		destRect = core::rect<s32>(
			topLeftCornerX + panelWidth - offsetX - textSize.Width, 
			windowY - offsetY + windowSizeY - textSize.Height,
			topLeftCornerX + panelWidth - offsetX,
			windowY - offsetY + windowSizeY);
		fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
		s32 usedCounters = moveListTree[windowIndex].getNumberOfRequiredBulletCounters();
		s32 additionalOffsetX = 0;
		// draw trigger requirement
		if (usedCounters > 0) {
			core::dimension2d<u32> itemSize = core::dimension2d<u32>(0, 0);
			itemToDraw = L"uses ";
			itemToDraw += std::to_wstring(usedCounters);
			itemToDraw += usedCounters == 1 ? L" counter" : L" counters";
			textSize = notesFont->getDimension(itemToDraw.c_str());
			itemSize = textSize;
			offsetY = (s32)std::floor(1.1 * nameSize.Height);
			offsetX = (s32)std::floor(0.2 * nameSize.Height);
			destRect = core::rect<s32>(
				windowX + offsetX,
				windowY + offsetY,
				windowX + offsetX + textSize.Width,
				windowY + offsetY + textSize.Height);
			fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
			additionalOffsetX += textSize.Width;
		}
		// draw object requirement
		if (moveListTree[windowIndex].hasItemRequirements()){
			core::dimension2d<u32> itemSize = core::dimension2d<u32>(0, 0);
			itemToDraw = additionalOffsetX > 0 ? L" - with " : L"with ";
			u32 itemCount = 0;
			u32 vsize = moveListTree[windowIndex].getRequiredObjects().size();
			if (vsize > 0){
				for (u32 i = 0; i < vsize; ++i){
					std::string objectId = moveListTree[windowIndex].getRequiredObjects()[i];
					if (movelistCharacterReference->getObjectByName(objectId) != NULL){
						std::string nameToAdd = movelistCharacterReference->getObjectByName(objectId)->displayName;
						if (movelistCharacterReference->getObjectByName(objectId)->canBeRemoved() ||
							movelistCharacterReference->getObjectByName(objectId)->canBeShown()){
							if (nameToAdd == std::string()){
								nameToAdd = objectId;
							}
							if (itemCount > 0){
								itemToDraw += L", ";
							}
							itemToDraw += std::wstring(nameToAdd.begin(), nameToAdd.end());
							itemCount += 1;
						}
					}
				}
				if (itemCount > 0){
					textSize = notesFont->getDimension(itemToDraw.c_str());
					itemSize = textSize;
					offsetY = (s32)std::floor(1.1 * nameSize.Height);
					offsetX = (s32)std::floor(0.2 * nameSize.Height) + additionalOffsetX;
					destRect = core::rect<s32>(
						windowX + offsetX,
						windowY + offsetY,
						windowX + offsetX + textSize.Width,
						windowY + offsetY + textSize.Height);
					fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
					additionalOffsetX += textSize.Width;
				}
			}
			// if no dependency on objects is found, go to forbidden objects
			itemToDraw = additionalOffsetX > 0 && itemSize.Width == 0 ? L" - without " : L"without ";
			itemCount = 0;
			vsize = moveListTree[windowIndex].getForbiddenObjects().size();
			if (vsize > 0){
				for (u32 i = 0; i < vsize; ++i){
					std::string objectId = moveListTree[windowIndex].getForbiddenObjects()[i];
					if (movelistCharacterReference->getObjectByName(objectId) != NULL){
						std::string nameToAdd = movelistCharacterReference->getObjectByName(objectId)->displayName;
						if (movelistCharacterReference->getObjectByName(objectId)->canBeActivated()){
							if (nameToAdd == std::string()){
								nameToAdd = objectId;
							}
							if (itemCount > 0){
								itemToDraw += L", ";
							}
							itemToDraw += std::wstring(nameToAdd.begin(), nameToAdd.end());
							itemCount += 1;
						}
					}
				}
				if (itemCount > 0){
					textSize = notesFont->getDimension(itemToDraw.c_str());
					offsetY = (s32)std::floor(1.1 * nameSize.Height);
					offsetX = (s32)std::floor(0.2 * nameSize.Height) + additionalOffsetX;
					if (itemSize.Width > 0){
						itemToDraw = L" / " + itemToDraw;
						//offsetX += itemSize.Width;
					}
					destRect = core::rect<s32>(
						windowX + offsetX,
						windowY + offsetY,
						topLeftCornerX + offsetX + textSize.Width,
						windowX + offsetY + textSize.Height);
					fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
					additionalOffsetX += textSize.Width;
				}
			}
		}
		// check for pick-up requirements
		if (!moveListTree[windowIndex].getPickableItemId().empty() && !moveListTree[windowIndex].isMoveAvailableWithoutPickup()) {
			std::string objectId = moveListTree[windowIndex].getPickableItemId();
			std::string nameToAdd = movelistCharacterReference->getObjectByName(objectId)->displayName;
			if (movelistCharacterReference->getObjectByName(objectId)->canBeActivated()) {
				if (nameToAdd == std::string()) {
					nameToAdd = objectId;
				}
				itemToDraw = additionalOffsetX > 0 ? L" - " : std::wstring();
				itemToDraw += L"only near ";
				itemToDraw += std::wstring(nameToAdd.begin(), nameToAdd.end());
				textSize = notesFont->getDimension(itemToDraw.c_str());
				offsetY = (s32)std::floor(1.1 * nameSize.Height);
				offsetX = (s32)std::floor(0.2 * nameSize.Height) + additionalOffsetX;
				destRect = core::rect<s32>(
					windowX + offsetX,
					windowY + offsetY,
					topLeftCornerX + offsetX + textSize.Width,
					windowX + offsetY + textSize.Height);
				fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
			}
		}
		// draw input buffer
		s32 bufferLastX = drawInputBuffer(windowIndex, moveListTree[windowIndex].getInputStringLeft());
		// draw next item of the chain (if needed)
		if (moveListTree[windowIndex].isFollowupOnly() && moveListTree[windowIndex].getCopyOfFollowupMovesSet().size() > 0){
			std::string chainMoveName = moveListTree[windowIndex].getCopyOfFollowupMovesSet()[0].getName();
			std::replace(chainMoveName.begin(), chainMoveName.end(), '_', ' ');
			std::wstring itemToDraw = fk_addons::convertNameToNonASCIIWstring(chainMoveName);
			itemToDraw = L" [" + itemToDraw + L"]";
			//itemToDraw = std::wstring(chainMoveName.begin(), chainMoveName.end());
			textSize = notesFont->getDimension(itemToDraw.c_str());
			offsetY = (s32)std::floor(0.2 * textSize.Height);
			offsetX = 0;
			destRect = core::rect<s32>(
				bufferLastX,
				windowY - offsetY + windowSizeY - textSize.Height,
				bufferLastX + textSize.Width,
				windowY - offsetY + windowSizeY);
			fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
		} 
		else if (rootMovesBranches[windowIndex] > 0 && !demoOnly){
			std::wstring itemToDraw = std::to_wstring(rootMovesBranches[windowIndex]);
			itemToDraw = L" +" + itemToDraw;
			if (rootMovesBranches[windowIndex] > 1) {
				itemToDraw += L" chains";
			}else{
				itemToDraw += L" chain";
			}
			//itemToDraw = std::wstring(chainMoveName.begin(), chainMoveName.end());
			textSize = notesFont->getDimension(itemToDraw.c_str());
			offsetY = (s32)std::floor(0.2 * textSize.Height);
			offsetX = 0;
			destRect = core::rect<s32>(
				bufferLastX,
				windowY - offsetY + windowSizeY - textSize.Height,
				bufferLastX + textSize.Width,
				windowY - offsetY + windowSizeY);
			fk_addons::drawBorderedText(notesFont, itemToDraw, destRect);
		}
		if (!objectConditionFulfilled(moveListTree[windowIndex]) ||
			!bulletCounterConditionFulfilled(moveListTree[windowIndex]) ||
			(demoOnly && (moveListTree[windowIndex].getStance() != FK_Stance_Type::GroundStance &&
				moveListTree[windowIndex].getStance() != FK_Stance_Type::CrouchedStance))){
			core::rect<s32> winRect = core::rect<s32>(
				moveWindows[windowIndex]->getTopLeftCornerPosition().X,
				moveWindows[windowIndex]->getTopLeftCornerPosition().Y,
				moveWindows[windowIndex]->getTopLeftCornerPosition().X + moveWindows[windowIndex]->getWidth(),
				moveWindows[windowIndex]->getTopLeftCornerPosition().Y + moveWindows[windowIndex]->getHeight()
				);
			device->getVideoDriver()->draw2DRectangle(video::SColor(128, 96, 96, 96), winRect);
		}
	}

	// draw single icon
	void FK_MoveListPanel::drawIcon(video::ITexture* icon, s32 x, s32 y, s32 width, s32 height,
		bool useSmoothing){
		if (!icon){
			return;
		}
		// scale viewport
		core::dimension2d<u32> iconSize = icon->getSize();
		if (useSmoothing) {
			fk_addons::draw2DImage(device->getVideoDriver(), icon, 
				core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)width / iconSize.Width, (f32)height / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			device->getVideoDriver()->draw2DImage(icon, core::rect<s32>(x, y,
				x + width, y + height),
				core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, 0, true);
		}
	};

	// draw input buffer / returns last X used for an icon for chaining inputs
	s32 FK_MoveListPanel::drawInputBuffer(s32 index, std::deque<u32> buffer){
		u32 bufferSize = buffer.size();
		u32 iconSize = notesFont->getDimension(L"A").Height;
		u32 offset = 0.2 * iconSize;
		s32 windowX = topLeftCornerX;
		if (moveListTree[index].isFollowupOnly()){
			windowX = topLeftCornerX + (panelWidth - moveWindows[index]->getWidth());
		}
		s32 x1 = windowX + offset;
		s32 x2 = x1 + iconSize;
		s32 y2 = getWindowY(index) + moveWindows[index]->getHeight() - offset;
		s32 y1 = y2 - iconSize;
		s32 buttonCount = 0;
		for (u32 i = 0; i < bufferSize; ++i){
			buttonCount = 0;
			int button = buffer[i];
			if (button < 0)
				continue;
			if (button == FK_Input_Buttons::None) {
				x1 += iconSize / 4;
				continue;
			}
			u32 directionButton = button & (u32)FK_Input_Buttons::Any_Direction_Plus_Held;
			u32 punchButton = button & (u32)FK_Input_Buttons::Punch_Button;
			u32 kickButton = button & (u32)FK_Input_Buttons::Kick_Button;
			u32 techButton = button & (u32)FK_Input_Buttons::Tech_Button;
			u32 guardButton = button & (u32)FK_Input_Buttons::Guard_Button;
			if (buttonTexturesMap.count((FK_Input_Buttons)directionButton) > 0){
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)directionButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
			}
			if (buttonTexturesMap.count((FK_Input_Buttons)guardButton) > 0){
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)guardButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.count((FK_Input_Buttons)punchButton) > 0){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)punchButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.count((FK_Input_Buttons)kickButton) > 0){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)kickButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.count((FK_Input_Buttons)techButton) > 0){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)techButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
		}
		return x1;
	};

	// clear all windows
	void FK_MoveListPanel::clearWindows(){
		for each(FK_Window* window in moveWindows){
			delete window;
		}
		moveWindows.clear();
	};

	// comparator for sorting moves in vector
	bool compareMovesForSorting(FK_MoveListMove* m1, FK_MoveListMove* m2) {
		if ((u32)m1->getMove().getStance() < (u32)m2->getMove().getStance()){
			return true;
		}
		else if ((u32)m1->getMove().getStance() > (u32)m2->getMove().getStance()){
			return false;
		}
		else{
			u32 size1 = (u32)m1->getMove().getInputStringLeft().size();
			u32 size2 = (u32)m2->getMove().getInputStringLeft().size();
			u32 directionButton1 = 0;
			u32 actionButton1 = 0;
			u32 directionButton2 = 0;
			u32 actionButton2 = 0;
			for (s32 i = size1 - 1; i >= 0; --i){
				int button = m1->getMove().getInputStringLeft()[i];
				if (button < 0)
					continue;
				directionButton1 |= button & (u32)FK_Input_Buttons::Any_Direction_Plus_Held;
				actionButton1 |= button & (u32)FK_Input_Buttons::Any_NonDirectionButton;
			}
			for (s32 i = size2 - 1; i >= 0; --i){
				int button = m2->getMove().getInputStringLeft()[i];
				if (button < 0)
					continue;
				directionButton2 |= button & (u32)FK_Input_Buttons::Any_Direction_Plus_Held;
				actionButton2 |= button & (u32)FK_Input_Buttons::Any_NonDirectionButton;
			}
			if (actionButton1 < actionButton2){
				return true;
			}
			else if (actionButton1 > actionButton2){
				return false;
			}
			else{
				if (size1 < size2){
					return true;
				}
				else if (size1 > size2){
					return false;
				}
				else if (directionButton1 < directionButton2){
					return true;
				}
				else if (directionButton1 > directionButton2){
					return false;
				}
				else if (m1->getMove().getPriority() < m2->getMove().getPriority()) {
						return true;
				}
				else if (m1->getMove().getPriority() > m2->getMove().getPriority()) {
					return false;
				}
			}
		}
		return false;
	}

	void FK_MoveListPanel::parseMoves(FK_Character* character){
		for each(FK_Move tempMove in character->getMovesCollection()){
			FK_MoveListMove* newMoveForMoveList = new FK_MoveListMove();
			std::string keyForMap = tempMove.getName();
			if (moveMap.count(keyForMap) > 0){
				keyForMap += "_(alt.)";
			}
			tempMove.setName(keyForMap);
			newMoveForMoveList->setMove(tempMove);
			moveMap[keyForMap] = newMoveForMoveList;
			if (tempMove.isListedInMoveList() && objectMovelistInclusionConditionFulfilled(tempMove)){
				moveList.push_back(newMoveForMoveList);
			}
		}
		std::sort(moveList.begin(), moveList.end(), compareMovesForSorting);
		for each(FK_MoveListMove* tempMove in moveList){
			for each(FK_FollowupMove fwpMove in tempMove->getMove().getCopyOfFollowupMovesSet()){
				if (moveMap.count(fwpMove.getName()) > 0){
					if (moveMap[fwpMove.getName()]->getMove().isListedInMoveList() &&
						objectMovelistInclusionConditionFulfilled(moveMap[fwpMove.getName()]->getMove())){
						tempMove->addMoveToList(moveMap[fwpMove.getName()]);
					}
				}
			}
			for each(FK_FollowupMove fwpMove in tempMove->getMove().getCopyOfCancelIntoMovesSet()){
				if (moveMap.count(fwpMove.getName()) > 0){
					if (moveMap[fwpMove.getName()]->getMove().isListedInMoveList() &&
						objectMovelistInclusionConditionFulfilled(moveMap[fwpMove.getName()]->getMove())){
						tempMove->addMoveToList(moveMap[fwpMove.getName()]);
					}
				}
			}
		}
		// now, create the gigantic epileptic tree of possibilities ;)
		for each(FK_MoveListMove* tempMove in moveList){
			// 1st step: push root into the new tree
			if (!tempMove->getMove().isFollowupOnly()){
				FK_Move testMove = tempMove->getMove();
				// add input to show if the move is performed while running, crouching or jumping
				auto additionalBufferL = testMove.getInputStringLeft();
				auto additionalBufferR = testMove.getInputStringRight();
				if (testMove.getStance() == FK_Stance_Type::CrouchedStance) {
					additionalBufferL.push_front(FK_Input_Buttons::None);
					additionalBufferR.push_front(FK_Input_Buttons::None);
					additionalBufferL.push_front(FK_Input_Buttons::Down_Direction | FK_Input_Buttons::Guard_Button);
					additionalBufferR.push_front(FK_Input_Buttons::Down_Direction | FK_Input_Buttons::Guard_Button);
				}
				else if (testMove.getStance() == FK_Stance_Type::AirStance) {
					additionalBufferL.push_front(FK_Input_Buttons::None);
					additionalBufferR.push_front(FK_Input_Buttons::None);
					additionalBufferL.push_front(FK_Input_Buttons::UpRight_Direction | FK_Input_Buttons::Guard_Button);
					additionalBufferR.push_front(FK_Input_Buttons::UpLeft_Direction | FK_Input_Buttons::Guard_Button);
				}
				else if (testMove.getStance() == FK_Stance_Type::RunningStance) {
					additionalBufferL.push_front(FK_Input_Buttons::None);
					additionalBufferR.push_front(FK_Input_Buttons::None);
					additionalBufferL.push_front(FK_Input_Buttons::HeldRight_Direction);
					additionalBufferR.push_front(FK_Input_Buttons::HeldLeft_Direction);
					additionalBufferL.push_front(FK_Input_Buttons::Right_Direction);
					additionalBufferR.push_front(FK_Input_Buttons::Left_Direction);
				}
				testMove.setInputStringLeft(additionalBufferL);
				testMove.setInputStringRight(additionalBufferR);
				// insert root move at the end of the current list
				moveListTree.push_back(testMove);
				// set combo counter to 0
				// create the first sub-move
				s32 comboCounter = 0;
				FK_Move comboMove;
				comboMove.setStance(testMove.getStance());
				comboMove.setName(testMove.getName());
				comboMove.setDisplayName(testMove.getDisplayName());
				comboMove.setFollowUpOnly(true);
				comboMove.setNumberOfRequiredBulletCounters(testMove.getNumberOfRequiredBulletCounters());
				if (testMove.getTotalBulletDamage() > 0 && comboMove.getTotalBulletDamage() <= 0) {
					FK_Bullet dummyBullet = FK_Bullet();
					FK_Hitbox dummyHitbox = FK_Hitbox();
					dummyHitbox.setDamage(testMove.getTotalBulletDamage());
					dummyBullet.setHitbox(dummyHitbox);
					comboMove.addBullet(dummyBullet);
				}
				if (testMove.isImpactAttack()) {
					comboMove.setAsImpactAttack();
				}
				if (testMove.canBeFollowedOrCanceledOnHitOnly()) {
					comboMove.setFollowupOrCancelOnHitOnly();
				}
				for each (FK_MoveListMove* branchMove in tempMove->getFollowupMoves()){
					FK_Move comboMoveClone = comboMove;
					// create reference for avoiding infinite chains
					std::vector<std::string> parsedMoves;
					parsedMoves.push_back(tempMove->getMove().getName());
					// create the buffer to host the complete input
					std::vector<u32> buffer;
					for (u32 i = 0; i < testMove.getInputStringLeft().size(); ++i){
						buffer.push_back(testMove.getInputStringLeft()[i]);
					}
					if (!buffer.empty()) {
						buffer.push_back(FK_Input_Buttons::None);
					}
					for (s32 i = testMove.getStartingFrame(); i < testMove.getEndingFrame(); ++i){
						for each(auto pair in testMove.getToggledItemsAtFrame((u32)i)){
							std::string itemId = pair.first;
							FK_Move::FK_ObjectToggleFlag itemFlag = (FK_Move::FK_ObjectToggleFlag)pair.second;
							comboMoveClone.addToggledItemAtFrame(0, itemId, itemFlag);
						}
					}
					bool successfulAddonFlag = true;
					if (testMove.hasItemRequirements()){
						for each(std::string objectId in testMove.getRequiredObjects()){
							successfulAddonFlag &= comboMoveClone.addRequiredObjectId(objectId);
						}
						for each(std::string objectId in testMove.getForbiddenObjects()){
							successfulAddonFlag &= comboMoveClone.addForbiddenObjectId(objectId);
						}
					}
					if (successfulAddonFlag){
						// start following the chain (good luck!)
						fillTree(buffer, parsedMoves, branchMove, comboCounter, comboMoveClone);
					}
				}
			}
		}
		//... and, after that, store the index of every single root (i.e. non-followup) move (this helps visualization)
		u32 moveListSize = moveListTree.size();
		u32 counter = 0;
		u32 branchCounter = 0;
		s32 lastRootIndex = -1;
		for (u32 i = 0; i < moveListSize; ++i){
			FK_Move move = moveListTree[i];
			if (!move.isFollowupOnly()){
				rootMovesIndex[i] = counter;
				rootMovesBranches[i] = 0;
				lastRootIndex = i;
				counter++;
				branchCounter = 0;
			}else if(lastRootIndex >= 0){
				rootMovesBranches[lastRootIndex] += 1;
			}
		}
	};
	// fill the move list by recursively visiting all branches
	void FK_MoveListPanel::fillTree(std::vector<u32> buffer, std::vector<std::string> parsedMoves, 
		FK_MoveListMove* nextMove, s32& comboCounter, FK_Move comboMove){
		FK_Move moveToAdd = nextMove->getMove();
		std::string moveName = moveToAdd.getName();
		std::string moveNameToDisplay = moveToAdd.getDisplayName();
		bool closeBranch = false;
		bool repetition = false;
		bool successfulAddonFlag = true;
		if (moveToAdd.hasItemRequirements()){
			for each(std::string objectId in moveToAdd.getRequiredObjects()){
				successfulAddonFlag &= comboMove.addRequiredObjectId(objectId);
			}
			for each(std::string objectId in moveToAdd.getForbiddenObjects()){
				successfulAddonFlag &= comboMove.addForbiddenObjectId(objectId);
			}
		}
		if (moveToAdd.getNumberOfRequiredBulletCounters() > 0) {
			comboMove.setNumberOfRequiredBulletCounters(
				comboMove.getNumberOfRequiredBulletCounters() + moveToAdd.getNumberOfRequiredBulletCounters()
			);
		}
		if (std::find(parsedMoves.begin(), parsedMoves.end(), moveName) != parsedMoves.end() || !successfulAddonFlag){
			closeBranch = true;
			repetition = true;
		}
		else{
			std::vector<u32> buffer2;
			for (u32 i = 0; i < moveToAdd.getInputStringLeft().size(); ++i){
				buffer2.push_back(moveToAdd.getInputStringLeft()[i]);
			}
			if (!buffer2.empty()) {
				buffer2.push_back(FK_Input_Buttons::None);
			}
			buffer.insert(buffer.end(), buffer2.begin(),
				buffer2.end());
			parsedMoves.push_back(moveToAdd.getName());
			for (int i = moveToAdd.getStartingFrame(); i < moveToAdd.getEndingFrame(); ++i){
				for each(auto pair in moveToAdd.getToggledItemsAtFrame((u32)i)){
					std::string itemId = pair.first;
					FK_Move::FK_ObjectToggleFlag itemFlag = (FK_Move::FK_ObjectToggleFlag)pair.second;
					comboMove.addToggledItemAtFrame(0, itemId, itemFlag);
				}
			}
		}
		if (moveToAdd.getTotalBulletDamage() > 0 && comboMove.getTotalBulletDamage() <= 0) {
			FK_Bullet dummyBullet = FK_Bullet();
			FK_Hitbox dummyHitbox = FK_Hitbox();
			dummyHitbox.setDamage(moveToAdd.getTotalBulletDamage());
			dummyBullet.setHitbox(dummyHitbox);
			comboMove.addBullet(dummyBullet);
		}
		if (moveToAdd.isImpactAttack()) {
			comboMove.setAsImpactAttack();
		}
		if (moveToAdd.canBeFollowedOrCanceledOnHitOnly()) {
			comboMove.setFollowupOrCancelOnHitOnly();
		}
		bool moveIsRootMove = !(nextMove->getMove().isFollowupOnly());
		if (closeBranch || moveIsRootMove || nextMove->getFollowupMoves().size() == 0){
			// increase combo counter
			comboCounter += 1;
			// create summary move
			std::string newMoveName = comboMove.getDisplayName() + " - Chain #" + std::to_string(comboCounter);
			comboMove.setName(newMoveName);
			comboMove.setDisplayName(newMoveName);
			std::deque<u32> queuedBuffer(buffer.begin(), buffer.end());
			comboMove.setInputStringLeft(queuedBuffer);
			if (moveIsRootMove && nextMove->getFollowupMoves().size() > 0 && !repetition){
				std::vector<FK_FollowupMove> lastMoveForChaining_Container;
				FK_FollowupMove lastMoveForChaining;
				lastMoveForChaining.setName(moveNameToDisplay);
				lastMoveForChaining_Container.push_back(lastMoveForChaining);
				comboMove.setFollowupMovesSet(lastMoveForChaining_Container);
			}
			moveListTree.push_back(comboMove);
		}
		else{
			for each (FK_MoveListMove* branchMove in nextMove->getFollowupMoves()){
				bool successfulAddonFlag = true;
				FK_Move tempMove = branchMove->getMove();
				FK_Move comboMoveClone = comboMove;
				moveName = tempMove.getName();
				if (tempMove.hasItemRequirements()){
					for each(std::string objectId in tempMove.getRequiredObjects()){
						successfulAddonFlag &= comboMoveClone.addRequiredObjectId(objectId);
						if (!successfulAddonFlag) break;
					}
					for each(std::string objectId in tempMove.getForbiddenObjects()){
						successfulAddonFlag &= comboMoveClone.addForbiddenObjectId(objectId);
						if (!successfulAddonFlag) break;
					}
				}
				if (std::find(parsedMoves.begin(), parsedMoves.end(), moveName) != parsedMoves.end()){
					successfulAddonFlag = false;
				}
				if (successfulAddonFlag){
					fillTree(buffer, parsedMoves, branchMove, comboCounter, comboMove);
				}
			}
		}
	}

	bool FK_MoveListPanel::bulletCounterConditionFulfilled(FK_Move& move) {
		return movelistCharacterReference->getTriggerCounters() >= move.getNumberOfRequiredBulletCounters();
	}

	bool FK_MoveListPanel::hasSelectedRootMove()
	{
		return selectionFlag;
	}

	FK_Move FK_MoveListPanel::getSelectedMove()
	{
		return demoMove;
	}

	bool FK_MoveListPanel::objectConditionFulfilled(FK_Move& move){
		/* if the move has object requirements, check them before proceeding */
		bool moveAvailable = true;
		if (move.hasItemRequirements()){
			u32 count = movelistCharacterReference->getCharacterAdditionalObjects().size();
			for (u32 i = 0; i < count; ++i){
				std::string nameTag = movelistCharacterReference->getCharacterAdditionalObjects().at(i).uniqueNameId;
				if (nameTag != std::string()){
					if (move.isObjectRequired(nameTag)){
						moveAvailable &= movelistCharacterReference->getCharacterAdditionalObjects().at(i).isActive() ||
							(!movelistCharacterReference->getCharacterAdditionalObjects().at(i).broken && 
							move.canShowItem(nameTag)) || move.canRestoreItem(nameTag);
					}
					else if (move.isObjectForbidden(nameTag)){
						moveAvailable &= !movelistCharacterReference->getCharacterAdditionalObjects().at(i).isActive() ||
							(!movelistCharacterReference->getCharacterAdditionalObjects().at(i).broken &&
							move.canHideItem(nameTag)) || move.canBreakItem(nameTag);
					}
				}
				if (!moveAvailable){
					return false;
				}
			}
			if (move.getPickableItemId() != std::string() &&
				!move.canPickUpItem(movelistCharacterReference->getPickableObjectId()) &&
				!move.isMoveAvailableWithoutPickup()) {
				moveAvailable = false;
			}
		}
		return moveAvailable;
	}

	bool FK_MoveListPanel::objectMovelistInclusionConditionFulfilled(FK_Move& move){
		/* if the move has object requirements, check them before proceeding */
		bool moveAvailable = true;
		if (move.hasItemRequirements()) {
			u32 countForbidden = move.getForbiddenObjects().size();
			u32 countRequired = move.getRequiredObjects().size();
			for (u32 i = 0; i < countRequired + countForbidden; ++i) {
				if (i < countRequired) {
					moveAvailable &= (movelistCharacterReference->getObjectByName(move.getRequiredObjects().at(i)) != NULL &&
						movelistCharacterReference->getObjectByName(move.getRequiredObjects().at(i))->canBeActivated());
				}
				else {
					u32 k = i - countRequired;
					moveAvailable &= (movelistCharacterReference->getObjectByName(move.getForbiddenObjects().at(k)) != NULL &&
						movelistCharacterReference->getObjectByName(move.getForbiddenObjects().at(k))->canBeRemoved());
				}
				if (!moveAvailable) {
					break;
				}
			}
		}
		/*bool moveAvailable = true;
		if (move.hasItemRequirements()){
			u32 count = movelistCharacterReference->getCharacterAdditionalObjects().size();
			for (u32 i = 0; i < count; ++i){
				std::string nameTag = movelistCharacterReference->getCharacterAdditionalObjects().at(i).uniqueNameId;
				if (nameTag != std::string()){
					if (move.isObjectForbidden(nameTag)){
						moveAvailable &= (movelistCharacterReference->getCharacterAdditionalObjects().at(i).canBeRemoved());
					}
					if (move.isObjectRequired(nameTag) && !movelistCharacterReference->getCharacterAdditionalObjects().at(i).isActive()){
						moveAvailable &= movelistCharacterReference->getCharacterAdditionalObjects().at(i).canBeActivated();
					}
				}
				if (!moveAvailable){
					return false;
				}
			}
		}*/
		return moveAvailable;
	}

	s32 FK_MoveListPanel::getWindowY(s32 windowIndex){
		s32 y0 = 0;
		u32 drawingIndex = windowIndex;
		u32 drawingMainIndex = moveIndex;
		if (showRootMovesOnly && moveListTree[windowIndex].isFollowupOnly()) {
			if (windowIndex > rootMoveSelectedIndex && 
				(nextRootMoveSelectedIndex == 0 || windowIndex < nextRootMoveSelectedIndex)) {
				drawingIndex = rootMovesIndex[rootMoveSelectedIndex] + (windowIndex - rootMoveSelectedIndex);
				if (moveIndex >= rootMoveSelectedIndex &&
					(nextRootMoveSelectedIndex == 0 || moveIndex < nextRootMoveSelectedIndex)) {
					drawingMainIndex = rootMovesIndex[rootMoveSelectedIndex] + (moveIndex - rootMoveSelectedIndex);
				}
				else {
					if (moveIndex >= nextRootMoveSelectedIndex && nextRootMoveSelectedIndex > 0) {
						drawingMainIndex = rootMovesIndex[moveIndex] + rootMovesBranches[rootMoveSelectedIndex];
					}
					else {
						drawingMainIndex = rootMovesIndex[moveIndex];
					}
				}
			}
		}
		else if (showRootMovesOnly && rootMoveSelectedIndex >= 0) {
			if (moveIndex >= rootMoveSelectedIndex &&
				(nextRootMoveSelectedIndex == 0 || moveIndex < nextRootMoveSelectedIndex)) {
				drawingMainIndex = rootMovesIndex[rootMoveSelectedIndex] + (moveIndex - rootMoveSelectedIndex);
			}
			else {
				if (moveIndex >= nextRootMoveSelectedIndex  && nextRootMoveSelectedIndex > 0) {
					drawingMainIndex = rootMovesIndex[moveIndex] + rootMovesBranches[rootMoveSelectedIndex];
				}else{
					drawingMainIndex = rootMovesIndex[moveIndex];
				}
			}
			if (windowIndex > rootMoveSelectedIndex) {
				drawingIndex = rootMovesIndex[windowIndex] + rootMovesBranches[rootMoveSelectedIndex];
			}else{
				drawingIndex = rootMovesIndex[windowIndex];
			}
		}
		else if (showRootMovesOnly) {
			drawingIndex = rootMovesIndex[windowIndex];
			drawingMainIndex = rootMovesIndex[moveIndex];
		}
		if (drawingMainIndex > 2){
			y0 = topLeftCornerY + windowSizeY * (drawingIndex - (drawingMainIndex - 2));
		}
		else{
			y0 = topLeftCornerY + windowSizeY * drawingIndex;
		}
		return y0;
	}

	s32 FK_MoveListPanel::getFirstWindowX(){
		return topLeftCornerX;
	};
	s32 FK_MoveListPanel::getFirstWindowY(){
		return topLeftCornerY;
	};
	bool FK_MoveListPanel::isWindowVisible(s32 windowIndex){
		s32 windowY = getWindowY(windowIndex);
		bool toBeDrawn = windowY < (topLeftCornerY + panelHeight - 10);
		toBeDrawn &= windowY >= topLeftCornerY;
		return toBeDrawn;
	};
	bool FK_MoveListPanel::isActive(){
		return active_flag;
	}
	bool FK_MoveListPanel::isVisible(){
		return visible_flag;
	}
	void FK_MoveListPanel::setActive(bool new_active_flag){
		active_flag = new_active_flag;
	}
	void FK_MoveListPanel::setVisible(bool new_visible_flag){
		visible_flag = new_visible_flag;
	}

	void FK_MoveListPanel::increaseCursor(bool playSound){
		inputDelayMS = 0;
		if (playSound) {
			soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		}
		moveIndex += 1;
		moveIndex %= moveWindows.size();
	};

	void FK_MoveListPanel::decreaseCursor(bool playSound){
		inputDelayMS = 0;
		if (playSound) {
			soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		}
		moveIndex += moveWindows.size() - 1;
		moveIndex %= moveWindows.size();
	};


	void FK_MoveListPanel::toNextRootMove(bool playSound){
		increaseCursor(playSound);
		while (moveListTree[moveIndex].isFollowupOnly()){
			moveIndex += 1;
			moveIndex %= moveWindows.size();
		}
	};

	void FK_MoveListPanel::toPreviousRootMove(bool playSound){
		decreaseCursor(playSound);
		while (moveListTree[moveIndex].isFollowupOnly()){
			moveIndex += moveWindows.size() - 1;
			moveIndex %= moveWindows.size();
		}
	};

	void FK_MoveListPanel::closeMoveList(){
		inputDelayMS = 0;
		soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
		closeMoveList_flag = true;
	};

	void FK_MoveListPanel::toggleBranchMovesVisibility(){
		inputDelayMS = 0;
		soundManager->playSound("cursor", 100.f * gameOptions->getSFXVolume());
		if (showRootMovesOnly){
			showRootMovesOnly = false;
			rootMoveSelectedIndex = -1;
			nextRootMoveSelectedIndex = -1;
		}
		else{
			showRootMovesOnly = true;
			rootMoveSelectedIndex = -1;
			nextRootMoveSelectedIndex = -1;
			if (moveListTree[moveIndex].isFollowupOnly()){
				toPreviousRootMove(false);
			}
		}
	}

	bool FK_MoveListPanel::hasToBeClosed(){
		return closeMoveList_flag;
	}

	void FK_MoveListPanel::reset(bool resetIndex){
		selectionFlag = false;
		closeMoveList_flag = false;
		if (resetIndex){
			moveIndex = 0;
		}
	};

	void FK_MoveListPanel::resetSelectionTime() {
		now = device->getTimer()->getRealTime();
		then = now;
	}

	void FK_MoveListPanel::selectMoveForDemo() {
		inputDelayMS = 0;
		if (!objectConditionFulfilled(moveListTree[moveIndex]) ||
			!bulletCounterConditionFulfilled(moveListTree[moveIndex]) ||
			(moveListTree[moveIndex].getStance() != FK_Stance_Type::GroundStance &&
				moveListTree[moveIndex].getStance() != FK_Stance_Type::CrouchedStance)) {
			soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
			return;
		}
		else {
			selectionFlag = true;
			soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
			demoMove = moveListTree[moveIndex];
			closeMoveList_flag = true;
			setActive(false);
			setVisible(false);
		}
	}

	void FK_MoveListPanel::selectItem(){
		inputDelayMS = 0;
		s32 oldSelectedRoot = rootMoveSelectedIndex;
		s32 oldNextRoot = nextRootMoveSelectedIndex;
		if (rootMoveSelectedIndex == moveIndex) {
			selectionFlag = false;
		}else{
			selectionFlag = true;
		}
		if (!selectionFlag) {
			rootMoveSelectedIndex = -1;
			nextRootMoveSelectedIndex = -1;
		}else{
			rootMoveSelectedIndex = moveIndex;
			nextRootMoveSelectedIndex = rootMoveSelectedIndex + 1;
			nextRootMoveSelectedIndex %= moveWindows.size();
			while (moveListTree[nextRootMoveSelectedIndex].isFollowupOnly()) {
				nextRootMoveSelectedIndex += 1;
				nextRootMoveSelectedIndex %= moveWindows.size();
			}
		}
		if (selectionFlag && rootMovesBranches[rootMoveSelectedIndex] == 0) {
			soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
			rootMoveSelectedIndex = oldSelectedRoot;
			nextRootMoveSelectedIndex = oldNextRoot;
			selectionFlag = false;
		}else{
			soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
		}
	};

	void FK_MoveListPanel::cancelSelection() {
		//if (selectionFlag) {
		//	inputDelayMS = 0;
		//	soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		//	rootMoveSelectedIndex = -1;
		//	nextRootMoveSelectedIndex = -1;
		//	selectionFlag = false;
		//	if (moveListTree[moveIndex].isFollowupOnly()) {
		//		toPreviousRootMove(false);
		//	}
		//}
		//else {
			inputDelayMS = 0;
			closeMoveList();
		//}
	}

	void FK_MoveListPanel::loadTextures(){
		// keyboard button texture
		keyboardButtonTexture = device->getVideoDriver()->getTexture((resourcePath + "keyboard\\empty2.png").c_str());
		// joypad buttons textures
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_A_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Abutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_X_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Xbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Y_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Ybutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_B_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Bbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L1_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L1button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L2_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L2button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L3_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L3button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R1_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R1button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R2_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R2button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R3_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R3button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Select_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\backbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Start_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\startbutton.png").c_str());
		// button textures
		buttonTexturesMap[FK_Input_Buttons::Kick_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "kick_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Punch_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "punch_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Guard_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "guard_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Tech_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "tech_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Up_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_up.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldUp_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::UpRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upright.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldUpRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_uprightH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Right_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_right.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_rightH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::DownRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downright.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldDownRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downrightH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Down_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_down.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldDown_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::DownLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downleft.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldDownLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downleftH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Left_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_left.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_leftH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::UpLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upleft.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldUpLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upleftH.png").c_str());
		// attack type
		/*hitTypeTextureMap[FK_Attack_Type::HighAtk] = 
			device->getVideoDriver()->getTexture((resourcePath + "indicator_high.png").c_str());
		hitTypeTextureMap[FK_Attack_Type::MediumAtk] =
			device->getVideoDriver()->getTexture((resourcePath + "indicator_mid.png").c_str());
		hitTypeTextureMap[FK_Attack_Type::SpecialMediumAtk] =
			device->getVideoDriver()->getTexture((resourcePath + "indicator_mid.png").c_str());
		hitTypeTextureMap[FK_Attack_Type::LowAtk] =
			device->getVideoDriver()->getTexture((resourcePath + "indicator_low.png").c_str());
		hitTypeTextureMap[FK_Attack_Type::SpecialLowAtk] =
			device->getVideoDriver()->getTexture((resourcePath + "indicator_low.png").c_str());
		hitTypeTextureMap[FK_Attack_Type::ThrowAtk] =
			device->getVideoDriver()->getTexture((resourcePath + "indicator_throw.png").c_str());*/
		plusSignTex = device->getVideoDriver()->getTexture((resourcePath + "plus_sign.png").c_str());
		impactIconTex = device->getVideoDriver()->getTexture((resourcePath + "impact_icon.png").c_str());
		projectileIconTex = device->getVideoDriver()->getTexture((resourcePath + "projectile_icon.png").c_str());
	};

}