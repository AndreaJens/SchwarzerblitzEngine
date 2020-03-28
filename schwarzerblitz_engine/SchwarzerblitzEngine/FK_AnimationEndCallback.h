#ifndef FK_ANIMATIONCALLBACK_CLASS
#define FK_ANIMATIONCALLBACK_CLASS
#include<irrlicht.h>
#include"FK_Character.h"

using namespace irr;

namespace fk_engine{

	class FK_Character;

	class FK_AnimationEndCallback : public scene::IAnimationEndCallBack
	{
	public:
		// constructor
		FK_AnimationEndCallback(FK_Character* new_character);
		// destructor
		~FK_AnimationEndCallback();
		// callback event
		void OnAnimationEnd(scene::IAnimatedMeshSceneNode* node);
		// get character
		FK_Character* getCharacter();
	private:
		FK_Character* character;
	};
};

#endif