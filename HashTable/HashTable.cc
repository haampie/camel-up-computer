#include "HashTable.h"
#include "../Utilities/RandomGenerator.h"
#include <iostream>

HashTable::HashTable()
{
    RandomGenerator keys;

    // Create a table
    table = new HashedPosition*[TABLE_SIZE];

    for(int i = 0; i < TABLE_SIZE; ++i)
        table[i] = NULL;

    // Initialize the random values
    for(int i=0; i<5; ++i)
        for(int j=0; j<16; ++j)
            for(int k=0; k<5; ++k)
                zobristPiecesOnPositions[i][j][k] = keys.random64();

    for(int i=0; i<5; ++i)
        zobristHasMoved[i] = keys.random64();
}

// Calculate a zorbist key of a given position (only to initialize!)
U64 HashTable::mapPosition(std::vector<Camel*> &camels)
{
    U64 key=0;
    int n = camels.size();

    for(int i=0; i<n; ++i)
    {
        key ^= zobristPiecesOnPositions[i][camels[i]->position][camels[i]->height];
        
        if(camels[i]->hasMoved)
            key ^= zobristHasMoved[i];
    }
    
    return key;
}

HashedPosition * HashTable::get(U64 zkey) {
    int hash = (zkey % TABLE_SIZE);

    HashedPosition * state = table[hash];

    if(state != NULL && table[hash]->zkey == zkey)
    {
        ++hits;
        return state;
    }

    ++misses;

    return NULL;
}

void HashTable::put(U64 key, std::vector<double> &scores) {
    int hash = (key % TABLE_SIZE);

    // collision
    if(table[hash] != NULL)
    {
        // printf("%llu\n", key);
        ++collisions;
        delete table[hash];
    }

    table[hash] = new HashedPosition(key, scores);
}

unsigned int HashTable::getHits()
{
    return hits;
}

unsigned int HashTable::getMisses()
{
    return misses;
}

unsigned int HashTable::getCollisions()
{
    return collisions;
}

HashTable::~HashTable() {
    for (int i = 0; i < TABLE_SIZE; ++i)
        if (table[i] != NULL)
            delete table[i];

    delete[] table;
}