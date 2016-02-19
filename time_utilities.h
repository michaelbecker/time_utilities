/**
 *  @file
 *
 *  Utility functions around timespec and timeval data structures.
 *  This approach attempts to keep time data as a timeval (or timespec)
 *  data structure. This keeps the data in the most precise format 
 *  available. Converting to an integer, long, or double will 
 *  lose precision, and by using the functions in this file simply
 *  isn't needed.
 *
 *  The naming convention attempts to follow C format, 
 *  which means lower case, underscores, etc.
 *
 *  MIT License
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included 
 *  in all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT 
 *  OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
 *  THE USE OR OTHER DEALINGS IN THE SOFTWARE.man 
 */
#ifndef TIME_UTILITIES_H_
#define TIME_UTILITIES_H_

#include <time.h>


/**
 *  Various time conversions.
 */
#define NS_IN_SECOND    (1000000000L)
#define US_IN_SECOND    (1000000L)
#define MS_IN_SECOND    (1000L)
#define NS_IN_MS        (1000L * 1000L)
#define US_IN_MS        (1000L)


/**
 *  Get the realtime clock time.
 *  @param[out] ts time returned in this variable.
 *  @return 0 on success, -1 in failure.
 */
inline int timespec_now(struct timespec *ts)
{
    return clock_gettime(CLOCK_REALTIME, ts);
}


/**
 *  Get the monotonic clock time.
 *  @param[out] ts time returned in this variable.
 *  @return 0 on success, -1 in failure.
 */
inline int timespec_now_monotonic(struct timespec *ts)
{
    return clock_gettime(CLOCK_MONOTONIC, ts);
}


/**
 *  Get the monotonic raw clock time.
 *  @param[out] ts time returned in this variable.
 *  @return 0 on success, -1 in failure.
 */
inline int timespec_now_monotonic_raw(struct timespec *ts)
{
    return clock_gettime(CLOCK_MONOTONIC_RAW, ts);
}


/**
 *  Add two timespec values, normalizing the result.
 *  @param[out] sum = addend_a + addend_b
 *  @param[in] addend_a (already normalized)
 *  @param[in] addend_b (already normalized)
 */
inline void timespec_add(   struct timespec *sum, 
                            struct timespec *addend_a,
                            struct timespec *addend_b)
{
    sum->tv_sec = addend_a->tv_sec + addend_b->tv_sec;
    sum->tv_nsec = addend_a->tv_nsec + addend_b->tv_nsec;
    if (sum->tv_nsec >= NS_IN_SECOND){
        sum->tv_sec++;
        sum->tv_nsec -= NS_IN_SECOND;
    }
}


/**
 *  Subtract two timespec values, normalizing the result.
 *  @param[out] difference = minuend - subtrahend
 *  @param[in] minuend (already normalized)
 *  @param[in] subtrahend (already normalized)
 */
inline void timespec_subtract(  struct timespec *difference,
                                struct timespec *minuend,
                                struct timespec *subtrahend)
{
    difference->tv_sec = minuend->tv_sec - subtrahend->tv_sec;
    difference->tv_nsec = minuend->tv_nsec - subtrahend->tv_nsec;
    if (difference->tv_nsec < 0){
        difference->tv_sec--;
        difference->tv_nsec += NS_IN_SECOND;
    }
}


/**
 *  Fill in a timespec struct given milliseconds.
 *  @param[out] ts
 *  @param[in] ms
 */
inline void  timespec_from_ms(struct timespec *ts, int ms)
{
    ts->tv_sec = ms / MS_IN_SECOND;
    ts->tv_nsec = (ms % MS_IN_SECOND) * NS_IN_MS;
}


/**
 *  Compare two timespec structures, like strcmp.
 *  Expects the timespec arguments to be normalized.
 *  This function returns:
 *     -1 =>    a < b
 *      0 =>    a = b
 *      1 =>    a > b
 *  @param[in] a (already normalized)
 *  @param[in] b (already normalized)
 *  @return -1, 0, or 1
 */
inline int timespec_compare(struct timespec *a, struct timespec *b)
{
    if (a->tv_sec > b->tv_sec){
        return 1;
    }
    else if (a->tv_sec < b->tv_sec){
        return -1;
    }
    else if (a->tv_nsec > b->tv_nsec){
        return 1;
    }
    else if (a->tv_nsec < b->tv_nsec){
        return -1;
    }
    else{
        return 0;
    }
}


/**
 *  Fix a timespec struct so it is normalized.
 *  We assume nothing about how unnormalized the timespec is.
 *  @param[in|out] ts structure that will be normalized.
 */
inline void timespec_normalize(struct timespec *ts)
{
    while (ts->tv_nsec >= NS_IN_SECOND) {
        ts->tv_sec++;
        ts->tv_nsec -= NS_IN_SECOND;
    }
    while (ts->tv_nsec < 0) {
        ts->tv_sec--;
        ts->tv_nsec += NS_IN_SECOND;
    }
}


/*
 *  The struct timeval structure is not part of POSIX, however it is 
 *  used a lot in Linux / BSD / *nix code, and the structure is very 
 *  similar to the timespec structure. Define USING_TIMEVAL to get the 
 *  corresponding APIs.
 */
#ifdef USING_TIMEVAL
#include <sys/time.h>


/**
 *  Convert a struct timespec to a struct timeval.
 *  @param[out] tv
 *  @param[in] ts
 */
inline void timespec_to_timeval(struct timeval *tv, const struct timespec *ts)
{
    tv->tv_sec = ts->tv_sec;
    tv->tv_usec = ts->tv_nsec / 1000;
}


/**
 *  Convert a struct timeval to a struct timespec.
 *  @param[out] ts
 *  @param[in] tv
 */
inline void timeval_to_timespec(struct timespec *ts, const struct timeval *tv)
{
    ts->tv_sec = tv->tv_sec;
    ts->tv_nsec = tv->tv_usec * 1000;
}


/**
 *  Get the realtime clock time.
 *  @param[out] tv time returned in this variable.
 *  @return 0 on success, -1 in failure.
 */
inline int timeval_now(struct timeval *tv)
{
    struct timespec ts;
    int rc;
    rc = clock_gettime(CLOCK_REALTIME, &ts);
    timespec_to_timeval(tv, &ts);
    return rc;
}


/**
 *  Get the monotonic clock time.
 *  @param[out] tv time returned in this variable.
 *  @return 0 on success, -1 in failure.
 */
inline int timeval_now_monotonic(struct timeval *tv)
{
    struct timespec ts;
    int rc;
    rc = clock_gettime(CLOCK_MONOTONIC, &ts);
    timespec_to_timeval(tv, &ts);
    return rc;
}


/**
 *  Get the monotonic raw clock time.
 *  @param[out] tv time returned in this variable.
 *  @return 0 on success, -1 in failure.
 */
inline int timeval_now_monotonic_raw(struct timeval *tv)
{
    struct timespec ts;
    int rc;
    rc = clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    timespec_to_timeval(tv, &ts);
    return rc;
}


/**
 *  Add two timeval values, normalizing the result.
 *  @param[out] sum = addend_a + addend_b
 *  @param[in] addend_a (already normalized)
 *  @param[in] addend_b (already normalized)
 */
inline void timeval_add(struct timeval *sum, 
                        struct timeval *addend_a,
                        struct timeval *addend_b)
{
    sum->tv_sec = addend_a->tv_sec + addend_b->tv_sec;
    sum->tv_usec = addend_a->tv_usec + addend_b->tv_usec;
    if (sum->tv_usec >= US_IN_SECOND){
        sum->tv_sec++;
        sum->tv_usec -= US_IN_SECOND;
    }
}


/**
 *  Subtract two timeval values, normalizing the result.
 *  @param[out] difference = minuend - subtrahend
 *  @param[in] minuend (already normalized)
 *  @param[in] subtrahend (already normalized)
 */
inline void timeval_subtract(  struct timeval *difference,
                                struct timeval *minuend,
                                struct timeval *subtrahend)
{
    difference->tv_sec = minuend->tv_sec - subtrahend->tv_sec;
    difference->tv_usec = minuend->tv_usec - subtrahend->tv_usec;
    if (difference->tv_usec < 0){
        difference->tv_sec--;
        difference->tv_usec += US_IN_SECOND;
    }
}


/**
 *  Fill in a timeval struct given milliseconds.
 *  @param[out] tv
 *  @param[in] ms
 */
inline void  timeval_from_ms(struct timeval *tv, int ms)
{
    tv->tv_sec = ms / MS_IN_SECOND;
    tv->tv_usec = (ms % MS_IN_SECOND) * US_IN_MS;
}


/**
 *  Compare two timeval structures, like strcmp.
 *  Expects the timeval arguments to be normalized.
 *  This function returns:
 *     -1 =>    a < b
 *      0 =>    a = b
 *      1 =>    a > b
 *  @param[in] a (already normalized)
 *  @param[in] b (already normalized)
 *  @return -1, 0, or 1
 */
inline int timeval_compare(struct timeval *a, struct timeval *b)
{
    if (a->tv_sec > b->tv_sec){
        return 1;
    }
    else if (a->tv_sec < b->tv_sec){
        return -1;
    }
    else if (a->tv_usec > b->tv_usec){
        return 1;
    }
    else if (a->tv_usec < b->tv_usec){
        return -1;
    }
    else{
        return 0;
    }
}


/**
 *  Fix a timeval struct so it is normalized.
 *  We assume nothing about how unnormalized the timeval is.
 *  @param[in|out] ts structure that will be normalized.
 */
inline void timeval_normalize(struct timeval *tv)
{
    while (tv->tv_usec >= US_IN_SECOND) {
        tv->tv_sec++;
        tv->tv_usec -= US_IN_SECOND;
    }
    while (tv->tv_usec < 0) {
        tv->tv_sec--;
        tv->tv_usec += US_IN_SECOND;
    }
}

#endif /* USING_TIMEVAL */


#endif

