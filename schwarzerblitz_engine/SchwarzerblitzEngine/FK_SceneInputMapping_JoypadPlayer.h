#pragma once
#ifndef FK_SCENEINPUTMAPPING_JOYPADPLAYER_CLASS
#define FK_SCENEINPUTMAPPING_JOYPADPLAYER_CLASS

#include "FK_SceneInputMapping.h"

using namespace irr;

namespace fk_engine {
	class FK_SceneInputMapping_JoypadPlayer : public FK_SceneInputMapping {
	public:
		FK_SceneInputMapping_JoypadPlayer(u32 playerId);
		FK_SceneInputMapping_JoypadPlayer(u32 playerId, IrrlichtDevice* newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice* newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		void saveSettings();
	protected:
		void restoreOriginalSaveSettings();
		u32 getLastPressedButton();
		void updateSelection();
		void increaseMenuIndex();
		void decreaseMenuIndex();
		void initialize();
		void drawItem(u32 itemIndex);
		void drawAll(bool showBackground = true);
		void drawTitle();
		void swapSameInputButtons(FK_Input_Buttons lastModifiedButton, u32 oldKeyCode);
		void assignNewInput(FK_Input_Buttons buttonToReplace, u32 newKeyCode);
		void updateInput();
		virtual void exitSceneFromExitMenu(bool playSound = true) override;
	private:
		u32 playerIndex;
		std::vector <std::map<FK_Input_Buttons, u32>> originalInputMap;
		std::vector <std::map<FK_Input_Buttons, u32>> originalInputMapTranslated;
	};
};

#endif