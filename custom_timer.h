#ifndef CUSTOM_TIMER
#define CUSTOM_TIMER
#include <linux/time.h>

#define TIMER_INIT long TIMER; struct timespec start, end, difftime; 
#define TIMER_START getnstimeofday(&start);
#define TIMER_END getnstimeofday(&end); timespec_diff(&start, &end, &difftime); TIMER = difftime.tv_nsec;

void timespec_diff(struct timespec*, struct timespec*, struct timespec*);
#endif
