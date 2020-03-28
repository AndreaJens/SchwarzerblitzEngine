#ifndef FK_HUD_MANAGER_F_CLASSFK_HUDManager_font
#define FK_HUD_MANAGER_F_CLASS

#include<irrlicht.h>
#include<string>
#include"FK_Character.h"
#include"FK_InputBuffer.h"
#include"FK_FreeMatchStatistics.h"
#include"FK_Stage.h"

using namespace irr;

namespace fk_engine{
	class FK_HUDManager_font{
		const std::string HUD_Media_Directory = "hud\\";
		const enum FK_HUD_Strings{
			PauseText,
			PlayerWinText,
			RoundText,
			FinalText,
			TimeUpText,
			DrawResultText,
			DoubleText,
			RingoutText,
			ContinueText,
			KOText,
			FightText,
			PerfectText,
			SuccessText,
		};
		const enum FK_HUD_ConfigFileKeys{
			HighLifeColorTag,
			LifebarBackgroundColor,
			LowLifeColorTag,
			VanishingHighLifeColorTag,
			VanishingLowLifeColorTag,
			LifebarCasePosition,
			LifebarPosition,
			LifebarCaseTexture,
			LifebarTextures,
			LifebarCaseBWTexture,
			ComboDamageTextPosition,
			RoundIndicatorTexture,
			RoundIndicatorPosition,
			PlayerNameOffset,
			TriggerCountersPosition,
			TriggerCountersTexture,
			TriggerCountersBlinkColor,
			TriggerCountersUseAnimation,
			TriggerCounterGunCylinderTexture,
			TriggerCounterGunCylinderPosition,
			DrawTriggerCountersFromBorder,
			ShowPortraits,
			ShowNames,
			ShowLifebarCases,
			CenterPlayerNamesOnLifeBars,
			ShowGunCylinder,
			ShowEmptyRoundIndicators,
			ShowBlinkingRoundWinPrediction,
			PortraitPosition,
			ReferenceScreenSize,
			TimerVerticalOffset,
			ShowComboDamage,
			ShowComboCounter,
			MinimalHitNumberToShowDamage,
			DamageName,
			HitName,
			ShowVanishingLife,
			VanishingLifeWaitForHitstun,
			VanishingRatioPerSecond,
			TriggerRecoveryBarBorderColor,
			TriggerRecoveryBarBackgroundColor,
			TriggerRecoveryBarColorGradient,
			TriggerRecoveryBarSamePositionAsTriggers,
			TriggerRecoveryBarPosition,
			TriggerRecoveryBarFlashWhenMax,
			ShowTriggerCounters,
			ShowTriggerNumber,
			ShowSectorizedTriggerBar,
			TriggerNumberPosition,
			ShowHUD,
			ShowStageName,
			ShowStageDescription,
			ShowStageBGMName,
			ShowStageBGMAuthor,
			TrainingInputCoordinates,
			TrainingInputMaxSize,
			InputRecordText,
			InputRecordTimebar,
			ShowMaxComboDamage,
			MaxComboDamagePosition,
		};
	public:
		struct FK_HUD_Hitflag{
			FK_Hitbox hitbox = FK_Hitbox();
			core::vector3df position3D = core::vector3df(0,0,0);
			u32 lifetimeMS = 400;
			u32 fadeoutTimeMS = 200;
			u32 fadeInTimeMS = 100;
			u32 timerMS = 0;
			s32 frameAdvantage = 0;
			s32 cancelAdvantage = 0;
			bool showFrameAdvantage = false;
			bool isExpired();
			void update(u32 deltaFrameMS);
			s32 getOpacity();
		};
		enum TrainingRecordModePhase {
			Record,
			Standby,
			Control,
			Replay,
		};
	public:
		FK_HUDManager_font(IrrlichtDevice *new_device, FK_DatabaseAccessor accessor, std::string mediaPath, FK_Character* new_player1, FK_Character* new_player2);
		void drawHUD(u32 numberOfIndicatorsPl1, u32 numberOfIndicatorsPl2, u32 player1winNumber, u32 player2winNumber,
			s32 roundTimeS, bool pause, f32 frameDeltaTime_s);
		void createDefaultResourceParameters();
		bool loadConfigurationFile(std::string mediaPath);
		void loadDefaultAssets(std::string mediaPath);
		void loadInputBufferTextures(std::string resourcePath);
		void drawInputBuffer(FK_InputBuffer* playerinput);
		void drawEndMatchStatistics(FK_FreeMatchStatistics matchStatistics);
		void drawRound(int roundNumber, bool isFinalRound = false);
		void drawTimer(u32 time);
		void drawFightText();
		void drawKO(bool perfect);
		void drawSuccessText();
		void drawDoubleKO();
		void drawRingout();
		void drawDoubleRingout();
		void drawDraw();
		void drawPause();
		void drawTimeUp();
		void drawWin(int winnerId, bool matchEnd);
		void drawContinue(s32 secondsToGo);
		void drawGameOver();
		void drawNewRecord();
		void drawIntroText(int playerId);
		void drawStageIntroText(FK_Stage* stage);
		void setViewportSize(core::dimension2d<u32> new_size);
		void drawMatchSentence(std::string speakerName, std::string sentence);
		void drawMoveName(FK_Character* character);
		core::position2d<s32> getScreenCoordinatesFrom3DPosition(const core::vector3df & pos3d);
		void updateMoveHitFlags(u32 frameDeltaTimeMS);
		void addMoveHitFlag(ISceneNode* hitboxNode, FK_Hitbox* hitboxData, 
			bool showFrameAdvantage, s32 frameAdvantage, s32 cancelAdvantage);
		void drawMoveHitType(FK_HUD_Hitflag hitboxData);
		void drawMoveHitFlags();
		void drawTriggerRecoveryBar(s32 timerPlayer1, s32 maxTimerPlayer1, s32 timerPlayer2, s32 maxTimerPlayer2, 
			u32 currentTime);
		void drawInputFromMove(FK_Move& move, bool leftSide = true, bool trigger = false);
		void drawInputRecordTimeBar(s32 currentTimeMs, s32 maxTimeMs, TrainingRecordModePhase phase);
		void setOldLifeRatios(f32 player1ratio, f32 player2ratio);
		void drawTimeElapsed(u32 timeMS, u32 recordTimeMS);
		void drawSurvivalRecord(u32 numberOfWins, u32 recordNumberOfWins);
		void setInputButtonTextures(std::map<FK_Input_Buttons, u32> buttonInputLayout,
			std::map<FK_Input_Buttons, std::wstring> keyboardKeyLayout,
			bool useJoypadMap);
		void drawHUDAdditionalResetInput();
		void drawHUDAdditionalInputRecordInputDummyPhase(f32 delta_t_s);
		void drawHUDAdditionalInputRecordInputControlPhase(f32 delta_t_s);
		void drawHUDAdditionalInputRecordInputRecordPhase(f32 delta_t_s);
		void drawHUDAdditionalInputRecordInputReplayPhase(f32 delta_t_s);
		void drawAttractModeMessage(f32 delta_t_s);
	protected:
		void setupHUDTextStrings();
		void loadHitflagTextures(std::string texturePath);
		void drawSystemText(std::wstring text, core::rect<s32> destinationRect);
		void drawStatText(std::wstring text, core::rect<s32> destinationRect);
		void drawSystemTextCentered(std::wstring itemToDraw);
		void drawMultilineSystemTextCentered(std::vector<std::wstring> itemsToDraw);
		void drawBeautyRectangles(core::rect<s32> textRectangle);
		void drawLifeBars(f32 player1lifeRatio, f32 player2lifeRatio, bool pause);
		void drawPlayerNames(std::wstring player1name, std::wstring player2name);
		void drawTriggerNumber(u32 player1Triggers, u32 player2Triggers);
		void drawPlayerPortraits(bool pause);
		void drawComboCounters(u32 player1Combo, u32 player2Combo, f32 frameDeltaTimeS);
		void drawRoundIndicators(u32 numberOfIndicatorsPl1, u32 numberOfIndicatorsPl2, 
			u32 player1winNumber, u32 player2winNumber, f32 frameDeltaTime_s);
		void drawTriggerCounters(u32 player1Triggers, u32 player2Triggers, f32 frameDeltaTimeS);
		void drawCharacterObjectsLifetimeBars();
		void drawObjectHUDbarPlayer1(FK_Character::FK_CharacterAdditionalObject & object,
			FK_Character::FK_CharacterAdditionalObject::HUDbar &bar, f32 fillPercentage);
		void drawObjectHUDbarPlayer2(FK_Character::FK_CharacterAdditionalObject & object,
			FK_Character::FK_CharacterAdditionalObject::HUDbar &bar, f32 fillPercentage);
		void drawTriggerGunCylinders(f32 frameDeltaTimeS);
		void drawTriggerAnimations(f32 frameDeltaTimeS, bool pause);
		void drawIcon(video::ITexture* icon, s32 x, s32 y, s32 width, s32 height, s32 opacity = 255);
		void drawNormalText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
			video::SColor textColor = video::SColor(255, 255, 255, 255),
			bool horizontalCentering = false, bool verticalCentering = false);
		void calculateTriggerIconPosition(s32 index, s32 &startY, s32 &endY, s32 &startX1, 
			s32 &endX1, s32 &startX2, s32 &endX2);
		void drawMoveInput(FK_Move move, s32 x1, s32 y1, s32 opacity, bool leftSide = true);
		void loadSystemIcons(std::string mediaPath);
		void updateRecordAndReplayTimeCounter(f32 delta_t_s);
	private:
		// device
		IrrlichtDevice *device;
		video::IVideoDriver *driver;
		gui::IGUIFont *font;
		gui::IGUIFont *mainFont;
		gui::IGUIFont *textFont;
		gui::IGUIFont *timerFont;
		core::dimension2d<u32> screenSize;
		float scale_factorX;
		float scale_factorY;
		// backup variables for updates
		f32 oldLifeRatioPlayer1;
		f32 oldLifeRatioPlayer2;
		f32 oldComboCounterPlayer1;
		f32 oldComboCounterPlayer2;
		f32 comboTextDurationSPlayer1;
		f32 comboTextDurationSPlayer2;
		f32 comboTextMaxDurationS;
		s32 player1OldBulletCounterNumber;
		s32 player2OldBulletCounterNumber;
		//bool player1TriggerStatus;
		//bool player2TriggerStatus;
		u32 player1TriggerRotationDuration;
		u32 player2TriggerRotationDuration;
		u32 player1TriggerRotationCounter;
		u32 player2TriggerRotationCounter;
		u32 player1TriggerHighlightTimeMs;
		u32 player2TriggerHighlightTimeMs;
		f32 player1CylinderRotation;
		f32 player2CylinderRotation;
		// trigger animation variables
		core::dimension2d<u32> triggerAnimationCellSize;
		std::deque<core::position2di> triggerAnimationLocations;
		std::deque<f32> triggerAnimationTimePassed;
		// quotes variable
		std::wstring quoteToDrawPl1;
		std::wstring quoteToDrawPl2;
		// hud variables
		video::ITexture* lifebar1;
		//video::ITexture* lifebar1_bw;
		video::ITexture* lifebar1sub_bw;
		video::ITexture* lifebar2sub_bw;
		video::ITexture* roundIndicatorEmpty;
		video::ITexture* roundIndicatorFull;
		//video::ITexture* roundIndicatorFull_bw;
		video::ITexture* player1Portrait;
		video::ITexture* player2Portrait;
		// hud text
		std::map<FK_HUD_Strings, std::wstring> hudStrings;
		// trigger icons
		video::ITexture* triggerIcon;
		video::ITexture* triggerCylinderIcon;
		video::ITexture* triggerAnimationTexture;
		// buffer
		video::ITexture* plusSignTex;
		std::map<FK_Input_Buttons, video::ITexture*> buttonTexturesMap;
		video::ITexture* triggerInputTex;
		video::ITexture* triggerInputTexLborder;
		video::ITexture* triggerInputTexRborder;
		video::ITexture* triggerInputTexSingle;
		// in-match dialgoue
		video::ITexture* dialogueTextBack;
		video::ITexture* dialogueTextBackLBorder;
		video::ITexture* dialogueTextBackRBorder;
		// store pointers to players
		FK_Character* player1;
		FK_Character* player2;
		// additional training variables
		std::deque<FK_HUD_Hitflag> hitflags;
		std::map<FK_Attack_Type, video::ITexture*> hitflagsTextures;
		// current move name
		u32 currentMoveNameTimer;
		u32 currentMoveNameFadeoutDuration;
		u32 currentMoveNameDuration;
		std::string currentMoveName;
		std::wstring currentMoveNameToDisplay;
		FK_Move currentMove;
		std::deque<bool> isInputTriggered;
		bool currentMoveClearBuffer;
		bool moveNameDisplayPreviousMoveWasAThrow;
		// resource parameters
		core::dimension2d<u32> triggerAnimationSize;
		core::rect<s32> triggerAnimationFrameSize;
		core::rect<s32> lifebarCasePosition;
		core::rect<s32> lifebarPosition;
		core::rect<s32> roundIndicatorPosition;
		core::position2di comboTextOffset;
		core::position2di playerNameOffset;
		core::rect<s32> portraitPosition;
		core::rect<s32> triggerCounterPosition;
		core::rect<s32> triggerCylinderPosition;
		std::pair<std::wstring, std::wstring> damageName;
		std::pair<std::wstring, std::wstring> hitName;
		bool showComboNumber;
		bool showComboDamage;
		u32 minimalComboHitNumberToShow;
		bool showPlayerPortraits;
		bool showPlayerNames;
		bool showLifebarCases;
		bool showGunCylinder;
		bool centerPlayerNames;
		bool showEmptyRoundIndicators;
		bool showBlinkingRoundIndicatorPredictor;
		bool drawTriggersFromBorderToCenter;
		bool showVanishingDamage;
		bool vanishingDamageWaitForHitstun;
		f32 vanishingLifebarReductionPerSecond;
		video::SColor highLifeColor;
		video::SColor lowLifeColor;
		video::SColor highVanishingLifeColor;
		video::SColor lowVanishingLifeColor;
		video::SColor lifebarBackgroundColor;
		video::SColor triggerSuperBarHighColor;
		video::SColor triggerSuperBarLowColor;
		core::dimension2d<f32> referenceScreenSize;
		f32 roundIndicatorBlinkTime;
		f32 timerVerticalPosition;
		FK_DatabaseAccessor databaseAccessor;
		s32 maxTriggerCounters;
		video::SColor triggerRecoveryBarBorderColor;
		video::SColor triggerRecoveryBarBackgroundColor;
		video::SColor triggerRecoveryBarColor1;
		video::SColor triggerRecoveryBarColor2;
		core::rect<s32> triggerRecoveryBarPosition;
		bool triggerRecoveryBarSamePositionAsTriggers;
		bool triggerRecoveryBarFlashWhenMax;
		bool drawSectorizedRecoveryBar;
		video::SColor triggerRecoveryBarFlashColorAddon;
		bool showTriggerCounters;
		bool showTriggerNumber;
		core::position2di triggerNumberPosition;
		bool showStageName;
		bool showHUD;
		bool showStageDescription;
		bool showStageBGMName;
		bool showStageBGMAuthor;
		bool showComboMaxDamage;
		core::rect<s32> recordTimeBarLocation;
		std::map<TrainingRecordModePhase, std::wstring> recordInputText;
		std::wstring replayInputText;
		video::ITexture* recordButtonTex;
		video::ITexture* stopButtonTex;
		video::ITexture* playButtonTex;
		core::rect<f32> trainingInputCoordinates;
		s32 trainingInputMaxBufferSize;
		// additional input textures routine
		std::map<FK_Input_Buttons, u32> player1ButtonInputLayout;
		std::map<FK_Input_Buttons, std::wstring> player1KeyboardKeyLayout;
		bool player1UsesJoypad;
		/* button textures for navigation */
		std::map<FK_JoypadInput, video::ITexture*> joypadButtonsTexturesMap;
		video::ITexture* keyboardButtonTexture;
		/* record and replay */
		f32 recordAndReplayTimeCounterS;
		u32 recordAndReplayPhaseCounter;

		std::map<s32, std::wstring> recordAndReplayDummyPhaseText;
		std::map<s32, std::wstring> recordAndReplayControlPhaseText;
		std::map<s32, std::wstring> recordAndReplayRecordPhaseText;
		std::map<s32, std::wstring> recordAndReplayReplayPhaseText;

		std::map<s32, FK_Input_Buttons> recordAndReplayDummyPhaseInput;
		std::map<s32, FK_Input_Buttons> recordAndReplayControlPhaseInput;
		std::map<s32, FK_Input_Buttons> recordAndReplayRecordPhaseInput;
		std::map<s32, FK_Input_Buttons> recordAndReplayReplayPhaseInput;

		// attract mode
		u32 attractModeTimeCounter;
	};
};

#endif