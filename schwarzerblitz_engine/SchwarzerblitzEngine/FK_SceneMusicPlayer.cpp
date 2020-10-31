#include "FK_SceneMusicPlayer.h"
#include "ExternalAddons.h"

namespace fk_engine {

	FK_SceneMusicPlayer::FK_SceneMusicPlayer() : FK_SceneWithInput()
	{
		clear();
	}

	FK_SceneMusicPlayer::FK_SceneMusicPlayer(
		IrrlichtDevice* newDevice, core::array<SJoystickInfo> joypadInfo, FK_Options* newOptions) :
		FK_SceneWithInput(newDevice, joypadInfo, newOptions)
	{
		setupIrrlichtDevice();
		FK_SceneWithInput::readSaveFile();
		setupJoypad();
		setupInputMapper();
		initialize();
	}
	void FK_SceneMusicPlayer::setup(IrrlichtDevice* newDevice, core::array<SJoystickInfo> new_joypadInfo, FK_Options* newOptions)
	{
		FK_SceneWithInput::setup(newDevice, new_joypadInfo, newOptions);
		setupIrrlichtDevice();
		FK_SceneWithInput::readSaveFile();
		setupJoypad();
		setupInputMapper();
		initialize();
	}
	void FK_SceneMusicPlayer::dispose()
	{
		delete soundManager;
		music.stop();
		clear();
		FK_SceneWithInput::dispose();
	}

	void FK_SceneMusicPlayer::update()
	{
		now = device->getTimer()->getTime();
		delta_t_ms = now - then;
		then = now;
		if (cycleId == 0) {
			cycleId = 1;
			delta_t_ms = 0;
		}
		else {
			updateInput();
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

	bool FK_SceneMusicPlayer::isRunning()
	{
		return !backToMenu;
	}

	FK_Scene::FK_SceneType FK_SceneMusicPlayer::nameId()
	{
		return FK_Scene::FK_SceneType::SceneMusicPlayer;
	}

	void FK_SceneMusicPlayer::initialize()
	{
		clear();
		setupGraphics();
		loadSystemIcons();
		setupSystemButtonIcons();
		setupSoundManager();
		loadMusicList();
		setNextScene(FK_Scene::FK_SceneType::SceneMainMenu);
		setPreviousScene(FK_Scene::FK_SceneType::SceneMainMenu);
	}

	void FK_SceneMusicPlayer::clear()
	{
		soundManager = NULL;
		isPlayingMusic = false;
		musicIndex = 0;
		playingMusicIndex = -1;
		availableSongs.clear();
		backToMenu = false;
		lastInputTime = 0;
		player1UsesJoypad = false;
		player2UsesJoypad = false;
	}

	bool FK_SceneMusicPlayer::loadMusicList()
	{
		std::string musicListFileName = mediaPath + "scenes\\music_player\\config.txt";
		std::ifstream inputFile(musicListFileName);
		if (!inputFile) {
			return false;
		}
		std::string newBgmKey = "#BGM";
		std::string bgmVolumeKey = "#BGM_VOLUME";
		std::string bgmPitchKey = "#BGM_PITCH";
		std::string bgmMetadataKey ="#BGM_METADATA";
		std::string bgmDescription ="#DESCRIPTION";
		availableSongs.clear();
		std::string temp;
		BGMStruct newBgm;
		while (inputFile >> temp) {
			if (temp == newBgmKey) {
				inputFile >> temp;
				if (!newBgm.path.empty()) {
					availableSongs.push_back(newBgm);
				}
				newBgm = BGMStruct();
				newBgm.path = temp;
			}
			/* bgm volume */
			else if (temp == bgmVolumeKey) {
				f32 temp_float = 0;
				inputFile >> temp_float;
				newBgm.volume = temp_float;
			}
			/* bgm pitch */
			else if (temp == bgmPitchKey) {
				f32 temp_float = 0;
				inputFile >> temp_float;
				newBgm.pitch = temp_float;
			}
			else if (temp == bgmMetadataKey) {
				std::string bgmName;
				std::string bgmAuthor;
				inputFile >> bgmName >> bgmAuthor;
				std::replace(bgmName.begin(), bgmName.end(), '_', ' ');
				std::replace(bgmAuthor.begin(), bgmAuthor.end(), '_', ' ');
				newBgm.title = fk_addons::convertNameToNonASCIIWstring(bgmName);
				newBgm.author = fk_addons::convertNameToNonASCIIWstring(bgmAuthor);
			}
			else if (temp == bgmDescription) {
				std::string description;
				inputFile.ignore();
				std::getline(inputFile, description);
				newBgm.description = fk_addons::convertNameToNonASCIIWstring(description);
			}
		}
		if (!newBgm.path.empty()) {
			availableSongs.push_back(newBgm);
		}
		return availableSongs.size() > 0;
	}

	bool FK_SceneMusicPlayer::playSelectedSong()
	{
		if (availableSongs.empty() || musicIndex >= availableSongs.size()) {
			return false;
		}
		auto song = availableSongs[musicIndex];
		std::string filePath = mediaPath + song.path;
		if (!music.openFromFile(filePath.c_str())) {
			return false;
		}
		soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
		playingMusicIndex = musicIndex;
		music.setLoop(true);
		music.setVolume(song.volume * gameOptions->getMusicVolume());
		music.setPitch(song.pitch);
		music.play();
		isPlayingMusic = true;
		return true;
	}

	bool FK_SceneMusicPlayer::pauseSelectedSong()
	{
		if (music.getStatus() == sf::Music::Playing) {
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			music.pause();
			return true;
		}
		else if (music.getStatus() == sf::Music::Paused) {
			soundManager->playSound("confirm", 100.0 * gameOptions->getSFXVolume());
			music.play();
			return true;
		}
		else {
			return false;
		}
	}

	bool FK_SceneMusicPlayer::stopSelectedSong()
	{
		if (music.getStatus() == sf::Music::Playing ||
			music.getStatus() == sf::Music::Paused) {
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			music.stop();
			playingMusicIndex = -1;
			isPlayingMusic = false;
			return true;
		}
		else {
			return false;
		}
	}

	void FK_SceneMusicPlayer::setupSoundManager()
	{
		soundManager = new FK_SoundManager(commonResourcesPath + "sound_effects\\");
		soundManager->addSoundFromDefaultPath("cursor", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("correct", "Cursor2.ogg");
		soundManager->addSoundFromDefaultPath("confirm", "Decision1.ogg");
		soundManager->addSoundFromDefaultPath("cancel", "Cancel1.ogg");
	}

	// menu index operations
	void FK_SceneMusicPlayer::increaseMenuIndex(s32 increase) {
		lastInputTime = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		musicIndex += increase;
		if (increase > 1 && musicIndex >= availableSongs.size()) {
			musicIndex = availableSongs.size() - 1;
		}
		else {
			musicIndex %= availableSongs.size();
		}
		inputReceiver->reset();
	}
	void FK_SceneMusicPlayer::decreaseMenuIndex(s32 decrease) {
		lastInputTime = now;
		soundManager->playSound("cursor", 100.0 * gameOptions->getSFXVolume());
		musicIndex -= decrease;
		if (decrease > 1 && musicIndex < 0) {
			musicIndex = 0;
		}
		else {
			musicIndex += (s32)availableSongs.size();
			musicIndex %= availableSongs.size();
		}
		inputReceiver->reset();
	}

	void FK_SceneMusicPlayer::selectItem()
	{
		lastInputTime = now;
		playSelectedSong();
	}

	void FK_SceneMusicPlayer::cancelSelection()
	{
		lastInputTime = now;
		if (isPlayingMusic) {
			stopSelectedSong();
			return;
		}
		else {
			soundManager->playSound("cancel", 100.0 * gameOptions->getSFXVolume());
			backToMenu = true;
		}
	}

	void FK_SceneMusicPlayer::updateInput()
	{
		u32 lastInputTimePlayer1 = lastInputTime;
		u32 lastInputTimePlayer2 = lastInputTime;
		// update input
		if (joystickInfo.size() > 0) {
			player1input->update(now, inputReceiver->JoypadStatus(0), false);
		}
		else {
			player1input->update(now, inputReceiver->KeyboardStatus(), false);
		}
		if (joystickInfo.size() > 1) {
			player2input->update(now, inputReceiver->JoypadStatus(1), false);
		}
		else {
			player2input->update(now, inputReceiver->KeyboardStatus(), false);
		}

		u32 inputButtons = player1input->getPressedButtons() | player2input->getPressedButtons();
		updateMenuInputTimeThresholdMS(inputButtons, delta_t_ms);
		u32 inputThreshold = getMenuInputTimeThresholdMS();

		// check input for player 1
		if (now - lastInputTimePlayer1 > inputThreshold) {
			u32 directionPlayer1 = player1input->getLastDirection();
			u32 lastButtonPlayer1 = player1input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer1 == FK_Input_Buttons::Cancel_Button) {
				cancelSelection();
				return;
			}
			else {
				if (directionPlayer1 == FK_Input_Buttons::Down_Direction) {
					increaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer1 == FK_Input_Buttons::ScrollRight_Button) {
					increaseMenuIndex(5);
					return;
				}
				else if (directionPlayer1 == FK_Input_Buttons::Up_Direction) {
					decreaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer1 == FK_Input_Buttons::ScrollLeft_Button) {
					decreaseMenuIndex(5);
					return;
				}
				else {
					// select item
					if (lastButtonPlayer1 == FK_Input_Buttons::Selection_Button) {
						selectItem();
						return;
					}
					else if (lastButtonPlayer1 == FK_Input_Buttons::Modifier_Button) {
						lastInputTime = now;
						pauseSelectedSong();
						return;
					}
				}
			}
		}
		// check input for player 2
		if (now - lastInputTimePlayer2 > inputThreshold) {
			u32 directionPlayer2 = player2input->getLastDirection();
			u32 lastButtonPlayer2 = player2input->getPressedButtons() & FK_Input_Buttons::Any_MenuButton;
			if (lastButtonPlayer2 == FK_Input_Buttons::Cancel_Button) {
				cancelSelection();
				return;
			}
			else {
				if (directionPlayer2 == FK_Input_Buttons::Down_Direction) {
					increaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer2 == FK_Input_Buttons::ScrollRight_Button) {
					increaseMenuIndex(5);
					return;
				}
				else if (directionPlayer2 == FK_Input_Buttons::Up_Direction) {
					decreaseMenuIndex();
					return;
				}
				else if (lastButtonPlayer2 == FK_Input_Buttons::ScrollLeft_Button) {
					decreaseMenuIndex(5);
					return;
				}
				else {
					// select item
					if (lastButtonPlayer2 == FK_Input_Buttons::Selection_Button) {
						selectItem();
						return;
					}
					else if (lastButtonPlayer2 == FK_Input_Buttons::Modifier_Button) {
						lastInputTime = now;
						pauseSelectedSong();
						return;
					}
				}
			}
		}
	}

	void FK_SceneMusicPlayer::drawAll()
	{
		drawBackground();
		drawListOfTracks();
		drawNowPlayingTab();
		drawArrows();
		drawTitle();
	}

	// make a basic setup for player input
	void FK_SceneMusicPlayer::setupInputForPlayers() {
		FK_SceneWithInput::setupInputForPlayers();
		setupSystemButtonIcons();
	}

	// setup icons for keyboard and joypad input
	void FK_SceneMusicPlayer::setupSystemButtonIcons() {
		s32 mapId = -1;
		std::string currentJoypadName = std::string();
		if (joystickInfo.size() > 0) {
			player1UsesJoypad = true;
			mapId = 1;
			currentJoypadName = std::string(joystickInfo[0].Name.c_str());
		}
		else {
			player1UsesJoypad = false;
			mapId = 0;
		}
		setIconsBasedOnCurrentInputMap(buttonInputLayoutPlayer1, keyboardKeyLayoutPlayer1,
			mapId, player1UsesJoypad, currentJoypadName
		);
		mapId = -1;
		currentJoypadName = std::string();
		if (joystickInfo.size() > 1) {
			player2UsesJoypad = true;
			mapId = 2;
			currentJoypadName = std::string(joystickInfo[1].Name.c_str());
		}
		else {
			player2UsesJoypad = false;
			mapId = 0;
		}
		setIconsBasedOnCurrentInputMap(buttonInputLayoutPlayer2, keyboardKeyLayoutPlayer2,
			mapId, player2UsesJoypad, currentJoypadName
		);
	}

	void FK_SceneMusicPlayer::loadSystemIcons() {
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

	/* draw button icons */
	void FK_SceneMusicPlayer::drawButtonIcons(s32 offsetX, s32 offsetY) {

		std::wstring playSongText = L"play";
		std::wstring pauseSongText = L"pause";
		std::wstring resumeSongText = L"resume";
		std::wstring stopSongText = L"stop";
		std::wstring backToMenuText = L"back to menu";
		core::dimension2du offset = captionFont->getDimension(L"A");
		s32 originalY = offsetY;
		drawSingleButton(offsetX, offsetY, FK_Input_Buttons::Selection_Button, playSongText, true);
		offsetY = originalY;
		bool modifierActive = isPlayingMusic;
		if (music.getStatus() == sf::Music::Paused) {
			drawSingleButton(offsetX, offsetY, FK_Input_Buttons::Modifier_Button, resumeSongText, modifierActive);
		}
		else {
			drawSingleButton(offsetX, offsetY, FK_Input_Buttons::Modifier_Button, pauseSongText, modifierActive);
		}
		offsetY = originalY;
		if (isPlayingMusic) {
			drawSingleButton(offsetX, offsetY, FK_Input_Buttons::Cancel_Button, stopSongText, true);
		}
		else {
			drawSingleButton(offsetX, offsetY, FK_Input_Buttons::Cancel_Button, backToMenuText, true);
		}
		
	}

	void FK_SceneMusicPlayer::drawSingleButton(s32 &x, s32 &y, FK_Input_Buttons button, std::wstring text, bool active)
	{
		video::ITexture* tex;
		core::dimension2du offset = captionFont->getDimension(L"A");
		core::dimension2du textSize = hintfont->getDimension(L"A");
		u32 iconScreenSize = textSize.Height;
		s32 baseWidth = (s32)iconScreenSize;
		s32 baseHeight = (s32)iconScreenSize;
		s32 iconWidth = baseWidth;
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		video::SColor normalTextColor = video::SColor(255, 196, 196, 196);
		video::SColor normalBorderColor = video::SColor(64, 0, 0, 0);
		video::SColor inactiveTextColor = video::SColor(255, 128, 128, 128);

		// player 1
		if (player1UsesJoypad) {
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)buttonInputLayoutPlayer1[button]];
		}
		else {
			tex = keyboardButtonTexture;
		}

		// change text color accordingly
		video::SColor tcolor = normalTextColor;
		if (!active) {
			tcolor = inactiveTextColor;
		}
		// draw different icon and text, depending on the keyboard/joypad notation
		// start by drawing the modifier/change costume button
		core::dimension2d<u32> iconSize = tex->getSize();
		iconWidth = baseWidth;
		if (player1UsesJoypad) {
			fk_addons::draw2DImage(device->getVideoDriver(), tex, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)baseWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			std::wstring itemToDraw = keyboardKeyLayoutPlayer1[button];
			textSize = hintfont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = baseWidth;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			fk_addons::draw2DImage(device->getVideoDriver(), tex, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)iconWidth / iconSize.Width, (f32)baseHeight / iconSize.Height),
				true, video::SColor(255, 255, 255, 255), core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				x,
				y,
				x + iconWidth,
				y + baseIconSize);
			fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		std::wstring itemToDraw = L": " + text;
		textSize = hintfont->getDimension(itemToDraw.c_str());
		core::rect<s32> destRect = core::rect<s32>(
			x + iconWidth,
			y,
			x + iconWidth + textSize.Width,
			y + textSize.Height);
		fk_addons::drawBorderedText(hintfont, itemToDraw, destRect,
			tcolor, normalBorderColor);
		x += 2 * iconWidth + textSize.Width;
		y += iconWidth;
	}

	// draw arrows
	void FK_SceneMusicPlayer::drawArrows() {
		u32 arrowFrequency = 500;
		f32 phase = 2 * core::PI * (f32)(now % arrowFrequency) / arrowFrequency;
		s32 addonY = floor(10 * asin(cos(phase)) / core::PI);
		u32 arrowSize = 16;
		s32 baseArrowY = 42;
		s32 baseArrowX = 68;
		if (musicIndex > 3 && availableSongs.size() > 10) {
			s32 x1 = baseArrowX * scale_factorX - arrowSize * scale_factorX / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y1 = (baseArrowY + addonY) * scale_factorY;
			s32 y2 = y1 + arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowUp_tex->getSize().Width, arrowUp_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowUp_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
		if (musicIndex < availableSongs.size() - 8 && availableSongs.size() > 10) {
			s32 x1 = baseArrowX * scale_factorX - arrowSize * scale_factorX / 2;
			s32 x2 = x1 + arrowSize * scale_factorX;
			s32 y2 = screenSize.Height - (baseArrowY + addonY) * scale_factorY;
			s32 y1 = y2 - arrowSize * scale_factorY;
			// draw arrow on selected item
			core::rect<s32> destinationRect = core::rect<s32>(x1, y1, x2, y2);
			core::rect<s32> sourceRect = core::rect<s32>(0, 0, arrowDown_tex->getSize().Width, arrowDown_tex->getSize().Height);
			// create color array for loading screen
			video::SColor const color = video::SColor(255, 255, 255, 255);
			video::SColor const vertexColors_background[4] = {
				color,
				color,
				color,
				color
			};
			driver->draw2DImage(arrowDown_tex, destinationRect, sourceRect, 0, vertexColors_background, true);
		}
	}

	void FK_SceneMusicPlayer::drawBackground() {
		core::dimension2d<u32> backgroundSize = menuBackgroundTexture->getSize();
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
		driver->draw2DImage(menuBackgroundTexture, destinationRect, sourceRect, 0, vertexColors_background);
	}

	void FK_SceneMusicPlayer::drawTitle() {
		s32 x = screenSize.Width / 2;
		s32 y = screenSize.Height / 20;
		std::wstring itemString = L"Jukebox";
		s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
		x -= sentenceWidth / 2;
		y -= sentenceHeight / 2;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(font, itemString, destinationRect,
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
	}

	void FK_SceneMusicPlayer::drawNowPlayingTab() {
		video::SColor color(255, 255, 255, 255);
		//color = irr::video::SColor(255, 200, 200, 20);
		s32 x = screenSize.Width / 2;
		s32 y = (s32)std::ceil(66 * scale_factorY);
		std::wstring itemString = L"Now Playing:";
		s32 sentenceWidth = font->getDimension(itemString.c_str()).Width;
		s32 sentenceHeight = font->getDimension(itemString.c_str()).Height;
		core::rect<s32> destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(font, itemString, destinationRect,
			color, irr::video::SColor(128, 0, 0, 0));
		y = y + (s32)std::ceil(sentenceHeight * 1.1f);
		core::dimension2du fontHeight = captionFont->getDimension(L"A");
		if (isPlayingMusic) {
			//color = irr::video::SColor(255, 255, 255, 255);
			itemString = availableSongs[playingMusicIndex].description;
			sentenceWidth = captionFont->getDimension(itemString.c_str()).Width;
			sentenceHeight = captionFont->getDimension(itemString.c_str()).Height;
			destinationRect = core::rect<s32>(x,
				y,
				x + sentenceWidth,
				y + sentenceHeight);
			fk_addons::drawBorderedText(captionFont, itemString, destinationRect,
				color, irr::video::SColor(128, 0, 0, 0));
		}
		y = y + (s32)std::ceil(fontHeight.Height * 1.1f);
		itemString = L"Title:";
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			color, irr::video::SColor(128, 0, 0, 0));
		y = y + (s32)std::ceil(sentenceHeight * 1.1f);
		if (isPlayingMusic) {
			itemString = availableSongs[playingMusicIndex].title;
			sentenceWidth = captionFont->getDimension(itemString.c_str()).Width;
			sentenceHeight = captionFont->getDimension(itemString.c_str()).Height;
			destinationRect = core::rect<s32>(x,
				y,
				x + sentenceWidth,
				y + sentenceHeight);
			fk_addons::drawBorderedText(captionFont, itemString, destinationRect,
				color, irr::video::SColor(128, 0, 0, 0));
		}
		y = y + (s32)std::ceil(fontHeight.Height * 1.1f);
		itemString = L"Author:";
		sentenceWidth = submenufont->getDimension(itemString.c_str()).Width;
		sentenceHeight = submenufont->getDimension(itemString.c_str()).Height;
		destinationRect = core::rect<s32>(x,
			y,
			x + sentenceWidth,
			y + sentenceHeight);
		fk_addons::drawBorderedText(submenufont, itemString, destinationRect,
			color, irr::video::SColor(128, 0, 0, 0));
		y = y + (s32)std::ceil(sentenceHeight * 1.1f);
		if (isPlayingMusic) {
			itemString = availableSongs[playingMusicIndex].author;
			sentenceWidth = captionFont->getDimension(itemString.c_str()).Width;
			sentenceHeight = captionFont->getDimension(itemString.c_str()).Height;
			destinationRect = core::rect<s32>(x,
				y,
				x + sentenceWidth,
				y + sentenceHeight);
			fk_addons::drawBorderedText(captionFont, itemString, destinationRect,
				color, irr::video::SColor(128, 0, 0, 0));
		}
		fontHeight = font->getDimension(L"A");
		y = y + (s32)std::ceil(fontHeight.Height * 1.1f);
		drawMusicProgressBar(x, y);		
	}

	void FK_SceneMusicPlayer::drawMusicProgressBar(s32 offsetX, s32 offsetY) {
		core::dimension2d<u32> barSize(screenSize.Width * 0.3, screenSize.Height / 36);
		f32 completionPercentage = 0.f;
		s32 durationMinutes = 0;
		s32 durationSeconds = 0;
		s32 positionMinutes = 0;
		s32 positionSeconds = 0;
		if (isPlayingMusic) {
			auto musicDuration = music.getDuration();
			auto musicPosition = music.getPlayingOffset();
			completionPercentage = 100.f * musicPosition.asSeconds() / musicDuration.asSeconds();
			positionMinutes = (s32)std::floor(musicPosition.asSeconds() / 60);
			positionSeconds = (s32)std::floor(musicPosition.asSeconds() - positionMinutes * 60);
			durationMinutes = (s32)std::floor(musicDuration.asSeconds() / 60);
			durationSeconds = (s32)std::floor(musicDuration.asSeconds() - durationMinutes * 60);
		}
		s32 x = offsetX;
		s32 y = offsetY;
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
		std::wstring textToDraw;
		if (positionMinutes < 10) {
			textToDraw += L"0";
		}
		textToDraw += std::to_wstring(positionMinutes) + L":";
		if (positionSeconds < 10) {
			textToDraw += L"0";
		}
		textToDraw += std::to_wstring(positionSeconds) + L" / ";
		if (durationMinutes < 10) {
			textToDraw += L"0";
		}
		textToDraw += std::to_wstring(durationMinutes) + L":";
		if (durationSeconds < 10) {
			textToDraw += L"0";
		}
		textToDraw += std::to_wstring(durationSeconds);
		core::dimension2d<u32> textSize = captionFont->getDimension(textToDraw.c_str());
		s32 x1 = barRect.UpperLeftCorner.X;
		s32 y1 = barRect.LowerRightCorner.Y;
		s32 y2 = y1 + textSize.Height;
		s32 x2 = x1 + textSize.Width;
		core::rect<s32> textRect(x1, y1, x2, y2);
		fk_addons::drawBorderedText(captionFont, textToDraw, textRect);
		core::dimension2du fontHeight = submenufont->getDimension(L"A");
		y2 += (s32)std::ceil(fontHeight.Height * 1.1f);
		drawButtonIcons(x, y2);
	}

	void FK_SceneMusicPlayer::drawListOfTracks()
	{
		for (u32 index = 0; index < availableSongs.size(); ++index) {
			drawTrack(index);
		}
	}

	//draw episode icon and completion percentage
	void FK_SceneMusicPlayer::drawTrack(u32 index) {
		// draw episode icon
		s32 offsetY = 66;
		s32 offsetX = 4;
		s32 additionalY = 5;

		core::dimension2du fontHeight = captionFont->getDimension(L"A");

		s32 fixedOffsetY = (s32)std::ceil((f32)offsetY * scale_factorY);
		s32 fixedSpacingY = (s32)std::ceil((f32)additionalY * scale_factorY + (f32)fontHeight.Height);
		// the plus one serves to avoid a bug which caused the last episode not to be displayed correctly with certain screen resolutions
		f32 additionaSpacing = 1.f;
		s32 thresholdValue = (s32)std::ceil(((f32)availableSongs.size() + 1) * fixedSpacingY + additionaSpacing);
		// add offset
		if (musicIndex > 3 && availableSongs.size() > 10) {
			s32 bufferY = (s32)std::ceil((f32)additionalY * scale_factorY);
			thresholdValue = screenSize.Height - (fixedOffsetY + bufferY) - thresholdValue;
			fixedOffsetY -= (fixedSpacingY * (musicIndex - 3));
			while (fixedOffsetY < thresholdValue) {
				fixedOffsetY += fixedSpacingY;
			}
		}

		s32 x = (s32)std::ceil((f32)offsetX * scale_factorX);
		s32 y = fixedOffsetY + index * fixedSpacingY;

		if (y < offsetY * scale_factorY || y + fontHeight.Height > screenSize.Height - 0.9 * offsetY * scale_factorY) {
			return;
		}

		video::SColor color(255, 255, 255, 255);
		s32 textX = x + (u32)std::ceil(4.f * scale_factorX);
		if (index == musicIndex) {
			color = irr::video::SColor(255, 200, 200, 20);
			textX += (u32)std::ceil(15.f * scale_factorX);
		}
		else if (index == playingMusicIndex) {
			f32 magnificationPeriodMs = 1000.0;
			f32 phase = 2 * core::PI * ((f32)now / magnificationPeriodMs);
			phase = fmod(phase, 2 * core::PI);
			f32 magnificationAmplitude = 48;
			//f32 triangleWave = 2 * abs(2 * (phase / 2 - floor(phase / 2 + core::PI))) - 1;
			s32 addon = magnificationAmplitude / 2 + (s32)std::floor(magnificationAmplitude * sin(phase));
			color = video::SColor(255, 160 + addon, 160 + addon, 160 + addon);
		}
		core::vector2d<s32> drawingPosition(x, y);
		// draw event number in a corner
		std::wstring musicString = L"";
		if (index < 9) {
			musicString += L"0";
		}
		musicString += std::to_wstring(index + 1);
		musicString += L". ";
		musicString += availableSongs[index].description;
		core::dimension2d<u32> textSize = captionFont->getDimension(musicString.c_str());
		fk_addons::drawBorderedText(captionFont, musicString, core::rect<s32>(textX, y, textX + textSize.Width, y + textSize.Height),
			color);
	}

	void FK_SceneMusicPlayer::setupGraphics() {
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_MenuFontIdentifier.c_str());
		submenufont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		captionFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		hintfont = device->getGUIEnvironment()->getFont(fk_constants::FK_SmallNoteFontIdentifier.c_str());
		/* textures */
		menuBackgroundTexture = driver->getTexture((commonResourcesPath + "common_menu_items\\menuBackground_base.png").c_str());
		menuCaptionOverlay = driver->getTexture((commonResourcesPath + "common_menu_items\\menu_CaptionOverlay.png").c_str());
		arrowUp_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowU.png").c_str());
		arrowDown_tex = driver->getTexture((commonResourcesPath + "common_menu_items\\arrowD.png").c_str());
		// load joypad settings textures
		loadJoypadSetupTextures();
	}
}