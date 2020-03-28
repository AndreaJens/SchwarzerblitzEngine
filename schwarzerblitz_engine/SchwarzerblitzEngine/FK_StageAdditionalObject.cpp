#include"FK_StageAdditionalObject.h""

using namespace irr;

namespace fk_engine{
	FK_StageAdditionalObject::FK_StageAdditionalObject(){
		position = core::vector3df();
		rotation = core::vector3df();
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