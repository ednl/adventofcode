#ifndef STARTSTOPTIMER_H
#define STARTSTOPTIMER_H

// Record current time as start time
// Warn on Raspberry Pi if not running at max performance
void starttimer(void);

// Time difference in nanoseconds between now and last call to starttimer()
double stoptimer_ns(void);

// Time difference in microseconds between now and last call to starttimer()
double stoptimer_us(void);

// Time difference in milliseconds between now and last call to starttimer()
double stoptimer_ms(void);

// Time difference in seconds between now and last call to starttimer()
double stoptimer_s(void);

#endif  // STARTSTOPTIMER_H
