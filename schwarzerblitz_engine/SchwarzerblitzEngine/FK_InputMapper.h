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


# ifndef FK_INPUT_MAPPER_CLASS
# define FK_INPUT_MAPPER_CLASS

#include<irrlicht.h>
#include<map>
#include<string>
#include<vector>
#include<deque>
#include"FK_Database.h"

using namespace irr;

namespace fk_engine{
	class FK_InputMapper{
	public:
		FK_InputMapper();
		std::map<FK_Input_Buttons, u32>& getKeyboardMap(int playerId);
		std::map<FK_Input_Buttons, u32>& getJoypadMap(int playerId);
		void setKeyboardInputMap(int playerId, std::map<FK_Input_Buttons, u32> newMap);
		void setJoypadInputMap(int playerId, std::map<FK_Input_Buttons, u32> newMap);
		void setButtonKeyboard(int playerId, FK_Input_Buttons button, u32 input);
		void setButtonJoypad(int playerId, FK_Input_Buttons button, SEvent::SJoystickEvent & joypadData);
		void saveToFile(std::string joypadsettings_filename, std::string keymap_filename);
		void saveMapToFile(std::string filename);
		void saveJoypadTranslationMapToXboxControllerToFile(std::string filename);
		bool createNewJoypadTranslationMapToXboxController(std::string joypadName);
		bool setButtonForJoypadTranslationMapToXboxController(std::string joypadName, FK_JoypadInput buttonToMap, FK_JoypadInputNative nativeButton);
		FK_JoypadInput getTranslatedButtonFromJoypadMap(std::string joypadName, FK_JoypadInputNative nativeButton);
		void loadFromFile(std::string joypadsettings_filename, std::string keymap_filename, core::array<SJoystickInfo> &joypadInfo, 
			std::deque<std::pair<std::string, u32> > &newJoypadFound, bool createIfNotExistent = true);
	private:
		void saveSingleMapToFile(std::ofstream& output_file, std::map<FK_Input_Buttons, u32> mapToSave);
		void loadSingleMapFromFile(std::ifstream& input_file, std::map<FK_Input_Buttons, u32>& mapToLoad);
		void loadJoypadMapFromFile(std::ifstream& input_file, std::map<FK_Input_Buttons, u32>& mapToLoad, 
			std::map<FK_JoypadInput, FK_JoypadInputNative> & joypad_inputConversionMap);
		std::map<FK_Input_Buttons, u32> inputMap_player1_keyboard;
		std::map<FK_Input_Buttons, u32> inputMap_player1_joypad;
		std::map<FK_Input_Buttons, u32> inputMap_player2_keyboard;
		std::map<FK_Input_Buttons, u32> inputMap_player2_joypad;
		std::map<FK_Input_Buttons, u32> inputMap_single_player_keyboard;
		std::map<FK_Input_Buttons, u32> inputMap_emptyMap;
		std::map<FK_JoypadInput, FK_JoypadInputNative> joypadConversionMap_player1;
		std::map<FK_JoypadInput, FK_JoypadInputNative> joypadConversionMap_player2;
		std::map<std::string, std::map<FK_JoypadInput, FK_JoypadInputNative> > joypadConversionMap;
	};
}

#endif