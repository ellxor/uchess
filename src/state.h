#ifndef STATE_H_
#define STATE_H_

#include "position.h"

enum Color { WHITE, BLACK };

struct State {
	struct Position pos;
	enum Color side_to_move;

	int fify_move_clock;
	int movenumber;
};

#endif //STATE_H_
