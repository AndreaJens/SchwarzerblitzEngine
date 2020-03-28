#ifndef FK_SCENEGAMEATTRACT_CLASS
#define FK_SCENEGAMEATTRACT_CLASS

#include "FK_SceneGame.h"

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine {

	class FK_SceneGameAttractMode : public FK_SceneGame {
	public:
		FK_SceneGameAttractMode();
		void setupAttractMode(
			IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			FK_AdditionalSceneGameOptions new_additionalOptions);
	protected:
		virtual bool canPauseGame();
		virtual void updateInputForPauseMenuAndSkipOptions();
		/* is running */
		virtual bool isRunning();
		/* timer update */
		virtual void updateRoundTimer(u32 delta_t_ms);
		/* hud */
		virtual void drawAdditionalHUDOverlay(f32 delta_t_s);
		std::wstring getSceneNameForLoadingScreen();
	private:
		void loadCharacterList();
		void loadStageList();
		bool isCharacterAvailable(u32 character_index);
		void drawOverlay();
	private:
		std::vector<u32> availableCharacters;
		std::vector<std::string> characterPaths;
		std::vector<std::string> stagePaths;
		u32 durationTimerMs;
		u32 attractModeDurationTimerMs;
		bool processSkippingDemoMatch;
		bool exitScene;
		u32 skipDemoMatchCounterMs;
		u32 fadeOutDurationMs;
	};
};

#endif