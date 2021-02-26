/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.metrics;

import android.os.SystemClock;

import java.util.Locale;
import java.util.Objects;
import java.util.concurrent.TimeUnit;

/**
 * An {@link EventCounter} which tracks the frequency of events, not just that they occurred.
 */
public class EventFrequencyCounter extends EventCounter {
    public static final long DEFAULT_REPORTING_PERIOD = 1;
    public static final TimeUnit DEFAULT_REPORTING_PERIOD_TIME_UNIT = TimeUnit.SECONDS;
    public static final long DEFAULT_SAMPLING_PERIOD = 1;
    public static final TimeUnit DEFAULT_SAMPLING_PERIOD_TIME_UNIT = TimeUnit.SECONDS;

    private final long reportingPeriodNs;
    private final long samplingPeriodNs;
    private final long eventTimestamps[];
    private int eventTimestampsHead;

    /**
     * Construct a new {@link EventFrequencyCounter}, with a polling period of
     * {@link #DEFAULT_POLL_PERIOD}, a reporting period of {@link #DEFAULT_REPORTING_PERIOD}, and
     * a sampling period of {@link #DEFAULT_SAMPLING_PERIOD}.
     * @see EventFrequencyCounter#EventFrequencyCounter(String, long, TimeUnit, long, TimeUnit,
     *      long, TimeUnit, int)
     */
    public EventFrequencyCounter(String name, int maxEventsPerSamplingPeriod) {
        this(name, EventCounter.DEFAULT_POLL_PERIOD, EventCounter.DEFAULT_POLL_PERIOD_TIME_UNIT,
                maxEventsPerSamplingPeriod);
    }

    /**
     * Construct a new {@link EventFrequencyCounter}, with a reporting period of
     * {@link #DEFAULT_REPORTING_PERIOD}, and a sampling period of {@link #DEFAULT_SAMPLING_PERIOD}.
     * @see EventFrequencyCounter#EventFrequencyCounter(String, long, TimeUnit, long, TimeUnit,
     *      long, TimeUnit, int)
     */
    public EventFrequencyCounter(String name, long pollPeriod, TimeUnit pollPeriodTimeUnit,
                                 int maxEventsPerSamplingPeriod) {
        this(name, pollPeriod, pollPeriodTimeUnit, DEFAULT_REPORTING_PERIOD,
                DEFAULT_REPORTING_PERIOD_TIME_UNIT, maxEventsPerSamplingPeriod);
    }

    /**
     * Construct a new {@link EventFrequencyCounter}, with a sampling period of
     * {@link #DEFAULT_SAMPLING_PERIOD}.
     * @see EventFrequencyCounter#EventFrequencyCounter(String, long, TimeUnit, long, TimeUnit,
     *      long, TimeUnit, int)
     */
    public EventFrequencyCounter(String name, long pollPeriod, TimeUnit pollPeriodTimeUnit,
                                 long reportingPeriod, TimeUnit reportingPeriodTimeUnit,
                                 int maxEventsPerSamplingPeriod) {
        this(name, pollPeriod, pollPeriodTimeUnit, reportingPeriod, reportingPeriodTimeUnit,
                DEFAULT_SAMPLING_PERIOD, DEFAULT_SAMPLING_PERIOD_TIME_UNIT,
                maxEventsPerSamplingPeriod);
    }

    /**
     * Construct a new {@link EventFrequencyCounter} for events with the specified reporting period
     * (the units of frequency, eg events per second, events per 100 ms, etc). Events will be
     * collected over the specified sampling period, up to maxEventsPerSamplingPeriod, and the
     * frequency will be calculated as the number of events which occurred divided by the sampling
     * period, then scaled to the reporting period.
     * @param reportingPeriod the period over which the event frequency will be reported
     * @param reportingPeriodTimeUnit the {@link TimeUnit} of reportingPeriod
     * @param samplingPeriod the period over which event samples will be collected
     * @param samplingPeriodTimeUnit the {@link TimeUnit} of samplingPeriod
     * @param maxEventsPerSamplingPeriod the maximum number of samples which could reasonably be
     *                                   expected to occur during the sampling period. If more than
     *                                   this many events occur, the oldest ones will be discarded,
     *                                   and the frequency calculation will be performed with a
     *                                   truncated data set.
     * @throws NullPointerException if reportingPeriodTimeUnit or samplingPeriodTimeUnit are null
     * @throws IllegalArgumentException if reportingPeriod, samplingPeriod, or
     *                                  maxEventsPerSamplingPeriod are <= 0
     * @see EventCounter#EventCounter(String, long, TimeUnit)
     */
    public EventFrequencyCounter(String name, long pollPeriod, TimeUnit pollPeriodTimeUnit,
                                 long reportingPeriod, TimeUnit reportingPeriodTimeUnit,
                                 long samplingPeriod, TimeUnit samplingPeriodTimeUnit,
                                 int maxEventsPerSamplingPeriod) {
        super(name, pollPeriod, pollPeriodTimeUnit);

        Objects.requireNonNull(reportingPeriodTimeUnit, "reportingPeriodTimeUnit may not be null");
        Objects.requireNonNull(samplingPeriodTimeUnit, "samplingPeriodTimeUnit may not be null");
        if (reportingPeriod <= 0) {
            throw new IllegalArgumentException("reportingPeriod must be > 0");
        } else if (samplingPeriod <= 0) {
            throw new IllegalArgumentException("samplingPeriod must be > 0");
        } else if (maxEventsPerSamplingPeriod <= 0) {
            throw new IllegalArgumentException("maxEventsPerSamplingPeriod must be > 0");
        }

        reportingPeriodNs = reportingPeriodTimeUnit.toNanos(reportingPeriod);
        samplingPeriodNs = samplingPeriodTimeUnit.toNanos(samplingPeriod);
        eventTimestamps = new long[maxEventsPerSamplingPeriod];
    }

    @Override
    public void trigger() {
        trigger(SystemClock.elapsedRealtimeNanos());
    }

    /**
     * Trigger an event for this {@link EventFrequencyCounter}, which occurs at the specified
     * time
     * @param timestamp the time (in ns) at which this event occurred. This time must be relative
     *                  to {@link SystemClock#elapsedRealtimeNanos()}.
     */
    // N.B. timestamp must be relative to SystemClock.elapsedRealtimeNanos
    public void trigger(long timestamp) {
        synchronized (lock) {
            if (++eventTimestampsHead == eventTimestamps.length) {
                eventTimestampsHead = 0;
            }

            eventTimestamps[eventTimestampsHead] = timestamp;

            super.trigger();

            if (eventCounter > eventTimestamps.length) {
                eventCounter = eventTimestamps.length;
            }
        }
    }

    @Override
    public String getValue() {
        final long now = SystemClock.elapsedRealtimeNanos();

        final long timestampHead;
        final long timestampTail;
        final long eventCounterLocal;

        synchronized (lock) {
            // Expire events that are older than our sampling period
            // N.B. we cap eventCounter at maxEventsPerSamplingPeriod, so casting to int is safe
            int tail = (eventTimestamps.length + eventTimestampsHead - (int)eventCounter + 1) %
                    eventTimestamps.length;
            while (eventCounter > 0 && (now - eventTimestamps[tail]) > samplingPeriodNs) {
                eventCounter--;
                if (++tail == eventTimestamps.length) {
                    tail = 0;
                }
            }

            eventCounterLocal = eventCounter;
            timestampHead = eventTimestamps[eventTimestampsHead];
            timestampTail = eventTimestamps[tail];
        }

        // Must be at least 2 events in the sampling period to calculate a frequency
        if (eventCounterLocal < 2) {
            return "0.00";
        }

        return String.format(Locale.US, "%.2f", ((eventCounterLocal - 1) * reportingPeriodNs) /
                (float)(timestampHead - timestampTail));
    }
}
