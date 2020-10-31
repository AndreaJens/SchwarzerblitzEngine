#include "FK_SceneCharacterSelectExtra_Diorama.h"
#include "FK_Database.h"
#include "ExternalAddons.h"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace fk_engine {
	FK_SceneCharacterSelectExtra_Diorama::FK_SceneCharacterSelectExtra_Diorama() : FK_SceneCharacterSelectExtra() {

	};

	FK_Scene::FK_SceneExtraType FK_SceneCharacterSelectExtra_Diorama::getExtraSceneType() {
		return FK_Scene::FK_SceneExtraType::ExtraDiorama;
	}


	/* load character files */
	void FK_SceneCharacterSelectExtra_Diorama::loadCharacterList() {
		drawLoadingScreen(0);
		dummyCharacterDirectory = "chara_dummy\\";
		std::string characterFileName = charactersDirectory + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName.c_str());
		u32 characterId = 0;
		u32 characterIdArcadeOpponent = 0;
		std::string charaPath = std::string();
		std::vector<std::string> availableCharacterArray;
		availableCharacterArray = FK_SceneWithInput::unlockedCharacterDioramas;
		while (!characterFile.eof() && characterFile >> charaPath) {
			std::string key = charaPath;
			charaPath += "\\";
			int availabilityIdentifier;
			characterFile >> availabilityIdentifier;
			if (availabilityIdentifier == CharacterUnlockKeys::Character_AvailableFromStart ||
				availabilityIdentifier == CharacterUnlockKeys::Character_UnlockableAndShown ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_NoArcadeOpponent) {
				characterPaths.push_back(charaPath);
				characterHasAvailableEndingMap[characterId] = true;
				if (unlockAllFlag || std::find(
					availableCharacterArray.begin(),
					availableCharacterArray.end(), key) !=
					availableCharacterArray.end()) {
					availableCharacters.push_back(characterId);
				}
				characterId += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_Hidden_NoArcadeOpponent ||
				availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_Hidden) {
				if (unlockAllFlag || std::find(
					availableCharacterArray.begin(),
					availableCharacterArray.end(), key) !=
					availableCharacterArray.end()) {
					availableCharacters.push_back(characterId);
					characterPaths.push_back(charaPath);
					characterHasAvailableEndingMap[characterId] = true;
					characterId += 1;
				}
			}
		};
		characterFile.close();
		characterHasAvailableEndingMap[characterId] = true;
		availableCharacters.push_back(characterId);
		finalizeCharacterList();
		/*f32 totalProgress = (f32)characterPaths.size() + 1;
		f32 currentProgress = 0;
		for each(std::string chara_path in characterPaths) {
			std::string temp;
			loadSingleCharacterFile(charactersDirectory + chara_path, temp);
			std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
			characterNames.push_back(wtemp);
			currentProgress += 1.f;
			drawLoadingScreen(100.f * currentProgress / totalProgress);
		}
		std::string temp;
		loadSingleCharacterFile(charactersDirectory + dummyCharacterDirectory, temp);
		std::wstring wtemp = fk_addons::convertNameToNonASCIIWstring(temp);
		characterNames.push_back(wtemp);
		drawLoadingScreen(100.f);*/
	};
}