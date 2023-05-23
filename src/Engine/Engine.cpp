#include "Engine.h"

inline Board g_Board = Board();

void Engine::SetBoard(Board& board) {
	g_Board = board;
}

FINLINE Value GetNeighbourEval(CellVal a, CellVal b) {
	constexpr Value
		CELL_SAME_VAL = 16,
		CELL_HALF_VAL = 5;

	if (a == b) {
		return CELL_SAME_VAL;
	} else if (a == (b / 2) || b == (a / 2)) {
		return CELL_HALF_VAL;
	} else {
		return 0;
	}
}

Value Engine::GetBoardEval(Board& board) {

	Value eval = 0;
	for (size_t i = BOARD_DIM; i < BOARD_DIM_2; i++) {
		CellVal cellVal = board.valsFlat[i];

		CellVal cellMergeVal = 0;

		// Left neighbour
		if (i % BOARD_DIM != 0) {
			cellMergeVal += GetNeighbourEval(cellVal, board.valsFlat[i - 1]);
		}

		// Down neighbour
		{
			cellMergeVal += GetNeighbourEval(cellVal, board.valsFlat[i - BOARD_DIM]);
		}

		eval += cellMergeVal;
	}

	return eval;
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