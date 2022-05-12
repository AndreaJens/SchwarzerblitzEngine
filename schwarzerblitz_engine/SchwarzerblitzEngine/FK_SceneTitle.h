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

#ifndef FK_SCENETITLE_CLASS
#define FK_SCENETITLE_CLASS

#include<irrlicht.h>
#include<string>
#include<vector>
#include"FK_SceneWithInput.h"
#include"FK_SoundManager.h"

#include <SFML\Audio.hpp>

using namespace irr;

namespace fk_engine{

	class FK_SceneTitle : public FK_SceneWithInput{
	protected:
		const std::string TitleFilesDirectory = "scenes\\title\\";
		const std::string TitleConfigFileName = "config.ini";
		const std::string BackgroundPictureFileKey = "#BACKGROUND_PICTURE";
		const std::string BGMFileKey = "#BGM";
		const std::string CaptionTextFileKey = "#CAPTION_TEXT";
		const std::string CaptionColorFileKey = "#CAPTION_COLOR";
		const std::string CaptionPositionFileKey = "#CAPTION_POSITION";
		const std::string CaptionPeriodFileKey = "#CAPTION_PERIOD";
		const std::string AttractModeFileKey = "#ACTIVATE_DEMO_MODE";
		const u32 TransitionTimeMS = 1000;
		const u32 FlashTimeMs = 200;
	public:
		FK_SceneTitle();
		FK_SceneTitle(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void initialize();
		void update();
		void draw();
		void dispose();
		bool isRunning();
		FK_SceneType nameId();
	protected:
		void readConfigFile();
		void updateInput();
		void setupInputForPlayers();
		void setupSoundManager();
		void setupBGM();
		void drawBackground();
		void drawCaption();
		void drawOverlay();
		u32 now;
		u32 then;
		u32 delta_t_ms;
		std::string titleCaption;
		std::string resourcesPath;
		std::string configFileName;
		core::dimension2d<u32> screenSize;
		u32 cycleId;
		u32 transitionCounter;
		u32 flashCounter;
		std::string soundPath;
		FK_SoundManager* soundManager;
		/* BGM */
		std::string titleBGMName;
		f32 titleBGMvolume;
		f32 titleBGMpitch;
		sf::Music titleBGM;
		/* fonts and graphical elements */
		gui::IGUIFont *font;
		video::ITexture* backgroundTex;
		bool anyKeyPressed;
		bool cheatFlashEffect;
		u32 captionTimeMS;
		core::position2df captionPosition;
		std::wstring captionText;
		u32 captionPeriodDurationMS;
		video::SColor captionColor;
		u32 demoMatchTimerMs;
		u32 timeUntilDemoMatchMs;
		bool allowAttractMode;
	};
}
#endif