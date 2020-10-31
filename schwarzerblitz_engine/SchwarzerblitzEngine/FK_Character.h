#ifndef FK_CHARACTER_CLASS
#define FK_CHARACTER_CLASS
	
#include<irrlicht.h>
#include<string>
#include<vector>
#include<mutex>
#include<map>
#include<fstream>
#include"FK_Move.h"
#include"FK_Pose.h"
#include"FK_Bullet.h"
#include"FK_AnimationKeyMap.h"
#include "FK_AnimationEndCallback.h"
#include "FK_DatabaseAccessor.h"
#include "FK_CharacterStats.h"

using namespace irr;

namespace fk_engine{

	class FK_AnimationEndCallback;
	typedef std::pair<u32, u32> FKComboUnit;

	class FK_Character{
	private:
		// impact cancel time window
		s32 MaximumTimeImpactCancel = 300;
		u32 ArmorSpecialEffectDurationMS = 300;
		s32 MaximumNumberOfBufferFramesForCancel = 10;
	public:
		// moves modifiers
		const std::string SimpleModeMoveFileTag = "simple_";
		const std::string BossModeMoveFileTag = "boss_";
		const std::string AlternativeModeMoveFileTag = "ex_";
		const enum FK_AIArchetypes : u32{
			Aggressor = 0,
			Tactician = 1,
			Luchador = 2,
			Balanced = 3,
			JackOfAllTrades = 4,
		};
		const enum FK_GroundRotationDirection : s32{
			NoRotation = 0,
			RotationTowardsForeground = 1,
			RotationTowardsBackground = -1
		};
		const enum FK_JumpDirection : s32{
			JumpUp = 0,
			JumpForward = 1,
			JumpBackward = -1,
		};
		/* keys for character meshs */
		const enum FK_OutfitFileKeys{
			OutfitFilename,
			OutfitName,
			OutfitAvailable,
			OutfitAlternativeCharacterName,
			OutfitAlternativeDisplayName,
			OutfitMeshScale,
			OutfitBackfaceCulling,
		};
		/* keys for character creation file */
		const enum FK_CharacterFileKeys{
			CharacterName,
			CharacterDisplayName,
			CharacterMesh,
			CharacterMeshBackFaceCulling,
			CharacterMeshScale,
			CharacterHitSFXFile,
			CharacterHitSFXFileTrigger,
			CharacterMovesFile,
			CharacterBonesFile,
			CharacterHitboxFile,
			CharacterHurtboxFile,
			CharacterLife,
			CharacterWalkSpeed,
			CharacterJumpSpeed,
			CharacterWeight,
			CharacterStanceFrames,
			CharacterWalkFrames,
			CharacterBackWalkFrames,
			CharacterSidestepFrames,
			CharacterStandGuardFrames,
			CharacterCrouchingStanceFrames,
			CharacterCrouchingGuardFrames,
			CharacterCrouchingFrames,
			CharacterRunningFrames,
			CharacterJumpingFrames,
			CharacterBackwardJumpFrames,
			CharacterForwardJumpFrames,
			CharacterHitWeakHighFrontFrames,
			CharacterHitWeakMediumFrontFrames,
			CharacterHitWeakLowFrontFrames,
			CharacterHitWeakHighBackFrames,
			CharacterHitWeakMediumBackFrames,
			CharacterHitWeakLowBackFrames,
			CharacterHitStrongHighFrames,
			CharacterHitStrongHighBackFrames,
			CharacterHitStrongMediumFrames,
			CharacterHitStrongMediumBackFrames,
			CharacterHitStrongLowFrames,
			CharacterHitStrongLowBackFrames,
			CharacterGroundedSupineFrames,
			CharacterGroundedProneFrames,
			CharacterGroundedProneRotationFrames,
			CharacterGroundedSupineRotationFrames,
			CharacterSupineLandingFrames,
			CharacterWinAnimationFrames,
			CharacterSelectionScreenAnimationFrames,
			CharacterIntroAnimationFrames,
			CharacterHitStrongFlightFrames,
			CharacterHitStrongFlightBackFrames,
			CharacterWakeUpFrames,
			SpecialStance1AnimationFrames,
			SpecialStance2AnimationFrames,
			SpecialStance3AnimationFrames,
			SpecialStance4AnimationFrames,
			SpecialStance5AnimationFrames,
			CharacterAIArchetype,
		};

		// outfit
		struct FK_CharacterOutfit{
			std::string outfitName = std::string();
			std::wstring outfitWName = std::wstring();
			std::string outfitFilename = std::string();
			bool isAvailableFromBeginning = true;
			std::string outfitDirectory = std::string();
			std::string outfitCharacterName = std::string();
			std::wstring outfitWCharacterName = std::wstring();
			std::string outfitDisplayName = std::string();
			std::wstring outfitWDisplayName = std::wstring();
			f32 outfitMeshScale = -1.0f;
			s32 outfitId = -1;
			bool backfaceCulling = false;
		};
		// planned move for buffer
		struct FK_PlannedMove {
			FK_Move* move = NULL;
			s32 activationFrame = 0;
			void reset() {
				activationFrame = 0;
				move = NULL;
			}
			bool isValid() {
				return (move != NULL && activationFrame >= 0);
			}
		};

		// additional hitboxes
		struct FK_CharacterAdditionalHitbox{
			FK_Bones_Type parent = FK_Bones_Type::RootArmature;
			FK_Hitbox_Type type;
			core::vector3df scale = core::vector3df(1.0f, 1.0f, 1.0f);
			core::vector3df positionOffset = core::vector3df(0.f, 0.f, 0.f);
		};

		// character additional objects
		struct FK_CharacterAdditionalObject{
			enum DamageConditionType : u32{
				Always = 0,
				ObjectBroken = 1,
				ObjectInactive = 2,
			};
			enum AfterUseAction :s32 {
				Hide = 0,
				Break = 1,
				Show = 2,
				Restore = 3,
				Toggle = 4,
				NoAction = -1,
			};
			struct DamageCondition{
				DamageConditionType type = DamageConditionType::Always;
				std::string stringParam = std::string();
				f32 numericParam = 0.f;
			};
			struct ObjectAttackBuff{
				FK_Attack_Type attackType = FK_Attack_Type::NoType;
				FK_Hitbox_Type type = FK_Hitbox_Type::AllHitboxes;
				f32 damageMultiplier = 1.0f;
			};
			struct ObjectDefenseBuff{
				FK_Attack_Type type = FK_Attack_Type::AllAtk;
				f32 damageMultiplier = 1.0f;
			};
			struct HUDbar {
				video::SColor color1 = video::SColor(255, 0, 196, 196);
				video::SColor color2 = video::SColor(255, 0, 120, 180);
				video::SColor backgroundColor = video::SColor(128, 0, 0, 0);
				video::SColor borderColor = video::SColor(255, 192, 192, 192);
				bool reverseDirection = false;
				bool showWhenInactive = false;
				bool reverseWhenBroken = false;
				core::rect<s32> position = core::rect<s32>(24, 305, 120, 10);
				bool showIcon = false;
				core::rect<s32> iconPosition = core::rect<s32>(12, 305, 12, 12);
			};
			struct HUDicon {
				bool showWhenActive = false;
				bool showWhenInactive = false;
				core::rect<s32> position = core::rect<s32>(12, 305, 12, 12);
			};
			std::string uniqueNameId = std::string();
			std::string displayName = std::string();
			FK_Bones_Type parent = FK_Bones_Type::RootArmature;
			core::vector3df scale = core::vector3df(1.0f, 1.0f, 1.0f);
			core::vector3df positionOffset = core::vector3df(0.f, 0.f, 0.f);
			core::vector3df rotationAngle = core::vector3df(0.f, 0.f, 0.f);
			core::vector3df landingRotationAngleMin = core::vector3df(0.f, 0.f, 0.f);
			core::vector3df landingRotationAngleMax = core::vector3df(0.f, 0.f, 0.f);
			core::vector3df landingPositionOffset = core::vector3df(0.f, 0.f, 0.f);
			std::map<FK_Bones_Type, std::string> additionalBones;
			std::vector<FK_CharacterAdditionalHitbox> additionalHitboxes;
			std::string meshFilename = std::string();
			std::vector<std::string> propMeshFilename = std::vector<std::string>();
			std::string meshPath = std::string();
			f32 maxDamageLimit = 0.f;

			s32 objectArrayIndex = -1;
			bool backfaceCulling = false;
			bool applyShadow = false;
			bool lightRemoved = false;

			// breakable
			bool breakable = false;
			bool canBeRestored = false;
			bool resetBetweenRounds = false;
			f32 maximumBreakingLikelihood = 10.0f;
			bool broken = false;
			u32 reactionAbleToBreakObject = FK_Reaction_Type::ReverseStrongFlight | FK_Reaction_Type::StrongFlight;
			f32 endurance = 0;
			f32 maxEndurance = 0;

			// functions
			void resetFlags();
			void toggleVisibility(bool newVisibilityFlag);
			bool needsRefresh();
			bool needsRefresh() const;
			void markAsRefreshed();
			bool checkIfBroken(FK_Reaction_Type& reactionToCheck);
			void applyDamage(f32 damage);
			void setLightAsRemoved();
			bool isActive();
			bool isActive() const;
			bool isBroken();
			bool isBroken() const;
			bool lifetimeHasExpired();
			bool maximumUsesReached();
			bool hasDummyMesh();
			bool canBeDamaged(FK_Character* parentCharacter);
			bool hasDummyMesh() const;
			bool canBeRemoved() const;
			bool canBeActivated() const;
			bool canBeShown() const;
			bool hasMoveToPerformOnBreak();
			// breaking and prop
			bool flyAwayWhenBroken = false;
			bool disappearWhenHitGround = false;
			bool rotateWhenTouchingGround = false;
			bool leavePropOnGroundWhenRestored = false;
			// visible
			bool visible = true;
			bool defaultVisibility = true;
			bool needsRefreshFlag = false;
			// toggling
			bool canBeToggledByMove = false;
			bool canBeBrokenByMove = false;
			bool canBeToggledByObject = false;
			bool canBeBrokenByObject = false;
			bool hideWhenHit = false;
			// animate with mesh
			bool animateWithMesh = false;
			bool showWhenHit = false;
			// pickable
			bool canBePickedFromGround = false;
			// useable
			bool useable = false;
			s32 numberOfUsages = 0;
			s32 maxNumberOfUsages = 0;
			AfterUseAction afterUsageAction = AfterUseAction::Hide;
			// time action
			bool expiresWithTime = false;
			f32 maxLifetime = 0.f;
			f32 lifetimeTimer = 0.f;
			bool expiresDuringMove = false;
			HUDbar hudBar;
			HUDbar hudLifeBar;
			HUDicon hudIcon;
			// show time bar in HUD
			bool showTimeBar = false;
			bool showEnduranceBar = false;
			std::string moveToPerformOnBreak = std::string();
			// automatic action after expiration (uses or time)
			AfterUseAction expirationAction = AfterUseAction::Hide;
			std::vector<DamageCondition> damageConditions = std::vector<DamageCondition>();
			std::vector<ObjectDefenseBuff> defenseBuffs = std::vector<ObjectDefenseBuff>();
			std::vector<ObjectAttackBuff> attackBuffs = std::vector<ObjectAttackBuff>();
			std::vector<std::pair<std::string, AfterUseAction>> objectToToggleWhenBroken = std::vector<std::pair<std::string, AfterUseAction>>();
			u32 armorWhenActive = 0;
			u32 invincibilityWhenActive = 0;
		};

		// scene props
		struct FK_SceneProp{
			FK_CharacterAdditionalObject object;
			bool hasCollided = false;
			core::vector3df velocity = core::vector3df(0.f, 100.f, 0.f);
			bool mustBeRemoved = false;
			bool mustSetPlayerCollision = false;
			s32 ownerId = -1;
		};

		// character textures
		struct FK_CharacterTexture{
			enum SwitchConditionType : u32{
				None = 0,
				ObjectBroken = 1,
				ObjectInactive = 2,
				OnStartup = 3,
				LifeLesserThanPercent = 4,
			};
			struct SwitchCondition{
				SwitchConditionType type = SwitchConditionType::None;
				std::string stringParam = std::string();
				f32 numericParam = 0.f;
			};
			std::string textureFileName = std::string();
			std::string texturePath = std::string();
			s32 materialId = -1;
			bool alreadySwitched = false;
			bool canBeSwitched(FK_Character* parentCharacter);
			std::vector<SwitchCondition> switchConditions = std::vector<SwitchCondition>();
		};

		// constructor
		FK_Character();
		// setup and initialize
		FK_Character(FK_DatabaseAccessor newDatabaseAccessor, std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr, core::vector3df startingPosition, core::vector3df startingRotation,
			int outfit_id = 0, f32 maxLifeScale = 1.0f, std::string newMovesetTag = std::string(), 
			std::string newAdditionalObjectTag = std::string(),
			bool loadAllAnimations = false);
		// destructor
		~FK_Character();
		// copy constructor
		void setCharacterSelectionVariablesFromCharacter(const FK_Character &, std::string additionalPathString = std::string());
		void setup(FK_DatabaseAccessor newDatabaseAccessor, std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr, core::vector3df startingPosition, core::vector3df startingRotation,
			s32 outfit_id = 0, f32 maxLifeScale = 1.0f, std::string newMovesetTag = std::string(), 
			std::string newAdditionalObjectTag = std::string(),
			bool loadAllAnimations = false);
		void setup(FK_DatabaseAccessor newDatabaseAccessor, std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr, core::vector3df startingPosition, core::vector3df startingRotation,
			std::string outfitDirectory, f32 maxLifeScale = 1.0f, std::string newMovesetTag = std::string(),
			std::string newAdditionalObjectTag = std::string(),
			bool loadAllAnimations = false);
		void loadBasicVariables(
			FK_DatabaseAccessor newDatabaseAccessor,
			std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr, int outfit_id = 0, f32 maxLifeScale = 1.0f,
			std::string newMovesetTag = std::string(),
			bool loadMoves = false, bool loadQuotes = false);
		void loadBasicVariables(FK_DatabaseAccessor newDatabaseAccessor, std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr, std::string outfitDirectory, f32 maxLifeScale = 1.0f, 
			std::string newMovesetTag = std::string(),
			bool loadMoves = false, bool loadQuotes = false);
		void loadVariablesForSelectionScreen(FK_DatabaseAccessor newDatabaseAccessor, std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr);
		void loadVariablesForSelectionScreenExtra(FK_DatabaseAccessor newDatabaseAccessor, std::string characterFileName, std::string parentDirectory, std::string commonDirectory,
			scene::ISceneManager* smgr);
		void loadMeshsAndEffects(core::vector3df startingPosition, core::vector3df startingRotation, 
			std::string newAdditionalObjectTag = std::string(),
			bool createHitbox = false, bool loadSFX = false, bool loadAllAnimations = false, bool loadAllProjectileMeshs = false);
		void loadHitboxHurtboxCollections();
		void loadProjectileMeshs();
		void loadMoveAnimations();
		// Graphics section
		scene::IAnimatedMeshSceneNode* getAnimatedMesh();
		std::map<FK_Hurtbox_Type, scene::ISceneNode*> &getHurtboxCollection();
		std::map<FK_Hitbox_Type, scene::ISceneNode*> &getHitboxCollection();
		std::deque<FK_Move> &getMovesCollection();
		// number of alternative meshes
		int getNumberOfOutfits();
		std::string getAnimationDirectory();
		void toggleHitboxVisibility(bool newVisibility);
		void toggleHurtboxVisibility(bool newVisibility);
		bool checkHitboxVisibility();
		bool checkHurtboxVisibility();
		std::string getArmatureBone(FK_Bones_Type boneType);
		std::string getMeshName();
		FK_Move* getCurrentMove();
		FK_Move* getNextMove();
		FK_Move* getLastStackedMove();
		bool hasActiveMove(bool afterStartup = true);
		FK_Attack_Type getCurrentAttackType();
		void toggleLighting(bool lightingActive);
		void toggleWireframe(bool wireframeActive);
		// outfit id
		int getOutfitId();
		// Game logic section
		float getLife();
		float getMaxLife();
		float getLifeRatio();
		float getLifePercentage();
		void setLifeToMax();
		int getCurrentFrame();
		float getDamageScalingMultiplier();
		void setLife(float newLifeValue);
		void receiveDamage(float damage, bool toObjects = true);
		bool isKO();
		bool hasFullLife();
		bool isGrounded();
		// trigger manager
		bool canTriggerCombo();
		bool canRevengeTrigger();
		int getTriggerCounters();
		void addTriggerCounters(int value);
		void useTriggerCounters(int value);
		void setTriggerCounters(s32 value);
		// perform new move
		bool canPerformNewMove(FK_Move* newMove);
		void updateMoveDelayCounter(f32 delta_t_s);
		void resetMoveDelayCounter();
		void applyNewMove(FK_Move* newMove);
		bool isMoveInCurrentCombo(FK_Move* moveToTest);
		bool performMove(FK_Move* newMove, bool forceExecution = false);
		void resetMove(bool resetTrigger = true);
		/* general update*/
		void update(u32 directionButtonPressed, f32 delta_t_s, bool& isNewMovePerformed);
		/* additional trigger check */
		void updateTriggeringFlag(u32 allButtonsPressed);
		/* check if the character has to be rotated when the move ends */
		void setTrackingFlag(bool flag = true);
		bool hasToTrackOpponent();
		/* check if player is on the left side of the screen*/
		bool isOnLeftSide();
		std::string getName();
		std::wstring getWName();
		std::string getOriginalName();
		std::wstring getWOriginalName();
		std::string getDisplayName();
		std::wstring getWDisplayName();
		bool canAcceptInput();
		bool canMove();
		core::vector3df getPosition();
		core::vector3df getRotation();
		void setPosition(core::vector3df newPosition);
		void setRotation(core::vector3df newRotation);
		void setOnLeftSide(bool isLeft);
		void setOutfitId(s32 new_outfitId);
		// Physics section
		float getWalkingSpeed();
		float getJumpSpeed();
		float getWeight();
		/* direction lock is used for removing moves tracking */
		bool isDirectionLocked();
		void setDirectionLock(bool newDirectionLock, bool safetyLock = false);
		core::vector3df getLockedDirection();
		void setLockedDirection(core::vector3df new_Direction);
		void revertDirectionLock();
		void setNextMovement(core::vector3df new_movement);
		bool hasToSetLockedDirection();
		bool isCurrentMovePartOfAChain();
		core::vector3df getNextMovement();
		void resetNextMovement();
		core::vector3df getVelocityPerSecond();
		void setVelocityPerSecond(core::vector3df new_velocity);
		// Animation section
		void setBasicAnimation(FK_BasicPose_Type poseType, 
			bool force_replacement = false, 
			bool invert_animation = false,
			bool remove_direction_lock = true);
		void basicPoseUpdate(u32 directionButtonPressed = 0, f32 delta_t_s = 0);
		bool isAnimationEnded();
		// replace basic animation
		void replaceBasicAnimation(FK_BasicPose_Type pose, std::string animationName, s32 startFrame, s32 endFrame, bool loop);
		// Particle System for hits
		void createHitParticleSystem();
		void createGuardParticleSystem();
		void activateParticleEffect(FK_Hitbox_Type hitboxId, bool guard = false);
		void activateParticleEffect(core::vector3df position, bool guard = false);
		void updateParticleEffect(f32 delta_t_s);
		scene::IParticleSystemSceneNode* getHitParticleEffect();
		scene::IParticleSystemSceneNode* getGuardParticleEffect();
		// reactions to hits
		void setReaction(FK_Reaction_Type reactionType, bool guardBreakTrigger, f32 hitDirection, 
			f32 histunMultiplier = 1.0);
		bool isHitStun();
		void updateReaction(f32 delta_t_s);
		bool canCancelHitstun(bool verticalVelocity);
		void cancelHitstun();
		// guard
		bool isGuarding();
		void breakGuard();
		bool canPerformGuard();
		// can evade bullets
		bool canEvadeBullets();
		void setRingWallCollisionFlag(bool newFlag);
		bool isTouchingWall();
		// run
		bool isRunning();
		bool isCancelingRun();
		void updateRunning(u32 frameDelta_Ms);
		void cancelRunning(u32 timeMs = 0);
		// trigger
		bool isTriggering();
		bool isTriggerComboed();
		bool isTriggerModeActive();
		void activateTriggerMode();
		void setTriggerModeFlag();
		void cancelTriggerMode();
		void setTriggerComboedFlag();
		void cancelTriggerComboedFlag();
		bool isTriggerGuardActive();
		void updateTriggerGuard(f32 delta_t_s);
		// combo counter
		void increaseComboCounter(f32 damage = 0, s32 moveId = -1, s32 moveHitboxId = -1);
		void resetComboCounter();
		u32 getComboCounter() const;
		s32 getComboDamage() const;
		u32 getNumberOfRepetitionOfSameMoveInCombo(s32 moveId, s32 hitboxId) const;
		// impact cancel
		void allowForImpactCancelCheck();
		void checkForImpactCancel(u32 lastButtonPressed);
		void performImpactCancel();
		void performStandingCancel();
		void updateImpactCancelCheck(u32 delta_t_ms);
		bool isImpactRunning();
		bool isInImpactCancelState();
		// jump
		bool canJump();
		void performJump(FK_JumpDirection jumpDirection, f32 additionalAxialvelocity = 0.f);
		bool isJumping();
		void cancelJump(bool leaveTriggerStateUnchanged = false);
		void cancelJumpVariables();
		bool canCancelJump();
		bool isAirborne();
		float getGravity();
		// check if player is being thrown
		/* throw methods */
		bool canBeThrown();
		bool isThrowingOpponent();
		bool isBeingThrown();
		void setThrowFlag(bool newThrowFlag);
		void setThrowMove(FK_Move* throwToApply);
		void setThrowMoveReaction(FK_Reaction_Type newReaction);
		void setThrowingOpponentFlag(bool newThrowFlag);
		void setThrowAnimation(FK_Pose reactionAnimation, core::vector3df rotationAngle);
		void clearThrow();
		FK_Reaction_Type getThrowReaction();
		FK_Stance_Type getThrowStance();
		// states
		void setStance(FK_Stance_Type stance);
		FK_Stance_Type getStance();
		void resetVariables(bool resetObjects = true);
		void resetObject(u32 objectId);
		// get head
		scene::IBoneSceneNode* getBone(FK_Bones_Type bonetype);
		// animation manager
		core::stringw getCurrentAnimationName();
		bool setNewAnimation(core::stringw animationName, s32 beginFrame, s32 endFrame, bool isLooping);
		// read intro quotes from file
		void loadIntroQuotes();
		// load win quotes from file
		void loadWinQuotes();
		// load win quotes from external file
		void loadWinQuotesFromCharacterDirectory(std::string characterPath, std::string characterName, 
			FK_CharacterOutfit characterOutfit);
		// load intro quotes from external file
		void loadIntroQuotesFromCharacterDirectory(std::string characterPath, std::string characterName, 
			FK_CharacterOutfit characterOutfit);
		// override intro quotes
		void overrideIntroQuotes(std::vector<std::wstring> newIntroQuotes, std::string characterName);
		// override round win quotes
		void overrideRoundWinQuotes(std::vector<std::wstring> newRoundWinQuotes, std::string characterName);
		// override match win quotes
		void overrideMatchWinQuotes(std::vector<std::wstring> newMatchWinQuotes, std::string characterName);
		// read intro quotes from file
		/* get random intro quote */
		std::wstring getRandomIntroQuote(std::string key = "generic");
		std::vector<std::wstring> &getIntroQuotes(std::string key = "generic");
		/* get random winning quote */
		std::wstring getRandomWinQuote(std::string key, bool matchEndFlag);
		/* get random winning (round) quote */
		std::wstring getRandomRoundWinQuote(std::string key = "generic");
		std::vector<std::wstring> &getRoundWinQuotes(std::string key = "generic");
		/* get random winning (match) quote */
		std::wstring getRandomMatchWinQuote(std::string key = "generic");
		std::vector<std::wstring> &getMatchWinQuotes(std::string key = "generic");
		/* get character directory */
		std::string getCharacterDirectory();
		/* check if character has been completely loaded */
		bool hasBeenLoaded();
		/* check if character can be shot out of the arena*/
		bool canSufferRingOut();
		/* set ring-out */
		void setRingoutFlag(bool ringout);
		/* get ring out status*/
		bool getRingoutFlag();
		/* set this flag if the player is in the "stage ringout zone" */
		void setInStageRingoutZoneFlag(bool new_flag_value);
		// reset canBeRingOut flag
		void resetCanSufferRingoutFlag();
		/* get if the player is in the "stage ringout zone" */
		bool getInStageRingoutZoneFlag();
		/* manage bullets */
		std::deque<FK_Bullet*> & getBulletsCollection();
		/* update bullets */
		void updateBullets(f32 frameDeltaTime);
		void markAllBulletsForDisposal();
		/* toggle animation transition */
		void allowAnimationTransitions();
		void stopAnimationTransitions();
		// get special stance dictionary
		std::map<FK_Stance_Type, std::string> getSpecialStanceDictionary();
		// get invincibility
		bool isInvincible(FK_Attack_Type);
		// get armor
		bool hasArmor(FK_Attack_Type);
		// recover from grounding
		void recoverFromGrounding(bool setIdleStance = true);
		// get AI archetype
		FK_AIArchetypes getAIArchetype();
		// set opponent's throw animations
		void setOpponentThrowAnimation(std::deque<FK_Move>& moves, std::string opponentAnimationDirectory);
		// get current mesh path
		std::string getOutfitPath();
		// get character outfit
		FK_Character::FK_CharacterOutfit getOutfit(s32 currentOutftiId = -1);
		// set ground flag
		void setGroundCollisionFlag(bool newFlag);
		bool getGroundCollisionFlag();
		// check if character can perform jump attack
		bool canPerformJumpAttack();
		// update jumping variables
		void updateJump(u32 delta_t_ms);

		// throw processing flag
		void setThrowProcessingFlag(bool flag);

		// additional objects
		void updateAdditionalObjects(f32 delta_t_s, bool forcePositionUpdate);
		void loadAdditionalObjects(std::string filenameTag);
		void registerAdditionalObjects();
		void loadAdditionalObjectsMesh();
		void setAdditionalObjectsMovesDependencies();
		void setAdditionalObjectsObjectsDependencies();
		void setPickableObjectId(std::string nameId);
		std::string getPickableObjectId();
		FK_CharacterAdditionalObject* getObjectByName(std::string uniqueNameId);
		void applyDamageToObjects(f32 damage);
		void checkForBreakingObjects(FK_Reaction_Type& reactionToCheck);
		void checkForItemsToggledByBreak(u32 objectIndex);
		void checkForItemsToggledByBreak(std::string itemId);
		void removeLightFromObject(int objectId);
		void refreshObject(int objectId);
		bool isPickingUpObject();
		void resetPickingObjectFlag();
		void synchronizeAnimatedObjects();
		// get additional objects
		const std::vector<scene::IAnimatedMeshSceneNode*> &getCharacterAdditionalObjectsNodes() const;
		const std::vector<FK_CharacterAdditionalObject> &getCharacterAdditionalObjects() const;
		// load additional hitboxes
		void loadAdditionalHitboxes();
		// get buffs from objects
		f32 getObjectsAttackMultiplier(FK_Hitbox_Type hitboxType, FK_Attack_Type attack_type);
		f32 getObjectsDefenseMultiplier(FK_Attack_Type attackType);
		u32 getTotalObjectArmor();
		u32 getTotalObjectInvincibility();
		/* textures */
		void checkTextureSwitches();
		void resetStartupTextures();
		void loadTextureVariablesAndTextures();
		void readTextureConfigurationFile();
		void applyStartupTextures();
		void loadAllTextures();

		/* cornered */
		bool isCornered();
		void setCorneredFlag(bool newFlag);

		/* activate pushback */
		void activatePushback();
		void updatePushback(u32 delta_t_ms);
		bool hasPushback();

		/* damage multipliers */
		f32 getReceivedPhysicalDamageMultiplier();
		f32 getReceivedBulletDamageMultiplier();
		void setReceivedPhysicalDamageMultiplier(f32 newMultiplier);
		void setReceivedBulletDamageMultiplier(f32 newMultiplier);

		/* hit sounds */
		bool canPlayHitSound();
		void resetSoundTimer();
		void updateSoundTimer(u32 delta_t_ms);

		/* transformation */
		void checkForTransformation(u32 delta_t_ms);
		void performTransformation();
		void prepareTransformation(FK_Move::FK_TransformationMove transformation);
		bool hasToTransform();
		bool hasActiveTransformation();
		const std::string& getCurrentTransformationId();
		const std::vector<std::string>& getAvailableTransformationNames();

		/* special effects */
		void activateArmorSpecialEffect();
		void disableArmorSpecialEffect();
		bool hasActiveArmorSpecialEffect();
		bool hasActiveTriggerCancelSpecialEffect();

		/* guard transition */
		bool canUpdateGuardTransition();
		void updateGuardTransition(u32 lastButtonPressed);

		/* damage received */
		void setLastDamagingMoveId(u32 moveId);
		u32 getLastDamagingMoveId();

		/* stats */
		FK_CharacterStats getCharacterStats();
	protected:
		void setDatabaseDrivenVariables();
		void loadCommonMoves();
		void parseMoves();
		void parseBones();
		void parseCharacterFile(std::string filename);
		void parseCharacterFileForSelectionScreen(std::string filename);
		void parseMeshFiles(const std::string& additionalFolder = std::string());
		void parseMeshFilesExtra(const std::string& additionalFolder = std::string());
		void parseAllMeshFilesDirectorySearch(const std::string& additionalFolder = std::string());
		void parseAllMeshFilesDirectorySearchExtra(const std::string& additionalFolder = std::string());
		void parseSingleCostume(std::map<FK_OutfitFileKeys, std::string>& outfitKeysMap, std::string costumeDirectory);
		void fillHitboxCollection();
		void fillHurtboxCollection();
		// build array for followup moves
		//void buildFollowupMoveCollection();
		scene::ISceneNode *createHurtbox(FK_Bones_Type parent, core::vector3df size, core::vector3df offset);
		// set animation
		bool setAnimation(core::stringw animationName);
		// add animation from directX file
		bool addAnimationFromDirectXFile(core::stringw animationName, std::string inputFileName);
	private:
		/* reference pointer to the irrlicht sceneManager*/
		scene::ISceneManager *smgr_reference;
		/* reference poiinter to the mesh */
		scene::ISkinnedMesh* mesh;
		/* reference pointer to the animated model*/
		scene::IAnimatedMeshSceneNode* animatedMesh;
		/* particle emitter used during succesful hits */
		scene::IParticleSystemSceneNode* hitSFX;
		/* particle emitter used during guarded hits */
		scene::IParticleSystemSceneNode* guardSFX;
		/* texture for particle system */
		video::ITexture* hitSFXTexture;
		/* texture for trigger particle system*/
		video::ITexture* hitSFXTextureTrigger;
		/* collection of all hurtboxes (used in collisions and damage calculation)*/
		std::map<FK_Hurtbox_Type, scene::ISceneNode*> hurtboxCollection;
		/* collection of all hitboxes (used in collisions and damage calculation)*/
		std::map<FK_Hitbox_Type, scene::ISceneNode*> hitboxCollection;
		/* collection of all logical bones, associated with their internal name*/
		std::map<FK_Bones_Type, std::string> armatureBonesCollection;
		/* collection of all the available moves */
		std::deque<FK_Move> movesCollection;
		/* collection of all the followup moves for a specific move */
		//std::deque<FK_Move> currentMoveFollowupCollection;
		/* move currently performed */
		FK_Move *currentMove;
		/* move used as a storage to check for some nasty air move properties */
		FK_Move *storageMove;
		/* move used for making cancels easier */
		FK_PlannedMove plannedMove;
		/* next move in the fluid combo system */
		//FK_Move *nextMove;
		std::deque<FK_Move*> nextMove;
		/* current stance */
		FK_Stance_Type currentStance;
		/* outfit id */
		int outfitId;
		/* current life*/
		float life;
		/* max life */
		float maxLife;
		/* life scaling factor */
		float lifeScalingFactor;
		/* ratio with which the damage is scaled while being hit by a combo (to be implemented)*/
		float damageScalingMultiplier;
		/* walking speed*/
		float walkingSpeed;
		/* sidestep speed*/
		float sidestepSpeed;
		/* mixed step speed*/
		float mixedstepSpeed;
		/* running speed */
		float runningSpeed;
		/* jump speed*/
		float jumpSpeed;
		/* weight (useless at the moment, but could be of any help in the future)*/
		float weight;
		/* combo counter (reset to zero after combo fails)*/
		int comboCounter;
		/* combo damage (reset to zero after combo fails)*/
		f32 comboDamage;
		/* number of trigger counters left */
		int triggerCounters;
		/* frame rate at which the animations are played */
		float animationFrameRate;
		/* variable which holds the number of the last animation frame played*/
		float lastFrame;
		/* transition time between animations in seconds */
		float animationTransitionTime;
		/* flag which says if the player is on the left or on the right side of the screen */
		bool isLeftPlayer;
		/* flag which says if the player is currently uninputable*/
		bool hitStunFlag;
		/* duration of the hitstun*/
		u32 hitStunDuration;
		/* duration of the hitstun movement */
		u32 hitStunMovementDuration;
		/* reference time for the hitstun beginning */
		u32 hitStunReferenceTime;
		/* flag which is used to see if a move had active hitboxes
		(after which, no delay is applied for non-running, non-no-hitboxes moves) */
		bool lastMoveNoHitboxFlag;
		/* flag which tells if the character is guarding */
		bool guardFlag;
		/* flag which tells if the next attack is Triggered or not*/
		bool triggerFlag;
		/* flag which tells if a player is being hit by a trigger combo */
		bool triggerComboedFlag;
		/* this flag is set to true when a trigger combo is succesfully started */
		bool triggerModeFlag;
		/* set trigger guard duration */
		f32 triggerGuardDurationS;
		/* name of the character*/
		std::string name;
		std::wstring wname;
		/* name which is displayed on screen*/
		std::string displayName;
		std::wstring wdisplayName;
		/* position of the animated mesh*/
		core::vector3df position;
		/* backup position of the player */
		core::vector3df positionAtPreviousFrame;
		/* flag which tells ifpalyer can be ringed-out*/
		bool canBeSentOutOfRing;
		/* rotation of the animated mesh*/
		core::vector3df rotation;
		/* movement to be processed by the SceneUpdater */
		core::vector3df nextMovement;
		/* velocity to be applied to next movement*/
		core::vector3df velocityPerSecond;
		/* direction which is used to perform a move (must not change during move execution, otherwise there is no point in 
		sidestepping!)*/
		core::vector3df lockedDirection;
		/* boolean which defines if the direction is locked or not */
		bool directionLock;
		bool originalDirectionLock;
		/* this is used to prevent stalling when characters are overlapped / there is no clear left or right character */
		bool safetyDirectionLock;
		/* boolean which defines if a move was used as a followup. Mainly considered during direction lock decisions */
		bool moveIsPartOfAChain;
		/* data section: directory in which the character files are stored */
		std::string characterDirectory;
		/* data section: directory in which the animations files are stored */
		std::string animationDirectory;
		/* data section: directory in which files in common between characters are stored */
		std::string commonDirectory;
		/* data section: directory in which shared animations files are stored */
		std::string sharedAnimationDirectory;
		/* name of the file which contains the whole character information*/
		std::string characterFileName;
		/* name of the file which matches each bone which each logical bone of the model*/
		std::string bonesFileName;
		/* hitbox defining file (real hitbox -> logical hitbox) - unused at the moment */
		std::string hitboxFileName;
		/* hitbox defining file (real hurtbox -> logical hurtbox) - unused at the moment*/
		std::string hurtboxFileName;
		/* name of the file which contains the moves of the player*/
		std::string moveFileName;
		/* name of the mesh file for the player outfit*/
		std::vector<std::string> meshFileNames;
		/* struct vector which contains the information about every single outfit*/
		std::vector<FK_CharacterOutfit> characterOutfits;
		/* struct vector which contains only the available outfits */
		std::vector<FK_CharacterOutfit> availableCharacterOutfits;
		/* name of the texture file for the hit particle emitter*/
		std::string particleSystemTextureFileName;
		/* name of the texture file for the hit particle emitter (trigger mode only)*/
		std::string particleSystemTextureFileNameTrigger;
		/* expiration time of the particles */
		f32 particleExpirationTime;
		/* reference time when particles where activated*/
		f32 particleReferenceTime;
		/* map which connects a string to a FileKey (used to read the character file)*/
		std::map<FK_CharacterFileKeys, std::string> characterFileKeyMap;
		/* pose frames */
		std::map<FK_BasicPose_Type, FK_Pose> basicAnimations;
		/* time delay (in ms) after a move is performed (canceled via Flow Combo)*/
		u32 delayAfterMoveTimeCounterMs;
		/* this variable set the minimum time required to perform a move after one is finished */
		u32 delayAfterMoveTimeMs;
		/* this variable set the minimum time required to move after a move is performed */
		u32 delayMovementAfterMoveTimeMs;
		/* set of names of the moves performed in a row. It gets reset after a combo ends or is interrupted.
		This is especially used to prevent repetition in Fluid combos */
		std::vector<std::string> movesPerformedInCurrentCombo;
		/* simple gravity acceleration for testing purposes */
		float testGravity;
		float jumpGravity;
		/* boolean which puts a limit on the player rotation on the ground */
		bool hasRotatedOnTheGround;
		/* integer which stores the rotation direction in order to avoid "worm dances" */
		FK_GroundRotationDirection groundRotationDirection;
		/* animation manager - current animation name */
		core::stringw currentAnimationName;
		/* animation keymap manager */
		std::map<core::stringw, FK_AnimationKeyMap*> animationKeymap;
		/* backface culling off for mesh*/
		bool setBackfaceCulling;
		/* set a flag to refresh the collision ellipsoid at the next update*/
		bool refreshCollisionEllipsoid;
		/* additional variable to check if the box has been updated at least once */
		bool wasCollisionEllipsoidRefreshed;
		/* quotes arrays*/
		std::map<std::string, std::vector<std::wstring> > introQuotes;
		std::map<std::string, std::vector<std::wstring> > roundWinQuotes;
		std::map<std::string, std::vector<std::wstring> > matchWinQuotes;
		/* character loaded boolean (used for multi-threading) */
		bool m_thread_characterLoaded;
		/* animation callback */
		FK_AnimationEndCallback* animationCallback;
		/* thrown flag */
		bool isThrown;
		/* throwing flag */
		bool isThrowing;
		/* suffered throw */
		FK_Move targetThrowToApply;
		/* ring out flag */
		bool isRingOut;
		/* this variable is set externally by the position updater */
		bool isInStageRingoutZone;
		/* collection of bullets shot by the player */
		std::deque<FK_Bullet*> bulletsCollection;
		/* mutex for multithread operations */
		std::mutex m_threadMutex;
		/* time counter for rolling limit */
		u32 rollingTimeCounterMs;
		u32 maximumRollingTimeMs;
		bool hitWhileGrounded;
		/* check if player is jumping */
		bool jumpingFlag;
		FK_BasicPose_Type storedJumpingAnimation;
		f32 collisionFlag;
		/* special stance dictionary*/
		std::map<FK_Stance_Type, std::string> specialStanceDictionary;
		/* AI archetype */
		FK_AIArchetypes AIarchetype;
		/* impact attack point */
		core::vector3df positionWhenHitByImpactAttack;
		/* running buffer timer */
		s32 runningBufferTimerMs;
		s32 runningBufferTimerMsTarget;
		/* impact cancel check */
		bool canCheckForImpactCancelInput;
		bool impactCancelRun_flag;
		bool hasUsedImpactCancelInCurrentCombo;
		/* air attacks */
		bool isPerformingAirAttack;
		bool hasTouchedGround;
		u32 jumpCounterMs;
		u32 jumpAttackThresholdMs;
		/* mesh scale */
		f32 meshScale;
		/* additional objects */
		std::vector<scene::IAnimatedMeshSceneNode*> characterAdditionalObjectsMeshs;
		std::vector<FK_CharacterAdditionalObject> characterAdditionalObjects;
		std::map<std::string, FK_CharacterAdditionalObject*> namedAdditionalObjectsReferences;
		/* received damage multipliers */
		f32 receivedPhysicalDamageMultiplier;
		f32 receivedBulletDamageMultiplier;
		/* has to track opponent */
		bool opponentTrackingFlag;
		/* moveset tag */
		std::string movesetTag;
		/* impact cancel timer counter */
		s32 impactCancelTimerCounter;
		s32 impactCancelShaderEffectTimer;
		/* last frame for object update */
		s32 lastFrameForObjectUpdate;
		/* pickable object */
		std::string pickableObjectNameId;
		/* is picking object */
		bool isPickingObjectFlag;
		/* object textures*/
		std::vector<FK_CharacterTexture> characterTextures;
		/* hit sound timer */
		s32 hitSoundTimer;
		/* bouncing counter */
		s32 bouncingCounter;
		/* is transforming */
		bool isTransforming;
		bool resetTransitionTime;
		bool throwProcessingFlag;
		/* list of available transformations */
		std::map<std::string, FK_Move::FK_TransformationMove> availableTransformations;
		std::vector<std::string> availableTransformationsNames;
		/* basic form to be restored at new round*/
		FK_Move::FK_TransformationMove baseForm;
		/* currentl loaded transformation */
		FK_Move::FK_TransformationMove currentForm;
		FK_Move::FK_TransformationMove transformationToApply;
		bool isInBaseForm;
		/* check if is sidestepping against a wall (bullet evasion clause) */
		bool isMovingAgainstWalls;
		/* special effects */
		bool specialEffectArmor;
		u32 specialEffectArmorTimeCounterMS;
		/* set if character is cornered */
		bool corneredFlag;
		/* pushback */
		core::vector3df pushbackVelocity;
		s32 pushbackDuration;
		/* last move from which damage was received */
		u32 lastMoveReceivedDamageId;
		/* combo map for Infinite Prevention System */
		std::map<FKComboUnit, u32> currentComboLog;
		/* ad-hoc check for Trigger button and Trigger guard */
		bool triggerButtonPressedBeforeHitstun;
		/* database accessor */
		FK_DatabaseAccessor databaseAccessor;
		/* character stats */
		FK_CharacterStats stats;
		/* is sidestepping */
		bool isSidestepping;
		s32 sidestepCounter;
	};
};
#endif