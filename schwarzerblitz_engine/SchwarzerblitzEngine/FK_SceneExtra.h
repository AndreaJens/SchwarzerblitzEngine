#ifndef FK_SCENEEXTRA_CLASS
#define FK_SCENEEXTRA_CLASS

#include<irrlicht.h>

#include "FK_SceneWithInput.h"

using namespace irr;

namespace fk_engine {

	class FK_SceneExtra : public FK_SceneWithInput {
	public:
		FK_SceneExtra();
		virtual FK_Scene::FK_SceneExtraType getExtraSceneType();
		virtual FK_Scene::FK_SceneType nameId();
	protected:
		// scene type
		FK_Scene::FK_SceneExtraType extraSceneType;
	};
}

#endif
