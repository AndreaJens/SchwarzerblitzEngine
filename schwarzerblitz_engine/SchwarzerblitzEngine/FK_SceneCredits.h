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

#ifndef FK_SCENECREDITS_CLASS
#define FK_SCENECREDITS_CLASS

#include "FK_SceneWithInput.h"
#include <SFML\Audio.hpp>

using namespace irr;

namespace fk_engine{
	class FK_SceneCredits : public FK_SceneWithInput {
	public:
		const std::string CreditsResourcesDirectory = "scenes\\credits\\";
		const std::string TitleMarker = "**";
		const std::string SectionMarker = "--";
		const std::string EmptyLineMarker = "__";
		const std::string CreditsFileName = "credits.txt";
		const std::string PicturesFileName = "pictures.txt";
		const s32 defaultPictureDurationMs = 3000;
		const s32 defaultFadeInTimeMsSlideshow = 500;
		const video::SColor BackgroundColor = video::SColor(255, 0, 0, 0);
		const video::SColor NormalTextColor = video::SColor(255, 255, 255, 255);
		const video::SColor TitleColor = video::SColor(255, 200, 200, 50);
		const video::SColor SectionColor = video::SColor(255, 200, 200, 50);
		const video::SColor BorderColor = video::SColor(128, 0, 0, 0);
		const f32 AdvancementSpeed = 3.0f;
		const enum FK_CreditsStyle : u32{
			NormalText = 0,
			SectionText = 1,
			TitleText = 2
		};
	private:
		struct FK_CreditPicture {
			std::string path;
			s32 duration = 200;
		};
	public:
		FK_SceneCredits();
		FK_SceneCredits(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions, bool fadeToBlack,
			bool showPictures = false, std::string newConfigFile = std::string());
		void initialize();
		void update();
		void updateInput();
		void dispose();
		bool isRunning();
		FK_Scene::FK_SceneType nameId();
	protected:
		void setup();
		void drawAll();
		void drawLines();
		void drawBackground();
		void readLinesFromFile();
		void readPicturesFromFile();
		void drawLine(s32 x, s32 y, u32 index);
		void processLines();
		s32 processLine(s32 x, s32 y, std::string line);
		void setupBGM();
	private:
		/* lines to draw */
		std::vector<std::string> lines;
		std::vector<std::wstring> linesToDraw;
		std::vector<s32> linesToDrawY;
		std::vector<FK_CreditsStyle> linesToDrawStyle;
		/* screen */
		core::dimension2d<u32> screenSize;
		/* fonts */
		gui::IGUIFont *normalFont;
		gui::IGUIFont *sectionFont;
		gui::IGUIFont *titleFont;
		/* logic */
		bool terminateScene;
		u32 cycleId;
		/* coordinates*/
		f32 currentTextY;
		f32 lastElementPositionY;
		f32 basicSize;
		/* BGM */
		std::string creditsResourcesPath;
		std::string creditsBGMName;
		f32 creditsBGMvolume;
		f32 creditsBGMpitch;
		sf::Music credits_bgm;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 lastInputUpdateTime;
		video::ITexture* lastFrame;
		/* background opacity */
		f32 backgroundOpacity;
		f32 blackFadingOpacity;
		bool fadeToBlackFlag;
		u32 linesDrawn;
		s32 firstElementY;
		s32 referenceScreenShift;
		u32 firstElementIndex;
		bool showPicturesSlideshow;
		std::string configurationFileName;
		std::vector<FK_CreditPicture> pictures;
		s32 pictureDurationCounter;
		s32 currentPictureDurationMs;
		u32 pictureIndex;
	};
}

#endif