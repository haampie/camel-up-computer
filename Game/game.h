#ifndef GAME
#define GAME

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "../Camels/camel.h"
#include "../Cards/card.h"
#include "../Players/player.h"

class Game {
	private:
		int CAMELS;
		int LENGTH_TRACK;
		int maximumDepth;
		int unmovedCamels;
		int nodes;
		int bestMoveNumber;
		bool turnOfComputer;
		float scores[25];

		std::vector<Camel*> camels;
		std::vector<Player*> players;
		std::vector< std::vector<Card*> > piles;
		std::vector<int> order;
		
		void initialize(int nCamels);
		float getBestScore(float newScore, float oldScore, bool maximizing, int moveNumber);
		float getBestScore(float newScore, float oldScore, bool maximizing);
		void moveCamel(Camel * camel, int steps);
		void calculateOrder();
		float getEndScore();
		float calculateCardScore(Card * card);
		void finishRoundByRollingOnly(int chance);
		void updateFinishChances(int chance);
		float move(int depth, bool maximizing);
		Card * playerTakesCard(int pile, Player * player);
		void playerPutsLastCardBackOnPile(Player * player);

	public:
		Game();
		Game(int nCamels);
		int getNumberOfNodesSearched();
		void outputPiles();
		void listBestGameContinuation();
		void setDepth(int depth);
		void setUpFromFile();
		void outputScores();
		void outputBestMove(int depth, float score);
		float think();
};

bool sortCamels(const CamelPair& left, const CamelPair& right);

#endif