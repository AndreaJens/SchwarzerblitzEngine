#ifndef FK_MOVE_CLASS
#define FK_MOVE_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include<map>
#include<deque>
#include<fstream>
#include"FK_Hitbox.h"
#include"FK_FollowupMove.h"
#include"FK_Pose.h"
#include"FK_Bullet.h"

using namespace irr;

namespace fk_engine{

	class FK_Move{
	public:
		enum FK_ObjectToggleFlag : s32{
			Show = 1,
			Hide = -1,
			Toggle = 0,
			Restore = 2,
			Use = 3,
			Break = -2,
		};
		struct FK_CounterAttackMove{
			std::pair<s32, s32> activeFrames = std::pair<s32, s32>(-1, -1);
			std::string counterAttackMoveName = std::string();
			FK_Attack_Type attackType = FK_Attack_Type::NoType;
		};
		struct FK_ThrowCameraSettings{
			f32 planarDistanceFromMiddlePoint = 75.f;
			f32 rotationAngleYFromCameraPlane = 60.f;
			f32 offsetPositionYFromMiddlePoint = 12.0f;
			f32 targetYOffset = 0.f;
			s32 targetCharacter = 0;
			bool directCut = false;
		};
		// throw
		struct FK_ThrowMove{
			FK_Reaction_Type targetEndReaction = FK_Reaction_Type::NoReaction;
			FK_Reaction_Type performerEndReaction = FK_Reaction_Type::NoReaction;
			FK_Stance_Type targetEndStance = FK_Stance_Type::SupineStance;
			FK_Pose targetAnimation;
			FK_ThrowCameraSettings cameraSettings;
			u32 escapeInput = FK_Input_Buttons::Tech_Button;
			f32 startingDistance = -1.f;
			bool hasDamageScaling = false;
			std::vector<core::vector3df> targetMovementAtFrame;
		};
		// cinematic
		struct FK_MoveCinematic {
			std::vector<FK_ThrowCameraSettings> cameraSettings;
			f32 opponentTimerMultiplier = 0.1f;
			std::pair<s32, s32> cinematicFrames = std::pair<s32, s32>(0,0);
			FK_ThrowCameraSettings getCurrentCameraSetting(s32 currentFrame);
		};
		// transformation move
		struct FK_TransformationMove{
			bool hasActiveTransformation = false;
			bool isBasicForm = false;
			std::string newMeshName = std::string();
			s32 transformationFrame = -1;
			std::map<FK_BasicPose_Type, FK_Pose> newStances = std::map<FK_BasicPose_Type, FK_Pose>();
			std::string transformationNameTag = std::string();
			std::string previousFormNameTag = std::string();
			s32 durationMs = -1;
			bool resetEveryRound = true;
			bool hasBeenPerformed = false;
		};
	public:
		FK_Move();
		int getStartingFrame();
		int getEndingFrame();
		int getDuration();
		int getPriority();
		std::string getName();
		std::wstring getWDisplayName();
		std::string getDisplayName();
		std::string getAnimationName(bool leftSide = true);
		std::vector<FK_Hitbox> &getHitboxCollection();
		std::vector<FK_FollowupMove> &getFollowupMovesSet();
		std::vector<FK_FollowupMove> &getCancelIntoMovesSet();
		std::vector<FK_FollowupMove> getCopyOfFollowupMovesSet();
		std::vector<FK_FollowupMove> getCopyOfCancelIntoMovesSet();
		std::string getAutoFollowupMove();
		std::deque<u32> getInputStringLeft();
		std::deque<u32> getInputStringRight();
		std::deque<u32> getInputString(bool leftScreenSide);
		core::vector3df getMovementAtFrame(int frameNumber);
		std::string getSoundEffect(s32 frame, bool markAsPlayed = false);
		FK_Stance_Type getStance();
		FK_Stance_Type getEndStance();
		bool isListedInMoveList();
		bool isShownInHUD();
		bool hasFollowup();
		bool isFollowupOnly();
		bool canPlaySoundEffect(s32 frame = 0);
		bool canBeUsedVsGroundedOpponent();
		bool canBeUsedVsCrouchedOpponent();
		bool canBeUsedOnlyVsAirborneOpponents();
		f32 getMaxRange();
		f32 getMinRange();
		friend bool operator<(const FK_Move& m1, const FK_Move& m2);
		void setStartingFrame(int frame);
		void setEndingFrame(int frame);
		void setPriority(int priority);
		void setName(std::string name);
		void setDisplayName(std::string name);
		void setWDisplayName(std::wstring name);
		void setAnimationName(std::string animationName, bool leftSide = true);
		void setHitboxCollection(std::vector<FK_Hitbox>);
		void setFollowupMovesSet(std::vector<FK_FollowupMove>);
		void setVsGroundedOpponentFlag(bool new_flag);
		void setAntiAirOnlyFlag(bool new_flag);
		// set the vector for the moves this move can cancel into
		void setCancelIntoMovesSet(std::vector<FK_FollowupMove>);
		void setInputStringLeft(std::deque<u32>);
		void setInputStringRight(std::deque<u32>);
		void setMovementAtFrame(int frameNumber, float new_parMovement, float new_sideMovement, float new_vertMovement = 0);
		void setStance(FK_Stance_Type new_stance);
		void setEndStance(FK_Stance_Type new_stance);
		void setListed(bool new_listed_flag);
		void resetMovementFrameArray();
		void resetHitboxesHitFlag();
		void setRange(f32 newMinRange, f32 newMaxRange);
		void setFollowUpOnly(bool new_followup_only_flag);
		void setSoundEffectFlag(bool new_flag);
		void setAutoFollowupMove(std::string);
		// special buffer options
		bool canClearInputBufferOnExecution();
		void setClearInputBufferOnExecutionFlag(bool newFlag);
		// throws
		bool isThrow();
		void setThrowReactionForTarget(FK_Reaction_Type newReaction);
		void setThrowReactionForPerformer(FK_Reaction_Type newReaction);
		void setThrowStanceForTarget(FK_Stance_Type newStance);
		void setThrowStartingDistance(f32 distance);
		void setThrowEscapeInput(u32 inputButtons);
		FK_Reaction_Type getThrowReactionForTarget();
		FK_Reaction_Type getThrowReactionForPerformer();
		FK_Stance_Type getThrowStanceForTarget();
		f32 getThrowStartingDistance();
		u32 getThrowEscapeInput();
		void resetThrowTargetMovementArray();
		void setThrowTargetMovementAtFrame(int frameNumber, float movementPar, float movementSide, float movementVert);
		core::vector3df getThrowTargetMovementAtFrame(int frameNumber);
		void setThrowAsDamageScalable();
		/* get last active frame */
		bool isActive(s32 currentFrame);
		bool isStillActive(s32 currentFrame);
		bool isShootingProjectileBeforeFrameNr(s32 currentFrame);
		/* get camera settings*/
		FK_ThrowCameraSettings getThrowCameraSettings();
		/* set camera settings */
		void setThrowCameraSettings(f32 distance, f32 angle, f32 offsetY);
		/* damage scaling */
		bool isAffectedByDamageScaling();
		// bullet
		void addBullet(FK_Bullet new_bullet);
		s32 getMaximumBulletPriority();
		// sound effects
		void addSoundEffect(u32 frame, std::string identifier);
		void clearSoundEffects();
		void resetSoundsPlayed();
		std::deque<FK_Bullet> &getBulletCollection();
		// invincibility frames
		bool isInvincible(u32 frame, FK_Attack_Type attackType);
		void setInvincibilityFrame(u32 frame, FK_Attack_Type type);
		// armor frames
		bool hasArmor(u32 frame, FK_Attack_Type attackType);
		void setArmorFrame(u32 frame, FK_Attack_Type type);
		// range
		void setLowAttacksFlag();
		void setMidAttacksFlag();
		void setThrowAttacksFlag();
		bool hasLowAttacks();
		bool hasMidAttacks();
		bool hasThrowAttacks();
		// can be triggered
		bool canBeTriggered();
		void setAsThrow(std::string newThrowReactionAnimation, int startFrame, int endFrame);
		FK_Pose getThrowReactionAnimationPose();
		void setDirectionLockFlag(bool newLockFlag);
		bool getDirectionLockFlag();
		// get delay after move is performed
		void setDelayAfterMoveFlag(bool newLockFlag);
		bool getDelayAfterMoveFlag();
		void setDelayAfterMoveTime(s32 newDelay);
		s32 getDelayAfterMoveTime();
		// is taunt
		bool isTaunt();
		void setAsTaunt();
		//// is non-mirrorable
		bool cantBeTransversalMirrored();
		void applyUnmirrorableFlag();
		// maximum damage scaling
		void setMaximumDamageScaling(f32 damageScaling);
		f32 getMaximumDamangeScaling();
		// advanced tracking
		bool canTrackAfterAnimation();
		void setAfterAnimationTracking();
		/* can track before animation*/
		bool canTrackAtStartup();
		/* set the move to track before the animation plays */
		void setStartupTracking();
		/* can track after first hit */
		void setAfterHitTracking();
		bool canTrackAfterHit();
		/* has reduced gravity */
		bool hasLowerGravity();
		void setLowerGravityFlag();
		/* get maximum tracking on startup (default = 180deg) */
		f32 getMaximumTrackingAngleOnStartup();
		/* set maximum tracking angle on startup */
		void setMaximumTrackingAngleOnStartup(f32 newAngle);
		/* get maximum tracking during movement */
		f32 getMaximumTrackingAngleDuringMovement();
		/* set maximum tracking during movement (default = 90deg per sec) */
		void setMaximumTrackingAngleDuringMovement(f32 newAngle);
		/* add object to required items*/
		bool addRequiredObjectId(std::string itemId);
		/* add object to forbidden items*/
		bool addForbiddenObjectId(std::string itemId);
		/* check if object is required for move*/
		bool isObjectRequired(std::string itemId);
		/* check if object is forbidden for move*/
		bool isObjectForbidden(std::string itemId);
		/* check if move has items requirements*/
		bool hasItemRequirements();
		/* check if move can toggle item at frame */
		bool canToggleItemsAtFrame(u32 currentFrame);
		/* check if move can toggle item (at any frame)*/
		bool canActOnItem(std::string itemId, FK_ObjectToggleFlag actionFlag);
		bool canToggleItem(std::string itemId);
		bool canHideItem(std::string itemId);
		bool canShowItem(std::string itemId);
		bool canBreakItem(std::string itemId);
		bool canRestoreItem(std::string itemId);
		bool canUseItem(std::string itemId);
		/* can pick up item */
		bool canPickUpItem(std::string itemId);
		/* can pick up item at frame */
		bool canPickUpItemAtFrame(u32 frame, std::string itemId);
		/* get pickable item */
		std::string getPickableItemId();
		/* set pick up*/
		void setItemPickupAtFrame(u32 frame, std::string itemId);
		/* set move as pickup-only */
		void setAsPickupOnly();
		/* get if move is only available with pick-up*/
		bool isMoveAvailableWithoutPickup();
		/* add toggled item at frame */
		bool addToggledItemAtFrame(u32 currentFrame, std::string itemUniqueId, FK_ObjectToggleFlag toggleFlag);
		/* get required objects */
		const std::vector<std::string>& getRequiredObjects();
		/* get forbidden objects */
		const std::vector<std::string>& getForbiddenObjects();
		/* get toggled items */
		const std::vector<std::pair<std::string, s32> > & getToggledItemsAtFrame(u32 frame);
		// add counter attack
		void addCounterAttack(u32 startFrame, u32 endFrame, std::string moveName, FK_Attack_Type attackType);
		/* has transformation */
		bool hasTransformation();
		/* has transformation */
		bool hasTransformationAtFrame(u32 currentFrame);
		/* requires bullet counters */
		s32 getNumberOfRequiredBulletCounters();
		/* set requried bullet counters*/
		void setNumberOfRequiredBulletCounters(s32 counters);
		/* get if move activates trigger mode when used */
		bool getTriggerModeActivationFlag();
		/* set if move activates trigger mode when used*/
		void setTriggerModeActivationFlag(bool newFlag);
		/* get transformation data */
		FK_TransformationMove& getTransformation();
		/* get cinematic */
		FK_MoveCinematic& getCinematic();
		bool hasCinematic();
		bool hasActiveMoveCinematic(u32 currentFrame);
		/* force name to be displayed */
		bool hasForcedNameDisplay();
		void setForcedNameDisplay();
		/* check if it can cancel guard */
		bool canCancelGuard();
		void setCancelGuardFlag();
		/* set additional velocity on move end (only for air moves)*/
		void setAdditionalFallVelocity(f32 axialVelocity);
		/* check additional velocity on move end (only for air moves)*/
		f32 getAdditionalFallVelocity();
		/* check if move has hit target */
		bool hasConnectedWithTarget();
		/* set hit*/
		void markConnectionWithTarget();
		/* get total move damage */
		f32 getTotalMoveDamage();
		f32 getTotalBulletDamage();
		/* get if move can send the opponent out of the ring OR canceled as an impact run*/
		bool isImpactAttack();
		void setAsImpactAttack();
		/* get if move can be canceled/followed only on hit */
		bool canBeFollowedOrCanceledOnHitOnly();
		void setFollowupOrCancelOnHitOnly();
		/* set and get move id */
		u32 getMoveId();
		void setMoveId(u32 newId);
		/* set and get precise input flag */
		bool requiresPreciseInput();
		void setPreciseInputRequirementFlag(bool newFlag);
		/* set move as usable only during enemy attack */
		bool AIonlyDuringOpponentAttack();
		void setAIonlyDuringOpponentAttackFlag(bool newFlag);
		/* finalize attack types for AI */
		void finalizeAttackTypesFromHitboxes();
		FK_Attack_Type getAttackTypeAtFrame(s32 frame);
		/* check if move is multichainable */
		bool canBeUsedMultipleTimesInChain();
		void setAsMultichainable();
		/* flag for throws that cannot hit airborne opponents */
		bool canThrowAirborneOpponents();
		void setNoAirborneThrowFlag();
		// clear everything
		void clear();
	private:
		int startingFrame;
		int endingFrame;
		int movePriority;
		bool listedInMoveList;
		bool followupOnly;
		f32 moveMinRange;
		f32 moveMaxRange;
		std::string moveName;
		std::string displayName;
		std::wstring displayWName;
		std::string moveAnimationName;
		std::string moveAnimationNameRightSide;
		FK_Stance_Type stance;
		FK_Stance_Type endstance;
		std::vector<FK_Hitbox> hitboxCollection;
		std::vector<FK_FollowupMove> followupMoves;
		std::vector<FK_FollowupMove> cancelIntoMoves;
		std::deque<u32> inputStringLeft;
		std::deque<u32> inputStringRight;
		std::deque<FK_Bullet> bullets;
		std::map<u32, std::string> soundsToPlay;
		std::map<u32, bool> soundsPlayed;
		std::vector<core::vector3df> movementFrame;
		std::vector<u32> invincibilityFrames;
		std::vector<FK_Attack_Type> invincibilityType;
		std::vector<u32> armorFrames;
		std::vector<FK_Attack_Type> armorType;
		int getMovementFrameId(int frameNumber);
		// throw
		bool throwFlag;
		FK_ThrowMove moveThrow;
		FK_TransformationMove moveTransformation;
		std::string autoFollowupMoveName;
		// sfx
		bool playSoundEffectFlag;
		// a flag which set if the move has low attacks (useful for AI training)
		bool hasLowAttacksFlag;
		bool hasMidAttacksFlag;
		bool hasThrowAttacksFlag;
		bool directionLockFlag;
		bool hasDelayAfterMoveEndsFlag;
		// vs grounded opponent flag (useful for AI)
		bool vsGroundedOpponentFlag;
		bool antiAirOnlyFlag;
		// can clear input buffer on execution
		bool canClearInputBuffer_flag;
		// is taunt move
		bool isTauntMove;
		// can be used more than once in same chain
		bool isMultiChainable;
		/* flag for throws that cannot hit airborne opponents */
		bool noThrowOnAirborneFlag;
		// transversal mirror (check if UP and DOWN have to be mirrored when the move is performed from the other side)
		bool transversalMirrorFlag;
		// object moves (each string is a required object id)
		std::vector<std::string> requiredObjectsUniqueIds;
		std::vector<std::string> forbiddenObjectsUniqueIds;
		std::map<u32, std::vector<std::pair<std::string, s32> > > toggledObjectsAtFrame;
		//std::map<u32, std::vector<std::string> > breakingObjectsAtFrame;
		std::pair<u32, std::string> pickupItem;
		bool trackAfterHitFlag;
		bool lowerGravityFlag;
		std::vector<FK_CounterAttackMove> counterattackMoves;
		s32 afterMoveDelay_ms;
		f32 requiredSuperMeterUnits;
		f32 requiredTriggers;
		FK_MoveCinematic moveCinematic;
		bool pickupOnlyMove;
		bool activatesTrigger;
		// tracking angles
		f32 maximumTrackingAngleOnStartup;
		f32 maximumTrackingAngleDuringMove;
		// after animation tracking (rotate the character after the animation ends to face the opponent)
		bool afterAnimationTrackingFlag;
		bool beforeAnimationTrackingFlag;
		// force name display
		bool forceNameDisplayFlag;
		// can cancel guard
		bool cancelGuardFlag;
		// maximum damage scaling
		f32 maximumDamageScaling;
		// additional fall velocity
		f32 additionalFallVelocityAfterAirMove;
		// check if move has connected at least once
		bool moveHasHitTarget;
		// first/last active frame
		s32 firstActiveFrame;
		s32 lastActiveFrame;
		s32 firstFrameWithDamagingBullet;
		// maximum bullet priority
		s32 maximumBulletPriority;
		s32 firstBulletFrame;
		f32 totalMoveDamage;
		f32 totalBulletDamage;
		bool hasImpactHitboxes;
		// cancel on hit only property
		bool followupOrCancelOnHitOnlyFlag;
		// move id - used for fast comparisons
		u32 moveId;
		// requires precise input
		bool requiresPreciseInputFlag;
		// AI indication: only when opponent attacks
		bool AIflag_onlyDuringOpponentAttack;
		// attack type hitboxes
		std::vector<FK_Attack_Type> attackTypeAtFrame;
	};
};
#endif
