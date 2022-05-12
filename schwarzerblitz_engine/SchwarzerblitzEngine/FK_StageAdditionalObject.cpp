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

#include"FK_StageAdditionalObject.h""

using namespace irr;

namespace fk_engine{
	FK_StageAdditionalObject::FK_StageAdditionalObject(){
		position = core::vector3df();
		rotation = core::vector3df();
		scale = core::vector3df(1.f, 1.f, 1.f);
		meshName = std::string();
		particleTextureName = std::string();
		materialEffect = video::E_MATERIAL_TYPE::EMT_LIGHTMAP_M4;
		particleTextureName = std::string();
		circAnimator_center = core::vector3df();
		circAnimator_rotationAxis = core::vector3df(0, 1, 0);
		circAnimator_radius = 0.f;
		circAnimator_velocity = 0.f;
		linAnimator_startPoint = core::vector3df();
		linAnimator_endPoint = core::vector3df();
		linAnimator_velocity = 0.f;
		linAnimator_loop = false;
		linAnimator_pingpong = false;
		backface_culling = false;
		wireframeFlag = false;
		animatorType = FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType::NoAnimator;
	}
	std::string FK_StageAdditionalObject::getMeshName(){
		return meshName;
	};
	std::string FK_StageAdditionalObject::getParticleTextureName(){
		return particleTextureName;
	};
	core::vector3df FK_StageAdditionalObject::getPosition(){
		return position;
	};
	core::vector3df FK_StageAdditionalObject::getRotation(){ 
		return rotation; 
	}
	core::vector3df FK_StageAdditionalObject::getScale()
	{
		return scale;
	}
	FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType FK_StageAdditionalObject::getAnimatorType(){
		return animatorType;
	}
	video::E_MATERIAL_TYPE FK_StageAdditionalObject::getMaterialEffect(){
		return materialEffect;
	}

	bool FK_StageAdditionalObject::getBackfaceCulling(){
		return backface_culling;
	}

	bool FK_StageAdditionalObject::getWireframeFlag(){
		return wireframeFlag;
	}

	bool FK_StageAdditionalObject::isParticleSystem(){
		return !particleTextureName.empty();
	}

	bool FK_StageAdditionalObject::isBuildable(){
		return (isParticleSystem() || !(meshName.empty()));
	}

	scene::ISceneNodeAnimator* FK_StageAdditionalObject::setupAnimator(scene::ISceneManager* smgr){
		if (animatorType == FK_StageAdditionalObject_AnimatorType::CircularAnimator){
			return smgr->createFlyCircleAnimator(circAnimator_center, circAnimator_radius, 
				circAnimator_velocity, circAnimator_rotationAxis);
		}
		else if (animatorType == FK_StageAdditionalObject_AnimatorType::LinearLoopAnimator){
			u32 time_ms = (u32)floor(1000*(linAnimator_startPoint.getDistanceFrom(linAnimator_endPoint) / linAnimator_velocity));
			return smgr->createFlyStraightAnimator(linAnimator_startPoint, linAnimator_endPoint,
				time_ms, true, false);
		}
		else if (animatorType == FK_StageAdditionalObject_AnimatorType::LinearPingPongAnimator){
			u32 time_ms = (u32)floor(1000*(linAnimator_startPoint.getDistanceFrom(linAnimator_endPoint) / linAnimator_velocity));
			return smgr->createFlyStraightAnimator(linAnimator_startPoint, linAnimator_endPoint,
				time_ms, true, true);
		}
		return NULL;
	}

	void FK_StageAdditionalObject::setAnimatorType(FK_StageAdditionalObject::FK_StageAdditionalObject_AnimatorType new_animatorType){
		animatorType = new_animatorType;
	}

	void FK_StageAdditionalObject::setCircularAnimatorParameters(core::vector3df center, core::vector3df rotationAxis,
		f32 radius, f32 velocity){
		circAnimator_center = center;
		circAnimator_rotationAxis = rotationAxis;
		circAnimator_radius = radius;
		circAnimator_velocity = velocity;
	}

	void FK_StageAdditionalObject::setLinearAnimatorParameters(core::vector3df startingPoint, core::vector3df endingPoint,
		f32 velocity, bool loopFlag, bool pingpongFlag){
		linAnimator_startPoint = startingPoint;
		linAnimator_endPoint = endingPoint;
		linAnimator_velocity = velocity;
		linAnimator_loop = loopFlag;
		linAnimator_pingpong = pingpongFlag;
	}
	
	void FK_StageAdditionalObject::setMeshName(std::string new_meshName){
		meshName = new_meshName;
	}
	void FK_StageAdditionalObject::setParticleTextureName(std::string new_meshName){
		particleTextureName = new_meshName;
	}
	void FK_StageAdditionalObject::setPosition(core::vector3df new_position){
		position = new_position;
	}
	void FK_StageAdditionalObject::setRotation(core::vector3df new_rotation){
		rotation = new_rotation;
	}
	void FK_StageAdditionalObject::setScale(core::vector3df newScale)
	{
		scale = newScale;
	}
	void FK_StageAdditionalObject::setMaterialEffect(video::E_MATERIAL_TYPE new_effect){
		materialEffect = new_effect;
	}
	void FK_StageAdditionalObject::setWireframeFlag(){
		wireframeFlag = true;
	}
	void FK_StageAdditionalObject::setBackfaceCulling(bool new_flag){
		backface_culling = new_flag;
	}
	void FK_StageAdditionalObject::update(u32 deltaT_ms, core::vector3df nodePosition){
		if (animatorType == FK_StageAdditionalObject_AnimatorType::CircularAnimator){
			core::vector3df orientation = core::vector3df(nodePosition.Z, 0, -nodePosition.X);
			orientation = orientation * circAnimator_velocity / abs(circAnimator_velocity);
			rotation = orientation.getHorizontalAngle();
			//rotation = circAnimator_rotationAxis * circAnimator_velocity * deltaT_ms * 180 / core::PI;
		}
	}
}