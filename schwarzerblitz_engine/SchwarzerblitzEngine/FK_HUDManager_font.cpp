#include"FK_HUDManager_font.h"
#include"ExternalAddons.h"
#include<algorithm>
#include<iostream>
#undef max
#undef min

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

namespace fk_engine{

	// additional struct
	// update
	void FK_HUDManager_font::FK_HUD_Hitflag::update(u32 frameDeltaT_MS){
		timerMS += frameDeltaT_MS;
	}
	// check if expired
	bool FK_HUDManager_font::FK_HUD_Hitflag::isExpired(){
		return timerMS >= (fadeInTimeMS + fadeoutTimeMS + lifetimeMS);
	}
	// get opacity
	s32 FK_HUDManager_font::FK_HUD_Hitflag::getOpacity(){
		if (timerMS > (fadeInTimeMS + lifetimeMS)){
			return (s32)std::round(256.0f - (f32)((timerMS - (fadeInTimeMS + lifetimeMS)) * 255.0f / (f32)(fadeoutTimeMS)));
		}
		else if (timerMS > fadeInTimeMS){
			return 255;
		}
		else{
			return (s32)std::round(1 + (f32)((timerMS - (fadeInTimeMS + lifetimeMS)) * 255.0f / (f32)(fadeoutTimeMS)));
		}
	}

	// main methods
	FK_HUDManager_font::FK_HUDManager_font(IrrlichtDevice *new_device,
		FK_DatabaseAccessor accessor,
		std::string mediaPath,
		FK_Character* new_player1, FK_Character* new_player2){
		device = new_device;
		driver = device->getVideoDriver();
		databaseAccessor = accessor;
		maxTriggerCounters = accessor.getMaxTriggers();
		// get player paths
		std::string player1Path = new_player1->getCharacterDirectory();
		std::string player2Path = new_player2->getCharacterDirectory();
		player1 = new_player1;
		player2 = new_player2;
		// get screen size and scaling factor
		screenSize = driver->getScreenSize();
		font = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		mainFont = device->getGUIEnvironment()->getFont(fk_constants::FK_BattleFontIdentifier.c_str());
		textFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());
		timerFont = device->getGUIEnvironment()->getFont(fk_constants::FK_TimerFontIdentifier.c_str());
		// reset the quote variable
		quoteToDrawPl1 = std::wstring();
		quoteToDrawPl2 = std::wstring();
		// internal variables
		oldLifeRatioPlayer1 = 1.0;
		oldLifeRatioPlayer2 = 1.0;
		oldComboCounterPlayer1 = 0;
		oldComboCounterPlayer2 = 0;
		player1OldBulletCounterNumber = player1->getTriggerCounters();
		player2OldBulletCounterNumber = player2->getTriggerCounters();
		//bool player1TriggerStatus = false;
		//bool player2TriggerStatus = false;
		player1TriggerRotationDuration = 0;
		player2TriggerRotationDuration = 0;
		player1TriggerRotationCounter = 0;
		player2TriggerRotationCounter = 0;
		player1CylinderRotation = 0.0f;
		player2CylinderRotation = 0.0f;
		player1TriggerHighlightTimeMs = 0;
		player2TriggerHighlightTimeMs = 0;
		std::string characterPortrait1 = player1Path + player1->getOutfitPath() + "portrait.png";
		std::string characterPortrait2 = player2Path + player2->getOutfitPath() + "portrait.png";
		player1Portrait = driver->getTexture(characterPortrait1.c_str());
		if (!player1Portrait){
			player1Portrait = driver->getTexture((player1Path + "portrait0.png").c_str());
		}
		player2Portrait = driver->getTexture(characterPortrait2.c_str());
		if (!player2Portrait){
			player2Portrait = driver->getTexture((player2Path + "portrait0.png").c_str());
		}
		setupHUDTextStrings();
		loadInputBufferTextures(mediaPath + "movelist\\");
		loadHitflagTextures(mediaPath + "movelist\\");
		loadSystemIcons(mediaPath + "movelist\\");
		// create dialogue back textures
		// in-match dialgoue
		dialogueTextBack = driver->getTexture((mediaPath + "hud\\textBack.png").c_str());
		dialogueTextBackLBorder = driver->getTexture((mediaPath + "hud\\textBackL.png").c_str());
		dialogueTextBackRBorder = driver->getTexture((mediaPath + "hud\\textBackR.png").c_str());
		// current move variables
		currentMoveNameTimer = 0;
		currentMoveNameDuration = 3000;
		currentMoveNameFadeoutDuration = 300;
		currentMoveName = std::string();
		currentMove = FK_Move();
		currentMoveClearBuffer = true;
		roundIndicatorBlinkTime = 0.f;
		moveNameDisplayPreviousMoveWasAThrow = false;
		createDefaultResourceParameters();
		loadConfigurationFile(mediaPath);
		loadDefaultAssets(mediaPath);
		player1ButtonInputLayout.clear();
		player1KeyboardKeyLayout.clear();
		player1UsesJoypad = false;
		attractModeTimeCounter = 0;
	};

	void FK_HUDManager_font::createDefaultResourceParameters(){
		// setup resource variables
		referenceScreenSize = core::dimension2df((f32)fk_constants::FK_DefaultResolution.Width,
			(f32)fk_constants::FK_DefaultResolution.Height);
		scale_factorX = screenSize.Width / referenceScreenSize.Width;
		scale_factorY = screenSize.Height / referenceScreenSize.Height;
		triggerAnimationSize = core::dimension2d<u32>(4, 1);
		lifebarCasePosition = core::rect<s32>(0, 0, 320, 48);
		lifebarPosition = core::rect<s32>(27, 9, 213, 9);
		portraitPosition = core::rect<s32>(0, 0, 80, 48);
		roundIndicatorPosition = core::rect<s32>(10, 42, 16, 16);
		comboTextOffset = core::position2di(24, 20);
		playerNameOffset = core::position2di(32, 20);
		triggerCounterPosition = core::rect<s32>(10, 51, 12, 12);
		triggerCylinderPosition = core::rect<s32>(28, 16, 48, 48);
		timerVerticalPosition = 7;
		showPlayerPortraits = true;
		showPlayerNames = true;
		showLifebarCases = true;
		showGunCylinder = true;
		centerPlayerNames = false;
		showBlinkingRoundIndicatorPredictor = true;
		showEmptyRoundIndicators = true;
		drawTriggersFromBorderToCenter = false;

		showComboNumber = true;
		showComboDamage = true;
		minimalComboHitNumberToShow = 2;

		damageName = std::pair<std::wstring, std::wstring>(L"damage", L"damage");
		hitName = std::pair<std::wstring, std::wstring>(L"hit", L"hits");

		showVanishingDamage = true;
		vanishingDamageWaitForHitstun = true;
		vanishingLifebarReductionPerSecond = 1.0f;

		// create default colors
		video::SColor const standardGreenColor = video::SColor(255, 25, 240, 25);
		video::SColor const standardBlueColor = video::SColor(255, 25, 180, 255);
		//video::SColor const standardGreenColor = video::SColor(255, 225, 180, 25);
		video::SColor const darkerGreenColor = video::SColor(255, 25, 180, 25);
		video::SColor const standardPurpleColor = video::SColor(255, 255, 80, 255);
		video::SColor const fullPurpleColor = video::SColor(255, 255, 0, 255);
		video::SColor const fadingPurpleColor = video::SColor(255, 200, 100, 255);
		//video::SColor const darkerGreenColor = video::SColor(255, 255, 10, 25);
		video::SColor const yellowColor = video::SColor(255, 255, 255, 25);
		//video::SColor const yellowColor = video::SColor(255, 255, 185, 25);
		video::SColor const redColor = video::SColor(255, 255, 100, 25);
		video::SColor const standardGrayColor = video::SColor(255, 200, 200, 200);
		video::SColor const darkerGrayColor = video::SColor(255, 175, 175, 175);
		video::SColor const higherLifeBlueColor = video::SColor(255, 25, 240, 225);
		video::SColor const higherLifeDarkBlueColor = video::SColor(255, 25, 180, 165);
		video::SColor const fadingBlackBackground = video::SColor(128, 20, 0, 0);

		highLifeColor = standardBlueColor;
		lowLifeColor = standardPurpleColor;
		highVanishingLifeColor = yellowColor;
		lowVanishingLifeColor = redColor;
		lifebarBackgroundColor = fadingBlackBackground;

		triggerRecoveryBarBackgroundColor = video::SColor(128, 0, 0, 0);
		triggerRecoveryBarBorderColor = video::SColor(255, 192, 192, 192);
		triggerRecoveryBarColor1 = video::SColor(255, 0, 196, 196);
		triggerRecoveryBarColor2 = video::SColor(255, 0, 120, 180);
		triggerRecoveryBarSamePositionAsTriggers = true;
		triggerRecoveryBarPosition = core::rect<s32>(0, 0, 0, 0);
		triggerRecoveryBarFlashWhenMax = false;
		triggerRecoveryBarFlashColorAddon = video::SColor(0, 0, 0, 0);
		drawSectorizedRecoveryBar = false;

		showTriggerCounters = true;
		showTriggerNumber = false;
		triggerNumberPosition = core::position2di(0, 0);

		showHUD = true;
		showStageName = true;
		showStageDescription = true;

		showStageBGMName = false;
		showStageBGMAuthor = false;

		trainingInputMaxBufferSize = 20;
		trainingInputCoordinates = core::rect<f32>(10.f, 72.f, 18.8f, 20.7f);

		recordInputText[TrainingRecordModePhase::Record] = L"Recording...";
		recordInputText[TrainingRecordModePhase::Replay] = L"Replay";
		recordInputText[TrainingRecordModePhase::Standby] = L"Waiting";
		recordInputText[TrainingRecordModePhase::Control] = L"Control opponent";
		recordTimeBarLocation = core::rect<s32>(220, 60, 200, 8);

		showComboMaxDamage = false;

		recordAndReplayPhaseCounter = 0;

		recordAndReplayDummyPhaseText[0] = L"control dummy: ";
		recordAndReplayDummyPhaseText[1] = L"replay: ";

		recordAndReplayControlPhaseText[0] = L"record dummy: ";
		recordAndReplayControlPhaseText[1] = L"replay: ";
		recordAndReplayControlPhaseText[2] = L"cancel: ";

		recordAndReplayRecordPhaseText[0] = L"save replay: ";
		recordAndReplayRecordPhaseText[1] = L"cancel: ";

		recordAndReplayReplayPhaseText[0] = L"cancel: ";
		recordAndReplayReplayPhaseText[1] = L"cancel: ";
		recordAndReplayReplayPhaseText[2] = L"cancel: ";

		recordAndReplayDummyPhaseInput[0] = FK_Input_Buttons::Selection_Button;
		recordAndReplayDummyPhaseInput[1] = FK_Input_Buttons::Modifier_Button;

		recordAndReplayControlPhaseInput[0] = FK_Input_Buttons::Selection_Button;
		recordAndReplayControlPhaseInput[1] = FK_Input_Buttons::Modifier_Button;
		recordAndReplayControlPhaseInput[2] = FK_Input_Buttons::Cancel_Button;

		recordAndReplayRecordPhaseInput[0] = FK_Input_Buttons::Selection_Button;
		recordAndReplayRecordPhaseInput[1] = FK_Input_Buttons::Cancel_Button;

		recordAndReplayReplayPhaseInput[0] = FK_Input_Buttons::Cancel_Button;
		recordAndReplayReplayPhaseInput[1] = FK_Input_Buttons::Selection_Button;
		recordAndReplayReplayPhaseInput[2] = FK_Input_Buttons::Modifier_Button;

		recordAndReplayTimeCounterS = 0;
	}

	// load default assets
	void FK_HUDManager_font::loadDefaultAssets(std::string mediaPath){
		// create lifebar textures
		std::string resourcePath = mediaPath + FK_HUDManager_font::HUD_Media_Directory;
		if (lifebar1 == NULL){
			lifebar1 = driver->getTexture((resourcePath + "bar1_portrait.png").c_str());
		}
		/*if (lifebar1_bw == NULL){
			lifebar1_bw = driver->getTexture((resourcePath + "bar1_portrait_bw.png").c_str());
		}*/
		if (lifebar1sub_bw == NULL){
			lifebar1sub_bw = driver->getTexture((resourcePath + "lbar1_bw_light.png").c_str());
		}
		if (lifebar2sub_bw == NULL){
			lifebar2sub_bw = driver->getTexture((resourcePath + "lbar2_bw_light.png").c_str());
		}
		if (roundIndicatorEmpty == NULL){
			roundIndicatorEmpty = driver->getTexture((resourcePath + "roundDiamond.png").c_str());
		}
		if (roundIndicatorFull == NULL){
			roundIndicatorFull = driver->getTexture((resourcePath + "roundDiamondFull.png").c_str());
		}
		/*if (roundIndicatorFull_bw == NULL){
			roundIndicatorFull_bw = driver->getTexture((resourcePath + "roundDiamondFull_bw.png").c_str());
		}*/
		// trigger icons
		if (triggerIcon == NULL){
			triggerIcon = driver->getTexture((resourcePath + "triggerCounter_icon2.png").c_str());
		}
		if (triggerCylinderIcon == NULL){
			triggerCylinderIcon = driver->getTexture((resourcePath + "triggerGun2.png").c_str());
		}
		if (triggerAnimationTexture == NULL){
			triggerAnimationTexture = driver->getTexture((resourcePath + "triggerAnimation.png").c_str());
			triggerAnimationCellSize =
				core::dimension2d<u32>(triggerAnimationTexture->getSize().Width / triggerAnimationSize.Width, 
				triggerAnimationTexture->getSize().Height / triggerAnimationSize.Height);
		}

		if (recordButtonTex == NULL) {
			recordButtonTex = driver->getTexture((resourcePath + "record.png").c_str());
		}
		if (stopButtonTex == NULL) {
			stopButtonTex = driver->getTexture((resourcePath + "stop.png").c_str());
		}
		if (playButtonTex == NULL) {
			playButtonTex = driver->getTexture((resourcePath + "play.png").c_str());
		}
	}

	// load variables from file
	bool FK_HUDManager_font::loadConfigurationFile(std::string mediaPath){
		std::string filename = mediaPath + FK_HUDManager_font::HUD_Media_Directory + "config.txt";
		std::ifstream inputFile(filename.c_str());
		if (!inputFile){
			return false;
		}
		// create keys
		std::map<FK_HUD_ConfigFileKeys, std::string> fileKeys;
		fileKeys[HighLifeColorTag] = "#high_life_color";
		fileKeys[LowLifeColorTag] = "#low_life_color";
		fileKeys[LifebarBackgroundColor] = "#life_background_color";
		fileKeys[VanishingHighLifeColorTag] = "#high_lost_life_color";
		fileKeys[VanishingLowLifeColorTag] = "#low_lost_life_color";
		fileKeys[LifebarCasePosition] = "#lifebar_case_position";
		fileKeys[LifebarPosition] = "#lifebar_position";
		fileKeys[LifebarCaseTexture] = "#lifebar_case_texture";
		fileKeys[LifebarTextures] = "#lifebar_texture";
		fileKeys[LifebarCaseBWTexture] = "#lifebar_case_BW_texture";
		fileKeys[ComboDamageTextPosition] = "#combo_damage_text_position";
		fileKeys[RoundIndicatorTexture] = "#round_indicator_texture";
		fileKeys[RoundIndicatorPosition] = "#round_indicator_position";
		fileKeys[PlayerNameOffset] = "#player_name_offset";
		fileKeys[ShowGunCylinder] = "#show_trigger_gun_cylinder";
		fileKeys[TriggerCountersPosition] = "#trigger_counters_position";
		fileKeys[TriggerCountersTexture] = "#trigger_counters_texture";
		fileKeys[TriggerCountersUseAnimation] = "#trigger_counters_use_animation_texture";
		fileKeys[TriggerCounterGunCylinderTexture] = "#trigger_counters_gun_cylinder_texture";
		fileKeys[TriggerCounterGunCylinderPosition] = "#trigger_counters_gun_cylinder_position";
		fileKeys[ShowPortraits] = "#show_portraits";
		fileKeys[ShowNames] = "#show_names";
		fileKeys[ShowLifebarCases] = "#show_lifebar_case";
		fileKeys[CenterPlayerNamesOnLifeBars] = "#center_names_on_lifebar";
		fileKeys[PortraitPosition] = "#portrait_position";
		fileKeys[ReferenceScreenSize] = "#reference_screen_size";
		fileKeys[ShowBlinkingRoundWinPrediction] = "#show_life_lead_indicator";
		fileKeys[ShowEmptyRoundIndicators] = "#show_empty_round_indicators";
		fileKeys[DrawTriggerCountersFromBorder] = "#draw_trigger_counters_from_border";
		fileKeys[TimerVerticalOffset] = "#timer_vertical_position";
		fileKeys[ShowComboCounter] = "#show_combo_hit_number";
		fileKeys[ShowComboDamage] = "#show_combo_damage";
		fileKeys[MinimalHitNumberToShowDamage] = "#show_combo_text_after_N_hits";
		fileKeys[HitName] = "#hit_name";
		fileKeys[DamageName] = "#damage_name";
		fileKeys[ShowVanishingLife] = "#show_vanishing_lifebar";
		fileKeys[VanishingLifeWaitForHitstun] = "#pause_vanishing_life_on_hitstun";
		fileKeys[VanishingRatioPerSecond] = "#vanishing_life_ratio_per_second";
		fileKeys[TriggerRecoveryBarBackgroundColor] = "#trigger_regen_bar_background_color";
		fileKeys[TriggerRecoveryBarBorderColor] = "#trigger_regen_bar_border_color";
		fileKeys[TriggerRecoveryBarColorGradient] = "#trigger_regen_bar_gradient_colors";
		fileKeys[TriggerRecoveryBarSamePositionAsTriggers] = "#trigger_bar_same_position_as_triggers";
		fileKeys[TriggerRecoveryBarPosition] = "#trigger_recovery_bar_position";
		fileKeys[TriggerRecoveryBarFlashWhenMax] = "#trigger_recovery_bar_flash_max";
		fileKeys[ShowTriggerCounters] = "#show_trigger_counters";
		fileKeys[ShowSectorizedTriggerBar] = "#show_sectorized_trigger_bar";
		fileKeys[ShowTriggerNumber] = "#show_trigger_number";
		fileKeys[TriggerNumberPosition] = "#trigger_number_position";
		fileKeys[ShowHUD] = "#show_hud";
		fileKeys[ShowStageName] = "#show_stage_name";
		fileKeys[ShowStageDescription] = "#show_stage_description";
		fileKeys[ShowStageBGMName] = "#show_stage_bgm_name";
		fileKeys[ShowStageBGMAuthor] = "#show_stage_bgm_author";
		fileKeys[TrainingInputCoordinates] = "#training_input_coordinates";
		fileKeys[TrainingInputMaxSize] = "#training_input_max_buffer_size";

		std::string resourcePath = mediaPath + FK_HUDManager_font::HUD_Media_Directory;
		std::string temp;
		while (inputFile >> temp){
			if (!inputFile){
				break;
			}
			if (temp == fileKeys[ShowPortraits]){
				s32 flag;
				inputFile >> flag;
				showPlayerPortraits = flag > 0;
			}
			else if (temp == fileKeys[ShowHUD]) {
				s32 flag;
				inputFile >> flag;
				showHUD = flag > 0;
			}
			else if (temp == fileKeys[ShowStageName]) {
				s32 flag;
				inputFile >> flag;
				showStageName = flag > 0;
			}
			else if (temp == fileKeys[ShowStageDescription]) {
				s32 flag;
				inputFile >> flag;
				showStageDescription = flag > 0;
			}
			else if (temp == fileKeys[ShowStageBGMName]) {
				s32 flag;
				inputFile >> flag;
				showStageBGMName = flag > 0;
			}
			else if (temp == fileKeys[ShowStageBGMAuthor]) {
				s32 flag;
				inputFile >> flag;
				showStageBGMAuthor = flag > 0;
			}
			else if (temp == fileKeys[ShowNames]){
				s32 flag;
				inputFile >> flag;
				showPlayerNames = flag > 0;
			}
			else if (temp == fileKeys[ShowLifebarCases]){
				s32 flag;
				inputFile >> flag;
				showLifebarCases = flag > 0;
			}
			else if (temp == fileKeys[ShowGunCylinder]){
				s32 flag;
				inputFile >> flag;
				showGunCylinder = flag > 0;
			}
			else if (temp == fileKeys[ShowEmptyRoundIndicators]){
				s32 flag;
				inputFile >> flag;
				showEmptyRoundIndicators = flag > 0;
			}
			else if (temp == fileKeys[ShowBlinkingRoundWinPrediction]){
				s32 flag;
				inputFile >> flag;
				showBlinkingRoundIndicatorPredictor = flag > 0;
			}
			else if (temp == fileKeys[CenterPlayerNamesOnLifeBars]){
				s32 flag;
				inputFile >> flag;
				centerPlayerNames = flag > 0;
			}
			else if (temp == fileKeys[DrawTriggerCountersFromBorder]){
				s32 flag;
				inputFile >> flag;
				drawTriggersFromBorderToCenter = flag > 0;
			}
			else if (temp == fileKeys[HighLifeColorTag]){
				s32 a, r, g, b;
				inputFile >> a >> r>> g >> b;
				highLifeColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[LowLifeColorTag]){
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				lowLifeColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[LifebarBackgroundColor]){
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				lifebarBackgroundColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[VanishingHighLifeColorTag]){
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				highVanishingLifeColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[VanishingLowLifeColorTag]){
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				lowVanishingLifeColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[LifebarCasePosition]){
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				lifebarCasePosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[LifebarPosition]){
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				lifebarPosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[TriggerCounterGunCylinderPosition]){
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				triggerCylinderPosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[RoundIndicatorPosition]){
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				roundIndicatorPosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[TriggerCountersPosition]){
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				triggerCounterPosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[PortraitPosition]){
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				portraitPosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[PlayerNameOffset]){
				s32 x, y;
				inputFile >> x >> y;
				playerNameOffset = core::dimension2di(x, y);
			}
			else if (temp == fileKeys[TimerVerticalOffset]){
				f32 y;
				inputFile >> y;
				timerVerticalPosition = y;
			}
			else if (temp == fileKeys[ComboDamageTextPosition]){
				s32 x, y;
				inputFile >> x >> y;
				comboTextOffset = core::dimension2di(x, y);
			}
			else if (temp == fileKeys[ReferenceScreenSize]){
				f32 w, h;
				inputFile >> w >> h;
				referenceScreenSize = core::dimension2df(w, h);
			}
			else if (temp == fileKeys[LifebarCaseTexture]){
				inputFile >> temp;
				lifebar1 = driver->getTexture((resourcePath + temp).c_str());
			}
			else if (temp == fileKeys[LifebarTextures]){
				inputFile >> temp;
				lifebar1sub_bw = driver->getTexture((resourcePath + temp).c_str());
				inputFile >> temp;
				lifebar2sub_bw = driver->getTexture((resourcePath + temp).c_str());
			}
			else if (temp == fileKeys[TriggerCountersTexture]){
				inputFile >> temp;
				triggerIcon = driver->getTexture((resourcePath + temp).c_str());
			}
			else if (temp == fileKeys[TriggerCounterGunCylinderTexture]){
				inputFile >> temp;
				triggerCylinderIcon = driver->getTexture((resourcePath + temp).c_str());
			}
			else if (temp == fileKeys[RoundIndicatorTexture]){
				inputFile >> temp;
				roundIndicatorEmpty = driver->getTexture((resourcePath + temp).c_str());
				inputFile >> temp;
				roundIndicatorFull = driver->getTexture((resourcePath + temp).c_str());
			}
			else if (temp == fileKeys[ShowComboCounter]) {
				s32 flag;
				inputFile >> flag;
				showComboNumber = flag > 0;
			}
			else if (temp == fileKeys[ShowComboDamage]) {
				s32 flag;
				inputFile >> flag;
				showComboDamage = flag > 0;
			}
			else if (temp == fileKeys[MinimalHitNumberToShowDamage]) {
				u32 flag;
				inputFile >> flag;
				minimalComboHitNumberToShow = flag;
			}
			else if (temp == fileKeys[HitName]) {
				inputFile >> temp;
				hitName.first = std::wstring(temp.begin(), temp.end());
				inputFile >> temp;
				hitName.second = std::wstring(temp.begin(), temp.end());
			}
			else if (temp == fileKeys[DamageName]) {
				inputFile >> temp;
				damageName.first = std::wstring(temp.begin(), temp.end());
				inputFile >> temp;
				damageName.second = std::wstring(temp.begin(), temp.end());
			}
			else if (temp == fileKeys[ShowVanishingLife]) {
				s32 flag;
				inputFile >> flag;
				showVanishingDamage = flag > 0;
			}
			else if (temp == fileKeys[VanishingLifeWaitForHitstun]) {
				s32 flag;
				inputFile >> flag;
				vanishingDamageWaitForHitstun = flag > 0;
			}
			else if (temp == fileKeys[VanishingRatioPerSecond]) {
				f32 flag;
				inputFile >> flag;
				vanishingLifebarReductionPerSecond = flag;
			}
			else if (temp == fileKeys[TriggerRecoveryBarBorderColor]) {
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				triggerRecoveryBarBorderColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[TriggerRecoveryBarBackgroundColor]) {
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				triggerRecoveryBarBackgroundColor = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[TriggerRecoveryBarColorGradient]) {
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				triggerRecoveryBarColor1 = video::SColor(a, r, g, b);
				inputFile >> a >> r >> g >> b;
				triggerRecoveryBarColor2 = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[TriggerRecoveryBarSamePositionAsTriggers]) {
				s32 flag;
				inputFile >> flag;
				triggerRecoveryBarSamePositionAsTriggers = flag > 0;
			}
			else if (temp == fileKeys[TriggerRecoveryBarPosition]) {
				s32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				triggerRecoveryBarPosition = core::rect<s32>(x, y, w, h);
			}
			else if (temp == fileKeys[TriggerRecoveryBarFlashWhenMax]) {
				s32 flag;
				inputFile >> flag;
				triggerRecoveryBarFlashWhenMax = flag > 0;
				s32 a, r, g, b;
				inputFile >> a >> r >> g >> b;
				triggerRecoveryBarFlashColorAddon = video::SColor(a, r, g, b);
			}
			else if (temp == fileKeys[ShowSectorizedTriggerBar]) {
				s32 flag;
				inputFile >> flag;
				drawSectorizedRecoveryBar = flag > 0;
			}
			else if (temp == fileKeys[ShowTriggerCounters]) {
				s32 flag;
				inputFile >> flag;
				showTriggerCounters = flag > 0;
			}
			else if (temp == fileKeys[ShowTriggerNumber]) {
				s32 flag;
				inputFile >> flag;
				showTriggerNumber = flag > 0;
			}
			else if (temp == fileKeys[TriggerNumberPosition]) {
				s32 x, y;
				inputFile >> x >> y;
				triggerNumberPosition = core::position2di(x, y);
			}
			else if (temp == fileKeys[TrainingInputCoordinates]) {
				f32 x, y, w, h;
				inputFile >> x >> y >> w >> h;
				trainingInputCoordinates = core::rect<f32>(x, y, w, h);
			}
			else if (temp == fileKeys[TrainingInputMaxSize]) {
				s32 flag;
				inputFile >> flag;
				trainingInputMaxBufferSize = flag;
				if (trainingInputMaxBufferSize < 0) {
					trainingInputMaxBufferSize = 0;
				}
			}
		}

		return true;
	}

	// load input buffer textures
	void FK_HUDManager_font::loadInputBufferTextures(std::string resourcePath){
		// button textures
		buttonTexturesMap[FK_Input_Buttons::Kick_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "kick_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Punch_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "punch_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Guard_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "guard_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Trigger_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "trigger_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Tech_Button] =
			device->getVideoDriver()->getTexture((resourcePath + "tech_button.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Up_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_up.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldUp_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::UpRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upright.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldUpRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_uprightH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Right_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_right.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_rightH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::DownRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downright.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldDownRight_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downrightH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Down_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_down.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldDown_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::DownLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downleft.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldDownLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_downleftH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::Left_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_left.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_leftH.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::UpLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upleft.png").c_str());
		buttonTexturesMap[FK_Input_Buttons::HeldUpLeft_Direction] =
			device->getVideoDriver()->getTexture((resourcePath + "direction_upleftH.png").c_str());
		plusSignTex = device->getVideoDriver()->getTexture((resourcePath + "plus_sign.png").c_str());
		triggerInputTex = device->getVideoDriver()->getTexture((resourcePath + "trigger_action.png").c_str());
		triggerInputTexLborder = device->getVideoDriver()->getTexture((resourcePath + "trigger_action_border_L.png").c_str());
		triggerInputTexRborder = device->getVideoDriver()->getTexture((resourcePath + "trigger_action_border_R.png").c_str());
		triggerInputTexSingle = device->getVideoDriver()->getTexture((resourcePath + "trigger_action_single.png").c_str());
	}

	// setup HUD text
	void FK_HUDManager_font::setupHUDTextStrings(){
		hudStrings[FK_HUD_Strings::ContinueText] = L"Continue?";
		hudStrings[FK_HUD_Strings::PauseText] = L"Pause";
		hudStrings[FK_HUD_Strings::RoundText] = L"Round";
		hudStrings[FK_HUD_Strings::PlayerWinText] = L"wins";
		hudStrings[FK_HUD_Strings::FinalText] = L"Final";
		hudStrings[FK_HUD_Strings::RingoutText] = L"Ring Out";
		hudStrings[FK_HUD_Strings::TimeUpText] = L"Time up";
		hudStrings[FK_HUD_Strings::KOText] = L"K. O.";
		hudStrings[FK_HUD_Strings::FightText] = L"Fight";
		hudStrings[FK_HUD_Strings::PerfectText] = L"Perfect";
		hudStrings[FK_HUD_Strings::DoubleText] = L"Double";
		hudStrings[FK_HUD_Strings::DrawResultText] = L"Draw";
		hudStrings[FK_HUD_Strings::SuccessText] = L"Success";
	}

	// load hitflag textures
	void FK_HUDManager_font::loadHitflagTextures(std::string texturePath){
		hitflags.clear();
		hitflagsTextures.clear();
		hitflagsTextures[FK_Attack_Type::MidThrowAtk] = driver->getTexture((texturePath + "throw.png").c_str());
		hitflagsTextures[FK_Attack_Type::LowThrowAtk] = driver->getTexture((texturePath + "throw.png").c_str());
		hitflagsTextures[FK_Attack_Type::HighThrowAtk] = driver->getTexture((texturePath + "throw.png").c_str());
		hitflagsTextures[FK_Attack_Type::HighAtk] = driver->getTexture((texturePath + "high.png").c_str());
		hitflagsTextures[FK_Attack_Type::MediumAtk] = driver->getTexture((texturePath + "mid.png").c_str());
		hitflagsTextures[FK_Attack_Type::LowAtk] = driver->getTexture((texturePath + "low.png").c_str());
		hitflagsTextures[FK_Attack_Type::HighBlockableThrow] = driver->getTexture((texturePath + "high.png").c_str());
		hitflagsTextures[FK_Attack_Type::MidBlockableThrow] = driver->getTexture((texturePath + "mid.png").c_str());
		hitflagsTextures[FK_Attack_Type::LowBlockableThrow] = driver->getTexture((texturePath + "low.png").c_str());
	}

	//draw HUD given the life ratio
	void FK_HUDManager_font::drawHUD(u32 numberOfIndicatorsPl1, u32 numberOfIndicatorsPl2, u32 player1winNumber, u32 player2winNumber,
		s32 roundTimeS, bool pause, f32 frameDeltaTime_s){
		if (!showHUD) {
			return;
		}
		/* reset "quote to draw" for intros and such */
		quoteToDrawPl1 = std::wstring();
		quoteToDrawPl2 = std::wstring();
		/* draw trigger counters*/
		f32 player1lifeRatio = player1->getLifeRatio();
		f32 player2lifeRatio = player2->getLifeRatio();
		u32 player1Triggers = player1->getTriggerCounters();
		u32 player2Triggers = player2->getTriggerCounters();
		/* update old ratio variables */
		oldLifeRatioPlayer1 = std::max(oldLifeRatioPlayer1, player1lifeRatio);
		oldLifeRatioPlayer2 = std::max(oldLifeRatioPlayer2, player2lifeRatio);
		/* reduce oldLifeRatio by a set amount per second */
		if ((!vanishingDamageWaitForHitstun || !player1->isHitStun()) || 
			player1->isKO() || (player1->isHitStun() && player1->getVelocityPerSecond().Z != 0)){
			oldLifeRatioPlayer1 -= vanishingLifebarReductionPerSecond * frameDeltaTime_s;
		}
		if ((!vanishingDamageWaitForHitstun || !player2->isHitStun()) ||
			player2->isKO() || (player2->isHitStun() && player2->getVelocityPerSecond().Z != 0)){
			oldLifeRatioPlayer2 -= vanishingLifebarReductionPerSecond * frameDeltaTime_s;
		}
		/* update trigger indicators */
		u32 triggerRotationDurationMs = 400;
		if (player1->getTriggerCounters() != player1OldBulletCounterNumber){
			if (player1->getTriggerCounters() < player1OldBulletCounterNumber) {
				player1TriggerRotationDuration = triggerRotationDurationMs;
				player1TriggerRotationCounter = 0;
				s32 startY, endY, startX1, endX1, startX2, endX2;
				calculateTriggerIconPosition(maxTriggerCounters - 1,
					startY, endY, startX1, endX1, startX2, endX2);
				triggerAnimationLocations.push_back(core::position2di((endX1 + startX1) / 2, (endY + startY) / 2));
				triggerAnimationTimePassed.push_back(0);
				player1OldBulletCounterNumber -= 1;
			}else{
				player1OldBulletCounterNumber = player1->getTriggerCounters();
			}
			//player1TriggerStatus = player1->isTriggerModeActive();
		}
		if (player2->getTriggerCounters() != player2OldBulletCounterNumber){
			if (player2->getTriggerCounters() < player2OldBulletCounterNumber) {
				//if (player2->isTriggerModeActive()){
				player2TriggerRotationDuration = triggerRotationDurationMs;
				player2TriggerRotationCounter = 0;
				s32 startY, endY, startX1, endX1, startX2, endX2;
				calculateTriggerIconPosition(maxTriggerCounters - 1,
					startY, endY, startX1, endX1, startX2, endX2);
				triggerAnimationLocations.push_back(core::position2di((endX2 + startX2) / 2, (endY + startY) / 2));
				triggerAnimationTimePassed.push_back(0);
				player2OldBulletCounterNumber -= 1;
			}else{
				player2OldBulletCounterNumber = player2->getTriggerCounters();
			}
			//};
			//player2TriggerStatus = player2->isTriggerModeActive();
		}
		/* draw trigger cylinders (if allowed)*/
		if (showGunCylinder){
			drawTriggerGunCylinders(frameDeltaTime_s);
		}
		/* draw character portraits*/
		if (showPlayerPortraits){
			drawPlayerPortraits(pause);
		}
		/* draw life bars */
		drawLifeBars(player1lifeRatio, player2lifeRatio, pause);
		/* draw player names */
		if (showPlayerNames){
			drawPlayerNames(player1->getWDisplayName(), player2->getWDisplayName());
		}
		/* check for trigger moves */
		if (showTriggerCounters) {
			drawTriggerCounters(player1Triggers, player2Triggers, frameDeltaTime_s);
		}
		/* check for trigger number */
		if (showTriggerNumber) {
			drawTriggerNumber(player1Triggers, player2Triggers);
		}
		/* check for time-dependent objects */
		drawCharacterObjectsLifetimeBars();
		/* draw "win round" indicators */
		drawRoundIndicators(numberOfIndicatorsPl1, numberOfIndicatorsPl2, player1winNumber, player2winNumber, frameDeltaTime_s);
		/* draw round timer*/
		drawTimer(roundTimeS);
		/* draw trigger animations */
		drawTriggerAnimations(frameDeltaTime_s, pause);
		/* draw combo indicators */
		drawComboCounters(player1->getComboCounter(), player2->getComboCounter(), frameDeltaTime_s);
		/* draw pause text, if the game is paused */
		u32 timeDeltaInMs = (u32)std::ceil(1000.0*frameDeltaTime_s);
		updateMoveHitFlags(timeDeltaInMs);
		if (pause){
			drawPause();
		}
	};

	/* draw trigger explosion animation */
	void FK_HUDManager_font::drawTriggerAnimations(f32 frameDeltaTime_s, bool pause){
		// for each item in the animation array...
		int size = triggerAnimationLocations.size();
		int deletionIndex = -1;
		f32 maxTime_s = 0.2;
		u32 numberOfFrames = triggerAnimationSize.Width * triggerAnimationSize.Height;
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
				s32 frameXSource = (frameId % (s32)triggerAnimationSize.Width) * (s32)triggerAnimationCellSize.Width;
				s32 frameYSource = (frameId / (s32)triggerAnimationSize.Width) * (s32)triggerAnimationCellSize.Height;
				driver->draw2DImage(triggerAnimationTexture,
					core::rect<s32>(startX, startY, endX, endY),
					core::rect<s32>(frameXSource, frameYSource,
					frameXSource + triggerAnimationCellSize.Width, frameYSource + triggerAnimationCellSize.Height),
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
	void FK_HUDManager_font::drawLifeBars(f32 player1lifeRatio, f32 player2lifeRatio, bool pause){
		// get life bar size
		core::dimension2d<u32> barSize = 
			core::dimension2d<u32>(lifebarCasePosition.LowerRightCorner.X, lifebarCasePosition.LowerRightCorner.Y);
		core::dimension2d<u32> barTexSize = lifebar1->getSize();
		core::dimension2d<u32> lbarSize = 
			core::dimension2d<u32>(lifebarPosition.LowerRightCorner.X, lifebarPosition.LowerRightCorner.Y);
		core::dimension2d<u32> lbarTexSize = lifebar1sub_bw->getSize();
		// get bar width adjusted by life ratio
		float barwidth1 = player1lifeRatio * lbarSize.Width;
		float barwidth2 = player2lifeRatio * lbarSize.Width;
		float barTexWidth1 = player1lifeRatio * lbarTexSize.Width;
		float barTexWidth2 = player2lifeRatio * lbarTexSize.Width;
		// get bar width adjusted by life ratio
		float vanishingBarwidth1 = oldLifeRatioPlayer1 * lbarSize.Width;
		float vanishingBarTexWidth1 = oldLifeRatioPlayer1 * lbarTexSize.Width;
		float vanishingBarwidth2 = oldLifeRatioPlayer2 * lbarSize.Width;
		float vanishingBarTexWidth2 = oldLifeRatioPlayer2 * lbarTexSize.Width;
		// create color array
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		/* prepare color arrays for hud lifebars */

		/* vanishing life bars color */
		video::SColor const vanishinglifebarColors_1[4] = {
			highVanishingLifeColor,
			highVanishingLifeColor,
			lowVanishingLifeColor,
			lowVanishingLifeColor
		};
		video::SColor const vanishinglifebarColors_2[4] = {
			lowVanishingLifeColor,
			lowVanishingLifeColor,
			highVanishingLifeColor,
			highVanishingLifeColor
		};
		/* normal life bars color */
		video::SColor const lifebarColors_1[4] = {
			highLifeColor,
			highLifeColor,
			lowLifeColor,
			lowLifeColor
		};
		video::SColor const lifebarColors_2[4] = {
			lowLifeColor,
			lowLifeColor,
			highLifeColor,
			highLifeColor
		};
		video::SColor const lifebarColors_background[4] = {
			lifebarBackgroundColor,
			lifebarBackgroundColor,
			lifebarBackgroundColor,
			lifebarBackgroundColor,
		};
		// set a couple constants (due to the graphics used for the HUD)
		f32 hudLifebarXoffsetFromCase = (f32)lifebarPosition.UpperLeftCorner.X;
		f32 hudLifebarYoffsetFromCase = (f32)lifebarPosition.UpperLeftCorner.Y;
		const f32 hudLifebarXoffsetFromCenterScreen =
			referenceScreenSize.Width / 2 - hudLifebarXoffsetFromCase - lbarSize.Width;

		// draw background lifebar
		int lifebar1x = screenSize.Width / 2 - (hudLifebarXoffsetFromCase +
			lbarSize.Width)*scale_factorX;
		driver->draw2DImage(lifebar1sub_bw,
			core::rect<s32>(lifebar1x,
			hudLifebarYoffsetFromCase * scale_factorY, lifebar1x + lbarSize.Width*scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(0, 0, lbarTexSize.Width, lbarTexSize.Height),
			0, lifebarColors_background, true);
		int x1 = screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX;
		int x2 = screenSize.Width / 2 + (hudLifebarXoffsetFromCase + lbarSize.Width)* scale_factorX;
		driver->draw2DImage(lifebar2sub_bw,
			core::rect<s32>(screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX,
			hudLifebarYoffsetFromCase * scale_factorY,
			screenSize.Width / 2 + (hudLifebarXoffsetFromCase + lbarSize.Width)* scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(0, 0, lbarTexSize.Width, lbarTexSize.Height), 0, lifebarColors_background, true);

		// draw first vanishing lifebars
		if (showVanishingDamage) {
			lifebar1x = screenSize.Width / 2 + (hudLifebarXoffsetFromCenterScreen + lbarSize.Width -
				barSize.Width - vanishingBarwidth1)*scale_factorX;
			driver->draw2DImage(lifebar1sub_bw,
				core::rect<s32>(lifebar1x,
					hudLifebarYoffsetFromCase * scale_factorY, lifebar1x + vanishingBarwidth1*scale_factorX,
					(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
				core::rect<s32>(lbarTexSize.Width - vanishingBarTexWidth1, 0, lbarTexSize.Width, lbarTexSize.Height),
				0, vanishinglifebarColors_1, true);

			x1 = screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX;
			x2 = screenSize.Width / 2 + (hudLifebarXoffsetFromCase + vanishingBarwidth2)* scale_factorX;
			driver->draw2DImage(lifebar2sub_bw,
				core::rect<s32>(screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX,
					hudLifebarYoffsetFromCase * scale_factorY,
					screenSize.Width / 2 + (hudLifebarXoffsetFromCase + vanishingBarwidth2)* scale_factorX,
					(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
				core::rect<s32>(0, 0, vanishingBarTexWidth2, lbarTexSize.Height), 0,
				vanishinglifebarColors_2, true);
		}
		// draw normal lifebars
		lifebar1x = screenSize.Width / 2 + (hudLifebarXoffsetFromCenterScreen + lbarSize.Width -
			barSize.Width - barwidth1)*scale_factorX;
		driver->draw2DImage(lifebar1sub_bw,
			core::rect<s32>(lifebar1x,
			hudLifebarYoffsetFromCase * scale_factorY, lifebar1x + barwidth1*scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(lbarTexSize.Width - barTexWidth1, 0, lbarTexSize.Width, lbarTexSize.Height),
			0, 
			lifebarColors_1, true);

		x1 = screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX;
		x2 = screenSize.Width / 2 + (hudLifebarXoffsetFromCase + barwidth2)* scale_factorX;
		driver->draw2DImage(lifebar2sub_bw,
			core::rect<s32>(screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX,
			hudLifebarYoffsetFromCase * scale_factorY,
			screenSize.Width / 2 + (hudLifebarXoffsetFromCase + barwidth2)* scale_factorX,
			(hudLifebarYoffsetFromCase + lbarSize.Height) * scale_factorY),
			core::rect<s32>(0, 0, barTexWidth2, lbarTexSize.Height), 0,
			/*pause ? lifebarColors_2_pause : */lifebarColors_2, true);

		if (showLifebarCases){
			s32 caseOffsetX = (s32)(lifebarCasePosition.UpperLeftCorner.X*scale_factorX);
			s32 caseOffsetY = (s32)(lifebarCasePosition.UpperLeftCorner.Y*scale_factorY);
			driver->draw2DImage(/*pause ? lifebar1_bw : */lifebar1,
				core::rect<s32>(screenSize.Width / 2 - caseOffsetX,
				caseOffsetY,
				screenSize.Width / 2 - caseOffsetX + barSize.Width*scale_factorX,
				caseOffsetY + barSize.Height*scale_factorY),
				core::rect<s32>(barTexSize.Width, 0, 0, barTexSize.Height), 0, vertexColors, true);

			driver->draw2DImage(/*pause ? lifebar1_bw : */lifebar1,
				core::rect<s32>(screenSize.Width / 2 - barSize.Width*scale_factorX + caseOffsetX,
				caseOffsetY,
				screenSize.Width / 2 + caseOffsetX,
				caseOffsetY + barSize.Height*scale_factorY),
				core::rect<s32>(0, 0, barTexSize.Width, barTexSize.Height), 0, vertexColors, true);
		}
	}

	/* draw trigger number */
	void FK_HUDManager_font::drawTriggerNumber(u32 triggerNumberPlayer1, u32 triggerNumberPlayer2) {
		std::wstring pl1TriggerNumber = std::to_wstring(triggerNumberPlayer1);
		std::wstring pl2TriggerNumber = std::to_wstring(triggerNumberPlayer2);
		s32 sentenceWidth = font->getDimension(pl1TriggerNumber.c_str()).Width;
		s32 sentenceHeight = font->getDimension(pl1TriggerNumber.c_str()).Height;
		s32 sentenceOffsetX = triggerNumberPosition.X; // distance from screen center (default: 68)
		s32 sentenceOffsetY = triggerNumberPosition.Y; // distance from the top of the screen
		core::rect<s32> destinationRect = core::rect<s32>(
			sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			sentenceOffsetX * scale_factorX + sentenceWidth,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		fk_addons::drawBorderedText(font, pl1TriggerNumber, destinationRect, irr::video::SColor(255, 255, 255, 255),
			irr::video::SColor(128, 0, 0, 0),
			centerPlayerNames);
		sentenceWidth = font->getDimension(pl2TriggerNumber.c_str()).Width;
		sentenceHeight = font->getDimension(pl2TriggerNumber.c_str()).Height;
		sentenceOffsetX = triggerNumberPosition.X; // distance from screen center (default: 68)
		sentenceOffsetY = triggerNumberPosition.Y; // distance from the top of the screen
		destinationRect = core::rect<s32>(screenSize.Width - sentenceWidth - sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width - sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		fk_addons::drawBorderedText(font, pl2TriggerNumber, destinationRect, irr::video::SColor(255, 255, 255, 255),
			irr::video::SColor(128, 0, 0, 0),
			centerPlayerNames);
	}

	/* draw player names */
	void FK_HUDManager_font::drawPlayerNames(std::wstring player1Name, std::wstring player2Name){
		// convert names to wide strings format to write them (irrlicht accepts only wide strings here)
		std::transform(player1Name.begin(), player1Name.end(), player1Name.begin(), toupper);
		std::transform(player2Name.begin(), player2Name.end(), player2Name.begin(), toupper);
		std::wstring pl1Name = std::wstring(player1Name.begin(), player1Name.end());
		std::wstring pl2Name = std::wstring(player2Name.begin(), player2Name.end());
		/* get width and height of the string*/
		s32 sentenceWidth = font->getDimension(pl1Name.c_str()).Width;
		s32 sentenceHeight = font->getDimension(pl1Name.c_str()).Height;
		s32 sentenceOffsetX = playerNameOffset.X; // distance from screen center (default: 68)
		s32 sentenceOffsetY = playerNameOffset.Y; // distance from the top of the screen
		// draw player 1 name near the lifebar
		core::rect<s32> destinationRect = core::rect<s32>(screenSize.Width / 2 - sentenceWidth - sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 - sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		if (centerPlayerNames){
			core::dimension2du lbarSize = core::dimension2du(
				(u32)lifebarPosition.LowerRightCorner.X,
				(u32)lifebarPosition.LowerRightCorner.Y);
			f32 hudLifebarXoffsetFromCase = (f32)lifebarPosition.UpperLeftCorner.X;
			// draw background lifebar
			int lifebar1x = screenSize.Width / 2 - (hudLifebarXoffsetFromCase +
				lbarSize.Width)*scale_factorX;
			destinationRect = core::rect<s32>(lifebar1x,
				sentenceOffsetY * scale_factorY, 
				lifebar1x + lbarSize.Width*scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY);
		}
		fk_addons::drawBorderedText(font, pl1Name, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0),
			centerPlayerNames);
		//drawNormalText(font, pl1Name, destinationRect);
		// draw player 2 name near the lifebar
		sentenceWidth = font->getDimension(pl2Name.c_str()).Width;
		sentenceHeight = font->getDimension(pl2Name.c_str()).Height;
		destinationRect = core::rect<s32>(screenSize.Width / 2 + sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			screenSize.Width / 2 + sentenceWidth + sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		if (centerPlayerNames){
			core::dimension2du lbarSize = core::dimension2du(
				(u32)lifebarPosition.LowerRightCorner.X,
				(u32)lifebarPosition.LowerRightCorner.Y);
			f32 hudLifebarXoffsetFromCase = (f32)lifebarPosition.UpperLeftCorner.X;
			destinationRect = core::rect<s32>(screenSize.Width / 2 + hudLifebarXoffsetFromCase * scale_factorX,
				sentenceOffsetY * scale_factorY,
				screenSize.Width / 2 + (hudLifebarXoffsetFromCase + lbarSize.Width)* scale_factorX,
				sentenceHeight + sentenceOffsetY * scale_factorY);
		}
		fk_addons::drawBorderedText(font, pl2Name, destinationRect, irr::video::SColor(255, 255, 255, 255), irr::video::SColor(128, 0, 0, 0),
			centerPlayerNames);
		//drawNormalText(font, pl2Name, destinationRect);
	}

	/* draw player portraits*/
	void FK_HUDManager_font::drawPlayerPortraits(bool pause){
		// create color array
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		// get portrait dimensions
		core::dimension2d<u32> portraitSize = core::dimension2d<u32>(
			(u32)portraitPosition.LowerRightCorner.X, 
			(u32)portraitPosition.LowerRightCorner.Y);
		core::dimension2d<u32> portraitTexSize = player1Portrait->getSize();
		// pause texture still to be implemented - draw portrait
		s32 portraitOffsetX = portraitPosition.UpperLeftCorner.X;
		s32 portraitOffsetY = portraitPosition.UpperLeftCorner.Y;
		driver->draw2DImage(player1Portrait,
			core::rect<s32>(portraitOffsetX*scale_factorX, portraitOffsetY*scale_factorY, portraitSize.Width*scale_factorX,
			portraitSize.Height*scale_factorY),
			core::rect<s32>(0, 0, portraitTexSize.Width, portraitTexSize.Height), 0, vertexColors, true);
		// draw player 2 portrait (mirrored on X)
		portraitTexSize = player2Portrait->getSize();
		driver->draw2DImage(player2Portrait,
			core::rect<s32>(screenSize.Width - (portraitSize.Width + portraitOffsetX)*scale_factorX, 
			portraitOffsetY*scale_factorY, 
			screenSize.Width - portraitOffsetX*scale_factorX,
			portraitSize.Height*scale_factorY),
			core::rect<s32>(portraitTexSize.Width, 0, 0, portraitTexSize.Height), 0, vertexColors, true);
	};
	/* draw combo counter */
	void FK_HUDManager_font::drawComboCounters(u32 player1ComboCounter, u32 player2ComboCounter, f32 frameDeltaTimeS){
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
		std::wstring buffer = 
			std::to_wstring((int)player1ComboCounter) + L" " + 
			(player1ComboCounter > 1 ? hitName.second : hitName.first);
		core::stringw pl1ComboCounter = core::stringw(buffer.c_str());
		buffer =
			std::to_wstring((int)player2ComboCounter) + L" " + 
			(player2ComboCounter > 1 ? hitName.second : hitName.first);
		core::stringw pl2ComboCounter = core::stringw(buffer.c_str());

		/* get damage*/
		std::wstring damageDonePlayer1 =
			std::to_wstring(player1->getComboDamage()) + L" " + 
			(player1->getComboDamage() > 1 ? damageName.second : damageName.first);
		std::wstring damageDonePlayer2 = 
			std::to_wstring(player2->getComboDamage()) + L" " + 
			(player2->getComboDamage() > 1 ? damageName.second : damageName.first);
		/* get width and height of the string*/
		s32 sentenceWidth = font->getDimension(damageDonePlayer1.c_str()).Width;
		s32 sentenceHeight = font->getDimension(damageDonePlayer1.c_str()).Height;
		s32 sentenceOffsetX = comboTextOffset.X; // distance from screen center
		s32 sentenceOffsetY = comboTextOffset.Y; // distance from the bottom of the screen
		// draw player 1 combo counter if combo > 0
		if (player1ComboCounter >= minimalComboHitNumberToShow && comboTextDurationSPlayer1 < comboTextMaxDurationS){
			if (player1->getComboDamage() > 0 && showComboDamage){
				// draw combo damage
				fk_addons::drawBorderedText(font, damageDonePlayer1.c_str(), core::rect<s32>(sentenceOffsetX * scale_factorX,
					screenSize.Height - sentenceHeight - sentenceOffsetY * scale_factorY,
					sentenceWidth + sentenceOffsetX * scale_factorX,
					screenSize.Height - sentenceOffsetY * scale_factorY),
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
					false, false);
			}
			if (showComboNumber) {
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
		}
		sentenceOffsetY = comboTextOffset.Y;
		// draw player 2 combo counter if combo > 0
		if (player2ComboCounter >= minimalComboHitNumberToShow && comboTextDurationSPlayer2 < comboTextMaxDurationS){
			// draw combo damage
			if (player2->getComboDamage() > 0 && showComboDamage){
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
			if (showComboNumber) {
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
	}

	/* draw round indicators*/
	void FK_HUDManager_font::drawRoundIndicators(u32 numberOfIndicatorsPl1, u32 numberOfIndicatorsPl2, 
		u32 player1winNumber, u32 player2winNumber, f32 frameDeltaTime_s){
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		player1winNumber = std::min(numberOfIndicatorsPl1, player1winNumber);
		player2winNumber = std::min(numberOfIndicatorsPl2, player2winNumber);
		core::dimension2d<u32> iconSize = core::dimension2d<u32>(roundIndicatorPosition.LowerRightCorner.X, 
			roundIndicatorPosition.LowerRightCorner.Y);
		core::dimension2d<u32> iconTexSize = roundIndicatorFull->getSize();
		s32 roundIndicatorY = roundIndicatorPosition.UpperLeftCorner.Y;
		s32 roundIndicatorXOffset = roundIndicatorPosition.UpperLeftCorner.X;

		s32 startY = roundIndicatorY * scale_factorY;
		s32 endY = startY + iconSize.Height * scale_factorY;

		f32 potentialRoundWinBlinkPeriod = 2.0f;
		roundIndicatorBlinkTime += frameDeltaTime_s;

		for (u32 i = 0; i < numberOfIndicatorsPl1; i++){
			s32 tempX1 = iconSize.Width * (i + 1) * scale_factorX;
			s32 startX1 = screenSize.Width / 2 - roundIndicatorXOffset * scale_factorX - tempX1;
			s32 endX1 = startX1 + iconSize.Width*scale_factorX;
			video::ITexture* texture;
			if (i < player1winNumber){
				//if (pause){
				//	texture = roundIndicatorFull_bw;
				//}
				//else{
				texture = roundIndicatorFull;
				//}
			}
			else{
				texture = roundIndicatorEmpty;
			}
			if (i < player1winNumber || (i >= player1winNumber && showEmptyRoundIndicators)){
				driver->draw2DImage(texture,
					core::rect<s32>(startX1, startY, endX1, endY),
					core::rect<s32>(0, 0, iconTexSize.Width, iconTexSize.Height), 0, vertexColors, true);
			}
			if (i == player1winNumber && 
				player1->getLifePercentage() > player2->getLifePercentage() &&
				!player2->isKO() && showBlinkingRoundIndicatorPredictor){
				f32 phase = -0.5*std::cos(roundIndicatorBlinkTime / potentialRoundWinBlinkPeriod * 2 * core::PI) + 0.5;
				video::SColor colorSemiTransp = video::SColor((s32)(196.0f * phase + 1), 255, 255, 255);
				video::SColor const vertexColors2[4] = {
					colorSemiTransp,
					colorSemiTransp,
					colorSemiTransp,
					colorSemiTransp
				};
				driver->draw2DImage(roundIndicatorFull,
					core::rect<s32>(startX1, startY, endX1, endY),
					core::rect<s32>(0, 0, iconTexSize.Width, iconTexSize.Height), 0, vertexColors2, true);
			}
		}
		for (u32 i = 0; i < numberOfIndicatorsPl2; i++){
			s32 tempX2 = iconSize.Width * i * scale_factorX;
			s32 startX2 = screenSize.Width / 2 + roundIndicatorXOffset * scale_factorX + tempX2;
			s32 endX2 = startX2 + iconSize.Width*scale_factorX;
			video::ITexture* texture;
			if (i < player2winNumber){
				/*if (pause){
					texture = roundIndicatorFull_bw;
				}
				else{*/
				texture = roundIndicatorFull;
				//}
			}
			else{
				texture = roundIndicatorEmpty;
			}
			if (i < player2winNumber || (i >= player2winNumber && showEmptyRoundIndicators)){
				driver->draw2DImage(texture,
					core::rect<s32>(startX2, startY, endX2, endY),
					core::rect<s32>(0, 0, iconTexSize.Width, iconTexSize.Height), 0, vertexColors, true);
			}
			if (i == player2winNumber && 
				player2->getLifePercentage() > player1->getLifePercentage() &&
				!player1->isKO() &&
				showBlinkingRoundIndicatorPredictor){
				f32 phase = -0.5*std::cos(roundIndicatorBlinkTime / potentialRoundWinBlinkPeriod * 2 * core::PI) + 0.5;
				video::SColor colorSemiTransp = video::SColor((s32)(196 * phase + 1), 255, 255, 255);
				video::SColor const vertexColors2[4] = {
					colorSemiTransp,
					colorSemiTransp,
					colorSemiTransp,
					colorSemiTransp
				};
				driver->draw2DImage(roundIndicatorFull,
					core::rect<s32>(startX2, startY, endX2, endY),
					core::rect<s32>(0, 0, iconTexSize.Width, iconTexSize.Height), 0, vertexColors2, true);
			}
		}
	}

	void FK_HUDManager_font::calculateTriggerIconPosition(s32 index,
		s32 &startY, s32 &endY, s32 &startX1, s32 &endX1,
		s32 &startX2, s32 &endX2){
		if (drawTriggersFromBorderToCenter){
			index = maxTriggerCounters - index;
		}
		s32 triggerIndicatorY = triggerCounterPosition.UpperLeftCorner.Y;//28
		s32 triggerIndicatorXOffset = triggerCounterPosition.UpperLeftCorner.X;//160
		core::dimension2d<u32> iconSize = core::dimension2d<u32>(triggerCounterPosition.LowerRightCorner.X,
			triggerCounterPosition.LowerRightCorner.Y);
		s32 tempX1 = iconSize.Width * index * scale_factorX;
		s32 tempX2 = iconSize.Width * (index + 1) * scale_factorX;
		startY = triggerIndicatorY * scale_factorY;
		endY = startY + iconSize.Height * scale_factorY;
		startX1 = triggerIndicatorXOffset * scale_factorX + tempX1;
		endX1 = startX1 + iconSize.Width * scale_factorX;
		startX2 = screenSize.Width - (triggerIndicatorXOffset * scale_factorX + tempX2);
		endX2 = startX2 + iconSize.Width * scale_factorX;
	}

	/* draw trigger gun cylinder */
	void FK_HUDManager_font::drawTriggerGunCylinders(f32 frameDeltaTimeS){
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		/* draw gun cylinder */
		core::dimension2d<u32> gunIconSize = core::dimension2d<u32>(
			triggerCylinderPosition.LowerRightCorner.X,
			triggerCylinderPosition.LowerRightCorner.Y);
		core::dimension2d<u32> gunIconTexSize = triggerCylinderIcon->getSize();
		f32 triggerAngleLimit = 360.0 / 6.0;
		s32 offsetX = gunIconSize.Width / 2 + triggerCylinderPosition.UpperLeftCorner.X;
		s32 offsetY = triggerCylinderPosition.UpperLeftCorner.Y;
		// player 1
		s32 gunCylinderPositionY1 = offsetY  * scale_factorY;
		s32 gunCylinderPositionX1 = offsetX * scale_factorX;
		s32 gunCylinderPositionY2 = gunCylinderPositionY1 + gunIconSize.Height * scale_factorY;
		s32 gunCylinderPositionX2 = gunCylinderPositionX1 + gunIconSize.Width * scale_factorX;
		s32 cylinderCenterX = gunCylinderPositionX1 + gunIconSize.Width * scale_factorX / 2;
		s32 cylinderCenterY = gunCylinderPositionY1 + gunIconSize.Height * scale_factorY / 2;
		u32 frameDeltaTimeMs = (u32)std::floor(1000.f * frameDeltaTimeS);
		player1TriggerRotationCounter += frameDeltaTimeMs;
		if (player1TriggerRotationDuration > 0){
			player1CylinderRotation = triggerAngleLimit * (f32)player1TriggerRotationCounter / player1TriggerRotationDuration;
		}
		if (player1TriggerRotationCounter >= player1TriggerRotationDuration){
			player1TriggerRotationDuration = 0;
			player1CylinderRotation = 0.0f;
		}
		f32 additionalScaleX = (f32)gunIconSize.Width / (f32)gunIconTexSize.Width;
		f32 additionalScaleY = (f32)gunIconSize.Height / (f32)gunIconTexSize.Height;
		core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1);
		core::position2d<s32>(cylinderCenterX, cylinderCenterY);
		fk_addons::drawBatch2DImage(driver, triggerCylinderIcon,
			core::rect<s32>(0, 0, gunIconTexSize.Width, gunIconTexSize.Height),
			core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1),
			core::position2d<s32>(cylinderCenterX, cylinderCenterY),
			player1CylinderRotation,
			core::vector2d<f32>(scale_factorX * additionalScaleX, scale_factorY * additionalScaleY),
			true,
			video::SColor(255, 255, 255, 255),
			core::rect<s32>(0, 0, screenSize.Width, screenSize.Height)
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
		fk_addons::drawBatch2DImage(driver, triggerCylinderIcon,
			core::rect<s32>(0, 0, gunIconTexSize.Width, gunIconTexSize.Height),
			core::position2d<s32>(gunCylinderPositionX1, gunCylinderPositionY1),
			core::position2d<s32>(cylinderCenterX, cylinderCenterY),
			player2CylinderRotation,
			core::vector2d<f32>(scale_factorX * additionalScaleX, scale_factorY * additionalScaleY),
			true,
			video::SColor(255, 255, 255, 255),
			core::rect<s32>(0, 0, screenSize.Width, screenSize.Height)
			);
	}

	void FK_HUDManager_font::drawObjectHUDbarPlayer1(
		FK_Character::FK_CharacterAdditionalObject & object,
		FK_Character::FK_CharacterAdditionalObject::HUDbar &hudBar, f32 ratio) {

		s32 barLength = 0;
		s32 barHeight = 0;
		s32 x11 = 0, x21 = 0;
		s32 x12 = 0, x22 = 0;
		s32 y11 = 0, y21 = 0;
		s32 x1, x2, y1, y2;
		s32 temp;

		video::SColor backgroundColor;
		video::SColor borderColor;

		video::SColor mainColor;
		video::SColor gradientColor;
		s32 lineWidth = (s32)std::ceil(1.f *scale_factorX);

		core::rect<s32> objectBarPosition = hudBar.position;
		barLength = objectBarPosition.LowerRightCorner.X;
		barHeight = objectBarPosition.LowerRightCorner.Y;
		barLength = (s32)std::round((f32)barLength * scale_factorX);
		barHeight = (s32)std::round((f32)barHeight * scale_factorY);
		if ((barLength % 2) != 0) {
			barLength += 1;
		}
		if ((barHeight % 2) != 0) {
			barHeight += 1;
		}
		x11 = objectBarPosition.UpperLeftCorner.X;
		y21 = objectBarPosition.UpperLeftCorner.Y;
		x11 = (s32)std::round((f32)x11 * scale_factorX);
		y21 = (s32)std::round((f32)y21 * scale_factorY);
		x22 = screenSize.Width - x11;

		x1 = x11;
		y1 = y21;
		x2 = x1 + barLength + 2 * lineWidth;
		y2 = y1 + barHeight + 2 * lineWidth;

		mainColor = hudBar.color1;
		gradientColor = hudBar.color2;
		backgroundColor = hudBar.backgroundColor;
		borderColor = hudBar.borderColor;

		driver->draw2DRectangle(backgroundColor, core::rect<s32>(x1, y1, x2, y2));
		
		if (hudBar.reverseDirection) {
			ratio = 1 - ratio;
		}
		ratio = core::clamp(ratio, 0.f, 1.f);
		// draw trigger regen bar
		x2 = x1 + (s32)std::round((barLength + 2 * lineWidth) *
			ratio);

		driver->draw2DRectangle(core::rect<s32>(x1, y1, x2, y2),
			gradientColor, mainColor, gradientColor, mainColor);
		// draw border
		x1 = x11;
		y1 = y21;
		x2 = x1 + barLength + 2 * lineWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = x11;
		y1 = y21 + barHeight + lineWidth;;
		x2 = x1 + barLength + 2 * lineWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = x11;
		y1 = y21;
		x2 = x1 + lineWidth;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = x11 + barLength + lineWidth;
		y1 = y21;
		x2 = x1 + lineWidth;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		// move new bar above
		//y21 -= 1.2 * barHeight;
		// if there is an icon, draw it!
		if (hudBar.showIcon) {
			std::string path = player1->getCharacterDirectory() + player1->getOutfitPath() + object.uniqueNameId + ".png";
			video::ITexture* tex = driver->getTexture(path.c_str());
			if (tex) {
				core::rect<s32> iconPosition = hudBar.iconPosition;
				core::dimension2d<u32> iconSize = tex->getSize();
				s32 w = iconPosition.LowerRightCorner.X;
				s32 h = iconPosition.LowerRightCorner.Y;
				w = (s32)std::round((f32)w * scale_factorX);
				h = (s32)std::round((f32)h * scale_factorY);
				s32 x1i = iconPosition.UpperLeftCorner.X;
				s32 y1i = iconPosition.UpperLeftCorner.Y;
				x1i = (s32)std::round((f32)x1i * scale_factorX);
				y1i = (s32)std::round((f32)y1i * scale_factorY);
				fk_addons::drawBatch2DImage(device->getVideoDriver(), tex, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1i, y1i), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)w / iconSize.Width, (f32)h / iconSize.Height),
					true,
					video::SColor(255, 255, 255, 255),
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				hudBar.showIcon = false;
			}
		}
	}

	void FK_HUDManager_font::drawObjectHUDbarPlayer2(FK_Character::FK_CharacterAdditionalObject & object,
		FK_Character::FK_CharacterAdditionalObject::HUDbar& hudBar, f32 ratio) {

		s32 barLength = 0;
		s32 barHeight = 0;
		s32 x11 = 0, x21 = 0;
		s32 x12 = 0, x22 = 0;
		s32 y11 = 0, y21 = 0;
		s32 x1, x2, y1, y2;
		s32 temp;

		video::SColor backgroundColor;
		video::SColor borderColor;

		video::SColor mainColor;
		video::SColor gradientColor;
		s32 lineWidth = (s32)std::ceil(1.f *scale_factorX);


		core::rect<s32> objectBarPosition = hudBar.position;
		barLength = objectBarPosition.LowerRightCorner.X;
		barHeight = objectBarPosition.LowerRightCorner.Y;
		barLength = (s32)std::round((f32)barLength * scale_factorX);
		barHeight = (s32)std::round((f32)barHeight * scale_factorY);
		if ((barLength % 2) != 0) {
			barLength += 1;
		}
		if ((barHeight % 2) != 0) {
			barHeight += 1;
		}
		x11 = objectBarPosition.UpperLeftCorner.X;
		y21 = objectBarPosition.UpperLeftCorner.Y;
		x11 = (s32)std::round((f32)x11 * scale_factorX);
		y21 = (s32)std::round((f32)y21 * scale_factorY);
		x22 = screenSize.Width - x11;

		mainColor = hudBar.color1;
		gradientColor = hudBar.color2;
		backgroundColor = hudBar.backgroundColor;
		borderColor = hudBar.borderColor;

		// draw basic black box
		y1 = y21;
		x2 = x22;
		x1 = x2 - barLength - 2 * lineWidth;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(backgroundColor, core::rect<s32>(x1, y1, x2, y2));
		
		if (hudBar.reverseDirection) {
			ratio = 1 - ratio;
		}
		ratio = core::clamp(ratio, 0.f, 1.f);
		// draw trigger regen bar
		x1 = x2 - (s32)std::round((barLength + 2 * lineWidth) * ratio);

		driver->draw2DRectangle(core::rect<s32>(x1, y1, x2, y2),
			mainColor, gradientColor, mainColor, gradientColor);
		// draw border
		x2 = x22;
		x1 = x2 - barLength - 2 * lineWidth;
		y1 = y21;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		y1 = y21 + barHeight + lineWidth;;
		x2 = x22;
		x1 = x2 - barLength - 2 * lineWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x2 = x22;
		x1 = x2 - lineWidth;
		y1 = y21;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x2 = x22 - barLength - lineWidth;
		x1 = x2 - lineWidth;
		y1 = y21;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		// move new bar above
		//y21 -= 1.2 * barHeight;
		if (hudBar.showIcon) {
			std::string path = player2->getCharacterDirectory() + player2->getOutfitPath() + object.uniqueNameId + ".png";
			video::ITexture* tex = driver->getTexture(path.c_str());
			if (tex) {
				core::rect<s32> iconPosition = hudBar.iconPosition;
				core::dimension2d<u32> iconSize = tex->getSize();
				s32 w = iconPosition.LowerRightCorner.X;
				s32 h = iconPosition.LowerRightCorner.Y;
				w = (s32)std::round((f32)w * scale_factorX);
				h = (s32)std::round((f32)h * scale_factorY);
				s32 x1i = iconPosition.UpperLeftCorner.X;
				s32 y1i = iconPosition.UpperLeftCorner.Y;
				x1i = (s32)std::round((f32)x1i * scale_factorX);
				y1i = (s32)std::round((f32)y1i * scale_factorY);
				x1i = screenSize.Width - x1i - w;
				fk_addons::drawBatch2DImage(device->getVideoDriver(), tex, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
					core::position2d<s32>(x1i, y1i), core::vector2d<s32>(0, 0), 0,
					core::vector2d<f32>((f32)w / iconSize.Width, (f32)h / iconSize.Height),
					true,
					video::SColor(255, 255, 255, 255),
					core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			else {
				hudBar.showIcon = false;
			}
		}
	}

	/* draw character objects lifetime bars */
	void FK_HUDManager_font::drawCharacterObjectsLifetimeBars() {
		s32 barLength = 0;
		s32 barHeight = 0;
		s32 x11 = 0, x21 = 0;
		s32 x12 = 0, x22 = 0;
		s32 y11 = 0, y21 = 0;
		s32 x1, x2, y1, y2;
		s32 temp;
		
		video::SColor backgroundColor;
		video::SColor borderColor;

		video::SColor mainColor;
		video::SColor gradientColor;
		s32 lineWidth = (s32)std::ceil(1.f *scale_factorX);
		
		// player 1
		// draw basic black box
		for each(auto object in player1->getCharacterAdditionalObjects()) {
			if ((object.isActive() || object.hudBar.showWhenInactive) && object.expiresWithTime && object.showTimeBar) {
				f32 lifetimeTimer = object.lifetimeTimer;
				if (!object.isActive()) {
					if (object.hudBar.reverseDirection) {
						lifetimeTimer = object.maxLifetime;
					}
					else {
						lifetimeTimer = 0;
					}
					if (object.isBroken() && object.hudBar.reverseWhenBroken) {
						lifetimeTimer = abs(lifetimeTimer - object.maxLifetime);
					}
				}
				f32 ratio = (object.maxLifetime - lifetimeTimer) / object.maxLifetime;
				drawObjectHUDbarPlayer1(object, object.hudBar, ratio);
			}
			if ((object.isActive() || object.hudLifeBar.showWhenInactive) && object.showEnduranceBar) {
				f32 endurance = object.endurance;
				if (!object.isActive()) {
					if (object.hudLifeBar.reverseDirection) {
						endurance = object.maxEndurance;
					}
					else {
						endurance = 0;
					}
					if (object.isBroken() && object.hudLifeBar.reverseWhenBroken) {
						endurance = abs(endurance - object.maxEndurance);
					}
				}
				f32 ratio = (object.maxEndurance - endurance) / object.maxEndurance;
				drawObjectHUDbarPlayer1(object, object.hudLifeBar, ratio);
			}
			if ((object.isActive() && object.hudIcon.showWhenActive) || 
				(!object.isActive() && object.hudIcon.showWhenInactive)) {
				std::string rootPath = player1->getCharacterDirectory() + player1->getOutfitPath() + object.uniqueNameId;
				std::string activePath = rootPath + "_icon.png";
				std::string inactivePath = rootPath + "_icon_inactive.png";
				std::string path = object.isActive() ? activePath : inactivePath;
				video::ITexture* tex = driver->getTexture(path.c_str());
				if (tex) {
					core::rect<s32> iconPosition = object.hudIcon.position;
					core::dimension2d<u32> iconSize = tex->getSize();
					s32 w = iconPosition.LowerRightCorner.X;
					s32 h = iconPosition.LowerRightCorner.Y;
					w = (s32)std::round((f32)w * scale_factorX);
					h = (s32)std::round((f32)h * scale_factorY);
					s32 x1i = iconPosition.UpperLeftCorner.X;
					s32 y1i = iconPosition.UpperLeftCorner.Y;
					x1i = (s32)std::round((f32)x1i * scale_factorX);
					y1i = (s32)std::round((f32)y1i * scale_factorY);
					fk_addons::drawBatch2DImage(device->getVideoDriver(), tex, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
						core::position2d<s32>(x1i, y1i), core::vector2d<s32>(0, 0), 0,
						core::vector2d<f32>((f32)w / iconSize.Width, (f32)h / iconSize.Height),
						true,
						video::SColor(255, 255, 255, 255),
						core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
				else {
					if (object.isActive()) {
						object.hudIcon.showWhenActive = false;
					}
					else {
						object.hudIcon.showWhenInactive = false;
					}
				}
			}
		}

		// player 2
		for each(auto object in player2->getCharacterAdditionalObjects()) {
			if ((object.isActive() || object.hudBar.showWhenInactive) && object.expiresWithTime && object.showTimeBar) {
				f32 lifetimeTimer = object.lifetimeTimer;
				if (!object.isActive()) {
					if (object.hudBar.reverseDirection) {
						lifetimeTimer = object.maxLifetime;
					}
					else {
						lifetimeTimer = 0;
					}
					if (object.isBroken() && object.hudBar.reverseWhenBroken) {
						lifetimeTimer = abs(lifetimeTimer - object.maxLifetime);
					}
				}
				f32 ratio = (object.maxLifetime - lifetimeTimer) / object.maxLifetime;
				drawObjectHUDbarPlayer2(object, object.hudBar, ratio);
			}
			if ((object.isActive() || object.hudLifeBar.showWhenInactive) && object.showEnduranceBar) {
				f32 endurance = object.endurance;
				if (!object.isActive()) {
					if (object.hudLifeBar.reverseDirection) {
						endurance = object.maxEndurance;
					}
					else {
						endurance = 0;
					}
					if (object.isBroken() && object.hudLifeBar.reverseWhenBroken) {
						endurance = abs(endurance - object.maxEndurance);
					}
				}
				f32 ratio = (object.maxEndurance - endurance) / object.maxEndurance;
				drawObjectHUDbarPlayer2(object, object.hudLifeBar, ratio);
			}
			if ((object.isActive() && object.hudIcon.showWhenActive) ||
				(!object.isActive() && object.hudIcon.showWhenInactive)) {
				std::string rootPath = player2->getCharacterDirectory() + player2->getOutfitPath() + object.uniqueNameId;
				std::string activePath = rootPath + "_icon.png";
				std::string inactivePath = rootPath + "_icon_inactive.png";
				std::string path = object.isActive() ? activePath : inactivePath;
				video::ITexture* tex = driver->getTexture(path.c_str());
				if (tex) {
					core::rect<s32> iconPosition = object.hudIcon.position;
					core::dimension2d<u32> iconSize = tex->getSize();
					s32 w = iconPosition.LowerRightCorner.X;
					s32 h = iconPosition.LowerRightCorner.Y;
					w = (s32)std::round((f32)w * scale_factorX);
					h = (s32)std::round((f32)h * scale_factorY);
					s32 x1i = iconPosition.UpperLeftCorner.X;
					s32 y1i = iconPosition.UpperLeftCorner.Y;
					x1i = (s32)std::round((f32)x1i * scale_factorX);
					y1i = (s32)std::round((f32)y1i * scale_factorY);
					x1i = screenSize.Width - x1i - w;
					fk_addons::drawBatch2DImage(device->getVideoDriver(), tex, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
						core::position2d<s32>(x1i, y1i), core::vector2d<s32>(0, 0), 0,
						core::vector2d<f32>((f32)w / iconSize.Width, (f32)h / iconSize.Height),
						true,
						video::SColor(255, 255, 255, 255),
						core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
				}
				else {
					if (object.isActive()) {
						object.hudIcon.showWhenActive = false;
					}
					else {
						object.hudIcon.showWhenInactive = false;
					}
				}
			}
		}
	}

	/* draw round indicators*/
	void FK_HUDManager_font::drawTriggerCounters(u32 player1Triggers, u32 player2Triggers, f32 frameDeltaTime){
		video::SColor const color = video::SColor(255, 255, 255, 255);
		video::SColor const vertexColors[4] = {
			color,
			color,
			color,
			color
		};
		/* get milliseconds */
		u32 frameDeltaTimeMs = floor(1000.f * frameDeltaTime);
		u32 selectedTriggerCycleMs = 1000;
		/* update trigger timers */
		if (player1->isTriggering()){
			player1TriggerHighlightTimeMs += frameDeltaTimeMs;
		}
		else{
			player1TriggerHighlightTimeMs = 0;
		}
		if (player2->isTriggering()){
			player2TriggerHighlightTimeMs += frameDeltaTimeMs;
		}
		else{
			player2TriggerHighlightTimeMs = 0;
		}
		/* create trigger highlight color array */
		f32 phase1 = - 0.5*std::cos((f32)player1TriggerHighlightTimeMs / selectedTriggerCycleMs * 2 * core::PI) + 0.5;
		f32 phase2 = - 0.5*std::cos((f32)player2TriggerHighlightTimeMs / selectedTriggerCycleMs * 2 * core::PI) + 0.5;
		video::SColor const selectedTriggercolor1 = video::SColor(255, 255, (s32)(255.0f * phase1), (s32)(255.0f * phase1));
		video::SColor const selectedVertexColors1[4] = {
			selectedTriggercolor1,
			selectedTriggercolor1,
			selectedTriggercolor1,
			selectedTriggercolor1
		};
		video::SColor const selectedTriggercolor2 = video::SColor(255, 255, (s32)(255.0f * phase2), (s32)(255.0f * phase2));
		video::SColor const selectedVertexColors2[4] = {
			selectedTriggercolor2,
			selectedTriggercolor2,
			selectedTriggercolor2,
			selectedTriggercolor2
		};
		/*
		driver->draw2DImage(triggerCylinderIcon,
		core::rect<s32>(gunCylinderPositionX1, gunCylinderPositionY1, gunCylinderPositionX2, gunCylinderPositionY2),
		core::rect<s32>(0, 0, gunIconSize.Width, gunIconSize.Height), 0, vertexColors, true);
		*/
		/* draw "bullets" */
		core::dimension2d<u32> iconSize = triggerIcon->getSize();
		s32 maxItems = maxTriggerCounters - 1;
		for (s32 i = maxItems; i >= 0; --i){
			// calculate trigger position
			s32 startY, endY, startX1, endX1, startX2, endX2;
			calculateTriggerIconPosition(i, startY, endY, startX1, endX1, startX2, endX2);
			if (player1Triggers > maxItems - i){
				driver->draw2DImage(triggerIcon,
					core::rect<s32>(startX1, startY, endX1, endY),
					core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, 
					(player1->isTriggering() && maxItems - i == 0) ? selectedVertexColors1 : vertexColors, true);
			}
			if (player2Triggers > maxItems - i){
				driver->draw2DImage(triggerIcon,
					core::rect<s32>(startX2, startY, endX2, endY),
					core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, 
					(player2->isTriggering() && maxItems - i == 0) ? selectedVertexColors2 : vertexColors, true);
			}
		}
	}

	/* draw text */
	void FK_HUDManager_font::drawNormalText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
		video::SColor textColor, bool horizontalCentering, bool verticalCentering){
		font->draw(text.c_str(),
			destinationRect,
			textColor,
			horizontalCentering,
			verticalCentering);
	};
	// draw system text
	void FK_HUDManager_font::drawSystemText(std::wstring itemToDraw, core::rect<s32> destRect){
		SColor borderColor = video::SColor(200, 0, 0, 0);
		fk_addons::drawBorderedText(mainFont, itemToDraw, destRect, video::SColor(255, 255, 255, 255), borderColor);
	}

	// draw statistics text
	void FK_HUDManager_font::drawStatText(std::wstring itemToDraw, core::rect<s32> destRect){
		SColor borderColor = video::SColor(200, 0, 0, 0);
		fk_addons::drawBorderedText(font, itemToDraw, destRect, video::SColor(255, 255, 255, 255), borderColor);
	}

	// draw system text centered
	void FK_HUDManager_font::drawSystemTextCentered(std::wstring itemToDraw){
		dimension2d<u32> objSize = mainFont->getDimension(itemToDraw.c_str());
		core::rect<s32> destRect(screenSize.Width / 2 - objSize.Width / 2,
			screenSize.Height / 2 - objSize.Height / 2,
			screenSize.Width / 2 + objSize.Width / 2,
			screenSize.Height / 2 + objSize.Height / 2);
		drawSystemText(itemToDraw, destRect);
		drawBeautyRectangles(destRect);
	}

	// draw system text centered
	void FK_HUDManager_font::drawMultilineSystemTextCentered(std::vector<std::wstring> itemsToDraw){
		if (itemsToDraw.empty()){
			return;
		}
		u32 size = itemsToDraw.size();
		dimension2d<u32> itemSize = mainFont->getDimension(itemsToDraw[0].c_str());
		dimension2d<u32> totalSize = itemSize * size;
		s32 minY = screenSize.Height;
		s32 maxY = 0;
		s32 maxWidth = 0;
		for (int i = 0; i < itemsToDraw.size(); ++i){
			std::wstring itemToDraw = itemsToDraw[i];
			dimension2d<u32> objSize = mainFont->getDimension(itemToDraw.c_str());
			if (objSize.Width > maxWidth){
				maxWidth = objSize.Width;
			}
			s32 textY = screenSize.Height / 2 - (totalSize.Height / 2) + i*(objSize.Height);
			if ((textY + objSize.Height) > maxY){
				maxY = textY + objSize.Height;
			}
			if (textY < minY){
				minY = textY;
			}
			core::rect<s32> destRect(screenSize.Width / 2 - objSize.Width / 2,
				textY,
				screenSize.Width / 2 + objSize.Width / 2,
				textY + objSize.Height);
			FK_HUDManager_font::drawSystemText(itemToDraw, destRect);
		}
		core::rect<s32> textRect(screenSize.Width / 2 - maxWidth / 2, minY, 
			screenSize.Width / 2 + maxWidth / 2, maxY);
		drawBeautyRectangles(textRect);
	}

	/* draw beauty rects */
	void FK_HUDManager_font::drawBeautyRectangles(core::rect<s32> textRectangle){
		u32 rectSizeX = screenSize.Width / 4;
		if (rectSizeX < textRectangle.getWidth()){
			rectSizeX = textRectangle.getWidth();
		}
		u32 rectSizeY = screenSize.Height / 64;
		s32 offsetY = -(s32)screenSize.Height / 32;
		video::SColor rectColor(255, 255, 255, 255);
		video::SColor borderColor(200, 0, 0, 0);
		//driver->draw2DRectangle(rectColor, textRectangle);
		core::rect<s32> beautyRect1(screenSize.Width / 2 - rectSizeX / 2,
			textRectangle.UpperLeftCorner.Y - offsetY - rectSizeY,
			screenSize.Width / 2 + rectSizeX / 2,
			textRectangle.UpperLeftCorner.Y - offsetY);
		driver->draw2DRectangle(rectColor, beautyRect1);
		driver->draw2DRectangleOutline(beautyRect1, borderColor);
		offsetY = screenSize.Height / 128;
		core::rect<s32> beautyRect2(screenSize.Width / 2 - rectSizeX / 2,
			textRectangle.LowerRightCorner.Y + offsetY,
			screenSize.Width / 2 + rectSizeX / 2,
			textRectangle.LowerRightCorner.Y + offsetY + rectSizeY);
		driver->draw2DRectangle(rectColor, beautyRect2);
		driver->draw2DRectangleOutline(beautyRect2, borderColor);
	}

	void FK_HUDManager_font::drawTimeElapsed(u32 timeMS, u32 recordTimeMS) {
		u32 timeS = timeMS / 1000;
		u32 recordTimeS = recordTimeMS / 1000;
		u32 hours = timeS / 3600;
		u32 minutes = (timeS - hours * 3600) / 60;
		u32 seconds = (timeS - hours * 3600 - minutes * 60);
		u32 centiseconds = (timeMS - (hours * 3600 + minutes * 60 + seconds) * 1000) / 10;
		if (hours > 99) {
			timeMS = 356400000 + 3540000 + 59000 + 999;
			timeS = timeMS / 1000;
			hours = timeS / 3600;
			minutes = (timeS - hours * 3600) / 60;
			seconds = (timeS - hours * 3600 - minutes * 60);
			centiseconds = (timeMS - (hours * 3600 + minutes * 60 + seconds) * 1000) / 10;
		}
		u32 digits[8] = {
			centiseconds - (centiseconds / 10) * 10,
			centiseconds / 10,
			seconds - (seconds / 10) * 10,
			seconds / 10,
			minutes - (minutes / 10) * 10,
			minutes / 10,
			hours - (hours / 10) * 10,
			hours / 10
		};
		s32 characterWidth = font->getDimension(L"5").Width;
		s32 separatorWidth = font->getDimension(L":").Width;
		s32 sentenceHeight = font->getDimension(L"5").Height;
		s32 sentenceOffsetX = 8 * scale_factorX; // distance from screen center
		s32 sentenceOffsetY = 8 * scale_factorY + sentenceHeight; // distance from the bottom of the screen
		for (u32 i = 0; i < 8; ++i) {
			fk_addons::drawBorderedText(font, std::to_wstring(digits[i]), core::rect<s32>(
				screenSize.Width - sentenceOffsetX - characterWidth,
				screenSize.Height - sentenceHeight - sentenceOffsetY,
				screenSize.Width - sentenceOffsetX,
				screenSize.Height - sentenceOffsetY),
				video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
				true, false);
			sentenceOffsetX += characterWidth;
			if (i == 3 || i == 5 || i == 1) {
				fk_addons::drawBorderedText(font, L":", core::rect<s32>(
					screenSize.Width - sentenceOffsetX - separatorWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					video::SColor(255, 255, 255, 255), video::SColor(128, 0, 0, 0),
					true, false);
				sentenceOffsetX += separatorWidth;
			}
		}
		hours = recordTimeS / 3600;
		minutes = (recordTimeS - hours * 3600) / 60;
		seconds = (recordTimeS - hours * 3600 - minutes * 60);
		centiseconds = (recordTimeMS - (hours * 3600 + minutes * 60 + seconds) * 1000) / 10;
		if (hours > 99) {
			recordTimeMS = 356400000 + 3540000 + 59000 + 999;
			timeS = recordTimeS / 1000;
			hours = timeS / 3600;
			minutes = (timeS - hours * 3600) / 60;
			seconds = (timeS - hours * 3600 - minutes * 60);
			centiseconds = (timeMS - (hours * 3600 + minutes * 60 + seconds) * 1000) / 10;
		}
		u32 recordDigits[8] = {
			centiseconds - (centiseconds / 10) * 10,
			centiseconds / 10,
			seconds - (seconds / 10) * 10,
			seconds / 10,
			minutes - (minutes / 10) * 10,
			minutes / 10,
			hours - (hours / 10) * 10,
			hours / 10
		};
		sentenceOffsetX = 8 * scale_factorX;
		sentenceOffsetY -= sentenceHeight;
		video::SColor recordColor(255, 250, 250, 0);
		// draw record
		for (u32 i = 0; i < 8; ++i) {
			fk_addons::drawBorderedText(font, std::to_wstring(recordDigits[i]), core::rect<s32>(
				screenSize.Width - sentenceOffsetX - characterWidth,
				screenSize.Height - sentenceHeight - sentenceOffsetY,
				screenSize.Width - sentenceOffsetX,
				screenSize.Height - sentenceOffsetY),
				recordColor, video::SColor(128, 0, 0, 0),
				true, false);
			sentenceOffsetX += characterWidth;
			if (i == 3 || i == 5 || i == 1) {
				fk_addons::drawBorderedText(font, L":", core::rect<s32>(
					screenSize.Width - sentenceOffsetX - separatorWidth,
					screenSize.Height - sentenceHeight - sentenceOffsetY,
					screenSize.Width - sentenceOffsetX,
					screenSize.Height - sentenceOffsetY),
					recordColor, video::SColor(128, 0, 0, 0),
					true, false);
				sentenceOffsetX += separatorWidth;
			}
		}
		std::wstring recordString = L"Record:";
		s32 sentenceWidth = font->getDimension(recordString.c_str()).Width;
		sentenceOffsetX += separatorWidth;
		fk_addons::drawBorderedText(font, recordString, core::rect<s32>(
			screenSize.Width - sentenceOffsetX - sentenceWidth,
			screenSize.Height - sentenceHeight - sentenceOffsetY,
			screenSize.Width - sentenceOffsetX,
			screenSize.Height - sentenceOffsetY),
			recordColor, video::SColor(128, 0, 0, 0),
			false, false);
	}
	void FK_HUDManager_font::drawSurvivalRecord(u32 numberOfWins, u32 recordNumberOfWins) {
		std::wstring winMatchName = L"wins";
		std::wstring currentWins = std::to_wstring(numberOfWins) + L" " + winMatchName;
		std::wstring recordWins = L"Record: " + std::to_wstring(recordNumberOfWins) + L" " + winMatchName;
		s32 sentenceWidth = font->getDimension(currentWins.c_str()).Width;
		s32 separatorWidth = font->getDimension(L":").Width;
		s32 sentenceHeight = font->getDimension(L"5").Height;
		s32 sentenceOffsetX = 8 * scale_factorX; // distance from screen center
		s32 sentenceOffsetY = 8 * scale_factorY + sentenceHeight;
		video::SColor recordColor(255, 250, 250, 0);
		fk_addons::drawBorderedText(font, currentWins, core::rect<s32>(
			screenSize.Width - sentenceOffsetX - sentenceWidth,
			screenSize.Height - sentenceHeight - sentenceOffsetY,
			screenSize.Width - sentenceOffsetX,
			screenSize.Height - sentenceOffsetY),
			video::SColor(255,255,255,255), video::SColor(128, 0, 0, 0),
			false, false);
		sentenceWidth = font->getDimension(recordWins.c_str()).Width;
		sentenceOffsetY -= sentenceHeight;
		fk_addons::drawBorderedText(font, recordWins, core::rect<s32>(
			screenSize.Width - sentenceOffsetX - sentenceWidth,
			screenSize.Height - sentenceHeight - sentenceOffsetY,
			screenSize.Width - sentenceOffsetX,
			screenSize.Height - sentenceOffsetY),
			recordColor, video::SColor(128, 0, 0, 0),
			false, false);
	}

	/* draw "[PLAYER NAME] wins" text */
	void FK_HUDManager_font::drawWin(int winnerId, bool matchEndFlag){
		bool player1Won = winnerId == -1;
		if ((player1Won ? quoteToDrawPl1 : quoteToDrawPl2) == std::wstring()){
			(player1Won ? quoteToDrawPl1 : quoteToDrawPl2) =
				(player1Won ? player1->getRandomWinQuote(player2->getName(), matchEndFlag) :
				player2->getRandomWinQuote(player1->getName(), matchEndFlag));
		}
		// draw "WIN" statement
		std::vector<std::wstring> textToDraw;
		std::wstring playerName;
		if (player1Won){
			std::wstring temp = player1->getWDisplayName();
			playerName = std::wstring(temp.begin(), temp.end());
		}
		else{
			std::wstring temp = player2->getWDisplayName();
			playerName = std::wstring(temp.begin(), temp.end());
		}
		textToDraw.push_back(playerName);
		textToDraw.push_back(hudStrings[FK_HUD_Strings::PlayerWinText]);
		drawMultilineSystemTextCentered(textToDraw);
		// draw winner sentence
		std::wstring quoteToDraw = player1Won ? quoteToDrawPl1 : quoteToDrawPl2;
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
	void FK_HUDManager_font::drawIntroText(int playerId){
		bool player1IntroFlag = playerId == -1;
		if ((player1IntroFlag ? quoteToDrawPl1 : quoteToDrawPl2) == std::wstring()){
			(player1IntroFlag ? quoteToDrawPl1 : quoteToDrawPl2) =
				(player1IntroFlag ? player1->getRandomIntroQuote(player2->getName()) :
				player2->getRandomIntroQuote(player1->getName()));
		}
		// draw intro sentence
		std::wstring quoteToDraw = player1IntroFlag ? quoteToDrawPl1 : quoteToDrawPl2;
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
			video::SColor(255, 0, 0, 0), true);
	}

	/* draw match sentence */
	void FK_HUDManager_font::drawMatchSentence(std::string speakerName, std::string sentence){
		// draw sentence
		std::wstring temp(sentence.begin(), sentence.end());
		s32 sentenceWidth = textFont->getDimension(temp.c_str()).Width;
		s32 sentenceHeight = textFont->getDimension(temp.c_str()).Height;
		s32 sentenceOffset = 8;
		s32 sentenceX = screenSize.Width / 2 - sentenceWidth / 2;
		s32 sentenceY = screenSize.Height - sentenceHeight - sentenceOffset * scale_factorY;
		core::rect<s32> destinationRect(sentenceX,
			sentenceY,
			sentenceX + sentenceWidth,
			screenSize.Height - sentenceOffset * scale_factorY);
		// draw back semi-transparent rectangle for better reading in bright stages
		video::SColor color(255, 255, 255, 255);
		video::SColor colorAry[4] = { color, color, color, color };
		core::rect<s32> sourceRect(0, 0, dialogueTextBack->getSize().Width, dialogueTextBack->getSize().Height);
		driver->draw2DImage(dialogueTextBack, destinationRect,
			sourceRect, 0, colorAry, true);
		sourceRect = core::rect<s32>(0, 0, dialogueTextBackLBorder->getSize().Width, dialogueTextBackLBorder->getSize().Height);
		core::rect<s32> destinationRectL(sentenceX - sentenceHeight,
			sentenceY,
			sentenceX,
			screenSize.Height - sentenceOffset * scale_factorY);
		driver->draw2DImage(dialogueTextBackLBorder, destinationRectL,
			sourceRect, 0, colorAry, true);
		sourceRect = core::rect<s32>(0, 0, dialogueTextBackRBorder->getSize().Width, dialogueTextBackRBorder->getSize().Height);
		core::rect<s32> destinationRectR(sentenceX + sentenceWidth,
			sentenceY,
			sentenceX + sentenceWidth + sentenceHeight,
			screenSize.Height - sentenceOffset * scale_factorY);
		driver->draw2DImage(dialogueTextBackRBorder, destinationRectR,
			sourceRect, 0, colorAry, true);
		// draw text (finally)
		fk_addons::drawBorderedText(textFont, temp, destinationRect,
			video::SColor(255, 255, 255, 255),
			video::SColor(255, 0, 0, 0), false);
		// draw speaker name
		temp = std::wstring(speakerName.begin(), speakerName.end());
		sentenceWidth = textFont->getDimension(temp.c_str()).Width;
		sentenceHeight = textFont->getDimension(temp.c_str()).Height;
		sentenceOffset = 2 * scale_factorY;
		sentenceY -= sentenceOffset;
		// draw back semi-transparent rectangle for better reading in bright stages
		destinationRect = core::rect<s32>(sentenceX,
			sentenceY - sentenceHeight,
			sentenceX + sentenceWidth,
			sentenceY);
		fk_addons::drawBorderedText(font, temp, destinationRect,
			video::SColor(255, 0, 100, 255),
			video::SColor(255, 0, 0, 0), false);
	}

	/* draw stage information */
	void FK_HUDManager_font::drawStageIntroText(FK_Stage* stage){
		// draw stage name
		std::wstring quoteToDraw = stage->getName();
		std::wstring temp(quoteToDraw.begin(), quoteToDraw.end());
		s32 sentenceWidth = font->getDimension(temp.c_str()).Width;
		s32 sentenceHeight = font->getDimension(temp.c_str()).Height;
		s32 sentenceOffsetX = 32;
		s32 sentenceOffsetY = 16;
		core::rect<s32> destinationRect(sentenceOffsetX * scale_factorX,
			sentenceOffsetY * scale_factorY,
			sentenceWidth + sentenceOffsetX * scale_factorX,
			sentenceHeight + sentenceOffsetY * scale_factorY);
		video::SColor textColor = video::SColor(255, 255, 255, 255);
		video::SColor borderColor = video::SColor(100, 0, 0, 0);
		// draw stage name
		if (showStageName) {
			fk_addons::drawBorderedText(font, temp, destinationRect, textColor, borderColor);
		}
		// draw stage description
		quoteToDraw = stage->getDescription();
		std::wstring caption(quoteToDraw.begin(), quoteToDraw.end());
		s32 captionWidth = textFont->getDimension(caption.c_str()).Width;
		s32 captionHeight = textFont->getDimension(caption.c_str()).Height;
		s32 captionOffsetX = sentenceOffsetX;
		s32 captionOffsetY = sentenceOffsetY * scale_factorY + sentenceHeight;
		if (showStageDescription) {
			destinationRect = core::rect<s32>(captionOffsetX * scale_factorX,
				captionOffsetY,
				captionWidth + captionOffsetX * scale_factorX,
				captionHeight + captionOffsetY);
			// draw caption
			fk_addons::drawBorderedText(textFont, caption, destinationRect, textColor, borderColor);
		}
		captionOffsetY = screenSize.Height - sentenceOffsetY * scale_factorY;
		if (showStageBGMAuthor && !stage->getBGMMetaData().second.empty()) {
			std::wstring caption = stage->getBGMMetaData().second;
			caption = caption;
			captionWidth = textFont->getDimension(caption.c_str()).Width;
			captionHeight = textFont->getDimension(caption.c_str()).Height;
			captionOffsetY -= captionHeight;
			destinationRect = core::rect<s32>(screenSize.Width - captionOffsetX * scale_factorX - captionWidth,
				captionOffsetY,
				screenSize.Width - captionOffsetX * scale_factorX,
				captionHeight + captionOffsetY);
			// draw caption
			fk_addons::drawBorderedText(textFont, caption, destinationRect, textColor, borderColor);
		}
		if (showStageBGMName && !stage->getBGMMetaData().first.empty()) {
			std::wstring caption = stage->getBGMMetaData().first;
			caption = L"♪ " + caption;
			captionWidth = textFont->getDimension(caption.c_str()).Width;
			captionHeight = textFont->getDimension(caption.c_str()).Height;
			captionOffsetY -= captionHeight;
			destinationRect = core::rect<s32>(screenSize.Width - captionOffsetX * scale_factorX - captionWidth,
				captionOffsetY,
				screenSize.Width - captionOffsetX * scale_factorX,
				captionHeight + captionOffsetY);
			// draw caption
			textColor = video::SColor(255, 0, 250, 25);
			borderColor = video::SColor(255, 0, 64, 0);
			fk_addons::drawBorderedText(textFont, caption, destinationRect, textColor, borderColor);
		}
	}

	/* draw "PAUSE" text */
	void FK_HUDManager_font::drawPause(){
		// draw black semi-transparent black screen
		core::rect<s32> screenRect = core::rect<s32>(0, 0, (s32)screenSize.Width, (s32)screenSize.Height);
		driver->draw2DRectangle(video::SColor(150, 0, 0, 0), screenRect);
		// draw pause text
		/*SColor borderColor = video::SColor(128, 64, 64, 64);
		std::wstring itemToDraw = hudStrings[FK_HUD_Strings::PauseText];
		dimension2d<u32> objSize = mainFont->getDimension(itemToDraw.c_str());
		core::rect<s32> destRect(screenSize.Width / 2 - objSize.Width / 2,
			screenSize.Height / 4 - objSize.Height / 2,
			screenSize.Width / 2 + objSize.Width / 2,
			screenSize.Height / 4 + objSize.Height / 2);
		drawSystemText(itemToDraw, destRect);
		drawBeautyRectangles(destRect);*/
	}

	/* draw [GAME OVER] */
	void FK_HUDManager_font::drawGameOver(){
		drawSystemTextCentered(L"Game Over");
	}

	/* draw [GAME OVER] */
	void FK_HUDManager_font::drawNewRecord() {
		drawSystemTextCentered(L"New Record");
	}

	/* draw [CONTINUE?]*/
	void FK_HUDManager_font::drawContinue(s32 secondsToGo){
		SColor borderColor = video::SColor(128, 64, 64, 64);
		std::wstring itemToDraw = hudStrings[FK_HUD_Strings::ContinueText];
		dimension2d<u32> objSize = mainFont->getDimension(itemToDraw.c_str());
		core::rect<s32> destRect(screenSize.Width / 2 - objSize.Width / 2,
			screenSize.Height / 2 - objSize.Height,
			screenSize.Width / 2 + objSize.Width / 2,
			screenSize.Height / 2);
		drawSystemText(itemToDraw, destRect);
		drawBeautyRectangles(destRect);
		if (secondsToGo < 10){
			if (secondsToGo < 0){
				secondsToGo = 0;
			}
			itemToDraw = std::to_wstring(secondsToGo);
			objSize = mainFont->getDimension(itemToDraw.c_str());
			destRect = core::rect<s32>(screenSize.Width / 2 - objSize.Width / 2,
				screenSize.Height / 2,
				screenSize.Width / 2 + objSize.Width / 2,
				screenSize.Height / 2 + objSize.Height / 2);
			drawSystemText(itemToDraw, destRect);
		}
		//drawBeautyRectangles(destRect);
	}

	/* draw "ROUND" text */
	void FK_HUDManager_font::drawRound(int currentRound, bool isFinalRound){
		if (isFinalRound){
			/* draw round text*/
			// draw "WIN" statement
			std::vector<std::wstring> textToDraw;
			textToDraw.push_back(hudStrings[FK_HUD_Strings::FinalText]);
			textToDraw.push_back(hudStrings[FK_HUD_Strings::RoundText]);
			drawMultilineSystemTextCentered(textToDraw);
		}
		else{
			/* draw round text*/
			drawSystemTextCentered(hudStrings[FK_HUD_Strings::RoundText] + L" " + std::to_wstring(currentRound));
		}
	}

	/* draw timer */
	void FK_HUDManager_font::drawTimer(u32 time){
		/* timer > 99 implies infinite time match */
		if (time > fk_constants::FK_MaxRoundTimerSeconds){
			return;
		}
		if (time < 0){
			time = 0;
		}
		u32 units = time % 10;
		u32 tens = time / 10;
		video::SColor borderColor(200, 64, 64, 64);
		video::SColor fontColor(255, 255, 255, 255);
		std::wstring itemToDrawUnits = std::to_wstring(units);
		std::wstring itemToDrawTens = std::to_wstring(tens);
		u32 offsetY = (u32)std::floor(timerVerticalPosition * scale_factorY);
		//u32 offsetX = screenSize.Height / 64;
		dimension2d<u32> objSize = timerFont->getDimension(itemToDrawTens.c_str());
		core::rect<s32> destRect(screenSize.Width / 2 - objSize.Width,
			offsetY,
			screenSize.Width / 2,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(timerFont, itemToDrawTens, destRect, fontColor, borderColor);
		objSize = timerFont->getDimension(itemToDrawUnits.c_str());
		destRect = core::rect<s32>(screenSize.Width / 2,
			offsetY,
			screenSize.Width / 2 + objSize.Width,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(timerFont, itemToDrawUnits, destRect, fontColor, borderColor);
	}

	/* draw "FIGHT" text */
	void FK_HUDManager_font::drawFightText(){
		drawSystemTextCentered(hudStrings[FK_HUD_Strings::FightText]);
	}

	/* draw "KO" text */
	void FK_HUDManager_font::drawKO(bool perfect){
		if (perfect){
			drawSystemTextCentered(hudStrings[FK_HUD_Strings::PerfectText]);
		}
		else{
			drawSystemTextCentered(hudStrings[FK_HUD_Strings::KOText]);
		}
	}

	/* draw "SUCCESS" text */
	void FK_HUDManager_font::drawSuccessText() {
		drawSystemTextCentered(hudStrings[FK_HUD_Strings::SuccessText]);
	}

	/* draw "RING OUT" text */
	void FK_HUDManager_font::drawRingout(){
		drawSystemTextCentered(hudStrings[FK_HUD_Strings::RingoutText]);
	}

	/* draw "DOUBLE RING OUT" text */
	void FK_HUDManager_font::drawDoubleRingout(){
		std::vector<std::wstring> textToDraw = {
			hudStrings[FK_HUD_Strings::DoubleText],
			hudStrings[FK_HUD_Strings::RingoutText]
		};
		drawMultilineSystemTextCentered(textToDraw);
	}

	/* draw "DRAW" text */
	void FK_HUDManager_font::drawDraw(){
		drawSystemTextCentered(hudStrings[FK_HUD_Strings::DrawResultText]);
	}

	/* draw "TIME UP" text */
	void FK_HUDManager_font::drawTimeUp(){
		drawSystemTextCentered(hudStrings[FK_HUD_Strings::TimeUpText]);
	}

	/* draw "DOUBLE KO" text */
	void FK_HUDManager_font::drawDoubleKO(){
		std::wstring itemToDraw = hudStrings[FK_HUD_Strings::DoubleText] + L" " + hudStrings[FK_HUD_Strings::KOText];
		drawSystemTextCentered(itemToDraw);
	}

	// draw single icon
	void FK_HUDManager_font::drawIcon(video::ITexture* icon, s32 x, s32 y, s32 width, s32 height, s32 opacity){
		if (!icon){
			return;
		}
		// scale viewport
		core::dimension2d<u32> iconSize = icon->getSize();
		if (opacity >= 255){
			fk_addons::drawBatch2DImage(device->getVideoDriver(), icon, core::rect<s32>(0, 0, iconSize.Width, iconSize.Height),
				core::position2d<s32>(x, y), core::vector2d<s32>(0, 0), 0,
				core::vector2d<f32>((f32)width / iconSize.Width, (f32)height / iconSize.Height),
				true,
				video::SColor(255, 255, 255, 255),
				core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else{
			video::SColor color(opacity, 255, 255, 255);
			//color.setAlpha(opacity);
			video::SColor const vertexColors[4] = {
				color,
				color,
				color,
				color
			};
			device->getVideoDriver()->draw2DImage(icon, core::rect<s32>(x, y,
				x + width, y + height),
				core::rect<s32>(0, 0, iconSize.Width, iconSize.Height), 0, vertexColors, true);
		}
	};

	// draw end match statistics
	void FK_HUDManager_font::drawEndMatchStatistics(FK_FreeMatchStatistics matchStatistics){
		//draw Player 1 statistics
		std::wstring itemToDraw = L"Player 1";
		dimension2d<u32> objSize = mainFont->getDimension(itemToDraw.c_str());
		s32 x1 = screenSize.Width / 64;
		s32 y1 = screenSize.Height / 32;
		core::rect<s32> destRect(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawSystemText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfWins(-1)) + L" Wins";
		objSize = font->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfRoundWins(-1)) + L" Rounds Won";
		objSize = font->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfPerfectVictories(-1)) + L" Perfect Wins";
		objSize = font->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfRingoutVictories(-1)) + L" Ring Out";
		objSize = font->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring((u32)(floor(matchStatistics.getAverageLifeRatio(1)))) + L" dmg / round";
		objSize = font->getDimension(itemToDraw.c_str());
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		// draw Player 2 statistics
		itemToDraw = L"Player 2";
		objSize = mainFont->getDimension(itemToDraw.c_str());
		x1 = screenSize.Width - (objSize.Width + screenSize.Width / 64);
		y1 = screenSize.Height / 32;
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawSystemText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfWins(1)) + L" Wins";
		objSize = font->getDimension(itemToDraw.c_str());
		x1 = screenSize.Width - (objSize.Width + screenSize.Width / 64);
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfRoundWins(1)) + L" Rounds Won";
		objSize = font->getDimension(itemToDraw.c_str());
		x1 = screenSize.Width - (objSize.Width + screenSize.Width / 64);
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfPerfectVictories(1)) + L" Perfect Wins";
		objSize = font->getDimension(itemToDraw.c_str());
		x1 = screenSize.Width - (objSize.Width + screenSize.Width / 64);
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring(matchStatistics.getNumberOfRingoutVictories(1)) + L" Ring Out";
		objSize = font->getDimension(itemToDraw.c_str());
		x1 = screenSize.Width - (objSize.Width + screenSize.Width / 64);
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
		y1 += objSize.Height;
		itemToDraw = std::to_wstring((u32)(floor(matchStatistics.getAverageLifeRatio(-1)))) + L" dmg / round";
		objSize = font->getDimension(itemToDraw.c_str());
		x1 = screenSize.Width - (objSize.Width + screenSize.Width / 64);
		destRect = core::rect<s32>(x1, y1, x1 + objSize.Width, y1 + objSize.Height);
		drawStatText(itemToDraw, destRect);
	}

	// draw input buffer
	void FK_HUDManager_font::drawInputBuffer(FK_InputBuffer* buffer){
		s32 y0 = (s32)floor(scale_factorY * trainingInputCoordinates.UpperLeftCorner.Y); //screenSize.Height / 5;
		s32 x0 = (s32)floor(scale_factorX * trainingInputCoordinates.UpperLeftCorner.X);
		s32 iconSize = (s32)floor(scale_factorX * trainingInputCoordinates.LowerRightCorner.X);
		s32 iconStandardOffset = (s32)floor(scale_factorY * trainingInputCoordinates.LowerRightCorner.Y);
		s32 iconIndex = 0;
		s32 historySize = (s32)buffer->getHistoryBuffer().size();
		s32 maxSize = std::min(historySize, trainingInputMaxBufferSize);
		s32 startingIndex = historySize - maxSize;
		if (maxSize < 0) {
			maxSize = 0;
		}
		for (int i = startingIndex; i < historySize; ++i){
			s32 x1 = x0;
			s32 y1 = y0 + iconIndex * iconStandardOffset;
			int button = buffer->getBufferElementFromHistory(i);
			if (button <= 0)
				continue;
			if (button != (u32)FK_Input_Buttons::Neutral_Input){
				iconIndex += 1;
			}
			u32 buttonCount = 0;
			u32 directionButton = button & (u32)FK_Input_Buttons::Any_Direction_Plus_Held;
			u32 punchButton = button & (u32)FK_Input_Buttons::Punch_Button;
			u32 kickButton = button & (u32)FK_Input_Buttons::Kick_Button;
			u32 techButton = button & (u32)FK_Input_Buttons::Tech_Button;
			u32 guardButton = button & (u32)FK_Input_Buttons::Guard_Button;
			u32 triggerButton = button & (u32)FK_Input_Buttons::Trigger_Button;
			if (buttonTexturesMap.find((FK_Input_Buttons)directionButton) != buttonTexturesMap.end()){
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)directionButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)triggerButton) != buttonTexturesMap.end()) {
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)triggerButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)guardButton) != buttonTexturesMap.end()){
				if (buttonCount > 0) {
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)guardButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)punchButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)punchButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)kickButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)kickButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)techButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)techButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize);
				x1 += iconSize;
				buttonCount += 1;
			}
		}
	};

	//set render screen size
	void FK_HUDManager_font::setViewportSize(core::dimension2d<u32> new_size){
		screenSize = core::dimension2d<u32>(new_size.Width, new_size.Height);
		// scale viewport
		scale_factorX = screenSize.Width / referenceScreenSize.Width;
		scale_factorY = screenSize.Height / referenceScreenSize.Height;
	}

	void FK_HUDManager_font::drawMoveName(FK_Character* character){
		if (character->getCurrentMove() != NULL){
			if ((currentMoveClearBuffer ||
				character->getCurrentMove()->getDisplayName() != currentMoveName) &&
				character->getCurrentMove()->isShownInHUD()){
				currentMoveNameTimer = 0;
				currentMoveName = character->getCurrentMove()->getDisplayName();
				if (character->getCurrentMove()->isThrow() && !moveNameDisplayPreviousMoveWasAThrow){
					currentMoveClearBuffer = false;
					moveNameDisplayPreviousMoveWasAThrow = true;
					//currentMoveNameToDisplay = currentMove.getDisplayName();
				}
				else if (character->getCurrentMove()->isFollowupOnly() && (!currentMove.isFollowupOnly() || moveNameDisplayPreviousMoveWasAThrow) &&
					!character->getCurrentMove()->hasForcedNameDisplay()){
					currentMoveNameToDisplay = currentMoveNameToDisplay /*currentMove.getWDisplayName()*/ + L" (Chain)";
					moveNameDisplayPreviousMoveWasAThrow = false;
				}
				else if (!character->getCurrentMove()->isFollowupOnly() || character->getCurrentMove()->hasForcedNameDisplay()){
					currentMoveNameToDisplay = character->getCurrentMove()->getWDisplayName();
					moveNameDisplayPreviousMoveWasAThrow = false;
				}
				auto tempBuffer = currentMove.getInputStringLeft();
				currentMove = FK_Move(*character->getCurrentMove());
				if (currentMoveClearBuffer){
					currentMoveClearBuffer = false;
					tempBuffer.clear();
					isInputTriggered.clear();
					//std::cout << "clearing..." << std::endl;
				}
				// add additional inputs for making it clear which stance it's the first move performed
				u32 offsetPosition = 0;
				if (tempBuffer.empty()) {
					auto additionalBufferL = currentMove.getInputString(character->isOnLeftSide());
					auto additionalBufferR = currentMove.getInputString(character->isOnLeftSide());
					if (currentMove.getStance() == FK_Stance_Type::CrouchedStance) {
						additionalBufferL.push_front(FK_Input_Buttons::None);
						additionalBufferR.push_front(FK_Input_Buttons::None);
						additionalBufferL.push_front(FK_Input_Buttons::Down_Direction | FK_Input_Buttons::Guard_Button);
						additionalBufferR.push_front(FK_Input_Buttons::Down_Direction | FK_Input_Buttons::Guard_Button);
						offsetPosition = 2;
					}else if(currentMove.getStance() == FK_Stance_Type::AirStance) {
						additionalBufferL.push_front(FK_Input_Buttons::None);
						additionalBufferR.push_front(FK_Input_Buttons::None);
						additionalBufferL.push_front(FK_Input_Buttons::UpRight_Direction | FK_Input_Buttons::Guard_Button);
						additionalBufferR.push_front(FK_Input_Buttons::UpLeft_Direction | FK_Input_Buttons::Guard_Button);
						offsetPosition = 2;
					}
					else if (currentMove.getStance() == FK_Stance_Type::RunningStance) {
						additionalBufferL.push_front(FK_Input_Buttons::None);
						additionalBufferR.push_front(FK_Input_Buttons::None);
						additionalBufferL.push_front(FK_Input_Buttons::HeldRight_Direction);
						additionalBufferR.push_front(FK_Input_Buttons::HeldLeft_Direction);
						additionalBufferL.push_front(FK_Input_Buttons::Right_Direction);
						additionalBufferR.push_front(FK_Input_Buttons::Left_Direction);
						offsetPosition = 3;
					}
					currentMove.setInputStringLeft(additionalBufferL);
					currentMove.setInputStringRight(additionalBufferR);
				}

				bool previousTriggerState = false;
				bool characterTriggerActive = character->isTriggerModeActive();
				bool characterRunCancel = character->isInImpactCancelState();

				if (!tempBuffer.empty() && !currentMove.getInputStringLeft().empty()) {
					tempBuffer.push_back(FK_Input_Buttons::None);
					previousTriggerState = isInputTriggered[isInputTriggered.size() - 1];
					isInputTriggered.push_back(previousTriggerState);
				}
				for (int i = 0; i < currentMove.getInputStringLeft().size(); ++i){
					u32 currentInput = currentMove.getInputString(character->isOnLeftSide())[i];
					if (characterTriggerActive && 
						!previousTriggerState &&
						!currentMove.getTriggerModeActivationFlag() &&
						!characterRunCancel &&
						i >= offsetPosition &&
						(currentInput & FK_Input_Buttons::Any_NonDirectionButton)) {
						currentInput |= FK_Input_Buttons::Trigger_Button;
						previousTriggerState = true;
					}
					tempBuffer.push_back(currentInput);
					isInputTriggered.push_back(characterTriggerActive);
				}
				currentMove.setInputStringLeft(tempBuffer);
			}
		}
		else{
			if (character->getNextMove() == NULL){
				currentMoveClearBuffer = true;
			}
		}
		if (currentMoveNameTimer <= currentMoveNameDuration + currentMoveNameFadeoutDuration){
			if (!currentMoveName.empty()){
				std::wstring temp = currentMoveNameToDisplay;
				//std::replace(temp.begin(), temp.end(), L'_', L' ');
				s32 sentenceWidth = textFont->getDimension(temp.c_str()).Width;
				s32 sentenceHeight = textFont->getDimension(temp.c_str()).Height;
				s32 sentenceOffset = 8;
				s32 sentenceX = screenSize.Width / 2 - sentenceWidth / 2;
				s32 sentenceY = screenSize.Height - sentenceHeight - sentenceOffset * scale_factorY;
				core::rect<s32> destinationRect(sentenceX,
					sentenceY,
					sentenceX + sentenceWidth,
					screenSize.Height - sentenceOffset * scale_factorY);
				s32 opacity = (currentMoveNameTimer > currentMoveNameDuration ?
					2 + (s32)std::round(255.0f*(1 - (f32)(currentMoveNameTimer - currentMoveNameDuration) /
					currentMoveNameFadeoutDuration)) :
					255);
				// draw back semi-transparent rectangle for better reading in bright stages
				video::SColor color(255, 255, 255, 255);
				video::SColor colorAry[4] = { color, color, color, color };
				core::rect<s32> sourceRect(0, 0, dialogueTextBack->getSize().Width, dialogueTextBack->getSize().Height);
				driver->draw2DImage(dialogueTextBack, destinationRect,
					sourceRect, 0, colorAry, true);
				sourceRect = core::rect<s32>(0, 0, dialogueTextBackLBorder->getSize().Width, dialogueTextBackLBorder->getSize().Height);
				core::rect<s32> destinationRectL(sentenceX - sentenceHeight,
					sentenceY,
					sentenceX,
					screenSize.Height - sentenceOffset * scale_factorY);
				driver->draw2DImage(dialogueTextBackLBorder, destinationRectL,
					sourceRect, 0, colorAry, true);
				sourceRect = core::rect<s32>(0, 0, dialogueTextBackRBorder->getSize().Width, dialogueTextBackRBorder->getSize().Height);
				core::rect<s32> destinationRectR(sentenceX + sentenceWidth,
					sentenceY,
					sentenceX + sentenceWidth + sentenceHeight,
					screenSize.Height - sentenceOffset * scale_factorY);
				driver->draw2DImage(dialogueTextBackRBorder, destinationRectR,
					sourceRect, 0, colorAry, true);
				// draw move input
				//sentenceX = screenSize.Width / 3;
				drawMoveInput(currentMove, 0, sentenceY - sentenceHeight, opacity);
				// then, draw move name
				fk_addons::drawBorderedText(textFont, temp, destinationRect,
					video::SColor(opacity, 255, 255, 255),
					video::SColor(opacity / 2, 0, 0, 0), false);
			}
		}
		else{
			currentMoveNameTimer = 0;
			currentMoveName = std::string();
			currentMove.clear();
		}
	}

	void FK_HUDManager_font::drawInputFromMove(FK_Move &move, bool leftSide, bool trigger){
		std::wstring temp = move.getWDisplayName();
		if (!temp.empty()) {
			s32 sentenceWidth = textFont->getDimension(temp.c_str()).Width;
			s32 sentenceHeight = textFont->getDimension(temp.c_str()).Height;
			s32 sentenceOffset = 8;
			s32 sentenceX = screenSize.Width / 2 - sentenceWidth / 2;
			s32 sentenceY = screenSize.Height - sentenceHeight - sentenceOffset * scale_factorY;
			core::rect<s32> destinationRect(sentenceX,
				sentenceY,
				sentenceX + sentenceWidth,
				screenSize.Height - sentenceOffset * scale_factorY);
			s32 opacity = (currentMoveNameTimer > currentMoveNameDuration ?
				2 + (s32)std::round(255.0f*(1 - (f32)(currentMoveNameTimer - currentMoveNameDuration) /
					currentMoveNameFadeoutDuration)) :
				255);
			opacity = 255;
			// draw back semi-transparent rectangle for better reading in bright stages
			video::SColor color(255, 255, 255, 255);
			video::SColor colorAry[4] = { color, color, color, color };
			core::rect<s32> sourceRect(0, 0, dialogueTextBack->getSize().Width, dialogueTextBack->getSize().Height);
			driver->draw2DImage(dialogueTextBack, destinationRect,
				sourceRect, 0, colorAry, true);
			sourceRect = core::rect<s32>(0, 0, dialogueTextBackLBorder->getSize().Width, dialogueTextBackLBorder->getSize().Height);
			core::rect<s32> destinationRectL(sentenceX - sentenceHeight,
				sentenceY,
				sentenceX,
				screenSize.Height - sentenceOffset * scale_factorY);
			driver->draw2DImage(dialogueTextBackLBorder, destinationRectL,
				sourceRect, 0, colorAry, true);
			sourceRect = core::rect<s32>(0, 0, dialogueTextBackRBorder->getSize().Width, dialogueTextBackRBorder->getSize().Height);
			core::rect<s32> destinationRectR(sentenceX + sentenceWidth,
				sentenceY,
				sentenceX + sentenceWidth + sentenceHeight,
				screenSize.Height - sentenceOffset * scale_factorY);
			driver->draw2DImage(dialogueTextBackRBorder, destinationRectR,
				sourceRect, 0, colorAry, true);
			// draw move input
			//sentenceX = screenSize.Width / 3;
			isInputTriggered.clear();
			isInputTriggered = std::deque<bool>(move.getInputString(leftSide).size(), trigger);
			//for (int i = 0; i < move.getInputString(leftSide).size(); ++i){
			//	isInputTriggered.push_back(trigger);
			//}
			drawMoveInput(move, 0, sentenceY - sentenceHeight, opacity, leftSide);
			fk_addons::drawBorderedText(textFont, temp, destinationRect,
				video::SColor(opacity, 255, 255, 255),
				video::SColor(opacity / 2, 0, 0, 0), false);
		}
	}

	void FK_HUDManager_font::drawInputRecordTimeBar(s32 currentTimeMs, s32 maxTimeMs, 
		FK_HUDManager_font::TrainingRecordModePhase recordPhase)
	{
		s32 barLength = recordTimeBarLocation.LowerRightCorner.X;
		s32 barHeight = recordTimeBarLocation.LowerRightCorner.Y;
		barLength = (s32)std::round((f32)barLength * scale_factorX);
		barHeight = (s32)std::round((f32)barHeight * scale_factorY);
		if ((barLength % 2) != 0) {
			barLength += 1;
		}
		if ((barHeight % 2) != 0) {
			barHeight += 1;
		}
		s32 x11 = recordTimeBarLocation.UpperLeftCorner.X;
		s32 y21 = recordTimeBarLocation.UpperLeftCorner.Y;
		x11 = (s32)std::round((f32)x11 * scale_factorX);
		y21 = (s32)std::round((f32)y21 * scale_factorY);
		s32 x22 = screenSize.Width - x11;

		video::SColor backgroundColor = video::SColor(1, 0, 0, 0);
		video::SColor borderColor = video::SColor(255,255,255,255);

		video::SColor mainColor = video::SColor(255, 30, 180, 220);
		video::SColor gradientColor = video::SColor(255, 0, 60, 220);

		s32 lineWidth = (s32)std::ceil(1.f *scale_factorX);

		// draw basic black box
		s32 x1 = x11;
		s32 y1 = y21;
		s32 x2 = x1 + barLength + 2 * lineWidth;
		s32 y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(backgroundColor, core::rect<s32>(x1, y1, x2, y2));
		// draw trigger regen bar
		x2 = x1 + (s32)std::round((barLength + 2 * lineWidth) * (1.0f - (f32)(currentTimeMs) / (f32)maxTimeMs));
		driver->draw2DRectangle(core::rect<s32>(x1, y1, x2, y2),
			gradientColor, mainColor, gradientColor, mainColor);
		// draw border
		x1 = x11;
		y1 = y21;
		x2 = x1 + barLength + 2 * lineWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = x11;
		y1 = y21 + barHeight + lineWidth;;
		x2 = x1 + barLength + 2 * lineWidth;
		y2 = y1 + lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = x11;
		y1 = y21;
		x2 = x1 + lineWidth;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
		x1 = x11 + barLength + lineWidth;
		y1 = y21;
		x2 = x1 + lineWidth;
		y2 = y1 + barHeight + 2 * lineWidth;
		driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));

		// draw text
		f32 recordBarFlashPeriod = 500.f;
		f32 phase = -0.5*std::cos((f32)currentTimeMs / recordBarFlashPeriod * 2 * core::PI) + 0.5;
		video::SColor recordTextColor = video::SColor(128, 255, 255, 255);
		video::SColor addonColor = video::SColor(
			(s32)(recordTextColor.getAlpha() * phase),
			(s32)(recordTextColor.getRed()  * phase),
			(s32)(recordTextColor.getRed() * phase),
			(s32)(recordTextColor.getBlue() * phase)
		);
		std::wstring stringToDraw = recordInputText[recordPhase];
		s32 sentenceWidth = font->getDimension(stringToDraw.c_str()).Width;
		s32 sentenceHeight = font->getDimension(stringToDraw.c_str()).Height;
		s32 sentenceOffsetX = x11 + barLength / 2 - sentenceWidth / 2; // distance from screen center (default: 68)
		s32 sentenceOffsetY = y21 - sentenceHeight; // distance from the top of the screen
		core::rect<s32> destinationRect = core::rect<s32>(
			sentenceOffsetX,
			sentenceOffsetY,
			sentenceOffsetX + sentenceWidth,
			sentenceHeight + sentenceOffsetY);
		fk_addons::drawBorderedText(font, stringToDraw, destinationRect, recordTextColor + addonColor,
			irr::video::SColor(128, 0, 0, 0));
	}

	// set old life ratios for a more pleasant effect in survival mode
	void FK_HUDManager_font::setOldLifeRatios(f32 player1ratio, f32 player2ratio)
	{
		oldLifeRatioPlayer1 = player1ratio;
		oldLifeRatioPlayer2 = player2ratio;
	}

	// draw input buffer / returns last X used for an icon for chaining inputs
	void FK_HUDManager_font::drawMoveInput(FK_Move move, s32 x1, s32 y1, s32 opacity, bool leftSide){
		u32 bufferSize = move.getInputString(leftSide).size();
		auto buffer = move.getInputString(leftSide);
		if (bufferSize > 0 && buffer[bufferSize - 1] == FK_Input_Buttons::None) {
			bufferSize -= 1;
		}
		u32 iconSize = textFont->getDimension(L"A").Height;
		s32 buttonCount = 0;
		s32 offsetTotal = x1;
		x1 = 0;
		// first, calculate total size
		for (int i = 0; i < bufferSize; ++i){
			buttonCount = 0;
			int button = buffer[i];
			if (button < 0)
				continue;
			if (button == FK_Input_Buttons::None){
				if (i < bufferSize - 1) {
					if (isInputTriggered[i] && !isInputTriggered[i + 1]) {
						isInputTriggered[i] = false;
					}
				}
				x1 += iconSize / 4;
				continue;
			}
			u32 directionButton = button & (u32)FK_Input_Buttons::Any_Direction_Plus_Held;
			u32 punchButton = button & (u32)FK_Input_Buttons::Punch_Button;
			u32 kickButton = button & (u32)FK_Input_Buttons::Kick_Button;
			u32 techButton = button & (u32)FK_Input_Buttons::Tech_Button;
			u32 guardButton = button & (u32)FK_Input_Buttons::Guard_Button;
			u32 anyActionButton = button & (u32)FK_Input_Buttons::Any_Button;
			u32 triggerButton = button & (u32)FK_Input_Buttons::Trigger_Button;
			if (buttonTexturesMap.find((FK_Input_Buttons)directionButton) != buttonTexturesMap.end()){
				x1 += iconSize;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)guardButton) != buttonTexturesMap.end()){
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)triggerButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					x1 += iconSize;
				}
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)punchButton) != buttonTexturesMap.end()) {
				if (buttonCount > 0) {
					buttonCount = 0;
					x1 += iconSize;
				}
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)kickButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					x1 += iconSize;
				}
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)techButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					x1 += iconSize;
				}
				x1 += iconSize;
				buttonCount += 1;
			}
		}
		// then, draw the REAL icons (centered)
		x1 = offsetTotal + (screenSize.Width - x1) / 2;
		for (int i = 0; i < bufferSize; ++i){
			buttonCount = 0;
			int button = buffer[i];
			if (button < 0)
				continue;
			if (button == FK_Input_Buttons::None){
				if (isInputTriggered[i]){
					u32 baseTriggerIconSize = iconSize;
					u32 triggerIconSize = baseTriggerIconSize;
					video::ITexture* textureToDraw = triggerInputTex;
					drawIcon(textureToDraw, x1, y1 - (baseTriggerIconSize - iconSize), triggerIconSize / 4,
						baseTriggerIconSize, std::min(opacity, 254));
				}
				x1 += iconSize / 4;
				continue;
			}
			s32 additionalButtonCount = 0;
			u32 directionButton = button & (u32)FK_Input_Buttons::Any_Direction_Plus_Held;
			u32 triggerButton = button & (u32)FK_Input_Buttons::Trigger_Button;
			u32 punchButton = button & (u32)FK_Input_Buttons::Punch_Button;
			u32 kickButton = button & (u32)FK_Input_Buttons::Kick_Button;
			u32 techButton = button & (u32)FK_Input_Buttons::Tech_Button;
			u32 guardButton = button & (u32)FK_Input_Buttons::Guard_Button;
			u32 anyActionButton = button & ((u32)FK_Input_Buttons::Any_Button | (u32)FK_Input_Buttons::Any_Direction_Plus_Held);
			if (isInputTriggered.size() > i && isInputTriggered[i]){
				u32 baseTriggerIconSize = iconSize;
				u32 triggerIconSize = baseTriggerIconSize;
				if (buttonTexturesMap.find((FK_Input_Buttons)triggerButton) != buttonTexturesMap.end()) {
					additionalButtonCount += 1;
				}
				if (buttonTexturesMap.find((FK_Input_Buttons)guardButton) != buttonTexturesMap.end()){
					additionalButtonCount += additionalButtonCount > 0 ? 2 : 1;
				}
				if (buttonTexturesMap.find((FK_Input_Buttons)punchButton) != buttonTexturesMap.end()){
					additionalButtonCount += additionalButtonCount > 0 ? 2 : 1;
				}
				if (buttonTexturesMap.find((FK_Input_Buttons)kickButton) != buttonTexturesMap.end()){
					additionalButtonCount += additionalButtonCount > 0 ? 2 : 1;
				}
				if (buttonTexturesMap.find((FK_Input_Buttons)techButton) != buttonTexturesMap.end()){
					additionalButtonCount += additionalButtonCount > 0 ? 2 : 1;
				}
				if ((directionButton & anyActionButton) != 0){
					additionalButtonCount += 1;
				}
				/*triggerIconSize += (additionalButtonCount - 1) * baseTriggerIconSize;*/
				if (bufferSize > 1 && additionalButtonCount == 1 && (i == bufferSize - 1 || !isInputTriggered[i + 1]) &&
					(i > 0 && isInputTriggered[i - 1])){
					drawIcon(triggerInputTexRborder, x1, y1 - (baseTriggerIconSize - iconSize),
						triggerIconSize, baseTriggerIconSize, std::min(opacity, 254));
				}
				else if ((i == 0 && ((bufferSize > 1 && isInputTriggered[i + 1]) || additionalButtonCount > 1)) ||
					(i > 0 && !isInputTriggered[i - 1]) && (i < bufferSize - 1 || additionalButtonCount > 1)){
					drawIcon(triggerInputTexLborder, x1, y1 - (baseTriggerIconSize - iconSize),
						triggerIconSize, baseTriggerIconSize, std::min(opacity, 254));
				}
				else if (!(i > 0 && i < bufferSize - 1 && isInputTriggered[i - 1] &&
					isInputTriggered[i + 1]) && additionalButtonCount == 1){
					video::ITexture* textureToDraw = triggerInputTexSingle;
					drawIcon(textureToDraw, x1, y1 - (baseTriggerIconSize - iconSize), triggerIconSize,
						baseTriggerIconSize, std::min(opacity, 254));
				}else{
					video::ITexture* textureToDraw = triggerInputTex;
					drawIcon(textureToDraw, x1, y1 - (baseTriggerIconSize - iconSize), triggerIconSize,
						baseTriggerIconSize, std::min(opacity, 254));
				}
				additionalButtonCount -= 1;
				for (int j = 0; j < additionalButtonCount; ++j){
					s32 x0 = x1 + iconSize * (j + 1);
					if (j == additionalButtonCount - 1 && 
						(i == bufferSize - 1 || !isInputTriggered[i + 1])){
						drawIcon(triggerInputTexRborder, x0, y1 - (baseTriggerIconSize - iconSize),
							triggerIconSize, baseTriggerIconSize, std::min(opacity, 254));
					}
					else{
						drawIcon(triggerInputTex, x0, y1 - (baseTriggerIconSize - iconSize),
							triggerIconSize, baseTriggerIconSize, std::min(opacity, 254));
					}
				}
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)directionButton) != buttonTexturesMap.end()){
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)directionButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize, opacity);
				x1 += iconSize;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)triggerButton) != buttonTexturesMap.end()){
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)triggerButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize, opacity);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)guardButton) != buttonTexturesMap.end()) {
				if (buttonCount > 0) {
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize, opacity);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)guardButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize, opacity);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)punchButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize, opacity);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)punchButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize, opacity);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)kickButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize, opacity);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)kickButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize, opacity);
				x1 += iconSize;
				buttonCount += 1;
			}
			if (buttonTexturesMap.find((FK_Input_Buttons)techButton) != buttonTexturesMap.end()){
				if (buttonCount > 0){
					buttonCount = 0;
					drawIcon(plusSignTex, x1, y1, iconSize, iconSize, opacity);
					x1 += iconSize;
				}
				video::ITexture* buttonTex = buttonTexturesMap[(FK_Input_Buttons)techButton];
				drawIcon(buttonTex, x1, y1, iconSize, iconSize, opacity);
				x1 += iconSize;
				buttonCount += 1;
			}
		}
	}

	/* load buttons */
	void FK_HUDManager_font::loadSystemIcons(std::string mediaPath) {
		// keyboard button texture
		std::string resourcePath = mediaPath;
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

	// update record and replay tiem counter
	void FK_HUDManager_font::updateRecordAndReplayTimeCounter(f32 delta_t_s)
	{
		recordAndReplayTimeCounterS += delta_t_s;
		if (recordAndReplayTimeCounterS >= 2.f) {
			recordAndReplayTimeCounterS = 0.f;
			recordAndReplayPhaseCounter += 1;
		}
	}

	// set button input textures for player 1 (used in Training mode)
	void FK_HUDManager_font::setInputButtonTextures(
		std::map<FK_Input_Buttons, u32> buttonInputLayout, 
		std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayout, 
		bool useJoypadMap)
	{
		player1ButtonInputLayout = buttonInputLayout;
		player1KeyboardKeyLayout = keyboardKeyLayout;
		player1UsesJoypad = useJoypadMap;
	}

	// draw additional input for position reset
	void FK_HUDManager_font::drawHUDAdditionalResetInput()
	{
		video::SColor borderColor(200, 64, 64, 64);
		video::SColor fontColor(255, 255, 255, 255);
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		f32 helpVerticalPosition = 50.f;
		u32 offsetY = (u32)std::floor(helpVerticalPosition * scale_factorY);
		//u32 offsetX = screenSize.Height / 64;
		// assemble icons

		std::wstring resetText = L"reset position: ";
		dimension2d<u32> objSize = textFont->getDimension(resetText.c_str());
		dimension2d<u32> iconSize = dimension2d<u32>(objSize.Height, objSize.Height);
		dimension2d<u32> totalSize = dimension2d<u32>(objSize.Width + 5 * iconSize.Width, objSize.Height);
		
		// draw text
		s32 iconX = screenSize.Width / 2 - totalSize.Width / 2;
		core::rect<s32> destRect(
			iconX,
			offsetY,
			iconX + objSize.Width,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(textFont, resetText, destRect, fontColor, borderColor);

		//draw first icon
		iconX += objSize.Width;
		video::ITexture* tex = buttonTexturesMap[FK_Input_Buttons::Left_Direction];
		dimension2d<u32> texSize = tex->getSize();
		core::vector2di drawingPosition(iconX, offsetY);
		core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw second icon
		iconX += iconSize.Width;
		tex = buttonTexturesMap[FK_Input_Buttons::Down_Direction];
		drawingPosition = core::vector2di(iconX, offsetY);
		scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw third icon
		iconX += iconSize.Width;
		tex = buttonTexturesMap[FK_Input_Buttons::Right_Direction];
		drawingPosition = core::vector2di(iconX, offsetY);
		scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw plus sign icon
		iconX += iconSize.Width;
		tex = plusSignTex;
		drawingPosition = core::vector2di(iconX, offsetY);
		scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw reset button icon
		if (player1UsesJoypad) {
			iconX += iconSize.Width;
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
				player1ButtonInputLayout[FK_Input_Buttons::Help_Button]
			];
			drawingPosition = core::vector2di(iconX, offsetY);
			scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			iconX += iconSize.Width;
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[FK_Input_Buttons::Help_Button];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			u32 iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			scale = core::vector2df((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}
	}

	// draw additional input for position reset
	void FK_HUDManager_font::drawHUDAdditionalInputRecordInputDummyPhase(f32 delta_t_s)
	{
		updateRecordAndReplayTimeCounter(delta_t_s);
		if (recordAndReplayPhaseCounter >= recordAndReplayDummyPhaseText.size()) {
			recordAndReplayPhaseCounter = 0;
		}
		video::SColor borderColor(200, 64, 64, 64);
		video::SColor fontColor(255, 255, 255, 255);
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		f32 helpVerticalPosition = 50.f;
		u32 offsetY = (u32)std::floor(helpVerticalPosition * scale_factorY);
		//u32 offsetX = screenSize.Height / 64;
		// assemble icons

		std::wstring recordtext = recordAndReplayDummyPhaseText[recordAndReplayPhaseCounter];
		dimension2d<u32> objSize = textFont->getDimension(recordtext.c_str());
		dimension2d<u32> iconSize = dimension2d<u32>(objSize.Height, objSize.Height);
		dimension2d<u32> totalSize = dimension2d<u32>(objSize.Width + 3 * iconSize.Width, objSize.Height);

		// draw text
		s32 iconX = screenSize.Width / 2 - totalSize.Width / 2;
		core::rect<s32> destRect(
			iconX,
			offsetY,
			iconX + objSize.Width,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(textFont, recordtext, destRect, fontColor, borderColor);
		video::ITexture* tex;
		
		core::vector2di drawingPosition;
		iconX += objSize.Width;
		u32 iconWidth = 0;

		//draw reset button icon
		if (player1UsesJoypad) {
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[FK_Input_Buttons::Help_Button]
			];
			dimension2d<u32> texSize = tex->getSize(); 
			iconWidth = iconSize.Width;
			drawingPosition = core::vector2di(iconX, offsetY);
			core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[FK_Input_Buttons::Help_Button];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			dimension2d<u32> texSize = tex->getSize();
			core::vector2df scale((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		//draw plus sign icon
		iconX += iconWidth;
		tex = plusSignTex;
		dimension2d<u32> texSize = tex->getSize();
		drawingPosition = core::vector2di(iconX, offsetY);
		core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw reset button icon
		if (player1UsesJoypad) {
			iconX += iconSize.Width;
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[recordAndReplayDummyPhaseInput[recordAndReplayPhaseCounter]]
			];
			drawingPosition = core::vector2di(iconX, offsetY);
			scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			iconX += iconSize.Width;
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[
				recordAndReplayDummyPhaseInput[recordAndReplayPhaseCounter]];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			u32 iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			scale = core::vector2df((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}
	}

	// draw additional input for position reset
	void FK_HUDManager_font::drawHUDAdditionalInputRecordInputControlPhase(f32 delta_t_s)
	{
		updateRecordAndReplayTimeCounter(delta_t_s);
		if (recordAndReplayPhaseCounter >= recordAndReplayControlPhaseText.size()) {
			recordAndReplayPhaseCounter = 0;
		}
		video::SColor borderColor(200, 64, 64, 64);
		video::SColor fontColor(255, 255, 255, 255);
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		f32 helpVerticalPosition = 50.f;
		u32 offsetY = (u32)std::floor(helpVerticalPosition * scale_factorY);
		//u32 offsetX = screenSize.Height / 64;
		// assemble icons

		std::wstring recordtext = recordAndReplayControlPhaseText[recordAndReplayPhaseCounter];
		dimension2d<u32> objSize = textFont->getDimension(recordtext.c_str());
		dimension2d<u32> iconSize = dimension2d<u32>(objSize.Height, objSize.Height);
		dimension2d<u32> totalSize = dimension2d<u32>(objSize.Width + 3 * iconSize.Width, objSize.Height);

		// draw text
		s32 iconX = screenSize.Width / 2 - totalSize.Width / 2;
		core::rect<s32> destRect(
			iconX,
			offsetY,
			iconX + objSize.Width,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(textFont, recordtext, destRect, fontColor, borderColor);
		video::ITexture* tex;

		core::vector2di drawingPosition;
		iconX += objSize.Width;
		u32 iconWidth = 0;
		//draw reset button icon
		if (player1UsesJoypad) {
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[FK_Input_Buttons::Help_Button]
			];
			dimension2d<u32> texSize = tex->getSize();
			drawingPosition = core::vector2di(iconX, offsetY);
			iconWidth = iconSize.Width;
			core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[FK_Input_Buttons::Help_Button];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			dimension2d<u32> texSize = tex->getSize();
			core::vector2df scale((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		//draw plus sign icon
		iconX += iconWidth;
		tex = plusSignTex;
		dimension2d<u32> texSize = tex->getSize();
		drawingPosition = core::vector2di(iconX, offsetY);
		core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw reset button icon
		if (player1UsesJoypad) {
			iconX += iconSize.Width;
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[recordAndReplayControlPhaseInput[recordAndReplayPhaseCounter]]
			];
			drawingPosition = core::vector2di(iconX, offsetY);
			scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			iconX += iconSize.Width;
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[
				recordAndReplayControlPhaseInput[recordAndReplayPhaseCounter]];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			u32 iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			scale = core::vector2df((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}
	}

	// draw additional input for position reset
	void FK_HUDManager_font::drawHUDAdditionalInputRecordInputRecordPhase(f32 delta_t_s)
	{
		updateRecordAndReplayTimeCounter(delta_t_s);
		if (recordAndReplayPhaseCounter >= recordAndReplayRecordPhaseText.size()) {
			recordAndReplayPhaseCounter = 0;
		}
		video::SColor borderColor(200, 64, 64, 64);
		video::SColor fontColor(255, 255, 255, 255);
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		f32 helpVerticalPosition = 75.f;
		u32 offsetY = (u32)std::floor(helpVerticalPosition * scale_factorY);
		//u32 offsetX = screenSize.Height / 64;
		// assemble icons

		std::wstring recordtext = recordAndReplayRecordPhaseText[recordAndReplayPhaseCounter];
		dimension2d<u32> objSize = textFont->getDimension(recordtext.c_str());
		dimension2d<u32> iconSize = dimension2d<u32>(objSize.Height, objSize.Height);
		dimension2d<u32> totalSize = dimension2d<u32>(objSize.Width + 3 * iconSize.Width, objSize.Height);

		// draw text
		s32 iconX = screenSize.Width / 2 - totalSize.Width / 2;
		core::rect<s32> destRect(
			iconX,
			offsetY,
			iconX + objSize.Width,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(textFont, recordtext, destRect, fontColor, borderColor);
		video::ITexture* tex;

		core::vector2di drawingPosition;
		iconX += objSize.Width;
		u32 iconWidth = 0;
		//draw reset button icon
		if (player1UsesJoypad) {
			
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[FK_Input_Buttons::Help_Button]
			];
			dimension2d<u32> texSize = tex->getSize();
			drawingPosition = core::vector2di(iconX, offsetY);
			iconWidth = iconSize.Width;
			core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[FK_Input_Buttons::Help_Button];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			dimension2d<u32> texSize = tex->getSize();
			core::vector2df scale((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		//draw plus sign icon
		iconX += iconWidth;
		tex = plusSignTex;
		dimension2d<u32> texSize = tex->getSize();
		drawingPosition = core::vector2di(iconX, offsetY);
		core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw reset button icon
		if (player1UsesJoypad) {
			iconX += iconSize.Width;
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[recordAndReplayRecordPhaseInput[recordAndReplayPhaseCounter]]
			];
			drawingPosition = core::vector2di(iconX, offsetY);
			scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			iconX += iconSize.Width;
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[
				recordAndReplayRecordPhaseInput[recordAndReplayPhaseCounter]];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			u32 iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			scale = core::vector2df((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}
	}

	// draw additional input for position reset
	void FK_HUDManager_font::drawHUDAdditionalInputRecordInputReplayPhase(f32 delta_t_s)
	{
		updateRecordAndReplayTimeCounter(delta_t_s);
		if (recordAndReplayPhaseCounter >= recordAndReplayReplayPhaseText.size()) {
			recordAndReplayPhaseCounter = 0;
		}
		video::SColor borderColor(200, 64, 64, 64);
		video::SColor fontColor(255, 255, 255, 255);
		video::SColor keyboardLetterColor = video::SColor(255, 196, 196, 196);
		video::SColor keyboardLetterBorder = video::SColor(128, 0, 0, 0);
		f32 helpVerticalPosition = 75.f;
		u32 offsetY = (u32)std::floor(helpVerticalPosition * scale_factorY);
		//u32 offsetX = screenSize.Height / 64;
		// assemble icons

		std::wstring recordtext = recordAndReplayReplayPhaseText[recordAndReplayPhaseCounter];
		dimension2d<u32> objSize = textFont->getDimension(recordtext.c_str());
		dimension2d<u32> iconSize = dimension2d<u32>(objSize.Height, objSize.Height);
		dimension2d<u32> totalSize = dimension2d<u32>(objSize.Width + 3 * iconSize.Width, objSize.Height);

		// draw text
		s32 iconX = screenSize.Width / 2 - totalSize.Width / 2;
		core::rect<s32> destRect(
			iconX,
			offsetY,
			iconX + objSize.Width,
			offsetY + objSize.Height);
		fk_addons::drawBorderedText(textFont, recordtext, destRect, fontColor, borderColor);
		video::ITexture* tex;

		core::vector2di drawingPosition;
		iconX += objSize.Width;
		u32 iconWidth = 0;
		//draw reset button icon
		if (player1UsesJoypad) {

			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[FK_Input_Buttons::Help_Button]
			];
			dimension2d<u32> texSize = tex->getSize();
			drawingPosition = core::vector2di(iconX, offsetY);
			iconWidth = iconSize.Width;
			core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[FK_Input_Buttons::Help_Button];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			dimension2d<u32> texSize = tex->getSize();
			core::vector2df scale((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}

		//draw plus sign icon
		iconX += iconWidth;
		tex = plusSignTex;
		dimension2d<u32> texSize = tex->getSize();
		drawingPosition = core::vector2di(iconX, offsetY);
		core::vector2df scale((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
		fk_addons::drawBatch2DImage(driver, tex,
			core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
			0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));

		//draw reset button icon
		if (player1UsesJoypad) {
			iconX += iconSize.Width;
			tex = joypadButtonsTexturesMap[
				(FK_JoypadInput)
					player1ButtonInputLayout[recordAndReplayReplayPhaseInput[recordAndReplayPhaseCounter]]
			];
			drawingPosition = core::vector2di(iconX, offsetY);
			scale = core::vector2df((f32)iconSize.Width / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
		}
		else {
			iconX += iconSize.Width;
			drawingPosition = core::vector2di(iconX, offsetY);
			tex = keyboardButtonTexture;
			std::wstring itemToDraw = player1KeyboardKeyLayout[
				recordAndReplayReplayPhaseInput[recordAndReplayPhaseCounter]];
			core::dimension2du textSize = textFont->getDimension(itemToDraw.c_str());
			u32 baseIconSize = iconSize.Width;
			u32 iconWidth = baseIconSize;
			while (iconWidth < textSize.Width) {
				iconWidth += baseIconSize;
			}
			scale = core::vector2df((f32)iconWidth / texSize.Width, (f32)iconSize.Height / texSize.Height);
			fk_addons::drawBatch2DImage(driver, tex,
				core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
				0.0f, scale, true, fontColor, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			core::rect<s32> destRect = core::rect<s32>(
				drawingPosition.X,
				drawingPosition.Y,
				drawingPosition.X + iconWidth,
				drawingPosition.Y + baseIconSize);
			fk_addons::drawBorderedText(textFont, itemToDraw, destRect,
				keyboardLetterColor, keyboardLetterBorder,
				true, true);
		}
	}

	// draw attract mode message
	void FK_HUDManager_font::drawAttractModeMessage(f32 delta_t_s)
	{
		attractModeTimeCounter += (u32)(1000*delta_t_s);
		s32 x1, y1, x2, y2;
		core::position2df captionPosition = core::position2df(0.5, 0.9);
		u32 captionPeriodDurationMS = 1000;
		video::SColor captionColor = video::SColor(255, 255, 255, 255);
		std::wstring attractModeMessage = L"press any button";
		core::dimension2d<u32> captionSize = font->getDimension(attractModeMessage.c_str());
		x1 = (s32)floor((f32)screenSize.Width * captionPosition.X - captionSize.Width / 2);
		y1 = (s32)floor((f32)screenSize.Height * captionPosition.Y - captionSize.Height / 2);
		x2 = x1 + captionSize.Width;
		y2 = y1 + captionSize.Height;
		core::rect<s32> destinationRect(x1, y1, x2, y2);
		s32 alpha = (s32)floor((f32)captionColor.getAlpha() *
			(f32)(128 + 128 * cos(2 * core::PI * 
			(f32)attractModeTimeCounter / (f32)captionPeriodDurationMS)) / 255.0f);
		alpha = core::clamp(alpha, 1, 255);
		video::SColor color = captionColor;
		color.setAlpha(alpha);
		video::SColor bordercolor = (255, 0, 0, 0);
		bordercolor.setAlpha(alpha);
		fk_addons::drawBorderedText(font, attractModeMessage, destinationRect, color, bordercolor);
	}

	// get screen position from 3D position related to current camera
	core::position2d<s32> FK_HUDManager_font::getScreenCoordinatesFrom3DPosition(const core::vector3df & pos3d){
		if (!device->getSceneManager() || !driver)
			return core::position2d<s32>(-1000, -1000);
		auto camera = device->getSceneManager()->getActiveCamera();
		if (!camera)
			return core::position2d<s32>(-1000, -1000);
		core::dimension2d<u32> dim;
		dim = (driver->getCurrentRenderTargetSize());
		dim.Width /= 2;
		dim.Height /= 2;

		core::matrix4 trans = camera->getProjectionMatrix();
		trans *= camera->getViewMatrix();

		f32 transformedPos[4] = { pos3d.X, pos3d.Y, pos3d.Z, 1.0f };

		trans.multiplyWith1x4Matrix(transformedPos);

		if (transformedPos[3] < 0)
			return core::position2d<s32>(-10000, -10000);

		const f32 zDiv = transformedPos[3] == 0.0f ? 1.0f :
			core::reciprocal(transformedPos[3]);

		return core::position2d<s32>(
			dim.Width + core::round32(dim.Width * (transformedPos[0] * zDiv)),
			dim.Height - core::round32(dim.Height * (transformedPos[1] * zDiv)));
	}

	// draw hit flag for training. Each hit flag expires after a certain time has passed - 
	// position and hitbox are stored until time goes out
	void FK_HUDManager_font::drawMoveHitType(FK_HUDManager_font::FK_HUD_Hitflag hitboxData){
		f32 flagSize = std::ceil(64.0f * scale_factorX);
		core::position2d<s32> drawingPosition = getScreenCoordinatesFrom3DPosition(hitboxData.position3D);
		if (drawingPosition == core::position2d<s32>(-10000, -10000)){
			return;
		}
		if (hitflagsTextures.count(hitboxData.hitbox.getAttackType()) > 0){
			video::ITexture* tex = hitflagsTextures[hitboxData.hitbox.getAttackType()];
			video::SColor color(hitboxData.getOpacity(), 255, 255, 255);
			video::SColor const vertexColors[4] = {
				color,
				color,
				color,
				color
			};
			color.setAlpha(hitboxData.getOpacity());
			core::dimension2d<u32> texSize = tex->getSize();
			core::vector2d<f32> scale(flagSize / (f32)texSize.Width, flagSize / (f32)texSize.Height);
			drawingPosition.X -= (s32)(flagSize / 2);
			drawingPosition.Y -= (s32)(flagSize / 2);
			if (hitboxData.getOpacity() != 255){
				driver->draw2DImage(tex,
					core::rect<s32>(drawingPosition.X, drawingPosition.Y,
					drawingPosition.X + (s32)(flagSize), drawingPosition.Y + (s32)(flagSize)),
					core::rect<s32>(0, 0, texSize.Width, texSize.Height), 0,
					vertexColors, true);
			}
			else{
				fk_addons::drawBatch2DImage(driver, tex,
					core::rect<s32>(0, 0, texSize.Width, texSize.Height), drawingPosition, core::vector2d<s32>(0, 0),
					0.0f, scale, true, color, core::rect<s32>(0, 0, screenSize.Width, screenSize.Height));
			}
			// draw frame advantage
			if (hitboxData.showFrameAdvantage) {
				std::wstring frameData = std::to_wstring(hitboxData.frameAdvantage);
				if (hitboxData.frameAdvantage > 0) {
					frameData = L"+" + frameData;
				}
				if (hitboxData.cancelAdvantage != hitboxData.frameAdvantage) {
					std::wstring cancelData = std::to_wstring(hitboxData.cancelAdvantage);
					if (hitboxData.cancelAdvantage > 0) {
						cancelData = L"+" + cancelData;
					}
					frameData += L" / " + cancelData;
				}
				s32 sentenceHeight = textFont->getDimension(frameData.c_str()).Height;
				// draw text
				core::rect<s32> destinationRect(drawingPosition.X, drawingPosition.Y,
					drawingPosition.X + (s32)(flagSize), drawingPosition.Y + sentenceHeight);
				fk_addons::drawBorderedText(textFont, frameData, destinationRect,
					video::SColor(hitboxData.getOpacity(), 255, 255, 255),
					video::SColor(hitboxData.getOpacity(), 0, 0, 0), true);
			}
		}
	}

	// update all hitbox data
	void FK_HUDManager_font::updateMoveHitFlags(u32 frameDeltaTimeMS){
		for (int i = 0; i < hitflags.size(); ++i){
			hitflags[i].timerMS += frameDeltaTimeMS;
		}
		if (!hitflags.empty() && hitflags[0].isExpired()){
			hitflags.pop_front();
		}
		// usually, consider player 1 as the active player in training mode (since this feature is - well - used only in training mode
		if (player1->getCurrentMove() == NULL){
			currentMoveNameTimer += frameDeltaTimeMS;
		}
	}

	// add hitbox data to deque
	void FK_HUDManager_font::addMoveHitFlag(ISceneNode* hitboxNode, FK_Hitbox* hitboxData, 
		bool showFrameAdvantage,
		s32 frameAdvantage,
		s32 cancelAdvantage){
		FK_HUD_Hitflag hitflag;
		hitflag.position3D = hitboxNode->getAbsolutePosition();
		hitflag.hitbox = FK_Hitbox(*hitboxData);
		hitflag.showFrameAdvantage = showFrameAdvantage && frameAdvantage > -1000;
		hitflag.frameAdvantage = frameAdvantage;
		hitflag.cancelAdvantage = cancelAdvantage;
		hitflag.timerMS = 0;
		hitflags.push_back(hitflag);
	}

	//draw all stored hit move data from the oldest to the newest
	void FK_HUDManager_font::drawMoveHitFlags(){
		for each(auto hitboxData in hitflags){
			drawMoveHitType(hitboxData);
		}
	}

	// draw trigger recovery bar
	void FK_HUDManager_font::drawTriggerRecoveryBar(s32 timerPlayer1, s32 maxTimerPlayer1,
		s32 timerPlayer2, s32 maxTimerPlayer2, u32 currentTime){
		bool flashPlayer1Bar = false;
		bool flashPlayer2Bar = false;
		u32 sectorDivisionPlayer1 = maxTimerPlayer1;
		u32 sectorDivisionPlayer2 = maxTimerPlayer2;
		if (drawSectorizedRecoveryBar) {
			timerPlayer1 += maxTimerPlayer1 * player1->getTriggerCounters();
			timerPlayer2 += maxTimerPlayer2 * player2->getTriggerCounters();
			maxTimerPlayer1 *= maxTriggerCounters;
			maxTimerPlayer2 *= maxTriggerCounters;
		}
		if (player2->getTriggerCounters() >= maxTriggerCounters){
			timerPlayer2 = maxTimerPlayer2;
			if (triggerRecoveryBarFlashWhenMax) {
				flashPlayer2Bar = true;
			}
		}
		if (player1->getTriggerCounters() >= maxTriggerCounters){
			timerPlayer1 = maxTimerPlayer1;
			if (triggerRecoveryBarFlashWhenMax) {
				flashPlayer1Bar = true;
			}
		}
		f32 triggerBarFlashPeriod = 500.f;
		f32 phase = -0.5*std::cos((f32)currentTime / triggerBarFlashPeriod * 2 * core::PI) + 0.5;
		s32 barLength = 0;
		s32 barHeight = 0;
		s32 x11 = 0, x21 = 0;
		s32 x12 = 0, x22 = 0;
		s32 y11 = 0, y21 = 0;
		s32 x1, x2, y1, y2;
		s32 temp;

		if (triggerRecoveryBarSamePositionAsTriggers) {
			calculateTriggerIconPosition(0, y21, y11, x11, temp, temp, x22);
			calculateTriggerIconPosition(maxTriggerCounters - 1, temp, temp, temp, x12, x21, temp);
			barLength = x12 - x11;
			barHeight = y11 - y21;
			if ((barLength % 2) != 0) {
				barLength += 1;
			}
			if ((barHeight % 2) != 0) {
				barHeight += 1;
			}
		}else{
			barLength = triggerRecoveryBarPosition.LowerRightCorner.X;
			barHeight = triggerRecoveryBarPosition.LowerRightCorner.Y;
			barLength = (s32)std::round((f32)barLength * scale_factorX);
			barHeight = (s32)std::round((f32)barHeight * scale_factorY);
			if ((barLength % 2) != 0) {
				barLength += 1;
			}
			if ((barHeight % 2) != 0) {
				barHeight += 1;
			}
			x11 = triggerRecoveryBarPosition.UpperLeftCorner.X;
			y21 = triggerRecoveryBarPosition.UpperLeftCorner.Y; 
			x11 = (s32)std::round((f32)x11 * scale_factorX);
			y21 = (s32)std::round((f32)y21 * scale_factorY);
			x22 = screenSize.Width - x11;
		}
		s32 lineWidth = (s32)std::ceil(1.f *scale_factorX);

		
		video::SColor addonColor = video::SColor(
			(s32)(triggerRecoveryBarFlashColorAddon.getAlpha() * phase),
			(s32)(triggerRecoveryBarFlashColorAddon.getRed()  * phase),
			(s32)(triggerRecoveryBarFlashColorAddon.getRed() * phase),
			(s32)(triggerRecoveryBarFlashColorAddon.getBlue() * phase)
		);
		video::SColor backgroundColor = triggerRecoveryBarBackgroundColor;
		video::SColor borderColor = triggerRecoveryBarBorderColor;

		video::SColor mainColor = triggerRecoveryBarColor1;
		video::SColor gradientColor = triggerRecoveryBarColor2;
		if (maxTimerPlayer1 > 0){
			if (flashPlayer1Bar) {
				mainColor = mainColor + addonColor;
				gradientColor = gradientColor + addonColor;
			}
			// player 1
			// draw basic black box
			x1 = x11;
			y1 = y21;
			x2 = x1 + barLength + 2 * lineWidth;
			y2 = y1 + barHeight + 2 * lineWidth;
			driver->draw2DRectangle(backgroundColor, core::rect<s32>(x1, y1, x2, y2));
			// draw trigger regen bar
			x2 = x1 + (s32)std::round((barLength + 2 * lineWidth) * (f32)timerPlayer1 / (f32)maxTimerPlayer1);
			driver->draw2DRectangle(core::rect<s32>(x1, y1, x2, y2),
				gradientColor, mainColor, gradientColor, mainColor);
			// draw border
			x1 = x11;
			y1 = y21;
			x2 = x1 + barLength + 2 * lineWidth;
			y2 = y1 + lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			x1 = x11;
			y1 = y21 + barHeight + lineWidth;;
			x2 = x1 + barLength + 2 * lineWidth;
			y2 = y1 + lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			x1 = x11;
			y1 = y21;
			x2 = x1 + lineWidth;
			y2 = y1 + barHeight + 2 * lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			x1 = x11 + barLength + lineWidth;
			y1 = y21;
			x2 = x1 + lineWidth;
			y2 = y1 + barHeight + 2 * lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			if (drawSectorizedRecoveryBar) {
				//draw sectors
				for (int i = 1; i < maxTriggerCounters; ++i) {
					x1 = (s32)(x11 + (f32)barLength / maxTriggerCounters * i);
					y1 = y21;
					x2 = x1 + lineWidth;
					y2 = y1 + barHeight / 4 + lineWidth;
					driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
					y1 = y21 + barHeight * 3 / 4 + lineWidth;
					y2 = y1 + barHeight / 4 + lineWidth;
					driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
				}
			}
		}

		// player 2
		mainColor = triggerRecoveryBarColor1;
		gradientColor = triggerRecoveryBarColor2;
		if (maxTimerPlayer2 > 0){
			if (flashPlayer2Bar) {
				mainColor = mainColor + addonColor;
				gradientColor = gradientColor + addonColor;
			}
			// draw basic black box
			y1 = y21;
			x2 = x22;
			x1 = x2 - barLength - 2 * lineWidth;
			y2 = y1 + barHeight + 2 * lineWidth;
			driver->draw2DRectangle(backgroundColor, core::rect<s32>(x1, y1, x2, y2));
			// draw trigger regen bar
			x1 = x2 - (s32)std::round((barLength + 2 * lineWidth) * (f32)timerPlayer2 / (f32)maxTimerPlayer2);
			driver->draw2DRectangle(core::rect<s32>(x1, y1, x2, y2),
				mainColor, gradientColor, mainColor, gradientColor);
			// draw border
			x2 = x22;
			x1 = x2 - barLength - 2 * lineWidth;
			y1 = y21;
			y2 = y1 + lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			y1 = y21 + barHeight + lineWidth;;
			x2 = x22;
			x1 = x2 - barLength - 2 * lineWidth;
			y2 = y1 + lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			x2 = x22;
			x1 = x2 - lineWidth;
			y1 = y21;
			y2 = y1 + barHeight + 2 * lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			x2 = x22 - barLength - lineWidth;
			x1 = x2 - lineWidth;
			y1 = y21;
			y2 = y1 + barHeight + 2 * lineWidth;
			driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
			if (drawSectorizedRecoveryBar) {
				//draw sectors
				for (int i = 1; i < maxTriggerCounters; ++i) {
					x1 = (s32)(x22 - barLength - lineWidth + (f32)barLength / maxTriggerCounters * i);
					y1 = y21;
					x2 = x1 + lineWidth;
					y2 = y1 + barHeight / 4 + lineWidth;
					driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
					y1 = y21 + barHeight * 3 / 4 + lineWidth;
					y2 = y1 + barHeight / 4 + lineWidth;
					driver->draw2DRectangle(borderColor, core::rect<s32>(x1, y1, x2, y2));
				}
			}
		}
	}
};