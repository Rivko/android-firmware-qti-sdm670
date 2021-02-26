/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.metrics;

import android.os.Handler;
import android.os.Looper;

import java.util.LinkedHashMap;
import java.util.Objects;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

/**
 * A singleton class which aggregates all {@link Metric}s and provides a combined view to a single
 * interested party (such as a UI component designed to display the metrics)
 */
public class Metrics {
    /**
     * Classes may implement this interface to be notified when a {@link Metric} registered with
     * the {@link Metrics} instance has been updated (either via a regular polling operation, or by
     * an explicit change in state)
     */
    public interface MetricsUpdatedListener {
        /**
         * This method will be invoked when one or more of the tracked {@link Metric}s has changed
         */
        void onUpdateMetrics();
    }

    private static Metrics instance;

    private final LinkedHashMap<String, Metric> metrics = new LinkedHashMap<>();
    private Handler handler;
    private MetricsUpdatedListener listener;
    private ScheduledExecutorService scheduledExecutor;
    private ScheduledFuture<?> pollMetricsTask;

    /**
     * Get the singleton instance of {@link Metrics}
     * @return the singleton instance of {@link Metrics}
     */
    public static synchronized Metrics getInstance() {
        if (instance == null) {
            instance = new Metrics();
        }

        return instance;
    }

    private Metrics() { }

    /**
     * Register the {@link MetricsUpdatedListener}
     * @param listener an instance of {@link MetricsUpdatedListener}, or null to clear the current
     *                 listener
     * @param handler a handler on which the methods of listener will be invoked. If null,
     *                listener's methods will be invoked on the main application thread.
     */
    public synchronized void setMetricsUpdatedListener(MetricsUpdatedListener listener,
                                                       Handler handler) {
        if (this.listener != null) {
            this.handler.removeCallbacks(notifyListenerRunnable);
        }

        this.listener = listener;

        if (listener == null) {
            this.handler = null;
        } else if (handler == null) {
            this.handler = new Handler(Looper.getMainLooper());
        } else {
            this.handler = handler;
        }

        updatePollPeriod();
    }

    /**
     * Add a {@link Metric} to the set of tracked metrics
     * @param metric the {@link Metric} to add to {@link Metrics}. If a {@link Metric} with the same
     *               name (as reported by {@link Metric#getName()}) is already registered, it will
     *               be replaced.
     * @throws NullPointerException if metric is null
     */
    public synchronized void addMetric(Metric metric) {
        Objects.requireNonNull(metric, "metric may not be null");

        metrics.put(metric.getName(), metric);
        metric.reset();
        updatePollPeriod();
    }

    /**
     * Remove a {@link Metric} from the set of tracked metrics
     * @param metric the {@link Metric} to remove from {@link Metrics}, if it is registered.
     * @throws NullPointerException if metric is null
     */
    public synchronized void removeMetric(Metric metric) {
        Objects.requireNonNull(metric, "metric may not be null");

        String name = metric.getName();
        if (metrics.get(name) == metric) {
            metrics.remove(name);
            updatePollPeriod();
        }
    }

    /**
     * Get the aggregate value of all registered {@link Metric}s
     * @return a String containing the aggregate value of all registered {@link Metric}s
     */
    public synchronized String getAggregateValue() {
        StringBuilder sb = new StringBuilder();
        for (Metric m : metrics.values()) {
            sb.append(m.getName());
            sb.append(": ");
            sb.append(m.getValue());
            sb.append('\n');
        }
        return sb.toString();
    }

    /**
     * Metrics which report no polling period (ie return 0 from
     * {@link Metric#getDesiredPollingInterval()}) should instead invoke this method whenever they
     * change in value (which should ideally be relatively infrequent, otherwise high-frequency
     * polling would be a better design choice)
     * @param metric the {@link Metric} whose value has changed
     * @throws NullPointerException if metric is null
     */
    /*package*/ synchronized void metricUpdated(Metric metric) {
        Objects.requireNonNull(metric, "metric may not be null");

        if (metrics.containsValue(metric)) {
            notifyListener();
        }
    }

    // N.B. Must be called from a synchronized context
    private void updatePollPeriod() {
        if (pollMetricsTask != null) {
            pollMetricsTask.cancel(true);
            pollMetricsTask = null;
        }

        if (listener == null) {
            if (scheduledExecutor != null) {
                scheduledExecutor.shutdownNow();
                scheduledExecutor = null;
            }

            return;
        }

        long updatePollPeriodMs = Long.MAX_VALUE;

        for (Metric m : metrics.values()) {
            final long metricPollPeriodMs = m.getDesiredPollingInterval();
            if ( metricPollPeriodMs > 0 && metricPollPeriodMs < updatePollPeriodMs) {
                updatePollPeriodMs = metricPollPeriodMs;
            }
        }

        if (updatePollPeriodMs != Long.MAX_VALUE) {
            if (scheduledExecutor == null) {
                scheduledExecutor = Executors.newScheduledThreadPool(1);
            }

            pollMetricsTask = scheduledExecutor.scheduleAtFixedRate(pollMetricsRunnable,
                    0, updatePollPeriodMs, TimeUnit.MILLISECONDS);
        }
    }

    // N.B. Must be called from a synchronized context
    private void notifyListener() {
        if (listener != null) {
            handler.removeCallbacks(notifyListenerRunnable);
            handler.post(notifyListenerRunnable);
        }
    }

    private final Runnable pollMetricsRunnable = new Runnable() {
        @Override
        public void run() {
            synchronized (Metrics.this) {
                notifyListener();
            }
        }
    };

    private final Runnable notifyListenerRunnable = new Runnable() {
        @Override
        public void run() {
            final MetricsUpdatedListener localListener;
            synchronized (Metrics.this) {
                localListener = listener;
            }

            if (localListener != null) {
                localListener.onUpdateMetrics();
            }
        }
    };
}
