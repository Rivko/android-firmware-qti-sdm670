/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.misc;

import java.util.concurrent.atomic.AtomicInteger;

/**
 * This class is intended to be used as a base class for objects that require reference-counting
 * semantics for controlled cleanup. Rather than rely on the unpredictable timing of the garbage
 * collector, {@link ReferenceCountedObject}s allow clients to signal when they are done with an
 * object. When constructed, the reference count will initially be 1. When all owners have
 * {@link #release()}d this object, the {@link #onReleased()} method will be invoked.
 * <p><i>NOTE: this class is thread-safe; it's methods may be invoked in any thread context</i>
 */
public abstract class BaseReferenceCountedObject<T extends BaseReferenceCountedObject<T>>
        implements ReferenceCountedObject {
    private final AtomicInteger refCount = new AtomicInteger(1);

    /** {@inheritDoc} */
    @SuppressWarnings("unchecked")
    @Override
    public T retain() {
        while (true) {
            int current = refCount.get();
            if (current == 0) {
                throw new IllegalStateException("Attempt to retain a ref-counted object with 0 count");
            }
            if (refCount.compareAndSet(current, current + 1)) {
                return (T)this;
            }
        }
    }

    /** {@inheritDoc} */
    @SuppressWarnings("unchecked")
    @Override
    public T retain(int count) {
        if (count < 0) {
            throw new IllegalArgumentException("count must be >= 0");
        }

        while (true) {
            int current = refCount.get();
            if (current == 0) {
                throw new IllegalStateException("Attempt to retain a ref-counted object with 0 count");
            }
            if (refCount.compareAndSet(current, current + count)) {
                return (T)this;
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    public void release() {
        while (true) {
            int current = refCount.get();
            if (current == 0) {
                throw new IllegalStateException("Attempt to release a ref-counted object with 0 count");
            }
            if (refCount.compareAndSet(current, current - 1)) {
                if (current == 1) {
                    onReleased();
                }
                return;
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    public void release(int count) {
        if (count < 0) {
            throw new IllegalArgumentException("count must be >= 0");
        }

        while (true) {
            int current = refCount.get();
            if (current == 0) {
                throw new IllegalStateException("Attempt to release a ref-counted object with 0 count");
            } else if (current < count) {
                throw new IllegalStateException("Attempted to release more references than are outstanding");
            }
            if (refCount.compareAndSet(current, current - count)) {
                if (current == count) {
                    onReleased();
                }
                return;
            }
        }
    }

    /**
     * Subclasses should implement this method to do whatever action/cleanup/etc is required when
     * the reference count reaches 0.
     * <p><i>NOTE: This method will be invoked in an arbitrary thread context</i>
     */
    protected abstract void onReleased();
}
