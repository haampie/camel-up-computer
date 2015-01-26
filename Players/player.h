#ifndef PLAYER
#define PLAYER

#include <iostream>
#include <vector>
#include "../Cards/card.h"

typedef struct Player {
	int money;
	std::vector<Card*> cards;
} Player;

#endif