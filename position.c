#include "bits.h"
#include "movegen.h"
#include "position.h"
#include <string.h>

// note: square must be None
static inline
void set_square(struct Position *pos, square sq, enum PieceType T) {
	pos->X |= (bitboard)((T >> 0) & 1) << sq;
	pos->Y |= (bitboard)((T >> 1) & 1) << sq;
	pos->Z |= (bitboard)((T >> 2) & 1) << sq;
}

struct Position make_move(struct Position pos, struct Move move) {
	bitboard occ = occupied(pos);
	bitboard bits = pext(extract(pos, Info), ~occ);

	struct PositionInfo info;
	memcpy(&info, &bits, sizeof info);

	enum { A1 = 0, E1 = 4, H1 = 7, A8 = 56, H8 = 63 };

	// construct clear mask
	bitboard clear = ~occ; // clear all info to replace with new info
	clear |= 1L << move.start;
	clear |= 1L << move.end;

	// remove captured en-passant pawn
	if (info.has_en_passant && move.piece == Pawn) {
		square ep = info.en_passant + 40;
		if (move.end == ep) clear |= 1L << (ep + S);
	}

	// remove castling rook
	if (move.castling) {
		clear |= 1L << ((move.end < move.start) ? A1 : H1);
	}

	// clear bits
	pos.white &= ~clear;
	pos.X &= ~clear;
	pos.Y &= ~clear;
	pos.Z &= ~clear;

	// set moved pieces
	pos.white |= 1L << move.end;
	set_square(&pos, move.end, move.piece);

	// set castled rook
	if (move.castling) {
		square mid = (move.start + move.end) >> 1;
		pos.white |= 1L << mid;

		set_square(&pos, mid, Rook);
	}

	// update castling rights
	if (move.piece == King) {
		info.white_kingside = 0;
		info.white_queenside = 0;
	}

	if (move.start == A1) info.white_queenside = 0;
	if (move.start == H1) info.white_kingside  = 0;
	if (move.end   == A8) info.black_queenside = 0;
	if (move.end   == H8) info.black_kingside  = 0;

	// update new en-passant square
	info.has_en_passant = 0;

	if (move.piece == Pawn && move.end - move.start == N+N) {
		info.en_passant = ((move.start + move.end) >> 1) - 40;
		info.has_en_passant = 1;
	}

	info.side_to_move = !info.side_to_move;

	// rotate board
	pos.white = reverse(pos.white);
	pos.X = reverse(pos.X);
	pos.Y = reverse(pos.Y);
	pos.Z = reverse(pos.Z);

	// write info bits
	memcpy(&bits, &info, sizeof info);

	// swap white and black castling rights
	bitboard white_castling = 0b001100000;

	bits = pext(bits, ~white_castling)
	     | ((bits & white_castling) << 2);

	occ  = occupied(pos);
	bits = pdep(bits, ~occ);

	pos.X |= bits;
	pos.Y |= bits;
	pos.Z |= bits;

	// swap white & black bitboards
	pos.white = occ & ~pos.white;

	return pos;
}