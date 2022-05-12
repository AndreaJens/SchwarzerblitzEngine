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

#ifndef FK_SCENECHARASELECT_ARCADE_CLASS
#define FK_SCENECHARASELECT_ARCADE_CLASS

#include "FK_SceneCharacterSelect_Base.h"
#include "FK_Character.h"
#include "FK_Announcer.h"
#include "FK_SoundManager.h"
#include "FK_ArcadeCluster.h"
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
	class FK_SceneCharacterSelectArcade : public FK_SceneCharacterSelect_Base{
	public:
		FK_SceneCharacterSelectArcade();
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions,
			FK_SceneArcadeType newArcadeType,
			bool player1IsActiveFlag,
			int inputPlayerIndex = -1,
			u32 inputPlayer1OutfitId = 0);
		virtual void initialize();
		virtual void update();
		virtual void dispose();
		virtual FK_Scene::FK_SceneType nameId();
		virtual bool isRunning();
		std::vector<FK_ArcadeCluster> buildArcadeArray();
		std::vector<s32> buildAILevelArray(s32 basicLevel, s32 numberOfMatches, f32 lowVal = 0.8f, f32 highVal = 1.5f);
		bool isPlayer1Active();
		int getPlayerIndex();
		std::string getPlayerPath();
		int getPlayerOutfitId();
		FK_Character::FK_CharacterOutfit getPlayerOutfit();
		FK_Scene::FK_SceneArcadeType getArcadeType();
		virtual void setupInputForPlayers();
	protected:
		bool isCharacterSelectable(FK_Character* character);
		void setupSystemButtonIcons();
		virtual void loadCharacterList();
		void setupCharacters();
		void selectPlayerCharacter();
		void goBackToMainMenu();
		void updateInput();
		void updateInputPlayerSelection();
		void updateCharacter();
		void updateCharacterSelection(bool force_update = false);
		void finalizeCharacterList();
		FK_Character* loadCharacter(u32 characterIndex, std::string characterPath,
			core::vector3df position, float rotationAngleY, int &outfit_id, bool lighting = false, bool onlyValidOutfits = true);
		void loadPlayerCharacter(std::string characterPath,
			core::vector3df position, int &outfit_id, f32 playerAngle, bool changeOnlyOutfit);
		virtual void loadSingleCharacterOutfitList(u32 characterId, FK_Character* character);
		std::vector<std::string> loadStageList();
		virtual void drawCompletedArcadeIcon();
		void drawPlayerNames();
		void drawButtonIcon();
		void drawLockedCostumeSigns();
		void drawCostumeIndex(s32 sentenceOffsetX);
		void drawAll();
		bool isPlayerReady();
		// update input buffer
		void updateInputBuffer();
		FK_InputBuffer* getCurrentInputBuffer();
		std::vector<std::wstring> characterNames;
		void drawArcadeLadderLoadingScreen(f32 completionPercentage);
		std::vector<FK_ArcadeCluster> buildClassicArcadeArray();
		std::vector<FK_ArcadeCluster> buildSurvivalArcadeArray();
		bool checkIfEndingIsAvailable(std::string& characterPath);
	protected:
		std::map<u32, std::vector<std::pair<u32, bool>> > characterAvailableCostumes;
		bool player1IsActive;
		std::map<u32, bool> characterHasAvailableEndingMap;
		void drawSurvivalRecord();
		void drawTimeAttackRecord();
		void drawNoEndingDisclaimer();
	private:
		// selection map
		std::map<FK_Input_Buttons, SelectionDirection> selectionMap;
		/* path for players and other stuff*/
		std::string playerPath;
		s32 playerOutfitId;
		s32 playerOldOutfitId;
		bool playerReady;
		std::vector<u32> availableCharactersArcade;
		std::vector<std::string> characterPathsArcade;
		volatile s32 playerIndex;
		volatile s32 oldPlayerIndex;
		FK_Character* volatile playerCharacter;
		/* additional variables (e.g. to check scene flow)*/
		u32 cycleId;
		/* transition counter (used when scene ends) */
		u32 transitionCounter;
		u32 lastInputTime;
		/* mesh name buffer */
		std::deque<scene::IAnimatedMesh*> meshBuffer;
		std::map<u32, std::vector<FK_Character*> > characterBuffer;
		//
		bool backToMenu;
		bool forcePlayerSelection;
		bool firstSceneRender;
		FK_Scene::FK_SceneArcadeType arcadeType;
		FK_SelectionPreview playerPreview;
		bool playerUsesJoypad;
		std::map<FK_Input_Buttons, u32> buttonInputLayout;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayout;
		std::vector<bool> characterHasCompletedArcade;
	};
}
#endif