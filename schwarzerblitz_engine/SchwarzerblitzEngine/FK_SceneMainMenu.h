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

#ifndef FK_SCENEMAINMENU_CLASS
#define FK_SCENEMAINMENU_CLASS

#include "FK_SceneWithInput.h"
#include "FK_SoundManager.h"
#include "FK_InGameMenu.h"
#include "FK_DialogueWindow.h"
#include <vector>
#include <map>
#include <string>

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{
	class FK_SceneMainMenu : public FK_SceneWithInput{
	protected:
		const std::string MenuResourcesPath = "scenes\\main_menu\\";
		enum FK_MainMenuConfigFileKeys {
			MenuBGM,
			BackgroundPicture,
			CaptionBackground,
			ItemBackground,
			SelectedItemBackground,
			MainItemViewport,
			MainItemSize,
			MainItemTextOffset,
			SubItemSize,
			SubItemViewport,
			SubItemTextOffset,
			MainItemTextCentered,
			SubItemTextCentered,
			ItemBlinkingColor,
			MainMenuItems,
			MainMenuItemsEnd,
			FreeMatchItems,
			FreeMatchItemsEnd,
			ArcadeItems,
			ArcadeItemsEnd,
			TrainingItems,
			TrainingItemsEnd,
			ExtraItems,
			ExtraItemsEnd,
			SecretModeDefaultCaption,
			ShowCaptions,
			CancelBackToIntro,
			ShowMenuWithSubMenu,
			TutorialMessage,
			TutorialMessageEnd,
			ShowTutorialPrompt,
			ShowUnlockablesPrompt,
			UnlockablesMessage,
			UnlockablesMessageEnd,
			UnlockedCharacterMessage,
			UnlockedOutfitMessage,
			UnlockedStageMessage,
			UnlockedDioramaMessage,
			UnlockedGameModeMessage,
			UnlockedTrialMessage,
			UnlockedPictureMessage,
			UnlockedStoryEpisodeMessage,
		};
		enum FK_MenuTextAlignment : s32 {
			Left = -1,
			Center = 0,
			Right = 1,
		};
		enum FK_MenuSubMenuType : s32 {
			NoSubmenu = -1,
			FreeMatch = 0,
			Arcade = 1,
			Training = 2,
			Extra = 3,
		};
		struct FK_BlinkingItemColor {
			f32 colorTransitionPeriodMs = 1000.0f;
			video::SColor startingColor = video::SColor(255, 255, 255, 255);
			video::SColor endingColor = video::SColor(255, 255, 255, 255);
			video::SColor getCurrentColor(u32 now);
		};
	public:
		FK_SceneMainMenu();
		~FK_SceneMainMenu();
		FK_SceneMainMenu(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions, s32 menuIndex = 0);
		virtual void initialize();
		virtual void update();
		virtual void dispose();
		virtual FK_SceneType nameId();
		virtual bool isRunning();
		virtual bool player1HasSelected();
		FK_SceneFreeMatchType getFreeMatchType();
		FK_SceneArcadeType getArcadeType();
		FK_SceneTrainingType getTrainingType();
		FK_SceneExtraType getExtraType();
		s32 getCurrentMenuIndex();
		void compareUnlockables();
	protected:
		void playAssignedButtonSound();
		//
		void clearMenuVariables();
		//
		void drawBackground();
		void drawCaptions();
		void drawItems();
		void drawItemBars();
		void drawItemBar(s32 index, bool selected);
		//
		void drawSubMenuItemBars(u32 number, s32 index, u32 width);
		void drawSubMenuItems(std::vector<std::string> optionsStrings, s32 index);
		void drawFreeMatchItems();
		void drawFreeMatchItemBars();
		void drawArcadeItems();
		void drawArcadeItemBars();
		void drawTrainingItemBars();
		void drawTrainingItems();
		void drawExtraItemBars();
		void drawExtraItems();
		void drawSubMenuItemBar(s32 index, bool selected, u32 width = 280);
		//
		void drawAll();
		void updateInput();
		//
		void setupDefaultResources();
		bool loadConfigurationFile();
		void appendNewMenuItem(std::ifstream & inputFile, FK_SceneType sceneType);
		void appendNewMenuItemFreeMatch(std::ifstream & inputFile, FK_SceneFreeMatchType sceneType);
		void appendNewMenuItemArcade(std::ifstream & inputFile, FK_SceneArcadeType sceneType);
		void appendNewMenuItemTraining(std::ifstream & inputFile, FK_SceneTrainingType sceneType);
		void appendNewMenuItemExtra(std::ifstream & inputFile, FK_SceneExtraType sceneType);
		void setUnavailableMenuItems();
		//
		void setupSoundManager();
		void setupBGM();
		void setupGraphics();
		void setupMenuItems();
		void setupFreeMatchMenuItems();
		void setupArcadeMenuItems();
		void selectItem();
		bool isSubMenuActive();
		void increaseMenuIndex();
		void decreaseMenuIndex();
		void increaseMainMenuIndex();
		void decreaseMainMenuIndex();
		// subMenu
		void increaseSubMenuIndex();
		void decreaseSubMenuIndex();
		void cancelSubMenuSelection();
		void startSubMenuSelection();
		FK_MenuSubMenuType getCurrentSubMenuType();
		// tutorial menu
		void setupTutorialMenu();
		void updateTutorialMenu(u32 pressedButton);
		void activateTutorialMenu();
		void deactivateTutorialMenu();
		// unlock all menu
		void setupUnlockablesMenu();
		void updateUnlockablesMenu(u32 pressedButton);
		void activateUnlockablesMenu();
		void deactivateUnlockablesMenu();
		// unlockables
		void setupUnlockableWindow(std::vector<std::string> content);
		void updateUnlockableDisplay(u32 pressedButton);
		std::vector<std::string> processUnlockableContent();
		void activateUnlockableWindow();
		void deactivateUnlockableWindow();
	private:
		u32 menuIndex;
		core::vector2d<s32> options_spacing;
		std::vector<std::string> menuOptionsStrings;
		std::vector<FK_Scene::FK_SceneType> menuOptionsScenes;
		std::vector<std::string> menuOptionsCaptions;
		std::vector<bool> menuActiveItems;
		bool itemSelected;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 lastInputUpdateTime;
		u32 captionMovementTimeCounterMs;
		u32 captionStillPeriodThresholdMs;
		u32 captionStillPeriodCounterMs;
		u32 beforeTutorialTimer;
		u32 tutorialIntroMaxTime;
		/* additional variables (e.g. to check scene flow)*/
		u32 cycleId;
		/* sound manager and announcer */
		std::string voiceClipsPath;
		std::string soundPath;
		FK_SoundManager* soundManager;
		/* BGM */
		std::string menuBGMName;
		f32 menuBGMvolume;
		f32 menuBGMpitch;
		std::string systemBGMPath;
		sf::Music menu_bgm;
		/* transition counter (used when scene ends) */
		u32 transitionCounter;
		/* fonts and graphical elements */
		gui::IGUIFont *font;
		gui::IGUIFont *subMenufont;
		gui::IGUIFont *captionFont;
		core::dimension2d<u32> screenSize;
		float scale_factorX;
		float scale_factorY;
		/* graphic elements */
		video::ITexture* menuBackgroundTexture;
		video::ITexture* menuCaptionOverlay;
		video::ITexture* itemBarTexture;
		video::ITexture* itemBarSelectedTexture;

		u32 subMenuIndex;
		u32 subMenuSize;
		bool isSelectingSubMenu;
		FK_MenuSubMenuType subMenuType;

		// subMenu freematch
		std::vector<std::string> freeMatchMenuOptionsStrings;
		std::vector<FK_Scene::FK_SceneFreeMatchType> freeMatchMenuOptionsScenes;
		std::vector<std::string> freeMatchMenuOptionsCaptions;
		std::vector<bool> freeMatchMenuActiveItems;

		// subMenu arcade
		std::vector<std::string> arcadeMenuOptionsStrings;
		std::vector<FK_Scene::FK_SceneArcadeType> arcadeMenuOptionsScenes;
		std::vector<std::string> arcadeMenuOptionsCaptions;
		std::vector<bool> arcadeMenuActiveItems;

		// subMenu extra
		std::vector<std::string> extraMenuOptionsStrings;
		std::vector<FK_Scene::FK_SceneExtraType> extraMenuOptionsScenes;
		std::vector<std::string> extraMenuOptionsCaptions;
		std::vector<bool> extraMenuActiveItems;

		// subMenu training
		std::vector<std::string> trainingMenuOptionsStrings;
		std::vector<FK_Scene::FK_SceneTrainingType> trainingMenuOptionsScenes;
		std::vector<std::string> trainingMenuOptionsCaptions;
		std::vector<bool> trainingMenuActiveItems;
		
		bool lastSelectionFromPlayer1;
		FK_InGameMenu* tutorialMenu;
		FK_DialogueWindow* tutorialTextWindow;
		FK_InGameMenu* unlockablesMenu;
		FK_DialogueWindow* unlockablesTextWindow;
		// resources
		std::string menuBackgroundTextureName;
		std::string menuCaptionBackgroundTextureName;
		std::string menuItemBarTextureName;
		std::string menuSelectedItemBarTextureName;
		// item viewport
		core::rect<s32> itemMainViewport;
		core::rect<s32> itemMainSize;
		core::rect<s32> itemMainTextOffset;
		core::rect<s32> itemSubViewport;
		core::rect<s32> itemSubSize;
		core::rect<s32> itemSubTextOffset;
		FK_MenuTextAlignment itemMainAlignment;
		FK_MenuTextAlignment itemSubAlignment;
		FK_BlinkingItemColor itemBlinkingColor;
		std::string secretModeDefaultCaption;
		bool itemShowCaptions;
		bool backToIntroWhenCancelIsPressed;
		bool showMenuWithSubMenu;
		// tutorial prompt
		bool showTutorialPrompt;
		bool showUnlockablesPrompt;
		std::vector<std::string> tutorialPromptMessage;
		std::vector<std::string> unlockablesPromptMessage;
		// unlockables
		FK_UnlockedContent unlockablesToProcess;
		FK_DialogueWindow* unlockableContentWindow;
		u32 unlockableWindowTimer;
		bool canProcessUnlockedContent;
		std::string characterUnlockedMessage;
		std::string characterOutfitUnlockedMessage;
		std::string stageUnlockedMessage;
		std::string pictureUnlockedMessage;
		std::string characterDioramaUnlockedMessage;
		std::string gameModeUnlockedMessage;
		std::string storyEpisodeUnlockedMessage;
		std::string trialUnlockedMessage;
		/* input time */
		u32 lastInputTime;
	};
}


#endif