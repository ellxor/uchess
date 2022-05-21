#ifndef POSITION_H_
#define POSITION_H_

#include "bits.h"

enum PieceType {
	None, Pawn, Knight, Bishop, Rook, Queen, King, Info
};

struct Position {
	bitboard white, X,Y,Z;
};

enum Color {
	White, Black
};

struct PositionInfo {
	bitboard en_passant:      3,
	         has_en_passant:  1,
	         side_to_move:    1,
	         white_kingside:  1,
	         white_queenside: 1,
	         black_kingside:  1,
	         black_queenside: 1;

};

static inline bitboard extract(struct Position pos, enum PieceType T) {
	bitboard bb;
	bb  = (T & 1) ? pos.X : ~pos.X;
	bb &= (T & 2) ? pos.Y : ~pos.Y;
	bb &= (T & 4) ? pos.Z : ~pos.Z;
	return bb;
}

static inline bitboard occupied(struct Position pos) {
	return (pos.X ^ pos.Y) | (pos.X ^ pos.Z);
}

#endif /*POSITION_H_*/