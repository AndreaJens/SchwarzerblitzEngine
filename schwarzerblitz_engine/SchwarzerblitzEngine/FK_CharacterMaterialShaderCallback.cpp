#include"FK_CharacterMaterialShaderCallback.h"

using namespace irr;

namespace fk_engine{

	// default costructor
	FK_CharacterMaterialShaderCallback::FK_CharacterMaterialShaderCallback() : IShaderConstantSetCallBack(){
		device = NULL;
		lightManager = NULL;
		character = NULL;
	}

	// complete constructor
	FK_CharacterMaterialShaderCallback::FK_CharacterMaterialShaderCallback(
		IrrlichtDevice* newDevice, EffectHandler* newManager, FK_Character* newCharacter) : 
		IShaderConstantSetCallBack(){
		device = newDevice;
		lightManager = newManager;
		character = newCharacter;
	}

	// main callback for pixel/vertex shader
	void FK_CharacterMaterialShaderCallback::OnSetConstants(video::IMaterialRendererServices* services, s32 userData){
		video::IVideoDriver* driver = services->getVideoDriver();

		// set inverted world matrix
		// if we are using highlevel shaders (the user can select this when
		// starting the program), we must set the constants by name.

		//core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
		////invWorld.makeInverse();
		//services->setVertexShaderConstant("mWorld", invWorld.pointer(), 16);
	
		// set clip matrix

		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);

		services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

		core::vector3df pos = device->getSceneManager()->
			getActiveCamera()->getAbsolutePosition();

		//services->setVertexShaderConstant("mCameraPosition", reinterpret_cast<f32*>(&pos), 3);
	
		// set light color and position
		//video::SColorf lightColor = video::SColorf(1.0f, 1.0f, 1.0f, 0.0f);
		core::vector3df lightPosition = core::vector3df(0.f, 0.f, 0.f);
		core::vector3df lightDirection = core::vector3df(0.f, -1.f, 0.f);
		video::SColorf ambientColor = video::SColorf(0.9f, 0.9f, 0.9f, 1.f);
		video::SColorf lightColor = video::SColorf(1.f, 1.f, 1.f, 1.f);
		video::SColorf multipicationColor = video::SColorf(1.f, 1.f, 1.f, 1.f);
		video::SColorf additionalColor = video::SColorf(0.f, 0.f, 0.f, 1.f);
		FK_Attack_Type attackTypeFlag = (FK_Attack_Type)(FK_Attack_Type::AllAtk | FK_Attack_Type::BulletAttack);
		if ((character->hasActiveArmorSpecialEffect() && 
			character->hasArmor(attackTypeFlag)) ||
			character->isTriggerGuardActive() ||
			(character->getCurrentMove() != NULL && character->hasArmor(attackTypeFlag) &&
			character->getCurrentMove()->getName() == "SupineReversal")){
			additionalColor = video::SColorf(0.f, 0.3f, 0.3f, 1.f);
		}
		else if (character->hasActiveTriggerCancelSpecialEffect()) {
			additionalColor = video::SColorf(0.3f, 0.3f, 0.0f, 1.f);
		}

		if (lightManager != NULL){

			//lightColor = lightManager->getShadowLight(0).getLightColor();
			lightPosition = lightManager->getShadowLight(0).getPosition();
			lightDirection =
				lightManager->getShadowLight(0).getTarget() - lightManager->getShadowLight(0).getPosition();
			lightDirection = lightDirection.normalize();
			ambientColor = lightManager->getAmbientColor();
			lightColor = lightManager->getShadowLight(0).getLightColor();
		}

		services->setVertexShaderConstant("mAmbientColor", reinterpret_cast<f32*>(&ambientColor), 4);
		services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(&lightColor), 4);
		//services->setVertexShaderConstant("mLightPosition", reinterpret_cast<f32*>(&lightPosition), 3);
		//services->setVertexShaderConstant("mLightDirection", reinterpret_cast<f32*>(&lightDirection), 3);
		//f32 timeNow = (f32)device->getTimer()->getTime() / 1000.f;
		//services->setVertexShaderConstant("mTimeProgression", reinterpret_cast<f32*>(&timeNow), 1);
		services->setPixelShaderConstant("mMultiplicationColor", reinterpret_cast<f32*>(&multipicationColor), 4);
		services->setPixelShaderConstant("mAdditionColor", reinterpret_cast<f32*>(&additionalColor), 4);
		// set transposed world matrix

		//core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		//world = world.getTransposed();
		//services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

	}
}