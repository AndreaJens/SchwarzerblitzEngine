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