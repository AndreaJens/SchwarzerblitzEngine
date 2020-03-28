#include "FK_AnimationKeyMap.h"
#include<iostream>
#include<fstream>

namespace fk_engine{
		FK_AnimationKeyMap::FK_AnimationKeyMap(){
			reset();
		};
		void FK_AnimationKeyMap::reset(){
			frameCount = 0;
			PositionKeys.clear();
			RotationKeys.clear();
			ScaleKeys.clear();
		};
		// add position key
		bool FK_AnimationKeyMap::addPositionKey(core::stringw jointName, ISkinnedMesh::SPositionKey newKey){
			if (PositionKeys.count(jointName) == 0){
				PositionKeys[jointName] = core::array< ISkinnedMesh::SPositionKey >();
			}
			PositionKeys[jointName].push_back(newKey);
			return true;
		};
		// add rotation key
		bool FK_AnimationKeyMap::addRotationKey(core::stringw jointName, ISkinnedMesh::SRotationKey newKey){
			if (RotationKeys.count(jointName) == 0){
				RotationKeys[jointName] = core::array< ISkinnedMesh::SRotationKey >();
			}
			RotationKeys[jointName].push_back(newKey);
			return true;
		};
		// add scale key
		bool FK_AnimationKeyMap::addScaleKey(core::stringw jointName, ISkinnedMesh::SScaleKey newKey){
			if (ScaleKeys.count(jointName) == 0){
				ScaleKeys[jointName] = core::array< ISkinnedMesh::SScaleKey >();
			}
			ScaleKeys[jointName].push_back(newKey);
			return true;
		}
		bool FK_AnimationKeyMap::setOffsetRotation(core::stringw jointName, core::quaternion offsetRotation){
			if (AnimationReferenceSkeleton_Rotation.count(jointName) == 0){
				AnimationReferenceSkeleton_Rotation[jointName] = core::quaternion(0.f, 0.f, 0.f);
			}
			AnimationReferenceSkeleton_Rotation[jointName] = offsetRotation;
			return true;
		}
		bool FK_AnimationKeyMap::setOffsetPosition(core::stringw jointName, core::vector3df offsetPosition)
		{
			if (AnimationReferenceSkeleton_Position.count(jointName) == 0){
				AnimationReferenceSkeleton_Position[jointName] = core::vector3df();
			}
			AnimationReferenceSkeleton_Position[jointName] = offsetPosition;
			return true;
		}
		bool FK_AnimationKeyMap::setOffsetScale(core::stringw jointName, core::vector3df offsetScale)
		{
			if (AnimationReferenceSkeleton_Scale.count(jointName) == 0){
				AnimationReferenceSkeleton_Scale[jointName] = core::vector3df(1.0f, 1.0f, 1.0f);
			}
			AnimationReferenceSkeleton_Scale[jointName] = offsetScale;
			return true;
		}


		core::array<ISkinnedMesh::SPositionKey> & FK_AnimationKeyMap::getPositionKeys(core::stringw jointName){
			if (PositionKeys.count(jointName) == 0){
				PositionKeys[jointName] = core::array< ISkinnedMesh::SPositionKey >();
			}
			return PositionKeys[jointName];
		};
		core::array<ISkinnedMesh::SScaleKey> & FK_AnimationKeyMap::getScaleKeys(core::stringw jointName){
			if (ScaleKeys.count(jointName) == 0){
				ScaleKeys[jointName] = core::array< ISkinnedMesh::SScaleKey >();
			}
			return ScaleKeys[jointName];
		};
		core::array<ISkinnedMesh::SRotationKey>& FK_AnimationKeyMap::getRotationKeys(core::stringw jointName){
			if (RotationKeys.count(jointName) == 0){
				RotationKeys[jointName] = core::array< ISkinnedMesh::SRotationKey >();
			}
			return RotationKeys[jointName];
		};
		bool FK_AnimationKeyMap::setPositionArray(core::stringw jointName, core::array<ISkinnedMesh::SPositionKey> newKeys){
			PositionKeys[jointName] = newKeys;
			return true;
		}
		bool FK_AnimationKeyMap::setRotationArray(core::stringw jointName, core::array<ISkinnedMesh::SRotationKey> newKeys){
			RotationKeys[jointName] = newKeys;
			return true;
		}
		bool FK_AnimationKeyMap::setScaleArray(core::stringw jointName, core::array<ISkinnedMesh::SScaleKey> newKeys){
			ScaleKeys[jointName] = newKeys;
			return true;
		}

		core::quaternion FK_AnimationKeyMap::getOffsetRotation(core::stringw jointName){
			if (AnimationReferenceSkeleton_Rotation.count(jointName) == 0){
				AnimationReferenceSkeleton_Rotation[jointName] = core::quaternion(0.f, 0.f, 0.f);
			}
			return AnimationReferenceSkeleton_Rotation[jointName];
		}

		core::vector3df FK_AnimationKeyMap::getOffsetScale(core::stringw jointName){
			if (AnimationReferenceSkeleton_Scale.count(jointName) == 0){
				AnimationReferenceSkeleton_Scale[jointName] = core::vector3df(1.0f,1.0f,1.0f);
			}
			return AnimationReferenceSkeleton_Scale[jointName];
		}

		core::vector3df FK_AnimationKeyMap::getOffsetPosition(core::stringw jointName){
			if (AnimationReferenceSkeleton_Position.count(jointName) == 0){
				AnimationReferenceSkeleton_Position[jointName] = core::vector3df(0.0f, 0.0f, 0.0f);
			}
			return AnimationReferenceSkeleton_Position[jointName];
		}

		u32 FK_AnimationKeyMap::getFrameCount(){
			return frameCount;
		};
		void FK_AnimationKeyMap::setFrameCount(u32 newFrameCount){
			frameCount = newFrameCount;
		};

		void FK_AnimationKeyMap::makeAnimationConsistentWithOffsets(ISkinnedMesh* tempMesh){
			if (frameCount <= 0){
				return;
			}
			if (tempMesh == NULL){
				return;
			}
			//std::wofstream dumpFile("dump.txt");
			//core::array<ISkinnedMesh::SJoint*> AllJoints = ((ISkinnedMesh*)tempMesh)->getAllJoints();
			//dumpFile << AllJoints.size() << std::endl;
			//int cycleSize = AllJoints.size();
			//cycleSize = 2;
			//for (u32 i = 0; i < cycleSize; ++i)
			//{
			//	ISkinnedMesh::SJoint *joint = AllJoints[i];
			//	core::stringw jointName = joint->Name;
			//	int FrameSizeP = PositionKeys[jointName].size();
			//	int FrameSizeS = ScaleKeys[jointName].size();
			//	core::vector3df baseRotation = joint->GlobalMatrix.getRotationDegrees();
			//	baseRotation.X = core::degToRad(baseRotation.X);
			//	baseRotation.Y = core::degToRad(baseRotation.Y);
			//	baseRotation.Z = core::degToRad(baseRotation.Z);
			//	core::quaternion baseRotationQ = core::quaternion(baseRotation);
			//	core::vector3df basePosition = joint->GlobalMatrix.getTranslation();
			//	core::vector3df baseScale = joint->GlobalMatrix.getScale();
			//	for (int k = 0; k < FrameSizeP; ++k){
			//		auto temp = PositionKeys[jointName][k].position;
			//		PositionKeys[jointName][k].position = PositionKeys[jointName][k].position -
			//			AnimationReferenceSkeleton_Position[jointName] +
			//			basePosition;
			//		auto temp2 = PositionKeys[jointName][k].position;
			//		auto temp3 = basePosition - AnimationReferenceSkeleton_Position[jointName];
			//		//if (k == 0){
			//			//dumpFile << i << " " << k << std::endl;
			//			//dumpFile << jointName.c_str() << " - Delta Pos: " << temp3.X << " " << temp3.Y << " " << temp3.Z << std::endl;
			//			//dumpFile << jointName.c_str() << " - Old Pos: " << temp.X << " " << temp.Y << " " << temp.Z << std::endl;
			//			//dumpFile << jointName.c_str() << " - New Pos: " << temp2.X << " " << temp2.Y << " " << temp2.Z << std::endl;
			//		//}
			//	}
			//	for (int k = 0; k < FrameSizeS; ++k){
			//		auto temp = ScaleKeys[jointName][k].scale;
			//		ScaleKeys[jointName][k].scale = ScaleKeys[jointName][k].scale /
			//			AnimationReferenceSkeleton_Scale[jointName] *
			//			baseScale;
			//		auto temp3 = baseScale / AnimationReferenceSkeleton_Scale[jointName];
			//		auto temp2 = ScaleKeys[jointName][k].scale;
			//		//if (k == 0){
			//			//dumpFile << i << " " << k << std::endl;
			//			//dumpFile << jointName.c_str() << " - Scale ratio: " << temp3.X << " " << temp3.Y << " " << temp3.Z << std::endl;
			//			//dumpFile << jointName.c_str() << " - Old scale: " << temp.X << " " << temp.Y << " " << temp.Z << std::endl;
			//			//dumpFile << jointName.c_str() << " - New scale: " << temp2.X << " " << temp2.Y << " " << temp2.Z << std::endl;
			//		//}
			//	}
			//}
			//dumpFile.close();
		}
	}