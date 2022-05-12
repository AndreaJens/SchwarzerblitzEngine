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