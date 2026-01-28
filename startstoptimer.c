#include <stdio.h>
#include <time.h>
#include "startstoptimer.h"

// CLOCK_MONOTONIC_RAW is a Linux/Darwin extension so it might not be available
// on Posix systems. It is preferred because unaffected by any NTP adjustments
// including clock frequency (which is unrelated to CPU frequency, so even
// CLOCK_MONOTONIC will not vary with varying CPU frequency.)

// Difference calculation from https://en.cppreference.com/w/c/chrono/clock
//   1e3 * t1.tv_sec + 1e-6 * t1.tv_nsec - (1e3 * t0.tv_sec + 1e-6 * t0.tv_nsec)
// Not sure about implications of simplifying that, e.g. as:
//   1e3 * (t1.tv_sec - t0.tv_sec) + 1e-6 * (t1.tv_nsec - t0.tv_nsec)
// which on the face of it could have better precision.
// Update 28 Jan 2026: I went & did it anyway.

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

static struct timespec sst_t0, sst_t1;

// Record current time as stop time
// Return difference to start time in seconds with specified factor
static double sst_stop(const double factor)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &sst_t1);
    const double dsec  = (sst_t1.tv_sec  - sst_t0.tv_sec ) * factor;
    const double dnano = (sst_t1.tv_nsec - sst_t0.tv_nsec) * factor * 1e-9;
    return dsec + dnano;
}

// Record current time as start time
// Warn on Raspberry Pi if not running at max performance
void starttimer(void)
{
    FILE *f = fopen("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "r");
    if (f) {
        if (fgetc(f) != 'p' || fgetc(f) != 'e')
            fprintf(stderr,
                "Warning: CPU not optimised for performance.\n"
                "  Resolve: echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor\n"
                "  Setting will be restored to default 'ondemand' at reboot.\n");
        fclose(f);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &sst_t0);
}

// Time difference in nanoseconds between now and last call to starttimer()
double stoptimer_ns(void)
{
    return sst_stop(1e9);
}

// Time difference in microseconds between now and last call to starttimer()
double stoptimer_us(void)
{
    return sst_stop(1e6);
}

// Time difference in milliseconds between now and last call to starttimer()
double stoptimer_ms(void)
{
    return sst_stop(1e3);
}

// Time difference in seconds between now and last call to starttimer()
double stoptimer_s(void)
{
    return sst_stop(1.0);
}
