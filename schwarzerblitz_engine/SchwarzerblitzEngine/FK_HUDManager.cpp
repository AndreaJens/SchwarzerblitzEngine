#include"FK_HUDManager.h"
#include"ExternalAddons.h"
#include<algorithm>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

namespace fk_engine{
	FK_HUDManager::FK_HUDManager(IrrlichtDevice *new_device, std::string mediaPath, 
		FK_Character* new_player1, FK_Character* new_player2){
		device = new_device;
		driver = device->getVideoDriver();
		// get player paths
		std::string player1Path = new_player1->getCharacterDirectory();
		std::string player2Path = new_player2->getCharacterDirectory();
		player1 = new_player1;
		player2 = new_player2;
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		textFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		// reset the quote variable
		quoteToDrawPl1 = std::string();
		quoteToDrawPl2 = std::string();
		// internal variables
		oldLifeRatioPlayer1 = 1.0;
		oldLifeRatioPlayer2 = 1.0;
		oldComboCounterPlayer1 = 0;
		oldComboCounterPlayer2 = 0;
		bool player1TriggerStatus = false;
		bool player2TriggerStatus = false;
		player1TriggerRotationDuration = 0;
		player2TriggerRotationDuration = 0;
		player1TriggerRotationCounter = 0;
		player2TriggerRotationCounter = 0;
		player1CylinderRotation = 0.0f;
		player2CylinderRotation = 0.0f;
		// create lifebar textures
		lifebar1 = driver->getTexture((mediaPath + "hud\\bar1_portrait.png").c_str());
		lifebar1_bw = driver->getTexture((mediaPath + "hud\\bar1_portrait_bw.png").c_str());
		lifebar1sub = driver->getTexture((mediaPath + "hud\\lbar1.png").c_str());
		lifebar2sub = driver->getTexture((mediaPath + "hud\\lbar2.png").c_str());
		lifebar1sub_bw = driver->getTexture((mediaPath + "hud\\lbar1_bw_light.png").c_str());
		lifebar2sub_bw = driver->getTexture((mediaPath + "hud\\lbar2_bw_light.png").c_str());
		roundIndicatorEmpty = driver->getTexture((mediaPath + "hud\\roundDiamond.png").c_str());
		roundIndicatorFull = driver->getTexture((mediaPath + "hud\\roundDiamondFull.png").c_str());
		roundIndicatorFull_bw = driver->getTexture((mediaPath + "hud\\roundDiamondFull.png").c_str());
		// trigger icons
		triggerIcon = driver->getTexture((mediaPath + "hud\\triggerCounter_icon2.png").c_str());
		triggerCylinderIcon = driver->getTexture((mediaPath + "hud\\triggerGun2.png").c_str());
		triggerAnimationTexture = driver->getTexture((mediaPath + "hud\\triggerAnimation.png").c_str());
		triggerAnimationCellSize = 
			core::dimension2d<u32>(triggerAnimationTexture->getSize().Width / 4, triggerAnimationTexture->getSize().Height);
		// system text
		pauseText = driver->getTexture((mediaPath + "hud\\pause.png").c_str());
		KOText = driver->getTexture((mediaPath + "hud\\ko_bar.png").c_str());
		doubleKOText = driver->getTexture((mediaPath + "hud\\doubleko_bar.png").c_str());
		drawText = driver->getTexture((mediaPath + "hud\\draw.png").c_str());
		perfectText = driver->getTexture((mediaPath + "hud\\perfect_bar.png").c_str());
		roundText = driver->getTexture((mediaPath + "hud\\round_bar.png").c_str());
		finalRoundText = driver->getTexture((mediaPath + "hud\\finalround_bar.png").c_str());
		fightText = driver->getTexture((mediaPath + "hud\\fight_bar.png").c_str());
		timeEndText = driver->getTexture((mediaPath + "hud\\timeend_bar.png").c_str());
		ringoutText = driver->getTexture((mediaPath + "hud\\ringout_bar.png").c_str());
		doubleRingoutText = driver->getTexture((mediaPath + "hud\\double_ringout_bar.png").c_str());
		numberText = new video::ITexture*[10];
		for (int i = 0; i < 10; ++i){
			char number[10];
			sprintf_s(number, "%d", i);
			numberText[i] = driver->getTexture((mediaPath + "hud\\" + number + ".png").c_str());
		}
		winText = driver->getTexture((mediaPath + "hud\\win_bar.png").c_str());
		player1NameText = driver->getTexture((player1Path + "roundWinName.png").c_str());
		player2NameText = driver->getTexture((player2Path + "roundWinName.png").c_str());
		char buffer1[200];
		sprintf_s(buffer1, "%sportrait%i.png", player1Path.c_str(), player1->getOutfitId());
		char buffer2[200];
		sprintf_s(buffer2, "%sportrait%i.png", player2Path.c_str(), player2->getOutfitId());
		player1Portrait = driver->getTexture(buffer1);
		player2Portrait = driver->getTexture(buffer2);
		// prepare textures for input showing :3
		inputUp_ico = driver->getTexture((mediaPath + "input_res\\Up.png").c_str());
		inputDown_ico = driver->getTexture((mediaPath + "input_res\\Down.png").c_str());
		inputLeft_ico = driver->getTexture((mediaPath + "input_res\\Left.png").c_str());
		inputRight_ico = driver->getTexture((mediaPath + "input_res\\Right.png").c_str());
		inputUL_ico = driver->getTexture((mediaPath + "input_res\\UpLeft.png").c_str());
		inputUR_ico = driver->getTexture((mediaPath + "input_res\\UpRight.png").c_str());
		inputDL_ico = driver->getTexture((mediaPath + "input_res\\DownLeft.png").c_str());
		inputDR_ico = driver->getTexture((mediaPath + "input_res\\DownRight.png").c_str());
		inputUpH_ico = driver->getTexture((mediaPath + "input_res\\UpHeld.png").c_str());
		inputDownH_ico = driver->getTexture((mediaPath + "input_res\\DownHeld.png").c_str());
		inputLeftH_ico = driver->getTexture((mediaPath + "input_res\\LeftHeld.png").c_str());
		inputRightH_ico = driver->getTexture((mediaPath + "input_res\\RightHeld.png").c_str());
		inputULH_ico = driver->getTexture((mediaPath + "input_res\\UpLeftHeld.png").c_str());
		inputURH_ico = driver->getTexture((mediaPath + "input_res\\UpRightHeld.png").c_str());
		inputDLH_ico = driver->getTexture((mediaPath + "input_res\\DownLeftHeld.png").c_str());
		inputDRH_ico = driver->getTexture((mediaPath + "input_res\\DownRightHeld.png").c_str());
		inputA_ico = driver->getTexture((mediaPath + "input_res\\A.png").c_str());
		inputB_ico = driver->getTexture((mediaPath + "input_res\\B.png").c_str());
		inputX_ico = driver->getTexture((mediaPath + "input_res\\X.png").c_str());
		inputY_ico = driver->getTexture((mediaPath + "input_res\\Y.png").c_str());
		inputMask_ico = driver->getTexture((mediaPath + "input_res\\PadFace.png").c_str());
	};
	//draw HUD given the life ratio
	void FK_HUDManager::drawHUD(u32 numberOfIndicators, u32 player1winNumber, u32 player2winNumber, 
		s32 roundTimeS, bool pause, f32 frameDeltaTime_s){
		/* reset "quote to draw" for intros and such */
		quoteToDrawPl1 = std::string();
		quoteToDrawPl2 = std::string();
		/* draw trigger counters*/
		f32 player1lifeRatio = player1->getLifeRatio();
		f32 player2lifeRatio = player2->getLifeRatio();
		u32 player1Triggers = player1->getTriggerCounters();
		u32 player2Triggers = player2->getTriggerCounters();
		/* update old ratio variables */
		oldLifeRatioPlayer1 = std::max(oldLifeRatioPlayer1, player1lifeRatio);
		oldLifeRatioPlayer2 = std::max(oldLifeRatioPlayer2, player2lifeRatio);
		/* reduce oldLifeRatio by a set amount per second */
		const f32 barReductionPerSecond = 0.5;
		if (!player1->isHitStun()){
			oldLifeRatioPlayer1 -= barReductionPerSecond * frameDeltaTime_s;
		}
		if (!player2->isHitStun()){
			oldLifeRatioPlayer2 -= barReductionPerSecond * frameDeltaTime_s;
		}
		/* update trigger indicators */
		u32 triggerRotationDurationMs = 400;
		if (player1->isTriggerModeActive() != player1TriggerStatus){
			if (player1->isTriggerModeActive()){
				player1TriggerRotationDuration = triggerRotationDurationMs;
				player1TriggerRotationCounter = 0;
				s32 startY, endY, startX1, endX1, startX2, endX2;
				calculateTriggerIconPosition(fk_constants::FK_MaxTriggerCounterNumber - 1, 
					startY, endY, startX1, endX1, startX2, endX2);
				triggerAnimationLocations.push_back(core::position2di((endX1 + startX1) / 2, (endY + startY) / 2));
				triggerAnimationTimePassed.push_back(0);
			};
			player1TriggerStatus = player1->isTriggerModeActive();
		}
		if (player2->isTriggerModeActive() != player2TriggerStatus){
			if (player2->isTriggerModeActive()){
				player2TriggerRotationDuration = triggerRotationDurationMs;
				player2TriggerRotationCounter = 0;
				s32 startY, endY, startX1, endX1, startX2, endX2;
				calculateTriggerIconPosition(fk_constants::FK_MaxTriggerCounterNumber - 1, 
					startY, endY, startX1, endX1, startX2, endX2);
				triggerAnimationLocations.push_back(core::position2di((endX2 + startX2) / 2, (endY + startY) / 2));
				triggerAnimationTimePassed.push_back(0);
			};
			player2TriggerStatus = player2->isTriggerModeActive();
		}
		/* check for trigger moves */
		drawTriggerCounters(player1Triggers, player2Triggers, frameDeltaTime_s, pause);
		/* draw character portraits*/
		drawPlayerPortraits(pause);
		/* draw life bars */
		drawLifeBars(player1lifeRatio, player2lifeRatio, pause);
		/* draw player names */
		drawPlayerNames(player1->getDisplayName(), player2->getDisplayName());
		/* draw "win round" indicators */
		drawRoundIndicators(numberOfIndicators, player1winNumber,player2winNumber, pause);
		/* draw round timer*/
		drawTimer(roundTimeS);
		/* draw trigger animations */
		drawTriggerAnimations(frameDeltaTime_s, pause);
		/* draw combo indicators */
		drawComboCounters(player1->getComboCounter(), player2->getComboCounter(), frameDeltaTime_s);
		/* draw pause text, if the game is paused */
		if (pause){
			drawPause();
		}
	};

	/* draw trigger explosion animation */
	void FK_HUDManager::drawTriggerAnimations(f32 frameDeltaTime_s, bool pause){
		// for each item in the animation array...
		int size = triggerAnimationLocations.size();
		int deletionIndex = -1;
		f32 maxTime_s = 0.2;
		u32 numberOfFrames = 4;
		/* draw a proper animation */
		for (int i = 0; i < size; i++){
			if (triggerAnimationTimePassed[i] >= maxTime_s){
				deletionIndex = i;
			}
			else{
				int frameId = (s32)(floor(triggerAnimationTimePassed[i] * (s32)numberOfFrames / maxTime_s));
				frameId = core::clamp<s32>(frameId, 0, numberOfFrames - 1);
				video::SColor const color = video::SColor(255 * (1 - triggerAnimationTimePassed[i] / (3 * maxTime_s)), 255, 255, 255);
				triggerAnimationTimePassed[i] += frameDeltaTime_s;
				video::SColor const vertexColors[4] = {
					color,
					color,
					color,
					color
				};
				s32 startX = triggerAnimationLocations[i].X - triggerAnimationCellSize.Width * scale_factorX / 2;
				s32 endX = triggerAnimationLocations[i].X + triggerAnimationCellSize.Width * scale_factorX / 2;
				s32 startY = triggerAnimationLocations[i].Y - triggerAnimationCellSize.Width * scale_factorY / 2;
				s32 endY = triggerAnimationLocations[i].Y + triggerAnimationCellSize.Width * scale_factorY / 2;
				driver->draw2DImage(triggerAnimationTexture,
					core::rect<s32>(startX, startY, endX, endY),
					core::rect<s32>(frameId * triggerAnimationCellSize.Width, 0,
					(frameId + 1) * triggerAnimationCellSize.Width, triggerAnimationCellSize.Height), 
					0, vertexColors, true);
			}
		}
		/* then, delete all the expired animations from the stack */
		for (int i = 0; i < deletionIndex; i++){
			triggerAnimationTimePassed.pop_front();
			triggerAnimationLocations.pop_front();
		}
	};

	/* draw life bars*/
	void FK_HUDManager::drawLifeBars(f32 player1lifeRatio, f32 player2lifeRatio, bool pause){
		// get life bar size
		core::dimension2d<u32> barSize = lifebar1->getSize();
		core::dimension2d<u32> lbarSize = lifebar1sub->getSize();
		// get bar width adjusted by life ratio
		float barwidth1 = player1lifeRatio * lbarSize.Width;
		float barwidth2 = player2lifeRatio * lbarSize.Width;
		// get bar width adjusted by life ratio
		float vanishingBarwidth1 = oldLifeRatioPlayer1 * lbarSize.Width;
		float vanishingBarwidth2 = oldLifeRatioPlayer2 * lbarSize.Width;
		// create color array
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		/* prepare color arrays for hud lifebars */
		video::SColor const standardGreenColor = video::SColor(255, 25, 240, 25);
		video::SColor const darkerGreenColor = video::SColor(255, 25, 180, 25);
		video::SColor const yellowColor = video::SColor(255, 255, 255, 25);
		video::SColor const redColor = video::SColor(255, 255, 100, 25);
		video::SColor const standardGrayColor = video::SColor(255, 200, 200, 200);
		video::SColor const darkerGrayColor = video::SColor(255, 175, 175, 175);
		video::SColor const higherLifeBlueColor = video::SColor(255, 25, 240, 225);
		video::SColor const higherLifeDarkBlueColor = video::SColor(255, 25, 180, 165);
		/* vanishing life bars color */
		video::SColor const vanishinglifebarColors_1[4] = {
			yellowColor,
			yellowColor,
			redColor,
			redColor
		};
		video::SColor const vanishinglifebarColors_2[4] = {
			redColor,
			redColor,
			yellowColor,
			yellowColor
		};
		/* normal life bars color */
		video::SColor const lifebarColors_1[4] = {
			standardGreenColor,
			standardGreenColor,
			darkerGreenColor,
			darkerGreenColor
		};
		video::SColor const lifebarColors_1_pause[4] = {
			standardGrayColor,
			standardGrayColor,
			darkerGrayColor,
			darkerGrayColor
		};
		video::SColor const lifebarColors_2[4] = {
			darkerGreenColor,
			darkerGreenColor,
			standardGreenColor,
			standardGreenColor
		};
		video::SColor const lifebarColors_2_pause[4] = {
			darkerGrayColor,
			darkerGrayColor,
			standardGrayColor,
			standardGrayColor
		};
		// set a couple constants (due to the graphics used for the HUD)
		const f32 hudLifebarXoffsetFromCase = 27.0f;
		const f32 hudLifebarYoffsetFromCase = 9.0f;
		const f32 hudLifebarXoffsetFromCenterScreen = 80.0f;

		// draw first vanishing lifebars
		float lifebar1x = screenSize.Width / 2 + (hudLifebarXoffsetFromCenterScreen + lbarSize.Width - 
			barSize.Width - vanishingBarwidth1)*scale_factorX;
		driver->draw2DImage(lifebar1sub_bw,
			core::rect<s32>(lifebar1x,
			hudLifebarYoffsetFromCase * scale_factorY, lifebar1x + vanishingBarwidth1*scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(lbarSize.Width - vanishingBarwidth1, 0, lbarSize.Width, lbarSize.Height),
			0, pause ? lifebarColors_1_pause : vanishinglifebarColors_1, true);

		int x1 = screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX;
		int x2 = screenSize.Width / 2 + (hudLifebarXoffsetFromCase + vanishingBarwidth2)* scale_factorX;
		driver->draw2DImage(lifebar2sub_bw,
			core::rect<s32>(screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX, 
			hudLifebarYoffsetFromCase * scale_factorY,
			screenSize.Width / 2 + (hudLifebarXoffsetFromCase + vanishingBarwidth2)* scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(0, 0, vanishingBarwidth2, lbarSize.Height), 0, pause ? lifebarColors_2_pause : vanishinglifebarColors_2, true);

		// draw normal lifebars
		lifebar1x = screenSize.Width / 2 + (hudLifebarXoffsetFromCenterScreen + lbarSize.Width - 
			barSize.Width - barwidth1)*scale_factorX;
		driver->draw2DImage(lifebar1sub_bw,
			core::rect<s32>(lifebar1x,
			hudLifebarYoffsetFromCase * scale_factorY, lifebar1x + barwidth1*scale_factorX, 
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(lbarSize.Width - barwidth1, 0, lbarSize.Width, lbarSize.Height),
			0, pause ? lifebarColors_1_pause : lifebarColors_1, true);

		x1 = screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX;
		x2 = screenSize.Width / 2 + (hudLifebarXoffsetFromCase + barwidth2)* scale_factorX;
		driver->draw2DImage(lifebar2sub_bw,
			core::rect<s32>(screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX, 
			hudLifebarYoffsetFromCase * scale_factorY,
			screenSize.Width / 2 + (hudLifebarXoffsetFromCase + barwidth2)* scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(0, 0, barwidth2, lbarSize.Height), 0, pause ? lifebarColors_2_pause : lifebarColors_2, true);

		driver->draw2DImage(pause ? lifebar1_bw : lifebar1,
			core::rect<s32>(screenSize.Width / 2, 0, screenSize.Width / 2 + barSize.Width*scale_factorX,
			barSize.Height*scale_factorY),
			core::rect<s32>(barSize.Width, 0, 0, barSize.Height), 0, vertexColors, true);

		driver->draw2DImage(pause ? lifebar1_bw : lifebar1,
			core::rect<s32>(screenSize.Width / 2 - barSize.Width*scale_factorX, 0,
			screenSize.Width / 2, barSize.Height*scale_factorY),
			core::rect<s32>(0, 0, barSize.Width, barSize.Height), 0, vertexColors, true);
	}

	/* draw player names */
	void FK_HUDManager::drawPlayerNames(std::string player1Name, std::string player2Name){
		// convert names to wide strings format to write them (irrlicht accepts only wide strings here)
		std::transform(player1Name.begin(), player1Name.end(), player1Name.begin(), toupper);
		std::transform(player2Name.begin(), player2Name.end(), player2Name.begin(), toupper);
		std::wstring pl1Name = std::wstring(player1Name.begin(), player1Name.end());
		std::wstring pl2Name = std::wstring(player2Name.begin(), player2Name.end());
		/* get width and height of the string*/
		s32 sentenceWidth = font->getDimension(pl1Name.c_str()).Width;
		s32 sentenceHeight = font->getDimension(pl1Name.c_str()).Height;
		s32 sentenceOffsetX = 52; // distance from screen center
		s32 sentenceOffsetY = 20; // distance from the top of the screen
		// draw player 1 name near the lifebar
		core::rect<s32> destinationRect = core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		fk_addons::drawBorderedText(font, pl1Name, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		//drawNormalText(font, pl1Name, destinationRect);
		// draw player 2 name near the lifebar
		sentenceWidth = font->getDimension(pl2Name.c_str()).Width;
		sentenceHeight = font->getDimension(pl2Name.c_str()).Height;
		destinationRect = core::rect<s32>(screenSize.Width / 2 + sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 + sentenceWidth + sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		fk_addons::drawBorderedText(font, pl2Name, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0));
		//drawNormalText(font, pl2Name, destinationRect);
	}

	/* draw player portraits*/
	void FK_HUDManager::drawPlayerPortraits(bool pause){
		// create color array
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// get portrait dimensions
		core::dimension2d<u32> portraitSize = player1Portrait->getSize();
		// pause texture still to be implemented - draw portrait
		driver->draw2DImage(pause ? player1Portrait : player1Portrait,
			core::rect<s32>(0, 0, portraitSize.Width*scale_factorX,
			portraitSize.Height*scale_factorY),
			core::rect<s32>(0, 0, portraitSize.Width, portraitSize.Height), 0, vertexColors, true);
		// draw player 2 portrait (mirrored on X)
		portraitSize = player2Portrait->getSize();
		driver->draw2DImage(pause ? player2Portrait : player2Portrait,
			core::rect<s32>(screenSize.Width - portraitSize.Width*scale_factorX, 0, screenSize.Width,
			portraitSize.Height*scale_factorY),
			core::rect<s32>(portraitSize.Width, 0, 0, portraitSize.Height), 0, vertexColors, true);
	};
	/* draw combo counter */
	void FK_HUDManager::drawComboCounters(u32 player1ComboCounter, u32 player2ComboCounter, f32 frameDeltaTimeS){
		// set maximum duration for combo text
		comboTextMaxDurationS = 2.0f;
		comboTextDurationSPlayer1 += frameDeltaTimeS;
		comboTextDurationSPlayer2 += frameDeltaTimeS;
		// reset timers if the combo number has changed
		if (player1ComboCounter != oldComboCounterPlayer1){
			oldComboCounterPlayer1 = player1ComboCounter;
			comboTextDurationSPlayer1 = 0.f;
		}
		if (player2ComboCounter != oldComboCounterPlayer2){
			oldComboCounterPlayer2 = player2ComboCounter;
			comboTextDurationSPlayer2 = 0.f;
		}
		// convert names to wide strings format to write them (irrlicht accepts only wide strings here)
		char buffer[10];
		sprintf_s(buffer, "%i hits", (int)player1ComboCounter);
		core::stringw pl1ComboCounter = core::stringw(buffer);
		sprintf_s(buffer, "%i hits", (int)player2ComboCounter);
		core::stringw pl2ComboCounter = core::stringw(buffer);
		/* get damage*/
		std::wstring damageDonePlayer1 = std::to_wstring(player1->getComboDamage()) + L" damages";
		std::wstring damageDonePlayer2 = std::to_wstring(player2->getComboDamage()) + L" damages";
		/* get width and height of the string*/
		s32 sentenceWidth = font->getDimension(damageDonePlayer1.c_str()).Width;
		s32 sentenceHeight = font->getDimension(damageDonePlayer1.c_str()).Height;
		s32 sentenceOffsetX = 24; // distance from screen center
		s32 sentenceOffsetY = 20; // distance from the bottom of the screen
		// draw player 1 combo counter if combo > 0
		if (player1ComboCounter > 1 && comboTextDurationSPlayer1 < comboTextMaxDurationS){
			if (player1->getComboDamage() > 0){
				// draw combo damage
				fk_addons::drawBorderedText(font, damageDonePlayer1.c_str(), core::rect<s32>(sentenceOffsetX * scale_factorX,
					screenSize.Height - sentenceHeight - sentenceOffsetY * scale_factorY,
					sentenceWidth + sentenceOffsetX * scale_factorX,
					screenSize.Height - sentenceOffsetY * scale_factorY),
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
					false, false);
			}
			// draw combo counters
			sentenceWidth = font->getDimension(pl1ComboCounter.c_str()).Width;
			sentenceHeight = font->getDimension(pl1ComboCounter.c_str()).Height;
			sentenceOffsetY -= 15;
			fk_addons::drawBorderedText(font, pl1ComboCounter.c_str(), core::rect<s32>(sentenceOffsetX * scale_factorX,
				screenSize.Height - sentenceHeight - sentenceOffsetY * scale_factorY,
				sentenceWidth + sentenceOffsetX * scale_factorX,
				screenSize.Height - sentenceOffsetY * scale_factorY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0), 
				false, false);
		}
		sentenceOffsetY = 20;
		// draw player 2 combo counter if combo > 0
		if (player2ComboCounter > 1 && comboTextDurationSPlayer2 < comboTextMaxDurationS){
			// draw combo damage
			if (player2->getComboDamage() > 0){
				sentenceWidth = font->getDimension(damageDonePlayer2.c_str()).Width;
				sentenceHeight = font->getDimension(damageDonePlayer2.c_str()).Height;
				fk_addons::drawBorderedText(font, damageDonePlayer2.c_str(),
					core::rect<s32>(screenSize.Width - sentenceWidth - sentenceOffsetX * scale_factorX,
					screenSize.Height - sentenceHeight - sentenceOffsetY * scale_factorY,
					screenSize.Width - sentenceOffsetX * scale_factorX,
					screenSize.Height - sentenceOffsetY * scale_factorY),
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
					false, false);
			}
			// draw combo counters
			sentenceWidth = font->getDimension(pl2ComboCounter.c_str()).Width;
			sentenceHeight = font->getDimension(pl2ComboCounter.c_str()).Height;
			sentenceOffsetY -= 15;
			fk_addons::drawBorderedText(font, pl2ComboCounter.c_str(),
				core::rect<s32>(screenSize.Width - sentenceWidth - sentenceOffsetX * scale_factorX,
				screenSize.Height - sentenceHeight - sentenceOffsetY * scale_factorY,
				screenSize.Width - sentenceOffsetX * scale_factorX,
				screenSize.Height - sentenceOffsetY * scale_factorY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
				false, false);
		}
	}

	/* draw round indicators*/
	void FK_HUDManager::drawRoundIndicators(u32 numberOfIndicators, u32 player1winNumber, u32 player2winNumber, bool pause){
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		player1winNumber = std::min(numberOfIndicators, player1winNumber);
		player2winNumber = std::min(numberOfIndicators, player2winNumber);
		core::dimension2d<u32> iconSize = roundIndicatorEmpty->getSize();
		s32 roundIndicatorY = 28;
		s32 roundIndicatorXOffset = 16;
		for (u32 i = 0; i < numberOfIndicators; i++){
			s32 tempX1 = iconSize.Width * (i+1) * scale_factorX;
			s32 tempX2 = iconSize.Width * i * scale_factorX;
			s32 startY = roundIndicatorY * scale_factorY;
			s32 endY = startY + iconSize.Height * scale_factorY;
			s32 startX1 = screenSize.Width / 2 - roundIndicatorXOffset * scale_factorX - tempX1;
			s32 endX1 = startX1 + iconSize.Width*scale_factorX;
			s32 startX2 = screenSize.Width / 2 + roundIndicatorXOffset * scale_factorX + tempX2;
			s32 endX2 = startX2 + iconSize.Width*scale_factorX;
			video::ITexture* texture;
			if (i < player1winNumber){
				if (pause){
					texture = roundIndicatorFull_bw;
				}
				else{
					texture = roundIndicatorFull;
				}
			}
			else{
				texture = roundIndicatorEmpty;
			}
			driver->draw2DImage(texture,
				core::rect<s32>(startX1, startY, endX1, endY),
				core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, vertexColors, true);
			if (i < player2winNumber){
				if (pause){
					texture = roundIndicatorFull_bw;
				}
				else{
					texture = roundIndicatorFull;
				}
			}
			else{
				texture = roundIndicatorEmpty;
			}
			driver->draw2DImage(texture,
				core::rect<s32>(startX2, startY, endX2, endY),
				core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, vertexColors, true);
		}
	}

	void FK_HUDManager::calculateTriggerIconPosition(s32 index,
		s32 &startY, s32 &endY, s32 &startX1, s32 &endX1,
		s32 &startX2, s32 &endX2){
		s32 roundIndicatorY = 28;
		s32 roundIndicatorXOffset = 160;
		core::dimension2d<u32> iconSize = triggerIcon->getSize();
		s32 tempX1 = iconSize.Width * (index + 1) * scale_factorX;
		s32 tempX2 = iconSize.Width * index * scale_factorX;
		startY = roundIndicatorY * scale_factorY;
		endY = startY + iconSize.Height * scale_factorY;
		startX1 = screenSize.Width / 2 - roundIndicatorXOffset * scale_factorX - tempX1;
		endX1 = startX1 + iconSize.Width*scale_factorX;
		startX2 = screenSize.Width / 2 + roundIndicatorXOffset * scale_factorX + tempX2;
		endX2 = startX2 + iconSize.Width*scale_factorX;
	}


	/* draw round indicators*/
	void FK_HUDManager::drawTriggerCounters(u32 player1Triggers, u32 player2Triggers, f32 frameDeltaTime, bool pause){
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		/* get milliseconds */
		u32 frameDeltaTimeMs = floor(1000 * frameDeltaTime);
		/* draw gun cylinder */
		core::dimension2d<u32> gunIconSize = triggerCylinderIcon->getSize();
		f32 triggerAngleLimit = 360.0 / 6.0;
		s32 offsetX = gunIconSize.Width / 2 + 28;
		s32 offsetY = 16;
		// player 1
		s32 gunCylinderPositionY1 = offsetY  * scale_factorY;
		s32 gunCylinderPositionX1 = offsetX * scale_factorX;
		s32 gunCylinderPositionY2 = gunCylinderPositionY1 + gunIconSize.Height * scale_factorY;
		s32 gunCylinderPositionX2 = gunCylinderPositionX1 + gunIconSize.Width * scale_factorX;
		s32 cylinderCenterX = gunCylinderPositionX1 + gunIconSize.Width * scale_factorX / 2;
		s32 cylinderCenterY = gunCylinderPositionY1 + gunIconSize.Height * scale_factorY / 2;
		player1TriggerRotationCounter += frameDeltaTimeMs;
		if (player1TriggerRotationDuration > 0){
			player1CylinderRotation = triggerAngleLimit * (f32)player1TriggerRotationCounter / player1TriggerRotationDuration;
		}
		if (player1TriggerRotationCounter >= player1TriggerRotationDuration){
			player1TriggerRotationDuration = 0;
			player1CylinderRotation = 0.0f;
		}
		core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1);
		core::position2d<s32>(cylinderCenterX, cylinderCenterY);
		fk_addons::draw2DImage(driver, triggerCylinderIcon,
			core::rect<s32>(0, 0, gunIconSize.Width, gunIconSize.Height),
			core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1),
			core::position2d<s32>(cylinderCenterX, cylinderCenterY),
			player1CylinderRotation,
			core::vector2d<f32>(scale_factorX, scale_factorY)
			);
		/*
		driver->draw2DImage(triggerCylinderIcon,
			core::rect<s32>(gunCylinderPositionX1, gunCylinderPositionY1, gunCylinderPositionX2, gunCylinderPositionY2),
			core::rect<s32>(0, 0, gunIconSize.Width, gunIconSize.Height), 0, vertexColors, true);*/
		// player 2
		gunCylinderPositionX1 = screenSize.Width - (gunIconSize.Width + offsetX) * scale_factorX;
		gunCylinderPositionX2 = gunCylinderPositionX1 + gunIconSize.Width * scale_factorX;
		cylinderCenterX = gunCylinderPositionX1 + gunIconSize.Width * scale_factorX / 2;
		cylinderCenterY = gunCylinderPositionY1 + gunIconSize.Height * scale_factorY / 2;
		//player2CylinderRotation = player2CylinderRotation + 1.0f;
		player2TriggerRotationCounter += frameDeltaTimeMs;
		if (player2TriggerRotationDuration > 0){
			player2CylinderRotation = -triggerAngleLimit * (f32)player2TriggerRotationCounter / player2TriggerRotationDuration;
		}
		if (player2TriggerRotationCounter >= player2TriggerRotationDuration){
			player2TriggerRotationDuration = 0;
			player2CylinderRotation = 0.0f;
		}
		core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1);
		core::position2d<s32>(cylinderCenterX, cylinderCenterY);
		fk_addons::draw2DImage(driver, triggerCylinderIcon,
			core::rect<s32>(0, 0, gunIconSize.Width, gunIconSize.Height),
			core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1),
			core::position2d<s32>(cylinderCenterX, cylinderCenterY),
			player2CylinderRotation,
			core::vector2d<f32>(scale_factorX, scale_factorY)
			);
		/*
		driver->draw2DImage(triggerCylinderIcon,
			core::rect<s32>(gunCylinderPositionX1, gunCylinderPositionY1, gunCylinderPositionX2, gunCylinderPositionY2),
			core::rect<s32>(0, 0, gunIconSize.Width, gunIconSize.Height), 0, vertexColors, true);
		*/
		/* draw "bullets" */
		core::dimension2d<u32> iconSize = triggerIcon->getSize();
		s32 maxItems = fk_constants::FK_MaxTriggerCounterNumber - 1;
		for (s32 i = maxItems; i >= 0; --i){
			// calculate trigger position
			s32 startY, endY, startX1, endX1, startX2, endX2;
			calculateTriggerIconPosition(i, startY, endY, startX1, endX1, startX2, endX2);
			if (player1Triggers > maxItems - i){
				driver->draw2DImage(triggerIcon,
					core::rect<s32>(startX1, startY, endX1, endY),
					core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, vertexColors, true);
			}
			if (player2Triggers > maxItems - i){
				driver->draw2DImage(triggerIcon,
					core::rect<s32>(startX2, startY, endX2, endY),
					core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, vertexColors, true);
			}
		}
	}

	/* draw text */
	void FK_HUDManager::drawNormalText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
		video::SColor textColor, bool horizontalCentering, bool verticalCentering){
		font->draw(text.c_str(),
			destinationRect,
			textColor,
			horizontalCentering,
			verticalCentering);
	};

	/* draw "[PLAYER NAME] wins" text */
	void FK_HUDManager::drawWin(int winnerId, bool matchEndFlag){
		bool player1Won = winnerId == -1;
		if ((player1Won ? quoteToDrawPl1 : quoteToDrawPl2) == std::string()){
			(player1Won ? quoteToDrawPl1 : quoteToDrawPl2) = 
				(player1Won ? player1->getRandomWinQuote(player2->getName(), matchEndFlag) : 
				player2->getRandomWinQuote(player1->getName(), matchEndFlag));
		}
		// draw "WIN" statement
		core::dimension2d<u32> objSize = winText->getSize();
		driver->draw2DImage(winText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
		// draw name
		objSize = (player1Won ? player1NameText : player2NameText)->getSize();
		driver->draw2DImage(player1Won ? player1NameText : player2NameText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
		// draw winner sentence
		std::string quoteToDraw = player1Won ? quoteToDrawPl1 : quoteToDrawPl2;
		std::wstring temp(quoteToDraw.begin(), quoteToDraw.end());
		s32 sentenceWidth = textFont->getDimension(temp.c_str()).Width;
		s32 sentenceHeight = textFont->getDimension(temp.c_str()).Height;
		s32 sentenceOffset = 32;
		// draw text
		core::rect<s32> destinationRect(screenSize.Width / 2 - sentenceWidth,
			screenSize.Height - sentenceHeight - sentenceOffset * scale_factorY,
			screenSize.Width / 2 + sentenceWidth,
			screenSize.Height - sentenceOffset * scale_factorY);
		fk_addons::drawBorderedText(textFont, temp, destinationRect, 
			video::SColor(255, 255, 255, 255), 
			video::SColor(255, 0, 0, 0),
			true);
	}

	/* draw introduction sentence */
	void FK_HUDManager::drawIntroText(int playerId){
		bool player1IntroFlag = playerId == -1;
		if ((player1IntroFlag ? quoteToDrawPl1 : quoteToDrawPl2) == std::string()){
			(player1IntroFlag ? quoteToDrawPl1 : quoteToDrawPl2) =
				(player1IntroFlag ? player1->getRandomIntroQuote(player2->getName()) :
				player2->getRandomIntroQuote(player1->getName()));
		}
		// draw intro sentence
		std::string quoteToDraw = player1IntroFlag ? quoteToDrawPl1 : quoteToDrawPl2;
		std::wstring temp(quoteToDraw.begin(), quoteToDraw.end());
		s32 sentenceWidth = textFont->getDimension(temp.c_str()).Width;
		s32 sentenceHeight = textFont->getDimension(temp.c_str()).Height;
		s32 sentenceOffset = 32;
		// draw text
		core::rect<s32> destinationRect(screenSize.Width / 2 - sentenceWidth,
			screenSize.Height - sentenceHeight - sentenceOffset * scale_factorY,
			screenSize.Width / 2 + sentenceWidth,
			screenSize.Height - sentenceOffset * scale_factorY);
		fk_addons::drawBorderedText(textFont, temp, destinationRect, 
			video::SColor(255,255,255,255), 
			video::SColor(255, 0, 0, 0), true);
	}

	/* draw stage information */
	void FK_HUDManager::drawStageIntroText(FK_Stage* stage){
		// draw stage name
		std::string quoteToDraw = stage->getName();
		std::wstring temp(quoteToDraw.begin(), quoteToDraw.end());
		s32 sentenceWidth = font->getDimension(temp.c_str()).Width;
		s32 sentenceHeight = font->getDimension(temp.c_str()).Height;
		s32 sentenceOffsetX = 32;
		s32 sentenceOffsetY = 16;
		// draw stage name
		core::rect<s32> destinationRect(sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			sentenceWidth + sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		video::SColor textColor = video::SColor(255, 255, 255, 255);
		video::SColor borderColor = video::SColor(100, 0, 0, 0);
		fk_addons::drawBorderedText(font, temp, destinationRect, textColor, borderColor);
		// draw stage description
		quoteToDraw = stage->getDescription();
		std::wstring caption(quoteToDraw.begin(), quoteToDraw.end());
		s32 captionWidth = textFont->getDimension(caption.c_str()).Width;
		s32 captionHeight = textFont->getDimension(caption.c_str()).Height;
		s32 captionOffsetX = sentenceOffsetX;
		s32 captionOffsetY = sentenceOffsetY * scale_factorY + sentenceHeight;
		destinationRect = core::rect<s32>(captionOffsetX * scale_factorX,
			captionOffsetY,
			captionWidth + captionOffsetX * scale_factorX,
			captionHeight + captionOffsetY);
		// draw caption
		fk_addons::drawBorderedText(textFont, caption, destinationRect, textColor, borderColor);
	}

	/* draw "PAUSE" text */
	void FK_HUDManager::drawPause(){
		core::dimension2d<u32> objSize = pauseText->getSize();
		// draw centered "pause" item
		/*driver->draw2DImage(pauseText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);*/
		// draw pause sentence on top
		driver->draw2DImage(pauseText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 4 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 4 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "ROUND" text */
	void FK_HUDManager::drawRound(int currentRound, bool isFinalRound){
		if (isFinalRound){
			/* draw round text*/
			core::dimension2d<u32> objSize = finalRoundText->getSize();
			driver->draw2DImage(finalRoundText,
				core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
				screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
				screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
				screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
				core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
		}
		else{
			/* draw round text*/
			core::dimension2d<u32> objSize = roundText->getSize();
			driver->draw2DImage(roundText,
				core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
				screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
				screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
				screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
				core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
			/* draw round number */
			int adjustedRound = currentRound % 10;
			currentRound = currentRound / 10;
			core::dimension2d<u32> numSize = numberText[adjustedRound]->getSize();
			int unitNumberDistanceFromText = 64;
			if (currentRound > 0){
				unitNumberDistanceFromText = 75;
			}
			int number_X = screenSize.Width / 2 + unitNumberDistanceFromText*scale_factorX;
			driver->draw2DImage(numberText[adjustedRound],
				core::rect<s32>(number_X,
				screenSize.Height / 2 - numSize.Height / 2 * scale_factorY,
				number_X + numSize.Width * scale_factorX,
				screenSize.Height / 2 + numSize.Height / 2 * scale_factorY),
				core::rect<s32>(0, 0, numSize.Width, numSize.Height), 0, 0, true);
			if (currentRound > 0){
				unitNumberDistanceFromText = 42;
				number_X = screenSize.Width / 2 + unitNumberDistanceFromText*scale_factorX;
				driver->draw2DImage(numberText[currentRound],
					core::rect<s32>(number_X,
					screenSize.Height / 2 - numSize.Height / 2 * scale_factorY,
					number_X + numSize.Width * scale_factorX,
					screenSize.Height / 2 + numSize.Height / 2 * scale_factorY),
					core::rect<s32>(0, 0, numSize.Width, numSize.Height), 0, 0, true);
			}
		}
	}

	/* draw timer */
	void FK_HUDManager::drawTimer(u32 time){
		/* timer > 99 implies infinite time match */
		if (time > fk_constants::FK_MaxRoundTimerSeconds){
			return;
		}
		if (time < 0){
			time = 0;
		}
		u32 units = time % 10;
		u32 tens = time / 10;
		core::dimension2d<u32> numSize = numberText[units]->getSize();
		int startY = 0;
		int endY = startY + numSize.Height / 2;
		int startX = screenSize.Width / 2 - numSize.Width * scale_factorX / 8;
		driver->draw2DImage(numberText[units],
			core::rect<s32>(startX,
			startY * scale_factorY,
			startX + numSize.Width * scale_factorX / 2,
			endY * scale_factorY),
			core::rect<s32>(0, 0, numSize.Width, numSize.Height), 0, 0, true);
		startX = screenSize.Width / 2 - numSize.Width * 3 / 8 * scale_factorX;
		driver->draw2DImage(numberText[tens],
			core::rect<s32>(startX,
			startY * scale_factorY,
			startX + numSize.Width * scale_factorX / 2,
			endY * scale_factorY),
			core::rect<s32>(0, 0, numSize.Width, numSize.Height), 0, 0, true);
	}

	/* draw "FIGHT" text */
	void FK_HUDManager::drawFightText(){
		core::dimension2d<u32> objSize = fightText->getSize();
		driver->draw2DImage(fightText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "KO" text */
	void FK_HUDManager::drawKO(bool perfect){
		core::dimension2d<u32> objSize = (perfect ? perfectText : KOText)->getSize();
		driver->draw2DImage(perfect ? perfectText : KOText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "RING OUT" text */
	void FK_HUDManager::drawRingout(){
		core::dimension2d<u32> objSize = ringoutText->getSize();
		driver->draw2DImage(ringoutText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "DOUBLE RING OUT" text */
	void FK_HUDManager::drawDoubleRingout(){
		core::dimension2d<u32> objSize = doubleRingoutText->getSize();
		driver->draw2DImage(doubleRingoutText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "DRAW" text */
	void FK_HUDManager::drawDraw(){
		core::dimension2d<u32> objSize = (drawText)->getSize();
		driver->draw2DImage(drawText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "TIME UP" text */
	void FK_HUDManager::drawTimeUp(){
		core::dimension2d<u32> objSize = timeEndText->getSize();
		driver->draw2DImage(timeEndText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}

	/* draw "DOUBLE KO" text */
	void FK_HUDManager::drawDoubleKO(){
		core::dimension2d<u32> objSize = doubleKOText->getSize();
		driver->draw2DImage(doubleKOText,
			core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 - objSize.Height / 2 * scale_factorY,
			screenSize.Width / 2 + objSize.Width / 2 * scale_factorX,
			screenSize.Height / 2 + objSize.Height / 2 * scale_factorY),
			core::rect<s32>(0, 0, objSize.Width, objSize.Height), 0, 0, true);
	}


	// draw input buffer
	void FK_HUDManager::drawInputBuffer(FK_InputBuffer* buffer){
		int base_x = 32;
		int base_y = 24;
		int delta_y = 48;
		for (int i = 0; i < 10; i++){
			int button = buffer->getBufferElementFromHistory(i);
			if (button < 0)
				continue;
			//std::cout << "button:" << button << std::endl;
			base_x = 32;
			//std::cout << button << std::endl;
			if (button == 0){
				int x = base_x;
				drawIcon(inputMask_ico, base_x, base_y + i * delta_y);
				base_x = x;
			}
			if (button & FK_Input_Buttons::Up_Direction &&
				!(button & FK_Input_Buttons::Horizontal_Direction)){
				drawIcon(inputUp_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::Down_Direction &&
				!(button & FK_Input_Buttons::Horizontal_Direction)){
				drawIcon(inputDown_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::Right_Direction &&
				!(button & FK_Input_Buttons::Vertical_Direction)){
				drawIcon(inputRight_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::Left_Direction &&
				!(button & FK_Input_Buttons::Vertical_Direction)){
				drawIcon(inputLeft_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::DownLeft_Direction &&
				!(button & FK_Input_Buttons::UpRight_Direction)){
				drawIcon(inputDL_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::DownRight_Direction  &&
				!(button & FK_Input_Buttons::Up_Direction)){
				drawIcon(inputDR_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::UpRight_Direction &&
				!(button & FK_Input_Buttons::Left_Direction)){
				drawIcon(inputUR_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::UpLeft_Direction){
				drawIcon(inputUL_ico, base_x, base_y + i * delta_y);
			}
			/* start with held directions*/
			else if (button & FK_Input_Buttons::HeldUp_Direction &&
				!(button & FK_Input_Buttons::Horizontal_Direction)){
				drawIcon(inputUpH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldDown_Direction &&
				!(button & FK_Input_Buttons::Horizontal_Direction)){
				drawIcon(inputDownH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldRight_Direction &&
				!(button & FK_Input_Buttons::Vertical_Direction)){
				drawIcon(inputRightH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldLeft_Direction &&
				!(button & FK_Input_Buttons::Vertical_Direction)){
				drawIcon(inputLeftH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldDownLeft_Direction &&
				!(button & FK_Input_Buttons::HeldUpRight_Direction)){
				drawIcon(inputDLH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldDownRight_Direction  &&
				!(button & FK_Input_Buttons::HeldUp_Direction)){
				drawIcon(inputDRH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldUpRight_Direction &&
				!(button & FK_Input_Buttons::HeldLeft_Direction)){
				drawIcon(inputURH_ico, base_x, base_y + i * delta_y);
			}
			else if (button & FK_Input_Buttons::HeldUpLeft_Direction)
				drawIcon(inputULH_ico, base_x, base_y + i * delta_y);


			if (button & FK_Input_Buttons::Any_NonDirectionButton){
				int x = base_x;
				drawIcon(inputMask_ico, base_x, base_y + i * delta_y);
				base_x = x;
			}
			if (button & FK_Input_Buttons::Guard_Button){
				int x = base_x;
				drawIcon(inputA_ico, base_x, base_y + i * delta_y);
				base_x = x;
			}
			if (button & FK_Input_Buttons::Tech_Button){
				int x = base_x;
				drawIcon(inputB_ico, base_x, base_y + i * delta_y);
				base_x = x;
			}
			if (button & FK_Input_Buttons::Kick_Button){
				int x = base_x;
				drawIcon(inputX_ico, base_x, base_y + i * delta_y);
				base_x = x;
			}
			if (button & FK_Input_Buttons::Punch_Button){
				int x = base_x;
				drawIcon(inputY_ico, base_x, base_y + i * delta_y);
				base_x = x;
			}
		}
	};

	// draw single icon
	void FK_HUDManager::drawIcon(video::ITexture* icon, int& x, int y){
		if (!icon){
			return;
		}
		// scale viewport
		float scaleX = scale_factorX;
		float scaleY = scale_factorY;
		core::dimension2d<u32> iconSize = icon->getSize();
		driver->draw2DImage(icon, core::rect<s32>(x*scaleX, y*scaleY, (x + iconSize.Width)*scaleX, (y + iconSize.Height)*scaleY),
			core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, 0, true);
		x += iconSize.Width*scaleX;
	};

	//set render screen size
	void FK_HUDManager::setViewportSize(core::dimension2d<u32> new_size){
		screenSize = core::dimension2d<u32>(new_size.Width, new_size.Height);
		// scale viewport
		scale_factorX = screenSize.Width / (f32)fk_constants::FK_DefaultResolution.Width;
		scale_factorY = screenSize.Height / (f32)fk_constants::FK_DefaultResolution.Height;
	}
};