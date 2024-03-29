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

#ifndef FK_SCENEGAME_TRAINING_CLASS
#define FK_SCENEGAME_TRAINING_CLASS

#include"FK_SceneGameFreeMatch.h"

namespace fk_engine{
	class FK_SceneGameTraining : public FK_SceneGameFreeMatch{
	private:
		s32 InputCooldownForResetMs = 200;
		s32 MaxRecordTimeMs = 10000;
		u32 MaxTimeBeforeComboBecomesInvalidMs = 500;
		struct FK_TrainingInputRecordUnit {
			std::deque<u32> buffer;
			s32 time = 0;
			u32 pressedButtons = 0;
		};
	public:
		enum FK_TrainingOptionsDummy : u32{
			StandingDummy = 0,
			CrouchingDummy = 1,
			JumpingDummy = 2,
			WalkingDummy = 8,
			WalkBackwardDummy = 9,
			JumpingForwardDummy = 3,
			JumpingBackwardDummy = 4,
			RandomJump = 5,
			BackgroundSidestepDummy = 6,
			ForegroundSidestepDummy = 7,
			AIControlledDummy = 10,
			Player2ControlledDummy = 11,
			RecordDummy = 12,
		};
		enum FK_GuardOptionsDummy : u32{
			NoGuard = 0,
			RandomGuard = 1,
			AlwaysGuard = 2,
			OnlyStandingGuard = 3,
			OnlyCrouchingGuard = 4,
			GuardAfterFirstHit = 5,
		};
		enum FK_RecoveryOptionsDummy : u32{
			NoRecovery = 0,
			//Ukemi = 6,
			//BackgroundUkemi = 7,
			BackgroundRotation = 1,
			ForegroundRotation = 2,
			AdvanceRecovery = 3,
			BackwardsRecovery = 4,
			RandomRecovery = 8,
			CrouchingRecovery = 9,
			Reversal = 5,
			FrontRollRecovery = 6,
			BackRollRecovery = 7,
			TutorialRecovery = 99,
		};
		enum FK_UkemiOptionsDummy : u32 {
			NoUkemi = 0,
			FGUkemi = 1,
			BGUkemi = 2,
			FrontRollUkemi = 3,
			BackRollUkemi = 4,
			StandUpRecovery = 5,
			RandomUkemi = 6
		};
		enum FK_AfterGuardOptionsDummy : u32 {
			NoAfterGuardAction = 0,
			BackstepAfterGuard = 1,
			SidestepForegroundAfterGuard = 2,
			SidestepBackgroundAfterGuard = 3,
			RecordedActionAfterGuard = 4,
			MoveFromListAfterGuard = 5,
		};
		class FK_TrainingOptionsMenu{
		public:
			enum FK_TrainingOption : u32{
				DummyAction,
				DummyRecovery,
				DummyUkemi,
				DummyGuard,
				DummyAfterGuard,
				DummyAILevel,
				ReturnToMenu,
				CharacterSelect,
				NewPlayer1Path,
				NewPlayer2Path,
				NewStagePath,
				FreeCameraMode,
				MoveList,
				Resume,
			};
			FK_TrainingOptionsMenu();
			~FK_TrainingOptionsMenu();
			void draw();
			void update(u32 inputButtons);
			void setup(IrrlichtDevice* new_device);
			FK_TrainingOption getCurrentItem();
			void setCurrentTimeAsInputTime();
			void toggleItemActivity(FK_TrainingOption option, bool newStatus);
			bool decreaseCurrentItem();
			bool increaseCurrentItem();
			void increaseIndex();
			void decreaseIndex();
			void setViewport(core::dimension2d<u32> new_Viewport);
			bool canAcceptInput();
			u32 getParameterValue(FK_TrainingOption option);
		private:
			std::map<FK_TrainingOption, bool> itemActiveStatus;
			std::map<FK_TrainingOption, std::wstring> menuItems;
			std::map < FK_TrainingOption, std::vector<std::pair<u32, std::wstring>>> menuItemsParameters;
			std::vector<s32> drawingGaps;
			std::vector<FK_TrainingOption> menuList;
			std::vector<s32> menuItemsIndices;
			std::map<FK_TrainingOption, s32> menuOptionsIndices;
			s32 menuIndex;
			IrrlichtDevice* device;
			gui::IGUIFont* mainFont;
			gui::IGUIFont* textFont;
			core::dimension2d<u32> screenSize;
			f32 scaleFactorX;
			f32 scaleFactorY;
			u32 lastInputTime;
			u32 inputTimeThreshold;
		};
	private:
		const u32 damageLastingTimeTrainingMS = 1500;
	public:
		FK_SceneGameTraining();
		void setup(IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			std::string new_player1path,
			std::string new_player2path,
			FK_Character::FK_CharacterOutfit new_outfitPlayer1,
			FK_Character::FK_CharacterOutfit new_outfitPlayer2,
			std::string new_arenaPath,
			FK_SceneGame::FK_AdditionalSceneGameOptions newAdditionalOptions);
	protected:
		// methods to be overloaded from subclasses
		virtual void processEndOfRoundStatistics() override;
		virtual void setupInputForPlayers();
		virtual void applyDamageToPlayer(FK_Character* player, f32 damage, bool affectsObjects = true);
		virtual void updateAdditionalSceneLogic(u32 delta_t_ms);
		virtual void drawAdditionalHUDItems(f32 delta_t_s);
		virtual void drawHUDThrowName(FK_Character* performer);
		virtual void setupAdditionalParametersAfterLoading();
		virtual void setupAdditionalVariables();
		virtual void setupHUD();
		virtual void setupHUDManagerInputIcons();
		virtual std::wstring getSceneNameForLoadingScreen();
		virtual void processNewRoundStart();
		virtual void processRoundEnd();
		virtual bool showRoundIndicatorsHUD();
		virtual void updatePlayer1Input(u32 delta_t_ms, bool& newMove);
		virtual void updatePlayer2Input(u32 delta_t_ms, bool& newMove);
		virtual void updateCharactersPressedButtons(u32 &pressedButtonsPlayer1, u32 &pressedButtonsPlayer2);
		virtual void drawPauseMenu();
		virtual void updatePauseMenu();
		virtual void activatePauseMenu();
		virtual void resumeGame();
		virtual void disposeAdditionalVariables();
		virtual void initialize();
		/* pause menu */
		virtual void resetInputTimeForPauseMenu();
		virtual void updateInputForRecordAndReplayMode(const u32 &inputButtons, u32 delta_t_ms);
		virtual void updateRecordAndReplayFlow(u32 delta_t_ms); 
		virtual void addMoveHitFlag(ISceneNode* hitboxNode, FK_Hitbox* hitboxData,
			s32 frameAdvantage, s32 cancelAdvantage, bool showframeAdvantage);
		/* frame advantage*/
		virtual bool calculateFrameAdvantage(s32& frameAdvantage, s32& cancelAdvantage,
			FK_Hitbox * hitbox, f32 histunMultiplier, FK_Character* attacker, FK_Character* defender,
			bool guardBreak, bool counterAttack, bool defenderHasArmor) override;
		// process character stats
		virtual void processCharacterStats() override;
	protected:
		FK_RecoveryOptionsDummy dummyRecoveryOptions;
		s32 blackScreenResetCounter = 0;
	private:
		void loadSystemIcons();
		void updateDemoMoveList();
		u32 trainingCounterMS;
		u32 groundedCounterMS;
		u32 groundedDurationMS;
		//u32 jumpDurationMS;
		//u32 jumpCounterMS;
		bool updateGroundedTimer;
		bool updateGuardDuration;
		u32 guardDurationCounterMS;
		u32 currentPressedButtons;
		u32 guardDurationMS;
		FK_TrainingOptionsDummy dummyMovementOptions;
		FK_GuardOptionsDummy dummyGuardOptions;
		FK_AfterGuardOptionsDummy dummyAfterGuardOptions;
		FK_RecoveryOptionsDummy dummyRecoveryOptionsToCheck;
		FK_UkemiOptionsDummy dummyUkemiOptions;
		FK_TrainingOptionsMenu* trainingMenu;
		FK_Move afterGuardMove;
		bool dummyWasHitOnceInCurrentCombo;
		bool hasBlockedAnAttack;
		s32 dummyHitOnceMsCounter;
		//std::vector<std::string> stagePaths;
		//std::vector<std::wstring> stageNames;
		//std::vector<std::string> characterPaths;
		//std::vector<std::wstring> characterNames;
		bool isRecordingDummyActions;
		bool isControllingPlayer2ForRecording;
		bool isReplayingDummyActions;
		bool refreshBuffer;
		bool showMoveListForDemoMode;
		s32 inputCooldownForReset;
		s32 recordTimerMs;
		s32 recordReplayInputIndexMs;
		std::vector <FK_TrainingInputRecordUnit> recordBuffer;
		u32 recordBufferSize;
		std::deque<u32> recordedInputBuffer;
		std::map<FK_JoypadInput, video::ITexture*> joypadButtonsTexturesMap;
		video::ITexture* keyboardButtonTexture;
		FK_MoveListPanel* demoMoveList;
	};
}

#endif