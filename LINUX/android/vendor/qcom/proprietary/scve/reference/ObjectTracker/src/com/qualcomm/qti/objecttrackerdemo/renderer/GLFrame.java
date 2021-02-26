/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import android.opengl.GLES30;

import com.qualcomm.qti.khronos.EGLKHRImageBase.EGLImageKHR;
import com.qualcomm.qti.khronos.EGLSyncKHRContainer;
import com.qualcomm.qti.khronos.GLOES2EGLImage;
import com.qualcomm.qti.objecttracker.cameraengine.Frame;
import com.qualcomm.qti.objecttracker.misc.BaseReferenceCountedObject;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TextureFormat;

/**
 * This class wraps a GL texture containing a single frame of image data from the camera. It is a
 * {@link ReferenceCountedObject}; whomever creates the object is responsible for setting an initial
 * reference count (via {@link ReferenceCountedObject#reset(int)} based on the number of consumers
 * of the frame data. After it has been {@link #release()}d, it will be returned to the provided
 * {@link GLFrameTranslator} for reuse or cleanup.
 */
/*package*/ class GLFrame extends BaseReferenceCountedObject<GLFrame> {
    /**
     * The timestamp (from {@link Frame#timestamp} of the {@link Frame} from which this
     * {@link GLFrame} is derived
     */
    public final long timestamp;

    /** {@link EGLImageKHR} describing the GL texture containing camera preview frame data */
    public final EGLImageKHR image;

    /** {@link EGLSyncKHRContainer} to use when synchronizing on usage of {@link #image} */
    public final EGLSyncKHRContainer syncContainer;

    /** The {@link TextureFormat} of {@link #image} */
    public final TextureFormat textureFormat;

    private final GLFrameTranslator translator;

    /**
     * Construct a {@link GLFrame}, wrapping the specified GL texture data, and owned by the
     * specified {@link GLFrameTranslator}
     * @param timestamp See {@link #timestamp}
     * @param image See {@link #image}
     * @param syncContainer See {@link #syncContainer}
     * @param textureFormat See {@link #textureFormat}
     * @param translator the {@link GLFrameTranslator} which created and owns {@link #image}
     */
    /*package*/ GLFrame(long timestamp, EGLImageKHR image, TextureFormat textureFormat,
            EGLSyncKHRContainer syncContainer, GLFrameTranslator translator) {
        this.timestamp = timestamp;
        this.image = image;
        this.textureFormat = textureFormat;
        this.syncContainer = syncContainer;
        this.translator = translator;
    }

    /**
     * Bind {@link #image} to the specified texture handle. {@link #image} will be bound to the
     * {@link TextureFormat#glTextureTarget} GL texture target.
     * @param texture the texture handle to bind {@link #image} to
     */
    public void bindToTexture(int texture) {
        GLES30.glBindTexture(textureFormat.glTextureTarget, texture);
        GLOES2EGLImage.glEGLImageTargetTexture2DOES(textureFormat.glTextureTarget, image);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onReleased() {
        translator.releaseGLFrame(this);
    }
}
