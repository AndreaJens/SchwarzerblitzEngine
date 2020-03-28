#include"FK_AIManager.h"
#include<iostream>
#include<algorithm>

using namespace irr;

namespace fk_engine{

	/* initialization*/
	FK_AIManager::FK_AIManager(FK_Character* new_AI_player, FK_Character* new_target_player, int new_AILevel, u32 new_numberOfRounds){
		AIPlayer = new_AI_player;
		targetPlayer = new_target_player;
		AILevel = new_AILevel;
		numberOfRounds = new_numberOfRounds;
		numberOfRoundsWon = 0;
		numberOfRoundsLost = 0;
		currentTimer = 100 * 1000;
		reshuffleMoveSinceInvalid = false;
		AIarchetype = AIPlayer->getAIArchetype();
		isAvoidingBullet = false;
		reset();
	}
	/* destructor*/
	FK_AIManager::~FK_AIManager(){
		reset();
		AIPlayer = NULL;
		targetPlayer = NULL;
	}
	/* reset variables*/
	void FK_AIManager::reset(){
		AIInputBuffer.clear();
		cooldownTimeCounter = 0;
		timeCounter = 0;
		canChainCombo = true;
		canDefend = true;
		targetPlayerCurrentMoveName = std::string();
		AIPlayerCurrentMoveId = 0;
		AIPlayerCurrentMoveName = std::string();
		targetPlayerCurrentMoveId = 0;
		scheduledMoveIndex = -1;
		scheduledMoveFrame = 0;
		lastInput = 0;
		minimalButtonPressTimeMS = 200;
		buttonTimeCounterMS = minimalButtonPressTimeMS + 1;
		numberOfSufferedThrows = 0;
		numberOfChainAttempts = 0;
	}

	//! set AI level
	void FK_AIManager::setAILevel(u32 newLevel){
		AILevel = newLevel;
	}

	//! increase suffered throw counter (used in throw evasion likelihood)
	void FK_AIManager::increaseSufferedThrowsCounter(){
		numberOfSufferedThrows += 1;
	}

	//! reset suffered throw counter (used in throw evasion likelihood)
	void FK_AIManager::resetSufferedThrowsCounter(){
		numberOfSufferedThrows = 0;
	}

	//! followup move likelihood
	u32 FK_AIManager::getChainLikelihood(){
		/* base value = 10% of likelihood to keep on chaining attacks at level 0*/
		s32 baseValue = 5;
		/* increment the percentage by 7% for each level above 0*/
		s32 incrementPerLevel = 7;
		/* decrement likelihood by 10% for each failed attempt */
		s32 failureDecrement = 5;
		s32 value = baseValue + AILevel*incrementPerLevel - numberOfChainAttempts * failureDecrement;
		if (value < 0){
			value = 0;
		}
		return value;
	}
	//! guard likelihood
	u32 FK_AIManager::getGuardLikelihood(){
		/* base value = 5% of likelihood to keep on guarding attacks at level 0*/
		u32 baseValue = 5;
		/* increment the percentage by 5% for each level above 0*/
		u32 incrementPerLevel = 5;
		if (AILevel > 4){
			incrementPerLevel = 7;
		}
		u32 total = baseValue + AILevel*incrementPerLevel;
		/* do not increase odds above 40% */
		if (total > 75){
			return 75;
		}
		return total;
	}
	//! trigger guard likelihood
	u32 FK_AIManager::getTriggerLikelihood(){
		if (targetPlayer->isKO()){
			return 0;
		}
		/* base value = 35% of likelihood to keep on trigger attacks at level 0*/
		u32 baseValue = 5;
		/* increment the percentage by 15% for each level above 0*/
		u32 incrementPerLevel = 5;
		u32 total = baseValue + AILevel*incrementPerLevel;
		if (AIPlayer->isHitStun() && targetPlayer->getCurrentMove() != NULL &&
			(2*targetPlayer->getCurrentMove()->getTotalMoveDamage()) > AIPlayer->getLife()) {
			return 3 * total;
		}else if(AIPlayer->isHitStun() && targetPlayer->getCurrentMove() != NULL &&
			targetPlayer->getCurrentMove()->isImpactAttack() && AIPlayer->getInStageRingoutZoneFlag()) {
			return 3 * total;
		}
		if (total > 25){
			return 25;
		}
		return total;
	}
	//! trigger attack likelihood
	u32 FK_AIManager::getTriggerAttackLikelihood(){
		if (targetPlayer->isKO()){
			return 0;
		}
		/* base value = 35% of likelihood to keep on trigger attacks at level 0*/
		u32 baseValue = 2;
		/* increment the percentage by 15% for each level above 0*/
		u32 incrementPerLevel = 3;
		u32 total = baseValue + AILevel*incrementPerLevel;
		/* do not increase odds above 40% */
		if (total > 10){
			total = 10;
		}
		if (AIPlayer->getTriggerCounters() >= fk_constants::FK_MaxTriggerCounterNumber){
			total = total * 3 / 2;
		}
		if (isLosingRound()){
			total = total * 3 / 2;
			if (isOneRoundToVictory() || isOneRoundToLoss()){
				total *= 2;
				if (isAlmostTimeUp()){
					total *= 2;
				}
			}
		}
		return total;
	}

	//! get jump likelihood
	u32 FK_AIManager::getJumpLikelihood(){
		return 30;
	}

	//! get likelihood of escaping throws
	u32 FK_AIManager::getThrowEscapeLikelihood(){
		f32 baseValue = 2;
		f32 incrementPerHalfLevelSquare = 0.8f;
		baseValue += (f32)(incrementPerHalfLevelSquare * AILevel * AILevel) / 2;
		if (baseValue > 75.0f){
			baseValue = 75.0f;
		}
		if (AIPlayer->getStance() == FK_Stance_Type::WakeUpStance){
			baseValue *= 2;
		}
		baseValue *= (1 + 2 * (f32)numberOfSufferedThrows);
		return (u32)ceil(baseValue);
	}

	//! get waiting time after move ends
	u32 FK_AIManager::getAfterMoveCooldown(){
		/* base value = 600 ms of time delay between moves at level 0 */
		u32 baseValue = 2000;
		/* decrement the percentage by 75ms for each level above 0*/
		u32 decrementPerLevelUntil5 = 200;
		u32 decrementPerLevelAbove5 = 70;
		u32 decrement = 0;
		if (AILevel > 5){
			decrement = 5 * decrementPerLevelUntil5 + (AILevel - 5) * decrementPerLevelAbove5;
		}
		else{
			decrement = AILevel * decrementPerLevelUntil5;
		}
		u32 maxDelay = baseValue - decrement;
		if (decrement >= baseValue)
			return 0;
		u32 delay = maxDelay / 2 + (u32)std::floor((f32)maxDelay * rand() / RAND_MAX / 2);
		return delay;
	}

	bool FK_AIManager::tooNearToOpponent(){
		core::vector3df distance = targetPlayer->getPosition() - AIPlayer->getPosition();
		f32 distanceValue = distance.getLength();
		return distanceValue < 35.0;
	}

	/* check if the AI-controlled player is showing is back to the opponent */
	bool FK_AIManager::isShowingBackToOpponent(){
		/* TO FIX */
		//return false;
		float temp_ang = AIPlayer->getRotation().Y;
		core::vector2df normal1 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
		temp_ang = targetPlayer->getRotation().Y;
		core::vector2df normal2 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
		temp_ang = (targetPlayer->getPosition() - AIPlayer->getPosition()).getHorizontalAngle().Y;
		core::vector2df connectionDirection = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
		f32 attackDirection = normal1.dotProduct(connectionDirection);
		bool targetIsShowingBack = normal1.dotProduct(normal2) > 0;
		//bool targetIsShowingBack = attackDirection < 0.2;
		return targetIsShowingBack;
	}

	bool FK_AIManager::basicConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		return (
			runningConditionsAreFulfilled(move, distance) &&
			followupConditionFulfilled(move) &&
			rangeConditionFulfilled(move, distance) &&
			targetConditionFulfilled(move) &&
			stanceConditionFulfilled(move, stance) &&
			objectConditionFulfilled(move) &&
			bulletCounterConditionFulfilled(move) &&
			projectileConditionsFulfilled(move)
			);
	}

	bool FK_AIManager::scheduledMoveConditionsAreFulfilled(FK_Move& move, f32 distance) {
		return (move.getMaxRange() >= distance &&
			objectConditionFulfilled(move) &&
			bulletCounterConditionFulfilled(move) &&
			targetConditionFulfilled(move));
	}

	bool FK_AIManager::rangeConditionFulfilled(FK_Move& move, f32 distance){
		if (move.isTaunt() && (distance < move.getMinRange() || !targetPlayer->isGrounded())){
			return false;
		}
		if (targetPlayer->isTriggerComboed()/* || targetPlayer->isHitStun()*/){
			return move.getMaxRange() >= distance;
		}
		if ((AIPlayer->getCurrentMove() != NULL && (
			AIPlayer->getCurrentMove()->isThrow() ||
			(targetPlayer->isHitStun() && !targetPlayer->isGuarding()))) ||
			((targetPlayer->getStance() & FK_Stance_Type::AnyGroundedStance) != 0 ||
			targetPlayer->getStance() == FK_Stance_Type::WakeUpStance)
			){
			return move.getMaxRange() >= distance;
		}
		return (move.getMaxRange() >= distance &&
				move.getMinRange() <= distance);
	}
	bool FK_AIManager::stanceConditionFulfilled(FK_Move& move, FK_Stance_Type stance){
		if (AIPlayer->getCurrentMove() != NULL) {
			return true;
		}
		return (move.getStance() == stance || move.getStance() == AIPlayer->getStance());
	}

	bool FK_AIManager::followupConditionFulfilled(FK_Move& move){
		return (!(move.isFollowupOnly() && AIPlayer->getCurrentMove() == NULL));
	}

	bool FK_AIManager::objectConditionFulfilled(FK_Move& move){
		/* if the move has object requirements, check them before proceeding */
		bool moveAvailable = true;
		if (move.getPickableItemId() != std::string() &&
			!move.canPickUpItem(AIPlayer->getPickableObjectId()) &&
			!move.isMoveAvailableWithoutPickup()) {
			return false;
		}
		if (move.hasItemRequirements()){
			bool moveAvailable = true;
			u32 countForbidden = move.getForbiddenObjects().size();
			u32 countRequired = move.getRequiredObjects().size();
			for (u32 i = 0; i < countRequired + countForbidden; ++i) {
				if (i < countRequired) {
					moveAvailable &= (AIPlayer->getObjectByName(move.getRequiredObjects().at(i)) != NULL &&
						AIPlayer->getObjectByName(move.getRequiredObjects().at(i))->isActive());
				}
				else {
					u32 k = i - countRequired;
					moveAvailable &= (AIPlayer->getObjectByName(move.getForbiddenObjects().at(k)) != NULL &&
						!AIPlayer->getObjectByName(move.getForbiddenObjects().at(k))->isActive());
				}
				if (!moveAvailable) {
					return false;
				}
			}
			//u32 count = AIPlayer->getCharacterAdditionalObjects().size();
			//for (u32 i = 0; i < count; ++i){
			//	std::string nameTag = AIPlayer->getCharacterAdditionalObjects().at(i).uniqueNameId;
			//	if (nameTag != std::string()){
			//		if (move.isObjectRequired(nameTag)){
			//			moveAvailable &= AIPlayer->getCharacterAdditionalObjects().at(i).isActive();
			//		}
			//		else if (move.isObjectForbidden(nameTag)){
			//			moveAvailable &= !AIPlayer->getCharacterAdditionalObjects().at(i).isActive();
			//		}
			//		// check if move can pick up object only
			//		if (move.canPickUpItem(nameTag) &&
			//			AIPlayer->getPickableObjectId() != nameTag &&
			//			!move.canBeTriggered()){
			//			moveAvailable = false;
			//		}
			//	}
			//	if (!moveAvailable){
			//		return false;
			//	}
			//}
		}
		return moveAvailable;
	}

	bool FK_AIManager::targetConditionFulfilled(FK_Move& move){
		if (move.AIonlyDuringOpponentAttack() && !targetPlayer->hasActiveMove()) {
			return false;
		}
		if (move.canBeUsedOnlyVsAirborneOpponents() && !targetPlayer->isAirborne()) {
			return false;
		}
		bool groundedFlag = targetPlayer->isGrounded() && move.canBeUsedVsGroundedOpponent();
		bool standingFlag = targetPlayer->isHitStun() || !(targetPlayer->isGrounded() || targetPlayer->getStance() == FK_Stance_Type::CrouchedStance);
		bool crouchingFlag = targetPlayer->getStance() == FK_Stance_Type::CrouchedStance && move.canBeUsedVsCrouchedOpponent();
		return (groundedFlag || standingFlag || crouchingFlag);
	}

	bool FK_AIManager::bulletCounterConditionFulfilled(FK_Move& move){
		if (move.getNumberOfRequiredBulletCounters() > 0){
			if (targetPlayer->isKO() || AIPlayer->getTriggerCounters() < move.getNumberOfRequiredBulletCounters()){
				return false;
			}
		}
		return true;
	}

	bool FK_AIManager::additionalConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		switch (AIarchetype){
		case FK_Character::FK_AIArchetypes::Aggressor:
			return additionalAggressorConditionsAreFulfilled(move, stance, distance);
		case FK_Character::FK_AIArchetypes::Tactician:
			return additionalTacticianConditionsAreFulfilled(move, stance, distance);
		case FK_Character::FK_AIArchetypes::Balanced:
			return additionalBalancedConditionsAreFulfilled(move, stance, distance);
		case FK_Character::FK_AIArchetypes::Luchador:
			return additionalLuchadorConditionsAreFulfilled(move, stance, distance);
		case FK_Character::FK_AIArchetypes::JackOfAllTrades:
			return additionalJackOfAllTradesConditionsAreFulfilled(move, stance, distance);
		default:
			return true;
		}
		return true;
	}

	// aggressor is just plain, brutal attack tactics
	bool FK_AIManager::additionalAggressorConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		return true;
	}
	// tactician prefers to slowly creep towards the opponent or waiting for it, so no running moves are used
	bool FK_AIManager::additionalTacticianConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		if (move.getEndStance() == FK_Stance_Type::RunningStance){
			return distance > 130;
		}
		return true;
	}
	// balanced is tactician when they have the life lead and a mild aggressor in the other scenario
	bool FK_AIManager::additionalBalancedConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		if (AIPlayer->getLife() >= targetPlayer->getLife()){
			return additionalTacticianConditionsAreFulfilled(move, stance, distance);
		}
		else{
			return additionalAggressorConditionsAreFulfilled(move, stance, distance);
		}
		return true;
	}

	// luchador focuses on countering and evading
	bool FK_AIManager::additionalLuchadorConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		if (targetPlayer->hasActiveMove()){
			FK_Attack_Type type = targetPlayer->getCurrentAttackType();
			if (move.hasArmor(move.getStartingFrame(), type) || move.isInvincible(move.getStartingFrame(), type)){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			if (AIPlayer->getCurrentMove() != NULL && !AIPlayer->hasActiveMove()) {
				return true;
			}
			else {
				if (move.isInvincible(move.getStartingFrame(), FK_Attack_Type::AllAtk) &&
					!move.canBeTriggered() &&
					move.getTotalBulletDamage() == 0) {
					if (AIPlayer->getStance() == FK_Stance_Type::GroundStance ||
						AIPlayer->getStance() == FK_Stance_Type::CrouchedStance/* ||
						AIPlayer->getStance() == FK_Stance_Type::RunningStance*/) {
						return false;
					}
				}
				else {
					return true;
				}
			}
		}
		return true;
	}

	// balanced is tactician when they have the life lead and a mild aggressor in the other scenario
	bool FK_AIManager::additionalJackOfAllTradesConditionsAreFulfilled(FK_Move& move, FK_Stance_Type stance, f32 distance){
		return additionalLuchadorConditionsAreFulfilled(move, stance, distance) && 
			additionalBalancedConditionsAreFulfilled(move, stance, distance);
	}

	/* load additional settings for the AI, if any*/
	void FK_AIManager::loadAIAdvancedSettings()
	{
		if (AIPlayer == NULL) {
			return;
		}
		std::string rootPath = AIPlayer->getCharacterDirectory();
		std::string testPath = rootPath + AIPlayer->getOutfitPath();
		std::ifstream configFile((testPath + AdvancedAISettingsFileName).c_str());
		if (!configFile) {
			configFile.clear();
			configFile.open((rootPath + AdvancedAISettingsFileName).c_str());
			if (!configFile) {
				return;
			}
		}
	}

	void FK_AIManager::getAvailableMovesIndices(std::vector<int>&availableMovesIndices, FK_Stance_Type stance, f32 distanceValue){
		/* set available moves */
		std::vector<int> availableMovesIndicesGuardCorrected = availableMovesIndices;
		int movesetSize = AIPlayer->getMovesCollection().size();
		FK_Move* testMove = AIPlayer->getLastStackedMove();
		if (testMove != NULL && testMove->canBeFollowedOrCanceledOnHitOnly() &&
			!testMove->hasConnectedWithTarget()) {
			availableMovesIndices.clear();
			availableMovesIndicesGuardCorrected.clear();
			movesetSize = 0;
			return;
		}
		testMove = NULL;
		for (int i = 0; i < movesetSize; ++i){
			if (basicConditionsAreFulfilled(AIPlayer->getMovesCollection()[i], stance, distanceValue) &&
				additionalConditionsAreFulfilled(AIPlayer->getMovesCollection()[i], stance, distanceValue)){
				// check for guarding
				availableMovesIndices.push_back(i);
				if (targetPlayer->isGuarding()){
					if (targetPlayer->getStance() == CrouchedStance && AIPlayer->getMovesCollection()[i].hasMidAttacks()){
						availableMovesIndicesGuardCorrected.push_back(i);
					}
					else if (AIPlayer->getMovesCollection()[i].hasLowAttacks()){
						availableMovesIndicesGuardCorrected.push_back(i);
					}
				}
			}
		}
		if (!availableMovesIndicesGuardCorrected.empty()){
			availableMovesIndices = availableMovesIndicesGuardCorrected;
		}
	}

	/* update AI input */
	bool FK_AIManager::updateAIInput(u32 delta_t_ms, u32 timerValue){
		currentTimer = timerValue;
		buttonTimeCounterMS += delta_t_ms;
		timeCounter += delta_t_ms;
		if (isAvoidingBullet) {
			return false;
		}
		core::vector3df distance = targetPlayer->getPosition() - AIPlayer->getPosition();
		f32 distanceValue = distance.getLength();
		float temp_ang = targetPlayer->getRotation().Y;
		core::vector2df normal1 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
		temp_ang = AIPlayer->getRotation().Y;
		core::vector2df normal2 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
		f32 directionProduct = normal2.dotProduct(normal1);
		FK_Stance_Type endStance = AIPlayer->getStance();
		if (AIPlayer->getLastStackedMove() != NULL){
			endStance = AIPlayer->getLastStackedMove()->getEndStance();
		}
		// reset scheduled move
		if (AIPlayer->getCurrentMove() == NULL ||
			AIPlayer->getCurrentMove()->getEndingFrame() < (s32)scheduledMoveFrame ||
			(AIPlayer->isHitStun() || AIPlayer->isGuarding())
			){
			scheduledMoveIndex = -1;
			scheduledMoveFrame = 0;
		}
		if (scheduledMoveIndex >= 0){
			//std::cout << AIPlayer->getDisplayName() << " waiting for scheduled move... " << scheduledMoveFrame << std::endl;
			if (AIPlayer->getCurrentFrame() >= (s32)scheduledMoveFrame){
				bool executionFlag = 
					scheduledMoveConditionsAreFulfilled(AIPlayer->getMovesCollection()[scheduledMoveIndex], distanceValue);
				if (executionFlag) {
					executionFlag &= AIPlayer->performMove(&(AIPlayer->getMovesCollection()[scheduledMoveIndex]));
					scheduledMoveIndex = -1;
					scheduledMoveFrame = 0;
				}else{
					scheduledMoveIndex = -1;
					scheduledMoveFrame = 0;
					numberOfChainAttempts = 0;
					reshuffleMoveSinceInvalid = true;
				}
				return executionFlag;
			}
		}
		else{
			/* prepare move array */
			std::vector<int> availableMovesIndices;
			getAvailableMovesIndices(availableMovesIndices, endStance, distanceValue);
			/* reset last move name */
			if (AIPlayer->getCurrentMove() == NULL){
				AIPlayerCurrentMoveName = std::string();
				AIPlayerCurrentMoveId = 0;
			}
			/* check for new move */
			if (AIPlayer->getCurrentMove() != NULL && AIPlayer->getNextMove() == NULL){
				if (availableMovesIndices.empty()) {
					return false;
				}
				int followupSize = AIPlayer->getCurrentMove()->getFollowupMovesSet().size();
				int movesNumber = followupSize;
				movesNumber += AIPlayer->getCurrentMove()->getCancelIntoMovesSet().size();
				if (movesNumber > 0 && AIPlayer->canAcceptInput()){
					double random = ((double)rand() / (RAND_MAX)) * 100;
					bool allowChain = reshuffleMoveSinceInvalid ||
						AIPlayerCurrentMoveId != AIPlayer->getCurrentMove()->getMoveId() ||
						(AIPlayer->getCurrentMove()->getEndStance() != AIPlayer->getStance() ||
						!AIPlayer->getCurrentMove()->canBeTriggered() ||
						AIPlayer->getCurrentMove()->getCancelIntoMovesSet().size() > 0);
					AIPlayerCurrentMoveId = AIPlayer->getCurrentMove()->getMoveId();
					if (allowChain){
						if (random <= getChainLikelihood() || reshuffleMoveSinceInvalid){
							random = ((double)rand() / (RAND_MAX));
							int index = irr::core::clamp((s32)(floor(random*movesNumber)), 0, movesNumber - 1);
							// perform followup move
							FK_FollowupMove tempMove;
							bool isFollowupMove = false;
							if (index >= followupSize){
								index -= followupSize;
								tempMove = AIPlayer->getCurrentMove()->getCancelIntoMovesSet()[index];
							}
							else{
								isFollowupMove = true;
								tempMove = AIPlayer->getCurrentMove()->getFollowupMovesSet()[index];
							}
							// now, check the whole moves array until the correct one is found...
							//int movesCollectionSize = AIPlayer->getMovesCollection().size();
							int movesCollectionSize = availableMovesIndices.size();
							for (int i = 0; i < movesCollectionSize; ++i){
								int moveIndex = availableMovesIndices[i];
								if (AIPlayer->getMovesCollection()[moveIndex].getMoveId() == tempMove.getMoveId()){
									FK_Move* testMove = &(AIPlayer->getMovesCollection()[moveIndex]);
									if (testMove->getInputStringLeft().empty() ||
										!rangeConditionFulfilled(*testMove, distanceValue) ||
										!objectConditionFulfilled(*testMove) ||
										AIPlayer->isMoveInCurrentCombo(testMove)
										){
										reshuffleMoveSinceInvalid = true;
										//numberOfChainAttempts = 0;
										return false;
									}
									else{
										//std::cout << "AI-" << AIPlayer->getDisplayName() << " will chain: " << tempMove.getName() << std::endl;
										scheduledMoveIndex = moveIndex;
										if (isFollowupMove) {
											s32 tempFrame = tempMove.getInputWindowEndingFrame();
											tempFrame = core::clamp(tempFrame, tempMove.getInputWindowStartingFrame(),
												AIPlayer->getCurrentMove()->getEndingFrame() - 1);
											scheduledMoveFrame = (u32)tempFrame;
										}
										else {
											scheduledMoveFrame = (u32)tempMove.getInputWindowStartingFrame();
										}
										numberOfChainAttempts = 0;
										reshuffleMoveSinceInvalid = false;
										return false;// AIPlayer->performMove(&(AIPlayer->getMovesCollection()[i]));
									}
								}
							}
						}
						else{
							numberOfChainAttempts += 1;
						}
						reshuffleMoveSinceInvalid = false;
					}
				}
			}
			else if (AIPlayer->getNextMove() == NULL){
				/* reset number of chain attempts */
				numberOfChainAttempts = 0;
				/* check if there is a cooldown to expire*/
				if (timeCounter < cooldownTimeCounter){
					return false;
				}
				//if (/*!isShowingBackToOpponent() &&*/!tooNearToOpponent()){
					if (AIPlayer->canAcceptInput()){
						if (!targetPlayer->isKO()){
							int movesNumber = availableMovesIndices.size();
							if (movesNumber <= 0){
								return false;
							}
							double random = ((double)rand() / (RAND_MAX));
							int index = irr::core::clamp((s32)(floor(random*movesNumber)), 0, movesNumber - 1);
							// translate the "available moves" index into a move index
							index = availableMovesIndices[index];
							FK_Move tempMove = AIPlayer->getMovesCollection()[index];
							while (tempMove.getStance() != AIPlayer->getStance() || tempMove.isFollowupOnly()){
								double random = ((double)rand() / (RAND_MAX));
								index = irr::core::clamp((s32)(floor(random*movesNumber)), 0, movesNumber - 1);;
								index = availableMovesIndices[index];
								tempMove = AIPlayer->getMovesCollection()[index];
							}
							// set cooldown timer
							if (tempMove.getEndStance() == FK_Stance_Type::RunningStance ||
								AIPlayer->isJumping()){
								cooldownTimeCounter = 0;
							}
							else{
								cooldownTimeCounter = getAfterMoveCooldown();
							}
							timeCounter = 0;
							return AIPlayer->performMove(&(AIPlayer->getMovesCollection()[index]));
						}
						//}
					}
				}
			//}
		}
		return false;
	}

	/* check if player can move toward opponent */
	bool FK_AIManager::approachingMovementConditionsAreFulfilled(f32 distance){
		bool distanceFlag = distance > 55;
		bool specialConditionsFlag = true;
		if (AIarchetype == FK_Character::FK_AIArchetypes::Tactician || AIarchetype == FK_Character::FK_AIArchetypes::Balanced){
			specialConditionsFlag = AIPlayer->getLife() <= targetPlayer->getLife();
			specialConditionsFlag |= !isAlmostTimeUp();//currentTimer > 99 * 1000;
		}
		if (AIarchetype == FK_Character::FK_AIArchetypes::Tactician){
			specialConditionsFlag &= !(AIPlayer->isRunning());
		}
		return distanceFlag && specialConditionsFlag;
	}

	/* check if player can jump toward opponent */
	bool FK_AIManager::jumpingConditionsAreFulfilled(f32 distance){
		bool distanceFlag = distance < 150 && distance > 80;
		double random = double(100*rand() / RAND_MAX);
		if (random > getJumpLikelihood()){
			distanceFlag = false;
		}
		bool specialConditionsFlag = true;
		if (AIarchetype == FK_Character::FK_AIArchetypes::Tactician || AIarchetype == FK_Character::FK_AIArchetypes::Balanced){
			specialConditionsFlag = AIPlayer->getLife() <= targetPlayer->getLife();
			specialConditionsFlag |= currentTimer > 99 * 1000;
		}
		if (AIarchetype == FK_Character::FK_AIArchetypes::Tactician){
			specialConditionsFlag &= !(AIPlayer->isRunning());
		}
		return distanceFlag && specialConditionsFlag;
	}

	// basic running conditions fulfilled
	bool FK_AIManager::runningConditionsAreFulfilled(FK_Move& move, f32 distance) {
		if (move.getEndStance() != FK_Stance_Type::RunningStance ||
			AIPlayer->getCurrentMove() != NULL) {
			return true;
		}
		return additionalRunningConditionsAreFulfilled(distance);
	}

	// additional conditions for running
	bool FK_AIManager::additionalRunningConditionsAreFulfilled(f32 distance){
		/*s32 frame = targetPlayer->getCurrentFrame();*/
		if (targetPlayer->isRunning() || targetPlayer->hasActiveMove() || 
			 (targetPlayer->getCurrentMove() != NULL && 
				 targetPlayer->getCurrentMove()->getTotalBulletDamage() > 0)){
			return false;
		}
		if (AIarchetype == FK_Character::FK_AIArchetypes::Tactician){
			return distance > 100;
		}
		else if(AIarchetype == FK_Character::FK_AIArchetypes::Balanced){
			return (distance > 100 || AIPlayer->getLife() < targetPlayer->getLife());
		}
		return true;
	}

	bool FK_AIManager::projectileConditionsFulfilled(FK_Move& move) {
		if (targetPlayer->getCurrentMove() != NULL &&
			move.getMaximumBulletPriority() >= 0 &&
			move.getMaximumBulletPriority() < targetPlayer->getCurrentMove()->getMaximumBulletPriority()) {
			return false;
		}
		return true;
	}

	/* update button pressed (throw escape)*/
	u32 FK_AIManager::getAIThrowAttemptEscapeButtons(){
		u32 button = (u32)FK_Input_Buttons::Tech_Button;
		if (targetPlayer->getCurrentMove() != NULL){
			u32 button = targetPlayer->getCurrentMove()->getThrowEscapeInput();
		}
		double random = ((double)rand() / (RAND_MAX)) * 100;
		//std::cout << "RDN: " << random << " LKL: " << getThrowEscapeLikelihood() << std::endl;
		if (random < getThrowEscapeLikelihood()){
			return button;
		}
		else{
			return getAIButtonPressed();
		}
	}

	/* update button pressed */
	u32 FK_AIManager::getAIButtonPressed(){
		u32 buttonsPressed = 0;
		// bullet guard as priority!
		isAvoidingBullet = false;
		core::vector3df characterDistance = targetPlayer->getPosition() - AIPlayer->getPosition();
		f32 distanceLength = characterDistance.getLength();
		for each(FK_Bullet* bullet in targetPlayer->getBulletsCollection()){
			f32 potentialDamage = bullet->getHitbox().getDamage();
			bool armorCondition = AIPlayer->hasArmor(FK_Attack_Type::BulletAttack) &&
				potentialDamage < 1.1 * AIPlayer->getLife();
			bool invincibilityCondition = AIPlayer->isInvincible(FK_Attack_Type::BulletAttack);
			if (!armorCondition && !invincibilityCondition && 
				!bullet->isDisposeable() && bullet->getNode()->isVisible()){
				core::vector3df bulletPosition = bullet->getPosition();
				f32 distance = bulletPosition.getDistanceFrom(AIPlayer->getPosition());
				// this check was distance <= 70.f before, for simplicity reasons
				f32 maxRadius = std::max(bullet->getHitboxRadius().X, std::max(bullet->getHitboxRadius().Y, bullet->getHitboxRadius().Z));
				f32 delta_t_frame = 2 / 60.f;
				f32 bulletDistanceTravelledInOneFrame = bullet->getVelocity().X * delta_t_frame;
				f32 playerDistanceTravelledInOneFrame = AIPlayer->getVelocityPerSecond().X  * delta_t_frame;
				f32 characterHitboxOffset = 50.0f;
				f32 distanceToCheck = 2 * maxRadius + 2 * abs(bulletDistanceTravelledInOneFrame) + abs(playerDistanceTravelledInOneFrame) + characterHitboxOffset;
				//std::cout << distanceToCheck << std::endl;
				// f32 distanceToCheck = 70.f;
				/*if (bullet->getHitbox().getDamage() > 0 && distance > distanceToCheck) {
					if (distanceLength > 150.f && !targetPlayer->hasActiveMove()) {
						isAvoidingBullet = true;
						buttonsPressed = FK_Input_Buttons::Up_Direction;
						if (!AIPlayer->isRunning()) {
							if (AIPlayer->isOnLeftSide()) {
								buttonsPressed |= (u32)FK_Input_Buttons::Right_Direction;
							}
							else {
								buttonsPressed |= (u32)FK_Input_Buttons::Left_Direction;
							}
						}
						lastInput = buttonsPressed;
						buttonTimeCounterMS = 0;
						return buttonsPressed;
					}
				}*/
				f32 bulletThresholdDistance = 320.f;
				f32 bulletJumpingDistance = 120.f;
				if (bullet->canBeJumpedOver() && 
					distance <= 1.2 * distanceToCheck && 
					distanceLength >= bulletJumpingDistance) {
					u32 button = fk_constants::FK_GuardButton;
					buttonsPressed |= button;
					buttonTimeCounterMS = 0;
					buttonsPressed |= FK_Input_Buttons::Up_Direction;
					if (AIPlayer->isOnLeftSide()) {
						buttonsPressed |= (u32)FK_Input_Buttons::Right_Direction;
					}
					else {
						buttonsPressed |= (u32)FK_Input_Buttons::Left_Direction;
					}
					lastInput = buttonsPressed;
					buttonTimeCounterMS = 0;
					return buttonsPressed;
				}
				else if (distance <= distanceToCheck){
					u32 button = fk_constants::FK_GuardButton;
					if (/*distanceLength < bulletThresholdDistance &&*/ (
						bullet->getHitbox().canBeStandGuarded() ||
						bullet->getHitbox().canBeCrouchGuarded()
						)) {
						buttonsPressed |= button;
						buttonTimeCounterMS = 0;
					}
					if (!(bullet->getHitbox().canBeStandGuarded())/* || 
						distanceLength >= bulletThresholdDistance*/
						){
						if (bullet->getHitbox().canBeCrouchGuarded()/* && 
							characterDistance.getLength() < bulletThresholdDistance*/){
							u32 button = fk_constants::FK_GuardButton;
							buttonsPressed |= button;
							buttonTimeCounterMS = 0;
							double random = ((double)rand() / (RAND_MAX)) * 100;
							if (random > 30){
								buttonsPressed |= FK_Input_Buttons::Down_Direction;
							}
							else{
								buttonsPressed |= FK_Input_Buttons::Up_Direction;
								if (AIPlayer->isOnLeftSide()){
									buttonsPressed |= (u32)FK_Input_Buttons::Right_Direction;
								}
								else{
									buttonsPressed |= (u32)FK_Input_Buttons::Left_Direction;
								}
							}
						}
						else{
							isAvoidingBullet = true;
							buttonsPressed = FK_Input_Buttons::Up_Direction;
							if (!AIPlayer->isRunning()) {
								if (AIPlayer->isOnLeftSide()) {
									buttonsPressed |= (u32)FK_Input_Buttons::Right_Direction;
								}
								else {
									buttonsPressed |= (u32)FK_Input_Buttons::Left_Direction;
								}
							}
						}
					}
					lastInput = buttonsPressed;
					buttonTimeCounterMS = 0;
					return buttonsPressed;
				}
			}
		}
		// if it's running, then press the direction towards the enemy
		if (AIPlayer->isRunning() && !(targetPlayer->isKO())){
			core::vector3df distance = targetPlayer->getPosition() - AIPlayer->getPosition();
			if (additionalRunningConditionsAreFulfilled(distance.getLength())){
				if (AIPlayer->isOnLeftSide()){
					return (u32)FK_Input_Buttons::Right_Direction;
				}
				else{
					return (u32)FK_Input_Buttons::Left_Direction;
				}
			}
		}
		if (buttonTimeCounterMS > minimalButtonPressTimeMS){
			core::vector3df distance = targetPlayer->getPosition() - AIPlayer->getPosition();
			f32 distanceValue = distance.getLength();
			float temp_ang = targetPlayer->getRotation().Y;
			core::vector2df normal1 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
			temp_ang = AIPlayer->getRotation().Y;
			core::vector2df normal2 = core::vector2df(cos(temp_ang*core::DEGTORAD), sin(temp_ang*core::DEGTORAD));
			f32 directionProduct = normal2.dotProduct(normal1);
			//std::cout << "pressing buttons... " << AIPlayer->getStance() << std::endl;
			// guard against bullets OR move on the side
			if (AIPlayer->getCurrentMove() == NULL){
				if (AIPlayer->isTriggerGuardActive()){
					buttonsPressed |= fk_constants::FK_GuardButton;
				}
				if (((!targetPlayer->isGrounded() || AILevel > 2 || (AILevel <= 2 && currentTimer < 100)) ||
					AIPlayer->isGrounded()) &&
					!targetPlayer->isKO()){
					//if (distanceValue < 60.0){
						double random = ((double)rand() / (RAND_MAX)) * 100;
						if (//targetPlayer->getLifePercentage() < 10.0 || 
							(
							random <= getTriggerLikelihood() && (
							(AIPlayer->isTriggerComboed() && targetPlayer->getComboCounter() >= 3) ||
							(AIPlayer->isHitStun() && AIPlayer->getLifePercentage() < 35.0) ||
							targetPlayer->getComboCounter() >= 3 ||
							(AIPlayer->isHitStun() && AIPlayer->getInStageRingoutZoneFlag())
							))
							){
							u32 button = fk_constants::FK_TriggerButton;
							buttonsPressed |= button;
						}
					//}
					if (AIPlayer->isGrounded()){
						// manage grounded reactions
						if (cooldownTimeCounter == 0){
							cooldownTimeCounter = 200;
							timeCounter = 0;
						}
						else if (timeCounter > cooldownTimeCounter || 
							targetPlayer->hasActiveMove()){
							// set random for grounded action
							double random = ((double)rand() / (RAND_MAX)) * 100;
							if (random < 20){
								u32 button = fk_constants::FK_GuardButton;
								buttonsPressed = button;
							}
							else if (random < 40){
								u32 button = AIPlayer->isOnLeftSide() ? FK_Input_Buttons::Right_Direction :
									FK_Input_Buttons::Left_Direction;
								buttonsPressed = button;
							}
							else if (random < 60){
								u32 button = fk_constants::FK_GuardButton | FK_Input_Buttons::Down_Direction;
								buttonsPressed = button;
							}
							else if (random < 80){
								u32 button = FK_Input_Buttons::Down_Direction;
								buttonsPressed = button;
							}
							else{
								u32 button = FK_Input_Buttons::Up_Direction;
								buttonsPressed = button;
							}
						}
					}
					auto attackType = targetPlayer->getCurrentAttackType();
					bool armorCondition = AIPlayer->hasArmor(attackType);
					bool invincibilityCondition = AIPlayer->isInvincible(attackType);
					if (attackType == FK_Attack_Type::NoType) {
						armorCondition = false;
						invincibilityCondition = false;
					}
					if (targetPlayer->hasActiveMove(false) &&
						!targetPlayer->getCurrentMove()->hasThrowAttacks() && 
						/*targetPlayer->getCurrentMove()->canBeTriggered() && */
						distanceValue <= 1.2*targetPlayer->getCurrentMove()->getMaxRange() &&
						!armorCondition &&
						!invincibilityCondition){
						if (!AIPlayer->isGuarding()){
							/* give chance to guard only once per attack */
							double random = ((double)rand() / (RAND_MAX)) * 100;
							if (random <= getGuardLikelihood() &&
								targetPlayerCurrentMoveId != targetPlayer->getCurrentMove()->getMoveId()){
								u32 button = fk_constants::FK_GuardButton;
								buttonsPressed |= button;
								if (targetPlayer->getCurrentMove()->hasLowAttacks() &&
									(!targetPlayer->getCurrentMove()->hasMidAttacks() ||
									((targetPlayer->getCurrentAttackType() & FK_Attack_Type::LowAtks) != 0))){
									if (AIPlayer->getStance() == FK_Stance_Type::CrouchedStance){
										buttonsPressed |= FK_Input_Buttons::Down_Direction;
										if (AIPlayer->isOnLeftSide()) {
											buttonsPressed |= FK_Input_Buttons::Right_Direction;
										}
										else {
											buttonsPressed |= FK_Input_Buttons::Left_Direction;
										}
									}
									else{
										double random2 = ((double)rand() / (RAND_MAX)) * 100;
										if ((u32)(ceil(random2)) < getJumpLikelihood()){
											if (AIPlayer->isOnLeftSide()){
												buttonsPressed |= FK_Input_Buttons::UpRight_Direction;
											}
											else{
												buttonsPressed |= FK_Input_Buttons::UpLeft_Direction;
											}
										}
										else{
											buttonsPressed |= FK_Input_Buttons::Down_Direction;
										}
									}
								}
							}
							targetPlayerCurrentMoveId = targetPlayer->getCurrentMove()->getMoveId();
						}
						else{
							if (targetPlayer->isTriggerModeActive() && !AIPlayer->isHitStun()){

							}
							else{
								if (AIPlayer->getStance() == FK_Stance_Type::CrouchedStance){
									if (((targetPlayer->getCurrentAttackType() & FK_Attack_Type::LowAtks) != 0) ||
										!targetPlayer->getCurrentMove()->hasMidAttacks()){
										u32 button = fk_constants::FK_GuardButton;
										buttonsPressed |= button;
										buttonsPressed |= FK_Input_Buttons::Down_Direction;
									}
									else{
										buttonsPressed |= FK_Input_Buttons::Down_Direction;
									}
								}
								else{
									u32 button = fk_constants::FK_GuardButton;
									buttonsPressed |= button;
									if (((targetPlayer->getCurrentAttackType() & FK_Attack_Type::LowAtks) != 0)){
										buttonsPressed |= FK_Input_Buttons::Down_Direction;
									}
								}
							}
						}
					}
					else{
						/* reset target player move */
						targetPlayerCurrentMoveName = std::string();
						targetPlayerCurrentMoveId = 0;
						if (approachingMovementConditionsAreFulfilled(distanceValue)){
							u32 button = 0;
							button = FK_Input_Buttons::Left_Direction;
							if (AIPlayer->isOnLeftSide())
								button = FK_Input_Buttons::Right_Direction;
							buttonsPressed |= button;
							if (!AIPlayer->isRunning() && AIPlayer->getCurrentMove() == NULL &&
								jumpingConditionsAreFulfilled(distanceValue)){
								buttonsPressed |= FK_Input_Buttons::Guard_Button | FK_Input_Buttons::Up_Direction;
							}
						}
						else if (tooNearToOpponent() || isShowingBackToOpponent()){
							u32 button = 0;
							button = FK_Input_Buttons::Right_Direction;
							if (AIPlayer->isOnLeftSide())
								button = (u32)FK_Input_Buttons::Left_Direction;
							buttonsPressed |= button;
						}
						else {
							/*
							u32 button = 0;
							double random = ((double)rand() / (RAND_MAX)) * 100;
							if (random >= 65){
							button = button | FK_Input_Buttons::Up_Direction;
							}
							else if (random >= 30){
							button = button | FK_Input_Buttons::Down_Direction;
							}
							*/
							//buttonsPressed |= button;
						}
					}
				}
			}
			if (timeCounter > cooldownTimeCounter){
				cooldownTimeCounter = 0;
			}
			// manage trigger likelihood
			if (AIPlayer->getCurrentMove() != NULL){
				double random = ((double)rand() / (RAND_MAX)) * 100;
				if (targetPlayer->isGuarding()){
					random /= 1.5;
				}
				/*if (random < getTriggerAttackLikelihood() && 
					((targetPlayer->isGuarding() && AIPlayer->getCurrentMove()->hasFollowup()) ||
					!targetPlayer->isGuarding())){
					u32 button = fk_constants::FK_TriggerButton;
					buttonsPressed |= button;
				}*/
				if (random < getTriggerAttackLikelihood() &&
					AIPlayer->getCurrentMove()->hasFollowup() && 
					(targetPlayer->isGuarding() || targetPlayer->isHitStun())) {
					u32 button = fk_constants::FK_TriggerButton;
					buttonsPressed |= button;
				}
			}
			lastInput = buttonsPressed;
			buttonTimeCounterMS = 0;
		}else{
			buttonsPressed = lastInput;
		}
		return buttonsPressed;
	}

	void FK_AIManager::updateWinNumbers(u32 AIplayerRoundWon, u32 targetPlayerRoundWon){
		numberOfRoundsWon = AIplayerRoundWon;
		numberOfRoundsLost = targetPlayerRoundWon;
	}

	bool FK_AIManager::isWinningRound(){
		bool lifeFlag = AIPlayer->getLife() > targetPlayer->getLife();
		return lifeFlag;
	}
	bool FK_AIManager::isLosingRound(){
		bool lifeFlag = AIPlayer->getLife() < targetPlayer->getLife();
		return lifeFlag;
	}
	bool FK_AIManager::isWinningMatch(){
		return numberOfRoundsWon > numberOfRoundsLost;
	}
	bool FK_AIManager::isLosingMatch(){
		return numberOfRoundsWon < numberOfRoundsLost;
	}
	bool FK_AIManager::isOneRoundToVictory(){
		return numberOfRoundsWon >= numberOfRounds / 2;
	}
	bool FK_AIManager::isOneRoundToLoss(){
		return numberOfRoundsLost >= numberOfRounds / 2;
	}
	bool FK_AIManager::isAlmostTimeUp(){
		return currentTimer < 15 * 1000;
	}
}