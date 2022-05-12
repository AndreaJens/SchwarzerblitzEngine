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