/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import android.opengl.GLES30;
import android.util.Log;

import com.qualcomm.qti.khronos.EGL15Helper;
import com.qualcomm.qti.khronos.EGLKHRGLImage;
import com.qualcomm.qti.khronos.EGLSyncKHRContainer;
import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.EGLKHRImageBase;
import com.qualcomm.qti.khronos.EGL15Helper.EGLContextToken;
import com.qualcomm.qti.khronos.EGLKHRImageBase.EGLImageKHR;
import com.qualcomm.qti.khronos.GLES30Helper.SavedFboToken;
import com.qualcomm.qti.objecttracker.cameraengine.Frame;
import com.qualcomm.qti.objecttracker.cameraengine.FrameSink;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.ThreadedFrameSink;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.InputTextureRenderer;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TextureFormat;

/**
 * This class implements a {@link FrameSink} which receives raw frames from the camera, and
 * translates them into OpenGL textures for consumption by renderer(s).
 */
public class GLFrameTranslator extends ThreadedFrameSink {
    private static final String TAG = "GLFrameTranslator";
    private static final int NUM_INPUT_TEXTURES = 2;
    private static final int NUM_TRANSLATED_TEXTURES = 4;
    private static final float[] TRANSFORM_MIRROR_VERT = new float[] {
        1f, 0f, 0f, 0f, 0f, -1f, 0f, 0f, 0f, 0f, 1f, 0f, 0f, 1f, 0f, 1f };
    private static final long SYNC_TIMEOUT = TimeUnit.NANOSECONDS.convert(500,
            TimeUnit.MILLISECONDS);
    private static final long TRANSLATION_TEXTURE_TIMEOUT = TimeUnit.MILLISECONDS.toMillis(500);

    // These fields are only accessed from the GLFrameTranslator thread
    private EGLContextToken eglToken;
    private InputTextureRenderer renderer;
    private final InputTexture[] inputTextures = new InputTexture[NUM_INPUT_TEXTURES];
    private int inputTextureIdx;

    // These fields may be accessed in arbitrary thread contexts; protect all accesses with lock
    private final Object lock = new Object();
    private final HashMap<EGLImageKHR, TranslatedTexture> records =
            new HashMap<EGLImageKHR, TranslatedTexture>();
    private final LinkedBlockingQueue<TranslatedTexture> availableRecords =
            new LinkedBlockingQueue<TranslatedTexture>();
    private final ArrayList<GLFrameSink> sinks = new ArrayList<GLFrameSink>();

    // These fields are written only by the GLFrameTranslator thread. They may be safely read there
    // without synchronization, but all other accesses require protection via lock.
    private boolean running;
    private CoordinateSpace cameraSpace;

    /**
     * Construct a new {@link GLFrameTranslator}
     */
    public GLFrameTranslator() {
        super(TAG);
    }

    /**
     * Register a {@link GLFrameSink}. This object will receive notifications of events from this
     * {@link GLFrameTranslator}. If the {@link GLFrameTranslator} is currently running, the
     * sink's {@link GLFrameSink#onInitialize(int, int, int)} method will be invoked immediately.
     * @param sink {@link GLFrameSink} to register
     * @throws IllegalArgumentException if sink is null
     */
    public void addGLFrameSink(GLFrameSink sink) {
        if (sink == null) {
            throw new IllegalArgumentException("sink cannot be null");
        }

        boolean notify;
        CoordinateSpace cameraSpace;

        synchronized (lock) {
            sinks.add(sink);
            notify = running;
            cameraSpace = this.cameraSpace;
        }

        if (notify) {
            sink.onInitialize(cameraSpace, TextureFormat.RGBA);
        }
    }

    /**
     * Unregister a {@link GLFrameSink}. The object will no longer receive notifications of events
     * from this {@link GLFrameTranslator}. If the {@link GLFrameTranslator} is currently running,
     * the sink's {@link GLFrameSink#onRelease()} method will be invoked immediately.
     * @param sink {@link GLFrameSink} to unregister
     * @throws IllegalArgumentException if sink is null
     */
    public void removeGLFrameSink(GLFrameSink sink) {
        if (sink == null) {
            throw new IllegalArgumentException("sink cannot be null");
        }

        boolean notify;

        synchronized (lock) {
            sinks.remove(sink);
            notify = running;
        }

        if (notify) {
            sink.onRelease();
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void doStart(CoordinateSpace cameraSpace) {
        Log.v(TAG, "GL frame translator started, frame size=(" + cameraSpace.width + "," +
                cameraSpace.height + ")");

        eglToken = EGL15Helper.initializeGLES30EGLContext();
        EGL15Helper.makeEGLContextCurrent(eglToken);

        synchronized (lock) {
            this.cameraSpace = cameraSpace;
            createRenderResources();
            running = true;

            for (GLFrameSink sink : sinks) {
                sink.onInitialize(cameraSpace, TextureFormat.RGBA);
            }
        }
    }

    /** {@inheritDoc} */
    @Override
    protected void doStop() {
        synchronized (lock) {
            for (GLFrameSink sink : sinks) {
                sink.onRelease();
            }

            running = false;
            destroyRenderResources();
        }

        EGL15Helper.releaseEGLContext(eglToken);
        eglToken = null;

        Log.v(TAG, "GL frame translator stopped");
    }

    /** {@inheritDoc} */
    @Override
    protected void doFrame(Frame frame) {
        final long timestamp = frame.timestamp;

        // Select the current input texture, and then advance to the next one
        InputTexture inputTexture = inputTextures[inputTextureIdx++];
        if (inputTextureIdx >= inputTextures.length) {
            inputTextureIdx = 0;
        }
        inputTexture.waitUntilAvailable();

        // Copy the frame data into the YUV textures, and release the frame for reuse
        // NOTE: this uses PBOs instead of a straight glTexSubImage2D for a ~33% performance boost
        // NOTE: we handle buffer synchronization using a GL sync object, so mapping the buffers
        // with GL_MAP_UNSYNCHRONIZED_BIT is safe and removes any chance of extra driver overhead.
        ByteBuffer loadBuffer;
        GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, inputTexture.loadBuffer[0]);
        loadBuffer = (ByteBuffer)GLES30.glMapBufferRange(GLES30.GL_PIXEL_UNPACK_BUFFER, 0,
                cameraSpace.width * cameraSpace.height,
                GLES30.GL_MAP_WRITE_BIT | GLES30.GL_MAP_UNSYNCHRONIZED_BIT);
        loadBuffer.put(frame.imageDataNV21, 0, cameraSpace.width * cameraSpace.height);
        GLES30.glUnmapBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, inputTexture.loadTexture[0]);
        GLES30.glTexSubImage2D(GLES30.GL_TEXTURE_2D, 0, 0, 0, cameraSpace.width, cameraSpace.height,
                GLES30.GL_RED, GLES30.GL_UNSIGNED_BYTE, null);
        GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, inputTexture.loadBuffer[1]);
        loadBuffer = (ByteBuffer)GLES30.glMapBufferRange(GLES30.GL_PIXEL_UNPACK_BUFFER, 0,
                cameraSpace.width * cameraSpace.height / 2,
                GLES30.GL_MAP_WRITE_BIT | GLES30.GL_MAP_UNSYNCHRONIZED_BIT);
        loadBuffer.put(frame.imageDataNV21, cameraSpace.width * cameraSpace.height,
                cameraSpace.width * cameraSpace.height / 2);
        GLES30.glUnmapBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, inputTexture.loadTexture[1]);
        GLES30.glTexSubImage2D(GLES30.GL_TEXTURE_2D, 0, 0, 0, cameraSpace.width / 2,
                cameraSpace.height / 2, GLES30.GL_RG, GLES30.GL_UNSIGNED_BYTE, null);
        GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, 0);

        frame.release();
        frame = null;

        TranslatedTexture rec = null;
        try {
            do {
                rec = availableRecords.poll(TRANSLATION_TEXTURE_TIMEOUT, TimeUnit.MILLISECONDS);
                if (rec == null) {
                    Log.w(TAG, "GLFrameTranslator stalled for a significant period - probable " +
                            "leak of GLFrame resource");
                }
            } while (rec == null);
        } catch (InterruptedException e) {
            Log.i(TAG, "Interrupted while waiting for an available translation record");
            Thread.currentThread().interrupt();
            return;
        }

        // Wait for intermediate texture within rec to be available. It may be referenced by a
        // rendering operation still pending on the GPU in another context.
        rec.syncContainer.awaitComplete();

        // Create a texture for the load image, and render it to an intermediate texture. This
        // implicitly converts from NV21 to RGB, which makes future texture sample operations
        // significantly more efficient.
        renderer.setInputTexture(TRANSFORM_MIRROR_VERT, inputTexture.loadTexture);
        SavedFboToken fboToken = GLES30Helper.setFboConfiguration(rec.intermediateFbo, 0, 0,
                cameraSpace.width, cameraSpace.height);
        GLES30.glClearColor(0f, 0f, 0f, 1f);
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
        renderer.render();
        GLES30Helper.restoreFboConfiguration(fboToken);

        // Insert syncs for both InputTexture and TranslatedTexture, for waiting until free
        inputTexture.insertSync();
        rec.syncContainer.insertSync();
        GLES30.glFlush();

        // Dispatch the rendered GL frame to all sinks
        GLFrame glFrame = new GLFrame(timestamp, rec.intermediateImage, TextureFormat.RGBA,
                rec.syncContainer, this);
        synchronized (lock) {
            glFrame.retain(sinks.size());
            for (GLFrameSink sink : sinks) {
                sink.onFrame(glFrame);
            }
        }
        glFrame.release(); // release our reference to the GLFrame
    }

    /**
     * Invoke when a {@link GLFrame} is no longer being used by any {@link GLFrameSink}s. It will
     * take the necessary steps to recycle or clean up the {@link GLFrame}.
     * <p><i>NOTE: may be invoked in arbitrary thread contexts</i>
     * @param frame a {@link GLFrame} to recycle or clean up
     */
    /*package*/ void releaseGLFrame(GLFrame frame) {
        synchronized (lock) {
            TranslatedTexture rec = records.get(frame.image);
            if (rec != null) {
                availableRecords.add(rec);
            }
        }
    }

    // N.B. must be invoked with lock held, in the context of the GLFrameTranslator thread
    private void createRenderResources() {
        // Should always be clear, but horrible things will happen if they aren't, so clear again
        records.clear();
        availableRecords.clear();

        for (int i = 0; i < inputTextures.length; i++) {
            inputTextures[i] = new InputTexture(cameraSpace.width, cameraSpace.height);
        }
        inputTextureIdx = 0;

        for (int i = 0; i < NUM_TRANSLATED_TEXTURES; i++) {
            TranslatedTexture rec = new TranslatedTexture(cameraSpace.width, cameraSpace.height);
            records.put(rec.intermediateImage, rec);
            availableRecords.add(rec);
        }

        renderer = new InputTextureRenderer(cameraSpace, cameraSpace, TextureFormat.NV21);
        renderer.init();
    }

    // N.B. must be invoked with lock held, in the context of the GLFrameTranslator thread
    private void destroyRenderResources() {
        renderer.release();
        renderer = null;

        for (TranslatedTexture rec : records.values()) {
            rec.release();
        }

        for (int i = 0; i < inputTextures.length; i++) {
            inputTextures[i].release();
            inputTextures[i] = null;
        }

        records.clear();
        availableRecords.clear();
    }

    private static class InputTexture {
        final int loadBuffer[] = new int[2];
        final int loadTexture[] = new int[2];

        private boolean released;
        private long sync;

        InputTexture(int width, int height) {
            GLES30.glGenBuffers(loadBuffer.length, loadBuffer, 0);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, loadBuffer[0]);
            GLES30.glBufferData(GLES30.GL_PIXEL_UNPACK_BUFFER, width * height, null,
                    GLES30.GL_STREAM_DRAW);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, loadBuffer[1]);
            GLES30.glBufferData(GLES30.GL_PIXEL_UNPACK_BUFFER, width * height / 2, null,
                    GLES30.GL_STREAM_DRAW);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, 0);
            loadTexture[0] = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, loadTexture[0]);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_R8, width, height);
            loadTexture[1] = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, loadTexture[1]);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RG8, width / 2, height / 2);
        }

        void insertSync() {
            if (released) {
                throw new IllegalStateException("InputTexture has been released");
            }

            if (sync != 0) {
                GLES30.glDeleteSync(sync);
            }

            sync = GLES30.glFenceSync(GLES30.GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }

        void waitUntilAvailable() {
            if (sync == 0) {
                return;
            }

            GLES30.glClientWaitSync(sync, 0, SYNC_TIMEOUT);
            GLES30.glDeleteSync(sync);
            sync = 0;
        }

        void release() {
            if (released) {
                return;
            }

            if (sync != 0) {
                GLES30.glDeleteSync(sync);
                sync = 0;
            }

            GLES30.glDeleteBuffers(loadBuffer.length, loadBuffer, 0);
            GLES30.glDeleteTextures(loadTexture.length, loadTexture, 0);
            Arrays.fill(loadBuffer, 0);
            Arrays.fill(loadTexture, 0);
        }
    }

    private class TranslatedTexture {
        final int intermediateTexture;
        final int intermediateFbo;
        final EGLImageKHR intermediateImage;
        final EGLSyncKHRContainer syncContainer;

        private boolean released;

        TranslatedTexture(int width, int height) {
            intermediateTexture = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, intermediateTexture);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RGB8, width, height);
            intermediateFbo = GLES30Helper.generateFbo(GLES30.GL_TEXTURE_2D, intermediateTexture,
                    0, 0, 0, 0);
            intermediateImage = EGLKHRImageBase.eglCreateImageKHR(eglToken.display,
                    eglToken.context, EGLKHRGLImage.EGL_GL_TEXTURE_2D_KHR, intermediateTexture,
                    null);
            syncContainer = new EGLSyncKHRContainer(eglToken.display);
        }

        // N.B. This method must be invoked in the thread context which created this object
        void release() {
            GLES30.glDeleteFramebuffers(1, new int[] { intermediateFbo }, 0);
            GLES30.glDeleteTextures(1, new int[] { intermediateTexture }, 0);
            releaseEGL();
        }

        // N.B. This method may be invoked in an arbitrary thread context
        void releaseEGL() {
            if (!released) {
                released = true;
                EGLKHRImageBase.eglDestroyImageKHR(eglToken.display, intermediateImage);
            }
        }

        @Override
        protected void finalize() throws Throwable {
            super.finalize();
            releaseEGL();
        }
    }
}
