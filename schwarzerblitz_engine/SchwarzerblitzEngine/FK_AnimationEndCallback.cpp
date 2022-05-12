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

#include"FK_AnimationEndCallback.h"
#include<iostream>
#include<string>

using namespace irr;

namespace fk_engine{

		// constructor
		FK_AnimationEndCallback::FK_AnimationEndCallback(FK_Character* new_character){
			character = new_character;
		};
		// destructor
		FK_AnimationEndCallback::~FK_AnimationEndCallback(){};
		// callback event
		void FK_AnimationEndCallback::OnAnimationEnd(scene::IAnimatedMeshSceneNode* node){
			if (character->getCurrentMove() != NULL && character->getNextMove() == NULL){
				if (character->getCurrentMove()->canTrackAfterAnimation()){
					character->setTrackingFlag();
				}
			}
			if (character->isBeingThrown() || character->isThrowingOpponent()){
				//std::wstring aniName(character->getCurrentAnimationName().c_str());
				//std::wcout << aniName << " callback called!" <<std::endl;
				FK_Reaction_Type throwReaction = character->getThrowReaction();
				FK_Stance_Type throwStance = character->getThrowStance();
				if (throwReaction != FK_Reaction_Type::NoReaction){
					if (character->isKO()){
						character->setReaction(fk_constants::FK_KOReaction, false, 0);
					}else{
						character->setReaction(throwReaction, false, 0);
					}
				}
				else{
					if (!character->isKO() && throwStance != FK_Stance_Type::NoStance){
						character->setStance(throwStance);
						switch (throwStance) {
						case FK_Stance_Type::SupineStance:
							character->setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation, true);
							break;
						case FK_Stance_Type::GroundStance:
							character->setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
							break;
						case FK_Stance_Type::CrouchedStance:
							character->setBasicAnimation(FK_BasicPose_Type::CrouchingIdleAnimation, true);
							break;
						default:
							break;
						}
					}
					else{
						if (character->isBeingThrown()){
							character->setStance(FK_Stance_Type::SupineStance);
							character->setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation, true);
						}
					}

				}
				character->setThrowFlag(false);
				character->setThrowingOpponentFlag(false);
				character->clearThrow();
			}
			if (character->getStance() == FK_Stance_Type::LandingStance){
				if (character->getCurrentMove() != NULL){
					if (character->getNextMove() != NULL){
						character->performMove(character->getNextMove());
					} else{
						character->resetMove(false);
					}
				}
				character->setStance(FK_Stance_Type::SupineStance);
				character->setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation);
			}
			if (character->getStance() == FK_Stance_Type::WakeUpStance){
				character->setVelocityPerSecond(irr::core::vector3df(0, 0, 0));
				if (character->isGuarding()){
					character->setBasicAnimation(FK_BasicPose_Type::GuardingAnimation, true);
				}
				else{
					character->setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
				}
				character->setStance(FK_Stance_Type::GroundStance);
			}
		};
		// get character
		FK_Character* FK_AnimationEndCallback::getCharacter(){
			return character;
		}
};