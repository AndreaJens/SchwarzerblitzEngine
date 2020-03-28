#ifndef FK_SCENEINPUTMAPPING_JOYPADS_CLASS
#define FK_SCENEINPUTMAPPING_JOYPADS_CLASS

#include "FK_SceneInputMapping.h"

using namespace irr;

namespace fk_engine{
	class FK_SceneInputMapping_Joypads : public FK_SceneInputMapping{
	public:
		FK_SceneInputMapping_Joypads();
		FK_SceneInputMapping_Joypads(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		void saveSettings();
	protected:
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
		void togglePlayer();
		void drawPlayerTags();
	private:
		u32 playerIndex;
	};
};

#endif