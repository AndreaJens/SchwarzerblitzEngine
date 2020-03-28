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