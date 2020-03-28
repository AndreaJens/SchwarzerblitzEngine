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
			SceneGameAttractMode = 1 << 21
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