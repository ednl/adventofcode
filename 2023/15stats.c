#include <stdio.h>
#include <stdint.h>

#define NAME "../aocinput/2023-15-input.txt"
#define N 4000

typedef struct step {
    char lens[8];
    uint32_t ord, id, box, f;
} Step;

typedef struct box {
    uint32_t add[32], rem[32];
    uint32_t alen, rlen;
} Box;

static Step step[N];
static Box box[256];

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    uint32_t count = 0;
    Step* s = step;
    do {
        s->ord = count++;
        char* lens = s->lens;
        uint32_t* id = &s->id;
        int c;
        while ((c = fgetc(f)) >= 'a') {
            *lens++ = (char)c;
            *id += (unsigned)c;
            *id *= 17;
        }
        *lens = '\0';
        s->box = *id & 0xff;
        switch (c) {
            case '-':
                s->f = 0;
                box[s->box].rem[box[s->box].rlen++] = *id;
                break;
            case '=':
                s->f = fgetc(f) & 15;
                box[s->box].add[box[s->box].alen++] = *id;
                break;
        }
        ++s;
    } while (count < N && fgetc(f) == ',');  // comma or newline
    fclose(f);

    uint32_t maxadd = 0, maxrem = 0, maxtot = 0;
    for (int i = 0; i < 256; ++i) {
        if (box[i].alen > maxadd) maxadd = box[i].alen;
        if (box[i].rlen > maxrem) maxrem = box[i].rlen;
        const uint32_t tot = box[i].alen + box[i].rlen;
        if (tot > maxtot) maxtot = tot;
    }
    printf("add=%u rem=%u tot=%u\n", maxadd, maxrem, maxtot);

    return 0;
}
