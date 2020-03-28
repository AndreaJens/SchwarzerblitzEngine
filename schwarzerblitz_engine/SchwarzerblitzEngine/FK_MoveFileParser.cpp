#include"FK_MoveFileParser.h"
#include"FK_Bullet.h"
#include<sstream>
#include<iostream>

namespace fk_engine{

	/* parse move file given its filename */
	FK_MoveFileParser::FK_MoveFileParser(){
		setupParserMap();
	};
	FK_MoveFileParser::FK_MoveFileParser(std::string fileName){
		setupParserMap();
		resetAllVariables();
		std::ifstream moveInputFile(fileName.c_str());
		parseMovesFromFile(moveInputFile);
		moveInputFile.close();
	};
	FK_MoveFileParser::~FK_MoveFileParser(){
		parsedMoves.clear();
		readerMap.clear();
		attackTypeMap.clear();
		readerMap.clear();
		hitboxTypeMap.clear();
		reactionTypeMap.clear();
		cinematicAttributesMap.clear();
		inputMap.clear();
		stanceTypeMap.clear();
		availableTransformations.clear();
	};
	void FK_MoveFileParser::setupParserMap(){
		/* define the identifiers and associate them with the required enum */
		readerMap[FK_FileKey_Type::MoveIdentifier] = "#MOVE";
		readerMap[FK_FileKey_Type::MoveAnimationIdentifier] = "#ANIMATION";
		readerMap[FK_FileKey_Type::FileIdentifierEnd] = "#END";
		readerMap[FK_FileKey_Type::FramesIdentifier] = "#FRAMES";
		readerMap[FK_FileKey_Type::FollowUpIdentifier] = "#FOLLOWUP";
		readerMap[FK_FileKey_Type::FollowUpIdentifierEnd] = "#FOLLOWUP_END";
		readerMap[FK_FileKey_Type::MovementIdentifier] = "#MOVEMENT";
		readerMap[FK_FileKey_Type::MovementIdentifierEnd] = "#MOVEMENT_END";
		readerMap[FK_FileKey_Type::InputIdentifier] = "#INPUT";
		readerMap[FK_FileKey_Type::InputIdentifierEnd] = "#INPUT_END";
		readerMap[FK_FileKey_Type::HitboxIdentifier] = "#HITBOX";
		readerMap[FK_FileKey_Type::HitboxIdentifierEnd] = "#HITBOX_END";
		readerMap[FK_FileKey_Type::StanceIdentifier] = "#STANCE";
		readerMap[FK_FileKey_Type::EndStanceIdentifier] = "#NEWSTANCE";
		readerMap[FK_FileKey_Type::RangeIdentifier] = "#RANGE";
		readerMap[FK_FileKey_Type::FollowupOnlyIdentifier] = "#FOLLOWUP_ONLY";
		readerMap[FK_FileKey_Type::ThrowIdentifier] = "#THROW";
		readerMap[FK_FileKey_Type::ThrowIdentifierEnd] = "#THROW_END";
		readerMap[FK_FileKey_Type::BulletIdentifier] = "#BULLET";
		readerMap[FK_FileKey_Type::BulletIdentifierEnd] = "#BULLET_END";
		readerMap[FK_FileKey_Type::NoSFXIndentifier] = "#NO_SOUNDS";
		readerMap[FK_FileKey_Type::MoveAutoFollowup] = "#AUTO_FOLLOWUP";
		readerMap[FK_FileKey_Type::MoveCancelInto] = "#CANCEL_INTO";
		readerMap[FK_FileKey_Type::MoveCancelIntoEnd] = "#CANCEL_INTO_END";
		readerMap[FK_FileKey_Type::WorksVSGroundedOpponent] = "#VS_GROUNDED";
		readerMap[FK_FileKey_Type::WorksVSAirOpponent] = "#ANTI_AIR_ONLY";
		readerMap[FK_FileKey_Type::NoDirectionLock] = "#NO_DIRECTION_LOCK";
		readerMap[FK_FileKey_Type::NoDelayAfterMove] = "#NO_DELAY";
		readerMap[FK_FileKey_Type::DisplayNameIdentifier] = "#DISPLAY_NAME";
		readerMap[FK_FileKey_Type::SoundFramesIdentifier] = "#SOUNDS";
		readerMap[FK_FileKey_Type::SoundFramesIdentifier_End] = "#SOUNDS_END";
		readerMap[FK_FileKey_Type::RightSideAnimation] = "#ANIMATION_RIGHTSIDE";
		readerMap[FK_FileKey_Type::InvincibilityFrames] = "#INVINCIBILITY_FRAMES";
		readerMap[FK_FileKey_Type::AdvInvincibility] = "#INVINCIBILITY_FRAMES_AGAINST";
		readerMap[FK_FileKey_Type::AdvInvincibilityEnd] = "#INVINCIBILITY_FRAMES_AGAINST_END";
		readerMap[FK_FileKey_Type::ArmorFrames] = "#ARMOR_FRAMES_AGAINST";
		readerMap[FK_FileKey_Type::ArmorFramesEnd] = "#ARMOR_FRAMES_AGAINST_END";
		readerMap[FK_FileKey_Type::ThrowEscapeInput] = "#THROW_ESCAPE_INPUT";
		readerMap[FK_FileKey_Type::ThrowEscapeInputEnd] = "#THROW_ESCAPE_INPUT_END";
		readerMap[FK_FileKey_Type::NonBufferConsuming] = "#NO_BUFFER_CLEAR";
		readerMap[FK_FileKey_Type::TauntMoveIdentifier] = "#TAUNT";
		readerMap[FK_FileKey_Type::NoRightSideReversal] = "#NO_RIGHTSIDE_MIRROR";
		readerMap[FK_FileKey_Type::RotateTowardOpponentAfterAnimation] = "#TRACK_AFTER_ANIMATION";
		readerMap[FK_FileKey_Type::RotateTowardOpponentBeforeAnimation] = "#TRACK_BEFORE_ANIMATION";
		readerMap[FK_FileKey_Type::OnlyWithSpecificObject] = "#ONLY_WITH_OBJECT";
		readerMap[FK_FileKey_Type::OnlyWithoutSpecificObject] = "#ONLY_WITHOUT_OBJECT";
		readerMap[FK_FileKey_Type::ToggleItemIdentifier] = "#MODIFY_OBJECT_AT_FRAME";
		readerMap[FK_FileKey_Type::PickupItemIdentifier] = "#PICKUP_OBJECT_AT_FRAME";
		readerMap[FK_FileKey_Type::PickupOnlyMoveIdentifier] = "#PICKUP_ONLY";
		readerMap[FK_FileKey_Type::LowerGravityIdentifier] = "#REDUCE_GRAVITY";
		readerMap[FK_FileKey_Type::TrackAfterHitIdentifier] = "#TRACK_ON_HIT";
		readerMap[FK_FileKey_Type::CounterattackIdentifier] = "#COUNTERATTACK";
		readerMap[FK_FileKey_Type::CounterattackEndIdentifier] = "#COUNTERATTACK_END";
		readerMap[FK_FileKey_Type::DelayAfterMoveMSIdentifier] = "#DELAY_AFTER_MOVE_MS";
		readerMap[FK_FileKey_Type::BulletCountersRequiredIdentifier] = "#REQUIRES_BULLET_COUNTERS";
		readerMap[FK_FileKey_Type::ActivateTriggerModeIdentifier] = "#ACTIVATE_TRIGGER_MODE";
		readerMap[FK_FileKey_Type::TransformationIdentifier] = "#TRANSFORMATION";
		readerMap[FK_FileKey_Type::TransformationIdentifierEnd] = "#TRANSFORMATION_END";
		readerMap[FK_FileKey_Type::MaximumTrackingAngleStartupIdentifier] = "#MAXIMUM_TRACKING_ANGLE_STARTUP";
		readerMap[FK_FileKey_Type::MaximumTrackingAngleMovementIdentifier] = "#MAXIMUM_STEERING_ANGLE";
		readerMap[FK_FileKey_Type::ForceNameDisplayIdentifier] = "#FORCE_NAME_DISPLAY";
		readerMap[FK_FileKey_Type::MaximumDamageScalingIdentifier] = "#MINIMUM_DAMAGE_RATIO_AFTER_SCALING";
		readerMap[FK_FileKey_Type::StopTimeCinematicIdentifier] = "#CINEMATIC";
		readerMap[FK_FileKey_Type::StopTimeCinematicIdentifierEnd] = "#CINEMATIC_END";
		readerMap[FK_FileKey_Type::AdditionalFallVelocityIdentifier] = "#ADDITIONAL_VELOCITY_AFTER_AIR_MOVE";
		readerMap[FK_FileKey_Type::PriorityIdentifier] = "#MOVELIST_PRIORITY";
		readerMap[FK_FileKey_Type::CancelOrFollowupOnHitOnlyIdentifier] = "#NO_CANCELS_ON_WHIFF";
		readerMap[FK_FileKey_Type::TightInputIdentifier] = "#REQUIRES_PRECISE_INPUT";
		readerMap[FK_FileKey_Type::MultichainableMoveIdentifier] = "#MULTICHAINABLE";
		readerMap[FK_FileKey_Type::OnlyDuringEnemyAttackIdentifier] = "#ONLY_WHEN_OPPONENT_ATTACKS";
		/* do the same for hitbox type... */
		hitboxTypeMap["None"] = FK_Hitbox_Type::NoHitbox;
		hitboxTypeMap["RightPunch"] = FK_Hitbox_Type::RightPunch;
		hitboxTypeMap["RightElbow"] = FK_Hitbox_Type::RightElbow;
		hitboxTypeMap["RightKnee"] = FK_Hitbox_Type::RightKnee;
		hitboxTypeMap["RightFoot"] = FK_Hitbox_Type::RightFoot;
		hitboxTypeMap["LeftPunch"] = FK_Hitbox_Type::LeftPunch;
		hitboxTypeMap["LeftElbow"] = FK_Hitbox_Type::LeftElbow;
		hitboxTypeMap["LeftKnee"] = FK_Hitbox_Type::LeftKnee;
		hitboxTypeMap["LeftFoot"] = FK_Hitbox_Type::LeftFoot;
		hitboxTypeMap["Extra1"] = FK_Hitbox_Type::AdditionalHitbox1;
		hitboxTypeMap["Extra2"] = FK_Hitbox_Type::AdditionalHitbox2;
		hitboxTypeMap["Extra3"] = FK_Hitbox_Type::AdditionalHitbox3;
		hitboxTypeMap["Extra4"] = FK_Hitbox_Type::AdditionalHitbox4;
		hitboxTypeMap["Extra5"] = FK_Hitbox_Type::AdditionalHitbox5;
		hitboxTypeMap["Extra6"] = FK_Hitbox_Type::AdditionalHitbox6;
		hitboxTypeMap["Extra7"] = FK_Hitbox_Type::AdditionalHitbox7;
		hitboxTypeMap["Extra8"] = FK_Hitbox_Type::AdditionalHitbox8;
		hitboxTypeMap["Extra9"] = FK_Hitbox_Type::AdditionalHitbox9;
		hitboxTypeMap["Extra10"] = FK_Hitbox_Type::AdditionalHitbox10;
		hitboxTypeMap["Head"] = FK_Hitbox_Type::Head;
		hitboxTypeMap["Torso"] = FK_Hitbox_Type::Torso;
		/* ... hitbox identifiers... */
		hitboxGuardBreakIdentifier = "!";
		hitboxSingleHitIdentifier = "1hit";
		hitboxMultiHitIdentifier = "multiHit";
		hitboxStartFrameIdentifier = "start";
		hitboxEndFrameIdentifier = "end";
		attackTypeMap["None"] = FK_Attack_Type::NoType;
		attackTypeMap["High"] = FK_Attack_Type::HighAtk;
		attackTypeMap["Mid"] = FK_Attack_Type::MediumAtk;
		attackTypeMap["Low"] = FK_Attack_Type::LowAtk;
		attackTypeMap["LowS"] = FK_Attack_Type::SpecialLowAtk;
		attackTypeMap["MidS"] = FK_Attack_Type::SpecialMediumAtk;
		attackTypeMap["Throw"] = FK_Attack_Type::MidThrowAtk;
		attackTypeMap["HighThrow"] = FK_Attack_Type::HighThrowAtk;
		attackTypeMap["MidThrow"] = FK_Attack_Type::MidThrowAtk;
		attackTypeMap["LowThrow"] = FK_Attack_Type::LowThrowAtk;
		attackTypeMap["HighHitThrow"] = FK_Attack_Type::HighBlockableThrow;
		attackTypeMap["MidHitThrow"] = FK_Attack_Type::MidBlockableThrow;
		attackTypeMap["LowHitThrow"] = FK_Attack_Type::LowBlockableThrow;
		attackTypeMapInvincibility["HighAtks"] = FK_Attack_Type::HighAtks;
		attackTypeMapInvincibility["LowAtks"] = FK_Attack_Type::LowAtks;
		attackTypeMapInvincibility["MidAtks"] = FK_Attack_Type::MidAtks;
		attackTypeMapInvincibility["UpperbodyAtks"] = FK_Attack_Type::HighAndMidAtks;
		attackTypeMapInvincibility["LowerbodyAtks"] = FK_Attack_Type::MidAndLowAtks;
		attackTypeMapInvincibility["HighAndLowAtks"] = FK_Attack_Type::HighAndLowAtks;
		attackTypeMapInvincibility["Throws"] = FK_Attack_Type::ThrowAtk;
		attackTypeMapInvincibility["Full"] = FK_Attack_Type::AllAtk;
		attackTypeMapInvincibility["All"] = FK_Attack_Type::AllAtk;
		attackTypeMapInvincibility["AllAtks"] = FK_Attack_Type::AllAtk;
		attackTypeMapInvincibility["Projectiles"] = FK_Attack_Type::BulletAttack;
		attackTypeMapInvincibility["Bullets"] = FK_Attack_Type::BulletAttack;
		attackTypeMapInvincibility["AtksAndBullets"] = 
			FK_Attack_Type((u32)(FK_Attack_Type::AllAtk) | (u32)(FK_Attack_Type::BulletAttack));
		/* ... bullet keys... */
		bulletAttributesMap[FK_FileKey_BulletType::BulletMeshName] = "#mesh";
		bulletAttributesMap[FK_FileKey_BulletType::BulletHitbox] = "#hitbox";
		bulletAttributesMap[FK_FileKey_BulletType::BulletHitboxEnd] = "#hitbox_end";
		bulletAttributesMap[FK_FileKey_BulletType::BulletRange] = "#range";
		bulletAttributesMap[FK_FileKey_BulletType::BulletVelocity] = "#velocity";
		bulletAttributesMap[FK_FileKey_BulletType::BulletHitboxRadius] = "#hitbox_radius";
		bulletAttributesMap[FK_FileKey_BulletType::BulletBillboardTexture] = "#billboard";
		bulletAttributesMap[FK_FileKey_BulletType::BulletReflection] = "#reflection";
		bulletAttributesMap[FK_FileKey_BulletType::BulletScale] = "#scale";
		bulletAttributesMap[FK_FileKey_BulletType::BulletRotation] = "#angle";
		bulletAttributesMap[FK_FileKey_BulletType::BulletShadow] = "#apply_shadow";
		bulletAttributesMap[FK_FileKey_BulletType::BulletParticleEmitter] = "#particles";
		bulletAttributesMap[FK_FileKey_BulletType::BulletParticleEmitterAdvanced] = "#particle_system";
		bulletAttributesMap[FK_FileKey_BulletType::BulletBackfaceCulling] = "#no_backface";
		bulletAttributesMap[FK_FileKey_BulletType::BulletCanBeJumped] = "#can_be_jumped_over";
		/* ... throw keys... */
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowAnimation] = "#animation";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowMovement] = "#target_movement";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowMovementEnd] = "#target_movement_end";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowReactionPerformer] = "#end_reaction_performer";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowReactionTarget] = "#end_reaction_target";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowReactionStance] = "#end_stance_target";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowStartingDistance] = "#starting_distance";
		throwAttributesMap[FK_FileKey_ThrowKeys::ActivateThrowDamageScaling] = "#has_damage_scaling";
		throwAttributesMap[FK_FileKey_ThrowKeys::ThrowCameraSetup] = "#camera_settings";
		/* ... transformations ... */
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationMesh] = "#mesh";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationNameTag] = "#name_id";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationFrame] = "#frame";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationIdleStance] = "#idle";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWalkingAnimation] = "#walk";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBackWalkAnimation] = "#walk_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationSidestepAnimation] = "#sidestep";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationCrouchingIdleStance] = "#crouching";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationRunningAnimation] = "#run";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationGuardAnimation] = "#guard";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationCrouchingGuardAnimation] = "#crouching_guard";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationJumpAnimation] = "#jump";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBackwardJumpAnimation] = "#jump_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationForwardJumpAnimation] = "#jump_forward";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitHighAnimation] = "#hit_weak_high";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitHighAnimationBack] = "#hit_weak_high_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitMidAnimation] = "#hit_weak_mid";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitMidAnimationBack] = "#hit_weak_mid_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitLowAnimation] = "#hit_weak_low";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitLowAnimationBack] = "#hit_weak_low_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitHighAnimation] = "#hit_strong_high";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitHighAnimationBack] = "#hit_strong_high_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitMidAnimation] = "#hit_strong_mid";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitMidAnimationBack] = "#hit_strong_mid_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitLowAnimation] = "#hit_strong_low";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitLowAnimationBack] = "#hit_strong_low_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitFlightAnimation] = "#hit_strong_flight";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitFlightAnimationBack] = "#hit_strong_flight_back";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationSupineAnimation] = "#supine";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationSupineRollAnimation] = "#supine_roll";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationLandingAnimation] = "#landing";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWakeUpAnimation] = "#wakeup";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWinAnimation] = "#win";
		// transformation attributes
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBackToStandardForm] = "#back_to_normal_form";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationDurationMS] = "#duration_ms";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationPermanent] = "#permanent_transformation";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBlueprintTag] = "#copy_from_transformation";
		transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationPreviousForm] = "#previous_form";
		/*... cinematics ... */
		cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicCameraSettings] = "#camera_settings";
		cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicCameraSettingsEnd] = "#camera_settings_end";
		cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicStopTime] = "#opponent_time_multiplier";
		cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicFrames] = "#frames";
		/* ... and reaction type */
		reactionTypeMap["None"] = FK_Reaction_Type::NoReaction;
		reactionTypeMap["StrongHigh"] = FK_Reaction_Type::StrongHigh;
		reactionTypeMap["StrongMedium"] = FK_Reaction_Type::StrongMedium;
		reactionTypeMap["StrongMid"] = FK_Reaction_Type::StrongMedium;
		reactionTypeMap["StrongLow"] = FK_Reaction_Type::StrongLow;
		reactionTypeMap["WeakHigh"] = FK_Reaction_Type::WeakHigh;
		reactionTypeMap["WeakMedium"] = FK_Reaction_Type::WeakMedium;
		reactionTypeMap["WeakMid"] = FK_Reaction_Type::WeakMedium;
		reactionTypeMap["WeakLow"] = FK_Reaction_Type::WeakLow;
		reactionTypeMap["Flight"] = FK_Reaction_Type::StrongFlight;
		reactionTypeMap["LongFlight"] = FK_Reaction_Type::StrongFlight;
		reactionTypeMap["ReverseFlight"] = FK_Reaction_Type::ReverseStrongFlight;
		reactionTypeMap["ShortFlight"] = FK_Reaction_Type::WeakFlight;
		reactionTypeMap["WeakFlight"] = FK_Reaction_Type::WeakFlight;
		reactionTypeMap["StandFlight"] = FK_Reaction_Type::StandingFlight;
		reactionTypeMap["Smackdown"] = FK_Reaction_Type::SmackdownLanding;
		/* store the interpolation identifier as ">" (default)*/
		interpolationIdentifier = ">";
		/* create input map */
		inputMap["0"] = FK_Input_Buttons::None; // for throw escape only
		inputMap["*"] = FK_Input_Buttons::Neutral_Input;
		inputMap["2"] = FK_Input_Buttons::Down_Direction;
		inputMap["3"] = FK_Input_Buttons::DownRight_Direction;
		inputMap["1"] = FK_Input_Buttons::DownLeft_Direction;
		inputMap["4"] = FK_Input_Buttons::Left_Direction;
		inputMap["7"] = FK_Input_Buttons::UpLeft_Direction;
		inputMap["8"] = FK_Input_Buttons::Up_Direction;
		inputMap["9"] = FK_Input_Buttons::UpRight_Direction;
		inputMap["6"] = FK_Input_Buttons::Right_Direction;
		inputMap["H2"] = FK_Input_Buttons::HeldDown_Direction;
		inputMap["H3"] = FK_Input_Buttons::HeldDownRight_Direction;
		inputMap["H1"] = FK_Input_Buttons::HeldDownLeft_Direction;
		inputMap["H4"] = FK_Input_Buttons::HeldLeft_Direction;
		inputMap["H7"] = FK_Input_Buttons::HeldUpLeft_Direction;
		inputMap["H8"] = FK_Input_Buttons::HeldUp_Direction;
		inputMap["H9"] = FK_Input_Buttons::HeldUpRight_Direction;
		inputMap["H6"] = FK_Input_Buttons::HeldRight_Direction;
		inputMap["G"] = FK_Input_Buttons::Guard_Button;
		inputMap["T"] = FK_Input_Buttons::Tech_Button;
		inputMap["K"] = FK_Input_Buttons::Kick_Button;
		inputMap["P"] = FK_Input_Buttons::Punch_Button;
		inputMap["Tg"] = FK_Input_Buttons::Trigger_Button;
		// deptecated - only for retro compatibility/testing - to be deleted soon!!!
		inputMap["A"] = FK_Input_Buttons::Guard_Button; //deprecated
		inputMap["B"] = FK_Input_Buttons::Tech_Button; //deprecated
		inputMap["X"] = FK_Input_Buttons::Kick_Button; //deprecated
		inputMap["Y"] = FK_Input_Buttons::Punch_Button; //deprecated
		inputMap["R"] = FK_Input_Buttons::Trigger_Button; //deprecated
		inputMap["v"] = FK_Input_Buttons::Down_Direction;
		inputMap["v>"] = FK_Input_Buttons::DownRight_Direction;
		inputMap["<v"] = FK_Input_Buttons::DownLeft_Direction;
		inputMap["<"] = FK_Input_Buttons::Left_Direction;
		inputMap["<^"] = FK_Input_Buttons::UpLeft_Direction;
		inputMap["^"] = FK_Input_Buttons::Up_Direction;
		inputMap["^>"] = FK_Input_Buttons::UpRight_Direction;
		inputMap[">"] = FK_Input_Buttons::Right_Direction;
		inputMap["Hv"] = FK_Input_Buttons::HeldDown_Direction;
		inputMap["Hv>"] = FK_Input_Buttons::HeldDownRight_Direction;
		inputMap["H<v"] = FK_Input_Buttons::HeldDownLeft_Direction;
		inputMap["H<"] = FK_Input_Buttons::HeldLeft_Direction;
		inputMap["H<^"] = FK_Input_Buttons::HeldUpLeft_Direction;
		inputMap["H^"] = FK_Input_Buttons::HeldUp_Direction;
		inputMap["H^>"] = FK_Input_Buttons::HeldUpRight_Direction;
		inputMap["H>"] = FK_Input_Buttons::HeldRight_Direction;
		/* define the input combination identifier as "+" */
		combinationIdentifier = "+";
		/* then, parse stances... */
		stanceTypeMap["Special1"] = FK_Stance_Type::SpecialStance1;
		stanceTypeMap["Special2"] = FK_Stance_Type::SpecialStance2;
		stanceTypeMap["Special3"] = FK_Stance_Type::SpecialStance3;
		stanceTypeMap["Special4"] = FK_Stance_Type::SpecialStance4;
		stanceTypeMap["Special5"] = FK_Stance_Type::SpecialStance5;
		stanceTypeMap["Ground"] = FK_Stance_Type::GroundStance;
		stanceTypeMap["Idle"] = FK_Stance_Type::GroundStance;
		stanceTypeMap["Crouch"] = FK_Stance_Type::CrouchedStance;
		stanceTypeMap["Crouched"] = FK_Stance_Type::CrouchedStance;
		stanceTypeMap["Air"] = FK_Stance_Type::AirStance;
		stanceTypeMap["Prone"] = FK_Stance_Type::ProneStance;
		stanceTypeMap["Supine"] = FK_Stance_Type::SupineStance;
		stanceTypeMap["Running"] = FK_Stance_Type::RunningStance;
		stanceTypeMap["Landing"] = FK_Stance_Type::LandingStance;
		stanceTypeMap["None"] = FK_Stance_Type::NoStance;
		/*... then, define the listed-in-the-move-list attribute identifier */
		listedInMoveListIdentifier = "!";
	};

	/* get hitbox parser for later use*/
	const std::map<std::string, FK_Hitbox_Type>& FK_MoveFileParser::getHitboxParsingMap(){
		return hitboxTypeMap;
	};
	/* get reaction parser for later use*/
	const std::map<std::string, FK_Reaction_Type>& FK_MoveFileParser::getReactionParsingMap(){
		return reactionTypeMap;
	};

	void FK_MoveFileParser::resetHitboxVariables(){
		hitboxType = FK_Hitbox_Type::NoHitbox;
		hitboxReaction = FK_Reaction_Type::NoReaction;
		hitboxDamage = 0;
		hitboxStartFrame = 0;
		hitboxEndFrame = 0;
		hitboxOneHit = true;
		hitboxPriority = 0;
		hitboxGuardBreak = false;
		hitboxAttackType = FK_Attack_Type::NoType;
	};

	void FK_MoveFileParser::resetMoveVariables(){
		moveName = std::string();
		movePriority = 0;
		moveStartingFrame = 0;
		moveEndingFrame = 0;
		moveFollowUp.clear();
		moveCancels.clear();
		hitboxes.clear();
	};

	void FK_MoveFileParser::resetInputVariables(){
		moveInputLeft.clear();
		moveInputRight.clear();
	};

	void FK_MoveFileParser::resetAllVariables(){
		resetMoveVariables();
		resetHitboxVariables();
		resetInputVariables();
	};

	std::deque<FK_Move> FK_MoveFileParser::getParsedMoves(){
		return parsedMoves;
	};

	std::deque<FK_Move> FK_MoveFileParser::parseMovesFromFile(std::ifstream &moveInputFile){
		/* start cycling through the file*/
		int moveCounter = 0;
		while (!moveInputFile.eof()){
			/* the initial value is taken via getline and then istringstreamed.
			This will solve some nasty problems related to whitespace (don't ask, it's better, okay?)*/
			std::string stringToParse = std::string();
			//std::getline(moveInputFile, stringToParse);
			//std::istringstream buffer(stringToParse);
			//buffer >> stringToParse;
			moveInputFile >> stringToParse;
			/* if the END tag is found, store last move and break cycle*/
			if (stringToParse == readerMap[FK_FileKey_Type::FileIdentifierEnd]){
				if (moveCounter > 0){
					move.finalizeAttackTypesFromHitboxes();
					move.setCancelGuardFlag();
					move.setMoveId((u32)(moveCounter));
					parsedMoves.push_back(move);
				}
				break;
			}
			/* if the MOVE tag is found, create a new move. If at least one move was already parsed, store it.*/
			std::string readmap = readerMap[FK_FileKey_Type::MoveIdentifier];
			//int parseResult = strcmp(stringToParse.c_str(), readerMap[FK_FileKey_Type::MoveIdentifier].c_str());
			if (stringToParse == readerMap[FK_FileKey_Type::MoveIdentifier]){
				if (moveCounter > 0){
					move.finalizeAttackTypesFromHitboxes();
					move.setCancelGuardFlag();
					move.setMoveId((u32)(moveCounter));
					parsedMoves.push_back(move);
					resetAllVariables();
					move.clear();
				}
				moveCounter += 1;
				/* store move name*/
				std::string temp;
				moveInputFile >> temp;
				if (temp == listedInMoveListIdentifier){
					move.setListed(true);
					moveInputFile >> moveName;
				}
				else{
					moveName = temp;
				}
				move.setName(moveName);
			}
			/* go on parsing only when the first instance of the move identifier is found */
			if (moveCounter > 0){
				/* if the ANIMATION tag is found, add animation information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::MoveAnimationIdentifier]){
					/* store move frames*/
					std::string temp = std::string();
					moveInputFile >> temp;
					move.setAnimationName(temp);
					if (move.getAnimationName(false).empty()){
						move.setAnimationName(temp, false);
					}
				}
				/* if the FRAMES tag is found, add frame information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::FramesIdentifier]){
					/* store move frames*/
					moveInputFile >> moveStartingFrame;
					moveInputFile >> moveEndingFrame;
					move.setStartingFrame(moveStartingFrame);
					move.setEndingFrame(moveEndingFrame);
					move.addSoundEffect(moveStartingFrame, "swing");
					move.resetMovementFrameArray();
				}
				/* if the DISPLAY_NAME tag is found, add a display name for the current move (used in move list) */
				if (stringToParse == readerMap[FK_FileKey_Type::DisplayNameIdentifier]){
					/* store move frames*/
					std::string temp = std::string();
					moveInputFile >> temp;
					move.setDisplayName(temp);
				}
				/* if the STANCE tag is found, add start stance information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::StanceIdentifier]){
					/* store move frames*/
					std::string temp = std::string();
					moveInputFile >> temp;
					move.setStance(stanceTypeMap[temp]);
					move.setEndStance(stanceTypeMap[temp]);
				}
				/* if the NEWSTANCE tag is found, add end stance information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::EndStanceIdentifier]){
					/* store move frames*/
					std::string temp = std::string();
					moveInputFile >> temp;
					move.setEndStance(stanceTypeMap[temp]);
				}
				/* if the PRIORITY tag is found, add end stance information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::PriorityIdentifier]) {
					/* store move frames*/
					s32 priority = 0;
					moveInputFile >> priority;
					move.setPriority(priority);
				}
				/* if the RANGE tag is found, add range information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::RangeIdentifier]){
					/* store range frames*/
					//TO BE FIXED
					f32 tempMinRange = 0.0, tempMaxRange;
					moveInputFile >> tempMinRange >> tempMaxRange;
					move.setRange(tempMinRange, tempMaxRange);
				}
				/* if the DELAY_AFTER_MOVE_MS tag is found, add delay information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::DelayAfterMoveMSIdentifier]){
					/* store ms count*/
					s32 delay = -1;
					moveInputFile >> delay;
					move.setDelayAfterMoveTime(delay);
				}
				/* if the REQUIRES_BULLET_COUNTERS tag is found, add bullet usage information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::BulletCountersRequiredIdentifier]){
					/* store bullet number */
					s32 requiredBullets = 0;
					moveInputFile >> requiredBullets;
					move.setNumberOfRequiredBulletCounters(requiredBullets);
				}
				/* if the REQUIRES_PRECISE_INPUT tag is found, set move input as strict */
				if (stringToParse == readerMap[FK_FileKey_Type::TightInputIdentifier]) {
					move.setPreciseInputRequirementFlag(true);
				}
				/* if the AI_ONLY_DURING_OPPONENT_ATTACK tag is found, set AI flag for this situations */
				if (stringToParse == readerMap[FK_FileKey_Type::OnlyDuringEnemyAttackIdentifier]) {
					move.setAIonlyDuringOpponentAttackFlag(true);
				}
				/* if the ACTIVATE_TRIGGER_MODE tag is found, add trigger mode activation among move properties*/
				if (stringToParse == readerMap[FK_FileKey_Type::ActivateTriggerModeIdentifier]) {
					/* store range frames*/
					move.setTriggerModeActivationFlag(true);
				}
				/* if the NO_CANCEL_ON_WHIFF tag is found, the move won't have any active followup/cancels until it hits the target*/
				if (stringToParse == readerMap[FK_FileKey_Type::CancelOrFollowupOnHitOnlyIdentifier]) {
					/* store range frames*/
					move.setFollowupOrCancelOnHitOnly();
				}
				/* if the FOLLOWUP_ONLY tag is found, set the move as only available through followup*/
				if (stringToParse == readerMap[FK_FileKey_Type::FollowupOnlyIdentifier]){
					/* set move as followup only*/
					move.setFollowUpOnly(true);
				}
				/* if the TAUNT tag is found, set the move as only available as a taunt*/
				if (stringToParse == readerMap[FK_FileKey_Type::TauntMoveIdentifier]){
					/* set move as taunt*/
					move.setAsTaunt();
				}
				/* if the ADDITIONAL_VELOCITY_AFTER_AIR_MOVE tag is found, set additional fall velocity*/
				if (stringToParse == readerMap[FK_FileKey_Type::AdditionalFallVelocityIdentifier]) {
					/* set additional fall velocity */
					f32 velocity;
					moveInputFile >> velocity;
					move.setAdditionalFallVelocity(velocity);
				}
				/* if the TOGGLE_ITEM_AT_FRAME tag is found, add an item to the list of toggled items (visiblity-switch)*/
				if (stringToParse == readerMap[FK_FileKey_Type::ToggleItemIdentifier]){
					/* get item id and frame, then set toggle flag */
					std::string itemId, key;
					u32 frame;
					moveInputFile >> itemId >> frame >> key;
					FK_Move::FK_ObjectToggleFlag flag;
					if (key == "show"){
						flag = FK_Move::FK_ObjectToggleFlag::Show;
					}
					else if (key == "hide"){
						flag = FK_Move::FK_ObjectToggleFlag::Hide;
					}
					else if (key == "toggle"){
						flag = FK_Move::FK_ObjectToggleFlag::Toggle;
					}
					else if (key == "restore"){
						flag = FK_Move::FK_ObjectToggleFlag::Restore;
					}
					else if (key == "break"){
						flag = FK_Move::FK_ObjectToggleFlag::Break;
					}
					else if (key == "use") {
						flag = FK_Move::FK_ObjectToggleFlag::Use;
					}
					move.addToggledItemAtFrame(frame, itemId, flag);
				}
				/* if the PICKUP_ITEM_AT_FRAME tag is found, set an item as pickable at the specified frame*/
				if (stringToParse == readerMap[FK_FileKey_Type::PickupItemIdentifier]){
					/* get item id and frame, then set toggle flag */
					std::string itemId;
					u32 frame;
					moveInputFile >> itemId >> frame;
					move.setItemPickupAtFrame(frame, itemId);
				}
				/* if the PICKUP_ONLY tag is found, set the move as only available when item can be picked up */
				if (stringToParse == readerMap[FK_FileKey_Type::PickupOnlyMoveIdentifier]) {
					move.setAsPickupOnly();
				}
				/* if the NO_RIGHTSIDE_MIRROR tag is found, set the move as non-mirrorable while char is on right side*/
				if (stringToParse == readerMap[FK_FileKey_Type::NoRightSideReversal]){
					/* set move as unmirrorable */
					move.applyUnmirrorableFlag();
				}
				/* if the FORCE_NAME_DISPLAY tag is found, set the move to show name even if it's a followup */
				if (stringToParse == readerMap[FK_FileKey_Type::ForceNameDisplayIdentifier]) {
					/* set the forced display flag*/
					move.setForcedNameDisplay();
				}
				/* if the MINIMUM_DAMAGE_RATIO_AFTER_SCALING tag is found, set the move's maximum startup angle */
				if (stringToParse == readerMap[FK_FileKey_Type::MaximumDamageScalingIdentifier]) {
					f32 scaling;
					moveInputFile >> scaling;
					move.setMaximumDamageScaling(scaling);
				}
				/* if the MAXIMUM_TRACKING_ANGLE_STARTUP tag is found, set the move's maximum startup angle */
				if (stringToParse == readerMap[FK_FileKey_Type::MaximumTrackingAngleStartupIdentifier]) {
					f32 angle;
					moveInputFile >> angle;
					move.setMaximumTrackingAngleOnStartup(angle);
				}
				/* if the MAXIMUM_STEERING_ANGLE tag is found, set the move's maximum steering angle */
				if (stringToParse == readerMap[FK_FileKey_Type::MaximumTrackingAngleMovementIdentifier]) {
					f32 angle;
					moveInputFile >> angle;
					move.setMaximumTrackingAngleDuringMovement(angle);
				}
				/* if the TRACK_AFTER_ANIMATION tag is found, set the move to rotate the character after the move ends*/
				if (stringToParse == readerMap[FK_FileKey_Type::RotateTowardOpponentAfterAnimation]){
					/* set move after animation tracking*/
					move.setAfterAnimationTracking();
				}
				/* if the TRACK_BEFORE_ANIMATION tag is found, set the move to rotate the character after the move ends*/
				if (stringToParse == readerMap[FK_FileKey_Type::RotateTowardOpponentBeforeAnimation]){
					/* set startup tracking*/
					move.setStartupTracking();
				}
				/* if the NO_BUFFER_CLEAR tag is found, set the move as non-buffer-consuming (it's usually automatic for cancels)*/
				if (stringToParse == readerMap[FK_FileKey_Type::NonBufferConsuming]){
					/* set move as non-buffer-clearing after execution */
					move.setClearInputBufferOnExecutionFlag(false);
				}
				/* if the VS_GROUNDED tag is found, set the move as available to hit grounded targets*/
				if (stringToParse == readerMap[FK_FileKey_Type::WorksVSGroundedOpponent]){
					/* set move as effective vs grounded opponents*/
					move.setVsGroundedOpponentFlag(true);
				}
				/* if the MULTICHAINABLE tag is found, set move as uesable many times in same chain */
				if (stringToParse == readerMap[FK_FileKey_Type::MultichainableMoveIdentifier]) {
					move.setAsMultichainable();
				}
				/* if the ANTI_AIR_ONLY tag is found, set the move as available to hit only air target*/
				if (stringToParse == readerMap[FK_FileKey_Type::WorksVSAirOpponent]) {
					/* set move as effective vs grounded opponents*/
					move.setAntiAirOnlyFlag(true);
				}
				/* if the NO_SOUNDS tag is found, disable the "swing" SFX */
				if (stringToParse == readerMap[FK_FileKey_Type::NoSFXIndentifier]){
					move.setSoundEffectFlag(false);
				}
				/* if the NO_DIRECTION_LOCK tag is found, the move has no direction lock*/
				if (stringToParse == readerMap[FK_FileKey_Type::NoDirectionLock]){
					move.setDirectionLockFlag(false);
				}
				/* if the NO_DELAY tag is found, the move has no delay after execution*/
				if (stringToParse == readerMap[FK_FileKey_Type::NoDelayAfterMove]){
					move.setDelayAfterMoveFlag(false);
				}
				/* if the RIGHTSIDE_INVERSION tag is found, the move has no delay after execution*/
				if (stringToParse == readerMap[FK_FileKey_Type::RightSideAnimation]){
					std::string temp = std::string();
					moveInputFile >> temp;
					move.setAnimationName(temp, false);
				}
				/* if the INVINCIBILITY_FRAMES tag is found, store invincibility frames from starting to end*/
				if (stringToParse == readerMap[FK_FileKey_Type::InvincibilityFrames]){
					u32 firstFrame, lastFrame;
					moveInputFile >> firstFrame;
					moveInputFile >> lastFrame;
					for (int i = firstFrame; i <= lastFrame; ++i){
						move.setInvincibilityFrame(i, FK_Attack_Type::AllAtk);
					}
				}
				/* if the INVINCIBILITY_FRAMES_AGAINST tag is found, cycle until the ending tag is found*/
				if (stringToParse == readerMap[FK_FileKey_Type::AdvInvincibility]){
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the INVINCIBILITY_FRAMES_AGAINST_END tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::AdvInvincibilityEnd]){
							break;
						}
						else{
							u32 firstFrame, lastFrame;
							moveInputFile >> firstFrame;
							moveInputFile >> lastFrame;
							for (int i = firstFrame; i <= lastFrame; ++i){
								move.setInvincibilityFrame(i, attackTypeMapInvincibility[temp]);
							}
						}
					}
				}
				/* if the ARMOR_FRAMES_AGAINST tag is found, cycle until the ending tag is found*/
				if (stringToParse == readerMap[FK_FileKey_Type::ArmorFrames]) {
					while (!moveInputFile.eof()) {
						std::string temp;
						moveInputFile >> temp;
						/* if the ARMOR_FRAMES_AGAINST_END tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::ArmorFramesEnd]) {
							break;
						}
						else {
							u32 firstFrame, lastFrame;
							moveInputFile >> firstFrame;
							moveInputFile >> lastFrame;
							for (int i = firstFrame; i <= lastFrame; ++i) {
								move.setArmorFrame(i, attackTypeMapInvincibility[temp]);
							}
						}
					}
				}
				/* if the ONLY_WITH_OBJECT tag is found, add required object information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::OnlyWithSpecificObject]){
					/* store move frames*/
					std::string temp = std::string();
					moveInputFile >> temp;
					move.addRequiredObjectId(temp);
				}
				/* if the ONLY_WITHOUT_OBJECT tag is found, add required object information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::OnlyWithoutSpecificObject]){
					/* store object id*/
					std::string temp = std::string();
					moveInputFile >> temp;
					move.addForbiddenObjectId(temp);
				}
				/* if the REDUCED_GRAVITY tag is found, reduce gravity on move (i.e. don't use jump gravity)*/
				if (stringToParse == readerMap[FK_FileKey_Type::LowerGravityIdentifier]){
					move.setLowerGravityFlag();
				}
				/* if the TRACK_ON_HIT tag is found, add tracking when moves hit for the first time*/
				if (stringToParse == readerMap[FK_FileKey_Type::TrackAfterHitIdentifier]){
					move.setAfterHitTracking();
				}
				/* if the CINEMATIC tag is found, add end stance information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::StopTimeCinematicIdentifier]) {
					/* parse throw */
					move.getCinematic() = FK_Move::FK_MoveCinematic();
					while (!moveInputFile.eof()) {
						std::string temp;
						moveInputFile >> temp;
						/* if the CINEMATIC_END tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::StopTimeCinematicIdentifierEnd]) {
							break;
						}
						else if (temp == cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicFrames]) {
							s32 frameStart, frameEnd;
							moveInputFile >> frameStart >> frameEnd;
							move.getCinematic().cinematicFrames = std::pair<s32,s32>(frameStart, frameEnd);
							FK_Move::FK_ThrowCameraSettings tempSettings;
							for (s32 i = 0; i < frameEnd-frameStart + 1; ++i) {
								move.getCinematic().cameraSettings.push_back(tempSettings);
							}
						}
						else if (temp == cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicStopTime]) {
							f32 timeMultiplier = 1.0f;
							moveInputFile >> timeMultiplier;
							move.getCinematic().opponentTimerMultiplier = timeMultiplier;
						}
						else if (temp == cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicCameraSettings]) {
							if (move.getCinematic().cinematicFrames == std::pair<s32, s32>(0, 0) && 
								move.getStartingFrame() == move.getEndingFrame()) {
								continue;
							}
							std::string line;
							s32 lastFrame = 0;
							while (!moveInputFile.eof()) {
								std::getline(moveInputFile, line);
								if (line.empty()) continue;
								/* if the MOVEMENT end tag is found, break cycle*/
								if (line == cinematicAttributesMap[FK_FileKey_CinematicKeys::CinematicCameraSettingsEnd]) {
									break;
								}
								std::istringstream lineReader(line);
								bool interpolate = false;
								s32 frame = 0;
								std::string target = "target";
								f32 distance = 0.0f;
								f32 angleY = 0.0f;
								f32 offsetY = 0.0f;
								f32 targetOffsetY = 0.0f;
								lineReader >> temp;
								if (temp == interpolationIdentifier) {
									interpolate = true;
									lineReader >> frame;
								}
								else {
									frame = atoi(temp.c_str());
								}
								lineReader >> target >> distance >> angleY >> offsetY >> targetOffsetY;
								FK_Move::FK_ThrowCameraSettings tempSettings;
								tempSettings.offsetPositionYFromMiddlePoint = offsetY;
								tempSettings.planarDistanceFromMiddlePoint = distance;
								tempSettings.rotationAngleYFromCameraPlane = angleY;
								tempSettings.targetYOffset = targetOffsetY;
								tempSettings.targetCharacter = target == "self" ? 1 : target == "target" ? -1 : 0;
								tempSettings.directCut = true;
								s32 startFrame = move.getCinematic().cinematicFrames.first;
								s32 endFrame = move.getCinematic().cinematicFrames.second - startFrame;
								if (interpolate && (frame - lastFrame) > 1) {
									FK_Move::FK_ThrowCameraSettings newSettings;
									FK_Move::FK_ThrowCameraSettings oldSettings = 
										move.getCinematic().cameraSettings[frame - startFrame];
									s32 delta = frame - lastFrame;
									f32 deltaAngle = (angleY - oldSettings.rotationAngleYFromCameraPlane) / delta;
									f32 deltaTargetOffsetY = (targetOffsetY - oldSettings.targetYOffset) / delta;
									f32 deltaDistance = (distance - oldSettings.planarDistanceFromMiddlePoint) / delta;
									f32 deltaOffsetY = (offsetY - oldSettings.offsetPositionYFromMiddlePoint) / delta;
									for (s32 i = 1; i < frame - lastFrame; ++i) {
										newSettings.directCut = false;
										newSettings.offsetPositionYFromMiddlePoint =
											oldSettings.offsetPositionYFromMiddlePoint + i*deltaOffsetY;
										newSettings.targetYOffset =
											oldSettings.targetYOffset + i*deltaTargetOffsetY;
										newSettings.planarDistanceFromMiddlePoint =
											oldSettings.planarDistanceFromMiddlePoint + i*deltaDistance;
										newSettings.rotationAngleYFromCameraPlane =
											oldSettings.rotationAngleYFromCameraPlane + i*deltaAngle;
										newSettings.targetCharacter = tempSettings.targetCharacter;
										move.getCinematic().cameraSettings[lastFrame + i - startFrame] = newSettings;
									}
								}
								for (s32 i = frame - startFrame; i < endFrame + 1; ++i) {
									move.getCinematic().cameraSettings[i] = tempSettings;
								}
								lastFrame = frame;
							}
						}
					}
				}
				/* if the TRANSFORMATION tag is found, add end stance information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::TransformationIdentifier]){
					/* parse throw */
					move.getTransformation() = FK_Move::FK_TransformationMove();
					move.getTransformation().hasActiveTransformation = true;
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the TRANSFORMATION_END tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::TransformationIdentifierEnd]){
							if (move.getTransformation().transformationNameTag != std::string() &&
								availableTransformations.count(move.getTransformation().transformationNameTag) == 0) {
								availableTransformations[move.getTransformation().transformationNameTag] = move.getTransformation();
							}
							break;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationMesh]){
							moveInputFile >> temp;
							move.getTransformation().newMeshName = temp;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationNameTag]) {
							moveInputFile >> temp;
							move.getTransformation().transformationNameTag = temp;
							// restore transformation if already declared before
							if (availableTransformations.count(temp) > 0) {
								move.getTransformation() = availableTransformations[move.getTransformation().transformationNameTag];
							}
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBlueprintTag]) {
							moveInputFile >> temp;
							// restore transformation if already declared before
							if (availableTransformations.count(temp) > 0) {
								move.getTransformation() = availableTransformations[move.getTransformation().transformationNameTag];
							}
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationPreviousForm]) {
							moveInputFile >> temp;
							move.getTransformation().previousFormNameTag = temp;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationFrame]){
							s32 frame = 0;
							moveInputFile >> frame;
							move.getTransformation().transformationFrame = frame;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBackToStandardForm]) {
							move.getTransformation().isBasicForm = true;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationPermanent]) {
							move.getTransformation().resetEveryRound = false;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationDurationMS]) {
							s32 durationMs = 0;
							moveInputFile >> durationMs;
							move.getTransformation().durationMs = durationMs;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationIdleStance]){
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::IdleAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWalkingAnimation]){
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::WalkingAnimation] = newAnimation;
							move.getTransformation().newStances[FK_BasicPose_Type::BackWalkingAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBackWalkAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::BackWalkingAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationSidestepAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::SidestepAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationCrouchingIdleStance]){
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::CrouchingIdleAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationRunningAnimation]){
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::RunningAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWinAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::WinAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationGuardAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::GuardingAnimation] = newAnimation;
						}						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationGuardAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::GuardingAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationCrouchingGuardAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::CrouchingGuardAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationJumpAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::JumpingAnimation] = newAnimation;
							move.getTransformation().newStances[FK_BasicPose_Type::ForwardJumpAnimation] = newAnimation;
							move.getTransformation().newStances[FK_BasicPose_Type::BackwardJumpAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationForwardJumpAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::ForwardJumpAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationBackwardJumpAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::BackwardJumpAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationSupineAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::GroundedSupineAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationSupineRollAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, true);
							move.getTransformation().newStances[FK_BasicPose_Type::GroundedSupineRotationAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWakeUpAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::SupineWakeUpAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationLandingAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::SupineLandingAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitHighAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitWeakHighAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitHighAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitWeakHighBackAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitLowAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitWeakLowAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitLowAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitWeakLowBackAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitMidAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitWeakMediumAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationWeakHitMidAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitWeakMediumBackAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitHighAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongHighAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitHighAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongHighBackAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitLowAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongLowAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitLowAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongLowBackAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitMidAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongMediumAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitMidAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongMediumBackAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitFlightAnimation]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongFlightAnimation] = newAnimation;
						}
						else if (temp == transformationAttributesMap[FK_FileKey_TransformationKeys::TransformationStrongHitFlightAnimationBack]) {
							s32 frameStart, frameEnd;
							moveInputFile >> temp >> frameStart >> frameEnd;
							FK_Pose newAnimation(temp, frameStart, frameEnd, false);
							move.getTransformation().newStances[FK_BasicPose_Type::HitStrongFlightBackAnimation] = newAnimation;
						}
					}
				}
				/* if the THROW tag is found, add end stance information to current move*/
				if (stringToParse == readerMap[FK_FileKey_Type::ThrowIdentifier]){
					/* parse throw */
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the THROW end tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::ThrowIdentifierEnd]){
							break;
						}
						/* if the ANIMATION tag is found, store throw */
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowAnimation]){
							int temp_startFrame, temp_endFrame;
							moveInputFile >> temp >> temp_startFrame >> temp_endFrame;
							move.setAsThrow(temp, temp_startFrame, temp_endFrame);
							move.resetThrowTargetMovementArray();
						}
						/* if the REACTION_PERFORMER tag is found, set performer reaction after throw */
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowReactionPerformer]){
							moveInputFile >> temp;
							move.setThrowReactionForPerformer(reactionTypeMap[temp]);
						}
						/* if the REACTION_TARGET tag is found, set target reaction after throw */
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowReactionTarget]){
							moveInputFile >> temp;
							move.setThrowReactionForTarget(reactionTypeMap[temp]);
						}
						/* if the STANCE_TARGET tag is found, set target stance */
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowReactionStance]){
							moveInputFile >> temp;
							move.setThrowStanceForTarget(stanceTypeMap[temp]);
						}
						/* if the CAMERA_SETTINGS tag is found, set target distance */
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowCameraSetup]){
							f32 distance = 0.0f;
							f32 angleY = 0.0f;
							f32 offsetY = 0.0f;
							moveInputFile >> distance >> angleY >> offsetY;
							move.setThrowCameraSettings(distance,angleY, offsetY);
						}
						/* if the DAMAGE_SCALING tag is found, set damage scaling for throw*/
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ActivateThrowDamageScaling]){
							move.setThrowAsDamageScalable();
						}
						/* if the CAMERA_DISTANCE tag is found, set target distance */
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowStartingDistance]){
							f32 distance = 0.0f;
							moveInputFile >> distance;
							move.setThrowStartingDistance(distance);
						}
						/* if the MOVEMENT tag is found, set target movement*/
						else if (temp == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowMovement]){
							int moveFrame = move.getThrowReactionAnimationPose().getStartingFrame();
							float lastParMovement = 0;
							float lastSideMovement = 0;
							float lastVertMovement = 0;
							int lastMoveFrame = moveFrame;
							float parMovement = 0;
							float sideMovement = 0;
							float vertMovement = 0;
							bool interpolate = false;
							std::string line;
							while (!moveInputFile.eof()){
								std::getline(moveInputFile, line);
								if (line.empty()) continue;
								/* if the MOVEMENT end tag is found, break cycle*/
								if (line == throwAttributesMap[FK_FileKey_ThrowKeys::ThrowMovementEnd]){
									break;
								}
								std::istringstream lineReader(line);
								interpolate = false;
								if (!lineReader.eof()){
									/* store previous values */
									lastMoveFrame = moveFrame;
									lastParMovement = parMovement;
									lastSideMovement = sideMovement;
									lastVertMovement = vertMovement;
									/* look for the interpolation iterator*/
									lineReader >> temp;
									if (temp == interpolationIdentifier){
										interpolate = true;
										lineReader >> moveFrame;
									}
									else{
										moveFrame = atoi(temp.c_str());
									}
									lineReader >> parMovement;
									lineReader >> sideMovement;
									lineReader >> vertMovement;
								}
								/* store movement */
								if (interpolate){
									int interpolationDuration = moveFrame - lastMoveFrame;
									float parallelIncrement = parMovement / interpolationDuration;
									float sideIncrement = sideMovement / interpolationDuration;
									float verticalIncrement = vertMovement / interpolationDuration;
									for (int k = 1; k <= interpolationDuration; k++){
										int tframe = lastMoveFrame + k;
										float tpar = parallelIncrement;
										float tside = sideIncrement;
										float tvert = verticalIncrement;
										move.setThrowTargetMovementAtFrame(tframe, tpar, tside, tvert);
									}
								}
								else{
									move.setThrowTargetMovementAtFrame(moveFrame, parMovement, sideMovement, vertMovement);
								}
							}
						}
					}
				}
				/* if the SOUNDS tag is found, cycle until the ending tag is found*/
				if (stringToParse == readerMap[FK_FileKey_Type::SoundFramesIdentifier]){
					move.clearSoundEffects();
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the SOUNDS_END tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::SoundFramesIdentifier_End]){
							break;
						}
						else{
							s32 frame = atoi(temp.c_str());
							moveInputFile >> temp;
							move.addSoundEffect(frame, temp);
						}
					}
				}
				/* if the FOLLOWUP tag is found, cycle until the ending tag is found*/
				if (stringToParse == readerMap[FK_FileKey_Type::FollowUpIdentifier]){
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the FOLLOWUP end tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::FollowUpIdentifierEnd]){
							break;
						}
						else{
							FK_FollowupMove temp_move;
							temp_move.setName(temp);
							int tempFrame;
							moveInputFile >> tempFrame;
							temp_move.setInputWindowStartingFrame(tempFrame);
							moveInputFile >> tempFrame;
							temp_move.setInputWindowEndingFrame(tempFrame);
							moveFollowUp.push_back(temp_move);
						}
					}
					/* store followup moves*/
					move.setFollowupMovesSet(moveFollowUp);
				}
				/* if the AUTO_FOLLOWUP tag is found, add auto followup move (move without input) */
				if (stringToParse == readerMap[FK_FileKey_Type::MoveAutoFollowup]) {
					std::string temp;
					moveInputFile >> temp;
					move.setAutoFollowupMove(temp);
				}
				/* if the CANCEL_INTO tag is found, cycle until the ending tag is found*/
				if (stringToParse == readerMap[FK_FileKey_Type::MoveCancelInto]){
					//move.setClearInputBufferOnExecutionFlag(false);
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the FOLLOWUP end tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::MoveCancelIntoEnd]){
							break;
						}
						else{
							FK_FollowupMove temp_move;
							temp_move.setName(temp);
							int tempFrame;
							moveInputFile >> tempFrame;
							temp_move.setInputWindowStartingFrame(tempFrame);
							moveInputFile >> tempFrame;
							temp_move.setInputWindowEndingFrame(tempFrame);
							moveCancels.push_back(temp_move);
						}
					}
					/* store cancel moves*/
					move.setCancelIntoMovesSet(moveCancels);
				}
				/* parse COUNTERATTACKS for the move */
				if (stringToParse == readerMap[FK_FileKey_Type::CounterattackIdentifier]){
					std::string line;
					while (!(moveInputFile.eof())){
						std::getline(moveInputFile, line);
						if (line.empty()) continue;
						/* if the COUNTERATTACK end tag is found, break cycle*/
						if (line == readerMap[FK_FileKey_Type::CounterattackEndIdentifier]){
							break;
						}
						else{
							std::istringstream lineReader(line);
							std::string temp;
							u32 startFrameCounter, endFrameCounter;
							FK_Attack_Type attackTypeCounter;
							std::string moveNameCounter;
							lineReader >> startFrameCounter >> endFrameCounter >> temp >> moveNameCounter;
							attackTypeCounter = attackTypeMap[temp];
							move.addCounterAttack(startFrameCounter, endFrameCounter, moveNameCounter, attackTypeCounter);
						}
					}
				}
				/* if the BULLET tag is found, cycle until the ending tag is found*/
				if (stringToParse == readerMap[FK_FileKey_Type::BulletIdentifier]){
					FK_Bullet tempBullet;
					u32 bulletHitboxIndex = 0;
					while (!moveInputFile.eof()){
						std::string temp;
						moveInputFile >> temp;
						/* if the FOLLOWUP end tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::BulletIdentifierEnd]){
							break;
						}
						/* otherwise, read key by key */
						// mesh
						if (strcmp(temp.c_str(),
							bulletAttributesMap[FK_FileKey_BulletType::BulletMeshName].c_str()) == 0){
							moveInputFile >> temp;
							tempBullet.setMeshPath(temp);
						}
						// range
						if (strcmp(temp.c_str(),
							bulletAttributesMap[FK_FileKey_BulletType::BulletRange].c_str()) == 0){
							f32 t_range;
							moveInputFile >> t_range;
							tempBullet.setRange(t_range);
						}
						// velocity
						if (strcmp(temp.c_str(),
							bulletAttributesMap[FK_FileKey_BulletType::BulletVelocity].c_str()) == 0){
							f32 vx, vy, vz;
							moveInputFile >> vx >> vy >> vz;
							tempBullet.setVelocity(core::vector3df(vx, vy, vz));
						}
						// scale
						if (strcmp(temp.c_str(),
							bulletAttributesMap[FK_FileKey_BulletType::BulletScale].c_str()) == 0){
							f32 sx, sy, sz;
							moveInputFile >> sx >> sy >> sz;
							tempBullet.setScale(core::vector3df(sx, sy, sz));
						}
						// rotation
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletRotation]){
							f32 rx, ry, rz;
							moveInputFile >> rx >> ry >> rz;
							tempBullet.setIntrinsicRotation(core::vector3df(rx, ry, rz));
						}
						// hitbox radius
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletHitboxRadius]){
							f32 rx, ry, rz;
							moveInputFile >> rx >> ry >> rz;
							tempBullet.setHitboxRadius(core::vector3df(rx, ry, rz));
						}
						// can be reflected by succesful guard?
						if (strcmp(temp.c_str(),
							bulletAttributesMap[FK_FileKey_BulletType::BulletReflection].c_str()) == 0){
							f32 t_multiplier;
							moveInputFile >> t_multiplier;
							tempBullet.setReflection(true, t_multiplier);
						}
						// mark as billboard
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletBillboardTexture]){
							tempBullet.markAsBillboard();
							moveInputFile >> temp;
							tempBullet.setMeshPath(temp);
						}
						// create particle emitter
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletParticleEmitter]){
							moveInputFile >> temp;
							tempBullet.setParticleEmitter(temp);
						}
						// create advanced particle emitter
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletParticleEmitterAdvanced]){
							s32 minPart, maxPart, durationMS;
							moveInputFile >> temp >> minPart >> maxPart >> durationMS;
							tempBullet.setParticleEmitter(temp, minPart, maxPart, durationMS);
						}
						// apply shadow
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletShadow]){
							tempBullet.setShadowToBullet();
						}
						// backfaceCulling
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletBackfaceCulling]){
							tempBullet.setBackfaceCullingFlag(true);
						}
						// can be jumped
						if (temp == bulletAttributesMap[FK_FileKey_BulletType::BulletCanBeJumped]) {
							tempBullet.setJumpableFlag(true);
						}
						// hitbox
						if (temp ==	bulletAttributesMap[FK_FileKey_BulletType::BulletHitbox]){
							std::string line;
							while (!(moveInputFile.eof())){
								std::getline(moveInputFile, line);
								if (line.empty()) continue;
								/* if the HITBOX end tag is found, break cycle*/
								if (line ==	bulletAttributesMap[FK_FileKey_BulletType::BulletHitboxEnd]){
									break;
								}
								else{
									bulletHitboxIndex += 1;
									FK_Hitbox temp_hitbox = readHitboxFromLine(line);
									temp_hitbox.setInternalId(bulletHitboxIndex);
									tempBullet.setHitbox(temp_hitbox);
								}
							}
						}
					}
					/* store bullets */
					move.addBullet(tempBullet);
				}
				/* if the MOVEMENT tag is found AFTER the FRAMES tag, cycle until the ending tag is found*/
				if (move.getDuration() > 0){
					if (stringToParse == readerMap[FK_FileKey_Type::MovementIdentifier]){
						std::string temp;
						int moveFrame = moveStartingFrame;
						float lastParMovement = 0;
						float lastSideMovement = 0;
						float lastVertMovement = 0;
						int lastMoveFrame = moveStartingFrame;
						float parMovement = 0;
						float sideMovement = 0;
						float vertMovement = 0;
						bool interpolate = false;
						std::string line;
						while (!moveInputFile.eof()){
							std::getline(moveInputFile, line);
							if (line.empty()) continue;
							/* if the MOVEMENT end tag is found, break cycle*/
							if (line == readerMap[FK_FileKey_Type::MovementIdentifierEnd]){
								break;
							}
							std::istringstream lineReader(line);
							interpolate = false;
							if (!lineReader.eof()){
								/* store previous values */
								lastMoveFrame = moveFrame;
								lastParMovement = parMovement;
								lastSideMovement = sideMovement;
								lastVertMovement = vertMovement;
								/* look for the interpolation iterator*/
								lineReader >> temp;
								if (temp == interpolationIdentifier){
									interpolate = true;
									lineReader >> moveFrame;
								}
								else{
									moveFrame = atoi(temp.c_str());
								}
								lineReader >> parMovement;
								lineReader >> sideMovement;
								lineReader >> vertMovement;
							}
							/* store movement */
							if (interpolate){
								int interpolationDuration = moveFrame - lastMoveFrame;
								float parallelIncrement = parMovement / interpolationDuration;
								float sideIncrement = sideMovement / interpolationDuration;
								float verticalIncrement = vertMovement / interpolationDuration;
								for (int k = 1; k <= interpolationDuration; k++){
									int tframe = lastMoveFrame + k;
									float tpar = parallelIncrement;
									float tside = sideIncrement;
									float tvert = verticalIncrement;
									move.setMovementAtFrame(tframe, tpar, tside, tvert);
								}
							}
							else{
								move.setMovementAtFrame(moveFrame, parMovement, sideMovement, vertMovement);
							}
						}
					}
				}
				/* next, parse the HITBOXes for the move (if frames have been defined)*/
				if (move.getDuration() > 0){
					if (stringToParse == readerMap[FK_FileKey_Type::HitboxIdentifier]){
						std::string line;
						while (!(moveInputFile.eof())){
							std::getline(moveInputFile, line);
							if (line.empty()) continue;
							/* if the HITBOX end tag is found, break cycle*/
							if (line == readerMap[FK_FileKey_Type::HitboxIdentifierEnd]){
								move.setHitboxCollection(hitboxes);
								break;
							}
							else{
								FK_Hitbox temp_hitbox = readHitboxFromLine(line);
								temp_hitbox.setInternalId(hitboxes.size() + 1);
								hitboxes.push_back(temp_hitbox);
							}
						}
					}
				}
				/* next, parse the  THROW ESCAPE INPUT for the move */
				if (stringToParse == readerMap[FK_FileKey_Type::ThrowEscapeInput]){
					u32 lastButton = 0;
					u32 button = 0;
					bool combination = false;
					while (!moveInputFile.eof()){
						lastButton = button;
						std::string temp;
						moveInputFile >> temp;
						/* if the INPUT end tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::ThrowEscapeInputEnd]){
							break;
						}
						else{
							if (temp == combinationIdentifier){
								combination = true;
							}
							else{
								button = inputMap[temp] & FK_Input_Buttons::Any_NonDirectionButton;
								if (combination){
									combination = false;
									button = button | lastButton;
								}
							}
						}
					}
					/* store input string*/
					move.setThrowEscapeInput(button);
				}
				/* next, parse the INPUT required for the move */
				if (stringToParse == readerMap[FK_FileKey_Type::InputIdentifier]){
					u32 lastButton = 0;
					u32 button = 0;
					bool combination = false;
					while (!moveInputFile.eof()){
						lastButton = button;
						std::string temp;
						moveInputFile >> temp;
						/* if the INPUT end tag is found, break cycle*/
						if (temp == readerMap[FK_FileKey_Type::InputIdentifierEnd]){
							break;
						}
						else{
							if (temp == combinationIdentifier){
								combination = true;
								moveInputLeft.pop_back();
								moveInputRight.pop_back();
							}
							else{
								button = inputMap[temp];
								if (combination){
									combination = false;
									button = button | lastButton;
								}
								u32 buttonRight = button;
								if (button & FK_Input_Buttons::Left_Direction){
									buttonRight = button - FK_Input_Buttons::Left_Direction + FK_Input_Buttons::Right_Direction;
								}
								else if (button & FK_Input_Buttons::Right_Direction){
									buttonRight = button - FK_Input_Buttons::Right_Direction + FK_Input_Buttons::Left_Direction;
								}
								else if (button & FK_Input_Buttons::HeldLeft_Direction){
									buttonRight = button - FK_Input_Buttons::HeldLeft_Direction + FK_Input_Buttons::HeldRight_Direction;
								}
								else if (button & FK_Input_Buttons::HeldRight_Direction){
									buttonRight = button - FK_Input_Buttons::HeldRight_Direction + FK_Input_Buttons::HeldLeft_Direction;
								}
								moveInputLeft.push_back(button);
								moveInputRight.push_back(buttonRight);
							}
						}
					}
					/* store input string*/
					move.setInputStringLeft(moveInputLeft);
					move.setInputStringRight(moveInputRight);
				}
			}
		}
		moveInputFile.close();
		moveInputFile.clear();
		// cleanup followups and add IDs for quciker matching
		setFollowupsIds(parsedMoves);
		return parsedMoves;
	};

	// set followups ID for fast tracking and comparison
	void FK_MoveFileParser::setFollowupsIds(std::deque<FK_Move>& parsedMoves) {
		u32 movesetSize = parsedMoves.size();
		for (u32 i = 0; i < movesetSize; ++i) {
			u32 followupSize = parsedMoves[i].getFollowupMovesSet().size();
			for (u32 j = 0; j < followupSize; ++j) {
				std::string nameToCheck = parsedMoves[i].getFollowupMovesSet()[j].getName();
				for (u32 k = 0; k < movesetSize; ++k) {
					if (nameToCheck == parsedMoves[k].getName()) {
						parsedMoves[i].getFollowupMovesSet()[j].setMoveId(
							parsedMoves[k].getMoveId());
					}
				}
			}
			followupSize = parsedMoves[i].getCancelIntoMovesSet().size();
			for (u32 j = 0; j < followupSize; ++j) {
				std::string nameToCheck = parsedMoves[i].getCancelIntoMovesSet()[j].getName();
				for (u32 k = 0; k < movesetSize; ++k) {
					if (nameToCheck == parsedMoves[k].getName()) {
						parsedMoves[i].getCancelIntoMovesSet()[j].setMoveId(
							parsedMoves[k].getMoveId());
					}
				}
			}
		}
		// add additional moves, if needed

		// FOLLOWUP
		for (u32 i = 0; i < movesetSize; ++i) {
			u32 followupSize = parsedMoves[i].getFollowupMovesSet().size();
			for (u32 j = 0; j < followupSize; ++j) {
				std::string nameToCheck = parsedMoves[i].getFollowupMovesSet()[j].getName();
				if (nameToCheck == FK_MoveFileParser::FollowupAllRootMovesTag) {
					u32 startFrame = parsedMoves[i].getFollowupMovesSet()[j].getInputWindowStartingFrame();
					u32 endFrame = parsedMoves[i].getFollowupMovesSet()[j].getInputWindowEndingFrame();
					for (u32 k = 0; k < movesetSize; ++k) {
						FK_FollowupMove newMove;
						if (!parsedMoves[k].isFollowupOnly() && 
							parsedMoves[k].getStance() == parsedMoves[i].getEndStance()) {
							newMove.setInputWindowStartingFrame(startFrame);
							newMove.setInputWindowEndingFrame(endFrame);
							newMove.setName(parsedMoves[k].getName());
							newMove.setMoveId(parsedMoves[k].getMoveId());
							parsedMoves[i].getFollowupMovesSet().push_back(newMove);
						}
					}
				} 
				else if (nameToCheck == FK_MoveFileParser::FollowupAllDamagingRootMovesTag) {
					u32 startFrame = parsedMoves[i].getFollowupMovesSet()[j].getInputWindowStartingFrame();
					u32 endFrame = parsedMoves[i].getFollowupMovesSet()[j].getInputWindowEndingFrame();
					for (u32 k = 0; k < movesetSize; ++k) {
						FK_FollowupMove newMove;
						if (!parsedMoves[k].isFollowupOnly() &&
							parsedMoves[k].getStance() == parsedMoves[i].getEndStance() &&
							(parsedMoves[k].canBeTriggered() ||
							parsedMoves[k].getTotalBulletDamage() > 0)) {
							newMove.setInputWindowStartingFrame(startFrame);
							newMove.setInputWindowEndingFrame(endFrame);
							newMove.setName(parsedMoves[k].getName());
							newMove.setMoveId(parsedMoves[k].getMoveId());
							parsedMoves[i].getFollowupMovesSet().push_back(newMove);
						}
					}
				}
			}
		}

		//CANCEL
		for (u32 i = 0; i < movesetSize; ++i) {
			u32 followupSize = parsedMoves[i].getCancelIntoMovesSet().size();
			for (u32 j = 0; j < followupSize; ++j) {
				std::string nameToCheck = parsedMoves[i].getCancelIntoMovesSet()[j].getName();
				if (nameToCheck == FK_MoveFileParser::FollowupAllRootMovesTag) {
					u32 startFrame = parsedMoves[i].getCancelIntoMovesSet()[j].getInputWindowStartingFrame();
					u32 endFrame = parsedMoves[i].getCancelIntoMovesSet()[j].getInputWindowEndingFrame();
					for (u32 k = 0; k < movesetSize; ++k) {
						FK_FollowupMove newMove;
						if (!parsedMoves[k].isFollowupOnly() &&
							parsedMoves[k].getStance() == parsedMoves[i].getEndStance()) {
							newMove.setInputWindowStartingFrame(startFrame);
							newMove.setInputWindowEndingFrame(endFrame);
							newMove.setName(parsedMoves[k].getName());
							newMove.setMoveId(parsedMoves[k].getMoveId());
							parsedMoves[i].getCancelIntoMovesSet().push_back(newMove);
						}
					}
				}
				else if (nameToCheck == FK_MoveFileParser::FollowupAllDamagingRootMovesTag) {
					u32 startFrame = parsedMoves[i].getCancelIntoMovesSet()[j].getInputWindowStartingFrame();
					u32 endFrame = parsedMoves[i].getCancelIntoMovesSet()[j].getInputWindowEndingFrame();
					for (u32 k = 0; k < movesetSize; ++k) {
						FK_FollowupMove newMove;
						if (!parsedMoves[k].isFollowupOnly() &&
							parsedMoves[k].getStance() == parsedMoves[i].getEndStance() &&
							(parsedMoves[k].canBeTriggered() ||
								parsedMoves[k].getTotalBulletDamage() > 0)) {
							newMove.setInputWindowStartingFrame(startFrame);
							newMove.setInputWindowEndingFrame(endFrame);
							newMove.setName(parsedMoves[k].getName());
							newMove.setMoveId(parsedMoves[k].getMoveId());
							parsedMoves[i].getCancelIntoMovesSet().push_back(newMove);
						}
					}
				}
			}
		}
	}

	/* read and receive hitbox*/
	FK_Hitbox FK_MoveFileParser::readHitboxFromLine(std::string line){
		resetHitboxVariables();
		std::istringstream lineReader(line);
		std::string temp;
		if (!lineReader.eof()){
			/* read hitbox type*/
			lineReader >> temp;
			/* if the guard break identifier is found, read one more step*/
			if (temp == hitboxGuardBreakIdentifier){
				hitboxGuardBreak = true;
				lineReader >> temp;
			}
			hitboxType = hitboxTypeMap[temp];
			/* read hitbox starting frame (if the "start" tag is found, the hitbox starts with
			the move*/
			lineReader >> temp;
			if (temp == hitboxStartFrameIdentifier){
				hitboxStartFrame = move.getStartingFrame();
			}
			else{
				hitboxStartFrame = atoi(temp.c_str());
			}
			/* read hitbox starting frame (if the "end" tag is found, the hitbox ends with
			the move*/
			lineReader >> temp;
			if (temp == hitboxEndFrameIdentifier){
				hitboxEndFrame = move.getEndingFrame();
			}
			else{
				hitboxEndFrame = atoi(temp.c_str());
			}
			/* store damage */
			lineReader >> hitboxDamage;
			/* check multi-hit properties*/
			lineReader >> temp;
			if (temp == hitboxSingleHitIdentifier){
				hitboxOneHit = true;
			}
			else if (temp == hitboxMultiHitIdentifier){
				hitboxOneHit = false;
			}
			/* store priority */
			lineReader >> hitboxPriority;
			/* store hitbox reaction*/
			lineReader >> temp;
			hitboxReaction = reactionTypeMap[temp];
			/* store hitbox type */
			lineReader >> temp;
			hitboxAttackType = attackTypeMap[temp];
			/* create hitbox*/
			FK_Hitbox temp_hitbox(hitboxStartFrame, hitboxEndFrame, hitboxDamage, hitboxPriority,
				hitboxGuardBreak, hitboxOneHit, hitboxType, hitboxReaction, hitboxAttackType);
			// return hitbox
			return temp_hitbox;
		}
		else{
			return FK_Hitbox();
		}
	}

}