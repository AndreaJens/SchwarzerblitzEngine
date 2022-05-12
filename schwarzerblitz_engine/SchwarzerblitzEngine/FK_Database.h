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

#ifndef FK_DATABASE
#define FK_DATABASE
/* This file contains all the definitions of the enums in the fk_engine namespace:
- FK_Input_Buttons: definition of the buttons for the actions
- FK_Hitbox_Type:   type of the hitbox to trigger
- FK_Reaction_Type: reaction to a hit
*/

#include<irrlicht.h>
#include<string>

using namespace irr;

namespace fk_engine{
	/* enum for each possible input in the game*/
	const enum  FK_Input_Buttons :u32{
		Neutral_Input = 1 << 18,
		None = 0,
		Down_Direction = 1 << 0,
		Left_Direction = 1 << 1,
		Up_Direction = 1 << 2,
		Right_Direction = 1 << 3,
		DownLeft_Direction = Down_Direction | Left_Direction,
		DownRight_Direction = Down_Direction | Right_Direction,
		UpLeft_Direction = Up_Direction | Left_Direction,
		UpRight_Direction = Up_Direction | Right_Direction,
		TaptoHoldButtonShift = 19,
		HeldDown_Direction = Down_Direction << TaptoHoldButtonShift,
		HeldLeft_Direction = Left_Direction << TaptoHoldButtonShift,
		HeldUp_Direction = Up_Direction << TaptoHoldButtonShift,
		HeldRight_Direction = Right_Direction << TaptoHoldButtonShift,
		HeldDownLeft_Direction = HeldDown_Direction | HeldLeft_Direction,
		HeldUpLeft_Direction = HeldUp_Direction | HeldLeft_Direction,
		HeldDownRight_Direction = HeldDown_Direction | HeldRight_Direction,
		HeldUpRight_Direction = HeldUp_Direction | HeldRight_Direction,
		Guard_Button = 1 << 4,
		Punch_Button = 1 << 5,
		Kick_Button = 1 << 6,
		Tech_Button = 1 << 7,
		Trigger_Button = 1 << 8,
		Selection_Button = 1 << 9,
		Cancel_Button = 1 << 10,
		Modifier_Button = 1 << 11,
		Menu_Pause_Button = 1 << 12,
		Help_Button = 1 << 13,
		Confirmation_Button = 1 << 14,
		ScrollLeft_Button = 1 << 15,
		ScrollRight_Button = 1 << 16,
		Taunt_Button = 1 << 17,
		Tech_plus_Punch_Button = Tech_Button | Punch_Button,
		Tech_plus_Kick_Button = Tech_Button | Kick_Button,
		Punch_plus_Kick_Button = Punch_Button | Kick_Button,
		Tech_plus_Guard_Button = Tech_Button | Guard_Button,
		Punch_plus_Guard_Button = Punch_Button | Guard_Button,
		Kick_plus_Guard_Button = Kick_Button | Guard_Button,
		Trigger_plus_Guard_Button = Guard_Button | Trigger_Button,
		Punch_plus_Kick_plus_Tech_Button = Punch_plus_Kick_Button | Tech_Button,
		All_Action_Buttons = Guard_Button | Punch_plus_Kick_plus_Tech_Button,
		Horizontal_Direction = Left_Direction | Right_Direction | HeldLeft_Direction | HeldRight_Direction,
		Vertical_Direction = Up_Direction | Down_Direction | HeldUp_Direction | HeldDown_Direction,
		Any_Direction = DownLeft_Direction | UpRight_Direction,
		Any_NonDirectionButton = Guard_Button | Tech_Button | Punch_Button | Kick_Button | Trigger_Button,
		Any_MenuButton = Selection_Button | Cancel_Button | Modifier_Button | Help_Button | Confirmation_Button |
						ScrollLeft_Button | ScrollRight_Button,
		Any_SystemButton = Menu_Pause_Button,
		Any_Button = Any_NonDirectionButton | Any_Direction | Any_SystemButton,
		Any_HeldButton = HeldDownRight_Direction | HeldUpLeft_Direction,
		Any_Direction_Plus_Held = Any_Direction | Any_HeldButton,
	};
	/* enum for joypad input */
	const enum  FK_JoypadInput : u32{
		JoypadInput_A_Button = 0,
		JoypadInput_B_Button = 1,
		JoypadInput_X_Button = 2,
		JoypadInput_Y_Button = 3,
		JoypadInput_L1_Button = 4,
		JoypadInput_R1_Button = 5,
		JoypadInput_Select_Button = 6,
		JoypadInput_Start_Button = 7,
		JoypadInput_L3_Button = 8,
		JoypadInput_R3_Button = 9,
		JoypadInput_L2_Button = 10,
		JoypadInput_R2_Button = 11,
		JoypadInput_Home_Button = 12,
		JoypadInputDown = 28,
		JoypadInputUp = 29,
		JoypadInputLeft = 30,
		JoypadInputRight = 31,
		JoypadInput_XboxNumberOfButtons = 13,
		JoypadInput_XboxNumberOfAssignableButtons = 12,
		JoypadInput_NoButton = 255
	};
	/* enum for joypad input (used to map joypad to Xbox-style buttons)*/
	const enum  FK_JoypadInputNative : u32{
		JoypadInput_Button1 = 0,
		JoypadInput_Button2 = 1,
		JoypadInput_Button3 = 2,
		JoypadInput_Button4 = 3,
		JoypadInput_Button5 = 4,
		JoypadInput_Button6 = 5,
		JoypadInput_Button7 = 6,
		JoypadInput_Button8 = 7,
		JoypadInput_Button9 = 8,
		JoypadInput_Button10 = 9,
		JoypadInput_Button11 = 10,
		JoypadInput_Button12 = 11,
		JoypadInput_Button13 = 12,
		JoypadInput_Button14 = 13,
		JoypadInput_Button15 = 14,
		JoypadInput_Button16 = 15,
		JoypadInput_Button17 = 16,
		JoypadInput_Button18 = 17,
		JoypadInput_Button19 = 18,
		JoypadInput_Button20 = 19,
		JoypadInput_Button21 = 20,
		JoypadInput_Button22 = 21,
		JoypadInput_Button23 = 22,
		JoypadInput_Button24 = 23,
		JoypadInput_Button25 = 24,
		JoypadInput_Button26 = 25,
		JoypadInput_Button27 = 26,
		JoypadInput_Button28 = 27,
		JoypadInput_Button29 = 28,
		JoypadInput_Button30 = 29,
		JoypadInput_Button31 = 30,
		JoypadInput_Button32 = 31,
		JoypadInput_Button33 = 32,
		JoypadInput_Button34 = 33,
		JoypadInput_Button35 = 34,
		JoypadInput_Button36 = 35,
		JoypadInput_Button37 = 36,
		JoypadInput_Button38 = 37,
		JoypadInput_Button39 = 38,
		JoypadInput_Button40 = 39,
		JoypadInput_NumberOfButtons = 40,
		JoypadInput_NoButton_Native = 255,
	};
	/* enum for each kind of hitbox in the game*/
	const enum  FK_Hitbox_Type : u32{
		RightPunch = 1,
		LeftPunch = 2,
		RightFoot = 3,
		LeftFoot = 4,
		RightKnee = 5,
		LeftKnee = 6,
		RightElbow = 7,
		LeftElbow = 8,
		Torso = 9,
		Head = 10,
		AdditionalHitbox1 = 11,
		AdditionalHitbox2 = 12,
		AdditionalHitbox3 = 13,
		AdditionalHitbox4 = 14,
		AdditionalHitbox5 = 15,
		AdditionalHitbox6 = 16,
		AdditionalHitbox7 = 17,
		AdditionalHitbox8 = 18,
		AdditionalHitbox9 = 19,
		AdditionalHitbox10 = 20,
		NoHitbox = 0,
		AllHitboxes = 21,
	};
	/* enum for each kind of hurtbox in the game*/
	const enum  FK_Hurtbox_Type{
		HeadHurtbox,
		TorsoHurtbox,
		SpineHurtbox,
		HipsHurtbox,
		LeftLegHurtbox,
		RightLegHurtbox,
		LeftWaistHurtbox,
		RightWaistHurtbox,
		NoHurtbox,
		FullBodyHurtbox,
		CrouchedHurtbox,
	};
	/* enum for each reaction type*/
	const enum  FK_Reaction_Type : u32{
		WeakHigh = 1,
		WeakMedium = 1 << 1,
		WeakLow = 1 << 2,
		WeakFlight = 1 << 3,
		StrongHigh = 1 << 4,
		StrongMedium = 1 << 5,
		StrongLow = 1 << 6,
		StrongFlight = 1 << 7,
		SmackdownLanding = 1 << 8,
		ReverseStrongFlight = 1 << 9,
		StandingFlight = 1 << 10,
		ReactionBitShift_WeakToStrong = 4,
		NoReaction = 0,
		AnyStrongReaction = StrongHigh | StrongMedium | StrongLow | SmackdownLanding,
		AnyWeakReaction = WeakHigh | WeakLow | WeakMedium,
		AnyHighReaction = StrongHigh | WeakHigh,
		AnyMediumReaction = StrongMedium | WeakMedium,
		AnyLowReaction = StrongLow | WeakLow,
		AnyImpactReaction = StrongFlight | ReverseStrongFlight | StandingFlight,
	};
	/* enum for each attack type*/
	const enum  FK_Attack_Type : u32{
		HighAtk = 1,
		MediumAtk = 1 << 1,
		LowAtk = 1 << 2,
		SpecialMediumAtk = 1 << 3,
		SpecialLowAtk = 1 << 4,
		HighThrowAtk = 1 << 5,
		MidThrowAtk = 1 << 6,
		LowThrowAtk = 1 << 7,
		HighBlockableThrow = 1 << 8,
		MidBlockableThrow = 1 << 9,
		LowBlockableThrow = 1 << 10,
		BulletAttack = 1 << 11,
		MidAtks = MediumAtk | MidThrowAtk | SpecialMediumAtk | MidBlockableThrow,
		HighAtks = HighAtk | HighThrowAtk | HighBlockableThrow,
		LowAtks = LowAtk | LowThrowAtk | SpecialLowAtk | LowBlockableThrow,
		HighAndMidAtks = HighAtks | MidAtks,
		HighAndLowAtks = HighAtks | MidAtks,
		MidAndLowAtks = MidAtks | LowAtks,
		BlockableThrowAtk = LowBlockableThrow | MidBlockableThrow | HighBlockableThrow,
		ThrowAtk = HighThrowAtk | LowThrowAtk | MidThrowAtk | BlockableThrowAtk,
		AllAtk = HighAtks | MidAtks | LowAtks,
		NoType = 0
	};
	/* enum for the move stances*/
	const enum  FK_Stance_Type : u32{
		NoStance = 0,
		GroundStance = 1,
		CrouchedStance = 1 << 1,
		SupineStance = 1 << 2,
		ProneStance = 1 << 3,
		LandingStance = 1 << 4,
		WakeUpStance = 1 << 5,
		AirStance = 1 << 6,
		RunningStance = 1 << 7,
		WinStance = 1 << 8,
		SpecialStance1 = 1 << 9,
		SpecialStance2 = SpecialStance1 << 1,
		SpecialStance3 = SpecialStance2 << 1,
		SpecialStance4 = SpecialStance3 << 1,
		SpecialStance5 = SpecialStance4 << 1,
		AnyStandardStance = GroundStance | CrouchedStance,
		AnyGroundedStance = SupineStance | ProneStance,
		AnySpecialStance = SpecialStance1 | SpecialStance2 | SpecialStance3 | SpecialStance4 | SpecialStance5,
	};
	/* enum for the move file parser keys*/
	const enum  FK_FileKey_Type{
		FileIdentifierEnd,
		MoveIdentifier,
		DisplayNameIdentifier,
		SoundFramesIdentifier,
		SoundFramesIdentifier_End,
		MoveAnimationIdentifier,
		FramesIdentifier,
		MovementIdentifier,
		MovementIdentifierEnd,
		FollowUpIdentifier,
		FollowUpIdentifierEnd,
		InputIdentifier,
		InputIdentifierEnd,
		HitboxIdentifier,
		HitboxIdentifierEnd,
		EndStanceIdentifier,
		StanceIdentifier,
		RangeIdentifier,
		FollowupOnlyIdentifier,
		ThrowIdentifier,
		ThrowIdentifierEnd,
		BulletIdentifier,
		BulletIdentifierEnd,
		NoSFXIndentifier,
		MoveCancelInto,
		MoveCancelIntoEnd,
		MoveAutoFollowup,
		WorksVSAirOpponent,
		WorksVSGroundedOpponent,
		NoDirectionLock,
		NoDelayAfterMove,
		RightSideAnimation,
		InvincibilityFrames,
		AdvInvincibility,
		AdvInvincibilityEnd,
		ArmorFrames,
		ArmorFramesEnd,
		ThrowEscapeInput,
		ThrowEscapeInputEnd,
		NonBufferConsuming,
		TauntMoveIdentifier,
		NoRightSideReversal,
		RotateTowardOpponentAfterAnimation,
		RotateTowardOpponentBeforeAnimation,
		OnlyWithSpecificObject,
		OnlyWithoutSpecificObject,
		CounterattackIdentifier,
		CounterattackEndIdentifier,
		NoThrowOnAerealIdentifier,
		ToggleItemIdentifier,
		PickupItemIdentifier,
		PickupOnlyMoveIdentifier,
		TrackAfterHitIdentifier,
		LowerGravityIdentifier,
		DelayAfterMoveMSIdentifier,
		BulletCountersRequiredIdentifier,
		ActivateTriggerModeIdentifier,
		TransformationIdentifier,
		TransformationIdentifierEnd,
		StopTimeCinematicIdentifier,
		StopTimeCinematicIdentifierEnd,
		MaximumTrackingAngleStartupIdentifier,
		MaximumTrackingAngleMovementIdentifier,
		ForceNameDisplayIdentifier,
		MaximumDamageScalingIdentifier,
		AdditionalFallVelocityIdentifier,
		PriorityIdentifier,
		CancelOrFollowupOnHitOnlyIdentifier,
		TightInputIdentifier,
		OnlyDuringEnemyAttackIdentifier,
		MultichainableMoveIdentifier,
	};

	/* enum for the move file parser keys (throw specific)*/
	const enum  FK_FileKey_ThrowKeys{
		ThrowAnimation,
		ThrowReactionTarget,
		ThrowReactionPerformer,
		ThrowReactionStance,
		ThrowMovement,
		ThrowMovementEnd,
		ThrowStartingDistance,
		ThrowCameraSetup,
		ActivateThrowDamageScaling,
		ThrowMaximumDamageScaling,
	};

	/* enum for the move file parser keys (transformation specific)*/
	const enum  FK_FileKey_TransformationKeys {
		TransformationMesh,
		TransformationNameTag,
		TransformationFrame,
		TransformationIdleStance,
		TransformationWalkingAnimation,
		TransformationSidestepAnimation,
		TransformationBackWalkAnimation,
		TransformationCrouchingIdleStance,
		TransformationRunningAnimation,
		TransformationJumpAnimation,
		TransformationForwardJumpAnimation,
		TransformationBackwardJumpAnimation,
		TransformationGuardAnimation,
		TransformationCrouchingGuardAnimation,
		TransformationWeakHitHighAnimation,
		TransformationWeakHitMidAnimation,
		TransformationWeakHitLowAnimation,
		TransformationStrongHitHighAnimation,
		TransformationStrongHitMidAnimation,
		TransformationStrongHitLowAnimation,
		TransformationWeakHitHighAnimationBack,
		TransformationWeakHitMidAnimationBack,
		TransformationWeakHitLowAnimationBack,
		TransformationStrongHitHighAnimationBack,
		TransformationStrongHitMidAnimationBack,
		TransformationStrongHitLowAnimationBack,
		TransformationStrongHitFlightAnimation,
		TransformationStrongHitFlightAnimationBack,
		TransformationSupineAnimation,
		TransformationSupineRollAnimation,
		TransformationLandingAnimation,
		TransformationStrongHitSmackdownAnimation,
		TransformationWakeUpAnimation,
		TransformationWinAnimation,
		TransformationBackToStandardForm,
		TransformationDurationMS,
		TransformationPermanent,
		TransformationPreviousForm,
		TransformationBlueprintTag,
	};

	/* enum for the move file parser keys (cinematic specific)*/
	const enum  FK_FileKey_CinematicKeys {
		CinematicFrames,
		CinematicStopTime,
		CinematicCameraSettings,
		CinematicCameraSettingsEnd,
	};


	/* enum for the move file parser keys (bullet specific)*/
	const enum  FK_FileKey_BulletType{
		BulletMeshName,
		BulletRange,
		BulletRotation,
		BulletAcceleration,
		BulletVelocity,
		BulletHitbox,
		BulletHitboxEnd,
		BulletHitboxRadius,
		BulletBillboardTexture,
		BulletReflection,
		BulletAngle,
		BulletScale,
		BulletShadow,
		BulletParticleEmitter,
		BulletBackfaceCulling,
		BulletParticleEmitterAdvanced,
		BulletCanBeJumped,
	};
	/* enumerate bones */
	const enum  FK_Bones_Type{
		NoArmature,
		HeadArmature,
		NeckArmature,
		TorsoArmature,
		SpineArmature,
		HipsArmature,
		RightArmArmature,
		LeftArmArmature,
		RightForearmArmature,
		LeftForearmArmature,
		RightWristArmature,
		LeftWristArmature,
		LeftFingersArmatureBase,
		LeftFingersArmatureMedium,
		LeftFingersArmatureTip,
		LeftThumbArmatureBase,
		LeftThumbArmatureTip,
		RightFingersArmatureBase,
		RightFingersArmatureMedium,
		RightFingersArmatureTip,
		RightThumbArmatureBase,
		RightThumbArmatureTip,
		LeftLegArmature,
		RightLegArmature,
		LeftKneeArmature,
		RightKneeArmature,
		LeftAnkleArmature,
		RightAnkleArmature,
		LeftFootArmature,
		RightFootArmature,
		AdditionalArmature01,
		AdditionalArmature02,
		AdditionalArmature03,
		AdditionalArmature04,
		AdditionalArmature05,
		AdditionalArmature06,
		AdditionalArmature07,
		AdditionalArmature08,
		AdditionalArmature09,
		AdditionalArmature10,
		RootArmature,
	};
	
	/* keys for stage creation file */
	enum FK_StageFileKeys{
		StageScale,
		StagePosition,
		StageName,
		StageArchiveName,
		StageDescription,
		StageDescriptionEnd,
		StageAmbientColor,
		StageLightColor,
		StageEmissionColor,
		StageBackgroundColor,
		StageHasSkybox,
		StageHasExternalSkybox,
		StageBGMName,
		StageBGMVolume,
		StageBGMPitch,
		StageRingoutDepth,
		StageMeshName,
		StageTextureEffect,
		StageShaderParametersBloom,
		StageShaderParametersBrightPass,
		StageWallboxCoordinates,
		StageCornerBoxCoordinates,
		StageComplexWallboxCoordinates,
		StageComplexWallboxCoordinates_End,
		StageComplexRingoutboxCoordinates,
		StageComplexRingoutboxCoordinates_End,
		StageComplexRingoutPlaneIndices,
		StageComplexRingoutPlaneIndices_End,
		StageRingoutboxCoordinates,
		StageRain,
		StageRaindropEffect,
		StageWaterMesh,
		StageAdditionalObject,
		StageAdditionalObject_ParticleSystemName,
		StageAdditionalObject_MeshName,
		StageAdditionalObject_Position,
		StageAdditionalObject_Rotation,
		StageAdditionalObject_Scale,
		StageAdditionalObject_BackfaceCulling,
		StageAdditionalObject_MaterialEffect,
		StageAdditionalObject_Animator,
		StageAdditionalObject_End,
		StageAdditionalObject_ExclusiveVisibilityCharacterCondition,
		StageAdditionalObject_ExclusiveInvisibilityCharacterCondition,
		StageAllowRingout,
		StageFogEffect,
		StageTechnicolorAmbientEffect,
		StageTechnicolorAmbientEffect_End,
		StageTechnicolorLightEffect,
		StageTechnicolorLightEffect_End,
		StageSplashRingout,
		StageSplashRingoutSoundEffect,
		StageSandSplashEffect,
		StageAdditionalShaders,
		StageAdditionalShadersEnd,
		StageDioramaLore,
		StageDioramaLoreEnd,
		StageBGMMetadata,
	};

	/* keys for basic stances which all characters have to have defined*/
	const enum FK_BasicPose_Type : u32{
		IdleAnimation,
		WalkingAnimation,
		BackWalkingAnimation,
		RunningAnimation,
		SidestepAnimation,
		CrouchingAnimation,
		GuardingAnimation,
		JumpingAnimation,
		ForwardJumpAnimation,
		BackwardJumpAnimation,
		CrouchingIdleAnimation,
		CrouchingGuardAnimation,
		HitWeakHighAnimation,
		HitWeakMediumAnimation,
		HitWeakLowAnimation,
		HitWeakHighBackAnimation,
		HitWeakMediumBackAnimation,
		HitWeakLowBackAnimation,
		HitStrongHighAnimation,
		HitStrongHighBackAnimation,
		HitStrongMediumAnimation,
		HitStrongMediumBackAnimation,
		HitStrongLowAnimation,
		HitStrongLowBackAnimation,
		HitStrongFlightAnimation,
		HitStrongFlightBackAnimation,
		GroundedSupineAnimation,
		GroundedProneAnimation,
		GroundedProneRotationAnimation,
		GroundedSupineRotationAnimation,
		SupineLandingAnimation,
		SupineWakeUpAnimation,
		WinAnimation,
		IntroAnimation,
		SpecialStance1Animation,
		SpecialStance2Animation,
		SpecialStance3Animation,
		SpecialStance4Animation,
		SpecialStance5Animation,
	};
	/* game options */
	const enum  FK_GameOptions{
		SoundVolumeOption,
		MuteSoundOption,
		MusicVolumeOption,
		MuteMusicOption,
		ScreenResolutionOption,
		FullscreenOption,
		DifficultyOption,
		NumberOfRoundsSinglePlayerOption,
		NumberOfRoundsMultiPlayerOption,
		GuardDamageOption,
		ShadowQualityOption,
		TurnShadowsOption,
		UseShadersOption,
	};
	/* wallbox plane direction */
	const enum FK_PlaneDirections : u32{
		PlaneNone = 0,
		PlaneNorth = 1,
		PlaneSouth = 1 << 1,
		PlaneEast = 1 << 2,
		PlaneWest = 1 << 3,
	};

	namespace fk_constants{
		/* save file name */
		const std::string FK_SaveFileName = "saveData.ini";
		/* rewards file name*/
		const std::string FK_RewardsFileName = "rewardsData.ini";
		/* cheat file name */
		const std::string FK_CheatsFileName = "cheats.ini";
		/* engine config file name*/
		const std::string FK_EngineConfigFileName = "engineConfig.ini";
		/* key for global statistics (internal use only)*/
		const std::string GlobalStatsIdentifier = "INTERNAL_GLOBAL_STATS_KEY";
		/* folder denomination for additional character mesh */
		const std::string AdditionalCharacterMeshSubfolderRoot = "Mesh";
		/* folder denomination for external character mesh */
		const std::string DLCCharacterMeshSubfolderRoot = "AdditionalMesh";
		/* folder denomination for extra (hidden) character mesh */
		const std::string ExtraCharacterMeshSubfolderRoot = "ExtraMesh";
		/* arcade configuration file */
		const std::string FK_ArcadeConfigFileName = "arcadeConfig.ini";
		/* personal arcade config file */
		const std::string FK_CharacterArcadeFlowFileName = "arcadeFlow.txt";
		/* steam workshop file */
		const std::string FK_WorkshopContentFileName = "workshop.ini";
		/* dlc file */
		const std::string FK_DLCContentFileName = "dlcs.ini";
		/* personal ending file */
		const std::string FK_CharacterArcadeEndingFileName = "ending.txt";
		/* personal arcade config file - alternate/secret ending*/
		const std::string FK_CharacterArcadeAlternateEndingFileName = "ending_alt.txt";
		/* personal profile config file */
		const std::string FK_CharacterProfileFileName = "profile.txt";
		/* folder for configuration file*/
		const static std::string FK_ConfigurationFileFolder = "config\\";
		/* media folder */
		const static std::string FK_MediaFileFolder = "media\\";
		/* workshop folder (must be placed inside media folder) */
		const static std::string FK_WorkshopFolder = "workshop\\";
		/* workshop folder (must be placed inside media folder) */
		const static std::string FK_DLCFolder = "dlc\\";
		/* character folder (must be placed inside media folder) */
		const static std::string FK_CharactersFileFolder = "characters\\";
		/* stages folder (must be placed inside media folder)*/
		const static std::string FK_StagesFileFolder = "stages\\";
		/* workshop story episodes folder (must be placed inside workshop folder)*/
		const static std::string FK_WorkshopStoryEpisodesFolder = "episodes\\";
		/* workshop trials folder (must be placed inside workshop folder)*/
		const static std::string FK_WorkshopTrialsFolder = "trials\\";
		/* dlc story episodes folder (must be placed inside dlc folder)*/
		const static std::string FK_DLCStoryEpisodesFolder = "episodes\\";
		/* dlc trials folder (must be placed inside dlc folder)*/
		const static std::string FK_DLCTrialsFolder = "trials\\";
		/* common resources folder (must be placed inside media folder)*/
		const static std::string FK_CommonResourcesFileFolder = "common\\";
		/* fonts folder (must be placed inside common resources folder)*/
		const static std::string FK_FontsFileFolder = "fonts\\";
		/* folder which contains graphics for showing the joypad settings screen  (must be placed inside common resources folder)*/
		const static std::string FK_JoypadSetupFileFolder = "joypad_setup\\";
		/* folder which contains sound effects (must be placed inside common resources folder) */
		const static std::string FK_SoundEffectsFileFolder = "sound_effects\\";
		/* folder which contains announcer's voice clips (must be placed inside common resources folder) */
		const static std::string FK_VoiceoverEffectsFileFolder = "voice_clips\\";
		/* character folder with story content (must be placed inside character folder) */
		const static std::string FK_CharacterStoryFolder = "story\\";
		/* character folder with story content (must be placed inside character folder) */
		const static std::string FK_CharacterArcadeFolder = "arcade\\";
		/* folder which contains system sounds and BGMs (must be placed inside common resources folder) */
		const static std::string FK_SystemBGMFileFolder =  "system_bgm\\";
		/* file which contains general settings (must be placed into Configuration folder) */
		const static std::string FK_ConfigurationFileName = "config.ini";
		/* file which contains general settings (must be placed into Configuration folder) */
		const static std::string FK_GameOptionsFileName = "options.ini";
		/* file which contains input mapping */
		const static std::string FK_InputMapFileName = "inputMap.ini";
		/* file which contains input mapping from any joypad to Xbox buttons */
		const static std::string FK_JoypadToXboxTranslatorFileName = "joypadToXboxMap.ini";
		/* string which defines the guard particle effect */
		const static std::string FK_GuardSFXFileName = "particlewhite.bmp";
		/* string with the name of the character portrait */
		const static std::string FK_CharacterPortraitFileName = "portrait.png";
		/* file which contains a list of the selectable characters */
		const static std::string FK_CharacterRosterFileName = "roster.ini";
		/* file which contains a list of the selectable stages */
		const static std::string FK_AvailableStagesFileName = "stages.ini";
		/* arcade last match splash screen (default) */
		const static std::string FK_ArcadeDefaultSplashScreenFileName = "finalMatchLoading.jpg";
		/* arcade last match tagline (default) */
		const static std::string FK_ArcadeDefaultLastMatchTagline = "Duel of the Fates";
		/* guard (defensive action) button */
		const static FK_Input_Buttons FK_GuardButton = FK_Input_Buttons::Guard_Button;
		/* pause button*/
		const static FK_Input_Buttons FK_PauseGameButton = FK_Input_Buttons::Menu_Pause_Button;
		/* trigger button */
		const static FK_Input_Buttons FK_TriggerButton = FK_Input_Buttons::Trigger_Button;
		/* KO reaction*/
		const static FK_Reaction_Type FK_KOReaction = FK_Reaction_Type::StrongFlight;
		/* damage boost from counter attack */
		const static f32 FK_CounterAttackDamageMultiplier = 1.2f;
		/* damage boost from trigger combo */
		const static f32 FK_TriggerComboDamageMultiplier = 1.2f;
		/* determine how many trigger counters are necessary to activate a trigger combo */
		const static u32 FK_CountersPerTriggerComboActivation = 1;
		const static u32 FK_CountersRestoredForWinAtRoundBeginning = 1;
		const static u32 FK_CountersRestoredForLoseAtRoundBeginning = 2;
		const static u32 FK_CountersRestoredForDrawAtRoundBeginning = 1;
		/* duration of trigger guard */
		const static f32 FK_TriggerGuardDurationS = 0.5f;
		/* hitstun multipliers */
		const static f32 FK_CounterAttackHitstunMultiplier = 1.5;
		const static f32 FK_TriggerComboHitstunMultiplier = 3;
		/* maximal ringout distance from border */
		const static u32 FK_CountersPerImpactCancelActivation = 1;
		const static f32 FK_MaximumDistanceFromRingBorderForRingout = 50.0f;
		const static u32 FK_BufferTimeBeforeRunningEndsMs = 200;
		const static u32 FK_ImpactCancelRunSpeedMultiplier = 2;
		const static f32 FK_ImpactCancelRunDamageMultiplier = 1.4f;
		/* basic animation rate */
		const static f32 FK_BasicAnimationRate = 24.0;
		/* spritefont name*/
		const static std::string FK_MenuFontName = "ethnocentric";
		const static std::string FK_MenuFontIdentifier = "menuFont";
		const static int FK_MenuFontBaseSize = 18;
		const static int FK_MenuFontSizes_Size = 15;
		const static int FK_MenuFontSizes[15] = {6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 36, 48, 56, 68, 72};
		const static std::string FK_BattleFontName = "ethnocentric";
		const static std::string FK_BattleFontIdentifier = "battleFont";
		const static int FK_BattleFontBaseSize = 24;
		const static int FK_BattleFontSizes_Size = 13;
		const static int FK_BattleFontSizes[13] = {10, 12, 14, 16, 18, 20, 22, 24, 36, 48, 56, 68, 72 };
		const static std::string FK_TimerFontName = "ethnocentric";
		const static std::string FK_TimerFontIdentifier = "timerFont";
		const static int FK_TimerFontBaseSize = 12;
		const static int FK_TimerFontSizes_Size = 15;
		const static int FK_TimerFontSizes[15] = {6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 36, 48, 56, 68, 72 };
		const static std::string FK_GameFontName = "ethnocentric";
		const static std::string FK_GameFontIdentifier = "gameFont";
		const static int FK_GameFontBaseSize = 10;
		const static int FK_GameFontSizes_Size = 10;
		const static int FK_GameFontSizes[10] = {6, 8, 10, 12, 14, 16, 18, 20, 24, 28};
		const static std::string FK_WindowFontName = "textFont";
		const static std::string FK_WindowFontIdentifier = "winFont";
		const static int FK_WindowFontBaseSize = 10;
		const static int FK_WindowFontSizes_Size = 9;
		const static int FK_WindowFontSizes[9] = {8, 10, 12, 14, 16, 18, 20, 24, 28 };
		const static std::string FK_SmallNoteFontName = "textFont";
		const static std::string FK_SmallNoteFontIdentifier = "notesFont";
		const static int FK_SmallNoteFontBaseSize = 8;
		const static int FK_SmallNoteFontSizes_Size = 10;
		const static int FK_SmallNoteFontSizes[10] = { 4, 6, 8, 10, 12, 14, 16, 18, 20,  24};
		/* trigger counter max */
		const static u32 FK_MaxTriggerCounterNumber = 6;
		/* game speed (timer in-game normal velocity) */
		const static f32 FK_GameTimerNormalVelocity = 1.0f;
		const static f32 FK_GameTimerTriggerVelocity = 0.3f;
		/* flight time in MS for canceling bouncing */
		const static f32 FK_MinimumFlightTime = 100;
		const static u32 FK_MinimumTriggerCooldown = 150;
		const static u32 FK_MinimumTriggerInterval = 50;
		const static u32 FK_MaximumTriggerDuration = 250;
		/* maximum absolute duration for a round */
		const static u32 FK_MaxRoundTimerSeconds = 99;
		/* default resolution */
		const static irr::core::dimension2d<u32> FK_DefaultResolution = irr::core::dimension2d<u32>(640, 360);
		/* default character selection background file name*/
		const static std::string characterSelectionBackgroundFileName = "metal003.png";
		const static std::string characterSelectionCursorFileName = "selectionScreenCursorBig.png";
		const static std::string characterSelectionGenericPreviewFilename = "selectionScreenUnknown.png";
		const static std::string characterSelectionOverlayFileName = "selectionScreenOverlay.png";
		const static std::string characterSelectionLockedCharacterSignFileName = "lockedcostumeflag.png";
		const static std::string characterSelectionNoArcadeSignFileName = "noArcadeEnding.png";
		const static std::string characterSelectionEmptyCostumeDotFileName = "costumeDot.png";
		const static std::string characterSelectionFullCostumeDotFileName = "costumeDotFull.png";
	};
};
#endif