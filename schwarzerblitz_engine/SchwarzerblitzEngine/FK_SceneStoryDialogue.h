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

#ifndef FK_SCENESTORYDIALOGUE_CLASS
#define FK_SCENESTORYDIALOGUE_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include"FK_SceneWithInput.h"
#include"FK_DialogueWindow.h"
#include"FK_SoundManager.h"
#include"FK_StoryFlowCluster.h"
#include"FK_InGameMenu.h"

using namespace irr;

namespace fk_engine{

	class FK_SceneStoryDialogue : public FK_SceneWithInput{
	protected:
		struct StoryBGM{
			std::string bgmName = std::string();
			bool isStageBGM = false;
			f32 bgmVolume = 100.0f;
			f32 bgmPitch = 1.0f;;
			bool bgmLoop = true;
		};
		struct StorySFX {
			std::string sfxName = std::string();
			f32 sfxVolume = 100.0f;
			f32 sfxPitch = 1.0f;;
		};
		struct StoryEvent{
		protected:
			enum StoryEvent_Type{
				Dialogue,
				Pause,
				FadeIn,
				FadeOut,
				Flash,
				None,
			};
			StoryEvent_Type type = StoryEvent_Type::None;
		};
		struct StoryDialogue : public StoryEvent{
			enum WindowPosition : u32{
				Bottom = 1,
				Center = 2,
				Top = 0
			};
			std::string speakerName = std::string();
			std::vector<std::string> dialogueText = std::vector<std::string>();
			std::string mugshotFilename = std::string();
			std::string backgroundTextureName = std::string();
			bool takeBackgroundFromStagePreview = false;
			bool takeMugshotFromCharacterPreview = false;
			WindowPosition position = WindowPosition::Bottom;
			bool noBackgroundPicture = false;
			bool isBackgroundVisible = true;
			u32 additionalTimeAfterDialogue = 250;
			bool ignoreBufferTime = false;
			bool closeWindowsAfterDialogue = false;
			bool closeSingleWindowAfterDialogue = false;
			bool autoProgress = false;
			StoryBGM newBGM;
			StorySFX newSFX;
			std::string backgroundTransitionTextureName = std::string();
			u32 backgroundTransitionTimeMs = 0;
		};
	protected:
		const std::string SpeakerNameTag = "#SPEAKER";
		const std::string EndTag = "#END";
		const std::string LinesBeginTag = "#TEXT_BEGIN";
		const std::string LinesEndTag = "#TEXT_END";
		const std::string MugshotFilenameTag = "#MUGSHOT";
		const std::string CharacterMugshotFilenameTag = "#CHARA_MUGSHOT";
		const std::string BackgroundFilenameTag = "#BACKGROUND";
		const std::string StageBackgroundFilenameTag = "#STAGE_BACKGROUND";
		const std::string TopWindowTag = "#TOP_WINDOW";
		const std::string CenterWindowTag = "#CENTER_WINDOW";
		const std::string InvisibleBackgroundTag = "#NO_BACKGROUND";
		const std::string NoBackgroundPictureTag = "#NO_BACKGROUND_PICTURE";
		const std::string PauseMSTag = "#PAUSE";
		const std::string CloseWindowsTag = "#CLOSE_ALL_WINDOWS";
		const std::string CloseSingleWindowTag = "#CLOSE_WINDOW";
		const std::string SFXTag = "#SOUND";
		const std::string AutoProgressTag = "#AUTO";
		const std::string BGMTag = "#BGM";
		const std::string TransitionTag = "#TRANSITION";
		const std::string IgnoreBufferTimeTag = "#NO_BUFFER_TIME";
		const u32 FadeoutTimeMS = 500;
		const u32 FadeinTimeMS = 500;
		const u32 TypingSoundInterval = 64;
		const std::string StoryFilesDirectory = "scenes\\story\\common\\";
		const std::string StoryFilesMugshotDirectory = "mugshots\\";
		const std::string StoryFilesBGMDirectory = "bgm\\";
		const std::string StoryFilesSFXDirectory = "sfx\\";
		const u32 WindowActivityTimeMS = 300;
		const s32 SelectionTimerBufferMS = 200;
	public:
		FK_SceneStoryDialogue();
		FK_SceneStoryDialogue(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, 
			FK_Options* newOptions, FK_StoryEvent* newStoryEvent);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, 
			FK_Options* newOptions, FK_StoryEvent* newStoryEvent);
		void readFromFile(std::string filename);
		void initialize();
		void update();
		void drawAll();
		void dispose();
		bool isRunning();
		bool canSaveData();
		FK_SceneType nameId();
	protected:
		// scene menu
		virtual void setupChapterMenu();
		virtual void updateChapterMenu();
		void activateChapterMenu();
		void deactivateChapterMenu();
		void updateInput();
		void updateSaveFileData();
		void skipChapter();
		void setWindows();
		void setupSoundManager();
		void setupInitialBackground();
		void setupInitialBGM();
		void setupBGM(s32 dialogueIndex);
		void setupSFX(s32 dialogueIndex);
		//void readEventsFromFile(std::string filename);
		void drawBackground();
		std::vector<StoryDialogue> dialogues;
		std::string episodeResourceDirectory;
		s32 dialogueIndex;
		u32 now;
		u32 then;
		u32 delta_t_ms;
		u32 nowReal;
		u32 thenReal;
		s32 selection_timer_ms;
		std::vector<FK_DialogueWindow*> windows;
		std::string resourcesPath;
		std::string configFileName;
		core::dimension2d<u32> screenSize;
		video::ITexture* backgroundPicture;
		video::ITexture* transitionPicture;
		u32 cycleId;
		bool switchWindow;
		u32 switchWindowTimer;
		FK_SoundManager* soundManager;
		std::string soundPath;
		std::string stagesPath;
		std::string charactersPath;
		std::string commonResourcesStoryPath;
		bool isPerformingOutro;
		bool isPerformingIntro;
		bool endScene;
		bool skippingScene;
		bool canSkipDialogue;
		u32 fadeoutTimerMS;
		u32 typingSoundTimerMS;
		std::string currentBackground;
		/* BGM */
		std::string bgmName;
		sf::Music bgm;
		bool validBGM;
		StoryBGM currentBGM;
		FK_StoryEvent* storyEvent;
		FK_InGameMenu* chapterMenu;
		u32 transitionTimerMS;
	};
}
#endif