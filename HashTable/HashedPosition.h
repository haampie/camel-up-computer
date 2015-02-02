#ifndef HASHEDPOSITION
#define HASHEDPOSITION

#include <vector>
#include "../Utilities/definitions.h"

class HashedPosition
{
    public:
        U64 zkey;
        std::vector<double> chances;
        HashedPosition(U64 zkey, std::vector<double> &s);
};

#endif