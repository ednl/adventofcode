#include <stdio.h>    // printf, fopen, fclose, getline
#include <stdlib.h>   // malloc, realloc, free
#include <stdint.h>   // uint32_t
#include <stdbool.h>  // bool, true, false
#include <time.h>     // clock_gettime

#define READFILE  // if defined then read puzzle input from disk, else read from const

#define PLAYERS 2
#define MAXHAND 50
#define SETGROW 256

#define CRC64_CHARBITS   (UINT64_C(8))
#define CRC64_NBITS      (UINT64_C(64))
#define CRC64_MBITS      (CRC64_NBITS - CRC64_CHARBITS)
#define CRC64_MSB        (UINT64_C(1) << (CRC64_NBITS - UINT64_C(1)))
#define CRC64_TABLESIZE  (UINT64_C(1) << CRC64_CHARBITS)
#define CRC64_MAXINDEX   (CRC64_TABLESIZE - UINT64_C(1))
#define CRC64_COMPLEMENT (UINT64_C(-1))
#define CRC64_GPOLYNOM   (UINT64_C(0x42F0E1EBA9EA3693))

#if defined(READFILE)
static const char *inp = "../aocinput/2020-22-input.txt";
#else
static const unsigned char player1[] = {28, 50, 9, 11, 4, 45, 19, 26, 42, 43, 31, 46, 21, 40, 33, 20, 7, 6, 17, 44, 5, 39, 35, 27, 10};
static const unsigned char player2[] = {18, 16, 29, 41, 14, 12, 30, 37, 36, 24, 48, 38, 47, 34, 15, 8, 49, 23, 1, 3, 32, 25, 22, 13, 2};
static const size_t P1_SIZE = sizeof player1 / sizeof *player1;
static const size_t P2_SIZE = sizeof player2 / sizeof *player2;
#endif

typedef struct {
    unsigned int size, head;
    unsigned char card[MAXHAND];
} HAND, *PHAND;

typedef uint64_t setdata_t;
typedef struct {
    unsigned int len, maxlen;
    setdata_t *data;
} SET, *PSET;

// CRC-64 of a byte array, MSB-first version
static uint64_t crc64(unsigned char *data, unsigned int len)
{
    static uint64_t crc64_table[CRC64_TABLESIZE];
    static bool firstrun = true;
    uint64_t crc;
    unsigned int i, j;

    // Make big-endian (MSB) table of first 256 CRC-64 values
    if (firstrun) {
        firstrun = false;
        crc = CRC64_MSB;
        crc64_table[0] = 0;
        for (i = 1; i < CRC64_TABLESIZE; i <<= 1) {
            if (crc & CRC64_MSB) {
                crc = (crc << 1) ^ CRC64_GPOLYNOM;
            } else {
                crc <<= 1;
            }
            for (j = 0; j < i; ++j) {
                crc64_table[i + j] = crc ^ crc64_table[j];
            }
        }
    }
    // Calculate CRC-64 by using table of cached values
    crc = CRC64_COMPLEMENT;
    for (i = 0; i < len; ++i) {
        crc = (crc << CRC64_CHARBITS) ^ crc64_table[(data[i] ^ (crc >> CRC64_MBITS)) & CRC64_MAXINDEX];
    }
    return crc ^ CRC64_COMPLEMENT;
}

// Initialise set by allocating first batch of memory
// return true on success
static bool set_init(PSET s)
{
    s->len = 0;
    s->maxlen = SETGROW;
    size_t size = s->maxlen * sizeof(setdata_t);
    void *res = s->data == NULL ? malloc(size) : realloc(s->data, size);
    if (res) {
        s->data = (setdata_t*)res;
        return true;  // success
    }
    // Failure
    if (s->data) {
        free(s->data);
        s->data = NULL;
    }
    s->maxlen = 0;
    return false;
}

// Try to add value to set
// return true on success (false = already in set, or out of memory)
static bool set_add(PSET s, setdata_t val)
{
    unsigned int i = 0;

    // Check if already in set
    while (i < s->len && s->data[i] != val) {
        ++i;
    }
    // Refuse duplicate hash
    if (i < s->len) {
        return false;
    }
    // Grow if needed
    if (s->len == s->maxlen) {
        s->maxlen += SETGROW;
        size_t size = s->maxlen * sizeof(setdata_t);
        void *res = s->data ? realloc(s->data, size) : malloc(size);
        if (res) {
            s->data = (setdata_t*)res;
        } else {
            // No room in the inn
            s->maxlen -= SETGROW;
            return false;
        }
    }
    // Add new value at end
    s->data[s->len++] = val;
    return true;
}

// Free allocated memory of set
static void set_clean(PSET s)
{
    if (s->data) {
        free(s->data);
        s->data = NULL;
    }
    s->len = 0;
    s->maxlen = 0;
}

// Start or stop a timer
static double timer(void)
{
    static bool start = true;
    static struct timespec t0;
    struct timespec t1;

    if (start) {
        start = false;
        clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
        return 0;
    } else {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
        start = true;
        return 1.0 * t1.tv_sec + 1e-9 * t1.tv_nsec - (1.0 * t0.tv_sec + 1e-9 * t0.tv_nsec);
    }
}

// Read puzzle input into data structure
static void read(PHAND p)
{
#if defined(READFILE)
	FILE *fp;
	char *s = NULL;
	size_t t = 0;
	unsigned int i = 0, j = 0;
    int k;

	if ((fp = fopen(inp, "r")) != NULL) {
		while (getline(&s, &t, fp) > 0 && i < PLAYERS) {
            if (s[0] == 'P') {
                p[i].size = 0;
                p[i].head = 0;
            } else if (s[0] == '\n') {
                p[i++].size = j;
                j = 0;
            } else if (j < MAXHAND) {
                k = atoi(s);
                if (k > 0 && k <= 255) {
                    p[i].card[j++] = (unsigned char)k;
                }
            }
		}
        if (i < PLAYERS && j > p[i].size) {
            p[i].size = j;
        }
		free(s);
		fclose(fp);
	}
#else
    memcpy(&p[0].card, player1, sizeof player1);
    p[0].size = sizeof player1 / sizeof *player1;
    p[0].head = 0;
    memcpy(&p[1].card, player2, sizeof player2);
    p[1].size = sizeof player2 / sizeof *player2;
    p[1].head = 0;
#endif
}

// Show hands
static void show(PHAND p)
{
    unsigned int i, j, k;

    for (i = 0; i < PLAYERS; ++i) {
        printf("Player %i:", i + 1);
        k = p[i].head;
        for (j = 0; j < p[i].size; ++j) {
            printf(" %u", p[i].card[k++]);
            if (k == MAXHAND) {
                k = 0;
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Score of one hand
static uint32_t score(PHAND p)
{
    uint32_t id = 0;
    unsigned int size = p->size, head = p->head;

    while (size) {
        id += size-- * p->card[head++];
        if (head == MAXHAND) {
            head = 0;
        }
    }
    return id;
}

// Game ID = CRC-64 hash of two hands of cards
static setdata_t gameid(PHAND p)
{
    unsigned char buf[MAXHAND + 2];
    unsigned int i, j, k, n;

    // Copy circular to linear buffer
    n = 0;
    for (i = 0; i < PLAYERS; ++i) {
        k = p[i].head;
        for (j = 0; j < p[i].size; ++j) {
            buf[n++] = p[i].card[k++];
            if (k == MAXHAND) {
                k = 0;
            }
        }
        buf[n++] = 0;  // hands delimiter
    }
    return crc64(buf, n);
}

// Crab Combat part 1
static unsigned int game1(PHAND p)
{
    unsigned int i, win;
    unsigned char draw[PLAYERS];

    while (p[0].size && p[1].size) {

        for (i = 0; i < PLAYERS; ++i) {
            draw[i] = p[i].card[p[i].head++];
            if (p[i].head == MAXHAND) {
                p[i].head = 0;
            }
            p[i].size--;
        }
        win = draw[1] > draw[0];

        for (i = 0; i < PLAYERS; ++i) {
            p[win].card[(p[win].head + p[win].size++) % MAXHAND] = draw[(win + i) % PLAYERS];
        }
    }

    return p[1].size > p[0].size;
}

// Crab Combat part 2
static unsigned int game2(PHAND p)
{
    unsigned int i, j, k, win;
    unsigned char draw[PLAYERS], n, max[PLAYERS];
    SET uid = {.len = 0, .maxlen = 0, .data = NULL};  // needs init or the pointer is garbage and can't be realloc'ed
    HAND subgame[2];

    set_init(&uid);
    while (p[0].size && p[1].size) {

        // Duplicate game? (or: set can't be expanded)
        if (!set_add(&uid, gameid(p))) {
            set_clean(&uid);  // avoid memory leak
            return 0;  // player 1 wins
        }

        // Draw 1 card each
        for (i = 0; i < PLAYERS; ++i) {
            draw[i] = p[i].card[p[i].head++];
            if (p[i].head == MAXHAND) {
                p[i].head = 0;
            }
            p[i].size--;
        }

        // At least as many cards left as value drawn?
        if (p[0].size >= draw[0] && p[1].size >= draw[1]) {
            // Copy hands to subgame, and determine max per hand
            for (i = 0; i < PLAYERS; ++i) {
                max[i] = 0;
                subgame[i].size = draw[i];
                subgame[i].head = 0;
                k = p[i].head;
                for (j = 0; j < draw[i]; ++j) {
                    n = p[i].card[k++];
                    if (k == MAXHAND) {
                        k = 0;
                    }
                    subgame[i].card[j] = n;
                    if (n > max[i]) {
                        max[i] = n;
                    }
                }
            }
            // Recurse if necessary
            // If player 1 holds the highest card, they will win either by regular play or by repetition
            // If player 2 holds the highest card, player 1 might still win by repetition, so recursing is necessary
            win = max[0] > max[1] ? 0 : game2(subgame);
        } else {
            // Noth enough cards for subgame; determine winner by who drew the highest card
            win = draw[1] > draw[0];
        }

        // Stack in order at the bottom of the winner's hand
        for (i = 0; i < PLAYERS; ++i) {
            p[win].card[(p[win].head + p[win].size++) % MAXHAND] = draw[(win + i) % PLAYERS];
        }
    }
    set_clean(&uid);  // avoid memory leak
    return p[1].size > p[0].size;
}

int main(void)
{
    HAND player[PLAYERS];
    unsigned int win, res;

    // Part 1
    timer();
    read(player);
    // show(player);
    win = game1(player);
    res = score(&player[win]);
    printf("1: %u %u %.6f\n", win + 1, res, timer());  // for my puzzle input: 1 31629

    // Part 2
    timer();
    read(player);  // fresh data from disk
    win = game2(player);
    res = score(&player[win]);
    printf("2: %u %u %.6f\n", win + 1, res, timer());  // for my puzzle input: 1 35196

    return 0;
}
