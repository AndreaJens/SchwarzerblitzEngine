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
						ISceneNodeAnimator* pAnim1 = *(player->getAnimatedMesh()->getAnimators().begin());
						if (pAnim1->getType() == ESNAT_COLLISION_RESPONSE)
						{
							auto radius = ((scene::ISceneNodeAnimatorCollisionResponse*)pAnim1)->getEllipsoidRadius();
							auto currentTempPosition = player->getAnimatedMesh()->getPosition();
							if (currentTempPosition.Y < radius.Y)
								currentTempPosition.Y += 2;
							player->getAnimatedMesh()->setPosition(currentTempPosition);
						}
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