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