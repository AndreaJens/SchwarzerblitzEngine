#pragma once

#include <irrlicht.h>
#include <string>
#include <Windows.h>

using namespace irr;

namespace fk_addons{

	// check if directory exists
	BOOL directoryExists(LPCTSTR absolutePath);

	// check if file exists
	BOOL fileExists(LPCTSTR szPath);

	//draw bordered text
	void drawBorderedText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
		video::SColor textColor = video::SColor(255, 255, 255, 255),
		video::SColor borderColor = video::SColor(255, 0, 0, 0),
		bool horizontalCentering = false, bool verticalCentering = false,
		core::rect<s32> *clipRect = 0);

	// draw image with rotation
	void draw2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0,0,0,0));

	// draw batch 2D image (no need to expose children functions)
	void drawBatch2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	void drawRotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	void drawStraight2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	void drawUnrotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale = irr::core::vector2df(1.0f, 1.0f),
		bool useAlphaChannel = true,
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		irr::core::rect<irr::s32> viewport = irr::core::rect<irr::s32>(0, 0, 0, 0));

	// draw tiled image
	void draw2DTiledImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32>& sourceRect, 
		irr::core::position2d<irr::s32> offsetPosition = irr::core::position2d<irr::s32>(0, 0),
		irr::core::vector2d<irr::f32> scalingFactor = core::vector2d<irr::f32>(1.0, 1.0));

	//Convert a texture to an image. Remember to drop the returned pointer
	video::IImage* TextureImage(video::IVideoDriver* driver, video::ITexture* texture);

	//Convert an image to a texture. Remember to drop the returned pointer
	video::ITexture* ImageTexture(video::IVideoDriver* driver, video::IImage* image,
		core::stringc name = core::stringc(""));

	//Convert string into wstring, replacing special characters for non-ASCII
	std::wstring convertNameToNonASCIIWstring(std::string nameToConvert);

	// replace substring 
	bool replaceSubstring(std::string& stringToActOn, std::string substringToFind, std::string substringToReplace);

	// handle game breaking exceptions
	void handle_eptr(std::exception_ptr eptr);

}