/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.khronos;

import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;
import android.opengl.GLES30;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

/**
 * This class represents an abstract thread with an associated OpenGL ES 3 context. This thread
 * receives frames via a {@link SurfaceTexture}, and allows the user to optionally specify an output
 * {@link SurfaceTexture} to render into.
 */
public abstract class GLESContextThread implements AutoCloseable {
    protected enum State { NOT_STARTED, STARTED, CLOSED }

    private static final String TAG = "SocialCamera";
    private static final int MSG_INITIALIZED = 0x2000;
    private static final int MSG_CLOSE = 0x2001;
    private static final int MSG_SET_INPUT_SURFACE_TEXTURE_SIZE = 0x2002;
    private static final int MSG_SET_OUTPUT_SURFACE_TEXTURE = 0x2003;
    private static final int MSG_RENDER = 0x2004;

    private final HandlerThread glesThread;

    // All accesses to these fields must be from synchronized contexts
    private State state = State.NOT_STARTED;
    private Handler glesThreadHandler; // valid after STARTED
    private int paramInputTextureWidth;
    private int paramInputTextureHeight;
    private SurfaceTexture paramOutputSurfaceTexture;

    // These fields are owned by the GLES thread, and protected by the initialized countdown latch;
    // after it reaches 0, they may be read from other threads.
    private final CountDownLatch threadInitialized = new CountDownLatch(1);
    private SurfaceTexture inputSurfaceTexture;

    // This CountDownLatch is used to indicate when GLES teardown has proceeded to a point where the
    // caller of close() may continue executing
    private final CountDownLatch threadClosed = new CountDownLatch(1);

    // These fields are private to the GLES thread
    private EGLDisplay eglDisplay;
    private EGLConfig eglConfig;
    private EGLContext eglContext;
    private EGLSurface eglSurface;
    private int inputTexture;
    private int inputTextureWidth;
    private int inputTextureHeight;
    private SurfaceTexture outputSurfaceTexture;
    private int outputSurfaceWidth;
    private int outputSurfaceHeight;
    private final int[] workingBuf = new int[2];

    /**
     * Construct a new {@link GLESContextThread}
     * @param name the name of the {@link GLESContextThread}, for debug purposes
     */
    protected GLESContextThread(String name) {
        glesThread = new HandlerThread(name);
    }

    /**
     * Start the {@link GLESContextThread}. This may only be invoked once for any given thread.
     */
    public void start() {
        synchronized (this) {
            if (state != State.NOT_STARTED) {
                throw new IllegalStateException("start has already been invoked for '" +
                        glesThread.getName() + "'");
            }

            state = State.STARTED;
            glesThread.start();
            final Looper glesThreadLooper = glesThread.getLooper();
            glesThreadHandler = new Handler(glesThreadLooper, handlerThreadCallback);
            glesThreadHandler.sendEmptyMessage(MSG_INITIALIZED);

            onStarted(glesThreadLooper);
        }
    }

    /**
     * Close the current {@link GLESContextThread} (if it has been {@link #start()}ed)
     */
    @Override
    public void close() {
        final Handler localHandler;

        synchronized (this) {
            if (state == State.CLOSED) {
                Log.w(TAG, "close has already been invoked for thread '" +
                        glesThread.getName() + "'");
                return;
            } else if (state == State.NOT_STARTED) {
                return;
            }

            onClosed();

            state = State.CLOSED;
            localHandler = glesThreadHandler;
            glesThreadHandler = null;
        }

        localHandler.sendEmptyMessage(MSG_CLOSE);

        try {
            threadClosed.await(1, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            Log.w(TAG, "Interrupted while waiting for thread '" + glesThread.getName() +
                    "' to close");
            Thread.currentThread().interrupt();
        }
    }

    /**
     * Get the input {@link SurfaceTexture}, from which the {@link GLESContextThread} will receive
     * textures to be processed. This method will block until the {@link SurfaceTexture} is
     * available (which should happen very quickly after {@link #start()} is invoked).
     * @return the input {@link SurfaceTexture}, or null if {@link #start()} has not been invoked
     */
    public SurfaceTexture getInputSurfaceTexture() {
        synchronized (this) {
            if (state != State.STARTED) {
                Log.w(TAG, "getInputSurfaceTexture invoked when thread '" + glesThread.getName() +
                        "' not started");
                return null;
            }
        }

        try {
            if (!threadInitialized.await(1, TimeUnit.SECONDS)) {
                Log.w(TAG, "Thread '" + glesThread.getName() + "' failed to initialize within 1s");
            }
        } catch (InterruptedException e) {
            Log.w(TAG, "Interrupted while waiting for thread '" + glesThread.getName() +
                    "' to finish initializing", e);
            Thread.currentThread().interrupt();
        }

        return inputSurfaceTexture;
    }

    /**
     * Sets the size of the input {@link SurfaceTexture}. This may be used to specify native
     * dimensions for the input {@link SurfaceTexture} data, if appropriate.
     * @param width width of the input {@link SurfaceTexture}, in px
     * @param height height of the input {@link SurfaceTexture}, in px
     * @throws IllegalArgumentException if width or height are < 0
     */
    public void setInputSurfaceTextureSize(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be non-negative integers");
        }

        synchronized (this) {
            switch (state) {
                case NOT_STARTED:
                    paramInputTextureWidth = width;
                    paramInputTextureHeight = height;
                    break;

                case STARTED:
                    glesThreadHandler.obtainMessage(MSG_SET_INPUT_SURFACE_TEXTURE_SIZE,
                            width, height).sendToTarget();
                    break;

                case CLOSED:
                    Log.w(TAG, "setInputSurfaceTextureSize invoked after thread '" +
                            glesThread.getName() + "' has been closed");
                    break;

                default:
                    throw new IllegalStateException("Unexpected state: " + state);
            }
        }
    }

    /**
     * Sets the output {@link SurfaceTexture}, which will be used as the default {@link EGLSurface}
     * for rendering. If not set, or set to null, a dummy {@link SurfaceTexture} will be used as the
     * {@link EGLSurface}
     * @param st the {@link SurfaceTexture} for rendering into, or null to clear the output
     *           {@link SurfaceTexture} and render into a dummy {@link SurfaceTexture}
     */
    public void setOutputSurfaceTexture(SurfaceTexture st) {
        synchronized (this) {
            switch (state) {
                case NOT_STARTED:
                    paramOutputSurfaceTexture = st;
                    break;

                case STARTED:
                    glesThreadHandler.obtainMessage(MSG_SET_OUTPUT_SURFACE_TEXTURE, st)
                            .sendToTarget();
                    break;

                case CLOSED:
                    Log.w(TAG, "setOutputSurfaceTexture invoked after thread '" +
                            glesThread.getName() + "' has been closed");
                    break;

                default:
                    throw new IllegalStateException("Unexpected state: " + state);
            }
        }
    }

    /**
     * Request that a render operation be scheduled (via an invocation of
     * {@link #onRender(int, float[])} in the GLES thread context). This will be automatically
     * invoked each time a new image is available via the input {@link SurfaceTexture}. Subclasses
     * may also invoke this to request that rendering be scheduled.
     * @param forceConcurrentRenderRequests if true, force a render to be scheduled even if one is
     *                                      already pending
     */
    protected void requestRender(boolean forceConcurrentRenderRequests) {
        synchronized (this) {
            if (state != State.STARTED) {
                Log.v(TAG, "Render requested when not running; ignoring");
                return;
            }

            if (forceConcurrentRenderRequests || !glesThreadHandler.hasMessages(MSG_RENDER)) {
                glesThreadHandler.sendEmptyMessage(MSG_RENDER);
            }
        }
    }

    /**
     * Get the current {@link State} of the {@link GLESContextThread}. Note that this must be
     * invoked while synchronized on the GLESContextThread instance, otherwise the returned
     * {@link State} may not be change at any time.
     * @return the current {@link State}
     */
    protected State getState() {
        return state;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        if (state == State.STARTED) {
            close();
        }
    }

    /**
     * This method will be invoked in an arbitrary thread context, synchronized on this instance of
     * {@link GLESContextThread}, directly after the {@link GLESContextThread} has started. For the
     * duration of the invocation of this method, the state of {@link GLESContextThread} is
     * guaranteed to be {@link State#STARTED}.
     * @param glesThreadLooper the {@link Looper} for the GLES thread
     */
    protected abstract void onStarted(Looper glesThreadLooper);

    /**
     * This method will be invoked in an arbitrary thread context, synchronized on this instance of
     * {@link GLESContextThread}, directly before the {@link GLESContextThread} is stopped. For the
     * duration of the invocation of this method, the state of {@link GLESContextThread} is
     * guaranteed to be {@link State#STARTED}.
     */
    protected abstract void onClosed();

    /**
     * This method will be invoked in the context of the GLES thread when the OpenGL ES 3 context
     * has been created and made current. This will be the first method invoked in the GLES thread
     * context, and as such can be used for both general and GLES object initialization
     * @param textureType the {@link TextureType} of the input texture which will be provided to
     *                    {@link #onRender(int, float[])}
     */
    protected abstract void onContextCreated(TextureType textureType);

    /**
     * This method will be invoked in the context of the GLES thread directly before the OpenGL ES 3
     * context is destroyed. This will be the last method invoked in the GLES thread context, and as
     * such should be used for cleanup. It is unnecessary to clean up GL objects, as the context to
     * which they are bound will be destroyed immediately after returning from this method.
     */
    protected abstract void onContextRelease();

    /**
     * This method will be invoked in the context of the GLES thread whenever the size of the input
     * texture (as provided to {@link #onRender(int, float[])}) changes. It is guaranteed to be
     * invoked at least once before {@link #onRender(int, float[])} is invoked, but otherwise may be
     * invoked at any time.
     * @param width the width of the input texture, in px
     * @param height the height of the input texture, in px
     */
    protected abstract void onInputTextureSizeChanged(int width, int height);

    /**
     * This method will be invoked in the context of the GLES thread whenever the size of the output
     * framebuffer changes. It is guaranteed to be invoked at least once before
     * {@link #onRender(int, float[])} is invoked, but otherwise may be invoked at any time. The
     * GL viewport will be set to these dimensions before each invocation of
     * {@link #onInputTextureSizeChanged(int, int)}.
     * @param width the width of the output framebuffer, in px
     * @param height the height of the output framebuffer, in px
     */
    protected abstract void onFramebufferSizeChanged(int width, int height);

    /**
     * This method will be invoked in the context of the GLES thread whenever
     * {@link #requestRender(boolean)} is invoked. The default framebuffer will be bound, and
     * {@link EGL14#eglSwapBuffers(EGLDisplay, EGLSurface)} will be invoked immediately after this
     * method returns. Subclasses should perform their rendering here.
     * @param texture a GL texture handle (of the {@link TextureType} specified in
     *                {@link #onContextCreated(TextureType)}) containing the most recent input image
     * @param transform a 4x4 affine transform (in column-major order) to be applied to the texture
     *                  coordinates used when sampling texture
     */
    protected abstract void onRender(int texture, float[] transform);

    // N.B. Must be invoked in the context of the GLES thread
    private void createGLES30EglContext() {
        // N.B. This config won't actually be used for rendering; it points to a dummy surface.
        final int[] CONFIG_ATTRIBUTES = new int[] {
                EGL14.EGL_RENDERABLE_TYPE, EGLExt.EGL_OPENGL_ES3_BIT_KHR,
                EGL14.EGL_SURFACE_TYPE, EGL14.EGL_WINDOW_BIT,
                EGL14.EGL_COLOR_BUFFER_TYPE, EGL14.EGL_RGB_BUFFER,
                EGL14.EGL_RED_SIZE, 8,
                EGL14.EGL_GREEN_SIZE, 8,
                EGL14.EGL_BLUE_SIZE, 8,
                EGL14.EGL_ALPHA_SIZE, 0,
                EGL14.EGL_NONE
        };

        final int[] CONTEXT_ATTRIBUTES = new int[] {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL14.EGL_NONE
        };

        eglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if (eglDisplay == null || eglDisplay.equals(EGL14.EGL_NO_DISPLAY)) {
            throw new RuntimeException("Failed initializing EGL display");
        }

        int[] version = new int[2];
        if (!EGL14.eglInitialize(eglDisplay, version, 0, version, 1)) {
            throw new RuntimeException("Failed initializing EGL display connection");
        }

        int[] numConfigs = new int[1];
        EGLConfig[] configs = new EGLConfig[1];
        if (!EGL14.eglChooseConfig(eglDisplay, CONFIG_ATTRIBUTES, 0, configs, 0, configs.length,
                numConfigs, 0)) {
            throw new RuntimeException("Failed querying configs");
        } else if (numConfigs[0] != 1) {
            throw new RuntimeException("No matching EGL configurations found");
        } else {
            eglConfig = configs[0];
        }

        eglContext = EGL14.eglCreateContext(eglDisplay, eglConfig, EGL14.EGL_NO_CONTEXT,
                CONTEXT_ATTRIBUTES, 0);
        if (eglContext == null || eglContext.equals(EGL14.EGL_NO_CONTEXT)) {
            throw new RuntimeException("Failed creating OpenGL ES3 context");
        }
    }

    // N.B. Must be invoked in the context of the GLES thread
    private void releaseGLES30EglContext() {
        releaseEglSurface();
        EGL14.eglDestroyContext(eglDisplay, eglContext);
        eglContext = null;
        eglConfig = null;
        eglDisplay = null;
    }

    // N.B. Must be invoked in the context of the GLES thread
    private void createSurfaceTextureEglSurface(SurfaceTexture st) {
        final int[] EMPTY_ATTRIBUTES = new int[] {
                EGL14.EGL_NONE
        };

        if (outputSurfaceTexture != null) {
            releaseEglSurface();
        }

        if (st != null) {
            outputSurfaceTexture = st;
        } else {
            outputSurfaceTexture = new SurfaceTexture(0);
            outputSurfaceTexture.detachFromGLContext();
            outputSurfaceTexture.setDefaultBufferSize(1, 1);
        }

        eglSurface = EGL14.eglCreateWindowSurface(eglDisplay, eglConfig, st, EMPTY_ATTRIBUTES, 0);
        outputSurfaceWidth = -1;
        outputSurfaceHeight = -1;
        EGL14.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    }

    // N.B. Must be invoked in the context of the GLES thread
    private void releaseEglSurface() {
        EGL14.eglMakeCurrent(eglDisplay, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE,
                EGL14.EGL_NO_CONTEXT);
        EGL14.eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = null;
        outputSurfaceTexture = null;
    }

    // N.B. Must be invoked in the context of the GLES thread
    private void createGLESResources() {
        inputTexture = GLES30Helper.generateTexture(TextureType.TEXTURE_EXTERNAL.textureTarget);
        inputSurfaceTexture = new SurfaceTexture(inputTexture);
        inputSurfaceTexture.setOnFrameAvailableListener(inputSurfaceTextureListener);
        inputSurfaceTexture.setDefaultBufferSize(0, 0);
    }

    private final Handler.Callback handlerThreadCallback = new Handler.Callback() {
        final float[] inputSurfaceTextureTransform = new float[16];

        @Override
        public boolean handleMessage(Message msg) {
            boolean handled = true;

            switch (msg.what) {
                case MSG_INITIALIZED:
                    Log.v(TAG, "Thread '" + glesThread.getName() + "' starting");

                    // Grab initial values specified before start
                    final SurfaceTexture st;
                    synchronized (GLESContextThread.this) {
                        inputTextureWidth = paramInputTextureWidth;
                        inputTextureHeight = paramInputTextureHeight;
                        st = paramOutputSurfaceTexture;
                        paramOutputSurfaceTexture = null;
                    }

                    createGLES30EglContext();
                    createSurfaceTextureEglSurface(st);
                    createGLESResources();
                    threadInitialized.countDown();

                    onContextCreated(TextureType.TEXTURE_EXTERNAL);
                    onInputTextureSizeChanged(inputTextureWidth, inputTextureHeight);
                    break;

                case MSG_CLOSE:
                    // N.B. onContextRelease may block, and we should free up the output EGL surface
                    // immediately, in case it will be used for other rendering
                    setOutputSurfaceTexture(null);
                    GLES30.glFinish();
                    threadClosed.countDown();

                    onContextRelease();

                    // N.B. No need to manually destroy GLES resources; destroying the context will
                    // take care of it
                    releaseGLES30EglContext();
                    inputSurfaceTexture.release();
                    inputSurfaceTexture = null;

                    //noinspection ConstantConditions
                    Looper.myLooper().quit(); // N.B. No messages should have been sent after close
                    Log.v(TAG, "Thread '" + glesThread.getName() + "' stopped");
                    break;

                case MSG_SET_INPUT_SURFACE_TEXTURE_SIZE:
                    if (inputTextureWidth != msg.arg1 || inputTextureHeight != msg.arg2) {
                        inputTextureWidth = msg.arg1;
                        inputTextureHeight = msg.arg2;
                        onInputTextureSizeChanged(inputTextureWidth, inputTextureHeight);
                    }
                    break;

                case MSG_SET_OUTPUT_SURFACE_TEXTURE:
                    if (outputSurfaceTexture != msg.obj) {
                        createSurfaceTextureEglSurface(outputSurfaceTexture);
                    }
                    break;

                case MSG_RENDER:
                    // Check if the output surface size has changed since we last rendered
                    EGL14.eglQuerySurface(eglDisplay, eglSurface, EGL14.EGL_WIDTH, workingBuf, 0);
                    EGL14.eglQuerySurface(eglDisplay, eglSurface, EGL14.EGL_HEIGHT, workingBuf, 1);
                    if (workingBuf[0] != outputSurfaceWidth ||
                            workingBuf[1] != outputSurfaceHeight) {
                        outputSurfaceWidth = workingBuf[0];
                        outputSurfaceHeight = workingBuf[1];
                        onFramebufferSizeChanged(outputSurfaceWidth, outputSurfaceHeight);
                        GLES30.glViewport(0, 0, outputSurfaceWidth, outputSurfaceHeight);
                    }

                    inputSurfaceTexture.updateTexImage();
                    inputSurfaceTexture.getTransformMatrix(inputSurfaceTextureTransform);
                    onRender(inputTexture, inputSurfaceTextureTransform);
                    EGL14.eglSwapBuffers(eglDisplay, eglSurface);
                    break;

                default:
                    handled = false;
                    break;
            }

            return handled;
        }
    };

    private final SurfaceTexture.OnFrameAvailableListener inputSurfaceTextureListener =
            new SurfaceTexture.OnFrameAvailableListener() {
        @Override
        public void onFrameAvailable(SurfaceTexture surfaceTexture) {
            requestRender(false);
        }
    };
}
