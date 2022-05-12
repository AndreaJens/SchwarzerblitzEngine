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