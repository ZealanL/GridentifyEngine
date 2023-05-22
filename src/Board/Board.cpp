#include "Board.h"

constexpr int8_t DIR_OFFSETS[DIR_AMOUNT] = {
	-1, // left
	1, // right
	BOARD_DIM, // up
	-BOARD_DIM, // down
};

constexpr uint8_t OPPOSITE_DIRS[DIR_AMOUNT]{
	DIR_RIGHT,
	DIR_LEFT,
	DIR_DOWN,
	DIR_UP
};

template <uint8_t EXCLUDE_DIR, bool JUST_COUNT, typename T>
void MakeTripleMove(const Board& board, Pos from, Pos from2, T& listOrCount) {

	CellVal fromVal = board.valsFlat[from.idx];

	uint8_t 
		fromX = from.GetX(),
		fromY = from.GetY();

	for (size_t i = 0; i < DIR_AMOUNT; i++) {
		if (i == EXCLUDE_DIR)
			continue;

		if ((fromX == 0) && (i == DIR_LEFT))
			continue;

		if ((fromX == BOARD_DIM - 1) && (i == DIR_RIGHT))
			continue;

		if ((fromY == BOARD_DIM - 1) && (i == DIR_UP))
			continue;

		if ((fromY == 0) && (i == DIR_DOWN))
			continue;

		Pos to = Pos(from.idx + DIR_OFFSETS[i]);
		if (board.valsFlat[to.idx] == fromVal) {
			if constexpr (JUST_COUNT) {
				listOrCount++;
			} else {
				ASSERT(to.idx != from.idx && from.idx != from2.idx && to.idx != from2.idx);
				Move move = Move{
					Move::TYPE_TRIPLE,
					from, to, from2
				};
				listOrCount.Add(move);
			}
		}
	}
}

template <uint8_t DIR, bool JUST_COUNT, typename T>
void MakeMovesDir(const Board& board, T& listOrCount, size_t movesBitMask) {
	if constexpr (DIR == DIR_LEFT || DIR == DIR_RIGHT) {
		// Horizontal
		constexpr size_t 
			START_X = (DIR == DIR_LEFT) ? 1 : 0,
			END_X = BOARD_DIM - ((DIR == DIR_LEFT) ? 0 : 1);

		for (size_t y = 0; y < BOARD_DIM; y++) {
			const CellVal* row = board.vals[y];
			for (size_t x = START_X; x < END_X; x++) {

				size_t toX = (DIR == DIR_LEFT) ? (x - 1) : (x + 1);
				if (row[x] == row[toX]) {

					Pos to = Pos(toX, y);
					Pos from = Pos(x, y);

					size_t toMask = ((size_t)1 << (size_t)(to.idx));
					size_t fromMask = ((size_t)1 << (size_t)(from.idx));

					if constexpr (JUST_COUNT) {
						listOrCount++;
					} else {
						Move move = Move{
							Move::TYPE_NORMAL,
							Pos(x, y),
							Pos(toX, y)
						};
						listOrCount.Add(move);

						std::swap(move.to, move.from);
						listOrCount.Add(move);
					}

					if (movesBitMask & toMask) {
						// Triple move
						MakeTripleMove<OPPOSITE_DIRS[DIR], JUST_COUNT, T>(board, to, from, listOrCount);
					}

					if (movesBitMask & fromMask) {
						// Triple move
						MakeTripleMove<DIR, JUST_COUNT, T>(board, from, to, listOrCount);
					}

					movesBitMask |= fromMask;
					movesBitMask |= toMask;
				}
				
			}
		}
	} else {
		// Vertical
		constexpr size_t 
			START_Y = (DIR == DIR_UP) ? 0 : 1,
			END_Y = BOARD_DIM - ((DIR == DIR_UP) ? 1 : 0);

		constexpr size_t
			START_IDX = START_Y * BOARD_DIM,
			END_IDX = END_Y * BOARD_DIM;

		for (size_t i = START_IDX; i < END_IDX; i++) {
			size_t toI = (DIR == DIR_UP) ? (i + BOARD_DIM) : (i - BOARD_DIM);
			if (board.valsFlat[i] == board.valsFlat[toI]) {

				size_t fromMask = ((size_t)1 << i);
				size_t toMask = ((size_t)1 << toI);

				Pos from = Pos(i);
				Pos to = Pos(toI);

				if constexpr (JUST_COUNT) {
					listOrCount += board.valsFlat[i] == board.valsFlat[toI];
				} else {
					Move move = Move{
						Move::TYPE_NORMAL,
						from, to,
					};
					listOrCount.Add(move);

					std::swap(move.to, move.from);
					listOrCount.Add(move);
				}

				if (movesBitMask & toMask) {
					// Triple move
					MakeTripleMove<OPPOSITE_DIRS[DIR], JUST_COUNT, T>(board, to, from, listOrCount);
				}

				if (movesBitMask & fromMask) {
					// Triple move
					MakeTripleMove<DIR, JUST_COUNT, T>(board, from, to, listOrCount);
				}

				movesBitMask |= fromMask;
				movesBitMask |= toMask;
			}
		}
	}

}

void Board::MakeMoves(MoveList& moveList) {
	size_t movesBitMask = 0;
	MakeMovesDir<DIR_LEFT, false>(*this, moveList, movesBitMask);
	MakeMovesDir<DIR_UP, false>(*this, moveList, movesBitMask);
}

size_t Board::CountMoves() {
	size_t count = 0;
	size_t movesBitMask = 0;
	MakeMovesDir<DIR_LEFT, true>(*this, count, movesBitMask);
	MakeMovesDir<DIR_UP, true>(*this, count, movesBitMask);
	return count;
}

std::ostream& operator<<(std::ostream& stream, const Pos& pos) {
	stream << (char)('a' + pos.GetX()) << (int)(pos.GetY() + 1);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Move& move) {
	if (move.type == Move::TYPE_INVALID) {
		stream << "[INVALID]";
	} else if (move.type == Move::TYPE_TRIPLE) {
		stream << move.from2 << move.from << move.to;
	} else {
		stream << move.from << move.to;
	}
	
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Board& board) {
	stream << " {" << std::endl;
	for (size_t y = 0; y < BOARD_DIM; y++) {
		stream << "\t";
		for (size_t x = 0; x < BOARD_DIM; x++) {
			if (x > 0)
				stream << ", ";

			stream << board.vals[y][x];
		}
		stream << std::endl;
	}
	stream << "} ";
	return stream;
}
