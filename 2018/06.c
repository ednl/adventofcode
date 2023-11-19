#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define N 50     // number of points in the input
#define M 10000  // "safe distance" for part 2

typedef struct {
    int id, x, y, close, inf;
} Point;

static Point point[N] = {0};

// Return the ID of the closest point to a location
// or -1 if multiple points are equally close
static int closest(int x, int y, int outp)
{
    int mindist = INT_MAX, minindex = -1, multiple = 0;
    for (int i = 0; i < N; ++i) {
        int d = abs(point[i].x - x) + abs(point[i].y - y);
        if (d == 0) {
            if (outp)
                printf("%c", 'A' + i % 26);
            return i;
        } else if (d < mindist) {
            mindist = d;
            minindex = i;
            multiple = 0;
        } else if (d == mindist) {
            ++multiple;
            // Can't return early here because the last point might be closer
        }
    }
    if (multiple) {
        if (outp)
            printf(".");
        return -1;
    }
    if (outp)
        printf("%c", 'a' + minindex % 26);
    return minindex;
}

// Return whether the location falls within the "safe distance" area
// 1 = true, 0 = false
static int safedist(int x, int y)
{
    int d = 0;
    for (int i = 0; i < N; ++i) {
        d += abs(point[i].x - x) + abs(point[i].y - y);
        if (d >= M)
            return 0;
    }
    return 1;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2018-06-input.txt", "r");
    if (!f)
        return 1;

    // Read points from input and keep track of the dimensions
    int n = 0, x, y, xmin = INT_MAX, ymin = INT_MAX, xmax = 0, ymax = 0;
    while (n < N && fscanf(f, "%d, %d ", &x, &y) == 2) {
        point[n] = (Point){n, x, y, 0, 0};
        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
        ++n;
    }
    fclose(f);
    if (n < N || xmin >= xmax || ymin >= ymax)
        return 2;

    // Normalise width and height to start from zero
    for (int i = 0; i < n; ++i) {
        point[i].x -= xmin;
        point[i].y -= ymin;
    }
    xmax -= xmin;
    ymax -= ymin;

    // To which point are border locations closest?
    // That point has an infinite area.
    int i, j;
    for (x = 0; x <= xmax; ++x) {
        i = closest(x, 0, 0);
        j = closest(x, ymax, 0);
        if (i >= 0) point[i].inf = 1;
        if (j >= 0) point[j].inf = 1;
    }
    for (y = 1; y <= ymax - 1; ++y) {
        i = closest(0, y, 0);
        j = closest(xmax, y, 0);
        if (i >= 0) point[i].inf = 1;
        if (j >= 0) point[j].inf = 1;
    }

    // Count how many locations within the box are closest to a point
    // (border locations are always part of an infinite area, so skip them)
    for (x = 1; x < xmax; ++x) {
        for (y = 1; y < ymax; ++y) {
            if ((i = closest(x, y, 0)) >= 0) {
                point[i].close++;
            }
        }
    }

    // Debug output
    // printf("xmax=%d ymax=%d\n", xmax, ymax);
    // for (x = 0; x <= xmax; ++x) {
    //     for (y = 0; y <= ymax; ++y) {
    //         closest(x, y, 1);
    //     }
    //     printf("\n");
    // }

    // Which point (that is not infinite) has the largest area?
    int maxclose = 0, maxindex = -1;
    for (i = 0; i < N; ++i) {
        if (point[i].inf == 0 && point[i].close > maxclose) {
            maxclose = point[i].close;
            maxindex = i;
        }
    }
    if (maxindex >= 0) {
        printf("Part 1: %d\n", point[maxindex].close);
    }

    // How many locations have a "safe distance"?
    // Checking only the coordinates in/on the bounding box of points is enough for my input
    int safe = 0;
    for (x = 0; x <= xmax; ++x) {
        for (y = 0; y <= ymax; ++y) {
            safe += safedist(x, y);
        }
    }
    printf("Part 2: %d\n", safe);

    return 0;
}
