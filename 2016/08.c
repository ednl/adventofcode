#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define FNAME "../aocinput/2016-08-input.txt"
#define COLS 50
#define ROWS  6

static bool screen[ROWS][COLS], framebuf[ROWS][COLS];

static void rotrow(const int row, const int amount)
{
    int i = COLS, j = COLS - amount;
    while (i) {
        framebuf[row][--i] = screen[row][--j];
        if (!j)
            j = COLS;
    }
    memcpy(screen, framebuf, sizeof screen);
}

static void rotcol(const int col, const int amount)
{
    int i = ROWS, j = ROWS - amount;
    while (i) {
        framebuf[--i][col] = screen[--j][col];
        if (!j)
            j = ROWS;
    }
    memcpy(screen, framebuf, sizeof screen);
}

static void rect(const int w, const int h)
{
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            screen[i][j] = true;
    memcpy(framebuf, screen, sizeof framebuf);
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        perror(FNAME);
        return 1;
    }

    int a, b;
    char line[32];
    while (fgets(line, sizeof line, f))
        if (line[1] == 'e') {
            // rect 1x1
            if (sscanf(&line[5], "%dx%d", &a, &b) == 2)
                rect(a, b);
        } else if (line[7] == 'r') {
            // rotate row y=0 by 7
            if (sscanf(&line[13], "%d by %d", &a, &b) == 2)
                rotrow(a, b);
        } else {
            // rotate column x=0 by 1
            if (sscanf(&line[16], "%d by %d", &a, &b) == 2)
                rotcol(a, b);
        }
    fclose(f);

    int on = 0;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            on += screen[i][j];
            printf("%c", screen[i][j] ? '#' : ' ');
        }
        putchar('\n');
    }
    printf("Part 1: %d\n", on);  // 128
    // Part 2: EOARGPHYAO
    // ####  ##   ##  ###   ##  ###  #  # #   # ##   ##
    // #    #  # #  # #  # #  # #  # #  # #   ##  # #  #
    // ###  #  # #  # #  # #    #  # ####  # # #  # #  #
    // #    #  # #### ###  # ## ###  #  #   #  #### #  #
    // #    #  # #  # # #  #  # #    #  #   #  #  # #  #
    // ####  ##  #  # #  #  ### #    #  #   #  #  #  ##

    return 0;
}
