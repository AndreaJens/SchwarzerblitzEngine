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