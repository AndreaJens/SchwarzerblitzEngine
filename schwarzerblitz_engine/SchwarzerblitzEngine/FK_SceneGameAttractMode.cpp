#include "FK_SceneGameAttractMode.h"
#include "Shlwapi.h"
#include "FK_ArcadeCluster.h"
#pragma comment(lib, "Shlwapi.lib")

namespace fk_engine {



	FK_SceneGameAttractMode::FK_SceneGameAttractMode() : FK_SceneGame()
	{
		durationTimerMs = 0;
		// call random to improve variety
		rand();
		attractModeDurationTimerMs = 10000 + (u32) (5000.f * ((f32)rand() / RAND_MAX));
		skipIntro = true;
		processSkippingDemoMatch = false;
		exitScene = false;
		skipDemoMatchCounterMs = 0;
		fadeOutDurationMs = 1000;
	}

	// setup attract mode
	void FK_SceneGameAttractMode::setupAttractMode(
		IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		FK_AdditionalSceneGameOptions new_additionalOptions)
	{
		HMODULE hModule = GetModuleHandleW(NULL);
		TCHAR path[MAX_PATH];
		int pathLength = GetModuleFileName(hModule, path, MAX_PATH);
		TCHAR * out = PathFindFileName(path);
		std::string exeName(out);
		applicationPath = std::string(path).substr(0, pathLength - exeName.length());
		mediaPath = applicationPath + fk_constants::FK_MediaFileFolder;
		configurationFilesPath = applicationPath + fk_constants::FK_ConfigurationFileFolder;
		readSaveFile();
		setupResourcesPaths("", "", "");
		loadCharacterList();
		loadStageList();
		f32 randomIndex = (float)rand() / RAND_MAX;
		u32 charaIndex1 = (u32)(floor(randomIndex * characterPaths.size()));
		while (!isCharacterAvailable(charaIndex1)) {
			randomIndex = (float)rand() / RAND_MAX;
			charaIndex1 = (u32)(floor(randomIndex * characterPaths.size()));
		}
		std::string firstplayerPath = characterPaths[charaIndex1];
		randomIndex = (float)rand() / RAND_MAX;
		u32 charaIndex2 = (u32)(floor(randomIndex * characterPaths.size()));
		if (availableCharacters.size() < 2) {
			charaIndex2 = charaIndex1;
		}
		else {
			while (!isCharacterAvailable(charaIndex2) || charaIndex2 == charaIndex1) {
				randomIndex = (float)rand() / RAND_MAX;
				charaIndex2 = (u32)(floor(randomIndex * characterPaths.size()));
			}
		}
		std::string secondPlayerPath = characterPaths[charaIndex2];
		randomIndex = (float)rand() / RAND_MAX;
		u32 stageIndex = (u32)(floor(randomIndex * stagePaths.size()));
		std::string stageNewPath = stagePaths[stageIndex];
		new_additionalOptions.overrideTimer = true;
		new_additionalOptions.roundTimerInSeconds = 60;
		new_additionalOptions.allowRingout = false;

		FK_Character::FK_CharacterOutfit outfit1;
		FK_Character::FK_CharacterOutfit outfit2;

		FK_ArcadeCluster clusterPlayer1(charactersPath, firstplayerPath, 0, "", stageNewPath, 20);
		outfit1 = clusterPlayer1.getPlayerOutfit();
		FK_ArcadeCluster clusterPlayer2(charactersPath, secondPlayerPath, 0, "", stageNewPath, 20);
		outfit2 = clusterPlayer2.getPlayerOutfit();

		FK_SceneGame::setup(
			newDevice,
			new_joypadInfo,
			newOptions,
			firstplayerPath,
			secondPlayerPath,
			outfit1,
			outfit2,
			stageNewPath,
			FK_Scene::FK_SceneType::SceneGameAttractMode,
			new_additionalOptions,
			true,
			true,
			20,
			20
		);

		setNextScene(FK_SceneType::SceneTitle);
	}

	// an attract mode match cannot be paused
	bool FK_SceneGameAttractMode::canPauseGame()
	{
		return false;
	}

	/* get scene name for loading screen */
	std::wstring FK_SceneGameAttractMode::getSceneNameForLoadingScreen() {
		return L"Demo Match";
	}

	void FK_SceneGameAttractMode::drawOverlay() {
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		core::rect<s32> destRect(0, 0, screenSize.Width, screenSize.Height);
		if (skipDemoMatchCounterMs > 0) {
			s32 alpha = (s32)ceil(255.0f * (f32)skipDemoMatchCounterMs / (f32)fadeOutDurationMs);
			alpha = core::clamp(alpha, 1, 255);
			video::SColor color(alpha, 0, 0, 0);
			driver->draw2DRectangle(color, destRect);
		}
	}

	void FK_SceneGameAttractMode::updateInputForPauseMenuAndSkipOptions()
	{
		if (processSkippingDemoMatch){
			return;
		}
		if (joystickInfo.size() > 0) {
			player1input->update(nowReal, inputReceiver->JoypadStatus(0), false);
		}
		else {
			player1input->update(nowReal, inputReceiver->KeyboardStatus(), false);
		}
		if (joystickInfo.size() > 1) {
			player2input->update(nowReal, inputReceiver->JoypadStatus(1), false);
		}
		else {
			player2input->update(nowReal, inputReceiver->KeyboardStatus(), false);
		}
		u32 currentSkipInput = player1input->getPressedButtons() | player2input->getPressedButtons();
		currentSkipInput &= (FK_Input_Buttons::Any_Button | FK_Input_Buttons::Any_MenuButton);
		if (currentSkipInput) {
			processSkippingDemoMatch = true;
		}
	}

	bool FK_SceneGameAttractMode::isRunning()
	{
		return !exitScene;
	}

	void FK_SceneGameAttractMode::updateRoundTimer(u32 delta_t_ms)
	{
		FK_SceneGame::updateRoundTimer(delta_t_ms);
		durationTimerMs += delta_t_ms;
		if (!processSkippingDemoMatch) {
			if (durationTimerMs > attractModeDurationTimerMs) {
				processSkippingDemoMatch = true;
			}
			if (player1->getLifeRatio() < 0.2f || player2->getLifeRatio() < 0.2f) {
				processSkippingDemoMatch = true;
			}
		}
	}

	void FK_SceneGameAttractMode::drawAdditionalHUDOverlay(f32 delta_t_s)
	{
		hudManager->drawAttractModeMessage(delta_t_s);
		if (processSkippingDemoMatch) {
			skipDemoMatchCounterMs += (u32)(1000 * delta_t_s);
			drawOverlay();
		}
		if (processSkippingDemoMatch && skipDemoMatchCounterMs > fadeOutDurationMs) {
			exitScene = true;
		}
	}

	void FK_SceneGameAttractMode::loadCharacterList()
	{
		availableCharacters.clear();
		characterPaths.clear();
		std::string characterFileName = charactersPath + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName.c_str());
		u32 characterId = 0;
		std::string charaPath = std::string();
		while (!characterFile.eof() && characterFile >> charaPath) {
			std::string key = charaPath;
			charaPath += "\\";
			int availabilityIdentifier;
			characterFile >> availabilityIdentifier;
			if (availabilityIdentifier == CharacterUnlockKeys::Character_AvailableFromStart ||
				availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_AvailableFromStart) {
				availableCharacters.push_back(characterId);
				characterPaths.push_back(charaPath);
				characterId += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_UnlockableAndShown ||
				availabilityIdentifier == CharacterUnlockKeys::Character_UnlockableAndShown ||
				availabilityIdentifier == Character_Unlockable_NoArcadeOpponent) {
				if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()) {
					availableCharacters.push_back(characterId);
				}
				characterPaths.push_back(charaPath);
				characterId += 1;
			}
			else if (availabilityIdentifier == CharacterUnlockKeys::Character_FreeMatchOnly_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Hidden ||
				availabilityIdentifier == CharacterUnlockKeys::Character_Unlockable_Hidden_NoArcadeOpponent) {
				if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(), key) != unlockedCharacters.end()) {
					availableCharacters.push_back(characterId);
					characterPaths.push_back(charaPath);
					characterId += 1;
				}
			}
		};
		availableCharacters.push_back(characterId);
	}

	/* load stage files */
	void FK_SceneGameAttractMode::loadStageList() {
		stagePaths.clear();
		std::string stageFileName = stagesPath + fk_constants::FK_AvailableStagesFileName;
		std::ifstream stageFile(stageFileName.c_str());
		while (!stageFile.eof()) {
			std::string stagePath;
			s32 unlockId;
			stageFile >> stagePath >> unlockId;
			if (unlockId == StageUnlockKeys::Stage_AvailableFromStart || unlockId == Stage_OnlyFreeMatch_AvailableFromStart ||
				std::find(unlockedStages.begin(), unlockedStages.end(), stagePath) != unlockedStages.end()) {
				stagePath += "\\";
				stagePaths.push_back(stagePath);
			}
		};
	};

	bool FK_SceneGameAttractMode::isCharacterAvailable(u32 character_index) {
		bool availableFlag = false;
		if (std::find(availableCharacters.begin(), availableCharacters.end(), character_index) != availableCharacters.end()) {
			availableFlag = true;
		}
		return availableFlag;
	}

}
