#ifndef CARD
#define CARD

#include <iostream>

typedef struct Card {
    int camel;
    int moneyForWinningCamel;

    Card(int x, int y);    
} Card;

#endif