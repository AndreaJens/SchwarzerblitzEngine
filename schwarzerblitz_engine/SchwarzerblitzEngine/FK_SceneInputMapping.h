#ifndef FK_SCENEINPUTMAPPING_CLASS
#define FK_SCENEINPUTMAPPING_CLASS

#include "FK_SceneWithInput.h"
#include "FK_SoundManager.h"
#include "FK_InGameMenu.h"
#include <vector>
#include <map>
#include <string>

using namespace irr;

namespace fk_engine{
	class FK_SceneInputMapping : public FK_SceneWithInput{
	public:
		virtual void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		virtual void saveSettings();
		virtual void update();
		virtual void update(bool drawScene);
		virtual void dispose();
	protected:
		virtual void drawItem(u32 itemIndex);
		virtual void drawAll(bool showBackground = true);
		virtual void drawTitle();
		virtual void swapSameInputButtons(FK_Input_Buttons lastModifiedButton, u32 oldKeyCode);
		virtual void assignNewInput(FK_Input_Buttons buttonToReplace, u32 newKeyCode);
		virtual void updateInput();
	public:
		FK_SceneInputMapping();
		FK_SceneInputMapping(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		bool isRunning();
		FK_InputMapper* getInputMapper();
		FK_Scene::FK_SceneType nameId();
		void drawEmbedded(bool showBackground);
	protected:
		void initialize();
		void initializeAcceptedKeyboardButtonsMap();
		void initializeJoypadButtonsMap();
		void initializeButtonArrays();
		FK_Input_Buttons getButtonFromIndex(u32 index);
		void setupGraphics();
		void setupSoundManager();
		void setupJoypad();
		void selectItem();
		virtual void increaseMenuIndex();
		virtual void decreaseMenuIndex();
		//void cancelSelection();
		void drawBackground();
		void drawCaptions();
		void drawItems();
		void drawArrows();
		bool isOptionalButton(FK_Input_Buttons testButton);
		bool isDirection(FK_Input_Buttons testButton);
		bool isGameButton(FK_Input_Buttons testButton);
		bool isMenuButton(FK_Input_Buttons testButton);
		void exitScene(bool playSound = true);
		void saveAndExit(bool playSound = true);
		virtual u32 getLastPressedButton();
		virtual void updateSelection();
		void setupExitMenu();
		void updateExitMenu();
		void activateExitMenu();
		void deactivateExitMenu();
		video::SColor getSelectedKeyColor();
		video::SColor getInputReplacementColor();
	protected:
		/* key array */
		std::vector<FK_Input_Buttons> directionButtons;
		std::vector<FK_Input_Buttons> menuButtons;
		std::vector<FK_Input_Buttons> gameButtons;
		std::map <EKEY_CODE, std::string> acceptedKeys;
		std::map <FK_JoypadInput, std::string> joypadKeys;
		/* new input map */
		std::vector<std::map<FK_Input_Buttons, u32> > modifiedInputMap;
		/* old input to check every update */
		u32 oldInputButton;
		u32 lastInputTime;
		/* options flow*/
		core::vector2d<s32> options_spacing;
		std::vector<std::string> menuOptionsStrings;
		std::vector<std::string> menuOptionsCaptions;
		std::vector<bool> menuActiveItems;
		/* menu flow*/
		s32 menuIndex;
		bool itemSelected;
		bool endScene;
		/* fonts and graphical elements */
		gui::IGUIFont *font;
		gui::IGUIFont *submenufont;
		gui::IGUIFont *captionFont;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 arrowFrequency;
		u32 lastInputUpdateTime;
		u32 captionMovementTimeCounterMs;
		u32 captionStillPeriodThresholdMs;
		u32 captionStillPeriodCounterMs;
		/* additional variables (e.g. to check scene flow)*/
		u32 cycleId;
		/* sound manager and announcer */
		std::string voiceClipsPath;
		std::string soundPath;
		FK_SoundManager* soundManager;
		/* transition counter (used when scene ends) */
		u32 transitionCounter;
		/* graphic elements */
		video::ITexture* menuBackgroundTexture;
		video::ITexture* menuCaptionOverlay;
		video::ITexture* arrowUp_tex;
		video::ITexture* arrowDown_tex;
		core::dimension2d<u32> screenSize;
		float scale_factorX;
		float scale_factorY;
		FK_InGameMenu* exitMenu;
	};
};

#endif