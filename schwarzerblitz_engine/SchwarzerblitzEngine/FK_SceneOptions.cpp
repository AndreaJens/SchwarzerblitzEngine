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

#include "FK_SceneOptions.h"
#include "FK_SceneInputMapping_MultiplayerKeyboard.h"
#include "FK_SceneInputMapping_SingleplayerKeyboard.h"
#include "FK_SceneInputMapping_Joypads.h"
#include "FK_SceneInputMapping_JoypadPlayer.h"
#include "ExternalAddons.h"
#include <Windows.h>
#include<iostream>

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{
	FK_SceneOptions::FK_SceneOptions() : FK_SceneWithInput(){
		initialize();
	};
	FK_SceneOptions::FK_SceneOptions(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions) :
		FK_SceneWithInput(newDevice, joypadInfo, newOptions){
		initialize();
	};
	void FK_SceneOptions::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions){
		FK_SceneWithInput::setup(newDevice, joypadInfo, newOptions);
		backupOptions = FK_Options(*newOptions);
		initialize();
		initialResolution = newOptions->getResolution();
		initialScreenMode = newOptions->getScreenMode();
		setupIrrlichtDevice();
		setupJoypad();
		setupInputMapper();
		setupSoundManager();
		setupBGM();
		setupGraphics();
		setupMenuItems();
		setupTutorial();
	};
	FK_Scene::FK_SceneType FK_SceneOptions::nameId(){
		return FK_Scene::FK_SceneType::SceneOptions;
	}

	bool FK_SceneOptions::isRunning(){
		return backToMenu == false;
	}

	void FK_SceneOptions::drawBackground(){
		core::dimension2d<u32> backgroundSize = menuBackgroundTexture->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
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
		driver->draw2DImage(menuBackgroundTexture, destinationRect, sourceRect, 0, vertexColors_background);
	}

	void FK_SceneOptions::drawCaptions(){
		core::dimension2d<u32> backgroundSize = menuCaptionOverlay->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
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
		driver->draw2DImage(menuCaptionOverlay, destinationRect, sourceRect, 0, vertexColors_background, true);
		// draw caption text
		s32 fixedOffsetY = 320;
		s32 fixedOffsetX = 16 * scale_factorX;
		/* get width and height of the string*/
		std::wstring itemString;
		if (subcategorySelected){
			u32 captionIndex = getCurrentOptionIndex();
			itemString = std::wstring(menuOptionsCaptions[captionIndex].begin(), menuOptionsCaptions[captionIndex].end());
		}
		else{
			itemString = std::wstring(menuOptionsCategoriesCaptions[menuIndex].begin(), 
				menuOptionsCategoriesCaptions[menuIndex].end());
		}
		std::wstring captionRepetitionSeparator = L"     *     ";
		s32 sentenceWidth = captionFont->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = captionFont->getDimension(itemString.c_str()).Height;
		s32 separatorSize = captionFont->getDimension(captionRepetitionSeparator.c_str()).Width;
		fixedOffsetX -= ((s32)(captionMovementTimeCounterMs / 10 * scale_factorX)) % (sentenceWidth + separatorSize);
		std::wstring sentenceToDraw = itemString + captionRepetitionSeparator;
		sentenceWidth = captionFont->getDimension(sentenceToDraw.c_str()).Width;
		while (fixedOffsetX + sentenceWidth < screenSize.Width){
			sentenceToDraw += sentenceToDraw;
			sentenceWidth = captionFont->getDimension(sentenceToDraw.c_str()).Width;
		}
		s32 sentenceX = fixedOffsetX; // distance from screen center
		s32 sentenceY = fixedOffsetY; // distance from the top of the screen
		
		destinationRect = core::rect<s32>(sentenceX,
			sentenceY * scale_factorY,
			sentenceX * scale_factorX + sentenceWidth,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(captionFont, sentenceToDraw, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	void FK_SceneOptions::drawItems(){
		std::vector<u32> tempArray = getCurrentIndexArray();
		s32 menuItemsArraySize = tempArray.size();
		s32 fixedOffsetY = 52;
		s32 fixedSpacingY = 18;
		s32 fixedOffsetX = 32;
		s32 selectedItemAdditionalOffsetX = 15;
		for (s32 i = 0; i < menuItemsArraySize; ++i){
			u32 tempIndex = tempArray[i];
			/* get width and height of the string*/
			irr::video::SColor itemColor = irr::video::SColor(255, 255, 255, 255);
			std::wstring itemString(menuOptionsStrings[tempIndex].begin(), menuOptionsStrings[tempIndex].end());
			s32 sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
			s32 sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
			s32 sentenceX = fixedOffsetX; // distance from screen center
			if (i == menuIndex){
				sentenceX += selectedItemAdditionalOffsetX;
				itemColor = irr::video::SColor(255, 200, 200, 20);
			}
			if (i == getLastOptionIndex() - 1){
				fixedOffsetY += fixedSpacingY;
			}

			s32 sentenceY = fixedOffsetY + i * fixedSpacingY; // distance from the top of the screen

			core::rect<s32> destinationRect = core::rect<s32>(sentenceX * scale_factorX,
				sentenceY * scale_factorY,
				sentenceX * scale_factorX + sentenceWidth,
				sentenceHeight + sentenceY * scale_factorY);
			fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
				itemColor, irr::video::SColor(128, 0, 0, 0));

			if (additionalSpacingAfterOptionIndex[tempIndex]) {
				fixedOffsetY += fixedSpacingY / 3;
			}
		}
	}

	void FK_SceneOptions::drawItemValues(){
		std::vector<u32> tempArray = getCurrentIndexArray();
		s32 menuItemsArraySize = tempArray.size();
		s32 fixedOffsetY = 56;
		s32 fixedSpacingY = 18;
		s32 fixedOffsetX = 32;
		s32 selectedItemAdditionalOffsetX = 15;
		s32 selectedItemXL = 0;
		s32 selectedItemXR = 0;
		s32 selectedItemY = 0;
		s32 selectedItemDimension = 0;
		for (s32 i = 0; i < menuItemsArraySize - 2; ++i){
			/* get width and height of the string*/
			u32 optionIndex = tempArray[i];
			irr::video::SColor itemColor = irr::video::SColor(255, 20, 225, 120);
			std::wstring itemString = optionToString(optionIndex);
			s32 sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
			s32 sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
			s32 sentenceX = fixedOffsetX; // distance from screen center
			s32 sentenceY = fixedOffsetY + i * fixedSpacingY; // distance from the top of the screen
			if (i == getLastOptionIndex() - 1){
				fixedOffsetY += fixedSpacingY;
			}
			if (i == menuIndex){
				sentenceX += selectedItemAdditionalOffsetX;
				itemColor = irr::video::SColor(255, 20, 225, 220);
				selectedItemXL = screenSize.Width - (sentenceX * scale_factorX + sentenceWidth);
				selectedItemXR = screenSize.Width - sentenceX * scale_factorX;
				selectedItemY = (sentenceY + 5) * scale_factorY;
				selectedItemDimension = sentenceHeight * 3 / 4;
			}
			// in case RESOLUTION is selected in borderless window mode, make everything grey
			//if (i == 1 && gameOptions->getBorderlessWindowMode()){
			//	itemColor = irr::video::SColor(255, 96, 96, 96);
			//}
			// draw player 1 name near the lifebar
			core::rect<s32> destinationRect = core::rect<s32>(
				screenSize.Width - (sentenceX * scale_factorX + sentenceWidth),
				sentenceY * scale_factorY,
				screenSize.Width - sentenceX * scale_factorX,
				sentenceHeight + sentenceY * scale_factorY);
			fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
				itemColor, irr::video::SColor(128, 0, 0, 0));

			if (additionalSpacingAfterOptionIndex[optionIndex]) {
				fixedOffsetY += fixedSpacingY / 3;
			}
		}
		// draw arrow on selected item
		core::rect<s32> destinationRect = core::rect<s32>(
			selectedItemXL - selectedItemDimension, selectedItemY,
			selectedItemXL,
			selectedItemY + selectedItemDimension);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowLtex->getSize().Width, arrowLtex->getSize().Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(arrowLtex, destinationRect, sourceRect, 0, vertexColors_background, true);
		destinationRect = core::rect<s32>(
			selectedItemXR, selectedItemY,
			selectedItemXR + selectedItemDimension,
			selectedItemY + selectedItemDimension);
		driver->draw2DImage(arrowRtex, destinationRect, sourceRect, 0, vertexColors_background, true);
	}

	void FK_SceneOptions::drawTitle(){
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Options";
		if (subcategorySelected){
			switch (currentCategory){
			case FK_SceneOptions_SubSystem::GraphicsOptions:
				itemString = L"Graphic Options";
				break;
			case FK_SceneOptions_SubSystem::SoundOptions:
				itemString = L"Sound Options";
				break;
			case FK_SceneOptions_SubSystem::GameOptions:
				itemString = L"Game Options";
				break;
			default:
				break;
			}
		}
		s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
		x -= sentenceWidth / 2;
		y -= sentenceHeight / 2;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(font, itemString, destinationRect,
			irr::video::SColor(255,255,255,255), irr::video::SColor(128, 0, 0, 0));
	}

	void FK_SceneOptions::drawBasicCategories(){
		s32 menuItemsArraySize = menuOptionsCategoriesStrings.size();
		s32 fixedOffsetY = 42;
		s32 fixedSpacingY = 24;
		s32 fixedOffsetX = 32;
		s32 selectedItemAdditionalOffsetX = 15;
		for (s32 i = 0; i < menuItemsArraySize; ++i){
			/* get width and height of the string*/
			irr::video::SColor itemColor = irr::video::SColor(255, 255, 255, 255);
			std::wstring itemString(menuOptionsCategoriesStrings[i].begin(), menuOptionsCategoriesStrings[i].end());
			s32 sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
			s32 sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
			s32 sentenceX = fixedOffsetX; // distance from screen center
			if (i == menuIndex){
				sentenceX += selectedItemAdditionalOffsetX;
				itemColor = irr::video::SColor(255, 200, 200, 20);
			}
			if (!menuOptionsCategoriesActiveItems[i]){
				itemColor = irr::video::SColor(255, 96, 96, 96);
			}
			s32 sentenceY = fixedOffsetY + i * fixedSpacingY; // distance from the top of the screen
			// draw player 1 name near the lifebar
			core::rect<s32> destinationRect = core::rect<s32>(sentenceX * scale_factorX,
				sentenceY * scale_factorY,
				sentenceX * scale_factorX + sentenceWidth,
				sentenceHeight + sentenceY * scale_factorY);
			fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
				itemColor, irr::video::SColor(128, 0, 0, 0));
		}
	}

	// increase/toggle option
	void FK_SceneOptions::increaseOption(u32 optionIndex){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		std::vector<FK_Options::FK_ShadowsQuality> shadowQualities = {
			FK_Options::FK_ShadowsQuality::VeryLowQualityShadows,
			FK_Options::FK_ShadowsQuality::LowQualityShadows,
			FK_Options::FK_ShadowsQuality::MediumQualityShadows,
			FK_Options::FK_ShadowsQuality::HighQualityShadows,
			FK_Options::FK_ShadowsQuality::VeryHighQualityShadows,
			FK_Options::FK_ShadowsQuality::ComputerMeltingQualityShadows,
		};
		s32 shadowIndex = std::find(shadowQualities.begin(), 
			shadowQualities.end(), 
			(FK_Options::FK_ShadowsQuality)gameOptions->getShadowQuality()) - shadowQualities.begin();
		// timers
		std::vector<FK_Options::FK_TimerOptions> timers = {
			FK_Options::FK_TimerOptions::Timer15seconds,
			FK_Options::FK_TimerOptions::Timer30seconds,
			FK_Options::FK_TimerOptions::Timer45seconds,
			FK_Options::FK_TimerOptions::Timer60seconds,
			FK_Options::FK_TimerOptions::Timer99seconds,
			FK_Options::FK_TimerOptions::Infinite,
		};
		s32 arcadeTimerIndex = std::find(timers.begin(),
			timers.end(),
			(FK_Options::FK_TimerOptions)gameOptions->getTimerArcade()) - timers.begin();
		s32 freeMatchTimerIndex = std::find(timers.begin(),
			timers.end(),
			(FK_Options::FK_TimerOptions)gameOptions->getTimerFreeMatch()) - timers.begin();
		// rounds
		std::vector<FK_Options::FK_NumberOfRounds> rounds = {
			FK_Options::FK_NumberOfRounds::OneRound,
			FK_Options::FK_NumberOfRounds::ThreeRounds,
			FK_Options::FK_NumberOfRounds::FiveRounds,
			FK_Options::FK_NumberOfRounds::SevenRounds,
			FK_Options::FK_NumberOfRounds::NineRounds,
			FK_Options::FK_NumberOfRounds::ElevenRounds,
			FK_Options::FK_NumberOfRounds::ThirteenRounds,
			FK_Options::FK_NumberOfRounds::FifteenRounds,
		};
		s32 arcadeRoundsIndex = std::find(rounds.begin(),
			rounds.end(),
			(FK_Options::FK_NumberOfRounds)gameOptions->getNumberOfRoundsArcade()) - rounds.begin();
		s32 freeMatchRoundsIndex = std::find(rounds.begin(),
			rounds.end(),
			(FK_Options::FK_NumberOfRounds)gameOptions->getNumberOfRoundsFreeMatch()) - rounds.begin();

		// AI
		std::vector<FK_Options::FK_AILevel> AILevels = {
			FK_Options::FK_AILevel::BeginnerAI,
			FK_Options::FK_AILevel::EasyAI,
			FK_Options::FK_AILevel::MediumAI,
			FK_Options::FK_AILevel::HardAI,
			FK_Options::FK_AILevel::ExtremeAI,
			FK_Options::FK_AILevel::HellOnEarthAI,
		};
		s32 AIindex = std::find(AILevels.begin(),
			AILevels.end(),
			(FK_Options::FK_AILevel)gameOptions->getAILevel()) - AILevels.begin();

		// FPS
		std::vector<FK_Options::FK_FPSLimit> FPSLimitOptions{
			FK_Options::FK_FPSLimit::NoFPSLimit,
			FK_Options::FK_FPSLimit::Limit144FPS,
			FK_Options::FK_FPSLimit::Limit120FPS,
			FK_Options::FK_FPSLimit::Limit60FPS
		};

		s32 FPSIndex = std::find(FPSLimitOptions.begin(),
			FPSLimitOptions.end(),
			(FK_Options::FK_FPSLimit)gameOptions->getFPSLimiter()) - FPSLimitOptions.begin();

		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FullscreenMode){
			u32 screenIndex = (u32)gameOptions->getScreenMode();
			screenIndex += 1;
			if (screenIndex > (u32)(FK_Options::FK_ScreenMode::Fullscreen)){
				screenIndex = 0;
			}
			gameOptions->setScreenMode((FK_Options::FK_ScreenMode)screenIndex);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Resolution){
			//if (!gameOptions->getBorderlessWindowMode()){
				gameOptions->increaseResolutionIndex(gameOptions->getFullscreen());
			//}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_DynamicResolution) {
			gameOptions->setDynamicResolutionScalingFlag(!gameOptions->getDynamicResolutionScalingFlag());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_LightEffects){
			u32 lightOptionIndex = (u32)(gameOptions->getLightEffectsActiveFlag());
			lightOptionIndex += 1;
			lightOptionIndex %= FK_Options::FK_ShadowsSettings::ShadowOptionsSize;
			gameOptions->setLightEffectsActiveFlag((FK_Options::FK_ShadowsSettings)lightOptionIndex);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ShadowQuality){
			shadowIndex += 1;
			shadowIndex %= shadowQualities.size();
			gameOptions->setShadowQuality(shadowQualities[shadowIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_PostProcessingEffects){
			u32 ppOptionIndex = (u32)(gameOptions->getPostProcessingShadersFlag());
			ppOptionIndex += 1;
			ppOptionIndex %= FK_Options::FK_PostProcessingEffect::PostProcessingOptionsSize;
			gameOptions->setPostProcessingShadersFlag((FK_Options::FK_PostProcessingEffect)ppOptionIndex);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FPSLimit) {
			FPSIndex += 1;
			FPSIndex %= FPSLimitOptions.size();
			gameOptions->setFPSLimiter(FPSLimitOptions[FPSIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_MasterVolume) {
			s32 volume = (s32)(100 * gameOptions->getMasterVolume());
			volume += 5;
			if (volume >= 100) {
				volume = 100;
			}
			gameOptions->setMasterVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_SFXMute){
			gameOptions->setSFXMute(!gameOptions->getSFXMute());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_SFXVolume){
			s32 volume = (s32)(100 * gameOptions->getNominalSFXVolume());
			volume += 5;
			if (volume >= 100){
				volume = 100;
			}
			gameOptions->setSFXVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_VoiceMute){
			gameOptions->setVoicesMute(!gameOptions->getVoicesMute());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_VoiceVolume){
			s32 volume = (s32)(100 * gameOptions->getNominalVoicesVolume());
			volume += 5;
			if (volume >= 100){
				volume = 100;
			}
			gameOptions->setVoicesVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_BGMMute){
			gameOptions->setMusicMute(!gameOptions->getMusicMute());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_BGMVolume){
			s32 volume = (s32)(100 * gameOptions->getNominalMusicVolume());
			volume += 5;
			if (volume >= 100){
				volume = 100;
			}
			gameOptions->setMusicVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ArcadeTimer){
			arcadeTimerIndex += 1;
			arcadeTimerIndex %= timers.size();
			gameOptions->setTimerArcade(timers[arcadeTimerIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FreeMatchTimer){
			freeMatchTimerIndex += 1;
			freeMatchTimerIndex %= timers.size();
			gameOptions->setTimerFreeMatch(timers[freeMatchTimerIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ArcadeRounds){
			arcadeRoundsIndex += 1;
			arcadeRoundsIndex %= rounds.size();
			gameOptions->setNumberOfRoundsArcade((u32)rounds[arcadeRoundsIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FreeMatchRounds){
			freeMatchRoundsIndex += 1;
			freeMatchRoundsIndex %= rounds.size();
			gameOptions->setNumberOfRoundsFreeMatch((u32)rounds[freeMatchRoundsIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_TourneyMode) {
			gameOptions->setTourneyMode(!gameOptions->getTourneyMode());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_AILevel){
			AIindex += 1;
			AIindex %= AILevels.size();
			gameOptions->setAILevel(AILevels[AIindex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Player1InputDelay) {
			u32 frameDelay = (u32)gameOptions->getInputDelayPlayer1();
			frameDelay += 1;
			if (frameDelay > maxInputDelay) {
				frameDelay = 0;
			}
			gameOptions->setInputDelayPlayer1(frameDelay);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Player2InputDelay) {
			u32 frameDelay = (u32)gameOptions->getInputDelayPlayer2();
			frameDelay += 1;
			if (frameDelay > maxInputDelay) {
				frameDelay = 0;
			}
			gameOptions->setInputDelayPlayer2(frameDelay);
		}
		menu_bgm.setVolume(menuBGMvolume * gameOptions->getMusicVolume());
	}

	void FK_SceneOptions::decreaseOption(u32 optionIndex){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		std::vector<FK_Options::FK_ShadowsQuality> shadowQualities = {
			FK_Options::FK_ShadowsQuality::VeryLowQualityShadows,
			FK_Options::FK_ShadowsQuality::LowQualityShadows,
			FK_Options::FK_ShadowsQuality::MediumQualityShadows,
			FK_Options::FK_ShadowsQuality::HighQualityShadows,
			FK_Options::FK_ShadowsQuality::VeryHighQualityShadows,
			FK_Options::FK_ShadowsQuality::ComputerMeltingQualityShadows,
		};
		s32 shadowIndex = std::find(shadowQualities.begin(),
			shadowQualities.end(),
			(FK_Options::FK_ShadowsQuality)gameOptions->getShadowQuality()) - shadowQualities.begin();
		// timers
		std::vector<FK_Options::FK_TimerOptions> timers = {
			FK_Options::FK_TimerOptions::Timer15seconds,
			FK_Options::FK_TimerOptions::Timer30seconds,
			FK_Options::FK_TimerOptions::Timer45seconds,
			FK_Options::FK_TimerOptions::Timer60seconds,
			FK_Options::FK_TimerOptions::Timer99seconds,
			FK_Options::FK_TimerOptions::Infinite,
		};
		s32 arcadeTimerIndex = std::find(timers.begin(),
			timers.end(),
			(FK_Options::FK_TimerOptions)gameOptions->getTimerArcade()) - timers.begin();
		s32 freeMatchTimerIndex = std::find(timers.begin(),
			timers.end(),
			(FK_Options::FK_TimerOptions)gameOptions->getTimerFreeMatch()) - timers.begin();
		// rounds
		std::vector<FK_Options::FK_NumberOfRounds> rounds = {
			FK_Options::FK_NumberOfRounds::OneRound,
			FK_Options::FK_NumberOfRounds::ThreeRounds,
			FK_Options::FK_NumberOfRounds::FiveRounds,
			FK_Options::FK_NumberOfRounds::SevenRounds,
			FK_Options::FK_NumberOfRounds::NineRounds,
			FK_Options::FK_NumberOfRounds::ElevenRounds,
			FK_Options::FK_NumberOfRounds::ThirteenRounds,
			FK_Options::FK_NumberOfRounds::FifteenRounds,
		};
		s32 arcadeRoundsIndex = std::find(rounds.begin(),
			rounds.end(),
			(FK_Options::FK_NumberOfRounds)gameOptions->getNumberOfRoundsArcade()) - rounds.begin();
		s32 freeMatchRoundsIndex = std::find(rounds.begin(),
			rounds.end(),
			(FK_Options::FK_NumberOfRounds)gameOptions->getNumberOfRoundsFreeMatch()) - rounds.begin();

		// AI
		std::vector<FK_Options::FK_AILevel> AILevels = {
			FK_Options::FK_AILevel::BeginnerAI,
			FK_Options::FK_AILevel::EasyAI,
			FK_Options::FK_AILevel::MediumAI,
			FK_Options::FK_AILevel::HardAI,
			FK_Options::FK_AILevel::ExtremeAI,
			FK_Options::FK_AILevel::HellOnEarthAI,
		};
		s32 AIindex = std::find(AILevels.begin(),
			AILevels.end(),
			(FK_Options::FK_AILevel)gameOptions->getAILevel()) - AILevels.begin();

		// FPS
		std::vector<FK_Options::FK_FPSLimit> FPSLimitOptions{
			FK_Options::FK_FPSLimit::NoFPSLimit,
			FK_Options::FK_FPSLimit::Limit144FPS,
			FK_Options::FK_FPSLimit::Limit120FPS,
			FK_Options::FK_FPSLimit::Limit60FPS
		};

		s32 FPSIndex = std::find(FPSLimitOptions.begin(),
			FPSLimitOptions.end(),
			(FK_Options::FK_FPSLimit)gameOptions->getFPSLimiter()) - FPSLimitOptions.begin();

		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FullscreenMode){
			u32 screenIndex = (u32)gameOptions->getScreenMode();
			if (screenIndex == 0){
				screenIndex = (u32)(FK_Options::FK_ScreenMode::Fullscreen);
			}
			else{
				screenIndex -= 1;
			}
			gameOptions->setScreenMode((FK_Options::FK_ScreenMode)screenIndex);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Resolution){
			//if (!gameOptions->getBorderlessWindowMode()){
				gameOptions->decreaseResolutionIndex(gameOptions->getFullscreen());
			//}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_DynamicResolution) {
			gameOptions->setDynamicResolutionScalingFlag(!gameOptions->getDynamicResolutionScalingFlag());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_LightEffects){
			u32 lightOptionIndex = (u32)(gameOptions->getLightEffectsActiveFlag());
			if (lightOptionIndex <= 0){
				lightOptionIndex = FK_Options::FK_ShadowsSettings::ShadowOptionsSize - 1;
			}
			else{
				lightOptionIndex -= 1;
			}
			gameOptions->setLightEffectsActiveFlag((FK_Options::FK_ShadowsSettings)lightOptionIndex);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ShadowQuality){
			if (shadowIndex <= 0){
				shadowIndex = shadowQualities.size() - 1;
			}
			else{
				shadowIndex -= 1;
			}
			gameOptions->setShadowQuality(shadowQualities[shadowIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_PostProcessingEffects){
			u32 ppOptionIndex = (u32)(gameOptions->getPostProcessingShadersFlag());
			if (ppOptionIndex <= 0){
				ppOptionIndex = FK_Options::FK_PostProcessingEffect::PostProcessingOptionsSize - 1;
			}
			else{
				ppOptionIndex -= 1;
			}
			gameOptions->setPostProcessingShadersFlag((FK_Options::FK_PostProcessingEffect)ppOptionIndex);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FPSLimit) {
			if (FPSIndex <= 0) {
				FPSIndex = FPSLimitOptions.size() - 1;
			}
			else {
				FPSIndex -= 1;
			}
			gameOptions->setFPSLimiter(FPSLimitOptions[FPSIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_MasterVolume) {
			s32 volume = (s32)(100 * gameOptions->getMasterVolume());
			volume -= 5;
			if (volume <= 0) {
				volume = 0;
			}
			gameOptions->setMasterVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_SFXMute){
			gameOptions->setSFXMute(!gameOptions->getSFXMute());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_SFXVolume){
			s32 volume = (s32)(100 * gameOptions->getNominalSFXVolume());
			volume -= 5;
			if (volume <= 0){
				volume = 0;
			}
			gameOptions->setSFXVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_VoiceMute){
			gameOptions->setVoicesMute(!gameOptions->getVoicesMute());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_VoiceVolume){
			s32 volume = (s32)(100 * gameOptions->getNominalVoicesVolume());
			volume -= 5;
			if (volume <= 0){
				volume = 0;
			}
			gameOptions->setVoicesVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_BGMMute){
			gameOptions->setMusicMute(!gameOptions->getMusicMute());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_BGMVolume){
			s32 volume = (s32)(100 * gameOptions->getNominalMusicVolume());
			volume -= 5;
			if (volume <= 0){
				volume = 0;
			}
			gameOptions->setMusicVolume((f32)(volume) / 100.0f);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ArcadeTimer){
			if (arcadeTimerIndex <= 0){
				arcadeTimerIndex = timers.size() - 1;
			}
			else{
				arcadeTimerIndex -= 1;
			}
			gameOptions->setTimerArcade(timers[arcadeTimerIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FreeMatchTimer){
			if (freeMatchTimerIndex <= 0){
				freeMatchTimerIndex = timers.size() - 1;
			}
			else{
				freeMatchTimerIndex -= 1;
			}
			gameOptions->setTimerFreeMatch(timers[freeMatchTimerIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ArcadeRounds){
			if (arcadeRoundsIndex <= 0){
				arcadeRoundsIndex = rounds.size() - 1;
			}
			else{
				arcadeRoundsIndex -= 1;
			}
			gameOptions->setNumberOfRoundsArcade((u32)rounds[arcadeRoundsIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FreeMatchRounds){
			if (freeMatchRoundsIndex <= 0){
				freeMatchRoundsIndex = rounds.size() - 1;
			}
			else{
				freeMatchRoundsIndex -= 1;
			}
			gameOptions->setNumberOfRoundsFreeMatch((u32)rounds[freeMatchRoundsIndex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_TourneyMode) {
			gameOptions->setTourneyMode(!gameOptions->getTourneyMode());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_AILevel){
			if (AIindex <= 0){
				AIindex = AILevels.size() - 1;
			}
			else{
				AIindex -= 1;
			}
			gameOptions->setAILevel(AILevels[AIindex]);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Player1InputDelay) {
			u32 frameDelay = (u32)gameOptions->getInputDelayPlayer1();
			if (frameDelay == 0) {
				frameDelay = maxInputDelay;
			}
			else {
				frameDelay -= 1;
			}
			gameOptions->setInputDelayPlayer1(frameDelay);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Player2InputDelay) {
			u32 frameDelay = (u32)gameOptions->getInputDelayPlayer2();
			if (frameDelay == 0) {
				frameDelay = maxInputDelay;
			}
			else {
				frameDelay -= 1;
			}
			gameOptions->setInputDelayPlayer2(frameDelay);
		}
		menu_bgm.setVolume(menuBGMvolume * gameOptions->getMusicVolume());
	}

	// translate option to printable string
	std::wstring FK_SceneOptions::optionToString(u32 optionIndex){
		std::wstring itemString = L"";
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FullscreenMode){
			if (gameOptions->getFullscreen()){
				itemString = L"Full Screen";
			}
			else if (gameOptions->getBorderlessWindowMode()){
				itemString = L"Borderless Window";
			}
			else{
				itemString = L"Windowed";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Resolution){
			itemString += std::to_wstring(gameOptions->getResolution().Width);
			itemString += L"x";
			itemString += std::to_wstring(gameOptions->getResolution().Height);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_LightEffects){
			if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::FullShadows){
				itemString = L"ON";
			}
			else if (gameOptions->getLightEffectsActiveFlag() == FK_Options::FK_ShadowsSettings::OnlyCharaShadows){
				itemString = L"basic shadows";
			}
			else{
				itemString = L"OFF";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ShadowQuality){
			switch (gameOptions->getShadowQuality()){
			case FK_Options::FK_ShadowsQuality::VeryLowQualityShadows:
				itemString = L"Lowest";
				break;
			case FK_Options::FK_ShadowsQuality::LowQualityShadows:
				itemString = L"Low";
				break;
			case FK_Options::FK_ShadowsQuality::MediumQualityShadows:
				itemString = L"Medium";
				break;
			case FK_Options::FK_ShadowsQuality::HighQualityShadows:
				itemString = L"High";
				break;
			case FK_Options::FK_ShadowsQuality::VeryHighQualityShadows:
				itemString = L"Higher";
				break;
			case FK_Options::FK_ShadowsQuality::ComputerMeltingQualityShadows:
				itemString = L"Highest";
				break;
			default:
				itemString = L"Error";
				break;
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_PostProcessingEffects){
			if (gameOptions->getPostProcessingShadersFlag() == FK_Options::FK_PostProcessingEffect::AllEffects){
				itemString = L"Bloom & Blur";
			}
			else if(gameOptions->getPostProcessingShadersFlag() == FK_Options::FK_PostProcessingEffect::BloomOnly){
				itemString = L"Bloom";
			}
			else if (gameOptions->getPostProcessingShadersFlag() == FK_Options::FK_PostProcessingEffect::BlurOnly){
				itemString = L"Blur";
			}else{
				itemString = L"No effects";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FPSLimit) {
			switch (gameOptions->getFPSLimiter()) {
			case FK_Options::FK_FPSLimit::NoFPSLimit:
				itemString = L"OFF";
				break;
			case FK_Options::FK_FPSLimit::Limit144FPS:
				itemString = L"144 fps";
				break;
			case FK_Options::FK_FPSLimit::Limit120FPS:
				itemString = L"120 fps";
				break;
			case FK_Options::FK_FPSLimit::Limit60FPS:
				itemString = L"60 fps";
				break;
			case FK_Options::FK_FPSLimit::Limit30FPS:
				itemString = L"30 fps";
				break;
			case FK_Options::FK_FPSLimit::Limit24FPS:
				itemString = L"24 fps";
				break;
			default:
				itemString = L"???";
				break;
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_MasterVolume) {
			itemString += std::to_wstring((u32)(100 * gameOptions->getMasterVolume()));
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_SFXMute){
			if (gameOptions->getSFXMute()){
				itemString = L"OFF";
			}
			else{
				itemString = L"ON";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_SFXVolume){
			itemString += std::to_wstring((u32)(100 * gameOptions->getNominalSFXVolume()));
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_VoiceMute){
			if (gameOptions->getVoicesMute()){
				itemString = L"OFF";
			}
			else{
				itemString = L"ON";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_VoiceVolume){
			itemString += std::to_wstring((u32)(100 * gameOptions->getNominalVoicesVolume()));
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_BGMMute){
			if (gameOptions->getMusicMute()){
				itemString = L"OFF";
			}
			else{
				itemString = L"ON";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_BGMVolume){
			itemString += std::to_wstring((u32)(100 * gameOptions->getNominalMusicVolume()));
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ArcadeTimer){
			if (gameOptions->getTimerArcade() < 100){
				itemString = std::to_wstring(gameOptions->getTimerArcade());
			}
			else{
				itemString = L"00";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FreeMatchTimer){
			if (gameOptions->getTimerFreeMatch() < 100){
				itemString = std::to_wstring(gameOptions->getTimerFreeMatch());
			}
			else{
				itemString = L"00";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_ArcadeRounds){
			itemString = std::to_wstring((s32)std::floor((f32)gameOptions->getNumberOfRoundsArcade() / 2.f) + 1);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_FreeMatchRounds){
			itemString = std::to_wstring((s32)std::floor((f32)gameOptions->getNumberOfRoundsFreeMatch() / 2.f) + 1);
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_DynamicResolution) {
			if (gameOptions->getDynamicResolutionScalingFlag()) {
				itemString = L"ON";
			}
			else {
				itemString = L"OFF";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_TourneyMode) {
			if (gameOptions->getTourneyMode()) {
				itemString = L"ON";
			}
			else {
				itemString = L"OFF";
			}
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Player1InputDelay) {
			itemString = std::to_wstring(gameOptions->getInputDelayPlayer1());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_Player2InputDelay) {
			itemString = std::to_wstring(gameOptions->getInputDelayPlayer2());
		}
		if (optionIndex == FK_SceneOptions_OptionsIndex::Opt_AILevel){
			switch (gameOptions->getAILevel()){
			case FK_Options::FK_AILevel::BeginnerAI:
				itemString = L"Beginner";
				break;
			case FK_Options::FK_AILevel::EasyAI:
				itemString = L"Easy";
				break;
			case FK_Options::FK_AILevel::MediumAI:
				itemString = L"Medium";
				break;
			case FK_Options::FK_AILevel::HardAI:
				itemString = L"Hard";
				break;
			case FK_Options::FK_AILevel::ExtremeAI:
				itemString = L"Extreme";
				break;
			case FK_Options::FK_AILevel::HellOnEarthAI:
				itemString = L"Hell on Earth";
				break;
			default:
				itemString = L"Something in-between...";
				break;
			}
		}
		return itemString;
	}

	void FK_SceneOptions::drawAll(){
		drawBackground();
		if(showTutorial){
			gameTutorial->draw();
		}
		else{
			if (subcategorySelected){
				drawItems();
				drawItemValues();
			}
			else{
				drawBasicCategories();
			}
			drawCaptions();
			drawTitle();
		}
	}

	void FK_SceneOptions::initialize(){
		keyMappingSubscene = NULL;
		itemSelected = false;
		subcategorySelected = false;
		backToMenu = false;
		currentCategory = FK_SceneOptions_SubSystem::NoCategory;
		menuIndex = 0;
		currentCategoryIndex = 0;
		lastInputPlayer2 = 0;
		lastInputPlayer1 = 0;
		initialResolution = core::dimension2d<u32>();
		initialScreenMode = FK_Options::FK_ScreenMode::Windowed;
		menuOptionsStrings = std::vector<std::string>();
		menuOptionsCaptions = std::vector<std::string>();
		menuActiveItems = std::vector<bool>();
		/* time */
		then = 0;
		now = 0;
		delta_t_ms = 0;
		cycleId = 0;
		/* path */
		menuBGMName = std::string();
		menuBGMvolume = 60.0f;
		menuBGMpitch = 1.0f;
		/* caption movement */
		captionStillPeriodThresholdMs = 1000;
		setNextScene(FK_SceneType::SceneMainMenu);
		lastInputTimePlayer1 = 0;
		lastInputTimePlayer2 = 0;
	}
	void FK_SceneOptions::dispose(){
		itemSelected = false;
		menu_bgm.stop();
		menuIndex = 0;
		menuOptionsStrings.clear();
		menuActiveItems.clear();
		menuOptionsCaptions.clear();
		graphicsOptionsIndex.clear();
		gameOptionsIndex.clear();
		soundOptionsIndex.clear();
		menuOptionsCategoriesStrings.clear();
		menuOptionsCategoriesCaptions.clear();
		menuOptionsCategoriesClasses.clear();
		menuOptionsCategoriesActiveItems.clear();
		if (keyMappingSubscene != NULL){
			keyMappingSubscene->dispose();
			delete keyMappingSubscene;
			keyMappingSubscene = NULL;
			inputReceiver = NULL;
		}
		delete soundManager;
		delete gameTutorial;
		FK_SceneWithInput::dispose();
	}

	void FK_SceneOptions::softDispose() {
		itemSelected = false;
		menu_bgm.stop();
		menuIndex = 0;
		menuOptionsStrings.clear();
		menuActiveItems.clear();
		menuOptionsCaptions.clear();
		graphicsOptionsIndex.clear();
		gameOptionsIndex.clear();
		soundOptionsIndex.clear();
		menuOptionsCategoriesStrings.clear();
		menuOptionsCategoriesCaptions.clear();
		menuOptionsCategoriesClasses.clear();
		menuOptionsCategoriesActiveItems.clear();
		if (keyMappingSubscene != NULL) {
			keyMappingSubscene->dispose();
			delete keyMappingSubscene;
			keyMappingSubscene = NULL;
		}
		delete soundManager;
		delete gameTutorial;
		FK_SceneWithInput::dispose(false);
	}

	u32 FK_SceneOptions::getCurrentTime()
	{
		return now;
	}

	bool FK_SceneOptions::isInInputAssignmentScreen()
	{
		return subcategorySelected &&
			(currentCategory == FK_SceneOptions_SubSystem::KeyboardRemappingSolo ||
				currentCategory == FK_SceneOptions_SubSystem::KeyboardRemappingMultiplayer ||
				currentCategory == FK_SceneOptions_SubSystem::JoypadRemapping ||
				currentCategory == FK_SceneOptions_SubSystem::JoypadRemappingPlayer1 ||
				currentCategory == FK_SceneOptions_SubSystem::JoypadRemappingPlayer2);
	}

	void FK_SceneOptions::drawKeyMappingScene(bool drawSceneBackground)
	{
		if (keyMappingSubscene != NULL) {
			((FK_SceneInputMapping*)keyMappingSubscene)->drawEmbedded(drawSceneBackground);
		}
	}

	void FK_SceneOptions::updateStandardScene(bool drawScene){
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		// if this is the first update cycle, play "select character" voice and the BGM
		if (cycleId == 0){
			cycleId = 1;
			menu_bgm.play();
			delta_t_ms = 0;
		}
		if (hasToSetupJoypad()){
			updateJoypadSetup(delta_t_ms);
		}
		else{
			if (showTutorial){
				updateTutorial();
			}
			else{
				captionStillPeriodCounterMs += delta_t_ms;
				if (captionStillPeriodCounterMs > captionStillPeriodThresholdMs){
					captionMovementTimeCounterMs += delta_t_ms;
				}
				updateInput();
			}
		}
		// draw scene
		if (drawScene) {
			driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
			FK_SceneWithInput::executePreRenderingRoutine(&backupOptions);
			drawAll();
			if (hasToSetupJoypad()) {
				drawJoypadSetup(now);
			}
			FK_SceneWithInput::executePostRenderingRoutine(&backupOptions);
			driver->endScene();
		}
	}

	void FK_SceneOptions::updateKeyMapping(bool drawScene){
		if (keyMappingSubscene != NULL && subcategorySelected){
			((FK_SceneInputMapping*)keyMappingSubscene)->update(drawScene);
			if (!keyMappingSubscene->isRunning()){
				inputReceiver->reset();
				delete inputMapper;
				FK_SceneWithInput::setupInputMapper();
				keyMappingSubscene->dispose();
				delete keyMappingSubscene;
				keyMappingSubscene = NULL;
				resetInputRoutines();
				//soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
				goBackToMainMenu();
			}
		}
	}
	void FK_SceneOptions::initializeKeyboardMappingMultiplayer(){
		if (keyMappingSubscene != NULL){
			keyMappingSubscene->dispose();
			delete keyMappingSubscene;
			keyMappingSubscene = NULL;
			resetInputRoutines();
		}
		keyMappingSubscene = new FK_SceneInputMapping_MultiplayerKeyboard(device, joystickInfo, gameOptions);
	}

	void FK_SceneOptions::initializeKeyboardMappingSolo(){
		if (keyMappingSubscene != NULL){
			keyMappingSubscene->dispose();
			delete keyMappingSubscene;
			keyMappingSubscene = NULL;
			resetInputRoutines();
		}
		keyMappingSubscene = new FK_SceneInputMapping_SingleplayerKeyboard(device, joystickInfo, gameOptions);
	}

	void FK_SceneOptions::initializeJoypadMapping(s32 playerIndex){
		if (keyMappingSubscene != NULL){
			keyMappingSubscene->dispose();
			delete keyMappingSubscene;
			keyMappingSubscene = NULL;
			resetInputRoutines();
		}
		if (playerIndex < 0) {
			keyMappingSubscene = new FK_SceneInputMapping_Joypads(device, joystickInfo, gameOptions);
		}
		else {
			keyMappingSubscene = new FK_SceneInputMapping_JoypadPlayer(playerIndex, device, joystickInfo, gameOptions);
		}
	}

	void FK_SceneOptions::update() {
		update(true);
	}

	void FK_SceneOptions::update(bool drawSceneInsideUpdate){
		if (isInInputAssignmentScreen()){
			updateKeyMapping(drawSceneInsideUpdate);
		}
		else{
			updateStandardScene(drawSceneInsideUpdate);
		}
	}

	// check if screen needs to be refreshed
	bool FK_SceneOptions::needsScreenRefreshing(){
		return (initialScreenMode != gameOptions->getScreenMode() || initialResolution != gameOptions->getResolution());
	}

	// setup BGM
	void FK_SceneOptions::setupBGM(){
		systemBGMPath = commonResourcesPath + "system_bgm\\";
		menuBGMName = "menu(astral)_cut2.ogg";
		std::string filePath = systemBGMPath + menuBGMName;
		if (!menu_bgm.openFromFile(filePath.c_str()))
			return;
		menu_bgm.setLoop(true);
		menu_bgm.setVolume(menuBGMvolume * gameOptions->getMusicVolume());
		menu_bgm.setPitch(menuBGMpitch);
	}

	// setup sound manager
	void FK_SceneOptions::setupSoundManager(){
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}

	void FK_SceneOptions::setupGraphics(){
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		submenufont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		captionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		/* textures */
		menuBackgroundTexture = driver->getTexture((commonResourcesPath + "common_menu_items\\menuBackground_base.png").c_str());
		menuCaptionOverlay = driver->getTexture((commonResourcesPath + "common_menu_items\\menu_CaptionOverlay.png").c_str());
		arrowLtex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowL.png").c_str());
		arrowRtex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowR.png").c_str());
		arrowLtexSelected = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowL_sel.png").c_str());
		arrowRtexSelected = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowR_sel.png").c_str());
		// load joypad settings textures
		loadJoypadSetupTextures();
	}

	// setup sound manager
	void FK_SceneOptions::setupMenuItems(){
		// main menu items
		menuOptionsCategoriesStrings = {
			"Graphics",
			"Sound",
			"Game Options",
			"Keyboard settings (single player)",
			"Keyboard settings (multiplayer)",
			"Joypad settings (player 1)",
			"Joypad settings (player 2)",
			"Remap joypad to std. controller",
			"Open game guide",
			"Credits",
			"Back to main menu",
		};
		menuOptionsCategoriesCaptions = {
			"Customize graphics-related options, such as screen resolution and quality of shadows",
			"Change the volume for music and sound effects",
			"Customize game options, such as the AI level and the number of rounds, for both ARCADE and FREE MATCH mode",
			"Customize the keyboard layout for single player game modes",
			"Customize the keyboard layout for split-keyboard multiplayer games",
			"Customize the joypad layout for player 1",
			"Customize the joypad layout for player 2",
			"Remap the joypad layout on the standard joypad layout used throughout the game. Normally, you need to do this only once",
			"Open a quick reference guide about the game mechanics. You can access this document from the PAUSE MENU anytime during a match",
			"Watch the game credits, to learn who did what in this game",
			"Go back to main menu",
		};
		menuOptionsCategoriesClasses = {
			FK_SceneOptions_SubSystem::GraphicsOptions,
			FK_SceneOptions_SubSystem::SoundOptions,
			FK_SceneOptions_SubSystem::GameOptions,
			FK_SceneOptions_SubSystem::KeyboardRemappingSolo,
			FK_SceneOptions_SubSystem::KeyboardRemappingMultiplayer,
			FK_SceneOptions_SubSystem::JoypadRemappingPlayer1,
			FK_SceneOptions_SubSystem::JoypadRemappingPlayer2,
			FK_SceneOptions_SubSystem::JoypadToXboxTranslation,
			FK_SceneOptions_SubSystem::ShowTutorial,
			FK_SceneOptions_SubSystem::ShowCredits,
			FK_SceneOptions_SubSystem::BackToMenu,
		};
		menuOptionsCategoriesActiveItems = { true, true, true, true, true, false, false, false, true, true, true };
		if (joystickInfo.size() > 0){
			menuOptionsCategoriesActiveItems[5] = true;
			menuOptionsCategoriesActiveItems[7] = true;
		}

		if (joystickInfo.size() > 1) {
			menuOptionsCategoriesActiveItems[6] = true;
		}
		
		// graphics [0 - 4]
		menuOptionsStrings.push_back("Screen mode");
		menuOptionsStrings.push_back("Resolution");
		menuOptionsStrings.push_back("Shadows");
		menuOptionsStrings.push_back("Shadowmap quality");
		menuOptionsStrings.push_back("Advanced graphic effects");
		graphicsOptionsIndex = {
			0, 1, 16, 2, 3, 4, 21
		};
		// sound
		menuOptionsStrings.push_back("Play SFX");
		menuOptionsStrings.push_back("SFX volume");
		menuOptionsStrings.push_back("Play voices");
		menuOptionsStrings.push_back("Voices volume");
		menuOptionsStrings.push_back("Play BGM");
		menuOptionsStrings.push_back("BGM volume");
		soundOptionsIndex = {
			19,5,6,7,8,9,10,
		};
		// game
		menuOptionsStrings.push_back("Match Timer (arcade mode)");
		menuOptionsStrings.push_back("Match Timer (free match)");
		menuOptionsStrings.push_back("Number of rounds (arcade mode)");
		menuOptionsStrings.push_back("Number of rounds (free match)");
		menuOptionsStrings.push_back("AI Level");

		gameOptionsIndex = {
			11, 12, 13, 14, 15, 17, 18, 20
		};

		// additional and late options
		menuOptionsStrings.push_back("Dynamic resolution");
		menuOptionsStrings.push_back("Player 1 frame delay");
		menuOptionsStrings.push_back("Player 2 frame delay");
		menuOptionsStrings.push_back("Master volume");
		menuOptionsStrings.push_back("Tourney mode");
		menuOptionsStrings.push_back("FPS limit");

		// other
		menuOptionsStrings.push_back("Cancel");
		menuOptionsStrings.push_back("Confirm");
		// add last two elements to every array
		graphicsOptionsIndex.push_back(menuOptionsStrings.size() - 2);
		graphicsOptionsIndex.push_back(menuOptionsStrings.size() - 1);
		soundOptionsIndex.push_back(menuOptionsStrings.size() - 2);
		soundOptionsIndex.push_back(menuOptionsStrings.size() - 1);
		gameOptionsIndex.push_back(menuOptionsStrings.size() - 2);
		gameOptionsIndex.push_back(menuOptionsStrings.size() - 1);
		// graphics
		menuOptionsCaptions.push_back("Switch between FULL SCREEN and WINDOWED mode - this option is rendered effective once you CONFIRM the changes and go BACK TO THE MAIN MENU");
		menuOptionsCaptions.push_back("Change screen resolution -  this option is rendered effective once you CONFIRM the changes and go BACK TO THE MAIN MENU");
		menuOptionsCaptions.push_back("Toggle SHADOWS ON or OFF. Disabling them could improve performances on older computers");
		menuOptionsCaptions.push_back("Set shadow quality. A higher number requires a higher computational power. If shadows are disabled, this setting has no effect");
		menuOptionsCaptions.push_back("Toggle advanced light effects, such as bloom and blur shaders. Disabling them could improve performances on older computers");
		// sound
		menuOptionsCaptions.push_back("Toggle sound effects");
		menuOptionsCaptions.push_back("Change the volume of sound effects. This value is ignored if PLAY SFX is set to OFF");
		menuOptionsCaptions.push_back("Toggle voice effects");
		menuOptionsCaptions.push_back("Change the volume of voice effects. This value is ignored if PLAY VOICES is set to OFF");
		menuOptionsCaptions.push_back("Toggle background music");
		menuOptionsCaptions.push_back("Change the volume of the music. This value is ignored if PLAY BGM is set to OFF");
		// game
		menuOptionsCaptions.push_back("Set the timer for ARCADE mode. 00 means infinite time");
		menuOptionsCaptions.push_back("Set the timer for FREE MATCH mode. 00 means infinite time");
		menuOptionsCaptions.push_back("Set the number of rounds in a match for ARCADE mode");
		menuOptionsCaptions.push_back("Set the number of rounds in a match for FREE MATCH mode");
		menuOptionsCaptions.push_back("Set the AI level for STORY, ARCADE, and FREE MATCH mode.");

		// additional
		menuOptionsCaptions.push_back("When ON, if the frame rate is low, dynamic resolution tries to improve performance by reducing the rendering size. Switching it OFF prevents this optimization, by always keeping the nominal resolution active");
		menuOptionsCaptions.push_back("Set additional frames of input delay for player 1. This can be useful while playing against a friend through Parsec/Steam Remote Play Together");
		menuOptionsCaptions.push_back("Set additional frames of input delay for player 2. This can be useful while playing against a friend through Parsec/Steam Remote Play Together");
		menuOptionsCaptions.push_back("Change the master volume. This value affects all sounds in-game");
		menuOptionsCaptions.push_back("Set Tourney Mode to ON to use the game in a tourney setup. This mode restricts the selection of modes, characters, stages, costumes and options to those that are currently considered tournament legal");
		menuOptionsCaptions.push_back("Set the maximum FPS count for the game. This can relieve some stress from the graphic card in case of heavy usage.");

		//other
		menuOptionsCaptions.push_back("Cancel changes and go back to the option screen");
		menuOptionsCaptions.push_back("Confirm changes and go back to the option screen");
		for (s32 i = 0; i < menuOptionsStrings.size(); ++i){
			menuActiveItems.push_back(true);
			additionalSpacingAfterOptionIndex.push_back(false);
		}

		//set additional spacing after certain options
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_Resolution] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_FPSLimit] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_SFXVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_VoiceVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_MasterVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_BGMVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_FreeMatchTimer] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_FreeMatchRounds] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_AILevel] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_Player2InputDelay] = true;
	}

	/* setup tutorial */
	void FK_SceneOptions::setupTutorial(){
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		showTutorial = false;
		// setup tutorial
		gameTutorial = new FK_Tutorial(driver, mediaPath + "tutorial\\", screenSize, submenufont);
	}

	// update tutorial
	void FK_SceneOptions::updateTutorial(){
		u32 inputButtons = 0;
		u32 inputThreshold = 200;
		if (now - lastInputTimePlayer1 < inputThreshold &&
			now - lastInputTimePlayer2 < inputThreshold) {
			return;
		}
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
		u32 menuButtons = inputButtons & FK_Input_Buttons::Any_MenuButton;
		u32 directionButtons = inputButtons & FK_Input_Buttons::Any_Direction;
		if ((inputButtons & FK_Input_Buttons::Cancel_Button) != 0){
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			showTutorial = false;
			//inputReceiver->reset();
			lastInputTimePlayer1 = now;
			lastInputTimePlayer2 = now;
			return;
		}
		if ((directionButtons == FK_Input_Buttons::Right_Direction) || (menuButtons == FK_Input_Buttons::ScrollRight_Button)){
			if (gameTutorial->isLastPage()){
				soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			}
			else{
				soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				gameTutorial->goToNextPage();
			}
			lastInputTimePlayer1 = now;
			lastInputTimePlayer2 = now;
			return;
		}
		if ((directionButtons == FK_Input_Buttons::Left_Direction) || (menuButtons == FK_Input_Buttons::ScrollLeft_Button)){
			if (gameTutorial->isFirstPage()){
				soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			}
			else{
				soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				gameTutorial->goToPreviousPage();
			}
			lastInputTimePlayer1 = now;
			lastInputTimePlayer2 = now;
			return;
		}
	}

	void FK_SceneOptions::onConfirm(){
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		gameOptions->overwrite();
		//menu_bgm.stop();
		//setupBGM();
		//menu_bgm.play();
		goBackToMainMenu();
	}

	void FK_SceneOptions::onCancel(){
		soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
		gameOptions->reload();
		f32 volume = menuBGMvolume * gameOptions->getMusicVolume();
		menu_bgm.setVolume(volume);
		goBackToMainMenu();
	}

	void FK_SceneOptions::selectItem(){
		inputReceiver->reset();
		if (menuIndex == getLastOptionIndex()){
			onConfirm();
			return;
		}
		if (subcategorySelected){
			if (menuIndex == getLastOptionIndex() - 1){
				onCancel();
				return;
			}
		}else{
			if (menuOptionsCategoriesActiveItems[menuIndex]){
				soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
				subcategorySelected = true;
				currentCategoryIndex = menuIndex;
				currentCategory = menuOptionsCategoriesClasses[menuIndex];
				menuIndex = 0;
				if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::KeyboardRemappingMultiplayer){
					initializeKeyboardMappingMultiplayer();
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::KeyboardRemappingSolo){
					initializeKeyboardMappingSolo();
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::JoypadRemapping){
					initializeJoypadMapping(-1);
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::JoypadRemappingPlayer1) {
					initializeJoypadMapping(0);
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::JoypadRemappingPlayer2) {
					initializeJoypadMapping(1);
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::JoypadToXboxTranslation){
					for (int i = 0; i < joystickInfo.size(); ++i){
						std::string joypadName = std::string(joystickInfo[i].Name.c_str());
						newJoypadFound.push_back(std::pair<std::string, u32>(joypadName, i));
					}
					menuIndex = currentCategoryIndex;
					subcategorySelected = false;
					return;
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::ShowTutorial){
					showTutorial = true;
					subcategorySelected = false;
					gameTutorial->goToBeginning();
					menuIndex = currentCategoryIndex;
					return;
				}
				else if (currentCategory == FK_SceneOptions::FK_SceneOptions_SubSystem::ShowCredits){
					menuIndex = currentCategoryIndex;
					subcategorySelected = false;
					backToMenu = true;
					setNextScene(FK_Scene::FK_SceneType::SceneCredits);
				}
			}
			else{
				soundManager->playSound("cancel", 100.f * gameOptions->getSFXVolume());
				return;
			}
		}
	}

	std::vector<u32> FK_SceneOptions::getCurrentIndexArray(){
		if (subcategorySelected){
			switch (currentCategory){
			case FK_SceneOptions_SubSystem::GraphicsOptions:
				return graphicsOptionsIndex;
				break;
			case FK_SceneOptions_SubSystem::SoundOptions:
				return soundOptionsIndex;
				break;
			case FK_SceneOptions_SubSystem::GameOptions:
				return gameOptionsIndex;
				break;
			default:
				return std::vector<u32>();
				break;
			}
		}
		return std::vector<u32>();
	}

	u32 FK_SceneOptions::getLastOptionIndex(){
		if (subcategorySelected){
			switch (currentCategory){
			case FK_SceneOptions_SubSystem::GraphicsOptions:
				return graphicsOptionsIndex.size() - 1;
				break;
			case FK_SceneOptions_SubSystem::SoundOptions:
				return soundOptionsIndex.size() - 1;
				break;
			case FK_SceneOptions_SubSystem::GameOptions:
				return gameOptionsIndex.size() - 1;
				break;
			default:
				return 1;
				break;
			}
		}
		else{
			return menuOptionsCategoriesStrings.size() - 1;
		}
	}

	u32 FK_SceneOptions::getCurrentOptionIndex(){
		if (subcategorySelected){
			switch (currentCategory){
			case FK_SceneOptions_SubSystem::GraphicsOptions:
				return graphicsOptionsIndex[menuIndex];
				break;
			case FK_SceneOptions_SubSystem::SoundOptions:
				return soundOptionsIndex[menuIndex];
				break;
			case FK_SceneOptions_SubSystem::GameOptions:
				return gameOptionsIndex[menuIndex];
				break;
			default:
				return 1;
				break;
			}
		}
		else{
			return 0;
		}
	}

	void FK_SceneOptions::increaseMenuIndex(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		menuIndex += 1;
		menuIndex %= (getLastOptionIndex() + 1);
		captionStillPeriodCounterMs = 0;
		captionMovementTimeCounterMs = 0;
	}

	void FK_SceneOptions::decreaseMenuIndex(){
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		if (menuIndex == 0){
			menuIndex = getLastOptionIndex();
		}
		else{
			menuIndex -= 1;
		}
		captionStillPeriodCounterMs = 0;
		captionMovementTimeCounterMs = 0;
	}

	void FK_SceneOptions::updateInput(){
		//lastInputTimePlayer1 = player1input->getLastInputTime();
		//lastInputTimePlayer2 = player2input->getLastInputTime();
		u32 inputThreshold = 200;
		// check input for player 1
		if (now - lastInputTimePlayer1 > inputThreshold){
			if (joystickInfo.size() > 0){
				player1input->update(now, inputReceiver->JoypadStatus(0),false);
			}
			else{
				player1input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer1 = player1input->getLastDirection();
			u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button){
				lastInputTimePlayer1 = now;
				onCancel();
				return;
			}
			else{
				if (subcategorySelected){
					u32 subMenuIndex = getCurrentOptionIndex();
					if (directionPlayer1 == FK_Input_Buttons::Right_Direction){
						lastInputTimePlayer1 = now;
						increaseOption(subMenuIndex);
						return;
					}
					else if (directionPlayer1 == FK_Input_Buttons::Left_Direction){
						lastInputTimePlayer1 = now;
						decreaseOption(subMenuIndex);
						return;
					}
				}
				if (directionPlayer1 == FK_Input_Buttons::Down_Direction){
					lastInputTimePlayer1 = now;
					increaseMenuIndex();
				}
				else if (directionPlayer1 == FK_Input_Buttons::Up_Direction){
					lastInputTimePlayer1 = now;
					decreaseMenuIndex();
				}
				else{
					// select stage
					if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button){
						lastInputTimePlayer1 = now;
						selectItem();
						return;
					}
				}
			}
		}
		// check input for player 2
		if (now - lastInputTimePlayer2 > inputThreshold){
			if (joystickInfo.size() > 1){
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			else{
				player2input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer2 = player2input->getLastDirection();
			u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button){
				lastInputTimePlayer2 = now;
				onCancel();
			}
			else{
				if (subcategorySelected){
					u32 subMenuIndex = getCurrentOptionIndex();
					if (directionPlayer2 == FK_Input_Buttons::Right_Direction){
						lastInputTimePlayer2 = now;
						increaseOption(subMenuIndex);
						return;
					}
					else if (directionPlayer2 == FK_Input_Buttons::Left_Direction){
						lastInputTimePlayer2 = now;
						decreaseOption(subMenuIndex);
						return;
					}
				}
				if (directionPlayer2 == FK_Input_Buttons::Down_Direction){
					lastInputTimePlayer2 = now;
					increaseMenuIndex();
				}
				else if (directionPlayer2 == FK_Input_Buttons::Up_Direction){
					lastInputTimePlayer2 = now;
					decreaseMenuIndex();
				}
				else{
					// select stage
					if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button){
						lastInputTimePlayer2 = now;
						selectItem();
						return;
					}
				}
			}
		}
	}

	// go back to main menu
	void FK_SceneOptions::goBackToMainMenu(){
		inputReceiver->reset();
		if (needsScreenRefreshing()){
			setNextScene(FK_Scene::FK_SceneType::SceneOptions);
			backToMenu = true;
		}
		else if (subcategorySelected){
			subcategorySelected = false;
			menuIndex = currentCategoryIndex;
			return;
		}
		else{
			setNextScene(FK_Scene::FK_SceneType::SceneMainMenu);
			backToMenu = true;
		}
	};

	// joypad assignment update
	void FK_SceneOptions::playAssignedButtonSound() {
		soundManager->playSound("confirm", 100.f * gameOptions->getSFXVolume());
	}
}
