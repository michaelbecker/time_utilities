/**
 *  @file
 *
 *  Unit test code of time_utilities.h
 *
 *  To compile:
 *  gcc -Wall unit_test_time_utilities.c -o unit_test_time_utilities
 *
 *  To test:
 *  ./unit_test_time_utilities
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
 *  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#define USING_TIMEVAL
#include "time_utilities.h"


#define PRINT_TS(x_) \
    printf("%s.tv_sec = %d  .tv_nsec = %d\n",  \
    #x_, (int)x_.tv_sec, (int)x_.tv_nsec);

#define INIT_TS(x_, sec_, nsec_) \
    x_.tv_sec = sec_; \
    x_.tv_nsec = nsec_;

#define ASSERT_TS_VALID(x_, sec_, nsec_) \
    assert(x_.tv_sec == sec_);  \
    assert(x_.tv_nsec == nsec_);


void test_add_timespec(void)
{
    struct timespec a;
    struct timespec b;
    struct timespec c;
    
    INIT_TS(a, 1, 10);
    INIT_TS(b, 2, 20);
    timespec_add(&c, &a, &b);
    ASSERT_TS_VALID(c, 3, 30);

    INIT_TS(a, 1, 999999999);
    INIT_TS(b, 1, 2);
    timespec_add(&c, &a, &b);
    ASSERT_TS_VALID(c, 3, 1);

    INIT_TS(a, 1, 999999999);
    INIT_TS(b, 1, 999999999);
    timespec_add(&c, &a, &b);
    ASSERT_TS_VALID(c, 3, 999999998);
}


void test_subtract_timespec()
{
    struct timespec a;
    struct timespec b;
    struct timespec c;
    
    INIT_TS(a, 10, 100);
    INIT_TS(b, 2, 20);
    timespec_subtract(&c, &a, &b);
    ASSERT_TS_VALID(c, 8, 80);

    INIT_TS(a, 100, 1);
    INIT_TS(b, 1, 20);
    timespec_subtract(&c, &a, &b);
    ASSERT_TS_VALID(c, 98, 999999981);
}


void test_ms_timespec(void)
{
    struct timespec a;
    timespec_from_ms(&a, 1000);
    ASSERT_TS_VALID(a, 1, 0);

    timespec_from_ms(&a, 1);
    ASSERT_TS_VALID(a, 0, 1000000);

    timespec_from_ms(&a, 99999);
    ASSERT_TS_VALID(a, 99, 999000000);
}


void test_compare_timespec(void)
{
    struct timespec a;
    struct timespec b;
    int rc;


    INIT_TS(a, 10, 0);
    INIT_TS(b, 5, 0);
    rc = timespec_compare(&a, &b);
    assert(rc == 1);

    INIT_TS(a, 10, 0);
    INIT_TS(b, 50, 0);
    rc = timespec_compare(&a, &b);
    assert(rc == -1);

    INIT_TS(a, 44, 0);
    INIT_TS(b, 44, 0);
    rc = timespec_compare(&a, &b);
    assert(rc == 0);


    INIT_TS(a, 5, 30);
    INIT_TS(b, 5, 29);
    rc = timespec_compare(&a, &b);
    assert(rc == 1);

    INIT_TS(a, 5, 16);
    INIT_TS(b, 5, 61);
    rc = timespec_compare(&a, &b);
    assert(rc == -1);

    INIT_TS(a, 5, 777);
    INIT_TS(b, 5, 777);
    rc = timespec_compare(&a, &b);
    assert(rc == 0);


    INIT_TS(a, 6, 0);
    INIT_TS(b, 5, 999999999);
    rc = timespec_compare(&a, &b);
    assert(rc == 1);

    INIT_TS(a, 5, 999999999);
    INIT_TS(b, 6, 0);
    rc = timespec_compare(&a, &b);
    assert(rc == -1);

    INIT_TS(a, 44, 999999999);
    INIT_TS(b, 44, 999999999);
    rc = timespec_compare(&a, &b);
    assert(rc == 0);
}


void test_normalize_timespec()
{
    struct timespec a;

    INIT_TS(a, 11, 123456789);
    timespec_normalize(&a);
    ASSERT_TS_VALID(a, 11, 123456789);

    INIT_TS(a, 10, 1000000000);
    timespec_normalize(&a);
    ASSERT_TS_VALID(a, 11, 0);
    
    INIT_TS(a, 10, 2147483647);
    timespec_normalize(&a);
    ASSERT_TS_VALID(a, 12, 147483647);
    
    INIT_TS(a, 10, -1);
    timespec_normalize(&a);
    ASSERT_TS_VALID(a, 9, 999999999);

    INIT_TS(a, 10, -999999999);
    timespec_normalize(&a);
    ASSERT_TS_VALID(a, 9, 1);

    INIT_TS(a, 10, -2147483647);
    timespec_normalize(&a);
    ASSERT_TS_VALID(a, 7, 852516353);
}



#define PRINT_TV(x_) \
    printf("%s.tv_sec = %d  .tv_usec = %d\n",  \
    #x_, (int)x_.tv_sec, (int)x_.tv_usec);

#define INIT_TV(x_, sec_, usec_) \
    x_.tv_sec = sec_; \
    x_.tv_usec = usec_;

#define ASSERT_TV_VALID(x_, sec_, usec_) \
    assert(x_.tv_sec == sec_);  \
    assert(x_.tv_usec == usec_);


void test_add_timeval(void)
{
    struct timeval a;
    struct timeval b;
    struct timeval c;
    
    INIT_TV(a, 1, 10);
    INIT_TV(b, 2, 20);
    timeval_add(&c, &a, &b);
    ASSERT_TV_VALID(c, 3, 30);

    INIT_TV(a, 1, 999999);
    INIT_TV(b, 1, 2);
    timeval_add(&c, &a, &b);
    ASSERT_TV_VALID(c, 3, 1);

    INIT_TV(a, 1, 999999);
    INIT_TV(b, 1, 999999);
    timeval_add(&c, &a, &b);
    ASSERT_TV_VALID(c, 3, 999998);
}


void test_subtract_timeval()
{
    struct timeval a;
    struct timeval b;
    struct timeval c;
    
    INIT_TV(a, 10, 100);
    INIT_TV(b, 2, 20);
    timeval_subtract(&c, &a, &b);
    ASSERT_TV_VALID(c, 8, 80);

    INIT_TV(a, 100, 1);
    INIT_TV(b, 1, 20);
    timeval_subtract(&c, &a, &b);
    ASSERT_TV_VALID(c, 98, 999981);
}


void test_ms_timeval(void)
{
    struct timeval a;
    timeval_from_ms(&a, 1000);
    ASSERT_TV_VALID(a, 1, 0);

    timeval_from_ms(&a, 1);
    ASSERT_TV_VALID(a, 0, 1000);

    timeval_from_ms(&a, 99999);
    ASSERT_TV_VALID(a, 99, 999000);
}


void test_compare_timeval(void)
{
    struct timeval a;
    struct timeval b;
    int rc;


    INIT_TV(a, 10, 0);
    INIT_TV(b, 5, 0);
    rc = timeval_compare(&a, &b);
    assert(rc == 1);

    INIT_TV(a, 10, 0);
    INIT_TV(b, 50, 0);
    rc = timeval_compare(&a, &b);
    assert(rc == -1);

    INIT_TV(a, 44, 0);
    INIT_TV(b, 44, 0);
    rc = timeval_compare(&a, &b);
    assert(rc == 0);


    INIT_TV(a, 5, 30);
    INIT_TV(b, 5, 29);
    rc = timeval_compare(&a, &b);
    assert(rc == 1);

    INIT_TV(a, 5, 16);
    INIT_TV(b, 5, 61);
    rc = timeval_compare(&a, &b);
    assert(rc == -1);

    INIT_TV(a, 5, 777);
    INIT_TV(b, 5, 777);
    rc = timeval_compare(&a, &b);
    assert(rc == 0);


    INIT_TV(a, 6, 0);
    INIT_TV(b, 5, 999999);
    rc = timeval_compare(&a, &b);
    assert(rc == 1);

    INIT_TV(a, 5, 999999);
    INIT_TV(b, 6, 0);
    rc = timeval_compare(&a, &b);
    assert(rc == -1);

    INIT_TV(a, 44, 999999);
    INIT_TV(b, 44, 999999);
    rc = timeval_compare(&a, &b);
    assert(rc == 0);
}


void test_normalize_timeval()
{
    struct timeval a;

    INIT_TV(a, 11, 123456);
    timeval_normalize(&a);
    ASSERT_TV_VALID(a, 11, 123456);

    INIT_TV(a, 10, 1000000);
    timeval_normalize(&a);
    ASSERT_TV_VALID(a, 11, 0);
    
    INIT_TV(a, 10, 2147483647);
    timeval_normalize(&a);
    ASSERT_TV_VALID(a, 2157, 483647);
    
    INIT_TV(a, 10, -1);
    timeval_normalize(&a);
    ASSERT_TV_VALID(a, 9, 999999);

    INIT_TV(a, 10, -999999);
    timeval_normalize(&a);
    ASSERT_TV_VALID(a, 9, 1);

    INIT_TV(a, 3000, -2147483647);
    timeval_normalize(&a);
    ASSERT_TV_VALID(a, 852, 516353);
}


int main (void)
{
    printf("Unit testing C based time utilities\n");

    test_add_timespec();
    test_subtract_timespec();
    test_ms_timespec();
    test_compare_timespec();
    test_normalize_timespec();

    test_add_timeval();
    test_subtract_timeval();
    test_ms_timeval();
    test_compare_timeval();
    test_normalize_timeval();
    
    printf("Passed\n");
    return 0;
}

