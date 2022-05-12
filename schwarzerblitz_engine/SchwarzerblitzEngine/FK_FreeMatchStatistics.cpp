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

#include"FK_FreeMatchStatistics.h"

namespace fk_engine{
	FK_FreeMatchStatistics::FK_FreeMatchStatistics(){
		clear();
	}

	void FK_FreeMatchStatistics::clear(){
		numberOfRoundsPlayed = 0;
		victoriesPlayer1 = 0;
		victoriesPlayer2 = 0;
		roundWonPlayer1 = 0;
		roundWonPlayer2 = 0;
		damageReceivedPlayer1 = 0.f;
		damageReceivedPlayer2 = 0.f;
		numberOfRingoutVictoryPlayer1 = 0;
		numberOfRingoutVictoryPlayer2 = 0;
		numberOfPerfectsPlayer1 = 0;
		numberOfPerfectsPlayer2 = 0;
		numberOfRoundsPlayed = 0;
	}

	void FK_FreeMatchStatistics::setRoundResult(s32 winnerId, FK_Character* player1, FK_Character* player2){
		if (winnerId <= 0){
			roundWonPlayer1 += 1;
		}
		if (winnerId >= 0){
			roundWonPlayer2 += 1;
		}
		numberOfRoundsPlayed += 1;
		damageReceivedPlayer1 += player1->getMaxLife() - player1->getLife();
		damageReceivedPlayer2 += player2->getMaxLife() - player2->getLife();
	}
	void FK_FreeMatchStatistics::setMatchResult(s32 winnerId){
		if (winnerId < 0){
			victoriesPlayer1 += 1;
		}
		if (winnerId > 0){
			victoriesPlayer2 += 1;
		}
	};
	void FK_FreeMatchStatistics::markRingout(s32 winnerId){
		if (winnerId < 0){
			numberOfRingoutVictoryPlayer1 += 1;
		}
		if (winnerId > 0){
			numberOfRingoutVictoryPlayer2 += 1;
		}
	}
	void FK_FreeMatchStatistics::markPerfect(s32 winnerId){
		if (winnerId < 0){
			numberOfPerfectsPlayer1 += 1;
		}
		if (winnerId > 0){
			numberOfPerfectsPlayer2 += 1;
		}
	}
	f32 FK_FreeMatchStatistics::getScorePlayer1(){
		return 0.f;
	}
	f32 FK_FreeMatchStatistics::getScorePlayer2(){
		return 0.f;
	}
	u32 FK_FreeMatchStatistics::getNumberOfWins(s32 playerId){
		if (playerId < 0){
			return victoriesPlayer1;
		}
		if (playerId > 0){
			return victoriesPlayer2;
		}
		return 0;
	}
	u32 FK_FreeMatchStatistics::getNumberOfRoundWins(s32 playerId){
		if (playerId < 0){
			return roundWonPlayer1;
		}
		if (playerId > 0){
			return roundWonPlayer2;
		}
		return 0;
	}
	u32 FK_FreeMatchStatistics::getNumberOfRingoutVictories(s32 playerId){
		if (playerId < 0){
			return numberOfRingoutVictoryPlayer1;
		}
		if (playerId > 0){
			return numberOfRingoutVictoryPlayer2;
		}
		return 0;
	}
	u32 FK_FreeMatchStatistics::getNumberOfPerfectVictories(s32 playerId){
		if (playerId < 0){
			return numberOfPerfectsPlayer1;
		}
		if (playerId > 0){
			return numberOfPerfectsPlayer2;
		}
		return 0;
	}
	f32 FK_FreeMatchStatistics::getAverageLifeRatio(s32 playerId){
		if (numberOfRoundsPlayed == 0){
			return 0.f;
		}
		if (playerId < 0){
			return damageReceivedPlayer1 / (f32)numberOfRoundsPlayed;
		}
		if (playerId > 0){
			return damageReceivedPlayer2 / (f32)numberOfRoundsPlayed;
		}
		return 0.f;
	}
}