#include <iostream>
#include <cstdio>
#include <vector>
#include "Game/game.h"
#include "Players/player.h"
#include "Camels/camel.h"
#include "Cards/card.h"

int main(int argc, char **argv)
{
	int depth = 3;

	if(argc > 1)
	{
		depth = atoi(argv[1]);
	}

	// Create a standard game
	Game game;

	// TODO: set initial positions

	game.setUpFromFile();
	game.setDepth(depth);
	printf("Expected score at depth %d: %2.2f\n", depth, game.think());
	printf("Number of nodes: %d\n\n", game.getNumberOfNodesSearched());

	game.listBestGameContinuation();

    return 0;
}