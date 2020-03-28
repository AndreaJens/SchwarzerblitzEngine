#ifndef FK_SCENECHARASELECT_EXTRA_DIORAMA_CLASS
#define FK_SCENECHARASELECT_EXTRA_DIORAMA_CLASS

#include "FK_SceneCharacterSelectExtra.h"
#include "FK_StoryFlowCluster.h"

using namespace irr;

namespace fk_engine {
	class FK_SceneCharacterSelectExtra_Diorama : public FK_SceneCharacterSelectExtra {
	public:
		FK_SceneCharacterSelectExtra_Diorama();
		FK_Scene::FK_SceneExtraType getExtraSceneType();
	protected:
		void loadCharacterList();
	};
}
#endif