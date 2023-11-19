#include <stdio.h>
#include <stdlib.h>  // qsort

#define ALPH_LEN    26
#define NAME_LEN    80
#define NAME_FMT   "80"
#define CHKSUM_LEN   5
#define CHKSUM_FMT  "5"

static char name[NAME_LEN + 1] = {0};
static char checksum[CHKSUM_LEN + 1] = {0};

typedef struct {
    int count;
    char letter;
} AlphHist;
static AlphHist hist[ALPH_LEN] = {0};

// Sort by count descending, letter ascending
static int cmphist(const void* a, const void* b)
{
    const AlphHist *h1 = (const AlphHist*)a;
    const AlphHist *h2 = (const AlphHist*)b;
    const int countdiff = h2->count - h1->count;  // safe because INT_MIN is not present
    if (!countdiff)
        return h1->letter - h2->letter;  // alphabetically if same frequency
    return countdiff;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-04-input.txt", "r");
    if (!f)
        return 1;

    int i, n, sectorid, sectorsum = 0;
    while (fscanf(f, "%"NAME_FMT"[-a-z]%n%d[%"CHKSUM_FMT"s] ", name, &n, &sectorid, checksum) == 3)
    {
        // Reset histogram array
        for (int j = 0; j < ALPH_LEN; ++j)
            hist[j] = (AlphHist){0, 'a' + (char)j};

        // Delete last char which is always a dash
        if (n > 0)
            name[--n] = '\0';

        // Count letter occurences in the name
        for (int j = 0; j < n; ++j)
        {
            int bin = name[j] - 'a';
            if (bin >= 0 && bin < ALPH_LEN)
                hist[bin].count++;  // count the letters
        }

        // Sort histogram
        // Because qsort sorts in place, hist needs to be a struct
        // to be able to track the letter as well as its frequency
        qsort(hist, ALPH_LEN, sizeof(AlphHist), cmphist);

        // Compare checksum to the 5 most frequent letters
        i = 0;
        while (i < CHKSUM_LEN && checksum[i] == hist[i].letter)
            ++i;

        // If it's a valid checksum then sum the Sector ID and decrypt the name
        if (i == CHKSUM_LEN) {
            sectorsum += sectorid;
            for (int j = 0; j < n; ++j) {
                if (name[j] == '-')
                    name[j] = ' ';
                else
                    name[j] = 'a' + (name[j] - 'a' + sectorid) % ALPH_LEN;
            }
            printf("%d : %s\n", sectorid, name);
        }
    }
    fclose(f);

    // Solution (print to stderr to avoid grep for part 2)
    fprintf(stderr, "Part 1: %d\n", sectorsum);  // 137896

    // Part 2 is "the Sector ID of the room where North Pole objects are stored"
    // so can probably be found by, e.g.: ./a.out | fgrep north
    // => Part 2: 501

    return 0;
}
