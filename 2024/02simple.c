#include <stdio.h>  // fopen, fclose, fgets, sscanf, printf
#include <ctype.h>  // isdigit, isspace
#include <stdbool.h>

static bool issafe(const char *const a, const int first, const int last, const int step)
{
    for (int i = first; i != last; i += step) {
        const int gap = a[i + step] - a[i];
        if (gap < 1 || gap > 3)
            return false;
    }
    return true;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2024-02-input.txt", "r");
    if (!f)
        return 1;
    char *s, report[BUFSIZ];  // BUFSIZ is defined in stdio, probably as 1024
    int safe = 0, damp = 0;
    while ((s = fgets(report, sizeof report, f))) {  // get one report (= one line from input file)
        // Parse values in report, all values in input file are: 0<val<100
        int val, len = 0;
        while (sscanf(s, "%d", &val) == 1) {  // read one number
            while (isdigit(*s)) ++s;  // skip all digits
            while (isspace(*s)) ++s;  // skip all spaces and newlines
            // 's' now points either to the next number, or to the string terminator '\0'
            report[len++] = val & 127;  // save value (0<=val<=127) to char array, increment length
        }
        safe += issafe(report, 0, len - 1, 1) || issafe(report, len - 1, 0, -1);
    }
    fclose(f);
    printf("%d %d\n", safe, damp);
    return 0;
}
