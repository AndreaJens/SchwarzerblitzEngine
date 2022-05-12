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

#ifndef FK_BULLET_CLASS_DEFINE
#define FK_BULLET_CLASS_DEFINE

#include<irrlicht.h>
#include<string>
#include "FK_Database.h"
#include "FK_Hitbox.h"

using namespace irr;

namespace fk_engine{
	class FK_Bullet{
	public:
		FK_Bullet();
		~FK_Bullet();
		enum BulletOwner : s32{
			Player1 = -1,
			None = 0,
			Player2 = 1,
		};
		struct FK_BulletParticleEmitter{
			std::string textureName;
			s32 maxParticles;
			s32 minParticles;
			s32 duration;
		};
		// setup with individual variables
		void setup(scene::ISceneManager* new_smgr, BulletOwner new_ownerId, 
			std::string new_meshPath, f32 new_range,
			core::vector3df new_position, core::vector3df new_rotation, core::vector3df new_velocity,
			core::vector3df new_hitboxRadius, FK_Hitbox new_hitbox, bool backfaceCulling = false);
		// setup using other bullet pointer as a base
		void setup(FK_Bullet* otherBullet);
		bool createParticleEmitter();
		BulletOwner getOwner();
		scene::ISceneNode* getNode();
		scene::IParticleSystemSceneNode* getParticleEmitter();
		void setPosition(core::vector3df new_position);
		void setRange(f32 new_range);
		void setHitbox(FK_Hitbox new_hitbox);
		void setVelocity(core::vector3df new_velocity);
		void setAcceleration(core::vector3df new_acceleration);
		void setRotation(core::vector3df new_rotation);
		void applyRotation(core::vector3df new_rotation);
		void setIntrinsicRotation(core::vector3df new_rotation);
		void setMeshPath(std::string new_MeshPath);
		void setHitboxRadius(core::vector3df new_radius);
		void setScale(core::vector3df new_scale);
		void setShadowToBullet();
		void setParticleEmitter(std::string textureName, 
			s32 particleEmitterMinParticles = -1, s32 particleEmitterMaxParticles = -1,
			s32 particleEmitterDuration = -1);
		void setOriginalMoveId(u32 moveId);
		void setBackfaceCullingFlag(bool new_flag);
		void setJumpableFlag(bool new_flag);
		std::string getParticleEmitterTextureName();
		FK_BulletParticleEmitter getParticleEmitterData();
		f32 getRange();
		core::vector3df getPosition();
		core::vector3df getRotation();
		core::vector3df getIntrinsicRotation();
		core::vector3df getScale();
		// velocity has to be given in term of parallel, perpendicular and vertical velocity
		core::vector3df getVelocity();
		core::vector3df getAcceleration();
		core::vector3df getHitboxRadius();
		std::string getMeshPath();
		scene::ISceneNode* getHitboxNode();
		FK_Hitbox &getHitbox();
		void update(f32 frame_delta_time = 0);
		void changeOwner(BulletOwner new_owner);
		void reflect();
		bool isDisposeable();
		void markAsBillboard();
		bool getBillboardFlag();
		bool hasLightAssigned();
		bool getBackfaceCullingFlag();
		u32 getOriginalMoveId();
		bool canBeReflected();
		bool canBeJumpedOver();
		void setReflection(bool new_reflectionFlag, f32 new_reflectionMultiplier);
		f32 getReflectionDamageMultiplier();
		void assignShadowEffect();
		bool hasShadow();
		bool canBeRemoved() const;
		bool setupMesh(scene::ISceneManager* smgr);
		void prepareForDisposal();
		void dispose();
	private:
		scene::ISceneManager* smgr;
		//scene::IAnimatedMesh* sphereMesh;
		BulletOwner ownerId;
		f32 range;
		std::string meshFilePath;
		scene::ISceneNode* node;
		scene::ISceneNode* hitboxNode;
		core::vector3df position;
		core::vector3df starting_position;
		core::vector3df rotation;
		core::vector3df intrinsicRotation;
		core::vector3df hitboxRadius;
		core::vector3df velocityPerSecond;
		core::vector3df accelerationPerSecond;
		core::vector3df scale;
		FK_Hitbox hitbox;
		bool removable;
		bool disposeable;
		bool isBillboard;
		bool reflectable;
		u32  originalMoveId;
		f32  reflectDamageScaling;
		bool lightAssignedFlag;
		bool applyShadowFlag;
		bool backfaceCullingFlag;
		bool canBeJumpedOverFlag;
		scene::IParticleSystemSceneNode* particleEmitter;
		s32 disposalCounter;
		FK_BulletParticleEmitter particleEmitterData;
	};
}

#endif