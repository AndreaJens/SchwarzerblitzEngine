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