#include <stdio.h>
#include <stdlib.h>    // atoi
#include <stdint.h>    // int64_t, INT64_C
#include <inttypes.h>  // PRId64

#define LINES  100
#define LINELEN 32

#define DECK  INT64_C(119315717514047)
#define TIMES INT64_C(101741582076661)
#define START INT64_C(2020)

typedef enum action {
    SHIFTUP, SHIFTDN, REVERSE, DISTRIB
} Action;

typedef struct shuffle {
    Action key;
    int val;
} Shuffle;

static Shuffle shuffle[LINES];

int main(void)
{
    FILE *f = fopen("../aocinput/2019-22-input.txt", "r");
    if (!f)
        return 1;

    char s[LINELEN];
    for (int i = 0, n; i < LINES && fgets(s, sizeof s, f); ++i)
        switch (s[5]) {
        case 'i':  // "deal into new stack"
            shuffle[i] = (Shuffle){REVERSE, 0};
            break;
        case 'w':  // "deal with increment X"
            shuffle[i] = (Shuffle){DISTRIB, atoi(&s[20])};
            break;
        default:  // "cut X" where s[5]='\n' or a digit
			n = atoi(&s[4]);
			shuffle[i] = n > 0 ? (Shuffle){SHIFTUP, n} : (Shuffle){SHIFTDN, -n};
            break;
        }
    fclose(f);

    int64_t index = START, count = 0;
    do {
        for (int i = 0; i < LINES; ++i) {
            switch (shuffle[i].key) {
                case SHIFTUP: index -= shuffle[i].val; if (index < 0) index += DECK; break;
                case SHIFTDN: index += shuffle[i].val; if (index >= DECK) index -= DECK; break;
                case REVERSE: index = DECK - 1 - index; break;
                case DISTRIB: index = index * shuffle[i].val % DECK; break;
            }
        }
        ++count;
    } while (index != START && count < TIMES);

    printf("%"PRId64" %"PRId64"\n", count, index);
    return 0;
}
