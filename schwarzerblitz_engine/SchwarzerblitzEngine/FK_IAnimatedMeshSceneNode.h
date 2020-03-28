#pragma once
#include <irrlicht.h>

using namespace irr;

namespace fk_module{

class FK_IAnimatedMeshSceneNode :
	public scene::IAnimatedMeshSceneNode
{
public:
	FK_IAnimatedMeshSceneNode(const scene::IAnimatedMeshSceneNode &mesh);
	~FK_IAnimatedMeshSceneNode();
private:
	int animation_id;
};
};

