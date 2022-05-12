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

#ifndef FK_SCENE_CLASS
#define FK_SCENE_CLASS

#include<string>
#include<irrlicht.h>

using namespace irr;

namespace fk_engine{
	class FK_Scene{
	public:
		const enum FK_SceneType : u32 {
			SceneCharacterSelection = 1,
			SceneOptions = 1 << 1,
			SceneMainMenu = 1 << 2,
			SceneGameGeneric = 1 << 3,
			SceneGameFreeMatch = 1 << 4,
			SceneGameArcade = 1 << 5,
			SceneExtra = 1 << 6,
			SceneStoryModeDialogue = 1 << 8,
			SceneCharacterSelectionArcade = 1 << 7,
			SceneQuitGame = 1 << 17,
			SceneFreeMatchSelection = 1 << 9,
			SceneInputMapping = 1 << 10,
			SceneCredits = 1 << 11,
			SceneIntro = 1 << 12,
			SceneTitle = 1 << 13,
			SceneStoryModeMatch = 1 << 14,
			SceneStoryModeSelection = 1 << 15,
			SceneTrainingSelection = 1 << 18,
			SceneGameTutorial = 1 << 16,
			SceneCharacterSelectionExtra = 1 << 19,
			SceneArcadeCutscene = 1 << 20,
			AnySceneStory = SceneStoryModeMatch | SceneStoryModeDialogue,
			SceneGameAttractMode = 1 << 21,
			SceneMusicPlayer = 1 << 22,
		};
		const enum FK_SceneFreeMatchType : u32{
			FreeMatchMultiplayer = 0,
			FreeMatchPlayer1VsCPU = 1,
			FreeMatchCPUVsPlayer2 = 2,
			FreeMatchCPUVsCPU = 3,
			FreeMatchTraining = 4,
			FreeMatchTutorial = 5,
			FreeMatchTagTeam = 6,
			FreeMatchTeamBattle = 7,
			FreeMatchHumanPlayerVsCPU = 8,
			FreeMatchCPUVsCPUPlayer2Selects = 9,
			FreeMatchCPUVsCPUPlayer1Selects = 10,
		};
		const enum FK_SceneExtraType : u32{
			ExtraNone = 0,
			ExtraGallery = 1,
			ExtraSoundPlayer = 2,
			ExtraProfiles = 3,
			ExtraStages = 4,
			ExtraDiorama = 5,
			ExtraDioramaSelection = 6,
			ExtraGallerySelection = 7,
			ExtraArcadeEndingSelection = 8,
			ExtraArcadeEndingGallery = 9,
			ExtraMusicPlayer = 10,
			ExtraDiscordLink = 11,
			ExtraGachablitz = 12,
		};
		const enum FK_SceneArcadeType : u32{
			ArcadeClassic = 0,
			ArcadeSpecial1 = 1,
			ArcadeSpecial2 = 2,
			ArcadeSpecial3 = 3,
			ArcadeSpecial4 = 4,
			ArcadeSurvival = 5,
			ArcadeTimeAttack = 6, 
		};
		const enum FK_SceneTrainingType : u32 {
			TrainingNormal = 0,
			TrainingTrial = 1,
			TrainingTutorial = 2,
		};
		FK_Scene(){};
		virtual ~FK_Scene() {};
		virtual void initialize() = 0;
		virtual void update() = 0;
		virtual void dispose() = 0;
		virtual FK_SceneType nameId() = 0;
		virtual bool isRunning() = 0;
		virtual FK_SceneType getNextScene() = 0;
		virtual FK_SceneType getPreviousScene() = 0;
		virtual void setNextScene(FK_SceneType) = 0;
		virtual void setPreviousScene(FK_SceneType) = 0;
	};
}
#endif