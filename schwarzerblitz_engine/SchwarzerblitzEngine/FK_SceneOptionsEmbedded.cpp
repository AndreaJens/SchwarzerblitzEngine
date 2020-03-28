#include "FK_SceneOptionsEmbedded.h"

namespace fk_engine {

	void FK_SceneOptionsEmbedded::drawAll()
	{
		if (isInInputAssignmentScreen()){
			FK_SceneOptionsEmbedded::drawBackground();
			FK_SceneOptions::drawKeyMappingScene(false);
		}
		else {
			FK_SceneOptions::drawAll();
			if (hasToSetupJoypad()) {
				drawJoypadSetup(getCurrentTime());
			}
		}
	}

	void FK_SceneOptionsEmbedded::setupMenuItems()
	{
		// main menu items
		menuOptionsCategoriesStrings = {
			"Game Options",
			"Keyboard settings (single player)",
			"Keyboard settings (multiplayer)",
			"Joypad settings",
			"Remap joypad to std. controller",
			"Back",
		};
		menuOptionsCategoriesCaptions = {
			"Customize game options",
			"Customize the keyboard layout for single player game modes",
			"Customize the keyboard layout for split-keyboard multiplayer games",
			"Customize the joypad layout (can be selected only if at least a joypad is connected)",
			"Remap the joypad layout on the standard joypad layout used throughout the game. Normally, you need to do this only once",
			"Go back to selection",
		};
		menuOptionsCategoriesClasses = {
			FK_SceneOptions_SubSystem::GameOptions,
			FK_SceneOptions_SubSystem::KeyboardRemappingSolo,
			FK_SceneOptions_SubSystem::KeyboardRemappingMultiplayer,
			FK_SceneOptions_SubSystem::JoypadRemapping,
			FK_SceneOptions_SubSystem::JoypadToXboxTranslation,
			FK_SceneOptions_SubSystem::BackToMenu,
		};
		menuOptionsCategoriesActiveItems = { true, true, true, false, false, true };
		if (joystickInfo.size() > 0) {
			menuOptionsCategoriesActiveItems[3] = true;
			menuOptionsCategoriesActiveItems[4] = true;
		}

		// graphics [0 - 4]
		menuOptionsStrings.push_back("Screen mode");
		menuOptionsStrings.push_back("Resolution");
		menuOptionsStrings.push_back("Shadows");
		menuOptionsStrings.push_back("Shadowmap quality");
		menuOptionsStrings.push_back("Advanced graphic effects");
		graphicsOptionsIndex.clear();
		// sound
		menuOptionsStrings.push_back("Play SFX");
		menuOptionsStrings.push_back("SFX volume");
		menuOptionsStrings.push_back("Play voices");
		menuOptionsStrings.push_back("Voices volume");
		menuOptionsStrings.push_back("Play BGM");
		menuOptionsStrings.push_back("BGM volume");
		soundOptionsIndex = {
			5,6,7,8,9,10,
		};
		// game
		menuOptionsStrings.push_back("Match Timer (arcade mode)");
		menuOptionsStrings.push_back("Match Timer (free match)");
		menuOptionsStrings.push_back("Number of rounds (arcade mode)");
		menuOptionsStrings.push_back("Number of rounds (free match)");
		menuOptionsStrings.push_back("AI Level");

		gameOptionsIndex = {
			11,12,13,14,15
		};

		// additional and late options
		menuOptionsStrings.push_back("Dynamic resolution");

		// other
		menuOptionsStrings.push_back("Cancel");
		menuOptionsStrings.push_back("Confirm");
		// add last two elements to every array
		graphicsOptionsIndex.push_back(menuOptionsStrings.size() - 2);
		graphicsOptionsIndex.push_back(menuOptionsStrings.size() - 1);
		soundOptionsIndex.push_back(menuOptionsStrings.size() - 2);
		soundOptionsIndex.push_back(menuOptionsStrings.size() - 1);
		gameOptionsIndex.push_back(menuOptionsStrings.size() - 2);
		gameOptionsIndex.push_back(menuOptionsStrings.size() - 1);
		// graphics
		menuOptionsCaptions.push_back("Switch between FULL SCREEN and WINDOWED mode - this option is rendered effective once you CONFIRM the changes and go BACK TO THE MAIN MENU");
		menuOptionsCaptions.push_back("Change screen resolution -  this option is rendered effective once you CONFIRM the changes and go BACK TO THE MAIN MENU");
		menuOptionsCaptions.push_back("Toggle SHADOWS ON or OFF. Disabling them could improve performances on older computers");
		menuOptionsCaptions.push_back("Set shadow quality. A higher number requires a higher computational power. If shadows are disabled, this setting has no effect");
		menuOptionsCaptions.push_back("Toggle advanced light effects, such as bloom and blur shaders. Disabling them could improve performances on older computers");
		// sound
		menuOptionsCaptions.push_back("Toggle sound effects");
		menuOptionsCaptions.push_back("Change the volume of sound effects. This value is ignored if PLAY SFX is set to OFF");
		menuOptionsCaptions.push_back("Toggle voice effects");
		menuOptionsCaptions.push_back("Change the volume of voice effects. This value is ignored if PLAY VOICES is set to OFF");
		menuOptionsCaptions.push_back("Toggle background music");
		menuOptionsCaptions.push_back("Change the volume of the music. This value is ignored if PLAY BGM is set to OFF");
		// game
		menuOptionsCaptions.push_back("Set the timer for ARCADE mode. 00 means infinite time");
		menuOptionsCaptions.push_back("Set the timer for FREE MATCH mode. 00 means infinite time");
		menuOptionsCaptions.push_back("Set the number of rounds in a match for ARCADE mode");
		menuOptionsCaptions.push_back("Set the number of rounds in a match for FREE MATCH mode");
		menuOptionsCaptions.push_back("Set the AI level for STORY, ARCADE, and FREE MATCH mode.");

		// additional
		menuOptionsCaptions.push_back("When ON, if the frame rate is low, dynamic resolution tries to improve performance by reducing the rendering size. Switching it OFF prevents this optimization, by always keeping the nominal resolution active");

		//other
		menuOptionsCaptions.push_back("Cancel changes and go back to the option screen");
		menuOptionsCaptions.push_back("Confirm changes and go back to the option screen");
		for (s32 i = 0; i < menuOptionsStrings.size(); ++i) {
			menuActiveItems.push_back(true);
			additionalSpacingAfterOptionIndex.push_back(false);
		}

		//set additional spacing after certain options
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_Resolution] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_DynamicResolution] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_SFXVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_VoiceVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_BGMVolume] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_FreeMatchTimer] = true;
		additionalSpacingAfterOptionIndex[FK_SceneOptions_OptionsIndex::Opt_FreeMatchRounds] = true;
	}

	void FK_SceneOptionsEmbedded::drawBackground()
	{
		core::dimension2d<u32> screenSize;
		screenSize = driver->getScreenSize();
		if (gameOptions->getBorderlessWindowMode()) {
			screenSize = borderlessWindowRenderTarget->getSize();
		}
		core::rect<s32> screenRect = core::rect<s32>(0, 0, (s32)screenSize.Width, (s32)screenSize.Height);
		driver->draw2DRectangle(video::SColor(128, 0, 0, 0), screenRect);
	}

	void FK_SceneOptionsEmbedded::drawCaptions()
	{
	
	}
	void FK_SceneOptionsEmbedded::setupBGM()
	{
	}
	;

}
