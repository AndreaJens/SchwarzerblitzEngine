#ifndef FK_SCENEGAME_STORY_CLASS
#define FK_SCENEGAME_STORY_CLASS

#include"FK_SceneGame.h"
#include"FK_StoryFlowCluster.h"
#include"FK_DialogueWindow.h"

namespace fk_engine{
	class FK_SceneGameStory : public FK_SceneGame{
	public:
		FK_SceneGameStory();
		void setup(IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			FK_StoryMatch* newMatchSettings);
		bool hasCanceledMatch();
		bool canUpdateInput();
		void dispose();
	protected:
		void drawLoadingScreen(f32 completionPercentage);
		void loadLoadingTextures();
		std::vector<std::string> getPauseMenuItems();
		void updateAdditionalSceneLogic(u32 delta_t_ms);
		bool updateAIInput(u32 delta_t_ms, u32 playerID);
		void drawAdditionalHUDItems(f32 delta_t_s);
		void setupAdditionalVariables();
		void setupAdditionalParametersAfterLoading();
		std::wstring getSceneNameForLoadingScreen();
		void setupImpactEffect(FK_Reaction_Type reaction, s32 playerId);
		bool showWinText();
		void processMatchEnd();
		void updateSaveFileData();
		int getRoundWinner(bool updateWins = false);
		bool isEndOfRound();
		void setupMatchWinConditionWindow();
		bool showRoundIndicatorsHUD();
		/* identifiers for sound */
		virtual std::string getEndOfRoundSoundIdentifier(const s32& winnerId, const bool& perfectFlag);
		virtual void drawKOtext(const s32& winnerId, const bool &perfectFlag);
	private:
		/* arcade parameters */
		u32 arcadeRoundNumber;
		FK_StoryMatch* matchSettings;
		FK_InMatchDialogue dialogue;
		bool dialogueInMatch;
		FK_DialogueWindow* winConditionDialogueWindow;
	};
}

#endif