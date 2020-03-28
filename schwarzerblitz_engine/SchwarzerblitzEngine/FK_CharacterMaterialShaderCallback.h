#ifndef FK_CHARACTERMATERIAL_SHADERCLASS
#define FK_CHARACTERMATERIAL_SHADERCLASS

#include<irrlicht.h>
#include"XEffects.h"
#include"FK_Character.h"

using namespace irr;

namespace fk_engine{

	class FK_CharacterMaterialShaderCallback : public video::IShaderConstantSetCallBack
	{
	public:
		FK_CharacterMaterialShaderCallback();
		FK_CharacterMaterialShaderCallback(IrrlichtDevice* newDevice, EffectHandler* newLightManager, FK_Character* newCharacter);
		virtual void OnSetConstants(video::IMaterialRendererServices* services,
			s32 userData);
	private:
		EffectHandler* lightManager;
		IrrlichtDevice* device;
		FK_Character* character;
	};
}

#endif