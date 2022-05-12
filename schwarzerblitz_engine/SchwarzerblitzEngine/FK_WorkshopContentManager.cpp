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
