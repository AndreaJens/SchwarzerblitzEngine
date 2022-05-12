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