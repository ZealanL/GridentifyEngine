#pragma once
#include "../Board/Board.h"

typedef int64_t Value;

namespace Engine {
	void SetBoard(Board& board);

	Value GetBoardEval(Board& board);

	Move FindBestMove(size_t depth = 4, bool print = true, Value* evalOut = NULL);
}