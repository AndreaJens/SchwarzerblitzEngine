#include"FK_SceneWithInput.h"
#include"ExternalAddons.h"
#include"FK_Reward.h"
#include "Shlwapi.h"
#include<iostream>
#include<algorithm>

#pragma comment(lib, "Shlwapi.lib")

using namespace gui;

#ifdef _DEBUG
	#define UNLOCK_ALL_REWARDS 
#endif

namespace fk_engine{

	void FK_SceneWithInput::FK_UnlockedContent::clear() {
		unlockedCharacters.clear();
		unlockedStages.clear();
		unlockedOutfits.clear();
		unlockedGameModes.clear();
		unlockedStoryEpisodes.clear();
		unlockedTutorialChallenges.clear();
		unlockedCharacterDioramas.clear();
		unlockedArcadeEndings.clear();
		unlockedGalleryPictures.clear();
	};

	FK_SceneWithInput::FK_SceneWithInput() : FK_Scene(){
		joystickInfo = core::array<SJoystickInfo>();
		currentActiveCheatCodes = 0;
	};
	FK_SceneWithInput::FK_SceneWithInput(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo,
		FK_Options* newOptions) : FK_Scene(){
		setup(newDevice, joypadInfo, newOptions);
	};

	/* setup scene*/
	void FK_SceneWithInput::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions){
		device = newDevice;
		player1input = NULL;
		player2input = NULL;
		inputMapper = NULL;
		inputSetIndex = 0;
		joypadUpdateTimeMS = 0;
		joypadConfigTimeSinceLastButtonPress = 0;
		newJoypadWindow = NULL;
		buttonsSet.clear();
		joypadConfiguredWindowTimerMS = 0;
		joypadConfiguredWindowShown = false;
		joypadConfigCurrentNumberOfButtons = 0;
		gameOptions = newOptions;
		newJoypadFound = std::deque<std::pair<std::string, u32> >();
		joystickInfo = new_joypadInfo;
		/* get the executable path to load resources */
		applicationPath = ".\\";
		mediaPath = applicationPath + fk_constants::FK_MediaFileFolder;
		commonResourcesPath = mediaPath + fk_constants::FK_CommonResourcesFileFolder;
		fontPath = commonResourcesPath + fk_constants::FK_FontsFileFolder;
		joypadSetupTexturesPath = commonResourcesPath + fk_constants::FK_JoypadSetupFileFolder;
		configurationFilesPath = applicationPath + fk_constants::FK_ConfigurationFileFolder;
		oldKeyCode = KEY_KEY_CODES_COUNT;
		tutorialCleared = false;
		tutorialPresented = false;
		unlockablesChoicePresented = false;
		unlockAllFlag = false;
		unlockAllCharactersFlag = false;
		unlockAllStagesFlag = false;
		unlockExtraCostumesForeverFlag = false;
		timeSinceLastInputMS = 0;
		lastMenuInputDirectionPressed = 0;
		// setup the database accessor
		std::string filename = configurationFilesPath + fk_constants::FK_EngineConfigFileName;
		if (gameOptions->getTourneyMode()) {
			filename = configurationFilesPath + "tourney_" + fk_constants::FK_EngineConfigFileName;
		}
		databaseAccessor.setupFromFile(filename);
		if (newOptions->getTourneyMode()) {
			setCurrentActiveCheatCodes(0);
		}
		else {
			setCurrentActiveCheatCodes(newOptions->getActiveCheatCodes());
		}
	};

	void FK_SceneWithInput::dispose(bool disposeGraphics) {
		unlockedCharacters.clear();
		unlockedStages.clear();
		unlockedOutfits.clear();
		unlockedGameModes.clear();
		unlockedStoryEpisodes.clear();
		unlockedTutorialChallenges.clear();
		unlockedCharacterDioramas.clear();
		unlockedArcadeEndings.clear();
		previouslyUnlockedContent.clear();
		currentlyUnlockedContent.clear();
		delete player1input;
		delete player2input;
		delete inputMapper;
		if (inputReceiver != NULL) {
			delete inputReceiver;
		}
		inputReceiver = NULL;
		if (device != NULL) {
			device->setEventReceiver(NULL);
		}
		if (newJoypadWindow != NULL) {
			delete newJoypadWindow;
		}
		if (disposeGraphics) {
			// clear the scene of all scene nodes
			smgr->clear();
			// remove all meshes from the mesh cache
			scene::IMeshCache* cache = smgr->getMeshCache();
			while (cache->getMeshCount())
				cache->removeMesh(cache->getMeshByIndex(0));
			// remove all textures
			smgr->getVideoDriver()->removeAllTextures();
		}
		return;
	}

	/* check for joypads connected to the device and store information */
	void FK_SceneWithInput::setupJoypad(){
		// activate joypads
		if (device->activateJoysticks(joystickInfo))
		{
			std::cout << "Joystick support is enabled and " << joystickInfo.size() << " joystick(s) are present." << std::endl;

			for (u32 joystick = 0; joystick < joystickInfo.size(); ++joystick)
			{
				std::cout << "Joystick " << joystick << ":" << std::endl;
				std::cout << "\tName: '" << joystickInfo[joystick].Name.c_str() << "'" << std::endl;
				std::cout << "\tAxes: " << joystickInfo[joystick].Axes << std::endl;
				std::cout << "\tButtons: " << joystickInfo[joystick].Buttons << std::endl;
				std::cout << "\tHat is: ";

				switch (joystickInfo[joystick].PovHat)
				{
				case SJoystickInfo::POV_HAT_PRESENT:
					std::cout << "present" << std::endl;
					break;

				case SJoystickInfo::POV_HAT_ABSENT:
					std::cout << "absent" << std::endl;
					break;

				case SJoystickInfo::POV_HAT_UNKNOWN:
				default:
					std::cout << "unknown" << std::endl;
					break;
				}
			}
		}
	};

	// get currently active joypad information
	core::array<SJoystickInfo> FK_SceneWithInput::getJoypadInfo(){
		return joystickInfo;
	}

	/* scene flow */
	FK_Scene::FK_SceneType FK_SceneWithInput::getNextScene(){
		return nextSceneTag;
	};
	FK_Scene::FK_SceneType FK_SceneWithInput::getPreviousScene(){
		return previousSceneTag;
	};
	void FK_SceneWithInput::setNextScene(FK_Scene::FK_SceneType newTag){
		nextSceneTag = newTag;
	};
	void FK_SceneWithInput::setPreviousScene(FK_Scene::FK_SceneType newTag){
		previousSceneTag = newTag;
	}

	/* get irrlicht device*/
	IrrlichtDevice * FK_SceneWithInput::getIrrlichtDevice() const
	{
		return device;
	}

	FK_InputReceiver* FK_SceneWithInput::getInputReceiver()
	{
		return inputReceiver;
	}

	/* set previously unlocked content for comparison*/
	void FK_SceneWithInput::setPreviouslyUnlockedContent(FK_UnlockedContent oldContent)
	{
		previouslyUnlockedContent.clear();
		previouslyUnlockedContent = oldContent;
	};

	/* get previously unlocked content for comparison*/
	FK_SceneWithInput::FK_UnlockedContent FK_SceneWithInput::getPreviouslyUnlockedContent()
	{
		return previouslyUnlockedContent;
	}

	/* get currently unlocked content for comparison*/
	FK_SceneWithInput::FK_UnlockedContent FK_SceneWithInput::getCurrentlyUnlockedContent()
	{
		return currentlyUnlockedContent;
	};

	/* create the input mapper and get it ready*/
	void FK_SceneWithInput::setupInputMapper(bool createMapIfNewJoypadIsFound){
		/* initialize the input mapper */
		inputMapper = new FK_InputMapper();
		/* load the input map from file, if already stored */
		std::string joypadfilename = configurationFilesPath + fk_constants::FK_JoypadToXboxTranslatorFileName;
		std::string mapfilename = configurationFilesPath + fk_constants::FK_InputMapFileName;
		inputMapper->loadFromFile(joypadfilename, mapfilename, joystickInfo, newJoypadFound, createMapIfNewJoypadIsFound);
		delete player1input;
		delete player2input;
		player1input = NULL;
		player2input = NULL;
		setupInputForPlayers();
		/* set input receiver */
		if (device->getEventReceiver() != NULL){
			inputReceiver = (FK_InputReceiver*)(device->getEventReceiver());
		}
		else{
			inputReceiver = new FK_InputReceiver();
			device->setEventReceiver(inputReceiver);
		}
		inputReceiver->setJoypadInfo(joystickInfo);
	};

	/* setup irrlicht device */
	void FK_SceneWithInput::setupIrrlichtDevice(){
		driver = device->getVideoDriver();
		smgr = device->getSceneManager();
		smgr->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
		guienv = device->getGUIEnvironment();
		device->getTimer()->setSpeed(1.0f);
		// create texture for RENDER-TO-TEXTURE
		core::dimension2d<u32> borderlessResolution = gameOptions->getResolution();
		borderlessResolution.Width = (u32)ceil(borderlessResolution.Width * gameOptions->getSamplingFactor());
		borderlessResolution.Height = (u32)ceil(borderlessResolution.Height * gameOptions->getSamplingFactor());
		borderlessWindowRenderTarget = driver->addRenderTargetTexture(borderlessResolution, "RTT0");
		/* setup game font using the GUI built-in method*/
		IGUISkin* skin = guienv->getSkin();
		// scale viewport
		f32 scale_factorX = (f32)driver->getScreenSize().Width / fk_constants::FK_DefaultResolution.Width;
		f32 scale_factorY = (f32)driver->getScreenSize().Height / fk_constants::FK_DefaultResolution.Height;
		if (gameOptions->getBorderlessWindowMode()){
			scale_factorX = (f32)borderlessWindowRenderTarget->getSize().Width / fk_constants::FK_DefaultResolution.Width;
			scale_factorY = (f32)borderlessWindowRenderTarget->getSize().Height / fk_constants::FK_DefaultResolution.Height;
		}
		// build font name based on the screen scale factor - the default size is 10 for a 640x360 window
		core::stringc fontName = core::stringc(fk_constants::FK_GameFontName.c_str());
		int fontSize = fk_constants::FK_GameFontBaseSize;
		f32 tempFontSize = fontSize * scale_factorX;
		for (int i = 0; i < fk_constants::FK_GameFontSizes_Size; ++i){
			if (tempFontSize < fk_constants::FK_GameFontSizes[i]){
				fontSize = fk_constants::FK_GameFontSizes[i-1];
				break;
			}
			else{
				if (i == fk_constants::FK_GameFontSizes_Size - 1){
					fontSize = fk_constants::FK_GameFontSizes[i];
				}
			}
		}
		if (tempFontSize < fk_constants::FK_GameFontSizes[0]){
			fontSize = fk_constants::FK_GameFontSizes[0];
		}
		fontName += fontSize;
		fontName += ".xml";
		std::string fontNameStr = std::string(fontName.c_str());
		IGUIFont* gamefont = guienv->getFont((fontPath + fontNameStr).c_str());
		guienv->addFont(fk_constants::FK_GameFontIdentifier.c_str(), gamefont);
		/* create the window/text font as well */
		// build font name based on the screen scale factor - the default size is 10 for a 640x360 window
		fontName = core::stringc(fk_constants::FK_WindowFontName.c_str());
		fontSize = fk_constants::FK_WindowFontBaseSize;
		tempFontSize = fontSize * scale_factorX;
		for (int i = 0; i < fk_constants::FK_WindowFontSizes_Size; ++i){
			if (tempFontSize < fk_constants::FK_WindowFontSizes[i]){
				fontSize = fk_constants::FK_WindowFontSizes[i - 1];
				break;
			}
			else{
				if (i == fk_constants::FK_WindowFontSizes_Size - 1){
					fontSize = fk_constants::FK_WindowFontSizes[i];
				}
			}
		}
		if (tempFontSize < fk_constants::FK_WindowFontSizes[0]){
			fontSize = fk_constants::FK_WindowFontSizes[0];
		}
		fontName += fontSize;
		fontName += ".xml";
		fontNameStr = std::string(fontName.c_str());
		IGUIFont* winfont = guienv->getFont((fontPath + fontNameStr).c_str());
		guienv->addFont(fk_constants::FK_WindowFontIdentifier.c_str(), winfont);

		// create font for hints and small notes
		// build font name based on the screen scale factor - the default size is 10 for a 640x360 window
		fontName = core::stringc(fk_constants::FK_SmallNoteFontName.c_str());
		fontSize = fk_constants::FK_SmallNoteFontBaseSize;
		tempFontSize = fontSize * scale_factorX;
		for (int i = 0; i < fk_constants::FK_SmallNoteFontSizes_Size; ++i) {
			if (tempFontSize < fk_constants::FK_SmallNoteFontSizes[i]) {
				fontSize = fk_constants::FK_SmallNoteFontSizes[i - 1];
				break;
			}
			else {
				if (i == fk_constants::FK_SmallNoteFontSizes_Size - 1) {
					fontSize = fk_constants::FK_SmallNoteFontSizes[i];
				}
			}
		}
		if (tempFontSize < fk_constants::FK_SmallNoteFontSizes[0]) {
			fontSize = fk_constants::FK_SmallNoteFontSizes[0];
		}
		fontName += fontSize;
		fontName += ".xml";
		fontNameStr = std::string(fontName.c_str());
		IGUIFont* notesfont = guienv->getFont((fontPath + fontNameStr).c_str());
		guienv->addFont(fk_constants::FK_SmallNoteFontIdentifier.c_str(), notesfont);

		// create menu font
		// build font name based on the screen scale factor - the default size is 24 for a 640x360 window
		fontName = core::stringc(fk_constants::FK_MenuFontName.c_str());
		fontSize = fk_constants::FK_MenuFontBaseSize;
		tempFontSize = fontSize * scale_factorX;
		for (int i = 0; i < fk_constants::FK_MenuFontSizes_Size; ++i){
			if (tempFontSize < fk_constants::FK_MenuFontSizes[i]){
					fontSize = fk_constants::FK_MenuFontSizes[i - 1];
					break;
			}else{
				if (i == fk_constants::FK_MenuFontSizes_Size - 1){
					fontSize = fk_constants::FK_MenuFontSizes[i];
				}
			}
		}
		if (tempFontSize < fk_constants::FK_MenuFontSizes[0]){
			fontSize = fk_constants::FK_MenuFontSizes[0];
		}
		fontName += fontSize;
		fontName += ".xml";
		fontNameStr = std::string(fontName.c_str());
		IGUIFont* menufont = guienv->getFont((fontPath + fontNameStr).c_str());
		guienv->addFont(fk_constants::FK_MenuFontIdentifier.c_str(), menufont);
		// create battle font
		// build font name based on the screen scale factor - the default size is 24 for a 640x360 window
		fontName = core::stringc(fk_constants::FK_BattleFontName.c_str());
		fontSize = fk_constants::FK_BattleFontBaseSize;
		tempFontSize = fontSize * scale_factorX;
		for (int i = 0; i < fk_constants::FK_BattleFontSizes_Size; ++i){
			if (tempFontSize < fk_constants::FK_BattleFontSizes[i]){
				fontSize = fk_constants::FK_BattleFontSizes[i - 1];
				break;
			}
			else{
				if (i == fk_constants::FK_BattleFontSizes_Size - 1){
					fontSize = fk_constants::FK_BattleFontSizes[i];
				}
			}
		}
		if (tempFontSize < fk_constants::FK_BattleFontSizes[0]){
			fontSize = fk_constants::FK_BattleFontSizes[0];
		}
		fontName += fontSize;
		fontName += ".xml";
		fontNameStr = std::string(fontName.c_str());
		IGUIFont* battlefont = guienv->getFont((fontPath + fontNameStr).c_str());
		guienv->addFont(fk_constants::FK_BattleFontIdentifier.c_str(), battlefont);
		// timerFont
		fontName = core::stringc(fk_constants::FK_TimerFontName.c_str());
		fontSize = fk_constants::FK_TimerFontBaseSize;
		tempFontSize = fontSize * scale_factorX;
		for (int i = 0; i < fk_constants::FK_TimerFontSizes_Size; ++i){
			if (tempFontSize < fk_constants::FK_TimerFontSizes[i]){
				fontSize = fk_constants::FK_TimerFontSizes[i - 1];
				break;
			}
			else{
				if (i == fk_constants::FK_TimerFontSizes_Size - 1){
					fontSize = fk_constants::FK_TimerFontSizes[i];
				}
			}
		}
		if (tempFontSize < fk_constants::FK_TimerFontSizes[0]){
			fontSize = fk_constants::FK_TimerFontSizes[0];
		}
		fontName += fontSize;
		fontName += ".xml";
		fontNameStr = std::string(fontName.c_str());
		IGUIFont* timerfont = guienv->getFont((fontPath + fontNameStr).c_str());
		guienv->addFont(fk_constants::FK_TimerFontIdentifier.c_str(), timerfont);
	};

	/* load joypad textures */
	void FK_SceneWithInput::loadJoypadSetupTextures(){
		core::stringc tex_fileName_root = core::stringc(joypadSetupTexturesPath.c_str()) + "joypadSettingsBase";
		core::stringc tex_extension = ".png";
		u32 numberOfButtonsToSet = FK_JoypadInput::JoypadInput_XboxNumberOfAssignableButtons;
		if (joypadSettingsTextures.empty()){
			for (u32 i = 0; i < numberOfButtonsToSet; ++i){
				core::stringc tex_fileName = tex_fileName_root + "_";
				tex_fileName += i;
				tex_fileName += tex_extension;
				video::ITexture* tempTex = driver->getTexture(tex_fileName);
				joypadSettingsTextures.push_back(tempTex);
			}
			video::ITexture* tempTex = driver->getTexture(tex_fileName_root + tex_extension);
			joypadSettingsTextures.push_back(tempTex);
		}
	}

	/*pre-render routine */
	void FK_SceneWithInput::executePreRenderingRoutine(FK_Options* options){
		if (options == NULL){
			options = gameOptions;
		}
		if (options->getBorderlessWindowMode()){
			smgr->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
			driver->setRenderTarget(borderlessWindowRenderTarget, ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		}
	}

	/* post render routine */
	void FK_SceneWithInput::executePostRenderingRoutine(FK_Options* options){
		if (options == NULL){
			options = gameOptions;
		}
		if (options->getBorderlessWindowMode()){
			driver->setRenderTarget(0, ECBF_COLOR | ECBF_DEPTH);
			irr::core::vector2df scale((f32)((f32)driver->getScreenSize().Width / (f32)borderlessWindowRenderTarget->getSize().Width),
				(f32)((f32)driver->getScreenSize().Height / (f32)borderlessWindowRenderTarget->getSize().Height));
			fk_addons::draw2DImage(driver, borderlessWindowRenderTarget,
				core::rect<s32>(0, 0, borderlessWindowRenderTarget->getSize().Width, borderlessWindowRenderTarget->getSize().Height),
				core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, false);
			/*driver->draw2DImage(borderlessWindowRenderTarget, core::rect<s32>(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height),
				core::rect<s32>(0, 0, borderlessWindowRenderTarget->getSize().Width, borderlessWindowRenderTarget->getSize().Height));*/
		}
	}

	/* check if a new joypad has to be set up */
	bool FK_SceneWithInput::hasToSetupJoypad(){
		return !(newJoypadFound.empty());
	}

	/* update and draw joypad setup */
	void FK_SceneWithInput::updateJoypadSetup(u32 delta_t_ms) {
		if (newJoypadWindow != NULL) {
			newJoypadWindow->update(delta_t_ms);
			if (newJoypadWindow->isClosed() && !newJoypadWindow->isAnimating()) {
				delete newJoypadWindow;
				newJoypadWindow = NULL;
			}
		}
		if (newJoypadWindow != NULL && newJoypadWindow->isAnimating()) {
			return;
		}
		if (joypadUpdateTimeMS >= 0 && joypadUpdateTimeMS < (s32)JoypadSetupTestTimeMS) {
			if (!(newJoypadFound.empty())) {
				std::vector<std::string> text = { "New joypad found:" };
				text.push_back(newJoypadFound[0].first);
				text.push_back("Available buttons are being checked");
				if (newJoypadWindow == NULL) {
					setupJoypadConfigWindow(text);
					activateJoypadConfigWindow();
					u32 numberOfNativeButtons = FK_JoypadInputNative::JoypadInput_NumberOfButtons;
					buttonPressTime.clear();
					buttonsNotToFire.clear();
					for (u32 i = 0; i < numberOfNativeButtons; ++i) {
						buttonPressTime.push_back(0);
						buttonsNotToFire.push_back(false);
					}
				}
				else {
					joypadCosmeticDotsNumber = (u32)std::round((f32)joypadUpdateTimeMS / 200);
					joypadCosmeticDotsNumber %= 4;
					for (u32 i = 0; i < joypadCosmeticDotsNumber; ++i) {
						text[2] += ".";
					}
					setJoypadConfigWindowText(text);
					text.clear();
				}
				joypadUpdateTimeMS += (s32)delta_t_ms;
				u32 numberOfNativeButtons = FK_JoypadInputNative::JoypadInput_NumberOfButtons;
				bool *keyArray = inputReceiver->JoypadStatus(newJoypadFound[0].second);
				for (u32 i = 0; i < numberOfNativeButtons; ++i) {
					if (keyArray[i]) {
						buttonPressTime[i] += delta_t_ms;
					}
				}
			}
		}
		else if (joypadUpdateTimeMS >= (s32)JoypadSetupTestTimeMS) {
			if (!(newJoypadFound.empty())) {
				joypadUpdateTimeMS += (s32)delta_t_ms;
				u32 numberOfNativeButtons = FK_JoypadInputNative::JoypadInput_NumberOfButtons;
				bool *keyArray = inputReceiver->JoypadStatus(newJoypadFound[0].second);
				for (u32 i = 0; i < numberOfNativeButtons; ++i) {
					if (buttonPressTime[i] >= JoypadSetupTestTimeMS) {
						buttonsNotToFire[i] = true;
					}
				}
				// stop directions from "firing" joypad events in the mapping process
				buttonsNotToFire[FK_JoypadInput::JoypadInputDown] = true;
				buttonsNotToFire[FK_JoypadInput::JoypadInputUp] = true;
				buttonsNotToFire[FK_JoypadInput::JoypadInputRight] = true;
				buttonsNotToFire[FK_JoypadInput::JoypadInputLeft] = true;
				// set assignable buttons
				inputSetIndexMap.clear();
				inputSetIndexMap = std::vector<u32>();
				s32 numberOfButtons = (s32)FK_JoypadInput::JoypadInput_XboxNumberOfAssignableButtons;
				if (numberOfButtons > joystickInfo[newJoypadFound[0].second].Buttons) {
					numberOfButtons = joystickInfo[newJoypadFound[0].second].Buttons;
				}
				std::string joypadName = newJoypadFound[0].first;
				std::transform(joypadName.begin(), joypadName.end(), joypadName.begin(), ::tolower);
				// check if it is an XInput controller and add 2 buttons to the total if it is
				std::vector<std::string> testCases = { "xbox", "xinput" };
				for each (std::string tag in testCases) {
					if (joypadName.find(tag) != std::string::npos) {
						numberOfButtons += 2;
						break;
					}
				}
				if (numberOfButtons >= (s32)FK_JoypadInput::JoypadInput_XboxNumberOfAssignableButtons) {
					inputSetIndexMap = {
						0, 1, 2, 3, 7, 6, 5, 4, 11, 10, 9, 8
					};
				}
				else if (numberOfButtons >= 10) {
					inputSetIndexMap = {
						0, 1, 2, 3, 7, 6, 5, 4, 11, 10
					};
				}
				else if (numberOfButtons >= 8) {
					inputSetIndexMap = {
						0, 1, 2, 3, 7, 6, 5, 4
					};
				}
				else if (numberOfButtons >= 6) {
					inputSetIndexMap = std::vector<u32>{ 0, 1, 2, 3, 7, 5 };
				}
				else {
					inputSetIndexMap = std::vector<u32>{ 0, 1, 2, 3, 5 };
				}
				joypadConfigCurrentNumberOfButtons = (s32)inputSetIndexMap.size();
				joypadUpdateTimeMS = -1;
				deactivateJoypadConfigWindow();
				for (u32 i = 0; i < FK_JoypadInput::JoypadInput_XboxNumberOfAssignableButtons; ++i) {
					inputMapper->setButtonForJoypadTranslationMapToXboxController(newJoypadFound[0].first,
						(FK_JoypadInput)i, FK_JoypadInputNative::JoypadInput_NoButton_Native);
				}
			}
		}
		else{
			joypadConfigTimeSinceLastButtonPress += (s32)delta_t_ms;
			std::string mapfilename = configurationFilesPath + fk_constants::FK_InputMapFileName;
			std::string joypadfilename = configurationFilesPath + fk_constants::FK_JoypadToXboxTranslatorFileName;
			u32 numberOfNativeButtons = FK_JoypadInputNative::JoypadInput_NumberOfButtons;
			if (!(newJoypadFound.empty())) {
				if (joypadConfigCurrentNumberOfButtons < 0) {
					return;
				}
				if (inputSetIndex < joypadConfigCurrentNumberOfButtons) {
					bool *keyArray = inputReceiver->JoypadStatus(newJoypadFound[0].second);
					for (u32 i = 0; i < numberOfNativeButtons; ++i) {
						if (keyArray[i] && !buttonsNotToFire[i]) {
							if (std::find(buttonsSet.begin(), buttonsSet.end(), i) == buttonsSet.end()) {
								if(i != oldKeyCode){
									inputMapper->setButtonForJoypadTranslationMapToXboxController(newJoypadFound[0].first,
										(FK_JoypadInput)(inputSetIndexMap[inputSetIndex]), (FK_JoypadInputNative)i);
									inputSetIndex += 1;
									playAssignedButtonSound();
									oldKeyCode = i;
									buttonsSet.push_back(i);
									joypadConfigTimeSinceLastButtonPress = 0;
								}
							} else if (inputSetIndex >= MinimumNumberOfJoypadButtonsToSet){
								if (i != oldKeyCode || (i == oldKeyCode && joypadConfigTimeSinceLastButtonPress > MinimumTimeFromLastButtonToAbortCommandMS)) {
									playAssignedButtonSound();
									inputSetIndex = joypadConfigCurrentNumberOfButtons;
									return;
								}
							}
						}
					}
				}
				else if (newJoypadWindow == NULL && !joypadConfiguredWindowShown) {
					std::vector<std::string> text;
					text.push_back("New joypad successfully configured:");
					text.push_back(newJoypadFound[0].first);
					text.push_back("You can reconfigure it via Options > Remap joypad to std. controller");
					setupJoypadConfigWindow(text);
					activateJoypadConfigWindow();
					//playAssignedButtonSound();
					/* save input */
					inputMapper->saveJoypadTranslationMapToXboxControllerToFile(joypadfilename);
					/* mark flag */
					joypadConfiguredWindowShown = true;
					joypadConfiguredWindowTimerMS = 0;
					text.clear();
				}
				else if (joypadConfiguredWindowTimerMS <= JoypadSetupConfirmTimeMS) {
					joypadConfiguredWindowTimerMS += delta_t_ms;
					if (joypadConfiguredWindowTimerMS >= JoypadSetupConfirmTimeMS) {
						deactivateJoypadConfigWindow();
					}
				}
				else {
					newJoypadFound.pop_front();
					joypadUpdateTimeMS = 0;
					joypadConfigTimeSinceLastButtonPress = 0;
					buttonsNotToFire.clear();
					buttonPressTime.clear();
					inputSetIndexMap.clear();
					inputSetIndex = 0;
					buttonsSet.clear();
					if (newJoypadFound.empty()) {
						inputMapper->saveJoypadTranslationMapToXboxControllerToFile(joypadfilename);
						inputMapper->loadFromFile(joypadfilename, mapfilename, joystickInfo, newJoypadFound, false);
						delete player1input;
						delete player2input;
						player1input = NULL;
						player2input = NULL;
						setupInputForPlayers();
					}
					if (newJoypadWindow != NULL) {
						delete newJoypadWindow;
						newJoypadWindow = NULL;
					}
					joypadConfiguredWindowTimerMS = 0;
					joypadConfiguredWindowShown = false;
				}
			}
		}
	}

	// make a basic setup for player input
	void FK_SceneWithInput::setupInputForPlayers(s32 timeWindowBetweenInputBeforeClearing_ms,
		s32 mergeTimeForInputAtSameFrame_ms, s32 timeToRegisterHeldButton_ms,
		s32 timeToRegisterNullInput_ms,	s32 maxNumberOfRecordedButtonInInputBuffer){
		if (joystickInfo.size() > 1){
			player1input = new FK_InputBuffer(1, inputMapper->getJoypadMap(1),
				timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms,
				timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
			player2input = new FK_InputBuffer(2, inputMapper->getJoypadMap(2),
				timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms,
				timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
		}
		else if (joystickInfo.size() > 0){
			player1input = new FK_InputBuffer(1, inputMapper->getJoypadMap(1),
				timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms,
				timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
			player2input = new FK_InputBuffer(2, inputMapper->getKeyboardMap(0),
				timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms,
				timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
		}
		else{
			player1input = new FK_InputBuffer(1, inputMapper->getKeyboardMap(0),
				timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms,
				timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
			player2input = new FK_InputBuffer(2, inputMapper->getKeyboardMap(-1),
				timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms,
				timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
		}
	}

	void FK_SceneWithInput::setupInputForPlayers() {
		s32 maxNumberOfRecordedButtonInInputBuffer = 10;
		s32 timeWindowBetweenInputBeforeClearing_ms = 200;
		s32 mergeTimeForInputAtSameFrame_ms = 20;
		s32 timeToRegisterHeldButton_ms = 200; //it was originally 40.
		s32 timeToRegisterNullInput_ms = 200;
		setupInputForPlayers(timeWindowBetweenInputBeforeClearing_ms,
			mergeTimeForInputAtSameFrame_ms, timeToRegisterHeldButton_ms,
			timeToRegisterNullInput_ms, maxNumberOfRecordedButtonInInputBuffer);
	}

	/* draw joypad setup */
	void FK_SceneWithInput::drawJoypadSetup(u32 time_ms){
		if (joypadSettingsTextures.empty()){
			return;
		}
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		bool drawSetup = newJoypadWindow == NULL;
		if (newJoypadWindow != NULL) {
			drawSetup = !newJoypadWindow->isClosing();
		}
		if (joypadUpdateTimeMS < 0 && drawSetup) {
			/* draw background image*/
			u32 arraySize = joypadSettingsTextures.size();
			core::dimension2d<u32> backgroundSize = joypadSettingsTextures[arraySize - 1]->getSize();
			f32 scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
			f32 scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
			s32 dest_width = screenSize.Width;
			s32 dest_height = screenSize.Height;
			s32 x = 0;
			s32 y = 0;
			f32 scaleX = (f32)screenSize.Width / (f32)backgroundSize.Width;
			f32 scaleY = (f32)screenSize.Height / (f32)backgroundSize.Height;

			core::rect<s32> destinationRect = core::rect<s32>(0, 0,
				dest_width,
				dest_height);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, backgroundSize.Width, backgroundSize.Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			f32 phase = (f32)(time_ms % 1000) / 1000 * 2.0f * 3.1415926f;
			u32 opacity = 128 + 127 * std::cos(phase);
			video::SColor const colorInput = video::SColor(opacity, 255, 255, 255);
			video::SColor const vertexColors_input[4] = {
				colorInput,
				colorInput,
				colorInput,
				colorInput
			};
			fk_addons::draw2DImage(driver, joypadSettingsTextures[arraySize - 1], sourceRect, core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0.f, core::vector2df(scaleX, scaleY), true, color, destinationRect);
			//driver->draw2DImage(joypadSettingsTextures[arraySize - 1], destinationRect, sourceRect, 0, vertexColors_background, true);
			/* draw current input button texture */
			if (inputSetIndex < inputSetIndexMap.size()) {
				u32 textureIndex = inputSetIndexMap[inputSetIndex];
				driver->draw2DImage(joypadSettingsTextures[textureIndex], destinationRect, sourceRect, 0, vertexColors_input, true);
			}
			video::SColor font_color = video::SColor(255, 0, 174, 255);
			u32 green = 140 + 60 * std::cos(phase);
			video::SColor highlight_color = video::SColor(255, green, green, 200);
			video::SColor list_color = video::SColor(255, 255, 200, 150);
			std::vector<core::stringw> button_descriptions(arraySize - 1);
			button_descriptions[0] = "A";
			button_descriptions[1] = "B";
			button_descriptions[2] = "X";
			button_descriptions[3] = "Y";
			button_descriptions[4] = "L1";
			button_descriptions[5] = "R1";
			button_descriptions[6] = "Back";
			button_descriptions[7] = "Start";
			button_descriptions[8] = "L3";
			button_descriptions[9] = "R3";
			button_descriptions[10] = "L2";
			button_descriptions[11] = "R2";
			core::stringw buttonSuffixLeft = " button: ";
			core::stringw buttonSuffixRight = " Button ";
			gui::IGUIFont* titleFont = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
			gui::IGUIFont* basicFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
			gui::IGUIFont* textFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
			s32 sentenceX, sentenceY, offsetX, offsetY;
			u32 setButtonSize = buttonsSet.size();

			core::stringw title = L"New joypad found: ";
			s32 sentenceWidth = basicFont->getDimension(title.c_str()).Width;
			s32 sentenceHeight = basicFont->getDimension(title.c_str()).Height;
			sentenceY = 5;

			core::stringw joyname = newJoypadFound[0].first.c_str();
			s32 sentenceWidth2 = textFont->getDimension(joyname.c_str()).Width;
			s32 sentenceHeight2 = textFont->getDimension(joyname.c_str()).Height;

			sentenceX = screenSize.Width / 2 - (sentenceWidth + sentenceWidth2) / 2;

			destinationRect = core::rect<s32>(sentenceX,
				sentenceY * scale_factorY,
				sentenceX + sentenceWidth,
				sentenceHeight + sentenceY * scale_factorY);
			fk_addons::drawBorderedText(basicFont, std::wstring(title.c_str()), destinationRect,
				irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));

			sentenceY += 4;

			destinationRect = core::rect<s32>(sentenceX + sentenceWidth,
				sentenceY * scale_factorY,
				sentenceX + sentenceWidth + sentenceWidth2,
				sentenceHeight2 + sentenceY * scale_factorY);
			fk_addons::drawBorderedText(textFont, std::wstring(joyname.c_str()), destinationRect,
				font_color, irr::video::SColor(128, 0, 0, 0));

			if (inputSetIndex >= MinimumNumberOfJoypadButtonsToSet) {
				video::SColor add_text_color = video::SColor(128 + opacity / 2 , 230, 174, 25);
				std::wstring completionMessage = L"Press any already assigned button to save&exit";
				s32 sentenceWidth3 = textFont->getDimension(completionMessage.c_str()).Width;
				s32 add_sentenceX = screenSize.Width / 2 - (sentenceWidth3) / 2;
				s32 add_sentenceY = sentenceHeight2 + (s32)std::floor((sentenceY + 4.f) * scale_factorY);
				destinationRect = core::rect<s32>(add_sentenceX,
					add_sentenceY,
					add_sentenceX + sentenceWidth3,
					add_sentenceY + sentenceHeight2);
				fk_addons::drawBorderedText(textFont, completionMessage, destinationRect,
					add_text_color, irr::video::SColor(64 + opacity / 4, 0, 0, 0));
			}

			sentenceX = 360;
			offsetX = 120;
			offsetY = 40;
			s32 additionalOffsetY = (s32)std::round(4 * scale_factorY);
			for (u32 k = 0; k < inputSetIndexMap.size(); ++k) {
				u32 i = inputSetIndexMap[k];
				core::stringw item = button_descriptions[i] + buttonSuffixLeft;
				if (i == 6 || i == 7) {
					item = button_descriptions[i] + ":";
				}
				s32 sentenceWidth = basicFont->getDimension(item.c_str()).Width;
				s32 sentenceHeight = basicFont->getDimension(item.c_str()).Height;
				sentenceY = offsetY * scale_factorY + (s32)(k * sentenceHeight * 1.1);
				destinationRect = core::rect<s32>(sentenceX * scale_factorX,
					sentenceY,
					sentenceX * scale_factorX + sentenceWidth,
					sentenceHeight + sentenceY);
				fk_addons::drawBorderedText(basicFont, std::wstring(item.c_str()), destinationRect,
					k == setButtonSize ? list_color : irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
				if (k < setButtonSize) {
					item = buttonSuffixRight;
					item += buttonsSet[k] + 1;
					sentenceWidth = basicFont->getDimension(item.c_str()).Width;
					sentenceHeight = basicFont->getDimension(item.c_str()).Height;
					core::rect<s32> destinationRect = core::rect<s32>((sentenceX + offsetX) * scale_factorX,
						sentenceY + additionalOffsetY,
						(sentenceX + offsetX) * scale_factorX + sentenceWidth,
						sentenceHeight + sentenceY + additionalOffsetY);
					fk_addons::drawBorderedText(textFont, std::wstring(item.c_str()), destinationRect,
						font_color, irr::video::SColor(128, 0, 0, 0));
				}
				else if (k == setButtonSize) {
					item = " Waiting for input...";
					sentenceWidth = basicFont->getDimension(item.c_str()).Width;
					sentenceHeight = basicFont->getDimension(item.c_str()).Height;
					core::rect<s32> destinationRect = core::rect<s32>((sentenceX + offsetX) * scale_factorX,
						sentenceY + additionalOffsetY,
						(sentenceX + offsetX) * scale_factorX + sentenceWidth,
						sentenceHeight + sentenceY + additionalOffsetY);
					fk_addons::drawBorderedText(textFont, std::wstring(item.c_str()), destinationRect,
						highlight_color, irr::video::SColor(128, 0, 0, 0));
				}
			}
		}

		if (joypadUpdateTimeMS >= 0) {
			s32 dest_width = screenSize.Width;
			s32 dest_height = screenSize.Height;
			core::rect<s32> destinationRect = core::rect<s32>(0, 0,
				dest_width,
				dest_height);
			driver->draw2DRectangle(video::SColor(128, 0, 0, 0), destinationRect);
		}
		if (newJoypadWindow != NULL) {
			newJoypadWindow->draw(screenSize, 1.f, 1.f, false);
		}
	}

	/* overridable function which plays a sound on assign - not implemented in main class */
	void FK_SceneWithInput::playAssignedButtonSound()
	{
		return;
	}


	/* load single character file */
	std::string FK_SceneWithInput::getCharacterNameFromFile(std::string characterPath){
		std::string playerName = std::string();
		std::ifstream characterFile((characterPath + "character.txt").c_str());
		if (!characterFile){
			return std::string();
		}
		std::string characterFileKeyName = "#NAME";
		while (!characterFile.eof()){
			std::string temp;
			characterFile >> temp;
			/* check if temp matches one of the keys*/
			/* name */
			if (strcmp(temp.c_str(), characterFileKeyName.c_str()) == 0){
				characterFile >> temp;
				playerName = temp;
				break;
			}
		}
		characterFile.close();
		return playerName;
	};


	// manage cheats file
	void FK_SceneWithInput::readCheatsFile() {
		std::ifstream cheatFile(configurationFilesPath + fk_constants::FK_CheatsFileName);
		if (!cheatFile) {
			return;
		}
		availableCheats.clear();
		while (cheatFile) {
			FK_Cheat cheat;
			bool success = cheat.readFromFile(cheatFile);
			if (success) {
				availableCheats.push_back(cheat);
			}
		}
	}

	// check for cheats
	bool FK_SceneWithInput::updateCheatsCheck() {
		for each(FK_Cheat cheatToCheck in availableCheats) {
			if (cheatToCheck.getInputSequence().empty()) {
				continue;
			}
			if ((currentActiveCheatCodes & cheatToCheck.getCheatType()) != 0) {
				continue;
			}
			if (currentActiveCheatCodes == 0 && cheatToCheck.getCheatType() == 0) {
				continue;
			}
			bool success = player1input->checkForSequence(
				cheatToCheck.getInputSequence());
			if (!success) {
				success = player2input->checkForSequence(
					cheatToCheck.getInputSequence());
			}
			if (success) {
				if (cheatToCheck.getCheatType() == FK_Cheat::FK_CheatType::NoCheat) {
					currentActiveCheatCodes = 0;
				}
				else {
					currentActiveCheatCodes |= (u32)cheatToCheck.getCheatType();
				}
				return true;
			}
		}
		return false;
	}

	// check for cheats
	bool FK_SceneWithInput::isInputingCheat() {
		u32 playerInput = player1input->getPressedButtons() | player2input->getPressedButtons();
		u32 inputToCheck = (playerInput & FK_Input_Buttons::Trigger_Button);
		if (inputToCheck == 0) {
			return false;
		}
		if (gameOptions->getTourneyMode()) {
			return false;
		}
		return true;
	}

	// manage save files and rewards
	void FK_SceneWithInput::readRewardFile(){
		std::ifstream rewardFile(configurationFilesPath + fk_constants::FK_RewardsFileName);
		if (!rewardFile){
			return;
		}
		FK_RewardUnlockStruct unlockingParameters;
		unlockingParameters.characterStats = characterStatisticsStructure;
		unlockingParameters.arcadeWins = numberOfArcadeBeatenPerDifficultyLevel;
		unlockingParameters.survivalRecords = survivalRecordPerDifficultyLevel;
		unlockingParameters.timeAttackRecords = timeAttackRecordPerDifficultyLevel;
		unlockingParameters.isTutorialCleared = tutorialCleared;
		unlockingParameters.storyEpisodesCleared = storyEpisodesCleared;
		unlockingParameters.challengesCleared = tutorialChallengesCleared;
		unlockingParameters.specialArcadeTags = specialTagsAcquired;
		while (rewardFile){
			FK_Reward reward;
			bool success = reward.readFromFile(rewardFile);
			if (success){
				unlockingParameters.unlockedCharacters = unlockedCharacters;
				bool rewardUnlocked = reward.rewardIsUnlocked(unlockingParameters);
//#ifdef UNLOCK_ALL_REWARDS
//				rewardUnlocked = true;
//#endif
				if (unlockAllFlag || gameOptions->getTourneyMode()){
					rewardUnlocked = true;
				}
				
				else if (reward.type == FK_Reward::RewardType::Character && (
					unlockAllCharactersFlag ||
					(currentActiveCheatCodes & FK_Cheat::CheatUnlockAllCharacters) != 0)) {
					rewardUnlocked = true;
				}
				
				else if(reward.type == FK_Reward::RewardType::Stage && (
					unlockAllStagesFlag ||
					(currentActiveCheatCodes & FK_Cheat::CheatUnlockAllStages) != 0)) {
					rewardUnlocked = true;
				}

				else if (reward.type == FK_Reward::RewardType::StoryEpisode &&
					(currentActiveCheatCodes & FK_Cheat::CheatUnlockAllStoryEpisodes) != 0) {
					rewardUnlocked = true;
				}

				else if (reward.type == FK_Reward::RewardType::Costume &&
					(currentActiveCheatCodes & FK_Cheat::CheatUnlockAllCostumes) != 0) {
					rewardUnlocked = true;
				}

				if (rewardUnlocked){
					if (reward.type == FK_Reward::RewardType::Character){
						if (std::find(unlockedCharacters.begin(), unlockedCharacters.end(),
							reward.rewardKey) == unlockedCharacters.end()) {
							unlockedCharacters.push_back(reward.rewardKey);
						}
					}
					else if (reward.type == FK_Reward::RewardType::Stage){
						if (std::find(unlockedStages.begin(), unlockedStages.end(),
							reward.rewardKey) == unlockedStages.end()) {
							unlockedStages.push_back(reward.rewardKey);
						}
					}
					else if (reward.type == FK_Reward::RewardType::Costume){
						if (std::find(unlockedOutfits.begin(), unlockedOutfits.end(),
							reward.rewardKey) == unlockedOutfits.end()) {
							unlockedOutfits.push_back(reward.rewardKey);
						}
					}
					else if (reward.type == FK_Reward::RewardType::GameMode){
						if (std::find(unlockedGameModes.begin(), unlockedGameModes.end(),
							reward.rewardKey) == unlockedGameModes.end()) {
							unlockedGameModes.push_back(reward.rewardKey);
						}
					}
					else if (reward.type == FK_Reward::RewardType::Challenge){
						if (std::find(unlockedTutorialChallenges.begin(), unlockedTutorialChallenges.end(),
							reward.rewardKey) == unlockedTutorialChallenges.end()) {
							unlockedTutorialChallenges.push_back(reward.rewardKey);
						}
					}
					else if (reward.type == FK_Reward::RewardType::StoryEpisode){
						if (std::find(unlockedStoryEpisodes.begin(), unlockedStoryEpisodes.end(),
							reward.rewardKey) == unlockedStoryEpisodes.end()) {
							unlockedStoryEpisodes.push_back(reward.rewardKey);
						}
					}
					else if (reward.type == FK_Reward::RewardType::Diorama) {
						if (std::find(unlockedCharacterDioramas.begin(), unlockedCharacterDioramas.end(),
							reward.rewardKey) == unlockedCharacterDioramas.end()) {
							unlockedCharacterDioramas.push_back(reward.rewardKey);
						}
					}
				}
			}
			else{
				break;
			}
		}
		/* update current unlocked content */
		currentlyUnlockedContent.clear();
		currentlyUnlockedContent.unlockedCharacters = unlockedCharacters;
		currentlyUnlockedContent.unlockedStages = unlockedStages;
		currentlyUnlockedContent.unlockedOutfits = unlockedOutfits;
		currentlyUnlockedContent.unlockedGameModes = unlockedGameModes;
		currentlyUnlockedContent.unlockedTutorialChallenges = unlockedTutorialChallenges;
		currentlyUnlockedContent.unlockedStoryEpisodes = unlockedStoryEpisodes;
		currentlyUnlockedContent.unlockedCharacterDioramas = unlockedCharacterDioramas;
		currentlyUnlockedContent.unlockedArcadeEndings = unlockedArcadeEndings;
		unlockingParameters.characterStats.clear();
		unlockingParameters.arcadeWins.clear();
		unlockingParameters.storyEpisodesCleared.clear();
		unlockingParameters.challengesCleared.clear();
	}

	void FK_SceneWithInput::writeSaveFile(){
		std::ofstream saveFile(configurationFilesPath + fk_constants::FK_SaveFileName);
		std::string charactersDirectory = mediaPath + fk_constants::FK_CharactersFileFolder;
		std::string stagesResourcePath = mediaPath + fk_constants::FK_StagesFileFolder;
		std::vector<std::string> characterPaths;
		// load character file
		std::string characterFileName = charactersDirectory + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName);
		while (!characterFile.eof()){
			std::string charaPath;
			s32 unlockId;
			characterFile >> charaPath >> unlockId;
			characterPaths.push_back(charaPath);
		};
		characterFile.close();
		// save arcade wins
		saveFile << fk_saveFileAddons::GlobalStatisticsForSaveFileTag << std::endl;
		for each(std::pair<u32, u32> level_arcade in numberOfArcadeBeatenPerDifficultyLevel){
			saveFile << fk_saveFileAddons::AILevelTagForArcade << "\t" << level_arcade.first << "\t" << level_arcade.second << std::endl;
		}
		for each(std::pair<u32, std::pair<std::string, u32>> level_arcade in survivalRecordPerDifficultyLevel) {
			saveFile << fk_saveFileAddons::AILevelTagForSurvivalArcade << "\t" << level_arcade.first << 
				"\t" << level_arcade.second.second <<
				"\t" << level_arcade.second.first << std::endl;
		}
		for each(std::pair<u32, std::pair<std::string, u32>> level_arcade in timeAttackRecordPerDifficultyLevel) {
			saveFile << fk_saveFileAddons::AILevelTagForTimeAttackArcade << "\t" << level_arcade.first <<
				"\t" << level_arcade.second.second <<
				"\t" << level_arcade.second.first << std::endl;
		}
		// save if tutorial has been cleared / presented at least once
		saveFile << fk_saveFileAddons::TutorialForSaveFileTag << std::endl;
		saveFile << (u32)tutorialPresented << " " << (u32)unlockablesChoicePresented << std::endl;
		// save "unlock all" string
		if (unlockAllFlag){
			saveFile << fk_saveFileAddons::UnlockAllContentsTag << std::endl;
		}
		if (unlockAllCharactersFlag) {
			saveFile << fk_saveFileAddons::UnlockAllCharactersTag << std::endl;
		}
		if (unlockAllStagesFlag) {
			saveFile << fk_saveFileAddons::UnlockAllStagesTag << std::endl;
		}
		if (unlockExtraCostumesForeverFlag) {
			saveFile << fk_saveFileAddons::UnlockExtraCostumesTag << std::endl;
		}
		// save story episodes cleared
		saveFile << fk_saveFileAddons::StoryEpisodesClearedForSaveFileTag << std::endl;
		for each(std::string episode in storyEpisodesCleared){
			saveFile << episode << std::endl;
		}
		// save extra tags
		saveFile << fk_saveFileAddons::CollectedTagsForSaveFileTags << std::endl;
		for each(std::string tag in specialTagsAcquired) {
			saveFile << tag << std::endl;
		}
		// save tutorial challenges cleared
		saveFile << fk_saveFileAddons::ChallengeClearedForSaveFileTag << std::endl;
		for each(std::string challenge in tutorialChallengesCleared){
			saveFile << challenge << std::endl;
		}
		// save character data
		saveFile << std::endl << fk_saveFileAddons::CharacterStatisticsForSaveFileTag << std::endl;
		for each (std::string path in characterPaths){
			std::string key = getCharacterNameFromFile(charactersDirectory + path + "\\");
			if (characterStatisticsStructure.count(key) == 0){
				characterStatisticsStructure[key] = FK_CharacterStatistics();
				std::replace(key.begin(), key.end(), ' ', '_');
				characterStatisticsStructure[key].characterName = key;
			}
			for each (std::string vs_path in characterPaths){
				std::string key_vs = getCharacterNameFromFile(charactersDirectory + vs_path + "\\");
				if (characterStatisticsStructure[key].VsRecords.count(key_vs) == 0){
					characterStatisticsStructure[key].VsRecords[key_vs] = FK_CharacterVSStatistics();
					characterStatisticsStructure[key].VsRecords[key_vs].characterName = key_vs;
				}
			}
			fk_saveFileAddons::writeCharacterStatisticsToFile(characterStatisticsStructure[key], saveFile);
		}
		saveFile.clear();
		saveFile.close();
	}
	void FK_SceneWithInput::readSaveFile(){
		std::ifstream saveFile(configurationFilesPath + fk_constants::FK_SaveFileName);
		std::string charactersDirectory = mediaPath + fk_constants::FK_CharactersFileFolder;
		std::vector<std::string> characterPaths;
		// load character file
		std::string characterFileName = charactersDirectory + fk_constants::FK_CharacterRosterFileName;
		std::ifstream characterFile(characterFileName);
		while (!characterFile.eof()){
			std::string charaPath;
			s32 unlockId;
			characterFile >> charaPath >> unlockId;
			//characterStatisticsStructure[charaPath] = FK_CharacterStatistics();
			characterPaths.push_back(charaPath);
		};
		characterFile.close();
		if (!saveFile){
			saveFile.clear();
			saveFile.close();
			writeSaveFile();
		}
		else{
			// load global statistics
			std::string tempTag;
			while (saveFile >> tempTag){
				if (tempTag == fk_saveFileAddons::StoryEpisodesClearedForSaveFileTag){
					break;
				}
				else if (tempTag == fk_saveFileAddons::UnlockAllContentsTag){
					unlockAllFlag = true;
				}
				else if (tempTag == fk_saveFileAddons::UnlockAllCharactersTag) {
					unlockAllCharactersFlag = true;
				}
				else if (tempTag == fk_saveFileAddons::UnlockAllStagesTag) {
					unlockAllStagesFlag = true;
				}
				else if (tempTag == fk_saveFileAddons::UnlockExtraCostumesTag) {
					unlockExtraCostumesForeverFlag = true;
				}
				else if (tempTag == fk_saveFileAddons::TutorialForSaveFileTag){
					u32 tutorialShownFlag, unlockablesChoicePresentedFlag;
					saveFile >> tutorialShownFlag >> unlockablesChoicePresentedFlag;
					tutorialPresented = tutorialShownFlag > 0;
					unlockablesChoicePresented = unlockablesChoicePresentedFlag > 0;
				}
				else if (tempTag == fk_saveFileAddons::AILevelTagForArcade){
					u32 level, number;
					saveFile >> level >> number;
					numberOfArcadeBeatenPerDifficultyLevel[level] = number;
				}
				else if (tempTag == fk_saveFileAddons::AILevelTagForSurvivalArcade) {
					u32 level, number;
					std::string path;
					saveFile >> level >> number >> path;
					survivalRecordPerDifficultyLevel[level].first = path;
					survivalRecordPerDifficultyLevel[level].second = number;
				}
				else if (tempTag == fk_saveFileAddons::AILevelTagForTimeAttackArcade) {
					u32 level, number;
					std::string path;
					saveFile >> level >> number >> path;
					timeAttackRecordPerDifficultyLevel[level].first = path;
					timeAttackRecordPerDifficultyLevel[level].second = number;
				}
			}
			// load story chapters cleared
			while (saveFile >> tempTag){
				if (tempTag == fk_saveFileAddons::CharacterStatisticsForSaveFileTag || 
					tempTag == fk_saveFileAddons::ChallengeClearedForSaveFileTag ||
					tempTag == fk_saveFileAddons::CollectedTagsForSaveFileTags){
					break;
				}
				else{
					storyEpisodesCleared.push_back(tempTag);
				}
			}
			// load collected tags
			if (tempTag == fk_saveFileAddons::CollectedTagsForSaveFileTags) {
				while (saveFile >> tempTag) {
					if (tempTag == fk_saveFileAddons::CharacterStatisticsForSaveFileTag ||
						tempTag == fk_saveFileAddons::ChallengeClearedForSaveFileTag) {
						break;
					}
					else {
						specialTagsAcquired.push_back(tempTag);
					}
				}
			}
			// tutorial challenges
			if (tempTag == fk_saveFileAddons::ChallengeClearedForSaveFileTag){
				while (saveFile >> tempTag){
					if (tempTag == fk_saveFileAddons::CharacterStatisticsForSaveFileTag){
						break;
					}
					else{
						tutorialChallengesCleared.push_back(tempTag);
					}
				}
			}
			// load character statistics
			FK_CharacterStatistics temp;
			while (saveFile){
				if (fk_saveFileAddons::readCharacterStatisticsFromFile(temp, saveFile)){
					std::string key = temp.characterName;
					characterStatisticsStructure[key] = temp;
					for each (std::string vs_path in characterPaths){
						std::string key_vs = getCharacterNameFromFile(charactersDirectory + vs_path + "\\");
						if (characterStatisticsStructure[key].VsRecords.count(key_vs) == 0){
							characterStatisticsStructure[key].VsRecords[key_vs] = FK_CharacterVSStatistics();
							characterStatisticsStructure[key].VsRecords[key_vs].characterName = key_vs;
						}
					}
				}
			}
			// add missing characters
			for each (std::string path in characterPaths){
				std::string key = getCharacterNameFromFile(charactersDirectory + path + "\\");
				if (characterStatisticsStructure.count(key) == 0){
					//std::cout << key << " " << characterStatisticsStructure[key].characterName << (key == characterStatisticsStructure[key].characterName) << std::endl;
					characterStatisticsStructure[key] = FK_CharacterStatistics();
					characterStatisticsStructure[key].characterName = key;
					for each (std::string vs_path in characterPaths){
						std::string key_vs = getCharacterNameFromFile(charactersDirectory + vs_path + "\\");
						if (characterStatisticsStructure[key].VsRecords.count(key_vs) == 0){
							characterStatisticsStructure[key].VsRecords[key_vs] = FK_CharacterVSStatistics();
							characterStatisticsStructure[key].VsRecords[key_vs].characterName = key_vs;
						}
					}
				}
			}
		}
		saveFile.clear();
		saveFile.close();
		// now, create global statistics for general/reward use
		characterStatisticsStructure[fk_constants::GlobalStatsIdentifier] = FK_CharacterStatistics();
		characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].characterName = fk_constants::GlobalStatsIdentifier;
		for each (std::string path in characterPaths){
			std::string key = getCharacterNameFromFile(charactersDirectory + path + "\\");
			if (characterStatisticsStructure.count(key) > 0){
				characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].numberOfBeatenArcadeMode +=
					characterStatisticsStructure[key].numberOfBeatenArcadeMode;
				characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].numberOfFreeMatchesPlayed +=
					characterStatisticsStructure[key].numberOfFreeMatchesPlayed;
				characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].numberOfContinues +=
					characterStatisticsStructure[key].numberOfContinues;
				characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].numberOfRingoutPerformed +=
					characterStatisticsStructure[key].numberOfRingoutPerformed;
				characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].numberOfPerfectWins +=
					characterStatisticsStructure[key].numberOfPerfectWins;
			}
			// halve the  number of free matches to keep in mind that two players are there at the same time
			characterStatisticsStructure[fk_constants::GlobalStatsIdentifier].numberOfFreeMatchesPlayed /= 2;
		}
		// read cheats
		readCheatsFile();
		// check rewards
		readRewardFile();
		// check reward pictures
		checkGalleryPictures();
		// load workshop items 
		loadWorkshopContent();
		// load DLC items
		loadDLCContent();
	}

	void FK_SceneWithInput::loadWorkshopContent()
	{
		FK_WorkshopContentManager workshopManager;
		std::string filename = configurationFilesPath + fk_constants::FK_WorkshopContentFileName;
		workshopManager.loadFromFile(filename);
		enabledWorkshopItems = workshopManager.getEnabledItems();
	}
	void FK_SceneWithInput::loadDLCContent()
	{
		FK_WorkshopContentManager dlcManager;
		std::string filename = configurationFilesPath + fk_constants::FK_DLCContentFileName;
		dlcManager.loadFromFile(filename);
		enabledDLCItems = dlcManager.getEnabledItems();
	}
	void FK_SceneWithInput::updateSaveFileData(){

	}

	// cheat codes
	u32 FK_SceneWithInput::getCurrentActiveCheatCodes()
	{
		return currentActiveCheatCodes;
	}

	void FK_SceneWithInput::setCurrentActiveCheatCodes(u32 newCheatCodes) {
		if (currentActiveCheatCodes != newCheatCodes) {
			currentActiveCheatCodes = newCheatCodes;
		}
	}


	// joypad setup window
	void FK_SceneWithInput::setupJoypadConfigWindow(std::vector<std::string>& content) {
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		s32 width = screenSize.Width * 3 / 4;
		s32 x = (screenSize.Width - width) / 2;
		s32 height = 3 * screenSize.Height / 16;
		s32 y = (screenSize.Height - height) / 2;
		core::rect<s32> frame(x, y, x + width, y + height);
		//f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		//core::rect<s32> frame(50, 140, 590, 220);
		if (newJoypadWindow != NULL) {
			delete newJoypadWindow;
			newJoypadWindow = NULL;
		}
		newJoypadWindow = new FK_DialogueWindow(device, frame, "windowskin.png", commonResourcesPath,
			1.f,
			std::string(), content, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
	}

	// set joypad config window text
	void FK_SceneWithInput::setJoypadConfigWindowText(std::vector<std::string>& content) {
		if (newJoypadWindow == NULL) {
			return;
		}
		newJoypadWindow->reset(1.f, std::string(), content, FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::AllTogether);
	}

	/* a boolean which returns if sepia effect has to be shown */
	f32* FK_SceneWithInput::getSceneParametersForShader(FK_SceneShaderTypes newShader) {
		return NULL;
	}

	u32 FK_SceneWithInput::getMenuInputTimeThresholdMS() {
		if (timeSinceLastInputMS < TotalTimeBeforeNoThresholdInMenuInputsMS) {
			return TimeThresholdForMenuInputsMS;
		}
		else {
			return TimeThresholdForMenuInputMinimumMS;
		}
	}

	void FK_SceneWithInput::updateMenuInputTimeThresholdMS(u32 inputPressed, u32 delta_t_ms) {
		u32 directionPressed = inputPressed & FK_Input_Buttons::Any_Direction;
		if ((directionPressed & lastMenuInputDirectionPressed) != 0) {
			timeSinceLastInputMS += delta_t_ms;
		}
		else {
			timeSinceLastInputMS = 0;
			lastMenuInputDirectionPressed = directionPressed;
		}
	}

	void FK_SceneWithInput::resetInputRoutines(bool createInputMapIfNotFound)
	{
		setupJoypad();
		setupInputMapper(createInputMapIfNotFound);
	}

	void FK_SceneWithInput::activateJoypadConfigWindow() {
		if (newJoypadWindow == NULL) {
			return;
		}
		newJoypadWindow->open(200);
		newJoypadWindow->setVisibility(true);
		newJoypadWindow->setActive(true);
	}

	void FK_SceneWithInput::deactivateJoypadConfigWindow() {
		if (newJoypadWindow == NULL) {
			return;
		}
		newJoypadWindow->close(200);
	}

	// load all pictures for the gallery
	void FK_SceneWithInput::checkGalleryPictures() {
		std::string path = mediaPath + "scenes\\gallery\\pictures.txt";
		std::ifstream unlockablePicturesFile(path.c_str());
		if (!unlockablePicturesFile) {
			return;
		}

		FK_RewardUnlockStruct unlockingParameters;
		unlockingParameters.characterStats = characterStatisticsStructure;
		unlockingParameters.arcadeWins = numberOfArcadeBeatenPerDifficultyLevel;
		unlockingParameters.isTutorialCleared = tutorialCleared;
		unlockingParameters.storyEpisodesCleared = storyEpisodesCleared;
		unlockingParameters.challengesCleared = tutorialChallengesCleared;
		unlockingParameters.unlockedCharacters = unlockedCharacters;

		std::string pictureFolderBeginTag = "#folder";
		std::string pictureEnclosureBeginTag = "#new_picture";
		std::string pictureEnclosureEndTag = "#new_picture_end";
		std::string pictureFrameTag = "#frame";
		std::string temp;
		std::string tempFolder = mediaPath;
		u32 pictureNumber = 0;
		while (unlockablePicturesFile) {
			unlockablePicturesFile >> temp;
			if (temp == pictureFolderBeginTag) {
				unlockablePicturesFile >> temp;
				tempFolder = mediaPath + temp;
			}
			else if (temp == pictureEnclosureBeginTag) {
				FK_Reward reward;
				bool success = reward.readFromFile(unlockablePicturesFile);
				if (success) {
					pictureNumber += 1;
					bool rewardUnlocked = 
						FK_SceneWithInput::unlockAllFlag || reward.rewardIsUnlocked(unlockingParameters);
					if(rewardUnlocked){
						unlockedGalleryPictures.push_back(pictureNumber);
					}
				}
			}
		}
		currentlyUnlockedContent.unlockedGalleryPictures = unlockedGalleryPictures;
	}

	/* set button icon */
	void FK_SceneWithInput::setIconsBasedOnCurrentInputMap(
		std::map<FK_Input_Buttons, u32>&buttonInputLayout,
		std::map<FK_Input_Buttons, std::wstring> &keyboardKeyLayout,
		s32 mapIndex, bool useJoypadMap, std::string joypadName) {
		if (useJoypadMap && joypadName.empty()) {
			return;
		}
		std::map<FK_Input_Buttons, u32> mapToCheck;
		if (useJoypadMap) {
			mapToCheck = std::map<FK_Input_Buttons, u32>(inputMapper->getJoypadMap(mapIndex));

			buttonInputLayout[FK_Input_Buttons::Selection_Button] =
				inputMapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative)mapToCheck[FK_Input_Buttons::Selection_Button]);
			buttonInputLayout[FK_Input_Buttons::Modifier_Button] =
				inputMapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative)mapToCheck[FK_Input_Buttons::Modifier_Button]);
			buttonInputLayout[FK_Input_Buttons::ScrollRight_Button] =
				inputMapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative)mapToCheck[FK_Input_Buttons::ScrollRight_Button]);
			buttonInputLayout[FK_Input_Buttons::ScrollLeft_Button] =
				inputMapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative)mapToCheck[FK_Input_Buttons::ScrollLeft_Button]);
			buttonInputLayout[FK_Input_Buttons::Cancel_Button] =
				inputMapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative)mapToCheck[FK_Input_Buttons::Cancel_Button]);
			buttonInputLayout[FK_Input_Buttons::Help_Button] =
				inputMapper->getTranslatedButtonFromJoypadMap(joypadName,
				(FK_JoypadInputNative)mapToCheck[FK_Input_Buttons::Help_Button]);
		}
		else {
			mapToCheck = std::map<FK_Input_Buttons, u32>(inputMapper->getKeyboardMap(mapIndex));
			// create map
			std::map <EKEY_CODE, std::wstring> acceptedKeys;
			// directions
			acceptedKeys[EKEY_CODE::KEY_UP] = L"Up";
			acceptedKeys[EKEY_CODE::KEY_DOWN] = L"Down";
			acceptedKeys[EKEY_CODE::KEY_LEFT] = L"Left";
			acceptedKeys[EKEY_CODE::KEY_RIGHT] = L"Right";

			// letters and numbers
			std::vector <std::wstring> letters = {
				L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J",
				L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T",
				L"U", L"V", L"W", L"X", L"Y", L"Z"
			};
			std::vector <std::wstring> numbers = {
				L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9",
			};
			for (u32 i = (u32)(EKEY_CODE::KEY_KEY_A); i <= (u32)(EKEY_CODE::KEY_KEY_Z); ++i) {
				acceptedKeys[(EKEY_CODE)i] = letters[i - (u32)(EKEY_CODE::KEY_KEY_A)];
			}
			for (u32 i = (u32)(EKEY_CODE::KEY_KEY_0); i <= (u32)(EKEY_CODE::KEY_KEY_9); ++i) {
				acceptedKeys[(EKEY_CODE)i] = numbers[i - (u32)(EKEY_CODE::KEY_KEY_0)];
			}

			// additional signs
			acceptedKeys[EKEY_CODE::KEY_PLUS] = L"+";  // PLUS key
			acceptedKeys[EKEY_CODE::KEY_MINUS] = L"-";  // MINUS key
			acceptedKeys[EKEY_CODE::KEY_COMMA] = L",";  // COMMA key
			acceptedKeys[EKEY_CODE::KEY_PERIOD] = L".";  // PERIOD key

														 // function buttons
			acceptedKeys[EKEY_CODE::KEY_BACK] = L"BCK";  // BACKSPACE key
			acceptedKeys[EKEY_CODE::KEY_TAB] = L"TAB";  // TAB key
			acceptedKeys[EKEY_CODE::KEY_CLEAR] = L"CLR";  // CLEAR key
			acceptedKeys[EKEY_CODE::KEY_RETURN] = L"ENTER";  // ENTER key
			acceptedKeys[EKEY_CODE::KEY_LSHIFT] = L"SHIFT";  // SHIFT key
			acceptedKeys[EKEY_CODE::KEY_LCONTROL] = L"L CTRL";  // CTRL key
			acceptedKeys[EKEY_CODE::KEY_LMENU] = L"L ALT";  // ALT key
			acceptedKeys[EKEY_CODE::KEY_RSHIFT] = L"R SHIFT";  // SHIFT key
			acceptedKeys[EKEY_CODE::KEY_RCONTROL] = L"R CTRL";  // CTRL key
			acceptedKeys[EKEY_CODE::KEY_RMENU] = L"R ALT";  // ALT key
			acceptedKeys[EKEY_CODE::KEY_PAUSE] = L"PAUSE";  // PAUSE key
			acceptedKeys[EKEY_CODE::KEY_INSERT] = L"INS";  // INSERT key
			acceptedKeys[EKEY_CODE::KEY_DELETE] = L"DEL";  // DELETE key
			acceptedKeys[EKEY_CODE::KEY_SPACE] = L"SPACE";  // SPACE key
			acceptedKeys[EKEY_CODE::KEY_PRIOR] = L"Pg-";  // PG UP key
			acceptedKeys[EKEY_CODE::KEY_NEXT] = L"Pg+";  // PG DOWN key
			acceptedKeys[EKEY_CODE::KEY_END] = L"END";  // END key
			acceptedKeys[EKEY_CODE::KEY_HOME] = L"HOME";  // HOME key
			acceptedKeys[EKEY_CODE::KEY_CAPITAL] = L"CAPS"; // CAPS LOCK key
			acceptedKeys[EKEY_CODE::KEY_ESCAPE] = L"ESC"; //ESCAPE key

														  // numpad
			std::vector <std::wstring> numpad = {
				L"N0",L"N1",L"N2",L"N3",L"N4",L"N5",L"N6",L"N7",L"N8",L"N9",
				L"N*", L"N+", L"N.", L"N-", L"N,", L"N/"
			};
			for (u32 i = (u32)(EKEY_CODE::KEY_NUMPAD0); i <= (u32)(EKEY_CODE::KEY_DIVIDE); ++i) {
				acceptedKeys[(EKEY_CODE)i] = numpad[i - (u32)(EKEY_CODE::KEY_NUMPAD0)];
			}

			//OEM keys
			acceptedKeys[EKEY_CODE::KEY_OEM_1] = L"OEM1";
			acceptedKeys[EKEY_CODE::KEY_OEM_2] = L"OEM2";
			acceptedKeys[EKEY_CODE::KEY_OEM_3] = L"OEM3";
			acceptedKeys[EKEY_CODE::KEY_OEM_4] = L"OEM4";
			acceptedKeys[EKEY_CODE::KEY_OEM_5] = L"OEM5";
			acceptedKeys[EKEY_CODE::KEY_OEM_6] = L"OEM6";
			acceptedKeys[EKEY_CODE::KEY_OEM_7] = L"OEM7";
			acceptedKeys[EKEY_CODE::KEY_OEM_8] = L"OEM8";
			acceptedKeys[EKEY_CODE::KEY_OEM_102] = L"OEM9";

			//F keys
			acceptedKeys[EKEY_CODE::KEY_F1] = L"F1";
			acceptedKeys[EKEY_CODE::KEY_F2] = L"F2";
			acceptedKeys[EKEY_CODE::KEY_F3] = L"F3";
			acceptedKeys[EKEY_CODE::KEY_F4] = L"F4";
			acceptedKeys[EKEY_CODE::KEY_F5] = L"F5";
			acceptedKeys[EKEY_CODE::KEY_F6] = L"F6";
			acceptedKeys[EKEY_CODE::KEY_F7] = L"F7";
			acceptedKeys[EKEY_CODE::KEY_F8] = L"F8";
			acceptedKeys[EKEY_CODE::KEY_F9] = L"F9";
			acceptedKeys[EKEY_CODE::KEY_F10] = L"F10";
			acceptedKeys[EKEY_CODE::KEY_F11] = L"F11";
			acceptedKeys[EKEY_CODE::KEY_F12] = L"F12";
			acceptedKeys[EKEY_CODE::KEY_F13] = L"F13";
			acceptedKeys[EKEY_CODE::KEY_F14] = L"F14";
			acceptedKeys[EKEY_CODE::KEY_F15] = L"F15";
			acceptedKeys[EKEY_CODE::KEY_F16] = L"F16";
			acceptedKeys[EKEY_CODE::KEY_F17] = L"F17";
			acceptedKeys[EKEY_CODE::KEY_F18] = L"F18";
			acceptedKeys[EKEY_CODE::KEY_F19] = L"F19";
			acceptedKeys[EKEY_CODE::KEY_F20] = L"F20";
			acceptedKeys[EKEY_CODE::KEY_F21] = L"F21";
			acceptedKeys[EKEY_CODE::KEY_F22] = L"F22";
			acceptedKeys[EKEY_CODE::KEY_F23] = L"F23";
			acceptedKeys[EKEY_CODE::KEY_F24] = L"F24";

			// read real input
			keyboardKeyLayout[FK_Input_Buttons::Selection_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::Selection_Button]];
			keyboardKeyLayout[FK_Input_Buttons::Modifier_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::Modifier_Button]];
			keyboardKeyLayout[FK_Input_Buttons::ScrollRight_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::ScrollRight_Button]];
			keyboardKeyLayout[FK_Input_Buttons::ScrollLeft_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::ScrollLeft_Button]];
			keyboardKeyLayout[FK_Input_Buttons::Cancel_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::Cancel_Button]];
			keyboardKeyLayout[FK_Input_Buttons::Help_Button] =
				acceptedKeys[(EKEY_CODE)mapToCheck[FK_Input_Buttons::Help_Button]];
		}
	}
}