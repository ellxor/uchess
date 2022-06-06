#ifndef UCHESS_H_
#define UCHESS_H_

#include <stddef.h>
#include <stdint.h>

#define MAX_MOVELIST_LENGTH 256

typedef uint64_t bitboard;

enum PieceType {
	None, Pawn, Knight, Bishop, Rook, Queen, King, Info,
};

struct Position {
	bitboard white, X,Y,Z;
};

struct Move {
	uint16_t start: 6, end: 6, piece: 3, castling: 1;
};

struct MoveList {
	struct Move moves[MAX_MOVELIST_LENGTH];
	size_t length;
};

#endif //UCHESS_H_
