#include "main.h"

int main(int argc, char **argv)
{
	int depth = 3;

	if(argc > 1)
	{
		depth = atoi(argv[1]);
	}

	// Create a standard game
	Game game;

	game.setUpFromFile();
	game.setDepth(depth);

	printf("\n");
	printf("Expected score at depth %d: %2.2f\n", depth, game.think());
	printf("Number of nodes: %d\n\n", game.getNumberOfNodesSearched());

	game.outputHashResults();
	game.listBestGameContinuation();

    return 0;
}