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

#include<irrlicht.h>
#include"ExternalAddons.h"
#include<iostream>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <tchar.h>
#include <vector>
#include <regex>

using namespace irr;
using namespace video;
using namespace core;
using namespace scene;

namespace fk_addons{

	// constants for character replacement
	namespace fk_string_tags{
		const std::vector<std::pair<std::wstring, std::wstring> > nonASCIIReplacements = {
			//diaeresis
			std::pair<std::wstring, std::wstring>(L":ee:", L"ë"),
			std::pair<std::wstring, std::wstring>(L":ae:", L"ä"),
			std::pair<std::wstring, std::wstring>(L":ue:", L"ü"),
			std::pair<std::wstring, std::wstring>(L":oe:", L"ö"),
			std::pair<std::wstring, std::wstring>(L":ie:", L"ï"),
			std::pair<std::wstring, std::wstring>(L":EE:", L"Ë"),
			std::pair<std::wstring, std::wstring>(L":AE:", L"Ä"),
			std::pair<std::wstring, std::wstring>(L":UE:", L"Ü"),
			std::pair<std::wstring, std::wstring>(L":OE:", L"Ö"),
			std::pair<std::wstring, std::wstring>(L":IE:", L"Ï"),

			// acute/grave stress
			std::pair<std::wstring, std::wstring>(L":'A:", L"À"),
			std::pair<std::wstring, std::wstring>(L":A':", L"Á"),
			std::pair<std::wstring, std::wstring>(L":'O:", L"Ò"),
			std::pair<std::wstring, std::wstring>(L":O':", L"Ó"),
			std::pair<std::wstring, std::wstring>(L":'U:", L"Ù"),
			std::pair<std::wstring, std::wstring>(L":U':", L"Ú"),
			std::pair<std::wstring, std::wstring>(L":'I:", L"Ì"),
			std::pair<std::wstring, std::wstring>(L":I':", L"Í"),
			std::pair<std::wstring, std::wstring>(L":'E:", L"È"),
			std::pair<std::wstring, std::wstring>(L":E':", L"É"),
			std::pair<std::wstring, std::wstring>(L":a':", L"á"),
			std::pair<std::wstring, std::wstring>(L":'a:", L"à"),
			std::pair<std::wstring, std::wstring>(L":'o:", L"ò"),
			std::pair<std::wstring, std::wstring>(L":o':", L"ó"),
			std::pair<std::wstring, std::wstring>(L":'u:", L"ù"),
			std::pair<std::wstring, std::wstring>(L":u':", L"ú"),
			std::pair<std::wstring, std::wstring>(L":'i:", L"ì"),
			std::pair<std::wstring, std::wstring>(L":i':", L"í"),
			std::pair<std::wstring, std::wstring>(L":'e:", L"è"),
			std::pair<std::wstring, std::wstring>(L":e':", L"é"),

			//c cediglia
			std::pair<std::wstring, std::wstring>(L":cc:", L"ç"),
			std::pair<std::wstring, std::wstring>(L":CC:", L"Ç"),

			//tilde
			std::pair<std::wstring, std::wstring>(L":~n:", L"ñ"),
			std::pair<std::wstring, std::wstring>(L":~N:", L"Ñ"),
			std::pair<std::wstring, std::wstring>(L":~a:", L"ã"),
			std::pair<std::wstring, std::wstring>(L":~A:", L"Ã"),
			std::pair<std::wstring, std::wstring>(L":~o:", L"õ"),
			std::pair<std::wstring, std::wstring>(L":~O:", L"Õ"),

			//eszet
			std::pair<std::wstring, std::wstring>(L":ss:", L"ß"),

			//circumflex stress
			std::pair<std::wstring, std::wstring>(L":^A:", L"Â"),
			std::pair<std::wstring, std::wstring>(L":^O:", L"Ô"),
			std::pair<std::wstring, std::wstring>(L":^U:", L"Û"),
			std::pair<std::wstring, std::wstring>(L":^I:", L"Î"),
			std::pair<std::wstring, std::wstring>(L":^E:", L"Ê"),
			std::pair<std::wstring, std::wstring>(L":^a:", L"â"),
			std::pair<std::wstring, std::wstring>(L":^o:", L"ô"),
			std::pair<std::wstring, std::wstring>(L":^u:", L"û"),
			std::pair<std::wstring, std::wstring>(L":^i:", L"î"),
			std::pair<std::wstring, std::wstring>(L":^e:", L"ê"),

			//inverse marks
			std::pair<std::wstring, std::wstring>(L":!:", L"¡"),
			std::pair<std::wstring, std::wstring>(L":?:", L"¿"),
		};
	}

	// check if directory exists
	BOOL directoryExists(LPCTSTR absolutePath)
	{
		if (_taccess_s(absolutePath, 0) == 0)
		{
			struct _stat status;
			_tstat(absolutePath, &status);
			return (status.st_mode & S_IFDIR) != 0;
		}

		return FALSE;
	}

	//check if file exists
	BOOL fileExists(LPCTSTR szPath)
	{
		DWORD dwAttrib = GetFileAttributes(szPath);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	void drawBorderedText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
		video::SColor textColor, video::SColor borderColor, bool horizontalCentering, bool verticalCentering,
		core::rect<s32> *clipRect){
		s32 deltaX[8] = { 1, 0, 0, -1, 1, 1, -1, -1 };
		s32 deltaY[8] = { 0, -1, 1, 0, 1, -1, -1, 1 };
		video::SColor borderColorHalf(borderColor);
		borderColorHalf.setAlpha(borderColor.getAlpha() / 2);
		borderColor.setAlpha(borderColor.getAlpha() / 2);
		//draw black frame
		for (int i = 4; i < 8; ++i){
			core::vector2d<s32> frameOffset(deltaX[i], deltaY[i]);
			font->draw(text.c_str(),
				destinationRect + frameOffset,
				borderColor,
				horizontalCentering,
				verticalCentering,
				clipRect);
		}
		for (int i = 0; i < 4; ++i){
			core::vector2d<s32> frameOffset(deltaX[i], deltaY[i]);
			font->draw(text.c_str(),
				destinationRect + frameOffset,
				borderColor,
				horizontalCentering,
				verticalCentering,
				clipRect);
		}
		font->draw(text.c_str(),
			destinationRect,
			textColor,
			horizontalCentering,
			verticalCentering,
			clipRect);
	};

	void draw2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {
		if (rotation == 0){
			drawStraight2DImage(driver, texture,
				sourceRect, position,
				scale, useAlphaChannel, color,
				viewport);
		}
		else{
			drawRotated2DImage(driver, texture,
				sourceRect, position,
				rotationPoint, rotation,
				scale, useAlphaChannel, color,
				viewport);
		}
		//drawRotated2DImage(driver, texture,
		//	sourceRect, position,
		//	rotationPoint, rotation,
		//	scale, useAlphaChannel, color,
		//	viewport);
	};

	void drawRotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {

		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());

		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
		driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());

		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

		// Find horizontal and vertical axes after rotation
		irr::f32 c = cos(-rotation*irr::core::DEGTORAD);
		irr::f32 s = sin(-rotation*irr::core::DEGTORAD);
		irr::core::vector2df horizontalAxis(c, s);
		irr::core::vector2df verticalAxis(s, -c);

		// First, we'll find the offset of the center and then where the center would be after rotation
		irr::core::vector2df centerOffset(position.X + sourceRect.getWidth() / 2.0f*scale.X - rotationPoint.X, position.Y + sourceRect.getHeight() / 2.0f*scale.Y - rotationPoint.Y);
		irr::core::vector2df center = centerOffset.X*horizontalAxis - centerOffset.Y*verticalAxis;
		center.X += rotationPoint.X;
		center.Y += rotationPoint.Y;

		// Now find the corners based off the center
		irr::core::vector2df cornerOffset(sourceRect.getWidth()*scale.X / 2.0f, sourceRect.getHeight()*scale.Y / 2.0f);
		verticalAxis *= cornerOffset.Y;
		horizontalAxis *= cornerOffset.X;
		irr::core::vector2df corner[4];
		corner[0] = center + verticalAxis - horizontalAxis;
		corner[1] = center + verticalAxis + horizontalAxis;
		corner[2] = center - verticalAxis - horizontalAxis;
		corner[3] = center - verticalAxis + horizontalAxis;

		// Find the uv coordinates of the sourceRect
		irr::core::vector2df textureSize(texture->getSize().Width, texture->getSize().Height);
		irr::core::vector2df uvCorner[4];
		uvCorner[0] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.UpperLeftCorner.Y);
		uvCorner[1] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.UpperLeftCorner.Y);
		uvCorner[2] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.LowerRightCorner.Y);
		uvCorner[3] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.LowerRightCorner.Y);
		for (irr::s32 i = 0; i < 4; i++)
			uvCorner[i] /= textureSize;

		// Vertices for the image
		irr::video::S3DVertex vertices[4];
		irr::u16 indices[6] = { 0, 1, 2, 3, 2, 1 };

		if (viewport.getWidth() == 0){
			viewport = driver->getViewPort();
		}

		// Convert pixels to world coordinates
		irr::core::vector2df screenSize(viewport.getWidth(), viewport.getHeight());
		for (irr::s32 i = 0; i < 4; i++) {
			vertices[i].Pos = irr::core::vector3df(((corner[i].X / screenSize.X) - 0.5f)*2.0f, ((corner[i].Y / screenSize.Y) - 0.5f)*-2.0f, 1);
			vertices[i].TCoords = uvCorner[i];
			vertices[i].Color = color;
		}

		// Create the material
		// IMPORTANT: For irrlicht 1.8 and above you MUST ADD THIS LINE:
		//material.BlendOperation = irr::video::EBO_ADD;
		irr::video::SMaterial material;
		material.Lighting = false;
		material.ZWriteEnable = false;
		material.ZBuffer = false;
		material.BackfaceCulling = false;
		material.TextureLayer[0].Texture = texture;
		material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		material.BlendOperation = irr::video::EBO_ADD;
		if (useAlphaChannel)
			material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		else
			material.MaterialType = irr::video::EMT_SOLID;

		driver->setMaterial(material);
		driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);

		// Restore projection, world, and view matrices
		driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
		driver->setTransform(irr::video::ETS_WORLD, oldWorldMat);
	};

	void drawStraight2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {

		sourceRect.UpperLeftCorner.X += 1;
		sourceRect.UpperLeftCorner.Y += 1;
		// Store and clear the projection matrix
		irr::core::matrix4 oldProjMat = driver->getTransform(irr::video::ETS_PROJECTION);
		driver->setTransform(irr::video::ETS_PROJECTION, irr::core::matrix4());

		// Store and clear the view matrix
		irr::core::matrix4 oldViewMat = driver->getTransform(irr::video::ETS_VIEW);
		driver->setTransform(irr::video::ETS_VIEW, irr::core::matrix4());

		// Store and clear the world matrix
		irr::core::matrix4 oldWorldMat = driver->getTransform(irr::video::ETS_WORLD);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

		// Find horizontal and vertical axes after rotation
		f32 x0 = position.X;
		f32 y0 = position.Y;
		f32 x1 = x0 + texture->getSize().Width * scale.X;
		f32 y1 = y0 + texture->getSize().Height * scale.Y;
		irr::core::vector2df corner[4];
		corner[0] = irr::core::vector2df(x0, y0);
		corner[1] = irr::core::vector2df(x1, y0);
		corner[2] = irr::core::vector2df(x0, y1);
		corner[3] = irr::core::vector2df(x1, y1);

		// Find the uv coordinates of the sourceRect
		irr::core::vector2df textureSize(texture->getSize().Width, texture->getSize().Height);
		irr::core::vector2df uvCorner[4];
		f32 tx0, ty0, tx1, ty1;
		// X UV coordinates
		if (textureSize.X == sourceRect.LowerRightCorner.X) {
			tx1 = 1.f;
		}
		else{
			tx1 = sourceRect.LowerRightCorner.X / textureSize.X;
		}
		if (sourceRect.UpperLeftCorner.X == 0) {
			tx0 = 0.f;
		}
		else {
			tx0 = sourceRect.UpperLeftCorner.X / textureSize.X;
		}
		// Y UV coordinates
		if (textureSize.Y == sourceRect.LowerRightCorner.Y) {
			ty1 = 1.f;
		}
		else {
			ty1 = sourceRect.LowerRightCorner.Y / textureSize.Y;
		}
		if (sourceRect.UpperLeftCorner.Y == 0) {
			ty0 = 0.f;
		}
		else {
			ty0 = sourceRect.UpperLeftCorner.Y / textureSize.Y;
		}

		uvCorner[0] = irr::core::vector2df(tx0, ty0);
		uvCorner[1] = irr::core::vector2df(tx1, ty0);
		uvCorner[2] = irr::core::vector2df(tx0, ty1);
		uvCorner[3] = irr::core::vector2df(tx1, ty1);

		// Vertices for the image
		irr::video::S3DVertex vertices[4];
		irr::u16 indices[6] = { 0, 1, 2, 3, 2, 1 };

		if (viewport.getWidth() == 0) {
			viewport = driver->getViewPort();
		}

		// Convert pixels to world coordinates
		irr::core::vector2df screenSize(viewport.getWidth(), viewport.getHeight());
		for (irr::s32 i = 0; i < 4; i++) {
			vertices[i].Pos = irr::core::vector3df(((corner[i].X / screenSize.X) - 0.5f)*2.0f, ((corner[i].Y / screenSize.Y) - 0.5f)*-2.0f, 1);
			vertices[i].TCoords = uvCorner[i];
			vertices[i].Color = color;
		}

		// Create the material
		// IMPORTANT: For irrlicht 1.8 and above you MUST ADD THIS LINE:
		//material.BlendOperation = irr::video::EBO_ADD;
		irr::video::SMaterial material;
		material.Lighting = false;
		material.ZWriteEnable = false;
		material.ZBuffer = false;
		material.BackfaceCulling = false;
		material.TextureLayer[0].Texture = texture;
		material.BlendOperation = irr::video::EBO_ADD;
		if (useAlphaChannel)
			material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		else
			material.MaterialType = irr::video::EMT_SOLID;

		driver->setMaterial(material);
		driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);

		// Restore projection, world, and view matrices
		driver->setTransform(irr::video::ETS_PROJECTION, oldProjMat);
		driver->setTransform(irr::video::ETS_VIEW, oldViewMat);
		driver->setTransform(irr::video::ETS_WORLD, oldWorldMat);
	};

	void drawBatchRotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {
		
		// Find horizontal and vertical axes after rotation
		irr::f32 c = cos(-rotation*irr::core::DEGTORAD);
		irr::f32 s = sin(-rotation*irr::core::DEGTORAD);
		irr::core::vector2df horizontalAxis(c, s);
		irr::core::vector2df verticalAxis(s, -c);

		// First, we'll find the offset of the center and then where the center would be after rotation
		irr::core::vector2df centerOffset(position.X + sourceRect.getWidth() / 2.0f*scale.X - rotationPoint.X, position.Y + sourceRect.getHeight() / 2.0f*scale.Y - rotationPoint.Y);
		irr::core::vector2df center = centerOffset.X*horizontalAxis - centerOffset.Y*verticalAxis;
		center.X += rotationPoint.X;
		center.Y += rotationPoint.Y;

		// Now find the corners based off the center
		irr::core::vector2df cornerOffset(sourceRect.getWidth()*scale.X / 2.0f, sourceRect.getHeight()*scale.Y / 2.0f);
		verticalAxis *= cornerOffset.Y;
		horizontalAxis *= cornerOffset.X;
		irr::core::vector2df corner[4];
		corner[0] = center + verticalAxis - horizontalAxis;
		corner[1] = center + verticalAxis + horizontalAxis;
		corner[2] = center - verticalAxis - horizontalAxis;
		corner[3] = center - verticalAxis + horizontalAxis;

		// Find the uv coordinates of the sourceRect
		irr::core::vector2df textureSize(texture->getSize().Width, texture->getSize().Height);
		irr::core::vector2df uvCorner[4];
		uvCorner[0] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.UpperLeftCorner.Y);
		uvCorner[1] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.UpperLeftCorner.Y);
		uvCorner[2] = irr::core::vector2df(sourceRect.UpperLeftCorner.X, sourceRect.LowerRightCorner.Y);
		uvCorner[3] = irr::core::vector2df(sourceRect.LowerRightCorner.X, sourceRect.LowerRightCorner.Y);
		for (irr::s32 i = 0; i < 4; i++)
			uvCorner[i] /= textureSize;

		// Vertices for the image
		irr::video::S3DVertex vertices[4];
		irr::u16 indices[6] = { 0, 1, 2, 3, 2, 1 };

		if (viewport.getWidth() == 0) {
			viewport = driver->getViewPort();
		}

		// Convert pixels to world coordinates
		irr::core::vector2df screenSize(viewport.getWidth(), viewport.getHeight());
		for (irr::s32 i = 0; i < 4; i++) {
			vertices[i].Pos = irr::core::vector3df(((corner[i].X / screenSize.X) - 0.5f)*2.0f, ((corner[i].Y / screenSize.Y) - 0.5f)*-2.0f, 1);
			vertices[i].TCoords = uvCorner[i];
			vertices[i].Color = color;
		}

		// Create the material
		// IMPORTANT: For irrlicht 1.8 and above you MUST ADD THIS LINE:
		//material.BlendOperation = irr::video::EBO_ADD;
		irr::video::SMaterial material;
		material.Lighting = false;
		material.ZWriteEnable = false;
		material.ZBuffer = false;
		material.BackfaceCulling = false;
		material.TextureLayer[0].Texture = texture;
		material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		material.BlendOperation = irr::video::EBO_ADD;
		if (useAlphaChannel)
			material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		else
			material.MaterialType = irr::video::EMT_SOLID;

		driver->setMaterial(material);
		driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
	};

	void drawBatchStraight2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {

		sourceRect.UpperLeftCorner.X += 1;
		sourceRect.UpperLeftCorner.Y += 1;
		// Find horizontal and vertical axes after rotation
		f32 x0 = position.X;
		f32 y0 = position.Y;
		f32 x1 = x0 + texture->getSize().Width * scale.X;
		f32 y1 = y0 + texture->getSize().Height * scale.Y;
		irr::core::vector2df corner[4];
		corner[0] = irr::core::vector2df(x0, y0);
		corner[1] = irr::core::vector2df(x1, y0);
		corner[2] = irr::core::vector2df(x0, y1);
		corner[3] = irr::core::vector2df(x1, y1);

		// Find the uv coordinates of the sourceRect
		irr::core::vector2df textureSize(texture->getSize().Width, texture->getSize().Height);
		irr::core::vector2df uvCorner[4];
		f32 tx0, ty0, tx1, ty1;
		// X UV coordinates
		if (textureSize.X == sourceRect.LowerRightCorner.X) {
			tx1 = 1.f;
		}
		else {
			tx1 = sourceRect.LowerRightCorner.X / textureSize.X;
		}
		if (sourceRect.UpperLeftCorner.X == 0) {
			tx0 = 0.f;
		}
		else {
			tx0 = sourceRect.UpperLeftCorner.X / textureSize.X;
		}
		// Y UV coordinates
		if (textureSize.Y == sourceRect.LowerRightCorner.Y) {
			ty1 = 1.f;
		}
		else {
			ty1 = sourceRect.LowerRightCorner.Y / textureSize.Y;
		}
		if (sourceRect.UpperLeftCorner.Y == 0) {
			ty0 = 0.f;
		}
		else {
			ty0 = sourceRect.UpperLeftCorner.Y / textureSize.Y;
		}

		uvCorner[0] = irr::core::vector2df(tx0, ty0);
		uvCorner[1] = irr::core::vector2df(tx1, ty0);
		uvCorner[2] = irr::core::vector2df(tx0, ty1);
		uvCorner[3] = irr::core::vector2df(tx1, ty1);

		// Vertices for the image
		irr::video::S3DVertex vertices[4];
		irr::u16 indices[6] = { 0, 1, 2, 3, 2, 1 };

		if (viewport.getWidth() == 0) {
			viewport = driver->getViewPort();
		}

		// Convert pixels to world coordinates
		irr::core::vector2df screenSize(viewport.getWidth(), viewport.getHeight());
		for (irr::s32 i = 0; i < 4; i++) {
			vertices[i].Pos = irr::core::vector3df(((corner[i].X / screenSize.X) - 0.5f)*2.0f, ((corner[i].Y / screenSize.Y) - 0.5f)*-2.0f, 1);
			vertices[i].TCoords = uvCorner[i];
			vertices[i].Color = color;
		}

		// Create the material
		// IMPORTANT: For irrlicht 1.8 and above you MUST ADD THIS LINE:
		//material.BlendOperation = irr::video::EBO_ADD;
		irr::video::SMaterial material;
		material.Lighting = false;
		material.ZWriteEnable = false;
		material.ZBuffer = false;
		material.BackfaceCulling = false;
		material.TextureLayer[0].Texture = texture;
		material.BlendOperation = irr::video::EBO_ADD;
		if (useAlphaChannel)
			material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		else
			material.MaterialType = irr::video::EMT_SOLID;

		driver->setMaterial(material);
		driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
	};

	void drawBatch2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::position2d<irr::s32> rotationPoint, irr::f32 rotation,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {
		if (rotation == 0) {
			drawBatchStraight2DImage(driver, texture,
				sourceRect, position,
				scale, useAlphaChannel, color,
				viewport);
		}
		else {
			drawBatchRotated2DImage(driver, texture,
				sourceRect, position,
				rotationPoint, rotation,
				scale, useAlphaChannel, color,
				viewport);
		}
	}

	void drawUnrotated2DImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32> sourceRect, irr::core::position2d<irr::s32> position,
		irr::core::vector2df scale, bool useAlphaChannel, irr::video::SColor color,
		irr::core::rect<irr::s32> viewport) {
		video::SColor colors[4] = { color, color, color, color };
		driver->draw2DImage(texture,
			core::rect<s32>(position.X, position.Y,
			position.X + sourceRect.getWidth() *scale.X,
			position.Y + sourceRect.getHeight() *scale.Y),
			sourceRect,&viewport,
			colors,useAlphaChannel);
	};

	// draw tiled image (RPG Maker style parallax)
	void draw2DTiledImage(irr::video::IVideoDriver *driver, irr::video::ITexture* texture,
		irr::core::rect<irr::s32>& sourceRect, irr::core::position2d<irr::s32> offsetPosition, 
		irr::core::vector2d<irr::f32> scalingFactor){
		// first, calculate the number of tiles needed
		core::dimension2d<u32> textureSize = texture->getSize();
		core::dimension2d<u32> screenSize = driver->getScreenSize();
		core::dimension2d<f32> adjustedSize = core::dimension2d<f32>(floor(textureSize.Width * scalingFactor.X),
			floor(textureSize.Height * scalingFactor.Y));
		// the "+1" part is used to prevent missing tiles when the offset is in the negative direction!
		u32 numberOfTexturesHorizontal = ceil((f32)screenSize.Width / adjustedSize.Width) + 1;
		u32 numberOfTexturesVertical = ceil((f32)screenSize.Height / adjustedSize.Height) + 1;
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// start from -1 to size + 1 take into account the offset (i.e. two images more has to be drawn to cover for the 
		// black part
		for (s32 i = -1; i <= (s32)numberOfTexturesHorizontal; ++i){
			
			for (s32 j = -1; j <= (s32)numberOfTexturesVertical; ++j){
				s32 x0 = i * adjustedSize.Width + offsetPosition.X;
				s32 y0 = j * adjustedSize.Height + offsetPosition.Y;
				core::rect<irr::s32> destinationRect = core::rect<irr::s32>(
					x0,
					y0,
					x0 + static_cast<s32>(adjustedSize.Width),
					y0 + static_cast<s32>(adjustedSize.Height));
				driver->draw2DImage(texture, destinationRect, sourceRect, 
					0, vertexColors, true);
			}
		}
	}

	video::IImage* TextureImage(video::IVideoDriver* driver, video::ITexture* texture) {

		video::IImage* image = driver->createImageFromData(
			texture->getColorFormat(),
			texture->getSize(),
			texture->lock(),
			false  //copy mem
			);

		texture->unlock();
		return image;

	}

	video::ITexture* ImageTexture(video::IVideoDriver* driver, video::IImage* image, core::stringc name) {

		video::ITexture* texture = driver->addTexture(name.c_str(), image);
		texture->grab();
		return texture;

	}

	std::wstring convertNameToNonASCIIWstring(std::string nameToConvert){
		std::wstring temp(nameToConvert.begin(), nameToConvert.end());

		for (auto it = fk_string_tags::nonASCIIReplacements.begin(); it != fk_string_tags::nonASCIIReplacements.end(); ++it){
			const std::wstring key = (*it).first;
			const std::wstring replacement = (*it).second;

			std::string::size_type n = 0;
			while ((n = temp.find(key, n)) != std::wstring::npos)
			{
				temp.replace(n, key.size(), replacement);
				n += replacement.size();
			}
		}
		return temp;
	}

	// replace substring
	bool replaceSubstring(std::string & stringToActOn, std::string substringToFind, std::string substringToReplace)
	{
		std::string temp = std::regex_replace(stringToActOn, std::regex(substringToFind), substringToReplace);
		if (stringToActOn == temp) {
			return false;
		}else{
			stringToActOn = temp;
		}
		return true;
	}

	

	void handle_eptr(std::exception_ptr eptr) // passing by value is ok
	{
		try {
			if (eptr) {
				std::rethrow_exception(eptr);
			}
		}
		catch (const std::exception& e) {
			std::cout << "Caught exception \"" << e.what() << "\"\n";
		}
	}

}
