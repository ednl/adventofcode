#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXAMPLE 0
#if EXAMPLE == 1
#define INPUT "../aocinput/2016-21-example.txt"
#define N 8
#define PWD "abcde"
#else
#define INPUT "../aocinput/2016-21-input.txt"
#define N 100
#define PWD "abcdefgh"
#define PW2 "fbgdceah"
#endif

typedef enum {
    NOP, MOVU, MOVD, SWAPI, SWAPC, REV, ROTC, ROTR
} opcode_t;

typedef struct {
    opcode_t op;
    int x, y;
} cmd_t;

static cmd_t algo[N];

static char pwd[] = PWD;
static const int len = (int)(sizeof pwd) - 1;

static int parse(void)
{
    FILE *f = fopen(INPUT, "r");
    if (!f)
        return 0;
    int n = 0;
    char *line = NULL;
    size_t size;
    ssize_t linelen;
    while (n < N && (linelen = getline(&line, &size, f)) > 0) {
        opcode_t op = NOP;
        int x = 0, y = 0;
        if (line[0] == 'm') {         // move position X to position Y
            x = line[14] - '0';
            y = line[28] - '0';
            op = x < y ? MOVU : MOVD;
        } else if (line[0] == 's') {  // swap ...
            if (line[5] == 'p') {     // swap position X with position Y
                op = SWAPI;
                x = line[14] - '0';
                y = line[30] - '0';
            } else {                  // swap letter X with letter Y
                op = SWAPC;
                x = line[12];
                y = line[26];
            }
        } else if (line[1] == 'e') {  // reverse positions X through Y
            op = REV;
            x = line[18] - '0';
            y = line[28] - '0';
        } else {                      // rotate ...
            switch (line[7]) {
                case 'b': op = ROTC; x = line[35];               break;  // rotate based on position of letter X
                case 'l': op = ROTR; y = len - (line[12] - '0'); break;  // rotate left  X steps
                case 'r': op = ROTR; y = line[13] - '0';         break;  // rotate right X steps
            }
        }
        algo[n++] = (cmd_t){op, x, y};
    }
    fclose(f);
    free(line);
    return n;
}

static void scramble(int n)
{
    char t, tmp[len];
    int x, y;
    for (int i = 0; i < n; ++i) {
        cmd_t *c = &algo[i];
        switch (c->op) {
            case NOP: break;
            case MOVU:
                t = pwd[c->x];
                for (int k = c->x; k < c->y; ++k)
                    pwd[k] = pwd[k + 1];
                pwd[c->y] = t;
                break;
            case MOVD:
                t = pwd[c->x];
                for (int k = c->x; k > c->y; --k)
                    pwd[k] = pwd[k - 1];
                pwd[c->y] = t;
                break;
            case SWAPI:
                t = pwd[c->x];
                pwd[c->x] = pwd[c->y];
                pwd[c->y] = t;
                break;
            case SWAPC:
                x = 0;
                y = 0;
                while (pwd[x] != c->x) ++x;
                while (pwd[y] != c->y) ++y;
                pwd[x] = (char)c->y;
                pwd[y] = (char)c->x;
                break;
            case REV:
                x = c->x;
                y = c->y;
                while (x < y) {
                    t = pwd[x];
                    pwd[x++] = pwd[y];
                    pwd[y--] = t;
                }
                break;
            case ROTC:  // rotate based on position of letter X
                x = 0;
                while (pwd[x] != c->x) ++x;
                c->y = (1 + x + (x >= 4)) % len;
                // Fall through
            case ROTR:  // rotate right Y steps
                if (c->y) {
                    for (int k = 0; k < len; ++k)
                        tmp[(k + c->y) % len] = pwd[k];
                    memcpy(pwd, tmp, len);
                }
                break;
        }
    }
}

#if !EXAMPLE
static void unscramble(int n)
{
    char t, tmp[len];
    int x, y;
    for (int i = n - 1; i >= 0; --i) {
        cmd_t *c = &algo[i];
        switch (c->op) {
            case NOP: break;
            case MOVU:  // move position X to position Y (X < Y)
                t = pwd[c->y];
                for (int k = c->y; k > c->x; --k)
                    pwd[k] = pwd[k - 1];
                pwd[c->x] = t;
                break;
            case MOVD:  // move position X to position Y (X > Y)
                t = pwd[c->y];
                for (int k = c->y; k < c->x; ++k)
                    pwd[k] = pwd[k + 1];
                pwd[c->x] = t;
                break;
            case SWAPI:  // swap position X with position Y
                t = pwd[c->x];
                pwd[c->x] = pwd[c->y];
                pwd[c->y] = t;
                break;
            case SWAPC:  // swap letter X with letter Y
                x = 0;
                y = 0;
                while (pwd[x] != c->x) ++x;
                while (pwd[y] != c->y) ++y;
                pwd[x] = (char)c->y;
                pwd[y] = (char)c->x;
                break;
            case REV:  // reverse positions X through Y
                x = c->x;
                y = c->y;
                while (x < y) {
                    t = pwd[x];
                    pwd[x++] = pwd[y];
                    pwd[y--] = t;
                }
                break;
            case ROTC:  // rotate based on position of letter X
                x = 0;
                while (pwd[x] != c->x) ++x;
                c->y = (!x || (x & 1) ? (x >> 1) + 1 : (x >> 1) + 5) % len;
                // Fall through
            case ROTR:  // rotate right Y steps
                if (c->y) {
                    for (int k = 0; k < len; ++k)
                        tmp[k] = pwd[(k + c->y) % len];
                    memcpy(pwd, tmp, len);
                }
                break;
        }
    }
}
#endif

int main(void)
{
    int n = parse();
    scramble(n);
    printf("Part 1: %s\n", pwd);  // bfheacgd
#if !EXAMPLE
    strcpy(pwd, PW2);
    unscramble(n);
    printf("Part 2: %s\n", pwd);  // gcehdbfa
#endif
    return 0;
}
