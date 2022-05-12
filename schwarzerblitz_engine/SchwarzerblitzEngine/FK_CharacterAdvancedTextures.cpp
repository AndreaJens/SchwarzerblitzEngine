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

#include"FK_Character.h"
#include<algorithm>
#include<iostream>

using namespace irr;

namespace fk_engine{

	bool FK_Character::FK_CharacterTexture::canBeSwitched(FK_Character* parentCharacter){
		if (alreadySwitched){
			return false;
		}
		else if (switchConditions.empty()){
			return true;
		}
		else{
			bool canBeSwitched = true;
			for each(FK_Character::FK_CharacterTexture::SwitchCondition condition in switchConditions){
				switch (condition.type){
				case FK_Character::FK_CharacterTexture::SwitchConditionType::OnStartup:
					canBeSwitched = true;
					break;
				case FK_Character::FK_CharacterTexture::SwitchConditionType::ObjectBroken:
					if (parentCharacter->getObjectByName(condition.stringParam) != NULL &&
						!parentCharacter->getObjectByName(condition.stringParam)->broken){
						canBeSwitched = false;
					}
					break;
				case FK_Character::FK_CharacterTexture::SwitchConditionType::LifeLesserThanPercent:
					if (parentCharacter->getLifePercentage() >= condition.numericParam){
						canBeSwitched = false;
					}
					break;
				case FK_Character::FK_CharacterTexture::SwitchConditionType::ObjectInactive:
					if (parentCharacter->getObjectByName(condition.stringParam) != NULL &&
						parentCharacter->getObjectByName(condition.stringParam)->isActive()){
						canBeSwitched = false;
					}
					break;
				default:
					canBeSwitched = false;
					break;
				}
				if (!canBeSwitched){
					return false;
				}
			}
			return canBeSwitched;
		}
		return true;
	}

	// FK_CHARACTER
	void FK_Character::checkTextureSwitches(){
		if (animatedMesh != NULL){
			u32 size = characterTextures.size();
			for (u32 i = 0; i < size; ++i){
				if (characterTextures[i].canBeSwitched(this)){
					std::string texturePath = characterTextures[i].texturePath + characterTextures[i].textureFileName;
					video::ITexture* tempTex = smgr_reference->getVideoDriver()->getTexture(texturePath.c_str());
					animatedMesh->getMaterial(characterTextures[i].materialId).setTexture(0, tempTex);
					characterTextures[i].alreadySwitched = true;
				}
			}
		}
	}
	void FK_Character::resetStartupTextures(){
		if (animatedMesh != NULL){
			u32 size = characterTextures.size();
			for (u32 i = 0; i < size; ++i){
				characterTextures[i].alreadySwitched = false;
			}
		}
		applyStartupTextures();
	}

	void FK_Character::loadTextureVariablesAndTextures(){
		readTextureConfigurationFile();
		loadAllTextures();
	}

	void FK_Character::readTextureConfigurationFile(){
		std::string TextureBeginKey = "#texture";
		std::string TextureEndKey = "#texture_end";
		std::string SwitchConditionBeginKey = "#switch_conditions";
		std::string SwitchConditionsEndKey = "#switch_conditions_end";
		std::string MaterialIdKey = "#material_id";
		std::string TextureFilenameKey = "#filename";

		std::string SwitchConditionTypeBrokenObjectKey = "##object_broken";
		std::string SwitchConditionTypeInactiveObjectKey = "##object_inactive";
		std::string SwitchConditionTypeLifeBelowPercentKey = "##life_below_percentage";

		std::string configFile = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory + "textures.txt";
		std::string resourcePath = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory;
		std::ifstream configurationFile(configFile.c_str());
		if (!configurationFile){
			return;
		}
		FK_CharacterTexture newTexture;
		std::string temp;
		while (configurationFile >> temp){
			if (temp == TextureBeginKey){
				newTexture = FK_Character::FK_CharacterTexture();
				newTexture.texturePath = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory;
			}
			else if (temp == TextureEndKey){
				characterTextures.push_back(newTexture);
			}
			else if (temp == TextureFilenameKey){
				configurationFile >> temp;
				newTexture.textureFileName = temp;
			}
			else if (temp == MaterialIdKey){
				u32 value;
				configurationFile >> value;
				newTexture.materialId = value;
			}
			else if (temp == SwitchConditionBeginKey){
				while (configurationFile >> temp){
					if (temp == SwitchConditionsEndKey){
						break;
					}
					else if (temp == SwitchConditionTypeBrokenObjectKey){
						configurationFile >> temp;
						FK_Character::FK_CharacterTexture::SwitchCondition condition;
						condition.type = FK_Character::FK_CharacterTexture::SwitchConditionType::ObjectBroken;
						condition.stringParam = temp;
						newTexture.switchConditions.push_back(condition);
					}
					else if (temp == SwitchConditionTypeInactiveObjectKey){
						configurationFile >> temp;
						FK_Character::FK_CharacterTexture::SwitchCondition condition;
						condition.type = FK_Character::FK_CharacterTexture::SwitchConditionType::ObjectInactive;
						condition.stringParam = temp;
						newTexture.switchConditions.push_back(condition);
					}
					else if (temp == SwitchConditionTypeLifeBelowPercentKey){
						f32 value;
						configurationFile >> value;
						FK_Character::FK_CharacterTexture::SwitchCondition condition;
						condition.type = FK_Character::FK_CharacterTexture::SwitchConditionType::LifeLesserThanPercent;
						condition.numericParam = value;
						newTexture.switchConditions.push_back(condition);
					}
				}
			}
		}
	}

	void FK_Character::applyStartupTextures(){
		if (animatedMesh != NULL){
			u32 size = characterTextures.size();
			for (u32 i = 0; i < size; ++i){
				if (characterTextures[i].switchConditions.empty()){
					if (characterTextures[i].materialId < (s32)animatedMesh->getMaterialCount()){
						std::string texturePath = characterTextures[i].texturePath + characterTextures[i].textureFileName;
						video::ITexture* tempTex = smgr_reference->getVideoDriver()->getTexture(texturePath.c_str());
						animatedMesh->getMaterial(characterTextures[i].materialId).setTexture(0, tempTex);
					}
					characterTextures[i].alreadySwitched = true;
				}
			}
		}
	}
	void FK_Character::loadAllTextures(){
		// load textures in the cache to switch them more effectively during game
		u32 size = characterTextures.size();
		for (u32 i = 0; i < size; ++i){
			std::string texturePath = characterTextures[i].texturePath + characterTextures[i].textureFileName;
			video::ITexture* tempTex = smgr_reference->getVideoDriver()->getTexture(texturePath.c_str());
		}
	}
}