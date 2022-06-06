#ifndef UCHESS_EXTRA_H_
#define UCHESS_EXTRA_H_

#include "uchess.h"

#include <assert.h>
#include <x86intrin.h>

static inline
enum PieceType get_piece(struct Position pos, int square) {
	assert(0 <= square && square <= 64);

	int T = 0;

	T |= ((pos.X >> square) & 1) << 0;
	T |= ((pos.Y >> square) & 1) << 1;
	T |= ((pos.Z >> square) & 1) << 2;

	return (enum PieceType) T;
}

static inline
bitboard extract_bitboard(struct Position pos, enum PieceType T) {
	assert(T != Info && "use extract info instead");

	bitboard bb;
	bb  = (T & 1) ? pos.X : ~pos.X;
	bb &= (T & 2) ? pos.Y : ~pos.Y;
	bb &= (T & 4) ? pos.Z : ~pos.Z;
	return bb;
}

static inline
bitboard extract_info(struct Position pos) {
	bitboard occ = (pos.X ^ pos.Y) | (pos.X ^ pos.Z);
	bitboard info = pos.X & pos.Y & pos.Z;

	return _pext_u64(info, ~occ);
}

#endif //UCHESS_EXTRA_H_
