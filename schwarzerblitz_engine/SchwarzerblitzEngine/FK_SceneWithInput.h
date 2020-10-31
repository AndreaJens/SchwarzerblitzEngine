#ifndef FK_SCENEINPUT_CLASS
#define FK_SCENEINPUT_CLASS

#include <irrlicht.h>
#include"FK_Scene.h"
#include "FK_InputReceiver.h"
#include "FK_InputBuffer.h"
#include "FK_InputMapper.h"
#include "FK_Options.h"
#include "FK_CharacterStatistics_Struct.h"
#include "FK_DatabaseAccessor.h"
#include "FK_PixelShaderSceneGameCallback.h"
#include "FK_DialogueWindow.h"
#include "FK_Cheat.h"
#include "FK_WorkshopContentManager.h"
#include <string>

using namespace irr;
using namespace gui;
using namespace video;
using namespace scene;

namespace fk_engine{
	class FK_SceneWithInput : public FK_Scene{
	private:
		u32 JoypadSetupTestTimeMS = 2000;
		u32 JoypadSetupConfirmTimeMS = 2000;
		u32 MinimumNumberOfJoypadButtonsToSet = 7;
		u32 MinimumTimeFromLastButtonToAbortCommandMS = 1000;
		u32 TimeThresholdForMenuInputsMS = 150;
		u32 TimeThresholdForMenuInputMinimumMS = 35;
		u32 TotalTimeBeforeNoThresholdInMenuInputsMS = 450;
	private:
		u32 lastMenuInputDirectionPressed = 0;
		u32 timeSinceLastInputMS = 0;
		u32 currentActiveCheatCodes = 0;
	public:
		enum StageUnlockKeys : s32 {
			Stage_AvailableFromStart = 0,
			Stage_Unlockable = 1,
			Stage_OnlyFreeMatch_AvailableFromStart = 2,
			Stage_OnlyFreeMatch_Unlockable = 3,
		};
		enum CharacterUnlockKeys : s32 {
			Character_AvailableFromStart = 0,
			Character_UnlockableAndShown = 1,
			Character_Hidden = -1,
			Character_FreeMatchOnly_AvailableFromStart = 2,
			Character_FreeMatchOnly_UnlockableAndShown = 3,
			Character_FreeMatchOnly_Hidden = -2,
			Character_UnplayableAndHidden = -5,
			Character_Unlockable_NoArcadeOpponent = -3,
			Character_Unlockable_Hidden_NoArcadeOpponent = -4,
		};
		struct FK_UnlockedContent {
			std::vector<std::string> unlockedCharacters;
			std::vector<std::string> unlockedStages;
			std::vector<std::string> unlockedOutfits;
			std::vector<std::string> unlockedGameModes;
			std::vector<std::string> unlockedStoryEpisodes;
			std::vector<std::string> unlockedTutorialChallenges;
			std::vector<std::string> unlockedCharacterDioramas;
			std::vector<std::string> unlockedArcadeEndings;
			std::deque<u32> unlockedGalleryPictures;
			void clear();
		};
	public:
		FK_SceneWithInput();
		FK_SceneWithInput(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		virtual ~FK_SceneWithInput() {};
		virtual void setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions);
		core::array<SJoystickInfo> getJoypadInfo();
		virtual void dispose(bool disposeGraphics = true);
		virtual FK_SceneType getNextScene();
		virtual FK_SceneType getPreviousScene();
		virtual void setNextScene(FK_SceneType);
		virtual void setPreviousScene(FK_SceneType);
		virtual IrrlichtDevice* getIrrlichtDevice() const;
		virtual FK_InputReceiver* getInputReceiver();
		virtual void setPreviouslyUnlockedContent(FK_UnlockedContent oldContent);
		virtual FK_UnlockedContent getPreviouslyUnlockedContent();
		virtual FK_UnlockedContent getCurrentlyUnlockedContent();
		virtual f32* getSceneParametersForShader(FK_SceneShaderTypes newShader);
		virtual void resetInputRoutines(bool createInputMapIfNotFound = false);
		u32 getCurrentActiveCheatCodes();
		void setCurrentActiveCheatCodes(u32 newCheatCodes);
	protected:
		void setupIrrlichtDevice();
		void executePreRenderingRoutine(FK_Options* options = NULL);
		void executePostRenderingRoutine(FK_Options* options = NULL);
		virtual void setupJoypad();
		void setupInputMapper(bool createMapIfNotFound = true);
		virtual void setupInputForPlayers(s32 timeWindowBetweenInputBeforeClearing_ms,
			s32 mergeTimeForInputAtSameFrame_ms,
			s32 timeToRegisterHeldButton_ms,
			s32 timeToRegisterNullInput_ms,
			s32 maxNumberOfRecordedButtonInInputBuffer);
		virtual void setupInputForPlayers();
		void loadJoypadSetupTextures();
		void updateJoypadSetup(u32 delta_t_ms);
		void drawJoypadSetup(u32 time_ms);
		virtual void playAssignedButtonSound();
		bool hasToSetupJoypad();
		std::string getCharacterNameFromFile(std::string characterPath);
		void readRewardFile();
		void writeSaveFile();
		void readSaveFile();
		void loadWorkshopContent();
		void loadDLCContent();
		virtual void updateSaveFileData();
		void setupJoypadConfigWindow(std::vector<std::string>& content);
		void setJoypadConfigWindowText(std::vector<std::string>& content);
		void activateJoypadConfigWindow();
		void deactivateJoypadConfigWindow();
		void setIconsBasedOnCurrentInputMap(std::map<FK_Input_Buttons, u32>&buttonInputLayout,
			std::map<FK_Input_Buttons, std::wstring> &keyboardInputLayout,
			s32 mapIndex, bool useJoypadMap = false,
			std::string joypadName = std::string());
		u32 getMenuInputTimeThresholdMS();
		void updateMenuInputTimeThresholdMS(u32 inputPressed, u32 delta_t_ms);
		// cheats
		void readCheatsFile(); 
		bool updateCheatsCheck();
		bool isInputingCheat();
		void checkGalleryPictures();
	protected:
		/* irrlicht device */
		IrrlichtDevice* device;
		IVideoDriver* driver;
		ISceneManager* smgr;
		IGUIEnvironment* guienv;
		/* input manager */
		FK_InputMapper *inputMapper;
		/* input buffer */
		FK_InputBuffer *player1input;
		FK_InputBuffer *player2input;
		/* input receiver*/
		FK_InputReceiver* inputReceiver;
		/* game options */
		FK_Options* gameOptions;
		/* joypad info */
		core::array<SJoystickInfo> joystickInfo;
		/* application path */
		std::string applicationPath;
		std::string fontPath;
		std::string commonResourcesPath;
		std::string mediaPath;
		std::string joypadSetupTexturesPath;
		std::string configurationFilesPath;
		/* scene flow */
		FK_SceneType nextSceneTag;
		FK_SceneType previousSceneTag;
		/* joypad settings */
		std::deque<std::pair<std::string, u32> > newJoypadFound;
		std::vector<video::ITexture*> joypadSettingsTextures;
		u32 inputSetIndex;
		u32 oldKeyCode;
		std::string newJoypadName;
		std::vector<u32> buttonsSet;
		u32 newJoypadId;
		video::ITexture* borderlessWindowRenderTarget;
		std::vector<std::string> unlockedCharacters;
		std::vector<std::string> unlockedStages;
		std::vector<std::string> unlockedOutfits;
		std::vector<std::string> unlockedGameModes;
		std::vector<std::string> unlockedStoryEpisodes;
		std::vector<std::string> unlockedTutorialChallenges;
		std::vector<std::string> unlockedCharacterDioramas;
		std::vector<std::string> unlockedArcadeEndings;
		std::deque<u32> unlockedGalleryPictures;
		bool unlockAllFlag;
		bool unlockAllCharactersFlag;
		bool unlockAllStagesFlag;
		bool unlockExtraCostumesForeverFlag;
		std::map<u32, u32> numberOfArcadeBeatenPerDifficultyLevel;
		std::map<u32, std::pair<std::string, u32>> survivalRecordPerDifficultyLevel;
		std::map<u32, std::pair<std::string, u32>> timeAttackRecordPerDifficultyLevel;
		std::map<std::string, FK_CharacterStatistics> characterStatisticsStructure;
		std::vector<std::string> storyEpisodesCleared;
		std::vector<std::string> tutorialChallengesCleared;
		std::vector<std::string> specialTagsAcquired;
		bool tutorialCleared;
		bool tutorialPresented;
		bool unlockablesChoicePresented;
		FK_CharacterStatistics globalStatistics;
		/* database accessor */
		FK_DatabaseAccessor databaseAccessor;
		FK_UnlockedContent previouslyUnlockedContent;
		FK_UnlockedContent currentlyUnlockedContent;
		/* joypad setup */
		s32 joypadUpdateTimeMS;
		u32 joypadCosmeticDotsNumber;
		std::vector<u32> buttonPressTime;
		std::vector<bool> buttonsNotToFire;
		std::vector<u32> inputSetIndexMap;
		FK_DialogueWindow* newJoypadWindow;
		bool joypadConfiguredWindowShown;
		u32 joypadConfiguredWindowTimerMS;
		u32 joypadConfigCurrentNumberOfButtons;
		u32 joypadConfigTimeSinceLastButtonPress;
		std::vector<FK_Cheat> availableCheats;
		std::vector<FK_WorkshopContentManager::WorkshopItem> enabledWorkshopItems;
		std::vector<FK_WorkshopContentManager::WorkshopItem> enabledDLCItems;
	};
}
#endif