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

#ifndef FK_AI_MANAGER_CLASS
#define FK_AI_MANAGER_CLASS

#include"FK_Character.h"
#include"FK_InputBuffer.h"
#include<string>

using namespace irr;

namespace fk_engine{
	class FK_AIManager{
	private:
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
		u32 lastTimeDeltaMs;
		FK_Character::FK_AIArchetypes AIarchetype;
		u32 numberOfChainAttempts;
		s32 lastAIFrameNumber;
		bool reshuffleMoveSinceInvalid;
		bool isAvoidingBullet;
	};
}

#endif