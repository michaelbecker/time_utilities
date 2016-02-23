/**
 *  @file
 *
 *  Unit test code of time_utilities.hpp
 *
 *  To compile:
 *  g++ -Wall -std=c++11 unit_test_time_utilities.cpp -o unit_test_time_utilities
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
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <sys/time.h>

#define USING_TIMEVAL
#include "time_utilities.hpp"


#define PRINT_TS(x_) \
    std::cout   << #x_<< ".tv_sec = " << x_.tv_sec << " " \
                << #x_<< ".tv_nsec = " << x_.tv_nsec \
                << std::endl;

#define INIT_TS(x_, sec_, nsec_) \
    x_.tv_sec = sec_; \
    x_.tv_nsec = nsec_;

#define ASSERT_CTS_VALID(X_, sec_, nsec_) \
{\
    struct timespec X_ ## tmp_ = X_.c_timespec(); \
    assert(X_ ## tmp_.tv_sec == sec_);  \
    assert(X_ ## tmp_.tv_nsec == nsec_); \
}


void TestCtorsCTimeSpec()
{
    struct timespec a;

    INIT_TS(a, 12, 13);
    CTimeSpec A {a};
    ASSERT_CTS_VALID(A, 12, 13);
    
    CTimeSpec AA {14, 15};
    ASSERT_CTS_VALID(AA, 14, 15);
    
    CTimeSpec B {1000};
    ASSERT_CTS_VALID(B, 1, 0);

    CTimeSpec C {1};
    ASSERT_CTS_VALID(C, 0, 1000000);

    CTimeSpec D {99999};
    ASSERT_CTS_VALID(D, 99, 999000000);

    INIT_TS(a, 11, 123456789);
    CTimeSpec E {a};
    ASSERT_CTS_VALID(E, 11, 123456789);

    INIT_TS(a, 10, 1000000000);
    CTimeSpec F {a};
    ASSERT_CTS_VALID(F, 11, 0);

    INIT_TS(a, 10, 2147483647);
    CTimeSpec G {a};
    ASSERT_CTS_VALID(G, 12, 147483647);

    INIT_TS(a, 10, -1);
    CTimeSpec H {a};
    ASSERT_CTS_VALID(H, 9, 999999999);

    INIT_TS(a, 10, -999999999);
    CTimeSpec I {a};
    ASSERT_CTS_VALID(I, 9, 1);

    INIT_TS(a, 10, -2147483647);
    CTimeSpec J {a};
    ASSERT_CTS_VALID(J, 7, 852516353);
}


void TestCoutOperatorCTimeSpec()
{
    struct timespec a {12, 13};
    CTimeSpec A {a};
    PRINT_TS(a);
    std::cout << A << std::endl;
}


void TestAddCTimeSpec()
{
    CTimeSpec A, B, C;

    A = {1, 10};
    B = {2, 20};
    C = A + B;
    ASSERT_CTS_VALID(C, 3, 30);

    A = {1, 999999999};
    B = {1, 2};
    C = A + B;
    ASSERT_CTS_VALID(C, 3, 1);

    A = {1, 999999999};
    C = A + A;
    ASSERT_CTS_VALID(C, 3, 999999998);

    A = {1, 10};
    B = {2, 20};
    A += B;
    ASSERT_CTS_VALID(A, 3, 30);

    A = {1, 999999999};
    B = {1, 2};
    A += B;
    ASSERT_CTS_VALID(A, 3, 1);

    A = {1, 999999999};
    A += A;
    ASSERT_CTS_VALID(A, 3, 999999998);
}


void TestSubtractCTimeSpec()
{
    CTimeSpec A, B, C;

    A = {10, 100};
    B = {2, 20};
    C = A - B;
    ASSERT_CTS_VALID(C, 8, 80);

    A = {100, 1};
    B = {1, 20};
    C = A - B;
    ASSERT_CTS_VALID(C, 98, 999999981);

    A = {10, 100};
    B = {2, 20};
    A -= B;
    ASSERT_CTS_VALID(A, 8, 80);

    A = {100, 1};
    B = {1, 20};
    A -= B;
    ASSERT_CTS_VALID(A, 98, 999999981);
}


void TestCompareCTimeSpec()
{
    CTimeSpec A, B;

    A = {10, 0};
    B = {5, 0};
    assert(A > B);
    assert(A != B);
    assert(A >= B);

    A = {10, 0};
    B = {50, 0};
    assert(A < B);
    assert(A != B);
    assert(A <= B);

    A = {44, 0};
    B = {44, 0};
    assert(A == B);
    assert(A <= B);
    assert(A >= B);

    A = {5, 30};
    B = {5, 29};
    assert(A > B);
    assert(A != B);
    assert(A >= B);

    A = {5, 16};
    B = {5, 61};
    assert(A < B);
    assert(A != B);
    assert(A <= B);

    A = {5, 777};
    B = {5, 777};
    assert(A == B);
    assert(A <= B);
    assert(A >= B);
}


#define PRINT_TV(x_) \
    std::cout   << #x_<< ".tv_sec = " << x_.tv_sec << " " \
                << #x_<< ".tv_usec = " << x_.tv_usec \
                << std::endl;

#define INIT_TV(x_, sec_, usec_) \
    x_.tv_sec = sec_; \
    x_.tv_usec = usec_;

#define ASSERT_CTV_VALID(X_, sec_, usec_) \
{\
    struct timeval X_ ## tmp_ = X_.c_timeval(); \
    assert(X_ ## tmp_.tv_sec == sec_);  \
    assert(X_ ## tmp_.tv_usec == usec_); \
}


void TestCtorsCTimeVal()
{
    struct timeval a;

    INIT_TV(a, 12, 13);
    CTimeVal A {a};
    ASSERT_CTV_VALID(A, 12, 13);
    
    CTimeVal AA {14, 15};
    ASSERT_CTV_VALID(AA, 14, 15);
    
    CTimeVal B {1000};
    ASSERT_CTV_VALID(B, 1, 0);

    CTimeVal C {1};
    ASSERT_CTV_VALID(C, 0, 1000);

    CTimeVal D {99999};
    ASSERT_CTV_VALID(D, 99, 999000);

    INIT_TV(a, 11, 123456);
    CTimeVal E {a};
    ASSERT_CTV_VALID(E, 11, 123456);

    INIT_TV(a, 10, 1000000);
    CTimeVal F {a};
    ASSERT_CTV_VALID(F, 11, 0);

    INIT_TV(a, 10, 2147483647);
    CTimeVal G {a};
    ASSERT_CTV_VALID(G, 2157, 483647);

    INIT_TV(a, 10, -1);
    CTimeVal H {a};
    ASSERT_CTV_VALID(H, 9, 999999);

    INIT_TV(a, 10, -999999);
    CTimeVal I {a};
    ASSERT_CTV_VALID(I, 9, 1);

    INIT_TV(a, 3000, -2147483647);
    CTimeVal J {a};
    ASSERT_CTV_VALID(J, 852, 516353);
}


void TestCoutOperatorCTimeVal()
{
    struct timeval a {12, 13};
    CTimeVal A {a};
    PRINT_TV(a);
    std::cout << A << std::endl;
}


void TestAddCTimeVal()
{
    CTimeVal A, B, C;

    A = {1, 10};
    B = {2, 20};
    C = A + B;
    ASSERT_CTV_VALID(C, 3, 30);

    A = {1, 999999};
    B = {1, 2};
    C = A + B;
    ASSERT_CTV_VALID(C, 3, 1);

    A = {1, 999999};
    C = A + A;
    ASSERT_CTV_VALID(C, 3, 999998);

    A = {1, 10};
    B = {2, 20};
    A += B;
    ASSERT_CTV_VALID(A, 3, 30);

    A = {1, 999999};
    B = {1, 2};
    A += B;
    ASSERT_CTV_VALID(A, 3, 1);

    A = {1, 999999};
    A += A;
    ASSERT_CTV_VALID(A, 3, 999998);
}


void TestSubtractCTimeVal()
{
    CTimeVal A, B, C;

    A = {10, 100};
    B = {2, 20};
    C = A - B;
    ASSERT_CTV_VALID(C, 8, 80);

    A = {100, 1};
    B = {1, 20};
    C = A - B;
    ASSERT_CTV_VALID(C, 98, 999981);

    A = {10, 100};
    B = {2, 20};
    A -= B;
    ASSERT_CTV_VALID(A, 8, 80);

    A = {100, 1};
    B = {1, 20};
    A -= B;
    ASSERT_CTV_VALID(A, 98, 999981);
}


void TestCompareCTimeVal()
{
    CTimeVal A, B;

    A = {10, 0};
    B = {5, 0};
    assert(A > B);
    assert(A != B);
    assert(A >= B);

    A = {10, 0};
    B = {50, 0};
    assert(A < B);
    assert(A != B);
    assert(A <= B);

    A = {44, 0};
    B = {44, 0};
    assert(A == B);
    assert(A <= B);
    assert(A >= B);

    A = {5, 30};
    B = {5, 29};
    assert(A > B);
    assert(A != B);
    assert(A >= B);

    A = {5, 16};
    B = {5, 61};
    assert(A < B);
    assert(A != B);
    assert(A <= B);

    A = {5, 777};
    B = {5, 777};
    assert(A == B);
    assert(A <= B);
    assert(A >= B);
}


void TestConversions()
{
    struct timeval a;
    INIT_TV(a, 33, 44);
    CTimeSpec A1 {a};
    ASSERT_CTS_VALID(A1, 33, 44000);

    INIT_TV(a, 33, 1000000);
    CTimeSpec A2 {a};
    ASSERT_CTS_VALID(A2, 34, 0);

    INIT_TV(a, 33, -1);
    CTimeSpec A3 {a};
    ASSERT_CTS_VALID(A3, 32, 999999000);

    
    struct timespec b;
    INIT_TS(b, 33, 999);
    CTimeVal B1 {b};
    ASSERT_CTV_VALID(B1, 33, 0);

    INIT_TS(b, 12, 1000);
    CTimeVal B2 {b};
    ASSERT_CTV_VALID(B2, 12, 1);

    INIT_TS(b, 12, 999999999);
    CTimeVal B3 {b};
    ASSERT_CTV_VALID(B3, 12, 999999);

    INIT_TS(b, 12, 1999999999);
    CTimeVal B4 {b};
    ASSERT_CTV_VALID(B4, 13, 999999);

    INIT_TS(b, 33, -999);
    CTimeVal B5 {b};
    ASSERT_CTV_VALID(B5, 33, 0);

    INIT_TS(b, 12, -1000);
    CTimeVal B6 {b};
    ASSERT_CTV_VALID(B6, 11, 999999);
}


int main()
{
    std::cout << "Unit testing C++ based time utilities" << std::endl;
    
    TestCtorsCTimeSpec();
    TestCoutOperatorCTimeSpec();
    TestAddCTimeSpec();
    TestSubtractCTimeSpec();
    TestCompareCTimeSpec();

    TestCtorsCTimeVal();
    TestCoutOperatorCTimeVal();
    TestAddCTimeVal();
    TestSubtractCTimeVal();
    TestCompareCTimeVal();

    TestConversions();

    std::cout << "passed" << std::endl;
    return 0;
}



