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

#include"FK_Move.h"
#include"ExternalAddons.h"
#include<algorithm>

namespace fk_engine{
	/* this class handles all the parts of a single move, including input and hitboxes*/
	FK_Move::FK_Move(){
		clear();
	};
	int FK_Move::getStartingFrame(){
		return startingFrame;
	};
	int FK_Move::getEndingFrame(){
		return endingFrame;
	};
	/* duration has to take into account that the first frame is also played */
	int FK_Move::getDuration(){
		return (endingFrame - startingFrame + 1);
	};
	int FK_Move::getPriority(){
		return movePriority;
	};
	std::string FK_Move::getName(){
		return moveName;
	};
	std::string FK_Move::getDisplayName(){
		return displayName;
	};
	std::wstring FK_Move::getWDisplayName(){
		return displayWName;
	};
	/* get name of the animation file*/
	std::string FK_Move::getAnimationName(bool leftSide){
		if (leftSide){
			return moveAnimationName;
		}
		else{
			return moveAnimationNameRightSide;
		}
	};
	std::vector<FK_Hitbox> &FK_Move::getHitboxCollection(){
		return hitboxCollection;
	};
	std::vector<FK_FollowupMove> &FK_Move::getFollowupMovesSet(){
		return followupMoves;
	};
	// get the "cancel into" moves set
	std::vector<FK_FollowupMove> &FK_Move::getCancelIntoMovesSet(){
		return cancelIntoMoves;
	}
	std::vector<FK_FollowupMove> FK_Move::getCopyOfFollowupMovesSet() {
		return followupMoves;
	};
	// get the "cancel into" moves set
	std::vector<FK_FollowupMove> FK_Move::getCopyOfCancelIntoMovesSet() {
		return cancelIntoMoves;
	}

	std::string FK_Move::getAutoFollowupMove()
	{
		return autoFollowupMoveName;
	}
	void FK_Move::setAutoFollowupMove(std::string moveName) {
		autoFollowupMoveName = moveName;
	}
	
	std::deque<u32> FK_Move::getInputString(bool leftScreenSide){
		return leftScreenSide ? inputStringLeft : inputStringRight;
	}
	std::deque<u32> FK_Move::getInputStringLeft(){
		return inputStringLeft;
	}
	std::deque<u32> FK_Move::getInputStringRight(){
		return inputStringRight;
	}
	core::vector3df FK_Move::getMovementAtFrame(int frameNumber){
		if (frameNumber >= getStartingFrame() && frameNumber <= getStartingFrame() + getDuration()){
			if ((s32)movementFrame.size() <= getMovementFrameId(frameNumber)){
				return core::vector3df(0, 0, 0);
			}
			return movementFrame[getMovementFrameId(frameNumber)];
		}
		else{
			return core::vector3df(0,0,0);
		}
	};
	FK_Stance_Type FK_Move::getStance(){
		return stance;
	};
	FK_Stance_Type FK_Move::getEndStance(){
		return endstance;
	}
	bool FK_Move::isListedInMoveList(){
		return (listedInMoveList && (forceNameDisplayFlag || inputStringLeft.size() > 0 ||
			hasFollowup()));
	}

	bool FK_Move::isShownInHUD() {
		return listedInMoveList;
	}

	// check if move has followups
	bool FK_Move::hasFollowup(){
		return !(followupMoves.empty() && cancelIntoMoves.empty());
	}

	// check if move can only be accessed as a followup
	bool FK_Move::isFollowupOnly(){
		return followupOnly;
	}

	// check if move "swing" sound has to be played
	bool FK_Move::canPlaySoundEffect(s32 frameNumber){
		if (!playSoundEffectFlag){
			return false;
		}
		if (soundsToPlay.empty()){
			return false;
		}
		if (soundsToPlay.count((u32)frameNumber) > 0){
			return !soundsPlayed[frameNumber];
		}
		return false;
	}
	// get sound effect
	std::string FK_Move::getSoundEffect(s32 frameNumber, bool markAsPlayed){
		if (canPlaySoundEffect(frameNumber)){
			soundsPlayed[frameNumber] |= markAsPlayed;
			return soundsToPlay[frameNumber];
		}
		return std::string();
	}
	// add new sound effect
	void FK_Move::addSoundEffect(u32 frame, std::string identifier){
		soundsToPlay[frame] = identifier;
		soundsPlayed[frame] = false;
	}
	// clear all sound effects
	void FK_Move::clearSoundEffects(){
		soundsToPlay.clear();
		soundsPlayed.clear();
	}
	// reset all sound effects to "non-played"
	void FK_Move::resetSoundsPlayed(){
		for each (std::pair<u32, bool> flag in soundsPlayed){
			soundsPlayed[flag.first] = false;
		}
		soundsPlayed;
		return;
	}

	// set sound effect flag
	void FK_Move::setSoundEffectFlag(bool new_flag){
		playSoundEffectFlag = new_flag;
	}

	// check if the move is a long range move (used by AI)
	f32 FK_Move::getMinRange(){
		return moveMinRange;
	}
	f32 FK_Move::getMaxRange(){
		return moveMaxRange;
	}
	void FK_Move::setStartingFrame(int frame){
		startingFrame = frame;
	};
	void FK_Move::setEndingFrame(int frame){
		endingFrame = frame;
	};
	void FK_Move::setPriority(int priority){
		movePriority = priority;
	};
	void FK_Move::setName(std::string name){
		moveName = name;
		if (displayName.empty()){
			setDisplayName(name);
		}
	}
	void FK_Move::setDisplayName(std::string name){
		std::string temp = name;
		std::replace(temp.begin(), temp.end(), '_', ' ');
		displayName = temp;
		displayWName = fk_addons::convertNameToNonASCIIWstring(temp);
	}
	void FK_Move::setWDisplayName(std::wstring name) {
		displayWName = name;
	}
	void FK_Move::setAnimationName(std::string name, bool leftSide){
		if (leftSide){
			moveAnimationName = name;
		}
		else{
			moveAnimationNameRightSide = name;
		}
	}
	void FK_Move::setHitboxCollection(std::vector<FK_Hitbox> newCollection){
		hitboxCollection = newCollection;
		lastActiveFrame = -1;
		firstActiveFrame = -1;
		for each(FK_Hitbox box in hitboxCollection) {
			if (firstActiveFrame < 0 || box.getStartingFrame() < firstActiveFrame) {
				firstActiveFrame = box.getStartingFrame();
			}
			if (box.getEndingFrame() > lastActiveFrame) {
				lastActiveFrame = box.getEndingFrame();
			}
			totalMoveDamage += box.getDamage();
			if (box.getReaction() == FK_Reaction_Type::StrongFlight) {
				hasImpactHitboxes = true;
			}
		}
	};
	void FK_Move::setFollowupMovesSet(std::vector<FK_FollowupMove> newCollection){
		followupMoves = newCollection;
	};
	// set the moves this moves can cancel into
	void FK_Move::setCancelIntoMovesSet(std::vector<FK_FollowupMove> newCollection){
		cancelIntoMoves = newCollection;
	};
	void FK_Move::setInputStringLeft(std::deque<u32> newCollection){
		inputStringLeft = newCollection;
	};
	void FK_Move::setInputStringRight(std::deque<u32> newCollection){
		inputStringRight = newCollection;
	};
	void FK_Move::setMovementAtFrame(int frameNumber, float movementPar, float movementSide, float movementVert){
		if (frameNumber >= getStartingFrame() && frameNumber <= getEndingFrame()){
			movementFrame[getMovementFrameId(frameNumber)] = core::vector3df(movementPar, movementSide, movementVert);
		}
	};
	void FK_Move::setStance(FK_Stance_Type new_stance){
		stance = new_stance;
	}
	void FK_Move::setEndStance(FK_Stance_Type new_stance){
		endstance = new_stance;
	}
	void FK_Move::setListed(bool new_listedFlag){
		listedInMoveList = new_listedFlag;
	}
	// set move range (used by AI)
	void FK_Move::setRange(f32 new_minRange, f32 new_maxRange){
		moveMinRange = new_minRange;
		moveMaxRange = new_maxRange;
	}
	// set if the move can only be accessed as a followup
	void FK_Move::setFollowUpOnly(bool new_followup_only_flag){
		followupOnly = new_followup_only_flag;
	};
	void FK_Move::resetMovementFrameArray(){
		movementFrame.clear();
		movementFrame = std::vector<core::vector3df>(getDuration(), core::vector3df(0,0,0));
	};

	void FK_Move::resetHitboxesHitFlag(){
		moveHasHitTarget = false;
		for (auto hitboxIter = hitboxCollection.begin(); hitboxIter != hitboxCollection.end(); ++hitboxIter){
			hitboxIter->setHit(false);
		}
	}
	// add bullet to collection
	void FK_Move::addBullet(FK_Bullet new_bullet){
		bullets.push_back(new_bullet);
		if (maximumBulletPriority < new_bullet.getHitbox().getPriority()) {
			maximumBulletPriority = new_bullet.getHitbox().getPriority();
		}
		if (firstBulletFrame < 0 || firstBulletFrame > new_bullet.getHitbox().getStartingFrame()) {
			firstBulletFrame = new_bullet.getHitbox().getStartingFrame();
		}
		if (new_bullet.getHitbox().getDamage() > 0 && 
			(firstFrameWithDamagingBullet < 0 || firstFrameWithDamagingBullet > new_bullet.getHitbox().getStartingFrame())) {
			firstFrameWithDamagingBullet = new_bullet.getHitbox().getStartingFrame();
		}
		totalBulletDamage += new_bullet.getHitbox().getDamage();
	}
	// get bullet collection
	std::deque<FK_Bullet> &FK_Move::getBulletCollection(){
		return bullets;
	}

	// clear variables
	void FK_Move::clear(){
		moveId = 0;
		AIflag_onlyDuringOpponentAttack = false;
		canClearInputBuffer_flag = false;
		requiresPreciseInputFlag = false;
		startingFrame = 0;
		endingFrame = 0;
		movePriority = 0;
		moveName = std::string();
		displayName = std::string();
		displayWName = std::wstring();
		soundsToPlay.clear();
		followupOnly = false;
		listedInMoveList = false;
		moveAnimationName = std::string();
		moveAnimationNameRightSide = std::string();
		stance = FK_Stance_Type::NoStance;
		endstance = stance;
		moveMinRange = 0.0;
		moveMaxRange = 0.0;
		followupOnly = false;
		vsGroundedOpponentFlag = false;
		hitboxCollection.clear();
		followupMoves.clear();
		cancelIntoMoves.clear();
		inputStringLeft.clear();
		inputStringRight.clear();
		invincibilityFrames.clear();
		invincibilityType.clear();
		movementFrame.clear();
		//throwMovementFrame.clear();
		throwFlag = false;
		playSoundEffectFlag = true;
		//throwReactionAnimationPose = FK_Pose();
		bullets.clear();
		hasLowAttacksFlag = false;
		hasMidAttacksFlag = false;
		hasThrowAttacksFlag = false;
		directionLockFlag = true;
		hasDelayAfterMoveEndsFlag = true;
		isTauntMove = false;
		transversalMirrorFlag = false;
		moveThrow = FK_ThrowMove();
		afterAnimationTrackingFlag = false;
		beforeAnimationTrackingFlag = false;
		requiredObjectsUniqueIds.clear();
		forbiddenObjectsUniqueIds.clear();
		toggledObjectsAtFrame.clear();
		//breakingObjectsAtFrame.clear();
		pickupItem.first = 0;
		pickupItem.second = std::string();
		trackAfterHitFlag = false;
		lowerGravityFlag = false;
		afterMoveDelay_ms = -1;
		counterattackMoves.clear();
		moveTransformation = FK_TransformationMove();
		requiredTriggers = 0;
		requiredSuperMeterUnits = 0.f;
		moveCinematic = FK_MoveCinematic();
		pickupOnlyMove = false;
		antiAirOnlyFlag = false;
		autoFollowupMoveName = std::string();
		armorFrames.clear();
		armorType.clear();
		activatesTrigger = false;
		maximumTrackingAngleOnStartup = 180.f;
		maximumTrackingAngleDuringMove = 180.f;
		// force name display for followup moves
		forceNameDisplayFlag = false;
		// cancel guard
		cancelGuardFlag = false;
		// maximum damage scaling
		maximumDamageScaling = 0.f;
		// additional fall velocity to prevent intersection
		additionalFallVelocityAfterAirMove = 1.0f;
		// boolean which states if move has hit opponent
		moveHasHitTarget = false;
		firstActiveFrame = -1;
		lastActiveFrame = -1;
		firstBulletFrame = -1;
		firstFrameWithDamagingBullet = -1;
		/* get move maximum bullet priority */
		maximumBulletPriority = -1;
		/* get move maximum damage */
		totalMoveDamage = 0;
		totalBulletDamage = 0;
		/* check if move can send out of the ring*/
		hasImpactHitboxes = false;
		/* decides if the move must be canceled only after having connected with the target */
		followupOrCancelOnHitOnlyFlag = false;
		/* set if move can be chained more than once in the same chain */
		isMultiChainable = false;
		/* store all attack types per frame for the AI */
		attackTypeAtFrame.clear();
		/* flag for throws that cannot hit airborne opponents */
		noThrowOnAirborneFlag = false;
	};
	/* private functions */
	int FK_Move::getMovementFrameId(int frameNumber){
		return (frameNumber - getStartingFrame());
	};
	// set operator < to make order between moves
	bool operator<(const FK_Move& m1, const FK_Move& m2){
		/*
		bool flag_prior = m1.movePriority < m2.movePriority;
		bool flag_equal_prior = m1.movePriority == m2.movePriority;
		*/
		bool flag_size = m1.inputStringLeft.size() < m2.inputStringLeft.size();
		//return (flag_prior || (flag_equal_prior && flag_size));
		return flag_size;
	}

	/* check if move can be triggered in Trigger Assault */
	bool FK_Move::canBeTriggered(){
		return !hitboxCollection.empty();
	}

	/* check if the move is a taunt*/
	bool FK_Move::isTaunt(){
		return isTauntMove;
	}

	/* set move as taunt (for A.I. processing) */
	void FK_Move::setAsTaunt(){
		isTauntMove = true;
	}

	/* check if the move needs to rotate the character after the animation ends*/
	bool FK_Move::canTrackAfterAnimation(){
		return afterAnimationTrackingFlag;
	}

	/* set the move to track after animation ends */
	void FK_Move::setAfterAnimationTracking(){
		afterAnimationTrackingFlag = true;
	}

	/* can track before animation*/
	bool FK_Move::canTrackAtStartup(){
		return beforeAnimationTrackingFlag;
	}

	/* set the move to track before the animation plays */
	void FK_Move::setStartupTracking(){
		beforeAnimationTrackingFlag = true;
	}

	/* check if move can be transversally mirrored */
	bool FK_Move::cantBeTransversalMirrored(){
		return transversalMirrorFlag;
	}
	/* set move as unmirrorable */
	void FK_Move::applyUnmirrorableFlag(){
		transversalMirrorFlag = true;
	}

	/* pickup items */
	/* can pick up item */
	bool FK_Move::canPickUpItem(std::string itemId){
		return pickupItem.second == itemId;
	}
	/* can pick up item at frame */
	bool FK_Move::canPickUpItemAtFrame(u32 frame, std::string itemId){
		return canPickUpItem(itemId) && pickupItem.first == frame;
	}
	/* get pickable item */
	std::string FK_Move::getPickableItemId(){
		return pickupItem.second;
	}
	/* set pick up*/
	void FK_Move::setItemPickupAtFrame(u32 frame, std::string itemId){
		pickupItem.first = frame;
		pickupItem.second = itemId;
	}

	/* set move as valid only with item pickup*/
	void FK_Move::setAsPickupOnly()
	{
		pickupOnlyMove = true;
	}

	/* check if move is valid also without pickup*/
	bool FK_Move::isMoveAvailableWithoutPickup()
	{
		return !pickupOnlyMove;
	}

	/* add object to required items*/
	bool FK_Move::addRequiredObjectId(std::string itemId){
		if (!(isObjectRequired(itemId) || isObjectForbidden(itemId))){
			requiredObjectsUniqueIds.push_back(itemId);
		}
		if (isObjectForbidden(itemId)){
			return false;
		}
		if (isObjectRequired(itemId)){
			return true;
		}
		return false;
	}

	/* add object to forbidden items*/
	bool FK_Move::addForbiddenObjectId(std::string itemId){
		if (!(isObjectRequired(itemId) || isObjectForbidden(itemId))){
			forbiddenObjectsUniqueIds.push_back(itemId);
		}
		if (isObjectForbidden(itemId)){
			return true;
		}
		if (isObjectRequired(itemId)){
			return false;
		}
		return false;
	}

	/* add object to required items*/
	bool FK_Move::isObjectRequired(std::string itemId){
		return find(requiredObjectsUniqueIds.begin(), requiredObjectsUniqueIds.end(), itemId) != requiredObjectsUniqueIds.end();
	}

	/* add object to forbidden items*/
	bool FK_Move::isObjectForbidden(std::string itemId){
		return find(forbiddenObjectsUniqueIds.begin(), forbiddenObjectsUniqueIds.end(), itemId) != forbiddenObjectsUniqueIds.end();
	}

	/* check if move has item requirements*/
	bool FK_Move::hasItemRequirements(){
		return !(getPickableItemId().empty() && requiredObjectsUniqueIds.empty() && forbiddenObjectsUniqueIds.empty());
	}

	/* get required objects */
	const std::vector<std::string>& FK_Move::getRequiredObjects(){
		return requiredObjectsUniqueIds;
	}
	/* get forbidden objects */
	const std::vector<std::string>& FK_Move::getForbiddenObjects(){
		return forbiddenObjectsUniqueIds;
	}

	/* can toggle items at frame */
	bool FK_Move::canToggleItemsAtFrame(u32 currentFrame){
		return (toggledObjectsAtFrame.count(currentFrame) > 0 && toggledObjectsAtFrame[currentFrame].size() > 0);
	}

	/* can act on item */
	bool FK_Move::canActOnItem(std::string itemId, FK_ObjectToggleFlag flag){
		std::pair<std::string, s32> itemToSearch = std::make_pair(itemId, flag);
		for (auto it = toggledObjectsAtFrame.begin(); it != toggledObjectsAtFrame.end(); ++it) {
			std::vector<std::pair<std::string, s32>> vec = it->second;
			if (std::find(vec.begin(), vec.end(), itemToSearch) != vec.end()){
				return true;
			}
		}
		return false;
	}

	/* can toggle item (at any frame) */
	bool FK_Move::canToggleItem(std::string itemId){
		return canActOnItem(itemId, FK_ObjectToggleFlag::Toggle);
	}

	/* can hide item (at any frame) */
	bool FK_Move::canHideItem(std::string itemId){
		return canActOnItem(itemId, FK_ObjectToggleFlag::Hide);
	}

	/* can show item (at any frame) */
	bool FK_Move::canShowItem(std::string itemId){
		return canActOnItem(itemId, FK_ObjectToggleFlag::Show);
	}

	/* can break item (at any frame) */
	bool FK_Move::canBreakItem(std::string itemId){
		return canActOnItem(itemId, FK_ObjectToggleFlag::Break);
	}

	/* can restore item (at any frame) */
	bool FK_Move::canRestoreItem(std::string itemId){
		return canActOnItem(itemId, FK_ObjectToggleFlag::Restore);
	}

	/* can use item (at any frame) */
	bool FK_Move::canUseItem(std::string itemId) {
		return canActOnItem(itemId, FK_ObjectToggleFlag::Use);
	}

	/* get toggled items at frame */
	const std::vector<std::pair<std::string, s32>>& FK_Move::getToggledItemsAtFrame(u32 currentFrame){
		if (toggledObjectsAtFrame.count(currentFrame) == 0){
			toggledObjectsAtFrame[currentFrame] = std::vector<std::pair<std::string, s32>>();
		}
		return toggledObjectsAtFrame[currentFrame];
	}

	/* add toggled item at frame - return false if already in list for current frame */
	bool FK_Move::addToggledItemAtFrame(u32 currentFrame, std::string itemUniqueId, FK_Move::FK_ObjectToggleFlag toggleFlag){
		if (toggledObjectsAtFrame.count(currentFrame) == 0){
			toggledObjectsAtFrame[currentFrame] = std::vector<std::pair<std::string, s32>>();
		}
		std::vector<std::pair<std::string, s32>> vec = toggledObjectsAtFrame[currentFrame];
		std::pair<std::string, s32> itemToFind = std::make_pair(itemUniqueId, toggleFlag);
		if (std::find(vec.begin(), vec.end(), itemToFind) == vec.end()){
			toggledObjectsAtFrame[currentFrame].push_back(
				std::pair<std::string, s32>(itemUniqueId, toggleFlag)
				);
			return true;
		}
		else{
			return false;
		}
		return false;
	}

	/* can break items at frame */
	//bool FK_Move::canBreakItemsAtFrame(u32 currentFrame){
	//	return (breakingObjectsAtFrame.count(currentFrame) > 0 && breakingObjectsAtFrame[currentFrame].size() > 0);
	//}

	/* get breaking items at frame */
	//const std::vector<std::string>& FK_Move::getBreakingItemsAtFrame(u32 currentFrame){
	//	if (breakingObjectsAtFrame.count(currentFrame) == 0){
	//		breakingObjectsAtFrame[currentFrame] = std::vector<std::string>();
	//	}
	//	return breakingObjectsAtFrame[currentFrame];
	//}

	/* add breaking item at frame - return false if already in list for current frame */
	//bool FK_Move::addBreakingItemAtFrame(u32 currentFrame, std::string itemUniqueId){
	//	if (breakingObjectsAtFrame.count(currentFrame) == 0){
	//		breakingObjectsAtFrame[currentFrame] = std::vector<std::string>();
	//	}
	//	if (std::find(breakingObjectsAtFrame[currentFrame].begin(), breakingObjectsAtFrame[currentFrame].end(), itemUniqueId) ==
	//		breakingObjectsAtFrame[currentFrame].end()){
	//		breakingObjectsAtFrame[currentFrame].push_back(itemUniqueId);
	//		return true;
	//	}
	//	else{
	//		return false;
	//	}
	//}

	/* can affect items at frame */
	//bool FK_Move::canAffectItemsAtFrame(u32 currentFrame){
	//	return canToggleItemsAtFrame(currentFrame) ||
	//		canBreakItemsAtFrame(currentFrame) ||
	//}

	/* set if move can hit grounded opponent */
	void FK_Move::setVsGroundedOpponentFlag(bool new_flag){
		vsGroundedOpponentFlag = new_flag;
	}

	void FK_Move::setAntiAirOnlyFlag(bool new_flag)
	{
		antiAirOnlyFlag = new_flag;
	}

	/* check if move can hit grounded opponent */
	bool FK_Move::canBeUsedVsGroundedOpponent(){
		return vsGroundedOpponentFlag;
	}

	/* check if move can hit crouched opponent */
	bool FK_Move::canBeUsedVsCrouchedOpponent(){
		return !canBeTriggered() || hasMidAttacks() || hasLowAttacks();
	}

	bool FK_Move::canBeUsedOnlyVsAirborneOpponents()
	{
		return antiAirOnlyFlag;
	}

	/* check / set if move has low hitting hitboxes */
	void FK_Move::setLowAttacksFlag(){
		hasLowAttacksFlag = false;
		for each(FK_Hitbox hitbox in hitboxCollection){
			if (hitbox.canBeCrouchGuarded() && !hitbox.canBeStandGuarded()){
				hasLowAttacksFlag = true;
				break;
			}
		}
		for each(FK_Bullet bullet in bullets) {
			if (bullet.getHitbox().getAttackType() && !bullet.getHitbox().canBeStandGuarded()) {
				hasLowAttacksFlag = true;
				break;
			}
		}
	}

	/* check / set if move has mid hitting hitboxes */
	void FK_Move::setMidAttacksFlag(){
		hasMidAttacksFlag = false;
		for each(FK_Hitbox hitbox in hitboxCollection){
			if ((hitbox.getAttackType() & FK_Attack_Type::MidAtks) != 0){
				hasMidAttacksFlag = true;
				break;
			}
		}
		for each(FK_Bullet bullet in bullets) {
			if ((bullet.getHitbox().getAttackType() & FK_Attack_Type::MidAtks) != 0){
				hasMidAttacksFlag = true;
				break;
			}
		}
	}

	/* check / set if move has throw hitboxes */
	void FK_Move::setThrowAttacksFlag(){
		hasThrowAttacksFlag = false;
		for each(FK_Hitbox hitbox in hitboxCollection){
			if ((hitbox.getAttackType() & FK_Attack_Type::ThrowAtk) != 0){
				hasThrowAttacksFlag = true;
			}
		}
	}

	// check if move has low attacks
	bool FK_Move::hasLowAttacks(){
		return hasLowAttacksFlag;
	}

	// check if move has mid attacks
	bool FK_Move::hasMidAttacks(){
		return hasMidAttacksFlag;
	}

	// check if move has throw attacks
	bool FK_Move::hasThrowAttacks(){
		return hasThrowAttacksFlag;
	}

	// set the "non-consuming" input buffer flag
	void FK_Move::setClearInputBufferOnExecutionFlag(bool newFlag){
		canClearInputBuffer_flag = newFlag;
	}

	// check if moves clear input buffer on execution
	bool FK_Move::canClearInputBufferOnExecution(){
		return canClearInputBuffer_flag;
	}


	// direction lock
	void FK_Move::setDirectionLockFlag(bool newLockFlag){
		directionLockFlag = newLockFlag;
	}
	bool FK_Move::getDirectionLockFlag(){
		return directionLockFlag;
	}
	// delay after move
	void FK_Move::setDelayAfterMoveFlag(bool newLockFlag){
		hasDelayAfterMoveEndsFlag = newLockFlag;
	}
	bool FK_Move::getDelayAfterMoveFlag(){
		return hasDelayAfterMoveEndsFlag;
	}
	// delay after move
	void FK_Move::setDelayAfterMoveTime(s32 new_delay){
		afterMoveDelay_ms = new_delay;
	}
	s32 FK_Move::getDelayAfterMoveTime(){
		return afterMoveDelay_ms;
	}
	// invincibility
	bool FK_Move::isInvincible(u32 frame, FK_Attack_Type type){
		if ((type & FK_Attack_Type::HighAtks) != 0 && getEndStance() == FK_Stance_Type::CrouchedStance){
			return true;
		}
		if ((type & FK_Attack_Type::LowAtks) != 0 && getEndStance() == FK_Stance_Type::AirStance){
			return true;
		}
		auto it = find(invincibilityFrames.begin(), invincibilityFrames.end(), frame);
		if (it == invincibilityFrames.end()){
			return false;
		}
		u32 index = it - invincibilityFrames.begin();
		return ((invincibilityType[index] & type) != 0);
	}
	void FK_Move::setInvincibilityFrame(u32 frame, FK_Attack_Type type){
		auto it = find(invincibilityFrames.begin(), invincibilityFrames.end(), frame);
		if (it != invincibilityFrames.end()) {
			u32 index = it - invincibilityFrames.begin();
			invincibilityType[index] = (FK_Attack_Type)(invincibilityType[index] | type);
		}
		else {
			invincibilityFrames.push_back(frame);
			invincibilityType.push_back(type);
		}
	}

	// armor
	bool FK_Move::hasArmor(u32 frame, FK_Attack_Type type) {
		auto it = find(armorFrames.begin(), armorFrames.end(), frame);
		if (it == armorFrames.end()) {
			return false;
		}
		u32 index = it - armorFrames.begin();
		return ((armorType[index] & type) != 0);
	}
	void FK_Move::setArmorFrame(u32 frame, FK_Attack_Type type) {
		auto it = find(armorFrames.begin(), armorFrames.end(), frame);
		if (it != armorFrames.end()) {
			u32 index = it - armorFrames.begin();
			armorType[index] = (FK_Attack_Type)(armorType[index] | type);
		}
		else {
			armorFrames.push_back(frame);
			armorType.push_back(type);
		}
	}
	// throws

	/* set as throw*/
	void FK_Move::setAsThrow(std::string newThrowReactionAnimation, int startFrame, int endFrame){
		throwFlag = true;
		moveThrow.targetAnimation = FK_Pose(newThrowReactionAnimation, startFrame, endFrame, false);
	}

	/* check if is throw */
	bool FK_Move::isThrow(){
		return throwFlag;
	}
	/* get throw animation */
	FK_Pose FK_Move::getThrowReactionAnimationPose(){
		if (!isThrow()){
			return FK_Pose();
		}
		return moveThrow.targetAnimation;
	};

	void FK_Move::setThrowReactionForTarget(FK_Reaction_Type newReaction){
		moveThrow.targetEndReaction = newReaction;
	}
	void FK_Move::setThrowReactionForPerformer(FK_Reaction_Type newReaction){
		moveThrow.performerEndReaction = newReaction;
	}
	void FK_Move::setThrowStanceForTarget(FK_Stance_Type newStance){
		moveThrow.targetEndStance = newStance;
	}
	void FK_Move::setThrowStartingDistance(f32 distance){
		moveThrow.startingDistance = distance;
	}
	void FK_Move::setThrowEscapeInput(u32 newInput){
		moveThrow.escapeInput = newInput;
	}
	FK_Reaction_Type FK_Move::getThrowReactionForTarget(){
		return moveThrow.targetEndReaction;
	}
	FK_Reaction_Type FK_Move::getThrowReactionForPerformer(){
		return moveThrow.performerEndReaction;
	}
	FK_Stance_Type FK_Move::getThrowStanceForTarget(){
		return moveThrow.targetEndStance;
	}
	f32 FK_Move::getThrowStartingDistance(){
		return moveThrow.startingDistance;
	}
	u32 FK_Move::getThrowEscapeInput(){
		return moveThrow.escapeInput;
	}

	/* get active frames */
	bool FK_Move::isActive(s32 currentFrame) {
		return currentFrame >= firstActiveFrame && currentFrame <= lastActiveFrame;
	}

	/* get last active frame */
	bool FK_Move::isStillActive(s32 currentFrame) {
		return currentFrame >= startingFrame && currentFrame <= lastActiveFrame;
	}

	/* get if move has projectile attack before the test frame */
	bool FK_Move::isShootingProjectileBeforeFrameNr(s32 currentFrame) {
		return firstFrameWithDamagingBullet < currentFrame;
	}

	f32 FK_Move::getTotalMoveDamage() {
		return totalMoveDamage;
	}

	f32 FK_Move::getTotalBulletDamage() {
		return totalBulletDamage;
	}

	s32 FK_Move::getMaximumBulletPriority() {
		return maximumBulletPriority;
	}

	void FK_Move::resetThrowTargetMovementArray(){
		u32 duration = moveThrow.targetAnimation.getEndingFrame() - moveThrow.targetAnimation.getStartingFrame() + 1;
		moveThrow.targetMovementAtFrame.clear();
		moveThrow.targetMovementAtFrame = std::vector<core::vector3df>(duration, core::vector3df(0, 0, 0));
	}
	void FK_Move::setThrowTargetMovementAtFrame(int frameNumber, float movementPar, float movementSide, float movementVert){
		if (frameNumber >= moveThrow.targetAnimation.getStartingFrame() && frameNumber <= moveThrow.targetAnimation.getEndingFrame()){
			s32 frame = frameNumber - moveThrow.targetAnimation.getStartingFrame();
			moveThrow.targetMovementAtFrame[frame] = 
				core::vector3df(movementPar, movementSide, movementVert);
		}
	};
	core::vector3df FK_Move::getThrowTargetMovementAtFrame(int frameNumber){
		if (frameNumber >= moveThrow.targetAnimation.getStartingFrame() && frameNumber <= moveThrow.targetAnimation.getEndingFrame()){
			s32 frame = frameNumber - moveThrow.targetAnimation.getStartingFrame();
			return moveThrow.targetMovementAtFrame[frame];
		}
		else{
			return core::vector3df(0, 0, 0);
		}
	};

	/* get camera settings*/
	FK_Move::FK_ThrowCameraSettings FK_Move::getThrowCameraSettings(){
		return moveThrow.cameraSettings;
	}
	/* set camera settings */
	void FK_Move::setThrowCameraSettings(f32 distance, f32 angle, f32 offsetY){
		moveThrow.cameraSettings.planarDistanceFromMiddlePoint = distance;
		moveThrow.cameraSettings.rotationAngleYFromCameraPlane = angle;
		moveThrow.cameraSettings.offsetPositionYFromMiddlePoint = offsetY;
	}

	void FK_Move::setThrowAsDamageScalable(){
		moveThrow.hasDamageScaling = true;
	}

	bool FK_Move::isAffectedByDamageScaling(){
		if (isThrow()){
			return moveThrow.hasDamageScaling;
		}
		return true;
	}

	/* can track after first hit */
	void FK_Move::setAfterHitTracking(){
		trackAfterHitFlag = true;
	}
	bool FK_Move::canTrackAfterHit(){
		return trackAfterHitFlag;
	}
	/* has reduced gravity */
	bool FK_Move::hasLowerGravity(){
		return lowerGravityFlag;
	}
	void FK_Move::setLowerGravityFlag(){
		lowerGravityFlag = true;
	}
	// get tracking angle on startup
	f32 FK_Move::getMaximumTrackingAngleOnStartup()
	{
		return maximumTrackingAngleOnStartup;
	}
	// set tracking angle on startup
	void FK_Move::setMaximumTrackingAngleOnStartup(f32 newAngle)
	{
		maximumTrackingAngleOnStartup = core::clamp(newAngle, 0.f, newAngle);
	}
	// get tracking angle on startup
	f32 FK_Move::getMaximumTrackingAngleDuringMovement()
	{
		return maximumTrackingAngleDuringMove;
	}
	// set tracking angle on startup
	void FK_Move::setMaximumTrackingAngleDuringMovement(f32 newAngle)
	{
		maximumTrackingAngleDuringMove = core::clamp(newAngle, 0.f, newAngle);
	}
	/* get/set maximum damage scaling */
	void FK_Move::setMaximumDamageScaling(f32 damageScaling) {
		maximumDamageScaling = damageScaling;
	}
	f32 FK_Move::getMaximumDamangeScaling() {
		return maximumDamageScaling;
	}
	/* check if it can cancel guard */
	bool FK_Move::canCancelGuard() {
		return cancelGuardFlag;
	}
	void FK_Move::setCancelGuardFlag() {
		if (inputStringLeft.empty()) {
			cancelGuardFlag = false;
		}
		else {
			u32 button = inputStringLeft[inputStringLeft.size() - 1];
			if ((button & FK_Input_Buttons::Guard_Button) != 0) {
				cancelGuardFlag = true;
			}
		}
	}
	// set additional movement speed after air move
	void FK_Move::setAdditionalFallVelocity(f32 axialVelocity)
	{
		additionalFallVelocityAfterAirMove = axialVelocity;
		if (additionalFallVelocityAfterAirMove == 0.f) {
			additionalFallVelocityAfterAirMove = 1.0f;
		}
	}
	f32 FK_Move::getAdditionalFallVelocity()
	{
		return additionalFallVelocityAfterAirMove;
	}
	bool FK_Move::hasConnectedWithTarget()
	{
		return moveHasHitTarget;
	}
	void FK_Move::markConnectionWithTarget()
	{
		moveHasHitTarget = true;
	}

	void FK_Move::setFollowupOrCancelOnHitOnly() {
		followupOrCancelOnHitOnlyFlag = true;
	}

	u32 FK_Move::getMoveId()
	{
		return moveId;
	}

	void FK_Move::setMoveId(u32 newId)
	{
		moveId = newId;
	}

	bool FK_Move::requiresPreciseInput()
	{
		return requiresPreciseInputFlag;
	}

	void FK_Move::setPreciseInputRequirementFlag(bool newFlag)
	{
		requiresPreciseInputFlag = newFlag;
	}

	bool FK_Move::AIonlyDuringOpponentAttack()
	{
		return AIflag_onlyDuringOpponentAttack;
	}

	void FK_Move::setAIonlyDuringOpponentAttackFlag(bool newFlag)
	{
		AIflag_onlyDuringOpponentAttack = newFlag;
	}

	bool FK_Move::canBeFollowedOrCanceledOnHitOnly() {
		return followupOrCancelOnHitOnlyFlag;
	}

	/* finalize and retrieve attack types */
	void FK_Move::finalizeAttackTypesFromHitboxes() {
		attackTypeAtFrame.clear();
		for (s32 i = startingFrame; i < endingFrame + 1; ++i) {
			attackTypeAtFrame.push_back(FK_Attack_Type::NoType);
		}
		for each(FK_Hitbox hitbox in hitboxCollection) {
			s32 firstActiveFrame = hitbox.getStartingFrame();
			s32 lastActiveFrame = hitbox.getEndingFrame();
			if (lastActiveFrame >= endingFrame) {
				lastActiveFrame = endingFrame;
			}
			if (firstActiveFrame < startingFrame) {
				firstActiveFrame = startingFrame;
			}
			for (s32 k = firstActiveFrame; k < lastActiveFrame + 1; ++k) {
				attackTypeAtFrame[k - startingFrame] = 
					(FK_Attack_Type)((u32)attackTypeAtFrame[k - startingFrame] | (u32)hitbox.getAttackType());
			}
		}
		setLowAttacksFlag();
		setMidAttacksFlag();
		setThrowAttacksFlag();
	}

	FK_Attack_Type FK_Move::getAttackTypeAtFrame(s32 currentFrame) {
		if (currentFrame >= firstActiveFrame && currentFrame <= lastActiveFrame) {
			s32 index = currentFrame - startingFrame;
			return attackTypeAtFrame[index];
		}
		else {
			return FK_Attack_Type::NoType;
		}
	}

	/* check if move is multichainable */
	bool FK_Move::canBeUsedMultipleTimesInChain()
	{
		return isMultiChainable;
	}

	/* set move as multichainable */
	void FK_Move::setAsMultichainable()
	{
		isMultiChainable = true;
	}

	bool FK_Move::canThrowAirborneOpponents()
	{
		return !noThrowOnAirborneFlag;
	}

	void FK_Move::setNoAirborneThrowFlag()
	{
		noThrowOnAirborneFlag = true;
	}

	// add counter attack
	void FK_Move::addCounterAttack(u32 startFrame, u32 endFrame, std::string moveName, FK_Attack_Type attackType){
		FK_CounterAttackMove newMove;
		newMove.activeFrames = std::pair<s32, s32>((s32)startFrame, (s32)endFrame);
		newMove.counterAttackMoveName = moveName;
		newMove.attackType = attackType;
	}

	/* force name to be displayed */
	bool FK_Move::hasForcedNameDisplay() {
		return forceNameDisplayFlag;
	}
	/* set force name display */
	void FK_Move::setForcedNameDisplay() {
		forceNameDisplayFlag = true;
	}

	/* has transformation */
	bool FK_Move::hasTransformation(){
		return moveTransformation.hasActiveTransformation;
	}
	/* has transformation */
	bool FK_Move::hasTransformationAtFrame(u32 currentFrame){
		return hasTransformation() && ((s32)currentFrame == moveTransformation.transformationFrame);
	}
	/* requires bullet counters */
	s32 FK_Move::getNumberOfRequiredBulletCounters(){
		return requiredTriggers;
	}
	/* get if move activates Trigger mode */
	bool FK_Move::getTriggerModeActivationFlag() {
		return activatesTrigger;
	}
	/* set if move activates trigger mode when used*/
	void FK_Move::setTriggerModeActivationFlag(bool newFlag) {
		activatesTrigger = newFlag;
	}
	/* set required bullet counters*/
	void FK_Move::setNumberOfRequiredBulletCounters(s32 counters){
		requiredTriggers = counters;
	}
	/* get transformation data */
	FK_Move::FK_TransformationMove& FK_Move::getTransformation(){
		return moveTransformation;
	}
	/* get move cinematic data*/
	FK_Move::FK_MoveCinematic& FK_Move::getCinematic() {
		return moveCinematic;
	}
	/* get if move has cinematic */
	bool FK_Move::hasCinematic() {
		return (moveCinematic.cinematicFrames.first != moveCinematic.cinematicFrames.second);
	}
	/* get if move has active cinematic */
	bool FK_Move::hasActiveMoveCinematic(u32 currentFrame) {
		return (hasCinematic() &&
			moveCinematic.cinematicFrames.first >= 0 &&
			(u32)moveCinematic.cinematicFrames.first <= currentFrame &&
			(u32)moveCinematic.cinematicFrames.second >= currentFrame);
	}

	/* get if move has impact attacks */
	bool FK_Move::isImpactAttack() {
		return hasImpactHitboxes;
	}

	void FK_Move::setAsImpactAttack() {
		hasImpactHitboxes = true;
	}

	//------------------------------ CAMERA SETTINGS ----------------------------//

	/* get current cinematic camera setting */
	FK_Move::FK_ThrowCameraSettings FK_Move::FK_MoveCinematic::getCurrentCameraSetting(s32 frame) {
		if (frame < cinematicFrames.first) {
			return FK_ThrowCameraSettings();
		}
		if (frame > cinematicFrames.second) {
			frame = cinematicFrames.second;
		}
		return cameraSettings[frame - cinematicFrames.first];
	}
};