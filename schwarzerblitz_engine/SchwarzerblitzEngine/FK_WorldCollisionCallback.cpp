#include "FK_WorldCollisionCallback.h"
#include<iostream>

using namespace irr;

namespace fk_engine{


	FK_WorldCollisionCallback::FK_WorldCollisionCallback(FK_Character *new_player){
				player = new_player;
			};
	bool FK_WorldCollisionCallback::onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse& animator)
			{
				bool hasVerticalVelocity = player->getVelocityPerSecond().Z != 0;
				//hasVerticalVelocity = false; //debug only!
				if (hasVerticalVelocity &&
					player->isJumping() ||
					player->getStance() == FK_Stance_Type::AirStance ||
					player->getCurrentMove() != NULL && player->getCurrentMove()->getStance() == FK_Stance_Type::AirStance){
					player->cancelJump();
					player->setVelocityPerSecond(core::vector3df(0, 0, 0));
					if (player->getCurrentMove() == NULL ||
						(player->getCurrentMove() != NULL && (
						(player->getCurrentFrame() > (player->getCurrentMove()->getStartingFrame() + 1))/* ||
						(player->getNextMove() != NULL && player->getNextMove()->getStance() != FK_Stance_Type::AirStance)*/
						)
						)){
						player->setGroundCollisionFlag(true);
						return false;
					}
					else{
						return false;
					}
				}
				else{
					if (player->isHitStun() && player->canCancelHitstun(hasVerticalVelocity) || player->getRingoutFlag()){
						if (hasVerticalVelocity){
							player->resetCanSufferRingoutFlag();
							player->setStance(FK_Stance_Type::LandingStance);
							player->setBasicAnimation(FK_BasicPose_Type::SupineLandingAnimation);
							player->setDirectionLock(true);
						}
						player->setVelocityPerSecond(core::vector3df(0, 0, 0));
						player->cancelHitstun();
					}
					return false;
				}
				player->setVelocityPerSecond(core::vector3df(player->getVelocityPerSecond().X, player->getVelocityPerSecond().Y, 0));
				return false;
			}

	FK_WorldCollisionCallbackProp::FK_WorldCollisionCallbackProp(ISceneNode* newNode, FK_Character::FK_SceneProp* newProp){
		node = newNode;
		prop = newProp;
	};
	FK_WorldCollisionCallbackProp::~FK_WorldCollisionCallbackProp(){
		
	}
	bool FK_WorldCollisionCallbackProp::onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse& animator)
	{
		if (!prop->hasCollided){
			prop->hasCollided = true;
			prop->mustSetPlayerCollision = true;
			//ISceneNodeAnimator* pAnim = *(node->getAnimators().getLast());
			//if (pAnim->getType() == ESNAT_ROTATION){
			//	node->removeAnimator(pAnim);
			//}
			auto itBgn = node->getAnimators().begin();
			auto itEnd = node->getAnimators().end();
			for (auto iterator = itBgn; iterator != itEnd; ++iterator) {
				ISceneNodeAnimator* pAnim = *(iterator);
				if (pAnim->getType() == ESNAT_ROTATION){
					pAnim->setEnabled(false, 0);
					break;
				}
			}
		}
		return false;
	}
};