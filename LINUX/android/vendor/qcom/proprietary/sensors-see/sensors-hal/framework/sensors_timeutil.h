/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <cstdint>

/* This is platform specific code, so make sure that we are compiling
   for correct architecture */
#if defined(__aarch64__)
#define TARGET_ARM64
#elif defined(__arm__)
#define TARGET_ARM
#else
#error "target cpu architecture not supported"
#endif

#include "utils/SystemClock.h"

#include "sensors_log.h"

/**
 * @brief Sensors time utilities
 *
 * Provides utility functions for
 *  - reading QTimer ticks/frequency and timestamps
 *  - converting QTimer timestamps to android system clock
 *    timestamps and vice versa.
 */
class sensors_timeutil
{
public:

    /**
     * @brief get singleton instance of the timeutil class
     * @return sensors_timeutil&
     */
    static sensors_timeutil& get_instance()
    {
        static sensors_timeutil inst;
        return inst;
    }

    /**
     * @brief reads the current QTimer count value
     * @return uint64_t QTimer tick-count
     */
    uint64_t qtimer_get_ticks()
    {
    #if defined(TARGET_ARM64)
        unsigned long long val = 0;
        asm volatile("mrs %0, cntvct_el0" : "=r" (val));
        return val;
    #else
        uint64_t val;
        unsigned long lsb = 0, msb = 0;
        asm volatile("mrrc p15, 1, %[lsb], %[msb], c14"
                     : [lsb] "=r" (lsb), [msb] "=r" (msb));
        val = ((uint64_t)msb << 32) | lsb;
        return val;
    #endif
    }

    /**
     * @brief get QTimer frequency in Hz
     * @return uint64_t
     */
    uint64_t qtimer_get_freq()
    {
    #if defined(TARGET_ARM64)
        uint64_t val = 0;
        asm volatile("mrs %0, cntfrq_el0" : "=r" (val));
        return val;
    #else
        uint32_t val = 0;
        asm volatile("mrc p15, 0, %[val], c14, c0, 0" : [val] "=r" (val));
        return val;
    #endif
    }

    /**
     * @brief get time in nanoseconds since boot-up
     * @return uint64_t nanoseconds
     */
    uint64_t qtimer_get_time_ns()
    {
        return qtimer_ticks_to_ns(qtimer_get_ticks());
    }

    /**
     * @brief convert the qtimer tick value of nanoseconds
     * @param ticks
     * @return uint64_t qtimer time in nanoseconds
     */
    uint64_t qtimer_ticks_to_ns(uint64_t ticks)
    {
        return uint64_t(double(ticks) * (double(NSEC_PER_SEC) / double(_qtimer_freq)));
    }

    /**
     * @brief convert qtimer timestamp (in ns) to android system
     *        time (in ns)
     * @param qtimer_ts
     * @return int64_t
     */
    int64_t qtimer_ns_to_elapsedRealtimeNano(uint64_t qtimer_ts)
    {
        return (int64_t(qtimer_ts) + _offset_ns);
    }

    /**
     * @brief convert android system time (in ns) to qtimer
     *        timestamp (in ns)
     * @param android_ts
     * @return uint64_t
     */
    uint64_t elapsedRealtimeNano_to_qtimer_ns(int64_t android_ts)
    {
        return (android_ts - _offset_ns);
    }

    /**
     * @brief convert qtimer timestamp (in ticks) to android
     *        timestamp (in ns)
     * @param qtimer_ticks
     * @return int64_t
     */
    int64_t qtimer_ticks_to_elapsedRealtimeNano(uint64_t qtimer_ticks)
    {
        return qtimer_ns_to_elapsedRealtimeNano(qtimer_ticks_to_ns(qtimer_ticks));
    }
    /**
     * @brief get offset between sensor time system and Android time system (in ns)
     * @return uint64_t
     */
    uint64_t getElapsedRealtimeNanoOffset()
    {
        return _offset_ns;
    }

private:
    sensors_timeutil() :
        _qtimer_freq(qtimer_get_freq()),
        _offset_ns(android::elapsedRealtimeNano() - int64_t(qtimer_get_time_ns()))
    {
    }
    ~sensors_timeutil() = default;
    /* delete copy constructors/assignment for singleton operation */
    sensors_timeutil(const sensors_timeutil&) = delete;
    sensors_timeutil& operator=(const sensors_timeutil&) = delete;

    /* QTimer frequency in Hz */
    const uint64_t _qtimer_freq;

    /* constant offset between the android elapsedRealTimeNano clock
       and QTimer clock in nanoseconds */
    // PEND: Move this variable back into the private section (see PEND below)
//    const int64_t _offset_ns;

    const uint64_t NSEC_PER_SEC = 1000000000ull;

public:
    // For some unknown reason, the Android and QTimer timestamps used to
    // calculate the _offset_ns only make sense if _offset_ns is a public
    // variable. I suspect that our method for reading the QTimer value may
    // have some unaccounted caveats.
    // PEND: Make this variable private again once we fix the root-cause for
    // the invalid QTimer values read during bootup
    const int64_t _offset_ns;
};
