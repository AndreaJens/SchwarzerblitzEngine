#include <irrlicht.h>
#include "FK_Database.h"
#include "FK_Options.h"
#include <fstream>
#include <algorithm>
#include<iostream>
#include<windows.h>

using namespace irr;
using namespace core;

namespace fk_engine{
	FK_Options::FK_Options(){
		screenResolutionArray.clear();
		windowSizeArray.clear();
		//current_resolutionFlag = FK_ScreenResolutions::Wide1280x720;
		//current_resolutionFlag_Windowed = FK_ScreenResolutions::Wide640x360;
		screenMode = FK_ScreenMode::Windowed;
		soundEffectsMasterVolume = 1.0;
		bgmMasterVolume = 1.0;
		voicesEffectsMasterVolume = 1.0;
		bgmMuteFlag = false;
		sfxMuteFlag = false;
		voicesMuteFlag = false;
		arcadeTimerOption = FK_TimerOptions::Timer60seconds;
		freeMatchTimerOption = FK_TimerOptions::Timer60seconds;
		numberOfRoundsArcade = 3;
		numberOfRoundsFreeMatch = 3;
		allowPostProcessingEffects = FK_PostProcessingEffect::AllEffects;
		allowLighting = FK_ShadowsSettings::FullShadows;
		textureQuality = FK_TextureQuality::MediumQualityTextures;
		shadowsQuality = FK_ShadowsQuality::MediumQualityShadows;
		AIlevel = FK_AILevel::MediumAI;
		monitorSize = core::dimension2d<u32>(0, 0);
		samplingFactor = 1.0f;
		useDynamicResolutionScaling = true;

		optionLabels = std::map<FK_OptionType, std::string>();
		optionLabels[FK_OptionType::FullscreenMode] = "Fullscreen";
		optionLabels[FK_OptionType::FullscreenResolution] = "Fullscreen_Resolution";
		optionLabels[FK_OptionType::WindowedModeResolution] = "Window_Resolution";
		optionLabels[FK_OptionType::DynamicResolution] = "Dynamic_Resolution";
		optionLabels[FK_OptionType::SFXVolume] = "SFX_Volume";
		optionLabels[FK_OptionType::SFXMute] = "SFX_Mute";
		optionLabels[FK_OptionType::VoiceVolume] = "Voice_Volume";
		optionLabels[FK_OptionType::VoiceMute] = "Voice_Mute";
		optionLabels[FK_OptionType::BGMVolume] = "BGM_Volume";
		optionLabels[FK_OptionType::BGMMute] = "BGM_Mute";
		optionLabels[FK_OptionType::ArcadeTimer] = "Arcade_Timer";
		optionLabels[FK_OptionType::FreeMatchTimer] = "FreeMatch_Timer";
		optionLabels[FK_OptionType::ArcadeRounds] = "Arcade_Rounds";
		optionLabels[FK_OptionType::FreeMatchRounds] = "FreeMatch_Rounds";
		optionLabels[FK_OptionType::PostProcessingEffects] = "Allow_PostProcessing";
		optionLabels[FK_OptionType::LightEffects] = "Allow_Lights";
		optionLabels[FK_OptionType::ShadowQuality] = "Shadows_Quality";
		optionLabels[FK_OptionType::AILevel] = "AI_Level";
		optionLabels[FK_OptionType::BorderlessWindowSamplingFactor] = "ScreenSamplingFactor";
	}

	FK_Options::FK_Options(core::dimension2d<u32> newMonitorSize) : FK_Options(){
		monitorSize = newMonitorSize;
		setAvailableScreenResolutions();
	};

	// reload file
	bool FK_Options::reload(){
		return loadFromFile(optionFileName);
	}

	// overwrite previous configuration file
	bool FK_Options::overwrite(){
		return saveToFile(optionFileName);
	}

	/* file loaders / writers */
	bool FK_Options::loadFromFile(std::string filename){
		int numberOfOptions = optionLabels.size();
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			std::cout << "WARNING: option file could not be loaded!!!" << std::endl;
			std::cout << "File will be created instead." << std::endl;
			saveToFile(filename.c_str());
			return false;
		}
		optionFileName = filename;
		std::string temp;
		u32 tempVal;
		for (int i = 0; i < numberOfOptions; ++i){
			if (inputFile >> temp){
				inputFile >> tempVal;
				if (temp == optionLabels[FK_OptionType::FullscreenMode]){
					setScreenMode((FK_ScreenMode)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::FullscreenResolution]){
					screenResolutionIndex = (u32)tempVal;
				}
				else if (temp == optionLabels[FK_OptionType::WindowedModeResolution]){
					windowResolutionIndex = (u32)tempVal;
				}
				else if (temp == optionLabels[FK_OptionType::SFXVolume]){
					setSFXVolume(((f32)tempVal) / 100.0f);
				}
				else if (temp == optionLabels[FK_OptionType::SFXMute]){
					setSFXMute((bool)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::VoiceVolume]){
					setVoicesVolume(((f32)tempVal) / 100.0f);
				}
				else if (temp == optionLabels[FK_OptionType::VoiceMute]){
					setVoicesMute((bool)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::BGMVolume]){
					setMusicVolume(((f32)tempVal) / 100.0f);
				}
				else if (temp == optionLabels[FK_OptionType::BGMMute]){
					setMusicMute((bool)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::ArcadeTimer]){
					setTimerArcade((FK_TimerOptions)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::FreeMatchTimer]){
					setTimerFreeMatch((FK_TimerOptions)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::ArcadeRounds]){
					setNumberOfRoundsArcade(tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::FreeMatchRounds]){
					setNumberOfRoundsFreeMatch(tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::PostProcessingEffects]){
					setPostProcessingShadersFlag((FK_PostProcessingEffect)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::LightEffects]){
					setLightEffectsActiveFlag((FK_ShadowsSettings)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::ShadowQuality]){
					setShadowQuality((FK_ShadowsQuality)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::AILevel]){
					setAILevel((FK_AILevel)tempVal);
				}
				else if (temp == optionLabels[FK_OptionType::BorderlessWindowSamplingFactor]){
					setSamplingFactor((f32)tempVal / 100.0f);
				}
				else if (temp == optionLabels[FK_OptionType::DynamicResolution]) {
					setDynamicResolutionScalingFlag((bool)tempVal);
				}
				else{
					continue;
				}
			}
		}
		return true;
	};
	bool FK_Options::saveToFile(std::string filename){
		optionFileName = filename;
		u32 numberOfOptions = (s32)optionLabels.size();
		std::map<FK_OptionType, u32> optionValues;
		optionValues[FK_OptionType::FullscreenMode] = (u32)screenMode;
		optionValues[FK_OptionType::FullscreenResolution] = (u32)screenResolutionIndex;
		optionValues[FK_OptionType::WindowedModeResolution] = (u32)windowResolutionIndex;
		optionValues[FK_OptionType::DynamicResolution] = (u32)useDynamicResolutionScaling;
		optionValues[FK_OptionType::SFXVolume] = (u32)(soundEffectsMasterVolume * 100);
		optionValues[FK_OptionType::SFXMute] = (u32)sfxMuteFlag;
		optionValues[FK_OptionType::VoiceVolume] = (u32)(voicesEffectsMasterVolume * 100);
		optionValues[FK_OptionType::VoiceMute] = (u32)voicesMuteFlag;
		optionValues[FK_OptionType::BGMVolume] = (u32)(bgmMasterVolume * 100);
		optionValues[FK_OptionType::BGMMute] = (u32)bgmMuteFlag;
		optionValues[FK_OptionType::ArcadeTimer] = (u32)arcadeTimerOption;
		optionValues[FK_OptionType::FreeMatchTimer] = (u32)freeMatchTimerOption;
		optionValues[FK_OptionType::ArcadeRounds] = (u32)numberOfRoundsArcade;
		optionValues[FK_OptionType::FreeMatchRounds] = (u32)numberOfRoundsFreeMatch;
		optionValues[FK_OptionType::PostProcessingEffects] = (u32)allowPostProcessingEffects;
		optionValues[FK_OptionType::LightEffects] = (u32)allowLighting;
		optionValues[FK_OptionType::ShadowQuality] = (u32)shadowsQuality;
		optionValues[FK_OptionType::AILevel] = (u32)AIlevel;
		optionValues[FK_OptionType::BorderlessWindowSamplingFactor] = (u32)(100 * samplingFactor);

		FK_OptionType* keys = new FK_OptionType[numberOfOptions] {
			FullscreenMode,
			FullscreenResolution,
			WindowedModeResolution,
			DynamicResolution,
			SFXVolume,
			SFXMute,
			VoiceVolume,
			VoiceMute,
			BGMVolume,
			BGMMute,
			ArcadeTimer,
			FreeMatchTimer,
			ArcadeRounds,
			FreeMatchRounds,
			PostProcessingEffects,
			LightEffects,
			ShadowQuality,
			AILevel,
			BorderlessWindowSamplingFactor,
		};

		std::ofstream outputFile(filename.c_str());
		if (!outputFile){
			std::cout << "WARNING: option file could not be created!!!" << std::endl;
			return false;
		}
		for (u32 i = 0; i < numberOfOptions; ++i){
			outputFile << optionLabels[keys[i]] << std::endl << optionValues[keys[i]] << std::endl;
		}
		delete[] keys;
		outputFile.close();
		return true;
	};

	bool FK_Options::setScreenMode(FK_ScreenMode new_screenMode){
		bool refreshFlag = screenMode != new_screenMode;
		screenMode = new_screenMode;
		return refreshFlag;
	}

	void FK_Options::setSFXVolume(f32 new_volume){
		soundEffectsMasterVolume = new_volume;
	};

	void FK_Options::setSFXMute(bool muteSFX){
		sfxMuteFlag = muteSFX;
	};

	void FK_Options::setVoicesVolume(f32 new_volume){
		voicesEffectsMasterVolume = new_volume;
	};

	void FK_Options::setVoicesMute(bool muteVoices){
		voicesMuteFlag = muteVoices;
	};

	void FK_Options::setMusicVolume(f32 new_volume){
		bgmMasterVolume = new_volume;
	};

	void FK_Options::setMusicMute(bool muteMusic){
		bgmMuteFlag = muteMusic;
	};

	void FK_Options::setAILevel(FK_AILevel newAILevel){
		AIlevel = newAILevel;
	};

	void FK_Options::setTimerArcade(FK_TimerOptions timerOption){
		arcadeTimerOption = timerOption;
	};

	void FK_Options::setTimerFreeMatch(FK_TimerOptions timerOption){
		freeMatchTimerOption = timerOption;
	};

	void FK_Options::setNumberOfRoundsArcade(u32 numberOfRounds){
		numberOfRoundsArcade = numberOfRounds;
	};
	void FK_Options::setNumberOfRoundsFreeMatch(u32 numberOfRounds){
		numberOfRoundsFreeMatch = numberOfRounds;
	};

	void FK_Options::setLightEffectsActiveFlag(FK_Options::FK_ShadowsSettings lightingActive){
		allowLighting = lightingActive;
	};

	void FK_Options::setTextureQuality(FK_TextureQuality newQuality){
		textureQuality = newQuality;
	};

	void FK_Options::setShadowQuality(FK_ShadowsQuality newQuality){
		shadowsQuality = newQuality;
	};

	void FK_Options::setPostProcessingShadersFlag(FK_Options::FK_PostProcessingEffect shadersActive){
		allowPostProcessingEffects = shadersActive;
	};

	void FK_Options::setSamplingFactor(f32 newFactor){
		samplingFactor = newFactor;
	}

	/* FK_Options::getters */
	core::dimension2d<u32> FK_Options::getResolution(){
		core::dimension2d<u32> screenResolution;
		std::pair<u32, u32> resolution;
		if (getFullscreen()){
			if (screenResolutionIndex >= screenResolutionArray.size()) {
				for (int i = 0; i < screenResolutionArray.size(); ++i) {
					if (screenResolutionArray[i].first >= 1280) {
						screenResolutionIndex = i;
						break;
					}
				}
			}
			resolution = screenResolutionArray[screenResolutionIndex];
		}
		else{
			if (windowResolutionIndex >= windowSizeArray.size()) {
				for (int i = 0; i < windowSizeArray.size(); ++i) {
					if (windowSizeArray[i].first == 960 && windowSizeArray[i].second == 540) {
						windowResolutionIndex = i;
						break;
					}
				}
			}
			resolution = windowSizeArray[windowResolutionIndex];
		}
		screenResolution = core::dimension2d<u32>(resolution.first, resolution.second);
		return screenResolution;
	};


	FK_Options::FK_ScreenMode FK_Options::getScreenMode(){
		return screenMode;
	}

	bool FK_Options::getFullscreen(){
		return screenMode == FK_ScreenMode::Fullscreen;
	};

	bool FK_Options::getBorderlessWindowMode(){
		return screenMode == FK_ScreenMode::BorderlessWindow;
	}

	f32 FK_Options::getSFXVolume(){
		if (sfxMuteFlag){
			return 0.0f;
		}
		else{
			return soundEffectsMasterVolume;
		}
	};
	bool FK_Options::getSFXMute(){
		return sfxMuteFlag;
	};

	f32 FK_Options::getVoicesVolume(){
		if (voicesMuteFlag){
			return 0.0f;
		}
		else{
			return voicesEffectsMasterVolume;
		}
	};
	bool FK_Options::getVoicesMute(){
		return voicesMuteFlag;
	};
	f32 FK_Options::getMusicVolume(){
		if (bgmMuteFlag){
			return 0.0f;
		}
		else{
			return bgmMasterVolume;
		}
	};
	f32 FK_Options::getNominalMusicVolume(){
		return bgmMasterVolume;
	}
	f32 FK_Options::getNominalSFXVolume(){
		return soundEffectsMasterVolume;
	}
	f32 FK_Options::getNominalVoicesVolume(){
		return voicesEffectsMasterVolume;
	}
	f32 FK_Options::getSamplingFactor(){
		return samplingFactor;
	}
	bool FK_Options::getMusicMute(){
		return bgmMuteFlag;
	};
	u32 FK_Options::getAILevel(){
		return (u32)AIlevel;
	};
	u32 FK_Options::getTimerArcade(){
		return (u32)arcadeTimerOption;
	};
	u32 FK_Options::getTimerFreeMatch(){
		return (u32)freeMatchTimerOption;
	};
	u32 FK_Options::getNumberOfRoundsArcade(){
		return numberOfRoundsArcade;
	};
	u32 FK_Options::getNumberOfRoundsFreeMatch(){
		return numberOfRoundsFreeMatch;
	};
	FK_Options::FK_ShadowsSettings FK_Options::getLightEffectsActiveFlag(){
		return allowLighting;
	};
	u32 FK_Options::getTextureQuality(){
		return (u32)textureQuality;
	};
	u32 FK_Options::getShadowQuality(){
		return (u32)shadowsQuality;
	};
	FK_Options::FK_PostProcessingEffect FK_Options::getPostProcessingShadersFlag(){
		return allowPostProcessingEffects;
	}

	void FK_Options::setActiveCheatCodes(u32 cheatCodes)
	{
		activeCheatCodes = cheatCodes;
	}
	
	u32 FK_Options::getActiveCheatCodes() {
		return activeCheatCodes;
	}

	bool FK_Options::getDynamicResolutionScalingFlag()
	{
		return useDynamicResolutionScaling;
	}

	// detect all available screen resolutions
	void FK_Options::setAvailableScreenResolutions(){
		screenResolutionArray.clear();
		windowSizeArray.clear();
		// create sorting function for the two arrays
		struct sort_pairs {
			bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) {
				if (left.first == right.first){
					return left.second < right.second;
				}
				else{
					return left.first < right.first;
				}
			}
		};
		// add default window sizes
		windowSizeArray.push_back(std::pair<u32, u32>(640, 360));
		windowSizeArray.push_back(std::pair<u32, u32>(960, 540));
		windowSizeArray.push_back(std::pair<u32, u32>(1280, 720));
		windowResolutionIndex = 1;
		DEVMODE dm = { 0 };
		dm.dmSize = sizeof(dm);
		for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++) {
			u32 w = dm.dmPelsWidth;
			u32 h = dm.dmPelsHeight;
			/*std::cout << "Mode #" << iModeNum << " = " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << std::endl;*/
			std::pair<u32, u32> newItem(w, h);
			// limit supported resolutions to 1920 x 1080 for font issues
			if (newItem.first <= 1920){
				if (find(screenResolutionArray.begin(), screenResolutionArray.end(), newItem) ==
					screenResolutionArray.end()){
					screenResolutionArray.push_back(newItem);
				}
				if (find(windowSizeArray.begin(), windowSizeArray.end(), newItem) ==
					windowSizeArray.end()){
					windowSizeArray.push_back(newItem);
				}
			}
		}
		// sort arrays
		std::sort(screenResolutionArray.begin(), screenResolutionArray.end(), sort_pairs());
		std::sort(windowSizeArray.begin(), windowSizeArray.end(), sort_pairs());
		// set correct default indices
		windowResolutionIndex = -1;
		/*std::distance(std::find(windowSizeArray.begin(), windowSizeArray.end(),
			std::pair<u32, u32>(960, 540)), windowSizeArray.begin());*/
		for (int i = 0; i < windowSizeArray.size(); ++i){
			if (windowSizeArray[i].first == 960 && windowSizeArray[i].second == 540){
				windowResolutionIndex = i;
				break;
			}
		}
		screenResolutionIndex = -1;
		for (int i = 0; i < screenResolutionArray.size(); ++i){
			if (screenResolutionArray[i].first >= 1280){
				screenResolutionIndex = i;
				break;
			}
		}
		if (screenResolutionIndex < 0){
			screenResolutionIndex = screenResolutionArray.size() - 1;
		}
	}
	// increase resoution index in options
	void FK_Options::increaseResolutionIndex(bool fullscreen){
		if (fullscreen){
			screenResolutionIndex += 1;
			screenResolutionIndex %= screenResolutionArray.size();
		}
		else{
			windowResolutionIndex += 1;
			windowResolutionIndex %= windowSizeArray.size();
		}
	}
	// decrease resolution index in options
	void FK_Options::decreaseResolutionIndex(bool fullscreen){
		if (fullscreen){
			screenResolutionIndex -= 1;
			if (screenResolutionIndex < 0) screenResolutionIndex += screenResolutionArray.size();
		}
		else{
			windowResolutionIndex -= 1;
			if (windowResolutionIndex < 0) windowResolutionIndex += windowSizeArray.size();
		}
	}
	void FK_Options::setDynamicResolutionScalingFlag(bool dynamicResolutionFlag)
	{
		useDynamicResolutionScaling = dynamicResolutionFlag;
	}
}