/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TextureFormat;

import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;
import android.view.View;
import android.view.View.OnAttachStateChangeListener;

/**
 * This class implements a {@link Renderer} for the Object Tracker Demo app
 */
public class ObjectTrackerRenderer implements Renderer {
    private static final String TAG = "ObjectTrackerRenderer";

    private final GLSurfaceView view;
    private final RendererFactory rendererFactory;
    private final ObjectTracker tracker;

    // These fields are only accessed from the GLSurfaceView thread
    private boolean frameSinkRunning;
    private int frameTexture;
    private int trackedObjectsTexture;
    private int rendererGeneration;
    private com.qualcomm.qti.objecttrackerdemo.renderer.primitives.Renderer renderer;
    private GLFrameRendererAdapter rendererAdapter;
    private boolean configurationChanged;
    private CoordinateSpace cameraSpace = CoordinateSpace.EMPTY;
    private CoordinateSpace displaySpace = CoordinateSpace.EMPTY;
    private TextureFormat frameTextureFormat;

    /**
     * Construct an {@link ObjectTrackerRenderer}
     * @param glSurfaceView {@link GLSurfaceView} which will host this {@link Renderer}
     * @param translator {@link GLFrameTranslator} from which this object will receive frames to
     *      render
     * @param rendererFactory A {@link RendererFactory} which can be used to construct renderers
     * @param tracker An {@link ObjectTracker} from which to receive tracked object updates
     * @throws IllegalArgumentException if glSurfaceView, translator, rendererFactory, or tracker
     *      are null
     */
    private ObjectTrackerRenderer(GLSurfaceView glSurfaceView, final GLFrameTranslator translator,
            RendererFactory rendererFactory, ObjectTracker tracker) {
        if (glSurfaceView == null) {
            throw new IllegalArgumentException("glSurfaceView cannot be null");
        } else if (translator == null) {
            throw new IllegalArgumentException("translator cannot be null");
        } else if (rendererFactory == null) {
            throw new IllegalArgumentException("rendererFactory cannot be null");
        } else if (tracker == null) {
            throw new IllegalArgumentException("tracker may not be null");
        }

        view = glSurfaceView;
        this.rendererFactory = rendererFactory;
        this.tracker = tracker;

        view.addOnAttachStateChangeListener(new OnAttachStateChangeListener() {
            @Override
            public void onViewAttachedToWindow(View v) {
                translator.addGLFrameSink(glFrameSink);
            }

            @Override
            public void onViewDetachedFromWindow(View v) {
                translator.removeGLFrameSink(glFrameSink);
            }
        });
    }

    /**
     * Create an {@link ObjectTrackerRenderer} and attach it to the provided {@link GLSurfaceView}.
     * The {@link GLSurfaceView} should be uninitialized and ready to accept a {@link Renderer} when
     * this method is invoked.
     * @param view {@link GLSurfaceView} which will host this {@link Renderer}
     * @param translator {@link GLFrameTranslator} from which this object will receive frames to
     *      render
     * @param rendererFactory A {@link RendererFactory} which can be used to construct renderers
     * @param objectTracker An {@link ObjectTracker} from which to receive tracked object updates
     * @return the {@link ObjectTrackerRenderer} which is created and attached to view
     */
    public static ObjectTrackerRenderer attachTo(GLSurfaceView view, GLFrameTranslator translator,
            RendererFactory rendererFactory, ObjectTracker objectTracker) {
        view.setEGLContextClientVersion(3);
        ObjectTrackerRenderer renderer = new ObjectTrackerRenderer(view, translator,
                rendererFactory, objectTracker);
        view.setRenderer(renderer);
        view.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        return renderer;
    }

    /** {@inheritDoc} */
    @Override
    public void onSurfaceCreated(GL10 ignored, EGLConfig ignored2) {
        Log.v(TAG, "Surface created");

        invalidateFrameResources();
    }

    /** {@inheritDoc} */
    @Override
    public void onSurfaceChanged(GL10 ignored, int width, int height) {
        Log.v(TAG, "Surface changed, size=(" + width + "," + height + ")");

        displaySpace = CoordinateSpace.forDisplay(width, height, view.getDisplay());
        configurationChanged = true;
    }

    /** {@inheritDoc} */
    @Override
    public void onDrawFrame(GL10 ignored) {
        if (configurationChanged || rendererGeneration < rendererFactory.getGeneration()) {
            configurationChanged = false;
            createFrameResources();
        }

        GLES30.glClearColor(0f, 0f, 0f, 1f);
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);

        if (rendererAdapter != null) {
            rendererAdapter.render();
        }

        GLES30.glFlush();
    }

    // N.B. must be called in the context of the GLSurfaceView's thread
    private void doInitialize(CoordinateSpace cameraSpace, TextureFormat textureFormat) {
        Log.v(TAG, "ObjectTrackerRenderer initialized [" + cameraSpace.width + "," +
                cameraSpace.height + "]");

        this.cameraSpace = cameraSpace;
        rendererGeneration = RendererFactory.INVALID_GENERATION;
        frameTextureFormat = textureFormat;
        frameSinkRunning = true;
        configurationChanged = true;
    }

    // N.B. must be called in the context of the GLSurfaceView's thread
    private void doRelease() {
        Log.v(TAG, "ObjectTrackerRenderer released");

        frameSinkRunning = false;
    }

    // N.B. must be called in the context of the GLSurfaceView's thread
    private void doFrame(GLFrame frame) {
        if (frameSinkRunning) {
            rendererAdapter.doInputFrame(frame);
            frame.release();
            frame.syncContainer.awaitComplete();
        } else {
            frame.release();
        }
    }

    // N.B. must be called in the context of the GLSurfaceView's thread
    private void invalidateFrameResources() {
        frameTexture = 0;
        trackedObjectsTexture = 0;
        renderer = null;

        if (rendererAdapter != null) {
            rendererAdapter.release();
            rendererAdapter = null;
        }
    }

    // N.B. must be called in the context of the GLSurfaceView's thread
    private void createFrameResources() {
        if (!frameSinkRunning) {
            return;
        }

        if (frameTexture != 0) {
            GLES30.glDeleteTextures(1, new int[] { frameTexture }, 0);
        }
        frameTexture = GLES30Helper.generateTexture(frameTextureFormat.glTextureTarget);

        if (trackedObjectsTexture != 0) {
            GLES30.glDeleteTextures(1, new int[] { trackedObjectsTexture }, 0);
        }
        trackedObjectsTexture = GLES30Helper.generateTexture(frameTextureFormat.glTextureTarget);

        if (renderer != null) {
            renderer.release();
        }
        rendererGeneration = rendererFactory.getGeneration();
        renderer = rendererFactory.createRenderer(RendererFactory.Usage.PREVIEW, cameraSpace,
                displaySpace, frameTextureFormat);
        renderer.init();

        if (rendererAdapter != null) {
            rendererAdapter.release();
        }
        rendererAdapter = new GLFrameRendererAdapter(tracker);
        rendererAdapter.setRenderer(renderer);
        rendererAdapter.setTextures(frameTexture, trackedObjectsTexture);
    }

    private GLFrameSink glFrameSink = new GLFrameSink() {
        @Override
        public void onInitialize(final CoordinateSpace cameraSpace,
                final TextureFormat textureFormat) {
            view.queueEvent(new Runnable() {
                @Override
                public void run() {
                    doInitialize(cameraSpace, textureFormat);
                }
            });
            view.requestRender();
        }

        @Override
        public void onRelease() {
            view.queueEvent(new Runnable() {
                @Override
                public void run() {
                    doRelease();
                }
            });
            view.requestRender();
        }

        @Override
        public void onFrame(final GLFrame frame) {
            view.queueEvent(new Runnable() {
                @Override
                public void run() {
                    doFrame(frame);
                }
            });
            view.requestRender();
        }
    };
}
