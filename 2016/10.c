#include <stdio.h>
#include <stdbool.h>
#include <string.h>   // memcpy

#define BOTS 256
#define BINS  32
#define CMP0  17
#define CMP1  61

typedef struct {
    int id;
    int chips;    // number of chips holding
    int chip[2];  // chip values
    int give[2];  // index of next bot or output bin
    bool (*func[2])(const int, const int);  // function pointer to give chips to bot or output bin
} Bot;

static Bot bot[BOTS];
static int bin[BINS];
static int bots, bins;
static int cap[BOTS], next[BOTS];
static int capcount, nextcount;

static bool tobot(const int i, const int val)
{
    if (i >= bots || bot[i].chips > 1)
        return false;
    Bot * const b = bot + i;
    b->chip[b->chips++] = val;
    if (b->chips == 2) {
        if (b->chip[0] > b->chip[1]) {
            int t = b->chip[0];
            b->chip[0] = b->chip[1];
            b->chip[1] = t;
        }
        next[nextcount++] = i;
    }
    return true;
}

static bool tobin(const int i, const int val)
{
    if (i >= bins || bin[i])
        return false;
    bin[i] = val;
    return true;
}

static bool parse(void)
{
    FILE *f = fopen("../aocinput/2016-10-input.txt", "r");
    if (!f)
        return false;
    char buf[64];
    int x, y, z;
    while (fgets(buf, sizeof buf, f)) {
        if (buf[0] == 'v') {
            if (sscanf(buf, "value %d goes to bot %d", &x, &y) == 2) {
                if (y >= bots) bots = y + 1;
                tobot(y, x);
            }
        } else {
            if (sscanf(buf, "bot %d gives low to bot %d and high to bot %d", &x, &y, &z) == 3) {
                if (y >= bots) bots = y + 1;
                if (z >= bots) bots = z + 1;
                bot[x].func[0] = tobot;
                bot[x].func[1] = tobot;
            } else if (sscanf(buf, "bot %d gives low to output %d and high to bot %d", &x, &y, &z) == 3) {
                if (y >= bins) bins = y + 1;
                if (z >= bots) bots = z + 1;
                bot[x].func[0] = tobin;
                bot[x].func[1] = tobot;
            } else if (sscanf(buf, "bot %d gives low to bot %d and high to output %d", &x, &y, &z) == 3) {
                if (y >= bots) bots = y + 1;
                if (z >= bins) bins = z + 1;
                bot[x].func[0] = tobot;
                bot[x].func[1] = tobin;
            } else if (sscanf(buf, "bot %d gives low to output %d and high to output %d", &x, &y, &z) == 3) {
                if (y >= bins) bins = y + 1;
                if (z >= bins) bins = z + 1;
                bot[x].func[0] = tobin;
                bot[x].func[1] = tobin;
            }
            if (x >= bots) bots = x + 1;
            bot[x].id = x;
            bot[x].give[0] = y;
            bot[x].give[1] = z;
        }
    }
    fclose(f);
    return true;
}

static int run(void)
{
    int comparebot = -1;
    while (nextcount) {
        // printf("%d\n", nextcount);
        memcpy(cap, next, (size_t)nextcount * sizeof *next);
        capcount = nextcount;
        nextcount = 0;

        for (int i = 0; i < capcount; ++i) {
            Bot * const b = bot + cap[i];
            if (comparebot == -1 && b->chip[0] == CMP0 && b->chip[1] == CMP1)
                comparebot = b->id;
            for (int j = 0; j < 2; ++j)
                if (!(*(b->func[j]))(b->give[j], b->chip[j]))
                    putchar('!');
            b->chips = 0;
        }
    }
    return comparebot;
}

int main(void)
{
    if (!parse())
        return 1;
    printf("Part 1: %d\n", run());  // 147
    printf("Part 2: %d\n", bin[0] * bin[1] * bin[2]);  // 55637
    return 0;
}
