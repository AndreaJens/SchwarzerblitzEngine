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