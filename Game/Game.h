#ifndef GAME
#define GAME

#include <vector>
#include "../Utilities/definitions.h"
#include "../Camels/Camel.h"
#include "../Cards/Card.h"
#include "../HashTable/HashedPosition.h"
#include "../HashTable/HashTable.h"
#include "../Players/Player.h"

const int PLAYERS = 2;
const int CAMELS = 5;
const int CARDS_ON_TILE = 3;
const int LENGTH_TRACK = 16;
const int CAMELS_SQUARED = CAMELS*CAMELS;

class Game {
	private:
		// Game position description
		std::vector<Player*> players;
		std::vector< std::vector<Card*> > piles;
		std::vector<Camel*> order;
		std::vector<double> chances;
		std::vector<Camel*> camels;
		
		// Hasing data
		HashTable hashTable;
		U64 currentState = 0;
		
		// Some defaults
		int maximumDepth = 3;
		int unmovedCamels = CAMELS;
		int nodes = 0;
		int bestMoveNumber = 0;
		bool computersTurn = true;

		void initialize(int nCamels);
		double getBestScore(double newScore, double oldScore, bool maximizing, int moveNumber);
		double getBestScore(double newScore, double oldScore, bool maximizing);
		void moveCamel(Camel * camel, int steps, bool forward);
		double getEndScore();
		double calculateCardScore(Card * card);
		bool finishRoundByRollingOnly(int chance);
		void updateFinishChances(int chance);
		void updateFinishChancesFromMemory(int chance, HashedPosition * state);
		double move(int depth, bool maximizing, double alpha, double beta);
		Card * playerTakesCard(int pile, Player * player);
		void playerPutsLastCardBackOnPile(Player * player);

	public:
		Game();
		~Game();
		void setUpFromFile();
		void setDepth(int depth);
		double think();
		int getNumberOfNodesSearched();
		
		// debugging information
		void listBestGameContinuation();
		void outputBestMove(int depth, double score);
		void outputCamelDetails();
		void outputChances();
		void outputCurrentState();
		void outputHashResults();
		void outputPiles();
};

#endif