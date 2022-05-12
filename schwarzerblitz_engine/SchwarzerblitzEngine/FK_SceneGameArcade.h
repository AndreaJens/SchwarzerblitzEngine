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

#ifndef FK_SCENEGAME_ARCADE_CLASS
#define FK_SCENEGAME_ARCADE_CLASS

#include"FK_SceneGame.h"
#include"FK_ArcadeCluster.h"
#include"FK_StoryFlowCluster.h"

namespace fk_engine{
	class FK_SceneGameArcade : public FK_SceneGame{
	public:
		FK_SceneGameArcade();
		void setup(IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			FK_Scene::FK_SceneArcadeType newArcadeType,
			std::string new_player1path,
			std::string new_player2path,
			FK_Character::FK_CharacterOutfit new_outfitPlayer1,
			FK_Character::FK_CharacterOutfit new_outfitPlayer2,
			std::string new_arenaPath,
			bool player1isPlaying,
			u32 AI_level,
			u32 matchNumber,
			FK_AdditionalSceneGameOptions newAdditionalOptions,
			FK_ArcadeProgress currentArcadeProgress);
		bool player1isActivePlayer();
		u32 getArcadeMatchNumber();
		void setArcadeMatchNumber(u32 matchNumber);
		void setArcadeLastMatch();
		FK_Scene::FK_SceneArcadeType getArcadeType();
		FK_StoryEvent getArcadeEnding();
		FK_ArcadeProgress getCurrentArcadeProgress();
	protected:
		void drawLoadingScreen(f32 completionPercentage);
		void drawFinalMatchLoadingScreen(f32 completionPercentage);
		void loadLoadingTextures();
		void updateAdditionalSceneLogic(u32 delta_t_ms);
		void drawAdditionalHUDItems(f32 delta_t_s);
		void setupAdditionalVariables();
		void setupAdditionalParametersAfterLoading();
		std::wstring getSceneNameForLoadingScreen();
		void setupImpactEffect(FK_Reaction_Type reaction, s32 playerId);
		bool showWinText();
		bool checkForNewRecord();
		void processMatchEnd();
		void loadArcadeConfigFile();
		void updateSaveFileData();
		void updateVisualEffectsLastMatch(u32 delta_t_ms);
		void updateRoundTimer(u32 delta_t_ms);
		void processAdditionalGameOverRoutine();
	private:
		/* arcade parameters */
		u32 arcadeRoundNumber;
		bool arcadeLastMatch_flag;
		bool climaticBattle_flag;
		bool player1IsPlaying_flag;
		video::ITexture* lastRoundLoadingTex;
		std::string arcadeAddonsPath;
		std::wstring arcadeMatchTagline;
		std::string arcadeSpecialMatchSplashscreenTexture;
		FK_ArcadeSettings arcadeSettings;
		FK_ArcadeFlow characterArcadeFlow;
		bool resetRoundsForClimatic;
		u32 interfaceScrewRandomCounter;
		FK_Scene::FK_SceneArcadeType arcadeType;
		FK_StoryEvent arcadeEnding;
		FK_ArcadeProgress arcadeProgress;
		FK_ArcadeSpecialMatch currentSpecialMatch;
		bool forceNewRecordMessage;
	};
}

#endif