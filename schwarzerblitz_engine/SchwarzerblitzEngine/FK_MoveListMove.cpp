#include"FK_MoveListMove.h"

namespace fk_engine{
	FK_MoveListMove::FK_MoveListMove(){
		clear();
	}
	void FK_MoveListMove::setMove(FK_Move newMove){
		move = newMove;
	}
	FK_Move FK_MoveListMove::getMove(){
		return move;
	}
	void FK_MoveListMove::clear(){
		move.clear();
		followUpMoves.clear();
	}
	void FK_MoveListMove::addMoveToList(FK_MoveListMove* movePointer){
		followUpMoves.push_back(movePointer);
	}
	std::deque<FK_MoveListMove*> & FK_MoveListMove::getFollowupMoves(){
		return followUpMoves;
	}
}