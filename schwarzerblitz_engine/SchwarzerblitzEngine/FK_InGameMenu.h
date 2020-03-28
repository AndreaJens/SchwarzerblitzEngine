#ifndef FK_INGAMEMENUCLASS
#define FK_INGAMEMENUCLASS

#include"FK_Window.h"
#include"FK_SoundManager.h"
#include"ExternalAddons.h"
#include"FK_Options.h"
#include<irrlicht.h>
#include<vector>

using namespace irr;

namespace fk_engine{
	class FK_InGameMenu{
	public:
		FK_InGameMenu();
		FK_InGameMenu(IrrlichtDevice *new_device, FK_SoundManager* scene_soundManager, 
			FK_Options* scene_options, std::vector<std::string> items, std::string skinName, std::string skinPath, f32 x = -1, f32 y = -1);
		void setup(IrrlichtDevice *new_device, FK_SoundManager* scene_soundManager, 
			FK_Options* scene_options, std::vector<std::string> items, std::string skinName, std::string skinPath, f32 x = -1, f32 y = -1);
		void reset();
		void resetSelection();
		void setViewport(core::position2di position, core::dimension2d<u32> viewportSize);
		s32 getIndex();
		bool isActive();
		bool isVisible();
		bool itemIsSelected();
		void setActive(bool new_active_flag);
		void setVisible(bool new_visibility_flag);
		void update(u32 pressedButton, bool playSoundOnItemSelect = true);
		core::rect<s32> getFrame();
		void draw();
		void setPosition(s32 x, s32 y);
		void resetLastInputTime();
		bool canAcceptInput();
	protected:
		void selectItem(bool playSoundOnItemSelect);
		void cancelSelection(bool playSoundOnItemSelect);
		void increaseCursor();
		void decreaseCursor();
	private:
		FK_Window* menuWindow;
		core::dimension2d<u32> screenSize;
		IrrlichtDevice *device;
		std::vector<std::string> menuItems;
		s32 menuIndex;
		bool selectionFlag;
		FK_SoundManager* soundManager;
		FK_Options* gameOptions;
		gui::IGUIFont* font;
		u32 now;
		u32 then;
		u32 inputDelayMS;
		f32 scaleFactorX;
		f32 scaleFactorY;
	};
}

#endif