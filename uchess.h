#ifndef UCHESS_H_
#define UCHESS_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <x86intrin.h>

#define MAX_MOVELIST_LENGTH 256

typedef uint64_t bitboard;

enum PieceType {
	None, Pawn, Knight, Bishop, Rook, Queen, King, Info,
};

enum PositionInfo {
	EP_MASK = 0x0ff,  // 8-bit en passant mask (file)
	WK_MASK = 0x100,  // castling masks
	WQ_MASK = 0x200,
	BK_MASK = 0x400,
	BQ_MASK = 0x800,
};

struct Position {
	bitboard white, X, Y, Z;
};

enum Color { BLACK, WHITE };

struct State {
	struct Position pos;
	enum Color side_to_move;

	unsigned fify_move_clock;
	unsigned movenumber;
};


// forsyths edwards notation
struct State parse_fen(const char *fen, bool *ok, FILE *stream);
size_t generate_fen(struct State state, char *buffer);

// standard algebraic notation
size_t generate_san(struct Move move, struct State state, char *buffer, bool check_and_mate);
size_t generate_uci(struct Move move, struct State state, char *buffer);


// chess logic
struct Move {
	uint16_t start: 6, end: 6, piece: 3, castling: 1;
};

struct MoveList {
	struct Move moves[MAX_MOVELIST_LENGTH];
	size_t length;
};

// initialises tables used for move generation
void init_bitbase();

struct MoveList generate_moves(struct Position pos);
struct Position make_move(struct Position pos, struct Move move);
bitboard enemy_checks(struct Position pos);

// inline functions
static inline
enum PieceType get_piece(struct Position pos, int square) {
	assert(0 <= square && square < 64);
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

static inline
bitboard occupied(struct Position pos) {
	return (pos.X ^ pos.Y) | (pos.X ^ pos.Z);
}


#endif //UCHESS_H_
