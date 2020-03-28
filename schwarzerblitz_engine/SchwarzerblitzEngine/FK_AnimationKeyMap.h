#ifndef IRRLICHT_EXTEND_ANIMATION_KEY_MAP_H
#define IRRLICHT_EXTEND_ANIMATION_KEY_MAP_H

#include "ISkinnedMesh.h"
#include<map>

using namespace irr;
using namespace scene;

namespace fk_engine{
		class FK_AnimationKeyMap{
		public: 
			FK_AnimationKeyMap();
			void reset();
			bool addPositionKey(core::stringw jointName, ISkinnedMesh::SPositionKey newKey);
			bool addRotationKey(core::stringw jointName, ISkinnedMesh::SRotationKey newKey);
			bool addScaleKey(core::stringw jointName, ISkinnedMesh::SScaleKey newKey);
			bool setPositionArray(core::stringw jointName, core::array<ISkinnedMesh::SPositionKey> newKeys);
			bool setRotationArray(core::stringw jointName, core::array<ISkinnedMesh::SRotationKey> newKeys);
			bool setScaleArray(core::stringw jointName, core::array<ISkinnedMesh::SScaleKey> newKeys);
			bool setOffsetRotation(core::stringw jointName, core::quaternion);
			bool setOffsetPosition(core::stringw jointName, core::vector3df);
			bool setOffsetScale(core::stringw jointName, core::vector3df);
			core::array<ISkinnedMesh::SPositionKey> & getPositionKeys(core::stringw jointName);
			core::array<ISkinnedMesh::SScaleKey> & getScaleKeys(core::stringw jointName);
			core::array<ISkinnedMesh::SRotationKey>& getRotationKeys(core::stringw jointName);
			core::quaternion getOffsetRotation(core::stringw jointName);
			core::vector3df getOffsetScale(core::stringw jointName);
			core::vector3df getOffsetPosition(core::stringw jointName);
			u32 getFrameCount();
			void setFrameCount(u32 newFrameCount);
			void makeAnimationConsistentWithOffsets(ISkinnedMesh* mesh);
		private:
			//! Animation keys causing translation change
			std::map<core::stringc, core::array<ISkinnedMesh::SPositionKey>> PositionKeys;
			//! Animation keys causing scale change
			std::map<core::stringc, core::array<ISkinnedMesh::SScaleKey>> ScaleKeys;
			//! Animation keys causing rotation change
			std::map<core::stringc, core::array<ISkinnedMesh::SRotationKey>> RotationKeys;
			//! Offset rotations to be subtracted in case the model has a different skeleton conformation
			std::map<core::stringc, core::quaternion> AnimationReferenceSkeleton_Rotation;
			std::map<core::stringc, core::vector3df> AnimationReferenceSkeleton_Position;
			std::map<core::stringc, core::vector3df> AnimationReferenceSkeleton_Scale;
			/* number of frames*/
			u32 frameCount;
		};
}

#endif