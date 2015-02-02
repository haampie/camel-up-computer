#ifndef HASHTABLE
#define HASHTABLE

#include <vector>
#include "HashedPosition.h"
#include "../Camels/Camel.h"
#include "../Utilities/definitions.h"

const int TABLE_SIZE = 2000003;
 
class HashTable {
	private:
    	HashedPosition **table;
    	unsigned int hits = 0;
    	unsigned int misses = 0;
    	unsigned int collisions = 0;
	public:
		HashTable();
        U64 zobristPiecesOnPositions[5][16][5]; // #camel #position #height
        U64 zobristHasMoved[5]; // #camel
		U64 mapPosition(std::vector<Camel*> &camels);
		HashedPosition * get(U64 key);
		void put(U64 key, std::vector<double> &scores);
		unsigned int getHits();
		unsigned int getMisses();
		unsigned int getCollisions();
		~HashTable();
};

#endif