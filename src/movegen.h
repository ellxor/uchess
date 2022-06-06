#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include <stddef.h>
#include <stdint.h>
#include "position.h"

struct Move {
	uint16_t start: 6, end: 6, piece: 3, castling: 1;
};

struct MoveList {
	struct Move moves[256];
	size_t length;
};

struct MoveList generate_moves(struct Position pos);
struct Position make_move(struct Position pos, struct Move move);

#endif /*MOVEGEN_H_*/
