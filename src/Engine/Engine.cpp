#include "Engine.h"

inline Board g_Board = Board();

void Engine::SetBoard(Board& board) {
	g_Board = board;
}

Value Engine::GetBoardEval(Board& board) {

	// Based on https://github.com/LHolten/Gridentify/blob/master/test.py#L33
	constexpr Value CORNER_WEIGHTS[BOARD_DIM_2] = {
		9, 6, 4, 6, 9,
		6, 4, 2, 4, 6,
		4, 2, 1, 2, 4,
		6, 4, 2, 4, 6,
		9, 6, 4, 6, 9
	};

	Value cornerEval = 0;
	for (size_t i = 0; i < BOARD_DIM_2; i++) {
		CellVal cellVal = board.valsFlat[i];
		cornerEval += cellVal * CORNER_WEIGHTS[i] * CORNER_WEIGHTS[i];
	}

	Value pairCount = board.CountPairs();

	// Use move count as value
	return (cornerEval * pairCount);
}


Value SearchRecursive(Board& board, size_t depthRemaining, size_t& totalLeafNodes, Move* bestMoveOut) {

	if (depthRemaining > 0) {
		MoveList moves;
		board.MakeMoves(moves);

		Value bestEval = -1;
		for (Move move : moves) {

			ASSERT(move.IsValid());
			Value sumEval = 0;
			if (move.type == Move::TYPE_NORMAL) {
				// Normal move
				for (uint32_t newVal = 1; newVal <= 3; newVal++) {
					board.ExecuteNormalMove(move, newVal);
					Value eval = SearchRecursive(board, depthRemaining - 1, totalLeafNodes, NULL);
					board.UndoNormalMove(move);

					sumEval += eval;
				}
			} else {
				// Triple move

				ASSERT(move.type == Move::TYPE_TRIPLE);
				for (CellVal newVal1 = 1; newVal1 <= 3; newVal1++) {
					for (CellVal newVal2 = 1; newVal2 <= 3; newVal2++) {
						board.ExecuteTripleMove(move, newVal1, newVal2);
						Value eval = SearchRecursive(board, depthRemaining - 1, totalLeafNodes, NULL);
						board.UndoTripleMove(move);

						sumEval += eval / 3;
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
	bestEval = SearchRecursive(board, depth, totalLeafNodes, &bestMove);

	if (print)
		LOG("Best move: " << bestMove << ", eval: " << bestEval);

	if (evalOut)
		*evalOut = bestEval;

	return bestMove;
}