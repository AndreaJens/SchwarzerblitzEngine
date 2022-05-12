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

#ifndef FK_CHALLENGESELECT_TUTORIAL_CLASS
#define FK_CHALLENGESELECT_TUTORIAL_CLASS

#include "FK_SceneWithInput.h"
#include "FK_Character.h"
#include "FK_Announcer.h"
#include "FK_SoundManager.h"
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
	class FK_SceneChallengeSelectionTutorial : public FK_SceneWithInput{
	public:
		struct FK_TutorialCluster{
			std::wstring challengeName = std::wstring();
			std::vector<std::string> description = std::vector<std::string>();
			std::string configFileName = std::string();
			std::string parentDirectory = std::string();
			std::string relativePath = std::string();
			std::wstring challengeId = std::wstring();
			std::string challengeStoreName = std::string();
			void setup(std::string challengeDirectory,
				std::string challengeRelativePath, std::string configurationFilename);
		};
	private:
		const std::string TutorialListFileName = "tutorialList.txt";
		const std::string ChallengeListFileName = "challengeList.txt";
		const std::string ChallengeFlowFileName = "phases.txt";
		const std::string ChallengeConfigurationFileName = "config.txt";
		const std::string ChallengeMainDirectory = "media\\scenes\\tutorial\\challenges\\";
	public:
		FK_SceneChallengeSelectionTutorial();
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions, FK_Scene::FK_SceneTrainingType newChallengeSelectionType, s32 currentchallengeIndex = 0);
		virtual void initialize();
		virtual void update();
		virtual void dispose();
		virtual FK_Scene::FK_SceneType nameId();
		virtual bool isRunning();
		// get selected challenge
		FK_TutorialCluster getSelectedChallenge();
		s32 getCurrentIndex();
		FK_Scene::FK_SceneTrainingType getChallengeType();
	protected:
		void setupGraphics();
		void setupSoundManager();
		void setupBGM();
		void updateInput();
		void loadChallengeList();
		void drawBackground();
		void drawChallenge(u32 index);
		void drawCurrentChallengePreview();
		void drawTitle();
		void drawChallengeList();
		void drawArrows();
		void drawAll();
		bool isChallengeAvailable(u32 index);
		void increaseMenuIndex(s32 value = 1);
		void decreaseMenuIndex(s32 value = 1);
		void cancelSelection();
		void selectItem();
		void goBackToMainMenu();
	private:
		/* application path */
		std::string challengeDirectory;
		std::string challengeCommonMediaPath;
		/* textures */
		std::vector<video::ITexture*> challengeIcons;
		video::ITexture* lockedChallengePreview;
		video::ITexture* defaultChallengeIcon;
		video::ITexture* background;
		video::ITexture* lockedChallengeIcon;
		video::ITexture* previewFrame;
		video::ITexture* completionBoxTex;
		video::ITexture* challengeCompleteTick;
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
		std::vector<u32> availableChallenges;
		std::vector<std::string> challengeNames;
		std::vector<bool> completedChallenges;
		s32 challengeIndex;
		s32 oldChallengeIndex;
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
		bool challengeSelected;
		/* story challenges collection*/
		std::vector<FK_TutorialCluster> challenges;
		/* fonts */
		gui::IGUIFont* font;
		gui::IGUIFont* mediumFont;
		gui::IGUIFont* captionFont;
		/* scene type */
		FK_Scene::FK_SceneTrainingType challengeSelectionType;
		/* input time */
		u32 lastInputTime;
	};
}
#endif