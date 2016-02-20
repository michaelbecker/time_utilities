/**
 *  @file
 *
 *  Utility classes around timespec and timeval data structures.
 *  This approach attempts to keep time data as a timeval (or timespec)
 *  data structure. This keeps the data in the most precise format 
 *  available. Converting to an integer, long, or double will 
 *  lose precision, and by using the functions in this file simply
 *  isn't needed.
 *
 *  This header requires C++11 support.
 *
 *  Naming convention is Pascal case, with the dreaded "C" prefix 
 *  in front of classes, mostly because it's a more suscint way to 
 *  denote that this is a wrapper class around already existing 
 *  structures.
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
#ifndef TIME_UTILITIES_HPP__
#define TIME_UTILITIES_HPP__

#include <iostream>
#include <ctime>


/**
 *  Various time conversions.
 */
#define NS_IN_SECOND    (1000000000L)
#define US_IN_SECOND    (1000000L)
#define MS_IN_SECOND    (1000L)
#define NS_IN_MS        (1000L * 1000L)
#define US_IN_MS        (1000L)


class CTimeSpec 
{
    public:

        CTimeSpec() 
        : ts {0, 0}
        {}

        CTimeSpec(unsigned int ms) 
        : ts {ms / MS_IN_SECOND, (ms % MS_IN_SECOND) * NS_IN_MS}
        {}

        CTimeSpec(struct timespec t)
        {
            while (t.tv_nsec >= NS_IN_SECOND) {
                t.tv_sec++;
                t.tv_nsec -= NS_IN_SECOND;
            }
            while (t.tv_nsec < 0) {
                t.tv_sec--;
                t.tv_nsec += NS_IN_SECOND;
            }

            ts = t;
        }

        CTimeSpec(time_t sec, long nsec) 
        {
            while (nsec >= NS_IN_SECOND) {
                sec++;
                nsec -= NS_IN_SECOND;
            }
            while (nsec < 0) {
                sec--;
                nsec += NS_IN_SECOND;
            }

            ts.tv_sec = sec;
            ts.tv_nsec = nsec;
        }

        static CTimeSpec Now()
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            return CTimeSpec {ts};
        }

        static CTimeSpec NowMonotonic()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return CTimeSpec {ts};
        }

        static CTimeSpec NowMonotonicRaw()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            return CTimeSpec {ts};
        }

        struct timespec c_timespec()
        {
            return ts;
        }

        CTimeSpec& operator+=(const CTimeSpec& rhs)
        {
            ts.tv_sec += rhs.ts.tv_sec;
            ts.tv_nsec += rhs.ts.tv_nsec;
            if (ts.tv_nsec >= NS_IN_SECOND){
                ts.tv_sec++;
                ts.tv_nsec -= NS_IN_SECOND;
            }
            return *this;
        }

        CTimeSpec& operator-=(const CTimeSpec& rhs)
        {
            ts.tv_sec -= rhs.ts.tv_sec;
            ts.tv_nsec -= rhs.ts.tv_nsec;
            if (ts.tv_nsec < 0){
                ts.tv_sec--;
                ts.tv_nsec += NS_IN_SECOND;
            }
            return *this;
        }

        bool operator!=(const CTimeSpec& rhs) const
        {
            if (ts.tv_sec != rhs.ts.tv_sec)
                return true;
            else if (ts.tv_nsec != rhs.ts.tv_nsec)
                return true;
            else 
                return false;
        }

        bool operator==(const CTimeSpec& rhs) const
        {
            return !(*this != rhs);
        }

        bool operator<(const CTimeSpec& rhs) const 
        {
            if (ts.tv_sec < rhs.ts.tv_sec)
                return true;
            else if (ts.tv_nsec < rhs.ts.tv_nsec)
                return true;
            else 
                return false;
        }
        
        bool operator>(const CTimeSpec& rhs) const 
        {
            if (ts.tv_sec > rhs.ts.tv_sec)
                return true;
            else if (ts.tv_nsec > rhs.ts.tv_nsec)
                return true;
            else 
                return false;
        }

        bool operator<=(const CTimeSpec& rhs) const
        {
            if (*this < rhs)
                return true;
            else if (*this == rhs)
                return true;
            else 
                return false;
        }

        bool operator>=(const CTimeSpec& rhs) const
        {
            if (*this > rhs)
                return true;
            else if (*this == rhs)
                return true;
            else 
                return false;
        }

    private:
        struct timespec ts;

    ///
    ///  Operators
    ///
    friend std::ostream& operator<< (std::ostream& os, const CTimeSpec& ts)
    {
        os << "(" << ts.ts.tv_sec << " sec, " << ts.ts.tv_nsec << " nsec)";
        return os;
    }

    friend CTimeSpec operator+ (const CTimeSpec& lhs, const CTimeSpec& rhs)
    {
        struct timespec sum;
        sum.tv_sec = lhs.ts.tv_sec + rhs.ts.tv_sec;
        sum.tv_nsec = lhs.ts.tv_nsec + rhs.ts.tv_nsec;
        if (sum.tv_nsec >= NS_IN_SECOND){
            sum.tv_sec++;
            sum.tv_nsec -= NS_IN_SECOND;
        }
        
        return CTimeSpec(sum);
    }

    friend CTimeSpec operator- (const CTimeSpec& lhs, const CTimeSpec& rhs)
    {
        struct timespec difference;
        difference.tv_sec = lhs.ts.tv_sec - rhs.ts.tv_sec;
        difference.tv_nsec = lhs.ts.tv_nsec - rhs.ts.tv_nsec;
        if (difference.tv_nsec < 0){
            difference.tv_sec--;
            difference.tv_nsec += NS_IN_SECOND;
        }
        
        return CTimeSpec(difference);
    }
};


#endif


