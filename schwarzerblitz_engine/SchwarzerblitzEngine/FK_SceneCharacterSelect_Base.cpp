#include"FK_SceneCharacterSelect_Base.h"
#include"ExternalAddons.h"
#include<irrlicht.h>
#include<iostream>

namespace fk_engine{

	FK_SceneCharacterSelect_Base::FK_SceneCharacterSelect_Base() : FK_SceneWithInput(){
		characterRaycastedPositionCalculatedFlag = false;
		isEmbeddedSceneOptionsActive = false;
		embeddedSceneOptions = NULL;
	};

	FK_SceneCharacterSelect_Base::~FK_SceneCharacterSelect_Base() {

	};
	// setup BGM
	void FK_SceneCharacterSelect_Base::setupBGM(){
		//std::string filePath = systemBGMPath + "decisions(mcleod-character_select_cut).ogg";
		std::string filePath = systemBGMPath + sceneResources.BGMName;
		if (!character_selection_bgm.openFromFile(filePath.c_str()))
			return;
		character_selection_bgm.setLoop(true);
		character_selection_bgm.setVolume(sceneResources.BGMVolume * gameOptions->getMusicVolume());
		character_selection_bgm.setPitch(sceneResources.BGMPitch);
	}

	// setup announcer
	void FK_SceneCharacterSelect_Base::setupAnnouncer(){
		announcer = new FK_Announcer(voiceClipsPath);
		announcer->addSoundFromDefaultPath("select_character", "select_character.wav");
		announcer->addSoundFromDefaultPath("select_arena", "select_arena.wav");
	}

	// setup sound manager
	void FK_SceneCharacterSelect_Base::setupSoundManager(){
		soundManager = new FK_SoundManager(soundPath);
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}

	//!setup camera
	void FK_SceneCharacterSelect_Base::setupCamera(){
		/* reset camera position*/
		float baseCameraDistance = sceneResources.cameraPosition.Z;
		float baseCameraYPosition = sceneResources.cameraPosition.Y;
		float cameraDistance = baseCameraDistance; //take into account the X position of the models!!!
		float cameraYPosition = baseCameraYPosition;
		camera = smgr->addCameraSceneNode();
		camera->setPosition(core::vector3df(sceneResources.cameraPosition.X, cameraYPosition, cameraDistance));
		camera->setTarget(core::vector3df(0, 0, 0));
		camera->setFOV(irr::core::HALF_PI / 4);
		camera->OnRegisterSceneNode();
	}


	/* setup textures */
	void FK_SceneCharacterSelect_Base::setupTextures(){
		std::string resourcePath = mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath;
		// background
		background = driver->getTexture((resourcePath + sceneResources.backgroundTexture).c_str());
		// overlay
		overlay = driver->getTexture((resourcePath + sceneResources.overlayTexture).c_str());
		// selector
		selector = driver->getTexture((resourcePath + sceneResources.characterSelectorTexture).c_str());
		// generic preview
		missingPreview = driver->getTexture((resourcePath + sceneResources.lockedCharacterIconTexture).c_str());
		// locked costume sign
		lockedCostumeSign = driver->getTexture((resourcePath + sceneResources.lockedCostumeMarkTexture).c_str());
		// no arcade ending sign
		noArcadeEndingSign = driver->getTexture((resourcePath + sceneResources.noArcadeEndingMarkTexture).c_str());
		// arcade complete logo
		completedArcadeModeLogoTexture = driver->getTexture((resourcePath + sceneResources.arcadeModeCompletedLogo).c_str());
		// costume dots
		currentCostumeDotTex = driver->getTexture((resourcePath +
			sceneResources.currentCostumeIconTexture).c_str());
		costumeDotTex = driver->getTexture((resourcePath +
			sceneResources.availableCostumeIconTexture).c_str());
		// character icons
		int size = characterPaths.size();
		if (characterIcons.size() == 0){
			for (int i = 0; i < size; i++){
				std::string fullPath = charactersDirectory +
					characterPaths[i];
				if (!isValidCharacterPath(fullPath) && isValidCharacterPath(characterPaths[i])) {
					fullPath = characterPaths[i];
				}
				video::ITexture* characterIcon = driver->getTexture((fullPath + "selectionPreview0.png").c_str());
				if (!characterIcon){
					continue;
				}
				characterIcons.push_back(characterIcon);
			};
			video::ITexture* characterIcon = driver->getTexture((charactersDirectory +
				dummyCharacterDirectory + "selectionPreview0.png").c_str());
			characterIcons.push_back(characterIcon);
		};
		// setup screen size multipliers
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()){
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)sceneResources.referenceScreenSize.Width;
		scale_factorY = screenSize.Height / (f32)sceneResources.referenceScreenSize.Height;

		// load system icons
		loadSystemIcons();
	};


	// set icon size and offset
	void FK_SceneCharacterSelect_Base::setIconSizeAndOffset(){
		u32 numberOfCharacters = characterPaths.size();
		// get the maximum screen size to be used for characters
		u32 maximumCharacterScreenWidth = sceneResources.characterSelectBoxPosition.LowerRightCorner.X;
		u32 maximumCharacterScreenHeight = sceneResources.characterSelectBoxPosition.LowerRightCorner.Y;
		// get maximum and minimum icon size
		f32 iconMaximumSizeWidth = sceneResources.maxIconSize.Width;
		f32 iconMinimumSizeWidth = sceneResources.minIconSize.Width;
		f32 iconMaximumSizeHeight = sceneResources.maxIconSize.Height;
		f32 iconMinimumSizeHeight = sceneResources.minIconSize.Height;
		// get maximum and minimum offset between icons
		f32 iconMaximumOffsetX = sceneResources.maxIconOffset.Width;
		f32 iconMaximumOffsetY = sceneResources.maxIconOffset.Height;
		f32 iconMinimumOffsetX = sceneResources.minIconOffset.Width;
		f32 iconMinimumOffsetY = sceneResources.minIconOffset.Height;
		// calculate the maximum number of characters that can be displayed
		maxRows = static_cast<u32>(floor((f32)(maximumCharacterScreenHeight + iconMaximumOffsetY) /
			(iconMaximumSizeHeight + iconMaximumOffsetY)));
		maxColumns = static_cast<u32>(floor((f32)(maximumCharacterScreenWidth + iconMaximumOffsetX) /
			(iconMaximumSizeWidth + iconMaximumOffsetX)));
		u32 maxCharacters = maxRows * maxColumns;
		iconSize = sceneResources.maxIconSize;
		iconOffset = sceneResources.maxIconOffset;
		// if the number of characters is smaller than the number of places, proceed as usual - else, shrink icons
		while (numberOfCharacters >= maxCharacters){
			iconOffset = iconOffset * 0.99;
			iconSize = iconSize * 0.99;
			// re-calculate the maximum number of characters that can be displayed
			maxRows = static_cast<u32>(floor((f32)(maximumCharacterScreenHeight + iconOffset.Height) /
				(iconSize.Height + iconMaximumOffsetY)));
			maxColumns = static_cast<u32>(floor((f32)(maximumCharacterScreenWidth + iconOffset.Width) /
				(iconSize.Width + iconOffset.Width)));
			maxCharacters = maxRows * maxColumns;
			if (iconSize.Width <= sceneResources.minIconSize.Width || iconOffset.Width <= sceneResources.minIconOffset.Width){
				iconSize = sceneResources.minIconSize;
				iconOffset = sceneResources.minIconOffset;
				break;
			}
			if (iconSize.Height <= sceneResources.minIconSize.Height || iconOffset.Height <= sceneResources.minIconOffset.Height) {
				iconSize = sceneResources.minIconSize;
				iconOffset = sceneResources.minIconOffset;
				break;
			}
		}
		maxOccupiedRows = (s32)floor((numberOfCharacters) / maxColumns);
		sceneResources.characterSelectBoxPosition.LowerRightCorner.X = 
			(f32)(maxRows * (iconSize.Width + iconOffset.Width)) - iconOffset.Width;
		sceneResources.characterSelectBoxPosition.LowerRightCorner.Y = 
			(f32)(maxRows * (iconSize.Height + iconOffset.Height)) - iconOffset.Height;
	}

	// get nearest character to select
	s32 FK_SceneCharacterSelect_Base::getNearestNeighbourIndex(s32 index, SelectionDirection direction){
		// get number of characters
		u32 numberOfCharacters = characterPaths.size();
		// get row index
		u32 rowIndex = floor(index / maxColumns);
		u32 old_index = index;
		// get number of characters in a row
		s32 charactersInTheRow = maxColumns;
		s32 charactersInTheLastRow = (s32)(numberOfCharacters + 1) - (s32)(maxColumns * (maxOccupiedRows));
		if (rowIndex == maxOccupiedRows){
			charactersInTheRow = charactersInTheLastRow;
		}
		switch (direction){
		case SelectionDirection::Right:
			index += 1;
			index = rowIndex * maxColumns + (index - rowIndex * maxColumns) % charactersInTheRow;
			break;
		case SelectionDirection::Left:
			index = index - rowIndex * maxColumns;
			index += charactersInTheRow - 1;
			index %= charactersInTheRow;
			index = rowIndex * maxColumns + index;
			break;
		case SelectionDirection::Up:
			if (rowIndex == 0){
				f32 currentPositionPercent = (f32)(index % maxColumns) / maxColumns;
				// this apparently useless condition is needed to avoid rounding mistakes (0.5 rounded high instead of low)
				if (charactersInTheLastRow % 2 == 0) {
					currentPositionPercent -= 0.01f;
				}
				u32 newIndex = (u32)std::round(currentPositionPercent * (f32)charactersInTheLastRow);
				index = (maxRows - 1) * maxColumns + newIndex;
				index = std::min((s32)characterPaths.size(), (s32)index);
			}
			else{
				if (rowIndex == maxOccupiedRows){
					f32 currentPositionPercent = (f32)((index - rowIndex * maxColumns) % charactersInTheRow) / charactersInTheRow;
					if (charactersInTheRow % 2 == 0) {
						currentPositionPercent -= 0.01f;
					}
					u32 newIndex = (u32)std::round(currentPositionPercent * (f32)maxColumns);
					index = (rowIndex - 1) * maxColumns + newIndex;
				}
				else{
					index -= maxColumns;
				}
			}
			break;
		case SelectionDirection::Down:
			if (rowIndex == maxOccupiedRows){
				f32 currentPositionPercent = (f32)((index - rowIndex * maxColumns) % charactersInTheLastRow) / charactersInTheLastRow;
				if (charactersInTheRow % 2 == 0) {
					currentPositionPercent -= 0.01f;
				}
				u32 newIndex = (u32)std::round(currentPositionPercent * (f32)maxColumns);
				index = newIndex;
			}
			else{
				if (rowIndex == maxOccupiedRows - 1){
					f32 currentPositionPercent = (f32)(index % maxColumns) / maxColumns;
					if (charactersInTheRow % 2 == 0) {
						currentPositionPercent -= 0.01f;
					}
					u32 newIndex = (u32)std::round(currentPositionPercent * (f32)charactersInTheLastRow);
					index = (rowIndex + 1) * maxColumns + newIndex;
					index = std::min((s32)characterPaths.size(), (s32)index);
				}
				else{
					index += maxColumns;
				}
			}
			break;
		default:
			break;
		}
		return index;
	}

	// get screen position for the nth character icon
	core::rect<s32> FK_SceneCharacterSelect_Base::getCharacterIconPosition(u32 icon_index){
		// get number of characters
		u32 numberOfCharacters = characterPaths.size();
		// declare variables for later use
		s32 x = 0;
		s32 y = 0;
		s32 offsetX = 0;
		s32 offsetY = 0;
		s32 dest_width = 0;
		s32 dest_height = 0;
		s32 extra_x = 0;
		s32 extra_y = 0;
		dest_width = (s32)(floor(iconSize.Width * scale_factorX));
		dest_height = (s32)(floor(iconSize.Height * scale_factorY));
		offsetX = (s32)(floor(iconOffset.Width * scale_factorX));
		offsetY = (s32)(floor(iconOffset.Height * scale_factorY));
		u32 rowIndex = floor(icon_index / maxColumns);
		s32 charactersInTheRow = maxColumns;
		if (rowIndex == maxOccupiedRows){
			charactersInTheRow = (s32)(numberOfCharacters + 1) - (s32)(maxColumns * (maxOccupiedRows));
			s32 additionalCharactersInTheRow = maxColumns - charactersInTheRow;
			if (additionalCharactersInTheRow > 0){
				//f32 additionalSpace = additionalCharactersInTheRow * (iconSize + iconOffset) - iconOffset;
				s32 lastRowWidthExcess = additionalCharactersInTheRow * (dest_width + offsetX);
				f32 additionalSpace = lastRowWidthExcess / 2;
				extra_x = std::round(additionalSpace);
			}
		}
		s32 extraOffsetX = (s32)(floor((f32)rowIndex * sceneResources.iconRowColumnOffset.Width * scale_factorX));
		s32 extraOffsetY = (s32)(floor((f32)(icon_index % maxColumns) * sceneResources.iconRowColumnOffset.Height * scale_factorY));
		extra_x += extraOffsetX + icon_index % maxColumns * (dest_width + offsetX);
		extra_y += extraOffsetY + floor(icon_index / maxColumns) * (dest_height + offsetY);
		// set the origin point
		s32 characterScreenWidth = ((maxColumns)* (iconSize.Width + iconOffset.Width) - iconOffset.Width);
		s32 characterScreenHeight = ((maxOccupiedRows + 1) * (iconSize.Height + iconOffset.Height) - iconOffset.Height);
		if (sceneResources.centerCharacterBox == FK_CenterCharacterBox::FullCentering ||
			sceneResources.centerCharacterBox == FK_CenterCharacterBox::CenterOnlyX) {
			s32 originX = ((s32)sceneResources.referenceScreenSize.Width - (s32)characterScreenWidth) / 2;
			x = (originX * scale_factorX + extra_x);
		}else{
			s32 originX = sceneResources.characterSelectBoxPosition.UpperLeftCorner.X;
			x = (originX * scale_factorX + extra_x);
		}
		if (sceneResources.centerCharacterBox == FK_CenterCharacterBox::FullCentering ||
			sceneResources.centerCharacterBox == FK_CenterCharacterBox::CenterOnlyY) {
			s32 originY = ((s32)sceneResources.referenceScreenSize.Height - (s32)characterScreenHeight) / 2;
			y = (originY * scale_factorY + extra_y);
		}
		else {
			s32 originY = sceneResources.characterSelectBoxPosition.UpperLeftCorner.Y;
			y = (originY * scale_factorY + extra_y);
		}
		return core::rect<s32>(x, y,
			x + dest_width,
			y + dest_height);
	}

	bool FK_SceneCharacterSelect_Base::isValidCharacterPath(std::string path)
	{
		std::ifstream testFile(path + "character.txt");
		if (!testFile) {
			return false;
		}
		return true;
	}

	bool FK_SceneCharacterSelect_Base::isValidStagePath(std::string path)
	{
		std::ifstream testFile(path + "config.txt");
		if (!testFile) {
			return false;
		}
		return true;
	}

	// draw screen
	void FK_SceneCharacterSelect_Base::drawCharacterScreen(){
		int size = characterIcons.size();
		// all character icons must have the same width and height
		core::dimension2d<u32> texSize = characterIcons[0]->getSize();
		s32 width = texSize.Width;
		s32 height = texSize.Height;
		for (int i = 0; i < size; i++){
			core::rect<s32> destinationRect = getCharacterIconPosition(i);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, width, height);
			bool availabilityFlag = isCharacterAvailable(i);
			driver->draw2DImage(availabilityFlag ? characterIcons[i] : missingPreview, destinationRect, sourceRect);
		}
	};

	/* draw screen overlay */
	void FK_SceneCharacterSelect_Base::drawScreenOverlay(){
		if (sceneResources.hasOverlay) {
			core::dimension2d<u32> texSize = overlay->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			video::SColor const color = video::SColor(255, 255, 255, 255);
			irr::core::vector2df scale((f32)((f32)screenSize.Width / (f32)texSize.Width),
				(f32)((f32)(screenSize.Height) / (f32)texSize.Height));
			fk_addons::draw2DImage(driver, overlay,
				sourceRect,
				core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
	}

	bool FK_SceneCharacterSelect_Base::isCharacterAvailable(u32 character_index){
		bool availableFlag = false;
		if (std::find(availableCharacters.begin(), availableCharacters.end(), character_index) != availableCharacters.end()){
			availableFlag = true;
		}
		return availableFlag;
	}

	// check if character can be selected
	bool FK_SceneCharacterSelect_Base::isCharacterSelectable(FK_Character* character){
		if (character == NULL){
			return false;
		}
		else if (character->getAnimatedMesh() == NULL){
			return false;
		}
		return !(character->getAnimatedMesh()->getMaterial(0).Lighting);
	}

	/* draw background*/
	void FK_SceneCharacterSelect_Base::drawBackground(){
		if (sceneResources.hasBackground) {
			core::dimension2d<u32> texSize = background->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			switch (sceneResources.backgroundMode) {
			case FK_SceneBackgroundMode::Tiled:
			{
				offset.X += sceneResources.backgroundMovementVelocity.X * scale_factorX * (f32)delta_t_ms / 1000.0;
				offset.Y += sceneResources.backgroundMovementVelocity.Y * scale_factorY * (f32)delta_t_ms / 1000.0;
				offset.X = fmod(offset.X, (texSize.Width * scale_factorX));
				offset.Y = fmod(offset.Y, (texSize.Height * scale_factorY));
				core::vector2d<s32> intOffset((s32)ceil(offset.X), (s32)ceil(offset.Y));
				fk_addons::draw2DTiledImage(driver, background, sourceRect, intOffset,
					core::vector2d<f32>(scale_factorX, scale_factorY));
				break;
			}
			case FK_SceneBackgroundMode::Stretched:
			{
				f32 scaleX = (f32)screenSize.Width / (f32)texSize.Width;
				f32 scaleY = (f32)screenSize.Height / (f32)texSize.Height;
				fk_addons::draw2DImage(driver, background, sourceRect, core::vector2d<s32>(0, 0), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>(scaleX, scaleY));
				break;
			}
			case FK_SceneBackgroundMode::KeepRatioWidthScale:
			{
				f32 scaleX = (f32)screenSize.Width / (f32)texSize.Width;
				f32 scaleY = scaleX;
				s32 offsetY = std::round(((s32)screenSize.Height - texSize.Height * scaleY) / 2);
				fk_addons::draw2DImage(driver, background, sourceRect, core::vector2d<s32>(0, offsetY), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>(scaleX, scaleY));
				break;
			}
			case FK_SceneBackgroundMode::KeepRatioHeightScale:
			{
				f32 scaleY = (f32)screenSize.Height / (f32)texSize.Height;
				f32 scaleX = scaleY;
				s32 offsetX = std::round(((s32)screenSize.Width - texSize.Width * scaleX) / 2);
				fk_addons::draw2DImage(driver, background, sourceRect, core::vector2d<s32>(offsetX, 0), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>(scaleX, scaleY));
				break;
			}
			case FK_SceneBackgroundMode::CenteredUnstretched:
			{
				f32 scaleY = scale_factorX;
				f32 scaleX = scale_factorY;
				s32 offsetY = std::round(((s32)screenSize.Height - texSize.Height * scaleY) / 2);
				s32 offsetX = std::round(((s32)screenSize.Width - texSize.Width * scaleX) / 2);
				fk_addons::draw2DImage(driver, background, sourceRect, core::vector2d<s32>(offsetX, offsetY), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>(scaleX, scaleY));
				break;
			}
			}
		}
	};

	/* draw character preview */
	void FK_SceneCharacterSelect_Base::drawCharacterPreview(bool player1Side,
		FK_SelectionPreview& preview) {
		if (preview.texture == NULL) {
			return;
		}
		core::rect<f32> characterImagePreviewPosition =
			player1Side ? 
			sceneResources.characterImagePreviewPositionPlayer1 : sceneResources.characterImagePreviewPositionPlayer2;
		s32 x1 = (s32)std::round(characterImagePreviewPosition.UpperLeftCorner.X * scale_factorX);
		s32 y1 = (s32)std::round(characterImagePreviewPosition.UpperLeftCorner.Y * scale_factorY);
		if (!player1Side) {
			x1 = screenSize.Width - (s32)std::round(characterImagePreviewPosition.UpperLeftCorner.X * scale_factorX);
			x1 -= (s32)std::round(characterImagePreviewPosition.LowerRightCorner.X * scale_factorX);
		}
		f32 width = characterImagePreviewPosition.LowerRightCorner.X * scale_factorX;
		f32 height = characterImagePreviewPosition.LowerRightCorner.Y * scale_factorY;
		f32 scaleX = width / (f32)preview.texture->getSize().Width;
		f32 scaleY = height / (f32)preview.texture->getSize().Height;
		core::rect<s32> sourceRect(0, 0, preview.texture->getSize().Width, preview.texture->getSize().Height);

		video::SColor color(255, 255, 255, 255);
		if (!preview.selectable) {
			color = video::SColor(255, 32, 32, 32);
		}

		fk_addons::draw2DImage(driver, preview.texture, sourceRect, core::vector2d<s32>(x1, y1), core::vector2d<s32>(0, 0), 0,
			core::vector2d<f32>(scaleX, scaleY), true, color, 
			core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
	}


	/* draw player selector */
	void FK_SceneCharacterSelect_Base::drawPlayerSelector(u32 selectorIndex,
		bool playerReady, f32 phaseOffset, video::SColor selectorColor){
		core::dimension2d<u32> texSize = selector->getSize();
		s32 width = texSize.Width;
		s32 height = texSize.Height;
		// get the amplification factor
		f32 magnificationAmplitude = sceneResources.characterSelectorOscillationAmplitude;
		f32 magnificationOffset = sceneResources.characterSelectorOscillationOffset;
		f32 magnificationPeriodMs = sceneResources.characterSelectorOscillationPeriodMS;
		f32 phase = 2 * core::PI * ((f32)now / magnificationPeriodMs + phaseOffset);
		phase = fmod(phase, 2 * core::PI);
		//f32 triangleWave = 2 * abs(2 * (phase / 2 - floor(phase / 2 + core::PI))) - 1;
		f32 magnificationFactor = (magnificationOffset + magnificationAmplitude * sin(phase));
		if (playerReady){
			magnificationFactor = 1.0;
		}
		// get the current icon's position and size
		core::rect<s32> iconRect = getCharacterIconPosition(selectorIndex);
		// set X and Y
		s32 x = iconRect.UpperLeftCorner.X;
		//(originX + ((width / 2 + offsetX) * ((s32)selectorIndex % picturesPerRow) + delta_x)) * scale_factorX;
		s32 y = iconRect.UpperLeftCorner.Y;
		s32 icon_width = iconRect.getWidth();
		s32 icon_height = iconRect.getHeight();
		s32 dest_width = icon_width * magnificationFactor;
		s32 dest_height = icon_height * magnificationFactor;
		if (sceneResources.characterSelectorAnimationMode == FK_CursorAnimationMode::Alpha) {
			dest_width = icon_width;
			dest_height = icon_height;
			s32 alpha = std::round(255 * magnificationFactor);
			alpha = core::clamp(alpha, 1, 255);
			selectorColor.setAlpha((u32)alpha);
		}
		video::SColor const color = selectorColor;
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// add additional spacing to compensate for the dimension change over time
		s32 delta_x = (icon_width - dest_width) / 2;
		s32 delta_y = (icon_height - dest_height) / 2;
		x += delta_x;
		y += delta_y;
		//(originY + ((height / 2 + offsetY) * floor((s32)selectorIndex / picturesPerRow) + delta_y)) * scale_factorY;
		core::rect<s32> destinationRect = core::rect<s32>(x, y, x + dest_width, y + dest_height);
		core::rect<s32> sourceRect = core::rect<s32>(0, 0, width, height);
		driver->draw2DImage(selector, destinationRect, sourceRect, 0, vertexColors, true);
	}

	/* load single character file */
	void FK_SceneCharacterSelect_Base::loadSingleCharacterFile(std::string characterPath, std::string &playerName){
		std::ifstream characterFile((characterPath + "character.txt").c_str());
		std::string characterFileKeyName = "#NAME";
		while (!characterFile.eof()){
			std::string temp;
			characterFile >> temp;
			/* check if temp matches one of the keys*/
			/* name */
			if (strcmp(temp.c_str(), characterFileKeyName.c_str()) == 0){
				characterFile >> temp;
				std::replace(temp.begin(), temp.end(), '_', ' ');
				playerName = temp;
				break;
			}
		}
		characterFile.close();
		if (characterInfos.count(characterPath) == 0) {
			FK_Character* tempCharacter = new FK_Character;
			if (unlockExtraCostumesForeverFlag ||  (getCurrentActiveCheatCodes() & FK_Cheat::CheatUnlockExtraCostumes)) {
				tempCharacter->loadVariablesForSelectionScreenExtra(databaseAccessor, "character.txt", characterPath, commonResourcesPath, smgr);
			}
			else {
				tempCharacter->loadVariablesForSelectionScreen(databaseAccessor, "character.txt", characterPath, commonResourcesPath, smgr);
			}
			characterInfos[characterPath] = tempCharacter;
			tempCharacter = NULL;
		}
	};

	// check if RANDOM can be selected
	bool FK_SceneCharacterSelect_Base::randomCanBeSelected(u32 inputButtons){
		std::string dummyPath = dummyCharacterDirectory.substr(0, dummyCharacterDirectory.size() - 1);
		bool unlocked = std::find(unlockedCharacters.begin(), unlockedCharacters.end(),
			dummyPath) != unlockedCharacters.end();
		bool triggerPressed = ((inputButtons & FK_Input_Buttons::Any_SystemButton) != FK_Input_Buttons::Selection_Button) &&
			(inputButtons & FK_Input_Buttons::Trigger_Button) != 0;
		return (unlocked && triggerPressed);
	}

	// check if SIMPLE MODE is active
	bool FK_SceneCharacterSelect_Base::isSimpleModeSelected(u32 inputButtons){
		/*bool triggerPressed = (inputButtons & FK_Input_Buttons::ScrollLeft_Button) != 0;
		return triggerPressed;*/
		return false;
	}

	// help with texture removal
	u32	FK_SceneCharacterSelect_Base::getNodeUsedTextures(scene::ISceneNode* node, core::array<video::ITexture*>& a)
	{
		u32 count = 0;

		for (u32 m = 0; m < node->getMaterialCount(); ++m)
		{
			const video::SMaterial& material = node->getMaterial(m);

			for (u32 n = 0; n < video::MATERIAL_MAX_TEXTURES; ++n)
			{
				video::ITexture* texture = material.getTexture(n);
				if (!texture)
					break; // if texture n is not set, assume n+1 is not

						   // make sure texture is not in array before we try to put
						   // it in
				if (a.linear_search(texture) < 0) {
					a.push_back(texture);

					// increment count of textures added
					count += 1;
				}
			}
		}

		// now recurse on children... 
		core::list<scene::ISceneNode*>::ConstIterator begin = node->getChildren().begin();
		core::list<scene::ISceneNode*>::ConstIterator end = node->getChildren().end();

		for (/**/; begin != end; ++begin)
			count += getNodeUsedTextures(*begin, a);

		return count;
	}


	void FK_SceneCharacterSelect_Base::removeCharacterNodeFromCache(FK_Character* character) {
		if (sceneResources.useImagePreviewsInsteadOfMesh) {
			return;
		}
		core::array<video::ITexture*> texToRemove;
		getNodeUsedTextures(character->getAnimatedMesh(), texToRemove);
		u32 size = character->getCharacterAdditionalObjectsNodes().size();
		for (u32 i = 0; i < size; ++i) {
			if (character->getCharacterAdditionalObjectsNodes().at(i) != NULL) {
				smgr->getMeshCache()->removeMesh(character->getCharacterAdditionalObjectsNodes().at(i)->getMesh());
			}
		}
		smgr->getMeshCache()->removeMesh(character->getAnimatedMesh()->getMesh());
		size = texToRemove.size();
		for (u32 i = 0; i < size; ++i) {
			device->getVideoDriver()->removeTexture(texToRemove[i]);
		}
	}
	/* load scene configuration file*/
	bool FK_SceneCharacterSelect_Base::loadConfigurationFile(){
		std::string resourcePath = mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath;
		std::string filename = resourcePath + "config.txt";
		std::ifstream inputFile(filename.c_str());
		if (!inputFile) {
			return false;
		}
		std::map<FK_CharacterSelectFileKeys, std::string> fileKeys;
		fileKeys[FK_CharacterSelectFileKeys::ScreenBGM] = "#bgm";
		fileKeys[FK_CharacterSelectFileKeys::DefaultScreenSize] = "#reference_screen_size";
		fileKeys[FK_CharacterSelectFileKeys::BackgroundTexture] = "#background_picture";
		fileKeys[FK_CharacterSelectFileKeys::OverlayTexture] = "#overlay_texture";
		fileKeys[FK_CharacterSelectFileKeys::ShowOverlay] = "#show_overlay";
		fileKeys[FK_CharacterSelectFileKeys::ShowBackground] = "#show_background";
		fileKeys[FK_CharacterSelectFileKeys::BackgroundMode] = "#background_drawing_mode";
		fileKeys[FK_CharacterSelectFileKeys::BackgroundColor] = "#background_color";
		fileKeys[FK_CharacterSelectFileKeys::BackgroundVelocity] = "#background_tile_velocity";
		fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorColors] = "#selector_colors";
		fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorTexture] = "#selector_texture";
		fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorAnimationParams] = "#selector_animation_parameters";
		fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorAnimationMode] = "#selector_animation_mode";
		fileKeys[FK_CharacterSelectFileKeys::StartingIndexes] = "#starting_indexes";
		fileKeys[FK_CharacterSelectFileKeys::ShowRingoutBox] = "#show_ringout_box";
		fileKeys[FK_CharacterSelectFileKeys::AllowRingoutToggle] = "#allow_ringout_toggle";
		fileKeys[FK_CharacterSelectFileKeys::CenterCharacterBox] = "#center_character_box";
		fileKeys[FK_CharacterSelectFileKeys::CharacterSelectIconViewport] = "#character_box_size";
		fileKeys[FK_CharacterSelectFileKeys::CharacterNamesPosition] = "#character_names_position";
		fileKeys[FK_CharacterSelectFileKeys::CharacterIconMinSize] = "#character_icons_min_size";
		fileKeys[FK_CharacterSelectFileKeys::CharacterIconMaxSize] = "#character_icons_max_size";
		fileKeys[FK_CharacterSelectFileKeys::CharacterIconMinOffset] = "#character_icons_min_offset";
		fileKeys[FK_CharacterSelectFileKeys::CharacterIconMaxOffset] = "#character_icons_max_offset";
		fileKeys[FK_CharacterSelectFileKeys::CharacterIconRowColumnOffset] = "#character_icons_row_columns_offset";
		fileKeys[FK_CharacterSelectFileKeys::UseBitmapPreviewsInsteadOfMesh] = "#use_image_previews";
		fileKeys[FK_CharacterSelectFileKeys::CharacterImagePreviewPosition] = "#image_previews_position";
		fileKeys[FK_CharacterSelectFileKeys::CharacterMeshIdlePosition] = "#mesh_idle_position";
		fileKeys[FK_CharacterSelectFileKeys::CharacterMeshSelectionPosition] = "#mesh_selection_position";
		fileKeys[FK_CharacterSelectFileKeys::CameraPosition] = "#camera_position";
		fileKeys[FK_CharacterSelectFileKeys::CharacterRotationAngle] = "#mesh_rotation_angles";
		fileKeys[FK_CharacterSelectFileKeys::Use2DCoordinatesForCharacters] = "#use_screen_coordinates";
		fileKeys[FK_CharacterSelectFileKeys::DrawLoadingScreen] = "#draw_loading_screen";
		fileKeys[FK_CharacterSelectFileKeys::LoadingScreenTexture] = "#loading_screen_texture";
		fileKeys[FK_CharacterSelectFileKeys::LoadingScreenTextureArcadeLadder] = "#loading_screen_arcade_ladder_texture";
		fileKeys[FK_CharacterSelectFileKeys::ArcadeModeCompletedLogo] = "#arcade_mode_completed_logo";
		fileKeys[FK_CharacterSelectFileKeys::ShowArcadeModeCompletedLogo] = "#show_arcade_mode_completed_logo";
		fileKeys[FK_CharacterSelectFileKeys::MaximumNumberOfFullSizeCostumeDots] = "#max_number_of_full_size_costume_dots";
		fileKeys[FK_CharacterSelectFileKeys::CostumeDotsVerticalPosition] = "#vertical_position_of_costume_dots";
		std::string temp;
		while (inputFile >> temp) {
			if (!inputFile) {
				break;
			}
			if (temp == fileKeys[FK_CharacterSelectFileKeys::DefaultScreenSize]) {
				inputFile >> sceneResources.referenceScreenSize.Width >> sceneResources.referenceScreenSize.Height;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::StartingIndexes]) {
				inputFile >> sceneResources.startingPlayerIndexes.first >> sceneResources.startingPlayerIndexes.second;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::UseBitmapPreviewsInsteadOfMesh]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.useImagePreviewsInsteadOfMesh = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::ScreenBGM]) {
				inputFile >> sceneResources.BGMName >> sceneResources.BGMVolume >> sceneResources.BGMPitch;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::BackgroundTexture]) {
				inputFile >> sceneResources.backgroundTexture;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::BackgroundMode]) {
				u32 value;
				inputFile >> value;
				if (value < (u32)FK_SceneBackgroundMode::NumberOfBackgroundItems) {
					sceneResources.backgroundMode = (FK_SceneBackgroundMode)value;
				}
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::BackgroundColor]) {
				u32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				sceneResources.backgroundColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::BackgroundVelocity]) {
				inputFile >> sceneResources.backgroundMovementVelocity.X >> sceneResources.backgroundMovementVelocity.Y;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::ShowBackground]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.hasBackground = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::OverlayTexture]) {
				inputFile >> sceneResources.overlayTexture;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::ShowOverlay]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.hasOverlay = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::MaximumNumberOfFullSizeCostumeDots]) {
				u32 numberOfCostumeDots;
				inputFile >> numberOfCostumeDots;
				sceneResources.maximumNumberOfFullSizeOutfitDots = numberOfCostumeDots;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::MaximumNumberOfFullSizeCostumeDots]) {
				inputFile >> sceneResources.costumeDotsVerticalPosition;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorColors]) {
				u32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				sceneResources.characterSelectorColors.first = video::SColor(a, r, g, b);
				inputFile >> a >> r >> g >> b;
				sceneResources.characterSelectorColors.second = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorTexture]) {
				inputFile >> sceneResources.characterSelectorTexture;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorAnimationParams]) {
				f32 period, amplitude, offset, phase1, phase2;
				inputFile >> period >> amplitude >> offset >> phase1 >> phase2;
				sceneResources.characterSelectorOscillationAmplitude = amplitude;
				sceneResources.characterSelectorOscillationOffset = offset;
				sceneResources.characterSelectorOscillationPhases.first = phase1;
				sceneResources.characterSelectorOscillationPhases.second = phase2;
				sceneResources.characterSelectorOscillationPeriodMS = period;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterSelectorAnimationMode]) {
				u32 value;
				inputFile >> value;
				if (value < (u32)FK_CursorAnimationMode::NumberOfAnimationItems) {
					sceneResources.characterSelectorAnimationMode = (FK_CursorAnimationMode)value;
				}
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::ShowRingoutBox]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.drawStageBorderBox = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::AllowRingoutToggle]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.allowRingoutToggle = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::ArcadeModeCompletedLogo]) {
				inputFile >> sceneResources.arcadeModeCompletedLogo;
				f32 offsetX, offsetY, sizeX, sizeY;
				inputFile >> offsetX >> offsetY >> sizeX >> sizeY;
				sceneResources.arcadeLogoPosition = core::rect<f32>(offsetX, offsetY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::ShowArcadeModeCompletedLogo]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.showArcadeModeCompletedLogo = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterSelectIconViewport]) {
				s32 offsetX, offsetY, sizeX, sizeY;
				inputFile >> offsetX >> offsetY >> sizeX >> sizeY;
				sceneResources.characterSelectBoxPosition = core::rect<s32>(offsetX, offsetY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterImagePreviewPosition]) {
				f32 offsetX, offsetY, sizeX, sizeY;
				inputFile >> offsetX >> offsetY >> sizeX >> sizeY;
				sceneResources.characterImagePreviewPositionPlayer1 = core::rect<f32>(offsetX, offsetY, sizeX, sizeY);
				inputFile >> offsetX >> offsetY >> sizeX >> sizeY;
				sceneResources.characterImagePreviewPositionPlayer2 = core::rect<f32>(offsetX, offsetY, sizeX, sizeY);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterIconMaxSize]) {
				f32 width, height;
				inputFile >> width >> height;
				sceneResources.maxIconSize = core::dimension2d<f32>(width, height);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterIconMinSize]) {
				f32 width, height;
				inputFile >> width >> height;
				sceneResources.minIconSize = core::dimension2d<f32>(width, height);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterIconMinOffset]) {
				f32 offsetX, offsetY;
				inputFile >> offsetX >> offsetY;
				sceneResources.minIconOffset = core::dimension2d<f32>(offsetX, offsetY);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterIconMaxOffset]) {
				f32 offsetX, offsetY;
				inputFile >> offsetX >> offsetY;
				sceneResources.maxIconOffset = core::dimension2d<f32>(offsetX, offsetY);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterIconRowColumnOffset]) {
				f32 offsetX, offsetY;
				inputFile >> offsetX >> offsetY;
				sceneResources.iconRowColumnOffset = core::dimension2d<f32>(offsetX, offsetY);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CenterCharacterBox]) {
				s32 flag;
				inputFile >> flag;
				flag = core::clamp(flag, 0, (s32)CenterCharacterBoxNoOfEntries);
				sceneResources.centerCharacterBox = (FK_CenterCharacterBox)flag;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterNamesPosition]) {
				s32 x, y;
				inputFile >> x >> y;
				sceneResources.characterNameOffset = core::vector2d<s32>(x, y);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterMeshIdlePosition]) {
				f32 x, y, z;
				inputFile >> x >> y >> z;
				sceneResources.player1CharacterPositionIdle = core::vector3df(x, y, z);
				inputFile >> x >> y >> z;
				sceneResources.player2CharacterPositionIdle = core::vector3df(x, y, z);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterMeshSelectionPosition]) {
				f32 x, y, z;
				inputFile >> x >> y >> z;
				sceneResources.player1CharacterPositionSelect = core::vector3df(x, y, z);
				inputFile >> x >> y >> z;
				sceneResources.player2CharacterPositionSelect = core::vector3df(x, y, z);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CameraPosition]) {
				f32 x, y, z;
				inputFile >> x >> y >> z;
				sceneResources.cameraPosition = core::vector3df(x, y, z);
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::CharacterRotationAngle]) {
				f32 a1, a2;
				inputFile >> a1 >> a2;
				sceneResources.player1CharacterYRotationAngle = a1;
				sceneResources.player2CharacterYRotationAngle = a2;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::Use2DCoordinatesForCharacters]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.use2DCoordinates = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::DrawLoadingScreen]) {
				s32 flag;
				inputFile >> flag;
				sceneResources.drawLoadingScreen = flag > 0;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::LoadingScreenTexture]) {
				inputFile >> sceneResources.loadingScreenTexture;
			}
			else if (temp == fileKeys[FK_CharacterSelectFileKeys::LoadingScreenTextureArcadeLadder]) {
				inputFile >> sceneResources.arcadeLadderloadingScreenTexture;
			}
		}
		return true;
	}

	/* setup basic resources */
	void FK_SceneCharacterSelect_Base::setupBasicResources() {
		sceneResources.referenceScreenSize.Width = fk_constants::FK_DefaultResolution.Width;
		sceneResources.referenceScreenSize.Height = fk_constants::FK_DefaultResolution.Height;
		sceneResources.allowRandomCharacterSelection = true;
		sceneResources.allowRandomStageSelection = true;
		sceneResources.allowRingoutToggle = true;
		sceneResources.drawStageBorderBox = true;
		sceneResources.hasOverlay = true;
		sceneResources.hasBackground = true;
		sceneResources.backgroundColor = video::SColor(255, 0, 255, 0);
		sceneResources.backgroundTexture = fk_constants::characterSelectionBackgroundFileName;
		sceneResources.backgroundMode = FK_SceneBackgroundMode::Tiled;
		sceneResources.overlayTexture = fk_constants::characterSelectionOverlayFileName;
		sceneResources.characterSelectorTexture = fk_constants::characterSelectionCursorFileName;
		sceneResources.lockedCharacterIconTexture = fk_constants::characterSelectionGenericPreviewFilename;
		sceneResources.lockedCostumeMarkTexture = fk_constants::characterSelectionLockedCharacterSignFileName;
		sceneResources.noArcadeEndingMarkTexture = fk_constants::characterSelectionNoArcadeSignFileName;
		sceneResources.currentCostumeIconTexture = fk_constants::characterSelectionFullCostumeDotFileName;
		sceneResources.availableCostumeIconTexture = fk_constants::characterSelectionEmptyCostumeDotFileName;
		sceneResources.maximumNumberOfFullSizeOutfitDots = 8;
		sceneResources.costumeDotsVerticalPosition = 331;
		sceneResources.loadingScreenTexture = "..\\title\\introScreenOld.png";
		sceneResources.arcadeLadderloadingScreenTexture = "..\\title\\introScreenOld.png";
		sceneResources.arcadeModeCompletedLogo = "completedArcadeIcon.png";
		sceneResources.showArcadeModeCompletedLogo = true;
		sceneResources.arcadeLogoPosition = core::rect<f32>(0.6f, 0.0f, 0.3f, 0.3f);
		sceneResources.BGMName = "menu(astral)_cut2.ogg";
		sceneResources.BGMVolume = 60.f;
		sceneResources.BGMPitch = 1.0f;
		sceneResources.characterSelectBoxPosition = core::rect<s32>(0, 0, 212, 270);
		sceneResources.centerCharacterBox = FK_CenterCharacterBox::FullCentering;
		sceneResources.minIconSize = core::dimension2d<f32>(16.f, 16.f);
		sceneResources.maxIconSize = core::dimension2d<f32>(64.f, 64.f);
		sceneResources.minIconOffset = core::dimension2d<f32>(2.f, 2.f);
		sceneResources.maxIconOffset = core::dimension2d<f32>(10.f, 10.f);
		sceneResources.iconRowColumnOffset = core::dimension2d<f32>(0.f, 0.f);
		sceneResources.backgroundMovementVelocity = core::vector2d<f32>(-50.f, -50.f);
		sceneResources.characterSelectorColors.first = video::SColor(255, 255, 50, 0);
		sceneResources.characterSelectorColors.second = video::SColor(255, 0, 50, 255);
		sceneResources.characterSelectorOscillationPhases = std::pair<f32, f32>(0.f, 0.5);
		sceneResources.characterSelectorOscillationAmplitude = 0.1f;
		sceneResources.characterSelectorOscillationOffset = 0.9f;
		sceneResources.characterSelectorOscillationPeriodMS = 1000.f;
		sceneResources.startingPlayerIndexes = std::pair<s32, s32>(1, 2);
		sceneResources.characterNameOffset = core::vector2d<s32>(120, 40);
		sceneResources.outfitSelectTextOffset = core::vector2d<s32>(0, 325);
		sceneResources.backgroundMode = FK_SceneBackgroundMode::Tiled;
		sceneResources.characterSelectorAnimationMode = FK_CursorAnimationMode::Zoom;
		sceneResources.player1CharacterPositionIdle = core::vector3d<f32>(80.f, 0.f, 0.f);
		sceneResources.player2CharacterPositionIdle = core::vector3d<f32>(-80.f, 0.f, 0.f);
		sceneResources.player1CharacterPositionSelect = core::vector3d<f32>(40.f, 150.f, -10.f);
		sceneResources.player2CharacterPositionSelect = core::vector3d<f32>(-40.f, 150.f, -10.f);
		sceneResources.cameraPosition = core::vector3d<f32>(0.f, 0.f, 320.0f);
		sceneResources.player1CharacterYRotationAngle = -15;
		sceneResources.player2CharacterYRotationAngle = 15;
		sceneResources.useImagePreviewsInsteadOfMesh = false;
		sceneResources.characterImagePreviewPositionPlayer1 = core::rect<f32>(50, 120, 96, 96);
		sceneResources.characterImagePreviewPositionPlayer2 = core::rect<f32>(50, 120, 96, 96);
		sceneResources.use2DCoordinates = false;
		sceneResources.drawLoadingScreen = false;

		// setup screen size multipliers
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)sceneResources.referenceScreenSize.Width;
		scale_factorY = screenSize.Height / (f32)sceneResources.referenceScreenSize.Height;

		// setup icon maps for system buttons

	}

	void FK_SceneCharacterSelect_Base::loadSystemIcons() {
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

	void FK_SceneCharacterSelect_Base::setupSystemButtonIcons() {

	}

	void FK_SceneCharacterSelect_Base::calculateRaycastedCoordinates() {
		player1RaycastedCoordinates = getIdleCharacterPosition(true);
		player2RaycastedCoordinates = getIdleCharacterPosition(false);
		player1RaycastedSelectionCoordinates = getSelectedCharacterPosition(true);
		player2RaycastedSelectionCoordinates = getSelectedCharacterPosition(false);
		characterRaycastedPositionCalculatedFlag = true;
	}

	core::vector3df FK_SceneCharacterSelect_Base::get3DPositionFrom2DCoordinates(core::vector3d<s32> coordinates) {
		//camera->OnRegisterSceneNode();
		core::vector2d<s32> testCoordinates(coordinates.X, coordinates.Y);
		core::line3df rayCasted = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(
			testCoordinates, smgr->getActiveCamera());
		f32 distance = abs(camera->getPosition().Z - coordinates.Z);
		return rayCasted.start + (rayCasted.getVector().normalize() * distance);
	}

	core::vector3df FK_SceneCharacterSelect_Base::getIdleCharacterPosition(bool player1Side) {
		if (sceneResources.use2DCoordinates) {
			if (characterRaycastedPositionCalculatedFlag) {
				return (player1Side ? 
					player1RaycastedCoordinates : player2RaycastedCoordinates);
			}
			else {
				core::vector3df temp_position = player1Side ?
					sceneResources.player1CharacterPositionIdle : sceneResources.player2CharacterPositionIdle;
				core::vector3d<s32> integerCoordinates;
				f32 Xscale = scale_factorX;
				f32 Yscale = scale_factorY;
				core::dimension2du baseSize = screenSize;
				if (gameOptions->getBorderlessWindowMode()) {
					baseSize = driver->getScreenSize();
					Xscale *= (f32)baseSize.Width / (f32)screenSize.Width;
					Yscale *= (f32)baseSize.Width / (f32)screenSize.Width;
					/*baseSize = borderlessWindowRenderTarget->getSize();
					Xscale = baseSize.Width / (f32)sceneResources.referenceScreenSize.Width;
					Yscale = baseSize.Height / (f32)sceneResources.referenceScreenSize.Height;*/
				}
				integerCoordinates.X = (s32)(std::floor(Xscale * temp_position.X));
				integerCoordinates.Y = (s32)(std::floor(Yscale * temp_position.Y));
				integerCoordinates.X += (s32)baseSize.Width / 2;
				integerCoordinates.Y += (s32)baseSize.Height / 2;
				integerCoordinates.Z = (s32)(std::floor(temp_position.Z));
				return get3DPositionFrom2DCoordinates(integerCoordinates);
			}
		}
		else {
			core::vector3df temp_position = player1Side ?
				sceneResources.player1CharacterPositionIdle : sceneResources.player2CharacterPositionIdle;
			f32 playerXposition = temp_position.X;
			if ((f32)screenSize.Width / screenSize.Height < 1.7 && !gameOptions->getBorderlessWindowMode()) {
				playerXposition = playerXposition * ((f32)screenSize.Width / screenSize.Height / 1.7);
			}
			temp_position.X = playerXposition;
			return temp_position;
		}
	}
	core::vector3df FK_SceneCharacterSelect_Base::getSelectedCharacterPosition(bool player1Side) {
		if (sceneResources.use2DCoordinates) {
			if (characterRaycastedPositionCalculatedFlag) {
				return (player1Side ? 
					player1RaycastedSelectionCoordinates : player2RaycastedSelectionCoordinates);
			}
			else {
				core::vector3df temp_position = player1Side ?
					sceneResources.player1CharacterPositionSelect : sceneResources.player2CharacterPositionSelect;
				core::vector3d<s32> integerCoordinates;
				f32 Xscale = scale_factorX;
				f32 Yscale = scale_factorY;
				core::dimension2du baseSize = screenSize;
				if (gameOptions->getBorderlessWindowMode()) {
					baseSize = driver->getScreenSize();
					Xscale *= (f32)baseSize.Width / (f32)screenSize.Width;
					Yscale *= (f32)baseSize.Width / (f32)screenSize.Width;
				}
				integerCoordinates.X = (s32)(std::floor(Xscale * temp_position.X));
				integerCoordinates.Y = (s32)(std::floor(Yscale * temp_position.Y));
				integerCoordinates.X += (s32)baseSize.Width / 2;
				integerCoordinates.Y += (s32)baseSize.Height / 2;
				integerCoordinates.Z = (s32)(std::floor(temp_position.Z));
				return get3DPositionFrom2DCoordinates(integerCoordinates);
			}
		}
		else {
			core::vector3df final_position = player1Side ?
				sceneResources.player1CharacterPositionSelect : sceneResources.player2CharacterPositionSelect;
			//core::vector3df temp_position = player1Side ?
			//	sceneResources.player1CharacterPositionIdle : sceneResources.player2CharacterPositionIdle;
			////core::vector3df temp_position = getIdleCharacterPosition(player1Side);
			//f32 incrementX = final_position.X - temp_position.X;
			//f32 incrementZ = final_position.Z - temp_position.Z;
			//if ((f32)screenSize.Width / screenSize.Height < 1.7 && !gameOptions->getBorderlessWindowMode()) {
			//	incrementX = -30 * ((f32)screenSize.Width / screenSize.Height / 1.7);
			//	incrementZ = 120 * ((f32)screenSize.Width / screenSize.Height / 1.7);
			//}
			//if (!player1Side) {
			//	incrementX = -incrementX;
			//}
			//temp_position.X += incrementX;
			//temp_position.Z += incrementZ;
			if ((f32)screenSize.Width / screenSize.Height < 1.7 && !gameOptions->getBorderlessWindowMode()) {
				final_position.X = final_position.X * ((f32)screenSize.Width / screenSize.Height / 1.7);
				final_position.Z = final_position.Z * ((f32)screenSize.Width / screenSize.Height / 1.7);
			}
			//final_position.Y = final_position.Y;
			return final_position;
		}
	}

	void FK_SceneCharacterSelect_Base::drawLoadingScreenProgressBar(f32 completionPercentage) {
		core::dimension2d<u32> barSize(screenSize.Width * 0.7, screenSize.Height / 36);
		s32 x = (screenSize.Width - barSize.Width) / 2;
		s32 y = 0;
		s32 offsetY = screenSize.Height / 64;
		y = screenSize.Height - offsetY - barSize.Height;
		core::rect<s32> destinationRect = core::rect<s32>(x, y,
			x + barSize.Width,
			y + barSize.Height);
		core::rect<s32> destinationRectIn = destinationRect;
		destinationRectIn.UpperLeftCorner = destinationRectIn.UpperLeftCorner + core::vector2d<s32>(1, 1);
		destinationRectIn.LowerRightCorner = destinationRectIn.LowerRightCorner - core::vector2d<s32>(1, 1);
		core::rect<s32> destinationRectOut = destinationRect;
		destinationRectOut.UpperLeftCorner = destinationRectOut.UpperLeftCorner - core::vector2d<s32>(1, 1);
		destinationRectOut.LowerRightCorner = destinationRectOut.LowerRightCorner + core::vector2d<s32>(1, 1);
		barSize.Width = completionPercentage / 100.0 * barSize.Width;
		core::rect<s32> barRect = core::rect<s32>(x, y,
			x + barSize.Width,
			y + barSize.Height);
		video::SColor const rectColor1(255, 0, 147, 255);
		video::SColor const rectColor2(255, 0, 127, 127);
		driver->draw2DRectangle(barRect, rectColor1, rectColor2, rectColor1, rectColor2);
		driver->draw2DRectangleOutline(destinationRect);
		driver->draw2DRectangleOutline(destinationRectIn, video::SColor(128, 128, 128, 128));
		driver->draw2DRectangleOutline(destinationRectOut, video::SColor(255, 0, 0, 0));
		auto font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		std::wstring textToDraw = L"Loading...";
		core::dimension2d<u32> textSize = font->getDimension(textToDraw.c_str());
		s32 x1 = barRect.UpperLeftCorner.X;
		s32 y2 = barRect.UpperLeftCorner.Y;
		s32 y1 = y2 - textSize.Height;
		s32 x2 = x1 + textSize.Width;
		core::rect<s32> textRect(x1, y1, x2, y2);
		fk_addons::drawBorderedText(font, textToDraw, textRect);
	}

	void FK_SceneCharacterSelect_Base::callEmbeddedSceneOptions()
	{
		if (embeddedSceneOptions != NULL) {
			disposeEmbeddedSceneOptions();
		}
		else {
			embeddedSceneOptions = new FK_SceneOptionsEmbedded;
			embeddedSceneOptions->setup(device, joystickInfo, gameOptions);
			isEmbeddedSceneOptionsActive = true;
		}
	}

	void FK_SceneCharacterSelect_Base::updateEmbeddedSceneOptions()
	{
		if (embeddedSceneOptions != NULL) {
			embeddedSceneOptions->update(false);
			if (!embeddedSceneOptions->isRunning()) {
				Sleep(100);
				disposeEmbeddedSceneOptions();
			}
		}
	}

	void FK_SceneCharacterSelect_Base::disposeEmbeddedSceneOptions()
	{
		if (embeddedSceneOptions != NULL) {
			embeddedSceneOptions->softDispose();
			delete embeddedSceneOptions;
			embeddedSceneOptions = NULL;
			setupInputMapper();
		}
		isEmbeddedSceneOptionsActive = false;
	}

	//! draw loading screen
	void FK_SceneCharacterSelect_Base::drawLoadingScreen(f32 completionPercentage) {
		if (!sceneResources.drawLoadingScreen) {
			return;
		}
		// get arena background for loading screen
		std::string resourcePath = mediaPath + FK_SceneCharacterSelect_Base::SelectionResourcePath +
			sceneResources.loadingScreenTexture;
		video::ITexture* loadingTexture = NULL;
		if (!sceneResources.loadingScreenTexture.empty()) {
			loadingTexture = driver->getTexture(resourcePath.c_str());
		}
		if (!loadingTexture) {
			sceneResources.loadingScreenTexture = std::string();
		}
		
		// create color array for background
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 255, 255, 0));

		FK_SceneWithInput::executePreRenderingRoutine();
		// draw stage picture
		if (loadingTexture) {
			core::dimension2d<u32> texSize = loadingTexture->getSize();
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, texSize.Width, texSize.Height);
			core::vector2df scale((f32)((f32)screenSize.Width / (f32)texSize.Width),
				(f32)((f32)screenSize.Height / (f32)texSize.Height));
			video::SColor color(255, 255, 255, 255);
			fk_addons::draw2DImage(driver, loadingTexture, sourceRect, core::vector2d<s32>(0, 0),
				core::vector2d<s32>(0, 0), 0, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			video::SColor const color_bck = video::SColor(255, 0, 0, 0);
			driver->draw2DRectangle(color_bck, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		// draw overlay
		drawLoadingScreenProgressBar(completionPercentage);
		FK_SceneWithInput::executePostRenderingRoutine();
		driver->endScene();
	}
}
