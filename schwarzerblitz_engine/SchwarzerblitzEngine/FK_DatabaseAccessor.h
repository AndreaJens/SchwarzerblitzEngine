#ifndef FK_DATABASEACCESSOR_STRUCT
#define FK_DATABASEACCESSOR_STRUCT

#include"FK_Database.h"
#include<fstream>

namespace fk_engine {
		class FK_DatabaseAccessor {
		public:
			FK_DatabaseAccessor();
			void setupFromFile(std::string filename);
			// bools
			bool triggerAllowed();
			bool triggerCombosAllowed();
			bool triggerGuardAllowed();
			bool ringoutAllowed();
			bool slowmoEffectAllowed();
			bool triggerRegenAllowed();
			bool runCancelAllowed();
			bool slowmoEffectOnCounterAttackAllowed();
			bool playSoundEffectOnCounterAttack();
			bool showRoundWinScreen();
			bool showStageIntro();
			bool extendedJugglesAllowed();
			f32 getJuggleDamageMultiplier();
			// triggers
			s32 getTriggerRegenPerRoundWon();
			s32 getTriggerRegenPerRoundLost();
			s32 getTriggerRegenPerDraw();
			s32 getMaxTriggers();
			s32 getStartingTriggers();
			// counter attack
			f32 getCounterAttackDamageMultiplier();
			f32 getCounterAttackHitstunMultiplier();
			u32 getCounterAttackEffectDurationMs();
			f32 getCounterAttackSlowmoTimerVelocity();
			f32 getCounterAttackCameraZoomingDistance();
			u32 getCounterAttackEffectCooldownMs();
			// trigger regen
			s32 getTriggerRegenTimeMs();
			s32 getTriggerRegenTimeOnBlock();
			s32 getTriggerRegenTimeOnHit();
			bool zeroRegenOnTriggerUse();
			bool allowTriggerGainInTriggerMode();
			// trigger combos
			f32 getTriggerComboDamageMultiplier();
			f32 getTriggerComboHitstunMultiplier();
			s32 getTriggerCountersPerActivation();
			// trigger guard
			u32 getTriggerGuardDurationMs();
			// run cancel
			s32 getTriggerCountersPerRunCancelActivation();
			f32 getRunCancelDamageMultiplier();
			f32 getRunCancelSpeedMultiplier();
			// slowmo effect
			f32 getNormalTimerVelocity();
			f32 getSlowmoTimerVelocity();
			u32 getSlowmoDurationMs();
			u32 getSlowmoCooldownMs();
			u32 getSlowmoIntervalMs();
			// ringout
			f32 getRingoutDistanceFromWall();
			// animations
			f32 getAnimationFramerateFPS();
			// characters
			// walkspeed
			f32 getBasicCharacterWalkSpeed();
			f32 getBasicCharacterSidestepSpeed();
			f32 getBasicCharacterMixedStepSpeed();
			f32 getBasicCharacterRunningSpeed();
			// cooldown after move
			u32 getCharacterMoveCooldownDurationMs();
			// hitstun
			u32 getTriggerGuardFrameAdvantage();
			// weak
			u32 getWeakHitstunDurationMs();
			u32 getWeakHitstunMovementDurationMs();
			// strong
			u32 getStrongHitstunDurationMs();
			u32 getStrongHitstunMovementDurationMs();
			// max duration
			u32 getMaxHitstunDurationMs();
			// guard
			u32 getGuardHitstunDurationMs();
			u32 getGuardHitstunMovementDurationMs();
			// max push trasnverse speed
			f32 getMaxTransversalSpeedAfterHit();
			// game mechanics - pushback
			s32 getMaxPushbackDurationMs();
			bool blockPushbackAllowed();
			f32 getPushbackVelocity();
			// game mechanics - run jump cancel
			bool runJumpCancelAllowed();
		protected:
			void initialize();
		private:
			// booleans
			bool allowTriggerUse;
			bool allowTriggerCombo;
			bool allowTriggerGuard;
			bool allowRingout;
			bool applySlowmoEffectOnTrigger;
			bool allowTriggerRegen;
			bool allowRunCancel;
			// counter attacks
			f32 counterAttackDamageMultiplier;
			f32 counterAttackHitstunMultiplier;
			// triggers (super counters)
			s32 triggerCountersForRoundWon;
			s32 triggerCountersForRoundLost;
			s32 triggerCountersForDraw;
			s32 maxTriggerCounters;
			s32 initialTriggerCounters;
			s32 triggerRegenTimeMs;
			s32 triggerRegenTimeOnBlock;
			s32 triggerRegenTimeOnHit;
			bool triggerRegenZeroOnUse;
			bool triggerRegenGainInTriggerMode;
			// trigger combo
			f32 triggerComboDamageMultiplier;
			f32 triggerComboHitstunMultiplier;
			s32 triggerCountersPerActivation;
			// trigger guard
			u32 triggerGuardDurationMs;
			// run cancel
			s32 triggerCountersPerRunCancelActivation;
			f32 runCancelSpeedMultiplier;
			f32 runCancelDamageMultiplier;
			// animation frame rate
			f32 animationFrameRateFPS;
			// slowmo effect
			f32 normalTimerVelocity;
			f32 slowmoTimerVelocity;
			u32 slowmoDurationMs;
			u32 slowmoMinimumCooldownMs;
			u32 slowmoIntervalMs;
			// trigger guard frame advante
			u32 triggerGuardFrameAdvantage;
			// ringout
			f32 ringoutDistanceFromWall;
			// character
			f32 characterWalkingSpeed;
			f32 characterSidestepSpeed;
			f32 characterMixedStepSpeed;
			f32 characterRunningSpeed;
			u32 characterWeakHitstunDurationMs;
			u32 characterWeakHitstunMovementDurationMs;
			u32 characterStrongHitstunDurationMs;
			u32 characterStrongHitstunMovementDurationMs;
			u32 characterMaxHitstunDurationMs;
			u32 characterMoveCooldownMs;
			u32 characterGuardHitstunDurationMs;
			u32 characterGuardHitstunMovementDurationMs;
			f32 characterMaxTransversalSpeedAfterHit;
			// counter attack effect
			bool allowSlowmoOnCounterattack;
			u32 counterAttackEffectDurationMs;
			u32 counterAttackEffectCooldownMs;
			f32 counterAttackSlowmoTimerVelocity;
			bool showRoundWinFlag;
			bool showStageIntroFlag;
			bool playCounterAttackSoundEffectFlag;
			f32 counterAttackCameraZoomingDistance;
			// pushback
			s32 maxPushbackDurationMs;
			bool allowPushbackOnBlock;
			f32 pushbackVelocity;
			// jump run cancel
			bool allowJumpRunCancel;
			// allow extended juggles
			bool extendedJugglesAllowedFlag;
			// juggle damage multiplier
			f32 juggleDamageMultiplier;
		};
}

#endif
