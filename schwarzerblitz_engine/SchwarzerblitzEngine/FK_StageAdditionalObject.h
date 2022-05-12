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

#pragma once
#include<irrlicht.h>
#include<string>

using namespace irr;

namespace fk_engine{
	class FK_StageAdditionalObject{
	public:
		enum FK_StageAdditionalObject_AnimatorType{
			NoAnimator,
			CircularAnimator,
			LinearPingPongAnimator,
			LinearLoopAnimator,
		};
		FK_StageAdditionalObject();
		std::string getMeshName();
		std::string getParticleTextureName();
		core::vector3df getPosition();
		core::vector3df getRotation();
		core::vector3df getScale();
		FK_StageAdditionalObject_AnimatorType getAnimatorType();
		video::E_MATERIAL_TYPE getMaterialEffect();
		scene::ISceneNodeAnimator* setupAnimator(scene::ISceneManager*);
		bool getBackfaceCulling();
		bool getWireframeFlag();
		void setAnimatorType(FK_StageAdditionalObject_AnimatorType);
		void setCircularAnimatorParameters(core::vector3df center, core::vector3df rotationAxis, f32 radius, f32 velocity);
		void setLinearAnimatorParameters(core::vector3df startingPoint, core::vector3df endingPoint, f32 velocity, bool loop, bool pingpong);
		void setMeshName(std::string);
		void setPosition(core::vector3df position);
		void setRotation(core::vector3df rotation);
		void setScale(core::vector3df newScale);
		void setMaterialEffect(video::E_MATERIAL_TYPE);
		void setParticleTextureName(std::string new_name);
		void setBackfaceCulling(bool new_flag);
		void setWireframeFlag();
		void update(u32 deltaT_ms, core::vector3df nodePosition);
		bool isParticleSystem();
		bool isBuildable();
	private:
		std::string particleTextureName;
		core::vector3df position;
		core::vector3df rotation;
		core::vector3df scale;
		FK_StageAdditionalObject_AnimatorType animatorType;
		std::string meshName;
		video::E_MATERIAL_TYPE materialEffect;
		core::vector3df circAnimator_center;
		core::vector3df circAnimator_rotationAxis;
		f32 circAnimator_radius;
		f32 circAnimator_velocity;
		core::vector3df linAnimator_startPoint;
		core::vector3df linAnimator_endPoint;
		f32 linAnimator_velocity;
		bool linAnimator_loop;
		bool linAnimator_pingpong;
		bool backface_culling;
		bool wireframeFlag;
	};
}