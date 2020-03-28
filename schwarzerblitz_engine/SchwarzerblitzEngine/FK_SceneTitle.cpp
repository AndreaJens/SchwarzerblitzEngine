#include"FK_SceneTitle.h"
#include"ExternalAddons.h"
#include<iostream>

using namespace irr;

namespace fk_engine{
	FK_SceneTitle::FK_SceneTitle(){
		initialize();
	};
	FK_SceneTitle::FK_SceneTitle(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions){
		initialize();
		setup(newDevice, new_joypadInfo, newOptions);
	};
	void FK_SceneTitle::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions){
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper(false);
		resourcesPath = mediaPath + FK_SceneTitle::TitleFilesDirectory;
		configFileName = resourcesPath + FK_SceneTitle::TitleConfigFileName;
		readConfigFile();
		setupBGM();
		setupSoundManager();
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		setNextScene(FK_SceneType::SceneMainMenu);
		inputReceiver->reset();
		readSaveFile();
	};

	void FK_SceneTitle::initialize(){
		now = 0;
		then = 0;
		delta_t_ms = 0;
		cycleId = 0;
		anyKeyPressed = false;
		cheatFlashEffect = false;
		captionTimeMS = 0;
		captionPosition = core::position2df(0.5, 0.5);
		captionText = L"Press Start";
		captionPeriodDurationMS = 1000;
		captionColor = video::SColor(255,255,255,255);
		titleBGMName = std::string();
		titleBGMvolume = 100.0f;
		titleBGMpitch = 1.0f;
		transitionCounter = 0;
		flashCounter = 0;
		demoMatchTimerMs = 0;
		timeUntilDemoMatchMs = 15000;
		allowAttractMode = false;
	};

	void FK_SceneTitle::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		if (cycleId == 0){
			cycleId = 1;
			delta_t_ms = 0;
			if (titleBGMName != std::string()){
				titleBGM.play();
			}
		}
		else{
			captionTimeMS += delta_t_ms;
			if (!isInputingCheat() && demoMatchTimerMs > FK_SceneTitle::timeUntilDemoMatchMs) {
				transitionCounter += delta_t_ms;
				if (!anyKeyPressed) {
					setNextScene(FK_SceneType::SceneGameAttractMode);
					anyKeyPressed = true;
				}
			}
			else if (anyKeyPressed) {
				if (flashCounter > FK_SceneTitle::FlashTimeMs) {
					transitionCounter += delta_t_ms;
				}
				else {
					flashCounter += delta_t_ms;
				}
			}
			else if (cheatFlashEffect) {
				if (flashCounter > FK_SceneTitle::FlashTimeMs) {
					cheatFlashEffect = false;
					flashCounter = 0;
				}
				else {
					flashCounter += delta_t_ms;
				}
			}
			else{
				updateInput();
				demoMatchTimerMs += delta_t_ms;
			}
			draw();
		}
	};

	void FK_SceneTitle::draw(){
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		drawBackground();
		drawCaption();
		if (anyKeyPressed || cheatFlashEffect){
			drawOverlay();
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	};

	void FK_SceneTitle::dispose(){
		// stop BGM, if playing
		if (titleBGMName != std::string()){
			titleBGM.stop();
		}
		// delete sound manager
		delete soundManager;
		FK_SceneWithInput::dispose();
	};

	bool FK_SceneTitle::isRunning(){
		return !(anyKeyPressed && transitionCounter > FK_SceneTitle::TransitionTimeMS);
	};
	FK_Scene::FK_SceneType FK_SceneTitle::nameId(){
		return FK_SceneType::SceneTitle;
	};
	void FK_SceneTitle::updateInput(){
		bool inputCheat = isInputingCheat();
		if (joystickInfo.size() > 0){
			player1input->update(now, inputReceiver->JoypadStatus(0), inputCheat);
		}
		else{
			player1input->update(now, inputReceiver->KeyboardStatus(), inputCheat);
		}
		u32 buttonsPressed = player1input->getPressedButtons();
		if (joystickInfo.size() > 1){
			player2input->update(now, inputReceiver->JoypadStatus(1), inputCheat);
		}
		else{
			player2input->update(now, inputReceiver->KeyboardStatus(), inputCheat);
		}
		inputCheat |= isInputingCheat();
		if (inputCheat) {
			bool success = updateCheatsCheck();
			if (success) {
				if(getCurrentActiveCheatCodes() == 0){
					soundManager->playSound("cheat_clear", 100.0 * gameOptions->getSFXVolume());
				}
				else {
					soundManager->playSound("cheat", 100.0 * gameOptions->getSFXVolume());
				}
				player1input->setBuffer(std::deque<u32>());
				player2input->setBuffer(std::deque<u32>());
				flashCounter = 0;
				cheatFlashEffect = true;
				return;
			}
		}
		else {
			buttonsPressed |= player2input->getPressedButtons();
			buttonsPressed &= ~FK_Input_Buttons::Any_Direction;
			if (buttonsPressed > 0) {
				anyKeyPressed = true;
				soundManager->playSound("trigger", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
			}
		}
	};

	void FK_SceneTitle::setupInputForPlayers() {
		s32 maxNumberOfRecordedButtonInInputBuffer = 35;
		s32 timeWindowBetweenInputBeforeClearing_ms = 1000;
		s32 mergeTimeForInputAtSameFrame_ms = 20;
		s32 timeToRegisterHeldButton_ms = 200; //it was originally 40.
		s32 timeToRegisterNullInput_ms = 1200;
		FK_SceneWithInput::setupInputForPlayers(timeWindowBetweenInputBeforeClearing_ms,
			mergeTimeForInputAtSameFrame_ms, timeToRegisterHeldButton_ms,
			timeToRegisterNullInput_ms, maxNumberOfRecordedButtonInInputBuffer);
	}


	void FK_SceneTitle::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("trigger", "trigger.ogg");
		soundManager->addSoundFromDefaultPath("cheat", "cheat_unlock.ogg");
		soundManager->addSoundFromDefaultPath("cheat_clear", "cheat_clear.ogg");
	};

	void FK_SceneTitle::setupBGM(){
		std::string titleResourcesPath = mediaPath + FK_SceneTitle::TitleFilesDirectory;
		std::string filePath = titleResourcesPath + titleBGMName;
		if (!titleBGM.openFromFile(filePath.c_str())){
			titleBGMName = std::string();
			return;
		}
		titleBGM.setLoop(true);
		titleBGM.setVolume(titleBGMvolume * gameOptions->getMusicVolume());
		titleBGM.setPitch(titleBGMpitch);
	};

	void FK_SceneTitle::readConfigFile(){
		std::ifstream inputFile((mediaPath + TitleFilesDirectory + TitleConfigFileName).c_str());
		std::string temp;
		while (inputFile >> temp){
			if (temp == FK_SceneTitle::BackgroundPictureFileKey){
				inputFile >> temp;
				backgroundTex = driver->getTexture((mediaPath + TitleFilesDirectory + temp).c_str());
			}
			else if (temp == FK_SceneTitle::BGMFileKey){
				inputFile >> titleBGMName >> titleBGMvolume >> titleBGMpitch;
			}
			else if (temp == FK_SceneTitle::AttractModeFileKey) {
				u32 x;
				inputFile >> x;
				allowAttractMode = x > 0;
				inputFile >> timeUntilDemoMatchMs;
			}
			else if (temp == FK_SceneTitle::CaptionColorFileKey){
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				captionColor = video::SColor(a, r, g, b);
			}
			else if (temp == FK_SceneTitle::CaptionPeriodFileKey){
				u32 time;
				inputFile >> time;
				captionPeriodDurationMS = time;
			}
			else if (temp == FK_SceneTitle::CaptionPositionFileKey){
				f32 x, y;
				inputFile >> x >>y;
				captionPosition = core::position2df(x, y);
			}
			else if (temp == FK_SceneTitle::CaptionTextFileKey){
				std::string line;
				while (line.empty()){
					std::getline(inputFile, line);
				};
				captionText = std::wstring(line.begin(), line.end());
			}
		}
	};

	void FK_SceneTitle::drawBackground(){
		core::dimension2d<u32> frameSize = backgroundTex->getSize();
		core::rect<s32> sourceRect(0, 0, frameSize.Width, frameSize.Height);
		irr::core::vector2df scale((f32)((f32)screenSize.Width / (f32)frameSize.Width),
			(f32)((f32)screenSize.Height / (f32)frameSize.Height));
		video::SColor color(255, 255, 255, 255);
		fk_addons::draw2DImage(driver, backgroundTex, sourceRect, core::vector2d<s32>(0, 0),
			core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
	}

	void FK_SceneTitle::drawCaption(){
		s32 x1, y1, x2, y2;
		core::dimension2d<u32> captionSize = font->getDimension(captionText.c_str());
		x1 = (s32)floor((f32)screenSize.Width * captionPosition.X - captionSize.Width / 2);
		y1 = (s32)floor((f32)screenSize.Height * captionPosition.Y - captionSize.Height / 2);
		x2 = x1 + captionSize.Width;
		y2 = y1 + captionSize.Height;
		core::rect<s32> destinationRect(x1, y1, x2, y2);
		s32 alpha = (s32)floor((f32)captionColor.getAlpha() *
			(f32)(128 + 128 * cos(2 * core::PI * (f32)captionTimeMS / (f32)captionPeriodDurationMS)) / 255.0f);
		alpha = core::clamp(alpha, 1, 255);
		if (flashCounter > 0){
			alpha = 255;
		}
		video::SColor color = captionColor;
		color.setAlpha(alpha);
		video::SColor bordercolor = (255, 0, 0, 0);
		bordercolor.setAlpha(alpha);
		fk_addons::drawBorderedText(font, captionText, destinationRect, color, bordercolor);
	}

	void FK_SceneTitle::drawOverlay(){
		core::rect<s32> destRect(0, 0, screenSize.Width, screenSize.Height);
		if (transitionCounter > 0){
			s32 alpha = (s32)ceil(255.0f * (f32)transitionCounter / (f32)FK_SceneTitle::TransitionTimeMS);
			alpha = core::clamp(alpha, 1, 255);
			video::SColor color(alpha, 0, 0, 0);
			driver->draw2DRectangle(color, destRect);
		}
		else if (flashCounter > 0){
			s32 alpha = (s32)floor((f32)(128 - 128 * cos(2 * core::PI * (f32)flashCounter / (f32)FK_SceneTitle::FlashTimeMs)));
			alpha = core::clamp(alpha, 1, 255);
			video::SColor color(alpha, 255, 255, 255);
			driver->draw2DRectangle(color, destRect);
		}
	}


}
