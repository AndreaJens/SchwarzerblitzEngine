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

#include "FK_SceneCharacterSelectExtra_ArcadeEnding.h"
#include "FK_Database.h"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace fk_engine {

	FK_SceneCharacterSelectExtra_ArcadeEnding::FK_SceneCharacterSelectExtra_ArcadeEnding() : FK_SceneCharacterSelectExtra() {

	};

	FK_Scene::FK_SceneExtraType FK_SceneCharacterSelectExtra_ArcadeEnding::getExtraSceneType() {
		return FK_Scene::FK_SceneExtraType::ExtraArcadeEndingGallery;
	}

	/* get cutscene if it is unlocked */
	FK_StoryEvent FK_SceneCharacterSelectExtra_ArcadeEnding::getCutsceneToPlay()
	{
		FK_StoryEvent cutscene;
		auto playerOutfit = FK_SceneCharacterSelectArcade::getPlayerOutfit();
		std::string charactersPath = FK_SceneCharacterSelectArcade::charactersDirectory;
		std::string playerPath = FK_SceneCharacterSelectArcade::getPlayerPath();
		u32 extraEndingInput = player1input->getPressedButtons();
		if (!player1IsActive) {
			extraEndingInput = player2input->getPressedButtons();
		}
		extraEndingInput &= FK_Input_Buttons::Trigger_Button;
		// play extra ending if Trigger input is pressed before the scene
		bool validEndingFile = false;
		std::string endingFilePath;
		std::string relativePath;
		std::string arcadeEndingConfigFileName;
		if (extraEndingInput != 0) {
			endingFilePath =
				charactersPath + playerPath + playerOutfit.outfitDirectory +
				fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeAlternateEndingFileName;
			std::ifstream inputFile(endingFilePath.c_str());
			relativePath = playerPath + playerOutfit.outfitDirectory;
			if (!inputFile) {
				inputFile.clear();
				inputFile.close();
				endingFilePath =
					charactersPath + playerPath +
					fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeAlternateEndingFileName;
				relativePath = playerPath;
				inputFile = std::ifstream(endingFilePath.c_str());
			}
			if (inputFile.good()) {
				validEndingFile = true;
				arcadeEndingConfigFileName = fk_constants::FK_CharacterArcadeAlternateEndingFileName;
			}
		}
		if (!validEndingFile) {
			endingFilePath =
				charactersPath + playerPath + playerOutfit.outfitDirectory +
				fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeEndingFileName;
			std::ifstream inputFile(endingFilePath.c_str());
			relativePath = playerPath + playerOutfit.outfitDirectory;
			if (!inputFile) {
				inputFile.clear();
				inputFile.close();
				endingFilePath =
					charactersPath + playerPath +
					fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeEndingFileName;
				relativePath = playerPath;
				inputFile = std::ifstream(endingFilePath.c_str());
			}
			if (inputFile.good()) {
				validEndingFile = true;
				arcadeEndingConfigFileName = fk_constants::FK_CharacterArcadeEndingFileName;
			}
		}
		if (!validEndingFile) {
			return cutscene;
		}
		else {
			cutscene.setup(arcadeEndingConfigFileName,
				charactersPath + relativePath + fk_constants::FK_CharacterArcadeFolder, relativePath);
			cutscene.playCredits = false;
			cutscene.allowMenu = true;
			cutscene.saveStoryCompletion = false;
			cutscene.saveWhenSkipped = false;
			cutscene.fadeWhenEnding = true;
		}
		return cutscene;
	}

	// draw screen
	void FK_SceneCharacterSelectExtra_ArcadeEnding::drawCharacterScreen()
	{
		int size = characterIcons.size();
		// all character icons must have the same width and height
		core::dimension2d<u32> texSize = characterIcons[0]->getSize();
		s32 width = texSize.Width;
		s32 height = texSize.Height;
		for (int i = 0; i < size; i++) {
			core::rect<s32> destinationRect = getCharacterIconPosition(i);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, width, height);
			bool availabilityFlag = isCharacterAvailable(i);
			if (characterHasAvailableEndingMap[i]) {
				driver->draw2DImage(availabilityFlag ? characterIcons[i] : missingPreview, destinationRect, sourceRect);
			}
			else {
				const irr::video::SColor colors[4] = {
					irr::video::SColor(255, 128, 128, 128),
					irr::video::SColor(255, 128, 128, 128),
					irr::video::SColor(255, 128, 128, 128),
					irr::video::SColor(255, 128, 128, 128),
				};
				driver->draw2DImage(missingPreview, destinationRect, sourceRect, 0, colors);
			}
		}
		//drawNoEndingDisclaimer();
	}

	/* check if extra ending is available to play */
	bool FK_SceneCharacterSelectExtra_ArcadeEnding::checkIfExtraEndingIsAvailable(std::string& characterPath) {
		std::string charactersPath = FK_SceneCharacterSelectArcade::charactersDirectory;
		std::string endingFilePath =
			charactersPath + characterPath +
			fk_constants::FK_CharacterArcadeFolder + fk_constants::FK_CharacterArcadeAlternateEndingFileName;
		std::ifstream inputFile(endingFilePath.c_str());
		if (!inputFile) {
			inputFile.clear();
			inputFile.close();
			return false;
		}
		return true;
	}

	/* load single character outfit list */
	void FK_SceneCharacterSelectExtra_ArcadeEnding::loadSingleCharacterOutfitList(
		u32 characterId, FK_Character* character) {
		bool isRandomSelection = false;
		u32 selectionId = 0;
		if (availableEndingsIndexMap.find(characterId) == availableEndingsIndexMap.end()) {
			isRandomSelection = true;
			selectionId = characterPaths.size();
		}
		selectionId = availableEndingsIndexMap[characterId];
		u32 maxNumberOfOutfits = character->getNumberOfOutfits();
		for (u32 i = 0; i < maxNumberOfOutfits; ++i) {
			FK_Character::FK_CharacterOutfit outfit = character->getOutfit(i);
			std::string charaPath;
			if (isRandomSelection) {
				charaPath = dummyCharacterDirectory;
			}
			else {
				charaPath = characterPaths[selectionId];
			} 
			std::string key = charaPath + outfit.outfitDirectory;
			if (i == 0) {
				bool unlockedEnding = std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), charaPath) !=
					storyEpisodesCleared.end();
				unlockedEnding |= unlockAllFlag;
				unlockedEnding |= isRandomSelection;
				characterAvailableCostumes[characterId].push_back(std::pair<u32, bool>(i, unlockedEnding));
			} else if (checkIfEndingIsAvailable(key)) {
				bool unlockedEnding = std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), key) !=
					storyEpisodesCleared.end();
				unlockedEnding |= unlockAllFlag;
				characterAvailableCostumes[characterId].push_back(std::pair<u32, bool>(i, unlockedEnding));
			}
		}
	}

	/* load character files */
	void FK_SceneCharacterSelectExtra_ArcadeEnding::loadCharacterList() {
		availableEndingsIndexMap.clear();
		characterHasAvailableEndingMap.clear();
		u32 availabilityIndex = 0;
		drawLoadingScreen(0);
		dummyCharacterDirectory = "chara_dummy";
		if (!checkIfEndingIsAvailable(dummyCharacterDirectory + "\\")) {
			if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), dummyCharacterDirectory) != unlockedCharacters.end()) {
				unlockedCharacters.erase(
					std::find(unlockedCharacters.begin(), unlockedCharacters.end(), dummyCharacterDirectory)
				);
			}
		}
		dummyCharacterDirectory += "\\";
		std::string characterFileName = charactersDirectory + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName.c_str());
		u32 characterId = 0;
		u32 characterIdArcadeOpponent = 0;
		std::string charaPath = std::string();
		std::vector<std::string> availableCharacterArray;

		for each (auto unlockable in storyEpisodesCleared) {
			u32 nameIndex = std::find(unlockable.begin(), unlockable.end(), '\\') - unlockable.begin();
			std::string charaName = unlockable.substr(0, nameIndex);
			if (!charaName.empty()) {
				availableCharacterArray.push_back(charaName);
			}
		}

		while (!characterFile.eof() && characterFile >> charaPath) {
			std::string key = charaPath;
			charaPath += "\\";
			int availabilityIdentifier;
			characterFile >> availabilityIdentifier;
			if (availabilityIdentifier == CharacterUnlockKeys::Character_AvailableFromStart ||
				availabilityIdentifier == CharacterUnlockKeys::Character_UnlockableAndShown ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_NoArcadeOpponent) {
				characterHasAvailableEndingMap[availabilityIndex] = checkIfEndingIsAvailable(charaPath);
				//if (!checkIfEndingIsAvailable(charaPath)) {
				//	continue;
				//}
				characterPaths.push_back(charaPath);
				availableEndingsIndexMap[characterId] = availabilityIndex;
				if ((unlockAllFlag && characterHasAvailableEndingMap[availabilityIndex]) || std::find(
					availableCharacterArray.begin(),
					availableCharacterArray.end(), key) !=
					availableCharacterArray.end()) {
					availableCharacters.push_back(characterId);
				}
				availabilityIndex += 1;
				characterId += 1;

			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_Hidden_NoArcadeOpponent) {
				characterHasAvailableEndingMap[availabilityIndex] = checkIfEndingIsAvailable(charaPath);
				//if (!checkIfEndingIsAvailable(charaPath)) {
				//	continue;
				//}
				if ((unlockAllFlag && characterHasAvailableEndingMap[availabilityIndex]) || std::find(
					availableCharacterArray.begin(),
					availableCharacterArray.end(), key) !=
					availableCharacterArray.end()) {
					availableCharacters.push_back(characterId);
					characterPaths.push_back(charaPath);
					availableEndingsIndexMap[characterId] = availabilityIndex;
					availabilityIndex += 1;
					characterId += 1;
				}
			}
		};
		characterFile.close();
		availableCharacters.push_back(characterId);
		characterHasAvailableEndingMap[availabilityIndex] = true;
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
		finalizeCharacterList();
	};
}