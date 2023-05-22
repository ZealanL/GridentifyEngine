#pragma once
#include "../Framework.h"

#define BOARD_DIM 5
#define BOARD_DIM_2 (BOARD_DIM * BOARD_DIM)

#define RAND_CELL() (rand() % 3 + 1)

enum {
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,

	DIR_AMOUNT
};

typedef uint32_t CellVal;

struct Pos {
	uint8_t idx;

	Pos() {}

	Pos(uint8_t idx) : idx(idx) {}

	template<typename T>
	Pos(T x, T y) {
		idx = (y * BOARD_DIM) + x;
	}

	uint8_t GetX() const {
		return idx % BOARD_DIM;
	}

	uint8_t GetY() const {
		return idx / BOARD_DIM;
	}

	friend std::ostream& operator<<(std::ostream& stream, const Pos& pos);
};

struct Move {
	enum {
		TYPE_INVALID = 0,

		TYPE_NORMAL,
		TYPE_TRIPLE
	};

	uint8_t type;

	Pos from, to;
	Pos from2; // If triple

	static Move GetInvalid() {
		return Move{ TYPE_INVALID, 0, 0, 0 };
	}

	bool IsValid() const {
		if (type == TYPE_INVALID)
			return false;

		if (type == TYPE_TRIPLE && from2.idx > BOARD_DIM_2)
			return false;
		
		return (from.idx != to.idx) && (from.idx < BOARD_DIM_2) && (to.idx < BOARD_DIM_2);
	}

	friend std::ostream& operator<<(std::ostream& stream, const Move& move);
};

#define MAX_MOVES ((BOARD_DIM * (BOARD_DIM - 1)) * DIR_AMOUNT * 2)
struct MoveList {
	size_t size = 0;
	Move data[MAX_MOVES];

	const Move& operator[](size_t i) const {
		ASSERT(i < MAX_MOVES);
		return data[i];
	}

	Move& operator[](size_t i) {
		ASSERT(i < MAX_MOVES);
		return data[i];
	}

	const Move* begin() const {
		return data;
	}

	const Move* end() const {
		return data + size;
	}

	void Add(Move move) {
		ASSERT(size < MAX_MOVES);
		data[size] = move;
		size++;
	}
};

struct Board {
	union {
		// Y is first dimension
		CellVal vals[BOARD_DIM][BOARD_DIM];

		CellVal valsFlat[BOARD_DIM_2];
	};

	FINLINE void Reset() {
		for (size_t i = 0; i < BOARD_DIM_2; i++)
			valsFlat[i] = RAND_CELL();
	}

	FINLINE void ExecuteNormalMove(Move move, CellVal newVal) {
		ASSERT(move.type == Move::TYPE_NORMAL);

		valsFlat[move.to.idx] *= 2;
		valsFlat[move.from.idx] = newVal;
	}

	FINLINE void ExecuteTripleMove(Move move, CellVal val1, CellVal val2) {
		ASSERT(move.type == Move::TYPE_TRIPLE);

		valsFlat[move.to.idx] *= 3;
		valsFlat[move.from.idx] = val1;
		valsFlat[move.from2.idx] = val2;
	}

	FINLINE void UndoNormalMove(Move move) {
		ASSERT(move.type == Move::TYPE_NORMAL);

		CellVal oldVal = valsFlat[move.to.idx] / 2;
		valsFlat[move.to.idx] = oldVal;
		valsFlat[move.from.idx] = oldVal;
	}

	FINLINE void UndoTripleMove(Move move) {
		ASSERT(move.type == Move::TYPE_TRIPLE);

		CellVal oldVal = valsFlat[move.to.idx] / 3;
		valsFlat[move.to.idx] = oldVal;
		valsFlat[move.from.idx] = oldVal;
		valsFlat[move.from2.idx] = oldVal;
	}

	void MakeMoves(MoveList& moveList);
	size_t CountMoves();

	friend std::ostream& operator<<(std::ostream& stream, const Board& board);
};