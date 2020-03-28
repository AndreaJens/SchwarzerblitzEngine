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