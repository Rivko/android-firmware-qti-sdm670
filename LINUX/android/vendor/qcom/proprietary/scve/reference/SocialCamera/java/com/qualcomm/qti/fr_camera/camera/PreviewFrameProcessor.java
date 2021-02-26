/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera;

import android.graphics.ImageFormat;
import android.media.ImageDirectByteBufferNV21;
import android.media.ImageNV21;
import android.opengl.EGL14;
import android.opengl.EGLDisplay;
import android.opengl.GLES30;
import android.opengl.Matrix;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;

import com.qualcomm.qti.fr_camera.coordinate.Orientation;
import com.qualcomm.qti.khronos.EGLKHRFenceSync;
import com.qualcomm.qti.khronos.EGLSyncKHR;
import com.qualcomm.qti.khronos.GLESContextThread;
import com.qualcomm.qti.khronos.GLES30Ext;
import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.TextureType;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Objects;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

/**
 * This class acts as a sink for camera preview frames (via a SurfaceTexture), and processes them
 * for two purposes: a pass-through for preview display, along with rotation & output as NV21 for
 * CPU processing use cases. This allows configuring the camera with just a single preview output
 * stream, which potentially allows for higher camera preview resolutions.
 */
public class PreviewFrameProcessor extends GLESContextThread {
    /**
     * Classes should implement this interface to receive rotated preview frames
     */
    public interface Callback {
        /**
         * This method will be invoked each time a new rotated preview frame is available
         * @param previewFrameProcessor the previewFrameProcessor instance which produced this
         *                              rotated preview {@link ImageNV21}
         * @param im the rotated preview frame, encoded as a {@link ImageFormat#YUV_420_888}
         *           {@link ImageNV21}
         */
        void onPreviewFrameRotationComplete(PreviewFrameProcessor previewFrameProcessor,
                                            ImageNV21 im);
    }

    private static final String TAG = "SocialCamera";
    private static final int MSG_SET_CALLBACK = 0x3000;
    private static final int MSG_OUTPUT_BUFFER_RENDERED = 0x3001;
    private static final int MAX_NUM_OUTPUT_BUFFERS = 3;
    private static final int[] GL_COLOR_ATTACHMENTS = new int[] {
            GLES30.GL_COLOR_ATTACHMENT0, GLES30.GL_COLOR_ATTACHMENT1 };

    private final Orientation orientation;

    // These fields may only be accessed while synchronized
    private Handler glesThreadHandler;
    private Callback paramCallback;

    // These fields are private to the PreviewFrameProcessor thread
    private final PreviewFrameRenderer previewRenderer = new PreviewFrameRenderer();
    private final PreviewFrameRenderer nv21BufferRenderer = new PreviewFrameRenderer();
    private EGLDisplay eglDisplay;
    private int inputTextureWidth;
    private int inputTextureHeight;
    private int framebufferWidth;
    private int framebufferHeight;
    private int outputImageWidth;
    private int outputImageHeight;
    private int outputImageWidthVU;
    private int outputImageHeightVU;
    private int outputImageStride;
    private int outputImageSize;
    private final float[] outputImageTransform = new float[16];
    private final float[] outputImageTransformConcat = new float[16];
    private ExecutorService pendingOutputBufferExecutor;
    private final Semaphore outstandingOutputBufferSem = new Semaphore(MAX_NUM_OUTPUT_BUFFERS);
    private final int[] outputImageTextures = new int[3];
    private final int[] outputImageFbos = new int[2];
    private final int[] outputBuffers = new int[MAX_NUM_OUTPUT_BUFFERS];
    private final boolean[] outputBuffersValid = new boolean[MAX_NUM_OUTPUT_BUFFERS];
    private Callback callback;

    // outputBuffersAvailable is used in multiple thread contexts; synchronize when accessing
    private final boolean[] outputBuffersAvailable = new boolean[MAX_NUM_OUTPUT_BUFFERS];

    /**
     * Construct a new {@link PreviewFrameProcessor}
     * @param o the current {@link Orientation} of the device, relative to its baseline
     * @throws NullPointerException if o is null
     */
    public PreviewFrameProcessor(Orientation o) {
        super("PreviewFrameProcessor");

        Objects.requireNonNull(o, "o may not be null");

        orientation = o;
    }

    /**
     * Sets a {@link Callback} to invoke when a new preview {@link ImageNV21} is available.
     * <p/><i> NOTE: if the {@link PreviewFrameProcessor} is running, there may be several frames
     * in-flight for which the old callback (if any) will still be invoked.
     * @param callback a {@link Callback} to invoke whenever a new preview {@link ImageNV21} is
     *                 available, or null to clear the currently registered callback
     * @param handler if non-null, a {@link Handler} on which callback will be invoked. If null, the
     *                callback will be invoked in an arbitrary thread context.
     */
    public void setCallback(Callback callback, Handler handler) {
        // If a handler is specified, wrap callback to invoke via the specified handler
        if (callback != null && handler != null) {
            callback = new CallbackWithHandler(callback, handler);
        }

        synchronized (this) {
            switch (getState()) {
                case NOT_STARTED:
                    paramCallback = callback;
                    break;

                case STARTED:
                    glesThreadHandler.obtainMessage(MSG_SET_CALLBACK, callback).sendToTarget();
                    break;

                case CLOSED:
                    Log.w(TAG, "setCallback invoked after PreviewFrameProcessor has been closed");
                    break;

                default:
                    throw new IllegalStateException("Unknown state: " + getState());
            }
        }
    }

    // N.B. Invoked in a synchronized context
    @Override
    protected void onStarted(Looper glesThreadLooper) {
        glesThreadHandler = new Handler(glesThreadLooper, glesThreadCallback);
    }

    // N.B. Invoked in a synchronized context
    @Override
    protected void onClosed() {
    }

    @Override
    protected void onContextCreated(TextureType textureType) {
        // N.B. Callback may have been set before GLESContextThread was created
        synchronized (PreviewFrameProcessor.this) {
            callback = paramCallback;
        }

        eglDisplay = EGL14.eglGetCurrentDisplay();

        previewRenderer.init(textureType, PreviewFrameRenderer.ColourFormat.ABGR_8888);
        nv21BufferRenderer.init(textureType, PreviewFrameRenderer.ColourFormat.NV21);

        // N.B. We are just passing the texture through without scaling or correcting for aspect
        // ratio, so size is irrelevant
        previewRenderer.resized(0, 0, 0, 0);

        for (int i = 0; i < outputImageTextures.length; i++) {
            outputImageTextures[i] = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
        }

        GLES30.glGenFramebuffers(outputImageFbos.length, outputImageFbos, 0);

        // Create buffer handles for the output images. The actual storage won't be specified until
        // it is required.
        GLES30.glGenBuffers(outputBuffers.length, outputBuffers, 0);
        Arrays.fill(outputBuffersValid, false);
        synchronized (outputBuffersAvailable) {
            Arrays.fill(outputBuffersAvailable, true);
        }

        pendingOutputBufferExecutor = Executors.newSingleThreadExecutor();
    }

    @Override
    protected void onContextRelease() {
        try {
            if (!outstandingOutputBufferSem.tryAcquire(MAX_NUM_OUTPUT_BUFFERS,
                    5, TimeUnit.SECONDS)) {
                Log.e(TAG, "Timed out waiting for all outstanding output buffers to be released");
            }
        } catch (InterruptedException e) {
            Log.w(TAG, "Interrupted while waiting for outstanding buffers to be released");
            Thread.currentThread().interrupt();
        }

        pendingOutputBufferExecutor.shutdown();
        pendingOutputBufferExecutor = null;

        // N.B. No need to synchronize, outputBuffers is created on this thread, and never modified
        GLES30.glDeleteBuffers(outputBuffers.length, outputBuffers, 0);

        GLES30.glDeleteTextures(outputImageTextures.length, outputImageTextures, 0);
        GLES30.glDeleteFramebuffers(outputImageFbos.length, outputImageFbos, 0);

        previewRenderer.release();
        nv21BufferRenderer.release();

        eglDisplay = null;
    }

    @Override
    protected void onInputTextureSizeChanged(int width, int height) {
        inputTextureWidth = width;
        inputTextureHeight = height;

        // Rotation is accounted for in the texture transform - specify the original width and
        // height so that the proper YUV color space (BT601 vs BT709) is selected
        nv21BufferRenderer.resized(inputTextureWidth, inputTextureHeight,
                inputTextureWidth, inputTextureHeight);

        updateOutputImageBuffers();
    }

    @Override
    protected void onFramebufferSizeChanged(int width, int height) {
        framebufferWidth = width;
        framebufferHeight = height;
    }

    @Override
    protected void onRender(int texture, float[] transform) {
        // If a callback is registered, render a rotated frame to an output Image
        if (callback != null) {
            final int i = tryAcquireOutputBuffer();
            if (i != -1) {
                GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, outputImageFbos[0]);
                GLES30.glViewport(0, 0, outputImageWidth, outputImageHeight);
                Matrix.multiplyMM(outputImageTransformConcat, 0, transform, 0,
                        outputImageTransform, 0);
                nv21BufferRenderer.render(texture, outputImageTransformConcat);
                // N.B. blit the VU plane to a half-sized texture to halve its horz/vert resolution
                GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, outputImageFbos[1]);
                GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT1);
                GLES30.glBlitFramebuffer(0, 0, outputImageWidth, outputImageHeight,
                        0, 0, outputImageWidthVU, outputImageHeightVU,
                        GLES30.GL_COLOR_BUFFER_BIT, GLES30.GL_LINEAR);
                // N.B. all draws have been issued, now perform reads into the output buffer. FBOs
                // have pixel store params set appropriately.
                GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputBuffers[i]);
                GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT0);
                GLES30.glPixelStorei(GLES30.GL_PACK_ROW_LENGTH, outputImageStride);
                GLES30Ext.glReadPixels(0, 0, outputImageWidth, outputImageHeight, GLES30.GL_RED,
                        GLES30.GL_UNSIGNED_BYTE, 0);
                GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);
                // N.B. Technically, only GL_FRAMEBUFFER can be invalidated, but our Adreno driver
                // supports invalidating within the read framebuffer too
                GLES30.glInvalidateFramebuffer(GLES30.GL_READ_FRAMEBUFFER, 2, GL_COLOR_ATTACHMENTS,
                        0);
                GLES30.glBindFramebuffer(GLES30.GL_READ_FRAMEBUFFER, outputImageFbos[1]);
                GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputBuffers[i]);
                GLES30.glPixelStorei(GLES30.GL_PACK_ROW_LENGTH, outputImageStride / 2); // in pixels
                GLES30Ext.glReadPixels(0, 0, outputImageWidthVU, outputImageHeightVU, GLES30.GL_RG,
                        GLES30.GL_UNSIGNED_BYTE, outputImageHeight * outputImageStride);
                GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);
                // N.B. See note above re: glInvalidateFramebuffer
                GLES30.glInvalidateFramebuffer(GLES30.GL_READ_FRAMEBUFFER, 1, GL_COLOR_ATTACHMENTS,
                        0);

                EGLSyncKHR sync = EGLKHRFenceSync.eglCreateSyncKHR(eglDisplay,
                        EGLKHRFenceSync.EGL_SYNC_FENCE_KHR, null);
                GLES30.glFlush();

                pendingOutputBufferExecutor.execute(new PendingRenderToBuffer(eglDisplay, sync, i,
                        outputImageWidth, outputImageHeight, outputImageStride, outputImageSize));
            } else {
                Log.w(TAG, "No output buffers available, dropping frame");
            }
        }

        // Render for the output surface (ie preview display)
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, 0);
        GLES30.glViewport(0, 0, framebufferWidth, framebufferHeight);
        previewRenderer.render(texture, transform);
    }

    private void updateOutputImageBuffers() {
        if (callback != null) {
            if (!orientation.isRightAngle) {
                outputImageWidth = inputTextureWidth;
                outputImageHeight = inputTextureHeight;
            } else {
                //noinspection SuspiciousNameCombination
                outputImageWidth = inputTextureHeight;
                //noinspection SuspiciousNameCombination
                outputImageHeight = inputTextureWidth;
            }
            outputImageWidthVU = (outputImageWidth + 1) / 2;
            outputImageHeightVU = (outputImageHeight + 1) / 2;

            // N.B. For asynchronous glReadPixels to PBOs, Adreno 300 & 400 series require 32-byte
            // aligned pixel pack rows, and Adreno 500 series requires 64-byte aligned rows
            outputImageStride = (outputImageWidthVU * 2 + 63) & ~63;
            outputImageSize = (outputImageHeight + outputImageHeightVU) * outputImageStride;
        } else {
            outputImageWidth = 0;
            outputImageHeight = 0;
            outputImageWidthVU = 0;
            outputImageHeightVU = 0;
            outputImageStride = 0;
            outputImageSize = 0;
        }

        // N.B. flag output buffer as invalid - it will be respecified the next time it is used
        Arrays.fill(outputBuffersValid, false);

        // Specify the NV21 textures which the output image will be rendered into
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, outputImageTextures[0]);
        GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, GLES30.GL_R8, outputImageWidth,
                outputImageHeight, 0, GLES30.GL_RED, GLES30.GL_UNSIGNED_BYTE, null);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, outputImageTextures[1]);
        GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, GLES30.GL_RG8, outputImageWidth,
                outputImageHeight, 0, GLES30.GL_RG, GLES30.GL_UNSIGNED_BYTE, null);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, outputImageTextures[2]);
        GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, GLES30.GL_RG8, outputImageWidthVU,
                outputImageHeightVU, 0, GLES30.GL_RG, GLES30.GL_UNSIGNED_BYTE, null);

        // Specify the framebuffer used for rendering the output image, and for reading the full-
        // resolution Y plane
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, outputImageFbos[0]);
        GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT0,
                GLES30.GL_TEXTURE_2D, outputImageTextures[0], 0);
        GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT1,
                GLES30.GL_TEXTURE_2D, outputImageTextures[1], 0);
        GLES30.glDrawBuffers(2, GL_COLOR_ATTACHMENTS, 0);
        if (GLES30.glCheckFramebufferStatus(GLES30.GL_FRAMEBUFFER) !=
                GLES30.GL_FRAMEBUFFER_COMPLETE) {
            Log.w(TAG, "Output Y framebuffer is incomplete!");
        }

        // Specify the framebuffer used for receiving and reading the scaled-down VU plane
        GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, outputImageFbos[1]);
        GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT0,
                GLES30.GL_TEXTURE_2D, outputImageTextures[2], 0);
        if (GLES30.glCheckFramebufferStatus(GLES30.GL_FRAMEBUFFER) !=
                GLES30.GL_FRAMEBUFFER_COMPLETE) {
            Log.w(TAG, "Output VU framebuffer is incomplete!");
        }

        Matrix.setIdentityM(outputImageTransform, 0);
        // N.B. OpenGL matrix operations are preorder; read this backwards for the logical ordering
        Matrix.translateM(outputImageTransform, 0, 0.5f, 0.5f, 0f);
        Matrix.scaleM(outputImageTransform, 0, 1f, -1f, 1f);
        Matrix.rotateM(outputImageTransform, 0, orientation.degrees, 0f, 0f, 1f);
        Matrix.translateM(outputImageTransform, 0, -0.5f, -0.5f, 0f);
    }

    private int tryAcquireOutputBuffer() {
        int i;
        synchronized (outputBuffersAvailable) {
            for (i = 0; i < outputBuffers.length; i++) {
                if (outputBuffersAvailable[i]) {
                    outputBuffersAvailable[i] = false;
                    break;
                }
            }
        }

        if (i == outputBuffers.length) {
            return -1;
        }

        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputBuffers[i]);
        if (!outputBuffersValid[i]) {
            GLES30.glBufferData(GLES30.GL_PIXEL_PACK_BUFFER, outputImageSize, null,
                    GLES30.GL_STREAM_READ);
            outputBuffersValid[i] = true;
        } else {
            GLES30.glUnmapBuffer(GLES30.GL_PIXEL_PACK_BUFFER);
        }
        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);

        return i;
    }

    private final Handler.Callback glesThreadCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            boolean handled = true;

            switch (msg.what) {
                case MSG_SET_CALLBACK:
                    callback = (Callback)msg.obj;
                    updateOutputImageBuffers();
                    break;

                case MSG_OUTPUT_BUFFER_RENDERED:
                    // Acquire a permit on the outstanding buffer semaphore before we send this
                    // output buffer to the registered callback.
                    if (!outstandingOutputBufferSem.tryAcquire()) {
                        Log.e(TAG, "Expected to acquire a permit on the output buffer semaphore, " +
                                "but failed");
                        break;
                    }

                    callback.onPreviewFrameRotationComplete(PreviewFrameProcessor.this,
                            ((PendingRenderToBuffer)msg.obj).createImage());
                    break;

                default:
                    handled = false;
                    break;
            }

            return handled;
        }
    };

    private static class CallbackWithHandler implements Callback {
        private final Callback callback;
        private final Handler handler;

        public CallbackWithHandler(Callback callback, Handler handler) {
            this.callback = callback;
            this.handler = handler;
        }

        @Override
        public void onPreviewFrameRotationComplete(
                final PreviewFrameProcessor previewFrameProcessor, final ImageNV21 im) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    callback.onPreviewFrameRotationComplete(previewFrameProcessor, im);
                }
            });
        }
    }

    private class PendingRenderToBuffer implements Runnable {
        private static final long TIMEOUT_NS = 1000000000l;

        private final EGLDisplay dpy;
        private final EGLSyncKHR sync;
        private final int bufferIdx;
        private final int bufferWidth;
        private final int bufferHeight;
        private final int bufferStride;
        private final int bufferSize;

        PendingRenderToBuffer(EGLDisplay dpy, EGLSyncKHR sync, int bufferIdx, int bufferWidth,
                              int bufferHeight, int bufferStride, int bufferSize) {
            this.dpy = dpy;
            this.sync = sync;
            this.bufferIdx = bufferIdx;
            this.bufferWidth = bufferWidth;
            this.bufferHeight = bufferHeight;
            this.bufferStride = bufferStride;
            this.bufferSize = bufferSize;
        }

        @Override
        public void run() {
            EGLKHRFenceSync.eglClientWaitSyncKHR(dpy, sync, 0, TIMEOUT_NS);
            EGLKHRFenceSync.eglDestroySyncKHR(dpy, sync);

            // N.B. glesThreadHandler is created during onStarted, which occurs before onRender may
            // be invoked, and is not destroyed until this object is garbage-collected. It must be
            // synchronized to ensure visibility, since onStarted runs asynchronously to the
            // creation of the GLES thread.
            synchronized (PreviewFrameProcessor.this) {
                glesThreadHandler.obtainMessage(MSG_OUTPUT_BUFFER_RENDERED, this).sendToTarget();
            }
        }

        private ImageNV21 createImage() {
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputBuffers[bufferIdx]);
            ByteBuffer buf = (ByteBuffer)GLES30.glMapBufferRange(GLES30.GL_PIXEL_PACK_BUFFER, 0,
                    bufferSize, GLES30.GL_MAP_READ_BIT);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);

            PreviewFrameProcessorImage im = new PreviewFrameProcessorImage(bufferIdx, bufferWidth,
                    bufferHeight, bufferStride, buf);
            im.setTimestamp(SystemClock.elapsedRealtimeNanos());

            return im;
        }
    }

    private class PreviewFrameProcessorImage extends ImageDirectByteBufferNV21 {
        private final int bufferIdx;

        public PreviewFrameProcessorImage(int bufferIdx, int bufferWidth, int bufferHeight,
                                          int bufferStride, ByteBuffer buf) {
            super(buf, bufferWidth, bufferHeight, bufferStride);
            this.bufferIdx = bufferIdx;
        }

        @Override
        protected void onClosed() {
            super.onClosed();
            outstandingOutputBufferSem.release();

            synchronized (outputBuffersAvailable) {
                outputBuffersAvailable[bufferIdx] = true;
            }
        }
    }
}
