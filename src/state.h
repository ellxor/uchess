#ifndef STATE_H_
#define STATE_H_

#include "position.h"

enum Color { WHITE, BLACK };

struct State {
	struct Position pos;
	enum Color side_to_move;

	unsigned fify_move_clock;
	unsigned movenumber;
};

#endif //STATE_H_
