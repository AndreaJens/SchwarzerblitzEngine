#include"FK_InputMapper.h"
#include<fstream>
#include<iostream>

using namespace irr;

namespace fk_engine{
	FK_InputMapper::FK_InputMapper(){
		/* create joypad translation map */
		for (u32 i = 0; i < FK_JoypadInput::JoypadInput_XboxNumberOfButtons; ++i){
			joypadConversionMap_player1[(FK_JoypadInput)i] = (FK_JoypadInputNative)i;
			joypadConversionMap_player2[(FK_JoypadInput)i] = (FK_JoypadInputNative)i;
		}
		joypadConversionMap_player1[FK_JoypadInput::JoypadInput_NoButton] = FK_JoypadInputNative::JoypadInput_NoButton_Native;
		joypadConversionMap_player2[FK_JoypadInput::JoypadInput_NoButton] = FK_JoypadInputNative::JoypadInput_NoButton_Native;
		/* create empty map */
		inputMap_emptyMap = std::map<FK_Input_Buttons, u32>();
		/* create default map for single player */
		inputMap_single_player_keyboard[FK_Input_Buttons::Up_Direction] = (u32)EKEY_CODE::KEY_UP;
		inputMap_single_player_keyboard[FK_Input_Buttons::Down_Direction] = (u32)EKEY_CODE::KEY_DOWN;
		inputMap_single_player_keyboard[FK_Input_Buttons::Left_Direction] = (u32)EKEY_CODE::KEY_LEFT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Right_Direction] = (u32)EKEY_CODE::KEY_RIGHT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Guard_Button] = (u32)EKEY_CODE::KEY_KEY_D;
		inputMap_single_player_keyboard[FK_Input_Buttons::Tech_Button] = (u32)EKEY_CODE::KEY_KEY_S;
		inputMap_single_player_keyboard[FK_Input_Buttons::Kick_Button] = (u32)EKEY_CODE::KEY_KEY_A;
		inputMap_single_player_keyboard[FK_Input_Buttons::Punch_Button] = (u32)EKEY_CODE::KEY_KEY_W;
		inputMap_single_player_keyboard[FK_Input_Buttons::Trigger_Button] = (u32)EKEY_CODE::KEY_KEY_E;
		inputMap_single_player_keyboard[FK_Input_Buttons::Menu_Pause_Button] = (u32)EKEY_CODE::KEY_ESCAPE;
		// Menu buttons
		inputMap_single_player_keyboard[FK_Input_Buttons::Selection_Button] = (u32)EKEY_CODE::KEY_KEY_C;
		inputMap_single_player_keyboard[FK_Input_Buttons::Cancel_Button] = (u32)EKEY_CODE::KEY_KEY_X;
		inputMap_single_player_keyboard[FK_Input_Buttons::Modifier_Button] = (u32)EKEY_CODE::KEY_KEY_Z;
		inputMap_single_player_keyboard[FK_Input_Buttons::Confirmation_Button] = (u32)EKEY_CODE::KEY_RETURN;
		inputMap_single_player_keyboard[FK_Input_Buttons::ScrollLeft_Button] = (u32)EKEY_CODE::KEY_KEY_Q;
		inputMap_single_player_keyboard[FK_Input_Buttons::ScrollRight_Button] = (u32)EKEY_CODE::KEY_KEY_E;
		inputMap_single_player_keyboard[FK_Input_Buttons::Help_Button] = (u32)EKEY_CODE::KEY_OEM_102;
		// Hotkeys
		inputMap_single_player_keyboard[FK_Input_Buttons::Punch_plus_Kick_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Trigger_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Punch_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Kick_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Tech_plus_Punch_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Tech_plus_Kick_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Tech_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = KEY_KEY_CODES_COUNT;
		inputMap_single_player_keyboard[FK_Input_Buttons::All_Action_Buttons] = KEY_KEY_CODES_COUNT;

		/* create default for player 1 keyboard*/
		inputMap_player1_keyboard[FK_Input_Buttons::Up_Direction] = (u32) EKEY_CODE::KEY_KEY_W;
		inputMap_player1_keyboard[FK_Input_Buttons::Down_Direction] = (u32) EKEY_CODE::KEY_KEY_S;
		inputMap_player1_keyboard[FK_Input_Buttons::Left_Direction] = (u32) EKEY_CODE::KEY_KEY_A;
		inputMap_player1_keyboard[FK_Input_Buttons::Right_Direction] = (u32) EKEY_CODE::KEY_KEY_D;
		inputMap_player1_keyboard[FK_Input_Buttons::Guard_Button] = (u32) EKEY_CODE::KEY_KEY_M;
		inputMap_player1_keyboard[FK_Input_Buttons::Tech_Button] = (u32) EKEY_CODE::KEY_KEY_N;
		inputMap_player1_keyboard[FK_Input_Buttons::Kick_Button] = (u32) EKEY_CODE::KEY_KEY_B;
		inputMap_player1_keyboard[FK_Input_Buttons::Punch_Button] = (u32) EKEY_CODE::KEY_KEY_V;
		inputMap_player1_keyboard[FK_Input_Buttons::Trigger_Button] = (u32) EKEY_CODE::KEY_KEY_G;
		inputMap_player1_keyboard[FK_Input_Buttons::Menu_Pause_Button] = (u32)EKEY_CODE::KEY_ESCAPE;
		// Menu buttons
		inputMap_player1_keyboard[FK_Input_Buttons::Selection_Button] = inputMap_player1_keyboard[FK_Input_Buttons::Guard_Button];
		inputMap_player1_keyboard[FK_Input_Buttons::Cancel_Button] = inputMap_player1_keyboard[FK_Input_Buttons::Tech_Button];
		inputMap_player1_keyboard[FK_Input_Buttons::Modifier_Button] = inputMap_player1_keyboard[FK_Input_Buttons::Kick_Button];
		inputMap_player1_keyboard[FK_Input_Buttons::Confirmation_Button] = inputMap_player1_keyboard[FK_Input_Buttons::Menu_Pause_Button];
		inputMap_player1_keyboard[FK_Input_Buttons::ScrollLeft_Button] = (u32)EKEY_CODE::KEY_KEY_Q;
		inputMap_player1_keyboard[FK_Input_Buttons::ScrollRight_Button] = (u32)EKEY_CODE::KEY_KEY_E;
		inputMap_player1_keyboard[FK_Input_Buttons::Help_Button] = (u32)EKEY_CODE::KEY_OEM_102;
		// Hotkeys
		inputMap_player1_keyboard[FK_Input_Buttons::Punch_plus_Kick_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Trigger_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Punch_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Kick_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Tech_plus_Punch_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Tech_plus_Kick_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Tech_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player1_keyboard[FK_Input_Buttons::All_Action_Buttons] = KEY_KEY_CODES_COUNT;
		/* create default for player 2 keyboard*/
		inputMap_player2_keyboard[FK_Input_Buttons::Up_Direction] = (u32) EKEY_CODE::KEY_NUMPAD5;
		inputMap_player2_keyboard[FK_Input_Buttons::Down_Direction] = (u32) EKEY_CODE::KEY_NUMPAD2;
		inputMap_player2_keyboard[FK_Input_Buttons::Left_Direction] = (u32) EKEY_CODE::KEY_NUMPAD1;
		inputMap_player2_keyboard[FK_Input_Buttons::Right_Direction] = (u32) EKEY_CODE::KEY_NUMPAD3;
		inputMap_player2_keyboard[FK_Input_Buttons::Guard_Button] = (u32) EKEY_CODE::KEY_KEY_I;
		inputMap_player2_keyboard[FK_Input_Buttons::Tech_Button] = (u32) EKEY_CODE::KEY_KEY_K;
		inputMap_player2_keyboard[FK_Input_Buttons::Kick_Button] = (u32) EKEY_CODE::KEY_KEY_J;
		inputMap_player2_keyboard[FK_Input_Buttons::Punch_Button] = (u32) EKEY_CODE::KEY_KEY_L;
		inputMap_player2_keyboard[FK_Input_Buttons::Trigger_Button] = (u32) EKEY_CODE::KEY_KEY_O;
		inputMap_player2_keyboard[FK_Input_Buttons::Menu_Pause_Button] = (u32)EKEY_CODE::KEY_DIVIDE;
		// Menu buttons
		inputMap_player2_keyboard[FK_Input_Buttons::Selection_Button] = inputMap_player2_keyboard[FK_Input_Buttons::Guard_Button];
		inputMap_player2_keyboard[FK_Input_Buttons::Cancel_Button]    = inputMap_player2_keyboard[FK_Input_Buttons::Tech_Button];
		inputMap_player2_keyboard[FK_Input_Buttons::Modifier_Button] = inputMap_player2_keyboard[FK_Input_Buttons::Kick_Button];
		inputMap_player2_keyboard[FK_Input_Buttons::Confirmation_Button] = inputMap_player2_keyboard[FK_Input_Buttons::Menu_Pause_Button];
		inputMap_player2_keyboard[FK_Input_Buttons::ScrollLeft_Button] = (u32)EKEY_CODE::KEY_KEY_U;
		inputMap_player2_keyboard[FK_Input_Buttons::ScrollRight_Button] = (u32)EKEY_CODE::KEY_KEY_O;
		inputMap_player2_keyboard[FK_Input_Buttons::Help_Button] = (u32)EKEY_CODE::KEY_MULTIPLY;
		// Hotkeys
		inputMap_player2_keyboard[FK_Input_Buttons::Punch_plus_Kick_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Trigger_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Punch_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Kick_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Tech_plus_Punch_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Tech_plus_Kick_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Tech_plus_Guard_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = KEY_KEY_CODES_COUNT;
		inputMap_player2_keyboard[FK_Input_Buttons::All_Action_Buttons] = KEY_KEY_CODES_COUNT;
		/* create default joypad map for player 1*/
		inputMap_player1_joypad[FK_Input_Buttons::Up_Direction] = FK_JoypadInput::JoypadInputUp;
		inputMap_player1_joypad[FK_Input_Buttons::Down_Direction] = FK_JoypadInput::JoypadInputDown;
		inputMap_player1_joypad[FK_Input_Buttons::Left_Direction] = FK_JoypadInput::JoypadInputLeft;
		inputMap_player1_joypad[FK_Input_Buttons::Right_Direction] = FK_JoypadInput::JoypadInputRight;
		inputMap_player1_joypad[FK_Input_Buttons::Guard_Button] = FK_JoypadInput::JoypadInput_A_Button;
		inputMap_player1_joypad[FK_Input_Buttons::Tech_Button] = FK_JoypadInput::JoypadInput_B_Button;
		inputMap_player1_joypad[FK_Input_Buttons::Kick_Button] = FK_JoypadInput::JoypadInput_X_Button;
		inputMap_player1_joypad[FK_Input_Buttons::Punch_Button] = FK_JoypadInput::JoypadInput_Y_Button;
		inputMap_player1_joypad[FK_Input_Buttons::Trigger_Button] = FK_JoypadInput::JoypadInput_R1_Button;
		inputMap_player1_joypad[FK_Input_Buttons::Menu_Pause_Button] = FK_JoypadInput::JoypadInput_Start_Button;
		// Menu buttons
		inputMap_player1_joypad[FK_Input_Buttons::Selection_Button] = inputMap_player1_joypad[FK_Input_Buttons::Guard_Button];
		inputMap_player1_joypad[FK_Input_Buttons::Cancel_Button] = inputMap_player1_joypad[FK_Input_Buttons::Tech_Button];
		inputMap_player1_joypad[FK_Input_Buttons::Modifier_Button] = inputMap_player1_joypad[FK_Input_Buttons::Kick_Button];
		inputMap_player1_joypad[FK_Input_Buttons::Confirmation_Button] = inputMap_player1_joypad[FK_Input_Buttons::Menu_Pause_Button];
		inputMap_player1_joypad[FK_Input_Buttons::ScrollLeft_Button] = FK_JoypadInput::JoypadInput_L1_Button;
		inputMap_player1_joypad[FK_Input_Buttons::ScrollRight_Button] = FK_JoypadInput::JoypadInput_R1_Button;
		inputMap_player1_joypad[FK_Input_Buttons::Help_Button] = FK_JoypadInput::JoypadInput_Select_Button;
		// Hotkeys
		inputMap_player1_joypad[FK_Input_Buttons::Punch_plus_Kick_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Trigger_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Punch_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Kick_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Tech_plus_Punch_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Tech_plus_Kick_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Tech_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player1_joypad[FK_Input_Buttons::All_Action_Buttons] = FK_JoypadInput::JoypadInput_NoButton;
		/* create default joypad map for player 2*/
		inputMap_player2_joypad[FK_Input_Buttons::Up_Direction] = FK_JoypadInput::JoypadInputUp;
		inputMap_player2_joypad[FK_Input_Buttons::Down_Direction] = FK_JoypadInput::JoypadInputDown;
		inputMap_player2_joypad[FK_Input_Buttons::Left_Direction] = FK_JoypadInput::JoypadInputLeft;
		inputMap_player2_joypad[FK_Input_Buttons::Right_Direction] = FK_JoypadInput::JoypadInputRight;
		inputMap_player2_joypad[FK_Input_Buttons::Guard_Button] = FK_JoypadInput::JoypadInput_A_Button;
		inputMap_player2_joypad[FK_Input_Buttons::Tech_Button] = FK_JoypadInput::JoypadInput_B_Button;
		inputMap_player2_joypad[FK_Input_Buttons::Kick_Button] = FK_JoypadInput::JoypadInput_X_Button;
		inputMap_player2_joypad[FK_Input_Buttons::Punch_Button] = FK_JoypadInput::JoypadInput_Y_Button;
		inputMap_player2_joypad[FK_Input_Buttons::Trigger_Button] = FK_JoypadInput::JoypadInput_R1_Button;
		inputMap_player2_joypad[FK_Input_Buttons::Menu_Pause_Button] = FK_JoypadInput::JoypadInput_Start_Button;
		// Menu buttons
		inputMap_player2_joypad[FK_Input_Buttons::Selection_Button] = inputMap_player2_joypad[FK_Input_Buttons::Guard_Button];
		inputMap_player2_joypad[FK_Input_Buttons::Cancel_Button] = inputMap_player2_joypad[FK_Input_Buttons::Tech_Button];
		inputMap_player2_joypad[FK_Input_Buttons::Modifier_Button] = inputMap_player2_joypad[FK_Input_Buttons::Kick_Button];
		inputMap_player2_joypad[FK_Input_Buttons::Confirmation_Button] = inputMap_player2_joypad[FK_Input_Buttons::Menu_Pause_Button];
		inputMap_player2_joypad[FK_Input_Buttons::ScrollLeft_Button] = FK_JoypadInput::JoypadInput_L1_Button;
		inputMap_player2_joypad[FK_Input_Buttons::ScrollRight_Button] = FK_JoypadInput::JoypadInput_R1_Button;
		inputMap_player2_joypad[FK_Input_Buttons::Help_Button] = FK_JoypadInput::JoypadInput_Select_Button;
		// Hotkeys
		inputMap_player2_joypad[FK_Input_Buttons::Punch_plus_Kick_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Trigger_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Punch_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Kick_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Tech_plus_Punch_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Tech_plus_Kick_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Tech_plus_Guard_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = FK_JoypadInput::JoypadInput_NoButton;
		inputMap_player2_joypad[FK_Input_Buttons::All_Action_Buttons] = FK_JoypadInput::JoypadInput_NoButton;

		
	}
	/* remap keyboard button*/
	void FK_InputMapper::setButtonKeyboard(int playerId, FK_Input_Buttons gameButton, u32 inputButton){
		std::map<FK_Input_Buttons, u32> inputMap = getKeyboardMap(playerId);
		inputMap[gameButton] = inputButton;
	};

	void FK_InputMapper::setButtonJoypad(int playerId, FK_Input_Buttons button, SEvent::SJoystickEvent & joypadData){
		std::map<FK_Input_Buttons, u32> inputMap = getJoypadMap(playerId);
		joypadData.ButtonStates;
		/* to be implemented!!! */
	}

	std::map<FK_Input_Buttons, u32>& FK_InputMapper::getKeyboardMap(int playerId){
		if (playerId == 0){
			return inputMap_single_player_keyboard;
		}else if(playerId == 1){
			return inputMap_player1_keyboard;
		}
		else if (playerId == 2){
			return inputMap_player2_keyboard;
		}
		return inputMap_emptyMap;
	}

	std::map<FK_Input_Buttons, u32>& FK_InputMapper::getJoypadMap(int playerId){
		std::map<FK_Input_Buttons, u32> inputMap = std::map<FK_Input_Buttons, u32>();
		if (playerId == 1){
			return inputMap_player1_joypad;
		}
		else if (playerId == 2){
			return inputMap_player2_joypad;
		}
		return inputMap_emptyMap;
	}
	void FK_InputMapper::setKeyboardInputMap(int playerId, std::map<FK_Input_Buttons, u32> newMap){
		if (playerId == 0){
			inputMap_single_player_keyboard = newMap;
		}else if (playerId == 1){
			inputMap_player1_keyboard = newMap;
		}
		else if (playerId == 2){
			inputMap_player2_keyboard = newMap;
		}
	}

	void FK_InputMapper::setJoypadInputMap(int playerId, std::map<FK_Input_Buttons, u32> newMap){
		if (playerId == 1){
			inputMap_player1_joypad = newMap;
		}
		else if (playerId == 2){
			inputMap_player2_joypad = newMap;
		}
	}

	/* save all information to file*/
	void FK_InputMapper::saveToFile(std::string joypadsettings_filename, std::string keymap_filename){
		saveJoypadTranslationMapToXboxControllerToFile(joypadsettings_filename);
		saveMapToFile(keymap_filename);
	}

	/* create new interpreter for current joypad */
	bool FK_InputMapper::createNewJoypadTranslationMapToXboxController(std::string joypadName){
		if (joypadConversionMap.find(joypadName) == joypadConversionMap.end()){
			joypadConversionMap[joypadName] = std::map<FK_JoypadInput, FK_JoypadInputNative>();
			for (u32 i = 0; i < FK_JoypadInput::JoypadInput_XboxNumberOfButtons; ++i){
				joypadConversionMap[joypadName][(FK_JoypadInput)i] = (FK_JoypadInputNative)i;
			}
			joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_NoButton] = FK_JoypadInputNative::JoypadInput_NoButton_Native;
			return true;
		}
		return false;
	}

	/* back-translate buttons*/
	FK_JoypadInput FK_InputMapper::getTranslatedButtonFromJoypadMap(std::string joypadName, FK_JoypadInputNative nativeButton){
		if (joypadConversionMap.find(joypadName) == joypadConversionMap.end()){
			std::cout << "Warning! You are trying to assign button to non-existing map!" << std::endl;
			return FK_JoypadInput::JoypadInput_NoButton;
		}
		for (auto iter = joypadConversionMap[joypadName].begin(); iter != joypadConversionMap[joypadName].end(); ++iter)
		{
			if (iter->second == nativeButton){
				return iter->first;
			}
		}
		return FK_JoypadInput::JoypadInput_NoButton;
	}

	/* map button to Xbox button */
	bool FK_InputMapper::setButtonForJoypadTranslationMapToXboxController(std::string joypadName, 
		FK_JoypadInput buttonToMap, FK_JoypadInputNative nativeButton){
		if (joypadConversionMap.find(joypadName) == joypadConversionMap.end()){
			std::cout << "Warning! You are trying to assign button to non-existing map!" << std::endl;
			return false;
		}
		joypadConversionMap[joypadName][buttonToMap] = nativeButton;
		return true;
	}

	/* save joypad settings to file*/
	void FK_InputMapper::saveJoypadTranslationMapToXboxControllerToFile(std::string filename){
		/* open file*/
		std::ofstream outputFile(filename.c_str());
		for (auto iter = joypadConversionMap.begin(); iter != joypadConversionMap.end(); ++iter){
			std::string joypadName = iter->first;
			outputFile << joypadName << std::endl;
			outputFile << "A\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_A_Button] << std::endl;
			outputFile << "B\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_B_Button] << std::endl;
			outputFile << "X\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_X_Button] << std::endl;
			outputFile << "Y\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_Y_Button] << std::endl;
			outputFile << "LB\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_L1_Button] << std::endl;
			outputFile << "RB\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_R1_Button] << std::endl;
			outputFile << "Back\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_Select_Button] << std::endl;
			outputFile << "Start\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_Start_Button] << std::endl;
			outputFile << "L3\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_L3_Button] << std::endl;
			outputFile << "R3\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_R3_Button] << std::endl;
			outputFile << "LT\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_L2_Button] << std::endl;
			outputFile << "RT\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_R2_Button] << std::endl;
			outputFile << "Home\t" << joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_Home_Button] << std::endl << std::endl;
		}
		outputFile.close();
	}

	// save map to file
	void FK_InputMapper::saveSingleMapToFile(std::ofstream& outputFile, std::map<FK_Input_Buttons, u32> mapToSave){
		outputFile << "^\t" << mapToSave[FK_Input_Buttons::Up_Direction] << std::endl;
		outputFile << "v\t" << mapToSave[FK_Input_Buttons::Down_Direction] << std::endl;
		outputFile << ">\t" << mapToSave[FK_Input_Buttons::Left_Direction] << std::endl;
		outputFile << "<\t" << mapToSave[FK_Input_Buttons::Right_Direction] << std::endl;
		outputFile << "Guard\t" << mapToSave[FK_Input_Buttons::Guard_Button] << std::endl;
		outputFile << "Tech\t" << mapToSave[FK_Input_Buttons::Tech_Button] << std::endl;
		outputFile << "Kick\t" << mapToSave[FK_Input_Buttons::Kick_Button] << std::endl;
		outputFile << "Punch\t" << mapToSave[FK_Input_Buttons::Punch_Button] << std::endl;
		outputFile << "Trigger\t" << mapToSave[FK_Input_Buttons::Trigger_Button] << std::endl;
		outputFile << "Menu/Pause\t" << mapToSave[FK_Input_Buttons::Menu_Pause_Button] << std::endl;
		// menu
		outputFile << "Select\t" << mapToSave[FK_Input_Buttons::Selection_Button] << std::endl;
		outputFile << "Cancel\t" << mapToSave[FK_Input_Buttons::Cancel_Button] << std::endl;
		outputFile << "Modifier\t" << mapToSave[FK_Input_Buttons::Modifier_Button] << std::endl;
		outputFile << "Help\t" << mapToSave[FK_Input_Buttons::Help_Button] << std::endl;
		outputFile << "Confirm\t" << mapToSave[FK_Input_Buttons::Confirmation_Button] << std::endl;
		outputFile << "ScrollLeft\t" << mapToSave[FK_Input_Buttons::ScrollLeft_Button] << std::endl;
		outputFile << "ScrollRight\t" << mapToSave[FK_Input_Buttons::ScrollRight_Button] << std::endl;
		// hotkeys
		outputFile << "Hotkey:P+K\t" << mapToSave[FK_Input_Buttons::Punch_plus_Kick_Button] << std::endl;
		outputFile << "Hotkey:Tg+G\t" << mapToSave[FK_Input_Buttons::Trigger_plus_Guard_Button] << std::endl;
		outputFile << "Hotkey:P+G\t" << mapToSave[FK_Input_Buttons::Punch_plus_Guard_Button] << std::endl;
		outputFile << "Hotkey:K+G\t" << mapToSave[FK_Input_Buttons::Kick_plus_Guard_Button] << std::endl;
		outputFile << "Hotkey:Tc+P\t" << mapToSave[FK_Input_Buttons::Tech_plus_Punch_Button] << std::endl;
		outputFile << "Hotkey:Tc+K\t" << mapToSave[FK_Input_Buttons::Tech_plus_Kick_Button] << std::endl;
		outputFile << "Hotkey:Tc+G\t" << mapToSave[FK_Input_Buttons::Tech_plus_Guard_Button] << std::endl;
		outputFile << "Hotkey:Tc+P+K\t" << mapToSave[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] << std::endl;
		outputFile << "Hotkey:Tc+G+P+K\t" << mapToSave[FK_Input_Buttons::All_Action_Buttons] << std::endl;
		// add a newline
		outputFile << std::endl;
		// return on succes
		return;
	}

	void FK_InputMapper::loadSingleMapFromFile(std::ifstream& inputFile, std::map<FK_Input_Buttons, u32>& mapToLoad){
		std::string dump;
		u32 code;
		/* load player 1 keyboard settings*/
		inputFile >> dump;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Up_Direction] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Down_Direction] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Left_Direction] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Right_Direction] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Guard_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Kick_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Trigger_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Menu_Pause_Button] = (EKEY_CODE)code;
		// menu
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Selection_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Cancel_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Modifier_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Help_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Confirmation_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::ScrollLeft_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::ScrollRight_Button] = (EKEY_CODE)code;
		// hotkeys
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_plus_Kick_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Trigger_plus_Guard_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_plus_Guard_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Kick_plus_Guard_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_plus_Punch_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_plus_Kick_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_plus_Guard_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = (EKEY_CODE)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::All_Action_Buttons] = (EKEY_CODE)code;
		return;
	}

	// load joypad map from file
	void FK_InputMapper::loadJoypadMapFromFile(std::ifstream& inputFile, std::map<FK_Input_Buttons, u32>& mapToLoad, 
		std::map<FK_JoypadInput, FK_JoypadInputNative> & joypad_inputConversionMap){
		std::string dump;
		u32 code;
		inputFile >> dump;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Up_Direction] = (FK_JoypadInput)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Down_Direction] = (FK_JoypadInput)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Left_Direction] = (FK_JoypadInput)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Right_Direction] = (FK_JoypadInput)code;
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Guard_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Kick_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Trigger_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Menu_Pause_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		// menu
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Selection_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Cancel_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Modifier_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Help_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Confirmation_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::ScrollLeft_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::ScrollRight_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		// hotkeys
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_plus_Kick_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Trigger_plus_Guard_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_plus_Guard_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Kick_plus_Guard_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_plus_Punch_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_plus_Kick_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Tech_plus_Guard_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::Punch_plus_Kick_plus_Tech_Button] = joypad_inputConversionMap[(FK_JoypadInput)code];
		inputFile >> dump >> code;
		mapToLoad[FK_Input_Buttons::All_Action_Buttons] = joypad_inputConversionMap[(FK_JoypadInput)code];
		return;
	}

	/* save input map on file */
	void FK_InputMapper::saveMapToFile(std::string filename){
		/* open file*/
		std::ofstream outputFile(filename.c_str());
		/* dump single player settings*/
		outputFile << "***single_player***\t" << std::endl;
		saveSingleMapToFile(outputFile, inputMap_single_player_keyboard);
		/* dump player 1 settings*/
		outputFile << "***P1_Keyboard***\t" << std::endl;
		saveSingleMapToFile(outputFile, inputMap_player1_keyboard);
		/* dump player 1 joypad setting*/
		outputFile << "***P1_Joypad***\t" << std::endl;
		saveSingleMapToFile(outputFile, inputMap_player1_joypad);
		/* dump player 2 settings*/
		outputFile << "***P2_Keyboard***\t" << std::endl;
		saveSingleMapToFile(outputFile, inputMap_player2_keyboard);
		/* dump player 2 joypad settings*/
		outputFile << "***P2_Joypad***\t" << std::endl;
		saveSingleMapToFile(outputFile, inputMap_player2_joypad);
		outputFile.close();
	};
	/* load input map from file*/
	void FK_InputMapper::loadFromFile(std::string joypadsettings_filename, std::string keymap_filename, 
		core::array<SJoystickInfo> &joypadInfo, std::deque<std::pair<std::string, u32> > &newJoypadFound, bool createIfNotExistent){
		/* open joypad configuration file*/
		newJoypadFound.clear();
		std::ifstream inputFile(joypadsettings_filename.c_str()); 
		if (!inputFile){
			inputFile.clear();
			inputFile.close();
			if (createIfNotExistent){
				saveJoypadTranslationMapToXboxControllerToFile(joypadsettings_filename);
			}
		}
		else{
			/* this string will be used to store all the strings in the file*/
			std::string dump;
			std::string joypadName;
			u32 code;
			/* load player 1 joypad translation */
			while (inputFile){
				std::string joypadName;
				std::getline(inputFile, joypadName);
				if (joypadName.empty()) continue;
				for (u32 i = 0; i < FK_JoypadInput::JoypadInput_XboxNumberOfButtons; ++i){
					inputFile >> dump >> code;
					joypadConversionMap[joypadName][(FK_JoypadInput)i] = (FK_JoypadInputNative)code;
				}
				joypadConversionMap[joypadName][FK_JoypadInput::JoypadInput_NoButton] = FK_JoypadInputNative::JoypadInput_NoButton_Native;
			}
			inputFile.close();
		}
		std::string joypadName;
		bool newMapCreated = false;
		if (joypadInfo.size() > 0){
			joypadName = std::string(joypadInfo[0].Name.c_str());
			if (joypadConversionMap.find(joypadName) != joypadConversionMap.end()){
				joypadConversionMap_player1 = joypadConversionMap[joypadName];
			}
			else{
				createNewJoypadTranslationMapToXboxController(joypadName);
				newJoypadFound.push_back(std::pair<std::string, u32>(joypadName, 0));
				newMapCreated = true;
			}
		}
		if (joypadInfo.size() > 1){
			joypadName = std::string(joypadInfo[1].Name.c_str());
			if (joypadConversionMap.find(joypadName) != joypadConversionMap.end()){
				joypadConversionMap_player2 = joypadConversionMap[joypadName];
			}
			else{
				createNewJoypadTranslationMapToXboxController(joypadName);
				newJoypadFound.push_back(std::pair<std::string, u32>(joypadName, 1));
				newMapCreated = true;
			}
		}
		if (newMapCreated && createIfNotExistent){
			saveJoypadTranslationMapToXboxControllerToFile(joypadsettings_filename);
		}
		/* open file*/
		inputFile = std::ifstream(keymap_filename.c_str());
		if (!inputFile){
			if (createIfNotExistent){
				inputFile.clear();
				inputFile.close();
				saveMapToFile(keymap_filename);
			}
		}
		else{
			/* this string will be used to store all the strings in the file*/
			/* load single player keyboard settings */
			loadSingleMapFromFile(inputFile, inputMap_single_player_keyboard);
			/* load player 1 keyboard settings */
			loadSingleMapFromFile(inputFile, inputMap_player1_keyboard);
			/* load player 1 joypad settings*/
			loadJoypadMapFromFile(inputFile, inputMap_player1_joypad, joypadConversionMap_player1);
			/* load player 2 keyboard settings*/
			loadSingleMapFromFile(inputFile, inputMap_player2_keyboard);
			/* load player 2 joypad settings*/
			if (joypadInfo.size() < 2){
				joypadConversionMap_player2 = joypadConversionMap_player1;
			}
			loadJoypadMapFromFile(inputFile, inputMap_player2_joypad, joypadConversionMap_player2);
			inputFile.close();
		}
	};
};