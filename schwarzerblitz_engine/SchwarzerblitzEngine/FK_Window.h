#ifndef FK_WINDOW_CLASS
#define FK_WINDOW_CLASS

#include <irrlicht.h>
#include <string>

using namespace irr;

namespace fk_engine{
	class FK_Window{
	public:
		FK_Window(IrrlichtDevice *new_device);
		FK_Window(IrrlichtDevice *new_device, s32 x, s32 y, s32 width, s32 height,
			std::string new_skinName, std::string new_path);
		FK_Window(IrrlichtDevice *new_device, core::rect<s32> newFrame,
			std::string new_skinName, std::string new_path);
		FK_Window(IrrlichtDevice *new_device, core::vector2d<s32> newPosition, s32 width, s32 height,
			std::string new_skinName, std::string new_path);
		virtual void update(u32 delta_t_ms);
		void draw(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void drawSelector(s32 x, s32 y, s32 width, s32 height, s32 opacity, f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		void setPosition(core::vector2d<s32> new_position);
		bool isBackgroundVisible();
		bool isOpening();
		bool isClosing();
		void forceOpen();
		void open(u32 delta_t_ms);
		void close(u32 delta_t_ms);
		bool isVisible();
		void setVisibility(bool isVisible);
		void setBackgroundVisibility(bool isVisible);
		void setAlpha(s32 alpha);
		bool isOpen();
		bool isClosed();
		bool isAnimating();
		bool isActive();
		void setActive(bool activity);
		s32 getWidth();
		s32 getHeight();
		core::vector2d<s32> getCenterPosition();
		core::vector2d<s32> getTopLeftCornerPosition();
		const u32 SKIN_TILESIZE = 32;
		const core::rect<s32> SKIN_BORDER_TOPLEFT = core::rect<s32>(0, 0, SKIN_TILESIZE, SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_TOPRIGHT = core::rect<s32>(SKIN_TILESIZE, 0, 2 * SKIN_TILESIZE, SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_DOWNRIGHT = core::rect<s32>(SKIN_TILESIZE, SKIN_TILESIZE, 
			2 * SKIN_TILESIZE, 2 * SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_DOWNLEFT = core::rect<s32>(0, SKIN_TILESIZE, SKIN_TILESIZE, 2*SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_TOPSTRAIGHT = core::rect<s32>(SKIN_TILESIZE / 2, 0, SKIN_TILESIZE * 3 / 2, SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_DOWNSTRAIGHT = core::rect<s32>(SKIN_TILESIZE/2, SKIN_TILESIZE, SKIN_TILESIZE * 3 / 2, 2 * SKIN_TILESIZE);
		const core::rect<s32> SKIN_BORDER_LEFTSTRAIGHT = core::rect<s32>(0, SKIN_TILESIZE / 2, SKIN_TILESIZE, SKIN_TILESIZE * 3 / 2);
		const core::rect<s32> SKIN_BORDER_RIGHTSTRAIGHT = core::rect<s32>(SKIN_TILESIZE, SKIN_TILESIZE / 2, 2*SKIN_TILESIZE, SKIN_TILESIZE * 3 / 2);
		const core::rect<s32> SKIN_TILECORE = core::rect<s32>(2 * SKIN_TILESIZE, 0, 4 * SKIN_TILESIZE, 2 * SKIN_TILESIZE);
		const core::rect<s32> SKIN_SELECTOR = core::rect<s32>(0, 2 * SKIN_TILESIZE, 3 * SKIN_TILESIZE, 3 * SKIN_TILESIZE);
	protected:
		void setSkin(std::string new_skinName, std::string new_path);
		void drawBorders(f32 scaleX, f32 scaleY);
		void drawTile(f32 scaleX, f32 scaleY);
		core::rect<s32> getClippingRectangle(f32 scaleX = 1.0f, f32 scaleY = 1.0f);
		std::string resourcesPath;
	private:
		IrrlichtDevice *device;
		video::IVideoDriver *driver;
		gui::IGUIFont *font;
		core::rect<s32> windowFrame;
		core::rect<s32> currentFrame;
		bool stretched;
		bool visibilityFlag;
		bool openFlag;
		bool activeFlag;
		bool backgroundVisible;
		video::ITexture* skin;
		std::string skinName;
		std::string skinPath;
		u32 closingTimeCounterMs;
		u32 openingTimeCounterMs;
		u32 actionTimeCounterMs;
		s32 alpha;
		video::SColor windowColor;
	};
}

#endif