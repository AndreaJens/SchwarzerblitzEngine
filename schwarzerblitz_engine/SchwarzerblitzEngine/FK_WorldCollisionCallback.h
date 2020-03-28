#ifndef FK_WORLDCOLLIDER_CLASS
#define FK_WORLDCOLLIDER_CLASS

#include <irrlicht.h>
#include "FK_Character.h"

using namespace irr;

namespace fk_engine{

	class FK_WorldCollisionCallback : public scene::ICollisionCallback
	{
	public:
		FK_WorldCollisionCallback(FK_Character *new_player);
		virtual bool onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse& animator);
	private:
		FK_Character* player;
	};

	class FK_WorldCollisionCallbackProp : public scene::ICollisionCallback
	{
	public:
		FK_WorldCollisionCallbackProp(ISceneNode* node, FK_Character::FK_SceneProp* prop);
		~FK_WorldCollisionCallbackProp();
		virtual bool onCollision(const irr::scene::ISceneNodeAnimatorCollisionResponse& animator);
	private:
		ISceneNode* node;
		FK_Character::FK_SceneProp* prop;
	};

};

#endif