#include "FK_WorkshopContentManager.h"
#include<map>
#include<fstream>
#include<sstream>

namespace fk_engine {
	FK_WorkshopContentManager::FK_WorkshopContentManager()
	{
		clear();
	}

	bool FK_WorkshopContentManager::loadFromFile(std::string filename)
	{
		clear();
		std::map<std::string, WorkshopItemType> itemKeyToType{
			{"character",		WorkshopItemType::Character},
			{"stage",			WorkshopItemType::Stage},
			{"story_episode",	WorkshopItemType::StoryEpisode},
			{"trial_challenge", WorkshopItemType::TrialChallenge}
		};
		std::ifstream workshopFile(filename);
		if (!workshopFile.good()) {
			return false;
		}
		std::string line;
		while (std::getline(workshopFile, line)) {
			std::string temp;
			std::istringstream lineStream(line);
			WorkshopItem newItem;
			uint32_t fieldCounter = 0;
			while (lineStream) {
				std::getline(lineStream, temp, ';');
				switch(fieldCounter) {
				case 0:
					try {
						newItem.uniqueId = std::stoi(temp);
					}
					catch (...) {
						newItem.uniqueId = 0;
					}
					break;
				case 1:
					if (itemKeyToType.find(temp) != itemKeyToType.end()) {
						newItem.type = itemKeyToType[temp];
					}
					break;
				case 2:
					newItem.path = temp + "\\";
					break;
				case 3:
					try {
						newItem.enabled = std::stoi(temp) > 0;
					}
					catch (...) {
						newItem.enabled = false;
					}
					break;
				case 4:
					newItem.name = temp;
					break;
				default:
					break;
				}
				fieldCounter += 1;
			}
			if (newItem.type != WorkshopItemType::None) {
				allItems.push_back(newItem);
				if (newItem.enabled) {
					enabledItems.push_back(newItem);
				}
			}
		}
		return true;
	}
	std::vector<FK_WorkshopContentManager::WorkshopItem> FK_WorkshopContentManager::getEnabledItems()
	{
		return enabledItems;
	}
	void FK_WorkshopContentManager::clear()
	{
		allItems.clear();
		enabledItems.clear();
	}
	bool FK_WorkshopContentManager::loadFromSteam()
	{
#ifdef STEAM_SUPPORT
		if (!SteamUGC()) {
			return false;
		}
		uint32_t numOfSubscribedItems = SteamUGC()->GetNumSubscribedItems();
		PublishedFileId_t* subscribedItems = new PublishedFileId_t[numOfSubscribedItems];
		SteamUGC()->GetSubscribedItems(subscribedItems, numOfSubscribedItems);
		for (uint32_t i = 0; i < numOfSubscribedItems; ++i) {
			uint64 punSizeOnDisk = 0;
			uint32 cchFolderSize = _MAX_PATH;
			char* pchFolder = new char[cchFolderSize];
			uint32 punTimeStamp = 0;
			SteamUGC()->GetItemInstallInfo(subscribedItems[i], &punSizeOnDisk, pchFolder, cchFolderSize, &punTimeStamp);
			WorkshopItem newItem;
			newItem.uniqueId = subscribedItems[i];
			newItem.path = std::string(pchFolder);
			newItem.enabled = true;
			bool successfulLoad = parseWorkshopItemConfigFile(newItem);
			if (successfulLoad) {
				allItems.push_back(newItem);
				if (newItem.enabled) {
					enabledItems.push_back(newItem);
				}
			}
			delete[] pchFolder;
		}
		return true;
#endif
		return false;
	}

	bool FK_WorkshopContentManager::parseWorkshopItemConfigFile(WorkshopItem& newItem)
	{
		std::map<std::string, WorkshopItemType> itemKeyToType{
			{"character",		WorkshopItemType::Character},
			{"stage",			WorkshopItemType::Stage},
			{"episode",			WorkshopItemType::StoryEpisode},
			{"costume",			WorkshopItemType::Costume},
			{"challenge", WorkshopItemType::TrialChallenge}
		};
		std::ifstream workshopFile(newItem.path + "\\workshop.ini");
		if (!workshopFile) {
			return false;
		}
		std::string temp;
		while (workshopFile >> temp) {
			if (temp == "#TYPE") {
				workshopFile.ignore();
				std::getline(workshopFile, temp);
				if (itemKeyToType.find(temp) != itemKeyToType.end()) {
					newItem.type = itemKeyToType[temp];
				}
				else {
					return false;
				}
			}
		}
	}
}
