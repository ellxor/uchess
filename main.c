#include "bits.h"
#include "movegen.h"
#include "position.h"
#include <stdio.h>

size_t perft(struct Position pos, size_t depth) {
	if (depth == 0) return 1;

	struct MoveList list = generate_moves(pos);
	if (depth == 1) return list.length;

	size_t total = 0;

	for (size_t i = 0; i < list.length; i++) {
		struct Position child = make_move(pos, list.moves[i]);
		total += perft(child, depth - 1 );
	}

	return total;
}

int main() {
	init_bitbase();

	struct Position pos = {
		.white = 0x000000000000ffff,
		.X     = 0x2cff00000000ff2c,
		.Y     = 0x7600000000000076,
		.Z     = 0x9900000000000099,
	};

	bitboard mask = WK_MASK | WQ_MASK | BK_MASK | BQ_MASK;
	mask = pdep(mask, ~occupied(pos));

	pos.X |= mask;
	pos.Y |= mask;
	pos.Z |= mask;

	struct MoveList list = generate_moves(pos);
	enum { depth = 6 };

	size_t total = 0;

	for (size_t i = 0; i < list.length; i++) {
		struct Move move = list.moves[i];
		putchar('a' + move.start % 8);
		putchar('1' + move.start / 8);
		putchar('a' + move.end % 8);
		putchar('1' + move.end / 8);

		struct Position child = make_move(pos, move);
		size_t nodes = perft(child, depth - 1);

		printf(": %zu\n", nodes);
		total += nodes;
	}

	printf("\ntotal: %zu\n", total);
}