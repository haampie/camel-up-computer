#ifndef CAMEL
#define CAMEL

#include <iostream>

typedef struct Camel {
    int position;
    bool hasMoved;
    Camel *above = NULL;
    Camel *below = NULL;

    Camel() : position(1), hasMoved(false) {}
    Camel(int p, bool m);
} Camel;

typedef std::pair <int, Camel*> CamelPair;

#endif