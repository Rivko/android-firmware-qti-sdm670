/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.hardware;

/**
 * This interface represents a Panorama capture engine to which all realizations of the engine must
 * conform
 */
/*package*/ interface PanoramaEngine {
    /**
     * Initializes the {@link PanoramaEngine}. This must be invoked before any engine operations are
     * available.
     * @throws RuntimeException if the engine is already initialized
     */
    void init();

    /**
     * Releases the {@link PanoramaEngine}. This should be invoked after usage of the engine is
     * complete.
     * @throws RuntimeException if the engine is not initialized
     */
    void release();

    /**
     * Starts a Panorama capture operation
     * @param listener a {@link PanoramaAction.EventListener) on which Panorama event notifications
     *      will be delivered
     * @throws RuntimeException if the engine is not initialized
     * @throws IllegalArgumentException if listener is null
     */
    void startCapture(PanoramaActor.EventListener listener);

    /**
     * Completes a Panorama capture operation
     * @throws RuntimeException if the engine is not initialized
     */
    void completeCapture();

    /**
     * Cancels a Panorama capture operation
     * @throws RuntimeException if the engine is not initialized
     */
    void cancelCapture();
}
