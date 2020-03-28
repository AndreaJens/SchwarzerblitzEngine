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