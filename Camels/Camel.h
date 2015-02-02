#ifndef CAMEL
#define CAMEL

#include <stddef.h>

typedef struct Camel {
    int index;
    int order;
    int position;
    int height;
    bool hasMoved;
    Camel *above = NULL;
    Camel *below = NULL;

    Camel() : position(1), hasMoved(false) {}
    Camel(int i, int o, int p, int h, bool m);
} Camel;

#endif