#ifndef FK_SCENEDIORAMA_CLASS
#define FK_SCENEDIORAMA_CLASS

#include<irrlicht.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "XEffects.h"
#include "FK_Character.h"
#include "FK_WorldCollisionCallback.h"
#include "FK_Stage.h"
#include "FK_SceneExtra.h"
#include "FK_SoundManager.h"
#include "FK_PixelShaderStageCallback.h"
#include "FK_PixelShaderSceneGameCallback.h"
#include "FK_CharacterMaterialShaderCallback.h"
#include "FK_InGameMenu.h"
#include "FK_ScrollTextWindow.h"

using namespace irr;

namespace fk_engine {

	class FK_SceneDiorama : public FK_SceneExtra {
	protected:
		const f32 MinimalCameraDistance = 35.f;
		const f32 MaximalCameraDistance = 120.f;
		const f32 InitialCameraDistance = 70.f;
		const f32 InitialCameraAngle = 30.f;
		const f32 DeltaCameraAnglePerSecond = 60.f;
		const f32 DeltaCameraDistancePerSecond = 40.f;
		const f32 DeltaCameraOffsetYPerSecond = 25.f;
		const f32 InitialYOffset = 5.f;
		const f32 MinimalYOffset = -20.f;
		const f32 MaximalYOffset = 35.f;
		const u32 IntroDurationMs = 500;
		const f32 StageCameraDistance = 130.f;
		const f32 StageCameraOffset = 10.f;
		const f32 StageCameraTargetOffset = 30.f;
		const f32 StageCameraDeltaCameraAnglePerSecond = 15.f;
	protected:
		enum FK_DioramaMode : u32 {
			DioramaMenu = 0,
			DioramaProfile = 1,
			DioramaStage = 2,
			DioramaFreeCamera = 3,
		};
		struct FK_DioramaCamera {
			f32 cameraDistance = 1.f;
			f32 cameraAngle = 0.f;
			f32 cameraOffset = 0.f;
			f32 cameraTargetOffset = 0.f;
		};
		struct FK_DioramaCharacterInfo {
			std::wstring age;
			std::wstring nationality;
			std::wstring height;
			std::wstring weight;
			std::wstring nickname;
			std::vector<std::wstring> signatureMoves;
		};
	public:
		FK_SceneDiorama();
		void setup(IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			std::string new_characterFilename,
			FK_Character::FK_CharacterOutfit characterOutfit,
			std::string new_stagePath = std::string());
		virtual bool isRunning();
		void dispose();
		f32* getSceneParametersForShader(FK_SceneShaderTypes newShader);
	protected:
		void initialize();
		virtual void setupResourcesPaths(std::string new_player1path, std::string new_arenaPath);
		void setupAllObjects();
		void setupStage();
		void setupCharacter();
		void setupCharacterStageInteraction();
		void showCharacter();
		void hideCharacter();
		void setupLights();
		void setupSoundManager();
		void setupCamera();
		void setupCharacterShader();
		void setupTextWindow();
		void resetCameraPosition();
		void setCameraPosition(f32 distance, f32 angle, f32 offsetY, f32 targetOffsetY = 0.f);
		void updateCameraPosition(u32 pressedButtons, u32 delta_t_ms);
		void updateStagePanoramicCamera(u32 delta_t_ms);
		void updateInput(u32 delta_t_ms);
		void update();
		void draw();
		void drawCharacterInformation();
		void loadDioramaInformation();
		void setupMenu();
		void activateProfileView();
		void activateStageView();
		void deactivateTextWindow();
		bool isValidCharacterPath(std::string path);
		bool isValidStagePath(std::string path);
	private:
		// stage
		FK_Stage* stage;
		// character
		FK_Character* character;
		/* voice clips path */
		std::string voiceClipsPath;
		/* scene camera & scene manager */
		ICameraSceneNode * camera;
		/* collision manager with the arena*/
		FK_WorldCollisionCallback *characterCollisionCallback;
		/* light effects */
		EffectHandler* lightManager;
		E_FILTER_TYPE shadowFilterType;
		E_SHADOW_MODE shadowModeCharacters;
		E_SHADOW_MODE shadowModeStage;
		/* time information */
		u32 then;
		u32 now;
		u32 cycleId;
		s32 introCounter;
		s32 outroCounter;
		/* flux control*/
		bool backToCharacterSelection;
		/* paths */
		std::string character_configPath;
		std::string stage_configPath;
		std::string stagesPath;
		std::string charactersPath;
		/* sound manager */
		FK_SoundManager* soundManager;
		FK_Character::FK_CharacterOutfit characterOutfit;
		/* BGM */
		sf::Music stage_bgm;
		/* callback for various shaders */
		std::map<std::string, FK_PixelShaderSceneGameCallback*> scenePixelShaderCallback;
		/* stage shaders */
		std::vector<FK_PixelShaderStageCallback*> stagePixelShaderCallback;
		f32 * scenePixelShaderParameters;
		// shaders
		std::vector<s32> characterMeshMaterials;
		// camera variables
		f32 oldDistance;
		f32 oldAngle;
		f32 oldOffsetY;
		f32 oldTargetOffsetY;
		// screen variables
		core::dimension2d<u32> screenSize;
		// menu
		FK_InGameMenu* dioramaMenu;
		FK_DioramaMode dioramaMode;
		// character camera
		FK_DioramaCamera characterCamera;
		FK_DioramaCamera defaultCamera;
		FK_DioramaCamera stageCamera;
		FK_Pose characterPose;
		std::vector<std::string> characterObjectsToToggle;
		// text window
		FK_ScrollTextWindow* characterTextWindow;
		FK_ScrollTextWindow* stageTextWindow;
		std::vector<std::string> characterProfileLines;
		std::vector<std::string> stageInformationLines;
		FK_DioramaCharacterInfo dioramaCharacterInfo;
		f32 scale_factorX;
		f32 scale_factorY;
	};
}

#endif
