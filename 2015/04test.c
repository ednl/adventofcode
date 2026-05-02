#include <stdio.h>
#include "../startstoptimer.h"

// Count up decimal number
// *first points to  most significant digit
//   last points to least significant digit
// Prereq: *first - 1 must always be valid address and contain '0'
static void incr(char **first, char *last)
{
    (*last)++;             // start by increasing unit
    while (*last > '9') {  // cascade through nines
        *last-- = '0';     // set to zero and go to next power of ten
        (*last)++;         // add one (=ten)
    }
    if (last < *first)     // new digit?
        *first = last;
}

int main(void)
{
    starttimer();
    char buf[16] = "000000000000000";
    char *first = &buf[sizeof buf - 2];
    char *last = first;
    for (unsigned n = 1; n < 10000000; ++n) {
        incr(&first, last);
        // sprintf(buf, "%u", n);
    }
    printf("Time: %.0f us\n", stoptimer_us());  // incr: 19.7 ms, sprintf: 371 ms
}
