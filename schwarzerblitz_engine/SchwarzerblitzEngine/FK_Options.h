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


#ifndef FK_OPTIONS_CLASS
#define FK_OPTIONS_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include<map>

using namespace irr;
/* this class will be implemented soon (I hope) */
namespace fk_engine{
	class FK_Options{
	public:
		const enum FK_NumberOfRounds : u32{
			OneRound = 1,
			ThreeRounds = 3,
			FiveRounds = 5,
			SevenRounds = 7,
			NineRounds = 9,
			ElevenRounds = 11,
			ThirteenRounds = 13,
			FifteenRounds = 15,
		};
		const enum FK_PostProcessingEffect : u32{
			NoPostProcessingEffects = 0,
			BloomOnly = 1,
			BlurOnly = 4,
			AllEffects = 2,
			PostProcessingOptionsSize = AllEffects + 1,
		};
		enum FK_ShadowsSettings : u32{
			NoShadowEffect = 0,
			OnlyCharaShadows = 2,
			FullShadows = 1,
			ShadowOptionsSize = FullShadows + 1,
		};
		const enum FK_TimerOptions : u32{
			Infinite = 100,
			Timer15seconds = 15,
			Timer30seconds = 30,
			Timer45seconds = 45,
			Timer60seconds = 60,
			Timer99seconds = 99
		};
		const enum FK_ShadowsQuality : u32{
			NoShadows = 0,
			VeryLowQualityShadows = 64,
			LowQualityShadows = 128,
			MediumQualityShadows = 256,
			HighQualityShadows = 512,
			VeryHighQualityShadows = 1024,
			ComputerMeltingQualityShadows = 2048,
		};
		const enum FK_TextureQuality : u32{
			NoTextures = 0,
			LowQualityTextures = 128,
			MediumQualityTextures = 256,
			HighQualityTextures = 512,
			VeryHighQualityTextures = 1024,
			ComputerMeltingQualityTextures = 2048,
		};
		const enum FK_AILevel : u32{
			BeginnerAI = 0,
			EasyAI = 2,
			MediumAI = 5,
			HardAI = 9,
			VeryHardAI = 12,
			ExtremeAI = 15,
			HellOnEarthAI = 20,
		};
		const enum FK_ScreenMode : u32{
			Windowed = 0,
			BorderlessWindow = 1,
			Fullscreen = 2,
		};
		const enum FK_OptionType{
			FullscreenMode,
			FullscreenResolution,
			WindowedModeResolution,
			DynamicResolution,
			AILevel,
			BorderlessWindowSamplingFactor,
			BGMVolume,
			SFXVolume,
			BGMMute,
			SFXMute,
			VoiceMute,
			VoiceVolume,
			ShadowQuality,
			FreeMatchTimer,
			ArcadeTimer,
			FreeMatchRounds,
			ArcadeRounds,
			PostProcessingEffects,
			LightEffects,
			InputDelayPlayer1,
			InputDelayPlayer2,
			MasterVolume,
			TourneyMode,
			FPSLimiter,
		};
		const enum FK_FPSLimit : long long {
			NoFPSLimit = 0,
			Limit24FPS = 41666,
			Limit30FPS = 33333,
			Limit60FPS = 16666,
			Limit120FPS = 8333,
			Limit144FPS = 6944,
		};
	public:
		FK_Options();
		FK_Options(core::dimension2d<u32> newMonitorSize);
		/* loaders */
		bool loadFromFile(std::string filename);
		bool saveToFile(std::string filename);
		bool reload();
		bool overwrite();
		/* setters */
		//bool setResolution(FK_ScreenResolutions newResolution);
		//bool setResolutionFullscreen(FK_ScreenResolutions newResolution);
		//bool setResolutionWindowed(FK_ScreenResolutions newResolution);
		bool setScreenMode(FK_ScreenMode new_screenMode);
		void setSFXVolume(f32 new_volume);
		void setSFXMute(bool muteSFX);
		void setVoicesVolume(f32 new_volume);
		void setVoicesMute(bool muteVoices);
		void setMusicVolume(f32 new_volume);
		void setMusicMute(bool muteMusic);
		void setMasterVolume(f32 new_volume);
		void setAILevel(FK_AILevel newAILevel);
		void setTimerArcade(FK_TimerOptions timerOption);
		void setTimerFreeMatch(FK_TimerOptions timerOption);
		void setNumberOfRoundsArcade(u32 numberOfRounds);
		void setNumberOfRoundsFreeMatch(u32 numberOfRounds);
		void setLightEffectsActiveFlag(FK_ShadowsSettings lightingActive);
		void setTextureQuality(FK_TextureQuality newQuality);
		void setShadowQuality(FK_ShadowsQuality newQuality);
		void setPostProcessingShadersFlag(FK_PostProcessingEffect shadersActive);
		void setAvailableScreenResolutions();
		void setSamplingFactor(f32 newSamplingFactor);
		void increaseResolutionIndex(bool fullscreen);
		void decreaseResolutionIndex(bool fullscreen);
		void setDynamicResolutionScalingFlag(bool dynamicResolutionFlag);
		void setInputDelayPlayer1(u32 newPlayer1DelayFrames);
		void setInputDelayPlayer2(u32 newPlayer1DelayFrames);
		void setTourneyMode(bool tourneyFlag);
		void setFPSLimiter(FK_FPSLimit newFpsLimit);
		//std::vector<u32> getAvailableScreenResolutions(bool fullscreenResolutionFlag);
		/* getters */
		core::dimension2d<u32> getResolution();
		//core::dimension2d<u32> getResolutionByFlag();
		//FK_ScreenResolutions getResolutionFlag(bool fullscreenResolution);
		bool getFullscreen();
		bool getBorderlessWindowMode();
		FK_ScreenMode getScreenMode();
		f32 getSFXVolume();
		bool getSFXMute();
		f32 getVoicesVolume();
		bool getVoicesMute();
		f32 getMusicVolume();
		bool getMusicMute();
		f32 getMasterVolume();
		u32 getAILevel();
		u32 getTimerArcade();
		u32 getTimerFreeMatch();
		u32 getNumberOfRoundsArcade();
		u32 getNumberOfRoundsFreeMatch();
		f32 getNominalMusicVolume();
		f32 getNominalSFXVolume();
		f32 getNominalVoicesVolume();
		f32 getSamplingFactor();
		FK_ShadowsSettings getLightEffectsActiveFlag();
		u32 getTextureQuality();
		u32 getShadowQuality();
		u32 getInputDelayPlayer1();
		u32 getInputDelayPlayer2();
		FK_PostProcessingEffect getPostProcessingShadersFlag();
		void setActiveCheatCodes(u32 cheatCodes);
		u32 getActiveCheatCodes();
		bool getDynamicResolutionScalingFlag();
		bool getTourneyMode();
		FK_FPSLimit FK_Options::getFPSLimiter();
	private:
		//FK_ScreenResolutions current_resolutionFlag;
		//FK_ScreenResolutions current_resolutionFlag_Windowed;
		core::dimension2d<u32> monitorSize;
		f32 soundEffectsMasterVolume;
		f32 bgmMasterVolume;
		f32 voicesEffectsMasterVolume;
		bool bgmMuteFlag;
		bool sfxMuteFlag;
		bool voicesMuteFlag;
		FK_TimerOptions arcadeTimerOption;
		FK_TimerOptions freeMatchTimerOption;
		u32 numberOfRoundsArcade;
		u32 numberOfRoundsFreeMatch;
		bool useDynamicResolutionScaling;
		FK_PostProcessingEffect allowPostProcessingEffects;
		FK_ShadowsSettings allowLighting;
		FK_TextureQuality textureQuality;
		FK_ShadowsQuality shadowsQuality;
		FK_AILevel AIlevel;
		std::string optionFileName;
		std::vector<std::pair<u32, u32> > screenResolutionArray;
		std::vector<std::pair<u32, u32> > windowSizeArray;
		s32 screenResolutionIndex;
		s32 windowResolutionIndex;
		FK_ScreenMode screenMode;
		f32 samplingFactor;
		std::map<FK_OptionType, std::string> optionLabels;
		u32 activeCheatCodes = 0;
		u32 inputDelayPlayer1Frames = 0;
		u32 inputDelayPlayer2Frames = 0;
		f32 masterVolume;
		bool tourneyModeFlag;
		FK_FPSLimit fpsLimit;
	};
};
#endif