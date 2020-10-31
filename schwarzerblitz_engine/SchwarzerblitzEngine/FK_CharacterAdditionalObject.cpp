#include"FK_Character.h"
#include<algorithm>
#include<iostream>

using namespace irr;

namespace fk_engine{

	/* additional object methods */
	void FK_Character::FK_CharacterAdditionalObject::resetFlags(){
		broken = false;
		endurance = maxEndurance;
		lightRemoved = false;
		visible = defaultVisibility;
		numberOfUsages = 0;
		lifetimeTimer = 0;
	};

	void FK_Character::FK_CharacterAdditionalObject::setLightAsRemoved(){
		lightRemoved = true;
	}

	bool FK_Character::FK_CharacterAdditionalObject::isActive(){
		bool standardFlag = visible && !broken;
		//bool lifetimeFlag = !expiresWithTime || lifetimeTimer <= maxLifetime;
		//bool usageFlag = !useable || numberOfUsages < maxNumberOfUsages;
		return standardFlag;// && lifetimeFlag && usageFlag;
	}

	bool FK_Character::FK_CharacterAdditionalObject::isActive() const{
		bool standardFlag = visible && !broken;
		//bool lifetimeFlag = !expiresWithTime || lifetimeTimer <= maxLifetime;
		//bool usageFlag = !useable || numberOfUsages < maxNumberOfUsages;
		return standardFlag;// && lifetimeFlag && usageFlag;
	}

	bool FK_Character::FK_CharacterAdditionalObject::lifetimeHasExpired() {
		return expiresWithTime && lifetimeTimer > maxLifetime;
	}

	bool FK_Character::FK_CharacterAdditionalObject::maximumUsesReached() {
		return useable && numberOfUsages >= maxNumberOfUsages;
	}

	bool FK_Character::FK_CharacterAdditionalObject::isBroken() {
		return broken;
	}

	bool FK_Character::FK_CharacterAdditionalObject::isBroken() const {
		return broken;
	}

	bool FK_Character::FK_CharacterAdditionalObject::hasDummyMesh(){
		return meshFilename.empty();
	}

	bool FK_Character::FK_CharacterAdditionalObject::hasMoveToPerformOnBreak() {
		return !moveToPerformOnBreak.empty();
	}

	bool FK_Character::FK_CharacterAdditionalObject::hasDummyMesh() const{
		return meshFilename.empty();
	}

	void FK_Character::FK_CharacterAdditionalObject::toggleVisibility(bool newVisibilityFlag){
		if (visible != newVisibilityFlag){
			visible = newVisibilityFlag;
			needsRefreshFlag = visible;
		}
	}

	/* check if object can be removed in any way */
	bool FK_Character::FK_CharacterAdditionalObject::canBeRemoved() const{
		if (defaultVisibility && (breakable || canBeBrokenByMove || canBeToggledByMove || 
			canBeBrokenByObject || canBeToggledByObject || hideWhenHit || expiresWithTime)){
			return true;
		}
		if ((!defaultVisibility && (showWhenHit || canBeToggledByMove || canBeToggledByObject)) && 
			(breakable || canBeBrokenByMove || canBeBrokenByObject || expiresWithTime)){
			return true;
		}
		return false;
	}

	/* check if object can be activated in any way */
	bool FK_Character::FK_CharacterAdditionalObject::canBeActivated() const{
		if (defaultVisibility || showWhenHit){
			return true;
		}
		if (!defaultVisibility && (canBeToggledByMove || canBeToggledByObject)){
			return true;
		}
		return false;
	}

	/* check if object can be activated in any way */
	bool FK_Character::FK_CharacterAdditionalObject::canBeShown() const {
		if (!defaultVisibility && (canBeToggledByMove || canBeToggledByObject || showWhenHit)) {
			return true;
		}
		return false;
	}
	
	bool FK_Character::FK_CharacterAdditionalObject::needsRefresh(){
		return !hasDummyMesh() && needsRefreshFlag;
	}

	bool FK_Character::FK_CharacterAdditionalObject::needsRefresh() const{
		return !meshFilename.empty() && needsRefreshFlag;
	}

	void FK_Character::FK_CharacterAdditionalObject::markAsRefreshed(){
		needsRefreshFlag = false;
	}

	bool FK_Character::FK_CharacterAdditionalObject::checkIfBroken(FK_Reaction_Type& reactionToCheck){
		if (!breakable || broken){
			return false;
		}
		if (((u32)reactionToCheck & reactionAbleToBreakObject) == 0) {
			return false;
		}
		f32 maxBreakingLikelihood = maximumBreakingLikelihood;
		// object with negative likelihood break at first chance to do so (all endurance depleted)
		if (maxBreakingLikelihood < 0){
			if (endurance <= 0){
				broken = true;
				return true;
			}
			else{
				return false;
			}
		}
		f32 breakingLikelihood = maxBreakingLikelihood * (1 - endurance / maxEndurance);
		f32 random = 100.0f * ((f32)rand() / (RAND_MAX));
		if (random < breakingLikelihood){
			broken = true;
		}
		return broken;
	}

	void FK_Character::FK_CharacterAdditionalObject::applyDamage(f32 damage){
		endurance -= damage;
		if (endurance < 0){
			endurance = 0;
		}
	}

	bool FK_Character::FK_CharacterAdditionalObject::canBeDamaged(FK_Character* parentCharacter){
		if (damageConditions.empty()){
			return true;
		}
		else{
			bool canBeDamaged = true;
			for each(FK_Character::FK_CharacterAdditionalObject::DamageCondition condition in damageConditions){
				switch (condition.type){
				case FK_Character::FK_CharacterAdditionalObject::DamageConditionType::ObjectBroken:
					if (parentCharacter->getObjectByName(condition.stringParam) != NULL &&
						!parentCharacter->getObjectByName(condition.stringParam)->broken){
						canBeDamaged = false;
					}
					break;
				case FK_Character::FK_CharacterAdditionalObject::DamageConditionType::ObjectInactive:
					if (parentCharacter->getObjectByName(condition.stringParam) != NULL &&
						parentCharacter->getObjectByName(condition.stringParam)->isActive()){
						canBeDamaged = false;
					}
					break;
				default:
					canBeDamaged &= true;
					break;
				}
				if (!canBeDamaged){
					return false;
				}
			}
			return canBeDamaged;
		}
		return true;
	}

	void FK_Character::removeLightFromObject(int objectId){
		if (objectId < (s32)characterAdditionalObjects.size()){
			characterAdditionalObjects[objectId].setLightAsRemoved();
		}
	}

	void FK_Character::refreshObject(int objectId){
		if (objectId < (s32)characterAdditionalObjects.size()){
			characterAdditionalObjects[objectId].lightRemoved = false;
			characterAdditionalObjects[objectId].markAsRefreshed();
			//if (!characterAdditionalObjects[objectId].hasDummyMesh()) {
			//	characterAdditionalObjectsMeshs[objectId]->setTransitionTime(animationTransitionTime);
			//}
		}
	}

	/* additional objects update */
	void FK_Character::updateAdditionalObjects(f32 delta_t_s, bool forcePositionUpdate){
		u32 size = characterAdditionalObjects.size();
		for (u32 i = 0; i < size; ++i){
			// lifetime (not during moves)
			if (characterAdditionalObjects[i].expiresWithTime && characterAdditionalObjects[i].isActive()) {
				characterAdditionalObjects[i].lifetimeTimer += delta_t_s;
				if ((characterAdditionalObjects[i].expiresDuringMove || currentMove == NULL) && 
					characterAdditionalObjects[i].lifetimeHasExpired()) {
					switch (characterAdditionalObjects[i].expirationAction) {
					case FK_CharacterAdditionalObject::AfterUseAction::Hide:
						characterAdditionalObjects[i].toggleVisibility(false);
						break;
					case FK_CharacterAdditionalObject::AfterUseAction::Break:
						characterAdditionalObjects[i].broken = true;
						if (!characterAdditionalObjects[i].hasDummyMesh()) {
							characterAdditionalObjects[i].lightRemoved = false;
						}
						checkForItemsToggledByBreak(i);
						break;
					}
				}
			}
			// number of usages
			if (characterAdditionalObjects[i].maximumUsesReached() && characterAdditionalObjects[i].isActive()) {
				switch (characterAdditionalObjects[i].afterUsageAction) {
				case FK_CharacterAdditionalObject::AfterUseAction::Hide:
					characterAdditionalObjects[i].toggleVisibility(false);
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Break:
					characterAdditionalObjects[i].broken = true;
					if (!characterAdditionalObjects[i].hasDummyMesh()) {
						characterAdditionalObjects[i].lightRemoved = false;
					}
					checkForItemsToggledByBreak(i);
					break;
				}
			}
			// stop update if dummy mesh
			if (characterAdditionalObjects[i].hasDummyMesh()){
				continue;
			}
			/*if (characterAdditionalObjects[i].isActive()){
				characterAdditionalObjectsMeshs[i]->animateJoints();
			}*/
			bool refreshPosition = animatedMesh->isVisible() &&
				!characterAdditionalObjectsMeshs[i]->isVisible() &&
				characterAdditionalObjects[i].isActive() &&
				characterAdditionalObjects[i].parent == FK_Bones_Type::NoArmature;
			if (refreshPosition) {
				core::vector3df offset(0.f, 0.f, 0.f);
				core::vector3df direction;
				core::vector3df baseOffset = characterAdditionalObjects[i].positionOffset;
				if (!baseOffset.equals(core::vector3df(0.f, 0.f, 0.f))) {
					f32 newAngle = animatedMesh->getRotation().Y;
					direction.X = sin(newAngle*core::DEGTORAD);
					direction.Z = cos(newAngle*core::DEGTORAD);
					offset.X = baseOffset.Z * direction.X;
					offset.Z = baseOffset.Z * direction.Z;
					// Perform side movement (notice that buttons are arranged such that UP and DOWN keep on working in the same way)
					core::vector3df normal_direction;
					normal_direction.X = direction.Z;
					normal_direction.Z = -direction.X;
					//if (!isLeftPlayer) {
					//	normal_direction *= -1;
					//}
					offset.X += baseOffset.X * normal_direction.X;
					offset.Z += baseOffset.X * normal_direction.Z;
				}
				characterAdditionalObjectsMeshs[i]->setPosition(animatedMesh->getAbsolutePosition() + offset);
				core::vector3df rotation = characterAdditionalObjects[i].rotationAngle;
				characterAdditionalObjectsMeshs[i]->setRotation(animatedMesh->getRotation() + rotation);
			}
			if (forcePositionUpdate){
				if (characterAdditionalObjects[i].isActive()){
					characterAdditionalObjectsMeshs[i]->updateAbsolutePosition();
				}
			}
			characterAdditionalObjectsMeshs[i]->setVisible(animatedMesh->isVisible());
			if (!characterAdditionalObjects[i].isActive()){
				if (characterAdditionalObjectsMeshs[i]->isVisible()) {
					characterAdditionalObjectsMeshs[i]->setVisible(false);
					characterAdditionalObjectsMeshs[i]->setCurrentFrame(0, true);
					characterAdditionalObjectsMeshs[i]->OnRegisterSceneNode();
				}
			}
			else{
				characterAdditionalObjectsMeshs[i]->animateJoints();
			}
		}
		u32 frame = getCurrentFrame();
		std::string itemToPick = getPickableObjectId();
		if (itemToPick != std::string() && currentMove != NULL && currentMove->getPickableItemId() == itemToPick){
			if (currentMove->canPickUpItemAtFrame(frame, itemToPick) && !isPickingUpObject() && 
				namedAdditionalObjectsReferences[itemToPick]->isBroken()){
				isPickingObjectFlag = true;
				namedAdditionalObjectsReferences[itemToPick]->visible = true;
				namedAdditionalObjectsReferences[itemToPick]->broken = false;
				namedAdditionalObjectsReferences[itemToPick]->lightRemoved = false;
				namedAdditionalObjectsReferences[itemToPick]->endurance =
					namedAdditionalObjectsReferences[itemToPick]->maxEndurance;
				namedAdditionalObjectsReferences[itemToPick]->needsRefreshFlag = true;
				//std::cout << "setting PICK UP flag" << std::endl;
			}
		}
		if ((s32)frame != lastFrameForObjectUpdate){
			lastFrameForObjectUpdate = (s32)frame;
			if (currentMove != NULL && (currentMove->canToggleItemsAtFrame(frame))){
				for each(std::pair<std::string, s32> itemPair in 
					currentMove->getToggledItemsAtFrame(frame)){
					std::string itemId = itemPair.first;
					if (namedAdditionalObjectsReferences.count(itemId) > 0){
						if (itemPair.second == FK_Move::FK_ObjectToggleFlag::Toggle){
							if (!namedAdditionalObjectsReferences[itemId]->broken){
								namedAdditionalObjectsReferences[itemId]->toggleVisibility(
									!namedAdditionalObjectsReferences[itemId]->visible);
							}
						}
						else if (itemPair.second == FK_Move::FK_ObjectToggleFlag::Show){
							if (!namedAdditionalObjectsReferences[itemId]->broken){
								namedAdditionalObjectsReferences[itemId]->toggleVisibility(true);
							}
						}
						else if (itemPair.second == FK_Move::FK_ObjectToggleFlag::Hide){
							if (!namedAdditionalObjectsReferences[itemId]->broken){
								namedAdditionalObjectsReferences[itemId]->toggleVisibility(false);
							}
						}
						else if (itemPair.second == FK_Move::FK_ObjectToggleFlag::Restore){
							namedAdditionalObjectsReferences[itemId]->resetFlags();
							namedAdditionalObjectsReferences[itemId]->toggleVisibility(true);
							namedAdditionalObjectsReferences[itemId]->needsRefreshFlag = true;
						}
						else if (itemPair.second == FK_Move::FK_ObjectToggleFlag::Break){
							if (!namedAdditionalObjectsReferences[itemId]->broken){
								namedAdditionalObjectsReferences[itemId]->broken = true;
								namedAdditionalObjectsReferences[itemId]->lightRemoved = false;
								checkForItemsToggledByBreak(itemId);
							}
							
						}
						else if (itemPair.second == FK_Move::FK_ObjectToggleFlag::Use) {
							if (!namedAdditionalObjectsReferences[itemId]->broken) {
								namedAdditionalObjectsReferences[itemId]->numberOfUsages += 1;
							}
						}
					}
				}
			}
		}
	}

	/* register additional objects */
	void FK_Character::registerAdditionalObjects(){
		for (u32 i = 0; i < characterAdditionalObjectsMeshs.size(); ++i){
			characterAdditionalObjectsMeshs[i]->OnRegisterSceneNode();
		}
	}

	void FK_Character::applyDamageToObjects(f32 damage){
		for (u32 i = 0; i < characterAdditionalObjects.size(); ++i){
			if (characterAdditionalObjects[i].breakable && characterAdditionalObjects[i].isActive()){
				if (characterAdditionalObjects[i].canBeDamaged(this)){
					characterAdditionalObjects[i].applyDamage(damage);
				}
			}
			if (characterAdditionalObjects[i].isActive()){
				if (characterAdditionalObjects[i].hideWhenHit){
					characterAdditionalObjects[i].toggleVisibility(false);
					activateParticleEffect(characterAdditionalObjectsMeshs[i]->getAbsolutePosition(), false);
				}
			}
			else{
				if (characterAdditionalObjects[i].showWhenHit){
					characterAdditionalObjects[i].toggleVisibility(true);
					if (characterAdditionalObjects[i].expiresWithTime) {
						characterAdditionalObjects[i].lifetimeTimer = 0;
					}
				}
			}
		}
	}

	void FK_Character::resetObject(u32 objectId){
		if (objectId < characterAdditionalObjects.size()){
			characterAdditionalObjects[objectId].resetFlags();
		}
	}

	void FK_Character::checkForItemsToggledByBreak(u32 objectIndex) {
		for each(std::pair<std::string, FK_CharacterAdditionalObject::AfterUseAction> objectToActOn in 
			characterAdditionalObjects[objectIndex].objectToToggleWhenBroken) {
			if (getObjectByName(objectToActOn.first) != NULL) {
				switch (objectToActOn.second) {
				case FK_CharacterAdditionalObject::AfterUseAction::Hide:
					getObjectByName(objectToActOn.first)->toggleVisibility(false);
					getObjectByName(objectToActOn.first)->lightRemoved = false;
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Show:
					getObjectByName(objectToActOn.first)->toggleVisibility(true);
					getObjectByName(objectToActOn.first)->needsRefreshFlag = true;
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Restore:
					getObjectByName(objectToActOn.first)->resetFlags();
					getObjectByName(objectToActOn.first)->toggleVisibility(true);
					getObjectByName(objectToActOn.first)->needsRefreshFlag = true;
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Break:
					if (!getObjectByName(objectToActOn.first)->broken) {
						getObjectByName(objectToActOn.first)->broken = true;
						if (!getObjectByName(objectToActOn.first)->hasDummyMesh()) {
							getObjectByName(objectToActOn.first)->lightRemoved = false;
							s32 index = getObjectByName(objectToActOn.first)->objectArrayIndex;
							activateParticleEffect(characterAdditionalObjectsMeshs[index]->getAbsolutePosition(), false);
						}
						checkForItemsToggledByBreak(objectToActOn.first);
					}
					break;
				}
			}
		}
	}

	void FK_Character::checkForItemsToggledByBreak(std::string itemId) {
		if (getObjectByName(itemId) == NULL) {
			return;
		}
		for each(std::pair<std::string, FK_CharacterAdditionalObject::AfterUseAction> objectToActOn in
			getObjectByName(itemId)->objectToToggleWhenBroken) {
			if (getObjectByName(objectToActOn.first) != NULL) {
				switch (objectToActOn.second) {
				case FK_CharacterAdditionalObject::AfterUseAction::Hide:
					getObjectByName(objectToActOn.first)->toggleVisibility(false);
					getObjectByName(objectToActOn.first)->lightRemoved = false;
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Show:
					getObjectByName(objectToActOn.first)->toggleVisibility(true);
					getObjectByName(objectToActOn.first)->needsRefreshFlag = true;
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Restore:
					getObjectByName(objectToActOn.first)->resetFlags();
					getObjectByName(objectToActOn.first)->toggleVisibility(true);
					getObjectByName(objectToActOn.first)->needsRefreshFlag = true;
					break;
				case FK_CharacterAdditionalObject::AfterUseAction::Break:
					if (!getObjectByName(objectToActOn.first)->broken) {
						getObjectByName(objectToActOn.first)->broken = true;
						if (!getObjectByName(objectToActOn.first)->hasDummyMesh()) {
							getObjectByName(objectToActOn.first)->lightRemoved = false;
							s32 index = getObjectByName(objectToActOn.first)->objectArrayIndex;
							activateParticleEffect(characterAdditionalObjectsMeshs[index]->getAbsolutePosition(), false);
						}
						checkForItemsToggledByBreak(objectToActOn.first);
					}
					break;
				}
			}
		}
	}

	void FK_Character::checkForBreakingObjects(FK_Reaction_Type& reactionToCheck){
		for (u32 i = 0; i < characterAdditionalObjects.size(); ++i){
			if (characterAdditionalObjects[i].checkIfBroken(reactionToCheck)){
				if (!characterAdditionalObjects[i].hasDummyMesh()){
					activateParticleEffect(characterAdditionalObjectsMeshs[i]->getAbsolutePosition(), false);
				}
				checkForItemsToggledByBreak(i);
				if (!isKO() && characterAdditionalObjects[i].hasMoveToPerformOnBreak()) {
					for (auto it = getMovesCollection().begin(); it != getMovesCollection().end(); ++it)
					{
						FK_Move* move = &(*it);
						if (move->getName() == characterAdditionalObjects[i].moveToPerformOnBreak) {
							setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
							performMove(move, true);
							break;
						}
					}
				}
				if (!characterAdditionalObjects[i].hasDummyMesh()){
					break;
				}
			}
		}
	}

	FK_Character::FK_CharacterAdditionalObject* FK_Character::getObjectByName(std::string uniqueNameId){
		if (namedAdditionalObjectsReferences.count(uniqueNameId) > 0){
			return namedAdditionalObjectsReferences[uniqueNameId];
		}
		else{
			return NULL;
		}
	}

	void FK_Character::loadAdditionalObjects(std::string filenameTag){
		// key map
		std::string UniqueNameIdKey = "#name_id";
		std::string MoveListObjectNameKey = "#display_name";
		std::string RotationAngleWhenTouchingGroundKey = "#angle_on_ground_collision";
		std::string OffsetWhenTouchingGroundKey = "#offset_on_ground_collision";
		std::string ObjectBeginKey = "#NEW_OBJECT";
		std::string ObjectEndKey = "#NEW_OBJECT_END";
		std::string MeshKey = "#mesh";
		std::string ParentKey = "#parent";
		std::string PositionKey = "#offset";
		std::string ScaleKey = "#scale";
		std::string RotationKey = "#rotation";
		std::string IlluminationKey = "#apply_shadow";
		std::string BonesKey = "#newBones_begin";
		std::string BonesEndKey = "#newBones_end";
		std::string HitboxKey = "#newHitbox_begin";
		std::string HitboxEndKey = "#newHitbox_end";
		std::string ResetBetweenRoundsKey = "#reset_between_rounds";
		std::string BreakableKey = "#breakable";
		std::string BreakableEndKey = "#breakable_end";
		std::string PropMeshNameKey = "#broken_object_mesh";
		std::string BreakableMaxEnduranceKey = "#endurance";
		std::string BreakingLikelihoodKey = "#breaking_likelihood";
		std::string CanBePickedUpAgainKey = "#can_be_picked";
		std::string RestorationKey = "#can_be_restored";
		std::string FlyAwayWhenBrokenKey = "#allow_flight_when_broken";
		std::string DisappearWhenHittingGroundKey = "#disappear_when_hitting_ground";
		std::string RestorationLeavePropKey = "#leave_prop";
		std::string HiddenObjectKey = "#hidden";
		std::string HideOnHitKey = "#hide_when_hit";
		std::string ShowOnHitKey = "#show_when_hit";
		std::string ShowObjectWhenBrokenKey = "#show_object_when_broken";
		std::string ActOnObjectWhenBrokenKey = "#act_on_object_when_broken";
		std::string AnimateWithCharacterKey = "#animate_with_character";
		std::string BackfaceCullingKey = "#no_backface";
		// buffs
		std::string AttackBuffKey = "#attack_buff";
		std::string SpecificAttackBuffKey = "#advanced_attack_buff";
		std::string DefenseBuffKey = "#defense_buff";
		// armor
		std::string ArmorEffectWhenActiveKey = "#armor_when_active";
		// damage limiter
		std::string DamageLimiterKey = "#max_damage_limited_to";
		// invincibility
		std::string InvincibilityEffectWhenActiveKey = "#invincibility_when_active";
		// breaking conditions key
		std::string DamageConditionsStartKey = "#damage_conditions";
		std::string DamageConditionsEndKey = "#damage_conditions_end";
		std::string DamageConditionTypeBrokenObjectKey = "##object_broken";
		std::string DamageConditionTypeInactiveObjectKey = "##object_inactive";
		// object breaking reaction condition
		std::string BreakingReactionStartKey = "#broken_by_damage_type";
		std::string BreakingReactionEndKey = "#broken_by_damage_type_end";
		// move on destruction
		std::string MoveToUseWhenObjectIsBroken = "#move_on_object_break";
		// lifetime
		std::string LifetimeKey = "#lifetime_when_shown_ms";
		std::string LifetimeExpiresDuringMove = "#lifetime_expires_during_move";
		std::string LifetimeBarStartKey = "#show_lifetime_bar";
		std::string LifetimeBarEndKey = "#show_lifetime_bar_end";
		std::string LifetimeBarColorsKey = "##lifetime_bar_colors";
		std::string LifetimeBarBackgroundColorKey = "##lifetime_bar_background_color";
		std::string LifetimeBarBorderColorKey = "##lifetime_bar_border_color";
		std::string LifetimeBarReverseWhenBrokenKey = "##reverse_when_broken";
		std::string LifetimeBarDirectionKey = "##reverse_bar_direction";
		std::string LifetimeBarShowWhenInactiveKey = "##show_when_inactive";
		std::string LifetimeBarPositionKey = "##position";
		std::string LifetimeBarShowItemIconKey = "##show_item_icon";
		// endurance bar
		std::string EnduranceBarStartKey = "#show_life_bar";
		std::string EnduranceBarEndKey = "#show_life_bar_end";
		std::string EnduranceBarColorsKey = "##life_bar_colors";
		std::string EnduranceBarBackgroundColorKey = "##life_bar_background_color";
		std::string EnduranceBarBorderColorKey = "##life_bar_border_color";
		std::string EnduranceBarReverseWhenBrokenKey = "##reverse_when_broken";
		std::string EnduranceBarDirectionKey = "##reverse_bar_direction";
		std::string EnduranceBarShowWhenInactiveKey = "##show_when_inactive";
		std::string EnduranceBarPositionKey = "##position";
		std::string EnduranceBarShowItemIconKey = "##show_item_icon";
		// icon
		std::string HUDIconKey = "#show_item_icon";
		std::string HUDIconEndKey = "#show_item_icon_end";
		std::string HUDIconShowWhenActiveKey = "##show_when_active";
		std::string HUDIconShowWhenInactiveKey = "##show_when_inactive";
		std::string HUDIconPositionKey = "##position";
		// usages
		std::string UsagesKey = "#maximum_number_of_uses";
		// bone map
		std::map<std::string, FK_Bones_Type> stringToBoneMap;
		stringToBoneMap["INDEPENDENT"] = FK_Bones_Type::NoArmature;
		stringToBoneMap["NONE"] = FK_Bones_Type::RootArmature;
		stringToBoneMap["HEAD"] = FK_Bones_Type::HeadArmature;
		stringToBoneMap["NECK"] = FK_Bones_Type::NeckArmature;
		stringToBoneMap["TORSO"] = FK_Bones_Type::TorsoArmature;
		stringToBoneMap["SPINE"] = FK_Bones_Type::SpineArmature;
		stringToBoneMap["HIPS"] = FK_Bones_Type::HipsArmature;
		stringToBoneMap["SHOULDER_L"] = FK_Bones_Type::LeftArmArmature;
		stringToBoneMap["SHOULDER_R"] = FK_Bones_Type::RightArmArmature;
		stringToBoneMap["ELBOW_L"] = FK_Bones_Type::LeftForearmArmature;
		stringToBoneMap["ELBOW_R"] = FK_Bones_Type::RightForearmArmature;
		stringToBoneMap["WRIST_L"] = FK_Bones_Type::LeftWristArmature;
		stringToBoneMap["WRIST_R"] = FK_Bones_Type::RightWristArmature;
		stringToBoneMap["LEG_L"] = FK_Bones_Type::LeftLegArmature;
		stringToBoneMap["LEG_R"] = FK_Bones_Type::RightLegArmature;
		stringToBoneMap["KNEE_L"] = FK_Bones_Type::LeftKneeArmature;
		stringToBoneMap["KNEE_R"] = FK_Bones_Type::RightKneeArmature;
		stringToBoneMap["ANKLE_L"] = FK_Bones_Type::LeftAnkleArmature;
		stringToBoneMap["ANKLE_R"] = FK_Bones_Type::RightAnkleArmature;
		stringToBoneMap["FOOT_L"] = FK_Bones_Type::LeftFootArmature;
		stringToBoneMap["FOOT_R"] = FK_Bones_Type::RightFootArmature;
		stringToBoneMap["THUMB_L"] = FK_Bones_Type::LeftThumbArmatureTip;
		stringToBoneMap["THUMB_R"] = FK_Bones_Type::RightThumbArmatureTip;
		stringToBoneMap["FINGERS_L"] = FK_Bones_Type::LeftFingersArmatureMedium;
		stringToBoneMap["FINGERS_R"] = FK_Bones_Type::RightFingersArmatureMedium;
		stringToBoneMap["EXTRA_1"] = FK_Bones_Type::AdditionalArmature01;
		stringToBoneMap["EXTRA_2"] = FK_Bones_Type::AdditionalArmature02;
		stringToBoneMap["EXTRA_3"] = FK_Bones_Type::AdditionalArmature03;
		stringToBoneMap["EXTRA_4"] = FK_Bones_Type::AdditionalArmature04;
		stringToBoneMap["EXTRA_5"] = FK_Bones_Type::AdditionalArmature05;
		stringToBoneMap["EXTRA_6"] = FK_Bones_Type::AdditionalArmature06;
		stringToBoneMap["EXTRA_7"] = FK_Bones_Type::AdditionalArmature07;
		stringToBoneMap["EXTRA_8"] = FK_Bones_Type::AdditionalArmature08;
		stringToBoneMap["EXTRA_9"] = FK_Bones_Type::AdditionalArmature09;
		stringToBoneMap["EXTRA_10"] = FK_Bones_Type::AdditionalArmature10;
		// hitbox map
		std::map<std::string, FK_Hitbox_Type> hitboxTypeMap;
		hitboxTypeMap["None"] = FK_Hitbox_Type::NoHitbox;
		hitboxTypeMap["All"] = FK_Hitbox_Type::AllHitboxes;
		hitboxTypeMap["RightPunch"] = FK_Hitbox_Type::RightPunch;
		hitboxTypeMap["RightElbow"] = FK_Hitbox_Type::RightElbow;
		hitboxTypeMap["RightKnee"] = FK_Hitbox_Type::RightKnee;
		hitboxTypeMap["RightFoot"] = FK_Hitbox_Type::RightFoot;
		hitboxTypeMap["LeftPunch"] = FK_Hitbox_Type::LeftPunch;
		hitboxTypeMap["LeftElbow"] = FK_Hitbox_Type::LeftElbow;
		hitboxTypeMap["LeftKnee"] = FK_Hitbox_Type::LeftKnee;
		hitboxTypeMap["LeftFoot"] = FK_Hitbox_Type::LeftFoot;
		hitboxTypeMap["Extra1"] = FK_Hitbox_Type::AdditionalHitbox1;
		hitboxTypeMap["Extra2"] = FK_Hitbox_Type::AdditionalHitbox2;
		hitboxTypeMap["Extra3"] = FK_Hitbox_Type::AdditionalHitbox3;
		hitboxTypeMap["Extra4"] = FK_Hitbox_Type::AdditionalHitbox4;
		hitboxTypeMap["Extra5"] = FK_Hitbox_Type::AdditionalHitbox5;
		hitboxTypeMap["Extra6"] = FK_Hitbox_Type::AdditionalHitbox6;
		hitboxTypeMap["Extra7"] = FK_Hitbox_Type::AdditionalHitbox7;
		hitboxTypeMap["Extra8"] = FK_Hitbox_Type::AdditionalHitbox8;
		hitboxTypeMap["Extra9"] = FK_Hitbox_Type::AdditionalHitbox9;
		hitboxTypeMap["Extra10"] = FK_Hitbox_Type::AdditionalHitbox10;
		hitboxTypeMap["Head"] = FK_Hitbox_Type::Head;
		hitboxTypeMap["Torso"] = FK_Hitbox_Type::Torso;
		// attack type map
		std::map<std::string, FK_Attack_Type> attackTypeMap;
		attackTypeMap["HighAtks"] = FK_Attack_Type::HighAtks;
		attackTypeMap["LowAtks"] = FK_Attack_Type::LowAtks;
		attackTypeMap["MidAtks"] = FK_Attack_Type::MidAtks;
		attackTypeMap["UpperbodyAtks"] = FK_Attack_Type::HighAndMidAtks;
		attackTypeMap["Throws"] = FK_Attack_Type::ThrowAtk;
		attackTypeMap["All"] = FK_Attack_Type::AllAtk;
		attackTypeMap["Projectiles"] = FK_Attack_Type::BulletAttack;
		attackTypeMap["Bullets"] = FK_Attack_Type::BulletAttack;
		attackTypeMap["Full"] = FK_Attack_Type::AllAtk;
		attackTypeMap["All"] = FK_Attack_Type::AllAtk;
		attackTypeMap["AllAtks"] = FK_Attack_Type::AllAtk;
		attackTypeMap["AtksAndBullets"] =
			FK_Attack_Type((u32)(FK_Attack_Type::AllAtk) | (u32)(FK_Attack_Type::BulletAttack));
		// reaction type map
		std::map<std::string, u32> reactionTypeMap;
		reactionTypeMap["StrongFlight"] = FK_Reaction_Type::StrongFlight | FK_Reaction_Type::ReverseStrongFlight;
		reactionTypeMap["WeakAtks"] = FK_Reaction_Type::AnyWeakReaction;
		reactionTypeMap["StrongAtks"] = FK_Reaction_Type::AnyStrongReaction;
		reactionTypeMap["HighAtks"] = FK_Reaction_Type::AnyHighReaction;
		reactionTypeMap["MidAtks"] = FK_Reaction_Type::AnyMediumReaction;
		reactionTypeMap["LowAtks"] = FK_Reaction_Type::AnyLowReaction;
		reactionTypeMap["StrongHigh"] = FK_Reaction_Type::StrongHigh;
		reactionTypeMap["StrongMid"] = FK_Reaction_Type::StrongMedium;
		reactionTypeMap["StrongLow"] = FK_Reaction_Type::StrongLow;
		reactionTypeMap["WeakHigh"] = FK_Reaction_Type::WeakHigh;
		reactionTypeMap["WeakMid"] = FK_Reaction_Type::WeakMedium;
		reactionTypeMap["WeakLow"] = FK_Reaction_Type::WeakLow;
		reactionTypeMap["All"] = FK_Reaction_Type::AnyLowReaction | FK_Reaction_Type::AnyMediumReaction | FK_Reaction_Type::AnyHighReaction |
			FK_Reaction_Type::StrongFlight | FK_Reaction_Type::ReverseStrongFlight | FK_Reaction_Type::WeakFlight |
			FK_Reaction_Type::StandingFlight | FK_Reaction_Type::SmackdownLanding;
		// after use actions
		std::map<std::string, FK_CharacterAdditionalObject::AfterUseAction> afterUseActionsMap;
		afterUseActionsMap["hide"] = FK_CharacterAdditionalObject::AfterUseAction::Hide;
		afterUseActionsMap["show"] = FK_CharacterAdditionalObject::AfterUseAction::Show;
		afterUseActionsMap["restore"] = FK_CharacterAdditionalObject::AfterUseAction::Restore;
		afterUseActionsMap["break"] = FK_CharacterAdditionalObject::AfterUseAction::Break;
		// open file
		std::string temp;
		std::string configFile = characterDirectory + 
			availableCharacterOutfits[outfitId].outfitDirectory + 
			filenameTag + 
			"addons.txt";
		FK_Character::FK_CharacterAdditionalObject newObject;
		std::ifstream configurationFile(configFile.c_str());
		s32 numberOfObjects = 0;
		if (!configurationFile){
			if (!filenameTag.empty()) {
				configurationFile.clear();
				configurationFile.close();
				configFile = characterDirectory +
					availableCharacterOutfits[outfitId].outfitDirectory +
					"addons.txt";
				configurationFile = std::ifstream(configFile.c_str());
				if (!configurationFile) {
					return;
				}
			}
			return;
		}
		while (configurationFile >> temp){
			if (temp == ObjectBeginKey){
				newObject = FK_Character::FK_CharacterAdditionalObject();
				newObject.meshPath = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory;
			}
			else if (temp == ObjectEndKey){
				newObject.objectArrayIndex = numberOfObjects;
				characterAdditionalObjects.push_back(newObject);
				numberOfObjects += 1;
			}
			else if (temp == UniqueNameIdKey){
				configurationFile >> temp;
				newObject.uniqueNameId = temp;
			}
			else if (temp == MoveListObjectNameKey){
				configurationFile >> temp;
				std::replace(temp.begin(), temp.end(), '_', ' ');
				newObject.displayName = temp;
			}
			else if (temp == ParentKey){
				configurationFile >> temp;
				newObject.parent = stringToBoneMap[temp];
			}
			else if (temp == IlluminationKey){
				newObject.applyShadow = true;
			}
			else if (temp == BackfaceCullingKey){
				newObject.backfaceCulling = true;
			}
			else if (temp == MeshKey){
				configurationFile >> temp;
				newObject.meshFilename = temp;
			}
			else if (temp == PositionKey){
				f32 x, y, z;
				configurationFile >> x >> y >> z;
				newObject.positionOffset = core::vector3df(x, y, z);
			}
			else if (temp == ScaleKey){
				f32 x, y, z;
				configurationFile >> x >> y >> z;
				newObject.scale = core::vector3df(x, y, z);
			}
			else if (temp == RotationKey){
				f32 ax, ay, az;
				configurationFile >> ax >> ay >> az;
				newObject.rotationAngle = core::vector3df(ax, ay, az);
			}
			else if (temp == HiddenObjectKey){
				newObject.defaultVisibility = false;
				newObject.visible = false;
			}
			else if (temp == HideOnHitKey){
				newObject.hideWhenHit = true;
			}
			else if (temp == ShowOnHitKey){
				newObject.showWhenHit = true;
			}
			else if (temp == ResetBetweenRoundsKey){
				newObject.resetBetweenRounds = true;
			}
			else if (temp == AnimateWithCharacterKey){
				newObject.animateWithMesh = true;
			}
			else if (temp == ArmorEffectWhenActiveKey) {
				configurationFile >> temp;
				if (attackTypeMap.count(temp) > 0) {
					newObject.armorWhenActive |= attackTypeMap[temp];
				}
			}
			else if (temp == InvincibilityEffectWhenActiveKey) {
				configurationFile >> temp;
				if (attackTypeMap.count(temp) > 0) {
					newObject.invincibilityWhenActive |= attackTypeMap[temp];
				}
			}
			else if (temp == DamageLimiterKey) {
				f32 limit = 0.f;
				configurationFile >> limit;
				newObject.maxDamageLimit = limit;
			}
			else if (temp == AttackBuffKey){
				f32 value;
				configurationFile >> temp >> value;
				FK_CharacterAdditionalObject::ObjectAttackBuff buff;
				if (hitboxTypeMap.count(temp) > 0){
					buff.type = hitboxTypeMap[temp];
					buff.damageMultiplier = value;
					newObject.attackBuffs.push_back(buff);
				}else if (attackTypeMap.count(temp) > 0) {
					buff.attackType = attackTypeMap[temp];
					buff.damageMultiplier = value;
					newObject.attackBuffs.push_back(buff);
				}
			}
			else if (temp == SpecificAttackBuffKey) {
				f32 value;
				std::string attackType;
				configurationFile >> temp >> attackType >> value;
				FK_CharacterAdditionalObject::ObjectAttackBuff buff;
				if (hitboxTypeMap.count(temp) > 0 && attackTypeMap.count(attackType) > 0) {
					buff.type = hitboxTypeMap[temp];
					buff.attackType = attackTypeMap[attackType];
					buff.damageMultiplier = value;
					newObject.attackBuffs.push_back(buff);
				}
			}
			else if (temp == DefenseBuffKey){
				f32 value;
				configurationFile >> temp >> value;
				FK_CharacterAdditionalObject::ObjectDefenseBuff buff;
				if (attackTypeMap.count(temp) > 0){
					buff.type = attackTypeMap[temp];
					buff.damageMultiplier = value;
					newObject.defenseBuffs.push_back(buff);
				}
			}
			else if (temp == HUDIconKey) {
				newObject.showTimeBar = true;
				newObject.hudIcon = FK_CharacterAdditionalObject::HUDicon();
				while (configurationFile >> temp) {
					if (temp == HUDIconEndKey) {
						break;
					}
					else if (temp == HUDIconShowWhenActiveKey) {
						newObject.hudIcon.showWhenActive = true;
					}
					else if (temp == HUDIconShowWhenInactiveKey) {
						newObject.hudIcon.showWhenInactive = true;
					}
					else if (temp == HUDIconPositionKey) {
						u32 x, y, w, h;
						configurationFile >> x >> y >> w >> h;
						newObject.hudIcon.position = core::rect<s32>(x, y, w, h);
					}
				}
			}
			else if (temp == LifetimeKey) {
				f32 value;
				configurationFile >> value;
				value /= 1000.f;
				newObject.maxLifetime = value;
				newObject.expiresWithTime = true;
				configurationFile >> temp;
				if (afterUseActionsMap.count(temp) > 0) {
					newObject.expirationAction = afterUseActionsMap[temp];
				}
				else {
					newObject.expirationAction = FK_CharacterAdditionalObject::AfterUseAction::NoAction;
				}
			}
			else if (temp == LifetimeExpiresDuringMove) {
				newObject.expiresDuringMove = true;
			}
			else if (temp == LifetimeBarStartKey) {
				newObject.showTimeBar = true;
				newObject.hudBar = FK_CharacterAdditionalObject::HUDbar();
				while (configurationFile >> temp) {
					if (temp == LifetimeBarEndKey) {
						break;
					}
					else if (temp == LifetimeBarDirectionKey) {
						newObject.hudBar.reverseDirection = true;
					}
					else if (temp == LifetimeBarReverseWhenBrokenKey) {
						newObject.hudBar.reverseWhenBroken = true;
					}
					else if (temp == LifetimeBarShowWhenInactiveKey) {
						newObject.hudBar.showWhenInactive = true;
					}
					else if (temp == LifetimeBarPositionKey) {
						u32 x, y, w, h;
						configurationFile >> x >> y >> w >> h;
						newObject.hudBar.position = core::rect<s32>(x, y, w, h);
					}
					else if (temp == LifetimeBarShowItemIconKey) {
						u32 x, y, w, h;
						configurationFile >> x >> y >> w >> h;
						newObject.hudBar.showIcon = true;
						newObject.hudBar.iconPosition = core::rect<s32>(x, y, w, h);
					}
					else if (temp == LifetimeBarColorsKey) {
						u32 a, r, g, b;
						configurationFile >> a >> r >> g >> b;
						newObject.hudBar.color1 = video::SColor(a,r,g,b);
						configurationFile >> a >> r >> g >> b;
						newObject.hudBar.color2 = video::SColor(a, r, g, b);
					}
					else if (temp == LifetimeBarBackgroundColorKey) {
						u32 a, r, g, b;
						configurationFile >> a >> r >> g >> b;
						newObject.hudBar.backgroundColor = video::SColor(a, r, g, b);
					}
					else if (temp == LifetimeBarBorderColorKey) {
						u32 a, r, g, b;
						configurationFile >> a >> r >> g >> b;
						newObject.hudBar.borderColor = video::SColor(a, r, g, b);
					}
				}
				
			}
			else if (temp == EnduranceBarStartKey) {
				newObject.showEnduranceBar = true;
				newObject.hudLifeBar = FK_CharacterAdditionalObject::HUDbar();
				while (configurationFile >> temp) {
					if (temp == EnduranceBarEndKey) {
						break;
					}
					else if (temp == EnduranceBarDirectionKey) {
						newObject.hudLifeBar.reverseDirection = true;
					}
					else if (temp == EnduranceBarReverseWhenBrokenKey) {
						newObject.hudLifeBar.reverseWhenBroken = true;
					}
					else if (temp == EnduranceBarShowWhenInactiveKey) {
						newObject.hudLifeBar.showWhenInactive = true;
					}
					else if (temp == EnduranceBarPositionKey) {
						u32 x, y, w, h;
						configurationFile >> x >> y >> w >> h;
						newObject.hudLifeBar.position = core::rect<s32>(x, y, w, h);
					}
					else if (temp == EnduranceBarShowItemIconKey) {
						u32 x, y, w, h;
						configurationFile >> x >> y >> w >> h;
						newObject.hudLifeBar.showIcon = true;
						newObject.hudLifeBar.iconPosition = core::rect<s32>(x, y, w, h);
					}
					else if (temp == EnduranceBarColorsKey) {
						u32 a, r, g, b;
						configurationFile >> a >> r >> g >> b;
						newObject.hudLifeBar.color1 = video::SColor(a, r, g, b);
						configurationFile >> a >> r >> g >> b;
						newObject.hudLifeBar.color2 = video::SColor(a, r, g, b);
					}
					else if (temp == EnduranceBarBackgroundColorKey) {
						u32 a, r, g, b;
						configurationFile >> a >> r >> g >> b;
						newObject.hudLifeBar.backgroundColor = video::SColor(a, r, g, b);
					}
					else if (temp == EnduranceBarBorderColorKey) {
						u32 a, r, g, b;
						configurationFile >> a >> r >> g >> b;
						newObject.hudLifeBar.borderColor = video::SColor(a, r, g, b);
					}
				}
			}
			else if (temp == UsagesKey) {
				s32 value;
				configurationFile >> value;
				newObject.maxNumberOfUsages = value;
				newObject.useable = true;
				configurationFile >> temp;
				if (afterUseActionsMap.count(temp) > 0) {
					newObject.afterUsageAction = afterUseActionsMap[temp];
				}
				else {
					newObject.afterUsageAction = FK_CharacterAdditionalObject::AfterUseAction::NoAction;
				}
			}
			else if (temp == BreakableKey){
				newObject.breakable = true;
				newObject.maxEndurance = 1;
				newObject.canBeRestored = false;
				while (configurationFile >> temp){
					if (temp == BreakableEndKey){
						break;
					}
					// breakable by reaction
					else if (temp == BreakingReactionStartKey) {
						newObject.reactionAbleToBreakObject = FK_Reaction_Type::NoReaction;
						while (configurationFile >> temp) {
							if (temp == BreakingReactionEndKey) {
								break;
							}
							else {
								if (reactionTypeMap.count(temp) > 0) {
									newObject.reactionAbleToBreakObject |= reactionTypeMap[temp];
								}
							}
						}
					}
					// damage conditions
					else if (temp == DamageConditionsStartKey){
						while (configurationFile >> temp){
							if (temp == DamageConditionsEndKey){
								break;
							}
							else if (temp == DamageConditionTypeBrokenObjectKey){
								configurationFile >> temp;
								FK_Character::FK_CharacterAdditionalObject::DamageCondition condition;
								condition.type = FK_Character::FK_CharacterAdditionalObject::DamageConditionType::ObjectBroken;
								condition.stringParam = temp;
								newObject.damageConditions.push_back(condition);
							}
							else if (temp == DamageConditionTypeInactiveObjectKey){
								configurationFile >> temp;
								FK_Character::FK_CharacterAdditionalObject::DamageCondition condition;
								condition.type = FK_Character::FK_CharacterAdditionalObject::DamageConditionType::ObjectInactive;
								condition.stringParam = temp;
								newObject.damageConditions.push_back(condition);
							}
						}
					}
					// move when object is broken
					else if (temp == MoveToUseWhenObjectIsBroken) {
						configurationFile >> temp;
						newObject.moveToPerformOnBreak = temp;
					}
					// prop mesh
					else if (temp == PropMeshNameKey){
						configurationFile >> temp;
						newObject.propMeshFilename.push_back(temp);
					}
					else if (temp == BreakableMaxEnduranceKey){
						f32 newEndurance;
						configurationFile >> newEndurance;
						newObject.maxEndurance = newEndurance;
						newObject.endurance = newObject.maxEndurance;
					}
					else if (temp == BreakingLikelihoodKey){
						f32 maxBreakLikelihood;
						configurationFile >> maxBreakLikelihood;
						newObject.maximumBreakingLikelihood = maxBreakLikelihood;
					}
					else if (temp == RestorationKey){
						u32 canBeRestored = 0;
						configurationFile >> canBeRestored;
						newObject.canBeRestored = canBeRestored > 0;
					}
					else if (temp == FlyAwayWhenBrokenKey){
						newObject.flyAwayWhenBroken = true;
					}
					else if (temp == DisappearWhenHittingGroundKey){
						newObject.disappearWhenHitGround = true;
					}
					else if (temp == CanBePickedUpAgainKey){
						newObject.canBePickedFromGround = true;
					}
					else if (temp == ShowObjectWhenBrokenKey){
						configurationFile >> temp;
						newObject.objectToToggleWhenBroken.push_back(
							std::pair<std::string, FK_CharacterAdditionalObject::AfterUseAction>
							(temp, FK_CharacterAdditionalObject::AfterUseAction::Show));
					}
					else if (temp == ActOnObjectWhenBrokenKey) {
						std::string objectId;
						configurationFile >> objectId >> temp;
						FK_CharacterAdditionalObject::AfterUseAction action;
						if (afterUseActionsMap.count(temp) > 0) {
							action = afterUseActionsMap[temp];
						}
						else {
							action = FK_CharacterAdditionalObject::AfterUseAction::NoAction;
						}
						newObject.objectToToggleWhenBroken.push_back(
							std::pair<std::string, FK_CharacterAdditionalObject::AfterUseAction>
							(objectId, action));
					}
					else if (temp == RotationAngleWhenTouchingGroundKey){
						f32 ax, ay, az;
						configurationFile >> ax >> ay >> az;
						newObject.landingRotationAngleMin = core::vector3df(ax, ay, az);
						configurationFile >> ax >> ay >> az;
						newObject.landingRotationAngleMax = core::vector3df(ax, ay, az);
						newObject.rotateWhenTouchingGround = true;
					}
					else if (temp == OffsetWhenTouchingGroundKey){
						f32 x, y, z;
						configurationFile >> x >> y >> z;
						newObject.landingPositionOffset = core::vector3df(x, y, z);
					}
					else if (temp == RestorationLeavePropKey){
						newObject.leavePropOnGroundWhenRestored = true;
					}
				}
			}
			else if (temp == BonesKey){
				while (configurationFile){
					configurationFile >> temp;
					if (temp == BonesEndKey){
						break;
					}
					std::string boneName;
					configurationFile >> boneName;
					newObject.additionalBones[stringToBoneMap[temp]] = boneName;
				}
			}
			else if (temp == HitboxKey){
				while (configurationFile){
					configurationFile >> temp;
					if (temp == HitboxEndKey){
						break;
					}
					if (hitboxTypeMap.count(temp) == 0){
						break;
					}
					FK_Hitbox_Type type = hitboxTypeMap[temp];
					configurationFile >> temp;
					if (stringToBoneMap.count(temp) == 0){
						break;
					}
					/*if (newObject.additionalBones.count(stringToBoneMap[temp]) == 0){
					break;
					}*/
					FK_Bones_Type btype = stringToBoneMap[temp];
					f32 x, y, z;
					configurationFile >> x >> y >> z;
					core::vector3df offsetPosition(x, y, z);
					configurationFile >> x >> y >> z;
					core::vector3df hitboxScale(x, y, z);
					FK_CharacterAdditionalHitbox hitbox;
					hitbox.parent = btype;
					hitbox.type = type;
					hitbox.positionOffset = offsetPosition;
					hitbox.scale = hitboxScale;
					newObject.additionalHitboxes.push_back(hitbox);
				}
			}
		}
		// prepare list of all named objects
		for (u32 i = 0; i < characterAdditionalObjects.size(); ++i){
			if (characterAdditionalObjects[i].uniqueNameId != std::string()){
				namedAdditionalObjectsReferences[characterAdditionalObjects[i].uniqueNameId] = &characterAdditionalObjects[i];
			}
		}
		// load meshs
		loadAdditionalObjectsMesh();
	}
	void FK_Character::loadAdditionalObjectsMesh(){
		for each(auto object in characterAdditionalObjects){
			f32 scale = meshScale;
			if (availableCharacterOutfits[outfitId].outfitMeshScale > 0.f){
				scale = availableCharacterOutfits[outfitId].outfitMeshScale;
			}
			if (object.parent == FK_Bones_Type::RootArmature ||
				object.parent == FK_Bones_Type::NoArmature){
				scale = 1.f;
			}
			f32 scaleX = scale * object.scale.X;
			f32 scaleY = scale * object.scale.Y;
			f32 scaleZ = scale * object.scale.Z;
			std::string meshfile = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory + object.meshFilename;
			scene::IAnimatedMeshSceneNode* animatedNode;
			scene::ISkinnedMesh* mesh1 = (scene::ISkinnedMesh*)smgr_reference->getMesh(meshfile.c_str());
			if (object.hasDummyMesh()){
				characterAdditionalObjectsMeshs.push_back(NULL);
				continue;
			}
			animatedNode = smgr_reference->addAnimatedMeshSceneNode(mesh1);
			// load also prop mesh
			for each(std::string propKey in object.propMeshFilename){
				std::string propfile = characterDirectory + availableCharacterOutfits[outfitId].outfitDirectory + propKey;
				scene::ISkinnedMesh* mesh2 = (scene::ISkinnedMesh*)smgr_reference->getMesh(propfile.c_str());
			}
			animatedNode->setScale(core::vector3df(scaleX, scaleY, scaleZ));
			animatedNode->setMaterialFlag(video::EMF_LIGHTING, false);
			animatedNode->setJointMode(irr::scene::EJUOR_CONTROL);
			animatedNode->setAnimationSpeed(animationFrameRate);
			if (object.animateWithMesh) {
				animatedNode->setTransitionTime(animationTransitionTime);
			}else{
				animatedNode->setTransitionTime(0.f);
			}
			int mcount = animatedNode->getMaterialCount();
			for (int i = 0; i < mcount; i++){
				animatedNode->getMaterial(i).BackfaceCulling = object.backfaceCulling;
				animatedNode->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
				animatedNode->getMaterial(i).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			}
			std::string boneName = armatureBonesCollection[object.parent];
			scene::IBoneSceneNode* bone = animatedMesh->getJointNode(boneName.c_str());
			if (object.parent == FK_Bones_Type::NoArmature) {
				
			}else if (bone == NULL){
				animatedNode->setParent(animatedMesh);
			}
			else{
				animatedNode->setParent(bone);
			}
			animatedNode->setPosition(object.positionOffset);
			animatedNode->setRotation(object.rotationAngle);
			if (bone != NULL){
				bone->updateAbsolutePosition();
			}
			animatedNode->updateAbsolutePosition();
			animatedNode->setLoopMode(true);
			//animatedNode->setJointMode(irr::scene::EJUOR_CONTROL);
			characterAdditionalObjectsMeshs.push_back(animatedNode);
			// load hitboxes
			if (hitboxCollection.empty()){
				continue;
			}
			for each (FK_CharacterAdditionalHitbox extraHitbox in object.additionalHitboxes){
				if (extraHitbox.parent == FK_Bones_Type::RootArmature ||
					extraHitbox.parent == FK_Bones_Type::NoArmature){
					boneName = std::string();
				}
				else if (object.additionalBones.count(extraHitbox.parent) != 0){
					boneName = object.additionalBones[extraHitbox.parent];
				}
				else{
					continue;
				}
				scene::ISceneNode * hitbox = smgr_reference->addSphereSceneNode();
				hitbox->setMaterialFlag(video::EMF_LIGHTING, false);
				hitbox->setMaterialFlag(video::EMF_WIREFRAME, true);
				scene::IBoneSceneNode* bone;
				if (boneName.empty()){
					if (extraHitbox.parent == FK_Bones_Type::RootArmature) {
						hitbox->setParent(animatedNode);
					}
				}
				else{
					bone = animatedNode->getJointNode(boneName.c_str());
					if (bone == NULL){
						continue;
					}
					hitbox->setParent(bone);
				}
				hitbox->setScale(extraHitbox.scale);
				hitbox->setPosition(extraHitbox.positionOffset);
				//hitbox->setDebugDataVisible(scene::EDS_BBOX);
				hitbox->setVisible(false);
				hitboxCollection[extraHitbox.type] = hitbox;
				hitbox->updateAbsolutePosition();
			}
		}
	}

	void FK_Character::setAdditionalObjectsMovesDependencies(){
		if (movesCollection.empty()){
			return;
		}
		for each(FK_Move testMove in movesCollection){
			for (auto it = namedAdditionalObjectsReferences.begin(); it != namedAdditionalObjectsReferences.end(); ++it){
				if (testMove.canBreakItem(it->first)){
					it->second->canBeBrokenByMove = true;
				}
				if (testMove.canUseItem(it->first)) {
					it->second->canBeBrokenByMove = true;
				}
				if (testMove.canToggleItem(it->first)){
					it->second->canBeToggledByMove = true;
				}
				if (/*it->second->defaultVisibility && */testMove.canHideItem(it->first)){
					it->second->canBeBrokenByMove = true;
				}
				if (!it->second->defaultVisibility && (testMove.canShowItem(it->first) || testMove.canRestoreItem(it->first))){
					it->second->canBeToggledByMove = true;
				}
			}
		}
	}

	void FK_Character::setAdditionalObjectsObjectsDependencies() {
		if (characterAdditionalObjects.empty()) {
			return;
		}
		//create static copy of vector
		std::vector<FK_CharacterAdditionalObject> testVector(characterAdditionalObjects);
		for each(FK_CharacterAdditionalObject testObject in testVector) {
			for (auto it = namedAdditionalObjectsReferences.begin(); it != namedAdditionalObjectsReferences.end(); ++it) {
				for each (auto objPair in testObject.objectToToggleWhenBroken) {
					if (objPair.first == it->first) {
						if (objPair.second == FK_CharacterAdditionalObject::AfterUseAction::Break ||
							objPair.second == FK_CharacterAdditionalObject::AfterUseAction::Hide) {
							it->second->canBeBrokenByObject = true;
						}
						if (objPair.second == FK_CharacterAdditionalObject::AfterUseAction::Restore ||
							objPair.second == FK_CharacterAdditionalObject::AfterUseAction::Show) {
							if (!it->second->defaultVisibility) {
								it->second->canBeToggledByObject = true;
							}
						}
						if (objPair.second == FK_CharacterAdditionalObject::AfterUseAction::Toggle) {
							it->second->canBeToggledByObject = true;
						}
					}
				}
			}
		}
	}

	const std::vector<scene::IAnimatedMeshSceneNode*> &FK_Character::getCharacterAdditionalObjectsNodes() const{
		return characterAdditionalObjectsMeshs;
	};
	const std::vector<FK_Character::FK_CharacterAdditionalObject> &FK_Character::getCharacterAdditionalObjects() const{
		return characterAdditionalObjects;
	}

	/* store the nearest object name so that it can be picked again by player*/
	void FK_Character::setPickableObjectId(std::string nameId){
		pickableObjectNameId = nameId;
	}
	std::string FK_Character::getPickableObjectId(){
		return pickableObjectNameId;
	}

	bool FK_Character::isPickingUpObject(){
		return isPickingObjectFlag;
	}

	void FK_Character::resetPickingObjectFlag(){
		isPickingObjectFlag = false;
	}

	void FK_Character::synchronizeAnimatedObjects(){
		for (u32 k = 0; k < characterAdditionalObjects.size(); ++k){
			if (characterAdditionalObjects[k].animateWithMesh && !characterAdditionalObjects[k].hasDummyMesh()){
				characterAdditionalObjectsMeshs[k]->setAnimationSpeed(animatedMesh->getAnimationSpeed());
				characterAdditionalObjectsMeshs[k]->setCurrentFrame(animatedMesh->getFrameNr(), true);
			}
		}
	}

	f32 FK_Character::getObjectsAttackMultiplier(FK_Hitbox_Type hitboxType, FK_Attack_Type attack_type){
		f32 multiplier = 1.0f;
		if (!characterAdditionalObjects.empty()){
			for each(FK_CharacterAdditionalObject object in characterAdditionalObjects){
				if (object.isActive()){
					for each(FK_CharacterAdditionalObject::ObjectAttackBuff buff in object.attackBuffs){
						if (buff.type == hitboxType || buff.type == FK_Hitbox_Type::AllHitboxes){
							if (buff.attackType == FK_Attack_Type::NoType || ((attack_type & buff.attackType) != 0)) {
								multiplier *= buff.damageMultiplier;
							}
						}
					}
				}
			}
		}
		return multiplier;
	}

	f32 FK_Character::getObjectsDefenseMultiplier(FK_Attack_Type attackType){
		f32 multiplier = 1.0f;
		if (!characterAdditionalObjects.empty()){
			for each(FK_CharacterAdditionalObject object in characterAdditionalObjects){
				if (object.isActive()){
					for each(FK_CharacterAdditionalObject::ObjectDefenseBuff buff in object.defenseBuffs){
						if ((buff.type & attackType) > 0){
							multiplier /= buff.damageMultiplier;
						}
					}
				}
			}
		}
		return multiplier;
	}

	u32 FK_Character::getTotalObjectArmor() {
		u32 totalArmor = 0;
		if (!characterAdditionalObjects.empty()) {
			for each(FK_CharacterAdditionalObject object in characterAdditionalObjects) {
				if (object.isActive()) {
					totalArmor |= object.armorWhenActive;
				}
			}
		}
		return totalArmor;
	}

	u32 FK_Character::getTotalObjectInvincibility() {
		u32 totalInvincibility = 0;
		if (!characterAdditionalObjects.empty()) {
			for each(FK_CharacterAdditionalObject object in characterAdditionalObjects) {
				if (object.isActive()) {
					totalInvincibility |= object.invincibilityWhenActive;
				}
			}
		}
		return totalInvincibility;
	}
}