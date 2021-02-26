/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.misc;

/**
 * This interface defines the semantics of a reference-counted object, with fine-grained lifetime
 * control (as opposed to the unpredictable lifetime control provided by the garbage collector).
 * Classes which implement this interface are responsible for defining what actions occur when an
 * object has no outstanding references.
 */
public interface ReferenceCountedObject {
    /**
     * Take an additional reference to this object. This can only be used on objects that have non-
     * zero reference counts.
     * @return the current {@link ReferenceCountedObject}
     * @throws IllegalStateException if the current reference count is 0
     */
    public abstract ReferenceCountedObject retain();

    /**
     * Take <i>count</i> additional references to this object. This can only be used on objects that
     * have non-zero reference counts.
     * @param count the number of additional references to take on this object
     * @return the current {@link ReferenceCountedObject}
     * @throws IllegalArgumentException if count is < 0
     * @throws IllegalStateException if the current reference count is 0
     */
    public abstract ReferenceCountedObject retain(int count);

    /**
     * Release a reference to this object. This can only be used on objects that have non-zero
     * reference counts.
     * @throws IllegalStateException if the current reference count is 0
     */
    public abstract void release();

    /**
     * Release <i>count</i> reference to this object. This can only be used on objects that have
     * non-zero reference counts.
     * @throws IllegalArgumentException if count is < 0
     * @throws IllegalStateException if the current reference count is 0, or if releasing count
     *      references would cause the reference count to be less than zero
     */
    public abstract void release(int count);
}
