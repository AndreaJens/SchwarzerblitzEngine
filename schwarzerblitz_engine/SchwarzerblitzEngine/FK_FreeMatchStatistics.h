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