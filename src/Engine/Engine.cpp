#include "Engine.h"

inline Board g_Board = Board();

void Engine::SetBoard(Board& board) {
	g_Board = board;
}

Value Engine::GetBoardEval(Board& board) {
	// Use move count as value
	return board.CountMoves();
}


// Pessimistic mode searches assuming the worst possible RNG outcome,
//	instead of averaging the results of all possible outcomes
#define PESSIMISTIC 0

Value SearchRecursive(Board& board, size_t depthRemaining, size_t& totalLeafNodes,
#if PESSIMISTIC
	int64_t min,
#endif
	Move* bestMoveOut
) {

	if (depthRemaining > 0) {
		MoveList moves;
		board.MakeMoves(moves);

		Value bestEval = -1;
		for (Move move : moves) {

			ASSERT(move.IsValid());
			Value sumEval
#if PESSIMISTIC
				= INT32_MAX;
#else
				= 0;
#endif
			if (move.type == Move::TYPE_NORMAL) {
				// Normal move
				for (uint32_t newVal = 1; newVal <= 3; newVal++) {
					board.ExecuteNormalMove(move, newVal);
					Value eval = SearchRecursive(board, depthRemaining - 1, totalLeafNodes,

#if PESSIMISTIC
						bestEval,
#endif
						NULL
					);
					board.UndoNormalMove(move);

#if PESSIMISTIC
					if (eval <= min) {
						// Prune
						sumEval = eval;
						break;
					}

					sumEval = MIN(sumEval, eval);
#else
					sumEval += eval;
#endif
				}
			} else {
				// Triple move

				ASSERT(move.type == Move::TYPE_TRIPLE);
				for (CellVal newVal1 = 1; newVal1 <= 3; newVal1++) {
					for (CellVal newVal2 = 1; newVal2 <= 3; newVal2++) {
						board.ExecuteTripleMove(move, newVal1, newVal2);
						Value eval = SearchRecursive(board, depthRemaining - 1, totalLeafNodes,

#if PESSIMISTIC
							bestEval,
#endif
							NULL
						);
						board.UndoTripleMove(move);

#if PESSIMISTIC
						if (eval <= min) {
							// Prune
							sumEval = eval;
							break;
						}

						sumEval = MIN(sumEval, eval);
#else
						sumEval += eval / 3;
#endif
					}
				}
			}

			if (sumEval > bestEval) {
				if (bestMoveOut)
					*bestMoveOut = move;
				bestEval = sumEval;
			}
		}

		return bestEval;

	} else {
		totalLeafNodes++;
		return Engine::GetBoardEval(board);
	}
}

Move Engine::FindBestMove(size_t depth, bool print, Value* evalOut) {
	if (depth == 0)
		THROW("Depth must be greater than zero");

	Move bestMove = Move::GetInvalid();
	Value bestEval = -1;
	size_t totalLeafNodes = 0;
	Board board = g_Board;
	bestEval = SearchRecursive(board, depth, totalLeafNodes, 
#if PESSIMISTIC
		-1,
#endif	
		&bestMove);

	if (print)
		LOG("Best move: " << bestMove << ", eval: " << bestEval);

	if (evalOut)
		*evalOut = bestEval;

	return bestMove;
}