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

#ifndef FK_SCENEEPISODESELECT_STORY_CLASS
#define FK_SCENEEPISODESELECT_STORY_CLASS

#include "FK_SceneWithInput.h"
#include "FK_Character.h"
#include "FK_Announcer.h"
#include "FK_SoundManager.h"
#include "FK_StoryFlowCluster.h"
#include "FK_InGameMenu.h"
#include <vector>
#include <deque>
#include <string>
#include <thread>
#include <atomic>

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{
	class FK_SceneEpisodeSelectionStoryMode : public FK_SceneWithInput{
	private:
		const std::string EpisodeListFileName = "episodeList.txt";
		const std::string EpisodeFlowFileName = "episodeConfig.txt";
		const std::string EpisodeEditedFlowFileName = "episodeConfig_censored.txt";
		const std::string EpisodeEditedMarkdownFileName = "playEditedEpisode.ini";
		const std::string EpisodeMainDirectory = "media\\scenes\\story\\episodes\\";
	public:
		FK_SceneEpisodeSelectionStoryMode();
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, 
			FK_Options* newOptions, s32 currentEpisodeIndex = 0);
		virtual void initialize();
		virtual void update();
		virtual void dispose();
		virtual FK_Scene::FK_SceneType nameId();
		virtual bool isRunning();
		// get selected episode
		FK_StoryFlowCluster getSelectedEpisode();
		s32 getCurrentIndex();
	protected:
		void setupGraphics();
		void setupSoundManager();
		void setupBGM();
		void setupChapterMenu();
		void updateChapterMenu();
		void activateChapterMenu();
		void deactivateChapterMenu();
		void setupSafeContentMenu();
		void updateSafeContentMenu();
		void activateSafeContentMenu();
		void deactivateSafeContentMenu();
		void updateInput();
		void loadEpisodeList();
		void drawBackground();
		void drawEpisode(u32 index);
		void drawCurrentEpisodePreview();
		void drawTitle();
		void drawEpisodeList();
		void drawArrows();
		void drawAll();
		bool isEpisodeAvailable(u32 index);
		void increaseMenuIndex(s32 value = 1);
		void decreaseMenuIndex(s32 value = 1);
		void cancelSelection();
		void selectItem();
		void goBackToMainMenu();
	private:
		/* application path */
		std::string episodeDirectory;
		std::string storyCommonMediaPath;
		/* textures */
		std::vector<video::ITexture*> episodeIcons;
		video::ITexture* lockedEpisodePreview;
		video::ITexture* background;
		video::ITexture* lockedEpisodeIcon;
		video::ITexture* previewFrame;
		video::ITexture* completionBoxTex;
		video::ITexture* episodeCompleteTick;
		/* arrows */
		video::ITexture* arrowUp_tex;
		video::ITexture* arrowDown_tex;
		u32 arrowFrequency;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 lastInputUpdateTime;
		/* path for players and other stuff*/
		std::vector<u32> availableEpisodes;
		std::vector<std::string> episodeNames;
		std::vector<bool> completedEpisodes;
		std::vector<bool> editedEpisodes;
		s32 episodeIndex;
		s32 oldEpisodeIndex;
		// screen variables
		core::dimension2d<u32> screenSize;
		f32 scale_factorX;
		f32 scale_factorY;
		/* additional variables (e.g. to check scene flow)*/
		u32 cycleId;
		/* sound manager and announcer */
		std::string soundPath;
		FK_SoundManager* soundManager;
		/* BGM */
		std::string systemBGMPath;
		sf::Music bgm;
		std::string bgmName;
		f32 bgmVolume;
		f32 bgmPitch;
		/* transition counter (used when scene ends) */
		u32 transitionCounter;
		/* mesh name buffer */
		bool backToMenu;
		bool episodeSelected;
		/* story episodes collection*/
		std::vector<FK_StoryFlowCluster> episodes;
		/* fonts */
		gui::IGUIFont* font;
		gui::IGUIFont* mediumFont;
		gui::IGUIFont* captionFont;
		/* chapter select menu */
		FK_InGameMenu* chapterSelectMenu;
		FK_InGameMenu* safeContentMenu;
		FK_DialogueWindow* safeContentInformation;
		/* input time */
		u32 lastInputTime;
	};
}
#endif