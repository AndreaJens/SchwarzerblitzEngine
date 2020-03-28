#ifndef FK_HUD_MANAGER_CLASS
#define FK_HUD_MANAGER_CLASS

#include<irrlicht.h>
#include<string>
#include"FK_Character.h"
#include"FK_InputBuffer.h"
#include"FK_Stage.h";

using namespace irr;

namespace fk_engine{
	class FK_HUDManager{
	public:
		FK_HUDManager(IrrlichtDevice *new_device, std::string mediaPath, FK_Character* new_player1, FK_Character* new_player2);
		void drawHUD(u32 numberOfIndicators, u32 player1winNumber, u32 player2winNumber,
			s32 roundTimeS, bool pause, f32 frameDeltaTime_s);
		void drawInputBuffer(FK_InputBuffer* playerinput);
		void drawRound(int roundNumber, bool isFinalRound = false);
		void drawTimer(u32 time);
		void drawFightText();
		void drawKO(bool perfect);
		void drawDoubleKO();
		void drawRingout();
		void drawDoubleRingout();
		void drawDraw();
		void drawPause();
		void drawTimeUp();
		void drawWin(int winnerId, bool matchEnd);
		void drawIntroText(int playerId);
		void drawStageIntroText(FK_Stage* stage);
		void setViewportSize(core::dimension2d<u32> new_size);
	protected:
		void drawLifeBars(f32 player1lifeRatio, f32 player2lifeRatio, bool pause);
		void drawPlayerNames(std::string player1name, std::string player2name);
		void drawPlayerPortraits(bool pause);
		void drawComboCounters(u32 player1Combo, u32 player2Combo, f32 frameDeltaTimeS);
		void drawRoundIndicators(u32 numberOfIndicators, u32 player1winNumber, u32 player2winNumber, bool pause);
		void drawTriggerCounters(u32 player1Triggers, u32 player2Triggers, f32 frameDeltaTimeS, bool pause);
		void drawTriggerAnimations(f32 frameDeltaTimeS, bool pause);
		void drawIcon(video::ITexture* icon, int& x, int y);
		void drawNormalText(gui::IGUIFont *font, std::wstring text, core::rect<s32> destinationRect,
			video::SColor textColor = video::SColor(255, 255, 255, 255),
			bool horizontalCentering = false, bool verticalCentering = false);
		void calculateTriggerIconPosition(s32 index, s32 &startY, s32 &endY, s32 &startX1, s32 &endX1, s32 &startX2, s32 &endX2);
	private:
		// device
		IrrlichtDevice *device;
		video::IVideoDriver *driver;
		gui::IGUIFont *font;
		gui::IGUIFont *mainFont;
		gui::IGUIFont *textFont;
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
		bool player1TriggerStatus;
		bool player2TriggerStatus;
		u32 player1TriggerRotationDuration;
		u32 player2TriggerRotationDuration;
		u32 player1TriggerRotationCounter;
		u32 player2TriggerRotationCounter;
		f32 player1CylinderRotation;
		f32 player2CylinderRotation;
		// trigger animation variables
		core::dimension2d<u32> triggerAnimationCellSize;
		std::deque<core::position2di> triggerAnimationLocations;
		std::deque<f32> triggerAnimationTimePassed;
		// quotes variable
		std::string quoteToDrawPl1;
		std::string quoteToDrawPl2;
		// hud variables
		video::ITexture* lifebar1;
		video::ITexture* lifebar1sub;
		video::ITexture* lifebar2sub;
		video::ITexture* lifebar1_bw;
		video::ITexture* lifebar1sub_bw;
		video::ITexture* lifebar2sub_bw;
		video::ITexture* roundIndicatorEmpty;
		video::ITexture* roundIndicatorFull;
		video::ITexture* roundIndicatorFull_bw;
		video::ITexture* player1Portrait;
		video::ITexture* player2Portrait;
		// input buffer
		video::ITexture* inputUp_ico;
		video::ITexture* inputDown_ico;
		video::ITexture* inputLeft_ico;
		video::ITexture* inputRight_ico;
		video::ITexture* inputUL_ico;
		video::ITexture* inputUR_ico;
		video::ITexture* inputDL_ico;
		video::ITexture* inputDR_ico;
		video::ITexture* inputUpH_ico;
		video::ITexture* inputDownH_ico;
		video::ITexture* inputLeftH_ico;
		video::ITexture* inputRightH_ico;
		video::ITexture* inputULH_ico;
		video::ITexture* inputURH_ico;
		video::ITexture* inputDLH_ico;
		video::ITexture* inputDRH_ico;
		video::ITexture* inputA_ico;
		video::ITexture* inputB_ico;
		video::ITexture* inputX_ico;
		video::ITexture* inputY_ico;
		video::ITexture* inputMask_ico;
		// trigger icons
		video::ITexture* triggerIcon;
		video::ITexture* triggerCylinderIcon;
		video::ITexture* triggerAnimationTexture;
		// system text
		video::ITexture* pauseText;
		video::ITexture* KOText;
		video::ITexture* doubleKOText;
		video::ITexture* perfectText;
		video::ITexture* roundText;
		video::ITexture* finalRoundText;
		video::ITexture* fightText;
		video::ITexture* drawText;
		video::ITexture* timeEndText;
		video::ITexture** numberText;
		video::ITexture* ringoutText;
		video::ITexture* doubleRingoutText;
		video::ITexture* winText;
		video::ITexture* player1NameText;
		video::ITexture* player2NameText;
		// store pointers to players
		FK_Character* player1;
		FK_Character* player2;
	};
};

#endif