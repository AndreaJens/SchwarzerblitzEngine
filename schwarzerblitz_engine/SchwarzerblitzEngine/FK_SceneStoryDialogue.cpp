/*
	*** Schwarzerblitz 3D Fighting Game Engine  ***

	=================== Source Code ===================
	Copyright (C) 2016-2022 Andrea Demetrio

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation and/or
	   other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may be
	   used to endorse or promote products derived from  this software without specific
	   prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	=============== Additional Components ==============
	Please refer to the license/irrlicht/ and license/SFML/ folder for the license
	indications concerning those components. The irrlicht-schwarzerlicht engine and
	the SFML code and binaries are subject to their own licenses, see the relevant
	folders for more information.

	=============== Assets and resources ================
	Unless specificed otherwise in the Credits file, the assets and resources
	bundled with this engine are to be considered "all rights reserved" and
	cannot be redistributed without the owner's consent. This includes but it is
	not limited to the characters concepts / designs, the 3D models, the music,
	the sound effects, 2D and 3D illustrations, stages, icons, menu art.

	Tutorial Man, Evil Tutor, and Random:
	Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

#include"FK_SceneStoryDialogue.h"
#include"ExternalAddons.h"
#include<iostream>

using namespace irr;

namespace fk_engine{

	FK_SceneStoryDialogue::FK_SceneStoryDialogue(){
		initialize();
	}
	FK_SceneStoryDialogue::FK_SceneStoryDialogue(
		IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions, 
		FK_StoryEvent* newStoryEvent) :
		FK_SceneWithInput(newDevice, new_joypadInfo, newOptions){
		initialize();
		setup(newDevice, new_joypadInfo, newOptions, newStoryEvent);
	}
	
	void FK_SceneStoryDialogue::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions,
		FK_StoryEvent* newStoryEvent){
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper(false);
		storyEvent = newStoryEvent;
		if (storyEvent->playCredits){
			setNextScene(FK_Scene::FK_SceneType::SceneCredits);
		}
		else if (storyEvent->returnToStoryMenu){
			setNextScene(FK_Scene::FK_SceneType::SceneStoryModeSelection);
		}
		else{
			setNextScene(nameId());
		}
		setWindows();
		setupSoundManager();
		resourcesPath = mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory;
		stagesPath = mediaPath + "stages\\";
		charactersPath = mediaPath + "characters\\";
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		episodeResourceDirectory = storyEvent->storyDirectory;
		configFileName = episodeResourceDirectory + storyEvent->configFileName;
		readFromFile(configFileName);
		setupInitialBackground();
		//setupInitialBGM();
		setupChapterMenu();
		FK_SceneWithInput::readSaveFile();
	}
	void FK_SceneStoryDialogue::initialize(){
		now = 0;
		then = 0;
		delta_t_ms = 0;
		cycleId = 0;
		dialogueIndex = -1;
		switchWindow = false;
		switchWindowTimer = 0;
		selection_timer_ms = 0;
		isPerformingOutro = false;
		isPerformingIntro = true;
		canSkipDialogue = false;
		currentBackground = std::string();
		endScene = false;
		validBGM = false;
		skippingScene = false;
		windows.clear();
	}


	// setup sound manager
	void FK_SceneStoryDialogue::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}

	void FK_SceneStoryDialogue::setWindows(){
		f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		core::rect<s32> frameTop(50, 10, 590, 90);
		core::rect<s32> frameBottom(50, 270, 590, 350);
		core::rect<s32> frameCenter(50, 140, 590, 220);
		FK_DialogueWindow* window1 = new FK_DialogueWindow(device, frameTop, "windowskin.png", commonResourcesPath, scaleFactor,
			std::string(), std::vector<std::string>(), FK_DialogueWindow::DialogueWindowAlignment::Right,
			FK_DialogueWindow::DialogueTextMode::CharByChar);
		FK_DialogueWindow* window2 = new FK_DialogueWindow(device, frameBottom, "windowskin.png", commonResourcesPath, scaleFactor,
			std::string(), std::vector<std::string>(), FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::CharByChar);
		FK_DialogueWindow* window3 = new FK_DialogueWindow(device, frameCenter, "windowskin.png", commonResourcesPath, scaleFactor,
			std::string(), std::vector<std::string>(), FK_DialogueWindow::DialogueWindowAlignment::Left,
			FK_DialogueWindow::DialogueTextMode::CharByChar);
		window1->close(1);
		window2->close(1);
		window3->close(1);
		windows.push_back(window1);
		windows.push_back(window2);
		windows.push_back(window3);
		window1->setAlpha(200);
		window2->setAlpha(200);
		window3->setAlpha(200);
	}

	void FK_SceneStoryDialogue::update(){
		if (cycleId == 0){
			now = device->getTimer()->getTime();
			nowReal = device->getTimer()->getRealTime();
			thenReal = nowReal;
			then = now;
			cycleId = 1;
			delta_t_ms = 0;
			setupInitialBGM();
			if (validBGM){
				if (!(bgm.getStatus() == bgm.Playing)){
					bgm.play();
				}
			}
		}
		else{
			now = device->getTimer()->getTime();
			nowReal = device->getTimer()->getRealTime();
			delta_t_ms = now - then;
			if (nowReal - thenReal >= 16) {
				then = now;
				thenReal = nowReal;
				// update typing sound
				typingSoundTimerMS += delta_t_ms;
				// update transition progress
				if (transitionPicture != NULL) {
					transitionTimerMS += delta_t_ms;
				}
				// update outro and intro
				if (isPerformingIntro || isPerformingOutro) {
					fadeoutTimerMS += delta_t_ms;
					for each (auto window in windows) {
						if (window->isClosing()) {
							window->update(delta_t_ms);
						}
					}
					if (isPerformingOutro) {
						f32 ratio = 1 - (f32)fadeoutTimerMS / (f32)FK_SceneStoryDialogue::FadeoutTimeMS;
						if (ratio < 0) {
							ratio = 0.f;
						}
						bgm.setVolume(currentBGM.bgmVolume * gameOptions->getMusicVolume() * ratio);
					}
					if (isPerformingIntro && fadeoutTimerMS > FK_SceneStoryDialogue::FadeinTimeMS) {
						fadeoutTimerMS = FK_SceneStoryDialogue::FadeinTimeMS;
						isPerformingIntro = false;
					}
					else if (isPerformingOutro && fadeoutTimerMS > FK_SceneStoryDialogue::FadeoutTimeMS) {
						fadeoutTimerMS = FK_SceneStoryDialogue::FadeoutTimeMS;
						isPerformingOutro = false;
						if (!(skippingScene && !storyEvent->saveWhenSkipped)) {
							if (storyEvent->saveStoryCompletion) {
								updateSaveFileData();
							}
							storyEvent->saveStoryCompletion = true;
						}
					}
				}
				else {
					fadeoutTimerMS = 0;
					if (chapterMenu->isActive()) {
						updateChapterMenu();
					}
					else {
						updateInput();
						if (switchWindow) {
							switchWindowTimer += delta_t_ms;
						}
						for each (auto window in windows) {
							window->update(delta_t_ms);
						}
					}
				}
				// update windows
				std::string mugshotPath;
				f32 scaleFactor = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
				u32 additionalTime = FK_SceneStoryDialogue::WindowActivityTimeMS;
				if (dialogueIndex >= 0 && dialogueIndex < (s32)dialogues.size()) {
					if (dialogues[dialogueIndex].ignoreBufferTime) {
						additionalTime = 0;
					}
					additionalTime += dialogues[dialogueIndex].autoProgress ?
						dialogues[dialogueIndex].additionalTimeAfterDialogue : 0;
				}
				if (!switchWindow) {
					if (dialogueIndex >= 0 &&
						dialogueIndex < dialogues.size() &&
						dialogues[dialogueIndex].autoProgress && (
							(windows[0]->isAllTextDisplayed() && windows[0]->isActive()) ||
							(windows[1]->isAllTextDisplayed() && windows[1]->isActive()) ||
							(windows[2]->isAllTextDisplayed() && windows[2]->isActive())
							)) {
						switchWindow = true;
						switchWindowTimer = 0;
					}
					else
						if (!windows[0]->isActive() && !windows[1]->isActive() && !windows[2]->isActive()) {
							switchWindow = true;
							switchWindowTimer = 0;
						}
				}
				// set new dialogue
				else if (switchWindowTimer > additionalTime) {
					canSkipDialogue = true;
					switchWindowTimer = 0;
					switchWindow = false;
					if (dialogueIndex < (s32)dialogues.size()) {
						dialogueIndex += 1;
					}
					if (dialogueIndex >= 1 && dialogues[dialogueIndex - 1].closeSingleWindowAfterDialogue) {
						if (dialogueIndex < (s32)dialogues.size() &&
							dialogues[dialogueIndex - 1].position != dialogues[dialogueIndex].position) {
							u32 windowIndex = (u32)(dialogues[dialogueIndex - 1].position);
							windows[windowIndex]->close(200);
						}
					}
					if (dialogueIndex >= 1 && dialogues[dialogueIndex - 1].closeWindowsAfterDialogue) {
						for each (auto window in windows) {
							window->close(500);
						}
					}
					if (dialogueIndex >= (s32)dialogues.size()) {
						for each (auto window in windows) {
							window->close(500);
						}
						if (!isPerformingOutro && !endScene) {
							//if (!(skippingScene && !storyEvent->saveWhenSkipped)) {
							//	updateSaveFileData();
							//}
							//storyEvent->saveStoryCompletion = true;
							isPerformingOutro = true;
							endScene = true;
							fadeoutTimerMS = 0;
						}
						return;
					}
					// update BGM
					setupBGM(dialogueIndex);
					// play sound effect
					setupSFX(dialogueIndex);
					// update background
					std::string backgroundPath =
						mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory + dialogues[dialogueIndex].backgroundTextureName;
					if (dialogues[dialogueIndex].takeBackgroundFromStagePreview) {
						backgroundPath = stagesPath + dialogues[dialogueIndex].backgroundTextureName + "preview.jpg";
					}
					if (dialogues[dialogueIndex].noBackgroundPicture) {
						backgroundPicture = NULL;
						currentBackground = std::string();
					}
					else {
						if (backgroundPath != currentBackground && !(backgroundPath == mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory)) {
							//delete backgroundPicture;
							backgroundPicture = driver->getTexture(backgroundPath.c_str());
							if (!backgroundPicture) {
								backgroundPath = episodeResourceDirectory +
									dialogues[dialogueIndex].backgroundTextureName;
								backgroundPicture = driver->getTexture(backgroundPath.c_str());
							}
							currentBackground = backgroundPath;
						}
					}
					std::string transitionPath =
						mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory + dialogues[dialogueIndex].backgroundTransitionTextureName;
					if (/*dialogues[dialogueIndex].autoProgress &&*/ !(transitionPath == mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory)) {
						transitionPicture = driver->getTexture(transitionPath.c_str());
						if (!transitionPicture) {
							transitionPath = episodeResourceDirectory +
								dialogues[dialogueIndex].backgroundTransitionTextureName;
							transitionPicture = driver->getTexture(transitionPath.c_str());
						}
						if (!transitionPicture) {
							transitionPicture = NULL;
						}
						transitionTimerMS = 0;
					}
					else {
						transitionPicture = NULL;
					}
					// update mugshot for dialogue
					if (dialogues[dialogueIndex].takeMugshotFromCharacterPreview) {
						mugshotPath = charactersPath + dialogues[dialogueIndex].mugshotFilename + "FightPreview.png";
					}
					else {
						mugshotPath = mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory +
							FK_SceneStoryDialogue::StoryFilesMugshotDirectory + dialogues[dialogueIndex].mugshotFilename;
						if (!driver->getTexture(mugshotPath.c_str())) {
							mugshotPath = episodeResourceDirectory +
								dialogues[dialogueIndex].mugshotFilename;
						}
					}
					for each (auto window in windows) {
						window->setActive(false);
					}
					u32 activeWinIndex = dialogues[dialogueIndex].position;
					FK_DialogueWindow::DialogueWindowAlignment alignment = FK_DialogueWindow::DialogueWindowAlignment::Left;
					if (activeWinIndex == StoryDialogue::WindowPosition::Top) {
						alignment = FK_DialogueWindow::DialogueWindowAlignment::Right;
					}
					windows[activeWinIndex]->setActive(true);
					windows[activeWinIndex]->setVisibility(true);
					windows[activeWinIndex]->setBackgroundVisibility(dialogues[dialogueIndex].isBackgroundVisible);
					windows[activeWinIndex]->reset(scaleFactor, mugshotPath, dialogues[dialogueIndex].dialogueText,
						alignment,
						FK_DialogueWindow::DialogueTextMode::CharByChar);
					if (windows[activeWinIndex]->isClosed()) windows[activeWinIndex]->open(100);
				}
			}
		}
		// draw all
		drawAll();
	}
	void FK_SceneStoryDialogue::drawAll(){
		f32 scaleFactorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		f32 scaleFactorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		drawBackground();
		if (isRunning()){
			for each(auto window in windows){
				if (window->isOpen() && window->isActive() && !window->isAllTextDisplayed()){
					if (typingSoundTimerMS > FK_SceneStoryDialogue::TypingSoundInterval){
						if (window->canPlaySound()){
							typingSoundTimerMS = 0;
							soundManager->playSound("cursor", 30.0 * gameOptions->getSFXVolume(), 0.6f);
							window->flagSoundAsPlayed();
						}
					}
				}
				bool showArrow = dialogueIndex >= 0 && 
					dialogueIndex < dialogues.size() && 
					!dialogues[dialogueIndex].autoProgress;
				window->draw(screenSize, scaleFactorX, scaleFactorY, showArrow);
			}
		}
		if (chapterMenu->isVisible()) {
			driver->draw2DRectangle(SColor(128, 0, 0, 0),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			chapterMenu->draw();
		}
		else {
			//std::cout << "checking fade in/out " << isPerformingIntro << " " << isPerformingOutro << std::endl;
			// if it's fading in or out, draw an overlay
			if (isPerformingIntro) {
				video::SColor color((s32)1 + 254 * (1 - (f32)fadeoutTimerMS / FK_SceneStoryDialogue::FadeinTimeMS),
					0, 0, 0);
				driver->draw2DRectangle(color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else if (isPerformingOutro) {
				if (storyEvent->fadeWhenEnding) {
					//std::cout << (s32)(1 + 254 * std::ceil((f32)fadeoutTimerMS / FK_SceneStoryDialogue::FadeoutTimeMS)) << std::endl;
					s32 opacity = (s32)(1 + std::ceil(253 * (f32)fadeoutTimerMS / FK_SceneStoryDialogue::FadeoutTimeMS));
					video::SColor color(opacity,
						0, 0, 0);
					driver->draw2DRectangle(color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
					//std::cout << "outro progress:" << opacity << std::endl;
				}
			}
			else if (endScene && storyEvent->fadeWhenEnding) {
				video::SColor color(255, 0, 0, 0);
				driver->draw2DRectangle(color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}
	void FK_SceneStoryDialogue::dispose(){
		for each(auto window in windows){
			delete window;
		}
		if (bgm.getStatus() == bgm.Playing){
			bgm.stop();
		}
		delete soundManager;
		windows.clear();
		// clear the scene of all scene nodes
		FK_SceneWithInput::dispose();
	}
	bool FK_SceneStoryDialogue::isRunning(){
		return (dialogueIndex < (s32)dialogues.size() || (isPerformingOutro || isPerformingIntro));
	}

	bool FK_SceneStoryDialogue::canSaveData() {
		return storyEvent->saveStoryCompletion;
	}

	FK_Scene::FK_SceneType FK_SceneStoryDialogue::nameId(){
		return FK_SceneType::SceneStoryModeDialogue;
	}

	// skip chapter routine
	void FK_SceneStoryDialogue::skipChapter() {
		inputReceiver->reset();
		dialogueIndex = (s32)dialogues.size();
		isPerformingOutro = true;
		endScene = true;
		skippingScene = true;
		if (storyEvent->saveStoryCompletion) {
			storyEvent->saveWhenSkipped = true;
		}
		//storyEvent->saveStoryCompletion = storyEvent->saveStoryCompletion && storyEvent->saveWhenSkipped;
		storyEvent->fadeWhenEnding = true;
	}

	void FK_SceneStoryDialogue::updateInput(){
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
		// skip scene
		if (!endScene && 
			(inputButtons & FK_Input_Buttons::Confirmation_Button) != 0 ||
			(inputButtons & FK_Input_Buttons::Cancel_Button) != 0 ||
			(inputButtons & FK_Input_Buttons::Menu_Pause_Button) != 0){
			if (storyEvent->allowMenu) {
				activateChapterMenu();
				return;
			}else{
				skipChapter();
				return;
			}
		}
		// skip text
		if (selection_timer_ms > 0){
			selection_timer_ms -= (s32)delta_t_ms;
		}else{
			if ((inputButtons & FK_Input_Buttons::Selection_Button) != 0){
				inputReceiver->reset();
				selection_timer_ms = FK_SceneStoryDialogue::SelectionTimerBufferMS;
				if (dialogueIndex >= 0 && !dialogues[dialogueIndex].autoProgress) {
					if (!switchWindow && (
						(windows[0]->isAllTextDisplayed() && windows[0]->isActive()) ||
						(windows[1]->isAllTextDisplayed() && windows[1]->isActive()) ||
						(windows[2]->isAllTextDisplayed() && windows[2]->isActive())
						)) {
						soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						switchWindowTimer = 0;
						switchWindow = true;
						return;
					}
					else if ((!windows[0]->isAllTextDisplayed() || dialogues[dialogueIndex].additionalTimeAfterDialogue > 0) &&
						windows[0]->isActive() && windows[0]->isOpen()) {
						canSkipDialogue = false;
						//soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						windows[0]->setTextMode(FK_DialogueWindow::DialogueTextMode::AllTogether);
						dialogues[dialogueIndex].additionalTimeAfterDialogue = 10;
					}
					else if ((!windows[1]->isAllTextDisplayed() || dialogues[dialogueIndex].additionalTimeAfterDialogue > 0) &&
						windows[1]->isActive() && windows[1]->isOpen()) {
						canSkipDialogue = false;
						//soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						windows[1]->setTextMode(FK_DialogueWindow::DialogueTextMode::AllTogether);
						dialogues[dialogueIndex].additionalTimeAfterDialogue = 10;
					}
					else if ((!windows[2]->isAllTextDisplayed() || dialogues[dialogueIndex].additionalTimeAfterDialogue > 0) &&
						windows[2]->isActive() && windows[2]->isOpen()) {
						canSkipDialogue = false;
						//soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
						windows[2]->setTextMode(FK_DialogueWindow::DialogueTextMode::AllTogether);
						dialogues[dialogueIndex].additionalTimeAfterDialogue = 10;
					}
				}
			}
		}
	}

	/* update save file data */
	void FK_SceneStoryDialogue::updateSaveFileData(){
		if (storyEvent->saveStoryCompletion){
			if (std::find(storyEpisodesCleared.begin(), storyEpisodesCleared.end(), storyEvent->episodePath) ==
				storyEpisodesCleared.end()){
				storyEpisodesCleared.push_back(storyEvent->episodePath);
				writeSaveFile();
			}
		}
	}

	// draw background
	void FK_SceneStoryDialogue::drawBackground(){
		if (backgroundPicture != NULL){
			f32 scaleX = (f32)screenSize.Width / backgroundPicture->getSize().Width;
			f32 scaleY = (f32)screenSize.Height / backgroundPicture->getSize().Height;
			fk_addons::draw2DImage(driver, backgroundPicture,
				core::rect<s32>(0, 0, backgroundPicture->getSize().Width, backgroundPicture->getSize().Height),
				core::vector2d<s32>(0, 0), core::vector2d<s32>(0, 0), 0, core::vector2df(scaleX, scaleY),
				false, video::SColor(255,255,255,255),core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		if (dialogueIndex < dialogues.size()) {
			if (transitionPicture != NULL) {
				s32 alpha = 255.f - (s32)std::floor(255.f * (f32)transitionTimerMS / (f32)(dialogues[dialogueIndex].backgroundTransitionTimeMs + 1));
				alpha = core::clamp(alpha, 1, 255);
				video::SColor tempCol = video::SColor(alpha, 255, 255, 255);
				video::SColor colors[4] = {
					tempCol,
					tempCol,
					tempCol,
					tempCol,
				};
				driver->draw2DImage(transitionPicture,
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height),
					core::rect<s32>(0, 0, transitionPicture->getSize().Width, transitionPicture->getSize().Height),
					0, colors);
			}
		}
	}

	//setup initial background
	void FK_SceneStoryDialogue::setupInitialBackground(){
		std::string backgroundPath =
			mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory + dialogues[0].backgroundTextureName;
		if (dialogues[0].takeBackgroundFromStagePreview){
			backgroundPath = stagesPath + dialogues[0].backgroundTextureName + "preview.jpg";
			currentBackground = backgroundPath;
			backgroundPicture = driver->getTexture(backgroundPath.c_str());
		}
		else{
			backgroundPicture = driver->getTexture(backgroundPath.c_str());
			if (!backgroundPicture){
				backgroundPath = episodeResourceDirectory +
					dialogues[0].backgroundTextureName;
				backgroundPicture = driver->getTexture(backgroundPath.c_str());
			}
			currentBackground = backgroundPath;
		}
	};

	//setup initial BGM
	void FK_SceneStoryDialogue::setupInitialBGM(){
		setupBGM(0);
	}

	//setup BGM from dialogue
	void FK_SceneStoryDialogue::setupBGM(s32 dialogueIndex){
		if (dialogueIndex >= (s32)dialogues.size()){
			return;
		}
		std::string systemBGMPath = mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory + 
			FK_SceneStoryDialogue::StoryFilesBGMDirectory;
		StoryBGM newBGM = dialogues[dialogueIndex].newBGM;
		if (!newBGM.bgmName.empty()){
			if (!(currentBGM.bgmName != newBGM.bgmName || currentBGM.bgmVolume != newBGM.bgmVolume ||
				currentBGM.bgmPitch != newBGM.bgmPitch)){
				return;
			}
			bgmName = newBGM.bgmName;
			validBGM = false;
			std::string filePath = systemBGMPath + bgmName;
			std::ifstream testFile(filePath.c_str());
			if (testFile.good()){
				testFile.clear();
				testFile.close();
				validBGM = true;
			}
			else{
				testFile.clear();
				testFile.close();
				std::cout << "WARNING: bgm file " << filePath << " not found." << std::endl;
				validBGM = false;
			}
			if (!validBGM){
				filePath = episodeResourceDirectory + bgmName;
				testFile.open(filePath.c_str());
				if (testFile.good()){
					testFile.clear();
					testFile.close();
					validBGM = true;
				}
				else{
					testFile.clear();
					testFile.close();
					std::cout << "WARNING: bgm file " << filePath << " not found." << std::endl;
					validBGM = false;
					return;
				}
			}
			currentBGM = newBGM;
			if (bgm.getStatus() == bgm.Playing){
				bgm.stop();
			}
			validBGM = bgm.openFromFile(filePath.c_str());
			if (validBGM){
				bgm.setLoop(newBGM.bgmLoop);
				bgm.setVolume(newBGM.bgmVolume * gameOptions->getMusicVolume());
				bgm.setPitch(newBGM.bgmPitch);
				bgm.play();
			}
		}
		else{
			validBGM = false;
		}
	}

	//setup SFX from dialogue
	void FK_SceneStoryDialogue::setupSFX(s32 dialogueIndex) {
		if (dialogueIndex >= (s32)dialogues.size()) {
			return;
		}
		std::string systemSFXPath = mediaPath + FK_SceneStoryDialogue::StoryFilesDirectory +
			FK_SceneStoryDialogue::StoryFilesSFXDirectory;
		StorySFX newSFX = dialogues[dialogueIndex].newSFX;
		bool validSFX = true;
		if (!newSFX.sfxName.empty()) {
			std::string filePath = systemSFXPath + newSFX.sfxName;
			std::ifstream testFile(filePath.c_str());
			if (testFile.good()) {
				testFile.clear();
				testFile.close();
			}
			else {
				testFile.clear();
				testFile.close();
				std::cout << "WARNING: sound effect file " << filePath << " not found." << std::endl;
				validSFX = false;
			}
			if (!validSFX) {
				filePath = episodeResourceDirectory + newSFX.sfxName;
				testFile.open(filePath.c_str());
				if (testFile.good()) {
					testFile.clear();
					testFile.close();
		
				}
				else {
					testFile.clear();
					testFile.close();
					std::cout << "WARNING: sound effect file " << filePath << " not found." << std::endl;
					return;
				}
			}
			soundManager->addSoundFullPath(filePath, filePath);
			soundManager->playSound(filePath, newSFX.sfxVolume * gameOptions->getSFXVolume(), newSFX.sfxPitch);
		}
	}

	// chapter menu
	/* setup chapter menu */
	void FK_SceneStoryDialogue::setupChapterMenu() {
		std::vector<std::string> menuItems = {
				" Skip scene ",
				" Save & Exit ",
				"Cancel",
			};
		chapterMenu = new FK_InGameMenu(device, soundManager, gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		chapterMenu->setViewport(core::position2di(-1, -1), screenSize);
	}
	/* activate/deactivate chapter menu */
	void FK_SceneStoryDialogue::activateChapterMenu() {
		chapterMenu->setActive(true);
		chapterMenu->setVisible(true);
		chapterMenu->reset();
		Sleep(200);
	}
	void FK_SceneStoryDialogue::deactivateChapterMenu() {
		chapterMenu->setActive(false);
		chapterMenu->setVisible(false);
		inputReceiver->reset();
		Sleep(200);
	}
	/* update chapter menu */
	void FK_SceneStoryDialogue::updateChapterMenu() {
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
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
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
					soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
					setNextScene(FK_Scene::FK_SceneType::SceneStoryModeSelection);
					storyEvent->saveWhenSkipped = false;
					storyEvent->saveStoryCompletion = false;
					skipChapter();
					break;
				case 2:
					soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
					break;
				default:
					break;
				}
				deactivateChapterMenu();
			}
		}
	}
	// read dialogues from file
	void FK_SceneStoryDialogue::readFromFile(std::string filename){
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		StoryDialogue tempDialog;
		while (inputFile >> temp){
			if (temp == FK_SceneStoryDialogue::EndTag){
				dialogues.push_back(tempDialog);
				tempDialog = StoryDialogue();
			}
			else if (temp == FK_SceneStoryDialogue::TopWindowTag){
				tempDialog.position = StoryDialogue::WindowPosition::Top;
			}
			else if (temp == FK_SceneStoryDialogue::CenterWindowTag){
				tempDialog.position = StoryDialogue::WindowPosition::Center;
			}
			else if (temp == FK_SceneStoryDialogue::InvisibleBackgroundTag){
				tempDialog.isBackgroundVisible = false;
			}
			else if (temp == FK_SceneStoryDialogue::NoBackgroundPictureTag){
				tempDialog.noBackgroundPicture = true;
			}
			else if (temp == FK_SceneStoryDialogue::CloseWindowsTag){
				tempDialog.closeWindowsAfterDialogue = true;
			}
			else if (temp == FK_SceneStoryDialogue::TransitionTag) {
				inputFile >> temp;
				u32 time;
				inputFile >> time;
				tempDialog.backgroundTransitionTextureName = temp;
				tempDialog.backgroundTransitionTimeMs = time;
			}
			else if (temp == FK_SceneStoryDialogue::AutoProgressTag) {
				tempDialog.autoProgress = true;
			}
			else if (temp == FK_SceneStoryDialogue::CloseSingleWindowTag){
				tempDialog.closeSingleWindowAfterDialogue = true;
			}
			else if (temp == FK_SceneStoryDialogue::IgnoreBufferTimeTag) {
				tempDialog.ignoreBufferTime = true;
			}
			else if (temp == FK_SceneStoryDialogue::BackgroundFilenameTag){
				inputFile >> temp;
				tempDialog.backgroundTextureName = temp;
				tempDialog.takeBackgroundFromStagePreview = false;
			}
			else if (temp == FK_SceneStoryDialogue::StageBackgroundFilenameTag){
				inputFile >> temp;
				tempDialog.backgroundTextureName = temp;
				tempDialog.backgroundTextureName += "\\";
				tempDialog.takeBackgroundFromStagePreview = true;
			}
			else if (temp == FK_SceneStoryDialogue::MugshotFilenameTag){
				inputFile >> temp;
				tempDialog.mugshotFilename = temp;
				tempDialog.takeMugshotFromCharacterPreview = false;
			}
			else if (temp == FK_SceneStoryDialogue::CharacterMugshotFilenameTag){
				inputFile >> temp;
				tempDialog.mugshotFilename = temp;
				tempDialog.mugshotFilename += "\\";
				tempDialog.takeMugshotFromCharacterPreview = true;
			}
			else if (temp == FK_SceneStoryDialogue::PauseMSTag){
				u32 time;
				inputFile >> time;
				tempDialog.additionalTimeAfterDialogue = time;
			}
			else if (temp == FK_SceneStoryDialogue::BGMTag){
				f32 volume, pitch;
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t"){
					std::getline(inputFile, temp);
				}
				inputFile >> volume >> pitch;
				tempDialog.newBGM.bgmName = temp;
				tempDialog.newBGM.bgmVolume = volume;
				tempDialog.newBGM.bgmPitch = pitch;
			}
			else if (temp == FK_SceneStoryDialogue::SFXTag) {
				f32 volume, pitch;
				temp = std::string();
				while (temp.empty() || temp == " " || temp == "\t") {
					std::getline(inputFile, temp);
				}
				inputFile >> volume >> pitch;
				tempDialog.newSFX.sfxName = temp;
				tempDialog.newSFX.sfxVolume = volume;
				tempDialog.newSFX.sfxPitch = pitch;
			}
			else if (temp == FK_SceneStoryDialogue::LinesBeginTag){
				while (inputFile){
					std::getline(inputFile, temp);
					if (temp == FK_SceneStoryDialogue::LinesEndTag){
						break;
					}
					if (!(temp.empty())){
						tempDialog.dialogueText.push_back(temp);
					}
				}
			}
		}
	}
}