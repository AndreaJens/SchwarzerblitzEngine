#ifndef FK_SCENEOPTIONS_CLASS
#define FK_SCENEOPTIONS_CLASS

#include "FK_SceneWithInput.h"
#include "FK_SceneInputMapping.h"
#include "FK_SoundManager.h"
#include "FK_Tutorial.h"
#include <vector>
#include <map>
#include <string>

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{
	class FK_SceneOptions : public FK_SceneWithInput{
	public:
		FK_SceneOptions();
		FK_SceneOptions(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		virtual void initialize();
		virtual void update();
		virtual void update(bool drawSceneInsideUpdate);
		virtual void dispose();
		void softDispose();
		virtual FK_SceneType nameId();
		virtual bool isRunning();
		bool needsScreenRefreshing();
	protected:
		u32 getCurrentTime();
		bool isInInputAssignmentScreen();
		void drawKeyMappingScene(bool drawSceneBackground);
		void updateStandardScene(bool drawScene = true);
		void updateKeyMapping(bool drawScene = true);
		void initializeKeyboardMappingSolo();
		void initializeKeyboardMappingMultiplayer();
		void initializeJoypadMapping();
		virtual void drawBackground();
		virtual void drawCaptions();
		void drawTitle();
		void drawBasicCategories();
		void drawItems();
		void drawItemValues();
		void drawAll();
		void updateInput();
		void setupSoundManager();
		virtual void setupBGM();
		void setupGraphics();
		virtual void setupMenuItems();
		void setupTutorial();
		void updateTutorial();
		void selectItem();
		void increaseMenuIndex();
		void decreaseMenuIndex();
		void goBackToMainMenu();
		void playAssignedButtonSound();
		void onConfirm();
		void onCancel();
		std::wstring optionToString(u32 optionIndex);
		void increaseOption(u32 optionIndex);
		void decreaseOption(u32 optionIndex);
		u32 getLastOptionIndex();
		u32 getCurrentOptionIndex();
		std::vector<u32> getCurrentIndexArray();
	protected:
		enum FK_SceneOptions_SubSystem : s32 {
			GraphicsOptions = 0,
			SoundOptions = 1,
			GameOptions = 2,
			KeyboardRemappingSolo = 3,
			KeyboardRemappingMultiplayer = 4,
			JoypadRemapping = 5,
			JoypadToXboxTranslation = 6,
			ShowTutorial = 7,
			ShowCredits = 8,
			BackToMenu = -1,
			NoCategory = -2
		};
		enum FK_SceneOptions_OptionsIndex : u32 {
			Opt_FullscreenMode = 0,
			Opt_Resolution = 1,
			Opt_DynamicResolution = 16,
			Opt_AILevel = 15,
			Opt_BorderlessWindowSamplingFactor = 5,
			Opt_BGMVolume = 10,
			Opt_SFXVolume = 6,
			Opt_BGMMute = 9,
			Opt_SFXMute = 5,
			Opt_VoiceMute = 7,
			Opt_VoiceVolume = 8,
			Opt_ShadowQuality = 3,
			Opt_FreeMatchTimer = 12,
			Opt_ArcadeTimer = 11,
			Opt_FreeMatchRounds = 14,
			Opt_ArcadeRounds = 13,
			Opt_PostProcessingEffects = 4,
			Opt_LightEffects = 2,
		};

		std::vector<std::string> menuOptionsStrings;
		std::vector<std::string> menuOptionsCaptions;
		std::vector<u32> graphicsOptionsIndex;
		std::vector<u32> soundOptionsIndex;
		std::vector<u32> gameOptionsIndex;
		std::vector<bool> additionalSpacingAfterOptionIndex;
		std::vector<bool> menuActiveItems;
		std::vector<std::string> menuOptionsCategoriesStrings;
		std::vector<std::string> menuOptionsCategoriesCaptions;
		std::vector<FK_SceneOptions_SubSystem> menuOptionsCategoriesClasses;
		std::vector<bool> menuOptionsCategoriesActiveItems;

	private:
		u32 menuIndex;
		core::vector2d<s32> options_spacing;
		FK_SceneOptions_SubSystem currentCategory;
		u32 currentCategoryIndex;
		
		bool subcategorySelected;
		bool itemSelected;
		core::dimension2d<u32> initialResolution;
		FK_Options::FK_ScreenMode initialScreenMode;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 lastInputUpdateTime;
		u32 captionMovementTimeCounterMs;
		u32 captionStillPeriodThresholdMs;
		u32 captionStillPeriodCounterMs;
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
		gui::IGUIFont *submenufont;
		gui::IGUIFont *captionFont;
		core::dimension2d<u32> screenSize;
		float scale_factorX;
		float scale_factorY;
		/* graphic elements */
		video::ITexture* menuBackgroundTexture;
		video::ITexture* menuCaptionOverlay;
		video::ITexture* arrowLtex;
		video::ITexture* arrowRtex;
		video::ITexture* arrowLtexSelected;
		video::ITexture* arrowRtexSelected;
		bool backToMenu;
		FK_Options backupOptions;
		FK_Scene* keyMappingSubscene;
		u32 lastInputPlayer1;
		u32 lastInputPlayer2;
		FK_Tutorial* gameTutorial;
		bool showTutorial;
		u32 lastInputTimePlayer1;
		u32 lastInputTimePlayer2;
	};
}


#endif