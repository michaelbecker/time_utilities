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

#define ASSERT_TS_VALID(x_, sec_, nsec_) \
    assert(x_.tv_sec == sec_);  \
    assert(x_.tv_nsec == nsec_);

#define ASSERT_CTS_VALID(X_, sec_, nsec_) \
{\
    struct timespec X_ ## tmp_ = X_.c_timespec(); \
    assert(X_ ## tmp_.tv_sec == sec_);  \
    assert(X_ ## tmp_.tv_nsec == nsec_); \
}


void TestCtors()
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


void TestCoutOperator()
{
    struct timespec a {12, 13};
    CTimeSpec A {a};
    PRINT_TS(a);
    std::cout << A << std::endl;
}


void TestAdd()
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


void TestSubtract()
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


void TestCompare()
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


int main()
{
    std::cout << "Unit testing C++ based time utilities" << std::endl;
    
    TestCtors();
    TestCoutOperator();
    TestAdd();
    TestSubtract();
    TestCompare();

    std::cout << "passed" << std::endl;
    return 0;
}



