#pragma once
#include<string>
#include<vector>
#include<cstdint>

namespace fk_engine {
	class FK_WorkshopContentManager {
	public:
		enum class  WorkshopItemType : uint32_t{
			None = 0,
			Character = 1,
			Stage = 2,
			StoryEpisode = 3,
			TrialChallenge = 4,
			Costume = 5,
		};
		struct WorkshopItem {
			uint32_t uniqueId;
			std::string path;
			bool enabled;
			std::string name;
			WorkshopItemType type;
			WorkshopItem() {
				uniqueId = 0;
				type = WorkshopItemType::None;
				enabled = false;
			}
		};
	public:
		FK_WorkshopContentManager();
		bool loadFromSteam();
		bool loadFromFile(std::string filename);
		std::vector<WorkshopItem> getEnabledItems();
	private:
		void clear();
		bool parseWorkshopItemConfigFile(WorkshopItem& newItem);
	private:
		std::vector<WorkshopItem> allItems;
		std::vector<WorkshopItem> enabledItems;
	};
}