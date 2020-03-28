#ifndef FK_TUTORIAL_CLASS
#define FK_TUTORIAL_CLASS
#include<irrlicht.h>
#include<string>
#include<vector>

using namespace irr;

namespace fk_engine{
	class FK_Tutorial{
	private:
		enum FK_TutorialPageNumbersAlignment : s32{
			TPN_left = -1,
			TPN_center = 0,
			TPN_right = 1,
		};
		const std::string BackgroundKey = "#BACKGROUND";
		const std::string PagesKey = "#PAGES";
		const std::string OverlayKey = "#OVERLAY";
		const std::string PageNumberKey = "#SHOWPAGENUMBERS";
		const std::string PageNumberLeftKey = "left";
		const std::string PageNumberCenterKey = "center";
		const std::string PageNumberRightKey = "right";
	public:
		FK_Tutorial();
		FK_Tutorial(video::IVideoDriver* new_driver, std::string mediaDirectory, core::dimension2d<u32> viewport, gui::IGUIFont *new_font);
		void draw();
		void goToBeginning();
		void goToEnd();
		void goToNextPage();
		void goToPreviousPage();
		bool isFirstPage();
		bool isLastPage();
	private:
		void loadTexturesFromFile();
		void drawPageNumber();
		std::string resourcesPath;
		video::ITexture* tutorialBackgroundTexture;
		video::ITexture* tutorialOverlayTexture;
		std::vector<video::ITexture*> tutorialPagesTextures;
		s32 numberOfPages;
		s32 currentPageIndex;
		video::IVideoDriver* driver;
		core::dimension2d<u32> drawingViewport;
		gui::IGUIFont *font;
		bool showPageNumbers;
		FK_TutorialPageNumbersAlignment pageNumbersAlignment;
	};
}

#endif