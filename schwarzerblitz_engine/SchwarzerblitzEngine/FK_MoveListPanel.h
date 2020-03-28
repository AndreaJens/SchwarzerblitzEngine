#ifndef FK_MOVELISTPANEL_CLASS
#define FK_MOVELISTPANEL_CLASS

#include"FK_Window.h"
#include"FK_Character.h"
#include"FK_MoveListMove.h"
#include"FK_SoundManager.h"
#include"FK_Options.h"
#include"FK_InputMapper.h"
#include<string>
#include<map>

using namespace irr;

namespace fk_engine{
	class FK_MoveListPanel{
	public:
		FK_MoveListPanel();
		FK_MoveListPanel(IrrlichtDevice* device, FK_SoundManager* soundManager, FK_Options* sceneOptions, 
			core::dimension2d<u32> viewportSize, bool forDemoPurposes = false);
		FK_MoveListPanel(IrrlichtDevice* device, FK_SoundManager* soundManager, FK_Options* sceneOptions,
			core::dimension2d<u32> viewportSize,
			FK_Character* reference_character, std::string resourcesPath, std::string skinName, 
			bool forDemoPurposes = false);
		~FK_MoveListPanel();
		void setup(FK_Character* reference_character, std::string resourcesPath);
		void setSkin(std::string skinName);
		void draw(bool drawHelp = true);
		void update(u32 inputButtons, bool updateActionButtons = true);
		bool isActive();
		bool isVisible();
		void setActive(bool new_active_flag);
		void setVisible(bool new_visible_flag);
		void reset(bool resetIndex = true);
		void resetSelectionTime();
		bool hasToBeClosed();
		void setIconsBasedOnCurrentInputMap(FK_InputMapper* mapper, s32 mapIndex,
			bool isJoypad = false, std::string joypadName = std::string());
		bool hasSelectedRootMove();
		FK_Move getSelectedMove();
	protected:
		void loadTextures();
		s32 getWindowY(s32 windowIndex);
		void drawItem(s32 windowIndex);
		s32 drawInputBuffer(s32 index, std::deque<u32> inputBuffer);
		void drawIcon(video::ITexture* icon, s32 x, s32 y, s32 width, s32 height, bool useSmoothing = false);
		void clear();
		void clearWindows();
		void parseMoves(FK_Character* character);
		s32 getFirstWindowX();
		s32 getFirstWindowY();
		bool isWindowVisible(s32 windowIndex);
		void increaseCursor(bool playSound = true);
		void decreaseCursor(bool playSound = true);
		void toNextRootMove(bool playSound = true);
		void toPreviousRootMove(bool playSound = true);
		void selectItem();
		void selectMoveForDemo();
		void cancelSelection();
		void closeMoveList();
		void fillTree(std::vector<u32> buffer, std::vector<std::string> parsedMoves, 
			FK_MoveListMove* nextMove, s32& comboCounter, FK_Move comboMove);
		void toggleBranchMovesVisibility();
		void setupStanceMap(std::map<FK_Stance_Type, std::string> specialStanceDictionary);
		void setupWindows();
		bool objectMovelistInclusionConditionFulfilled(FK_Move& move);
		bool objectConditionFulfilled(FK_Move& move);
		bool bulletCounterConditionFulfilled(FK_Move& move);
	private:
		FK_SoundManager* soundManager;
		FK_Options* gameOptions;
		std::wstring characterName;
		std::string resourcePath;
		std::string skinName;
		std::string skinName_fwp;
		std::string skinName_trg;
		std::vector<FK_Window*> moveWindows;
		std::deque<FK_MoveListMove*> moveList;
		std::deque<FK_Move> moveListTree;
		std::map<FK_Stance_Type, std::wstring> stanceMapForMoveList;
		std::map<std::string, FK_MoveListMove*> moveMap;
		s32 moveIndex;
		gui::IGUIFont* mainFont;
		gui::IGUIFont* notesFont;
		u32 now;
		u32 then;
		IrrlichtDevice* device;
		bool visible_flag;
		bool active_flag;
		u32 inputDelayMS;
		bool selectionFlag;
		bool closeMoveList_flag;
		core::dimension2d<u32> screenSize;
		s32 topLeftCornerX;
		s32 topLeftCornerY;
		s32 panelWidth;
		s32 panelHeight;
		s32 windowSizeY;
		std::map<FK_Input_Buttons, video::ITexture*> buttonTexturesMap;
		std::map<FK_JoypadInput, video::ITexture*> joypadButtonsTexturesMap;
		video::ITexture* keyboardButtonTexture;
		//std::map<FK_Attack_Type, video::ITexture*> hitTypeTextureMap;
		video::ITexture* plusSignTex;
		video::ITexture* impactIconTex;
		video::ITexture* projectileIconTex;
		bool showRootMovesOnly;
		std::map<u32, u32> rootMovesIndex;
		std::map<u32, u32> rootMovesBranches;
		FK_Character* movelistCharacterReference;
		s32 rootMoveSelectedIndex;
		s32 nextRootMoveSelectedIndex;
		std::map<FK_Input_Buttons, u32> buttonInputLayout;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayout;
		bool showJoypadInputs;
		bool demoOnly;
		FK_Move demoMove;
	};
}

#endif