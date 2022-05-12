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

#ifndef FK_FREEMATCHSTATS_CLASS
#define FK_FREEMATCHSTATS_CLASS

#include"FK_Character.h"

namespace fk_engine{
	class FK_FreeMatchStatistics{
	public:
		FK_FreeMatchStatistics();
		void setRoundResult(s32 winnerId, FK_Character* player1, FK_Character* player2);
		void setMatchResult(s32 winnerId);
		void markRingout(s32 winnerId);
		void markPerfect(s32 winnerId);
		f32 getScorePlayer1();
		f32 getScorePlayer2();
		u32 getNumberOfWins(s32 playerId);
		u32 getNumberOfRoundWins(s32 playerId);
		u32 getNumberOfRingoutVictories(s32 playerId);
		u32 getNumberOfPerfectVictories(s32 playerId);
		f32 getAverageLifeRatio(s32 playerId);
		void clear();
	private:
		u32 victoriesPlayer1;
		u32 victoriesPlayer2;
		u32 roundWonPlayer1;
		u32 roundWonPlayer2;
		f32 damageReceivedPlayer1;
		f32 damageReceivedPlayer2;
		u32 numberOfRingoutVictoryPlayer1;
		u32 numberOfRingoutVictoryPlayer2;
		u32 numberOfPerfectsPlayer1;
		u32 numberOfPerfectsPlayer2;
		u32 numberOfRoundsPlayed;
	};
};

#endif