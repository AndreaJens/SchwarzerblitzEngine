#ifndef FK_AI_MANAGER_CLASS
#define FK_AI_MANAGER_CLASS

#include"FK_Character.h"
#include"FK_InputBuffer.h"
#include<string>

using namespace irr;

namespace fk_engine{
	class FK_AIManager{
	private:
		const std::string AdvancedAISettingsFileName = "AI_settings.txt";
		enum AdvancedAISettingsKeys {
			AdvAI_ComboStringBegin,
			AdvAI_ComboStringEnd,
		};
		struct AI_ComboString {
			std::vector<u32> movesId;
			u32 firstMoveId = 0;
			bool notDuringTrigger = false;
			bool onlyDuringTrigger = false;
		};
	public:
		FK_AIManager(FK_Character* new_AI_player, FK_Character* new_target_player, int AILevel, u32 numberOfrounds);
		~FK_AIManager();
		void reset();
		void updateWinNumbers(u32 AIplayerRoundWon, u32 targetPlayerRoundWon);
		bool updateAIInput(u32 delta_t_ms, u32 gameTimer);
		u32 getAIButtonPressed();
		u32 getAIThrowAttemptEscapeButtons();
		void increaseSufferedThrowsCounter();
		void resetSufferedThrowsCounter();
		void setAILevel(u32 newLevel);
	protected:
		void loadAIAdvancedSettings();
		void getAvailableMovesIndices(std::vector<int>& indices, FK_Stance_Type stance, f32 distanceValue);
		bool rangeConditionFulfilled(FK_Move& move, f32 distance);
		bool stanceConditionFulfilled(FK_Move& move, FK_Stance_Type stance);
		bool objectConditionFulfilled(FK_Move& move);
		bool followupConditionFulfilled(FK_Move& move);
		bool targetConditionFulfilled(FK_Move& move);
		bool bulletCounterConditionFulfilled(FK_Move& move);
		bool basicConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool scheduledMoveConditionsAreFulfilled(FK_Move& move, f32 distance);
		bool additionalConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool additionalAggressorConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool additionalTacticianConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool additionalBalancedConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool additionalLuchadorConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool additionalJackOfAllTradesConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance);
		bool approachingMovementConditionsAreFulfilled(f32 distance);
		bool jumpingConditionsAreFulfilled(f32 distance);
		bool additionalRunningConditionsAreFulfilled(f32 distance);
		bool projectileConditionsFulfilled(FK_Move& move);
		bool runningConditionsAreFulfilled(FK_Move& move, f32 distance);
		u32 getChainLikelihood();
		u32 getGuardLikelihood();
		u32 getTriggerLikelihood();
		u32 getTriggerAttackLikelihood();
		u32 getAfterMoveCooldown();
		u32 getThrowEscapeLikelihood();
		u32 getJumpLikelihood();
		bool isShowingBackToOpponent();
		bool tooNearToOpponent();
		bool isWinningRound();
		bool isLosingRound();
		bool isWinningMatch();
		bool isLosingMatch();
		bool isOneRoundToVictory();
		bool isOneRoundToLoss();
		bool isAlmostTimeUp();
	private:
		FK_Character* AIPlayer;
		FK_Character* targetPlayer;
		std::deque<FK_Input_Buttons> AIInputBuffer;
		//std::vector<int> longRangeMovesIndices;
		u32 timeCounter;
		u32 cooldownTimeCounter;
		bool canChainCombo;
		bool canDefend;
		std::string targetPlayerCurrentMoveName;
		std::string AIPlayerCurrentMoveName;
		u32 AIPlayerCurrentMoveId;
		u32 targetPlayerCurrentMoveId;
		int AILevel;
		u32 scheduledMoveFrame;
		s32 scheduledMoveIndex;
		u32 buttonTimeCounterMS;
		u32 lastInput;
		u32 minimalButtonPressTimeMS;
		u32 numberOfRounds;
		u32 numberOfRoundsWon;
		u32 numberOfRoundsLost;
		u32 currentTimer;
		u32 numberOfSufferedThrows;
		FK_Character::FK_AIArchetypes AIarchetype;
		u32 numberOfChainAttempts;
		bool reshuffleMoveSinceInvalid;
		bool isAvoidingBullet;
	};
}

#endif