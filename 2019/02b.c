///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 2: 1202 Program Alarm, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // fopen, fgetc, printf
#include <stdlib.h>  // atoi

#define BUFLEN 16
#define ADD  1
#define MUL  2
#define RET 99
#define TARGET 19690720

const char *inp = "../aocinput/2019-02-input.txt";

void copy(int *src, int *dst, int len)
{
    while (len--)
        *dst++ = *src++;
}

int main(void)
{
    FILE *fp;
    char val[BUFLEN];
    int i, c, err = 0;

    int *dat, *mem, noun, verb, ip, op, np, size = 1;
    int p1, p2, p3;

    // Open input file
    if ((fp = fopen(inp, "r")) == NULL) {
        puts("File not found");
        err = 1;
        goto quit;
    }

    // Count CSV fields
    while ((c = fgetc(fp)) != EOF)
        if (c == ',')
            ++size;

    // Reserve memory space
    dat = (int *)malloc(size * sizeof(int));
    mem = (int *)malloc(size * sizeof(int));

    if (dat == NULL || mem == NULL) {
        puts("Out of memory");
        err = 2;
        goto quit;
    }

    // Read data from file
    rewind(fp);
    i = 0;
    ip = 0;
    while ((c = fgetc(fp)) != EOF)
        if (c >= '0' && c <= '9' && i < BUFLEN - 1)
            val[i++] = c;
        else {
            if (i > 0 && ip < size) {
                val[i] = '\0';
                dat[ip++] = atoi(val);
            }
            i = 0;
        }
    if (i > 0 && ip < size) {
        val[i] = '\0';
        dat[ip++] = atoi(val);
    }
    fclose(fp);
    fp = NULL;

    // Seek target
    noun = 0;
    verb = 0;
    do {

        // Initialise program
        copy(dat, mem, size);
        mem[1] = noun;
        mem[2] = verb;

        // Run program
        ip = 0;
        while (ip < size && (op = mem[ip]) != RET) {
            if (op == ADD || op == MUL)
                np = 3;
            else
                np = 0;
            if (ip + np < size) {
                if (np == 3) {
                    p1 = mem[ip + 1];
                    p2 = mem[ip + 2];
                    p3 = mem[ip + 3];
                    if (p1 >= 0 && p2 >= 0 && p3 >= 0 &&
                        p1 < size && p2 < size && p3 < size)
                        switch (op) {
                            case ADD: mem[p3] = mem[p1] + mem[p2]; break;
                            case MUL: mem[p3] = mem[p1] * mem[p2]; break;
                        }
                }
            }
            ip += np + 1;
        }

        // Check target
        if (mem[0] == TARGET)
            break;

        // Next iteration
        ++verb;
        if (verb > RET) {
            verb = 0;
            ++noun;
        }

    } while (noun <= RET);
    printf("%i\n", noun * 100 + verb);

quit:
    if (fp != NULL)
        fclose(fp);
    if (dat != NULL)
        free(dat);
    if (mem != NULL)
        free(mem);
    return err;
}
