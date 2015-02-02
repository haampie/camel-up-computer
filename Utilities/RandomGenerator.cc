#include "./RandomGenerator.h"
#include <ctime>
#include <stdlib.h>

RandomGenerator::RandomGenerator()
{
   time_t now;
   srand((unsigned int)time(&now));
}

U64 RandomGenerator::random64()
{
   return rand()^((U64)rand()<<15)^((U64)rand()<<30)^((U64)rand()<<45)^((U64)rand()<<60);
}