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

#pragma once
#include "FK_SceneWithInput.h"
#include "FK_SoundManager.h"
#include <SFML\Audio.hpp>

namespace fk_engine {

	class FK_SceneMusicPlayer : public FK_SceneWithInput {
	private:
		struct BGMStruct {
			std::string path;
			std::wstring title;
			std::wstring author;
			std::wstring description;
			f32 volume = 1.f;
			f32 pitch = 1.f;
		};
	public:
		FK_SceneMusicPlayer();
		FK_SceneMusicPlayer(IrrlichtDevice* newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		/* setup scene*/
		void setup(IrrlichtDevice* newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		virtual void dispose() override;
		virtual void update() override;
		virtual bool isRunning() override;
		virtual FK_Scene::FK_SceneType nameId() override;
	private:
		void initialize();
		void clear();
		bool loadMusicList();
		void loadSystemIcons();
		bool playSelectedSong();
		bool stopSelectedSong();
		bool pauseSelectedSong();
		void setupSoundManager();
		virtual void setupInputForPlayers() override;
		void updateInput();
		void drawAll();
		void drawBackground();
		void drawArrows();
		void drawTitle();
		void drawNowPlayingTab();
		void drawMusicProgressBar(s32 offsetX, s32 offsetY);
		void drawListOfTracks();
		void drawTrack(u32 trackIndex);
		void drawButtonIcons(s32 offsetX, s32 offsetY);
		void drawSingleButton(s32 &x, s32 &y, FK_Input_Buttons button, std::wstring text, bool active);
		void setupSystemButtonIcons();
		void setupGraphics();
		void increaseMenuIndex(s32 increase = 1);
		void decreaseMenuIndex(s32 decrease = 1);
		void selectItem();
		void cancelSelection();
	private:
		/* BGM */
		sf::Music music;
		bool isPlayingMusic;
		bool backToMenu;
		s32 musicIndex;
		s32 playingMusicIndex;
		std::vector<BGMStruct> availableSongs;
		FK_SoundManager* soundManager;
		u32 now;
		u32 then;
		u32 cycleId;
		u32 delta_t_ms;
		u32 lastInputTime;
		/* fonts and graphical elements */
		gui::IGUIFont* font;
		gui::IGUIFont* submenufont;
		gui::IGUIFont* captionFont;
		gui::IGUIFont* hintfont;
		core::dimension2d<u32> screenSize;
		float scale_factorX;
		float scale_factorY;
		/* graphic elements */
		video::ITexture* menuBackgroundTexture;
		video::ITexture* menuCaptionOverlay;
		video::ITexture* arrowUp_tex;
		video::ITexture* arrowDown_tex;
		std::map<FK_Input_Buttons, u32> buttonInputLayoutPlayer1;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayoutPlayer1;
		std::map<FK_Input_Buttons, u32> buttonInputLayoutPlayer2;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayoutPlayer2;
		std::map<FK_JoypadInput, video::ITexture*> joypadButtonsTexturesMap;
		video::ITexture* keyboardButtonTexture;
		bool player1UsesJoypad;
		bool player2UsesJoypad;
	};

}