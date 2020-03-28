#include"FK_SceneGameTraining.h"

namespace fk_engine{

	FK_SceneGameTraining::FK_TrainingOptionsMenu::FK_TrainingOptionsMenu(){
		inputTimeThreshold = 200;
		lastInputTime = 0;
	}
	FK_SceneGameTraining::FK_TrainingOptionsMenu::~FK_TrainingOptionsMenu(){
		menuItems.clear();
		menuList.clear();
		menuItemsIndices.clear();
		menuItemsParameters.clear();
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::setViewport(core::dimension2d<u32> newViewport){
		screenSize = newViewport;
		scaleFactorX = screenSize.Width;
		scaleFactorY = screenSize.Height;
		scaleFactorX /= (f32)fk_constants::FK_DefaultResolution.Width;
		scaleFactorY /= (f32)fk_constants::FK_DefaultResolution.Height;
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::setup(IrrlichtDevice* new_device){

		device = new_device;
		screenSize = device->getVideoDriver()->getScreenSize();
		scaleFactorX = screenSize.Width;
		scaleFactorY = screenSize.Height;
		scaleFactorX /= (f32)fk_constants::FK_DefaultResolution.Width;
		scaleFactorY /= (f32)fk_constants::FK_DefaultResolution.Height;
		mainFont = device->getGUIEnvironment()->getFont(fk_constants::FK_GameFontIdentifier.c_str());
		textFont = device->getGUIEnvironment()->getFont(fk_constants::FK_WindowFontIdentifier.c_str());

		menuIndex = 0;

		menuItems[FK_TrainingOption::DummyAction] = L"Dummy state";
		menuItems[FK_TrainingOption::DummyAILevel] = L"Dummy AI level";
		menuItems[FK_TrainingOption::DummyGuard] = L"Dummy guard";
		menuItems[FK_TrainingOption::DummyAfterGuard] = L"After successful guard";
		menuItems[FK_TrainingOption::DummyRecovery] = L"Ground recovery";
		menuItems[FK_TrainingOption::DummyUkemi] = L"Impact recovery";
		menuItems[FK_TrainingOption::MoveList] = L"Open move list";
		menuItems[FK_TrainingOption::CharacterSelect] = L"Select character";
		menuItems[FK_TrainingOption::FreeCameraMode] = L"Free camera mode";
		menuItems[FK_TrainingOption::Resume] = L"Resume game";
		menuItems[FK_TrainingOption::ReturnToMenu] = L"To main menu";

		for (auto key : menuItems) {
			itemActiveStatus[key.first] = true;
		}

		menuList.push_back(FK_TrainingOption::DummyAction);
		menuList.push_back(FK_TrainingOption::DummyAILevel);
		menuList.push_back(FK_TrainingOption::DummyGuard);
		menuList.push_back(FK_TrainingOption::DummyAfterGuard);
		menuList.push_back(FK_TrainingOption::DummyRecovery);
		menuList.push_back(FK_TrainingOption::DummyUkemi);
		menuList.push_back(FK_TrainingOption::MoveList);
		menuList.push_back(FK_TrainingOption::FreeCameraMode);
		menuList.push_back(FK_TrainingOption::Resume);
		menuList.push_back(FK_TrainingOption::CharacterSelect);
		menuList.push_back(FK_TrainingOption::ReturnToMenu);

		for (s32 i = 0; i < menuList.size(); ++i){
			menuItemsIndices.push_back(0);
		}

		drawingGaps.push_back(5);
		drawingGaps.push_back(6);
		//drawingGaps.push_back(8);

		// states
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::StandingDummy, L"idle")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::CrouchingDummy, L"crouching")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::JumpingDummy, L"neutral jump")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::JumpingForwardDummy, L"forward jump")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::JumpingBackwardDummy, L"backward jump")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::RandomJump, L"random jump")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::BackgroundSidestepDummy, L"background sidestep")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::ForegroundSidestepDummy, L"foreground sidestep")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::WalkingDummy, L"walk forward")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
			FK_TrainingOptionsDummy::WalkBackwardDummy, L"walk backward")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
				std::pair<u32, std::wstring>(
					FK_TrainingOptionsDummy::Player2ControlledDummy, L"player 2")
			);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
				FK_TrainingOptionsDummy::RecordDummy, L"record & replay")
		);
		menuItemsParameters[FK_TrainingOption::DummyAction].push_back(
			std::pair<u32, std::wstring>(
				FK_TrainingOptionsDummy::AIControlledDummy, L"AI")
		);

		// guard
		menuItemsParameters[FK_TrainingOption::DummyGuard].push_back(
			std::pair<u32, std::wstring>(
			FK_GuardOptionsDummy::NoGuard, L"no guard")
			);
		menuItemsParameters[FK_TrainingOption::DummyGuard].push_back(
			std::pair<u32, std::wstring>(
			FK_GuardOptionsDummy::OnlyStandingGuard, L"standing guard")
			);
		menuItemsParameters[FK_TrainingOption::DummyGuard].push_back(
			std::pair<u32, std::wstring>(
			FK_GuardOptionsDummy::OnlyCrouchingGuard, L"crouching guard")
			);
		menuItemsParameters[FK_TrainingOption::DummyGuard].push_back(
			std::pair<u32, std::wstring>(
			FK_GuardOptionsDummy::AlwaysGuard, L"guard all")
			);
		menuItemsParameters[FK_TrainingOption::DummyGuard].push_back(
			std::pair<u32, std::wstring>(
				FK_GuardOptionsDummy::GuardAfterFirstHit, L"after 1st hit")
		);
		menuItemsParameters[FK_TrainingOption::DummyGuard].push_back(
			std::pair<u32, std::wstring>(
			FK_GuardOptionsDummy::RandomGuard, L"random guard")
			);

		// recovery
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
			FK_RecoveryOptionsDummy::NoRecovery, L"stand up")
			);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
			FK_RecoveryOptionsDummy::AdvanceRecovery, L"step forward")
			);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
			FK_RecoveryOptionsDummy::BackwardsRecovery, L"step backward")
			);
		/*menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
				FK_RecoveryOptionsDummy::CrouchingRecovery, L"crouching guard")
		);*/
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
			FK_RecoveryOptionsDummy::ForegroundRotation, L"foreground roll")
			);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
			FK_RecoveryOptionsDummy::BackgroundRotation, L"background roll")
			);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
				FK_RecoveryOptionsDummy::FrontRollRecovery, L"forward roll")
		);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
				FK_RecoveryOptionsDummy::BackRollRecovery, L"backward roll")
		);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
				FK_RecoveryOptionsDummy::Reversal, L"wake-up attack")
		);
		menuItemsParameters[FK_TrainingOption::DummyRecovery].push_back(
			std::pair<u32, std::wstring>(
			FK_RecoveryOptionsDummy::RandomRecovery, L"random")
			);

		// ukemi
		menuItemsParameters[FK_TrainingOption::DummyUkemi].push_back(
			std::pair<u32, std::wstring>(
				FK_UkemiOptionsDummy::NoUkemi, L"no recovery")
		);
		menuItemsParameters[FK_TrainingOption::DummyUkemi].push_back(
			std::pair<u32, std::wstring>(
				FK_UkemiOptionsDummy::BGUkemi, L"background ukemi")
		);
		menuItemsParameters[FK_TrainingOption::DummyUkemi].push_back(
			std::pair<u32, std::wstring>(
				FK_UkemiOptionsDummy::FGUkemi, L"foreground ukemi")
		);
		menuItemsParameters[FK_TrainingOption::DummyUkemi].push_back(
			std::pair<u32, std::wstring>(
				FK_UkemiOptionsDummy::FrontRollUkemi, L"forward roll")
		);
		menuItemsParameters[FK_TrainingOption::DummyUkemi].push_back(
			std::pair<u32, std::wstring>(
				FK_UkemiOptionsDummy::BackRollUkemi, L"backward roll")
		);
		menuItemsParameters[FK_TrainingOption::DummyUkemi].push_back(
			std::pair<u32, std::wstring>(
				FK_UkemiOptionsDummy::RandomUkemi, L"random ukemi")
		);

		// after guard
		menuItemsParameters[FK_TrainingOption::DummyAfterGuard].push_back(
			std::pair<u32, std::wstring>(
				FK_AfterGuardOptionsDummy::NoAfterGuardAction, L"no action")
		);
		menuItemsParameters[FK_TrainingOption::DummyAfterGuard].push_back(
			std::pair<u32, std::wstring>(
				FK_AfterGuardOptionsDummy::BackstepAfterGuard, L"backstep")
		);
		menuItemsParameters[FK_TrainingOption::DummyAfterGuard].push_back(
			std::pair<u32, std::wstring>(
				FK_AfterGuardOptionsDummy::SidestepForegroundAfterGuard, L"foreground sidestep")
		);
		menuItemsParameters[FK_TrainingOption::DummyAfterGuard].push_back(
			std::pair<u32, std::wstring>(
				FK_AfterGuardOptionsDummy::SidestepBackgroundAfterGuard, L"background sidestep")
		);
		menuItemsParameters[FK_TrainingOption::DummyAfterGuard].push_back(
			std::pair<u32, std::wstring>(
				FK_AfterGuardOptionsDummy::MoveFromListAfterGuard, L"move (select to choose)")
		);

		// AI
		for (u32 i = 0; i < 21; ++i){
			menuItemsParameters[FK_TrainingOption::DummyAILevel].push_back(
				std::pair<u32, std::wstring>(
				i, std::to_wstring(i))
				);
		}

		// store indices
		for (s32 i = 0; i < menuList.size(); ++i){
			menuOptionsIndices[menuList[i]] = i;
		}
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::increaseIndex(){
		menuIndex += 1;
		menuIndex %= menuList.size();
		lastInputTime = device->getTimer()->getRealTime();
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::decreaseIndex(){
		menuIndex -= 1;
		if (menuIndex < 0){
			menuIndex = menuList.size() - 1;
		}
		lastInputTime = device->getTimer()->getRealTime();
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::setCurrentTimeAsInputTime() {
		lastInputTime = device->getTimer()->getRealTime();
	}

	/* toggle item activity*/
	void FK_SceneGameTraining::FK_TrainingOptionsMenu::toggleItemActivity(FK_TrainingOption option, bool newStatus)
	{
		itemActiveStatus[option] = newStatus;
	}

	bool FK_SceneGameTraining::FK_TrainingOptionsMenu::decreaseCurrentItem(){
		if (!itemActiveStatus[getCurrentItem()]) {
			lastInputTime = device->getTimer()->getRealTime();
			return true;
		}
		if (menuItemsParameters[getCurrentItem()].size() > 0){
			menuItemsIndices[menuIndex] -= 1;
			if (menuItemsIndices[menuIndex] < 0){
				menuItemsIndices[menuIndex] = menuItemsParameters[getCurrentItem()].size() - 1;
			}
			lastInputTime = device->getTimer()->getRealTime();
			return true;
		}
		return false;
	}
	bool FK_SceneGameTraining::FK_TrainingOptionsMenu::increaseCurrentItem(){
		if (!itemActiveStatus[getCurrentItem()]) {
			lastInputTime = device->getTimer()->getRealTime();
			return true;
		}
		if (menuItemsParameters[getCurrentItem()].size() > 0){
			menuItemsIndices[menuIndex] += 1;
			if (menuItemsIndices[menuIndex] >= menuItemsParameters[getCurrentItem()].size()){
				menuItemsIndices[menuIndex] = 0;
			}
			lastInputTime = device->getTimer()->getRealTime();
			return true;
		}
		return false;
	}

	bool FK_SceneGameTraining::FK_TrainingOptionsMenu::canAcceptInput(){
		return device->getTimer()->getRealTime() - lastInputTime > inputTimeThreshold;
	}

	u32 FK_SceneGameTraining::FK_TrainingOptionsMenu::getParameterValue(
		FK_SceneGameTraining::FK_TrainingOptionsMenu::FK_TrainingOption option){
		s32 index = menuOptionsIndices[option];
		if (menuOptionsIndices.count(option) == 0){
			return 0;
		}
		if (menuItemsParameters[menuList[index]].empty()){
			return 0;
		}
		return menuItemsParameters[menuList[index]][menuItemsIndices[index]].first;
	}

	FK_SceneGameTraining::FK_TrainingOptionsMenu::FK_TrainingOption FK_SceneGameTraining::FK_TrainingOptionsMenu::getCurrentItem(){
		return menuList[menuIndex];
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::update(u32 inputButtons){
		inputButtons = inputButtons & FK_Input_Buttons::Any_MenuButton;
	}

	void FK_SceneGameTraining::FK_TrainingOptionsMenu::draw(){
		s32 sentenceOffsetX = 40 * scaleFactorX;
		s32 selectedItemAdditionalOffsetX = 20 * scaleFactorX;
		s32 sentenceInitialY = 70 * scaleFactorY;
		s32 sentenceOffsetY = sentenceInitialY;
		for (s32 i = 0; i < menuList.size(); ++i){ 
			std::wstring itemToDraw = menuItems[menuList[i]];
			s32 sentenceWidth = mainFont->getDimension(itemToDraw.c_str()).Width;
			s32 sentenceHeight = mainFont->getDimension(itemToDraw.c_str()).Height;
			video::SColor color(255, 255, 255, 255);
			if (!itemActiveStatus[menuList[i]]) {
				color = video::SColor(255, 128, 128, 128);
			}
			s32 sentenceX = sentenceOffsetX;
			if (i == menuIndex){
				sentenceX += selectedItemAdditionalOffsetX;
				color = irr::video::SColor(255, 200, 200, 20);
				if (!itemActiveStatus[menuList[i]]) {
					color = video::SColor(255, 160, 160, 160);
				}
			}
			fk_addons::drawBorderedText(mainFont, itemToDraw.c_str(),
				core::rect<s32>(sentenceX,
				sentenceOffsetY,
				sentenceX + sentenceWidth,
				sentenceOffsetY + sentenceHeight),
				color, video::SColor(128, 0, 0, 0),
				false, false);
			sentenceOffsetY += sentenceHeight;
			if (std::find(drawingGaps.begin(), drawingGaps.end(), i) != drawingGaps.end()){
				sentenceOffsetY += sentenceHeight / 2;
			}
		}
		// draw option values
		sentenceOffsetY = sentenceInitialY;
		for (s32 i = 0; i < menuList.size(); ++i){
			s32 sentenceHeight = (s32)mainFont->getDimension(L"A").Height;
			if (!menuItemsParameters[menuList[i]].empty()){
				std::wstring itemToDraw = menuItemsParameters[menuList[i]][menuItemsIndices[i]].second;
				s32 sentenceWidth = mainFont->getDimension(itemToDraw.c_str()).Width;
				sentenceHeight = mainFont->getDimension(itemToDraw.c_str()).Height;
				video::SColor color = video::SColor(255, 20, 225, 120);
				if (!itemActiveStatus[menuList[i]]) {
					color = video::SColor(255, 128, 128, 128);
				}
				s32 sentenceX = screenSize.Width - sentenceOffsetX - sentenceWidth;
				if (i == menuIndex){
					color = video::SColor(255, 20, 225, 220);
					if (!itemActiveStatus[menuList[i]]) {
						color = video::SColor(255, 160, 160, 160);
					}
				}
				fk_addons::drawBorderedText(mainFont, itemToDraw.c_str(),
					core::rect<s32>(sentenceX,
					sentenceOffsetY,
					sentenceX + sentenceWidth,
					sentenceOffsetY + sentenceHeight),
					color, video::SColor(128, 0, 0, 0),
					false, false);
			}
			sentenceOffsetY += sentenceHeight;
			if (std::find(drawingGaps.begin(), drawingGaps.end(), i) != drawingGaps.end()){
				sentenceOffsetY += sentenceHeight / 2;
			}
		}
	}
}