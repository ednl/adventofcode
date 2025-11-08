#include <stdio.h>     // printf, fopen, fclose, getline
#include <stdlib.h>    // malloc, realloc, free
#include <stdint.h>    // uint64_t, UINT64_C
#include <stdbool.h>   // bool, true, false
#include <time.h>      // clock_gettime

static const char *inp = "../aocinput/2020-22-example.txt";

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

typedef struct {
    unsigned int size, head;
    unsigned char card[MAXHAND];
} HAND, *PHAND;

typedef uint64_t setdata_t;
typedef struct {
    unsigned int len, maxlen;
    setdata_t *data;
} SET, *PSET;

// CRC-64 of byte data
static uint64_t crc64(unsigned char *data, unsigned int len)
{
    static uint64_t crc64_table[CRC64_TABLESIZE];
    static bool firstrun = true;
    uint64_t crc;
    unsigned int i, j;

    if (firstrun) {
        firstrun = false;
        // Make big-endian (MSB) table of first 256 CRC-64 values
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

    crc = CRC64_COMPLEMENT;
    for (i = 0; i < len; ++i) {
        crc = (crc << CRC64_CHARBITS) ^ crc64_table[(data[i] ^ (crc >> CRC64_MBITS)) & CRC64_MAXINDEX];
    }
    return crc ^ CRC64_COMPLEMENT;
}

// Allocate first batch of memory for set
// 1 = success, 0 = failure
static unsigned int set_init(PSET s)
{
    s->len = 0;
    s->maxlen = SETGROW;
    size_t size = s->maxlen * sizeof(setdata_t);
    void *res = s->data == NULL ? malloc(size) : realloc(s->data, size);
    if (res) {
        s->data = (setdata_t*)res;
        return 1;  // success
    }
    // Failure
    if (s->data) {
        free(s->data);
        s->data = NULL;
    }
    s->maxlen = 0;
    return 0;
}

// Try to add value to set
// 1 = success, 0 = failure (already in set, or out of memory)
static unsigned int set_add(PSET s, setdata_t val)
{
    unsigned int i, ins = s->len;

    // Determine insertion point
    while (ins > 0 && s->data[ins - 1] > val) {
        --ins;
    }
    // Already in set?
    if (ins > 0 && s->data[ins - 1] == val) {
        return 0;
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
            return 0;
        }
    }
    // Shift larger values
    for (i = s->len; i > ins; --i) {
        s->data[i] = s->data[i - 1];
    }
    // Insert new value
    s->data[ins] = val;
    s->len++;
    return 1;
}

// Free allocated memory of a set
static void set_clean(PSET s)
{
    if (s->data) {
        free(s->data);
        s->data = NULL;
    }
    s->len = 0;
    s->maxlen = 0;
}

// Read puzzle input into data structure
static void read(PHAND p)
{
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

// Hash function for two hands based on scoring from part 1
// static setdata_t gameid(PHAND p)
// {
//     // Unique enough (max = sum(squares(1..50)) = 42925 and 1<<16 = 65536)
//     return (score(p) << 16) | score(&p[1]);
// }

// Better hash function?
static setdata_t gameid2(PHAND p)
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

// Crab Combat part 2
static unsigned int game2(PHAND p)
{
    unsigned int i, j, k, win;
    unsigned char draw[PLAYERS], n, max[PLAYERS];
    SET uid = {.len = 0, .maxlen = 0, .data = NULL};  // needs init or the pointer is garbage and can't be realloc'ed
    HAND subgame[PLAYERS];

    set_init(&uid);
    while (p[0].size && p[1].size) {

        // Duplicate game? (or set can't be expanded)
        if (!set_add(&uid, gameid2(p))) {
            set_clean(&uid); // avoid memory leak
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
            win = max[0] > max[1] ? 0 : game2(subgame);
        } else {
            // Noth enough cards; simply compare draw
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

    read(player);
    win = game2(player);
    res = score(&player[win]);
    printf("%u %u\n", win + 1, res);
    return 0;
}
