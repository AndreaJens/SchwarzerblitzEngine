/*
	*** Schwarzerblitz 3D Fighting Game Engine  ***

	=================== Source Code ===================
	Copyright (C) 2016-2022 Andrea Demetrio

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation and/or
	   other materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its contributors may be
	   used to endorse or promote products derived from  this software without specific
	   prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	=============== Additional Components ==============
	Please refer to the license/irrlicht/ and license/SFML/ folder for the license
	indications concerning those components. The irrlicht-schwarzerlicht engine and
	the SFML code and binaries are subject to their own licenses, see the relevant
	folders for more information.

	=============== Assets and resources ================
	Unless specificed otherwise in the Credits file, the assets and resources
	bundled with this engine are to be considered "all rights reserved" and
	cannot be redistributed without the owner's consent. This includes but it is
	not limited to the characters concepts / designs, the 3D models, the music,
	the sound effects, 2D and 3D illustrations, stages, icons, menu art.

	Tutorial Man, Evil Tutor, and Random:
	Copyright (C) 2016-2022 Andrea Demetrio - all rights reserved
*/

#include "FK_SceneGameFreeMatch.h"

namespace fk_engine{
	// set constructor
	FK_SceneGameFreeMatch::FK_SceneGameFreeMatch() : FK_SceneGame(){
		freeMatchType = FK_SceneFreeMatchType::FreeMatchMultiplayer;
	}
	// process match end
	void FK_SceneGameFreeMatch::processMatchEnd(){
		processCharacterStats();
		matchStatistics.setMatchResult(winnerId);
		processingSceneEnd = true;
		FK_SceneGame::updateSaveFileData();
		activateEndMatchMenu();
	}
	// update end match menu
	void FK_SceneGameFreeMatch::updateEndMatchMenu(){
		u32 inputButtonsForMenu = player1input->getPressedButtons();
		if (getFreeMatchType() == FK_SceneFreeMatchType::FreeMatchCPUVsPlayer2){
			inputButtonsForMenu = player2input->getPressedButtons();
		}else if (getFreeMatchType() == FK_SceneFreeMatchType::FreeMatchCPUVsCPUPlayer2Selects) {
			inputButtonsForMenu = player2input->getPressedButtons();
		}
		else if (getFreeMatchType() == FK_SceneFreeMatchType::FreeMatchMultiplayer){
			inputButtonsForMenu |= player2input->getPressedButtons();
		}
		endMatchMenu->update(inputButtonsForMenu);
		f32 timerSpeed = fk_constants::FK_GameTimerNormalVelocity;
		if (endMatchMenu->itemIsSelected()){
			s32 menuIndex = endMatchMenu->getIndex();
			endMatchMenu->resetSelection();
			switch (menuIndex){
			case 0:
				Sleep(500);
				restartMatch();
				break;
			case 1:
				abortMatch = true;
				setNextScene(getPreviousScene());
				device->getTimer()->setSpeed(timerSpeed);
				Sleep(500);
				break;
			case 2:
				abortMatch = true;
				setNextScene(FK_SceneType::SceneMainMenu);
				device->getTimer()->setSpeed(timerSpeed);
				Sleep(500);
				break;
			default:
				break;
			}
		}
	}

	// activate/disable end match menu
	void FK_SceneGameFreeMatch::activateEndMatchMenu(){
		endMatchMenu->reset();
		endMatchMenu->setVisible(true);
		endMatchMenu->setActive(true);
	}
	void FK_SceneGameFreeMatch::disableEndMatchMenu(){
		endMatchMenu->setVisible(false);
		endMatchMenu->setActive(false);
	}

	// load end match menu after all other resources are
	void FK_SceneGameFreeMatch::setupAdditionalParametersAfterLoading(){
		setupEndMatchMenu();
	}

	// process round statistics
	void FK_SceneGameFreeMatch::processEndOfRoundStatistics(){
		std::string key1 = player1->getOriginalName();
		std::replace(key1.begin(), key1.end(), ' ', '_');
		std::string key2 = player2->getOriginalName();
		std::replace(key2.begin(), key2.end(), ' ', '_');
		double damageDonePlayer1 = (player2->getMaxLife() - player2->getLife());
		double damageDonePlayer2 = (player1->getMaxLife() - player1->getLife());
		if (!player1isAI){
			characterStatisticsStructure[key1].totalDamageDone += damageDonePlayer1;
			characterStatisticsStructure[key1].VsRecords[key2].totalDamageDone += damageDonePlayer1;
			characterStatisticsStructure[key1].totalDamageSuffered += damageDonePlayer2;
			characterStatisticsStructure[key1].VsRecords[key2].totalDamageSuffered += damageDonePlayer2;
		}
		if (!player2isAI){
			characterStatisticsStructure[key2].totalDamageDone += damageDonePlayer2;
			characterStatisticsStructure[key2].VsRecords[key1].totalDamageDone += damageDonePlayer2;
			characterStatisticsStructure[key2].totalDamageSuffered += damageDonePlayer1;
			characterStatisticsStructure[key2].VsRecords[key1].totalDamageSuffered += damageDonePlayer1;
		}
		if (winnerId == -1){
			if (!player1isAI){
				characterStatisticsStructure[key1].numberOfRoundsWon += 1;
				characterStatisticsStructure[key1].VsRecords[key2].numberOfRoundsWon += 1;
			}
			if (!player2isAI){
				characterStatisticsStructure[key2].numberOfRoundsLost += 1;
				characterStatisticsStructure[key2].VsRecords[key1].numberOfRoundsLost += 1;
			}
			if (isTimeUp()){
				if (!player1isAI){
					characterStatisticsStructure[key1].numberOfTimeoutWins += 1;
					characterStatisticsStructure[key1].VsRecords[key2].numberOfTimeoutWins += 1;
				}
				if (!player2isAI){
					characterStatisticsStructure[key2].numberOfTimeoutLosses += 1;
					characterStatisticsStructure[key2].VsRecords[key1].numberOfTimeoutLosses += 1;
				}
			}
		}
		else if (winnerId == 1){
			if (!player2isAI){
				characterStatisticsStructure[key2].numberOfRoundsWon += 1;
				characterStatisticsStructure[key2].VsRecords[key1].numberOfRoundsWon += 1;
			}
			if (!player1isAI){
				characterStatisticsStructure[key1].numberOfRoundsLost += 1;
				characterStatisticsStructure[key1].VsRecords[key2].numberOfRoundsLost += 1;
			}
			if (isTimeUp()){
				if (!player2isAI){
					characterStatisticsStructure[key2].numberOfTimeoutWins += 1;
					characterStatisticsStructure[key2].VsRecords[key1].numberOfTimeoutWins += 1;
				}
				if (!player1isAI){
					characterStatisticsStructure[key1].numberOfTimeoutLosses += 1;
					characterStatisticsStructure[key1].VsRecords[key2].numberOfTimeoutLosses += 1;
				}
			}
		}
		else{
			if (!player1isAI){
				characterStatisticsStructure[key1].numberOfDraws += 1;
				characterStatisticsStructure[key1].VsRecords[key2].numberOfDraws += 1;
			}
			if (!player2isAI){
				characterStatisticsStructure[key2].numberOfDraws += 1;
				characterStatisticsStructure[key2].VsRecords[key1].numberOfDraws += 1;
			}
		}
		FK_SceneGame::processEndOfRoundStatistics();
	}

	// update additional scene logic
	void FK_SceneGameFreeMatch::updateAdditionalSceneLogic(u32 delta_t_ms){
		/* update end match menu, if active*/
		if (endMatchMenu->isActive()){
			updateEndMatchMenu();
		}
	}

	/* draw additional HUD items*/
	void FK_SceneGameFreeMatch::drawAdditionalHUDItems(f32 delta_t_s){
		// draw end match menu
		if (endMatchMenu->isVisible()){
			endMatchMenu->draw();
		}
		if (endMatchMenu->isActive()){
			hudManager->drawEndMatchStatistics(matchStatistics);
		}
	}

	/* setup end match menu */
	void FK_SceneGameFreeMatch::setupEndMatchMenu(){
		std::vector<std::string> menuItems = {
			"Play Again",
			"Change characters",
			"back To main menu"
		};
		endMatchMenu = new FK_InGameMenu(device, getSoundManager(), gameOptions, menuItems, "windowskin.png", commonResourcesPath);
		if (gameOptions->getBorderlessWindowMode()){
			endMatchMenu->setViewport(core::dimension2di(-1, -1), borderlessWindowRenderTarget->getSize());
		}
	}

	/* restart match*/
	void FK_SceneGameFreeMatch::restartMatch(){
		disableEndMatchMenu();
		FK_SceneGame::restartMatch();
	}

	void FK_SceneGameFreeMatch::setFreeMacthType(FK_SceneFreeMatchType type){
		freeMatchType = type;
	}
	FK_Scene::FK_SceneFreeMatchType FK_SceneGameFreeMatch::getFreeMatchType(){
		return freeMatchType;
	}
}