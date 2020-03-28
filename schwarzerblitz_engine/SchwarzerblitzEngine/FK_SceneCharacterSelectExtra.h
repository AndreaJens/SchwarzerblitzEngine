#ifndef FK_SCENECHARASELECT_EXTRA_CLASS
#define FK_SCENECHARASELECT_EXTRA_CLASS

#include "FK_SceneCharacterSelectArcade.h"
#include "FK_StoryFlowCluster.h"

using namespace irr;

namespace fk_engine {
	class FK_SceneCharacterSelectExtra : public FK_SceneCharacterSelectArcade {
	public:
		FK_SceneCharacterSelectExtra();
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			bool player1IsActiveFlag,
			int inputPlayerIndex = 0,
			u32 inputPlayerOutfitId = 0);
		virtual FK_Scene::FK_SceneType nameId();
		virtual FK_Scene::FK_SceneExtraType getExtraSceneType();
		virtual void drawCompletedArcadeIcon();
	};
}
#endif