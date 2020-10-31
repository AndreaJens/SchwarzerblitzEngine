#pragma once
#include"FK_Scene.h"
#include<string>
#include<vector>

// we don't use this class if this build is not intended for Steam
namespace fk_engine {

	class FK_StatManager {
	private:
		struct Achievement {
			std::string id;
			bool achieved = false;
			int threshold = 0;
			int value = 0;
			Achievement() {
				achieved = false;
				threshold = 0;
				value = 0;
			}
			Achievement(std::string newId, int newThreshold, int newValue) {
				id = newId;
				threshold = newThreshold;
				value = newValue;
			}
			bool conditionsFulfilled() {
				return (value > threshold);
			}
		};
	public:
		FK_StatManager();
		bool retrieveStatsFromServer();
		bool uploadStatsToServer();
		bool checkForAchievements();
		bool checkForArcadeAchievements(FK_Scene::FK_SceneArcadeType sceneType);
		bool checkForStoryAchievements(std::vector<std::string> storyEpisodesCleared);
		bool checkForTrialAchievements(std::vector<std::string> trialsCompleted);
		bool checkForTagAchievements(std::vector<std::string> unlockedTags);
		bool manageDoubleRingoutAchievement();
	public:
		int numberOfUsedBulletCounters;
		int numberOfTriggerCombos;
		int numberOfTriggerGuards;
		int numberOfTriggerCancels;
		int maxComboDamage;
		int totalDamageDealt;
		int maxComboLength;
	};
}