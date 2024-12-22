#include <stdio.h>
#include <stdint.h>

#define NUMKEYS 11
#define DIRKEYS 5

typedef enum dir {
    ACT, RIGHT, DOWN, LEFT, UP
} Dir;

typedef struct pos {
    uint8_t x, y;
} Pos;

static const char *numpad[] = {
    "789",
    "456",
    "123",
    "_0A"};
static Pos numpos[11] = {  // order: A,0..9
    {2,3},{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0}};

static const char *dirpad[] = {
    "_^A",
    "<v>"};
static Pos dirpos[5] = {  // order: A,>,V,<,^ (same as 'enum dir')
    {2,0},{2,1},{1,1},{0,1},{1,0}};

static char movenum[NUMKEYS][NUMKEYS][6];  // max 5 moves on numpad
static char movedir[DIRKEYS][DIRKEYS][4];  // max 3 moves on dirpad

static Pos sub(const Pos a, const Pos b)
{
    return (Pos){a.x - b.x, a.y - b.y};
}

int main(void)
{
    // Define best moves on numpad
    for (int i = 0; i < NUMKEYS; ++i)
        for (int j = 0; j < NUMKEYS; ++j)
            if (i != j)
                ;

    // Define best moves on dirpad
    for (int i = 0; i < DIRKEYS; ++i)
        for (int j = 0; j < DIRKEYS; ++j)
            if (i != j)
                ;

    return 0;
}
