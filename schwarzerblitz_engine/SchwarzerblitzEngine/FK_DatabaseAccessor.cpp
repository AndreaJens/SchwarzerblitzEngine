#include"FK_DatabaseAccessor.h"

namespace fk_engine {
	FK_DatabaseAccessor::FK_DatabaseAccessor() {
		initialize();
	}

	void FK_DatabaseAccessor::initialize() {
		// booleans
		allowTriggerUse = true;
		allowTriggerCombo = true;
		allowTriggerGuard = true;
		allowRingout = true;
		applySlowmoEffectOnTrigger = true;
		allowTriggerRegen = false;
		allowRunCancel = true;
		// counter attacks
		allowSlowmoOnCounterattack = false;
		playCounterAttackSoundEffectFlag = false;
		counterAttackDamageMultiplier = fk_constants::FK_CounterAttackDamageMultiplier;
		counterAttackHitstunMultiplier = fk_constants::FK_CounterAttackHitstunMultiplier;
		// triggers (super counters)
		triggerCountersForRoundWon = fk_constants::FK_CountersRestoredForWinAtRoundBeginning;
		triggerCountersForRoundLost = fk_constants::FK_CountersRestoredForLoseAtRoundBeginning;
		triggerCountersForDraw = fk_constants::FK_CountersRestoredForDrawAtRoundBeginning;
		maxTriggerCounters = fk_constants::FK_MaxTriggerCounterNumber;
		initialTriggerCounters = fk_constants::FK_MaxTriggerCounterNumber;
		triggerRegenTimeMs = 2000;
		triggerRegenTimeOnBlock = 0;
		triggerRegenTimeOnHit = 0;
		triggerRegenZeroOnUse = true;
		triggerRegenGainInTriggerMode = false;
		// trigger combo
		triggerComboDamageMultiplier = fk_constants::FK_TriggerComboDamageMultiplier;
		triggerComboHitstunMultiplier = fk_constants::FK_TriggerComboHitstunMultiplier;
		triggerCountersPerActivation = fk_constants::FK_CountersPerTriggerComboActivation;
		// trigger guard
		triggerGuardDurationMs = (u32)std::round((1000) * fk_constants::FK_TriggerGuardDurationS);
		// run cancel
		triggerCountersPerRunCancelActivation = fk_constants::FK_CountersPerImpactCancelActivation;
		runCancelSpeedMultiplier = fk_constants::FK_ImpactCancelRunSpeedMultiplier;
		runCancelDamageMultiplier = fk_constants::FK_ImpactCancelRunDamageMultiplier;
		// animation frame rate
		animationFrameRateFPS = fk_constants::FK_BasicAnimationRate;
		// slowmo effect
		normalTimerVelocity = fk_constants::FK_GameTimerNormalVelocity;
		slowmoTimerVelocity = fk_constants::FK_GameTimerTriggerVelocity;
		slowmoDurationMs = fk_constants::FK_MaximumTriggerDuration;
		slowmoMinimumCooldownMs = fk_constants::FK_MinimumTriggerCooldown;
		slowmoIntervalMs = fk_constants::FK_MinimumTriggerInterval;
		// ringout
		ringoutDistanceFromWall = fk_constants::FK_MaximumDistanceFromRingBorderForRingout;
		// character
		characterWalkingSpeed = 80.0f;
		characterSidestepSpeed = 150.f;
		characterMixedStepSpeed = 100.f;
		characterRunningSpeed = 200.0f;
		characterWeakHitstunDurationMs = 250;
		characterWeakHitstunMovementDurationMs = 100;
		characterStrongHitstunDurationMs = 400;
		characterStrongHitstunMovementDurationMs = 200;
		characterMaxHitstunDurationMs = 600;
		characterMoveCooldownMs = 50;
		characterGuardHitstunDurationMs = 200;
		characterGuardHitstunMovementDurationMs = 100;
		characterMaxTransversalSpeedAfterHit = 50.f;
		// counter attack effect
		counterAttackEffectDurationMs = 0;
		counterAttackSlowmoTimerVelocity = 1.0f;
		counterAttackCameraZoomingDistance = 0.f;
		counterAttackEffectCooldownMs = 0;
		// round win flag
		showRoundWinFlag = true;
		// pushback
		pushbackVelocity = -350.f;
		maxPushbackDurationMs = 100;
		allowPushbackOnBlock = true;
		// trigger guard frame advantage
		triggerGuardFrameAdvantage = 0;
		// run jump cancel
		allowJumpRunCancel = true;
		// extended juggles
		extendedJugglesAllowedFlag = false;
		juggleDamageMultiplier = 1.f;
	}

	void FK_DatabaseAccessor::setupFromFile(std::string filename) {
		std::ifstream ifile(filename.c_str());
		if (!ifile) {
			return;
		}
		std::string temp;
		// start dissecting the file with key-based search
		while (ifile >> temp && !ifile.eof()) {

			// bools
			if (temp == "#allow_triggers") {
				s32 val;
				ifile >> val;
				allowTriggerUse = val != 0;
			}else if(temp == "#allow_trigger_combos") {
				s32 val;
				ifile >> val;
				allowTriggerCombo = val != 0;
			}
			else if (temp == "#allow_trigger_guard") {
				s32 val;
				ifile >> val;
				allowTriggerGuard = val != 0;
			}
			else if (temp == "#allow_run_cancel") {
				s32 val;
				ifile >> val;
				allowRunCancel = val != 0;
			}
			else if (temp == "#allow_ringout") {
				s32 val;
				ifile >> val;
				allowRingout = val != 0;
			}
			else if (temp == "#allow_trigger_regeneration") {
				s32 val;
				ifile >> val;
				allowTriggerRegen = val != 0;
			}
			else if (temp == "#allow_slowmo_effect") {
				s32 val;
				ifile >> val;
				applySlowmoEffectOnTrigger = val != 0;
			}
			else if (temp == "#allow_slowmo_on_counterattack") {
				s32 val;
				ifile >> val;
				allowSlowmoOnCounterattack = val != 0;
			}
			else if (temp == "#play_sound_effect_on_counterattack") {
				s32 val;
				ifile >> val;
				playCounterAttackSoundEffectFlag = val != 0;
			}
			else if (temp == "#show_round_win_outro") {
				s32 val;
				ifile >> val;
				showRoundWinFlag = val != 0;
			}
			else if (temp == "#show_stage_intro") {
				s32 val;
				ifile >> val;
				showStageIntroFlag = val != 0;
			}
			// triggers
			else if (temp == "#trigger_counters_for_round_won") {
				s32 val;
				ifile >> val;
				triggerCountersForRoundWon = val;
			}
			else if (temp == "#trigger_counters_for_round_lost") {
				s32 val;
				ifile >> val;
				triggerCountersForRoundLost = val;
			}
			else if (temp == "#trigger_counters_for_draw") {
				s32 val;
				ifile >> val;
				triggerCountersForDraw = val;
			}
			else if (temp == "#max_trigger_counters") {
				s32 val;
				ifile >> val;
				maxTriggerCounters = val;
			}
			else if (temp == "#initial_trigger_counters") {
				s32 val;
				ifile >> val;
				initialTriggerCounters = val;
			}

			// counter attack
			else if (temp == "#counterattack_damage_multiplier") {
				f32 val;
				ifile >> val;
				counterAttackDamageMultiplier = val;
			}
			else if (temp == "#counterattack_histun_multiplier") {
				f32 val;
				ifile >> val;
				counterAttackHitstunMultiplier = val;
			}
			else if (temp == "#counterattack_slowmo_duration_ms") {
				u32 val;
				ifile >> val;
				counterAttackEffectDurationMs = val;
			}
			else if (temp == "#counterattack_slowmo_cooldown_ms") {
				u32 val;
				ifile >> val;
				counterAttackEffectCooldownMs = val;
			}
			else if (temp == "#counterattack_slowmo_timer_velocity") {
				f32 val;
				ifile >> val;
				counterAttackSlowmoTimerVelocity = val;
			}
			else if (temp == "#counterattack_camera_zoom_distance") {
				f32 val;
				ifile >> val;
				counterAttackCameraZoomingDistance = val;
			}

			// trigger regen
			else if (temp == "#trigger_regeneration_time_ms") {
				s32 val;
				ifile >> val;
				triggerRegenTimeMs = val;
			}
			else if (temp == "#trigger_regeneration_on_hit") {
				s32 val;
				ifile >> val;
				triggerRegenTimeOnHit = val;
			}
			else if (temp == "#trigger_regeneration_on_block") {
				s32 val;
				ifile >> val;
				triggerRegenTimeOnBlock = val;
			}
			else if (temp == "#consume_regen_bar_on_trigger_use") {
				s32 val;
				ifile >> val;
				triggerRegenZeroOnUse = val != 0;
			}
			else if (temp == "#allow_trigger_regen_in_trigger_mode") {
				s32 val;
				ifile >> val;
				triggerRegenGainInTriggerMode = val != 0;
			}

			// trigger combos
			else if (temp == "#trigger_combo_damage_multiplier") {
				f32 val;
				ifile >> val;
				triggerComboDamageMultiplier = val;
			}
			else if (temp == "#trigger_combo_hitstun_multiplier") {
				f32 val;
				ifile >> val;
				triggerComboHitstunMultiplier = val;
			}
			else if (temp == "#trigger_counters_per_activation") {
				s32 val;
				ifile >> val;
				triggerCountersPerActivation = val;
			}

			// trigger guard
			else if (temp == "#trigger_guard_duration_ms") {
				u32 val;
				ifile >> val;
				triggerGuardDurationMs = val;
			}

			// run cancel
			else if (temp == "#run_cancel_damage_multiplier") {
				f32 val;
				ifile >> val;
				runCancelDamageMultiplier = val;
			}
			else if (temp == "#run_cancel_speed_multiplier") {
				f32 val;
				ifile >> val;
				runCancelSpeedMultiplier = val;
			}
			else if (temp == "#trigger_counters_per_run_cancel") {
				s32 val;
				ifile >> val;
				triggerCountersPerRunCancelActivation = val;
			}

			// animation FPS
			else if (temp == "#animation_framerate_fps") {
				f32 val;
				ifile >> val;
				animationFrameRateFPS = val;
			}

			// characters
			else if (temp == "#walking_speed") {
				f32 val;
				ifile >> val;
				characterWalkingSpeed = val;
			}
			else if (temp == "#sidestep_speed") {
				f32 val;
				ifile >> val;
				characterSidestepSpeed = val;
			}
			else if (temp == "#mixedstep_speed") {
				f32 val;
				ifile >> val;
				characterMixedStepSpeed = val;
			}
			else if (temp == "#running_speed") {
				f32 val;
				ifile >> val;
				characterRunningSpeed = val;
			}
			else if (temp == "#strong_hitstun_duration_ms") {
				u32 val;
				ifile >> val;
				characterStrongHitstunDurationMs = val;
			}
			else if (temp == "#weak_hitstun_duration_ms") {
				u32 val;
				ifile >> val;
				characterWeakHitstunDurationMs = val;
			}
			else if (temp == "#strong_hitstun_pushback_duration_ms") {
				u32 val;
				ifile >> val;
				characterStrongHitstunMovementDurationMs = val;
			}
			else if (temp == "#weak_hitstun_pushback_duration_ms") {
				u32 val;
				ifile >> val;
				characterWeakHitstunMovementDurationMs = val;
			}
			else if (temp == "#guard_hitstun_duration_ms") {
				u32 val;
				ifile >> val;
				characterGuardHitstunDurationMs = val;
			}
			else if (temp == "#guard_hitstun_pushback_duration_ms") {
				u32 val;
				ifile >> val;
				characterGuardHitstunMovementDurationMs = val;
			}
			else if (temp == "#max_transverse_hitstun_speed") {
				f32 val;
				ifile >> val;
				characterMaxTransversalSpeedAfterHit = val;
			}
			else if (temp == "#max_hitstun_duration_ms") {
				u32 val;
				ifile >> val;
				characterMaxHitstunDurationMs = val;
			}
			else if (temp == "#after_move_cooldown_duration_ms") {
				u32 val;
				ifile >> val;
				characterMoveCooldownMs = val;
			}
			// frame advantage trigger guard
			else if (temp == "#trigger_guard_frame_advantage") {
				u32 val;
				ifile >> val;
				triggerGuardFrameAdvantage = val;
			}
			// slowmo
			else if (temp == "#normal_timer_velocity") {
				f32 val;
				ifile >> val;
				normalTimerVelocity = val;
			}
			else if (temp == "#slowmo_timer_velocity") {
				f32 val;
				ifile >> val;
				slowmoTimerVelocity = val;
			}
			else if (temp == "#slowmo_duration_ms") {
				u32 val;
				ifile >> val;
				slowmoDurationMs = val;
			}
			else if (temp == "#slowmo_interval_ms") {
				u32 val;
				ifile >> val;
				slowmoIntervalMs = val;
			}
			else if (temp == "#slowmo_cooldown_ms") {
				u32 val;
				ifile >> val;
				slowmoMinimumCooldownMs = val;
			}
			// pushback
			else if (temp == "#pushback_duration_ms") {
				s32 val;
				ifile >> val;
				maxPushbackDurationMs = val;
			}
			else if (temp == "#pushback_velocity") {
				f32 val;
				ifile >> val;
				pushbackVelocity = val;
			}
			else if (temp == "#allow_pushback_on_block") {
				s32 val;
				ifile >> val;
				allowPushbackOnBlock = val != 0;
			}
			// run jump cancel
			else if (temp == "#allow_run_jump_cancel") {
				s32 val;
				ifile >> val;
				allowJumpRunCancel = val != 0;
			}
			// extend juggle modifier
			else if (temp == "#allow_extended_juggles") {
				s32 val;
				ifile >> val;
				extendedJugglesAllowedFlag = val != 0;
			}
			// extend juggle modifier
			else if (temp == "#juggle_damage_scaling_factor") {
				f32 val;
				ifile >> val;
				if (val >= 0.f) {
					juggleDamageMultiplier = val;
				}
			}
		}
	}

	bool FK_DatabaseAccessor::triggerAllowed()
	{
		return allowTriggerUse;
	}

	// boolean accessors
	bool FK_DatabaseAccessor::triggerCombosAllowed()
	{
		return triggerAllowed() && allowTriggerCombo;
	}
	bool FK_DatabaseAccessor::triggerGuardAllowed()
	{
		return triggerAllowed() && allowTriggerGuard;
	}
	bool FK_DatabaseAccessor::ringoutAllowed()
	{
		return allowRingout;
	}
	bool FK_DatabaseAccessor::slowmoEffectAllowed()
	{
		return applySlowmoEffectOnTrigger;
	}
	bool FK_DatabaseAccessor::slowmoEffectOnCounterAttackAllowed()
	{
		return allowSlowmoOnCounterattack;
	}
	bool FK_DatabaseAccessor::showRoundWinScreen()
	{
		return showRoundWinFlag;
	}
	bool FK_DatabaseAccessor::showStageIntro()
	{
		return showStageIntroFlag;
	}
	bool FK_DatabaseAccessor::extendedJugglesAllowed()
	{
		return extendedJugglesAllowedFlag;
	}
	f32 FK_DatabaseAccessor::getJuggleDamageMultiplier()
	{
		return juggleDamageMultiplier;
	}
	bool FK_DatabaseAccessor::triggerRegenAllowed()
	{
		return triggerAllowed() && allowTriggerRegen;
	}
	bool FK_DatabaseAccessor::runCancelAllowed()
	{
		return triggerAllowed() && allowRunCancel;
	}

	bool FK_DatabaseAccessor::playSoundEffectOnCounterAttack()
	{
		return playCounterAttackSoundEffectFlag;
	}


	// trigger information
	s32 FK_DatabaseAccessor::getTriggerRegenPerRoundWon()
	{
		if (triggerAllowed()) {
			return triggerCountersForRoundWon;
		}
		else {
			return 0;
		}
	}

	s32 FK_DatabaseAccessor::getTriggerRegenPerRoundLost()
	{
		if (triggerAllowed()) {
			return triggerCountersForRoundLost;
		}
		else {
			return 0;
		}
	}

	s32 FK_DatabaseAccessor::getTriggerRegenPerDraw()
	{
		if (triggerAllowed()) {
			return triggerCountersForDraw;
		}
		else {
			return 0;
		}
	}

	s32 FK_DatabaseAccessor::getMaxTriggers()
	{
		if (triggerAllowed()) {
			return maxTriggerCounters;
		}
		else {
			return 0;
		}
	}

	s32 FK_DatabaseAccessor::getStartingTriggers()
	{
		if (triggerAllowed()) {
			return initialTriggerCounters;
		}
		else {
			return 0;
		}
	}

	// counter attack

	f32 FK_DatabaseAccessor::getCounterAttackDamageMultiplier()
	{
		return counterAttackDamageMultiplier;
	}

	f32 FK_DatabaseAccessor::getCounterAttackHitstunMultiplier()
	{
		return counterAttackDamageMultiplier;
	}

	u32 FK_DatabaseAccessor::getCounterAttackEffectDurationMs()
	{
		return counterAttackEffectDurationMs;
	}

	f32 FK_DatabaseAccessor::getCounterAttackSlowmoTimerVelocity()
	{
		return counterAttackSlowmoTimerVelocity;
	}

	f32 FK_DatabaseAccessor::getCounterAttackCameraZoomingDistance()
	{
		return counterAttackCameraZoomingDistance;
	}

	u32 FK_DatabaseAccessor::getCounterAttackEffectCooldownMs()
	{
		return counterAttackEffectCooldownMs;
	}

	// trigger regen

	s32 FK_DatabaseAccessor::getTriggerRegenTimeMs()
	{
		if (triggerRegenAllowed()) {
			return triggerRegenTimeMs;
		}
		else {
			return -1;
		}
	}

	s32 FK_DatabaseAccessor::getTriggerRegenTimeOnBlock()
	{
		if (triggerRegenAllowed()) {
			return triggerRegenTimeOnBlock;
		}
		else {
			return 0;
		}
	}

	s32 FK_DatabaseAccessor::getTriggerRegenTimeOnHit()
	{
		if (triggerRegenAllowed()) {
			return triggerRegenTimeOnHit;
		}
		else {
			return 0;
		}
	}

	bool FK_DatabaseAccessor::zeroRegenOnTriggerUse()
	{
		return triggerRegenZeroOnUse || !triggerRegenAllowed();
	}

	// allow trigger regen during trigger mode
	bool FK_DatabaseAccessor::allowTriggerGainInTriggerMode()
	{
		return triggerRegenGainInTriggerMode && triggerRegenAllowed();
	}

	// trigger combos
	f32 FK_DatabaseAccessor::getTriggerComboDamageMultiplier()
	{
		if (triggerCombosAllowed()) {
			return triggerComboDamageMultiplier;
		}
		else {
			return 1.0f;
		}
	}

	f32 FK_DatabaseAccessor::getTriggerComboHitstunMultiplier()
	{
		if (triggerCombosAllowed()) {
			return triggerComboHitstunMultiplier;
		}
		else {
			return 1.0f;
		}
	}

	s32 FK_DatabaseAccessor::getTriggerCountersPerActivation()
	{
		if (triggerCombosAllowed()) {
			return triggerCountersPerActivation;
		}
		else {
			return 0;
		}
	}

	// trigger guard
	u32 FK_DatabaseAccessor::getTriggerGuardDurationMs()
	{
		if (triggerGuardAllowed()) {
			return triggerGuardDurationMs;
		}
		else {
			return 0;
		}
	}

	// run cancel
	s32 FK_DatabaseAccessor::getTriggerCountersPerRunCancelActivation()
	{
		if (runCancelAllowed()) {
			return triggerCountersPerRunCancelActivation;
		}
		else {
			return 0;
		}
	}

	f32 FK_DatabaseAccessor::getRunCancelDamageMultiplier()
	{
		if (runCancelAllowed()) {
			return runCancelDamageMultiplier;
		}
		else {
			return 1.f;
		}
	}

	f32 FK_DatabaseAccessor::getRunCancelSpeedMultiplier()
	{
		if (runCancelAllowed()) {
			return runCancelSpeedMultiplier;
		}
		else {
			return 1.f;
		}
	}

	// slowmo
	f32 FK_DatabaseAccessor::getNormalTimerVelocity()
	{
		return normalTimerVelocity;
	}

	f32 FK_DatabaseAccessor::getSlowmoTimerVelocity()
	{
		return slowmoTimerVelocity;
	}

	u32 FK_DatabaseAccessor::getSlowmoDurationMs()
	{
		if (slowmoEffectAllowed()) {
			return slowmoDurationMs;
		}
		else {
			return 0;
		}
	}

	u32 FK_DatabaseAccessor::getSlowmoCooldownMs()
	{
		if (slowmoEffectAllowed()) {
			return slowmoMinimumCooldownMs;
		}
		else {
			return 0;
		}
	}

	u32 FK_DatabaseAccessor::getSlowmoIntervalMs()
	{
		if (slowmoEffectAllowed()) {
			return slowmoIntervalMs;
		}
		else {
			return 0;
		}
	}

	// ringout
	f32 FK_DatabaseAccessor::getRingoutDistanceFromWall()
	{
		if (ringoutAllowed()) {
			return ringoutDistanceFromWall;
		}
		else {
			return -1.0f;
		}
	}

	// animations
	f32 FK_DatabaseAccessor::getAnimationFramerateFPS()
	{
		return animationFrameRateFPS;
	}

	// characters

	f32 FK_DatabaseAccessor::getBasicCharacterWalkSpeed()
	{
		return characterWalkingSpeed;
	}

	f32 FK_DatabaseAccessor::getBasicCharacterSidestepSpeed()
	{
		return characterSidestepSpeed;
	}

	f32 FK_DatabaseAccessor::getBasicCharacterMixedStepSpeed()
	{
		return characterMixedStepSpeed;
	}

	f32 FK_DatabaseAccessor::getBasicCharacterRunningSpeed()
	{
		return characterRunningSpeed;
	}

	u32 FK_DatabaseAccessor::getWeakHitstunDurationMs() {
		return characterWeakHitstunDurationMs;
	}

	u32 FK_DatabaseAccessor::getWeakHitstunMovementDurationMs()
	{
		return characterWeakHitstunMovementDurationMs;
	}

	u32 FK_DatabaseAccessor::getStrongHitstunDurationMs() {
		return characterStrongHitstunDurationMs;
	}
	u32 FK_DatabaseAccessor::getStrongHitstunMovementDurationMs()
	{
		return characterStrongHitstunMovementDurationMs;
	}
	u32 FK_DatabaseAccessor::getCharacterMoveCooldownDurationMs() {
		return characterMoveCooldownMs;
	}
	u32 FK_DatabaseAccessor::getTriggerGuardFrameAdvantage()
	{
		return triggerGuardFrameAdvantage;
	}
	u32 FK_DatabaseAccessor::getMaxHitstunDurationMs() {
		return characterMaxHitstunDurationMs;
	}

	u32 FK_DatabaseAccessor::getGuardHitstunDurationMs()
	{
		return characterGuardHitstunDurationMs;
	}

	u32 FK_DatabaseAccessor::getGuardHitstunMovementDurationMs()
	{
		return characterGuardHitstunMovementDurationMs;
	}

	f32 FK_DatabaseAccessor::getMaxTransversalSpeedAfterHit()
	{
		return characterMaxTransversalSpeedAfterHit;
	}

	s32 FK_DatabaseAccessor::getMaxPushbackDurationMs()
	{
		return maxPushbackDurationMs;
	}

	bool FK_DatabaseAccessor::blockPushbackAllowed() {
		return allowPushbackOnBlock;
	}

	f32 FK_DatabaseAccessor::getPushbackVelocity() {
		return pushbackVelocity;
	}

	bool FK_DatabaseAccessor::runJumpCancelAllowed()
	{
		return allowJumpRunCancel;
	}

}