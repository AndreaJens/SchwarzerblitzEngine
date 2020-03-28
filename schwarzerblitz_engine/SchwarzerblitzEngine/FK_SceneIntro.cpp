#include"FK_SceneIntro.h"
#include"ExternalAddons.h"
#include<fstream>
#include<iostream>

using namespace irr;

namespace fk_engine{

	FK_SceneIntro::FK_SceneIntro(){
		initialize();
	}
	FK_SceneIntro::FK_SceneIntro(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions){
		initialize();
		setup(newDevice, new_joypadInfo, newOptions);
	}
	void FK_SceneIntro::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions){
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper(false);
		resourcesPath = mediaPath + FK_SceneIntro::IntroFilesDirectory;
		configFileName = resourcesPath + FK_SceneIntro::IntroConfigFileName;
		FK_SceneWithInput::readSaveFile();
		readFramesFromFile();
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		setNextScene(FK_SceneType::SceneTitle);
		currentFramePhase = FK_SceneIntroFramePhase::IFP_fadein;
	}

	void FK_SceneIntro::initialize(){
		now = 0;
		then = 0;
		delta_t_ms = 0;
		frames.clear();
		currentFrameIndex = 0;
		currentFrameTime = 0;
		currentFrameOpacity = 0;
		currentFramePhase = FK_SceneIntroFramePhase::IFP_hidden;
		cycleId = 0;
	}

	void FK_SceneIntro::updateInput(){
		u32 inputButtons = 0;
		if (joystickInfo.size() > 0){
			player1input->update(now, inputReceiver->JoypadStatus(0));
		}
		else{
			player1input->update(now, inputReceiver->KeyboardStatus());
		}
		if (joystickInfo.size() > 1){
			player2input->update(now, inputReceiver->JoypadStatus(1));
		}
		else{
			player2input->update(now, inputReceiver->KeyboardStatus());
		}
		inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		inputButtons &= ~FK_Input_Buttons::Any_Direction;
		// skip frame
		if (inputButtons != 0 && frames[currentFrameIndex].canBeSkipped){
			inputReceiver->reset();
			if (currentFramePhase == FK_SceneIntroFramePhase::IFP_show){
				currentFrameTime = 0;
			}
			currentFramePhase = FK_SceneIntroFramePhase::IFP_fadeout;
		}
	}

	void FK_SceneIntro::update(){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		if (cycleId == 0){
			cycleId = 1;
			delta_t_ms = 0;
		}
		else{
			updateInput();
			switch (currentFramePhase){
			case FK_SceneIntroFramePhase::IFP_fadein:
				currentFrameTime += delta_t_ms;
				currentFrameOpacity = 255.0f * (f32)currentFrameTime / (f32)frames[currentFrameIndex].fadeInDuration_ms;
				if (currentFrameTime >= frames[currentFrameIndex].fadeInDuration_ms){
					currentFramePhase = FK_SceneIntroFramePhase::IFP_show;
					currentFrameTime = 0;
				}
				break;
			case FK_SceneIntroFramePhase::IFP_show:
				currentFrameTime += delta_t_ms;
				currentFrameOpacity = 255.0f;
				if (currentFrameTime >= frames[currentFrameIndex].duration_ms){
					currentFramePhase = FK_SceneIntroFramePhase::IFP_fadeout;
					currentFrameTime = 0;
				}
				break;
			case FK_SceneIntroFramePhase::IFP_fadeout:
				currentFrameTime += delta_t_ms;
				currentFrameOpacity = 255.0f * (1.0f - (f32)currentFrameTime / (f32)frames[currentFrameIndex].fadeOutDuration_ms);
				if (currentFrameTime >= frames[currentFrameIndex].fadeOutDuration_ms){
					currentFramePhase = FK_SceneIntroFramePhase::IFP_fadein;
					currentFrameIndex += 1;
					currentFrameTime = 0;
					if (currentFrameIndex < frames.size() && frames[currentFrameIndex].isVideoFrame) {
						frames[currentFrameIndex].videoFile = new TMovie;
						frames[currentFrameIndex].videoFile->LoadMovie(
							frames[currentFrameIndex].resourceFileName.c_str()
						);
						irr::core::dimension2du frameSize =
							irr::core::dimension2du(frames[currentFrameIndex].videoFile->MovieWidth(), frames[currentFrameIndex].videoFile->MovieHeight());
						//frames[currentFrameIndex].frameTexture = 
						//	driver->addRenderTargetTexture(frameSize, "RTTVideoPl");
						//driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, TRUE);
						driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, FALSE);
						frames[currentFrameIndex].frameTexture =
							driver->addTexture(frameSize, "imovie", video::ECF_A8R8G8B8);
						driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, TRUE);
						//frames[currentFrameIndex].videoFile->SetMovieFPS(frames[currentFrameIndex].fps);
					}
				}
				break;
			default:
				break;
			}
			draw();
		}
	}
	void FK_SceneIntro::draw(){
		
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		if (isRunning()){
			if (frames[currentFrameIndex].isVideoFrame && frames[currentFrameIndex].videoFile != NULL) {
				frames[currentFrameIndex].videoFile->NextMovieFrame();
				frames[currentFrameIndex].videoFile->DrawMovie(0, 0, frames[currentFrameIndex].frameTexture);
			}

			//driver->setRenderTarget(borderlessWindowRenderTarget, true, true, video::SColor(0, 0, 0, 255));

			core::dimension2d<u32> frameSize = frames[currentFrameIndex].frameTexture->getSize();
			core::rect<s32> sourceRect(0, 0, frameSize.Width, frameSize.Height);
			f32 scaleX = (f32)screenSize.Width / (f32)frameSize.Width;
			f32 scaleY = (f32)screenSize.Height / (f32)frameSize.Height;
			f32 scaleFactor = min(scaleX, scaleY);
			irr::core::vector2df scale(scaleFactor, scaleFactor);
			s32 frameOpacity = core::clamp((s32)ceil(currentFrameOpacity), 0, 255);
			s32 x = ((s32)screenSize.Width  - (s32)std::floor((f32)frameSize.Width  * scaleFactor)) / 2;
			s32 y = ((s32)screenSize.Height - (s32)std::floor((f32)frameSize.Height * scaleFactor)) / 2;
			video::SColor color(255, frameOpacity, frameOpacity, frameOpacity);
			//video::SColor color(255, 255, 255, 255);
			//driver->draw2DImage(frames[currentFrameIndex].frameTexture, core::vector2d<s32>(x, y));
			fk_addons::draw2DImage(driver, frames[currentFrameIndex].frameTexture, sourceRect, core::vector2d<s32>(x, y),
				core::vector2d<s32>(0, 0), 0, scale, false, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			//driver->setRenderTarget(0, true, true, video::SColor(0, 0, 0, 255));
			//driver->draw2DImage(borderlessWindowRenderTarget, core::vector2d<s32>(0, 0));
		
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}
	void FK_SceneIntro::dispose(){
		// clear the scene of all scene nodes
		for (int i = 0; i < frames.size(); ++i) {
			if (frames[i].isVideoFrame && frames[i].videoFile != NULL) {
				delete frames[i].videoFile;
			}
		}
		FK_SceneWithInput::dispose();
	}

	bool FK_SceneIntro::isRunning(){
		return (frames.size() == 0 || currentFrameIndex < frames.size());
	}
	FK_Scene::FK_SceneType FK_SceneIntro::nameId(){
		return FK_SceneType::SceneIntro;
	}
	void FK_SceneIntro::readFramesFromFile(){
		std::ifstream inputFile(configFileName.c_str());
		std::string temp;
		introFrame tempFrame;
		tempFrame.canBeSkipped = false;
		tempFrame.duration_ms = 1;
		tempFrame.fadeInDuration_ms = 1;
		tempFrame.fadeOutDuration_ms = 1;
		tempFrame.frameTexture = NULL;
		tempFrame.videoFile = NULL;
		tempFrame.isVideoFrame = false;
		tempFrame.resourceFileName = std::string();
		tempFrame.fps = 0;
		while (inputFile >> temp){
			if (temp == FK_SceneIntro::NewFrameFileKey){
				inputFile >> temp;
				tempFrame.frameTexture = driver->getTexture((resourcesPath + temp).c_str());
			}
			else if (temp == FK_SceneIntro::NewVideoFrameKey) {
				tempFrame.isVideoFrame = true;
				inputFile >> temp;
				tempFrame.resourceFileName = resourcesPath + temp;
			}
			else if (temp == FK_SceneIntro::VideoFrameFPS) {
				if (tempFrame.isVideoFrame) {
					u32 fpsValue = 0;
					inputFile >> fpsValue;
					tempFrame.fps = std::floor(1000.f / fpsValue);
				}
			}
			else if (temp == FK_SceneIntro::FadeInTimeMSFileKey){
				u32 time;
				inputFile >> time;
				tempFrame.fadeInDuration_ms = time;
			}
			else if (temp == FK_SceneIntro::FadeOutTimeMSFileKey){
				u32 time;
				inputFile >> time;
				tempFrame.fadeOutDuration_ms = time;
			}
			else if (temp == FK_SceneIntro::DurationTimeMSFileKey){
				u32 time;
				inputFile >> time;
				tempFrame.duration_ms = time;
			}
			else if (temp == FK_SceneIntro::SkippableFileKey){
				bool flag;
				inputFile >> flag;
				tempFrame.canBeSkipped = flag;
			}
			else if (temp == FK_SceneIntro::EndFrameFileKey){
				frames.push_back(tempFrame);
				tempFrame.canBeSkipped = false;
				tempFrame.duration_ms = 1;
				tempFrame.fadeInDuration_ms = 1;
				tempFrame.fadeOutDuration_ms = 1;
				tempFrame.frameTexture = NULL;
				tempFrame.videoFile = NULL;
				tempFrame.isVideoFrame = false;
				tempFrame.resourceFileName = std::string();
				tempFrame.fps = 0;
			}
		}
	}
}