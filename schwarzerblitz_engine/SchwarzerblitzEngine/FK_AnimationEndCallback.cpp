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