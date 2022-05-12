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

#include"FK_Hitbox.h"

using namespace irr;

namespace fk_engine{

		FK_Hitbox::FK_Hitbox(){
			startingFrame = 0;
			endingFrame = 0;
			damage = 0;
			priority = 0;
			canIgnoreGuard = false;
			hitSuccesful = false;
			singleHit = true;
			type = FK_Hitbox_Type::NoHitbox;
			attackType = FK_Attack_Type::NoType;
			reaction = FK_Reaction_Type::NoReaction;
			internalId = 0;
		};

		FK_Hitbox::FK_Hitbox(int newStartFrame, int newEndFrame, float newDamage,
			int newPriority, bool guardBreak, bool newSingleHit, FK_Hitbox_Type newType, 
			FK_Reaction_Type newReaction, FK_Attack_Type newAttackType){
			/* setup according to parameters */
			setup(newStartFrame, newEndFrame, newDamage,
				newPriority, guardBreak, newSingleHit, newType, newReaction,
				newAttackType);
		};

		void FK_Hitbox::setup(int newStartFrame, int newEndFrame, float newDamage,
			int newPriority, bool guardBreak, bool newSingleHit, FK_Hitbox_Type newType, 
			FK_Reaction_Type newReaction, FK_Attack_Type newAttackType){
			hitSuccesful = false;
			startingFrame = newStartFrame;
			endingFrame = newEndFrame;
			damage = newDamage;
			priority = newPriority;
			canIgnoreGuard = guardBreak;
			singleHit = newSingleHit;
			type = newType;
			reaction = newReaction;
			attackType = newAttackType;
			internalId = 0;
		};

		int FK_Hitbox::getStartingFrame() const {
			return startingFrame;
		};
		int FK_Hitbox::getEndingFrame() const {
			return endingFrame;
		}
		u32 FK_Hitbox::getInternalId() const
		{
			return internalId;
		}
		;
		float FK_Hitbox::getDamage() const {
			return damage;
		};
		void FK_Hitbox::setDamage(float new_damage){
			damage = new_damage;
		}
		int FK_Hitbox::getPriority() const {
			return priority;
		}
		void FK_Hitbox::setPriority(int newPriority)
		{
			priority = newPriority;
			if (priority < 0) {
				priority = 0;
			}
		}
		;
		FK_Attack_Type FK_Hitbox::getAttackType() const {
			return attackType;
		};
		bool FK_Hitbox::isGuardBreaking() const {
			return canIgnoreGuard;
		};
		bool FK_Hitbox::hasHit() const {
			return hitSuccesful;
		};
		void FK_Hitbox::setHit(bool hitFlag){
			hitSuccesful = hitFlag;
		}
		// internal id
		void FK_Hitbox::setInternalId(u32 newId)
		{
			internalId = newId;
		}
		/* determine if the hitbox is active */
		bool FK_Hitbox::isActive(int frameNumber){
			/* the hitbox is inactive outside of the bound frames*/
			bool flag = (frameNumber >= getStartingFrame() && frameNumber <= getEndingFrame());
			/* the hitbox is inactive if continuous hit is off and one hit has been scored*/
			flag &= !hitSuccesful;
			return flag;
		};
		bool FK_Hitbox::isSingleHit() const {
			return singleHit;
		};
		FK_Hitbox_Type FK_Hitbox::getType() const {
			return type;
		};
		FK_Reaction_Type FK_Hitbox::getReaction() const {
			return reaction;
		};
		/* check if stand guard can stop the attack */
		bool FK_Hitbox::canBeStandGuarded() const {
			if ((attackType & FK_Attack_Type::ThrowAtk) != 0 ){
				if (attackType & (FK_Attack_Type::HighBlockableThrow | FK_Attack_Type::MidBlockableThrow)){
					return true;
				}
				else{
					return false;
				}
			}
			bool guardFlag = false;
			if (attackType != FK_Attack_Type::LowAtk){
				guardFlag = true;
			}
			return (guardFlag && !isGuardBreaking());
		};
		/* check if crouch guard can stop the attack */
		bool FK_Hitbox::canBeCrouchGuarded() const {
			bool guardFlag = false;
			if (attackType != FK_Attack_Type::MidThrowAtk && 
				attackType != FK_Attack_Type::MediumAtk && 
				attackType != FK_Attack_Type::HighAtk &&
				attackType != FK_Attack_Type::MidBlockableThrow){
				guardFlag = true;
			}
			return (guardFlag && !isGuardBreaking());
		};

};