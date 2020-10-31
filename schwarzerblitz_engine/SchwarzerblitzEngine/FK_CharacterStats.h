#pragma once

namespace fk_engine {
	struct FK_CharacterStats {
		int numberOfUsedBullets;
		int numberOfTriggerCombos;
		int numberOfTriggerCancels;
		int numberOfTriggerGuards;
		f32 totalDamage;
		int maxComboDamage;
		int maxComboLength;
		void clear() {
			numberOfUsedBullets = 0;
			numberOfTriggerCombos = 0;
			numberOfTriggerCancels = 0;
			numberOfTriggerGuards = 0;
			maxComboLength = 0;
			totalDamage = 0.f;
			maxComboDamage = 0;
		}
		FK_CharacterStats() {
			clear();
		}
	};
}
