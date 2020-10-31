// we don't use this class if this build is not intended for Steam
#include "FK_AchievementManager.h"
#ifdef STEAM_SUPPORT
#include<vector>

namespace fk_engine {
	FK_StatManager::FK_StatManager()
	{
		numberOfUsedBulletCounters = 0;
		numberOfTriggerCombos = 0;
		numberOfTriggerGuards = 0;
		numberOfTriggerCancels = 0;
		maxComboDamage = 0;
		totalDamageDealt = 0;
		maxComboLength = 0;
		steamUserStats = SteamUserStats();
	}
	bool FK_StatManager::retrieveStatsFromServer()
	{
		if (!steamUserStats) {
			return false;
		}
		steamUserStats->GetStat("bullet_used_p1", &numberOfUsedBulletCounters);
		steamUserStats->GetStat("trigger_combo_p1", &numberOfTriggerCombos);
		steamUserStats->GetStat("trigger_guard_p1", &numberOfTriggerGuards);
		steamUserStats->GetStat("trigger_cancel_p1", &numberOfTriggerCancels);
		steamUserStats->GetStat("total_damage", &totalDamageDealt);
		steamUserStats->GetStat("max_combo_damage", &maxComboDamage);
		steamUserStats->GetStat("max_combo_length", &maxComboLength);
		return true;
	}

	bool FK_StatManager::uploadStatsToServer()
	{
		if (!steamUserStats) {
			return false;
		}
		// set stats
		steamUserStats->SetStat("bullet_used_p1", numberOfUsedBulletCounters);
		steamUserStats->SetStat("trigger_combo_p1", numberOfTriggerCombos);
		steamUserStats->SetStat("trigger_guard_p1", numberOfTriggerGuards);
		steamUserStats->SetStat("trigger_cancel_p1", numberOfTriggerCancels);
		steamUserStats->SetStat("total_damage", totalDamageDealt);
		steamUserStats->SetStat("max_combo_damage", maxComboDamage);
		steamUserStats->SetStat("max_combo_length", maxComboLength);
		// check for achievements
		bool achievementCheckSuccess = checkForAchievements();
		bool success = steamUserStats->StoreStats();
		return success && achievementCheckSuccess;
	}

	bool FK_StatManager::checkForAchievements()
	{
		if (!steamUserStats) {
			return false;
		}
		std::vector<Achievement> achievementsToCheck{
			Achievement("TRIGGER_GUARD",	19,		numberOfTriggerGuards),
			Achievement("TRIGGER_CANCEL",	19,		numberOfTriggerCancels),
			Achievement("TRIGGER_COMBO",	19,		numberOfTriggerCombos),
			Achievement("500_BULLETS",		499,	numberOfUsedBulletCounters),
			Achievement("DIAMOND_CRUSHER",	1199,	maxComboDamage),
			Achievement("COMBO_MASTER",		39,		maxComboLength)
		};
		for (auto &achievement : achievementsToCheck) {
			bool success = steamUserStats->GetAchievement(achievement.id.c_str(), &achievement.achieved);
			if (!achievement.achieved && achievement.conditionsFulfilled()) {
				steamUserStats->SetAchievement(achievement.id.c_str());
			}
		}
		return true;
	}

	// arcade
	bool FK_StatManager::checkForArcadeAchievements(FK_Scene::FK_SceneArcadeType sceneType)
	{
		if (!steamUserStats) {
			return false;
		}
		int classicArcade = 0;
		int timeAttack = 0;
		int survival = 0;
		switch (sceneType) {
		case FK_Scene::FK_SceneArcadeType::ArcadeClassic :
				classicArcade += 1;
				break;
		case FK_Scene::FK_SceneArcadeType::ArcadeSurvival:
			survival += 1;
			break;
		case FK_Scene::FK_SceneArcadeType::ArcadeTimeAttack:
			timeAttack += 1;
			break;
		default:
			break;			
		}
		std::vector<Achievement> achievementsToCheck{
			Achievement("ARCADE_1",		0,		classicArcade),
			Achievement("ARCADE_2",		0,		survival),
			Achievement("ARCADE_3",		0,		timeAttack),
		};
		for (auto& achievement : achievementsToCheck) {
			bool success = steamUserStats->GetAchievement(achievement.id.c_str(), &achievement.achieved);
			if (!achievement.achieved && achievement.conditionsFulfilled()) {
				steamUserStats->SetAchievement(achievement.id.c_str());
			}
		}
		bool success = steamUserStats->StoreStats();
		return true;
	}

	// story
	bool FK_StatManager::checkForStoryAchievements(std::vector<std::string> storyEpisodesCleared) {
		if (!steamUserStats) {
			return false;
		}
		int prologue = 0;
		int epilogue = 0;
		int allArcade = 0;
		prologue = (int)(std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), "prologue\\") != storyEpisodesCleared.end());
		epilogue = (int)(std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), "epilogue\\") != storyEpisodesCleared.end());
		bool allMeaningfulArcadeEndings = true;
		std::vector<std::string> meaningfulEndings{
			"chara_johnson\\",
			"chara_skeleton\\",
			"chara_tiger\\",
			"chara_graf\\",
			"chara_shark\\",
		    "chara_lobster\\",
			"chara_amy\\",
			"chara_night\\",
			"chara_cyphr\\",
			"chara_krave\\",
			"chara_renzo\\",
			"chara_elena\\",
			"chara_jenn\\",
			"chara_kiyoko\\",
			"chara_wally\\",
			"chara_mono\\",
		};
		for (auto ending : meaningfulEndings) {
			allMeaningfulArcadeEndings &= std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), ending) != storyEpisodesCleared.end();
		}
		allArcade = (int)allMeaningfulArcadeEndings;
		std::vector<Achievement> achievementsToCheck{
			Achievement("STORY_1",		0,		prologue),
			Achievement("STORY_2",		0,		allArcade),
			Achievement("STORY_3",		0,		epilogue),
		};
		for (auto& achievement : achievementsToCheck) {
			bool success = steamUserStats->GetAchievement(achievement.id.c_str(), &achievement.achieved);
			if (!achievement.achieved && achievement.conditionsFulfilled()) {
				steamUserStats->SetAchievement(achievement.id.c_str());
			}
		}
		return true;
	}

	// story
	bool FK_StatManager::checkForTagAchievements(std::vector<std::string> unlockedTags) {
		if (!steamUserStats) {
			return false;
		}
		int arkana = 0;
		int doll = 0;
		int k069 = 0;
		arkana = (int)(std::find(unlockedTags.begin(), unlockedTags.end(), "arkana") != unlockedTags.end());
		doll = (int)(std::find(unlockedTags.begin(), unlockedTags.end(), "doll") != unlockedTags.end());
		k069 = (int)(std::find(unlockedTags.begin(), unlockedTags.end(), "enigma") != unlockedTags.end());
		std::vector<Achievement> achievementsToCheck{
			Achievement("ARKANA",		0,		arkana),
			Achievement("DOLL",			0,		doll),
			Achievement("ENIGMA",		0,		k069),
		};
		for (auto& achievement : achievementsToCheck) {
			bool success = steamUserStats->GetAchievement(achievement.id.c_str(), &achievement.achieved);
			if (!achievement.achieved && achievement.conditionsFulfilled()) {
				steamUserStats->SetAchievement(achievement.id.c_str());
			}
		}
		return true;
	}

	// double ringout
	bool FK_StatManager::manageDoubleRingoutAchievement()
	{
		if (!steamUserStats) {
			return false;
		}
		Achievement achievement("DOUBLE_RINGOUT", 0, 1);
		bool success = false;
		steamUserStats->GetAchievement(achievement.id.c_str(), &achievement.achieved);
		if (!achievement.achieved && achievement.conditionsFulfilled()) {
			steamUserStats->SetAchievement(achievement.id.c_str());
			success = steamUserStats->StoreStats();
		}
		return success;
	}

	// story
	bool FK_StatManager::checkForTrialAchievements(std::vector<std::string> trialsCompleted) {
		if (!steamUserStats) {
			return false;
		}
		int tutorial = 0;
		int trials = 0;
		tutorial = (int)(std::find(trialsCompleted.begin(), trialsCompleted.end(), "tutorial_advanced2\\") != trialsCompleted.end());
		bool allTrials = true;
		std::vector<std::string> trialsToComplete{
			"subboss_doll1\\",
			"johnson1\\",
			"subboss_k069_1\\",
			"tutorialman1\\",
			"guestMira1\\",
			"guestLazor1\\",
			"evilobster1\\",
			"donner1\\",
			"tiger1\\",
			"shaz1\\",
			"renzo1\\",
			"wally1\\",
			"cyphr1\\",
			"jenn1\\",
			"mono1\\",
			"skeleton1\\",
			"graf1\\",
			"lejl1\\",
			"krave1\\",
			"elena1\\",
			"kiyoko1\\",
			"amy1\\",
		};
		for (auto trial : trialsToComplete) {
			allTrials &= std::find(trialsCompleted.begin(), trialsCompleted.end(), trial) != trialsCompleted.end();
		}
		trials = (int)allTrials;
		std::vector<Achievement> achievementsToCheck{
			Achievement("TUTORIAL",		0,		tutorial),
			Achievement("TRIALS",		0,		trials),
		};
		for (auto& achievement : achievementsToCheck) {
			bool success = steamUserStats->GetAchievement(achievement.id.c_str(), &achievement.achieved);
			if (!achievement.achieved && achievement.conditionsFulfilled()) {
				steamUserStats->SetAchievement(achievement.id.c_str());
			}
		}
		return true;
	}
}
#else
namespace fk_engine {
	FK_StatManager::FK_StatManager()
	{
		numberOfUsedBulletCounters = 0;
		numberOfTriggerCombos = 0;
		numberOfTriggerGuards = 0;
		numberOfTriggerCancels = 0;
		maxComboDamage = 0;
		maxComboLength = 0;
		totalDamageDealt = 0;
	}
	bool FK_StatManager::retrieveStatsFromServer()
	{
		return false;
	}
	bool FK_StatManager::uploadStatsToServer()
	{
		return false;
	}
	bool FK_StatManager::checkForAchievements()
	{
		return false;
	}
	bool FK_StatManager::checkForArcadeAchievements(FK_Scene::FK_SceneArcadeType)
	{
		return false;
	}
	bool FK_StatManager::checkForStoryAchievements(std::vector<std::string>)
	{
		return false;
	}
	bool FK_StatManager::checkForTagAchievements(std::vector<std::string> unlockedTags) {
		return false;
	}
	bool FK_StatManager::checkForTrialAchievements(std::vector<std::string> trialsCompleted) {
		return false;
	}

	// double ringout
	bool FK_StatManager::manageDoubleRingoutAchievement()
	{
		return false;
	}
}
#endif
