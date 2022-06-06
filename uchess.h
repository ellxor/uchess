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

struct MoveList generate_moves(struct Position pos);
struct Position make_move(struct Position pos, struct Move move);

#endif //UCHESS_H_
