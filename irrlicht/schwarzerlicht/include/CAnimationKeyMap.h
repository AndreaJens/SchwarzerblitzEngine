#ifndef IRRLICHT_EXTEND_ANIMATION_KEY_MAP_H
#define IRRLICHT_EXTEND_ANIMATION_KEY_MAP_H

#include "ISkinnedMesh.h"
#include<map>

namespace irr
{
	namespace scene{
		class CAnimationKeyMap{
		public: 
			CAnimationKeyMap();
			void reset();
			bool addPositionKey(core::stringw jointName, ISkinnedMesh::SPositionKey newKey);
			bool addRotationKey(core::stringw jointName, ISkinnedMesh::SRotationKey newKey);
			bool addScaleKey(core::stringw jointName, ISkinnedMesh::SScaleKey newKey);
			core::array<ISkinnedMesh::SPositionKey> & getPositionKeys(core::stringw jointName);
			core::array<ISkinnedMesh::SScaleKey> & getScaleKeys(core::stringw jointName);
			core::array<ISkinnedMesh::SRotationKey>& getRotationKeys(core::stringw jointName);
		private:
			//! Animation keys causing translation change
			std::map<core::stringc, core::array<ISkinnedMesh::SPositionKey>> PositionKeys;
			//! Animation keys causing scale change
			std::map<core::stringc, core::array<ISkinnedMesh::SScaleKey>> ScaleKeys;
			//! Animation keys causing rotation change
			std::map<core::stringc, core::array<ISkinnedMesh::SRotationKey>> RotationKeys;
		};
	}
}

#endif