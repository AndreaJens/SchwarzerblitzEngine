#include "FK_SceneGame.h"
#include"ExternalAddons.h"
#include"FK_CharacterMaterialShaderCallback.h"
#include <algorithm>
#include <stdlib.h>
#include <SFML\Audio.hpp>
#include "Shlwapi.h"
//#include <thread>

//#define DEBUG_FLAG_CHARACTER true
//#define WINE_COMPATIBLE_VERSION true

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{

	/* create the SceneGame object */
	FK_SceneGame::FK_SceneGame(){
		abortMatch = false;
		pausePlayer1 = false;
		pausePlayer2 = false;
		showMoveListPlayer1 = false;
		showMoveListPlayer2 = false;
		isGameOver = false;
		player1 = new FK_Character;
		player2 = new FK_Character;
		stage = new FK_Stage;
		announcerLoaded = false;
		soundsLoaded = false;
		charactersLoaded = false;
		stageLoaded = false;
		hudLoaded = false;
		allLoaded.store(false);
		loadingPercentage.store(0.0f);
		player1OldHitStun = false;
		player2OldHitStun = false;
		skipIntro = false;
		freeCameraModeActive = false;
		freeCameraSnap = false;
		freeCameraCanTakePicture = false;
		freeCameraSnapCounter = 0;
		resetThrowAttempt();
		matchStatistics.clear();
		impactEffectCounter = -1;
		matchStartingTime = -1;
		player1LifeMultiplier = 1.0f;
		player2LifeMultiplier = 1.0f;
		triggerRegenCounterMSPlayer1 = 0;
		triggerRegenCounterMSPlayer2 = 0;
		damageScaling = std::vector<f32>{
			1.0f,
			0.8f,
			0.6f,
			0.4f,
			0.3f,
			0.1f,
		};
		needMeshSynchronization = false;
		cinematicCameraActivePlayer1 = false;
		cinematicCameraActivePlayer2 = false;
		updateCounterAttackEffectFlag = false;
		counterAttackEffectCounterMs = 0;
		counterAttackEffectCooldownCounterMs = 0;
		triggerTimer = 0;
		player1CollisionCallback = NULL;
		player2CollisionCallback = NULL;
		lastSkipInputTime = 0;
		moveListPlayer1 = NULL;
		moveListPlayer2 = NULL;
		hudManager = NULL;
	};
	/* create the setup function */
	void FK_SceneGame::setup(IrrlichtDevice *newDevice,
		core::array<SJoystickInfo> new_joypadInfo,
		FK_Options* newOptions,
		std::string new_player1path,
		std::string new_player2path,
		FK_Character::FK_CharacterOutfit newPlayer1Outfit,
		FK_Character::FK_CharacterOutfit newPlayer2Outfit,
		std::string new_arenaPath,
		FK_SceneType currentSceneType,
		FK_AdditionalSceneGameOptions newAdditionalOptions,
		bool player1AI, bool player2AI,
		int new_player1AILevel, int new_player2AILevel){
		/* call method from superclass*/
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		/* proceed with storing the interesting variables */
		setupResourcesPaths(new_player1path, new_player2path, new_arenaPath);
		additionalOptions = newAdditionalOptions;
		sceneType = currentSceneType;
		cycleId = 0;
		now = 0;
		then = 0;
		nowReal = 0;
		thenReal = 0;
		pause = false;
		pauseButtonDown = false;
		timeupRoundEnd = false;
		//startRoundTimerMs = fk_constants::FK_MaxRoundTimerSeconds * 1000;
		startRoundTimerMs = additionalOptions.roundTimerInSeconds * 1000.0;
		roundTimerMs = startRoundTimerMs;
		totalMatchTimeMs = 0;
		// number of rounds
		if (additionalOptions.numberOfRounds > 0){
			numberOfRounds = additionalOptions.numberOfRounds;
		}
		else{
			numberOfRounds = gameOptions->getNumberOfRoundsFreeMatch();
		}
		if (additionalOptions.numberOfRoundsPlayer1MustWin > 0){
			numberOfRoundsPlayer1 = additionalOptions.numberOfRoundsPlayer1MustWin;
		}
		else{
			numberOfRoundsPlayer1 = std::floor(numberOfRounds / 2) + 1;
		}
		if (additionalOptions.numberOfRoundsPlayer2MustWin > 0){
			numberOfRoundsPlayer2 = additionalOptions.numberOfRoundsPlayer2MustWin;
		}
		else{
			numberOfRoundsPlayer2 = std::floor(numberOfRounds / 2) + 1;
		}
		shadowFilterType = E_FILTER_TYPE::EFT_NONE;
		shadowModeCharacters = E_SHADOW_MODE::ESM_BOTH;
		shadowModeStage = E_SHADOW_MODE::ESM_BOTH;
		playReplay = additionalOptions.playReplay;
		saveReplay = additionalOptions.saveReplay;
		player1LifeMultiplier = additionalOptions.lifeMultiplierPlayer1;
		player2LifeMultiplier = additionalOptions.lifeMultiplierPlayer2;
		replayTimeInformationRead = false;
		player1isAI = player1AI;
		player2isAI = player2AI;
		player1AILevel = new_player1AILevel;
		player2AILevel = new_player2AILevel;
		player1Outfit = newPlayer1Outfit;
		player2Outfit = newPlayer2Outfit;
		scenePixelShaderParameters = new f32[4];
		player1WasJumping = false;
		player2WasJumping = false;
		for (int i = 0; i < 4; ++i){
			scenePixelShaderParameters[i] = 0.0f;
		}
		// read save file
		FK_SceneWithInput::readSaveFile();
		// setup additional variables for every subclass
		setupAdditionalVariables();
		// load elements for drawing the loading screen
		initialize();
	};

	// make a basic setup for player input
	void FK_SceneGame::setupInputForPlayers(){
		// in case there are no joypads available, split keyboard
		s32 maxNumberOfRecordedButtonInInputBuffer = 10;
		s32 timeWindowBetweenInputBeforeClearing_ms = 200;
		s32 mergeTimeForInputAtSameFrame_ms = 20;
		s32 timeToRegisterHeldButton_ms = 125; //it was originally 40.
		s32 timeToRegisterNullInput_ms = 200;
		if (!(player1isAI || player2isAI) && joystickInfo.size() <= 0){
			player1input = new FK_InputBuffer(1, inputMapper->getKeyboardMap(1), 
				timeWindowBetweenInputBeforeClearing_ms, 
				mergeTimeForInputAtSameFrame_ms, 
				timeToRegisterHeldButton_ms, 
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
			player2input = new FK_InputBuffer(2, inputMapper->getKeyboardMap(2), 
				timeWindowBetweenInputBeforeClearing_ms, 
				mergeTimeForInputAtSameFrame_ms, 
				timeToRegisterHeldButton_ms, 
				timeToRegisterNullInput_ms,
				maxNumberOfRecordedButtonInInputBuffer);
		}
		else if (player1isAI && !player2isAI && joystickInfo.size() <= 0){
			player1input = new FK_InputBuffer(1, inputMapper->getKeyboardMap(-1), 
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
			FK_SceneWithInput::setupInputForPlayers(timeWindowBetweenInputBeforeClearing_ms,
				mergeTimeForInputAtSameFrame_ms, timeToRegisterHeldButton_ms,
				timeToRegisterNullInput_ms,	maxNumberOfRecordedButtonInInputBuffer);
		}
		setupMoveListsIconSetsBasedOnInput();
	}

	void FK_SceneGame::setupResourcesPaths(std::string new_player1path, std::string new_player2path, std::string new_arenaPath){
		player1_configPath = new_player1path;
		player2_configPath = new_player2path;
		arena_configPath = new_arenaPath;
		stagesPath = mediaPath + "stages\\";
		charactersPath = mediaPath + "characters\\";
		commonResourcesPath = mediaPath + "common\\";
		voiceClipsPath = mediaPath + "voice_clips\\";
	}

	/* initialize all the objects */
	void FK_SceneGame::initialize(){
		setupIrrlichtDevice();
		/* create render targets */
		createRenderTargetTextures();
		setupJoypad();
		setupInputMapper();
		loadLoadingTextures();
		setupGameObjects();
		setupLights();
		setupShaders();
		setupPauseMenu();
		setupTutorial();
		drawLoadingScreen(100.f);
	};

	// check if scene is running
	bool FK_SceneGame::isRunning(){
		return (!abortMatch);// && !processingSceneEnd);
	};

	// update input for pause and skip
	void FK_SceneGame::updateInputForPauseMenuAndSkipOptions() {
		/* update the input just for reading pause-related issues*/
		if (!canUpdateInput() || (player1isAI && player2isAI) || isPaused()) {
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
		}
		u32 currentSkipInput = player1input->getPressedButtons() | player2input->getPressedButtons();
		currentSkipInput &= (FK_Input_Buttons::Confirmation_Button | FK_Input_Buttons::Menu_Pause_Button |
			FK_Input_Buttons::Selection_Button);
		u32 currentButtonPlayer1ForPausePurpose = player1input->getPressedButtons() & fk_constants::FK_PauseGameButton;
		u32 currentButtonPlayer2ForPausePurpose = player2input->getPressedButtons() & fk_constants::FK_PauseGameButton;

		/* check if pause button is pressed */
		if (isPaused()) {
			if (freeCameraModeActive) {
				u32 deltaT = device->getTimer()->getRealTime();
				deltaT -= freeCameraParameters.lastCameraTimeUpdateMs;
				freeCameraParameters.lastCameraTimeUpdateMs = device->getTimer()->getRealTime();
				u32 buttonsPressed = 0;
				if (pausePlayer1) {
					buttonsPressed = player1input->getPressedButtons();
				}
				else if (pausePlayer2) {
					buttonsPressed = player2input->getPressedButtons();
				}
				else {
					buttonsPressed = (player1input->getPressedButtons() |
						player2input->getPressedButtons());
				}
				updateFreeCameraPosition(buttonsPressed, deltaT);
			}
			else if (isShowingMoveList()) {
				updateMoveListPanel();
			}
			else {
				updatePauseMenu();
				/* check if pause button has been pressed */
				if (!showTutorial &&
					!pauseButtonDown &&
					(pausePlayer1 && currentButtonPlayer1ForPausePurpose == fk_constants::FK_PauseGameButton ||
						pausePlayer2 && currentButtonPlayer2ForPausePurpose == fk_constants::FK_PauseGameButton)
					) {
					resumeGame();
					pauseButtonDown = true;
					//return;
				}
				else if (!(pausePlayer1 && currentButtonPlayer1ForPausePurpose == fk_constants::FK_PauseGameButton ||
					pausePlayer2 && currentButtonPlayer2ForPausePurpose == fk_constants::FK_PauseGameButton)) {
					//(!inputReceiver->IsKeyDown(EKEY_CODE::KEY_RETURN)){
					pauseButtonDown = false;
				}
			}
		}
		else {
			/* check if pause button has been pressed */
			if (!pauseButtonDown && (currentButtonPlayer1ForPausePurpose == fk_constants::FK_PauseGameButton ||
				currentButtonPlayer2ForPausePurpose == fk_constants::FK_PauseGameButton) &&
				canPauseGame()) {
				pauseGame();
				pauseButtonDown = true;
				if (currentButtonPlayer2ForPausePurpose == fk_constants::FK_PauseGameButton ||
					(player1isAI && !player2isAI)) {
					pausePlayer2 = true;
				}
				else {
					pausePlayer1 = true;
				}
				//return;
			}
			else if (!(currentButtonPlayer1ForPausePurpose == fk_constants::FK_PauseGameButton ||
				currentButtonPlayer2ForPausePurpose == fk_constants::FK_PauseGameButton)) {
				pauseButtonDown = false;
			}
		}

		// skip intro
		if (isProcessingIntro() && currentSkipInput != 0) {
			if (now - lastSkipInputTime > 200) {
				processingIntroPlayer1 = false;
				processingIntroPlayer2 = false;
				processingIntroStage = false;
				lastSkipInputTime = now;
				inputReceiver->reset();
				pauseButtonDown = currentButtonPlayer1ForPausePurpose == fk_constants::FK_PauseGameButton ||
					currentButtonPlayer2ForPausePurpose == fk_constants::FK_PauseGameButton;
			}
		}
		// skip round win
		if (!isEndOfMatch() && processingWinText && currentSkipInput != 0) {
			if (now - lastSkipInputTime > 500) {
				processingWinText = false;
				newRoundFlag = true;
				lastSkipInputTime = now;
				inputReceiver->reset();
			}
		}
	}

	// add move hitflag does nothing outside training mode
	void FK_SceneGame::addMoveHitFlag(ISceneNode *, FK_Hitbox *, s32, s32, bool)
	{
	}

	// frame advatnage (nothing outside training mode)
	bool FK_SceneGame::calculateFrameAdvantage(s32&, s32&, FK_Hitbox *, f32, FK_Character *, FK_Character *, bool, bool, bool)
	{
		return false;
	}

	void FK_SceneGame::draw(f32 frameDeltaTimeS) {
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		// execute post processing routine (e.g. in borderless window mode)
		// update light manager
		//smgr->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);

		// get render target
		video::ITexture* targetTex = getRenderTarget();
		lightManager->update((video::ITexture*)targetTex);
		// draw HUD
		if (getRenderTargetQuality() >= 0 && targetTex != NULL) {
			video::ITexture* renderTarget = gameOptions->getBorderlessWindowMode() ? borderlessWindowRenderTarget : 0;
			driver->setRenderTarget(renderTarget, ECBF_COLOR | ECBF_DEPTH);
			core::vector2df scale = getRenderTargetScaleFactor();
			core::recti viewport(0, 0, 0, 0);
			if (gameOptions->getBorderlessWindowMode()) {
				viewport.LowerRightCorner.X = (s32)borderlessWindowRenderTarget->getSize().Width;
				viewport.LowerRightCorner.Y = (s32)borderlessWindowRenderTarget->getSize().Height;
			}
			fk_addons::draw2DImage(driver, targetTex,
				core::rect<s32>(0, 0, targetTex->getSize().Width, targetTex->getSize().Height),
				core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, 
				false,
				video::SColor(255,255,255,255),
				viewport);
		}
		//! draw HUD
		//driver->setRenderTarget(gameOptions->getBorderlessWindowMode() ? borderlessWindowRenderTarget : 0, 0);
		if (!freeCameraModeActive) {
			drawHUD(frameDeltaTimeS);
		}
		else {
			if (freeCameraSnap) {
				core::dimension2d<u32> screenSize = driver->getScreenSize();
				if (gameOptions->getBorderlessWindowMode()) {
					screenSize = borderlessWindowRenderTarget->getSize();
				}
				f32 alphaParameter = 2 * (f32)freeCameraSnapCounter / (f32)freeCameraParameters.snapEffectDurationMs;
				s32 alpha = 1 + (s32)std::round(255 * alphaParameter);
				if (alphaParameter >= 1.f) {
					alpha = 255;
				}
				if (alphaParameter >= 2.f) {
					freeCameraCanTakePicture = true;
				}
				else {
					core::rect<s32> screenRect = core::rect<s32>(0, 0, (s32)screenSize.Width, (s32)screenSize.Height);
					driver->draw2DRectangle(video::SColor(alpha, 255, 255, 255), screenRect);
				}
			}
		}

		if (isPaused()) {
			if (!freeCameraModeActive) {
				if (isShowingMoveList()) {
					if (showMoveListPlayer1) {
						moveListPlayer1->draw();
					}
					else {
						moveListPlayer2->draw();
					}
				}
				else {
					drawPauseMenu();
				}
			}
		}
		//guienv->drawAll();
		// execute post processing routine (e.g. in borderless window mode)
		FK_SceneWithInput::executePostRenderingRoutine();
		/* end scene */
		driver->endScene();
	}

	// update the match processing
	void FK_SceneGame::update(){
		if (!abortMatch){
			/* update the timer*/
			if (isPlayingReplay() && canUpdateInput() && !isPaused()){
				//if (!isPaused()){
				readReplayData(replayTimeInformationRead);
				//}
			}
			else{
				nowReal = device->getTimer()->getRealTime();
				now = device->getTimer()->getTime();
				f32 timerSpeed = device->getTimer()->getSpeed();
				// slow game down if fps are dwindling
				u32 targetFrameSpeed = 40;
				if (cycleId > 0 && (nowReal - thenReal) > targetFrameSpeed) {
					u32 newTime = (u32)std::ceil((f32)then + (f32)targetFrameSpeed * timerSpeed);
					//std::cout << then << " "<< now << " " << newTime << std::endl;
					device->getTimer()->setTime(newTime);
					now = newTime;
				}
			}
			f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
			u32 frameDeltaMsReal = nowReal - thenReal;
			frameDeltaTime = std::fmax(std::fmin(frameDeltaTime, 0.5), 0.0);
			then = now;
			thenReal = nowReal;
			// stop updates until the driver has drawn the screen for the first time
			if (cycleId == 0){
				frameDeltaTime = 0;
				startingTime = device->getTimer()->getTime();
				// play BGM at the first cycle
				stage_bgm.play();
				cycleId = 1;
				nowReal = device->getTimer()->getRealTime();
				thenReal = device->getTimer()->getRealTime();
				now = device->getTimer()->getTime();
				then = now = device->getTimer()->getTime();
				// set random seend
				/* reset random number generator */
				u32 seed = time(0);
				srand(seed);
				if (additionalOptions.playReplay) {
					openReplayFile(seed);
					srand(seed);
				}else if (additionalOptions.saveReplay) {
					initializeReplayStorage(seed);
				}
				/* extract first random to improve quality later (first random is notoriously similar for nearby system times) */
				rand();
			}
			else {
				if (gameOptions->getDynamicResolutionScalingFlag()) {
					updateRenderTargetQuality(frameDeltaMsReal);
				}
			}
			
			// automatically pause game if the game is inactive
			if (canPauseGame() && !isPaused() && (!device->isWindowActive() || device->isWindowMinimized())) {
				pauseGame();
				if (player1isAI && !player2isAI) {
					pausePlayer2 = true;
				}
				else {
					pausePlayer1 = true;
				}
				return;
			}

			/* update scene skip */
			updateInputForPauseMenuAndSkipOptions();

			/* update additional scene logic (overridable)*/
			updateAdditionalSceneLogic((u32)(1000 * frameDeltaTime));

			/* update game logic state */
			updateGameStates((u32)(1000 * frameDeltaTime));
			
			/* update input*/
			if (isPlayingReplay()){
				if (!isPaused() && replayTimeInformationRead){
					updateInputReplay();
				}
			}
			else{
				if (saveReplay && canUpdateInput() && !isPaused()){
					storeReplayData();
				}
				if (canUpdateInput() && !isPaused()){
					updateInput(frameDeltaMsReal);
				}
			}

			/* update characters */
			updateCharacters(frameDeltaTime);
			/* update bullets */
			updateBullets();
			/* update lights */
			updateLights();
			/* update stage effects */
			updateStage((u32)(1000 * frameDeltaTime));
			/* update props */
			updateProps(frameDeltaTime);

			/* update round timer*/
			if (canUpdateRoundTimer()){
				updateRoundTimer(u32(1000 * frameDeltaTime));
			}

			// if a throw is not being processed...
			if (isProcessingThrow()){
				updateThrowAttempt((u32)(1000 * frameDeltaTime));
			}else{
				player1->setLastDamagingMoveId(0);
				player2->setLastDamagingMoveId(0);
				/* update bullet collisions */
				updateBulletCollisions();
				/* update reactions and collisions*/
				updateCollisions();
			}
			/* update camera position*/
			updateCamera(frameDeltaTime);

			/* draw */
			draw(frameDeltaTime);

			/* update bounding box after having drawn the scene */
			updateCharacterArenaInteraction();
			// synchronize animated object for character mesh after the first draw call of new round
			if (needMeshSynchronization){
				// synchronize objects
				player1->synchronizeAnimatedObjects();
				player2->synchronizeAnimatedObjects();
				needMeshSynchronization = false;
			}
		}
	};

	/* dispose additional variables */
	void FK_SceneGame::disposeAdditionalVariables(){
		// this method intentionally left blank
	}

	/* dispose scene */
	void FK_SceneGame::dispose(){
		// still have to find a good idea on how to manage this.
		//smgr->drop();
		stage_bgm.stop();
		deleteAI();
		brokenObjectCollisionCallbacks.clear();
		disposeAdditionalVariables();
		delete cameraManager;
		delete player1;
		delete player2;
		delete stage;
		delete player2CollisionCallback;
		delete player1CollisionCallback;
		delete hudManager;
		delete announcer;
		delete battleSoundManager;
		delete pauseMenu;
		delete moveListPlayer1;
		delete moveListPlayer2;
		delete pauseMenuTutorial;
		delete scenePixelShaderParameters;
		// delete all the props
		brokenObjectsProps.clear();
		for each(auto callback in scenePixelShaderCallback) {
			delete callback.second;
		}
		for each(auto callback in stagePixelShaderCallback) {
			delete callback;
		}
		FK_SceneWithInput::dispose();
	};
	/* create scene identifier for later use */
	FK_Scene::FK_SceneType FK_SceneGame::nameId(){
		return sceneType;
	}

	//! check if scene object are actively being loaded
	bool FK_SceneGame::isLoading(){
		//return(!(player1->hasBeenLoaded() && player2->hasBeenLoaded() && stage->hasBeenLoaded()));
		//return (!(stageLoaded && charactersLoaded && announcerLoaded && soundsLoaded));
		return !(allLoaded.load());
	}

	//! check if reaction is classified as strong (for sound and other purposes)
	bool FK_SceneGame::isStrongReaction(FK_Reaction_Type reactionToTest){
		if (reactionToTest == FK_Reaction_Type::StrongFlight ||
			reactionToTest == FK_Reaction_Type::ReverseStrongFlight ||
			reactionToTest == FK_Reaction_Type::StandingFlight ||
			reactionToTest == FK_Reaction_Type::StrongHigh ||
			reactionToTest == FK_Reaction_Type::StrongMedium ||
			reactionToTest == FK_Reaction_Type::StrongLow ||
			reactionToTest == FK_Reaction_Type::SmackdownLanding){
			return true;
		}
		return false;
	}

	//! get character name for loading purposes
	std::string FK_SceneGame::getCharacterName(std::string characterPath){
		std::string keyIdentifier = "#NAME";
		std::string filename = characterPath + "character.txt";
		std::string nameToReturn = "";
		/* open file and start parsing it */
		std::ifstream inputFile(filename.c_str());
		while (!inputFile.eof()){
			std::string temp;
			inputFile >> temp;
			/* check if temp matches one of the keys*/
			/* name */
			if (strcmp(temp.c_str(), keyIdentifier.c_str()) == 0){
				inputFile >> temp;
				nameToReturn = temp;
			}
		}
		return nameToReturn;
	};

	//! create render target textures
	void FK_SceneGame::createRenderTargetTextures() {
		if (!renderTargetTextures.empty()) {
			return;
		}
		renderTargetTexturesScaleFactors.clear();
		renderTargetTextures.clear();
		renderTargetIndex = -1;
		renderQualityBadFramesCounter = 0;
		renderQualityGoodFramesCounter = 0;
		core::dimension2d<u32> baseResolution = gameOptions->getResolution();
		core::dimension2d<u32> textureResolution = baseResolution;
		if (gameOptions->getBorderlessWindowMode()) {
			baseResolution.Width = (u32)ceil(baseResolution.Width * gameOptions->getSamplingFactor());
			baseResolution.Height = (u32)ceil(baseResolution.Height * gameOptions->getSamplingFactor());
		}
		// now, create textures with half size for reducing the graphic load when needed
		f32 scaleFactors[4] = { 1.5f, 2.f, 2.5f, 3.f};
		u32 numberOfRenderTextures = 4;
		f32 scaleFactorX = 1.f;
		f32 scaleFactorY = 1.f;
		for (u32 i = 0; i < numberOfRenderTextures; ++i) {
			textureResolution.Width = (u32)std::floor((f32)baseResolution.Width / scaleFactors[i]);
			textureResolution.Height = (u32)std::floor((f32)baseResolution.Height / scaleFactors[i]);
			scaleFactorX = (f32)baseResolution.Width / (f32)textureResolution.Width;
			scaleFactorY = (f32)baseResolution.Height / (f32)textureResolution.Height;
			renderTargetTexturesScaleFactors.push_back(core::vector2df(scaleFactorX, scaleFactorY));
			std::string renderTexName = "RenderTargetTex" + std::to_string(i);
			video::ITexture* tempTex = driver->addRenderTargetTexture(textureResolution, renderTexName.c_str());
			renderTargetTextures.push_back(tempTex);
		}
		currentRenderTargetResolution = baseResolution;
		oldRenderTargetResolution = baseResolution;
	}
	//! decrease render quality
	void FK_SceneGame::increaseRenderTextureQuality() {
		if (renderTargetIndex >= 0) {
			renderTargetIndex -= 1;
		}
		renderQualityGoodFramesCounter = 0;
		renderQualityBadFramesCounter = 0;
		if (getRenderTarget() != NULL) {
			currentRenderTargetResolution = getRenderTarget()->getSize();
		}else{
			currentRenderTargetResolution = gameOptions->getResolution();
		}
		if (currentRenderTargetResolution != oldRenderTargetResolution) {
			lightManager->setScreenRenderTargetResolution(currentRenderTargetResolution);
			oldRenderTargetResolution = currentRenderTargetResolution;
		}
	}

	//! increase render quality
	void FK_SceneGame::decreaseRenderTextureQuality() {
		renderTargetIndex += 1;
		if (renderTargetIndex > 0 && (u32)renderTargetIndex >= renderTargetTextures.size()) {
			renderTargetIndex = renderTargetTextures.size() - 1;
		}
		renderQualityGoodFramesCounter = 0;
		renderQualityBadFramesCounter = 0;
		if (getRenderTarget() != NULL) {
			currentRenderTargetResolution = getRenderTarget()->getSize();
		}
		else {
			currentRenderTargetResolution = gameOptions->getResolution();
		}
		if (currentRenderTargetResolution != oldRenderTargetResolution) {
			lightManager->setScreenRenderTargetResolution(currentRenderTargetResolution);
			oldRenderTargetResolution = currentRenderTargetResolution;
		}
	}

	//! create multi resolution render targets
	video::ITexture* FK_SceneGame::getRenderTarget() {
		if (renderTargetIndex < 0) {
			if (gameOptions->getBorderlessWindowMode()) {
				return borderlessWindowRenderTarget;
			}
			return NULL;
		}
		return renderTargetTextures[(u32)renderTargetIndex];
	}

	//! get render target
	s32 FK_SceneGame::getRenderTargetQuality() {
		return renderTargetIndex;
	}

	core::vector2df FK_SceneGame::getRenderTargetScaleFactor() {
		if (renderTargetIndex >= 0) {
			return renderTargetTexturesScaleFactors[renderTargetIndex];
		}
		else {
			return core::vector2df(1.f, 1.f);
		}
	}

	//! update render quality
	void FK_SceneGame::updateRenderTargetQuality(u32 frameDeltaTimeMs) {
		//! target is 60fps sharp, so we reduce the drawing size if this is not maintained, after 5 consecutive bad frames
		if (frameDeltaTimeMs > 20) {
			renderQualityBadFramesCounter += 1;
			renderQualityGoodFramesCounter = 0;
		}
		else {
			renderQualityBadFramesCounter = 0;
			renderQualityGoodFramesCounter += 1;
		}
		if (renderQualityBadFramesCounter >= 15) {
			decreaseRenderTextureQuality();
		}
		else if (renderQualityGoodFramesCounter >= 15) {
			increaseRenderTextureQuality();
		}
	}

	//! load loading textures
	void FK_SceneGame::loadLoadingTextures(){
		std::string previewPictureFilename = stagesPath + arena_configPath + "preview.jpg";
		stageTex = driver->getTexture(previewPictureFilename.c_str());
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		textFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		titleFont = device->getGUIEnvironment()->getFont(fk_constants::FK_BattleFontIdentifier.c_str());
		player1NameLoadingScreen = getCharacterName(charactersPath + player1_configPath);
		player2NameLoadingScreen = getCharacterName(charactersPath + player2_configPath);
		if (!player1Outfit.outfitCharacterName.empty()){
			player1NameLoadingScreen = player1Outfit.outfitCharacterName;
		}
		if (!player2Outfit.outfitCharacterName.empty()){
			player2NameLoadingScreen = player2Outfit.outfitCharacterName;
		}
		player1Preview = driver->getTexture((charactersPath + player1_configPath + 
			player1Outfit.outfitDirectory + "fightPreview.png").c_str());
		if (!player1Preview){
			player1Preview = driver->getTexture((charactersPath + player1_configPath + "fightPreview.png").c_str());
		}
		player2Preview = driver->getTexture((charactersPath + player2_configPath +
			player2Outfit.outfitDirectory + "fightPreview.png").c_str());
		if (!player2Preview){
			player2Preview = driver->getTexture((charactersPath + player2_configPath + "fightPreview.png").c_str());
		}
		// load story from stage file

	}
	// ! draw player names for loading screen
	void FK_SceneGame::drawPlayerNamesForLoadingScreen(){
		// convert names to wide strings format to write them (irrlicht accepts only wide strings here)
		std::wstring player1Name = fk_addons::convertNameToNonASCIIWstring(player1NameLoadingScreen);
		std::wstring player2Name = fk_addons::convertNameToNonASCIIWstring(player2NameLoadingScreen);
		std::replace(player1Name.begin(), player1Name.end(), L'_', L' ');
		std::replace(player2Name.begin(), player2Name.end(), L'_', L' ');
		//std::transform(player1Name.begin(), player1Name.end(), player1Name.begin(), toupper);
		//std::transform(player2Name.begin(), player2Name.end(), player2Name.begin(), toupper);
		std::wstring pl1Name = std::wstring(player1Name.begin(), player1Name.end());
		std::wstring pl2Name = std::wstring(player2Name.begin(), player2Name.end());
		/* get width and height of the string*/
		s32 sentenceWidth = font->getDimension(pl1Name.c_str()).Width;
		s32 sentenceHeight = font->getDimension(pl1Name.c_str()).Height;
		s32 sentenceOffsetX = 80; // distance from border
		s32 sentenceOffsetYPl1 = 60; // distance from the top of the screen (PL1)
		s32 sentenceOffsetYPl2 = 220; // distance from the top of the screen (PL2)
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		f32 scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		f32 scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		// draw player 1 name near the top
		core::rect<s32> destinationRect = core::rect<s32>(sentenceOffsetX * scale_factorX,
			sentenceOffsetYPl1 * scale_factorY,
			sentenceOffsetX * scale_factorX + sentenceWidth,
			sentenceHeight + sentenceOffsetYPl1 * scale_factorY);
		fk_addons::drawBorderedText(font, pl1Name, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		// draw player 2 name near the bottom
		sentenceWidth = font->getDimension(pl2Name.c_str()).Width;
		sentenceHeight = font->getDimension(pl2Name.c_str()).Height;
		destinationRect = core::rect<s32>(screenSize.Width - sentenceWidth - sentenceOffsetX * scale_factorX,
			sentenceOffsetYPl2 * scale_factorY,
			screenSize.Width - sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetYPl2 * scale_factorY);
		fk_addons::drawBorderedText(font, pl2Name, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		// draw VS at the center
		std::wstring vsString = L"VS";
		sentenceWidth = titleFont->getDimension(vsString.c_str()).Width;
		sentenceHeight = titleFont->getDimension(vsString.c_str()).Height;
		destinationRect = core::rect<s32>(screenSize.Width / 2 - sentenceWidth / 2,
			(sentenceOffsetYPl2 + sentenceOffsetYPl1) * scale_factorY / 2 - sentenceHeight / 2,
			screenSize.Width / 2 + sentenceWidth / 2,
			(sentenceOffsetYPl2 + sentenceOffsetYPl1) * scale_factorY / 2 + sentenceHeight / 2);
		fk_addons::drawBorderedText(titleFont, vsString, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		// draw player miniatures
		core::dimension2d<u32> miniatureSize = player1Preview->getSize();
		sentenceHeight = font->getDimension(pl1Name.c_str()).Height;
		s32 miniatureOffsetX = 80;
		s32 miniatureStartY = (sentenceOffsetYPl2 + sentenceOffsetYPl1 - miniatureSize.Height) * scale_factorY / 2 + sentenceHeight / 2;
		s32 miniatureEndY = (sentenceOffsetYPl2 + sentenceOffsetYPl1 + miniatureSize.Height) * scale_factorY / 2 + sentenceHeight / 2;
		s32 miniatureStartX_Pl1 = miniatureOffsetX * scale_factorX;
		s32 miniatureEndX_Pl1 = miniatureStartX_Pl1 + miniatureSize.Width * scale_factorX;
		s32 miniatureEndX_Pl2 = screenSize.Width - miniatureOffsetX * scale_factorX;
		s32 miniatureStartX_Pl2 = miniatureEndX_Pl2 - miniatureSize.Width * scale_factorX;
		driver->draw2DImage(player1Preview,
			core::rect<s32>(miniatureStartX_Pl1, miniatureStartY,
			miniatureEndX_Pl1,
			miniatureEndY),
			core::rect<s32>(0, 0, miniatureSize.Width, miniatureSize.Height));
		driver->draw2DImage(player2Preview,
			core::rect<s32>(miniatureStartX_Pl2, miniatureStartY,
			miniatureEndX_Pl2,
			miniatureEndY),
			core::rect<s32>(0, 0, miniatureSize.Width, miniatureSize.Height));
		
	}

	void FK_SceneGame::drawLoadingScreenProgressBar(f32 completionPercentage){
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		core::dimension2d<u32> barSize(screenSize.Width * 0.7, screenSize.Height / 36);
		s32 x = (screenSize.Width - barSize.Width) / 2;
		s32 y = 0;
		s32 offsetY = screenSize.Height / 64;
		y = screenSize.Height - offsetY - barSize.Height;
		core::rect<s32> destinationRect = core::rect<s32>(x, y,
			x + barSize.Width,
			y + barSize.Height);
		core::rect<s32> destinationRectIn = destinationRect;
		destinationRectIn.UpperLeftCorner = destinationRectIn.UpperLeftCorner + core::vector2d<s32>(1, 1);
		destinationRectIn.LowerRightCorner = destinationRectIn.LowerRightCorner - core::vector2d<s32>(1, 1);
		core::rect<s32> destinationRectOut = destinationRect;
		destinationRectOut.UpperLeftCorner = destinationRectOut.UpperLeftCorner - core::vector2d<s32>(1, 1);
		destinationRectOut.LowerRightCorner = destinationRectOut.LowerRightCorner + core::vector2d<s32>(1, 1);
		barSize.Width = completionPercentage / 100.0 * barSize.Width;
		core::rect<s32> barRect = core::rect<s32>(x, y,
			x + barSize.Width,
			y + barSize.Height);
		video::SColor const rectColor1(255, 0, 147, 255);
		video::SColor const rectColor2(255, 0, 127, 127);
		driver->draw2DRectangle(barRect, rectColor1, rectColor2, rectColor1, rectColor2);
		driver->draw2DRectangleOutline(destinationRect);
		driver->draw2DRectangleOutline(destinationRectIn, video::SColor(128, 128, 128, 128));
		driver->draw2DRectangleOutline(destinationRectOut, video::SColor(255, 0, 0, 0));
		std::wstring textToDraw = L"Schwarzerblitz";
		core::dimension2d<u32> textSize = font->getDimension(textToDraw.c_str());
		s32 x1 = barRect.UpperLeftCorner.X;
		s32 y2 = barRect.UpperLeftCorner.Y;
		s32 y1 = y2 - textSize.Height;
		s32 x2 = x1 + textSize.Width;
		core::rect<s32> textRect(x1, y1, x2, y2);
		fk_addons::drawBorderedText(font, textToDraw, textRect);
		// draw match number for arcade / mode name for any other
		textToDraw = getSceneNameForLoadingScreen();
		textSize = font->getDimension(textToDraw.c_str());
		x1 = screenSize.Width / 2 - textSize.Width / 2;
		y1 = screenSize.Height / 32;
		x2 = x1 + textSize.Width;
		y2 = y1 + textSize.Height;
		textRect = core::rect<s32>(x1, y1, x2, y2);
		fk_addons::drawBorderedText(font, textToDraw, textRect);
	}

	//! draw loading screen
	void FK_SceneGame::drawLoadingScreen(f32 completionPercentage){
		// get arena background for loading screen
		core::dimension2d<u32> texSize = stageTex->getSize();
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
		// draw preview (fullscreen)
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// create color array for background
		video::SColor const color_bck = video::SColor(255, 75, 75, 75);
		video::SColor const vertexColors_bck[4] = {
			color_bck,
			color_bck,
			color_bck,
			color_bck
		};
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		// draw stage picture
		driver->draw2DImage(stageTex, core::rect<s32>(0,0,screenSize.Width, screenSize.Height), sourceRect, 0, vertexColors_bck);
		// draw overlay
		drawLoadingScreenProgressBar(completionPercentage);
		// draw player names & miniatures
		drawPlayerNamesForLoadingScreen();
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}

	/* check if move lists are being shown */
	bool FK_SceneGame::isShowingMoveList(){
		return showMoveListPlayer1 || showMoveListPlayer2;
	};

	/* check if game is paused*/
	bool FK_SceneGame::isPaused(){
		return pause;
	};
	/* check if game can be paused*/
	bool FK_SceneGame::canPauseGame(){
		return !(newRoundFlag ||
			processingEndOfRound ||
			processingRoundText || 
			processingFightText ||
			processingWinText || 
			processingKOText || 
			processingContinue ||
			processingGameOver ||
			processingSceneEnd ||
			isProcessingIntro());
	}
	/* pause game*/
	void FK_SceneGame::pauseGame(){
		pause = true;
		device->getTimer()->setSpeed(0.0f);
		stage_bgm.setVolume(stage->getBGMPauseVolume() * gameOptions->getMusicVolume());
		activatePauseMenu();
	};

	/* pause game*/
	void FK_SceneGame::activatePauseMenu() {
		pauseMenu->reset();
		pauseMenu->setVisible(true);
		pauseMenu->setActive(true);
	};

	/* resume game*/
	void FK_SceneGame::resumeGame() {
		pausePlayer1 = false;
		pausePlayer2 = false;
		pause = false;
		f32 timerSpeed = databaseAccessor.getNormalTimerVelocity();
		if (isTriggerComboActive()) {
			timerSpeed = databaseAccessor.getSlowmoTimerVelocity();
		}
		device->getTimer()->setSpeed(timerSpeed);
		stage_bgm.setVolume(stage->getBGMVolume()* gameOptions->getMusicVolume());
		pauseMenu->setVisible(false);
		pauseMenu->setActive(false);
	};

	// update tutorial
	void FK_SceneGame::updateTutorial(u32 inputButtons) {
		u32 menuButtons = inputButtons & FK_Input_Buttons::Any_MenuButton;
		u32 directionButtons = inputButtons & FK_Input_Buttons::Any_Direction;
		u32 timeThresholdForNewInputMS = 200;
		if (device->getTimer()->getRealTime() - lastTimeInputTutorial < timeThresholdForNewInputMS) {
			return;
		}
		if ((inputButtons & FK_Input_Buttons::Cancel_Button) != 0) {
			battleSoundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			showTutorial = false;
			inputReceiver->reset();
			resetInputTimeForPauseMenu();
			lastTimeInputTutorial = device->getTimer()->getRealTime();
			return;
		}
		if ((directionButtons == FK_Input_Buttons::Right_Direction) || (menuButtons == FK_Input_Buttons::ScrollRight_Button)) {
			if (pauseMenuTutorial->isLastPage()) {
				battleSoundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				lastTimeInputTutorial = device->getTimer()->getRealTime();
			}
			else {
				battleSoundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				pauseMenuTutorial->goToNextPage();
				lastTimeInputTutorial = device->getTimer()->getRealTime();
			}
			//inputReceiver->reset();
			return;
		}
		if ((directionButtons == FK_Input_Buttons::Left_Direction) || (menuButtons == FK_Input_Buttons::ScrollLeft_Button)) {
			if (pauseMenuTutorial->isFirstPage()) {
				battleSoundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				lastTimeInputTutorial = device->getTimer()->getRealTime();
			}
			else {
				battleSoundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				pauseMenuTutorial->goToPreviousPage();
				lastTimeInputTutorial = device->getTimer()->getRealTime();
			}
			//inputReceiver->reset();
			return;
		}
	}

	// draw pause menu
	void FK_SceneGame::drawPauseMenu() {
		pauseMenu->draw();
		if (showTutorial) {
			pauseMenuTutorial->draw();
		}
	}

	// show move list
	void FK_SceneGame::showMoveList(bool player1Movelist) {
		if (player1Movelist) {
			showMoveListPlayer2 = false;
			moveListPlayer2->setActive(false);
			moveListPlayer2->setVisible(false);
			showMoveListPlayer1 = true;
			moveListPlayer1->setActive(true);
			moveListPlayer1->setVisible(true);
			moveListPlayer1->resetSelectionTime();
		}
		else {
			showMoveListPlayer1 = false;
			moveListPlayer1->setActive(false);
			moveListPlayer1->setVisible(false);
			showMoveListPlayer2 = true;
			moveListPlayer2->setActive(true);
			moveListPlayer2->setVisible(true);
			moveListPlayer1->resetSelectionTime();
		}
	}

	// update pause menu
	void FK_SceneGame::updatePauseMenu() {
		u32 inputButtonsForPauseMenu = player1input->getPressedButtons();
		if (!pausePlayer1 && !pausePlayer2) {
			inputButtonsForPauseMenu |= player2input->getPressedButtons();
		}
		else if (pausePlayer2) {
			inputButtonsForPauseMenu = player2input->getPressedButtons();
		}

		if (showTutorial) {
			updateTutorial(inputButtonsForPauseMenu);
		}
		else {
			if (pauseMenu->canAcceptInput() && (inputButtonsForPauseMenu & FK_Input_Buttons::Any_MenuButton) & FK_Input_Buttons::Cancel_Button){
				battleSoundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				Sleep(500);
				resumeGame();
				return;
			}
			pauseMenu->update(inputButtonsForPauseMenu);
			f32 timerSpeed = databaseAccessor.getNormalTimerVelocity();
			if (pauseMenu->itemIsSelected()){
				s32 pauseIndex = pauseMenu->getIndex();
				pauseMenu->resetSelection();
				switch (pauseIndex){
				case 0:
					Sleep(500);
					resumeGame();
					break;
				case 1:
					if (pausePlayer1){
						showMoveList(true);
					}
					else if (pausePlayer2){
						showMoveList(false);
					}
					else {
						battleSoundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					}
					break;
				case 2:
					showTutorial = true;
					pauseMenuTutorial->goToBeginning();
					Sleep(500);
					break;
				case 3:
					activateFreeCameraMode();
					Sleep(500);
					break;
				case 4:
					abortMatch = true;
					setNextScene(getPreviousScene());
					device->getTimer()->setSpeed(timerSpeed);
					winnerId = 0;
					//FK_SceneGame::updateSaveFileData();
					Sleep(500);
					break;
				case 5:
					abortMatch = true;
					winnerId = 0;
					//FK_SceneGame::updateSaveFileData();
					setNextScene(FK_SceneType::SceneMainMenu);
					device->getTimer()->setSpeed(timerSpeed);
					Sleep(500);
					break;
				default:
					break;
				}
			}
		}
	}

	/* input time for pause menu */
	void FK_SceneGame::resetInputTimeForPauseMenu() {
		pauseMenu->resetLastInputTime();
	}

	/* update move list show */
	void FK_SceneGame::updateMoveListPanel(){
		if (showMoveListPlayer1){
			u32 inputButtonsForMoveList = player1input->getPressedButtons();
			moveListPlayer1->update(inputButtonsForMoveList);
			if (moveListPlayer1->hasToBeClosed()){
				showMoveListPlayer1 = false;
				moveListPlayer1->setActive(false);
				moveListPlayer1->setVisible(false);
				moveListPlayer1->reset(false);
				inputReceiver->reset();
				resetInputTimeForPauseMenu();
				return;
			}
		}
		else{
			u32 inputButtonsForMoveList = player2input->getPressedButtons();
			moveListPlayer2->update(inputButtonsForMoveList);
			if (moveListPlayer2->hasToBeClosed()){
				showMoveListPlayer2 = false;
				moveListPlayer2->setActive(false);
				moveListPlayer2->setVisible(false);
				moveListPlayer2->reset(false);
				inputReceiver->reset();
				resetInputTimeForPauseMenu();
				return;
			}
		}
	}

	/* process the update of super meter gain (trigger regen) */
	void FK_SceneGame::updateTriggerRegeneration(bool player1WasHit, bool player2WasHit)
	{
		// setup trigger regeneration
		player2WasHit &= !player2->isKO();
		player1WasHit &= !player1->isKO();

		u32 triggerRegenPlayer1Ms = databaseAccessor.getTriggerRegenTimeMs();
		u32 triggerRegenPlayer2Ms = triggerRegenPlayer1Ms;
		if (triggerRegenPlayer1Ms < 0 || additionalOptions.triggerRegenerationTimeMSPlayer1 > 0) {
			triggerRegenPlayer1Ms = additionalOptions.triggerRegenerationTimeMSPlayer1;
		}
		if (triggerRegenPlayer2Ms < 0 || additionalOptions.triggerRegenerationTimeMSPlayer2 > 0) {
			triggerRegenPlayer2Ms = additionalOptions.triggerRegenerationTimeMSPlayer1;
		}

		if (databaseAccessor.triggerRegenAllowed() && (player1WasHit || player2WasHit)) {
			s32 amountOnHit = databaseAccessor.getTriggerRegenTimeOnHit();
			s32 amountOnBlock = databaseAccessor.getTriggerRegenTimeOnBlock();
			if (player1WasHit) {
				if (!(player2->isTriggerModeActive() && !databaseAccessor.allowTriggerGainInTriggerMode())) {
					triggerRegenCounterMSPlayer2 +=
						player1->isGuarding() ? amountOnHit / 2 : amountOnHit;
				}
				if (player1->isGuarding()) {
					triggerRegenCounterMSPlayer1 += amountOnBlock;
				}
				else {
					triggerRegenCounterMSPlayer1 += amountOnBlock / 2;
				}
			}
			if (player2WasHit) {
				if (!(player1->isTriggerModeActive() && !databaseAccessor.allowTriggerGainInTriggerMode())) {
					triggerRegenCounterMSPlayer1 +=
						player1->isGuarding() ? amountOnHit / 2 : amountOnHit;
				}
				if (player1->isGuarding()) {
					triggerRegenCounterMSPlayer2 += amountOnBlock;
				}
				else {
					triggerRegenCounterMSPlayer2 += amountOnBlock / 2;
				}
			}
		}
		if (triggerRegenCounterMSPlayer2 >= triggerRegenPlayer2Ms) {
			triggerRegenCounterMSPlayer2 = 0;
			player2->addTriggerCounters(1);
		}
		if (triggerRegenCounterMSPlayer1 >= triggerRegenPlayer1Ms) {
			triggerRegenCounterMSPlayer1 = 0;
			player1->addTriggerCounters(1);
		}
	}

	/* setup video driver, gui driver and scene manager */
	void FK_SceneGame::setupIrrlichtDevice(){
		FK_SceneWithInput::setupIrrlichtDevice();
	}

	/* a boolean which returns if sepia effect has to be shown */
	f32* FK_SceneGame::getSceneParametersForShader(FK_SceneShaderTypes newShader){
		if (newShader == FK_SceneShaderTypes::ShaderSepia){
			bool flag =/* isPaused() || */isTriggerComboActive();
			if (flag){
				scenePixelShaderParameters[0] = 1.0f;
			}
			else{
				scenePixelShaderParameters[0] = 0.0f;
			}
			scenePixelShaderParameters[1] = 0.f;
			scenePixelShaderParameters[2] = 0.f;
			scenePixelShaderParameters[3] = 0.f;
		}
		else if (newShader == FK_SceneShaderTypes::ShaderBloom){
			scenePixelShaderParameters[0] = stage->getParametersForShader().bloomValue;
			scenePixelShaderParameters[1] = stage->getParametersForShader().sceneValue;
			scenePixelShaderParameters[2] = 0.f;
			scenePixelShaderParameters[3] = 0.f;
		}
		else if (newShader == FK_SceneShaderTypes::ShaderBrightPass){
			scenePixelShaderParameters[0] = stage->getParametersForShader().brightnessCutHigh;
			scenePixelShaderParameters[1] = stage->getParametersForShader().brightnessCutLow;
			scenePixelShaderParameters[2] = 0.f;
			scenePixelShaderParameters[3] = 0.f;
		}
		else if (newShader == FK_SceneShaderTypes::ShaderRipple){
			scenePixelShaderParameters[0] = 0.0f;
			scenePixelShaderParameters[1] = 0.5f;
			scenePixelShaderParameters[2] = 0.5f;
			scenePixelShaderParameters[3] = processingTimeCounterMs / 1000.0f;
			if (processingKOText && processingTimeCounterMs < (u32)500){
				scenePixelShaderParameters[0] = 1.0f;
			}
		}
		else if (newShader == FK_SceneShaderTypes::ShaderZoomBlur){
			scenePixelShaderParameters[0] = 0.0f;
			scenePixelShaderParameters[1] = -0.2f;
			scenePixelShaderParameters[2] = 0.75f;
			scenePixelShaderParameters[3] = processingTimeCounterMs / 1000.0f;
			if (isUpdatingImpactEffect()){
				scenePixelShaderParameters[3] = (f32)impactEffectCounter / (2*(f32)FK_SceneGame::impactEffectDuration);
			}
			if (isUpdatingImpactEffect() || (processingKOText && processingTimeCounterMs < (u32)750)){
				scenePixelShaderParameters[0] = 1.0f;
			}
			else{
				scenePixelShaderParameters[0] = 0.0f;
			}
		}
		return scenePixelShaderParameters;
	}

	void FK_SceneGame::setupGameObjects(){
		f32 oldLoadPercentage = 0.0f;
		drawLoadingScreen(0.f);
		setupArena();
		oldLoadPercentage += 10.f;
		drawLoadingScreen(oldLoadPercentage);

		setupBattleSoundManager();
		oldLoadPercentage += 2.5f;
		drawLoadingScreen(oldLoadPercentage);

		setupCharacters(oldLoadPercentage);
		drawLoadingScreen(oldLoadPercentage);

		addCharacterSoundsToSoundManager();
		oldLoadPercentage += 2.5f;
		drawLoadingScreen(oldLoadPercentage);

		setupAnnouncer();
		oldLoadPercentage += 5.f;
		drawLoadingScreen(oldLoadPercentage);

		setupHUD();
		oldLoadPercentage += 5.f;
		drawLoadingScreen(oldLoadPercentage);

		setupCharacterArenaInteraction();
		setupCamera();
		oldLoadPercentage += 5.f;
		drawLoadingScreen(oldLoadPercentage);

		setupGameStates();
		allLoaded.store(true);
		std::cout << "All resources have been loaded succesfully " << std::endl;
		std::cout << "Loading flag = " << isLoading() << std::endl;
		//m_thread_cv.notify_one();
		// setup additional parameters after all resources are loaded
		setupAdditionalParametersAfterLoading();
		setupAI();
		oldLoadPercentage += 5.f;
		drawLoadingScreen(oldLoadPercentage);
		setupMoveLists(oldLoadPercentage);
	}

	/* get pause menu items */
	std::vector<std::string> FK_SceneGame::getPauseMenuItems(){
		std::vector<std::string> menuItems = {
			"Resume match",
			"Show move list",
			"Show game guide",
			"Free camera mode",
			"To character selection",
			"To main menu"
		};
		return menuItems;
	}

	/* setup pause menu */
	void FK_SceneGame::setupPauseMenu(){
		std::vector<std::string> menuItems = getPauseMenuItems();
		pauseMenu = new FK_InGameMenu(device, battleSoundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		if (gameOptions->getBorderlessWindowMode()){
			pauseMenu->setViewport(core::dimension2di(-1,-1), borderlessWindowRenderTarget->getSize());
		}
	}

	/* setup tutorial */
	void FK_SceneGame::setupTutorial(){
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		showTutorial = false;
		lastTimeInputTutorial = 0;
		// setup tutorial
		pauseMenuTutorial = new FK_Tutorial(driver, mediaPath + "tutorial\\", screenSize, font);
	}

	// set movelist icon set
	void FK_SceneGame::setupMoveListsIconSetsBasedOnInput() {
		if (moveListPlayer1 == NULL || moveListPlayer2 == NULL) {
			return;
		}
		bool useJoypad = false;
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		if (joystickInfo.size() > 0) {
			useJoypad = true;
			mapId = 1;
			currentJoypadName = std::string(joystickInfo[0].Name.c_str());
		}
		else {
			useJoypad = false;
			if (player2isAI) {
				mapId = 0;
			}
			else {
				mapId = 1;
			}
		}
		moveListPlayer1->setIconsBasedOnCurrentInputMap(inputMapper,
			mapId, useJoypad,
			currentJoypadName
		);
		useJoypad = false;
		mapId = -1;
		currentJoypadName = std::string();
		if (joystickInfo.size() > 1) {
			useJoypad = true;
			mapId = 2;
			currentJoypadName = std::string(joystickInfo[1].Name.c_str());
		}
		else {
			useJoypad = false;
			if (player1isAI || joystickInfo.size() > 0) {
				mapId = 0;
			}
			else {
				mapId = 2;
			}
		}
		moveListPlayer2->setIconsBasedOnCurrentInputMap(inputMapper,
			mapId, useJoypad,
			currentJoypadName
		);
	}

	/* setup move list */
	void FK_SceneGame::setupMoveLists(f32 &progress){
		std::string movelistResourcePath = mediaPath + "movelist\\";
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		moveListPlayer1 = new FK_MoveListPanel(device, battleSoundManager, gameOptions, screenSize,
			player1, movelistResourcePath, "windowskin.png");
		progress += 5.0f;
		drawLoadingScreen(progress);
		moveListPlayer2 = new FK_MoveListPanel(device, battleSoundManager, gameOptions, screenSize,
			player2, movelistResourcePath, "windowskin.png");
		progress += 5.0f;
		drawLoadingScreen(progress);
		setupMoveListsIconSetsBasedOnInput();
	}

	/* setup hud manager*/
	void FK_SceneGame::setupHUD(){
		hudManager = new FK_HUDManager_font(device, databaseAccessor, mediaPath, player1, player2);
		if (gameOptions->getBorderlessWindowMode()){
			hudManager->setViewportSize(borderlessWindowRenderTarget->getSize());
		}
		//m_threadMutex.try_lock();
		//Sleep(100);
		hudLoaded = true;
		//m_threadMutex.unlock();

		// setup HUD input maps for training mode
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		bool player1UsesJoypad = false;
		if (joystickInfo.size() > 0) {
			player1UsesJoypad = true;
			mapId = 1;
			currentJoypadName = std::string(joystickInfo[0].Name.c_str());
		}
		else {
			player1UsesJoypad = false;
			mapId = 0;
		}

		std::map<FK_Input_Buttons, u32> buttonInputLayoutPlayer1;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayoutPlayer1;
		setIconsBasedOnCurrentInputMap(buttonInputLayoutPlayer1, keyboardKeyLayoutPlayer1,
			mapId, player1UsesJoypad, currentJoypadName
		);

		hudManager->setInputButtonTextures(buttonInputLayoutPlayer1, keyboardKeyLayoutPlayer1, player1UsesJoypad);

		std::cout << "HUD loaded" << std::endl;
	}

	/* setup announcer*/
	void FK_SceneGame::setupAnnouncer(){
		// reset "start voice" boolean
		canPlayAnnouncerVoice = false;
		// create the announcer object
		announcer = new FK_Announcer(voiceClipsPath);
		// fill the announcer object
		announcer->addSoundFromDefaultPath("fight", "fight.wav");
		announcer->addSoundFromDefaultPath("ko", "ko.wav");
		announcer->addSoundFromDefaultPath("perfect", "perfect.wav");
		announcer->addSoundFromDefaultPath("draw", "draw.wav");
		announcer->addSoundFromDefaultPath("double_ko", "double_ko.wav");
		announcer->addSoundFromDefaultPath("ringout", "ringout.wav");
		announcer->addSoundFromDefaultPath("game_over", "game_over.wav");
		announcer->addSoundFromDefaultPath("continue", "continue.wav");
		announcer->addSoundFromDefaultPath("new_record", "new_record.wav");
		announcer->addSoundFromDefaultPath("success", "success.wav");
		announcer->addSoundFromDefaultPath("double_ringout", "double_ringout.wav");
		//add each round from 1 to 9
		int maxNumberOfRounds = 15;
		for (int i = 1; i <= maxNumberOfRounds; ++i){
			char buffer1[30];
			char buffer2[30];
			sprintf_s(buffer1, "round_%i", i);
			sprintf_s(buffer2, "round_%i.wav", i);
			announcer->addSoundFromDefaultPath(std::string(buffer1), std::string(buffer2));//m_threadMutex.try_lock();
		};
		announcer->addSoundFromDefaultPath("round_final", "round_final.wav");
		announcer->addSoundFromDefaultPath("time_up", "time_up.wav");
		// add winning announcements for each playing character
		std::string voiceName = charactersPath + player1_configPath + player1->getOutfitPath() + 
			fk_constants::FK_VoiceoverEffectsFileFolder + "win.wav";
		std::ifstream testFile(voiceName.c_str());
		if (testFile.good()){
			testFile.clear();
			testFile.close();
			announcer->addSoundFullPath("player1_wins", voiceName.c_str());
		}
		else{
			testFile.clear();
			testFile.close();
			announcer->addSoundFullPath("player1_wins", charactersPath + player1_configPath + 
				fk_constants::FK_VoiceoverEffectsFileFolder + "win.wav");
		}

		voiceName = charactersPath + player2_configPath + player2->getOutfitPath() +
			fk_constants::FK_VoiceoverEffectsFileFolder + "win.wav";
		testFile = std::ifstream(voiceName.c_str());
		if (testFile.good()){
			testFile.clear();
			testFile.close();
			announcer->addSoundFullPath("player2_wins", voiceName.c_str());
		}
		else{
			testFile.clear();
			testFile.close();
			announcer->addSoundFullPath("player2_wins", charactersPath + player2_configPath +
				fk_constants::FK_VoiceoverEffectsFileFolder + "win.wav");
		}
		//Sleep(100);
		announcerLoaded = true;
		//m_threadMutex.unlock();
		std::cout << "Announcer loaded" << std::endl;
	}

	/* set up battle sound manager */
	void FK_SceneGame::setupBattleSoundManager(){
		battleSoundManager = new FK_SoundManager(commonResourcesPath + "sound_effects\\");
		battleSoundManager->addSoundFromDefaultPath("trigger", "trigger.ogg");
		battleSoundManager->addSoundFromDefaultPath("guard", "guard03.wav");
		battleSoundManager->addSoundFromDefaultPath("hit_weak", "hitA03.wav");
		battleSoundManager->addSoundFromDefaultPath("hit_strong", "hitF02.wav");
		battleSoundManager->addSoundFromDefaultPath("hit_counter", "counter.wav");
		battleSoundManager->addSoundFromDefaultPath("wrong", "wrong.wav");
		battleSoundManager->addSoundFromDefaultPath("collect", "collect.ogg");
		battleSoundManager->addSoundFromDefaultPath("wry", "wry.ogg");
		battleSoundManager->addSoundFromDefaultPath("swing", "swing.ogg");
		battleSoundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		battleSoundManager->addSoundFromDefaultPath("correct", "Cursor2.ogg");
		battleSoundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		battleSoundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
		battleSoundManager->addSoundFromDefaultPath("splash", "splash.ogg");
		battleSoundManager->addSoundFromDefaultPath("camera", "camera.ogg");

		if (!stage->getSplashSoundEffect().filename.empty()) {
			battleSoundManager->addSoundFullPath("ringout_splash", 
				stagesPath + arena_configPath + stage->getSplashSoundEffect().filename);
		}
		else {
			battleSoundManager->addSoundFromDefaultPath("ringout_splash", "splash.ogg");
		}
		
		for (int i = 1; i <= 9; ++i){
			battleSoundManager->addSoundFullPath("continue" + std::to_string(i), voiceClipsPath + "continue" + std::to_string(i) + ".wav");
		}
		//m_threadMutex.try_lock();
		loadingPercentage.store(loadingPercentage.load() + 5.0f);
		//Sleep(100);
		soundsLoaded = true;
		////m_threadMutex.unlock();
		std::cout << "Sound loaded" << std::endl;
	}

	/* play hit/KO sounds on attack*/
	bool FK_SceneGame::playHitSounds(FK_Reaction_Type reaction, std::string playerTag, FK_Character* character){
		if (!character->canPlayHitSound() && !character->isKO()){
			return true;
		}
		if (character->isGuarding()){
			return false;
		}
		bool hasPlayedSound = false;
		if (character->isKO()){
			hasPlayedSound |= battleSoundManager->playSound(playerTag + "ko", 100.0 * gameOptions->getVoicesVolume());
			if (hasPlayedSound) {
				character->resetSoundTimer();
			}
		}
		else if (reaction == FK_Reaction_Type::StrongFlight || 
			reaction == FK_Reaction_Type::ReverseStrongFlight ||
			reaction == FK_Reaction_Type::StandingFlight){
			bool playSound = battleSoundManager->playSound(playerTag + "flight", 100.0 * gameOptions->getVoicesVolume());
			if (!playSound){
				hasPlayedSound |= playHitSounds(FK_Reaction_Type::StrongHigh, playerTag, character);
			}
			else{
				character->resetSoundTimer();
			}
		}
		else if (isStrongReaction(reaction)){
			bool playSound = battleSoundManager->playSound(playerTag + "hit_strong", 100.0 * gameOptions->getVoicesVolume());
			if (!playSound){
				hasPlayedSound |= playHitSounds(FK_Reaction_Type::WeakHigh, playerTag, character);
			}
			else{
				character->resetSoundTimer();
			}
		}
		else{
			hasPlayedSound |= battleSoundManager->playSound(playerTag + "hit_weak", 100.0 * gameOptions->getVoicesVolume());
			if (hasPlayedSound) {
				character->resetSoundTimer();
			}
		}
		return hasPlayedSound;
	}

	/* add additional sounds to sound manager from directory */
	void FK_SceneGame::addSoundsFromDirectoryToSoundManager(std::string directory, std::string extension, std::string prefix){
		HANDLE hFind;
		WIN32_FIND_DATA data;
		std::string browseDirectory = directory + "*." + extension;
		hFind = FindFirstFile(browseDirectory.c_str(), &data);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				std::string fileName = std::string(data.cFileName);
				std::string fileExt = std::string(PathFindExtensionA(data.cFileName));
				std::string fileKey = prefix + std::string(fileName).substr(0, fileName.length() - fileExt.length());
				bool success = battleSoundManager->addSoundFullPath(fileKey, directory + std::string(data.cFileName));
				if (success){
					std::cout << "Added " << fileName << " to sound manager with key " << fileKey << std::endl;
				}
				else{
					std::cout << "Can't add file - have you used a name which was already set?" << std::endl;
				}
				//printf("%s\n", data.cFileName);
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
	}

	/* add additional sounds to sound manager */
	void FK_SceneGame::addCharacterSoundsToSoundManager(){
		// start with Player 1
		// add outfit specific sounds
		std::string startingDirectory = charactersPath + player1_configPath + player1->getOutfitPath() +"sounds\\";
		if (fk_addons::directoryExists(startingDirectory.c_str())){
			// look for all .ogg files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "ogg", "p1_");
			// look for all .wav files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "wav", "p1_");
		}
		for each (std::string transformationId in player1->getAvailableTransformationNames()) {
			std::string directoryToCheck = charactersPath + player1_configPath + 
				player1->getOutfitPath() + transformationId + "\\sounds\\";
			if (fk_addons::directoryExists(directoryToCheck.c_str())) {
				// look for all .ogg files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "ogg", "p1_" + transformationId + "_");
				// look for all .wav files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "wav", "p1_" + transformationId + "_");
			}
			directoryToCheck = charactersPath + player1_configPath +
				"sounds\\" + transformationId + "\\";
			if (fk_addons::directoryExists(directoryToCheck.c_str())) {
				// look for all .ogg files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "ogg", "p1_" + transformationId + "_");
				// look for all .wav files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "wav", "p1_" + transformationId + "_");
			}
		}
		// add normal sounds
		startingDirectory = charactersPath + player1_configPath + "sounds\\";
		if (fk_addons::directoryExists(startingDirectory.c_str())){
			// look for all .ogg files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "ogg", "p1_");
			// look for all .wav files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "wav", "p1_");
		}
		// start with Player 2
		// add outfit specific sounds
		startingDirectory = charactersPath + player2_configPath + player2->getOutfitPath() +"sounds\\";
		if (fk_addons::directoryExists(startingDirectory.c_str())){
			// look for all .ogg files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "ogg", "p2_");
			// look for all .wav files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "wav", "p2_");
		}
		for each (std::string transformationId in player2->getAvailableTransformationNames()) {
			std::string directoryToCheck = charactersPath + player2_configPath + 
				player2->getOutfitPath() + transformationId + "\\sounds\\";
			if (fk_addons::directoryExists(directoryToCheck.c_str())) {
				// look for all .ogg files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "ogg", "p2_" + transformationId + "_");
				// look for all .wav files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "wav", "p2_" + transformationId + "_");
			}
			directoryToCheck = charactersPath + player2_configPath +
				"sounds\\" + transformationId + "\\";
			if (fk_addons::directoryExists(directoryToCheck.c_str())) {
				// look for all .ogg files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "ogg", "p2_" + transformationId + "_");
				// look for all .wav files
				addSoundsFromDirectoryToSoundManager(directoryToCheck, "wav", "p2_" + transformationId + "_");
			}
		}
		// add normal sounds
		startingDirectory = charactersPath + player2_configPath + "sounds\\";
		if (fk_addons::directoryExists(startingDirectory.c_str())){
			// look for all .ogg files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "ogg", "p2_");
			// look for all .wav files
			addSoundsFromDirectoryToSoundManager(startingDirectory, "wav", "p2_");
		}
	}

	/* set up game states */
	void FK_SceneGame::setupGameStates(){
		numberOfPlayer1Wins = 0;
		numberOfPlayer2Wins = 0;
		currentRoundNumber = 0;
		newRoundFlag = true;
		processingKOText = false;
		processingRoundText = false;
		processingFightText = false;
		processingEndOfRound = false;
		processingSceneEnd = false;
		processingContinue = false;
		processingGameOver = false;
		processingDurationMs = 2000;
		processingIntervalMs = 300;
		processingTimeCounterMs = 0;
		processingWinText = false;
		processingRecordText = false;
		processingKOTextDurationMs = 3000;
		processingWinTextDurationMs = 4000;
		processingIntroPlayer1 = false;
		processingIntroPlayer2 = false;
		introPhaseCounter = 0;
		continueCounterSeconds = 9;
		continueCounterDurationMS = 2000;
		continueCounterMS = 0;
		if (skipIntro) introPhaseCounter = 4;
		if (introPhaseCounter == 0 && !databaseAccessor.showStageIntro()) {
			introPhaseCounter = 1;
		}
		winnerId = -99;
	}

	/* setup the arena */
	void FK_SceneGame::setupArena(){
		std::string newStagePath = stagesPath + arena_configPath;
		//std::string newStagePath = stagesPath + "Elevator\\";
		//m_threadMutex.try_lock();
		stage->setup(device, driver, smgr, newStagePath);
		//m_threadMutex.unlock();
		/* play BGM!!!! */
		std::string filePath = newStagePath + stage->getBGMName();
		if (!stage_bgm.openFromFile(filePath.c_str()))
			return;
		stage_bgm.setLoop(true);
		stage_bgm.setVolume(stage->getBGMVolume() * gameOptions->getMusicVolume());
		stage_bgm.setPitch(stage->getBGMPitch());
		//m_threadMutex.try_lock();
		loadingPercentage.store(loadingPercentage.load() + 30.0f);
		//Sleep(100);
		stageLoaded = true;
		// set fog
		stage->setFog();
		if (!additionalOptions.allowRingout){
			stage->disallowRingout();
		}
		////m_threadMutex.unlock();
		std::cout << "Stage loaded" << std::endl;
	};

	void FK_SceneGame::setupPlayer1Character(f32 &progress) {

		std::string additionalObjectString = std::string();
		u32 timeInfo = player1input->getLastInputTime();
		timeInfo += 200;
		u32 pressedButtons = 0;

		if (player1isAI && !player2isAI) {
			if (joystickInfo.size() > 1) {
				player2input->update(timeInfo, inputReceiver->JoypadStatus(1), false);
			}
			else {
				player2input->update(timeInfo, inputReceiver->KeyboardStatus(), false);
			}
			pressedButtons = player2input->getPressedButtons();
		}
		else
		{
			if (joystickInfo.size() > 0) {
				player1input->update(timeInfo, inputReceiver->JoypadStatus(0), false);
			}
			else {
				player1input->update(timeInfo, inputReceiver->KeyboardStatus(), false);
			}
			pressedButtons = player1input->getPressedButtons();
		}

		if ((pressedButtons & FK_Input_Buttons::ScrollRight_Button) != 0) {
			additionalObjectString = "var_";
		}

		player1->setup(databaseAccessor,
			"character.txt", charactersPath + player1_configPath,
			commonResourcesPath,
			smgr, player1startingPosition,
			core::vector3df(0, 0, 0),
			player1Outfit.outfitDirectory,
			player1LifeMultiplier,
			additionalOptions.player1MovesetIdentifier,
			additionalObjectString,
			true);
		//m_threadMutex.unlock();
		//player1->getAnimatedMesh()->setDebugDataVisible(E_DEBUG_SCENE_TYPE::EDS_BBOX);
		player1->toggleHitboxVisibility(false);
		player1->toggleHurtboxVisibility(false);
		player1->toggleLighting(false);
		player1OldTriggerStatus = false;
		player1OldImpactCancelStatus = false;
		progress += 20.f;
		drawLoadingScreen(progress);
	}

	void FK_SceneGame::setupPlayer2Character(f32 &progress) {

		std::string additionalObjectString = std::string();
		u32 timeInfo = player2input->getLastInputTime();
		timeInfo += 200;
		u32 pressedButtons = 0;
		if (player2isAI) {
			if (joystickInfo.size() > 0) {
				player1input->update(timeInfo, inputReceiver->JoypadStatus(0), false);
			}
			else {
				player1input->update(timeInfo, inputReceiver->KeyboardStatus(), false);
			}
			pressedButtons = player1input->getPressedButtons();
		}
		else {
			if (joystickInfo.size() > 1) {
				player2input->update(timeInfo, inputReceiver->JoypadStatus(1), false);
			}
			else {
				player2input->update(timeInfo, inputReceiver->KeyboardStatus(), false);
			}
			pressedButtons = player2input->getPressedButtons();
		}

		if ((pressedButtons & FK_Input_Buttons::ScrollLeft_Button) != 0) {
			additionalObjectString = "var_";
		}

		u32 outfitIdPl2 = player2Outfit.outfitId;
		bool setupWithId = false;
		bool sameCostumePath = player1Outfit.outfitDirectory == player2Outfit.outfitDirectory;
		if (sameCostumePath && player1_configPath == player2_configPath) {
			if (player1Outfit.outfitId == player2Outfit.outfitId) {
				if (outfitIdPl2 == 0) {
					outfitIdPl2 = 1;
				}
				else {
					outfitIdPl2 = 0;
				}
				setupWithId = true;
			}
		}
		//m_threadMutex.lock();
		/*startingPosition = player2startingPosition;
		startingPosition.Y += (player2->getAnimatedMesh()->getScale().Y - 1.0f) * additionalYunitsPerScaleUnit;*/
		if (setupWithId) {
			player2->setup(
				databaseAccessor,
				"character.txt", charactersPath + ".\\" + player2_configPath,
				commonResourcesPath,
				smgr, player2startingPosition,
				core::vector3df(0, 180, 0),
				outfitIdPl2,
				player2LifeMultiplier,
				additionalOptions.player2MovesetIdentifier,
				additionalObjectString,
				true);
		}else{
			player2->setup(
				databaseAccessor,
				"character.txt", charactersPath + ".\\" + player2_configPath,
				commonResourcesPath,
				smgr, player2startingPosition,
				core::vector3df(0, 180, 0),
				player2Outfit.outfitDirectory,
				player2LifeMultiplier,
				additionalOptions.player2MovesetIdentifier,
				additionalObjectString,
				true);
		}
		//m_threadMutex.unlock();
		//player2->getAnimatedMesh()->setDebugDataVisible(E_DEBUG_SCENE_TYPE::EDS_BBOX);
		player2->toggleHitboxVisibility(false);
		player2->toggleHurtboxVisibility(false);
		player2->toggleLighting(false);
		player2OldTriggerStatus = false;
		player2OldImpactCancelStatus = false;
		progress += 20.f;
		drawLoadingScreen(progress);
	}

	/* setup the characters */
	void FK_SceneGame::setupCharacters(f32 &progress){
		/* load player 1 */
		setupPlayer1Character(progress);
		setupPlayer2Character(progress);

		// setup throw reaction moves
		player1->setOpponentThrowAnimation(player2->getMovesCollection());
		player2->setOpponentThrowAnimation(player1->getMovesCollection());
		////m_threadMutex.lock();
		
		//Sleep(100);
		charactersLoaded = true;
		std::cout << "Characters loaded" << std::endl;
		////m_threadMutex.unlock();
//#define DEBUG_FLAG_CHARACTER
#ifdef DEBUG_FLAG_CHARACTER
		player2->toggleHitboxVisibility(false);
		player2->toggleHurtboxVisibility(false);
		player1->toggleHitboxVisibility(true);
		player1->toggleHurtboxVisibility(false);
#endif
		// add additional win and intro quotes
		player1->loadWinQuotesFromCharacterDirectory(charactersPath + player2_configPath, player2->getName(), player2->getOutfit());
		player2->loadWinQuotesFromCharacterDirectory(charactersPath + player1_configPath, player1->getName(), player1->getOutfit());
		player1->loadIntroQuotesFromCharacterDirectory(charactersPath + player2_configPath, player2->getName(), player2->getOutfit());
		player2->loadIntroQuotesFromCharacterDirectory(charactersPath + player1_configPath, player1->getName(), player1->getOutfit());
		// reset triggers to zero if they can't be used
		if (!additionalOptions.allowTriggers){
			player1->setTriggerCounters(0);
			player2->setTriggerCounters(0);
		}
		// add additional damage multipliers from options
		player1->setReceivedPhysicalDamageMultiplier(additionalOptions.receivedPhysicalDamageMultiplierPlayer1);
		player1->setReceivedBulletDamageMultiplier(additionalOptions.receivedBulletDamageMultiplierPlayer1);
		player2->setReceivedPhysicalDamageMultiplier(additionalOptions.receivedPhysicalDamageMultiplierPlayer2);
		player2->setReceivedBulletDamageMultiplier(additionalOptions.receivedBulletDamageMultiplierPlayer2);
		progress += 10.f;
	};

	/* setup AI*/
	void FK_SceneGame::setupAI(){
		int AILevel1 = player1AILevel;
		int AILevel2 = player2AILevel;
		AI1_defenseCheckAgainstThrowPerformed = false;
		AI2_defenseCheckAgainstThrowPerformed = false;
		player1AIManager = new FK_AIManager(player1, player2, AILevel1, numberOfRounds);
		player2AIManager = new FK_AIManager(player2, player1, AILevel2, numberOfRounds);
	}
	/* reset AI */
	void FK_SceneGame::resetAI(){
		/* reset AI buffers (if any)*/
		player1AIManager->reset();
		player2AIManager->reset();
		player1AIManager->updateWinNumbers(numberOfPlayer1Wins, numberOfPlayer2Wins);
		player2AIManager->updateWinNumbers(numberOfPlayer2Wins, numberOfPlayer1Wins);
	}
	/* delete AI*/
	void FK_SceneGame::deleteAI(){
		delete player1AIManager;
		delete player2AIManager;
	}

	/* restart match*/
	void FK_SceneGame::restartMatch(){
		player1->setLifeToMax();
		player2->setLifeToMax();
		player1->resetVariables(true);
		player2->resetVariables(true);
		processingSceneEnd = false;
		processingContinue = false;
		processingEndOfRound = false;
		processingKOText = false;
		newRoundFlag = true;
		currentRoundNumber = 0;
		numberOfPlayer1Wins = 0;
		numberOfPlayer2Wins = 0;
		triggerRegenCounterMSPlayer2 = 0;
		triggerRegenCounterMSPlayer1 = 0;
		winnerId = -99;
		s32 maxNumberOfTriggers = databaseAccessor.getStartingTriggers();
		player1->setTriggerCounters(maxNumberOfTriggers);
		player2->setTriggerCounters(maxNumberOfTriggers);
		if (!additionalOptions.allowTriggers){
			player1->setTriggerCounters(0);
			player2->setTriggerCounters(0);
		}
		roundTimerMs = startRoundTimerMs;
		timeupRoundEnd = false;
		player1ringoutFlag = false;
		player2ringoutFlag = false;
		matchStartingTime = -1;
		for (u32 i = 0; i < brokenObjectsProps.size(); ++i){
			lightManager->removeShadowFromNode(brokenObjectsProps[i].second);
			//propVector[i].second->removeAnimators();
			brokenObjectsProps[i].second->remove();
			delete brokenObjectsProps[i].first;
		}
		brokenObjectsProps.clear();
		Sleep(200);
	}

	/* reset round*/
	void FK_SceneGame::resetRound(){
		//player1->stopAnimationTransitions();
		//player2->stopAnimationTransitions();
		smgr->setActiveCamera(camera);
		/* reset AI*/
		resetAI();
		/* reset timer */
		roundTimerMs = startRoundTimerMs;
		timeupRoundEnd = false;
		/* reset ringout flags */
		player1ringoutFlag = false;
		player2ringoutFlag = false;
		player1SplashEffectPlayed = false;
		player2SplashEffectPlayed = false;
		/* reset hitstun flags and combo counters*/
		player1OldHitStun = false;
		player2OldHitStun = false;
		player1->resetComboCounter();
		player2->resetComboCounter();
		player1->markAllBulletsForDisposal();
		player2->markAllBulletsForDisposal();
		counterAttackEffectCooldownCounterMs = databaseAccessor.getCounterAttackEffectCooldownMs();
		/* reset camera position*/
		float baseCameraDistance = 240.0f;
		float baseCameraYPosition = -16.0f;
		float cameraDistance = baseCameraDistance + 50.0f; //take into account the X position of the models!!!
		float cameraYPosition = baseCameraYPosition;
		cameraManager->resetCameraPosition();
		camera->setPosition(core::vector3df(cameraDistance, cameraYPosition, 0));
		camera->setTarget(core::vector3df(0, 0, 0));

		player2->resetVariables(false);
		if (additionalOptions.regenerateLifePlayer2OnNewRound || player2->isKO()){
			player2->setLifeToMax();
			if (additionalOptions.player2InitialRoundLifeMultiplier > 0.f) {
				f32 lifeToSet = min(player2->getMaxLife(), player2->getMaxLife() * additionalOptions.player2InitialRoundLifeMultiplier);
				player2->setLife(lifeToSet);
			}
		}
		//player2->setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
		//lightManager->removeShadowFromNode(player2->getAnimatedMesh());
		lightManager->addShadowToNode(player2->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
		for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
			if (player2->getCharacterAdditionalObjects().at(i).isActive() && 
				player2->getCharacterAdditionalObjects().at(i).applyShadow){
				lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i), 
					shadowFilterType, shadowModeCharacters);
			}
		}
		ISceneNodeAnimator* pAnim2 = *(player2->getAnimatedMesh()->getAnimators().begin());
		if (pAnim2->getType() == ESNAT_COLLISION_RESPONSE)
		{
			core::vector3df startingPosition = player2startingPosition;
			startingPosition.Y += (player2->getAnimatedMesh()->getScale().Y - 1.0f) * additionalYunitsPerScaleUnit;
			player2->getAnimatedMesh()->removeAnimator(pAnim2);
			player2->setPosition(startingPosition);
			player2->getAnimatedMesh()->updateAbsolutePosition();
		}
		player2->setRotation(core::vector3df(0, 180, 0));

		player1->resetVariables(false);
		if (additionalOptions.regenerateLifePlayer1OnNewRound || player1->isKO()){
			player1->setLifeToMax();
			if (additionalOptions.player1InitialRoundLifeMultiplier > 0.f) {
				f32 lifeToSet = min(player1->getMaxLife(), player1->getMaxLife() * additionalOptions.player1InitialRoundLifeMultiplier);
				player1->setLife(lifeToSet);
			}
		}
		//player1->setBasicAnimation(FK_BasicPose_Type::IdleAnimation, true);
		//lightManager->removeShadowFromNode(player1->getAnimatedMesh());
		lightManager->addShadowToNode(player1->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
		for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i){
			if (player1->getCharacterAdditionalObjects().at(i).isActive() &&
				player1->getCharacterAdditionalObjects().at(i).applyShadow){
				lightManager->addShadowToNode(player1->getCharacterAdditionalObjectsNodes().at(i), 
					shadowFilterType, shadowModeCharacters);
			}
		}
		ISceneNodeAnimator* pAnim1 = *(player1->getAnimatedMesh()->getAnimators().begin());
		if (pAnim1->getType() == ESNAT_COLLISION_RESPONSE)
		{
			core::vector3df startingPosition = player1startingPosition;
			startingPosition.Y += (player1->getAnimatedMesh()->getScale().Y - 1.0f) * additionalYunitsPerScaleUnit;
			player1->getAnimatedMesh()->removeAnimator(pAnim1);
			player1->setPosition(startingPosition);
			player1->getAnimatedMesh()->updateAbsolutePosition();
		}
		player1->setRotation(core::vector3df(0, 0, 0));
		setupCharacterArenaInteraction();
		player1->getAnimatedMesh()->OnRegisterSceneNode();
		player2->getAnimatedMesh()->OnRegisterSceneNode();

		/* set life ratios in HUD */
		hudManager->setOldLifeRatios(player1->getLifeRatio(), player2->getLifeRatio());

		/* restore 1 trigger to the winner and 2 for the loser of last round. Restore 1 each in case of draw*/
		if (additionalOptions.allowTriggers){
			if (winnerId == -1){
				player1->addTriggerCounters(databaseAccessor.getTriggerRegenPerRoundWon());
				player2->addTriggerCounters(databaseAccessor.getTriggerRegenPerRoundLost());
			}
			else if (winnerId == 1){
				player2->addTriggerCounters(databaseAccessor.getTriggerRegenPerRoundWon());
				player1->addTriggerCounters(databaseAccessor.getTriggerRegenPerRoundLost());
			}
			else if (winnerId == 0){
				player1->addTriggerCounters(databaseAccessor.getTriggerRegenPerDraw());
				player2->addTriggerCounters(databaseAccessor.getTriggerRegenPerDraw());
			}
		}
		player1NumberOfBulletCounters = player1->getTriggerCounters();
		player2NumberOfBulletCounters = player2->getTriggerCounters();
		// reset all props which shouldn't remain on the ground
		auto propVector = brokenObjectsProps;
		brokenObjectsProps.clear();
		for (u32 i = 0; i < propVector.size(); ++i){
			if (!propVector[i].first->object.canBeRestored || propVector[i].first->object.leavePropOnGroundWhenRestored){
				brokenObjectsProps.push_back(propVector[i]);
			}
			else{
				delete propVector[i].first;
				lightManager->removeShadowFromNode(propVector[i].second);
				//propVector[i].second->removeAnimators();
				propVector[i].second->remove();
			}
		}
		needMeshSynchronization = true;
		// synchronize objects
		player1->synchronizeAnimatedObjects();
		player2->synchronizeAnimatedObjects();
		//player1->allowAnimationTransitions();
		//player2->allowAnimationTransitions();
		cameraManager->alignPlayerTowardsPlayer(player1, player2);
		cameraManager->alignPlayerTowardsPlayer(player2, player1);
	}

	void FK_SceneGame::setupCharacterArenaInteraction(){
		/* create selector to manage collisions with the world */
		/* create stage collider */
		scene::ITriangleSelector* selector = 0;// stage->getStage()->getTriangleSelector();
		if (stage->getStage())
		{
			selector = smgr->createOctreeTriangleSelector(
				stage->getStageMesh()->getMesh(0), stage->getStage(), 128);
			stage->getStage()->setTriangleSelector(selector);
		}
		if (stage->getSkybox() != NULL){
			scene::ITriangleSelector* skyboxSelector = smgr->createTriangleSelectorFromBoundingBox(stage->getSkybox());
			stage->getSkybox()->setTriangleSelector(skyboxSelector);
			skyboxSelector->drop();
		}
		//m_threadMutex.unlock();
		/* prevent the creation of more than one collider*/
		if (player1CollisionCallback == 0){
			player1CollisionCallback = new FK_WorldCollisionCallback(player1);
			player2CollisionCallback = new FK_WorldCollisionCallback(player2);
		}

		if (selector)
		{
			//float gravity = 0.0f;
			/*
			const core::aabbox3d<f32> &box1 = player1->getAnimatedMesh()->getMesh()->getBoundingBox();
			core::vector3df ellipse_radius = (box1.MaxEdge - box1.getCenter());
			//core::vector3df ellipse_radius = core::vector3df(10, 44.5, 10);//(box.MaxEdge - box.getCenter());
			*/
			//m_threadMutex.lock();
			f32 sceneGravityToSet = sceneGravity;
			f32 modifier = 0.4f;
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatMoonGravity) != 0) {
				sceneGravityToSet *= modifier;
			}

			core::vector3df ellipse_radius = core::vector3df(20, 44.5, 20);
			ellipse_radius.Y *= player1->getAnimatedMesh()->getScale().Y;

			scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
				selector, player1->getAnimatedMesh(), ellipse_radius,
				core::vector3df(0, sceneGravityToSet, 0), core::vector3df(0, 0, 0), 0.01);

			anim->setCollisionCallback(player1CollisionCallback);
			//m_threadMutex.unlock();
		
			ellipse_radius = core::vector3df(20, 44.5, 20);
			ellipse_radius.Y *= player2->getAnimatedMesh()->getScale().Y;
			scene::ISceneNodeAnimatorCollisionResponse* anim2 = smgr->createCollisionResponseAnimator(
				selector, player2->getAnimatedMesh(), ellipse_radius,
				core::vector3df(0, sceneGravityToSet, 0), core::vector3df(0, 0, 0), 0.01);
			anim2->setCollisionCallback(player2CollisionCallback);
			selector->drop(); // As soon as we're done with the selector, drop it.
			//m_threadMutex.lock();
			player1->getAnimatedMesh()->addAnimator(anim);
			player2->getAnimatedMesh()->addAnimator(anim2);
			anim->drop();  // And likewise, drop the animator when we're done referring to it.
			anim2->drop();
			// synchronize objects
			/*player1->synchronizeAnimatedObjects();
			player2->synchronizeAnimatedObjects();*/
			//m_threadMutex.unlock();
		};
	};
	/* setup post-processing effects and lights */
	void FK_SceneGame::setupLights(){
		core::dimension2du renderSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			renderSize = borderlessWindowRenderTarget->getSize();
		}
		lightManager = new EffectHandler(device, renderSize, false, true, false);
		/*set filter for shadows*/
		shadowFilterType = (E_FILTER_TYPE)core::clamp<u32>(3, 0, 4);

		FK_Options::FK_PostProcessingEffect postProcessing = gameOptions->getPostProcessingShadersFlag();

		if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::FullShadows){
			shadowModeCharacters = E_SHADOW_MODE::ESM_BOTH;
			shadowModeStage = E_SHADOW_MODE::ESM_BOTH;
		}
		else if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::OnlyCharaShadows){
			shadowModeCharacters = E_SHADOW_MODE::ESM_CAST;
			shadowModeStage = E_SHADOW_MODE::ESM_RECEIVE;
		}
		else if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::NoShadowEffect){
			shadowModeCharacters = E_SHADOW_MODE::ESM_EXCLUDE;
			shadowModeStage = E_SHADOW_MODE::ESM_EXCLUDE;
		}
		/* set shadow detail level */
		u32 shadowDimen = gameOptions->getShadowQuality();
		/* add shadow to arena after disabling standard lighting - UPDATE: not needed, if one wants emission effects */
		if (gameOptions->getLightEffectsActiveFlag()) {
			lightManager->addShadowToNode(stage->getStage(), shadowFilterType, shadowModeStage);
		}
		// add shadow to water node
		if (stage->getStageWaterNode() != NULL){
			stage->getStageWaterNode()->setMaterialFlag(EMF_LIGHTING, false);
			if (gameOptions->getLightEffectsActiveFlag()) {
				lightManager->addShadowToNode(stage->getStageWaterNode(), shadowFilterType, shadowModeStage);
			}
		}
		//if (stage->hasAvailableSkybox()){
			//lightManager->addShadowToNode(stage->getSkybox(), shadowFilterType, E_SHADOW_MODE::ESM_RECEIVE);
		//}
		// add shadow to additional elements
		if (!stage->getAdditionalNodes().empty()){
			int size = stage->getAdditionalNodes().size();
			for (int i = 0; i < size; ++i){
				stage->getAdditionalNodes()[i]->setMaterialFlag(EMF_LIGHTING, false);
				if (gameOptions->getLightEffectsActiveFlag()){
					lightManager->addShadowToNode(stage->getAdditionalNodes()[i], shadowFilterType, shadowModeStage);
				}
			}
		}
		if (introPhaseCounter > 3){
			lightManager->addShadowToNode(player1->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			lightManager->addShadowToNode(player2->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i){
				if (player1->getCharacterAdditionalObjects().at(i).applyShadow){
					lightManager->addShadowToNode(player1->getCharacterAdditionalObjectsNodes().at(i), 
						shadowFilterType, shadowModeCharacters);
				}
			}
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
				if (player2->getCharacterAdditionalObjects().at(i).applyShadow){
					lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i), 
						shadowFilterType, shadowModeCharacters);
				}
			}
		}
		lightManager->setClearColour(stage->getBackgroundColor());
		lightManager->setAmbientColor(stage->getAmbientColor());
		/* add post processing effects */
		const stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";

		if (postProcessing == FK_Options::FK_PostProcessingEffect::AllEffects){
			s32 brightpassMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BrightPass2") + shaderExt);
			scenePixelShaderCallback["brightpass"] = new FK_PixelShaderSceneGameCallback(brightpassMaterialId, this, 
				FK_SceneShaderTypes::ShaderBrightPass);
			lightManager->setPostProcessingRenderCallback(brightpassMaterialId, scenePixelShaderCallback["brightpass"]);

			lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BlurHP") + shaderExt);
			lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BlurVP") + shaderExt);

			s32 bloomMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BloomP") + shaderExt);
			scenePixelShaderCallback["bloom"] = new FK_PixelShaderSceneGameCallback(bloomMaterialId, this, 
				FK_SceneShaderTypes::ShaderBloom);
			lightManager->setPostProcessingRenderCallback(bloomMaterialId, scenePixelShaderCallback["bloom"]);
			/*lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/edgeDetect") + shaderExt);*/
		}
		else if (postProcessing == FK_Options::FK_PostProcessingEffect::BloomOnly){
			s32 brightpassMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BrightPass2") + shaderExt);
			scenePixelShaderCallback["brightpass"] = new FK_PixelShaderSceneGameCallback(brightpassMaterialId, this,
				FK_SceneShaderTypes::ShaderBrightPass);
			lightManager->setPostProcessingRenderCallback(brightpassMaterialId, scenePixelShaderCallback["brightpass"]);
			s32 bloomMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/BloomP") + shaderExt);
			scenePixelShaderCallback["bloom"] = new FK_PixelShaderSceneGameCallback(bloomMaterialId, this,
				FK_SceneShaderTypes::ShaderBloom);
			lightManager->setPostProcessingRenderCallback(bloomMaterialId, scenePixelShaderCallback["bloom"]);
		}
		else{
			// increase brightness in ambient colour
			video::SColorHSL tempColor;
			tempColor.fromRGB(stage->getAmbientColor());
			tempColor.Luminance = 1.1 * tempColor.Luminance;
			tempColor.Saturation = 1.1 * tempColor.Saturation;
			video::SColorf tempColorRGBf;
			tempColor.toRGB(tempColorRGBf);
			video::SColor tempColorRGB = tempColorRGBf.toSColor();
			lightManager->setAmbientColor(tempColorRGB);
			//lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
			//	core::stringc("shaders/edgeDetect") + shaderExt);
		}
		// add stage shaders
		for each(FK_Stage::FK_StagePixelShader shader in stage->getAdditionalPixelShaders()) {
			if (!shader.parameters.empty()) {
				std::string path = stagesPath + arena_configPath + shader.filename;
				s32 materialId = lightManager->addPostProcessingEffectFromFile(core::stringc(path.c_str()) + shaderExt);
				FK_PixelShaderStageCallback* stageCallback = new FK_PixelShaderStageCallback(materialId, this, shader);
				stagePixelShaderCallback.push_back(stageCallback);
				lightManager->setPostProcessingRenderCallback(materialId, stagePixelShaderCallback[
					stagePixelShaderCallback.size() - 1]);
			}
		}

		if (driver->getDriverType() == EDT_DIRECT3D9){
			// sepia effect during trigger and pause
			s32 sepiaMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/Sepia_scene") + shaderExt);
			scenePixelShaderCallback["sepia"] = new FK_PixelShaderSceneGameCallback(sepiaMaterialId, this, FK_SceneShaderTypes::ShaderSepia);
			lightManager->setPostProcessingRenderCallback(sepiaMaterialId, scenePixelShaderCallback["sepia"]);
			// zoom blur at KO
			s32 zoomBlurMaterialId = lightManager->addPostProcessingEffectFromFile(core::stringc(applicationPath.c_str()) +
				core::stringc("shaders/zoomBlur") + shaderExt);
			scenePixelShaderCallback["zoomBlur"] = new FK_PixelShaderSceneGameCallback(zoomBlurMaterialId, this,
				FK_SceneShaderTypes::ShaderZoomBlur);
			lightManager->setPostProcessingRenderCallback(zoomBlurMaterialId, scenePixelShaderCallback["zoomBlur"]);
		}
		/* create lights*/
		lightManager->addShadowLight(SShadowLight(shadowDimen, vector3df(0, 90, 0), vector3df(0, 0, 0),
			stage->getLightColor(), 1.0f, 240.0f, 120.0f * DEGTORAD));
		lightManager->enableDepthPass(true);
		lightManager->addNodeToDepthPass(player1->getAnimatedMesh());
		lightManager->addNodeToDepthPass(player1->getHitParticleEffect());
		lightManager->addNodeToDepthPass(player1->getGuardParticleEffect());
		lightManager->addNodeToDepthPass(player2->getAnimatedMesh());
		lightManager->addNodeToDepthPass(player2->getHitParticleEffect());
		lightManager->addNodeToDepthPass(player2->getGuardParticleEffect());
		lightManager->addNodeToDepthPass(stage->getStage());
		if (stage->getRainEmitter() != NULL){
			lightManager->addNodeToDepthPass(stage->getRainEmitter());
		}
		if (stage->getStageWaterNode() != NULL){
			lightManager->addNodeToDepthPass(stage->getStageWaterNode());
		}
		for (int i = 0; i < stage->getAdditionalNodes().size(); ++i){
			lightManager->addNodeToDepthPass(stage->getAdditionalNodes()[i]);
		}

		// apply wireframe cheat, if any
		if (getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatWireframe) {
			int mcount = player1->getAnimatedMesh()->getMaterialCount();
			for (u32 i = 0; i < mcount; ++i) {
				player1->getAnimatedMesh()->getMaterial(i).Wireframe = true;
			}
			for (u32 j = 0; j < player1->getCharacterAdditionalObjectsNodes().size(); ++j) {
				if (!player1->getCharacterAdditionalObjects().at(j).hasDummyMesh()) {
					mcount = player1->getCharacterAdditionalObjectsNodes().at(j)->getMaterialCount();
					for (u32 i = 0; i < mcount; ++i) {
						player1->getCharacterAdditionalObjectsNodes().at(j)->getMaterial(i).Wireframe = true;
					}
				}
			}
			mcount = player2->getAnimatedMesh()->getMaterialCount();
			for (u32 i = 0; i < mcount; ++i) {
				player2->getAnimatedMesh()->getMaterial(i).Wireframe = true;
			}
			for (u32 j = 0; j < player2->getCharacterAdditionalObjectsNodes().size(); ++j) {
				if (!player2->getCharacterAdditionalObjects().at(j).hasDummyMesh()) {
					mcount = player2->getCharacterAdditionalObjectsNodes().at(j)->getMaterialCount();
					for (u32 i = 0; i < mcount; ++i) {
						player2->getCharacterAdditionalObjectsNodes().at(j)->getMaterial(i).Wireframe = true;
					}
				}
			}
		}
	};

	/* setup character shaders*/
	void FK_SceneGame::setupShaders(){
		// create a shader callback
		FK_CharacterMaterialShaderCallback* shaderCallback1 = new FK_CharacterMaterialShaderCallback(device, lightManager,
			player1);
		FK_CharacterMaterialShaderCallback* shaderCallback2 = new FK_CharacterMaterialShaderCallback(device, lightManager,
			player2);
		// create materials
		s32 characterMeshMaterial = 0;
		video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
		const core::stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";
		const core::stringc vsFileName = core::stringc(applicationPath.c_str()) +
			core::stringc("shaders/charaVxShader") + shaderExt;
		const core::stringc psFileName = core::stringc(applicationPath.c_str()) +
			core::stringc("shaders/charaVxShader") + shaderExt;

		const video::E_GPU_SHADING_LANGUAGE shadingLanguage = video::EGSL_DEFAULT;

		// create material from high level shaders (hlsl, glsl or cg)

		characterMeshMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain", 
			driver->queryFeature(video::EVDF_VERTEX_SHADER_3_0) ? EVST_VS_3_0 : EVST_VS_2_0,
			psFileName, "pixelMain", 
			driver->queryFeature(video::EVDF_PIXEL_SHADER_3_0) ? EPST_PS_3_0 : EPST_PS_2_0,
			shaderCallback1, video::EMT_TRANSPARENT_ALPHA_CHANNEL, 0, shadingLanguage);
		// add material to buffer
		characterMeshMaterialPlayer1.push_back(characterMeshMaterial);
		lightManager->addCustomMaterialToBuffer(characterMeshMaterial, video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		characterMeshMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain",
			driver->queryFeature(video::EVDF_VERTEX_SHADER_3_0) ? EVST_VS_3_0 : EVST_VS_2_0,
			psFileName, "pixelMain",
			driver->queryFeature(video::EVDF_PIXEL_SHADER_3_0) ? EPST_PS_3_0 : EPST_PS_2_0,
			shaderCallback2, video::EMT_TRANSPARENT_ALPHA_CHANNEL, 0, shadingLanguage);
		lightManager->addCustomMaterialToBuffer(characterMeshMaterial, video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		characterMeshMaterialPlayer2.push_back(characterMeshMaterial);

		//PLAYER 1

		characterMeshMaterial = characterMeshMaterialPlayer1[characterMeshMaterialPlayer1.size() - 1];

		// attach shader to player mesh
		//player1->getAnimatedMesh()->setMaterialType((video::E_MATERIAL_TYPE)characterMeshMaterial);
		for (int i = 0; i < player1->getAnimatedMesh()->getMaterialCount(); ++i){
			SMaterial mat = player1->getAnimatedMesh()->getMaterial(i); 
			if (mat.TextureLayer[0].Texture != NULL){
				player1->getAnimatedMesh()->getMaterial(i).MaterialType = (video::E_MATERIAL_TYPE)characterMeshMaterial;
			}	
		}
		// add shader to additional objects
		for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i){
			if (!player1->getCharacterAdditionalObjects().at(i).hasDummyMesh()){
				for (int k = 0; k < player1->getCharacterAdditionalObjectsNodes().at(i)->getMaterialCount(); ++k){
					SMaterial mat = player1->getCharacterAdditionalObjectsNodes().at(i)->getMaterial(k);
					if (mat.TextureLayer[0].Texture != NULL){
						player1->getCharacterAdditionalObjectsNodes().at(i)->getMaterial(k).MaterialType = 
							(video::E_MATERIAL_TYPE)characterMeshMaterial;
					}
				}
			}
		}

		//PLAYER 2

		characterMeshMaterial = characterMeshMaterialPlayer2[characterMeshMaterialPlayer2.size() - 1];
		// attach shader to player mesh
		//player2->getAnimatedMesh()->setMaterialType((video::E_MATERIAL_TYPE)characterMeshMaterial);
		//// add shader to additional objects
		//for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
		//	if (!player2->getCharacterAdditionalObjects().at(i).hasDummyMesh()){
		//		player2->getCharacterAdditionalObjectsNodes().at(i)->setMaterialType((video::E_MATERIAL_TYPE)characterMeshMaterial);
		//	}
		//}
		for (int i = 0; i < player2->getAnimatedMesh()->getMaterialCount(); ++i){
			SMaterial mat = player2->getAnimatedMesh()->getMaterial(i);
			if (mat.TextureLayer[0].Texture != NULL){
				player2->getAnimatedMesh()->getMaterial(i).MaterialType = (video::E_MATERIAL_TYPE)characterMeshMaterial;
			}
		}
		// add shader to additional objects
		for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
			if (!player2->getCharacterAdditionalObjects().at(i).hasDummyMesh()){
				for (int k = 0; k < player2->getCharacterAdditionalObjectsNodes().at(i)->getMaterialCount(); ++k){
					SMaterial mat = player2->getCharacterAdditionalObjectsNodes().at(i)->getMaterial(k);
					if (mat.TextureLayer[0].Texture != NULL){
						player2->getCharacterAdditionalObjectsNodes().at(i)->getMaterial(k).MaterialType =
							(video::E_MATERIAL_TYPE)characterMeshMaterial;
					}
				}
				//player2->getCharacterAdditionalObjectsNodes().at(i)->setMaterialType((video::E_MATERIAL_TYPE)characterMeshMaterial);
			}
		}

		// drop shader callbacks
		shaderCallback1->drop();
		shaderCallback2->drop();
	}

	/* setup the camera and the camera manager *WARNING* players must be initalized before this method is called! */
	void FK_SceneGame::setupCamera(){
		/* define base camera position*/
		float baseCameraDistance = 290.0f;
		float baseCameraYPosition = -23.0f;
		float cameraDistance = baseCameraDistance;// + 50.0f; //take into account the X position of the models!!!
		float cameraYPosition = baseCameraYPosition;
		/* initialize the camera */
		//m_threadMutex.lock();
		camera = smgr->addCameraSceneNode(0, vector3df(cameraDistance, cameraYPosition, 0), vector3df(0, 0, 0));
		camera->setAutomaticCulling(E_CULLING_TYPE::EAC_BOX);
		camera->setFarValue(20000.0f);
		camera->setNearValue(10.0f);
		camera->setAspectRatio(16.f / 9);

		freeCamera = smgr->addCameraSceneNode(0, vector3df(cameraDistance, cameraYPosition, 0), vector3df(0, 0, 0));
		freeCamera->setAutomaticCulling(E_CULLING_TYPE::EAC_BOX);
		freeCamera->setFarValue(20000.0f);
		freeCamera->setNearValue(10.0f);
		freeCamera->setAspectRatio(16.f / 9);
		f32 defaultFOVAngle = core::PI / 2.5;
		freeCamera->setFOV(defaultFOVAngle);
		//player1camera = smgr->addCameraSceneNode(0, vector3df(cameraDistance, cameraYPosition, 0), vector3df(0, 0, 0));
		//player1camera->setAutomaticCulling(E_CULLING_TYPE::EAC_BOX);
		//player1camera->setFarValue(20000.0f);
		//player1camera->setNearValue(20.0f);
		//player1camera->setAspectRatio(16.f / 9);

		////player1camera->setParent(player1->getHitboxCollection()[FK_Hitbox_Type::Head]);
		//player1camera->setParent(player1->getAnimatedMesh());
		//player1camera->setPosition(core::vector3df(0.f, 20.f, -100.f));
		//player1camera->setRotation(player1->getRotation());

		//player2camera = smgr->addCameraSceneNode(0, vector3df(cameraDistance, cameraYPosition, 0), vector3df(0, 0, 0));
		//player2camera->setAutomaticCulling(E_CULLING_TYPE::EAC_BOX);
		//player2camera->setFarValue(20000.0f);
		//player2camera->setNearValue(20.0f);
		//player2camera->setAspectRatio(16.f / 9);

		//player2camera->setParent(player2->getAnimatedMesh());
		//player2camera->setPosition(core::vector3df(0.f, 20.f, -100.f));
		//player2camera->setRotation(player2->getRotation());

		smgr->setActiveCamera(camera);
		//m_threadMutex.unlock();
		/* initalize the cameraManager*/
		cameraManager = new FK_SceneViewUpdater(smgr, player1,
			player2, stage, camera, cameraYPosition, baseCameraDistance, -23.0, 85.0, 150.0, 420.0);
		//cameraManager->addPlayerCameras(player1camera, player2camera);
	};

	// update character transformations, if needed
	void FK_SceneGame::updateCharactersTransformations() {
		/* check for transformations */
		/* remove shadows from all nodes before performing transformation, then assign shadows again to prevent crashes */
		if (player1->hasToTransform()) {
			lightManager->removeShadowFromNode(player1->getAnimatedMesh());
			for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i) {
				if (player1->getCharacterAdditionalObjects().at(i).applyShadow) {
					lightManager->removeShadowFromNode(player1->getCharacterAdditionalObjectsNodes().at(i));
				}
			}
			player1->performTransformation();
			lightManager->addShadowToNode(player1->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i) {
				if (player1->getCharacterAdditionalObjects().at(i).applyShadow) {
					if (player1->getCharacterAdditionalObjects().at(i).isActive()) {
						lightManager->addShadowToNode(player1->getCharacterAdditionalObjectsNodes().at(i),
							shadowFilterType, shadowModeCharacters);
					}
				}
			}
		}
		// player 2
		if (player2->hasToTransform()) {
			lightManager->removeShadowFromNode(player2->getAnimatedMesh());
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i) {
				if (player2->getCharacterAdditionalObjects().at(i).applyShadow) {
					lightManager->removeShadowFromNode(player2->getCharacterAdditionalObjectsNodes().at(i));
				}
			}
			player2->performTransformation();
			lightManager->addShadowToNode(player2->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i) {
				if (player2->getCharacterAdditionalObjects().at(i).applyShadow) {
					if (player2->getCharacterAdditionalObjects().at(i).isActive()) {
						lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i),
							shadowFilterType, shadowModeCharacters);
					}
				}
			}
		}
	}

	// update character ringout
	void FK_SceneGame::updateCharactersRingoutInteractions() {
		/* check for ringout if the game is active */
		if (player1->getPosition().Y < stage->getRingoutDepth()) {
			player1->setVelocityPerSecond(core::vector3d<f32>(0.f, 0.f, player1->getVelocityPerSecond().Z));
			if (canPauseGame() && !isTimeUp() && !player1->isKO()) {
				player1->receiveDamage(player1->getLife(), false);
				player1ringoutFlag = true;
			}
			if (!player1SplashEffectPlayed) {
				if (player1->hasActiveTransformation()) {
					std::string playerTag = "p1_";
					playerTag += player1->getCurrentTransformationId() + "_";
					if (!playHitSounds(FK_Reaction_Type::StrongFlight, playerTag, player1)) {
						playHitSounds(FK_Reaction_Type::StrongFlight, "p1_", player1);
					}
				}
				else {
					playHitSounds(FK_Reaction_Type::StrongFlight, "p1_", player1);
				}
				//battleSoundManager->playSound("p1_ko", 100.0 * gameOptions->getVoicesVolume());
				player1SplashEffectPlayed = true;
				if (stage->hasRingoutSplashEffect()) {
					stage->showWaterSplashEmitter(player1->getPosition(), 0);
					f32 volume = 100.f;
					f32 pitch = 1.f;
					if (!stage->getSplashSoundEffect().filename.empty()) {
						volume = stage->getSplashSoundEffect().volume;
						pitch = stage->getSplashSoundEffect().pitch;
					}
					battleSoundManager->playSound("ringout_splash", volume * gameOptions->getSFXVolume(), pitch);
				}
			}
			player1->cancelHitstun();
			if (player1->isAirborne()) {
				player1->cancelJumpVariables();
				core::vector3df velocityVector = player1->getVelocityPerSecond();
				velocityVector.Z = -100;
				player1->setVelocityPerSecond(velocityVector);
				//player1->resetMove();
			}
		}
		if (player2->getPosition().Y < stage->getRingoutDepth()) {
			player2->setVelocityPerSecond(core::vector3d<f32>(0.f, 0.f, player2->getVelocityPerSecond().Z));
			if (canPauseGame() && !isTimeUp() && !player2->isKO()) {
				player2->receiveDamage(player2->getLife(), false);
				player2ringoutFlag = true;
			}
			if (!player2SplashEffectPlayed) {
				if (player2->hasActiveTransformation()) {
					std::string playerTag = "p2_";
					playerTag += player2->getCurrentTransformationId() + "_";
					if (!playHitSounds(FK_Reaction_Type::StrongFlight, playerTag, player2)) {
						playHitSounds(FK_Reaction_Type::StrongFlight, "p2_", player2);
					}
				}
				else {
					playHitSounds(FK_Reaction_Type::StrongFlight, "p2_", player2);
				}
				//battleSoundManager->playSound("p2_ko", 100.0 * gameOptions->getVoicesVolume());
				player2SplashEffectPlayed = true;
				if (stage->hasRingoutSplashEffect()) {
					stage->showWaterSplashEmitter(player2->getPosition(), 1);
					f32 volume = 100.f;
					f32 pitch = 1.f;
					if (!stage->getSplashSoundEffect().filename.empty()) {
						volume = stage->getSplashSoundEffect().volume;
						pitch = stage->getSplashSoundEffect().pitch;
					}
					battleSoundManager->playSound("ringout_splash", volume * gameOptions->getSFXVolume(), pitch);
				}
			}
			player2->cancelHitstun();
			if (player2->isAirborne()) {
				player2->cancelJumpVariables();
				core::vector3df velocityVector = player2->getVelocityPerSecond();
				velocityVector.Z = -100;
				player2->setVelocityPerSecond(velocityVector);
				//player2->resetMove();
			}
		}
	}

	/* update characters */
	void FK_SceneGame::updateCharacters(f32 frameDeltaTime){
		/* update character transformations */
		updateCharactersTransformations();
		/* check for ringout if the game is active */
		updateCharactersRingoutInteractions();

		if (canUpdateRoundTimer()){
			//! apply progressive damage/regeneration, if possible
			updatePlayersPoisonEffect(frameDeltaTime);
			//! update trigger regeneration, if possible
			updatePlayersTriggerRegen((u32)std::round(1000 * frameDeltaTime));
		}

		//! check for tracking and rotations
		if (player1->hasToTrackOpponent()){
			cameraManager->alignPlayerTowardsPlayer(player1, player2);
			player1->setTrackingFlag(false);
		}
		if (player2->hasToTrackOpponent()){
			cameraManager->alignPlayerTowardsPlayer(player2, player1);
			player2->setTrackingFlag(false);
		}
		//! check for hitstun (reset combo counters)
		if (player1OldHitStun && !player1->isHitStun()){
			player2->resetComboCounter();
		}
		player1OldHitStun = player1->isHitStun();
		if (player2OldHitStun && !player2->isHitStun()){
			player1->resetComboCounter();
		}
		player2OldHitStun = player2->isHitStun();
		player1OldHitStun = player1->isHitStun();
		//! check for sound effects
		s32 player1Frame = (s32)std::floor(player1->getCurrentFrame());
		s32 player2Frame = (s32)std::floor(player2->getCurrentFrame());
		if (player1->getCurrentMove() != NULL && player1->getCurrentMove()->canPlaySoundEffect(player1Frame)){
			std::string soundKey = player1->getCurrentMove()->getSoundEffect(player1Frame, true);
			bool success = false;// battleSoundManager->hasSound("p1_" + soundKey);
			if (player1->hasActiveTransformation()) {
				std::string playerTag = "p1_";
				playerTag += player1->getCurrentTransformationId() + "_";
				success = battleSoundManager->hasSound(playerTag + soundKey);
				if (success) {
					battleSoundManager->playSound(playerTag + soundKey, 100.0 * gameOptions->getSFXVolume());
				}
			}
			if(!success){
				success = battleSoundManager->hasSound("p1_" + soundKey);
				if (success) {
					battleSoundManager->playSound("p1_" + soundKey, 100.0 * gameOptions->getSFXVolume());
				}
				else {
					battleSoundManager->playSound(soundKey, 100.0 * gameOptions->getSFXVolume());
				}
			}
		}
		if (player2->getCurrentMove() != NULL && player2->getCurrentMove()->canPlaySoundEffect(player2Frame)){
			std::string soundKey = player2->getCurrentMove()->getSoundEffect(player2Frame, true);
			//bool success = battleSoundManager->hasSound("p2_" + soundKey);
			bool success = false;
			if (player2->hasActiveTransformation()) {
				std::string playerTag = "p2_";
				playerTag += player2->getCurrentTransformationId() + "_";
				success = battleSoundManager->hasSound(playerTag + soundKey);
				if (success) {
					battleSoundManager->playSound(playerTag + soundKey, 100.0 * gameOptions->getSFXVolume());
				}
			}
			if (!success) {
				success = battleSoundManager->hasSound("p2_" + soundKey);
				if (success) {
					battleSoundManager->playSound("p2_" + soundKey, 100.0 * gameOptions->getSFXVolume());
				}
				else {
					battleSoundManager->playSound(soundKey, 100.0 * gameOptions->getSFXVolume());
				}
			}
		}

		//! check impact cancel for sound effect
		if (player1->isInImpactCancelState() != player1OldImpactCancelStatus) {
			player1OldImpactCancelStatus = player1->isInImpactCancelState();
			if (player1->isInImpactCancelState()) {
				player1OldTriggerStatus = player1->isTriggerModeActive();
				player1NumberOfBulletCounters = player1->getTriggerCounters();
				triggerRegenCounterMSPlayer1 = 0;
				battleSoundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
				cancelTriggerComboEffect();
			}
		}
		//! check impact cancel for sound effect
		if (player2->isInImpactCancelState() != player2OldImpactCancelStatus) {
			player2OldImpactCancelStatus = player2->isInImpactCancelState();
			if (player2->isInImpactCancelState()) {
				player2OldTriggerStatus = player2->isTriggerModeActive();
				player2NumberOfBulletCounters = player2->getTriggerCounters();
				triggerRegenCounterMSPlayer2 = 0;
				battleSoundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
				cancelTriggerComboEffect();
			}
		}

		//! check trigger status for sound effect
		if (player1->isTriggerModeActive() != player1OldTriggerStatus ||
			player1NumberOfBulletCounters != player1->getTriggerCounters()){
				
			if (player1NumberOfBulletCounters > player1->getTriggerCounters() ||
				(!player1OldTriggerStatus && player1->isTriggerModeActive())){
				if (databaseAccessor.zeroRegenOnTriggerUse()) {
					triggerRegenCounterMSPlayer1 = 0;
				}
				battleSoundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
				//std::cout << "playing trigger sound" << std::endl;
			}
			player1OldTriggerStatus = player1->isTriggerModeActive();
			player1NumberOfBulletCounters = player1->getTriggerCounters();
		}
		//! check trigger status for sound effect
		if (player2->isTriggerModeActive() != player2OldTriggerStatus ||
			player2NumberOfBulletCounters != player2->getTriggerCounters()){
			if (player2NumberOfBulletCounters > player2->getTriggerCounters() ||
				(!player2OldTriggerStatus && player2->isTriggerModeActive())){
				if (databaseAccessor.zeroRegenOnTriggerUse()) {
					triggerRegenCounterMSPlayer2 = 0;
				}
				battleSoundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
			}
			player2OldTriggerStatus = player2->isTriggerModeActive();
			player2NumberOfBulletCounters = player2->getTriggerCounters();
		}

		// update additional objects for players (that is, special effects like rotation and disappearing)


		/* check input */
		u32 player1InputButtons = 0;
		u32 player2InputButtons = 0;
		f32 frameDeltaTimeP1 = frameDeltaTime;
		f32 frameDeltaTimeP2 = frameDeltaTime;
		u32 frameDeltaMs = u32(frameDeltaTime * 1000);
		updateCharactersPressedButtons(player1InputButtons, player2InputButtons);
		bool newMovePlayer1 = false, newMovePlayer2 = false;
		// add support for cinematic moves (stop time and the likes)
		if (!cinematicCameraActivePlayer2) {
			if (player1->getCurrentMove() != NULL &&
				player1->getCurrentMove()->hasActiveMoveCinematic((u32)player1->getCurrentFrame()) &&
				player1->getCurrentMove()->getCinematic().opponentTimerMultiplier != 1.f) {
				frameDeltaTimeP2 *= player1->getCurrentMove()->getCinematic().opponentTimerMultiplier;
				player2InputButtons = 0;
				if (!cinematicCameraActivePlayer1) {
					cinematicCameraActivePlayer1 = true;
					player2->getAnimatedMesh()->SetInternalTimerScalingFactor(player1->getCurrentMove()->getCinematic().opponentTimerMultiplier);
				}
			}
			else if (cinematicCameraActivePlayer1) {
				player2->getAnimatedMesh()->SetInternalTimerScalingFactor(1.f);
				cinematicCameraActivePlayer1 = false;
			}
		}
		if (!cinematicCameraActivePlayer1) {
			if (player2->getCurrentMove() != NULL &&
				player2->getCurrentMove()->hasActiveMoveCinematic((u32)player2->getCurrentFrame()) &&
				player2->getCurrentMove()->getCinematic().opponentTimerMultiplier != 1.f) {
				frameDeltaTimeP1 *= player2->getCurrentMove()->getCinematic().opponentTimerMultiplier;
				player1InputButtons = 0;
				if (!cinematicCameraActivePlayer2) {
					cinematicCameraActivePlayer2 = true;
					player1->getAnimatedMesh()->SetInternalTimerScalingFactor(player2->getCurrentMove()->getCinematic().opponentTimerMultiplier);
				}
			}
			else if (cinematicCameraActivePlayer2) {
				player1->getAnimatedMesh()->SetInternalTimerScalingFactor(1.f);
				cinematicCameraActivePlayer2 = false;
			}
		}
		// perform proper update
		player1->update(player1InputButtons, frameDeltaTimeP1, newMovePlayer1);
		player2->update(player2InputButtons, frameDeltaTimeP2, newMovePlayer2);
		// update trigger combo visual effects
		updateTriggerComboEffect(frameDeltaMs, newMovePlayer1, newMovePlayer2);
		// update counter attack slowmo effect
		updateCounterattackEffects(frameDeltaMs);
		// update impact effect (if any)
		updateImpactEffect(frameDeltaMs);
		if (!isPaused()){
			if (!(player1->isTriggerModeActive() || player2->isTriggerModeActive())){
				stage_bgm.setVolume(stage->getBGMVolume() * gameOptions->getMusicVolume());
			}
		}
		// update walking sand splash effect
		if (stage->hasWalkingSandEffect()){
			if (player1->getNextMovement().getLength() >= 1 && player1->getNextMovement().Z == 0 || 
				player1->getStance() == FK_Stance_Type::LandingStance ||
				player1->getGroundCollisionFlag()){
				core::vector3df sandPosition = player1->getPosition();
				sandPosition.Y -= 50;
				stage->showSandSplashEmitter(sandPosition, 0);
				player1->setGroundCollisionFlag(false);
			}
			if (player2->getNextMovement().getLength() >= 1 && player2->getNextMovement().Z == 0 ||
				player2->getStance() == FK_Stance_Type::LandingStance ||
				player2->getGroundCollisionFlag()){
				core::vector3df sandPosition = player2->getPosition();
				sandPosition.Y -= 50;
				stage->showSandSplashEmitter(sandPosition, 1);
				player2->setGroundCollisionFlag(false);
			}
		}
	};

	//! update pressed buttons for each character (this is overridden in Tutorial mode)
	void FK_SceneGame::updateCharactersPressedButtons(u32 &player1InputButtons, u32 &player2InputButtons){
		player1InputButtons = player1input->getPressedButtons();
		player2InputButtons = player2input->getPressedButtons();
		/* WIP - read replay buttons instead of the normal ones */
		if (isPlayingReplay()){
			if (player1isAI) {
				player1InputButtons = getAIButtonPressed(1);
			}
			else {
				player1InputButtons = player1ReplayAllButtonsPressed;
			}
			if (player2isAI) {
				player2InputButtons = getAIButtonPressed(2);
			}
			else {
				player2InputButtons = player2ReplayAllButtonsPressed;
			}
		}
		else{
			/* WIP AI controls*/
			if (player1isAI){
				player1InputButtons = getAIButtonPressed(1);
			}
			if (player2isAI){
				player2InputButtons = getAIButtonPressed(2);
			}
		}
		if (!canUpdateInput() || isPaused()){
			player1InputButtons = 0;
			player2InputButtons = 0;
		}
	}

	//! apply progressive damage/regeneration, if possible
	void FK_SceneGame::updatePlayersPoisonEffect(f32 frameDeltaTime){
		player1->receiveDamage(additionalOptions.continuousDamagePlayer1perSecond * frameDeltaTime, false);
		player2->receiveDamage(additionalOptions.continuousDamagePlayer2perSecond * frameDeltaTime, false);
	}
	//! update trigger regeneration, if possible
	void FK_SceneGame::updatePlayersTriggerRegen(u32 frameDeltaTimeMS){
		if (additionalOptions.triggerRegenerationTimeMSPlayer1 > 0){
			if (player1->getTriggerCounters() < databaseAccessor.getMaxTriggers()) {
				triggerRegenCounterMSPlayer1 += (s32)frameDeltaTimeMS;
				if (triggerRegenCounterMSPlayer1 > additionalOptions.triggerRegenerationTimeMSPlayer1) {
					triggerRegenCounterMSPlayer1 = 0;
					player1->addTriggerCounters(1);
				}
			}
		}
		if (additionalOptions.triggerRegenerationTimeMSPlayer2 > 0){
			if (player2->getTriggerCounters() < databaseAccessor.getMaxTriggers()) {
				triggerRegenCounterMSPlayer2 += (s32)frameDeltaTimeMS;
				if (triggerRegenCounterMSPlayer2 > additionalOptions.triggerRegenerationTimeMSPlayer2) {
					triggerRegenCounterMSPlayer2 = 0;
					player2->addTriggerCounters(1);
				}
			}
		}
	}

	// remove it before erasing the bullet
	/* check if bullet can be erased  - this is not a member of the "character class" and it's used
	only to remove expired bullets*/
	static bool canEraseBullet(const FK_Bullet* bullet) {
		return bullet->canBeRemoved();
	}

	// update bullets
	void FK_SceneGame::updateBullets(){
		// set shadow rendering to bullets / remove it before disposing it
		E_SHADOW_MODE shadowMode;
		for each(FK_Bullet* bullet in player1->getBulletsCollection()){
			if (!bullet->isDisposeable() && !bullet->hasLightAssigned()){
				shadowMode = bullet->hasShadow() ? shadowModeCharacters : E_SHADOW_MODE::ESM_EXCLUDE;
				lightManager->addShadowToNode(bullet->getNode(), shadowFilterType, shadowMode);
				lightManager->addNodeToDepthPass(bullet->getNode());
				if (bullet->getParticleEmitter() != NULL){
					lightManager->addNodeToDepthPass(bullet->getParticleEmitter());
				}
				bullet->assignShadowEffect();
			}
			if ((bullet->isDisposeable() || !(bullet->getNode()->isVisible())) && bullet->hasLightAssigned()){
				lightManager->removeShadowFromNode(bullet->getNode());
				lightManager->removeNodeFromDepthPass(bullet->getNode());
			}
			if (bullet->canBeRemoved()) {
				if (bullet->getParticleEmitter() != NULL) {
					lightManager->removeShadowFromNode(bullet->getParticleEmitter());
					lightManager->removeNodeFromDepthPass(bullet->getParticleEmitter());
				}
				if (bullet->getNode() != NULL) {
					lightManager->removeShadowFromNode(bullet->getNode());
					lightManager->removeNodeFromDepthPass(bullet->getNode());
				}
				bullet->dispose();
			}
		}
		for each(FK_Bullet* bullet in player2->getBulletsCollection()){
			if (!bullet->isDisposeable() && !bullet->hasLightAssigned()){
				shadowMode = bullet->hasShadow() ? shadowModeCharacters : E_SHADOW_MODE::ESM_EXCLUDE;
				lightManager->addShadowToNode(bullet->getNode(), shadowFilterType, shadowMode);
				lightManager->addNodeToDepthPass(bullet->getNode());
				if (bullet->getParticleEmitter() != NULL){
					lightManager->addNodeToDepthPass(bullet->getParticleEmitter());
				}
				bullet->assignShadowEffect();
			}
			if ((bullet->isDisposeable() || !(bullet->getNode()->isVisible())) && bullet->hasLightAssigned()){
				lightManager->removeShadowFromNode(bullet->getNode());
				lightManager->removeNodeFromDepthPass(bullet->getNode());
			}
			if (bullet->canBeRemoved()) {
				if (bullet->getParticleEmitter() != NULL) {
					lightManager->removeShadowFromNode(bullet->getParticleEmitter());
					lightManager->removeNodeFromDepthPass(bullet->getParticleEmitter());
				}
				if (bullet->getNode() != NULL) {
					lightManager->removeShadowFromNode(bullet->getNode());
					lightManager->removeNodeFromDepthPass(bullet->getNode());
				}
				bullet->dispose();
			}
		}
		/* dispose of expired bullets */
		// player 1
		player1->getBulletsCollection().erase(std::remove_if(
			player1->getBulletsCollection().begin(), player1->getBulletsCollection().end(), canEraseBullet),
			player1->getBulletsCollection().end());
		// player 2
		player2->getBulletsCollection().erase(std::remove_if(
			player2->getBulletsCollection().begin(), player2->getBulletsCollection().end(), canEraseBullet),
			player2->getBulletsCollection().end());
		//std::cout << "current bullets P1:" << player1->getBulletsCollection().size() << std::endl;
	}

	void FK_SceneGame::updateCharacterArenaInteraction(){

		f32 jumpGravityToSet = jumpGravity;
		f32 sceneGravityToSet = sceneGravity;
		f32 modifier = 0.4f;

		if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatMoonGravity) != 0) {
			jumpGravityToSet *= modifier;
			sceneGravityToSet *= modifier;
		}

		ISceneNodeAnimator* pAnim1 = *(player1->getAnimatedMesh()->getAnimators().begin());
		if (pAnim1->getType() == ESNAT_COLLISION_RESPONSE)
		{
			if (player1->isJumping() && !player1WasJumping){
				player1WasJumping = true;
				((scene::ISceneNodeAnimatorCollisionResponse*)pAnim1)->setGravity(core::vector3df(0, jumpGravityToSet, 0));
			}
			else if (!player1->isJumping() && player1WasJumping){
				player1WasJumping = false;
				((scene::ISceneNodeAnimatorCollisionResponse*)pAnim1)->setGravity(core::vector3df(0, sceneGravityToSet, 0));
			}
		}
		ISceneNodeAnimator* pAnim2 = *(player2->getAnimatedMesh()->getAnimators().begin());
		if (pAnim2->getType() == ESNAT_COLLISION_RESPONSE)
		{
			if (player2->isJumping() && !player2WasJumping){
				player2WasJumping = true;
				((scene::ISceneNodeAnimatorCollisionResponse*)pAnim2)->setGravity(core::vector3df(0, jumpGravityToSet, 0));
			}
			else if (!player2->isJumping() && player2WasJumping){
				player2WasJumping = false;
				((scene::ISceneNodeAnimatorCollisionResponse*)pAnim2)->setGravity(core::vector3df(0, sceneGravityToSet, 0));
			}
		}
		/*
		if (player1->getCollisionEllipsoidRefreshFlag()){
			ISceneNodeAnimator* pAnim1 = *(player1->getAnimatedMesh()->getAnimators().begin());
			if (pAnim1->getType() == ESNAT_COLLISION_RESPONSE)
			{
				// this function is currently faulty
				const core::aabbox3d<f32> &box = player1->getAnimatedMesh()->getTransformedBoundingBox();
				core::vector3df ellipse_radius = (box.MaxEdge - box.getCenter());
				core::vector3df ellipse_offset = -box.getCenter();
				scene::ISceneNodeAnimatorCollisionResponse* anim = (scene::ISceneNodeAnimatorCollisionResponse*)pAnim1;
				anim->setEllipsoidRadius(ellipse_radius);
				anim->setEllipsoidTranslation(ellipse_offset);
				player1->setCollisionEllipsoidRefreshFlag(false);
			}
		}
		if (player2->getCollisionEllipsoidRefreshFlag()){
			ISceneNodeAnimator* pAnim2 = *(player2->getAnimatedMesh()->getAnimators().begin());
			if (pAnim2->getType() == ESNAT_COLLISION_RESPONSE)
			{
				const core::aabbox3d<f32> &box = player2->getAnimatedMesh()->getTransformedBoundingBox();
				core::vector3df ellipse_radius = (box.MaxEdge - box.getCenter());
				core::vector3df ellipse_offset = -box.getCenter();
				scene::ISceneNodeAnimatorCollisionResponse* anim = (scene::ISceneNodeAnimatorCollisionResponse*)pAnim2;
				anim->setEllipsoidRadius(ellipse_radius);
				anim->setEllipsoidTranslation(ellipse_offset);
				player2->setCollisionEllipsoidRefreshFlag(false);
			}
		}
		*/
	};

	/* update lights */
	void FK_SceneGame::updateLights(){
		// update ambient and light color (if needed)
		lightManager->setAmbientColor(stage->getAmbientColor());
		lightManager->getShadowLight(0).setLightColor(stage->getLightColor());
		// update light position
		core::vector3df lightPosition = lightManager->getShadowLight(0).getPosition();
		core::vector3df middlePoint = (player1->getPosition() + player2->getPosition()) / 2;
		middlePoint.Y = max(player1->getPosition().Y, player2->getPosition().Y);
		if (processingWinText || (processingSceneEnd && !isGameOver)){
			f32 lightDistance = 50.0f;
			middlePoint = (player2->getPosition() - player1->getPosition());
			middlePoint.Y = 0;
			middlePoint = middlePoint.normalize();
			middlePoint = (winnerId == -1 ? 
				lightDistance*middlePoint + player1->getPosition() : 
				-lightDistance*middlePoint + player2->getPosition());
			middlePoint.Y = max(player1->getPosition().Y, player2->getPosition().Y);
		}
		if (middlePoint.Y < 0) middlePoint.Y = 0;
		lightManager->getShadowLight(0).setTarget(middlePoint);
		middlePoint.Y = lightPosition.Y;
		lightManager->getShadowLight(0).setPosition(middlePoint);
		/* check for broken objects (to remove the shadow node from them) */
		for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i){
			if (player1->getCharacterAdditionalObjects().at(i).applyShadow){
				if (!player1->getCharacterAdditionalObjects().at(i).isActive() &&
					!player1->getCharacterAdditionalObjects().at(i).lightRemoved){
					lightManager->removeShadowFromNode(player1->getCharacterAdditionalObjectsNodes().at(i));
					player1->removeLightFromObject(i);
					// fly away, if the correct flag is set
					if (player1->getCharacterAdditionalObjects().at(i).broken &&
						player1->getCharacterAdditionalObjects().at(i).flyAwayWhenBroken){
						if (player1->getCharacterAdditionalObjects().at(i).propMeshFilename.empty()){
							createBreakableObjectProp(
								0,
								player1->getCharacterAdditionalObjects().at(i),
								player1->getCharacterAdditionalObjectsNodes().at(i));
						}
						else{
							for each(std::string path in player1->getCharacterAdditionalObjects().at(i).propMeshFilename){
								createBreakableObjectProp(
									0,
									player1->getCharacterAdditionalObjects().at(i),
									player1->getCharacterAdditionalObjectsNodes().at(i),
									player1->getCharacterAdditionalObjects().at(i).meshPath + path
									);
							}
						}
					}
				}
				else if (player1->getCharacterAdditionalObjects().at(i).needsRefresh()){
					lightManager->addShadowToNode(player1->getCharacterAdditionalObjectsNodes().at(i), shadowFilterType,
						shadowModeCharacters);
					if (player1->getCharacterAdditionalObjectsNodes().at(i)->getMesh() != NULL){
						//player1->getCharacterAdditionalObjectsNodes().at(i)->setTransitionTime(0.f);
						player1->getCharacterAdditionalObjectsNodes().at(i)->setCurrentFrame(0, true);
						player1->getCharacterAdditionalObjectsNodes().at(i)->OnRegisterSceneNode();
						player1->getCharacterAdditionalObjectsNodes().at(i)->updateAbsolutePosition();
					}
					player1->refreshObject(i);
				}
			}
			// refresh frame even if the object has no applied shadow
			else if (player1->getCharacterAdditionalObjects().at(i).needsRefresh()){
				if (player1->getCharacterAdditionalObjectsNodes().at(i)->getMesh() != NULL){
					//player1->getCharacterAdditionalObjectsNodes().at(i)->setTransitionTime(0.f);
					player1->getCharacterAdditionalObjectsNodes().at(i)->setCurrentFrame(0, true);
					player1->getCharacterAdditionalObjectsNodes().at(i)->OnRegisterSceneNode();
					player1->getCharacterAdditionalObjectsNodes().at(i)->updateAbsolutePosition();
				}
				player1->refreshObject(i);
			}
		}
		for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
			if (player2->getCharacterAdditionalObjects().at(i).applyShadow){
				if (!player2->getCharacterAdditionalObjects().at(i).isActive() &&
					!player2->getCharacterAdditionalObjects().at(i).lightRemoved){
					lightManager->removeShadowFromNode(player2->getCharacterAdditionalObjectsNodes().at(i));
					player2->removeLightFromObject(i);
					// fly away, if the correct flag is set
					if (player2->getCharacterAdditionalObjects().at(i).broken &&
						player2->getCharacterAdditionalObjects().at(i).flyAwayWhenBroken){
						if (player2->getCharacterAdditionalObjects().at(i).propMeshFilename.empty()){
							createBreakableObjectProp(
								1,
								player2->getCharacterAdditionalObjects().at(i),
								player2->getCharacterAdditionalObjectsNodes().at(i));
						}
						else{
							for each(std::string path in player2->getCharacterAdditionalObjects().at(i).propMeshFilename){
								createBreakableObjectProp(
									1,
									player2->getCharacterAdditionalObjects().at(i),
									player2->getCharacterAdditionalObjectsNodes().at(i),
									player2->getCharacterAdditionalObjects().at(i).meshPath + path
									);
							}
						}
					}
				}
				else if (player2->getCharacterAdditionalObjects().at(i).needsRefresh()){
					lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i), shadowFilterType,
						shadowModeCharacters);
					if (player2->getCharacterAdditionalObjectsNodes().at(i)->getMesh() != NULL){
						player2->getCharacterAdditionalObjectsNodes().at(i)->setCurrentFrame(0, true);
						player2->getCharacterAdditionalObjectsNodes().at(i)->OnRegisterSceneNode();
						player2->getCharacterAdditionalObjectsNodes().at(i)->updateAbsolutePosition();
					}
					player2->refreshObject(i);
				}
			}
			// refresh frame even if the object has no applied shadow
			else if (player2->getCharacterAdditionalObjects().at(i).needsRefresh()){
				if (player2->getCharacterAdditionalObjectsNodes().at(i)->getMesh() != NULL){
					player2->getCharacterAdditionalObjectsNodes().at(i)->setCurrentFrame(0, true);
					player2->getCharacterAdditionalObjectsNodes().at(i)->OnRegisterSceneNode();
					player2->getCharacterAdditionalObjectsNodes().at(i)->updateAbsolutePosition();
				}
				player2->refreshObject(i);
			}
		}
	}

	/* additional objects & props */
	void FK_SceneGame::createBreakableObjectProp(
		s32 ownerId,
		FK_Character::FK_CharacterAdditionalObject objectToAttach, 
		IAnimatedMeshSceneNode* nodeToCopy,
		std::string meshFileName){
		IAnimatedMesh* mesh;
		if (meshFileName.empty()){
			mesh = nodeToCopy->getMesh();
		}
		else{
			mesh = smgr->getMesh(meshFileName.c_str());
		}
		IAnimatedMeshSceneNode* newNode = smgr->addAnimatedMeshSceneNode(mesh);
		newNode->setScale(nodeToCopy->getAbsoluteTransformation().getScale());
		newNode->setPosition(nodeToCopy->getAbsolutePosition());
		newNode->setRotation(nodeToCopy->getAbsoluteTransformation().getRotationDegrees());
		if (objectToAttach.applyShadow){
			lightManager->addShadowToNode(newNode, shadowFilterType,
				shadowModeCharacters);
		}
		newNode->setJointMode(irr::scene::EJUOR_CONTROL);
		//animatedMesh->setAnimationSpeed(animationFrameRate);
		//animatedMesh->setTransitionTime(animationTransitionTime);
		int mcount = newNode->getMaterialCount();
		for (int i = 0; i < mcount; i++){
			newNode->getMaterial(i).BackfaceCulling = false;
			newNode->getMaterial(i).setFlag(video::EMF_ZWRITE_ENABLE, true);
			newNode->getMaterial(i).MaterialType = nodeToCopy->getMaterial(i).MaterialType;
			if (getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatWireframe) {
				newNode->getMaterial(i).Wireframe = true;
			}
		}
		newNode->setMaterialFlag(video::EMF_LIGHTING, false);
		newNode->setVisible(true);
		scene::ITriangleSelector* selector = 0;
		if (stage->getStage()){
			selector = smgr->createOctreeTriangleSelector(
				stage->getStageMesh()->getMesh(0), stage->getStage(), 128);
			stage->getStage()->setTriangleSelector(selector);
		}
		FK_Character::FK_SceneProp* prop = new FK_Character::FK_SceneProp;
		prop->ownerId = ownerId;
		prop->object = objectToAttach;
		f32 randomRadialVelocity = 40.0f;
		f32 randomX = (f32)rand() / RAND_MAX;
		f32 randomZ = (f32)rand() / RAND_MAX;
		prop->velocity = core::vector3df(
			-randomRadialVelocity + randomX * 2 * randomRadialVelocity, 
			200.f, 
			-randomRadialVelocity + randomZ * 2 * randomRadialVelocity);

		f32 sceneGravityToSet = sceneGravity;
		f32 modifier = 0.4f;
		if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatMoonGravity) != 0) {
			sceneGravityToSet *= modifier;
		}

		if (selector)
		{
			core::vector3df bbx_extent = newNode->getTransformedBoundingBox().getExtent();
			core::vector3df ellipse_radius = bbx_extent * 2.f;
			scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
				selector, newNode, ellipse_radius,
				core::vector3df(0, sceneGravityToSet, 0), core::vector3df(0, 0, 0), 0.01);
			//anim->setCollisionCallback(player1CollisionCallback);
			FK_WorldCollisionCallbackProp* collisionCallback = new FK_WorldCollisionCallbackProp(newNode, prop);
			anim->setCollisionCallback(collisionCallback);
			brokenObjectCollisionCallbacks.push_back(collisionCallback);
			newNode->addAnimator(anim);
			anim->drop();
			f32 randomAdditionalAngle = 10.f * (f32)rand() / RAND_MAX;
			scene::ISceneNodeAnimator* anim2 = smgr->createRotationAnimator(core::vector3df(0.f, 0.f, 15.f + randomAdditionalAngle));
			newNode->addAnimator(anim2);
			anim2->drop();
			u32 size = newNode->getAnimators().size();
			u32 b = size;
		};
		selector->drop();
		brokenObjectsProps.push_back(
			std::pair<FK_Character::FK_SceneProp*, IAnimatedMeshSceneNode*>(prop, newNode));
	}

	void FK_SceneGame::updateProps(f32 delta_t_s){
		s32 objectToRemove = -1;
		if (!player1->isPickingUpObject()){
			player1->setPickableObjectId(std::string());
		}
		if (!player2->isPickingUpObject()){
			player2->setPickableObjectId(std::string());
		}
		int pickupFlag = 0;
		for (u32 i = 0; i < brokenObjectsProps.size(); ++i){
			brokenObjectsProps[i].second->animateJoints();
			if (!brokenObjectsProps[i].first->hasCollided){
				brokenObjectsProps[i].second->setPosition(
					brokenObjectsProps[i].second->getPosition() +
					brokenObjectsProps[i].first->velocity * delta_t_s
					);
			}
			if (brokenObjectsProps[i].first->mustSetPlayerCollision){
				brokenObjectsProps[i].first->mustSetPlayerCollision = false;
				if (brokenObjectsProps[i].first->object.disappearWhenHitGround){
					objectToRemove = (s32)i;
					break;
				}
				else{
					brokenObjectsProps[i].second->removeAnimators();
					if (brokenObjectsProps[i].first->object.rotateWhenTouchingGround){
						core::vector3df landingRotationMin = brokenObjectsProps[i].first->object.landingRotationAngleMin;
						core::vector3df landingRotationMax = brokenObjectsProps[i].first->object.landingRotationAngleMax;
						core::vector3df landingRotation = landingRotationMin;
						if (landingRotationMax.X != landingRotationMin.X){
							float randAng = (float)rand() / RAND_MAX;
							landingRotation.X = landingRotationMin.X + randAng * (landingRotationMax.X - landingRotationMin.X);
							//std::cout << "Ang. X: " << landingRotation.Y << std::endl;
						}
						if (landingRotationMax.Y != landingRotationMin.Y){
							float randAng = (float)rand() / RAND_MAX;
							landingRotation.Y = landingRotationMin.Y + randAng * (landingRotationMax.Y - landingRotationMin.Y);
							//std::cout << "Ang. Y: " << landingRotation.Y << std::endl;
						}
						if (landingRotationMax.Z != landingRotationMin.Z){
							float randAng = (float)rand() / RAND_MAX;
							landingRotation.Z = landingRotationMin.Z + randAng * (landingRotationMax.Z - landingRotationMin.Z);
							//std::cout << "Ang. Z: " << landingRotation.Z << std::endl;
						}
						brokenObjectsProps[i].second->setRotation(landingRotation);
					}
					core::vector3df gravityBoundingBoxOffset = brokenObjectsProps[i].first->object.landingPositionOffset;
					//scene::ITriangleSelector* selector = 0;
					//selector = smgr->createTriangleSelectorFromBoundingBox(
					//	player1->getAnimatedMesh());
					//player1->getAnimatedMesh()->setTriangleSelector(selector);
					core::vector3df bbx_extent = brokenObjectsProps[i].second->getTransformedBoundingBox().getExtent();
					core::vector3df ellipse_radius = bbx_extent;
					core::vector3df ellipse_center = core::vector3df(0.f, 0.f, 0.f);
					//scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
					//	selector, brokenObjectsProps[i].second, ellipse_radius,
					//	core::vector3df(0, 0, 0), gravityBoundingBoxOffset, 0.01);
					//brokenObjectsProps[i].second->addAnimator(anim);
					//anim->drop();
					//selector->drop();
					//selector = smgr->createTriangleSelectorFromBoundingBox(
					//	player2->getAnimatedMesh());
					//player2->getAnimatedMesh()->setTriangleSelector(selector);
					//anim = smgr->createCollisionResponseAnimator(
					//	selector, brokenObjectsProps[i].second, ellipse_radius,
					//	core::vector3df(0, 0, 0), gravityBoundingBoxOffset, 0.01);
					//brokenObjectsProps[i].second->addAnimator(anim);
					//selector->drop();
					core::vector3df newPosition = brokenObjectsProps[i].second->getPosition();
					newPosition -= gravityBoundingBoxOffset;
					brokenObjectsProps[i].second->setPosition(newPosition);
					gravityBoundingBoxOffset += ellipse_center;

					f32 sceneGravityToSet = sceneGravity;
					f32 modifier = 0.4f;
					if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatMoonGravity) != 0) {
						sceneGravityToSet *= modifier;
					}

					scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
						stage->getStage()->getTriangleSelector(), brokenObjectsProps[i].second, ellipse_radius,
						core::vector3df(0, sceneGravityToSet, 0), -gravityBoundingBoxOffset, 10);
					brokenObjectsProps[i].second->addAnimator(anim);
					anim->drop();
					if (stage->hasWalkingSandEffect()){
						core::vector3df sandPosition = brokenObjectsProps[i].second->getPosition();
						sandPosition.Y += gravityBoundingBoxOffset.Y;
						stage->showSandSplashEmitter(sandPosition, 0);
					}
				}
			}
			else if (brokenObjectsProps[i].first->hasCollided){
				if (brokenObjectsProps[i].first->object.canBePickedFromGround &&
					brokenObjectsProps[i].first->object.uniqueNameId != std::string()){
					FK_Character* playerWhoPicks = brokenObjectsProps[i].first->ownerId == 0 ? player1 : player2;
					if (playerWhoPicks->isPickingUpObject() && playerWhoPicks->getPickableObjectId() != std::string() &&
						playerWhoPicks->getPickableObjectId() == brokenObjectsProps[i].first->object.uniqueNameId){
						bool collisionFlag = checkNodeCollision(playerWhoPicks->getAnimatedMesh(), brokenObjectsProps[i].second);
						if (collisionFlag){
							if (playerWhoPicks->isPickingUpObject()){
								pickupFlag = brokenObjectsProps[i].first->ownerId == 0 ? -1 : 1;
								objectToRemove = i;
								break;
							}
						}
					}
					if (playerWhoPicks->getPickableObjectId() == std::string() &&
						!playerWhoPicks->getObjectByName(brokenObjectsProps[i].first->object.uniqueNameId)->isActive()){
						core::vector3df position = playerWhoPicks->getPosition();
						/*position.Y = 0;
						core::vector3df propPosition = brokenObjectsProps[i].second->getAbsolutePosition();
						propPosition.Y = 0;
						f32 distance = position.getDistanceFrom(propPosition);
						f32 distanceForPickUp = 15.0f;*/
						bool collisionFlag = checkNodeCollision(playerWhoPicks->getAnimatedMesh(), brokenObjectsProps[i].second);
						if (/*distance <= distanceForPickUp && */ collisionFlag){
							playerWhoPicks->setPickableObjectId(brokenObjectsProps[i].first->object.uniqueNameId);
							/*std::cout << playerWhoPicks->getDisplayName() << " "<<
								playerWhoPicks->getOutfitId() << " "<< playerWhoPicks->getPickableObjectId() << std::endl;*/
						}
					}
				}
			}
		}
		// remove objects which disappeared from the ground
		if (objectToRemove >= 0){
			if (pickupFlag == -1){
				if (player1->isPickingUpObject()){
					player1->resetPickingObjectFlag();
				}
			}
			else if (pickupFlag == 1){
				if (player2->isPickingUpObject()){
					player2->resetPickingObjectFlag();
				}
			}
			lightManager->removeShadowFromNode(brokenObjectsProps[objectToRemove].second);
			brokenObjectsProps[objectToRemove].second->removeAnimators();
			brokenObjectsProps[objectToRemove].second->remove();
			brokenObjectCollisionCallbacks[objectToRemove]->drop();
			delete brokenObjectsProps[objectToRemove].first;
			brokenObjectsProps.erase(brokenObjectsProps.begin() + objectToRemove);
			brokenObjectCollisionCallbacks.erase(brokenObjectCollisionCallbacks.begin() + objectToRemove);
		}
	}

	/* update stage effects */
	void FK_SceneGame::updateStage(u32 delta_t_ms){
		stage->update(delta_t_ms);
		// update splashing water effect
		//if (player1->getStance() == FK_Stance_Type::LandingStance){
		//	stage->showWaterSplashEmitter(player1->getPosition(), 0);
		//}
		//if (player2->getStance() == FK_Stance_Type::LandingStance){
		//	stage->showWaterSplashEmitter(player2->getPosition(), 1);
		//}
	}

	/* update the game camera position*/
	void FK_SceneGame::updateCamera(f32 delta_t_s){
		if (processingWinText || ((processingRecordText || processingSceneEnd) && !isGameOver)){
			cameraManager->updateVictory(winnerId);
		}
		else if (processingIntroStage){
			processingIntroStage = cameraManager->updateStageViewIntro(delta_t_s);
		}else if (processingIntroPlayer1){
			cameraManager->updateIntro(-1);
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatBobbleHead) != 0) {
				player1->allowAnimationTransitions();
			}
		}
		else if (processingIntroPlayer2){
			cameraManager->updateIntro(1);
			if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatBobbleHead) != 0) {
				player2->allowAnimationTransitions();
			}
		}
		else if (processingContinue || processingGameOver || processingSceneEnd || isGameOver){
			cameraManager->updateContinue(-winnerId);
		}
		else{
			f32 additionalDistance = 0.f;
			if (updateCounterAttackEffectFlag) {
				additionalDistance = databaseAccessor.getCounterAttackCameraZoomingDistance();
			}
			cameraManager->update(delta_t_s, additionalDistance);
		}

		// update little bobble head cheat code
		if ((getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatBobbleHead) != 0) {
			IBoneSceneNode* headBone =
				player1->getAnimatedMesh()->getJointNode(
					player1->getArmatureBone(FK_Bones_Type::HeadArmature).c_str());
			if (headBone) {
				headBone->setScale(headBone->getScale() * 2.5f);
			}
			headBone = NULL;
			for (u32 i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i) {
				auto object = player1->getCharacterAdditionalObjectsNodes().at(i);
				if (object != NULL && 
					player1->getCharacterAdditionalObjects().at(i).animateWithMesh ) {
					IBoneSceneNode* headBone =
						object->getJointNode(
							player1->getArmatureBone(FK_Bones_Type::HeadArmature).c_str());
					if (headBone) {
						headBone->setScale(headBone->getScale() * 2.5f);
					}
					headBone = NULL;
				}
			}
			headBone =
				player2->getAnimatedMesh()->getJointNode(
					player2->getArmatureBone(FK_Bones_Type::HeadArmature).c_str());
			if (headBone) {
				headBone->setScale(headBone->getScale() * 2.5f);
			}
			headBone = NULL;
			for (u32 i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i) {
				auto object = player2->getCharacterAdditionalObjectsNodes().at(i);
				if (object != NULL &&
					player2->getCharacterAdditionalObjects().at(i).animateWithMesh) {
					IBoneSceneNode* headBone =
						object->getJointNode(
							player2->getArmatureBone(FK_Bones_Type::HeadArmature).c_str());
					if (headBone) {
						headBone->setScale(headBone->getScale() * 2.5f);
					}
					headBone = NULL;
				}
			}
		}
	};

	/* update timer*/
	void FK_SceneGame::updateRoundTimer(u32 delta_t_ms){
		/* 99 seconds is the maximum round time. More than 99 = infinite timer */
		if (roundTimerMs <= fk_constants::FK_MaxRoundTimerSeconds * 1000){
			roundTimerMs -= delta_t_ms;
		}
		totalMatchTimeMs += delta_t_ms;
	}

	/* process additional game over routine, for example for saving survival records*/
	void FK_SceneGame::processAdditionalGameOverRoutine()
	{

	}

	/* check for new record - false unless in Arcade*/
	bool FK_SceneGame::checkForNewRecord()
	{
		return false;
	}

	/* setup continue */
	void FK_SceneGame::resetInitialCharacterPositions(){
		// remove animators to place players in the right places
		ISceneNodeAnimator* pAnim2 = *(player2->getAnimatedMesh()->getAnimators().begin());
		if (pAnim2->getType() == ESNAT_COLLISION_RESPONSE)
		{
			core::vector3df startingPosition = player2startingPosition;
			startingPosition.Y += (player2->getAnimatedMesh()->getScale().Y - 1.0f) * additionalYunitsPerScaleUnit;
			player2->getAnimatedMesh()->removeAnimator(pAnim2);
			player2->setPosition(startingPosition);
			player2->getAnimatedMesh()->updateAbsolutePosition();
		}
		player2->setRotation(core::vector3df(0, 180, 0));
		ISceneNodeAnimator* pAnim1 = *(player1->getAnimatedMesh()->getAnimators().begin());
		if (pAnim1->getType() == ESNAT_COLLISION_RESPONSE)
		{
			core::vector3df startingPosition = player1startingPosition;
			startingPosition.Y += (player1->getAnimatedMesh()->getScale().Y - 1.0f) * additionalYunitsPerScaleUnit;
			player1->getAnimatedMesh()->removeAnimator(pAnim1);
			player1->setPosition(startingPosition);
			player1->getAnimatedMesh()->updateAbsolutePosition();
		}
		player1->setRotation(core::vector3df(0, 0, 0));
		// rebuild collision animators
		setupCharacterArenaInteraction();
	}

	/* setup continue */
	void FK_SceneGame::setupContinue() {
		processingContinue = true;
		continueCounterMS = 0;
		continueCounterSeconds = 10;
		canPlayAnnouncerVoice = true;
		continueCounterDurationMS = 1500;
		processingContinueTextDurationMs = 1000;
		showContinueTextFlag = false;
		continuePressedButtons = 0;
		if (winnerId == 1) {
			player1->getAnimatedMesh()->setVisible(true);
			player2->getAnimatedMesh()->setVisible(false);
			lightManager->addShadowToNode(player1->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i) {
				if (player1->getCharacterAdditionalObjects().at(i).applyShadow) {
					if (player1->getCharacterAdditionalObjects().at(i).isActive()) {
						lightManager->addShadowToNode(player1->getCharacterAdditionalObjectsNodes().at(i),
							shadowFilterType, shadowModeCharacters);
					}
				}
			}
			lightManager->removeShadowFromNode(player2->getAnimatedMesh());
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i) {
				if (player2->getCharacterAdditionalObjects().at(i).applyShadow) {
					lightManager->removeShadowFromNode(player2->getCharacterAdditionalObjectsNodes().at(i));
				}
			}
			player1->setStance(FK_Stance_Type::SupineStance);
			player1->setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation, true);
		}
		else {
			player2->getAnimatedMesh()->setVisible(true);
			player1->getAnimatedMesh()->setVisible(false);
			lightManager->addShadowToNode(player2->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i) {
				if (player2->getCharacterAdditionalObjects().at(i).applyShadow) {
					if (player2->getCharacterAdditionalObjects().at(i).isActive()) {
						lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i),
							shadowFilterType, shadowModeCharacters);
					}
				}
			}
			lightManager->removeShadowFromNode(player1->getAnimatedMesh());
			for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i) {
				if (player1->getCharacterAdditionalObjects().at(i).applyShadow) {
					lightManager->removeShadowFromNode(player1->getCharacterAdditionalObjectsNodes().at(i));
				}
			}
			player2->setStance(FK_Stance_Type::SupineStance);
			player2->setBasicAnimation(FK_BasicPose_Type::GroundedSupineAnimation, true);
		}
		resetInitialCharacterPositions();
	}

	/* update continue */
	void FK_SceneGame::updateContinue(u32 delta_t_ms){
		if (!showContinueTextFlag){
			processingTimeCounterMs += delta_t_ms;
			if (processingTimeCounterMs > processingIntervalMs){
				showContinueTextFlag = true;
				processingTimeCounterMs = 0;
				if (!additionalOptions.allowContinue) {
					continueCounterSeconds = 0;
					processingGameOver = true;
					isGameOver = true;
					processingContinue = false;
					canPlayAnnouncerVoice = true;
					processingTimeCounterMs = 0;
					processAdditionalGameOverRoutine();
				}
			}
		}
		else{
			if (canPlayAnnouncerVoice){
				canPlayAnnouncerVoice = false;
				if (additionalOptions.allowContinue){
					announcer->playSound("continue", 100.0 * gameOptions->getSFXVolume());
				}
			}
			continueCounterMS += delta_t_ms;
			if (continueCounterSeconds < 0){
				processingTimeCounterMs += delta_t_ms;
				if (processingTimeCounterMs > processingContinueTextDurationMs){
					processingGameOver = true;
					isGameOver = true;
					processingContinue = false;
					canPlayAnnouncerVoice = true;
					processingTimeCounterMs = 0;
					processAdditionalGameOverRoutine();
				}
			}
			else{
				// update continue input
				if (continueCounterSeconds < 10 && continueCounterSeconds >= 0){
					updateContinueInput();
				}
				if (continueCounterMS > continueCounterDurationMS){
					continueCounterSeconds -= 1;
					if (continueCounterSeconds > 0){
						battleSoundManager->playSound("continue" + std::to_string(continueCounterSeconds), 100.0 * gameOptions->getSFXVolume());
					}
					continueCounterMS = 0;
				}
			}
		}
	}

	/* update input during continue scene */
	void FK_SceneGame::updateContinueInput(){
		u32 pressedButtons = 0;
		if (winnerId == 1){
			if (joystickInfo.size() > 0){
				player1input->update(nowReal, inputReceiver->JoypadStatus(0), false);
			}
			else{
				player1input->update(nowReal, inputReceiver->KeyboardStatus(), false);
			}
			pressedButtons = player1input->getPressedButtons();
		}
		else{
			if (joystickInfo.size() > 1){
				player2input->update(nowReal, inputReceiver->JoypadStatus(1), false);
			}
			else{
				player2input->update(nowReal, inputReceiver->KeyboardStatus(), false);
			}
			pressedButtons = player2input->getPressedButtons();
		}
		if (continuePressedButtons != pressedButtons){
			continuePressedButtons = pressedButtons;
			if (((pressedButtons & (u32)FK_Input_Buttons::Confirmation_Button) != 0) ||
				((pressedButtons & (u32)FK_Input_Buttons::Selection_Button) != 0) ||
				((pressedButtons & (u32)FK_Input_Buttons::Guard_Button) != 0)){
				battleSoundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
				std::string key1 = player1->getOriginalName();
				std::replace(key1.begin(), key1.end(), ' ', '_');
				std::string key2 = player2->getOriginalName();
				std::replace(key2.begin(), key2.end(), ' ', '_');
				if (player1isAI){
					characterStatisticsStructure[key2].numberOfContinues += 1;
				}
				else{
					characterStatisticsStructure[key1].numberOfContinues += 1;
				}
				restartMatch();
				return;
			}
			else if ((pressedButtons & (u32)FK_Input_Buttons::Any_MenuButton) != 0 ||
				(pressedButtons & (u32)FK_Input_Buttons::Any_NonDirectionButton) != 0){
				continueCounterMS = continueCounterDurationMS;
			}
		}
	}

	/* this method must be implemented in a subclass*/
	void FK_SceneGame::processEndOfRoundStatistics(){

	}

	/* update game states*/
	void FK_SceneGame::updateGameStates(u32 delta_t_ms){
		/* check if intros should be played */
		if (processingIntroPlayer1 && player1->getAnimatedMesh()->getFrameNr() == player1->getAnimatedMesh()->getEndFrame()){
			processingIntroPlayer1 = false;
		}
		if (processingIntroPlayer2 && player2->getAnimatedMesh()->getFrameNr() == player2->getAnimatedMesh()->getEndFrame()){
			processingIntroPlayer2 = false;
		}
		// update stage introduction
		if (introPhaseCounter == 0){
			processingIntroStage = true;
			player1->getAnimatedMesh()->setVisible(false);
			//lightManager->removeShadowFromNode(player1->getAnimatedMesh());
			player2->getAnimatedMesh()->setVisible(false);
			//lightManager->removeShadowFromNode(player2->getAnimatedMesh());
			introPhaseCounter += 1;
		}
		// player 1 introduction
		if (introPhaseCounter == 1 && !processingIntroStage){
			resetRound();
			player1->setStance(FK_Stance_Type::NoStance);
			player1->stopAnimationTransitions();
			player1->setBasicAnimation(FK_BasicPose_Type::IntroAnimation, true);
			player2->getAnimatedMesh()->setVisible(false);
			lightManager->addShadowToNode(player1->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
				if (player2->getCharacterAdditionalObjects().at(i).isActive() &&
					player2->getCharacterAdditionalObjects().at(i).applyShadow){
					lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i),
						shadowFilterType, shadowModeCharacters);
				}
			}
			lightManager->removeShadowFromNode(player2->getAnimatedMesh());
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
				if (player2->getCharacterAdditionalObjects().at(i).isActive() &&
					player2->getCharacterAdditionalObjects().at(i).applyShadow){
					lightManager->removeShadowFromNode(player2->getCharacterAdditionalObjectsNodes().at(i));
				}
			}
			processingIntroPlayer1 = true;
			introPhaseCounter += 1;
		}
		// player 2 introduction
		else if (introPhaseCounter == 2 && !processingIntroPlayer1){
			resetRound();
			player2->setStance(FK_Stance_Type::NoStance);
			player2->stopAnimationTransitions();
			player2->setBasicAnimation(FK_BasicPose_Type::IntroAnimation, true);
			player1->getAnimatedMesh()->setVisible(false);
			lightManager->removeShadowFromNode(player1->getAnimatedMesh());
			for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i){
				if (player1->getCharacterAdditionalObjects().at(i).isActive() &&
					player1->getCharacterAdditionalObjects().at(i).applyShadow){
					lightManager->removeShadowFromNode(player1->getCharacterAdditionalObjectsNodes().at(i));
				}
			}
			lightManager->addShadowToNode(player2->getAnimatedMesh(), shadowFilterType, shadowModeCharacters);
			for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
				if (player2->getCharacterAdditionalObjects().at(i).isActive() &&
					player2->getCharacterAdditionalObjects().at(i).applyShadow){
					lightManager->addShadowToNode(player2->getCharacterAdditionalObjectsNodes().at(i),
						shadowFilterType, shadowModeCharacters);
				}
			}
			processingIntroPlayer2 = true;
			introPhaseCounter += 1;
		}
		// reset variables for starting the match
		if (introPhaseCounter == 3 && !processingIntroPlayer2){
			resetRound();
			introPhaseCounter += 1;
			player1->allowAnimationTransitions();
			player2->allowAnimationTransitions();
		}
		if (isProcessingIntro()){
			return;
		}
		/* check if player is KO */
		if (isEndOfRound() && !processingEndOfRound){
			processingEndOfRound = true;
			processingKOText = true;
			canPlayAnnouncerVoice = true;
			processingTimeCounterMs = 0;
			winnerId = getRoundWinner(true);
			matchStatistics.setRoundResult(winnerId, player1, player2);
			processEndOfRoundStatistics();
		}
		/* update time if continue is not being processed */
		if (!processingContinue){
			processingTimeCounterMs += delta_t_ms;
		}
		/* start new round */
		if (newRoundFlag){
			resetRound();
			processNewRoundStart();
		}
		if (processingRoundText){
			if (processingTimeCounterMs > processingDurationMs + processingIntervalMs){
				processingRoundText = false;
				processingFightText = true;
				canPlayAnnouncerVoice = true;
				//announcer->playSound("fight", 100.0 * gameOptions->getSFXVolume());
				processingTimeCounterMs = 0;
			}
		}
		if (processingFightText){
			if (processingTimeCounterMs > processingDurationMs + processingIntervalMs){
				processingFightText = false;
				processingTimeCounterMs = 0;
			}
		}
		if (processingKOText){
			if (processingTimeCounterMs > processingKOTextDurationMs){
				processRoundEnd();
			}
		}
		if (processingContinue){
			updateContinue(delta_t_ms);
		}
		if (processingGameOver){
			if (processingTimeCounterMs > processingDurationMs + processingIntervalMs){
				processingGameOver = false;
				processingTimeCounterMs = 0;
				if (checkForNewRecord()) {
					canPlayAnnouncerVoice = true;
					processingRecordText = true;
				}
				else {
					processingSceneEnd = true;
					abortMatch = true;
					setNextScene(FK_SceneType::SceneMainMenu);
				}
			}
		}
		if (processingWinText){
			if (processingTimeCounterMs > processingWinTextDurationMs){
				processingWinText = false;
				processingTimeCounterMs = 0;
				if (checkForNewRecord()) {
					canPlayAnnouncerVoice = true;
					processingRecordText = true;
				}else{
					if (isEndOfMatch()){
						//int winnerId = getMatchWinner();
						if (winnerId != 0){
							processMatchEnd();
						}
						else{
							newRoundFlag = true;
						}
					}
					else {
						newRoundFlag = true;
					}
				}
			}
		}
		if (processingRecordText) {
			if (processingTimeCounterMs > processingWinTextDurationMs) {
				processingRecordText = false;
				processingTimeCounterMs = 0;
				if (isGameOver) {
					processingSceneEnd = true;
					abortMatch = true;
					setNextScene(FK_SceneType::SceneMainMenu);
				}
				else if (isEndOfMatch()) {
					//int winnerId = getMatchWinner();
					if (winnerId != 0) {
						processMatchEnd();
					}
					else {
						newRoundFlag = true;
					}
				}
				else {
					newRoundFlag = true;
				}
			}
		}
	};
	/* check if HUD has to be shown */
	bool FK_SceneGame::showHUD(){
		return (!processingContinue &&
			!processingGameOver &&
			!processingSceneEnd &&
			!processingRecordText &&
			!(isProcessingIntro() ||
			(processingWinText && winnerId != 0))
			);
	}
	/* check if HUD should show the round number*/
	bool FK_SceneGame::showRoundText(){
		return (processingRoundText && processingTimeCounterMs > processingIntervalMs);
	};
	/* check if HUD should show the fight flag*/
	bool FK_SceneGame::showFightText(){
		return (processingFightText && processingTimeCounterMs > processingIntervalMs);
	};
	/* check if HUD should show the KO flag*/
	bool FK_SceneGame::showKOText(){
		return (processingKOText);
	};
	/* check if HUD should show the win flag*/
	bool FK_SceneGame::showWinText(){
		return (processingWinText && processingTimeCounterMs > processingIntervalMs);
	};
	/* check if HUD should show the new record flag*/
	bool FK_SceneGame::showNewRecordText() {
		return (processingRecordText && processingTimeCounterMs > processingIntervalMs);
	};
	/* check if HUD should show the continue flag*/
	bool FK_SceneGame::showContinueText(){
		return (processingContinue && showContinueTextFlag);
	};

	/* check if HUD should show the game over flag */
	bool FK_SceneGame::showGameOverText(){
		return ((isGameOver) && processingTimeCounterMs > processingIntervalMs);
	}

	/* check if intro is being processed */
	bool FK_SceneGame::isProcessingIntro(){
		return introPhaseCounter < 4;
	}

	/* check if player can receive input */
	bool FK_SceneGame::canUpdateInput(){
		if (processingRoundText || processingSceneEnd || 
			processingFightText || processingWinText || 
			processingRecordText ||
			isProcessingIntro() || isTimeUp()){
			return false;
		}
		if (isProcessingThrow()){
			return false;
		}
		if (processingContinue || processingGameOver){
			return false;
		}
		return true;
	}

	/* check if round timer can be updated */
	bool FK_SceneGame::canUpdateRoundTimer(){
		if (processingRoundText || processingFightText || processingWinText || 
			processingEndOfRound || processingKOText || isProcessingIntro() ||
			processingRecordText ||
			processingContinue || processingGameOver){
			return false;
		}
		return true;
	}

	/* check if the time is up*/
	bool FK_SceneGame::isTimeUp(){
		return roundTimerMs < 0;
	}

	/* produce proper win text line */
	std::string FK_SceneGame::getEndOfRoundSoundIdentifier(const s32& matchWinnerId, const bool& perfectFlag) {
		std::string soundIdentifier = perfectFlag ? "perfect" : "ko";
		return soundIdentifier;
	}

	/* draw K.O. text */
	void FK_SceneGame::drawKOtext(const s32& matchWinnerId, const bool&perfectFlag) {
		hudManager->drawKO(perfectFlag);
	}

	/* update hud manager and draw HUD*/
	void FK_SceneGame::drawHUD(f32 frameDeltaTime_s){

		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());
		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
		driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());
		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

		if (showHUD()){
			u32 numberOfRoundIndicators1 = numberOfRoundsPlayer1;
			u32 numberOfRoundIndicators2 = numberOfRoundsPlayer2;
			if (!showRoundIndicatorsHUD()){
				numberOfRoundIndicators1 = 0;
				numberOfRoundIndicators2 = 0;
			}
			u32 triggerRegenPlayer1Ms = databaseAccessor.getTriggerRegenTimeMs();
			u32 triggerRegenPlayer2Ms = triggerRegenPlayer1Ms;
			if (triggerRegenPlayer1Ms < 0 || additionalOptions.triggerRegenerationTimeMSPlayer1 > 0) {
				triggerRegenPlayer1Ms = additionalOptions.triggerRegenerationTimeMSPlayer1;
			}
			if (triggerRegenPlayer2Ms < 0 || additionalOptions.triggerRegenerationTimeMSPlayer2 > 0) {
				triggerRegenPlayer2Ms = additionalOptions.triggerRegenerationTimeMSPlayer1;
			}
			hudManager->drawTriggerRecoveryBar(triggerRegenCounterMSPlayer1, triggerRegenPlayer1Ms,
				triggerRegenCounterMSPlayer2, triggerRegenPlayer2Ms, now);
			hudManager->drawHUD(
				numberOfRoundIndicators1, numberOfRoundIndicators2, numberOfPlayer1Wins, numberOfPlayer2Wins,
				roundTimerMs / 1000,
				isPaused(), frameDeltaTime_s);
		}
		// show intro quote
		if (processingIntroStage){
			hudManager->drawStageIntroText(stage);
		}
		if (processingIntroPlayer1){
			hudManager->drawIntroText(-1);
		}
		// show player2 intro quote
		if (processingIntroPlayer2){
			hudManager->drawIntroText(1);
		}
		// draw additional HUD elements
		drawAdditionalHUDItems(frameDeltaTime_s);
		if (showRoundText()){
			// show [ROUND #NUM] text and play voice
			if (canPlayAnnouncerVoice){
				canPlayAnnouncerVoice = false;
				
				if (isFinalRound()){
					announcer->playSound("round_final", 100.0 * gameOptions->getSFXVolume(),true);
				}
				else{
					char buffer[30];
					sprintf_s(buffer, "round_%i", currentRoundNumber);
					announcer->playSound(std::string(buffer), 100.0 * gameOptions->getSFXVolume(),true);
				}
			}
			hudManager->drawRound(currentRoundNumber, isFinalRound());
		}
		else if (showFightText()){
			// show [FIGHT] text and play voice
			if (canPlayAnnouncerVoice){
				canPlayAnnouncerVoice = false;
				announcer->playSound("fight", 100.0 * gameOptions->getSFXVolume());
				if (matchStartingTime < 0){
					matchStartingTime = now;
				}
			}
			hudManager->drawFightText();
		}
		else if (showKOText()){
			if (isTimeUp()){
				// show [TIME UP] text and play voice
				if (canPlayAnnouncerVoice){
					canPlayAnnouncerVoice = false;
					announcer->playSound("time_up", 100.0 * gameOptions->getSFXVolume());
				}
				hudManager->drawTimeUp();
			}
			else{
				if (winnerId != 0){
					std::string key1 = player1->getOriginalName();
					std::replace(key1.begin(), key1.end(), ' ', '_');
					std::string key2 = player2->getOriginalName();
					std::replace(key2.begin(), key2.end(), ' ', '_');
					// show [KO] or [PERFECT] text and play voice
					bool perfectFlag = (winnerId == -1 ? player1 : player2)->hasFullLife();
					// or [RING OUT], if applicable
					if (player1ringoutFlag || player2ringoutFlag){
						if (canPlayAnnouncerVoice){
							canPlayAnnouncerVoice = false;
							announcer->playSound("ringout", 100.0 * gameOptions->getSFXVolume());
							matchStatistics.markRingout(winnerId);
							if (winnerId == -1){
								if (!player1isAI){
									characterStatisticsStructure[key1].numberOfRingoutPerformed += 1;
									characterStatisticsStructure[key1].VsRecords[key2].numberOfRingoutPerformed += 1;
								}
								if (!player2isAI){
									characterStatisticsStructure[key2].numberOfRingoutSuffered += 1;
									characterStatisticsStructure[key2].VsRecords[key1].numberOfRingoutSuffered += 1;
								}
							}
							else{
								if (!player2isAI){
									characterStatisticsStructure[key2].numberOfRingoutPerformed += 1;
									characterStatisticsStructure[key2].VsRecords[key1].numberOfRingoutPerformed += 1;
								}
								if (!player1isAI){
									characterStatisticsStructure[key1].numberOfRingoutSuffered += 1;
									characterStatisticsStructure[key1].VsRecords[key2].numberOfRingoutSuffered += 1;
								}
							}
						}
						hudManager->drawRingout();
					}
					else{
						std::string soundIdentifier = getEndOfRoundSoundIdentifier(winnerId, perfectFlag);
						if (canPlayAnnouncerVoice) {
							canPlayAnnouncerVoice = false;
							announcer->playSound(soundIdentifier, 100.0 * gameOptions->getSFXVolume());
							if (perfectFlag) {
								matchStatistics.markPerfect(winnerId);
								if (winnerId == -1) {
									if (!player1isAI) {
										characterStatisticsStructure[key1].numberOfPerfectWins += 1;
										characterStatisticsStructure[key1].VsRecords[key2].numberOfPerfectWins += 1;
									}
									if (!player2isAI) {
										characterStatisticsStructure[key2].numberOfPerfectLosses += 1;
										characterStatisticsStructure[key2].VsRecords[key1].numberOfPerfectLosses += 1;
									}
								}
								else {
									if (!player2isAI) {
										characterStatisticsStructure[key2].numberOfPerfectWins += 1;
										characterStatisticsStructure[key2].VsRecords[key1].numberOfPerfectWins += 1;
									}
									if (!player1isAI) {
										characterStatisticsStructure[key1].numberOfPerfectLosses += 1;
										characterStatisticsStructure[key1].VsRecords[key2].numberOfPerfectLosses += 1;
									}
								}
							}
						}
						drawKOtext(winnerId, perfectFlag);
						/*if (successFlag) {
							hudManager->drawSuccessText();
						}
						else {
							hudManager->drawKO(perfectFlag);
						}*/
					}
				}
				else{
					if (player1ringoutFlag && player2ringoutFlag){
						// show [DOUBLE RING OUT] text and play voice
						if (canPlayAnnouncerVoice){
							canPlayAnnouncerVoice = false;
							announcer->playSound("double_ringout", 100.0 * gameOptions->getSFXVolume());
						}
						hudManager->drawDoubleRingout();
					}
					else{
						// show [DOUBLE KO] text and play voice
						if (canPlayAnnouncerVoice){
							canPlayAnnouncerVoice = false;
							announcer->playSound("double_ko", 100.0 * gameOptions->getSFXVolume());
						}
						hudManager->drawDoubleKO();
					}
				}
			}
		}
		else if (showContinueText()){
			// show [CONTINUE] text and play voice
			hudManager->drawContinue(continueCounterSeconds);
		}
		else if (showNewRecordText()) {
			if (canPlayAnnouncerVoice) {
				canPlayAnnouncerVoice = false;
				announcer->playSound("new_record", 100.0 * gameOptions->getSFXVolume());
			}
			hudManager->drawNewRecord();
		}
		else if (showGameOverText()){
			if (canPlayAnnouncerVoice){
				canPlayAnnouncerVoice = false;
				announcer->playSound("game_over", 100.0 * gameOptions->getSFXVolume());
			}
			hudManager->drawGameOver();
		}
		else if (showWinText()){
			if (winnerId != 0){
				// show [WIN] text and play voice
				if (canPlayAnnouncerVoice){
					canPlayAnnouncerVoice = false;
					announcer->playSound(winnerId == -1 ? "player1_wins" : "player2_wins", 100.0 * gameOptions->getSFXVolume());
				}
				hudManager->drawWin(winnerId, isEndOfMatch());
			}
			else{
				if (isEndOfMatch() || isTimeUp()){
					// show [DRAW] text and play voice
					if (canPlayAnnouncerVoice){
						canPlayAnnouncerVoice = false;
						announcer->playSound("draw", 100.0 * gameOptions->getSFXVolume());
					}
					hudManager->drawDraw();
				}
			}
		}

		// draw end overlay if needed
		drawAdditionalHUDOverlay(frameDeltaTime_s);

		// Restore projection, world, and view matrices
		driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
		driver->setTransform(irr::video::ETS_WORLD, oldWorldMat);
	}

	/* workaround fix to prevent issues in Trial mode*/
	void FK_SceneGame::drawHUDThrowName(FK_Character* performer) {
		
	}

	/* check for hit collisions and set reactions */
	void FK_SceneGame::updateCollisions(){
		FK_Reaction_Type reaction_pl1 = FK_Reaction_Type::NoReaction;
		FK_Reaction_Type reaction_pl2 = FK_Reaction_Type::NoReaction;
		f32 hitstunMultiplier1, hitstunMultiplier2;
		f32 hitDirection1 = 0, hitDirection2 = 0;
		bool hitPl1 = false;
		bool hitPl2 = false;
		bool guardBreakTriggerPl1 = false;
		bool guardBreakTriggerPl2 = false;
		player1ThrowAttemptMove = NULL;
		player2ThrowAttemptMove = NULL;
		skipThrowCheckPlayer1 = false;
		skipThrowCheckPlayer2 = false;
		bool formerTriggerPlayer1 = player1->isTriggerModeActive();
		bool formerTriggerPlayer2 = player2->isTriggerModeActive();
		bool formerGuardPlayer1 = player2->isGuarding();
		bool formerGuardPlayer2 = player2->isGuarding();
		player1->setThrowingOpponentFlag(false);
		player2->setThrowingOpponentFlag(false);

		if (player2->isBeingThrown()) {
			hitPl1 = false;
		}else{
			hitPl1 = checkDamageCollision(player2, player1, reaction_pl1, guardBreakTriggerPl1, hitDirection1, hitstunMultiplier1, player2ThrowAttemptMove);
		}

		if (player1->isBeingThrown()) {
			hitPl2 = false;
		}
		else {
			hitPl2 = checkDamageCollision(player1, player2, reaction_pl2, guardBreakTriggerPl2, hitDirection2, hitstunMultiplier2, player1ThrowAttemptMove);
		}
		// meter gain
		updateTriggerRegeneration(hitPl1, hitPl2);

		//// stop throws in case of simultaneous hit
		if (hitPl1 && player2ThrowAttemptMove == NULL) {
			player1ThrowAttemptMove = NULL;
		}
		if (hitPl2 && player1ThrowAttemptMove == NULL) {
			player2ThrowAttemptMove = NULL;
		}
		// check throws
		if (hitPl2){
			player2->setReaction(reaction_pl2, guardBreakTriggerPl2,hitDirection2, hitstunMultiplier2);
			
			if (player2->isGuarding() && player2->isTouchingWall() && 
				!player1->isHitStun() && player1->getCurrentMove() != NULL) {
				player1->activatePushback();
			}

			if (player2->hasActiveTransformation()) {
				std::string playerTag = "p2_";
				playerTag += player2->getCurrentTransformationId() + "_";
				if (!playHitSounds(reaction_pl2, playerTag, player2)){
					playHitSounds(reaction_pl2, "p2_", player2);
				}
			}
			else {
				playHitSounds(reaction_pl2, "p2_", player2);
			}

			if (player1ThrowAttemptMove != NULL && (formerTriggerPlayer1 || hitDirection2 > 0 ||
				((player1ThrowAttemptMove->getThrowEscapeInput() == (u32)FK_Input_Buttons::None ||
				player2->getStance() == FK_Stance_Type::AirStance))) ||
				player2->isKO()
				/* || player2->isGuarding()*/){
				skipThrowCheckPlayer1 = true;
			}
			if (!formerGuardPlayer2 && !player2->isGuarding() && !player2->isKO()){
				setupImpactEffect(reaction_pl2, 2);
				if (!hitPl1 && reaction_pl2 == FK_Reaction_Type::StrongFlight){
					player1->allowForImpactCancelCheck();
				}
			}
			if (player1isAI && player1ThrowAttemptMove == NULL){
				player1AIManager->resetSufferedThrowsCounter();
			}
		};
		if (hitPl1){
			player1->setReaction(reaction_pl1, guardBreakTriggerPl1, hitDirection1, hitstunMultiplier1);

			if (player1->isGuarding() && player1->isTouchingWall() &&
				!player2->isHitStun() && player2->getCurrentMove() != NULL) {
				player2->activatePushback();
			}

			if (player1->hasActiveTransformation()) {
				std::string playerTag = "p1_";
				playerTag += player1->getCurrentTransformationId() + "_";
				if (!playHitSounds(reaction_pl1, playerTag, player1)) {
					playHitSounds(reaction_pl1, "p1_", player1);
				}
			}
			else {
				playHitSounds(reaction_pl1, "p1_", player1);
			}

			if (player2ThrowAttemptMove != NULL && (formerTriggerPlayer2 || hitDirection1 > 0 ||
				(player2ThrowAttemptMove->getThrowEscapeInput() == (u32)FK_Input_Buttons::None ||
				player1->getStance() == FK_Stance_Type::AirStance)) ||
				player1->isKO()
				/* || player1->isGuarding()*/){
				skipThrowCheckPlayer2 = true;
			}
			if (!formerGuardPlayer1 && !player1->isGuarding() && !player1->isKO()){
				setupImpactEffect(reaction_pl1, 1);
				if (!hitPl2 && reaction_pl1 == FK_Reaction_Type::StrongFlight){
					player2->allowForImpactCancelCheck();
				}
			}
			if (player2isAI && player2ThrowAttemptMove == NULL){
				player2AIManager->resetSufferedThrowsCounter();
			}
		};
		if (skipThrowCheckPlayer1 && skipThrowCheckPlayer2){
			skipThrowCheckPlayer1 = false;
			skipThrowCheckPlayer2 = false;
		}
		else if (skipThrowCheckPlayer1){
			player2ThrowAttemptMove = NULL;
			// update move name (must be done because of how throws work)
			player1->cancelJump(true);
			player2->cancelJump();
			drawHUDThrowName(player1);
			cameraManager->alignCharacters(player2);
			applySuccessfulThrow();
		}
		else if (skipThrowCheckPlayer2){
			player1ThrowAttemptMove = NULL;
			// update move name (must be done because of how throws work)
			player1->cancelJump();
			player2->cancelJump(true);
			//hudManager->drawMoveName(player2);
			drawHUDThrowName(player2);
			cameraManager->alignCharacters(player1);
		    applySuccessfulThrow();
		}
		if (player1ThrowAttemptMove != NULL && player2ThrowAttemptMove != NULL){
			AI1_defenseCheckAgainstThrowPerformed = false;
			AI2_defenseCheckAgainstThrowPerformed = false;
			player1->setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
			player2->setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
			player1->resetNextMovement();
			player2->resetNextMovement();
			cancelThrow();
		}
		else if (player1ThrowAttemptMove != NULL){
			player1->setThrowProcessingFlag(true);
			if (hitPl1){
				resetThrowAttempt();
			}
			else{
				AI1_defenseCheckAgainstThrowPerformed = false;
				AI2_defenseCheckAgainstThrowPerformed = false;
				player1->setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
				player2->setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
				player2->resetNextMovement();
				player2->resetMove();
			}
		}
		else if (player2ThrowAttemptMove != NULL){
			player2->setThrowProcessingFlag(true);
			if (hitPl2){
				resetThrowAttempt();
			}
			else{
				AI1_defenseCheckAgainstThrowPerformed = false;
				AI2_defenseCheckAgainstThrowPerformed = false;
				player1->setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
				player2->setVelocityPerSecond(core::vector3df(0.f, 0.f, 0.f));
				player1->resetNextMovement();
				player1->resetMove();
			}
		}
	};

	/* check for bullet collisions and set reactions */
	void FK_SceneGame::updateBulletCollisions(){
		FK_Reaction_Type reaction_pl1 = FK_Reaction_Type::NoReaction;
		FK_Reaction_Type reaction_pl2 = FK_Reaction_Type::NoReaction;
		f32 hitstunMultiplier1, hitstunMultiplier2;
		f32 hitDirection1 = 0, hitDirection2 = 0;
		bool hitPl1 = false;
		bool hitPl2 = false;
		for each(FK_Bullet* bullet in player1->getBulletsCollection()){
			/* check if bullet is already disposeable */
			if (bullet->canBeRemoved() || bullet->isDisposeable() || !bullet->getNode()->isVisible()){
				continue;
			}
			if (bullet->getHitboxNode() == NULL) {
				continue;
			}
			/* check collisions between bullets*/
			for each(FK_Bullet* target_bullet in player2->getBulletsCollection()){
				if (bullet->canBeRemoved() || target_bullet->isDisposeable() || !target_bullet->getNode()->isVisible()){
					continue;
				}
				bullet->getHitboxNode()->updateAbsolutePosition();
				target_bullet->getHitboxNode()->updateAbsolutePosition();
				if (target_bullet->getHitboxNode() == NULL) {
					continue;
				}
				if (checkNodeCollision(bullet->getHitboxNode(), target_bullet->getHitboxNode())){
					int newPriority = bullet->getHitbox().getPriority();
					int newPriorityTarget = target_bullet->getHitbox().getPriority();
					battleSoundManager->playSound("guard", 100.0 * gameOptions->getSFXVolume());
					if (bullet->getHitbox().getPriority() <= target_bullet->getHitbox().getPriority()){
						bullet->getHitbox().setHit(true);
						player1->activateParticleEffect(bullet->getPosition());
						newPriorityTarget -= 1;
					}
					if (target_bullet->getHitbox().getPriority() <= bullet->getHitbox().getPriority()){
						target_bullet->getHitbox().setHit(true);
						player2->activateParticleEffect(target_bullet->getPosition());
						newPriority -= 1;
					}
					if (target_bullet->getHitbox().getPriority() == bullet->getHitbox().getPriority()) {
						// create a nice impact effect
						impactEffectCounter = 0;
					}
					bullet->getHitbox().setPriority(newPriority);
					target_bullet->getHitbox().setPriority(newPriorityTarget);
				}
			}
			/* continue if the bullet has hit an opponent bullet */
			if (bullet->canBeRemoved() || bullet->isDisposeable() || !bullet->getNode()->isVisible()){
				continue;
			}
			// then, check damage (if throw is not being processed)
			if (!isProcessingThrow() && !player1->isKO()){
				
				hitPl2 = checkBulletDamageCollision(bullet, player1, player2, reaction_pl2, hitDirection2, hitstunMultiplier2);
				if (hitPl2){
					break;
				}
			}
		}
		// it is not necessary to check the collision between bullets two times...
		for each(FK_Bullet* bullet in player2->getBulletsCollection()){
			/* check if bullet is already disposeable */
			if (bullet->canBeRemoved() || bullet->isDisposeable() || !bullet->getNode()->isVisible()){
				continue;
			}
			if (!isProcessingThrow() && !player2->isKO()){
				// then, check damage
				hitPl1 = checkBulletDamageCollision(bullet, player2, player1, reaction_pl1, hitDirection1, hitstunMultiplier1);
				if (hitPl1){
					break;
				}
			}
		}
		if (hitPl2){
			player2->setReaction(reaction_pl2, false, hitDirection2, hitstunMultiplier2);
			if (player2->hasActiveTransformation()) {
				std::string playerTag = "p2_";
				playerTag += player2->getCurrentTransformationId() + "_";
				if (!playHitSounds(reaction_pl2, playerTag, player2)) {
					playHitSounds(reaction_pl2, "p2_", player2);
				}
			}
			else {
				playHitSounds(reaction_pl2, "p2_", player2);
			}
		};
		if (hitPl1){
			player1->setReaction(reaction_pl1, false, hitDirection1, hitstunMultiplier1);
			if (player1->hasActiveTransformation()) {
				std::string playerTag = "p1_";
				playerTag += player1->getCurrentTransformationId() + "_";
				if (!playHitSounds(reaction_pl1, playerTag, player1)) {
					playHitSounds(reaction_pl1, "p1_", player1);
				}
			}
			else {
				playHitSounds(reaction_pl1, "p1_", player1);
			}
		};
		// meter gain
		updateTriggerRegeneration(hitPl1, hitPl2);
	};

	/* update input management */
	void FK_SceneGame::updateInput(u32 delta_t_ms){
		bool newPlayer1Move = false;
		bool newPlayer2Move = false;
		updatePlayer1Input(delta_t_ms, newPlayer1Move);
		updatePlayer2Input(delta_t_ms, newPlayer2Move);
		// update visual effects for trigger
		updateTriggerComboEffect(0, newPlayer1Move, newPlayer2Move);
	};

	/* update input management */
	void FK_SceneGame::updateInputReplay(u32 delta_t_ms){
		bool newPlayer1Move = false;
		readReplayData(replayTimeInformationRead);
		if(player1isAI){
			updatePlayer1Input(delta_t_ms, newPlayer1Move);
		}
		else {
			if (player1ReplayBufferModified) {
				//newPlayer1Move = player1->performMove(player1input->readBuffer(
				//	player1->getMovesCollection(), 
				//	player1->isOnLeftSide(),
				//	player1->getStance()));
				newPlayer1Move = player1->performMove(player1input->readBuffer(player1));
				player1ReplayBufferModified = false;
			}
		}
		bool newPlayer2Move = false;
		if (player2isAI) {
			updatePlayer2Input(delta_t_ms, newPlayer2Move);
		}
		else {
			if (player2ReplayBufferModified) {
				//newPlayer2Move = player2->performMove(player2input->readBuffer(
				//	player2->getMovesCollection(), 
				//	player2->isOnLeftSide(),
				//	player2->getStance()));
				newPlayer2Move = player1->performMove(player2input->readBuffer(player2));
				player2ReplayBufferModified = false;
			}
		}
		updateTriggerComboEffect(0, newPlayer1Move, newPlayer2Move);
	};

	/* AI manager (integrated in the scene, for now, but later will be put in a separate class */
	u32 FK_SceneGame::getAIButtonPressed(u32 playerID){
		if (playerID == 1){
			return player1AIManager->getAIButtonPressed();
		}
		else if (playerID == 2){
			return player2AIManager->getAIButtonPressed();
		}
		return 0;
	};

	/* AI manager (integrated in the scene, for now, but later will be put in a separate class */
	u32 FK_SceneGame::getAIButtonPressedThrowDefense(u32 playerID){
		if (playerID == 1){
			if (!AI1_defenseCheckAgainstThrowPerformed){
				AI1_defenseCheckAgainstThrowPerformed = true;
				return player1AIManager->getAIThrowAttemptEscapeButtons();
			}
			else{
				return player1AIManager->getAIButtonPressed();
			}
		}
		else if (playerID == 2){
			if (!AI2_defenseCheckAgainstThrowPerformed){
				AI2_defenseCheckAgainstThrowPerformed = true;
				return player2AIManager->getAIThrowAttemptEscapeButtons();
			}
			else{
				return player2AIManager->getAIButtonPressed();
			}
		}
		return 0;
	};

	/* update AI input */
	bool FK_SceneGame::updateAIInput(u32 delta_t_ms, u32 playerID){
		if (playerID == 1){
			return player1AIManager->updateAIInput(delta_t_ms, roundTimerMs);
		}
		else if (playerID == 2){
			return player2AIManager->updateAIInput(delta_t_ms, roundTimerMs);
		}
		return 0;
	}

	/* get damage scaling factor */
	f32 FK_SceneGame::getDamageScalingFactor(f32 combodamage, FK_Character* targetPlayer){
		u32 counter = 0;
		//std::cout << "SCALING: " << combodamage << " " << targetPlayer->getMaxLife() / 2.5f;
		if (combodamage > targetPlayer->getMaxLife() / 2.5f){
			counter = 5;
		}
		else if (combodamage > targetPlayer->getMaxLife() / 3.f){
			counter = 4;
		}
		else if (combodamage > targetPlayer->getMaxLife() / 4.f){
			counter = 3;
		}
		else if (combodamage > targetPlayer->getMaxLife() / 5.f){
			counter = 2;
		}
		else if (combodamage > targetPlayer->getMaxLife() / 6.f){
			counter = 1;
		}
		counter = core::clamp(counter, (u32)0, damageScaling.size() - 1);
		//std::cout << " by factor " << damageScaling.at(counter) << std::endl;
		return damageScaling.at(counter);
	}

	/* check damage collision between bullets and player */
	bool FK_SceneGame::checkBulletDamageCollision(FK_Bullet* attackingBullet, FK_Character* bulletOwner, FK_Character* targetPlayer,
		FK_Reaction_Type &reactionToApply, f32 &hitDirection, f32 &hitstunMultiplier){
		// browse through the hurtbox of the target player
		bool hasDoneDamage = false;
		f32 damage = 0.0f;
		for (auto hurtboxIter = targetPlayer->getHurtboxCollection().begin(); 
			hurtboxIter != targetPlayer->getHurtboxCollection().end(); ++hurtboxIter){
			if ((attackingBullet->getHitbox().getAttackType() & FK_Attack_Type::HighAndMidAtks) != 0){
				if (/*targetPlayer->isGrounded() ||*/
					!(hurtboxIter->first == FK_Hurtbox_Type::HeadHurtbox ||
					hurtboxIter->first == FK_Hurtbox_Type::TorsoHurtbox ||
					hurtboxIter->first == FK_Hurtbox_Type::SpineHurtbox ||
					hurtboxIter->first == FK_Hurtbox_Type::HipsHurtbox)){
					continue;
				}
			}
			scene::ISceneNode* hitbox = attackingBullet->getHitboxNode();
			if (targetPlayer->getCurrentMove() == NULL && !targetPlayer->isHitStun() && targetPlayer->canEvadeBullets()){
				return false;
			}
			if (targetPlayer->isInvincible(attackingBullet->getHitbox().getAttackType()) ||
				targetPlayer->isInvincible(FK_Attack_Type::BulletAttack)){
				return false;
			}
			//hitbox->setVisible(true);
			//aabbox3d<f32> b1 = hitbox->getTransformedBoundingBox();
			// check if collision happened
			if (checkNodeCollision(hitbox, hurtboxIter->second) && !attackingBullet->getHitbox().hasHit()){
				attackingBullet->getHitbox().setHit(true);
				if (!(targetPlayer->isBeingThrown())){
					addMoveHitFlag(hitbox, &attackingBullet->getHitbox(), 0, 0, false);
				}
				// check guarding and so on
				float temp_ang = targetPlayer->getRotation().Y;
				core::vector2df normal1 = core::vector2df(cos(temp_ang*DEGTORAD), sin(temp_ang*DEGTORAD));
				temp_ang = attackingBullet->getRotation().Y - attackingBullet->getIntrinsicRotation().Y;
				core::vector2df normal2 = core::vector2df(cos(temp_ang*DEGTORAD), sin(temp_ang*DEGTORAD));
				/*temp_ang = (attackingBullet->getPosition() - targetPlayer->getPosition()).getHorizontalAngle().Y;
				core::vector2df connectionDirection = core::vector2df(cos(temp_ang*DEGTORAD), sin(temp_ang*DEGTORAD));*/
				bool targetIsShowingBack = normal1.dotProduct(normal2) > 0;
				/*core::vector3df connector = attackingBullet->getPosition() - targetPlayer->getPosition();
				connector.Y = 0;
				core::vector3df normal = core::vector3df(normal1.Y, 0, normal1.X);
				bool targetIsShowingBack = connector.dotProduct(normal) < 0;*/
				//hitDirection = -connector.dotProduct(normal);
				hitDirection = normal1.dotProduct(normal2);
				hitstunMultiplier = 1.0;
				bool guardBreakFlag = false;
				// check if attack can be guarded
				bool lowGuard = targetPlayer->isGuarding() && targetPlayer->getStance() == FK_Stance_Type::CrouchedStance;
				bool hiGuard = targetPlayer->isGuarding() &&
					(targetPlayer->getStance() == FK_Stance_Type::GroundStance ||
					targetPlayer->getStance() == FK_Stance_Type::WakeUpStance);
				bool hiGuardAttack = attackingBullet->getHitbox().canBeStandGuarded();
				bool lowGuardAttack = attackingBullet->getHitbox().canBeCrouchGuarded();
				bool hitDeflection = (lowGuard && lowGuardAttack) || (hiGuard && hiGuardAttack);
				/* check if the player has been hit on the back while guarding (guard break)*/
				if (hitDeflection){
					if (attackingBullet->getHitbox().isGuardBreaking() || targetIsShowingBack) {
						targetPlayer->breakGuard();
						guardBreakFlag = true;
						hitDeflection = false;
					}
				}
				bool targetHasArmor = /*!targetIsShowingBack && */targetPlayer->hasArmor(FK_Attack_Type::BulletAttack);
				// play sound effect
				if (hitDeflection){
					battleSoundManager->playSound("guard", 100.0 * gameOptions->getSFXVolume());
					// reverse bullet
					if (attackingBullet->canBeReflected()){
						FK_Bullet* new_bullet = new FK_Bullet();
						new_bullet->setup(attackingBullet);
						new_bullet->reflect();
						new_bullet->setupMesh(smgr);
						new_bullet->setPosition(attackingBullet->getPosition());
						targetPlayer->getBulletsCollection().push_back(new_bullet);
					}
				}
				else{
					f32 pitch = targetHasArmor ? 1.2f : 1.f;
					if (isStrongReaction(attackingBullet->getHitbox().getReaction())){
						battleSoundManager->playSound("hit_strong", 100.0 * gameOptions->getSFXVolume(), pitch);
					}
					else{
						battleSoundManager->playSound("hit_weak", 100.0 * gameOptions->getSFXVolume(), pitch);
					}
				}
				bulletOwner->activateParticleEffect(attackingBullet->getPosition(), hitDeflection);
				if (!hitDeflection){
					targetPlayer->breakGuard();
					damage = attackingBullet->getHitbox().getDamage();
					/* apply counterattack modifier damage -
					if the defending player is performing a move, has suffered from guard break
					or is attacked from behind*/
					if (targetPlayer->getCurrentMove() != NULL || targetIsShowingBack || guardBreakFlag){
						damage *= databaseAccessor.getCounterAttackDamageMultiplier();
						if (!(targetIsShowingBack && targetPlayer->getCurrentMove() == NULL)){
							hitstunMultiplier *= databaseAccessor.getCounterAttackHitstunMultiplier();
						}
					}
					// multiply damage by bullet damage multiplier
					damage *= targetPlayer->getReceivedBulletDamageMultiplier();
					// check if damage is higher than 0
					if (damage > 0){
						hasDoneDamage = true;
					}
					damage *= targetPlayer->getObjectsDefenseMultiplier(attackingBullet->getHitbox().getAttackType());
					damage *= targetPlayer->getObjectsDefenseMultiplier(FK_Attack_Type::BulletAttack);
					damage *= bulletOwner->getObjectsAttackMultiplier(FK_Hitbox_Type::AllHitboxes, FK_Attack_Type::BulletAttack);
					if (hasDoneDamage && damage <= 0){
						damage = 1;
					}
					if (damage >= targetPlayer->getLife()) {
						targetHasArmor = false;
					}
					if (hasDoneDamage) {
						targetPlayer->setLastDamagingMoveId(attackingBullet->getOriginalMoveId());
					}
					if (!isTimeUp() && !processingKOText) {
						applyDamageToPlayer(targetPlayer, damage, !targetHasArmor);
					}
				}
				if (targetHasArmor && !targetPlayer->isKO()) {
					reactionToApply = FK_Reaction_Type::NoReaction;
					targetPlayer->activateArmorSpecialEffect();
					return false;
				}
				else {
					if (targetPlayer->isKO() && !targetPlayer->isBeingThrown()) {
						reactionToApply = fk_constants::FK_KOReaction;
					}
					else {
						reactionToApply = attackingBullet->getHitbox().getReaction();
					}
				}
				//targetPlayer->resetComboCounter();
				if (hasDoneDamage){
					bulletOwner->increaseComboCounter(damage);
				}
				//attackingBullet->dispose();
				return true;
			}
		}
		return false;
	}

	/* check damage collision between players */
	bool FK_SceneGame::checkDamageCollision(FK_Character* attackingPlayer, FK_Character* targetPlayer,
		FK_Reaction_Type &reactionToApply, bool& guardBreakTrigger, f32 &hitDirection, f32 &hitstunMultiplier,
		FK_Move* &attemptedThrowMove){
		// set attempted throw to NULL
		attemptedThrowMove = NULL;
		// if target player is invisible, exit the process
		if (!(targetPlayer->getAnimatedMesh()->isVisible())){
			return false;
		}
		f32 damage = 0.0f;
		// proceed only if attacking player current move is not NULL
		if (attackingPlayer->getCurrentMove() != NULL){
			FK_Move* move = attackingPlayer->getCurrentMove();
			bool hasDoneDamage = false;
			// browse through the hurtboxes of the target player
			for (auto hitboxIter = move->getHitboxCollection().begin(); hitboxIter != move->getHitboxCollection().end(); ++hitboxIter){
				FK_Hitbox_Type type = hitboxIter->getType();
				if (targetPlayer->isInvincible(hitboxIter->getAttackType())){
					continue;
				}
				if (!hitboxIter->isActive(attackingPlayer->getAnimatedMesh()->getFrameNr())){
					continue;
				}
				scene::ISceneNode* hitbox = attackingPlayer->getHitboxCollection()[type];
				for (auto hurtboxIter = targetPlayer->getHurtboxCollection().begin(); hurtboxIter != targetPlayer->getHurtboxCollection().end(); ++hurtboxIter){
					// browse through the hitboxes of the attacking player move to check if one of the active ones intesect with
					// one of the hurtboxes
					if ((((targetPlayer->getStance() & FK_Stance_Type::AnyGroundedStance) != 0 || 
						targetPlayer->getStance() == FK_Stance_Type::WakeUpStance) &&
						!targetPlayer->isBeingThrown()) &&
						hurtboxIter->first == FK_Hurtbox_Type::FullBodyHurtbox){
						continue;
					}
					// check if the hitbox is active
					if (hitboxIter->isActive(attackingPlayer->getAnimatedMesh()->getFrameNr()) && checkNodeCollision(hitbox, hurtboxIter->second)){
						hitboxIter->setHit(true);
						attackingPlayer->getCurrentMove()->markConnectionWithTarget();
						//if (!targetPlayer->isHitStun()) {
						//	attackingPlayer->resetComboCounter();
						//}
						// if the hitbox is a SingleHit hitbox, deactivate all the other hitboxes with the same type
						if (hitboxIter->isSingleHit()){
							for (auto otherHitboxIter = move->getHitboxCollection().begin(); otherHitboxIter != move->getHitboxCollection().end(); ++otherHitboxIter){
								FK_Hitbox_Type otherType = otherHitboxIter->getType();
								if (type == otherType && otherHitboxIter->isSingleHit()){
									otherHitboxIter->setHit(true);
								}
							}
						}
						
						float basicDamageMultiplier = 1;
						// apply impact damage multiplier if the attacker is Impact Running
						if (attackingPlayer->isInImpactCancelState()){
							basicDamageMultiplier *= databaseAccessor.getRunCancelDamageMultiplier();
						}
						float temp_ang = targetPlayer->getRotation().Y;
						core::vector2df normal1 = core::vector2df(cos(temp_ang*DEGTORAD), sin(temp_ang*DEGTORAD));
						//temp_ang = attackingPlayer->getRotation().Y;
						//core::vector2df normal2 = core::vector2df(cos(temp_ang*DEGTORAD), sin(temp_ang*DEGTORAD));
						//temp_ang = (attackingPlayer->getPosition() - targetPlayer->getPosition()).getHorizontalAngle().Y;
						//core::vector2df connectionDirection = core::vector2df(cos(temp_ang*DEGTORAD), sin(temp_ang*DEGTORAD));
						//f32 attackDirection = normal2.dotProduct(connectionDirection);
						//bool targetIsShowingBack = normal1.dotProduct(normal2) > 0;
						//bool targetIsShowingBack = attackDirection > 0;
						core::vector3df connector = attackingPlayer->getPosition() - targetPlayer->getPosition();
						connector.Y = 0;
						core::vector3df normal = core::vector3df(normal1.Y, 0, normal1.X);
						//hitDirection = normal2.dotProduct(connectionDirection);
						//bool targetIsShowingBack = normal1.dotProduct(normal2) > 0;
						bool targetIsShowingBack = connector.dotProduct(normal) < 0;
						//attackDirection = normal1.dotProduct(normal2);
						f32 attackDirection = -connector.dotProduct(normal);
						hitstunMultiplier = 1.0;
						bool guardBreakFlag = false;
						// check if attack can be guarded
						bool lowGuard = targetPlayer->isGuarding() && targetPlayer->getStance() == FK_Stance_Type::CrouchedStance;
						bool hiGuard = targetPlayer->isGuarding() && 
							(targetPlayer->getStance() == FK_Stance_Type::GroundStance ||
							targetPlayer->getStance() == FK_Stance_Type::WakeUpStance);
						bool hiGuardAttack = hitboxIter->canBeStandGuarded();
						bool lowGuardAttack = hitboxIter->canBeCrouchGuarded();
						bool hitDeflection = (lowGuard && lowGuardAttack) || (hiGuard && hiGuardAttack);
						/* first, check if trigger guard has been activated */
						if (!targetIsShowingBack && !targetPlayer->isBeingThrown() &&
							targetPlayer->isTriggerGuardActive() && hitDeflection){
							attackingPlayer->activateParticleEffect(type, hitDeflection);
							FK_Reaction_Type reaction = FK_SceneGame::weakTriggerReaction;
							if (attackingPlayer->isTriggerModeActive()){
								reaction = FK_SceneGame::strongTriggerReaction;
								targetPlayer->cancelTriggerComboedFlag();
							}
							hasDoneDamage = false;
							attackingPlayer->resetComboCounter();
							targetPlayer->resetComboCounter();
							attackingPlayer->applyDamageToObjects(0);
							attackingPlayer->setReaction(reaction, false, attackDirection);
							attackingPlayer->cancelTriggerMode();
							targetPlayer->cancelTriggerMode();
							//std::cout << "Trigger guard counter!" << std::endl;
							// play guard sound effects
							battleSoundManager->playSound("guard", 100.0 * gameOptions->getSFXVolume());
							return false;
							/*
							if (!isTriggerComboActive()){
								activateTriggerComboEffect();
							}
							*/
						}
						else{
							bool counterAttackFlag = false;
							/* check if the player has been hit by a low attack while guarding or a mid attack while crouched and
							trigger a counter attack flag*/
							if (!hitDeflection){
								if (targetPlayer->isGuarding()){
									targetPlayer->breakGuard();
									guardBreakFlag = true;
									hitDeflection = false;
								}
							}
							/* check if the player has been hit on the back while guarding (guard break)*/
							if (hitDeflection){
								if (attackingPlayer->isTriggerModeActive() ||
									hitboxIter->isGuardBreaking() || targetIsShowingBack) {
									targetPlayer->breakGuard();
									guardBreakFlag = true;
									if (attackingPlayer->isTriggerModeActive() && 
										!(hitboxIter->isGuardBreaking() || targetIsShowingBack)){
										basicDamageMultiplier = 0.f;
									}
									guardBreakTrigger = attackingPlayer->isTriggerModeActive();
									hitDeflection = false;
								}
							}
							if (!hitDeflection){
								// activate tracking if the move applies it
								if (attackingPlayer->getCurrentMove()->canTrackAfterHit()){
									attackingPlayer->setDirectionLock(false);
								}
								// check if the attack is a throw attack
								if ((hitboxIter->getAttackType() & FK_Attack_Type::ThrowAtk) != 0) {
									// if the defending player is attacking, cancel the throw
									bool hitAttackingOpponents = 
										(hitboxIter->getAttackType() & FK_Attack_Type::BlockableThrowAtk) != 0 &&
									hitboxIter->getPriority() > 0;
									
									if (!hitAttackingOpponents && /*(targetPlayer->getCurrentMove() != NULL &&
										targetPlayer->getCurrentMove()->canBeTriggered()) &&
										!(targetPlayer->getCurrentMove()->hasThrowAttacks())*/
										!targetPlayer->canBeThrown()){
										return false;
									}
									attemptedThrowMove = attackingPlayer->getCurrentMove();
									cameraManager->alignCharacters(targetPlayer);
									throwTimer = 0;
									attackingPlayer->activateParticleEffect(hitboxIter->getType(), true);
									reactionToApply = hitboxIter->getReaction();
									hitDirection = attackDirection;
									targetPlayer->resetComboCounter();
									applyDamageToPlayer(targetPlayer, 0);
									// add hitbox to the HUD collection for use in z.B. training mode
									if (!(targetPlayer->isBeingThrown())) {
										addMoveHitFlag(hitbox, &(*hitboxIter), 0, 0, false);
									}
									return true;
								}
							}
							// check if target character has armor
							bool targetHasArmor =/* !targetIsShowingBack &&*/ targetPlayer->hasArmor(hitboxIter->getAttackType());
							// play sound effect
							if (hitDeflection){
								battleSoundManager->playSound("guard", 100.0 * gameOptions->getSFXVolume());
							}
							else{
								f32 pitch = targetHasArmor ? 1.2f : 1.f;
								if (targetPlayer->isBeingThrown() || isStrongReaction(hitboxIter->getReaction())){
									battleSoundManager->playSound("hit_strong", 100.0 * gameOptions->getSFXVolume(), pitch);
								}
								else{
									battleSoundManager->playSound("hit_weak", 100.0 * gameOptions->getSFXVolume(), pitch);
								}
							}
							attackingPlayer->activateParticleEffect(type, hitDeflection || targetHasArmor);
							if (!hitDeflection){
								targetPlayer->breakGuard();
								damage = hitboxIter->getDamage();
								// apply multiplier (guard break on trigger yields 0)
								damage *= basicDamageMultiplier;
								/* apply trigger modifier damage */
								if (attackingPlayer->isTriggerModeActive()){
									damage *= databaseAccessor.getTriggerComboDamageMultiplier();
									hitstunMultiplier *= databaseAccessor.getTriggerComboHitstunMultiplier();
									// set TriggerComboedFlag on the target player
									if (!targetHasArmor) {
										targetPlayer->setTriggerComboedFlag();
									}
									//std::cout << targetPlayer->getDisplayName() << " received " << damage << " damages!" << std::endl;
									if (!isTriggerComboActive()){
										activateTriggerComboEffect();
									}
									else{
										if (triggerTimer > databaseAccessor.getSlowmoIntervalMs()){
											cancelTriggerComboEffect();
										}
									}

								}
								/* apply counterattack modifier damage -
								if the defending player is performing a move, has suffered from guard break
								or is attacked from behind*/
								if (targetPlayer->getCurrentMove() != NULL || targetIsShowingBack || guardBreakFlag){
									damage *= databaseAccessor.getCounterAttackDamageMultiplier();
									if (!(targetIsShowingBack && targetPlayer->getCurrentMove() == NULL)){
										hitstunMultiplier *= databaseAccessor.getCounterAttackHitstunMultiplier();
										if (targetPlayer->getCurrentMove() != NULL) {
											activateCounterattackEffects();
										}
									}
								}
								// multiply damage by bullet damage multiplier
								damage *= targetPlayer->getReceivedPhysicalDamageMultiplier();
								// check if damage is greater than 0
								if (damage > 0){
									hasDoneDamage = true;
								}
								if (hasDoneDamage) {
									targetPlayer->setLastDamagingMoveId(
										attackingPlayer->getCurrentMove()->getMoveId());
								}
								// apply buffs and nerfs
								damage *= attackingPlayer->getObjectsAttackMultiplier(type, hitboxIter->getAttackType());
								damage *= targetPlayer->getObjectsDefenseMultiplier(hitboxIter->getAttackType());
								// apply damage scaling
								if (hasDoneDamage && (!(targetPlayer->isBeingThrown()) || 
									attackingPlayer->getCurrentMove()->isAffectedByDamageScaling())){
									f32 combodamage = attackingPlayer->getComboDamage();
									f32 scalingFactor = getDamageScalingFactor(combodamage, targetPlayer);
									scalingFactor = core::clamp(scalingFactor,
										attackingPlayer->getCurrentMove()->getMaximumDamangeScaling(),
										1.0f);
									damage *= scalingFactor;
									if (damage < 1){
										damage = 1;
									}
									//std::cout << damage << std::endl;
								}
								if (damage >= targetPlayer->getLife()) {
									targetHasArmor = false;
								}
								if (!isTimeUp() && !processingKOText){
									applyDamageToPlayer(targetPlayer, damage, !targetHasArmor);
								}
							}
							if (targetHasArmor && !targetPlayer->isKO()) {
								reactionToApply = FK_Reaction_Type::NoReaction;
								targetPlayer->activateArmorSpecialEffect();
								return false;
							}
							else {
								if (targetPlayer->isKO() && !targetPlayer->isBeingThrown()) {
									reactionToApply = fk_constants::FK_KOReaction;
								}
								else {
									reactionToApply = hitboxIter->getReaction();
								}
								hitDirection = attackDirection;
								targetPlayer->resetComboCounter();
								if (hasDoneDamage) {
									s32 moveId = -1;
									s32 hitboxId = -1;
									if (!attackingPlayer->isTriggerModeActive()) {
										moveId = static_cast<s32>(move->getMoveId());
										hitboxId = static_cast<s32>(hitboxIter->getInternalId());
										u32 numberOfRepetitions =
											attackingPlayer->getNumberOfRepetitionOfSameMoveInCombo(moveId, hitboxId);
										// apply hitstun multipliers for repeating moves
										if (numberOfRepetitions > 1) {
											hitstunMultiplier *= 0.5;
										}
										else if (numberOfRepetitions > 0) {
											hitstunMultiplier *= 0.7;
										}
									}
									attackingPlayer->increaseComboCounter(damage, moveId, hitboxId);
									// check if this move is inside the combo log for infinite prevention system
	
								}
							}
							// add hitbox to the HUD collection for use in z.B. training mode
							if (!(targetPlayer->isBeingThrown())) {
								s32 frameAdvantage = 0;
								s32 cancelAdvantage = 0;
								bool showAdvantage = calculateFrameAdvantage(
									frameAdvantage,
									cancelAdvantage,
									&(*hitboxIter),
									hitstunMultiplier,
									attackingPlayer, targetPlayer, 
									guardBreakTrigger,
									counterAttackFlag, targetHasArmor);
								addMoveHitFlag(hitbox, &(*hitboxIter), frameAdvantage, cancelAdvantage, showAdvantage);
							}
						}
						return true;
					}
				}
			}
		}
		return false;
	};

	/* check if game is processing throw */
	bool FK_SceneGame::isProcessingThrow(){
		if (player1ThrowAttemptMove == NULL && player2ThrowAttemptMove == NULL){
			return false;
		}
		return true;
	}

	/* update throw attempt*/
	void FK_SceneGame::updateThrowAttempt(u32 delta_t_ms){
		if (!isProcessingThrow()){
			return;
		}
		throwTimer += delta_t_ms;
		bool throwSuccessFlag = throwTimer >= throwMaxReactionTimeMs;
		if (throwSuccessFlag){
			if (player2ThrowAttemptMove == NULL && player1ThrowAttemptMove == NULL){
				resetThrowAttempt();
			}
			else{
				if (player1isAI && player2ThrowAttemptMove != NULL){
					player1AIManager->increaseSufferedThrowsCounter();
				}
				if (player2isAI && player1ThrowAttemptMove != NULL){
					player2AIManager->increaseSufferedThrowsCounter();
				}
				applySuccessfulThrow();
			}
		}
		else{
			/* check input */
			u32 bitmask = ~((u32)FK_Input_Buttons::Any_SystemButton | (u32)FK_Input_Buttons::Any_MenuButton);
			u32 player1InputButtons = player1input->getPressedButtons() & bitmask;
			u32 player2InputButtons = player2input->getPressedButtons() & bitmask;
			if (player1isAI){
				if (player2ThrowAttemptMove != NULL){
					player1InputButtons = getAIButtonPressedThrowDefense(1);
				}
				else{
					player1InputButtons = 0;
				}
			}
			if (player2isAI){
				if (player1ThrowAttemptMove != NULL){
					player2InputButtons = getAIButtonPressedThrowDefense(2);
				}
				else{
					player2InputButtons = 0;
				}
			}
			if (player1ThrowAttemptMove != NULL){
				if (!skipThrowCheckPlayer1 &&
					player1ThrowAttemptMove->getThrowEscapeInput() != 0 && 
					player2InputButtons == player1ThrowAttemptMove->getThrowEscapeInput()){
					cancelThrow();
				}
			}
			else{
				if (!skipThrowCheckPlayer2 &&
					player2ThrowAttemptMove->getThrowEscapeInput() != 0 &&
					player1InputButtons == player2ThrowAttemptMove->getThrowEscapeInput()){
					cancelThrow();
				}
			}
		}
	}

	/* reset throw attempt*/
	void FK_SceneGame::resetThrowAttempt(){
		throwTimer = 0;
		player1ThrowAttemptMove = NULL;
		player2ThrowAttemptMove = NULL;
		AI1_defenseCheckAgainstThrowPerformed = false;
		AI2_defenseCheckAgainstThrowPerformed = false;
		player1->setThrowProcessingFlag(false);
		player2->setThrowProcessingFlag(false);
	}

	/* cancel throw */
	void FK_SceneGame::cancelThrow(){
		battleSoundManager->playSound("guard", 100.0 * gameOptions->getSFXVolume());
		player1->breakGuard();
		player2->breakGuard();
		player1->setThrowProcessingFlag(false);
		player2->setThrowProcessingFlag(false);
		if (player1ThrowAttemptMove != NULL && player2ThrowAttemptMove != NULL){
			player1->setReaction(throwEscapeReactionPerformer, false, -1, 1);
			player2->setReaction(throwEscapeReactionPerformer, false, -1, 1);
		}
		else if (player1ThrowAttemptMove != NULL){
			player1->setReaction(throwEscapeReactionPerformer, false, -1, 1);
			player2->setReaction(throwEscapeReactionTarget, false, -1, 1);
		}
		else if (player2ThrowAttemptMove != NULL){
			player1->setReaction(throwEscapeReactionTarget, false, -1, 1);
			player2->setReaction(throwEscapeReactionPerformer, false, -1, 1);
		}
		player1->resetMove();
		player2->resetMove();
		if (player1isAI){
			player1AIManager->resetSufferedThrowsCounter();
		}
		if (player2isAI){
			player2AIManager->resetSufferedThrowsCounter();
		}
		resetThrowAttempt();
	}

	/* apply successful throw*/
	bool FK_SceneGame::applySuccessfulThrow(){
		throwTimer = 0;
		player1->setThrowProcessingFlag(false);
		player2->setThrowProcessingFlag(false);
		FK_Character* attackingPlayer = NULL;
		FK_Character* targetPlayer = NULL;
		bool player1IsAttacking = false;
		FK_Move* throwMove = NULL;
		if (player1ThrowAttemptMove != NULL){
			attackingPlayer = player1;
			targetPlayer = player2;
			player1IsAttacking = true;
			throwMove = player1ThrowAttemptMove;
		}
		else if (player2ThrowAttemptMove != NULL){
			attackingPlayer = player2;
			targetPlayer = player1;
			throwMove = player2ThrowAttemptMove;
		}
		if (throwMove != NULL){
			// if so, go on with the followup attack
			std::string throwMoveName = throwMove->getFollowupMovesSet()[0].getName();
			FK_Move* throwFollowup = NULL;
			// look into the move list for the followup throw attack
			for (auto moveIter = attackingPlayer->getMovesCollection().begin();
				moveIter != attackingPlayer->getMovesCollection().end();
				++moveIter){
				if (moveIter->getName() == throwMoveName.c_str()){
					throwFollowup = &(*moveIter);
					break;
				}
			}
			// then, force the player to perform it as a followup
			//battleSoundManager->playSound("guard");
			//attackingPlayer->activateParticleEffect(hitboxIter->getType(), true);
			attackingPlayer->markAllBulletsForDisposal();
			targetPlayer->markAllBulletsForDisposal();
			attackingPlayer->performMove(throwFollowup, true);
			attackingPlayer->setThrowingOpponentFlag(true);
			core::vector3df directionAngle = (attackingPlayer->getPosition() - targetPlayer->getPosition()).getHorizontalAngle();
			targetPlayer->setThrowAnimation(throwFollowup->getThrowReactionAnimationPose(), directionAngle);
			targetPlayer->setThrowMove(throwFollowup);
			applyDamageToPlayer(targetPlayer, 0);
			//std::cout << "Setting up throw... " << throwFollowup->getName() << std::endl;
			//reactionToApply = hitboxIter->getReaction();
			// reset combo counter 
			//attackingPlayer->resetComboCounter();
			resetThrowAttempt();
			return true;
		}
		return false;
	}

	/* collision manager */
	bool FK_SceneGame::checkNodeCollision(ISceneNode* one, ISceneNode* two){
		aabbox3d<f32> b1, b2;
		b1 = one->getTransformedBoundingBox();
		b2 = two->getTransformedBoundingBox();
		bool intersectFlag = b1.intersectsWithBox(b2);
		return intersectFlag;
	};

	/* trigger effect active */
	bool FK_SceneGame::isTriggerComboActive(){
		return triggerComboActiveEffect;
	}

	void FK_SceneGame::activateTriggerComboEffect(){
		if (triggerTimer > databaseAccessor.getSlowmoCooldownMs()){
			triggerComboActiveEffect = true;
			triggerTimer = 0;
			/* BGM HERE!!! */
			stage_bgm.setVolume(stage->getBGMTriggerVolume() * gameOptions->getMusicVolume());
			if (!isPaused()){
				device->getTimer()->setSpeed(databaseAccessor.getSlowmoTimerVelocity());
				//std::cout << "Trigger combo on" << std::endl;
			}
		}
	};

	void FK_SceneGame::cancelTriggerComboEffect(){
		triggerComboActiveEffect = false;
		triggerTimer = 0;
		//stage_bgm.setVolume(stage->getBGMVolume());
		if (!isPaused()){
			device->getTimer()->setSpeed(databaseAccessor.getNormalTimerVelocity());
			//std::cout << "Trigger combo off" << std::endl;
		}
	};

	/* update slowmo effect for trigger */
	void FK_SceneGame::updateTriggerComboEffect(u32 frameDelta, bool newMovePlayer1, bool newMovePlayer2){
		triggerTimer += frameDelta;
		if (isTriggerComboActive()){
			if (newMovePlayer1 && player1->getCurrentMove()->getNumberOfRequiredBulletCounters() > 0) {
				battleSoundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
			}
			if (newMovePlayer2 && player2->getCurrentMove()->getNumberOfRequiredBulletCounters() > 0) {
				battleSoundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
			}
			if (triggerTimer > databaseAccessor.getSlowmoDurationMs()){
				cancelTriggerComboEffect();
				return;
			}
			if (!(player1->isTriggerModeActive() || player2->isTriggerModeActive())){
				cancelTriggerComboEffect();
				stage_bgm.setVolume(stage->getBGMVolume() * gameOptions->getMusicVolume());
				return;
			}
			if ((newMovePlayer1 && player1->isTriggerModeActive()) || 
				(newMovePlayer2 && player2->isTriggerModeActive())){
				cancelTriggerComboEffect();
				return;
			}
		}
	}

	bool FK_SceneGame::isUpdatingImpactEffect(){
		return impactEffectCounter >= 0;
	}

	/* update slowmo effect for counter attack */
	void FK_SceneGame::updateCounterattackEffects(u32 frameDelta) {
		counterAttackEffectCooldownCounterMs += frameDelta;
		if (updateCounterAttackEffectFlag && databaseAccessor.slowmoEffectOnCounterAttackAllowed()) {
			counterAttackEffectCounterMs += frameDelta;
			if (counterAttackEffectCounterMs > databaseAccessor.getCounterAttackEffectDurationMs()) {
				cancelCounterattackEffects();
			}
		}
	}

	void FK_SceneGame::cancelCounterattackEffects() {
		counterAttackEffectCounterMs = 0;
		updateCounterAttackEffectFlag = false;
		if (!isPaused() && device->getTimer()->getSpeed() != databaseAccessor.getNormalTimerVelocity()) {
			device->getTimer()->setSpeed(databaseAccessor.getNormalTimerVelocity());
		}
	}

	void FK_SceneGame::activateCounterattackEffects() {
		counterAttackEffectCounterMs = 0;
		if (counterAttackEffectCooldownCounterMs > databaseAccessor.getCounterAttackEffectCooldownMs() &&
			databaseAccessor.slowmoEffectOnCounterAttackAllowed() &&
			device->getTimer()->getSpeed() == databaseAccessor.getNormalTimerVelocity()) {
			device->getTimer()->setSpeed(databaseAccessor.getCounterAttackSlowmoTimerVelocity());
			updateCounterAttackEffectFlag = true;
			counterAttackEffectCooldownCounterMs = 0;
			if (databaseAccessor.playSoundEffectOnCounterAttack()) {
				battleSoundManager->playSound("hit_counter", 100.0 * gameOptions->getSFXVolume());
			}
			setupImpactEffect(FK_Reaction_Type::StrongFlight);
		}
	}

	/* setup impact effect */
	void FK_SceneGame::setupImpactEffect(FK_Reaction_Type reaction, s32 playerId){
		if (reaction == FK_Reaction_Type::StrongFlight || 
			reaction == FK_Reaction_Type::ReverseStrongFlight ||
			reaction == FK_Reaction_Type::StandingFlight){
			impactEffectCounter = 0;
		}
	}

	/* update impact effect */
	void FK_SceneGame::updateImpactEffect(u32 frame_delta){
		if (isUpdatingImpactEffect()){
			impactEffectCounter += frame_delta;
		}
		if ((u32)impactEffectCounter >= FK_SceneGame::impactEffectDuration){
			impactEffectCounter = -1;
		}
	}

	/* game logic (win, lose, et cetera)*/
	bool FK_SceneGame::isEndOfRound(){
		return (player1->isKO() || player2->isKO() || isTimeUp());
	}

	bool FK_SceneGame::isEndOfMatch(){
		return numberOfPlayer1Wins >= numberOfRoundsPlayer1 ||
			numberOfPlayer2Wins >= numberOfRoundsPlayer2;
	}

	bool FK_SceneGame::isFinalRound(){
		return (numberOfPlayer1Wins >= numberOfRoundsPlayer1 - 1 &&
			numberOfPlayer2Wins >= numberOfRoundsPlayer2 - 1);
	}
	/* this function returns -1 if player 1 won, 1 if player 2 won or 0 if it was a double KO. 
	Internally, it updates the number of wins and losses if updateWins is set to true*/
	int FK_SceneGame::getRoundWinner(bool updateWins){
		if (isTimeUp()){
			if (player1->getLifePercentage() == player2->getLifePercentage()){
				if (updateWins){
					numberOfPlayer2Wins += 1;
					numberOfPlayer1Wins += 1;
				}
				return 0;
			}
			else if (player1->getLifePercentage() < player2->getLifePercentage()){
				if (updateWins){
					numberOfPlayer2Wins += 1;
				}
				return 1;
			}
			else if (player1->getLifePercentage() > player2->getLifePercentage()){
				if (updateWins){
					numberOfPlayer1Wins += 1;
				}
				return -1;
			}
		}
		else{
			if (player1->isKO() && player2->isKO()){
				if (updateWins){
					numberOfPlayer2Wins += 1;
					numberOfPlayer1Wins += 1;
				}
				return 0;
			}
			else if (player1->isKO()){
				if (updateWins){
					numberOfPlayer2Wins += 1;
				}
				return 1;
			}
			else if (player2->isKO()){
				if (updateWins){
					numberOfPlayer1Wins += 1;
				}
				return -1;
			}
		}
		return -99;
	}
	/* this function returns -1 if player 1 won, 1 if player 2 won or 0 if it was a double KO*/
	int FK_SceneGame::getMatchWinner(){
		if (numberOfPlayer1Wins >= numberOfRoundsPlayer1 && numberOfPlayer2Wins >= numberOfRoundsPlayer2){
			if (numberOfPlayer1Wins > numberOfPlayer2Wins){
				return -1;
			}
			else if (numberOfPlayer1Wins < numberOfPlayer2Wins){
				return 1;
			}
			else {
				return 0;
			}
		}
		else if (numberOfPlayer2Wins >= numberOfRoundsPlayer2){
			return 1;
		}
		else if (numberOfPlayer1Wins >= numberOfRoundsPlayer1){
			return -1;
		}
		return -99;
	}
	/* get current round */
	int FK_SceneGame::getCurrentRoundNumber(){
		return currentRoundNumber;
	}

	void FK_SceneGame::initializeReplayStorage(u32 randomSeed) {
		std::string replayFileName = applicationPath + "\\replays\\" + "replay.txt";
		replayFileOut.open(replayFileName.c_str());
		replayFileOut << randomSeed << std::endl;
		replayFileOut << arena_configPath << std::endl;
		replayFileOut << player1_configPath << std::endl;
		replayFileOut << player2_configPath << std::endl;
		replayFileOut << player1isAI << " "<< player1AILevel << std::endl;
		replayFileOut << player2isAI << " " << player2AILevel << std::endl;
	}

	void FK_SceneGame::openReplayFile(u32& randomSeed) {
		std::string replayFileName = applicationPath + "\\replays\\" + "replay.txt";
		replayFileIn.open(replayFileName.c_str());
		if (replayFileIn) {
			replayFileIn >> randomSeed;
			replayFileIn >> arena_configPath;
			replayFileIn >> player1_configPath;
			replayFileIn >> player2_configPath;
			replayFileIn >> player1isAI >> player1AILevel;
			replayFileIn >> player2isAI >> player2AILevel;
		}
	}

	/* store replay data */
	/* WIP - save input to file for replay every 200ms*/
	void FK_SceneGame::storeReplayData(){
		//if ((now - startingTime) % 100 <= 20){
			int size = player1input->getBuffer().size();
			replayFileOut << now << std::endl;
			for (int i = 0; i < 15; ++i){
				u32 value = 0;
				if (i < size)
					value = player1input->getBuffer()[i];
				replayFileOut << value << " ";
			}
			replayFileOut << std::endl << player1input->getPressedButtons() << std::endl;
			size = player2input->getBuffer().size();
			for (int i = 0; i < 15; ++i){
				u32 value = 0;
				if (i < size)
					value = player2input->getBuffer()[i];
				replayFileOut << value << " ";
			}
			replayFileOut << std::endl << player2input->getPressedButtons() << std::endl;
		//}
	};
	/* store replay data */
	/* WIP - save input to file for replay every 200ms*/
	void FK_SceneGame::readReplayData(bool &timeInformationHasBeenRead){
		if (replayFileIn){
			if (!timeInformationHasBeenRead){
				u32 time = 0;
				replayFileIn >> time;
				now = time;
				timeInformationHasBeenRead = true;
			}
			else{
				player1ReplayBufferModified = false;
				int size = player1ReplayBuffer.size();
				for (int i = 0; i < 15; ++i){
					u32 val;
					replayFileIn >> val;
					if (i < size){
						if (player1ReplayBuffer[i] != val) {
							player1ReplayBufferModified = true;
						}
						player1ReplayBuffer[i] = val;
					}
					else{
						if (val != 0){
							player1ReplayBuffer.push_back(val);
							player1ReplayBufferModified = true;
						}
					}
				}
				player1ReplayBuffer.erase(
					std::remove(player1ReplayBuffer.begin(), player1ReplayBuffer.end(), 0),
					player1ReplayBuffer.end());
				// player 2 buffer
				size = player2ReplayBuffer.size();
				replayFileIn >> player1ReplayAllButtonsPressed;
				player2ReplayBufferModified = false;
				for (int i = 0; i < 15; ++i){
					u32 val;
					replayFileIn >> val;
					if (i < size){
						if (player2ReplayBuffer[i] != val) {
							player2ReplayBufferModified = true;
						}
						player2ReplayBuffer[i] = val;
					}
					else{
						if (val != 0){
							player2ReplayBuffer.push_back(val);
							player2ReplayBufferModified = true;
						}
					}
				}
				player2ReplayBuffer.erase(
					std::remove(player2ReplayBuffer.begin(), player2ReplayBuffer.end(), 0),
					player2ReplayBuffer.end());
				replayFileIn >> player2ReplayAllButtonsPressed;
				player1input->setBuffer(player1ReplayBuffer);
				player2input->setBuffer(player2ReplayBuffer);
				timeInformationHasBeenRead = false;
			}
		}
	};

	bool FK_SceneGame::isPlayingReplay(){
		return playReplay;
	}
	s32 FK_SceneGame::getWinnerId(){
		return winnerId;
	}
	void FK_SceneGame::setMatchStatistics(FK_FreeMatchStatistics new_statistics){
		matchStatistics = new_statistics;
	}
	FK_FreeMatchStatistics FK_SceneGame::getMatchStatistics(){
		return matchStatistics;
	}

	bool FK_SceneGame::isProcessingRingout(){
		return (player1ringoutFlag || player2ringoutFlag);
	}


	// overloadable methods

	/* apply damage to player */
	void FK_SceneGame::applyDamageToPlayer(FK_Character* targetPlayer, f32 damage, bool affectsObjects){
		targetPlayer->receiveDamage(damage, affectsObjects);
	}

	/* update additional scene logic */
	void FK_SceneGame::updateAdditionalSceneLogic(u32 delta_t_ms){}

	/* draw additional HUD items*/
	void FK_SceneGame::drawAdditionalHUDItems(f32 delta_t_s){}

	/* draw additional HUD items*/
	void FK_SceneGame::drawAdditionalHUDOverlay(f32 delta_t_s) {}

	/* setup additional (i.e. class specific) variables */
	void FK_SceneGame::setupAdditionalVariables(){}

	/* setup additional (i.e. class specific) variables after resources are loaded */
	void FK_SceneGame::setupAdditionalParametersAfterLoading(){}

	/* get scene name for loading screen */
	std::wstring FK_SceneGame::getSceneNameForLoadingScreen(){
		return L"Free Match";
	}

	/* process new round start */
	void FK_SceneGame::processNewRoundStart(){
		processingTimeCounterMs = 0;
		processingRoundText = true;
		canPlayAnnouncerVoice = true;
		newRoundFlag = false;
		processingEndOfRound = false;
		currentRoundNumber += 1;
		// also, resync meshs
		//player1->synchronizeAnimatedObjects();
		//player2->synchronizeAnimatedObjects();
	}

	/* process round end */
	void FK_SceneGame::processRoundEnd(){
		// return until both players are on the ground
		if (player1->isAirborne() && player1->getPosition().Y > stage->getRingoutDepth() &&
			!player1ringoutFlag){
			return;
		}
		if (player2->isAirborne() && player2->getPosition().Y > stage->getRingoutDepth() &&
			!player2ringoutFlag) {
			return;
		}
		if (
			(player1->getCurrentMove() != NULL && !player1ringoutFlag) ||
			(player2->getCurrentMove() != NULL && !player2ringoutFlag)
			) {
			return;
		}
		if ((winnerId == -1 && player1->getPosition().Y < stage->getRingoutDepth()) ||
			(winnerId == 1 && player2->getPosition().Y < stage->getRingoutDepth())) {
			resetInitialCharacterPositions();
		}
		// then, show win text
		processingKOText = false;
		lastSkipInputTime = now;
		if (isEndOfMatch()){
			winnerId = getMatchWinner();
			if (additionalOptions.playReplay) {
				replayFileIn.close();
			}
			else if (additionalOptions.saveReplay) {
				replayFileOut.close();
			}
		}
		if (winnerId != 0){
			player1->cancelJumpVariables();
			player2->cancelJumpVariables();
			player1->setVelocityPerSecond(core::vector3df(0, 0, 0));
			player2->setVelocityPerSecond(core::vector3df(0, 0, 0));
			player1->cancelHitstun();
			player2->cancelHitstun();
			player1->resetMove();
			player2->resetMove();
			(winnerId == -1 ? player1 : player2)->setStance(FK_Stance_Type::WinStance);
			(winnerId == -1 ? player1 : player2)->setBasicAnimation(FK_BasicPose_Type::WinAnimation, true);
			(winnerId == 1 ? player1 : player2)->getAnimatedMesh()->setVisible(false);
			lightManager->removeShadowFromNode((winnerId == 1 ? player1 : player2)->getAnimatedMesh());
			if (winnerId == -1){
				for (int i = 0; i < player2->getCharacterAdditionalObjects().size(); ++i){
					if (player2->getCharacterAdditionalObjects().at(i).applyShadow){
						lightManager->removeShadowFromNode(player2->getCharacterAdditionalObjectsNodes().at(i));
					}
				}
			}
			else{
				for (int i = 0; i < player1->getCharacterAdditionalObjects().size(); ++i){
					if (player1->getCharacterAdditionalObjects().at(i).applyShadow){
						lightManager->removeShadowFromNode(player1->getCharacterAdditionalObjectsNodes().at(i));
					}
				}
			}
			processingTimeCounterMs = 0;
			processingWinText = (databaseAccessor.showRoundWinScreen() && additionalOptions.showRoundWinScreen) || isEndOfMatch();
			canPlayAnnouncerVoice = processingWinText;
			if (!processingWinText){
				newRoundFlag = true;
			}
		}
		else{
			if (isEndOfMatch()){
				processingWinText = true;
				canPlayAnnouncerVoice = true;
				processingTimeCounterMs = 0;
			}
			else{
				processingWinText = false;
				if (winnerId != 0){
					processingSceneEnd = true;
				}
				else{
					newRoundFlag = true;
				}
			}
		}
	}

	/* process match end */
	void FK_SceneGame::processMatchEnd(){
		processingSceneEnd = true;
		abortMatch = true;
	}

	/* show round diamonds indicators */
	bool FK_SceneGame::showRoundIndicatorsHUD(){
		return true;
	}

	/* update generic player input */
	void FK_SceneGame::updateGenericPlayerInput(FK_Character* character, FK_InputBuffer* buffer, 
		s32 joypadId, u32 delta_t_ms, bool &newMove) {
		if (joystickInfo.size() > joypadId) {
			buffer->update(nowReal, inputReceiver->JoypadStatus(joypadId));
		}
		else {
			buffer->update(nowReal, inputReceiver->KeyboardStatus());
		}
		if (newMove == false && character != NULL) {
			newMove = character->performMove(buffer->readBuffer(character));
		}
		buffer->resetModified();
	}

	/* update player 1 input */
	void FK_SceneGame::updatePlayer1Input(u32 delta_t_ms, bool & newMove){
		if (player1isAI){
			if (!cinematicCameraActivePlayer2) {
				newMove = updateAIInput(delta_t_ms, 1);
				if (newMove &&
					player1->getCurrentMove()->canBeTriggered() &&
					(getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatTriggerFest) != 0) {
					player1->setTriggerModeFlag();
				}
			}
		}
		else{
			if (joystickInfo.size() > 0){
				player1input->update(nowReal, inputReceiver->JoypadStatus(0));
			}
			else{
				player1input->update(nowReal, inputReceiver->KeyboardStatus());
			}
			if (player1input->isModified() && !cinematicCameraActivePlayer2){
				// first check for followup moves, if the array is not empty
				if (newMove == false){
					player1->updateTriggeringFlag(player1input->getPressedButtons());
					newMove = player1->performMove(player1input->readBuffer(player1));
					if (newMove && 
						player1->getCurrentMove()->canBeTriggered() &&
						(getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatTriggerFest) != 0) {
						player1->setTriggerModeFlag();
					}
				}
				player1input->resetModified();
			}
		}
	};

	/* update player 2 input*/
	void FK_SceneGame::updatePlayer2Input(u32 delta_t_ms, bool & newMove) {
		if (player2isAI) {
			if (!cinematicCameraActivePlayer1) {
				newMove = updateAIInput(delta_t_ms, 2);
				if (newMove &&
					player2->getCurrentMove()->canBeTriggered() &&
					(getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatTriggerFest) != 0) {
					player2->setTriggerModeFlag();
				}
			}
		}
		else {
			if (joystickInfo.size() > 1) {
				player2input->update(nowReal, inputReceiver->JoypadStatus(1));
			}
			else {
				player2input->update(nowReal, inputReceiver->KeyboardStatus());
			}
			if (player2input->isModified() && !cinematicCameraActivePlayer1) {
				if (newMove == false) {
					player2->updateTriggeringFlag(player2input->getPressedButtons());
					newMove = player2->performMove(player2input->readBuffer(player2));
					if (newMove &&
						player2->getCurrentMove()->canBeTriggered() &&
						(getCurrentActiveCheatCodes() & FK_Cheat::FK_CheatType::CheatTriggerFest) != 0) {
						player2->setTriggerModeFlag();
					}
				}
				player2input->resetModified();
			}
		}
	};

	/* get elapsed time */
	u32 FK_SceneGame::getElapsedTimeMS() {
		if (matchStartingTime < 0) {
			return 0;
		}
		return now - (u32)matchStartingTime;
	}

	/* get sound manager */
	FK_SoundManager* FK_SceneGame::getSoundManager() {
		return battleSoundManager;
	}

	FK_SceneGame::FK_AdditionalSceneGameOptions FK_SceneGame::getAdditionalOptions()
	{
		return additionalOptions;
	}

	/* initialize free camera mode */
	void FK_SceneGame::activateFreeCameraMode()
	{
		freeCameraModeActive = true;
		freeCamera->setPosition(camera->getPosition());
		freeCamera->setTarget(camera->getTarget());
		freeCamera->setRotation(camera->getRotation());
		freeCameraParameters = FK_FreeCameraParameters();
		freeCameraParameters.lastCameraTimeUpdateMs = device->getTimer()->getRealTime();
		smgr->setActiveCamera(freeCamera);
		f32 initialDistance = 0.f;
		setFreeCameraPosition(initialDistance, 0.f, 0.f);
	}

	/* update free camera mode */
	void FK_SceneGame::updateFreeCameraPosition(u32 pressedButtons, u32 delta_t_ms) {
		smgr->setActiveCamera(freeCamera);
		f32 delta_t_s = (f32)delta_t_ms / 1000.f;
		u32 directions = pressedButtons & FK_Input_Buttons::Any_Direction;
		u32 menuButtons = pressedButtons & (FK_Input_Buttons::Any_MenuButton | FK_Input_Buttons::Menu_Pause_Button);
		f32 newDistance = freeCameraParameters.currentCameraDistance;
		f32 newAngle = freeCameraParameters.currentCameraAngle;
		f32 newOffsetY = freeCameraParameters.currentCameraOffsetY;
		if (freeCameraSnap) {
			freeCameraSnapCounter += delta_t_ms;
			if (freeCameraCanTakePicture) {
				freeCameraSnap = false;
				freeCameraCanTakePicture = false;
				saveFreeCameraScreenshot();
				Sleep(100);
				return;
			}
		}
		if (!freeCameraSnap && (menuButtons & FK_Input_Buttons::Selection_Button) != 0) {
			battleSoundManager->playSound("camera", 100.0 * gameOptions->getSFXVolume());
			freeCameraSnap = true;
			freeCameraSnapCounter = 0;
			return;
		}
		if ((menuButtons & FK_Input_Buttons::Help_Button) != 0) {
			battleSoundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
			freeCameraParameters.targetIndex += 1;
			if (freeCameraParameters.targetIndex > 1) {
				freeCameraParameters.targetIndex = -1;
			}
			Sleep(200);
			return;
		}
		if ((menuButtons & FK_Input_Buttons::Modifier_Button) != 0) {
			if ((directions & FK_Input_Buttons::Up_Direction) != 0) {
				freeCameraParameters.currentCameraTargetOffsetY += 
					freeCameraParameters.deltaCameraOffsetYPerSecond * delta_t_s;
			}
			else if ((directions & FK_Input_Buttons::Down_Direction) != 0) {
				freeCameraParameters.currentCameraTargetOffsetY -= 
					freeCameraParameters.deltaCameraOffsetYPerSecond * delta_t_s;
			}
		}
		else {
			if ((directions & FK_Input_Buttons::Right_Direction) != 0) {
				newAngle -= freeCameraParameters.deltaCameraAnglePerSecond * delta_t_s;
			}
			else if ((directions & FK_Input_Buttons::Left_Direction) != 0) {
				newAngle += freeCameraParameters.deltaCameraAnglePerSecond * delta_t_s;
			}
			if ((directions & FK_Input_Buttons::Up_Direction) != 0) {
				newOffsetY += freeCameraParameters.deltaCameraOffsetYPerSecond * delta_t_s;
				newOffsetY = core::clamp(newOffsetY,
					freeCameraParameters.minimalYOffset,
					freeCameraParameters.maximalYOffset);
			}
			else if ((directions & FK_Input_Buttons::Down_Direction) != 0) {
				newOffsetY -= freeCameraParameters.deltaCameraOffsetYPerSecond * delta_t_s;
				newOffsetY = core::clamp(newOffsetY,
					freeCameraParameters.minimalYOffset,
					freeCameraParameters.maximalYOffset);
			}
		}
		if ((menuButtons & FK_Input_Buttons::ScrollLeft_Button) != 0) {
			newDistance += freeCameraParameters.deltaCameraDistancePerSecond * delta_t_s;
		}
		else if ((menuButtons & FK_Input_Buttons::ScrollRight_Button) != 0) {
			newDistance -= freeCameraParameters.deltaCameraDistancePerSecond * delta_t_s;
		}
		if ((menuButtons & (FK_Input_Buttons::Cancel_Button | FK_Input_Buttons::Menu_Pause_Button)) != 0){
			battleSoundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			freeCameraModeActive = false;
			smgr->setActiveCamera(camera);
			Sleep(500);
			return;
		}
		setFreeCameraPosition(newDistance, newAngle, newOffsetY, freeCameraParameters.currentCameraTargetOffsetY);
	}

	/* reset camera position */
	void FK_SceneGame::setFreeCameraPosition(f32 &distance, f32 new_angle, f32 offsetY, f32 targetOffsetY) {
		core::vector3d<f32> targetPosition = camera->getTarget();
		if (freeCameraParameters.targetIndex == -1) {
			targetPosition = player1->getPosition();
		}
		else if (freeCameraParameters.targetIndex == 1) {
			targetPosition = player2->getPosition();
		}
		f32 basicAngle = (camera->getPosition() - camera->getTarget()).getHorizontalAngle().Y;
		core::vector3df deltaD = camera->getPosition() - camera->getTarget();
		f32 basicOffset = deltaD.Y;
		deltaD = camera->getPosition() - targetPosition;
		deltaD.Y = 0;
		f32 basicDistance = deltaD.getLength();
		core::vector3df cameraPosition = targetPosition;
		f32 totalDistance = basicDistance + distance;
		if (totalDistance < freeCameraParameters.minimalCameraDistance) {
			distance = freeCameraParameters.minimalCameraDistance - basicDistance;
		}
		else if (totalDistance > freeCameraParameters.maximalCameraDistance) {
			distance = freeCameraParameters.maximalCameraDistance - basicDistance;
		}
		f32 cameraDistanceFromPlayer = basicDistance + distance;
		f32 angle = basicAngle + new_angle;
		f32 cameraOffsetYPositionFromTarget = basicOffset + offsetY;
		cameraPosition.X += sin(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Z += cos(angle*core::DEGTORAD) * cameraDistanceFromPlayer;
		cameraPosition.Y += cameraOffsetYPositionFromTarget;
		//cameraPosition.Z 
		freeCamera->setPosition(cameraPosition);
		targetPosition.Y += targetOffsetY;
		freeCamera->setTarget(targetPosition);
		freeCameraParameters.currentCameraAngle = new_angle;
		freeCameraParameters.currentCameraDistance = distance;
		freeCameraParameters.currentCameraOffsetY = offsetY;
		freeCameraParameters.currentCameraTargetOffsetY = targetOffsetY;
	}

	/* save free camera screenshot */
	void FK_SceneGame::saveFreeCameraScreenshot()
	{
		video::IImage* image = device->getVideoDriver()->createScreenShot();
		if (image)
		{
			time_t     now = time(0);
			struct tm  tstruct;
			//char       date[80];
			localtime_s(&tstruct, &now);
			std::string filename = "/screenshots/schwb_pic" +
				std::to_string(1900 + tstruct.tm_year) +
				(tstruct.tm_mon < 10 ? "0" : std::string()) +
				std::to_string(tstruct.tm_mon) +
				(tstruct.tm_mday < 10 ? "0" : std::string()) +
				std::to_string(tstruct.tm_mday) + "_" +
				(tstruct.tm_hour < 10 ? "0" : std::string()) +
				std::to_string(tstruct.tm_hour) +
				(tstruct.tm_min < 10 ? "0" : std::string()) +
				std::to_string(tstruct.tm_min) +
				(tstruct.tm_sec < 10 ? "0" : std::string()) +
				std::to_string(tstruct.tm_sec);
			std::string path = applicationPath + filename;
			bool result = device->getVideoDriver()->writeImageToFile(image, (path + ".png").c_str());
			//result = device->getVideoDriver()->writeImageToFile(image, (path + ".jpg").c_str(), 100);
			/*result = device->getVideoDriver()->writeImageToFile(image, (applicationPath + "/screenshots/test.jpg").c_str(), 100);
			result = device->getVideoDriver()->writeImageToFile(image, (applicationPath + "/screenshots/test.png").c_str(), 100);*/
			image->drop();
		}
	}

	/* update save file data */
	void FK_SceneGame::updateSaveFileData(){
		// update save file data for any human-controlled player
		std::string key1 = player1->getOriginalName();
		std::replace(key1.begin(), key1.end(), ' ', '_');
		std::string key2 = player2->getOriginalName();
		std::replace(key2.begin(), key2.end(), ' ', '_');
		if (!player1isAI){
			characterStatisticsStructure[key1].numberOfFreeMatchesPlayed += 1;
		}
		if (!player2isAI){
			characterStatisticsStructure[key2].numberOfFreeMatchesPlayed += 1;
		}
		if (winnerId == -1){
			if (!player1isAI){
				characterStatisticsStructure[key1].numberOfMatchesWon += 1;
				characterStatisticsStructure[key1].VsRecords[key2].numberOfMatchesWon += 1;
			}
			if (!player2isAI){
				characterStatisticsStructure[key2].numberOfMatchesLost += 1;
				characterStatisticsStructure[key2].VsRecords[key1].numberOfMatchesLost += 1;
			}
		}
		else if (winnerId == 1){
			if (!player2isAI){
				characterStatisticsStructure[key2].numberOfMatchesWon += 1;
				characterStatisticsStructure[key2].VsRecords[key1].numberOfMatchesWon += 1;
			}
			if (!player1isAI){
				characterStatisticsStructure[key1].numberOfMatchesLost += 1;
				characterStatisticsStructure[key1].VsRecords[key2].numberOfMatchesLost += 1;
			}
		}
		writeSaveFile();
	}
}