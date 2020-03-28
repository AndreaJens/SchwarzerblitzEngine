#ifndef FK_MOVELISTMOVE_CLASS
#define FK_MOVELISTMOVE_CLASS

#include"FK_Move.h"

namespace fk_engine{
	class FK_MoveListMove{
	public:
		FK_MoveListMove();
		void setMove(FK_Move newMove);
		FK_Move getMove();
		void clear();
		void addMoveToList(FK_MoveListMove* movePointer);
		std::deque<FK_MoveListMove*> & getFollowupMoves();
	private:
		FK_Move move;
		std::deque<FK_MoveListMove*> followUpMoves;
	};
}

#endif