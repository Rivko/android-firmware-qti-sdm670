/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.metrics;

import java.util.Objects;
import java.util.concurrent.TimeUnit;

/**
 * This {@link Metric} tracks the instantaneous state of the tracked metric. It can run either in
 * push mode (where it will notify {@link Metrics} every time the state changes), or in pull mode
 * (where {@link Metrics} will poll this {@link StateMetric} for changes).
 */
public class StateMetric extends Metric {
    private static final Metrics metrics = Metrics.getInstance();

    private final String name;
    private final String defaultState;
    private final long pollPeriodMs;
    private String state;

    /**
     * Construct a new {@link StateMetric} which will notify {@link Metrics} every time its state
     * changes
     * @param name the name of this {@link StateMetric}
     * @param defaultState the default value for this {@link StateMetric}. This will be the initial
     *                     value of the metric, as well as the value applied when {@link #reset()}
     *                     is invoked
     * @throws NullPointerException if name is null
     * @throws IllegalArgumentException if name is an empty string
     */
    public StateMetric(String name, String defaultState) {
        this(name, defaultState, 0, TimeUnit.SECONDS);
    }

    /**
     * Construct a new {@link StateMetric}
     * @param name the name of this {@link StateMetric}
     * @param defaultState the default value for this {@link StateMetric}. This will be the initial
     *                     value of the metric, as well as the value applied when {@link #reset()}
     *                     is invoked
     * @param pollPeriod the interval at which this {@link Metric} should be polled, or 0 if this
     *                   {@link Metric} should notify {@link Metrics} whenever its state changes
     * @param pollPeriodTimeUnit the {@link TimeUnit} of pollPeriod
     * @throws NullPointerException if name or pollPeriodTimeUnit is null
     * @throws IllegalArgumentException if name is an empty string, or if pollPeriod < 0
     */
    public StateMetric(String name, String defaultState, long pollPeriod,
                       TimeUnit pollPeriodTimeUnit) {
        Objects.requireNonNull(name, "name may not be null");
        Objects.requireNonNull(pollPeriodTimeUnit, "pollPeriodTimeUnit may not be null");
        if (name.isEmpty()) {
            throw new IllegalArgumentException("name may not be an empty string");
        } else if (pollPeriod < 0) {
            throw new IllegalArgumentException("pollPeriodMs must be >= 0");
        }

        this.name = name;
        this.defaultState = (defaultState != null ? defaultState : "");
        pollPeriodMs = pollPeriodTimeUnit.toMillis(pollPeriod);
        state = this.defaultState;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public synchronized String getValue() {
        return state;
    }

    /**
     * Set the state of this {@link StateMetric}
     * @param state the current state of this {@link StateMetric}. If null, the default state (as
     *              specified when this {@link StateMetric} was created) will be set.
     */
    public synchronized void setState(String state) {
        if (state == null) {
            state = defaultState;
        }

        if (!this.state.equals(state)) {
            this.state = state;

            if (pollPeriodMs == 0) {
                metrics.metricUpdated(this);
            }
        }
    }

    @Override
    /*package*/ long getDesiredPollingInterval() {
        return pollPeriodMs;
    }

    @Override
    /*package*/ void reset() {
        setState(null);
    }
}
