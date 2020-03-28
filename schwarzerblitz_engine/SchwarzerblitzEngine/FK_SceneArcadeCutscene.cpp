#include "FK_SceneArcadeCutscene.h"

namespace fk_engine {

	FK_Scene::FK_SceneType FK_SceneArcadeCutscene::nameId()
	{
		return FK_SceneType::SceneArcadeCutscene;
	}

	void FK_SceneArcadeCutscene::setupChapterMenu() {
		std::vector<std::string> menuItems = {
			" Skip scene ",
			"Cancel",
		};
		chapterMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		chapterMenu->setViewport(core::position2di(-1, -1), screenSize);
	}

	/* update chapter menu */
	void FK_SceneArcadeCutscene::updateChapterMenu() {
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0));
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus());
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1));
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus());
		}
		u32 inputButtonsForMenu = player1input->getPressedButtons();
		inputButtonsForMenu |= player2input->getPressedButtons();
		if ((inputButtonsForMenu & FK_Input_Buttons::Cancel_Button) != 0) {
			deactivateChapterMenu();
			return;
		}
		else {
			chapterMenu->update(inputButtonsForMenu, false);
			if (chapterMenu->itemIsSelected()) {
				s32 chapterMenuIndex = chapterMenu->getIndex();
				chapterMenu->resetSelection();
				switch (chapterMenuIndex) {
				case 0:
					soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
					skipChapter();
					break;
				case 1:
					soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					break;
				default:
					break;
				}
				deactivateChapterMenu();
			}
		}
	}

}
