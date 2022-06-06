#ifndef TEXT_H_
#define TEXT_H_

#include "movegen.h"
#include "state.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// forsyths edwards notation
struct State parse_fen(const char *fen, bool *ok, FILE *stream);
size_t generate_fen(struct State state, char *buffer);

// standard algebraic notation
size_t generate_san(struct Move move, struct State state, char *buffer);

#endif //TEXT_H_
