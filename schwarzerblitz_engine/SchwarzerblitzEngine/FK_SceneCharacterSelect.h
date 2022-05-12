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

#ifndef FK_SCENECHARASELECT_CLASS
#define FK_SCENECHARASELECT_CLASS

#include "FK_SceneCharacterSelect_Base.h"
#include "FK_Character.h"
#include "FK_Announcer.h"
#include "FK_SoundManager.h"
#include <vector>
#include <deque>
#include <string>
#include <thread>
#include <atomic>

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{
	class FK_SceneCharacterSelect : public FK_SceneCharacterSelect_Base{
	private:
		const u32 minimumTimeToLoadCharacterMS = 0;
		const u32 maximumNumberOfLoadedMeshs = 5;
		const u32 InputThresholdStageSelectionMs = 150;
	public:
		FK_SceneCharacterSelect();
		virtual ~FK_SceneCharacterSelect();
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions,
			FK_Scene::FK_SceneFreeMatchType newfreeMatchType,
			int inputPlayer1Index = 0, int inputPlayer2Index = 2, 
			u32 inputPlayer1OutfitId = 0, u32 inputPlayer2OutfitId = 0, 
			bool player2isCpu = false, bool player1isCPU = false);
		virtual void initialize();
		virtual void update();
		virtual void dispose();
		virtual FK_Scene::FK_SceneType nameId();
		virtual bool isRunning();
		virtual void setupInputForPlayers();
		virtual bool isCharacterSelectable(FK_Character* character, bool player1Selection);
		int getPlayer1Index();
		int getPlayer2Index();
		std::string getPlayer1Path();
		std::string getPlayer2Path();
		std::string getStagePath();
		int getPlayer1OutfitId();
		int getPlayer2OutfitId();
		FK_Character::FK_CharacterOutfit getPlayer1Outfit();
		FK_Character::FK_CharacterOutfit getPlayer2Outfit();
		std::string getPlayer1MovesetTag();
		std::string getPlayer2MovesetTag();
		bool getRingoutAllowanceFlag();
		FK_Scene::FK_SceneFreeMatchType getFreeMatchType();
	protected:
		void setupCharacters();
		void selectPlayer1Character();
		void selectPlayer2Character();
		void cancelPlayer1CharacterSelection();
		void cancelPlayer2CharacterSelection();
		void selectStage();
		void cancelStageSelection();
		void goBackToMainMenu();
		void updateInput();
		void updateInputPlayerSelection();
		void updateInputStageSelection();
		void updateCharacters();
		void updateStageSelection(bool force_update = false);
		void updateCharacterSelection(bool force_update = false);
		FK_Character* loadCharacter(std::string characterPath,
			core::vector3df position, float rotationAngleY, int &outfit_id, bool lighting = false, 
			bool onlyValidOutfit = false, bool player1IsSelecting = true);
		void loadPlayer1Character(std::string characterPath,
			core::vector3df position, int &outfit_id, f32 playerAngle, bool sameOutfit);
		void loadPlayer2Character(std::string characterPath,
			core::vector3df position, int &outfit_id, f32 playerAngle, bool sameOutfit);
		void loadCharacterList();
		void loadStageList();
		void drawStageScreen();
		void drawCostumeNames(s32 offsetX);
		void drawCostumeIndex(s32 offsetX);
		void drawButtonIcons();
		void drawPlayerNames();
		void drawLockedCostumeSigns();
		void drawBackground();
		void drawAll();
		bool isPlayer1Ready();
		bool isPlayer2Ready();
		bool isStageSelected();
		bool isVsCPU();
		bool canSelectStage();
		bool isP1vsP2();
		bool isP1vsCPU();
		bool isCPUvsP2();
		bool isCPUvsCPU();
		bool player1CanSelect();
		bool player2CanSelect();
		void drawProportionalStageWallbox(s32 stageIndex);
		void drawStageWallbox(s32 stageIndex);
		void toggleRingoutFlag();
		void setupSystemButtonIcons();
	private:
		/* path for players and other stuff*/
		std::string player1Path;
		s32 player1OutfitId;
		s32 player1OldOutfitId;
		std::string player2Path;
		s32 player2OutfitId;
		s32 player2OldOutfitId;
		core::vector2d<f32> offset;
		bool player1Ready;
		bool player2Ready;
		bool stageReady;
		std::vector<std::string> stagePaths;
		std::vector<std::wstring> stageNames;
		std::vector<u32> stageRingoutFlags;
		std::vector<core::aabbox3d<f32> > stageWallboxes;
		std::wstring currentStageName;
		s32 stageIndex;
		s32 oldStageIndex;
		volatile s32 player1Index;
		volatile s32 player2Index;
		volatile s32 oldIndexPlayer1;
		volatile s32 oldIndexPlayer2;
		FK_Character* volatile player1Character;
		FK_Character* volatile player2Character;
		/* threads */
		std::thread loadPlayer1CharacterThread;
		std::thread loadPlayer2CharacterThread;
		/* atomic variables */
		std::atomic<bool> player1CharacterLoaded;
		std::atomic<bool> player2CharacterLoaded;
		std::atomic<bool> player1ThreadProcessing;
		std::atomic<bool> player2ThreadProcessing;
		/* additional variables (e.g. to check scene flow)*/
		u32 cycleId;
		/* transition counter (used when scene ends) */
		u32 transitionCounter;
		/* selection times */
		//u32 lastTimeInputButtonsPlayer1;
		//u32 lastTimeInputButtonsPlayer2;
		/* vsCPU or multiplayer?*/
		bool player1IsCPU;
		bool player2IsCPU;
		/* announcer flag */
		bool stageSelectionAnnounced;
		/* mesh name buffer */
		std::deque<scene::IAnimatedMesh*> meshBuffer;
		std::map<u32, std::vector<FK_Character*> > characterBuffer;
		std::mutex m_threadMutex;
		//
		bool backToMenu;
		std::vector<std::wstring> characterNames;
		// meshloadtimer
		u32 player1SelectionMS;
		u32 player2SelectionMS;
		// selection map
		std::map<FK_Input_Buttons, SelectionDirection> selectionMap;
		// fonts
		gui::IGUIFont* font;
		gui::IGUIFont* capfont;
		gui::IGUIFont* hintfont;
		// kick button texture
		video::ITexture* modifierButtonTexture;
		std::map<FK_Input_Buttons, u32> buttonInputLayoutPlayer1;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayoutPlayer1;
		std::map<FK_Input_Buttons, u32> buttonInputLayoutPlayer2;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayoutPlayer2;
		bool player1UsesJoypad;
		bool player2UsesJoypad;
		bool forcePlayer1Selection;
		bool forcePlayer2Selection;
		// allow ringout
		bool allowRingoutFlag;
		// distance arrays for icons (optimize selection path)
		//std::vector<std::map<SelectionDirection, u32> > nearestCharacterIndex;
		// moveset tags
		std::string player1MovesetTag;
		std::string player2MovesetTag;
		FK_SceneFreeMatchType freeMatchType;
		FK_SelectionPreview player1Preview;
		FK_SelectionPreview player2Preview;
		bool firstSceneRender;
		/* input time */
		u32 lastInputTimePlayer1;
		u32 lastInputTimePlayer2;
	};
}
#endif