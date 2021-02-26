/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui;

import java.nio.ByteBuffer;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.AttributeSet;
import android.util.Log;

import com.qualcomm.qti.khronos.CameraFrameRenderer;
import com.qualcomm.qti.khronos.ColourFormat;
import com.qualcomm.qti.khronos.EGL14Helper;
import com.qualcomm.qti.khronos.EGLKHRFenceSync;
import com.qualcomm.qti.khronos.EGLSyncKHR;
import com.qualcomm.qti.khronos.GLES30Ext;
import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.GLOESEGLImage;
import com.qualcomm.qti.khronos.ImageBufferUtils;
import com.qualcomm.qti.khronos.CameraFrameRenderer.TextureType;
import com.qualcomm.qti.khronos.GLES30Helper.FboDescriptor;
import com.qualcomm.qti.khronos.GLES30Helper.SavedFboToken;

/**
 * This class provides a {@link GLSurfaceView} extended to render the {@link SurfaceTexture} stream
 * provided by the camera. It also allows for a "tap" to extract the camera preview frame in RGBA
 * format.
 */
public class CameraPreviewView extends GLSurfaceView implements GLSurfaceView.Renderer,
        SurfaceTexture.OnFrameAvailableListener {
    /**
     * Classes that are interested in receiving a copy of the current preview frame in RGBA format
     * should implement this interface, and register with
     * {@link CameraPreviewView# setOnFrameAvailableListener(OnFrameAvailableListener)}
     */
    public interface OnFrameAvailableListener {
        /**
         * This method will be invoked when a new preview frame is available for processing. When
         * this method is done with the buffer, it should call {@link Semaphore#release()} on the
         * provided {@link Semaphore}, so that the {@link CameraPreviewView} can reuse the buffer.
         * @param frame {@link OutputFrame} made available to listener
         * @return true if frame is consumed, false otherwise.
         */
        boolean onFrameAvailable(OutputFrame frame);
    }

    /**
     * Wrapper for the resources that make up a single output frame.
     */
    public static class OutputFrame {
        private static final int THUMB_SIZE_DP = 160; // Max width/height of thumbnail, in dp

        public final int width;
        public final int height;
        public final int stride;
        public final Semaphore sem = new Semaphore(0);
        public static final ColourFormat colorFormat = ColourFormat.NV21;
        public ByteBuffer buffer;
        public final Bitmap thumb;

        private final int[] imageTexture = new int[3];
        private final int imageTextureWidth;
        private final FboDescriptor[] imageFbo = new FboDescriptor[2];
        private final int imageBuffer;
        private final int[] imageBufferPlaneSize = new int[2];

        private final int thumbWidth;
        private final int thumbHeight;
        private final int thumbStride;
        private final int thumbTexture;
        private final int thumbTextureWidth;
        private final FboDescriptor thumbFbo;
        private final int thumbBuffer;
        private final int thumbBufferSize;

        private final int generation;
        private boolean released;

        /**
         * Construct an {@link OutputFrame}
         * @param context Application {@link Context}
         * @param display {@link EGLDisplay} on which to create {@link OutputFrame} resources
         * @param width Width of the frame, in px
         * @param height Height of the frame, in px
         * @param colorFormat {@link ColourFormat} of this {@link OutputFrame}
         */
        public OutputFrame(Context context, int width, int height, int generation) {
            this.width = width;
            this.height = height;
            this.generation = generation;
            // N.B. Since the output UV plane is only half the width of the Y plane, we need to make
            // sure its width is a multiple of 32px (Adreno constraint for asynchronous DMA'ed
            // glReadPixels)
            imageTextureWidth = adjustTextureWidth(width / 2) * 2;
            stride = imageTextureWidth * colorFormat.bytesPerPixel;
            imageBufferPlaneSize[0] = stride * height;
            imageBufferPlaneSize[1] = imageBufferPlaneSize[0] / 2;

            int thumbSize = (int)(THUMB_SIZE_DP * context.getResources().getDisplayMetrics().density);
            float aspect = (float)width / (float)height;
            if (aspect >= 1f) {
                thumbWidth = thumbSize;
                thumbHeight = (int)(thumbSize / aspect);
            } else {
                thumbWidth = (int)(thumbSize * aspect);
                thumbHeight = thumbSize;
            }
            thumbTextureWidth = adjustTextureWidth(thumbWidth);
            thumbStride = thumbTextureWidth * ColourFormat.ABGR_8888.bytesPerPixel;
            thumbBufferSize = thumbStride * thumbHeight;
            thumb = Bitmap.createBitmap(thumbWidth, thumbHeight, Config.ARGB_8888);

            imageTexture[0] = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_R8, imageTextureWidth, height);
            imageTexture[1] = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RG8, imageTextureWidth,
                    height);
            imageTexture[2] = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RG8, imageTextureWidth / 2,
                    height / 2);
            thumbTexture = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RGBA8, thumbTextureWidth,
                    thumbHeight);

            imageFbo[0] = GLES30Helper.createFboDescriptor(0, 0, width, height,
                    GLES30.GL_TEXTURE_2D, imageTexture[0], imageTexture[1]);
            imageFbo[1] = GLES30Helper.createFboDescriptor(0, 0, width / 2, height / 2,
                    GLES30.GL_TEXTURE_2D, imageTexture[2]);
            thumbFbo = GLES30Helper.createFboDescriptor(0, 0, thumbWidth, thumbHeight,
                    GLES30.GL_TEXTURE_2D, thumbTexture);

            final int[] handles = new int[2];
            GLES30.glGenBuffers(2, handles, 0);
            imageBuffer = handles[0];
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, imageBuffer);
            GLES30.glBufferData(GLES30.GL_PIXEL_PACK_BUFFER,
                    imageBufferPlaneSize[0] + imageBufferPlaneSize[1], null, GLES30.GL_STREAM_READ);
            thumbBuffer = handles[1];
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, thumbBuffer);
            GLES30.glBufferData(GLES30.GL_PIXEL_PACK_BUFFER, thumbBufferSize, null,
                    GLES30.GL_STREAM_READ);

            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);
        }

        /**
         * Make this buffer available for use again. This must be called on the thread which owns
         * the GL context against which this {@link OutputFrame} was created.
         */
        public void recycle() {
            if (buffer != null) {
                buffer = null;
                GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, imageBuffer);
                GLES30.glUnmapBuffer(GLES30.GL_PIXEL_PACK_BUFFER);
                GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);
            }
        }

        /**
         * Call this method to release the native resources associated with an {@link OutputFrame}.
         * This must be called on the thread which owns the GL context against which this
         * {@link OutputFrame} was created.
         */
        public void release() {
            if (!released) {
                released = true;
                recycle();
                GLES30.glDeleteTextures(4, new int[] { imageTexture[0], imageTexture[1],
                        imageTexture[2], thumbTexture }, 0);
                GLES30.glDeleteBuffers(2, new int[] { imageBuffer, thumbBuffer }, 0);
                GLES30Helper.destroyFboDescriptor(imageFbo[0]);
                GLES30Helper.destroyFboDescriptor(imageFbo[1]);
                GLES30Helper.destroyFboDescriptor(thumbFbo);
            }
        }

        /**
         * Round up width to a multiple of 32px. This is required for efficient glReadPixels to a
         * PBO.
         * @param width Width (in px) of the texture
         * @return Adjusted width (in px) of the texture.
         */
        //
        private static int adjustTextureWidth(int width) {
            return ((width + 31) & (~31));
        }
    }

    private static final String TAG = "CameraPreviewView";
    private static final int NUM_OUTPUT_BUFFERS = 2;
    private static final long TIMEOUT_500_MS_IN_NS = 500 * 1000 * 1000;

    // Only accessed from renderer thread
    private EGLDisplay eglDisplay;
    private int displayWidth;
    private int displayHeight;
    private int inputTexture;
    private final float[] inputTextureTransform = new float[16];
    private final CameraFrameRenderer displayRenderer = new CameraFrameRenderer();
    private final CameraFrameRenderer cpuRenderer = new CameraFrameRenderer();
    private final CameraFrameRenderer cpuThumbRenderer = new CameraFrameRenderer();
    // N.B. Certain JB-MR2 builds seem to have a problem with thread pool cleanup. Create a custom
    // one here (rather than using Executors.newCachedThreadPool()) that has a much shorter worker
    // thread cache lifetime, which avoids the issue.
    private final ExecutorService cpuRendererExecutor = new ThreadPoolExecutor(0,
            Integer.MAX_VALUE, 500, TimeUnit.MILLISECONDS, new SynchronousQueue<Runnable>());
    private final LinkedBlockingQueue<OutputFrame> outputFrames =
            new LinkedBlockingQueue<OutputFrame>();
    private final AtomicInteger outputFramesGeneration = new AtomicInteger();

    // Accessible from multiple thread contexts (no explicit protection required)
    private final SurfaceTexture surfaceTexture;
    private final AtomicBoolean updatingTexture = new AtomicBoolean();

    // Accessed from multiple thread contexts - protect with mLock
    private final Object lock = new Object();
    private OnFrameAvailableListener frameListener;
    private int previewWidth;
    private int previewHeight;
    private int rotation;
    private boolean reconfigured;
    private boolean previewReady = false;
    /**
     * Construct a {@link CameraPreviewView}
     * @param context {@link Activity} {@link Context}
     */
    public CameraPreviewView(Context context) {
        this(context, null);
    }

    /**
     * Construct a {@link CameraPreviewView}
     * @param context {@link Activity} {@link Context}
     * @param attributes {@link AttributeSet} for styling this {@link GLSurfaceView}
     */
    public CameraPreviewView(Context context, AttributeSet attributes) {
        super(context, attributes);

        setEGLContextClientVersion(3);
        setEGLConfigChooser(false);
        setRenderer(this);
        setRenderMode(RENDERMODE_WHEN_DIRTY);

        surfaceTexture = new SurfaceTexture(0);
        surfaceTexture.detachFromGLContext();
        surfaceTexture.setOnFrameAvailableListener(this);
    }

    /**
     * Get the {@link SurfaceTexture} which can be used to stream camera frames to this object
     * @return a {@link SurfaceTexture} appropriate for
     *         {@link Camera# setPreviewTexture(SurfaceTexture)}
     */
    public SurfaceTexture getSurfaceTexture() {
        return surfaceTexture;
    }

    /**
     * Sets (or clears) the {@link OnFrameAvailableListener} which will be notified when a new
     * camera frame is available in ARGB format format
     * @param l {@link OnFrameAvailableListener} to register, or null to clear the
     *        {@link OnFrameAvailableListener}
     */
    public void setOnFrameAvailableListener(OnFrameAvailableListener l) {
        synchronized (lock) {
            frameListener = l;
        }
    }

    /**
     * Sets the rotation to apply to the preview frame. This can be used to account for camera and
     * device orientation, such that the frame displays "correctly" to the user.
     * @param rotation Rotation, in degrees clockwise, to apply to the preview frame. Must be 0, 90,
     *        180, or 270.
     */
    public void setPreviewFrameRotation(int rotation) {
        if (rotation != 0 && rotation != 90 && rotation != 180 && rotation != 270) {
            throw new IllegalArgumentException("rotation must be 0, 90, 180, or 270");
        }

        synchronized (lock) {
            if (this.rotation != rotation) {
                this.rotation = rotation;
                reconfigured = true;
            }
        }
    }

    /**
     * Sets the output size of the preview frames made available via {@link
     * OnFrameAvailableListener#onFrameAvailable(OutputFrame)}. This may be flipped from the size of
     * frames from the camera, if a rotation is applied.
     * @param width Width of the output preview frames, in px
     * @param height Height of the output preview frames, in px
     */
    public void setPreviewFrameSize(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be >= 0");
        }

        synchronized (lock) {
            if (previewWidth != width || previewHeight != height) {
                previewWidth = width;
                previewHeight = height;
                reconfigured = true;
                if(!previewReady) {
                    previewReady = true;
                    Log.d(TAG, "preview ready");
                }
            }
        }
    }

    /*
     * ---------------------- GLSurfaceView.Renderer interface ----------------------
     */

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10 gl,
            javax.microedition.khronos.egl.EGLConfig config) {
        synchronized (lock) {
            reconfigured = true;
        }

        eglDisplay = EGL14.eglGetCurrentDisplay();

        inputTexture = GLES30Helper.generateTexture(GLOESEGLImage.GL_TEXTURE_EXTERNAL_OES);

        displayRenderer.init(TextureType.TEXTURE_EXTERNAL, ColourFormat.ABGR_8888);
        cpuRenderer.init(TextureType.TEXTURE_EXTERNAL, ColourFormat.NV21);
        cpuRenderer.resized(0, 0, 0, 0); // N.B. input and output always same size
        cpuThumbRenderer.init(TextureType.TEXTURE_EXTERNAL, ColourFormat.ABGR_8888);
        cpuThumbRenderer.resized(0, 0, 0, 0); // N.B. input and output always same aspect

        surfaceTexture.attachToGLContext(inputTexture);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl, int width,
            int height) {
        GLES30.glViewport(0, 0, width, height);

        displayWidth = width;
        displayHeight = height;

        synchronized (lock) {
            reconfigured = true;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl) {
        if(!previewReady) {
            Log.d(TAG, "preview not ready, wait");
            return;
        }
        boolean doCpuRender;

        // Update input texture if necessary
        if (updatingTexture.getAndSet(false)) {
            surfaceTexture.updateTexImage();

            // Added following two lines to fix the issue on 8996
            // TODO: check if we need the following instead
            // GLES30.glBindTexture(mTexType == TextureType.TEXTURE_EXTERNAL ?
            //             GLOESEGLImage.GL_TEXTURE_EXTERNAL_OES : GLES30.GL_TEXTURE_2D, 0);
            // GLES30.glBindTexture(mTexType == TextureType.TEXTURE_EXTERNAL ?
            //             GLOESEGLImage.GL_TEXTURE_EXTERNAL_OES : GLES30.GL_TEXTURE_2D, inputTexture);


            surfaceTexture.getTransformMatrix(inputTextureTransform);
        }

        synchronized (lock) {
            // Reconfigure output resources
            if (reconfigured) {
                reconfigured = false;
                recreateRenderResources();
            }

            doCpuRender = (frameListener != null);
        }

        // Render to a CPU buffer (if necessary)
        if (doCpuRender) {
            renderCpu();
        }

        // Render to the display
        renderDisplay();
    }

    /**
     * Helper method to do rendering with the intent of copying the data to the CPUs context (in
     * ARGB format) for further processing
     */
    private void renderCpu() {
        // NOTE: The OpenGL Matrix class uses pre-transforms, so you have to read the following
        // operations backward to obtain the logical order of what is happening.
        float[] texTransform = new float[16];
        Matrix.translateM(texTransform, 0, inputTextureTransform, 0, 0.5f, 0.5f, 0f);
        Matrix.rotateM(texTransform, 0, rotation, 0f, 0f, 1f);
        Matrix.scaleM(texTransform, 0, 1f, -1f, 1f);
        Matrix.translateM(texTransform, 0, -0.5f, -0.5f, 0f);

        final OutputFrame outputFrame;
        try {
            outputFrame = outputFrames.take();
        } catch (InterruptedException e) {
            Log.i(TAG, "Interrupted while waiting for an output frame", e);
            Thread.currentThread().interrupt();
            return;
        }

        outputFrame.recycle();

        SavedFboToken token = GLES30Helper.saveFboConfiguration();

        // Render Y & UV planes, and then downsample UV plane by a factor of 2
        GLES30Helper.setFboConfiguration(GLES30.GL_FRAMEBUFFER, outputFrame.imageFbo[0]);
        cpuRenderer.render(inputTexture, texTransform);
        GLES30Helper.setFboConfiguration(GLES30.GL_DRAW_FRAMEBUFFER, outputFrame.imageFbo[1]);
        GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT1);
        GLES30.glBlitFramebuffer(0, 0, outputFrame.width, outputFrame.height, 0, 0,
                outputFrame.width / 2, outputFrame.height / 2, GLES30.GL_COLOR_BUFFER_BIT,
                GLES30.GL_LINEAR);

        // Read Y & UV planes to the image buffer
        GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT0);
        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputFrame.imageBuffer);
        GLES30Ext.glReadPixels(0, 0, outputFrame.imageTextureWidth, outputFrame.height,
                GLES30.GL_RED, GLES30.GL_UNSIGNED_BYTE, 0);
        GLES30Helper.setFboConfiguration(GLES30.GL_READ_FRAMEBUFFER, outputFrame.imageFbo[1]);
        GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT0);
        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputFrame.imageBuffer);
        GLES30Ext.glReadPixels(0, 0, outputFrame.imageTextureWidth / 2, outputFrame.height / 2,
                GLES30.GL_RG, GLES30.GL_UNSIGNED_BYTE, outputFrame.imageBufferPlaneSize[0]);
        // N.B. Technically out of spec; only GL_FRAMEBUFFER is normally allowed, but QC's GL
        // implementation supports this
        GLES30.glInvalidateFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER,
                GLES30Helper.MAX_COLOR_ATTACHMENTS, GLES30Helper.GL_COLOR_BUFFERS, 0);
        GLES30.glInvalidateFramebuffer(GLES30.GL_READ_FRAMEBUFFER,
                GLES30Helper.MAX_COLOR_ATTACHMENTS, GLES30Helper.GL_COLOR_BUFFERS, 0);

        // Render thumbnail, and read to the thumb image buffer
        GLES30Helper.setFboConfiguration(GLES30.GL_FRAMEBUFFER, outputFrame.thumbFbo);
        cpuThumbRenderer.render(inputTexture, texTransform);
        GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT0);
        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputFrame.thumbBuffer);
        GLES30Ext.glReadPixels(0, 0, outputFrame.thumbTextureWidth, outputFrame.thumbHeight,
                GLES30.GL_RGBA, GLES30.GL_UNSIGNED_BYTE, 0);
        GLES30.glInvalidateFramebuffer(GLES30.GL_FRAMEBUFFER,
                GLES30Helper.MAX_COLOR_ATTACHMENTS, GLES30Helper.GL_COLOR_BUFFERS, 0);

        final EGLSyncKHR sync = EGLKHRFenceSync.eglCreateSyncKHR(eglDisplay,
                EGLKHRFenceSync.EGL_SYNC_FENCE_KHR, null);
        GLES30.glFlush();

        GLES30Helper.restoreFboConfiguration(token);
        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);

        cpuRendererExecutor.execute(new Runnable() {
            @Override
            public void run() {
                EGLKHRFenceSync.eglClientWaitSyncKHR(eglDisplay, sync, 0, TIMEOUT_500_MS_IN_NS);
                EGLKHRFenceSync.eglDestroySyncKHR(eglDisplay, sync);

                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        renderCpuComplete(outputFrame);
                    }
                });
            }
        });
    }

    /**
     * This method is invoked in the context of the CameraPreviewView thread when the operations
     * issues by {@link #renderCpu()} are complete
     * @param outputFrame the {@link OutputFrame} rendered to
     */
    private void renderCpuComplete(final OutputFrame outputFrame) {
        // Check if the GL context has been destroyed while this render was outstanding. If so,
        // just return the outputFrame to the queue; it contains nothing useful.
        // NOTE: On some devices, even if no context is bound, eglGetCurrentContext will return an
        // EGLContext object with a 0 handle (instead of EGL14.EGL_NO_CONTEXT)
        EGLContext eglContext = EGL14.eglGetCurrentContext();
        if (eglContext == EGL14.EGL_NO_CONTEXT ||
                EGL14Helper.getEGLObjectHandle(EGL14.eglGetCurrentContext()) == 0) {
            Log.w(TAG, "GL context destroyed while render outstanding; dropping frame");
            releaseOutputFrame(outputFrame);
            return;
        }

        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputFrame.imageBuffer);
        outputFrame.buffer = (ByteBuffer)GLES30.glMapBufferRange(GLES30.GL_PIXEL_PACK_BUFFER, 0,
                outputFrame.imageBufferPlaneSize[0] + outputFrame.imageBufferPlaneSize[1],
                GLES30.GL_MAP_READ_BIT);
        // N.B. Leave buffer mapped; it will be unmapped by releaseOutputFrame

        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, outputFrame.thumbBuffer);
        ByteBuffer thumbBuf = (ByteBuffer)GLES30.glMapBufferRange(GLES30.GL_PIXEL_PACK_BUFFER, 0,
                outputFrame.thumbBufferSize, GLES30.GL_MAP_READ_BIT);
        ImageBufferUtils.populateBitmapFromABGR8888(outputFrame.thumb, thumbBuf,
                outputFrame.thumbWidth, outputFrame.thumbHeight, outputFrame.thumbStride);
        GLES30.glUnmapBuffer(GLES30.GL_PIXEL_PACK_BUFFER);

        GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);

        // Notify the frame listener (if still valid) that a frame is ready
        final boolean wait;
        synchronized (lock) {
            wait = (frameListener != null && frameListener.onFrameAvailable(outputFrame));
        }

        if (wait) {
            // Frame listener consumed the frame, wait for it to finish with it
            cpuRendererExecutor.execute(new Runnable() {
                @Override
                public void run() {
                    try {
                        outputFrame.sem.acquire();
                    } catch (InterruptedException e) {
                        Log.i(TAG, "Interrupted while waiting for listener to finish", e);
                        Thread.currentThread().interrupt();
                    }

                    releaseOutputFrame(outputFrame);
                }
            });
        } else {
            releaseOutputFrame(outputFrame);
        }
    }

    /**
     * Either return the {@link OutputFrame} to the queue or release it, as appropriate.
     * <br><i>NOTE: this may be called in arbitrary thread contexts</i>
     * @param outputFrame the {@link OutputFrame} which is no longer used
     */
    private void releaseOutputFrame(final OutputFrame outputFrame) {
        if (outputFrame.generation == outputFramesGeneration.get()) {
            outputFrames.add(outputFrame);
        } else {
            // outputFrame is out of date, release it
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    outputFrame.release();
                }
            });
        }
    }

    /**
     * Helper method to do rendering of the preview frame to the display
     */
    private void renderDisplay() {
        // NOTE: The OpenGL Matrix class uses pre-transforms, so you have to read the following
        // operations backward to obtain the logical order of what is happening.
        float[] texTransform = new float[16];
        Matrix.translateM(texTransform, 0, inputTextureTransform, 0, 0.5f, 0.5f, 0f);
        Matrix.rotateM(texTransform, 0, rotation, 0f, 0f, 1f);
        Matrix.translateM(texTransform, 0, -0.5f, -0.5f, 0f);

        // Render to the display
        displayRenderer.render(inputTexture, texTransform);
    }

    /**
     * Recalculate and recreate rendering resources when a critical parameter changes (ie frame
     * size, rotation, etc).
     */
    private void recreateRenderResources() {
        displayRenderer.resized(displayWidth, displayHeight, previewWidth, previewHeight);

        // N.B. Make sure to increment frame generation before clearing queue, such that no new
        // frames will be returned to it while we clean it up
        outputFramesGeneration.incrementAndGet();
        for (OutputFrame outputFrame : outputFrames) {
            outputFrame.release();
        }
        outputFrames.clear();
        for (int i = 0; i < NUM_OUTPUT_BUFFERS; i++) {
            outputFrames.add(new OutputFrame(getContext(), previewWidth, previewHeight,
                    outputFramesGeneration.get()));
        }
    }

    /*
     * -------------- SurfaceTexture.OnFrameAvailableListener interface -------------
     */

    /**
     * {@inheritDoc}
     */
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        updatingTexture.set(true);
        requestRender();
    }
}
