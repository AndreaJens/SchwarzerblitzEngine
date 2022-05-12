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

#ifndef FK_HITBOX_CLASS
#define FK_HITBOX_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include"FK_Database.h"

using namespace irr;

namespace fk_engine{

	class FK_Hitbox{
	public:
		FK_Hitbox();
		FK_Hitbox(int newStartFrame, int newEndFrame, float newDamage,
			int newPriority, bool guardBreak, bool newSingleHit, FK_Hitbox_Type newType, 
			FK_Reaction_Type newReaction, FK_Attack_Type newAttackType);
		void setup(int newStartFrame, int newEndFrame, float newDamage,
			int newPriority, bool guardBreak, bool newSingleHit, FK_Hitbox_Type newType, 
			FK_Reaction_Type newReaction, FK_Attack_Type newAttackType);
		int getStartingFrame() const;
		int getEndingFrame() const;
		u32 getInternalId() const;
		float getDamage() const;
		int getPriority() const;
		void setPriority(int newPriority);
		void setDamage(float new_damage);
		bool isGuardBreaking() const;
		bool isActive(int frameNumber);
		bool isSingleHit() const;
		bool hasHit() const;
		void setHit(bool hitFlag);
		void setInternalId(u32 newId);
		FK_Hitbox_Type getType() const;
		FK_Reaction_Type getReaction() const;
		FK_Attack_Type getAttackType() const;
		bool canBeStandGuarded() const;
		bool canBeCrouchGuarded() const;
	private:
		u32 internalId;
		int startingFrame;
		int endingFrame;
		float damage;
		int priority;
		bool canIgnoreGuard;
		bool singleHit;
		bool hitSuccesful;
		FK_Hitbox_Type type;
		FK_Reaction_Type reaction;
		FK_Attack_Type attackType;
	};
};
#endif