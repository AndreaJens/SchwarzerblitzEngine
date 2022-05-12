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

#include"FK_Tutorial.h"
#include"ExternalAddons.h"
#include<fstream>

using namespace irr;

namespace fk_engine{
	FK_Tutorial::FK_Tutorial(){
		numberOfPages = 0;
		currentPageIndex = -1;
		driver = NULL;
		drawingViewport = core::dimension2d<u32>(0, 0);
		tutorialBackgroundTexture = NULL;
		tutorialOverlayTexture = NULL;
		showPageNumbers = false;
		pageNumbersAlignment = FK_TutorialPageNumbersAlignment::TPN_left;
	}
	
	FK_Tutorial::FK_Tutorial(video::IVideoDriver* new_driver, std::string mediaDirectory, core::dimension2d<u32> viewport, 
		gui::IGUIFont *new_font) :
		FK_Tutorial(){
		driver = new_driver;
		resourcesPath = mediaDirectory;
		drawingViewport = viewport;
		font = new_font;
		loadTexturesFromFile();
	}
	void FK_Tutorial::draw(){
		// create color array for all the components
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// draw background
		if (tutorialBackgroundTexture != NULL){
			core::dimension2d<u32> texSize = tutorialBackgroundTexture->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			//driver->draw2DImage(tutorialBackgroundTexture, core::rect<s32>(0, 0, drawingViewport.Width, drawingViewport.Height), 
			//	sourceRect,
			//	0, vertexColors, true);
			irr::core::vector2df scale((f32)((f32)drawingViewport.Width / (f32)texSize.Width), 
				(f32)((f32)drawingViewport.Height / (f32)texSize.Height));
			fk_addons::draw2DImage(driver, tutorialBackgroundTexture, sourceRect, core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, drawingViewport.Width, drawingViewport.Height));
		}
		// draw current page
		if (currentPageIndex >= 0){
			core::dimension2d<u32> texSize = tutorialPagesTextures[currentPageIndex]->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			irr::core::vector2df scale((f32)((f32)drawingViewport.Width / (f32)texSize.Width),
				(f32)((f32)drawingViewport.Height / (f32)texSize.Height));
			fk_addons::draw2DImage(driver, tutorialPagesTextures[currentPageIndex], sourceRect, core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, drawingViewport.Width, drawingViewport.Height));
		}
		// draw overlay
		if (tutorialOverlayTexture != NULL){
			core::dimension2d<u32> texSize = tutorialOverlayTexture->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			irr::core::vector2df scale((f32)((f32)drawingViewport.Width / (f32)texSize.Width),
				(f32)((f32)drawingViewport.Height / (f32)texSize.Height));
			fk_addons::draw2DImage(driver, tutorialOverlayTexture, sourceRect, core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, drawingViewport.Width, drawingViewport.Height));
		}
		// draw page numbers
		if (showPageNumbers){
			drawPageNumber();
		}
		//driver->setMaterial(materialOld);
	}
	void FK_Tutorial::drawPageNumber(){
		s32 x11, x21, x12, x22, x13, x23, y1, y2;
		u32 generalCharacterWidth = (u32)std::ceil(1.1f * (f32)(font->getDimension(L"W").Width));
		u32 generalCharacterHeight = (u32)std::ceil(1.1f * (f32)(font->getDimension(L"y").Height));
		u32 numbersWidth = font->getDimension(L"55").Width;
		u32 currentPageNumWidth = font->getDimension((std::to_wstring(currentPageIndex + 1)).c_str()).Width;
		u32 currentPageNumHeight = font->getDimension((std::to_wstring(currentPageIndex + 1)).c_str()).Height;
		u32 pageNumWidth = font->getDimension((std::to_wstring(numberOfPages)).c_str()).Width;
		u32 pageNumHeight = font->getDimension((std::to_wstring(numberOfPages)).c_str()).Height;
		u32 separatorWidth = font->getDimension(L"/").Width;
		u32 separatorHeight = font->getDimension(L"/").Height;
		y1 = drawingViewport.Height - 2 * generalCharacterHeight;
		y2 = y1 + generalCharacterHeight;
		if (pageNumbersAlignment == FK_TutorialPageNumbersAlignment::TPN_left){
			x11 = generalCharacterWidth;
			x21 = x11 + numbersWidth;
			x12 = x21;
			x22 = x12 + separatorWidth;
			x13 = x22;
			x23 = x13 + numbersWidth;
		}
		else if (pageNumbersAlignment == FK_TutorialPageNumbersAlignment::TPN_right){
			x11 = drawingViewport.Width - (2 * numbersWidth + separatorWidth) - generalCharacterWidth;
			x21 = x11 + numbersWidth;
			x12 = x21;
			x22 = x12 + separatorWidth;
			x13 = x22;
			x23 = x13 + numbersWidth;
		}
		else if (pageNumbersAlignment == FK_TutorialPageNumbersAlignment::TPN_center){
			x11 = (drawingViewport.Width - (2 * numbersWidth + separatorWidth)) / 2;
			x21 = x11 + numbersWidth;
			x12 = x21;
			x22 = x12 + separatorWidth;
			x13 = x22;
			x23 = x13 + numbersWidth;
		}
		fk_addons::drawBorderedText(font, (std::to_wstring(currentPageIndex + 1)).c_str(), core::rect<s32>(x11, y1, x21, y2),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true);
		fk_addons::drawBorderedText(font, L"/", core::rect<s32>(x12, y1, x22, y2), video::SColor(255, 255, 255, 255), 
			video::SColor(128, 0, 0, 0), true);
		fk_addons::drawBorderedText(font, (std::to_wstring(numberOfPages)).c_str(), core::rect<s32>(x13, y1, x23, y2),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true);
	}
	void FK_Tutorial::goToBeginning(){
		currentPageIndex = 0;
	}
	void FK_Tutorial::goToEnd(){
		currentPageIndex = numberOfPages - 1;
	}
	void FK_Tutorial::goToNextPage(){
		currentPageIndex += 1;
		if (currentPageIndex > numberOfPages){
			goToEnd();
		}
	}
	void FK_Tutorial::goToPreviousPage(){
		currentPageIndex -= 1;
		if (currentPageIndex < 0){
			goToBeginning();
		}
	}
	bool FK_Tutorial::isFirstPage(){
		return currentPageIndex == 0;
	}
	bool FK_Tutorial::isLastPage(){
		return currentPageIndex == numberOfPages - 1;
	}
	void FK_Tutorial::loadTexturesFromFile(){
		std::string filename = resourcesPath + "tutorial_config.ini";
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			return;
		}
		std::string temp;
		bool pages_flag = false;
		while (inputFile >> temp){
			if (temp == FK_Tutorial::BackgroundKey){
				inputFile >> temp;
				temp = resourcesPath + temp;
				tutorialBackgroundTexture = driver->getTexture(temp.c_str());
				pages_flag = false;
			}
			if (temp == FK_Tutorial::OverlayKey){
				inputFile >> temp;
				temp = resourcesPath + temp;
				tutorialOverlayTexture = driver->getTexture(temp.c_str());
				pages_flag = false;
			}
			if (temp == FK_Tutorial::PagesKey){
				pages_flag = true;
				continue;
			}
			if (temp == FK_Tutorial::PageNumberKey){
				showPageNumbers = true;
				inputFile >> temp;
				if (temp == FK_Tutorial::PageNumberLeftKey){
					pageNumbersAlignment = FK_TutorialPageNumbersAlignment::TPN_left;
				}
				else if (temp == FK_Tutorial::PageNumberRightKey){
					pageNumbersAlignment = FK_TutorialPageNumbersAlignment::TPN_right;
				}
				else if (temp == FK_Tutorial::PageNumberCenterKey){
					pageNumbersAlignment = FK_TutorialPageNumbersAlignment::TPN_center;
				}
				continue;
			}
			if (pages_flag){
				temp = resourcesPath + temp;
				video::ITexture* tempTex = driver->getTexture(temp.c_str());
				tutorialPagesTextures.push_back(tempTex);
			}
		}
		numberOfPages = tutorialPagesTextures.size();
	}
}