#include"FK_SceneInputMapping.h"
#include"ExternalAddons.h"
#include<Windows.h>
#include<iostream>

namespace fk_engine{
	FK_SceneInputMapping::FK_SceneInputMapping() : FK_SceneWithInput(){
		initialize();
	};
	FK_SceneInputMapping::FK_SceneInputMapping(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, 
		FK_Options *newOptions) : FK_SceneWithInput(newDevice, joypadInfo, newOptions){
		initialize();
		setup(newDevice, joypadInfo, newOptions);
	};
	void FK_SceneInputMapping::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, 
		FK_Options* newOptions){
		FK_SceneWithInput::setup(newDevice, joypadInfo, newOptions);
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper();
		setupSoundManager();
		setupGraphics();
		setupExitMenu();
		initializeAcceptedKeyboardButtonsMap();
		initializeButtonArrays();
	};
	
	void FK_SceneInputMapping::saveSettings(){
		
	};

	// initialize
	void FK_SceneInputMapping::initialize(){
		itemSelected = false;
		endScene = false;
		menuIndex = 0;
		oldInputButton = KEY_KEY_CODES_COUNT;
		lastInputTime = 0;
		menuOptionsStrings = std::vector<std::string>();
		menuOptionsCaptions = std::vector<std::string>();
		menuActiveItems = std::vector<bool>();
		modifiedInputMap = std::vector<std::map<FK_Input_Buttons, u32> >();
		/* time */
		then = 0;
		now = 0;
		delta_t_ms = 0;
		/* caption movement */
		captionStillPeriodThresholdMs = 1000;
		setNextScene(FK_SceneType::SceneOptions);
	}

	// initialize map of accepted keyboard inputs
	void FK_SceneInputMapping::initializeAcceptedKeyboardButtonsMap(){
		// directions
		acceptedKeys[EKEY_CODE::KEY_UP] = "ARW UP";
		acceptedKeys[EKEY_CODE::KEY_DOWN] = "ARW DOWN";
		acceptedKeys[EKEY_CODE::KEY_LEFT] = "ARW LEFT";
		acceptedKeys[EKEY_CODE::KEY_RIGHT] = "ARW RIGHT";

		// letters and numbers
		std::vector <std::string> letters = {
			"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
			"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
			"U", "V", "W", "X", "Y", "Z"
		};
		std::vector <std::string> numbers = {
			"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		};
		for (u32 i = (u32)(EKEY_CODE::KEY_KEY_A); i <= (u32)(EKEY_CODE::KEY_KEY_Z); ++i){
			acceptedKeys[(EKEY_CODE)i] = letters[i - (u32)(EKEY_CODE::KEY_KEY_A)];
		}
		for (u32 i = (u32)(EKEY_CODE::KEY_KEY_0); i <= (u32)(EKEY_CODE::KEY_KEY_9); ++i){
			acceptedKeys[(EKEY_CODE)i] = numbers[i - (u32)(EKEY_CODE::KEY_KEY_0)];
		}

		// additional signs
		acceptedKeys[EKEY_CODE::KEY_PLUS] = "+";  // PLUS key
		acceptedKeys[EKEY_CODE::KEY_MINUS] = "-";  // MINUS key
		acceptedKeys[EKEY_CODE::KEY_COMMA] = ",";  // COMMA key
		acceptedKeys[EKEY_CODE::KEY_PERIOD] = ".";  // PERIOD key

		// function buttons
		acceptedKeys[EKEY_CODE::KEY_BACK] = "BACKSPACE";  // BACKSPACE key
		acceptedKeys[EKEY_CODE::KEY_TAB] = "TAB";  // TAB key
		acceptedKeys[EKEY_CODE::KEY_CLEAR] = "CLEAR";  // CLEAR key
		acceptedKeys[EKEY_CODE::KEY_RETURN] = "ENTER";  // ENTER key
		acceptedKeys[EKEY_CODE::KEY_LSHIFT] = "L SHIFT";  // SHIFT key
		acceptedKeys[EKEY_CODE::KEY_LCONTROL] = "L CTRL";  // CTRL key
		acceptedKeys[EKEY_CODE::KEY_LMENU] = "L ALT";  // ALT key
		acceptedKeys[EKEY_CODE::KEY_RSHIFT] = "R SHIFT";  // SHIFT key
		acceptedKeys[EKEY_CODE::KEY_RCONTROL] = "R CTRL";  // CTRL key
		acceptedKeys[EKEY_CODE::KEY_RMENU] = "R ALT";  // ALT key
		acceptedKeys[EKEY_CODE::KEY_PAUSE] = "PAUSE";  // PAUSE key
		acceptedKeys[EKEY_CODE::KEY_INSERT] = "INS";  // INSERT key
		acceptedKeys[EKEY_CODE::KEY_DELETE] = "DEL";  // DELETE key
		acceptedKeys[EKEY_CODE::KEY_SPACE] = "SPACE";  // SPACE key
		acceptedKeys[EKEY_CODE::KEY_PRIOR] = "PG UP";  // PG UP key
		acceptedKeys[EKEY_CODE::KEY_NEXT] = "PG DOWN";  // PG DOWN key
		acceptedKeys[EKEY_CODE::KEY_END] = "END";  // END key
		acceptedKeys[EKEY_CODE::KEY_HOME] = "HOME";  // HOME key
		acceptedKeys[EKEY_CODE::KEY_CAPITAL] = "CAPS LOCK"; // CAPS LOCK key
		acceptedKeys[EKEY_CODE::KEY_ESCAPE] = "ESC"; //ESCAPE key

		// numpad
		std::vector <std::string> numpad = {
			"NUM 0","NUM 1","NUM 2","NUM 3","NUM 4","NUM 5","NUM 6","NUM 7","NUM 8","NUM 9",
			"NUM *", "NUM +", "NUM .", "NUM -", "NUM ,", "NUM /"
		};
		for (u32 i = (u32)(EKEY_CODE::KEY_NUMPAD0); i <= (u32)(EKEY_CODE::KEY_DIVIDE); ++i){
			acceptedKeys[(EKEY_CODE)i] = numpad[i - (u32)(EKEY_CODE::KEY_NUMPAD0)];
		}

		//OEM keys
		acceptedKeys[EKEY_CODE::KEY_OEM_1] = "OEM 1 (;:)";
		acceptedKeys[EKEY_CODE::KEY_OEM_2] = "OEM 2 (/?)";
		acceptedKeys[EKEY_CODE::KEY_OEM_3] = "OEM 3 (~)";
		acceptedKeys[EKEY_CODE::KEY_OEM_4] = "OEM 4 ([{)";
		acceptedKeys[EKEY_CODE::KEY_OEM_5] = "OEM 5 (\|)";
		acceptedKeys[EKEY_CODE::KEY_OEM_6] = "OEM 6 (]})";
		acceptedKeys[EKEY_CODE::KEY_OEM_7] = "OEM 7 ('\")";
		acceptedKeys[EKEY_CODE::KEY_OEM_8] = "OEM 8";
		acceptedKeys[EKEY_CODE::KEY_OEM_102] = "OEM 9 (<>)";

		//F keys
		acceptedKeys[EKEY_CODE::KEY_F1] = "F1";
		acceptedKeys[EKEY_CODE::KEY_F2] = "F2";
		acceptedKeys[EKEY_CODE::KEY_F3] = "F3";
		acceptedKeys[EKEY_CODE::KEY_F4] = "F4";
		acceptedKeys[EKEY_CODE::KEY_F5] = "F5";
		acceptedKeys[EKEY_CODE::KEY_F6] = "F6";
		acceptedKeys[EKEY_CODE::KEY_F7] = "F7";
		acceptedKeys[EKEY_CODE::KEY_F8] = "F8";
		acceptedKeys[EKEY_CODE::KEY_F9] = "F9";
		acceptedKeys[EKEY_CODE::KEY_F10] = "F10";
		acceptedKeys[EKEY_CODE::KEY_F11] = "F11";
		acceptedKeys[EKEY_CODE::KEY_F12] = "F12";
		acceptedKeys[EKEY_CODE::KEY_F13] = "F13";
		acceptedKeys[EKEY_CODE::KEY_F14] = "F14";
		acceptedKeys[EKEY_CODE::KEY_F15] = "F15";
		acceptedKeys[EKEY_CODE::KEY_F16] = "F16";
		acceptedKeys[EKEY_CODE::KEY_F17] = "F17";
		acceptedKeys[EKEY_CODE::KEY_F18] = "F18";
		acceptedKeys[EKEY_CODE::KEY_F19] = "F19";
		acceptedKeys[EKEY_CODE::KEY_F20] = "F20";
		acceptedKeys[EKEY_CODE::KEY_F21] = "F21";
		acceptedKeys[EKEY_CODE::KEY_F22] = "F22";
		acceptedKeys[EKEY_CODE::KEY_F23] = "F23";
		acceptedKeys[EKEY_CODE::KEY_F24] = "F24";
	}

	// initialize map of accepted keyboard inputs
	void FK_SceneInputMapping::initializeJoypadButtonsMap(){
		// directions
		joypadKeys[FK_JoypadInput::JoypadInputUp] = "D-PAD UP";
		joypadKeys[FK_JoypadInput::JoypadInputDown] = "D-PAD DOWN";
		joypadKeys[FK_JoypadInput::JoypadInputLeft] = "D-PAD LEFT";
		joypadKeys[FK_JoypadInput::JoypadInputRight] = "D-PAD RIGHT";
		joypadKeys[FK_JoypadInput::JoypadInput_A_Button] = "A";
		joypadKeys[FK_JoypadInput::JoypadInput_B_Button] = "B";
		joypadKeys[FK_JoypadInput::JoypadInput_X_Button] = "X";
		joypadKeys[FK_JoypadInput::JoypadInput_Y_Button] = "Y";
		joypadKeys[FK_JoypadInput::JoypadInput_R1_Button] = "R1";
		joypadKeys[FK_JoypadInput::JoypadInput_L1_Button] = "L1";
		joypadKeys[FK_JoypadInput::JoypadInput_R2_Button] = "R2";
		joypadKeys[FK_JoypadInput::JoypadInput_L2_Button] = "L2";
		joypadKeys[FK_JoypadInput::JoypadInput_Home_Button] = "HOME";
		joypadKeys[FK_JoypadInput::JoypadInput_L3_Button] = "L3";
		joypadKeys[FK_JoypadInput::JoypadInput_R3_Button] = "R3";
		joypadKeys[FK_JoypadInput::JoypadInput_Start_Button] = "START";
		joypadKeys[FK_JoypadInput::JoypadInput_Select_Button] = "BACK";
	}

	// initialize button arrays
	void FK_SceneInputMapping::initializeButtonArrays(){
		// initialize array of buttons to assign
		directionButtons = {
			FK_Input_Buttons::Up_Direction,
			FK_Input_Buttons::Down_Direction,
			FK_Input_Buttons::Left_Direction,
			FK_Input_Buttons::Right_Direction,
		};
		menuButtons = {
			FK_Input_Buttons::Selection_Button,
			FK_Input_Buttons::Cancel_Button,
			FK_Input_Buttons::Modifier_Button,
			FK_Input_Buttons::Help_Button,
			FK_Input_Buttons::ScrollLeft_Button,
			FK_Input_Buttons::ScrollRight_Button,
			FK_Input_Buttons::Confirmation_Button,
		};
		gameButtons = {
			FK_Input_Buttons::Punch_Button,
			FK_Input_Buttons::Kick_Button,
			FK_Input_Buttons::Tech_Button,
			FK_Input_Buttons::Guard_Button,
			FK_Input_Buttons::Menu_Pause_Button,
			FK_Input_Buttons::Trigger_Button,
			FK_Input_Buttons::Trigger_plus_Guard_Button,
			FK_Input_Buttons::Punch_plus_Kick_Button,
			FK_Input_Buttons::Tech_plus_Punch_Button,
			FK_Input_Buttons::Tech_plus_Kick_Button,
			FK_Input_Buttons::Punch_plus_Guard_Button,
			FK_Input_Buttons::Kick_plus_Guard_Button,
			FK_Input_Buttons::Tech_plus_Guard_Button,
			FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button,
			FK_Input_Buttons::All_Action_Buttons,
		};
		// create option strings as well
		menuOptionsStrings = {
			"UP",
			"DOWN",
			"LEFT",
			"RIGHT",
			"SELECT",
			"CANCEL",
			"TOGGLE / OUTFIT",
			"HELP / RESET TRAINING",
			"SCROLL LEFT",
			"SCROLL RIGHT",
			"CONFIRM",
			"PUNCH",
			"KICK",
			"TECH",
			"GUARD",
			"PAUSE/MENU",
			"TRIGGER",
			"TRIGGER GUARD",
			"P + K",
			"P + T",
			"K + T",
			"P + G",
			"K + G",
			"T + G",
			"P + K + T",
			"P + K + T + G",
		};
	}

	// setup graphics
	void FK_SceneInputMapping::setupGraphics(){
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		submenufont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		captionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		/* textures */
		menuBackgroundTexture = driver->getTexture((commonResourcesPath + "common_menu_items\\menuBackground_base.png").c_str());
		menuCaptionOverlay = driver->getTexture((commonResourcesPath + "common_menu_items\\menu_CaptionOverlay.png").c_str());
		arrowUp_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowU.png").c_str());
		arrowDown_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowD.png").c_str());
		arrowFrequency = 500;
		// load joypad settings textures
		loadJoypadSetupTextures();
	}

	// get button from index
	FK_Input_Buttons FK_SceneInputMapping::getButtonFromIndex(u32 itemIndex){
		u32 tempIndex = itemIndex;
		if (itemIndex >= directionButtons.size() + menuButtons.size()){
			tempIndex -= directionButtons.size() + menuButtons.size();
			return gameButtons[tempIndex];
		}
		else if (itemIndex >= directionButtons.size()){
			tempIndex -= directionButtons.size();
			return menuButtons[tempIndex];
		}
		else{
			return directionButtons[itemIndex];
		}
	}

	// draw item
	void FK_SceneInputMapping::drawItem(u32 itemIndex){

	}

	void FK_SceneInputMapping::drawArrows(){
		f32 phase = 2 * core::PI * (f32)(now % arrowFrequency) / arrowFrequency;
		s32 addonY = floor(10 * asin(cos(phase)) / core::PI);
		u32 arrowSize = 16;
		if (menuIndex > 5){
			s32 x1 = (screenSize.Width - arrowSize * scale_factorX) / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y1 = (54 + addonY) * scale_factorY;
			s32 y2 = y1 + arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowUp_tex->getSize().Width, arrowUp_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowUp_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
		if (menuIndex < menuOptionsStrings.size() - 5){
			s32 x1 = (screenSize.Width - arrowSize * scale_factorX) / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y2 = screenSize.Height - (60 + addonY) * scale_factorY;
			s32 y1 = y2 - arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowDown_tex->getSize().Width, arrowDown_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowDown_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
	}

	void FK_SceneInputMapping::drawBackground(){
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
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(menuBackgroundTexture, destinationRect, sourceRect, 0, vertexColors_background);
	}

	void FK_SceneInputMapping::drawCaptions(){
		core::dimension2d<u32> backgroundSize = menuCaptionOverlay->getSize();
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, backgroundSize.Width, backgroundSize.Height);
		// create color array for caption overlay
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
		std::wstring itemString = L"Browse the inputs with UP or DOWN, toggle player with LEFT or RIGHT, ";
		itemString += L"select the key you want to change, then press the new input. ";
		itemString += L"By pressing the same key already displayed, the input will be set to UNASSIGNED ";
		itemString += L"(notice that this works only for the additional game hotkeys - the mandatory buttons cannot be left blank). ";
		itemString += L"Once you are done, you can validate the changes with CONFIRM ";
		itemString += L"or restore the previous configuration by pressing CANCEL";
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

	// draw items
	void FK_SceneInputMapping::drawItems(){
		for (u32 i = 0; i < menuOptionsStrings.size(); ++i){
			drawItem(i);
		}
	}

	void FK_SceneInputMapping::drawTitle(){
		
	}
	
	// draw all
	void FK_SceneInputMapping::drawAll(bool showBackground){
		
	}

	// dispose
	void FK_SceneInputMapping::dispose(){
		itemSelected = false;
		endScene = false;
		menuIndex = 0;
		menuOptionsStrings.clear();
		menuActiveItems.clear();
		menuOptionsCaptions.clear();
		modifiedInputMap.clear();
		delete exitMenu;
		delete soundManager;
		FK_SceneWithInput::dispose(false);
	}

	void FK_SceneInputMapping::update() {
		update(true);
	}

	// update
	void FK_SceneInputMapping::update(bool drawScene){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		// if this is the first update cycle, play "select character" voice and the BGM
		if (cycleId == 0){
			cycleId = 1;
			delta_t_ms = 0;
			lastInputTime = now;
		}
		captionStillPeriodCounterMs += delta_t_ms;
		if (captionStillPeriodCounterMs > captionStillPeriodThresholdMs){
			captionMovementTimeCounterMs += delta_t_ms;
		}
		if (exitMenu->isActive()){
			updateExitMenu();
		} 
		else if (itemSelected){
			updateSelection();
		}
		else{
			updateInput();
		}
		// draw scene
		if (drawScene) {
			driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
			FK_SceneWithInput::executePreRenderingRoutine();
			drawAll();
			if (exitMenu->isVisible()) {
				exitMenu->draw();
			}
			if (hasToSetupJoypad()) {
				drawJoypadSetup(now);
			}
			FK_SceneWithInput::executePostRenderingRoutine();
			driver->endScene();
		}
	}

	// get last pressed button
	u32 FK_SceneInputMapping::getLastPressedButton(){
		u32 currentInput = KEY_KEY_CODES_COUNT;
		for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i){
			if (inputReceiver->IsKeyDown((EKEY_CODE)i) && acceptedKeys.count((EKEY_CODE)i) > 0){
				currentInput = i;
				break;
			}
		}
		return currentInput;
	}

	// update selection
	void FK_SceneInputMapping::updateSelection(){
		u32 inputThreshold = 200;
		if ((now - lastInputTime) > inputThreshold){
			u32 newInput = getLastPressedButton();
			if (newInput < KEY_KEY_CODES_COUNT && newInput != oldInputButton){
				lastInputTime = now;
				oldInputButton = newInput;
				itemSelected = false;
				Sleep(200);
				assignNewInput(getButtonFromIndex(menuIndex), newInput);
				soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			}
		}
	}

	bool FK_SceneInputMapping::isDirection(FK_Input_Buttons testButton){
		return (testButton & FK_Input_Buttons::Any_Direction) != 0;
	}
	bool FK_SceneInputMapping::isGameButton(FK_Input_Buttons testButton){
		return (testButton & FK_Input_Buttons::Any_Button) != 0;
	}
	bool FK_SceneInputMapping::isMenuButton(FK_Input_Buttons testButton){
		return (testButton & FK_Input_Buttons::Any_MenuButton) != 0;
	}

	// check if button is optional
	bool FK_SceneInputMapping::isOptionalButton(FK_Input_Buttons buttonToReplace){
		switch (buttonToReplace){
		case FK_Input_Buttons::Punch_plus_Guard_Button:
			return true;
		case FK_Input_Buttons::Punch_plus_Kick_Button:
			return true;
		case FK_Input_Buttons::Kick_plus_Guard_Button:
			return true;
		case FK_Input_Buttons::Tech_plus_Punch_Button:
			return true;
		case FK_Input_Buttons::Tech_plus_Kick_Button:
			return true;
		case FK_Input_Buttons::Tech_plus_Guard_Button:
			return true;
		case FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button:
			return true;
		case FK_Input_Buttons::Trigger_plus_Guard_Button:
			return true;
		case FK_Input_Buttons::All_Action_Buttons:
			return true;
		default:
			return false;
		}
		return false;
	}

	void FK_SceneInputMapping::swapSameInputButtons(FK_Input_Buttons lastModifiedButton, u32 oldKeyCode){
		
	}

	// assign new input
	void FK_SceneInputMapping::assignNewInput(FK_Input_Buttons buttonToReplace, u32 newKeyCode){
		
	}

	// update input
	void FK_SceneInputMapping::updateInput(){
		
	}

	void FK_SceneInputMapping::selectItem(){
		inputReceiver->reset();
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		itemSelected = true;
		lastInputTime = 0;
		oldInputButton = KEY_KEY_CODES_COUNT;
	}

	void FK_SceneInputMapping::increaseMenuIndex(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		menuIndex += 1;
		menuIndex %= menuOptionsStrings.size();
	}

	void FK_SceneInputMapping::decreaseMenuIndex(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		if (menuIndex == 0){
			menuIndex = menuOptionsStrings.size() - 1;
		}
		else{
			menuIndex -= 1;
		}
		//captionStillPeriodCounterMs = 0;
		//captionMovementTimeCounterMs = 0;
	}

	// setup sound manager
	void FK_SceneInputMapping::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}

	// end scene
	void FK_SceneInputMapping::exitScene(bool playSound){
		deactivateExitMenu();
		if (playSound){
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		}
		endScene = true;
		Sleep(200);
	}

	void FK_SceneInputMapping::exitSceneFromExitMenu(bool playSound)
	{
		exitScene(playSound);
	}
	

	// end scene
	void FK_SceneInputMapping::saveAndExit(bool playSound){
		saveSettings();
		if (playSound){
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		}
		exitScene(false);
	};

	// check if the scene is running
	bool FK_SceneInputMapping::isRunning(){
		return endScene == false;
	}

	// get scene name tag
	FK_Scene::FK_SceneType FK_SceneInputMapping::nameId(){
		return FK_Scene::FK_SceneType::SceneInputMapping;
	}

	void FK_SceneInputMapping::drawEmbedded(bool showBackground)
	{
		drawAll(showBackground);
		if (exitMenu->isVisible()) {
			exitMenu->draw();
		}
	}

	/* check for joypads connected to the device and store information */
	void FK_SceneInputMapping::setupJoypad(){
		// return if joypads have already been activated
		if (!joystickInfo.empty()){
			return;
		}
	};

	/* setup exit menu */
	void FK_SceneInputMapping::setupExitMenu(){
		std::vector<std::string> menuItems = {
			"Save changes and exit",
			"Exit without saving",
			"Cancel",
		};
		exitMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		if (gameOptions->getBorderlessWindowMode()) {
			exitMenu->setViewport(core::position2di(-1, -1), screenSize);
		}
	}

	/* update exit menu */
	void FK_SceneInputMapping::updateExitMenu(){
		if (joystickInfo.size() > 0){
			player1input->update(now, inputReceiver->JoypadStatus(0));
		}
		else{
			player1input->update(now, inputReceiver->KeyboardStatus());
		}
		if (joystickInfo.size() > 1){
			player2input->update(now, inputReceiver->JoypadStatus(1));
		}
		else{
			player2input->update(now, inputReceiver->KeyboardStatus());
		}
		u32 inputButtonsForMenu = player1input->getPressedButtons();
		inputButtonsForMenu |= player2input->getPressedButtons();
		if ((inputButtonsForMenu & FK_Input_Buttons::Cancel_Button) != 0){
			deactivateExitMenu();
			Sleep(300);
			return;
		}
		else{
			exitMenu->update(inputButtonsForMenu);
			if (exitMenu->itemIsSelected()){
				s32 exitMenuIndex = exitMenu->getIndex();
				exitMenu->resetSelection();
				switch (exitMenuIndex){
				case 0:
					saveAndExit(false);
					break;
				case 1:
					exitSceneFromExitMenu(false);
					break;
				case 2:
					deactivateExitMenu();
					Sleep(200);
					break;
				default:
					break;
				}
			}
		}
	}

	// manage the state of the exit menu
	void FK_SceneInputMapping::activateExitMenu(){
		exitMenu->setActive(true);
		exitMenu->setVisible(true);
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		Sleep(200);
	}
	void FK_SceneInputMapping::deactivateExitMenu(){
		exitMenu->reset();
		exitMenu->setActive(false);
		exitMenu->setVisible(false);
	}

	video::SColor FK_SceneInputMapping::getSelectedKeyColor()
	{
		if (itemSelected) {
			return getInputReplacementColor();
		}
		else {
			return irr::video::SColor(255, 20, 225, 220);
		}
	}

	video::SColor FK_SceneInputMapping::getInputReplacementColor()
	{
		f32 phase = (f32)(now % 1000) / 1000 * 2.0f * 3.1415926f;
		u32 offset = std::round(100 * std::cos(phase));
		return irr::video::SColor(255, 255, 125 + offset, 0);
	}

	FK_InputMapper* FK_SceneInputMapping::getInputMapper(){
		return inputMapper;
	}
}