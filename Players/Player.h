#ifndef PLAYER
#define PLAYER

#include <vector>
#include "../Cards/Card.h"

typedef struct Player {
	int money;
	std::vector<Card*> cards;
} Player;

#endif