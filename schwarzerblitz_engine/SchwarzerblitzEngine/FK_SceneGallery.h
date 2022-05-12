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

#ifndef FK_SCENEEXTRAGALLERY_CLASS
#define FK_SCENEEXTRAGALLERY_CLASS

#include<irrlicht.h>

#include "FK_SceneExtra.h"
#include "FK_SoundManager.h"
#include "map"

using namespace irr;

namespace fk_engine {

	class FK_SceneGallery : public FK_SceneExtra {
	private:
		const std::string ConfigFileName = "config.txt";
		const std::string PicturesFileName = "pictures.txt";

		struct FK_GalleryConfigOptions {
			core::dimension2d<u32> selectionIconSize = core::dimension2d<u32>(160, 90);
			core::vector2d<u32> selectionGridRowsAndColumns = core::vector2d<u32>(3, 2);
			core::rect<s32> selectionGridDimension = core::rect<s32>(50, 70, 540, 210);
			std::string bgmName = "title(atmosphere)_cut.ogg";
			f32 bgmVolume = 100.f;
			f32 bgmPitch = 1.f;
			u32 blinkingPeriodMs = 1000;
			std::string galleryName = "Gallery";
			std::string unavailablePicturePlaceholder = "lockedPreview.jpg";
			std::string backgroundTexture = "..\\main_menu\\menuBackground_base.png";
			std::string overlayTexture = "..\\main_menu\\menu_CaptionOverlay.png";
			bool drawGalleryName = true;
			bool drawOverlay = true;
			bool drawPageNumber = true;
			bool drawFrameNumber = true;
			core::vector2d<u32> pageNumberPosition = core::vector2d<u32>(540, 290);
			core::vector2d<u32> frameNumberPosition = core::vector2d<u32>(570, 330);
		};
		struct FK_GalleryPicture {
			std::string uniqueId = std::string();
			std::wstring caption = std::wstring();
			std::wstring lockedCaption = L"[???]";
			u32 numberOfFrames = 0;
			std::vector<std::string> frames;
		};
	public:
		FK_SceneGallery();
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions);
		void update();
		void dispose();
		bool isRunning();
	private:
		void initialize();
		void setupSoundManager();
		void setupBGM();
		void setupGraphics();
		void setupInputForPlayers();
		void readConfigurationFile();
		void loadGalleryPictures();
		void setupGalleryPages();
		void updateInput();
		void updatePictureInput();
		void drawAll();
		void drawSelectionGrid();
		void drawBackground();
		void drawOverlay();
		void drawTitle();
		void drawCaption();
		void drawSelectedPicture();
		void drawPageNumber();
		void drawFrameNumber();
		void drawArrows();
		void drawSystemButtons();
		void loadNextPage(bool fromInput = false);
		void loadPreviousPage(bool fromInput = false);
		void loadCurrentPageTextures();
		void loadCurrentFrameTextures();
		void setupSystemButtonIcons();
		void loadSystemIcons();
		void clearTextureCacheForCurrentPage();
		void clearTextureCacheForCurrentPicture();
		void goBackToMainMenu();
		void cancelSelection();
		void selectItem();
		void increaseIndex();
		void decreaseIndex();
		void increaseRow();
		void decreaseRow();
		void increaseFrameIndex();
	private:
		/* basic variables*/
		std::string sceneResourcesPath;
		bool backToMenu;
		bool pictureIsSelected;
		s32 currentFrameIndex;
		s32 currentPageIndex;
		s32 currentPictureIndex;
		/* time variables */
		u32 now;
		u32 then;
		u32 delta_t_ms;
		/* screen variables */
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
		/* configuration/customization options */
		FK_GalleryConfigOptions configOptions;
		std::vector<video::ITexture*> pageTextures;
		std::vector<video::ITexture*> frameTextures;
		std::map<std::string, bool> availablePictures;
		std::map<std::string, FK_GalleryPicture> picturesSet;
		std::vector<std::string> galleryPictures;
		std::vector<std::vector<std::string>> galleryPages;
		/* generic textures */
		video::ITexture* backgroundTexture;
		video::ITexture* overlayTexture;
		video::ITexture* lockedPictureTexture;
		/* arrows */
		video::ITexture* arrowLeft_tex;
		video::ITexture* arrowRight_tex;
		/* fonts */
		gui::IGUIFont* font;
		gui::IGUIFont* mediumFont;
		gui::IGUIFont* captionFont;
		/* title */
		std::wstring sceneName;
		/* time for input */
		u32 lastInputTimePlayer1;
		u32 lastInputTimePlayer2;
		/* button textures for navigation */
		std::map<FK_JoypadInput, video::ITexture*> joypadButtonsTexturesMap;
		video::ITexture* keyboardButtonTexture;
		bool playerUsesJoypad;
		std::map<FK_Input_Buttons, u32> buttonInputLayout;
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayout;
	};
}

#endif
