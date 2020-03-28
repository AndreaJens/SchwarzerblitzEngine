#ifndef FK_SCENECHARASELECTBASE_CLASS
#define FK_SCENECHARASELECTBASE_CLASS

#include "FK_SceneWithInput.h"
#include "FK_Character.h"
#include "FK_Announcer.h"
#include "FK_SoundManager.h"
#include "FK_SceneOptionsEmbedded.h"
#include <vector>
#include <deque>
#include <string>

using namespace irr;
namespace fk_engine{
	class FK_SceneCharacterSelect_Base : public FK_SceneWithInput{
	protected:
		const u32 DirectionInputThresholdMs = 150;
		const u32 NonDirectionInputThresholdMs = 300;
		const u32 InputThresholdMs = 200;
		const std::string SelectionResourcePath = "scenes\\character_selection\\";
		enum SelectionDirection : u32 {
			Down = 0,
			Up = 1,
			Left = 2,
			Right = 3
		};
		enum FK_CharacterSelectFileKeys {
			DefaultScreenSize,
			CharacterNamesOffset,
			ShowCharacterNames,
			ShowRingoutBox,
			AllowRingoutToggle,
			CharacterSelectorTexture,
			ShowOverlay,
			ShowBackground,
			BackgroundColor,
			CharacterSelectorColors,
			CharacterSelectorAnimationMode,
			CharacterSelectorAnimationParams,
			StartingIndexes,
			ScreenBGM,
			OverlayTexture,
			BackgroundTexture,
			BackgroundVelocity,
			BackgroundMode,
			MissingCharaTexture,
			MissingCharaOverlay,
			CostumeDotTextures,
			CharacterSelectIconViewport,
			CenterCharacterBox,
			CharacterNamesPosition,
			UseBitmapPreviewsInsteadOfMesh,
			CharacterIconMinSize,
			CharacterIconMaxSize,
			CharacterIconMinOffset,
			CharacterIconMaxOffset,
			CharacterIconRowColumnOffset,
			CharacterImagePreviewPosition,
			CharacterMeshIdlePosition,
			CharacterMeshSelectionPosition,
			CameraPosition,
			CharacterRotationAngle,
			Use2DCoordinatesForCharacters,
			DrawLoadingScreen,
			LoadingScreenTexture,
			LoadingScreenTextureArcadeLadder,
			ArcadeModeCompletedLogo,
			ShowArcadeModeCompletedLogo,
			MaximumNumberOfFullSizeCostumeDots,
			CostumeDotsVerticalPosition,
		};
		enum FK_SceneBackgroundMode : u32{
			Stretched = 0,
			Tiled = 1,
			KeepRatioWidthScale = 2,
			KeepRatioHeightScale = 3,
			CenteredUnstretched = 4,
			NumberOfBackgroundItems,
		};		
		enum FK_CursorAnimationMode : u32 {
			Zoom = 0,
			Alpha = 1,
			NumberOfAnimationItems,
		};
		enum FK_CenterCharacterBox : u32 {
			NoCentering = 0,
			FullCentering = 1,
			CenterOnlyX = 2,
			CenterOnlyY = 3,
			CenterCharacterBoxNoOfEntries,
		};
		struct FK_SceneResources {
			std::string backgroundTexture;
			std::string overlayTexture;
			std::string lockedCharacterIconTexture;
			std::string stageOverlayTexture;
			std::string characterSelectorTexture;
			std::string lockedCostumeMarkTexture;
			std::string currentCostumeIconTexture;
			std::string availableCostumeIconTexture;
			std::string loadingScreenTexture;
			std::string arcadeLadderloadingScreenTexture;
			std::string arcadeModeCompletedLogo;
			bool hasOverlay;
			bool hasBackground;
			bool drawStageBorderBox;
			bool allowRingoutToggle;
			bool showStageOverlay;
			bool showCharacterNames;
			bool showArcadeModeCompletedLogo;
			video::SColor backgroundColor;
			core::rect<f32> arcadeLogoPosition;
			core::rect<s32> stageBorderBoxPosition;
			core::rect<s32> characterSelectBoxPosition;
			FK_CenterCharacterBox centerCharacterBox;
			core::rect<s32> characterSelectOverlayPosition;
			core::rect<s32> stageSelectBoxPosition;
			core::rect<s32> stageSelectOverlayPosition;
			std::pair<s32, s32> startingPlayerIndexes;
			bool allowRandomCharacterSelection;
			bool allowRandomStageSelection;
			std::string randomCharacterPath;
			std::string randomStagePreviewTexture;
			core::dimension2d<u32> referenceScreenSize;
			core::dimension2d<f32> minIconSize;
			core::dimension2d<f32> maxIconSize;
			core::dimension2d<f32> minIconOffset;
			core::dimension2d<f32> maxIconOffset;
			core::dimension2d<f32> iconRowColumnOffset;
			core::vector2d<f32> backgroundMovementVelocity;
			std::pair<video::SColor, video::SColor> characterSelectorColors;
			f32 characterSelectorOscillationAmplitude;
			f32 characterSelectorOscillationOffset;
			f32 characterSelectorOscillationPeriodMS;
			std::pair<f32, f32> characterSelectorOscillationPhases;
			std::string BGMName;
			f32 BGMVolume;
			f32 BGMPitch;
			core::vector2d<s32> characterNameOffset;
			core::vector2d<s32> outfitSelectTextOffset;
			u32 maximumNumberOfFullSizeOutfitDots;
			s32 costumeDotsVerticalPosition;
			FK_SceneBackgroundMode backgroundMode;
			FK_CursorAnimationMode characterSelectorAnimationMode;
			core::vector3d<f32> player1CharacterPositionIdle;
			core::vector3d<f32> player2CharacterPositionIdle;
			f32 player1CharacterYRotationAngle;
			f32 player2CharacterYRotationAngle;
			core::vector3d<f32> player1CharacterPositionSelect;
			core::vector3d<f32> player2CharacterPositionSelect;
			core::vector3d<f32> cameraPosition;
			bool useImagePreviewsInsteadOfMesh;
			core::rect<f32> characterImagePreviewPositionPlayer1;
			core::rect<f32> characterImagePreviewPositionPlayer2;
			bool use2DCoordinates;
			bool drawLoadingScreen;
		};
		struct FK_SelectionPreview {
			std::string filename = std::string();
			video::ITexture* texture = NULL;
			bool selectable = true;
		};
	public:
		FK_SceneCharacterSelect_Base();
		virtual ~FK_SceneCharacterSelect_Base();
		virtual void drawPlayerSelector(u32 selectorIndex, bool playerReady, f32 phaseOffset, video::SColor selectorColor);
		virtual void drawCharacterPreview(bool player1Side, FK_SelectionPreview& preview);
		virtual void drawBackground();
		virtual void drawCharacterScreen();
		virtual void drawScreenOverlay();
		virtual void setupTextures();
		virtual void setupCamera();
		virtual void setupSoundManager();
		virtual void setupAnnouncer();
		virtual void setupBGM();
		virtual void loadSingleCharacterFile(std::string playerPath, std::string &playerName);
		virtual void setIconSizeAndOffset();
		virtual bool isCharacterAvailable(u32 index);
		virtual bool randomCanBeSelected(u32 inputButtons);
		virtual bool isSimpleModeSelected(u32 inputButtons);
		virtual bool isCharacterSelectable(FK_Character* character);
		virtual s32 getNearestNeighbourIndex(s32 index, SelectionDirection direction);
		virtual core::rect<s32> getCharacterIconPosition(u32 icon_index);
	protected:
		virtual u32 getNodeUsedTextures(ISceneNode* node, core::array<video::ITexture*>& a);
		virtual void removeCharacterNodeFromCache(FK_Character* character);
		virtual bool loadConfigurationFile();
		virtual void setupBasicResources();
		virtual void loadSystemIcons();
		virtual void setupSystemButtonIcons();
		virtual void calculateRaycastedCoordinates();
		virtual core::vector3df get3DPositionFrom2DCoordinates(core::vector3d<s32> coordinates);
		virtual core::vector3df getIdleCharacterPosition(bool player1Side);
		virtual core::vector3df getSelectedCharacterPosition(bool player1Side);
		virtual void drawLoadingScreen(f32 completionPercentage);
		virtual void drawLoadingScreenProgressBar(f32 completionPercentage);
		virtual void callEmbeddedSceneOptions();
		virtual void updateEmbeddedSceneOptions();
		virtual void disposeEmbeddedSceneOptions();
	protected:
		/* embedded scene options */
		FK_SceneOptionsEmbedded* embeddedSceneOptions;
		bool isEmbeddedSceneOptionsActive;
		/* application path */
		std::string stagesResourcePath;
		std::string charactersDirectory;
		std::string dummyCharacterDirectory;
		/* textures */
		video::ITexture* background;
		video::ITexture* overlay;
		video::ITexture* missingPreview;
		video::ITexture* lockedCostumeSign;
		video::ITexture* currentCostumeDotTex;
		video::ITexture* costumeDotTex;
		std::vector<video::ITexture*> characterIcons;
		std::map<FK_JoypadInput, video::ITexture*> joypadButtonsTexturesMap;
		video::ITexture* keyboardButtonTexture;
		video::ITexture* selector;
		video::ITexture* completedArcadeModeLogoTexture;
		/* camera */
		ICameraSceneNode* camera;
		/* time */
		u32 then;
		u32 now;
		u32 delta_t_ms;
		u32 lastInputUpdateTime;
		// icon size & offset
		core::dimension2d<f32> iconSize;
		core::dimension2d<f32> iconOffset;
		u32 maxRows;
		u32 maxColumns;
		s32 maxOccupiedRows;
		// screen variables
		core::vector2d<f32> offset;
		core::dimension2d<u32> screenSize;
		f32 scale_factorX;
		f32 scale_factorY;
		// player paths
		std::vector<u32> availableCharacters;
		std::vector<std::string> characterPaths;
		/* sound manager and announcer */
		std::string voiceClipsPath;
		std::string soundPath;
		FK_SoundManager* soundManager;
		FK_Announcer* announcer;
		/* BGM */
		std::string systemBGMPath;
		sf::Music character_selection_bgm;
		FK_SceneResources sceneResources;
		std::map<std::string, FK_Character*> characterInfos;
	private:
		bool characterRaycastedPositionCalculatedFlag;
		core::vector3d<f32> player1RaycastedCoordinates;
		core::vector3d<f32> player2RaycastedCoordinates;
		core::vector3d<f32> player1RaycastedSelectionCoordinates;
		core::vector3d<f32> player2RaycastedSelectionCoordinates;
	};
}


#endif