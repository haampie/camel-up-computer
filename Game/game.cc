#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>
#include "game.h"
#include "../Camels/camel.h"
#include "../Cards/card.h"
#include "../Players/player.h"

Game::Game(int nCamels)
{
	initialize(nCamels);
}

Game::Game()
{
	initialize(5);
}

void Game::initialize(int nCamels)
{
	int i=0, j=0, prev=1, curr=2, tmp;

	nodes = 0;
	CAMELS = nCamels;
	LENGTH_TRACK = 16;
	unmovedCamels = nCamels;
	maximumDepth = 3;

	for(i=0; i<nCamels*nCamels; ++i)
	{
		scores[i] = 0.0;
	}

	for(i=0; i<nCamels; ++i)
	{
		camels.push_back(new Camel);
	}

	for(i=0; i<2; ++i)
	{
		players.push_back(new Player);

	}

	order.resize(nCamels);
	piles.resize(nCamels);

	for(i=0; i<nCamels; ++i)
	{
		prev = 1;
		curr = 2;
		for(j=0; j<3; ++j)
		{
			piles[i].push_back(new Card(i, curr));

			// fibonacci recurrence
			tmp = curr;
			curr += prev;
			prev = tmp;
		}
	}
}

int Game::getNumberOfNodesSearched()
{
	return nodes;
}

float Game::calculateCardScore(Card * card)
{
	return (float) card->moneyForWinningCamel * scores[card->camel*CAMELS]
		+ scores[card->camel*CAMELS+1]
		- scores[card->camel*CAMELS+2]
		- scores[card->camel*CAMELS+3]
		- scores[card->camel*CAMELS+4];
}

float Game::getEndScore()
{
	float score = 0.0;

	calculateOrder();

	score = (float) (players[0]->money - players[1]->money);

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
	float winningChance = 1.0/(float)chance;

	// first calculate the win order.
	calculateOrder();

	// then loop over the camels and add the chances
	for(i=0; i<CAMELS; ++i)
	{
		scores[CAMELS*i + order[i]] += winningChance;
	}
}

void Game::finishRoundByRollingOnly(int chance)
{
	int i, j, newChance;

	++nodes;

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
				moveCamel(camels[i], j);
				camels[i]->hasMoved = true;
				--unmovedCamels;

				// Check if player has finished the race
				if(camels[i]->position > LENGTH_TRACK)
				{
					// Increase winning chances
					updateFinishChances(newChance);
				} else {

					// Check next turn
					finishRoundByRollingOnly(newChance);
				}

				// Put player back where he was
				moveCamel(camels[i], -j);
				camels[i]->hasMoved = false;
				++unmovedCamels;
			}
		}

		return;
	}

	// If there is no move to do, find the winner and increase his chances!
	updateFinishChances(chance);
}

float Game::think()
{
	return move(0, turnOfComputer);
}

float Game::move(int depth, bool maximizing)
{
	int i, j, current = maximizing ? 0 : 1;
	float score = 0.0, definiteScore = maximizing? -500.0 : 500.0;
	
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
			players[maximizing ? 0 : 1]->money += rollingBonusTops;
			players[maximizing ? 1 : 0]->money += rollingBonus;
		}

		
		// calculate winning chances
		finishRoundByRollingOnly(1);
		definiteScore = getEndScore();
		
		if(unmovedCamels > 0)
		{
			// give back the money!
			players[maximizing ? 0 : 1]->money -= rollingBonusTops;
			players[maximizing ? 1 : 0]->money -= rollingBonus;
		}

		std::string dots (depth, '-');
		// printf("%s score = %2.2f\n", dots.c_str(), definiteScore);

		for(i=0; i<CAMELS*CAMELS; ++i)
		{
			scores[i] = 0.0;
		}

		return definiteScore;
	}

	// Roll the dice!
	if(unmovedCamels > 0)
	{
		for(i=0; i<CAMELS; ++i)
		{
			if(camels[i]->hasMoved)
				continue;

			for(j=1; j<4; ++j)
			{
				players[current]->money += 1;
				camels[i]->hasMoved = true;
				moveCamel(camels[i], j);
				--unmovedCamels;

				score += move(depth+1, !maximizing);

				++unmovedCamels;
				moveCamel(camels[i], -j);
				camels[i]->hasMoved = false;
				players[current]->money -= 1;
			}
		}

		// calculate definite score as max/min over average
		score = score/((float)(3*unmovedCamels));

		if(depth == 0)
		{
			definiteScore = getBestScore(score, definiteScore, maximizing, 0);

			printf("Rollen geeft score %f, maximizing: %s\n", score, maximizing ? "ja" : "nee");
		} else 
		{
			definiteScore = getBestScore(score, definiteScore, maximizing, 0);
		}
	}

	// Pick a card!
	for(i=0; i<CAMELS; ++i)
	{
		// check if there is a card left
		if(piles[i].size() > 0)
		{
			// pick card from the pile
			playerTakesCard(i, players[current]);

			score = move(depth+1, !maximizing);

			if(depth == 0)
			{
				definiteScore = getBestScore(score, definiteScore, maximizing, i+1);

				printf("Kaart %d geeft score %f\n", i, score);
			} else 
			{
				definiteScore = getBestScore(score, definiteScore, maximizing, i+1);
			}

			// put card back on the pile;
			playerPutsLastCardBackOnPile(players[current]);
		}
	}

	// outputBestMove(depth, definiteScore);

	return definiteScore;
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

float Game::getBestScore(float newScore, float oldScore, bool maximizing)
{
	if(maximizing)
	{
		return std::max(newScore, oldScore);
	}

	return std::min(newScore, oldScore);
}

float Game::getBestScore(float newScore, float oldScore, bool maximizing, int moveNumber)
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

void Game::moveCamel(Camel * camel, int steps)
{
	int i, position = camel->position + steps;

	// don't do anything when nothing is asked
	if(camel->position == position)
		return;


	// [1] Find a new camel to jump on;
	Camel * jumpOn = NULL;

	for(i=0; i < CAMELS; ++i)
	{
		if(camels[i]->position == position && camels[i]->above == NULL)
		{
			jumpOn = camels[i];
			break;
		}
	}


	// [2] Move all camels with the one on top
	Camel * pointer = camel;

	while(pointer != NULL) {
		pointer->position = position;
		pointer = pointer->above;
	}

	// [3] Set connections
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

bool sortCamels(const CamelPair& left, const CamelPair& right)
{
	Camel * above = left.second->above;

	if(left.second->position == right.second->position)
	{
		while(above != NULL)
		{
			if(above == right.second)
				return false;

			above = above->above;
		}

		return true;
	}

	return left.second->position > right.second->position;
}

void Game::calculateOrder()
{
	int i=1;
	std::pair <int, Camel*> indexed_camel;
	std::vector<CamelPair> indexed_camels;


	// first index the camels by array index
	for(i=0; i<CAMELS; ++i)
	{
		indexed_camels.push_back(std::make_pair(i, camels[i]));
	}

	// sort them
	std::sort(indexed_camels.begin(), indexed_camels.end(), sortCamels);

	for(i=0; i<CAMELS; ++i)
	{
		order[indexed_camels[i].first] = i;
	}
}

void Game::setDepth(int depth)
{
	maximumDepth = depth;
}

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

void Game::outputScores()
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
			printf("%-1.2f ", scores[i*CAMELS+j]);
		}

		printf("\n");
	}

	printf("\n");
}

void Game::outputBestMove(int depth, float score)
{
	std::string dots (depth, '-');
	printf("%s %d %2.2f\n", dots.c_str(), bestMoveNumber, score);
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

void Game::setUpFromFile()
{
	std::ifstream input;
	int i, j, pos, numCardsTaken, pile, player;
	char moved;

	input.open("setup.txt");

	// who's turn is it?
	input >> i;
	if(i==0)
		turnOfComputer = true;
	else
		turnOfComputer = false;

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
		
		camels[i]->position = pos;

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

	// for(i=0; i<2; ++i)
	// {
	// 	printf("Cards of player %d: ", i);
	// 	for(k=0; k<players[i]->cards.size(); ++k)
	// 	{
	// 		printf("(%d, %d)", players[i]->cards.at(k)->camel, players[i]->cards.at(k)->moneyForWinningCamel);
	// 	}
	// }

	// printf("\n");

	input.close();
}