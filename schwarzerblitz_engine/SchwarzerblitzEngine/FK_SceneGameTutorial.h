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