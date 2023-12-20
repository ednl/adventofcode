#include <stdio.h>

#define NAME "../aocinput/2023-20-example.txt"
#define MODULES 58
#define BUSWIDTH 6

typedef enum signal {
    LO, HI, NOP
} Signal;

typedef enum type {
    NONE, BC, FLIP, CONJ
} Type;

typedef struct bus {
    int id[BUSWIDTH];
    Signal val[BUSWIDTH];
}

// not right yet, maybe split flip/conj because multi input & mem for conj
typedef struct module {
    int id;
    Type type;
    Signal inp;
    int out[OUTSIZE];
    int outlen;
    int state;
} Module;

static Module module[MODULES];

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[32];
    int n = 0;
    while (n < MODULES && fgets(buf, sizeof buf, f)) {
        switch (buf[0]) {
            case 'b': module[n].type = BC; break;
            case '%': module[n].type = FLIP; break;
            case '&': module[n].type = CONJ; break;
        }
        ++n;
    }
    fclose(f);
    return 0;
}
