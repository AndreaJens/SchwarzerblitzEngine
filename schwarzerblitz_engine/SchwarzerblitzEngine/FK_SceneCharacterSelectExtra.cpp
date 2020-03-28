#include "FK_SceneCharacterSelectExtra.h"

namespace fk_engine {
	FK_SceneCharacterSelectExtra::FK_SceneCharacterSelectExtra() : FK_SceneCharacterSelectArcade() {
		
	};

	void FK_SceneCharacterSelectExtra::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		bool player1IsActiveFlag,
		int inputPlayerIndex,
		u32 inputPlayerOutfitId) {
		FK_SceneCharacterSelectArcade::setup(newDevice, new_joypadInfo, newOptions,
			FK_Scene::FK_SceneArcadeType::ArcadeClassic, player1IsActiveFlag, inputPlayerIndex, inputPlayerOutfitId);
	}

	FK_Scene::FK_SceneType FK_SceneCharacterSelectExtra::nameId() {
		return FK_Scene::FK_SceneType::SceneCharacterSelectionExtra;
	}

	FK_Scene::FK_SceneExtraType FK_SceneCharacterSelectExtra::getExtraSceneType() {
		return FK_Scene::FK_SceneExtraType::ExtraNone;
	}
	// don't draw lightning-shaped icon in Extra mode
	void FK_SceneCharacterSelectExtra::drawCompletedArcadeIcon()
	{

	}

}