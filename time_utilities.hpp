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

#ifdef USING_TIMEVAL
#include <sys/time.h>
#endif


/**
 *  Various time conversions.
 */
#define NS_IN_SECOND    (1000000000L)
#define US_IN_SECOND    (1000000L)
#define MS_IN_SECOND    (1000L)
#define NS_IN_MS        (1000L * 1000L)
#define US_IN_MS        (1000L)


/**
 *  Wrapper class for working with c style timespec structures.
 */
class CTimeSpec 
{
    public:

        /**
         *  Default null ctor.
         */
        CTimeSpec() 
        : ts {0, 0}
        {}

        /**
         *  ctor 
         *  @param ms total milliseconds used to init the class.
         */
        CTimeSpec(unsigned int ms) 
        : ts {ms / MS_IN_SECOND, (ms % MS_IN_SECOND) * NS_IN_MS}
        {}

        /**
         *  ctor
         *  @param t timespec structure used to init the class.
         *  This ctor guarantees that the structure is normalized.
         */
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

        /**
         *  ctor
         *  @param sec seconds used to init the sec portion.
         *  @param nsec nseconds used to init the nsec portion.
         *  This ctor guarantees that the structure is normalized.
         */
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

#ifdef USING_TIMEVAL
        /**
         *  ctor - Create a CTimeSpec from a timeval struct.
         *  @param tv timeval structure used to init the class.
         *  This ctor guarantees that the structure is normalized.
         */
        CTimeSpec(struct timeval tv)
        {
            ts.tv_sec = tv.tv_sec;
            ts.tv_nsec = tv.tv_usec * 1000;
            
            while (ts.tv_nsec >= NS_IN_SECOND) {
                ts.tv_sec++;
                ts.tv_nsec -= NS_IN_SECOND;
            }
            while (ts.tv_nsec < 0) {
                ts.tv_sec--;
                ts.tv_nsec += NS_IN_SECOND;
            }
        }
#endif

        /**
         *  Static factory returning a CTimeSpec that represents "now"
         *  in wall clock time. See CLOCK_REALTIME.
         */
        static CTimeSpec Now()
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            return CTimeSpec {ts};
        }

        /**
         *  Static factory returning a CTimeSpec that represents "now"
         *  in monotonic time. See CLOCK_MONOTONIC.
         */
        static CTimeSpec NowMonotonic()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return CTimeSpec {ts};
        }

        /**
         *  Static factory returning a CTimeSpec that represents "now"
         *  in monotonic raw time. See CLOCK_MONOTONIC_RAW.
         */
        static CTimeSpec NowMonotonicRaw()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            return CTimeSpec {ts};
        }

        /**
         *  Utility function to return a copy of the internal 
         *  timespec structure.
         */
        struct timespec c_timespec()
        {
            return ts;
        }

        /**
         *  Adds a CTimeSpec to this one. 
         *  Guarantees the result is normalized.
         */
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

        /**
         *  Subtracts a CTimeSpec to this one. 
         *  Guarantees the result is normalized.
         */
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

        /**
         *  Compares a CTimeSpecs against this one 
         *  for non-equality.
         */
        bool operator!=(const CTimeSpec& rhs) const
        {
            if (ts.tv_sec != rhs.ts.tv_sec)
                return true;
            else if (ts.tv_nsec != rhs.ts.tv_nsec)
                return true;
            else 
                return false;
        }

        /**
         *  Compares a CTimeSpecs against this one 
         *  for equality.
         */
        bool operator==(const CTimeSpec& rhs) const
        {
            return !(*this != rhs);
        }

        /**
         *  Compares a CTimeSpecs against this one 
         *  for less than.
         */
        bool operator<(const CTimeSpec& rhs) const 
        {
            if (ts.tv_sec < rhs.ts.tv_sec)
                return true;
            else if (ts.tv_nsec < rhs.ts.tv_nsec)
                return true;
            else 
                return false;
        }
        
        /**
         *  Compares a CTimeSpecs against this one 
         *  for greater than.
         */
        bool operator>(const CTimeSpec& rhs) const 
        {
            if (ts.tv_sec > rhs.ts.tv_sec)
                return true;
            else if (ts.tv_nsec > rhs.ts.tv_nsec)
                return true;
            else 
                return false;
        }

        /**
         *  Compares a CTimeSpecs against this one 
         *  for less than or equals to.
         */
        bool operator<=(const CTimeSpec& rhs) const
        {
            if (*this < rhs)
                return true;
            else if (*this == rhs)
                return true;
            else 
                return false;
        }

        /**
         *  Compares a CTimeSpecs against this one 
         *  for greater than or equals to.
         */
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
        /**
         *  The internal data struct this class is wrapping.
         */
        struct timespec ts;

    /**
     *  Output operator for std::ostreams.
     */
    friend std::ostream& operator<< (std::ostream& os, const CTimeSpec& ts)
    {
        os << "(" << ts.ts.tv_sec << " sec, " << ts.ts.tv_nsec << " nsec)";
        return os;
    }

    /**
     *  Adds two CTimeSpecs and returns a new one which is the sum.
     */
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

    /**
     *  Subtracts two CTimeSpecs and returns a new one which is the 
     *  difference.
     */
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


/*
 *  The struct timeval structure is not part of POSIX, however it is 
 *  used a lot in Linux / BSD / *nix code, and the structure is very 
 *  similar to the timespec structure. Define USING_TIMEVAL to get the 
 *  corresponding APIs.
 */
#ifdef USING_TIMEVAL


/**
 *  Wrapper class for working with c style timeval structures.
 */
class CTimeVal 
{
    public:

        /**
         *  Default null ctor.
         */
        CTimeVal() 
        : tv {0, 0}
        {}

        /**
         *  ctor 
         *  @param ms total milliseconds used to init the class.
         */
        CTimeVal(unsigned int ms) 
        : tv {ms / MS_IN_SECOND, (ms % MS_IN_SECOND) * US_IN_MS}
        {}

        /**
         *  ctor
         *  @param t timeval structure used to init the class.
         *  This ctor guarantees that the structure is normalized.
         */
        CTimeVal(struct timeval t)
        {
            while (t.tv_usec >= US_IN_SECOND) {
                t.tv_sec++;
                t.tv_usec -= US_IN_SECOND;
            }
            while (t.tv_usec < 0) {
                t.tv_sec--;
                t.tv_usec += US_IN_SECOND;
            }

            tv = t;
        }

        /**
         *  ctor
         *  @param sec seconds used to init the sec portion.
         *  @param usec microseconds used to init the usec portion.
         *  This ctor guarantees that the structure is normalized.
         */
        CTimeVal(time_t sec, long usec) 
        {
            while (usec >= US_IN_SECOND) {
                sec++;
                usec -= US_IN_SECOND;
            }
            while (usec < 0) {
                sec--;
                usec += US_IN_SECOND;
            }

            tv.tv_sec = sec;
            tv.tv_usec = usec;
        }

        /**
         *  ctor - create a CTimeVal class from a timespec struct.
         *  @param t timeval structure used to init the class.
         *  This ctor guarantees that the structure is normalized.
         */
        CTimeVal(struct timespec ts)
        {
            tv.tv_sec = ts.tv_sec;
            tv.tv_usec = ts.tv_nsec / 1000;
            
            while (tv.tv_usec >= US_IN_SECOND) {
                tv.tv_sec++;
                tv.tv_usec -= US_IN_SECOND;
            }
            while (tv.tv_usec < 0) {
                tv.tv_sec--;
                tv.tv_usec += US_IN_SECOND;
            }
        }

        /**
         *  Static factory returning a CTimeVal that represents "now"
         *  in wall clock time. See CLOCK_REALTIME.
         */
        static CTimeVal Now()
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            return CTimeVal {ts};
        }

        /**
         *  Static factory returning a CTimeVal that represents "now"
         *  in monotonic time. See CLOCK_MONOTONIC.
         */
        static CTimeVal NowMonotonic()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return CTimeVal {ts};
        }

        /**
         *  Static factory returning a CTimeVal that represents "now"
         *  in monotonic raw time. See CLOCK_MONOTONIC_RAW.
         */
        static CTimeVal NowMonotonicRaw()
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            return CTimeVal {ts};
        }

        /**
         *  Utility function to return a copy of the internal 
         *  timeval structure.
         */
        struct timeval c_timeval()
        {
            return tv;
        }

        /**
         *  Adds a CTimeVal to this one. 
         *  Guarantees the result is normalized.
         */
        CTimeVal& operator+=(const CTimeVal& rhs)
        {
            tv.tv_sec += rhs.tv.tv_sec;
            tv.tv_usec += rhs.tv.tv_usec;
            if (tv.tv_usec >= US_IN_SECOND){
                tv.tv_sec++;
                tv.tv_usec -= US_IN_SECOND;
            }
            return *this;
        }

        /**
         *  Subtracts a CTimeVal to this one. 
         *  Guarantees the result is normalized.
         */
        CTimeVal& operator-=(const CTimeVal& rhs)
        {
            tv.tv_sec -= rhs.tv.tv_sec;
            tv.tv_usec -= rhs.tv.tv_usec;
            if (tv.tv_usec < 0){
                tv.tv_sec--;
                tv.tv_usec += US_IN_SECOND;
            }
            return *this;
        }

        /**
         *  Compares a CTimeVals against this one 
         *  for non-equality.
         */
        bool operator!=(const CTimeVal& rhs) const
        {
            if (tv.tv_sec != rhs.tv.tv_sec)
                return true;
            else if (tv.tv_usec != rhs.tv.tv_usec)
                return true;
            else 
                return false;
        }

        /**
         *  Compares a CTimeVals against this one 
         *  for equality.
         */
        bool operator==(const CTimeVal& rhs) const
        {
            return !(*this != rhs);
        }

        /**
         *  Compares a CTimeVals against this one 
         *  for less than.
         */
        bool operator<(const CTimeVal& rhs) const 
        {
            if (tv.tv_sec < rhs.tv.tv_sec)
                return true;
            else if (tv.tv_usec < rhs.tv.tv_usec)
                return true;
            else 
                return false;
        }
        
        /**
         *  Compares a CTimeVals against this one 
         *  for greater than.
         */
        bool operator>(const CTimeVal& rhs) const 
        {
            if (tv.tv_sec > rhs.tv.tv_sec)
                return true;
            else if (tv.tv_usec > rhs.tv.tv_usec)
                return true;
            else 
                return false;
        }

        /**
         *  Compares a CTimeVals against this one 
         *  for less than or equals to.
         */
        bool operator<=(const CTimeVal& rhs) const
        {
            if (*this < rhs)
                return true;
            else if (*this == rhs)
                return true;
            else 
                return false;
        }

        /**
         *  Compares a CTimeVals against this one 
         *  for greater than or equals to.
         */
        bool operator>=(const CTimeVal& rhs) const
        {
            if (*this > rhs)
                return true;
            else if (*this == rhs)
                return true;
            else 
                return false;
        }

    private:
        /**
         *  The internal data struct this class is wrapping.
         */
        struct timeval tv;

    /**
     *  Output operator for std::ostreams.
     */
    friend std::ostream& operator<< (std::ostream& os, const CTimeVal& tv)
    {
        os << "(" << tv.tv.tv_sec << " sec, " << tv.tv.tv_usec << " usec)";
        return os;
    }

    /**
     *  Adds two CTimeVals and returns a new one which is the sum.
     */
    friend CTimeVal operator+ (const CTimeVal& lhs, const CTimeVal& rhs)
    {
        struct timeval sum;
        sum.tv_sec = lhs.tv.tv_sec + rhs.tv.tv_sec;
        sum.tv_usec = lhs.tv.tv_usec + rhs.tv.tv_usec;
        if (sum.tv_usec >= US_IN_SECOND){
            sum.tv_sec++;
            sum.tv_usec -= US_IN_SECOND;
        }
        
        return CTimeVal(sum);
    }

    /**
     *  Subtracts two CTimeVals and returns a new one which is the 
     *  difference.
     */
    friend CTimeVal operator- (const CTimeVal& lhs, const CTimeVal& rhs)
    {
        struct timeval difference;
        difference.tv_sec = lhs.tv.tv_sec - rhs.tv.tv_sec;
        difference.tv_usec = lhs.tv.tv_usec - rhs.tv.tv_usec;
        if (difference.tv_usec < 0){
            difference.tv_sec--;
            difference.tv_usec += US_IN_SECOND;
        }
        
        return CTimeVal(difference);
    }
};


#endif /* USING_TIMEVAL */


#endif


