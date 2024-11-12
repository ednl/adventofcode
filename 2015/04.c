/**
 * Advent of Code 2015
 * Day 4: The Ideal Stocking Stuffer
 * https://adventofcode.com/2015/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 04.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 04.c ../startstoptimer.c
 */

#if __APPLE__
    #include <sys/sysctl.h>  // sysctlbyname
#elif __linux__
    #define _GNU_SOURCE  // must come before all includes, not just sched.h
    #include <sched.h>   // sched_getaffinity
#endif
#include <stdio.h>
#include <stdlib.h>     // exit, EXIT_FAILURE
#include <stdint.h>     // uint8_t, uint32_t, UINT32_C
#include <stdbool.h>    // bool, true, false
#include <stdatomic.h>  // atomic_bool
#include <pthread.h>    // pthread_create, pthread_join
#include "../startstoptimer.h"

// Personalised input from Advent of Code.
#define INPUT "iwrupvqb"
#define INPUTLEN 8

// Bit masks to match 5 or 6 zeros at start of 32 hex char digest (little-endian).
#define MASK5 UINT32_C(0x00F0FFFF)
#define MASK6 UINT32_C(0x00FFFFFF)

// Arbitrary limit to avoid dynamic allocation of arg array.
#define MAXTHREADS 32

typedef struct data {
    uint32_t start, step, mask, result;
} Data;

// Signal for other threads to exit when the desired value is found in one thread.
static atomic_bool found = false;

// Number of CPU cores available to this program.
// Return: value between lo and hi, inclusive.
static int coresavail(const int lo, const int hi)
{
    int n = 0;
    #if __APPLE__
        size_t size = sizeof n;
        sysctlbyname("hw.activecpu", &n, &size, NULL, 0);
    #elif __linux__
        cpu_set_t set;
        CPU_ZERO(&set);
        sched_getaffinity(0, sizeof set, &set);
        n = CPU_COUNT(&set);
    #elif _WIN32
+       n = atoi(getenv("NUMBER_OF_PROCESSORS") ?: "1");
    #endif
    return n < lo ? lo : (n > hi ? hi : n);
}

// Adapted for one-pass short message from https://en.wikipedia.org/wiki/MD5#Algorithm
static uint32_t md5(unsigned number)
{
    static const uint32_t rot[64] = {
        7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
        5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
        4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
        6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21};
    static const uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

    uint8_t chunk[64] = INPUT;  // message is always shorter than 56 bytes, so this is the first and last chunk
    unsigned msglen = INPUTLEN;

    uint8_t buf[16];
    unsigned digits = 0;
    while (number) {
        buf[digits++] = '0' | (number % 10u);
        number /= 10u;
    }
    while (digits)
        chunk[msglen++] = buf[--digits];
    chunk[msglen] = 0x80;  // append magic stop byte
    chunk[56] = msglen << 3;  // msglen in bits < 256 so only chunk[56] is ok

    // Break 512-bit message into sixteen 32-bit words M[j]
    uint32_t M[16];
    for (int j = 0; j < 16; ++j) {
        int k = j << 2;  // j * 4
        M[j] = (uint32_t)chunk[k + 3] << 24  // high to low prec: cast, shift, or
             | (uint32_t)chunk[k + 2] << 16
             | (uint32_t)chunk[k + 1] <<  8
             | (uint32_t)chunk[k];
    }

    uint32_t A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
    for (uint32_t i = 0; i < 64; ++i) {
        uint32_t F, g;
        switch (i >> 4) {
            case 0:
                F = D ^ (B & (C ^ D));
                g = i;
                break;
            case 1:
                F = C ^ (D & (B ^ C));
                g = (i * 5 + 1) & 0xf;
                break;
            case 2:
                F = B ^ C ^ D;
                g = (i * 3 + 5) & 0xf;
                break;
            default:
                F = C ^ (B | ~D);
                g = (i * 7) & 0xf;
                break;
        }
        F += A + K[i] + M[g];
        A = D;
        D = C;
        C = B;
        B += F << rot[i] | F >> (32 - rot[i]);
    }
    return A + 0x67452301;  // first 8 hex digits of 32-char hex digest
}

// Parallel execution in separate threads.
static void *loop(void *arg)
{
    Data *data = arg;
    uint32_t num = data->start;
    const uint32_t step = data->step;
    const uint32_t mask = data->mask;

    while (!atomic_load(&found)) {
        while (md5(++num) & MASK5);
        atomic_store(&found, true);
    }

    data->result = 0;
    return NULL;
}

int main(void)
{
    starttimer();

     // Launch parallel threads.
    const int threads = coresavail(1, MAXTHREADS);
    pthread_t tid[MAXTHREADS];  // thread IDs
    Data arg[MAXTHREADS];  // thread arguments going in and out
    for (int i = 0; i < threads; ++i) {
        arg[i] = (Data){
            .start = i + 1,  // thread 0 starts with 1, thread 1 with 2, etc.
            .step = threads,
            .mask = MASK5
        };  // output vars init to zero
        if (pthread_create(&tid[i], NULL, loop, &arg[i])) {
            fprintf(stderr, "Unable to launch thread %d of %d.\n", i + 1, threads);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish.
    for (int i = 0; i < threads; ++i)
        pthread_join(tid[i], NULL);

    // unsigned n = 0;
    // while (md5(++n) & MASK5);
    // printf("Part 1: %u\n", n);  // 346386
    // while (md5(++n) & MASK6);
    // printf("Part 2: %u\n", n);  // 9958218
    printf("Time: %.2f s\n", stoptimer_s());
    return 0;
}
