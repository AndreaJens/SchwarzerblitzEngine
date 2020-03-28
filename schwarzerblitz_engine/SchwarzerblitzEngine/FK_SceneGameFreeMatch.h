#ifndef FK_SCENEGAMEFREEMATCH_CLASS
#define FK_SCENEGAMEFREEMATCH_CLASS

#include "FK_SceneGame.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{

	class FK_SceneGameFreeMatch : public FK_SceneGame{
	protected:
		// methods to be overloaded from subclasses
		virtual void processMatchEnd();
	public:
		FK_SceneGameFreeMatch();
		FK_Scene::FK_SceneFreeMatchType getFreeMatchType();
		void setFreeMacthType(FK_SceneFreeMatchType type);
	protected:
		void processEndOfRoundStatistics();
		void updateAdditionalSceneLogic(u32 delta_t_ms);
		void drawAdditionalHUDItems(f32 delta_t_s);
		void setupAdditionalParametersAfterLoading();
		void restartMatch();
		// end match menu
		void setupEndMatchMenu();
		void updateEndMatchMenu();
		void activateEndMatchMenu();
		void disableEndMatchMenu();
	private:
		FK_Scene::FK_SceneFreeMatchType freeMatchType;
		FK_InGameMenu* endMatchMenu;
	};
}
#endif