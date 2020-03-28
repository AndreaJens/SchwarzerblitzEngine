#ifndef FK_SCENEGAME_TUTORIAL_CLASS
#define FK_SCENEGAME_TUTORIAL_CLASS

#include "FK_SceneGameTraining.h"
#include "FK_DialogueWindow.h"
#include <list>

namespace fk_engine{
	class FK_SceneGameTutorial : public FK_SceneGameTraining{
	private:
		const std::string TutorialInputFileName = "phases.txt";
		const u32 TypingSoundInterval = 64;
		const s32 SelectionTimerBufferMS = 200;
		enum FK_TutorialKeywords{
			PhaseBegin,
			PhaseEnd,
			MessageBegin,
			MessageEnd,
			PlayerPath,
			OpponentPath,
			StagePath,
			PhaseTypeBegin,
			PhaseTypeEnd,
			Description,
			InputBegin,
			InputEnd,
			FileEnd,
		};
	public:
		enum FK_TutorialPhase : s32{
			None = -1,
			MovementForward = 1,
			MovementBackwards = 2,
			MovementSidewards = 3,
			Crouching = 4,
			Jumping = 5,
			StandingGuard = 6,
			Punch = 7,
			Kick = 8,
			TechThrow = 9,
			TechSpecial = 10,
			FlowCombo = 11,
			Trigger = 12,
			Ringout = 13,
			GroundRecovery = 14,
			TriggerGuard = 15,
			ImpactRun = 16,
			Run = 17,
			Backstep = 18,
			ThrowEscape = 19,
			ImpactAttacks = 20,
			CrouchedGuard = 21,
			JumpAttacks = 22,
			Projectiles = 23,
			GenericText = 24,
			SpecificMove = 25,
			Ukemi = 26,
			CoinCollect = 27,
			CoinCollectCrouch = 29,
			TriggerRunCancel = 28,
			TriggerReversal = 30,
			TriggerSpecialMove = 31,
			TrueCombo = 32,
		};
		struct TutorialDialogue{
			enum WindowPosition : u32{
				Bottom = 1,
				Center = 2,
				Top = 0
			};
			std::vector<std::string> dialogueText = std::vector<std::string>();
			WindowPosition position = WindowPosition::Bottom;
			bool closeWindowsAfterDialogue = false;
			bool closeSingleWindowAfterDialogue = false;
		};
		struct FK_TutorialTrainingSet{
			FK_TutorialPhase phase = FK_TutorialPhase::None;
			std::string description;
			std::vector<f32> additionalNumericParameters = std::vector<f32>();
			std::vector<std::string> additionalStringParameters = std::vector<std::string>();
			std::vector<TutorialDialogue> messages = std::vector<TutorialDialogue>();
			std::deque<u32> inputArray = std::deque<u32>();
			bool phaseComplete = false;
			FK_Move inputAndHintData = FK_Move();
			u32 numberOfNumericParameters = 0;
			u32 numberOfStringParameters  = 0;
		};
	public:
		FK_SceneGameTutorial();
		void setup(IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			std::string tutorialFileName = std::string(),
			std::string tutorialId = std::string(),
			FK_Scene::FK_SceneTrainingType newChallengeType = FK_Scene::FK_SceneTrainingType::TrainingTutorial,
			u32 challengeIndex = 0);
		void dispose();
		virtual bool isRunning();
		u32 getChallengeIndex();
		FK_Scene::FK_SceneTrainingType getChallengeType();
	protected:
		void createMessageWindow();
		void loadTutorialFlowFile(std::string filename);
		void storeMoveIdsInTrainingSets();
		virtual void updateAdditionalSceneLogic(u32 delta_t_ms);
		virtual void updatePlayer1Input(u32 delta_t_ms, bool& newMove);
		virtual void updatePlayer2Input(u32 delta_t_ms, bool& newMove);
		virtual void drawHUDThrowName(FK_Character* performer);
		void updateMessageInput(u32 delta_t_ms);
		void updateLogicTriggers(u32 delta_t_ms);
		void setAdditionalLogicTriggers();
		virtual void setupAdditionalVariables();
		virtual std::wstring getSceneNameForLoadingScreen();
		virtual void drawAdditionalHUDItems(f32 delta_s);
		virtual bool canUpdateInput();
		virtual bool canPauseGame();
		virtual void drawPauseMenu();
		virtual void updatePauseMenu();
		virtual void activatePauseMenu();
		virtual void resumeGame();
		virtual void updateCharactersPressedButtons(u32 &pressedButtonsPlayer1, u32 &pressedButtonsPlayer2);
		virtual void updateTriggerComboEffect(u32 frameDelta, bool newMovePlayer1, bool newMovePlayer2);
		/* frame advantage*/
		virtual bool calculateFrameAdvantage(s32& frameAdvantage, s32& cancelAdvantage,
			FK_Hitbox * hitbox, FK_Character* attacker, FK_Character* defender,
			bool guardBreak, bool counterAttack, bool defenderHasArmor);
		/* identifiers for sound */
		virtual std::string getEndOfRoundSoundIdentifier(const s32& winnerId, const bool& perfectFlag);
		virtual void drawKOtext(const s32& winnerId, const bool &perfectFlag);
		void checkForPhaseCompletion();
		void markCurrentPhaseAsComplete(bool playSound = true);
		virtual std::vector<std::string> getPauseMenuItems();
		FK_TutorialTrainingSet getCurrentPhase();
		virtual void cancelThrow();
		bool opponentHasToAct();
		void createCollectibles();
		void updateCollectibles();
		void disposeCollectibles();
		void processRoundEnd();
		void processMatchEnd();
		int getRoundWinner(bool updateWins = false);
		int getMatchWinner();
		bool isEndOfRound();
		bool isEndOfMatch();
	private:
		std::string tutorialResourcesPath;
		s32 phaseIndex;
		s32 messageIndex;
		s32 selection_timer_ms;
		u32 playerOutfitId;
		u32 opponentOutfitId;
		std::string playerOutfitName;
		std::string opponentOutfitName;
		u32 typingSoundTimerMS;
		std::string playerPath;
		std::string opponentPath;
		std::string stagePath;
		s32 numberOfPhases;
		std::vector<FK_TutorialTrainingSet> tutorialPhases;
		FK_DialogueWindow* messageWindow;
		core::dimension2d<u32> screenSize;
		bool canSkipDialogue;
		bool switchDialogue;
		/* timers and stuff */
		u32 crouchingTimer;
		u32 numberOfJumps;
		u32 runningTimer;
		u32 numberOfMovesRepeat;
		u32 numberOfHitsOpponent;
		u32 playerSpecificMoveCount;
		FK_Stance_Type oldPlayerStance;
		bool playerJumpStatus;
		bool previousOpponentHitStun;
		f32 previousOpponentLife;
		f32 previousPlayerLife;
		bool previousPlayerHitStun;
		std::string oldOpponentMove;
		std::string currentOpponentMove;
		std::deque<std::string> parriedMoves;
		u32 numberOfHitsGuarded;
		s32 opponentAttackIndex;
		s32 opponentMoveTimer;
		u32 numberOfEvadedThrows;
		std::string tutorialDirectory;
		std::string tutorialId;
		u32 challengeIndex;
		std::string lastMoveName;
		u32 lastMoveId;
		u32 tutorialPhasesSize;
		u32 numberOfMovesToPerform;
		FK_Scene::FK_SceneTrainingType challengeType;
		std::vector<FK_Collectible*> collectibles;
		bool collectiblesCreated;
		//bool player1NewMoveCheck;
		std::string player1MoveToCheck;
	};
}

#endif