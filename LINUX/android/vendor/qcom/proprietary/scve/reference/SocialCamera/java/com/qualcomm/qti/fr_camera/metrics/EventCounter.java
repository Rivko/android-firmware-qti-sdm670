/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.metrics;

import java.util.Objects;
import java.util.concurrent.TimeUnit;

/**
 * A concrete {@link Metric} which counts events
 */
public class EventCounter extends Metric {
    /** The default period at which an {@link EventCounter} should be polled */
    public static final long DEFAULT_POLL_PERIOD = 250;

    /** The {@link TimeUnit} of {@link #DEFAULT_POLL_PERIOD} */
    public static final TimeUnit DEFAULT_POLL_PERIOD_TIME_UNIT = TimeUnit.MILLISECONDS;

    private final String name;
    private final long pollPeriodMs;

    // N.B. access to below members must be synchronized on lock
    protected final Object lock = new Object();
    protected long eventCounter;

    /**
     * Construct a new {@link EventCounter}, polled at the {@link #DEFAULT_POLL_PERIOD}
     * @see EventCounter#EventCounter(String, long, TimeUnit)
     */
    public EventCounter(String name) {
        this(name, DEFAULT_POLL_PERIOD, DEFAULT_POLL_PERIOD_TIME_UNIT);
    }

    /**
     * Construct a new {@link EventCounter}, polled at the specified interval
     * @param name the name of this {@link EventCounter}
     * @param pollPeriod the period at which this {@link EventCounter} should be polled
     * @param pollPeriodTimeUnit the {@link TimeUnit} of pollPeriod
     * @throws NullPointerException if name or pollPeriodTimeUnit is null
     * @throws IllegalArgumentException if name is an empty String, or if pollPeriod is <= 0
     */
    public EventCounter(String name, long pollPeriod, TimeUnit pollPeriodTimeUnit) {
        Objects.requireNonNull(name, "name may not be null");
        Objects.requireNonNull(pollPeriodTimeUnit, "pollPeriodTimeUnit may not be null");
        if (name.isEmpty()) {
            throw new IllegalArgumentException("name may not be an empty string");
        } else if (pollPeriod <= 0) {
            throw new IllegalArgumentException("pollPeriod must be > 0");
        }

        this.name = name;
        pollPeriodMs = pollPeriodTimeUnit.toMillis(pollPeriod);
    }

    /**
     * Reset this {@link EventCounter} to 0
     */
    @Override
    /*package*/ void reset() {
        synchronized (lock) {
            eventCounter = 0;
        }
    }

    /**
     * Trigger an event for this {@link EventCounter}
     */
    public void trigger() {
        synchronized (lock) {
            eventCounter++;
        }
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getValue() {
        final long val;
        synchronized (lock) {
            val = eventCounter;
        }

        return Long.toString(val);
    }

    @Override
    /*package*/ long getDesiredPollingInterval() {
        return pollPeriodMs;
    }
}
