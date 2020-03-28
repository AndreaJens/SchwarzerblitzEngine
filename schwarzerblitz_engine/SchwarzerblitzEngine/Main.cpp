//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h> 


#include <irrlicht.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <Windows.h>
#include <Winuser.h>
#include <time.h>
#include <fstream>

#include<atomic>
#include<thread>

#include "Shlwapi.h"

#include "FK_SceneCharacterSelect.h"
#include "FK_SceneCharacterSelectArcade.h"
#include "FK_SceneGameTutorial.h"
#include "FK_SceneChallengeSelectionTutorial.h"
#include "FK_SceneGameArcade.h"
#include "FK_SceneGameFreeMatch.h"
#include "FK_SceneGameStory.h"
#include "FK_SceneCredits.h"
#include "FK_SceneIntro.h"
#include "FK_SceneTitle.h"
#include "FK_SceneArcadeCutscene.h"
#include "FK_SceneEpisodeSelectionStoryMode.h"
#include "FK_SceneGallery.h"
#include"FK_SceneGameAttractMode.h"

#include "FK_SceneMainMenu.h"
#include "FK_SceneDiorama.h"
#include "FK_SceneCharacterSelectExtra_Diorama.h"
#include "FK_SceneCharacterSelectExtra_ArcadeEnding.h"
#include "FK_Options.h"
#include "FK_SceneOptions.h"
#include "FK_FreeMatchStatistics.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;
using namespace io;
using namespace gui;
using namespace fk_engine;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#ifndef _DEBUG
	#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#endif

HWND hWnd;
HWND hIrrlichtWindow;

static LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0){
		return 0;
	}
	switch (message)
	{
	case WM_SETFOCUS:
		SetFocus(hIrrlichtWindow);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void readGameConfigFile(FK_Options* gameOptions){
	HMODULE hModule = GetModuleHandleW(NULL);
	TCHAR path[MAX_PATH];
	int pathLength = GetModuleFileName(hModule, path, MAX_PATH);
	TCHAR * out = PathFindFileName(path);
	std::string exeName(out);
	std::string applicationPath = std::string(path).substr(0, pathLength - exeName.length());
	std::string filePath = applicationPath + fk_constants::FK_ConfigurationFileFolder + fk_constants::FK_ConfigurationFileName;
	filePath = applicationPath + fk_constants::FK_ConfigurationFileFolder + fk_constants::FK_GameOptionsFileName;
	gameOptions->loadFromFile(filePath);
}

// read stages from file
void readStageFile(std::vector<std::string> &stagePath){
	HMODULE hModule = GetModuleHandleW(NULL);
	TCHAR path[MAX_PATH];
	int pathLength = GetModuleFileName(hModule, path, MAX_PATH);
	TCHAR * out = PathFindFileName(path);
	std::string exeName(out);
	std::string applicationPath = std::string(path).substr(0, pathLength - exeName.length());
	std::string filePath = applicationPath + "media\\stages\\stages.txt";
	std::ifstream stageFile(filePath.c_str());
	std::string temp;
	while (stageFile){
		s32 unlockId = 0;
		stageFile >> temp >> unlockId;
		temp += "\\";
		stagePath.push_back(temp);
	}
	stageFile.close();
};

// this function resets the irrlicht device. This works independently on the Win32 embedding
IrrlichtDevice* resetDevice(IrrlichtDevice* oldDevice, bool fullscreen, core::dimension2d<u32> resolution){

	HWND mediaPlayerHWnd = FindWindow("BUTTON", "video");
	if (mediaPlayerHWnd != NULL) {
		DestroyWindow(mediaPlayerHWnd);
	}
	DestroyWindow(hIrrlichtWindow);
	DestroyWindow(hWnd);

	if (oldDevice != NULL){
		oldDevice->closeDevice();
		oldDevice->run();
		oldDevice->drop();
	}

	IrrlichtDevice* device = createDevice(video::EDT_DIRECT3D9, resolution, 32,
		fullscreen, false, false, NULL);
	if (!device){
		return NULL;
	}
	device->getCursorControl()->setVisible(false);
	core::stringw windowCaption = L"Schwarzerblitz";
	device->setWindowCaption(windowCaption.c_str());
	core::array<SJoystickInfo> joystickInfo;
	device->activateJoysticks(joystickInfo);

	return device;
}

// this function resets the Irrlicht device, but it embeds it into a win32 window - borderless mode is also available
IrrlichtDevice* resetDeviceWin32(IrrlichtDevice* oldDevice, bool borderless, core::dimension2d<u32> resolution){

	//FK_InputReceiver* inputReceiver = new FK_InputReceiver;
	HWND mediaPlayerHWnd = FindWindow("BUTTON", "video");
	if (mediaPlayerHWnd != NULL) {
		DestroyWindow(mediaPlayerHWnd);
	}
	DestroyWindow(hIrrlichtWindow);
	DestroyWindow(hWnd);

	if (oldDevice != NULL){
		oldDevice->closeDevice();
		oldDevice->run();
		oldDevice->drop();
		oldDevice = NULL;
	}

	//create Win32 window
	HINSTANCE hInstance = 0;
	// create dialog

	const char* Win32ClassName = "CIrrlichtWindowsTestDialog";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)CustomWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = DLGWINDOWEXTRA;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = Win32ClassName;
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);

	// set window style
	DWORD styleBase = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD styleAddon1 = WS_MAXIMIZEBOX | WS_SIZEBOX;
	DWORD styleAddon2 = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX;
	DWORD style = styleBase | styleAddon2;

	// set window size
	int windowWidth = resolution.Width + 16;
	int windowHeight = resolution.Height + 39;

	//get screen size
	HMONITOR monitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

	if (borderless){
		windowWidth = monitor_width;
		windowHeight = monitor_height;
		style = styleBase;
	}
	int x = (monitor_width - windowWidth) / 2;
	int y = (monitor_height - windowHeight) / 2;
	hWnd = CreateWindow(Win32ClassName, "Schwarzerblitz",
		style, x, y, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);
	
	ShowWindow(hWnd, SW_SHOW);
	SetFocus(hWnd);
	UpdateWindow(hWnd);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	windowWidth = clientRect.right;
	windowHeight = clientRect.bottom;

	hIrrlichtWindow = CreateWindow("BUTTON", "",
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		0, 0, windowWidth, windowHeight, hWnd, NULL, hInstance, NULL);

	irr::SIrrlichtCreationParameters param;
	param.DriverType = video::EDT_DIRECT3D9;
	param.Fullscreen = false;
	param.Bits = 32;
	param.Stencilbuffer = false;
	param.Vsync = true;
	param.WindowSize = resolution;
	//param.EventReceiver = inputReceiver;
	param.WindowId = reinterpret_cast<void*>(hIrrlichtWindow);

	IrrlichtDevice* device = irr::createDeviceEx(param);

	if (!device){
		return NULL;
	}
	device->getCursorControl()->setVisible(false);
	core::stringw windowCaption = L"Schwarzerblitz";
	device->setWindowCaption(windowCaption.c_str());
	SetFocus(hWnd);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	core::array<SJoystickInfo> joystickInfo;
	device->activateJoysticks(joystickInfo);

	SetFocus(hIrrlichtWindow);
	//ShowWindow(hIrrlichtWindow, SW_HIDE);
	return device;
}

void checkJoypadConnection(IrrlichtDevice* device, std::atomic_bool& hasToUpdateScene) {
	SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED);
	if (!device->checkConnectedJoypads()) {
		hasToUpdateScene.store(true);
	}
	else {
		hasToUpdateScene.store(false);
	}
}

int main(){
	/* check for memory leaks */
	/* get monitor size */
	HMONITOR monitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
	std::thread* buttonCheckThread = NULL;
	/* check for fullscreen*/
	bool fullscreen = false;
	bool enactReplay = false;
	bool saveReplay = false;
	bool playReplay = false;
	int AIPlayer1Level = -1;
	int AIPlayer2Level = -1;
	u32 round_timer_s = 60;
	u32 number_of_rounds = 3;
	u32 player1outfitId = 0;
	u32 player2outfitId = 0;
	s32 storyEpisodeIndex = 0;
	s32 currentMainMenuIndex = 0;
	s32 dioramaCharacterId = -1;
	u32 dioramaOutfitId = 0;
	FK_Character::FK_CharacterOutfit arcadeOutfitPlayer1;
	FK_Character::FK_CharacterOutfit arcadeOutfitPlayer2;
	std::vector<FK_ArcadeCluster> arcadeArray;
	int stageId = 0;
	bool lightsOn = true;
	std::string player1Path;
	std::string player2Path;
	std::string currentStagePath;
	// late patch for allowing smarter/smoother controls in Extra mode
	bool player1IsSelectingExtraMode = true;
	FK_Options* gameOptions = new FK_Options(irr::core::dimension2d<u32>(monitor_width, monitor_height));
	/* read config file */
	readGameConfigFile(gameOptions);
	fullscreen = gameOptions->getFullscreen();
	number_of_rounds = gameOptions->getNumberOfRoundsFreeMatch();
	round_timer_s = gameOptions->getTimerFreeMatch();
	irr::core::dimension2d<u32> screenResolution = gameOptions->getResolution();
	/* create irrlicht device */
	IrrlichtDevice *device = NULL;
	bool borderless = gameOptions->getBorderlessWindowMode();
	if (!fullscreen){
		device = resetDeviceWin32(device, borderless, screenResolution);
	}
	else{
		device = resetDevice(device, fullscreen, screenResolution);
	}
	/* reset random number generator */
	srand(time(0));
	/* extract first random to improve quality later (first random is notoriously similar for nearby system times) */
	rand();

	/* initalize scene */
	//FK_Scene *currentScene = new FK_SceneGame;
	//FK_Scene *currentScene = new FK_SceneCharacterSelect;
	FK_Scene *currentScene = new FK_SceneMainMenu;
	//FK_Scene *currentScene = new FK_SceneIntro;
	//FK_Scene *currentScene = new FK_SceneStoryDialogue;
#ifndef _DEBUG
	currentScene = new FK_SceneIntro;
#endif
	core::array<SJoystickInfo> joyInfo = core::array<SJoystickInfo>();
	bool AIplayer1 = AIPlayer1Level >= 0;
	bool AIplayer2 = AIPlayer2Level >= 0;
	int player1SelectionIndex = -1;
	int player2SelectionIndex = -1;
	bool windowIsOutOfFocus = false;

	//player1SelectionIndex = 1;// -1;
	//player2SelectionIndex = 2;// -1;

	bool preLoadCharacters = false;
	FK_SceneGame::FK_AdditionalSceneGameOptions additionalOptions = FK_SceneGame::FK_AdditionalSceneGameOptions();
	std::vector<std::string> stagePath;
	readStageFile(stagePath);
	stageId = stageId > stagePath.size() - 1 ? stagePath.size() - 1 : stageId;
	bool vsCPU = AIplayer1 || AIplayer2;

	u32 joypadTimerCheckInterval = 1000;
	u32 joypadTimerReference = device->getTimer()->getRealTime();
	std::atomic_bool hasToUpdateInput;
	hasToUpdateInput.store(false);

	// allocate unlocked content for comparison
	FK_SceneWithInput::FK_UnlockedContent previouslyUnlockedContent;
	FK_SceneWithInput::FK_UnlockedContent currentlyUnlockedContent;

	// allocate original match statistics and story cluster
	FK_FreeMatchStatistics matchStatistics;
	FK_StoryFlowCluster storyCluster;
	
	// setup initial scene
	FK_Scene::FK_SceneType nameId = currentScene->nameId();

	if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneIntro) {
		((FK_SceneIntro*)currentScene)->setup(device, joyInfo, gameOptions);
	}
	else if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneMainMenu){
		((FK_SceneMainMenu*)currentScene)->setup(device, joyInfo, gameOptions);
	}
	previouslyUnlockedContent = ((FK_SceneWithInput*)currentScene)->getCurrentlyUnlockedContent();

	while (device->run() && currentScene->isRunning()){
		//bool deviceStatus = device->run();
		// set name of window
		core::stringw windowCaption = L"Schwarzerblitz";
#ifdef _DEBUG
		windowCaption = L"Schwarzerblitz - FPS:";
		windowCaption += device->getVideoDriver()->getFPS();
#endif
		device->setWindowCaption(windowCaption.c_str());
		if (device->isWindowActive() && !device->isWindowFocused()){
			SetFocus(hIrrlichtWindow);
			//std::cout << "WINDOW FOCUS LOST!" << std::endl;
		}
		if (!device->isWindowActive() || device->isWindowMinimized()){
				windowIsOutOfFocus = true;
		}
		else {
			if (windowIsOutOfFocus && gameOptions->getFullscreen()) {
				if (currentScene != NULL) {
					((FK_SceneWithInput*)currentScene)->resetInputRoutines();
				}
			}
			windowIsOutOfFocus = false;
		}

		if (device->getTimer()->getRealTime() - joypadTimerReference > joypadTimerCheckInterval) {
			// The following sets the appropriate flags to prevent system to go into sleep mode.
			joypadTimerReference = device->getTimer()->getRealTime();
			if (buttonCheckThread != NULL) {
				buttonCheckThread->join();
				delete buttonCheckThread;
				buttonCheckThread = NULL;
			}
			buttonCheckThread = new std::thread(checkJoypadConnection, std::ref(device), std::ref(hasToUpdateInput));
		}

		if (hasToUpdateInput.load()) {
			((FK_SceneWithInput*)currentScene)->resetInputRoutines();
			hasToUpdateInput.store(false);
		}

		if (!device->checkIfAllJoysticksAreActive()) {
			((FK_SceneWithInput*)currentScene)->resetInputRoutines();
			hasToUpdateInput.store(false);
		}

		/*if (device->isWindowActive() && (!device->isWindowMinimized() && device->isWindowFocused())) {*/
			currentScene->update();
			if (!currentScene->isRunning()) {
				u32 currentActiveCheats = ((FK_SceneWithInput*)currentScene)->getCurrentActiveCheatCodes();
				gameOptions->setActiveCheatCodes(currentActiveCheats);
				//
				if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneMainMenu) {
					currentMainMenuIndex = ((FK_SceneMainMenu*)currentScene)->getCurrentMenuIndex();
					previouslyUnlockedContent.clear();
					previouslyUnlockedContent = ((FK_SceneMainMenu*)currentScene)->getCurrentlyUnlockedContent();
				}
				// quit
				if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneQuitGame) {
					break;
				}
				// main menu
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneMainMenu) {
					storyEpisodeIndex = 0;
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					additionalOptions = FK_SceneGame::FK_AdditionalSceneGameOptions();
					matchStatistics.clear();
					bool refreshFlag = false;
					if (tempSceneId == FK_Scene::FK_SceneType::SceneOptions) {
						refreshFlag = ((FK_SceneOptions*)currentScene)->needsScreenRefreshing();
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneStoryModeSelection) {
						auto temporaryUnlockedContent = ((FK_SceneWithInput*)currentScene)->getCurrentlyUnlockedContent();
						previouslyUnlockedContent.unlockedStoryEpisodes.clear();
						previouslyUnlockedContent.unlockedStoryEpisodes = temporaryUnlockedContent.unlockedStoryEpisodes;
					}
					else if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneGameTutorial) {
						auto temporaryUnlockedContent = ((FK_SceneWithInput*)currentScene)->getCurrentlyUnlockedContent();
						previouslyUnlockedContent.unlockedTutorialChallenges.clear();
						previouslyUnlockedContent.unlockedTutorialChallenges = temporaryUnlockedContent.unlockedTutorialChallenges;
					}
					currentScene->dispose();
					delete currentScene;
					if (refreshFlag) {
						fullscreen = gameOptions->getFullscreen();
						screenResolution = gameOptions->getResolution();
						bool borderless = gameOptions->getBorderlessWindowMode();
						if (!fullscreen) {
							device = resetDeviceWin32(device, borderless, screenResolution);
						}
						else {
							device = resetDevice(device, fullscreen, screenResolution);
						}
					}
					currentScene = new FK_SceneMainMenu;
					player1SelectionIndex = -1;
					player2SelectionIndex = -1;
					//player1SelectionIndex = 1;// -1;
					//player2SelectionIndex = 2;// -1;
					player1outfitId = 0;
					player2outfitId = 0;
					((FK_SceneMainMenu*)currentScene)->setup(device, joyInfo, gameOptions, currentMainMenuIndex);
					// set content to check
					((FK_SceneMainMenu*)currentScene)->setPreviouslyUnlockedContent(previouslyUnlockedContent);
					((FK_SceneMainMenu*)currentScene)->compareUnlockables();
					//previouslyUnlockedContent.clear();
					//previouslyUnlockedContent = ((FK_SceneMainMenu*)currentScene)->getCurrentlyUnlockedContent();
				}
				// settings
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneOptions) {
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					bool refreshFlag = false;
					if (tempSceneId == FK_Scene::FK_SceneType::SceneOptions) {
						refreshFlag = ((FK_SceneOptions*)currentScene)->needsScreenRefreshing();
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					if (refreshFlag) {
						fullscreen = gameOptions->getFullscreen();
						screenResolution = gameOptions->getResolution();
						bool borderless = gameOptions->getBorderlessWindowMode();
						if (!fullscreen) {
							device = resetDeviceWin32(device, borderless, screenResolution);
						}
						else {
							device = resetDevice(device, fullscreen, screenResolution);
						}
					}
					currentScene = new FK_SceneOptions;
					((FK_SceneOptions*)currentScene)->setup(device, joyInfo, gameOptions);
				}
				// intro
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneIntro) {
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneIntro;
					((FK_SceneIntro*)currentScene)->setup(device, joyInfo, gameOptions);
				}
				// title
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneTitle) {
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentMainMenuIndex = 0;
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneTitle;
					((FK_SceneTitle*)currentScene)->setup(device, joyInfo, gameOptions);
					
				}
				// credits
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneCredits) {
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					bool fadeToBlack = currentScene->nameId() == FK_Scene::FK_SceneType::SceneGameArcade ||
						(currentScene->nameId() & FK_Scene::AnySceneStory) != 0 ||
						currentScene->nameId() == FK_Scene::FK_SceneType::SceneArcadeCutscene;
					//fadeToBlack = true;
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneCredits(device, joyInfo, gameOptions, fadeToBlack);
					if (tempSceneId == FK_Scene::FK_SceneType::SceneOptions) {
						currentScene->setNextScene(tempSceneId);
					}
					
				}
				// arcade cutscene
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneArcadeCutscene) {
					FK_StoryEvent tempEvent;
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					FK_Scene::FK_SceneExtraType extraType = FK_Scene::FK_SceneExtraType::ExtraArcadeEndingGallery;
					if (tempSceneId == FK_Scene::FK_SceneType::SceneGameArcade) {
						tempEvent = ((FK_SceneGameArcade*)currentScene)->getArcadeEnding();
						if (tempEvent.playCredits) {
							tempSceneId = FK_Scene::FK_SceneType::SceneCredits;
						}
						else {
							tempSceneId = FK_Scene::FK_SceneType::SceneGameArcade;
						}
					}
					else if (tempSceneId == FK_Scene::FK_SceneType::SceneCharacterSelectionExtra) {
						tempEvent = ((FK_SceneCharacterSelectExtra_ArcadeEnding*)currentScene)->getCutsceneToPlay();
						dioramaCharacterId = ((FK_SceneCharacterSelectExtra_ArcadeEnding*)currentScene)->getPlayerIndex();
						dioramaOutfitId = ((FK_SceneCharacterSelectExtra_ArcadeEnding*)currentScene)->getPlayerOutfitId();
					}
					FK_StoryEvent* item = new FK_StoryEvent;
					item->allowMenu = tempEvent.allowMenu;
					item->configFileName = tempEvent.configFileName;
					item->episodePath = tempEvent.episodePath;
					item->storyDirectory = tempEvent.storyDirectory;
					item->saveStoryCompletion = tempEvent.saveStoryCompletion;
					item->playCredits = tempEvent.playCredits;
					item->saveWhenSkipped = tempEvent.saveWhenSkipped;
					item->fadeWhenEnding = tempEvent.fadeWhenEnding;
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneArcadeCutscene;
					((FK_SceneArcadeCutscene*)currentScene)->setup(device, joyInfo, gameOptions, (FK_StoryEvent*)item);
					currentScene->setNextScene(tempSceneId);
					item = NULL;
					
				}
				// story
				else if ((currentScene->getNextScene() & FK_Scene::FK_SceneType::AnySceneStory) != 0) {
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneStoryModeSelection) {
						storyCluster.setupFromEpisode(((FK_SceneEpisodeSelectionStoryMode*)currentScene)->getSelectedEpisode());
						storyEpisodeIndex = ((FK_SceneEpisodeSelectionStoryMode*)currentScene)->getCurrentIndex();
						storyCluster.loadProgress();
					}
					else {
						bool saveData = true;
						if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneStoryModeDialogue) {
							saveData = ((FK_SceneStoryDialogue*)currentScene)->canSaveData();
						}
						if (saveData) {
							storyCluster.saveProgress();
						}
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					FK_StoryItem* item = storyCluster.getNextEvent();
					if (item->getType() == FK_StoryItem::ItemType::Cutscene) {
						currentScene = new FK_SceneStoryDialogue;
						((FK_SceneStoryDialogue*)currentScene)->setup(device, joyInfo, gameOptions, (FK_StoryEvent*)item);
					}
					else if (item->getType() == FK_StoryItem::ItemType::Match) {
						currentScene = new FK_SceneGameStory;
						((FK_SceneGameStory*)currentScene)->setup(device, joyInfo, gameOptions, (FK_StoryMatch*)item);
					}
					item = NULL;
					currentScene->setPreviousScene(FK_Scene::FK_SceneType::SceneStoryModeSelection);
					
				}
				// story selection
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneStoryModeSelection) {
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneStoryModeDialogue) {
						if (((FK_SceneStoryDialogue*)currentScene)->canSaveData()) {
							storyCluster.saveProgress();
						}
					}
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneStoryModeMatch) {
						if (!((FK_SceneGameStory*)currentScene)->hasCanceledMatch()) {
							storyCluster.saveProgress();
						}
					}
					auto temporaryUnlockedContent = ((FK_SceneWithInput*)currentScene)->getCurrentlyUnlockedContent();
					previouslyUnlockedContent.unlockedStoryEpisodes.clear();
					previouslyUnlockedContent.unlockedStoryEpisodes = temporaryUnlockedContent.unlockedStoryEpisodes;
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneEpisodeSelectionStoryMode;
					((FK_SceneEpisodeSelectionStoryMode*)currentScene)->setup(device, joyInfo, gameOptions, storyEpisodeIndex);
					
				}
				// arcade
				else if ((currentScene->getNextScene() & FK_Scene::FK_SceneType::SceneGameArcade) != 0) {
					u32 arcadeMatchNumber = 0;
					bool activePlayerIsPlayer1 = true;
					FK_Scene::FK_SceneType sceneId = currentScene->getNextScene();
					FK_Scene::FK_SceneType oldSceneId = currentScene->nameId();
					FK_Scene::FK_SceneArcadeType arcadeType;
					FK_ArcadeProgress arcadeProgress;
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneCharacterSelectionArcade) {
						activePlayerIsPlayer1 = ((FK_SceneCharacterSelectArcade*)currentScene)->isPlayer1Active();
						arcadeType = ((FK_SceneCharacterSelectArcade*)currentScene)->getArcadeType();
						if (activePlayerIsPlayer1) {
							player1Path = ((FK_SceneCharacterSelectArcade*)currentScene)->getPlayerPath();
							player1outfitId = (u32)((FK_SceneCharacterSelectArcade*)currentScene)->getPlayerOutfitId();
							arcadeOutfitPlayer1 = ((FK_SceneCharacterSelectArcade*)currentScene)->getPlayerOutfit();
						}
						else {
							player2Path = ((FK_SceneCharacterSelectArcade*)currentScene)->getPlayerPath();
							player2outfitId = (u32)((FK_SceneCharacterSelectArcade*)currentScene)->getPlayerOutfitId();
							arcadeOutfitPlayer2 = ((FK_SceneCharacterSelectArcade*)currentScene)->getPlayerOutfit();
						}
						arcadeArray = ((FK_SceneCharacterSelectArcade*)currentScene)->buildArcadeArray();
					}
					else if ((currentScene->getNextScene() & FK_Scene::FK_SceneType::SceneGameArcade) != 0) {
						arcadeMatchNumber = ((FK_SceneGameArcade*)currentScene)->getArcadeMatchNumber();
						activePlayerIsPlayer1 = ((FK_SceneGameArcade*)currentScene)->player1isActivePlayer();
						arcadeType = ((FK_SceneGameArcade*)currentScene)->getArcadeType();
						arcadeProgress = ((FK_SceneGameArcade*)currentScene)->getCurrentArcadeProgress();
					}
					if (activePlayerIsPlayer1) {
						player2Path = arcadeArray[arcadeMatchNumber].getPlayerPath();
						player2outfitId = arcadeArray[arcadeMatchNumber].getPlayerOutfitId();
						arcadeOutfitPlayer2 = arcadeArray[arcadeMatchNumber].getPlayerOutfit();
					}
					else {
						player1Path = arcadeArray[arcadeMatchNumber].getPlayerPath();
						player1outfitId = arcadeArray[arcadeMatchNumber].getPlayerOutfitId();
						arcadeOutfitPlayer1 = arcadeArray[arcadeMatchNumber].getPlayerOutfit();
					}
					if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeClassic) {
						additionalOptions.numberOfRounds = -1;
						additionalOptions.allowArcadeClimaticBattle = true;
						additionalOptions.allowArcadeSpecialMatches = true;
					}
					else if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSpecial1) {
						additionalOptions.numberOfRounds = -1;
						additionalOptions.triggerRegenerationTimeMSPlayer1 = 2000;
						additionalOptions.triggerRegenerationTimeMSPlayer2 = 2000;
						additionalOptions.allowArcadeClimaticBattle = true;
					}
					else if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSpecial2) {
						additionalOptions.regenerateLifePlayer1OnNewRound = !activePlayerIsPlayer1;
						additionalOptions.regenerateLifePlayer2OnNewRound = activePlayerIsPlayer1;
						additionalOptions.showRoundWinScreen = false;
						additionalOptions.allowArcadeClimaticBattle = false;
						if (activePlayerIsPlayer1) {
							additionalOptions.lifeMultiplierPlayer1 = 1.0f;
							additionalOptions.lifeMultiplierPlayer2 = 1.0f;
							additionalOptions.numberOfRoundsPlayer2MustWin = 1;
							additionalOptions.triggerRegenerationTimeMSPlayer1 = 5000;
							additionalOptions.triggerRegenerationTimeMSPlayer2 = 0;
							if (additionalOptions.numberOfRoundsPlayer1MustWin < 2) {
								additionalOptions.numberOfRoundsPlayer1MustWin = 2;
							}
						}
						else {
							additionalOptions.lifeMultiplierPlayer2 = 1.2f;
							additionalOptions.lifeMultiplierPlayer1 = 0.9f;
							additionalOptions.numberOfRoundsPlayer1MustWin = 1;
							additionalOptions.triggerRegenerationTimeMSPlayer2 = 5000;
							additionalOptions.triggerRegenerationTimeMSPlayer1 = 0;
							if (additionalOptions.numberOfRoundsPlayer2MustWin < 2) {
								additionalOptions.numberOfRoundsPlayer2MustWin = 2;
							}
						}
					}
					else if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSpecial3) {
						additionalOptions.lifeMultiplierPlayer1 = 0;
						additionalOptions.lifeMultiplierPlayer2 = 0;
						additionalOptions.showRoundWinScreen = false;
						additionalOptions.allowArcadeClimaticBattle = false;
						additionalOptions.numberOfRounds += 2;
						additionalOptions.numberOfRounds = min((s32)additionalOptions.numberOfRounds, (s32)15);
					}
					else if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeSurvival) {
						additionalOptions.numberOfRoundsPlayer1MustWin = 1;
						additionalOptions.numberOfRoundsPlayer2MustWin = 1;
						additionalOptions.allowArcadeClimaticBattle = false;
						additionalOptions.allowContinue = false;
						additionalOptions.roundTimerInSeconds = 100;
						additionalOptions.overrideTimer = true;
						additionalOptions.playArcadeEnding = false;
						f32 survivalMultiplier = arcadeProgress.playerEnergyAtEndOfMatch;
						if (survivalMultiplier < 1.0f && survivalMultiplier > 0.f) {
							survivalMultiplier += 0.2f;
						}
						if (activePlayerIsPlayer1) {
							additionalOptions.player1InitialRoundLifeMultiplier = survivalMultiplier;
						}
						else {
							additionalOptions.player2InitialRoundLifeMultiplier = survivalMultiplier;
						}
						gameOptions->setActiveCheatCodes(0);
					}
					else if (arcadeType == FK_Scene::FK_SceneArcadeType::ArcadeTimeAttack) {
						additionalOptions.numberOfRoundsPlayer1MustWin = 3;
						additionalOptions.numberOfRoundsPlayer2MustWin = 3;
						additionalOptions.allowArcadeClimaticBattle = false;
						additionalOptions.allowContinue = true;
						additionalOptions.roundTimerInSeconds = 99;
						additionalOptions.overrideTimer = true;
						additionalOptions.playArcadeEnding = false;
						gameOptions->setActiveCheatCodes(0);
					}
					currentStagePath = arcadeArray[arcadeMatchNumber].getStagePath();
					bool setPlayer2asAI = activePlayerIsPlayer1;
					u32 setAIPlayerLevel = (u32)(arcadeArray[arcadeMatchNumber].getAILevel());
					FK_Scene::FK_SceneType newSceneId = currentScene->getNextScene();
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneGameArcade;
					((FK_SceneGameArcade*)currentScene)->setup(device, joyInfo, gameOptions,
						arcadeType,
						player1Path, player2Path,
						arcadeOutfitPlayer1, arcadeOutfitPlayer2,
						currentStagePath,
						setPlayer2asAI, setAIPlayerLevel,
						(arcadeMatchNumber + 1),
						additionalOptions,
						arcadeProgress);
					((FK_SceneGameArcade*)currentScene)->setPreviousScene(FK_Scene::FK_SceneType::SceneCharacterSelectionArcade);
					

				}
				// tutorial / challenge
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneGameTutorial) {
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					u32 challengeIndex = 0;
					FK_Scene::FK_SceneTrainingType challengeType = FK_Scene::FK_SceneTrainingType::TrainingTutorial;
					if (currentScene->nameId() == FK_Scene::SceneGameFreeMatch) {
						challengeIndex = ((FK_SceneGameTutorial*)currentScene)->getChallengeIndex();
						challengeType = ((FK_SceneGameTutorial*)currentScene)->getChallengeType();
					}
					else if (currentScene->nameId() == FK_Scene::SceneMainMenu) {
						challengeType = ((FK_SceneMainMenu*)currentScene)->getTrainingType();
					}
					auto temporaryUnlockedContent = ((FK_SceneWithInput*)currentScene)->getCurrentlyUnlockedContent();
					previouslyUnlockedContent.unlockedTutorialChallenges.clear();
					previouslyUnlockedContent.unlockedTutorialChallenges = temporaryUnlockedContent.unlockedTutorialChallenges;
					currentScene->dispose();
					currentScene = new FK_SceneChallengeSelectionTutorial;
					((FK_SceneChallengeSelectionTutorial*)currentScene)->setup(
						device, joyInfo, gameOptions, challengeType, challengeIndex);
					
				}
				// attract mode
				else if ((currentScene->getNextScene() & FK_Scene::FK_SceneType::SceneGameAttractMode) != 0) {
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneGameAttractMode;
					additionalOptions = FK_SceneGame::FK_AdditionalSceneGameOptions();
					((FK_SceneGameAttractMode*)currentScene)->setupAttractMode(
						device,
						joyInfo,
						gameOptions,
						additionalOptions);
				}
				// free match
				else if ((currentScene->getNextScene() & FK_Scene::FK_SceneType::SceneGameFreeMatch) != 0) {
					FK_Character::FK_CharacterOutfit outfitPlayer1;
					FK_Character::FK_CharacterOutfit outfitPlayer2;
					FK_Scene::FK_SceneFreeMatchType freeMatchType;
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneGameTutorial) {
						joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
						std::string challengeDirectory = ((FK_SceneChallengeSelectionTutorial*)currentScene)->getSelectedChallenge().relativePath;
						std::string challengeName = ((FK_SceneChallengeSelectionTutorial*)currentScene)->getSelectedChallenge().challengeStoreName;
						u32 challengeIndex = ((FK_SceneChallengeSelectionTutorial*)currentScene)->getCurrentIndex();
						FK_Scene::FK_SceneTrainingType challengeType = ((FK_SceneChallengeSelectionTutorial*)currentScene)->getChallengeType();
						currentScene->dispose();
						delete currentScene;
						currentScene = new FK_SceneGameTutorial;
						((FK_SceneGameTutorial*)currentScene)->setup(device, joyInfo, gameOptions,
							challengeDirectory, challengeName, challengeType, challengeIndex);
					}
					else {
						if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneCharacterSelection) {
							player1Path = ((FK_SceneCharacterSelect*)currentScene)->getPlayer1Path();
							player2Path = ((FK_SceneCharacterSelect*)currentScene)->getPlayer2Path();
							player1outfitId = (u32)((FK_SceneCharacterSelect*)currentScene)->getPlayer1OutfitId();
							player2outfitId = (u32)((FK_SceneCharacterSelect*)currentScene)->getPlayer2OutfitId();
							outfitPlayer1 = ((FK_SceneCharacterSelect*)currentScene)->getPlayer1Outfit();
							outfitPlayer2 = ((FK_SceneCharacterSelect*)currentScene)->getPlayer2Outfit();
							player1SelectionIndex = ((FK_SceneCharacterSelect*)currentScene)->getPlayer1Index();
							player2SelectionIndex = ((FK_SceneCharacterSelect*)currentScene)->getPlayer2Index();
							currentStagePath = ((FK_SceneCharacterSelect*)currentScene)->getStagePath();
							additionalOptions.allowRingout = ((FK_SceneCharacterSelect*)currentScene)->getRingoutAllowanceFlag();
							additionalOptions.player1MovesetIdentifier = ((FK_SceneCharacterSelect*)currentScene)->getPlayer1MovesetTag();
							additionalOptions.player2MovesetIdentifier = ((FK_SceneCharacterSelect*)currentScene)->getPlayer2MovesetTag();
							freeMatchType = ((FK_SceneCharacterSelect*)currentScene)->getFreeMatchType();
						}
						else if ((currentScene->nameId() & FK_Scene::FK_SceneType::SceneGameFreeMatch) != 0) {
							matchStatistics = ((FK_SceneGameFreeMatch*)currentScene)->getMatchStatistics();
							freeMatchType = ((FK_SceneGameFreeMatch*)currentScene)->getFreeMatchType();
						}
						number_of_rounds = gameOptions->getNumberOfRoundsFreeMatch();
						round_timer_s = gameOptions->getTimerFreeMatch();
						bool setPlayer1asAI = AIplayer1;
						bool setPlayer2asAI = AIplayer2;
						u32 setAIPlayer1Level = AIPlayer1Level;
						u32 setAIPlayer2Level = AIPlayer2Level;
						FK_Scene::FK_SceneType newSceneId = currentScene->getNextScene();
						FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
						joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
						currentScene->dispose();
						delete currentScene;
						if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchTraining) {
							currentScene = new FK_SceneGameTraining;
							((FK_SceneGameTraining*)currentScene)->setup(device, joyInfo, gameOptions,
								player1Path, player2Path,
								outfitPlayer1, outfitPlayer2,
								currentStagePath, additionalOptions);
						}
						else {
							if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchPlayer1VsCPU) {
								setPlayer1asAI = false;
								setPlayer2asAI = true;
								setAIPlayer1Level = gameOptions->getAILevel();
								setAIPlayer2Level = gameOptions->getAILevel();
							}
							else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsPlayer2) {
								setPlayer1asAI = true;
								setPlayer2asAI = false;
								setAIPlayer1Level = gameOptions->getAILevel();
								setAIPlayer2Level = gameOptions->getAILevel();
							}
							else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPU ||
								freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer1Selects ||
								freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer2Selects) {
								setPlayer1asAI = true;
								setPlayer2asAI = true;
								setAIPlayer1Level = gameOptions->getAILevel();
								setAIPlayer2Level = gameOptions->getAILevel();
							}
							else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchMultiplayer) {
								setPlayer1asAI = false;
								setPlayer2asAI = false;
								setAIPlayer1Level = gameOptions->getAILevel();
								setAIPlayer2Level = gameOptions->getAILevel();
							}
							currentScene = new FK_SceneGameFreeMatch;
							additionalOptions.roundTimerInSeconds = round_timer_s;
							additionalOptions.numberOfRounds = number_of_rounds;
							((FK_SceneGameFreeMatch*)currentScene)->setFreeMacthType(freeMatchType);
							((FK_SceneGameFreeMatch*)currentScene)->setup(device, joyInfo, gameOptions,
								player1Path, player2Path,
								outfitPlayer1, outfitPlayer2,
								currentStagePath,
								newSceneId,
								additionalOptions,
								setPlayer1asAI, setPlayer2asAI,
								setAIPlayer1Level, setAIPlayer2Level);
							((FK_SceneGameFreeMatch*)currentScene)->setMatchStatistics(matchStatistics);
						}
						currentScene->setPreviousScene(tempSceneId);
						currentScene->setNextScene(FK_Scene::FK_SceneType::SceneCharacterSelection);
						
					}
				}
				// character selection
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneCharacterSelection) {
					additionalOptions = FK_SceneGame::FK_AdditionalSceneGameOptions();
					if ((currentScene->nameId() & FK_Scene::FK_SceneType::SceneGameFreeMatch) != 0) {
						matchStatistics = ((FK_SceneGameFreeMatch*)currentScene)->getMatchStatistics();
					}
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					FK_Scene::FK_SceneType newSceneId = FK_Scene::FK_SceneType::SceneGameGeneric;
					bool player1IsCPU = AIplayer1;
					bool player2IsCPU = AIplayer2;
					bool player1HasSelected = true;
					FK_Scene::FK_SceneFreeMatchType freeMatchType;
					if ((tempSceneId & FK_Scene::FK_SceneType::SceneGameFreeMatch) != 0) {
						freeMatchType = ((FK_SceneGameFreeMatch*)currentScene)->getFreeMatchType();
						newSceneId = tempSceneId;
					}
					else if (tempSceneId == FK_Scene::FK_SceneType::SceneMainMenu) {
						freeMatchType = ((FK_SceneMainMenu*)currentScene)->getFreeMatchType();
						newSceneId = FK_Scene::FK_SceneType::SceneGameFreeMatch;
						player1HasSelected = ((FK_SceneMainMenu*)currentScene)->player1HasSelected();
					}
					// Check Free Match style and match it with selected mode
					if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchHumanPlayerVsCPU) {
						if (player1HasSelected) {
							freeMatchType = FK_Scene::FK_SceneFreeMatchType::FreeMatchPlayer1VsCPU;
						}else{
							freeMatchType = FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsPlayer2;
						}
					}
					
					if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPU) {
						if (player1HasSelected) {
							freeMatchType = FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer1Selects;
						}else{
							freeMatchType = FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer2Selects;
						}
					}
					
					// set VS CPU flag
					if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsPlayer2) {
						player1IsCPU = true;
						player2IsCPU = false;
					}
					else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchPlayer1VsCPU) {
						player1IsCPU = false;
						player2IsCPU = true;
					}
					else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer1Selects ||
						freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer2Selects) {
						player1IsCPU = true;
						player2IsCPU = true;
					}
					else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchMultiplayer) {
						player1IsCPU = false;
						player2IsCPU = false;
					}
					else if (freeMatchType == FK_Scene::FK_SceneFreeMatchType::FreeMatchTraining) {
						player1IsCPU = false;
						player2IsCPU = true;
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = NULL;
					currentScene = new FK_SceneCharacterSelect;
					((FK_SceneCharacterSelect*)currentScene)->setup(device, joyInfo, gameOptions,
						freeMatchType,
						player1SelectionIndex, player2SelectionIndex,
						player1outfitId, player2outfitId, player1IsCPU, player2IsCPU);
					currentScene->setNextScene(newSceneId);
					
				}
				// arcade character selection
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneCharacterSelectionArcade) {
					additionalOptions = FK_SceneGame::FK_AdditionalSceneGameOptions();
					bool player1Active = true;
					arcadeArray.clear();
					FK_Scene::FK_SceneType sceneId = FK_Scene::FK_SceneType::SceneGameArcade;
					/* test */
					//sceneId = FK_Scene::FK_SceneType::SceneExtra;

					FK_Scene::FK_SceneArcadeType arcadeType = FK_Scene::FK_SceneArcadeType::ArcadeClassic;
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneMainMenu) {
						player1Active = ((FK_SceneMainMenu*)currentScene)->player1HasSelected();
						arcadeType = ((FK_SceneMainMenu*)currentScene)->getArcadeType();
					}
					else if ((currentScene->nameId() & FK_Scene::FK_SceneType::SceneGameArcade) != 0) {
						player1Active = ((FK_SceneGameArcade*)currentScene)->player1isActivePlayer();
						arcadeType = ((FK_SceneGameArcade*)currentScene)->getArcadeType();
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					currentScene = new FK_SceneCharacterSelectArcade;
					((FK_SceneCharacterSelectArcade*)currentScene)->setup(device, joyInfo, gameOptions,
						arcadeType,
						player1Active,
						-1);
					currentScene->setNextScene(sceneId);
					
				}
				// extra character selection
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneCharacterSelectionExtra) {
					additionalOptions = FK_SceneGame::FK_AdditionalSceneGameOptions();
					FK_Scene::FK_SceneType sceneId = FK_Scene::FK_SceneType::SceneExtra;
					/* test */
					//sceneId = FK_Scene::FK_SceneType::SceneExtra;
					FK_Scene::FK_SceneExtraType extraType = FK_Scene::FK_SceneExtraType::ExtraDiorama;
					if (currentScene->nameId() == FK_Scene::FK_SceneType::SceneMainMenu) {
						player1IsSelectingExtraMode = ((FK_SceneMainMenu*)currentScene)->player1HasSelected();
						extraType = ((FK_SceneMainMenu*)currentScene)->getExtraType();
						dioramaCharacterId = -1;
						dioramaOutfitId = 0;
					}
					else if ((currentScene->nameId() & FK_Scene::FK_SceneType::SceneExtra) != 0) {
						extraType = ((FK_SceneExtra*)currentScene)->getExtraSceneType();
					}
					else if ((currentScene->nameId() & FK_Scene::FK_SceneType::SceneArcadeCutscene) != 0) {
						extraType = FK_Scene::FK_SceneExtraType::ExtraArcadeEndingGallery;
					}
					if (extraType == FK_Scene::FK_SceneExtraType::ExtraArcadeEndingGallery) {
						sceneId = FK_Scene::FK_SceneType::SceneArcadeCutscene;
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					switch (extraType) {
						case FK_Scene::FK_SceneExtraType::ExtraArcadeEndingGallery:
							currentScene = new FK_SceneCharacterSelectExtra_ArcadeEnding;
							break;
						case FK_Scene::FK_SceneExtraType::ExtraDiorama:
							currentScene = new FK_SceneCharacterSelectExtra_Diorama;
							break;
						default:
							currentScene = new FK_SceneCharacterSelectExtra;
							break;
					}
					((FK_SceneCharacterSelectExtra*)currentScene)->setup(device, joyInfo, gameOptions,
						player1IsSelectingExtraMode,
						dioramaCharacterId,
						dioramaOutfitId);
					currentScene->setNextScene(sceneId);
					
				}
				// extra
				else if (currentScene->getNextScene() == FK_Scene::FK_SceneType::SceneExtra) {
					FK_Scene::FK_SceneType tempSceneId = currentScene->nameId();
					FK_Scene::FK_SceneExtraType extraType = FK_Scene::FK_SceneExtraType::ExtraDiorama;
					if (tempSceneId == FK_Scene::FK_SceneType::SceneCharacterSelectionExtra) {
						player1Path = ((FK_SceneCharacterSelectExtra*)currentScene)->getPlayerPath();
						player1outfitId = (u32)((FK_SceneCharacterSelectExtra*)currentScene)->getPlayerOutfitId();
						arcadeOutfitPlayer1 = ((FK_SceneCharacterSelectExtra*)currentScene)->getPlayerOutfit();
						extraType = ((FK_SceneCharacterSelectExtra*)currentScene)->getExtraSceneType();
						dioramaCharacterId = ((FK_SceneCharacterSelectExtra*)currentScene)->getPlayerIndex();
						dioramaOutfitId = ((FK_SceneCharacterSelectExtra*)currentScene)->getPlayerOutfitId();
					}else if(tempSceneId == FK_Scene::FK_SceneType::SceneMainMenu){
						extraType = ((FK_SceneMainMenu*)currentScene)->getExtraType();
					}
					joyInfo = ((FK_SceneWithInput*)currentScene)->getJoypadInfo();
					currentScene->dispose();
					delete currentScene;
					if (extraType == FK_Scene::FK_SceneExtraType::ExtraDiorama) {
						currentScene = new FK_SceneDiorama;
						((FK_SceneDiorama*)currentScene)->setup(device, joyInfo, gameOptions,
							player1Path, arcadeOutfitPlayer1);
						currentScene->setPreviousScene(FK_Scene::FK_SceneType::SceneCharacterSelectionExtra);
						
					}else if(extraType == FK_Scene::FK_SceneExtraType::ExtraGallery){
						currentScene = new FK_SceneGallery;
						((FK_SceneGallery*)currentScene)->setup(device, joyInfo, gameOptions);
						currentScene->setPreviousScene(tempSceneId);
						
					}
				}
			}
		}
	//}
	SetThreadExecutionState(ES_CONTINUOUS);
	previouslyUnlockedContent.clear();
	currentlyUnlockedContent.clear();
	storyCluster.reset();
	currentScene->dispose();
	delete gameOptions;
	if (buttonCheckThread != NULL) {
		buttonCheckThread->join();
		delete buttonCheckThread;
		buttonCheckThread = NULL;
	}
	device->closeDevice();
	device->run();
	device->drop();
}