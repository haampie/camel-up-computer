#include "Game.h"
#include <algorithm>
#include <fstream>
#include <iostream>

Game::Game()
{
	for(int i=0; i<CAMELS_SQUARED; ++i)
	{
		chances.push_back(0.0);
	}

	// Make some new camels and set their order from back to front
	Camel * latestCamel = NULL;
	for(int i=0; i<CAMELS; ++i)
	{
		latestCamel = new Camel(i, CAMELS-1-i, 1, i, false);
		camels.push_back(latestCamel);
		order.insert(order.begin(), latestCamel);
	}

	// Create players
	for(int i=0; i<PLAYERS; ++i)
	{
		players.push_back(new Player);
	}

	// Create piles of cards
	piles.resize(CAMELS);

	for(int i=0; i<CAMELS; ++i)
	{
		piles[i].push_back(new Card(i, 2));
		piles[i].push_back(new Card(i, 3));
		piles[i].push_back(new Card(i, 5));
	}
}

double Game::think()
{
	return move(0, computersTurn, -500.0, 500.0);
}

double Game::move(int depth, bool maximizing, double alpha, double beta)
{
	int player = maximizing ? 0 : 1;
	double moveScore = 0.0, score = maximizing? -500.0 : 500.0;
	
	// Increase the number of nodes visited
	++nodes;

	// return heuristic score
	if(depth == maximumDepth || unmovedCamels == 0)
	{

		int rollingBonus=0, rollingBonusTops=0;
		if(unmovedCamels > 0)
		{
			rollingBonus = unmovedCamels / 2;
			rollingBonusTops = unmovedCamels - rollingBonus;

			// add money for dice rolls
			players[player]->money += rollingBonusTops;
			players[1-player]->money += rollingBonus;
		}

		
		// calculate winning chances
		bool fromMemory = finishRoundByRollingOnly(1);

		// hash them!
		if(!fromMemory)
		{
			hashTable.put(currentState, chances);
		}

		score = getEndScore();
		
		if(unmovedCamels > 0)
		{
			// give back the money!
			players[player]->money -= rollingBonusTops;
			players[1-player]->money -= rollingBonus;
		}

		for(int i=0; i<CAMELS_SQUARED; ++i)
		{
			chances[i] = 0.0;
		}

		return score;
	}

	// Roll the dice!
	if(unmovedCamels > 0)
	{
		for(int i=0; i<CAMELS; ++i)
		{
			if(camels[i]->hasMoved)
				continue;

			for(int j=1; j<4; ++j)
			{
				players[player]->money += 1;
				moveCamel(camels[i], j, true);

				moveScore += move(depth+1, !maximizing, alpha, beta);

				moveCamel(camels[i], -j, false);
				players[player]->money -= 1;
			}
		}

		// calculate definite score as max/min over average
		moveScore = moveScore/((double)(3*unmovedCamels));

		if(depth == 0)
		{
			printf("Rollen geeft score %f\n", moveScore);
			score = getBestScore(moveScore, score, maximizing, 0);
		} else {
			score = getBestScore(moveScore, score, maximizing);
		}


		// alpha beta pruning
		if(maximizing)
			alpha = std::max(alpha, score);
		else
			beta = std::min(beta, score);

		if(beta < alpha)
			return score;
	}

	// Pick a card!
	for(int i=CAMELS-1; i>=0; --i)
	{
		// check if there is a card left
		if(piles[i].size() > 0)
		{
			// pick card from the pile
			playerTakesCard(i, players[player]);

			moveScore = move(depth+1, !maximizing, alpha, beta);
			
			// put card back on the pile;
			playerPutsLastCardBackOnPile(players[player]);

			if(depth == 0)
			{
				score = getBestScore(moveScore, score, maximizing, i+1);
				printf("Kaart %d geeft score %f\n", i, moveScore);
			} else {
				score = getBestScore(moveScore, score, maximizing);
			}


			// alpha beta pruning
			if(maximizing)
				alpha = std::max(alpha, score);
			else
				beta = std::min(beta, score);

			if(beta < alpha)
				return score;
		}
	}

	return score;
}

void Game::moveCamel(Camel * camel, int steps, bool forward)
{
	int originalRelativePosition = camel->order;
	int from = camel->position;
	int to = camel->position + steps;
	int i, camelsOnTop=0, jumpedOver=0;
	Camel * pointer;

	// Do or undo move
	camel->hasMoved = forward;

	// Make move in state
	currentState ^= hashTable.zobristHasMoved[camel->index];
	
	if(forward)
		--unmovedCamels;
	else
		++unmovedCamels;

	// don't do anything when nothing is asked
	if(steps == 0)
		return;

	// [1] Find a new camel to jump on;
	Camel * jumpOn = NULL;

	for(i=0; i < CAMELS; ++i)
	{
		if(camels[i]->position == to && camels[i]->above == NULL)
		{
			jumpOn = camels[i];
			break;
		}
	}


	// [2] Move all camels with the one on top
	pointer = camel;
	while(pointer != NULL) {
		++camelsOnTop;
		pointer = pointer->above;
	}

	// [3] Update the orders of the Camel objects
	// Check how many places in the order the stack of camels obtains by the move
	if(steps > 0)
	{
		for(i=originalRelativePosition-1; i >= 0; --i)
		{
			if(order[i]->position > to)
				break;

			if(order[i]->position > from)
			{
				order[i]->order += camelsOnTop;
				--jumpedOver;
			}
		}
	} else 
	{
		for(i=originalRelativePosition+1; i < CAMELS; ++i)
		{
			if(order[i]->position <= to)
				break;

			if(order[i]->position <= from)
			{
				order[i]->order -= camelsOnTop;
				++jumpedOver;
			}
		}
	}

	// Set new positions + heights
	pointer = camel;
	int startingHeight=0;
	int counter=0;
	
	if(jumpOn != NULL)
		startingHeight = jumpOn->height+1;

	while(pointer != NULL) {
		// Undo state
		currentState ^= hashTable.zobristPiecesOnPositions[pointer->index][from][pointer->height];
		
		// Change order, position and height
		pointer->order += jumpedOver;
		pointer->position = to;
		pointer->height = startingHeight + counter;
		
		// Do state
		currentState ^= hashTable.zobristPiecesOnPositions[pointer->index][to][pointer->height];
		
		// Go to the camel on top
		pointer = pointer->above;
		++counter;
	}

	// Reset the camel index
	for(i=0; i < CAMELS; ++i)
	{
		order[camels[i]->order] = camels[i];
	}

	// [4] Set connections connections
	if( camel->below != NULL )
	{
		camel->below->above = NULL;
	}

	camel->below = jumpOn;

	if( jumpOn != NULL)
	{
		jumpOn->above = camel;
	}
}

int Game::getNumberOfNodesSearched()
{
	return nodes;
}

double Game::calculateCardScore(Card * card)
{
	return (double) card->moneyForWinningCamel * chances[card->camel*CAMELS]
		+ chances[card->camel*CAMELS+1]
		- chances[card->camel*CAMELS+2]
		- chances[card->camel*CAMELS+3]
		- chances[card->camel*CAMELS+4];
}

double Game::getEndScore()
{
	double score = 0.0;

	score = (double) (players[0]->money - players[1]->money);

	for(std::vector<Card*>::iterator card = players[0]->cards.begin() ; card != players[0]->cards.end(); ++card)
	{
		score += calculateCardScore(*card);
	}

	for(std::vector<Card*>::iterator card = players[1]->cards.begin() ; card != players[1]->cards.end(); ++card)
	{
		score -= calculateCardScore(*card);
	}

	return score;
}

void Game::updateFinishChances(int chance)
{
	int i=0;
	double winningChance = 1.0/(double)chance;


	// then loop over the camels and add the chances
	for(i=0; i<CAMELS; ++i)
	{
		chances[CAMELS*order[i]->index + i] += winningChance;
	}
}

void Game::updateFinishChancesFromMemory(int chance, HashedPosition * state)
{
	for(int i=0; i<CAMELS_SQUARED; ++i)
		chances[i] += state->chances[i]/((double) chance);
}

bool Game::finishRoundByRollingOnly(int chance)
{
	int i, j, newChance;

	++nodes;

	// Check if this result is cached!
	HashedPosition * state = hashTable.get(currentState);
	if(state != NULL)
	{
		updateFinishChancesFromMemory(chance, state);
		return true;
	}

	// Check if there is still a move to do
	if(unmovedCamels > 0)
	{
		// Update inversed chance, since we haven't have a winner yet
		newChance = 3*unmovedCamels*chance;

		// Loop over all camels
		for(i=0; i < CAMELS; ++i)
		{
			// Skip moved camels
			if(camels[i]->hasMoved)
				continue;

			// Loop over possible dice rolls
			for(j=1; j < 4; ++j)
			{
				
				// Move player ...
				moveCamel(camels[i], j, true);

				// Check if player has finished the race
				if(camels[i]->position >= LENGTH_TRACK)
				{
					// Increase winning chances
					updateFinishChances(newChance);
				} else {

					// Check next turn
					finishRoundByRollingOnly(newChance);
				}

				// Put player back where he was
				moveCamel(camels[i], -j, false);
			}
		}

		return false;
	}

	// If there is no move to do, find the winner and increase his chances!
	updateFinishChances(chance);
	return false;
}

Card * Game::playerTakesCard(int pile, Player * player)
{
	Card * currentCard = piles[pile].back();
	player->cards.push_back(currentCard);
	piles[pile].pop_back();

	return currentCard;
}

void Game::playerPutsLastCardBackOnPile(Player * player)
{
	Card * card = player->cards.back();
	piles[card->camel].push_back(card);
	player->cards.pop_back();
}

double Game::getBestScore(double newScore, double oldScore, bool maximizing)
{
	if(maximizing)
	{
		return std::max(newScore, oldScore);
	}

	return std::min(newScore, oldScore);
}

double Game::getBestScore(double newScore, double oldScore, bool maximizing, int moveNumber)
{
	if(maximizing)
	{
		if(newScore > oldScore)
		{
			bestMoveNumber = moveNumber;
			return newScore;
		}
		
		return oldScore;
	}
	
	if(newScore < oldScore)
	{
		bestMoveNumber = moveNumber;
		return newScore;
	}

	return oldScore;
}

void Game::setDepth(int depth)
{
	maximumDepth = depth;
}

void Game::setUpFromFile()
{
	std::ifstream input;
	int i, j, pos, numCardsTaken, pile, player;
	char moved;

	input.open("setup.txt");

	// who's turn is it?
	input >> i;
	if(i==0)
		computersTurn = true;
	else
		computersTurn = false;

	// get money
	input >> i;
	input >> j;

	players[0]->money = i;
	players[1]->money = j;

	// Camel positions
	for(i=0; i < CAMELS; ++i)
	{
		input >> pos;
		input >> moved;

		// start counting positions from 0 instead of 1;
		pos = pos - 1;
		
		camels[i]->position = pos;
		camels[i]->height = 0;

		if(moved == 'y')
		{
			camels[i]->hasMoved = true;
			--unmovedCamels;
		}

		// Stack camels!
		for(j=0; j < i; ++j)
		{
			if(camels[j]->position == pos && camels[j]->above == NULL)
			{
				camels[j]->above = camels[i];
				camels[i]->below = camels[j];
				camels[i]->height = 1 + camels[j]->height;
			}
		}
	}

	input >> numCardsTaken;

	for(i=0; i<numCardsTaken; ++i)
	{
		input >> player;
		input >> pile;

		playerTakesCard(pile, players[player]);
	}

	currentState = hashTable.mapPosition(camels);

	input.close();
}

Game::~Game()
{
	for(int i=0; i<CAMELS; ++i)
	{
		if(camels[i] != NULL)
			delete camels[i];

		for(int j=0; j<3; ++j)
			if(piles[i][j] != NULL)
				piles[i][j];
	}

	for(int i=0; i<2; ++i)
	{
		if(players[i] != NULL)
			delete players[i];
	}
}

// Some debugging information

void Game::outputPiles()
{
	int i=0;
	uint j=0;

	for(i=0; i<CAMELS; ++i)
	{
		for(j=0; j<piles[i].size(); ++j)
		{
			printf("%d ", piles[i][j]->moneyForWinningCamel);
		}
		printf("\n");
	}
}

void Game::outputCamelDetails()
{
	for(int i=0; i<CAMELS; ++i)
	{
		printf("Camel #%d:\n", i);
		printf("Position & height: (%d, %d)\n", camels[i]->position, camels[i]->height);
		printf("Order: %d\n", camels[i]->order);
	}

	printf("Camels according to the order index: ");
	
	for(int i=0; i<CAMELS; ++i)
	{
		printf("%d, ", order[i]->index);
	}
	printf("\n----\n");
}

void Game::outputChances()
{
	int i=0, j=0;

	printf("%2s | ","#");
	for(i=0; i<CAMELS; ++i)
	{
		printf("%-5d", i+1);
	}
	printf("\n");


	for(i=0; i<CAMELS; ++i)
	{
		printf("%2d | ", i+1);

		for(j=0; j<CAMELS; ++j)
		{
			printf("%-1.2f ", chances[i*CAMELS+j]);
		}

		printf("\n");
	}

	printf("\n");
}

void Game::outputBestMove(int depth, double score)
{
	std::string dots (depth, '-');
	printf("%s %d %2.2f\n", dots.c_str(), bestMoveNumber, score);
}

void Game::outputCurrentState()
{
	printf("Locally: %llu\n", currentState);
	printf("From class: %llu\n", hashTable.mapPosition(camels));
}

void Game::outputHashResults()
{
	printf("(Hits, misses, collisions): (%d, %d, %d)\n", hashTable.getHits(), hashTable.getMisses(), hashTable.getCollisions());
}

void Game::listBestGameContinuation()
{
	if(bestMoveNumber == 0)
	{
		printf("Rol de dobbelstenen\n");
		return;
	}

	printf("Pak een kaart van stapel %d\n", bestMoveNumber-1);
}