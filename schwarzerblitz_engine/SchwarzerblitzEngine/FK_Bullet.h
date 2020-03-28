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