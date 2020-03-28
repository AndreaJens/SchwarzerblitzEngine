#include"FK_SceneGallery.h"
#include"ExternalAddons.h"
#include"FK_Reward.h"

namespace fk_engine {

	FK_SceneGallery::FK_SceneGallery() : FK_SceneExtra() {
		extraSceneType = FK_Scene::FK_SceneExtraType::ExtraGallery;
		initialize();
	}

	void FK_SceneGallery::initialize() {
		configOptions = FK_SceneGallery::FK_GalleryConfigOptions();
		currentPageIndex = 0;
		currentPictureIndex = 0;
		currentFrameIndex = 0;
		pictureIsSelected = false;
		backToMenu = false;
		lastInputTimePlayer2 = 0;
		lastInputTimePlayer1 = 0;
	}

	// read configuration file
	void FK_SceneGallery::readConfigurationFile() {
		sceneResourcesPath = mediaPath + "scenes\\gallery\\";
		std::ifstream configFile(sceneResourcesPath + FK_SceneGallery::ConfigFileName.c_str());
		if (!configFile) {
			return;
		}
		sceneName = fk_addons::convertNameToNonASCIIWstring(configOptions.galleryName);
	}

	// load all pictures for the gallery
	void FK_SceneGallery::loadGalleryPictures() {
		std::string path = sceneResourcesPath + FK_SceneGallery::PicturesFileName;
		std::ifstream unlockablePicturesFile(path.c_str());
		if (!unlockablePicturesFile) {
			return;
		}

		FK_RewardUnlockStruct unlockingParameters;
		unlockingParameters.characterStats = characterStatisticsStructure;
		unlockingParameters.arcadeWins = numberOfArcadeBeatenPerDifficultyLevel;
		unlockingParameters.isTutorialCleared = tutorialCleared;
		unlockingParameters.storyEpisodesCleared = storyEpisodesCleared;
		unlockingParameters.challengesCleared = tutorialChallengesCleared;
		unlockingParameters.unlockedCharacters = unlockedCharacters;

		std::string pictureFolderBeginTag = "#folder";
		std::string pictureEnclosureBeginTag = "#new_picture";
		std::string pictureEnclosureEndTag = "#new_picture_end";
		std::string pictureCaptionTag = "#caption";
		std::string pictureLockedCaptionTag = "#locked_caption";
		std::string pictureFrameTag = "#frame";
		std::string temp;
		std::string tempFolder = mediaPath;
		availablePictures.clear();
		galleryPictures.clear();
		FK_GalleryPicture tempPicture;
		while (unlockablePicturesFile) {
			unlockablePicturesFile >> temp;
			if (temp == pictureFolderBeginTag) {
				unlockablePicturesFile >> temp;
				tempFolder = mediaPath + temp;
			}
			else if (temp == pictureFrameTag) {
				unlockablePicturesFile >> temp;
				tempPicture.frames.push_back(tempFolder + temp);
				tempPicture.numberOfFrames += 1;
			}
			else if (temp == pictureCaptionTag) {
				temp = std::string();
				while (unlockablePicturesFile) {
					while (temp.empty() || temp == " " || temp == "\t") {
						std::getline(unlockablePicturesFile, temp);
					}
					break;
				}
				tempPicture.caption = fk_addons::convertNameToNonASCIIWstring(temp);
			}
			else if (temp == pictureLockedCaptionTag) {
				temp = std::string();
				while (unlockablePicturesFile) {
					while (temp.empty() || temp == " " || temp == "\t") {
						std::getline(unlockablePicturesFile, temp);
					}
					break;
				}
				tempPicture.lockedCaption = fk_addons::convertNameToNonASCIIWstring(temp);
			}
			else if (temp == pictureEnclosureEndTag) {
				if (tempPicture.numberOfFrames > 0) {
					picturesSet[tempPicture.uniqueId] = tempPicture;
				}
			}
			else if (temp == pictureEnclosureBeginTag) {
				tempPicture = FK_GalleryPicture();
				FK_Reward reward;
				bool success = reward.readFromFile(unlockablePicturesFile);
				if (success) {
					tempPicture.uniqueId = reward.rewardKey;
					if (availablePictures.count(reward.rewardKey) == 0 ||
						!availablePictures[reward.rewardKey]) {
						bool rewardUnlocked = FK_SceneWithInput::unlockAllFlag || reward.rewardIsUnlocked(unlockingParameters);
						if (availablePictures.count(reward.rewardKey) == 0) {
							galleryPictures.push_back(reward.rewardKey);
							availablePictures[reward.rewardKey] = rewardUnlocked;
						}
						else {
							availablePictures[reward.rewardKey] |= rewardUnlocked;
						}
					}
				}
			}
		}
	}

	// fill page collection
	void FK_SceneGallery::setupGalleryPages() {
		s32 tempPageIndex = -1;
		u32 currentPageIndex = 0;
		u32 size = galleryPictures.size();
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		for(u32 i = 0; i < size; ++i){
			if ((i % pageSize) == 0) {
				galleryPages.push_back(std::vector<std::string>());
				currentPageIndex = tempPageIndex;
				tempPageIndex += 1;
			}
			std::string key = galleryPictures[i];
			galleryPages[tempPageIndex].push_back(key);
		}
	}

	void FK_SceneGallery::setup(IrrlichtDevice *newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions) {
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		setupIrrlichtDevice();
		FK_SceneWithInput::readSaveFile();
		readConfigurationFile();
		loadGalleryPictures();
		setupGalleryPages();
		setupJoypad();
		setupInputMapper();
		setupSoundManager();
		setupBGM();
		setupGraphics();
		setupSystemButtonIcons();
	}

	void FK_SceneGallery::setupSoundManager() {
		soundPath = commonResourcesPath + "sound_effects\\";
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}
	void FK_SceneGallery::setupBGM() {
		systemBGMPath = mediaPath + "scenes\\bgm\\";
		bgmName = configOptions.bgmName;
		bgmVolume = configOptions.bgmVolume;
		bgmPitch = configOptions.bgmPitch;
		std::string filePath = systemBGMPath + bgmName;
		if (!bgm.openFromFile(filePath.c_str()))
			return;
		bgm.setLoop(true);
		bgm.setVolume(bgmVolume * gameOptions->getMusicVolume());
		bgm.setPitch(bgmPitch);
	}

	// setup input for players
	void FK_SceneGallery::setupInputForPlayers()
	{
		FK_SceneWithInput::setupInputForPlayers();
		setupSystemButtonIcons();
	}

	bool FK_SceneGallery::isRunning() {
		return !backToMenu;
	}

	void FK_SceneGallery::update() {
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		if (cycleId == 0) {
			cycleId = 1;
			delta_t_ms = 0;
			bgm.play();
		}
		else {
			if (pictureIsSelected) {
				updatePictureInput();
			}
			else {
				updateInput();
			}
		}
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));
		FK_SceneWithInput::executePreRenderingRoutine();
		drawAll();
		if (hasToSetupJoypad()) {
			drawJoypadSetup(now);
		}
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}

	void FK_SceneGallery::dispose(){
		clearTextureCacheForCurrentPage();
		galleryPages.clear();
		FK_SceneWithInput::dispose();
	}

	void FK_SceneGallery::setupGraphics() {
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		// fonts
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		mediumFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		captionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		// textures
		// arrows
		// arrows
		arrowLeft_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowL.png").c_str());
		arrowRight_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowR.png").c_str());
		// check if the right texture is loaded
		backgroundTexture = driver->getTexture((sceneResourcesPath + configOptions.backgroundTexture).c_str());
		if (configOptions.drawOverlay) {
			overlayTexture = driver->getTexture((sceneResourcesPath + configOptions.overlayTexture).c_str());
		}else{
			overlayTexture = NULL;
		}
		lockedPictureTexture = driver->getTexture((sceneResourcesPath + configOptions.unavailablePicturePlaceholder).c_str());
		loadCurrentPageTextures();
		loadSystemIcons();
	}

	void FK_SceneGallery::drawAll() {
		drawBackground();
		drawSelectionGrid();
		drawArrows();
		if (configOptions.drawOverlay) {
			drawOverlay();
		}
		if (configOptions.drawPageNumber) {
			drawPageNumber();
		}
		if (configOptions.drawGalleryName) {
			drawTitle();
		}
		drawCaption();
		if (pictureIsSelected) {
			drawSelectedPicture();
			if (configOptions.drawFrameNumber) {
				drawFrameNumber();
			}
		}
	}

	void FK_SceneGallery::drawSelectionGrid() {
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		if (pageSize == 0) {
			return;
		}
		u32 pageTexSize = pageTextures.size();
		s32 x0 = configOptions.selectionGridDimension.UpperLeftCorner.X;
		s32 y0 = configOptions.selectionGridDimension.UpperLeftCorner.Y;
		s32 totalPageSizeX = (s32)configOptions.selectionGridRowsAndColumns.X * (s32)configOptions.selectionIconSize.Width;
		s32 totalPageSizeY = (s32)configOptions.selectionGridRowsAndColumns.Y * (s32)configOptions.selectionIconSize.Height;
		s32 intervalX = 0;
		s32 intervalY = 0;
		f32 targetAspectRatio = (f32)configOptions.selectionIconSize.Width / (f32)configOptions.selectionIconSize.Height;
		if (configOptions.selectionGridRowsAndColumns.X > 1) {
			intervalX = (configOptions.selectionGridDimension.LowerRightCorner.X - totalPageSizeX) / (configOptions.selectionGridRowsAndColumns.X - 1);
		}
		if (configOptions.selectionGridRowsAndColumns.Y > 1) {
			intervalY = (configOptions.selectionGridDimension.LowerRightCorner.Y - totalPageSizeY) / (configOptions.selectionGridRowsAndColumns.Y - 1);
		}
		u32 selectedIndex = currentPictureIndex % pageSize;
		for (u32 i = 0; i < pageTexSize; ++i) {
			u32 pictureIndex = 
				pageSize * currentPageIndex + i;
			std::string key = galleryPages[currentPageIndex][i];
			s32 colIndex = i % configOptions.selectionGridRowsAndColumns.X;
			s32 rowIndex = std::floor((s32)i / configOptions.selectionGridRowsAndColumns.X);
			s32 x = x0 + (intervalX + (s32)configOptions.selectionIconSize.Width) * colIndex;
			s32 y = y0 + (intervalY + (s32)configOptions.selectionIconSize.Height) * rowIndex;
			x = (s32)std::floor((f32)x * scale_factorX);
			y = (s32)std::floor((f32)y * scale_factorY);
			s32 wdth = (s32)configOptions.selectionIconSize.Width * scale_factorX;
			s32 hgt = (s32)configOptions.selectionIconSize.Height * scale_factorY;
			video::SColor color = video::SColor(255, 255, 255, 255);
			if (pageTextures[i] != NULL) {
				video::ITexture* tex =
					availablePictures[key] ? pageTextures[i] : lockedPictureTexture;
				s32 sourceWidth = tex->getSize().Width;
				s32 sourceHeight = tex->getSize().Height;
				f32 aspectRatio = (f32)sourceWidth / (f32)sourceHeight;
				if ((aspectRatio - targetAspectRatio) / targetAspectRatio < -0.1) {
					sourceHeight = (s32)std::floor((f32)sourceWidth / targetAspectRatio);
				}
				else if ((aspectRatio - targetAspectRatio) / targetAspectRatio > 0.1) {
					sourceWidth = (s32)std::floor((f32)sourceHeight * targetAspectRatio);
				}
				f32 scaleX = scale_factorX * (f32)configOptions.selectionIconSize.Width / (f32)tex->getSize().Width;
				f32 scaleY = scale_factorY * (f32)configOptions.selectionIconSize.Height / (f32)tex->getSize().Height;
				fk_addons::draw2DImage(driver, tex,
					core::rect<s32>(0, 0, sourceWidth, sourceHeight),
					core::vector2d<s32>(x, y), core::vector2d<s32>(0, 0), 0, core::vector2df(scaleX, scaleY),
					false, color , core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				std::wstring eventId = std::to_wstring(pictureIndex + 1);
				core::dimension2d<u32> textSize = captionFont->getDimension(eventId.c_str());
				s32 textX = x + 4;
				fk_addons::drawBorderedText(captionFont, eventId, core::rect<s32>(textX, y, textX + textSize.Width, y + textSize.Height),
					color);
			}
			if (i == selectedIndex) {
				f32 magnificationPeriodMs = 1000.0;
				f32 phase = 2 * core::PI * ((f32)now / magnificationPeriodMs);
				phase = fmod(phase, 2 * core::PI);
				f32 magnificationAmplitude = 24;
				s32 addon = magnificationAmplitude / 2 + (s32)std::floor(magnificationAmplitude * sin(phase));
				color = video::SColor(80 + addon, 255, 200, 0);
				driver->draw2DRectangle(color, core::rect<s32>(x, y, x + wdth, y + hgt));
			}
		}
	}

	void FK_SceneGallery::drawCaption() {
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		//u32 captionIndex = currentPictureIndex % pageSize;
		//captionIndex = core::clamp(captionIndex, (u32)0, galleryPages[currentPageIndex].size());
		//std::string key = galleryPages[currentPageIndex][captionIndex];
		std::string key = galleryPictures[currentPictureIndex];
		std::wstring captionText = picturesSet[key].caption;
		if (!availablePictures[key]) {
			captionText = picturesSet[key].lockedCaption;
		}
		if (captionText.empty()) {
			return;
		}
		// draw caption text
		s32 fixedOffsetY = 320;
		s32 fixedOffsetX = 16 * scale_factorX;
		/* get width and height of the string*/
		std::wstring itemString = captionText;
		s32 sentenceWidth = captionFont->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = captionFont->getDimension(itemString.c_str()).Height;
		std::wstring sentenceToDraw = itemString;
		sentenceWidth = captionFont->getDimension(sentenceToDraw.c_str()).Width;
		s32 sentenceY = fixedOffsetY; // distance from the top of the screen
									  // draw player 1 name near the lifebar
		core::rect<s32> destinationRect = core::rect<s32>(0,
			sentenceY * scale_factorY,
			screenSize.Width,
			sentenceHeight + sentenceY * scale_factorY);
		fk_addons::drawBorderedText(captionFont, sentenceToDraw, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0),
			true);
	}

	// draw arrows
	void FK_SceneGallery::drawArrows() {
		u32 arrowFrequency = 500;
		s32 x0 = configOptions.selectionGridDimension.UpperLeftCorner.X;
		s32 y0 = configOptions.selectionGridDimension.UpperLeftCorner.Y;
		s32 totalPageSizeY = (s32)configOptions.selectionGridRowsAndColumns.Y * (s32)configOptions.selectionIconSize.Height;
		f32 phase = 2 * core::PI * (f32)(now % arrowFrequency) / arrowFrequency;
		s32 addonX = floor(10 * asin(cos(phase)) / core::PI);
		u32 arrowSize = 16;
		s32 baseArrowY = y0 + (totalPageSizeY - arrowSize) / 2;
		s32 baseArrowX = 20;
		if (configOptions.selectionGridRowsAndColumns.Y > 1) {
			baseArrowY += (configOptions.selectionGridRowsAndColumns.Y - 1) *
				(configOptions.selectionGridDimension.LowerRightCorner.Y - totalPageSizeY) / 
				(configOptions.selectionGridRowsAndColumns.Y - 1) / 2;
		}
		if (galleryPages.size() > 1/* && currentPageIndex > 0*/) {
			s32 x1 = (baseArrowX + addonX) * scale_factorX - arrowSize * scale_factorX / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y1 = baseArrowY * scale_factorY;
			s32 y2 = y1 + arrowSize * scale_factorY;

			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowLeft_tex->getSize().Width, arrowLeft_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowLeft_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
		if (galleryPages.size() > 1 /* && currentPageIndex < galleryPages.size() - 1*/) {
			s32 x1 = screenSize.Width - ((baseArrowX + addonX) * scale_factorX) - arrowSize * scale_factorX / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y1 = baseArrowY * scale_factorY;
			s32 y2 = y1 + arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowRight_tex->getSize().Width, arrowRight_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowRight_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
		if (galleryPages.size() > 1) {
			drawSystemButtons();
		}
	}

	// draw system buttons
	void FK_SceneGallery::drawSystemButtons() {
		s32 x0 = configOptions.selectionGridDimension.UpperLeftCorner.X;
		s32 y0 = configOptions.selectionGridDimension.UpperLeftCorner.Y;
		s32 totalPageSizeY = (s32)configOptions.selectionGridRowsAndColumns.Y * (s32)configOptions.selectionIconSize.Height;
		u32 arrowSize = 16;
		s32 baseArrowY = y0 + (totalPageSizeY - arrowSize) / 2;
		s32 baseArrowX = 20;
		gui::IGUIFont* hintfont = device->getGUIEnvironment()->getFont(
			fk_constants::FK_WindowFontIdentifier.c_str());
		core::dimension2du textSize = hintfont->getDimension(L"A");
		u32 iconScreenSize = textSize.Height;
		s32 baseWidth = (s32)iconScreenSize;
		s32 baseHeight = (s32)iconScreenSize;
		s32 iconWidth = baseWidth;
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		video::ITexture* tempTexture;

		s32 x1 = (s32)std::floor(baseArrowX * scale_factorX);
		s32 y1 = (s32)std::floor((baseArrowY + 2 * arrowSize) * scale_factorY);

		if (playerUsesJoypad) {
			tempTexture = joypadButtonsTexturesMap[
				(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::ScrollLeft_Button]];
		}
		else {
			tempTexture = keyboardButtonTexture;
		}

		if (!tempTexture) {
			return;
		}
		// draw different icon and text, depending on the keyboard/joypad notation
		// start by drawing the modifier/change costume button
		core::dimension2d<u32> iconSize = tempTexture->getSize();
		iconWidth = baseWidth;
		if (playerUsesJoypad) {
			fk_addons::draw2DImage(device->getVideoDriver(), tempTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y1), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			std::wstring itemToDraw = keyboardKeyLayout[FK_Input_Buttons::ScrollLeft_Button];
			core::dimension2du letterSize = hintfont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = baseWidth;
			while (iconWidth < letterSize.Width) {
				iconWidth += baseIconSize;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), tempTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y1), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				x1,
				y1,
				x1 + iconWidth,
				y1 + baseIconSize);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		// draw scroll right button
		x1 = screenSize.Width - (baseArrowX * scale_factorX) - arrowSize * scale_factorX / 2;

		if (playerUsesJoypad) {
			tempTexture = joypadButtonsTexturesMap[
				(FK_JoypadInput)buttonInputLayout[FK_Input_Buttons::ScrollRight_Button]];
		}
		else {
			tempTexture = keyboardButtonTexture;
		}

		if (!tempTexture) {
			return;
		}
		// draw different icon and text, depending on the keyboard/joypad notation
		// start by drawing the modifier/change costume button
		iconSize = tempTexture->getSize();
		iconWidth = baseWidth;
		if (playerUsesJoypad) {
			fk_addons::draw2DImage(device->getVideoDriver(), tempTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y1), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			std::wstring itemToDraw = keyboardKeyLayout[FK_Input_Buttons::ScrollRight_Button];
			core::dimension2du letterSize = hintfont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = baseWidth;
			while (iconWidth < letterSize.Width) {
				iconWidth += baseIconSize;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), tempTexture, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x1, y1), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				x1,
				y1,
				x1 + iconWidth,
				y1 + baseIconSize);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}
	}

	// draw background
	void FK_SceneGallery::drawSelectedPicture() {
		driver->draw2DRectangle(video::SColor(196, 0, 0, 0), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		video::ITexture* tex = frameTextures[currentFrameIndex];
		core::dimension2d<u32> texSize = tex->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		f32 scaleX = (f32)screenSize.Width / (f32)tex->getSize().Width;
		f32 scaleY = (f32)screenSize.Height / (f32)tex->getSize().Height;
		if (scaleX < scaleY || scaleX > scaleY) {
			scaleY = std::min(scaleX, scaleY);
			scaleX = scaleY;
		};
		x = (screenSize.Width - texSize.Width * scaleX) / 2;
		y = (screenSize.Height - texSize.Height * scaleY) / 2;
		fk_addons::draw2DImage(driver, tex,
			sourceRect,
			core::vector2d<s32>(x, y), core::vector2d<s32>(0, 0), 0, core::vector2df(scaleX, scaleY),
			false, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
	}

	// draw background
	void FK_SceneGallery::drawOverlay() {
		core::dimension2d<u32> overlaySize = overlayTexture->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, overlaySize.Width, overlaySize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(overlayTexture, destinationRect, sourceRect, 0, vertexColors_background, true);
	}

	// draw background
	void FK_SceneGallery::drawBackground() {
		core::dimension2d<u32> backgroundSize = backgroundTexture->getSize();
		// load elements for the stage miniature
		s32 dest_width = screenSize.Width;
		s32 dest_height = screenSize.Height;
		s32 x = 0;
		s32 y = 0;
		core::rect<s32> destinationRect = core::rect<s32>(0, 0,
			dest_width,
			dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, backgroundSize.Width, backgroundSize.Height);
		// create color array for loading screen
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors_background[4] = {
			color,
			color,
			color,
			color
		};
		driver->draw2DImage(backgroundTexture, destinationRect, sourceRect, 0, vertexColors_background);
	}

	void FK_SceneGallery::drawPageNumber() {
		// draw page number
		std::wstring stageNumber = std::to_wstring(currentPageIndex + 1);
		std::wstring stageSize = std::to_wstring(galleryPages.size());
		std::wstring separator = L"/";
		u32 numberWidth = mediumFont->getDimension(stageNumber.c_str()).Width;
		u32 numberHeight = mediumFont->getDimension(stageNumber.c_str()).Height;
		u32 separationWidth = mediumFont->getDimension(L"00").Width;
		s32 fx, fy;
		fx = (s32)std::floor((f32)configOptions.pageNumberPosition.X * scale_factorX);
		fy = (s32)std::floor((f32)configOptions.pageNumberPosition.Y * scale_factorY);
		fk_addons::drawBorderedText(mediumFont, stageNumber.c_str(),
			core::rect<s32>(fx,
				fy,
				fx + numberWidth,
				fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		fx += separationWidth;
		numberWidth = mediumFont->getDimension(separator.c_str()).Width;
		fk_addons::drawBorderedText(mediumFont, separator.c_str(),
			core::rect<s32>(fx,
				fy,
				fx + numberWidth,
				fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		fx += separationWidth / 2;
		numberWidth = mediumFont->getDimension(stageSize.c_str()).Width;
		fk_addons::drawBorderedText(mediumFont, stageSize.c_str(),
			core::rect<s32>(fx,
				fy,
				fx + numberWidth,
				fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
	}

	void FK_SceneGallery::drawFrameNumber() {
		// draw page number
		std::wstring frameNumber = std::to_wstring(currentFrameIndex + 1);
		std::wstring frameSize = std::to_wstring(frameTextures.size());
		std::wstring separator = L"/";
		u32 numberWidth = mediumFont->getDimension(frameNumber.c_str()).Width;
		u32 numberHeight = mediumFont->getDimension(frameNumber.c_str()).Height;
		u32 separationWidth = mediumFont->getDimension(L"00").Width;
		s32 fx, fy;
		fx = (s32)std::floor((f32)configOptions.frameNumberPosition.X * scale_factorX);
		fy = (s32)std::floor((f32)configOptions.frameNumberPosition.Y * scale_factorY);
		fk_addons::drawBorderedText(mediumFont, frameNumber.c_str(),
			core::rect<s32>(fx,
				fy,
				fx + numberWidth,
				fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		fx += separationWidth;
		numberWidth = mediumFont->getDimension(separator.c_str()).Width;
		fk_addons::drawBorderedText(mediumFont, separator.c_str(),
			core::rect<s32>(fx,
				fy,
				fx + numberWidth,
				fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
		fx += separationWidth / 2;
		numberWidth = mediumFont->getDimension(frameSize.c_str()).Width;
		fk_addons::drawBorderedText(mediumFont, frameSize.c_str(),
			core::rect<s32>(fx,
				fy,
				fx + numberWidth,
				fy + numberHeight),
			video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), true, false);
	}

	void FK_SceneGallery::drawTitle() {
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		s32 sentenceWidth = font->getDimension(sceneName.c_str()).Width;
		s32 sentenceHeight = font->getDimension(sceneName.c_str()).Height;
		x -= sentenceWidth / 2;
		y -= sentenceHeight / 2;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + scale_factorY);
		fk_addons::drawBorderedText(font, sceneName, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	void FK_SceneGallery::loadSystemIcons() {
		// keyboard button texture
		std::string resourcePath = mediaPath + "movelist\\";
		keyboardButtonTexture = device->getVideoDriver()->getTexture((resourcePath + "keyboard\\empty2.png").c_str());
		// joypad buttons textures
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_A_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Abutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_X_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Xbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Y_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Ybutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_B_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\Bbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L1_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L1button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L2_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L2button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_L3_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\L3button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R1_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R1button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R2_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R2button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_R3_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\R3button.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Select_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\backbutton.png").c_str());
		joypadButtonsTexturesMap[FK_JoypadInput::JoypadInput_Start_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "joypad\\startbutton.png").c_str());
	}

	// setup icons for keyboard and joypad input
	void FK_SceneGallery::setupSystemButtonIcons() {
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		if (joystickInfo.size() > 0) {
			playerUsesJoypad = true;
			mapId = 1;
			currentJoypadName = std::string(joystickInfo[0].Name.c_str());
		}
		else {
			playerUsesJoypad = false;
			mapId = 0;
		}
		setIconsBasedOnCurrentInputMap(buttonInputLayout, keyboardKeyLayout,
			mapId, playerUsesJoypad, currentJoypadName
		);
	}

	void FK_SceneGallery::loadNextPage(bool fromInput) {
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		if (galleryPages.size() > 1) {
			clearTextureCacheForCurrentPage();
			currentPageIndex += 1;
			currentPageIndex %= galleryPages.size();
			if (fromInput) {
				currentPictureIndex = pageSize * currentPageIndex;
				soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
			}
			loadCurrentPageTextures();
		}
	}
	void FK_SceneGallery::loadPreviousPage(bool fromInput) {
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		if (galleryPages.size() > 1) {
			clearTextureCacheForCurrentPage();
			if (currentPageIndex == 0) {
				currentPageIndex = galleryPages.size();
			}
			currentPageIndex -= 1;
			if (fromInput) {
				currentPictureIndex = pageSize * currentPageIndex;
				soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
				inputReceiver->reset();
			}
			loadCurrentPageTextures();
		}
	}

	void FK_SceneGallery::loadCurrentPageTextures() {
		u32 size = galleryPages[currentPageIndex].size();
		for (u32 i = 0; i < size; ++i) {
			std::string previewImageKey = galleryPages[currentPageIndex][i];
			if (picturesSet[previewImageKey].numberOfFrames > 0) {
				std::string previewPath = picturesSet[previewImageKey].frames[0];
				video::ITexture* tempTex = driver->getTexture(previewPath.c_str());
				if (!tempTex) {
					tempTex = NULL;
				}
				pageTextures.push_back(tempTex);
			}
		}
	}

	void FK_SceneGallery::loadCurrentFrameTextures() {
		std::string key = galleryPictures[currentPictureIndex];
		u32 size = picturesSet[key].frames.size();
		for (u32 i = 0; i < size; ++i) {
			video::ITexture* tempTex = driver->getTexture(picturesSet[key].frames[i].c_str());
			if (!tempTex) {
				tempTex = NULL;
			}
			frameTextures.push_back(tempTex);
		}
	}

	void FK_SceneGallery::clearTextureCacheForCurrentPage() {
		u32 size = pageTextures.size();
		for (u32 i = 0; i < size; ++i) {
			device->getVideoDriver()->removeTexture(pageTextures[i]);
		}
		pageTextures.clear();
	}

	void FK_SceneGallery::clearTextureCacheForCurrentPicture() {
		u32 size = frameTextures.size();
		// avoid removing the first frame, since it is used as a preview
		for (u32 i = 1; i < size; ++i) {
			device->getVideoDriver()->removeTexture(frameTextures[i]);
		}
		frameTextures.clear();
	}

	void FK_SceneGallery::selectItem() {
		inputReceiver->reset();
		std::string key = galleryPictures[currentPictureIndex];
		if (availablePictures[key]) {
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			pictureIsSelected = true;
			currentFrameIndex = 0;
			loadCurrentFrameTextures();
		}
		else {
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		}
	}

	void FK_SceneGallery::cancelSelection() {
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		pictureIsSelected = false;
		clearTextureCacheForCurrentPicture();
		inputReceiver->reset();
		return;
	}

	void FK_SceneGallery::goBackToMainMenu() {
		setNextScene(getPreviousScene());
		inputReceiver->reset();
		backToMenu = true;
		soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
		Sleep(300);
		return;
	}

	void FK_SceneGallery::increaseIndex() {
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		bool goToNextPage = currentPictureIndex == galleryPictures.size() - 1;
		currentPictureIndex += 1;
		currentPictureIndex %= galleryPictures.size();
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		u32 pageIndex = std::floor(currentPictureIndex / pageSize);
		if (goToNextPage || pageIndex > currentPageIndex) {
			loadNextPage();
		}
		else if (pageIndex < currentPageIndex) {
			loadPreviousPage();
		}
	}
	void FK_SceneGallery::decreaseIndex() {
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		bool goToPreviousPage = false;
		if (currentPictureIndex == 0) {
			currentPictureIndex = galleryPictures.size();
			goToPreviousPage = true;
		}
		currentPictureIndex -= 1;
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		u32 pageIndex = std::floor(currentPictureIndex / pageSize);
		if (!goToPreviousPage && pageIndex > currentPageIndex) {
			loadNextPage();
		}
		else if (goToPreviousPage || pageIndex < currentPageIndex) {
			loadPreviousPage();
		}
	}
	void FK_SceneGallery::increaseRow() {
		if (configOptions.selectionGridRowsAndColumns.Y <= 1) {
			return;
		}
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		s32 normalizedIndex = currentPictureIndex % (s32)pageSize;
		s32 colIndex = normalizedIndex % configOptions.selectionGridRowsAndColumns.X;
		s32 rowIndex = std::floor((s32)normalizedIndex / configOptions.selectionGridRowsAndColumns.X);
		if (rowIndex < configOptions.selectionGridRowsAndColumns.Y - 1) {
			currentPictureIndex += (s32)configOptions.selectionGridRowsAndColumns.X;
			currentPictureIndex = core::clamp(currentPictureIndex, 0, (s32)galleryPictures.size() - 1);
		}
		else if (rowIndex == configOptions.selectionGridRowsAndColumns.Y - 1) {
			currentPictureIndex -= (s32)(configOptions.selectionGridRowsAndColumns.Y - 1) * 
				(s32)configOptions.selectionGridRowsAndColumns.X;
			currentPictureIndex = core::clamp(currentPictureIndex, 0, (s32)galleryPictures.size() - 1);
		}
	}
	void FK_SceneGallery::decreaseRow() {
		if (configOptions.selectionGridRowsAndColumns.Y <= 1) {
			return;
		}
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		u32 pageSize = configOptions.selectionGridRowsAndColumns.X * configOptions.selectionGridRowsAndColumns.Y;
		s32 normalizedIndex = currentPictureIndex % (s32)pageSize;
		s32 rowIndex = std::floor((s32)normalizedIndex / configOptions.selectionGridRowsAndColumns.X);
		if (rowIndex > 0) {
			currentPictureIndex -= (s32)configOptions.selectionGridRowsAndColumns.X;
			currentPictureIndex = core::clamp(currentPictureIndex, 0, (s32)galleryPictures.size() - 1);
		}
		else if (rowIndex == 0) {
			currentPictureIndex += (s32)(configOptions.selectionGridRowsAndColumns.Y - 1) *
				(s32)configOptions.selectionGridRowsAndColumns.X;
			currentPictureIndex = core::clamp(currentPictureIndex, 0, (s32)galleryPictures.size() - 1);
		}
	}

	void FK_SceneGallery::increaseFrameIndex() {
		inputReceiver->reset();
		std::string key = galleryPictures[currentPictureIndex];
		if (picturesSet[key].numberOfFrames > 1) {
			soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
			currentFrameIndex += 1;
			currentFrameIndex %= picturesSet[key].numberOfFrames;
		}
	}

	void FK_SceneGallery::updatePictureInput() {
		u32 inputThreshold = 200;
		// check input for player 1
		if (now - lastInputTimePlayer1 > inputThreshold) {
			if (joystickInfo.size() > 0) {
				player1input->update(now, inputReceiver->JoypadStatus(0), false);
			}
			else {
				player1input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer1 = player1input->getLastDirection();
			u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button) {
				lastInputTimePlayer1 = now;
				cancelSelection();
				return;
			}
			else {
				if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button) {
					lastInputTimePlayer1 = now;
					increaseFrameIndex();
					return;
				}
			}
		}
		// check input for player 2
		if (now - lastInputTimePlayer2 > inputThreshold) {
			if (joystickInfo.size() > 1) {
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			else {
				player2input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer2 = player2input->getLastDirection();
			u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button) {
				lastInputTimePlayer2 = now;
				cancelSelection();
				return;
			}
			else {
				if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button) {
					increaseFrameIndex();
					lastInputTimePlayer2 = now;
					return;
				}
			}
		}
	}

	void FK_SceneGallery::updateInput() {
		u32 inputThreshold = 200;
		// check input for player 1
		if (now - lastInputTimePlayer1 > inputThreshold) {
			if (joystickInfo.size() > 0) {
				player1input->update(now, inputReceiver->JoypadStatus(0), false);
			}
			else {
				player1input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer1 = player1input->getLastDirection();
			u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button) {
				lastInputTimePlayer1 = now;
				goBackToMainMenu();
				return;
			}
			else {
				if (directionPlayer1 == FK_Input_Buttons::Right_Direction) {
					lastInputTimePlayer1 = now;
					increaseIndex();
					return;
				}
				else if (directionPlayer1 == FK_Input_Buttons::Left_Direction) {
					lastInputTimePlayer1 = now;
					decreaseIndex();
					return;
				}
				else if (directionPlayer1 == FK_Input_Buttons::Down_Direction) {
					lastInputTimePlayer1 = now;
					increaseRow();
					return;
				}
				else if (lastButtonPlayer1 == FK_Input_Buttons::ScrollRight_Button) {
					lastInputTimePlayer1 = now;
					loadNextPage(true);
					return;
				}
				else if (directionPlayer1 == FK_Input_Buttons::Up_Direction) {
					lastInputTimePlayer1 = now;
					decreaseRow();
					return;
				}
				else if (lastButtonPlayer1 == FK_Input_Buttons::ScrollLeft_Button) {
					lastInputTimePlayer1 = now;
					loadPreviousPage(true);
					return;
				}
				else {
					// select item
					if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button) {
						lastInputTimePlayer1 = now;
						selectItem();
						return;
					}
				}
			}
		}
		// check input for player 2
		if (now - lastInputTimePlayer2 > inputThreshold) {
			if (joystickInfo.size() > 1) {
				player2input->update(now, inputReceiver->JoypadStatus(1), false);
			}
			else {
				player2input->update(now, inputReceiver->KeyboardStatus(), false);
			}
			u32 directionPlayer2 = player2input->getLastDirection();
			u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button) {
				lastInputTimePlayer2 = now;
				goBackToMainMenu();
				return;
			}
			else {
				if (directionPlayer2 == FK_Input_Buttons::Right_Direction) {
					lastInputTimePlayer2 = now;
					increaseIndex();
					return;
				}
				else if (directionPlayer2 == FK_Input_Buttons::Left_Direction) {
					lastInputTimePlayer2 = now;
					decreaseIndex();
					return;
				}
				else if (directionPlayer2 == FK_Input_Buttons::Down_Direction) {
					lastInputTimePlayer2 = now;
					increaseRow();
					return;
				}
				else if (lastButtonPlayer2 == FK_Input_Buttons::ScrollRight_Button) {
					lastInputTimePlayer2 = now;
					loadNextPage(true);
					return;
				}
				else if (directionPlayer2 == FK_Input_Buttons::Up_Direction) {
					lastInputTimePlayer2 = now;
					decreaseRow();
					return;
				}
				else if (lastButtonPlayer2 == FK_Input_Buttons::ScrollLeft_Button) {
					lastInputTimePlayer2 = now;
					loadPreviousPage(true);
					return;
				}
				else {
					// select item
					if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button) {
						lastInputTimePlayer2 = now;
						selectItem();
						return;
					}
				}
			}
		}
	}
}