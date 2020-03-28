#include "FK_SceneExtra.h"

namespace fk_engine {

	FK_SceneExtra::FK_SceneExtra() : FK_SceneWithInput() {
		extraSceneType = FK_Scene::FK_SceneExtraType::ExtraNone;
	}

	FK_Scene::FK_SceneExtraType FK_SceneExtra::getExtraSceneType() {
		return extraSceneType;
	};

	FK_Scene::FK_SceneType FK_SceneExtra::nameId() {
		return FK_Scene::FK_SceneType::SceneExtra;
	}

}
