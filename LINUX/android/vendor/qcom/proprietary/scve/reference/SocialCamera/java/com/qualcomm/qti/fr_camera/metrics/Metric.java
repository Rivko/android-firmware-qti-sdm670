/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.metrics;

/**
 * This class represents an abstract "metric", which performs some form of counting, measuring, or
 * display of configuration. It is implemented as an abstract class instead of an interface so that
 * it can define package-visible members, though it is intended to act as an interface.
 */
public abstract class Metric {
    /**
     * Get the human-readable name of this {@link Metric}
     * @return the human-readable name of this {@link Metric}
     */
    public abstract String getName();

    /**
     * Get the human-readable current value of this {@link Metric}
     * @return the human-readable current value of this {@link Metric}
     */
    public abstract String getValue();

    /**
     * Get the interval at which this metric would like to be polled for visual updates.
     * @return the desired polling interval (in ms) at which this metric would like to be polled
     */
    /*package*/ abstract long getDesiredPollingInterval();

    /**
     * Resets this {@link Metric} to its default or initial value, if applicable
     */
    /*package*/ abstract void reset();
}
