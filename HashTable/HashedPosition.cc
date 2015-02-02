#include "HashedPosition.h"

HashedPosition::HashedPosition(U64 zkey, std::vector<double> &s)
{
	this->zkey = zkey;
	chances = s;
}