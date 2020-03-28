#include "FK_Bullet.h"
#include<iostream>

using namespace irr;

namespace fk_engine{
	//! core initialization - setup all varaibles to default value
	FK_Bullet::FK_Bullet(){
		ownerId = BulletOwner::None;
		position = core::vector3df(0, 0, 0);
		rotation = core::vector3df(0, 0, 0);
		intrinsicRotation = core::vector3df(0, 0, 0);
		scale = core::vector3df(1.f, 1.f, 1.f);
		velocityPerSecond = core::vector3df(0, 0, 0);
		hitbox = FK_Hitbox();
		range = 0;
		meshFilePath = std::string();
		hitboxRadius = core::vector3df(0, 0, 0);
		removable = false;
		isBillboard = false;
		lightAssignedFlag = false;
		reflectDamageScaling = 1.0;
		reflectable = false;
		applyShadowFlag = false;
		disposalCounter = -1;
		particleEmitterData.maxParticles = 1000;
		particleEmitterData.minParticles = 500;
		particleEmitterData.duration = 500;
		backfaceCullingFlag = false;
		canBeJumpedOverFlag = false;
		hitboxNode = NULL;
		particleEmitter = NULL;
		originalMoveId = 0;
		node = NULL;
	};
	//! destructor 
	FK_Bullet::~FK_Bullet(){
		dispose();
	}
	//! setup the bullet. If new_smgr is equal to NULL, the bullet is just a container of all the variables which can be used
	// for example while reading the bullet characteristics from a moveset file
	void FK_Bullet::setup(scene::ISceneManager* new_smgr, FK_Bullet::BulletOwner new_ownerId,
		std::string new_meshPath, f32 new_range, core::vector3df new_position, 
		core::vector3df new_rotation, core::vector3df new_velocity,
		core::vector3df new_hitboxRadius, FK_Hitbox new_hitbox, bool backfaceCulling){
		smgr = new_smgr;
		ownerId = new_ownerId;
		starting_position = new_position;
		position = new_position;
		rotation = new_rotation;
		velocityPerSecond = new_velocity;
		range = new_range;
		meshFilePath = new_meshPath;
		hitboxRadius = new_hitboxRadius;
		hitbox = new_hitbox;
		reflectDamageScaling = 1.0;
		reflectable = false;
		particleEmitter = NULL;
		backfaceCullingFlag = backfaceCulling;
		if (smgr != NULL){
			setupMesh(smgr);
		}
	};

	//! setup the bullet using another bullet as a base
	void FK_Bullet::setup(FK_Bullet* otherBullet){
		ownerId = otherBullet->getOwner();
		position = otherBullet->getPosition();
		intrinsicRotation = otherBullet->getIntrinsicRotation();
		rotation = otherBullet->getRotation();
		velocityPerSecond = otherBullet->getVelocity();
		range = otherBullet->getRange();
		scale = otherBullet->getScale();
		meshFilePath = otherBullet->getMeshPath();
		backfaceCullingFlag = otherBullet->getBackfaceCullingFlag();
		hitboxRadius = otherBullet->getHitboxRadius();
		hitbox = otherBullet->getHitbox();
		hitbox.setHit(false);
		isBillboard = otherBullet->getBillboardFlag();
		reflectable = otherBullet->canBeReflected();
		reflectDamageScaling = otherBullet->getReflectionDamageMultiplier();
		particleEmitterData = otherBullet->getParticleEmitterData();
		applyShadowFlag = otherBullet->hasShadow();
		canBeJumpedOverFlag = otherBullet->canBeJumpedOver();
		originalMoveId = otherBullet->getOriginalMoveId();
	}

	//! setup mesh
	bool FK_Bullet::setupMesh(scene::ISceneManager* new_smgr){
		if (new_smgr == NULL && smgr == NULL){
			return false;
		}
		if (smgr == NULL){
			smgr = new_smgr;
		}
		// if the bullet is a billboard, the mesh file is instead used as a texture reference for "skinning" the billboard
		if (isBillboard){
			video::ITexture* tex = smgr->getVideoDriver()->getTexture(meshFilePath.c_str());
			if (!tex) {
				return false;
			}
			f32 scale_factor = 1.2f;
			core::dimension2d<f32> billBoardSize = core::dimension2d<f32>(hitboxRadius.Z, hitboxRadius.Y) * scale_factor;
			node = smgr->addBillboardSceneNode(0, billBoardSize, position);
			node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
			int mcount = node->getMaterialCount();
			for (int i = 0; i < mcount; i++){
				node->getMaterial(i).BackfaceCulling = false;
				node->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
				node->getMaterial(i).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			}
			node->setMaterialTexture(0, tex);
		}
		else{
			scene::IAnimatedMesh* mesh = smgr->getMesh(meshFilePath.c_str());
			if (!mesh) {
				return false;
			}
			node = smgr->addAnimatedMeshSceneNode(mesh, 0, -1, position, rotation);
			int mcount = node->getMaterialCount();
			for (int i = 0; i < mcount; i++){
				node->getMaterial(i).BackfaceCulling = backfaceCullingFlag;
				node->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
				node->getMaterial(i).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			}
			node->setScale(scale);
			((scene::IAnimatedMeshSceneNode*)node)->setTransitionTime(0.f);
			f32 frameNrStart = (f32)((scene::IAnimatedMeshSceneNode*)node)->getStartFrame();
			((scene::IAnimatedMeshSceneNode*)node)->setCurrentFrame(frameNrStart, true);
		}
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		// get maximal radius
		f32 maxRadius = hitboxRadius.X;
		if (hitboxRadius.Y > maxRadius){
			maxRadius = hitboxRadius.Y;
		}
		if (hitboxRadius.Z > maxRadius){
			maxRadius = hitboxRadius.Z;
		}
		hitboxNode = smgr->addSphereSceneNode(maxRadius);
		hitboxNode->setParent(node);
		hitboxNode->setPosition(core::vector3df(0, 0, 0));
		core::vector3df scale_factor = hitboxRadius / maxRadius;
		hitboxNode->setScale(hitboxRadius / maxRadius);
		hitboxNode->setMaterialFlag(video::EMF_LIGHTING, false);
		hitboxNode->setMaterialFlag(video::EMF_WIREFRAME, true);
		hitboxNode->setDebugDataVisible(scene::EDS_BBOX);
		hitboxNode->setVisible(false);
		if (particleEmitterData.textureName != std::string()){
			createParticleEmitter();
		}
		node->OnRegisterSceneNode();
		return true;
	}
	//! get bullet node
	scene::ISceneNode* FK_Bullet::getNode(){
		return node;
	}

	//! set backface culling settings
	void FK_Bullet::setBackfaceCullingFlag(bool newFlag){
		backfaceCullingFlag = newFlag;
	}

	void FK_Bullet::setJumpableFlag(bool new_flag)
	{
		canBeJumpedOverFlag = new_flag;
	}

	//! get backface culling settings
	bool FK_Bullet::getBackfaceCullingFlag(){
		return backfaceCullingFlag;
	}

	u32 FK_Bullet::getOriginalMoveId()
	{
		return originalMoveId;
	}

	//! set particle emitter texture
	void FK_Bullet::setParticleEmitter(std::string textureName, s32 minParticleNumber, s32 maxParticleNumber, s32 duration){
		particleEmitterData.textureName = textureName;
		if (minParticleNumber > 0){
			particleEmitterData.minParticles = minParticleNumber;
		}
		if (maxParticleNumber > 0){
			particleEmitterData.maxParticles = maxParticleNumber;
		}
		if (duration > 0){
			particleEmitterData.duration = duration;
		}
	}

	void FK_Bullet::setOriginalMoveId(u32 moveId)
	{
		originalMoveId = moveId;
	}

	//! set particle emitter texture
	std::string FK_Bullet::getParticleEmitterTextureName(){
		return particleEmitterData.textureName;
	}

	//! set particle emitter texture
	FK_Bullet::FK_BulletParticleEmitter FK_Bullet::getParticleEmitterData(){
		return particleEmitterData;
	}

	//! get particle emitter (if any)
	scene::IParticleSystemSceneNode* FK_Bullet::getParticleEmitter(){
		return particleEmitter;
	}

	//! create particle emitter
	bool FK_Bullet::createParticleEmitter(){
		video::ITexture* tex = smgr->getVideoDriver()->getTexture(particleEmitterData.textureName.c_str());
		if (!tex) {
			return false;
		}
		particleEmitter = smgr->addParticleSystemSceneNode(false, getNode());
		scene::IParticleEmitter* em = particleEmitter->createMeshEmitter(
			((scene::IAnimatedMeshSceneNode*)getNode())->getMesh(), true,// emitter size
			core::vector3df(0.0f, 0.1f, 0.0f),   // initial direction
			100.f, -1, false,
			particleEmitterData.minParticles,
			particleEmitterData.maxParticles,           // emit rate (0 assures that no particles are created until a hit)
			video::SColor(0, 255, 255, 255),       // darkest color
			video::SColor(0, 255, 255, 255),       // brightest color
			200, particleEmitterData.duration, 360,     // min and max age, angle
			core::dimension2df(8.0f, 8.0f),         // min size
			core::dimension2df(22.0f, 22.0f));        // max size

		particleEmitter->setEmitter(em); // this grabs the emitter
		em->drop(); // so we can drop it here without deleting it
		particleEmitter->setScale(getNode()->getAbsoluteTransformation().getScale());
		particleEmitter->setMaterialFlag(video::EMF_LIGHTING, false);
		particleEmitter->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		particleEmitter->setMaterialTexture(0, tex);
		particleEmitter->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		// set particle expiration time to 200 ms.
		return true;
	}

	//! set position
	void FK_Bullet::setPosition(core::vector3df new_position){
		position = new_position;
		starting_position = new_position;
	}
	//! set range
	void FK_Bullet::setRange(f32 new_range){
		range = new_range;
	}
	//! set hitbox
	void FK_Bullet::setHitbox(FK_Hitbox new_hitbox){
		hitbox = new_hitbox;
	}
	//! set velocity
	void FK_Bullet::setVelocity(core::vector3df new_velocity){ 
		velocityPerSecond = new_velocity; 
	}
	//! set hitbox radius
	void FK_Bullet::setHitboxRadius(core::vector3df new_radius){
		hitboxRadius = new_radius;
	}
	//! set rotation
	void FK_Bullet::setRotation(core::vector3df new_rotation){
		rotation = new_rotation;
	}
	//! apply rotation on top of intrinsic rotation
	void FK_Bullet::applyRotation(core::vector3df new_rotation){
		f32 angleY = atan2(velocityPerSecond.Y, velocityPerSecond.X) * 180.f / core::PI;
		core::vector3df baseRotation = core::vector3df(0, angleY, 0);
		rotation = intrinsicRotation + baseRotation + new_rotation;
	}
	//! set intrinsic rotation
	void FK_Bullet::setIntrinsicRotation(core::vector3df new_rotation){
		intrinsicRotation = new_rotation;
	}
	//! set scale
	void FK_Bullet::setScale(core::vector3df new_scale){
		scale = new_scale;
	}
	//! set mesh path
	void FK_Bullet::setMeshPath(std::string new_MeshPath){
		meshFilePath = new_MeshPath;
	}

	//! get bullet owner
	FK_Bullet::BulletOwner FK_Bullet::getOwner(){
		return ownerId;
	};

	//! get bullet range
	f32 FK_Bullet::getRange(){
		return range;
	};

	//! get hitbox
	FK_Hitbox& FK_Bullet::getHitbox(){
		return hitbox;
	}

	//! get position
	core::vector3df FK_Bullet::getPosition(){
		return position;
	}
	//! get rotation
	core::vector3df FK_Bullet::getRotation(){
		return rotation;
	}
	//! get intrinsic rotation
	core::vector3df FK_Bullet::getIntrinsicRotation(){
		return intrinsicRotation;
	}
	//! get position
	core::vector3df FK_Bullet::getScale(){
		return scale;
	}
	//! get velocity
	core::vector3df FK_Bullet::getVelocity(){
		return velocityPerSecond;
	}
	//! get hitbox radius
	core::vector3df FK_Bullet::getHitboxRadius(){
		return hitboxRadius;
	}
	//! get hitbox node for collision purpose
	scene::ISceneNode* FK_Bullet::getHitboxNode(){
		return hitboxNode;
	}

	//! get billboard flag
	bool FK_Bullet::getBillboardFlag(){
		return isBillboard;
	}

	//! get if bullet can be reflected
	bool FK_Bullet::canBeReflected(){
		return reflectable;
	}

	bool FK_Bullet::canBeJumpedOver()
	{
		return canBeJumpedOverFlag;
	}
	//! set damage scaling factor after reflection
	void FK_Bullet::setReflection(bool new_reflectionFlag, f32 new_reflectionMultiplier){
		reflectable = new_reflectionFlag;
		reflectDamageScaling = new_reflectionMultiplier;
	};

	//! get reflection damage multiplier
	f32 FK_Bullet::getReflectionDamageMultiplier(){
		return reflectDamageScaling;
	}

	bool FK_Bullet::hasLightAssigned(){
		return lightAssignedFlag;
	}
	void FK_Bullet::assignShadowEffect(){
		lightAssignedFlag = true;
	}
	bool FK_Bullet::hasShadow(){
		return applyShadowFlag;
	}
	void FK_Bullet::setShadowToBullet(){
		applyShadowFlag = true;
	}

	//! get mesh name
	std::string FK_Bullet::getMeshPath(){
		return meshFilePath;
	}

	//! mark as billboard (i.e. a 2d sprite in 3d)
	void  FK_Bullet::markAsBillboard(){
		isBillboard = true;
	}

	//! update bullet position
	void FK_Bullet::update(f32 frame_delta_time){
		if (disposalCounter > 0){
			disposalCounter -= (s32)std::round(1000.f * frame_delta_time);
			if (disposalCounter <= 0){
				removable = true;
				//dispose();
				return;
			}
		}
		else{
			core::vector3df realVelocity;
			float angle_degree = rotation.Y - intrinsicRotation.Y;
			float angle = angle_degree * 3.1415926f / 180.0f;
			// transform coordinates
			f32 velocity = 0.f;
			if (velocityPerSecond.Y == 0.f) {
				velocity = velocityPerSecond.X;
			}else{
				velocity = sqrt(velocityPerSecond.X*velocityPerSecond.X + velocityPerSecond.Y*velocityPerSecond.Y);
			}
			realVelocity.Z = velocity * cos(angle);// +velocityPerSecond.Y * sin(angle);
			realVelocity.X = velocity * sin(angle);// -velocityPerSecond.Y * cos(angle);
			realVelocity.Y = velocityPerSecond.Z;
			// update position
			position.X += realVelocity.X * frame_delta_time;
			position.Z += realVelocity.Z * frame_delta_time;
			position.Y += realVelocity.Y * frame_delta_time;
			//std::cout << angle_degree << " " << realVelocity.X << " " << realVelocity.Z << std::endl;
			// update node position
			node->setPosition(position);
			hitboxNode->updateAbsolutePosition();
		}
	};

	//! change bullet owner
	void FK_Bullet::changeOwner(FK_Bullet::BulletOwner new_owner){
		ownerId = new_owner;
	}
	//! reflect bullet
	void FK_Bullet::reflect(){
		rotation.Y = 180 + rotation.Y;
		hitbox.setDamage(hitbox.getDamage() * reflectDamageScaling);
		//velocityPerSecond *= 1.2;
	}
	//! check if the bullet can be disposed of
	bool FK_Bullet::isDisposeable(){
		if (range <= 0){
			return false;
		}
		if (disposalCounter > 0){
			return false;
		}
		bool rangeFlag = (position - starting_position).getLength() >= range;
		return ((hitbox.hasHit() && hitbox.isSingleHit()) || rangeFlag);
	};

	//! dispose of the bullet
	void FK_Bullet::prepareForDisposal() {
		if (particleEmitter != NULL && disposalCounter < 0 && !removable) {
			disposalCounter = 1000;
			particleEmitter->getEmitter()->setMaxParticlesPerSecond(0);
			particleEmitter->setParent(smgr->getRootSceneNode());
			node->setVisible(false);
			particleEmitter->setVisible(true);
		}
		else {
			removable = true;
		}
	};
	//! dispose of the bullet
	void FK_Bullet::dispose(){
		if (hitboxNode != NULL && hitboxNode->getReferenceCount() > 0) {
			hitboxNode->remove();
			hitboxNode = NULL;
		}
		if (particleEmitter != NULL && particleEmitter->getReferenceCount() > 0) {
			particleEmitter->remove();
			particleEmitter = NULL;
		}
		if (node != NULL && node->getReferenceCount() > 0) {
			node->remove();
			node = NULL;
		}
		removable = true;
	};
	//! check if bullet can be removed
	bool FK_Bullet::canBeRemoved() const{
		return removable;
	}
}