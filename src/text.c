#include "text.h"
#include "bits.h"
#include "position.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#define PRINTF(fmt, args) __attribute__((format (printf, fmt, args)))
#else
#define PRINTF(fmt, args)
#endif

#define RED	"\033[31;1m"
#define RESET	"\033[0m"


struct Parser {
	FILE *output;
	const char *fen;
	const char *offset;
};

static inline
char peek_next(struct Parser *parser) {
	return *parser->offset;
}

static inline
char chop_next(struct Parser *parser) {
	return *parser->offset++;
}

static inline
int chop_int(struct Parser *parser) {
	char *endptr;
	int val = strtol(parser->offset, &endptr, 10);

	if (endptr == parser->offset) {
		return 0;
	}

	parser->offset = endptr;
	return val;
}

static
enum PieceType lookup[0x80] = {
	['p'] = Pawn,
	['n'] = Knight,
	['b'] = Bishop,
	['r'] = Rook,
	['q'] = Queen,
	['k'] = King,
};


static inline PRINTF(2,3)
void log_error(struct Parser *parser, const char *fmt, ...) {
	// silence output on NULL
	if (parser->output == NULL)
		return;

	fprintf(parser->output, RED "error: " RESET);

	va_list args;
	va_start(args, fmt);
	vfprintf(parser->output, fmt, args);
	va_end(args);

	// print context
	fprintf(parser->output, "\n  fen | \"%s\"", parser->fen);
	fprintf(parser->output, "\n      |  ");

	const char *offset = parser->offset;
	while (offset > parser->fen) fputc(' ', parser->output);
	fprintf(parser->output, "^\n\n");
}

static inline
void set_square(struct Position *pos, int sq, enum PieceType T) {
	pos->X |= (bitboard)((T >> 0) & 1) << sq;
	pos->Y |= (bitboard)((T >> 1) & 1) << sq;
	pos->Z |= (bitboard)((T >> 2) & 1) << sq;
}

struct State parse_fen(const char *fen, bool *ok, FILE *stream) {
	struct Parser parser = {
		.fen = fen,
		.offset = fen,
		.output = stream,
	};

	struct Position pos;
	struct State state;

	bitboard white, black, info;
	int sq = 56, file = 0;

	// parse board
	while (sq != 8 && file != 8) {
		unsigned char c = chop_next(&parser);
		unsigned char lower = 0x20;

		if (file > 8) {
			log_error(&parser, "rank description is more than 8 squares");
			goto error;
		}

		// end of rank
		if (file == 8) {
			if (c != '/') {
				log_error(&parser, "expected / after end of rank description");
				goto error;
			}

			file = 0, sq += S+S;
			continue;
		}

		// empty squares
		if ('1' <= c && c <= '8') {
			int offset = c - '0';
			sq += offset, file += offset;
		}

		// piece
		else {
			enum PieceType piece = lookup[c | lower];

			if (piece == None) {
				log_error(&parser, "invalid pieces type; must be one of PNBRQK");
				goto error;
			}

			if (c & lower) black |= 1L << sq;
			else           white |= 1L << sq;

			set_square(&pos, sq, piece);
			sq++, file++;
		}
	}

	if (chop_next(&parser) != ' ') {
		log_error(&parser, "expected space before side-to-move");
		goto error;
	}

	// side-to-move
	switch (chop_next(&parser)) {
		case 'w': state.side_to_move = WHITE; pos.white = white; break;
		case 'b': state.side_to_move = BLACK; pos.white = black; break;

		default:
			log_error(&parser, "expected w or b for side-to-move");
			goto error;
	}

	if (chop_next(&parser) != ' ') {
		log_error(&parser, "expected space before castling rights");
		goto error;
	}

	// castling rights
	while (peek_next(&parser) != ' ') {
		switch (chop_next(&parser)) {
			case 'K': info |= WK_MASK; break;
			case 'Q': info |= WQ_MASK; break;
			case 'k': info |= BK_MASK; break;
			case 'q': info |= BQ_MASK; break;

			default:
				log_error(&parser, "expected one of KQkq for castling rights");
				goto error;
		}

		if (state.side_to_move == BLACK) {
			// swap castling sides
			info = ((info << 2) | (info >> 2)) & CA_MASK;
		}
	}

	if (chop_next(&parser) != ' ') {
		log_error(&parser, "expected space before en-passant square");
		goto error;
	}

	if (peek_next(&parser) == '-') {
		chop_next(&parser);
	}

	else {
		unsigned file = chop_next(&parser) - 'a';

		if (file >= 8) {
			log_error(&parser, "invalid file in en-passant square");
			goto error;
		}

		unsigned rank = chop_next(&parser) - '1';

		if (rank >= 8) {
			log_error(&parser, "invalid rank in en-passant square");
			goto error;
		}

		int valid_rank = (state.side_to_move == WHITE) ? 5 : 2; // 6th or 3rd rank

		if (rank != valid_rank) {
			log_error(&parser, "invalid en-passant square; not on correct rank");
			goto error;
		}

		// set en-passant bit
		info |= 1 << file;
	}

	// half-move clock and full-move number are optional
	if (peek_next(&parser) != '\0') {
		if (chop_next(&parser) != ' ') {
			log_error(&parser, "expected space before half-move clock");
			goto error;
		}
	}

	state.fify_move_clock = chop_int(&parser);

	if (peek_next(&parser) != '\0') {
		if (chop_next(&parser) != ' ') {
			log_error(&parser, "expected space before full-move number");
			goto error;
		}
	}

	state.movenumber = chop_int(&parser);

	// write info bits
	bitboard occ = white | black;
	info = pdep(info, ~occ);

	pos.X |= info;
	pos.Y |= info;
	pos.Z |= info;

	state.pos = pos;

	*ok = true;
	return state;

error:
	*ok = false;
	return state;
}


size_t generate_fen(struct State state, char *buffer) {
	return 0;
}


size_t generate_san(struct Move move, struct State state, char *buffer) {
	return 0;
}
