#ifndef FK_SCENEARCADECUTSCENE_CLASS
#define FK_SCENEARCADECUTSCENE_CLASS

#include"FK_SceneStoryDialogue.h"

using namespace irr;

namespace fk_engine {

	class FK_SceneArcadeCutscene : public FK_SceneStoryDialogue {
	public:
		FK_Scene::FK_SceneType nameId();
	protected:
		void setupChapterMenu();
		void updateChapterMenu();
	};
};

#endif