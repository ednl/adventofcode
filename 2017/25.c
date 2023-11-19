#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LEN   (10 * 1000)      // good value found by trial & error
#define RULES ('F' - 'A' + 1)  // puzzle input has states A-F

struct state {
    uint8_t act;     // transform tape value: 0=clear, 1=set, 2=flip, 3=no action
    char dir[2];     // cursor direction: -1=left, 1=right
    uint8_t nxt[2];  // next state: 0=A .. 5=F
};

static struct state rule[RULES] = {0};
static uint8_t state = 0;
static int loop = 0;
static uint8_t tape[LEN] = {0};
static int cursor = (LEN >> 2) * 3;  // balance of moves in my input is to the left, so start at 3/4

int main(void)
{
    // Read rules
    FILE *f = fopen("../aocinput/2017-25-input.txt", "r");
    if (f != NULL) {
        char c;
        if (fscanf(f, " Begin in state %c. ", &c) == 1) {
            state = (uint8_t)(c - 'A');
        }
        int n;
        if (fscanf(f, " Perform a diagnostic checksum after %d steps. ", &n) == 1) {
            loop = n;
        }
        for (int i = 0; i < RULES; ++i) {
            uint8_t k = (uint8_t)i;
            if (fscanf(f, " In state %c: ", &c) == 1) {
                k = (uint8_t)(c - 'A');
            }
            char b[2] = {0};
            for (int j = 0; j < 2; ++j) {
                int m;
                if (fscanf(f, " If the current value is %d: - Write the value %d. ", &n, &m) == 2) {
                    b[n] = (char)m;
                }
                char s[8];
                if (fscanf(f, " - Move one slot to the %7[a-z]. ", s) == 1) {
                    rule[k].dir[j] = s[0] == 'l' ? -1 : 1;
                }
                if (fscanf(f, " - Continue with state %c. ", &c) == 1) {
                    rule[k].nxt[j] = (uint8_t)(c - 'A');
                }
            }
            if (b[0] == 1 && b[1] == 0) {
                rule[k].act = 2;  // flip
            } else if (b[0] == 1 && b[1] == 1) {
                rule[k].act = 1;  // set
            } else if (b[0] == 0 && b[1] == 0) {
                rule[k].act = 0;  // clear
            } else {
                rule[k].act = 3;  // leave as is
            }
        }
        fclose(f);
    }

    // Print rules
    for (int i = 0; i < RULES; ++i) {
        printf("%c = ", 'A' + i);
        switch (rule[i].act) {
            case 0: printf("clr"); break;
            case 1: printf("set"); break;
            case 2: printf("flp"); break;
            default: printf("   "); break;
        }
        printf(" ; %c%c ; %c%c\n",
            rule[i].dir[0] == -1 ? 'L' : 'R',
            rule[i].dir[1] == -1 ? 'L' : 'R',
            'A' + rule[i].nxt[0],
            'A' + rule[i].nxt[1]);
    }

    // Do the Turing thing
    for (int i = 0; i < loop; ++i) {
        uint8_t val = tape[cursor];
        switch (rule[state].act) {  // 0=clear, 1=set, 2=flip, 3=no action
            case 0: tape[cursor]  = 0; break;
            case 1: tape[cursor]  = 1; break;
            case 2: tape[cursor] ^= 1; break;
        }
        cursor += rule[state].dir[val];
        // if (cursor < 0 || cursor >= LEN) {
        //     fprintf(stderr, "Cursor: %d\n", cursor);
        //     exit(1);
        // }
        state = rule[state].nxt[val];
    }

    // Checksum
    int sum = 0;
    for (int i = 0; i < LEN; ++i) {
        sum += tape[i];
    }
    printf("Checksum: %d\n", sum);

    return 0;
}
