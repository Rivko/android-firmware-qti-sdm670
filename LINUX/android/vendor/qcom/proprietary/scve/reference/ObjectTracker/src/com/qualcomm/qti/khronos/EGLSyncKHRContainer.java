/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import java.util.ArrayDeque;
import java.util.Iterator;
import java.util.concurrent.TimeUnit;

import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.util.Log;

import com.qualcomm.qti.khronos.EGLKHRFenceSync.EGLSyncKHR;

/**
 * This class acts as a container for multiple {@link EGLSyncKHR} objects. Within a single context,
 * a new sync object can replace an old one, but there is no guarantee when dealing with multiple
 * contexts on which order sync objects will be issued or completed in, so a container is required
 * to add and manage outstanding sync objects on a shared resource.
 */
public class EGLSyncKHRContainer {
    private static final String TAG = EGLSyncKHRContainer.class.getSimpleName();
    private static final int SYNC_CLEANUP_THRESHOLD = 10;
    private static final long MAX_WAIT_DURATION_NS = TimeUnit.NANOSECONDS.convert(1,
            TimeUnit.SECONDS);

    private final EGLDisplay dpy;
    private final ArrayDeque<EGLSyncKHR> syncs = new ArrayDeque<EGLSyncKHR>();
    private final Object awaitCompleteLock = new Object();

    /**
     * Construct a new {@link EGLSyncKHRContainer} instance
     * @param dpy the {@link EGLDisplay} on which this object will manage {@link EGLSyncKHR}s
     * @throws IllegalArgumentException if dpy is null or {@link EGL14#EGL_NO_DISPLAY}
     */
    public EGLSyncKHRContainer(EGLDisplay dpy) {
        if (dpy == null || dpy == EGL14.EGL_NO_DISPLAY) {
            throw new IllegalArgumentException("dpy must be a valid EGLDisplay");
        }

        this.dpy = dpy;
    }

    /**
     * Insert a new {@link EGLSyncKHR} object into the command stream for the current context, and
     * store it into this {@link EGLSyncKHRContainer}.
     * <p/><i>NOTE: must be called on a thread with an active EGL context</i>
     */
    public void insertSync() {
        EGLSyncKHR sync = EGLKHRFenceSync.eglCreateSyncKHR(dpy, EGLKHRFenceSync.EGL_SYNC_FENCE_KHR,
                null);
        if (sync == null || sync == EGLKHRFenceSync.EGL_NO_SYNC) {
            Log.w(TAG, "Failed inserting an EGL sync fence");
        }

        synchronized (syncs) {
            if (syncs.size() >= SYNC_CLEANUP_THRESHOLD) {
                clear(true);
            }
            syncs.add(sync);
        }
    }

    /**
     * Wait for all outstanding {@link EGLSyncKHR} objects stored in this container to complete.
     * After this method returns, this container will be empty.
     */
    public void awaitComplete() {
        final long end = System.nanoTime() + MAX_WAIT_DURATION_NS;

        synchronized (awaitCompleteLock) {
            EGLSyncKHR[] localSyncs;
            synchronized (syncs) {
                localSyncs = syncs.toArray(new EGLSyncKHR[syncs.size()]);
                syncs.clear();
            }

            for (EGLSyncKHR sync : localSyncs) {
                long timeout = end - System.nanoTime();
                if (timeout < 0) {
                    timeout = 0;
                }

                EGLKHRFenceSync.eglClientWaitSyncKHR(dpy, sync, 0, timeout);
                EGLKHRFenceSync.eglDestroySyncKHR(dpy, sync);
            }
        }
    }

    /**
     * Immediately clears all {@link EGLSyncKHR} objects in this container
     */
    public void clear() {
        synchronized (syncs) {
            clear(false);
        }
    }

    // N.B. Must be called with a lock on syncs. This method should complete very quickly.
    private void clear(boolean onlyCompleted) {
        Iterator<EGLSyncKHR> it = syncs.iterator();
        while (it.hasNext()) {
            EGLSyncKHR sync = it.next();
            if (!onlyCompleted || EGLKHRFenceSync.eglClientWaitSyncKHR(dpy, sync, 0, 0) ==
                    EGLKHRFenceSync.EGL_CONDITION_SATISFIED_KHR) {
                it.remove();
                EGLKHRFenceSync.eglDestroySyncKHR(dpy, sync);
            }
        }
    }
}
