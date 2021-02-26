/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import com.qualcomm.qti.khronos.EGL15Helper;
import com.qualcomm.qti.khronos.GLES30Ext;
import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.EGL15Helper.EGLContextToken;
import com.qualcomm.qti.objecttracker.misc.CoordinateSpace;
import com.qualcomm.qti.objecttracker.misc.Orientation;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttracker.tracker.TrackedObject;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.Renderer;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TextureFormat;
import com.qualcomm.qti.objecttrackerdemo.renderer.primitives.TrackedObjectRenderer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.net.Uri;
import android.opengl.GLES30;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import android.view.Display;

/**
 * {@link SnapshotRenderer} is responsible for consuming the output of {@link Camera#takePicture(
 * android.hardware.Camera.ShutterCallback, android.hardware.Camera.PictureCallback,
 * android.hardware.Camera.PictureCallback, android.hardware.Camera.PictureCallback)} and producing
 * a high-res image file in the device gallery that matches what is displayed in camera preview.
 */
@SuppressWarnings("deprecation")
public class SnapshotRenderer implements Camera.PictureCallback {
    /**
     * Classes can implement this interface to be notified when a snapshot render operation has
     * completed (either success or failure)
     */
    public static interface OnSnapshotRenderedListener {
        /**
         * This method will be invoked when snapshot rendering completes successfully. The
         * {@link Uri} of the rendered image is provided.
         * @param snapshot a {@link File} pointing to the rendered image
         */
        public void onSnapshotRendered(File snapshot);

        /**
         * This method will be invoked when snapshot rendering fails. Examine logcat for the cause
         * of the failure
         */
        public void onSnapshotRenderingFailed();
    }

    private static final String TAG = "SnapshotRenderer";

    private final Context context;
    private final CoordinateSpace previewSpace;
    private final CoordinateSpace snapshotSpace;
    private final Display displayInfo;
    private final RendererFactory rendererFactory;
    private final ObjectTracker tracker;
    private final OnSnapshotRenderedListener listener;
    private Handler renderThreadHandler;

    // These members may only be accessed from the render thread context
    private EGLContextToken eglToken;

    /**
     * Construct a new {@link SnapshotRenderer}.
     * @param context The application {@link Context} in which this {@link SnapshotRenderer} runs
     * @param previewSpace a {@link CoordinateSpace} describing the camera preview frames
     * @param snapshotSpace a {@link CoordinateSpace} describing the camera snapshot frames
     * @param displayInfo {@link Display} on which the camera preview is rendered
     * @param rendererFactory A {@link RendererFactory} used to create the appropriate renderer
     * @param tracker an {@link ObjectTracker} from which to receive tracked object updates
     * @param listener A callback interface on which to be notified when the snapshot render
     *      operation is complete. May be null.
     * @throws IllegalArgumentException if cameraInfo, displayInfo, rendererFactory or tracker are
     *      null
     */
    public SnapshotRenderer(Context context, CoordinateSpace previewSpace,
            CoordinateSpace snapshotSpace, Display displayInfo, RendererFactory rendererFactory,
            ObjectTracker tracker, OnSnapshotRenderedListener listener) {
        if (context == null) {
            throw new IllegalArgumentException("context cannot be null");
        } else if (previewSpace == null) {
            throw new IllegalArgumentException("previewSpace cannot be null");
        } else if (snapshotSpace == null) {
            throw new IllegalArgumentException("snapshotSpace cannot be null");
        } else if (displayInfo == null) {
            throw new IllegalArgumentException("displayInfo cannot be null");
        } else if (rendererFactory == null) {
            throw new IllegalArgumentException("rendererFactory cannot be null");
        } else if (tracker == null) {
            throw new IllegalArgumentException("tracker cannot be null");
        }

        this.context = context;
        this.previewSpace = previewSpace;
        this.snapshotSpace = snapshotSpace;
        this.displayInfo = displayInfo;
        this.rendererFactory = rendererFactory;
        this.tracker = tracker;
        this.listener = listener;
    }

    /** {@inheritDoc} */
    @Override
    public void onPictureTaken(byte[] data, Camera camera) {
        HandlerThread renderThread = new HandlerThread(TAG);
        renderThread.start();
        renderThreadHandler = new Handler(renderThread.getLooper());
        renderThreadHandler.post(onRenderThreadStarted);
        renderThreadHandler.post(new OnRequestRender(data));
    }

    // N.B. Must be posted to the render thread handler, and thus runs on the render thread
    private final Runnable onRenderThreadStarted = new Runnable() {
        @Override
        public void run() {
            Log.v(TAG, "Snapshot render thread started");

            eglToken = EGL15Helper.initializeGLES30EGLContext();
            EGL15Helper.makeEGLContextCurrent(eglToken);
        }
    };

    // N.B. Must be posted to the render thread handler, and thus runs on the render thread
    private final Runnable onRenderThreadStopped = new Runnable() {
        @Override
        public void run() {
            EGL15Helper.releaseEGLContext(eglToken);

            // Request that the render thread terminate
            Log.v(TAG, "Snapshot render thread stopped");
            Looper.myLooper().quit();
        }
    };

    // N.B. Must be posted to the render thread handler, and thus runs on the render thread
    private final class OnRequestRender implements Runnable {
        private final byte[] jpegData;
        private CoordinateSpace outputSpace;
        private Bitmap snapshot;
        private Bitmap renderedSnapshot;
        private int inputTexture;
        private int outputTexture;
        private int outputFbo;
        private int copyBuffer;
        private int copyBufferSize;
        private File outputFile;

        public OnRequestRender(byte[] jpegData) {
            this.jpegData = jpegData;
        }

        @Override
        public void run() {
            Log.v(TAG, "Rendering snapshot");

            boolean success = decodeJpeg() &&
                    createRenderResources() &&
                    render() &&
                    saveToDisk();
            releaseRenderResources();

            if (listener != null) {
                Handler h = new Handler(Looper.getMainLooper());
                if (success) {
                    h.post(new Runnable() {
                        @Override
                        public void run() {
                            listener.onSnapshotRendered(outputFile);
                        }
                    });
                } else {
                    h.post(new Runnable() {
                        @Override
                        public void run() {
                            listener.onSnapshotRenderingFailed();
                        }
                    });
                }
            }

            Log.v(TAG, "Snapshot render completed");

            renderThreadHandler.post(onRenderThreadStopped);
        }

        private boolean decodeJpeg() {
            snapshot = BitmapFactory.decodeByteArray(jpegData, 0, jpegData.length);
            if (snapshot == null) {
                Log.e(TAG, "Snapshot cannot be decoded");
                return false;
            }

            final boolean rotated = Orientation.relativeOrientation(snapshotSpace.orientation,
                    Orientation.fromSurfaceRotation(displayInfo.getRotation())).isRightAngle();
            final int rotatedWidth = (rotated ? snapshotSpace.height : snapshotSpace.width);
            final int rotatedHeight = (rotated ? snapshotSpace.width : snapshotSpace.height);
            outputSpace = CoordinateSpace.forDisplay(rotatedWidth, rotatedHeight, displayInfo);

            return true;
        }

        private boolean createRenderResources() {
            int[] handles = new int[1];

            inputTexture = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, inputTexture);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RGBA8, snapshotSpace.width,
                    snapshotSpace.height);

            outputTexture = GLES30Helper.generateTexture(GLES30.GL_TEXTURE_2D);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, outputTexture);
            GLES30.glTexStorage2D(GLES30.GL_TEXTURE_2D, 1, GLES30.GL_RGB8, outputSpace.width,
                    outputSpace.height);

            outputFbo = GLES30Helper.generateFbo(GLES30.GL_TEXTURE_2D, outputTexture, 0, 0, 0, 0);

            GLES30.glGenBuffers(1, handles, 0);
            copyBuffer = handles[0];
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, copyBuffer);
            copyBufferSize = outputSpace.width * outputSpace.height * 4;
            GLES30.glBufferData(GLES30.GL_PIXEL_PACK_BUFFER, copyBufferSize, null,
                    GLES30.GL_STREAM_COPY);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);

            return true;
        }

        private void releaseRenderResources() {
            GLES30.glDeleteBuffers(1, new int[] { copyBuffer }, 0);
            GLES30.glDeleteFramebuffers(1, new int[] { outputFbo }, 0);
            GLES30.glDeleteTextures(2, new int[] { inputTexture, outputTexture }, 0);
        }

        private boolean render() {
            GLES30Helper.copyBitmapToPbo(snapshot, copyBuffer);
            snapshot.recycle();

            GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, copyBuffer);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, inputTexture);
            GLES30.glTexSubImage2D(GLES30.GL_TEXTURE_2D, 0, 0, 0, snapshotSpace.width,
                    snapshotSpace.height, GLES30.GL_RGBA, GLES30.GL_UNSIGNED_BYTE, null);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_UNPACK_BUFFER, 0);

            GLES30Helper.setFboConfiguration(outputFbo, 0, 0, outputSpace.width,
                    outputSpace.height);

            Renderer renderer = rendererFactory.createRenderer(RendererFactory.Usage.SNAPSHOT,
                    previewSpace, outputSpace, TextureFormat.RGBA);
            renderer.init();
            renderer.setInputTexture(null, inputTexture);
            if (renderer instanceof TrackedObjectRenderer) {
                ((TrackedObjectRenderer)renderer).setTrackedObjects(
                        TrackedObject.Position.getTrackedObjectPositions(
                        tracker.getTrackedObjects()), null, inputTexture);
            }
            renderer.render();
            renderer.release();

            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, copyBuffer);
            GLES30Ext.glReadPixels(0, 0, outputSpace.width, outputSpace.height, GLES30.GL_RGBA,
                    GLES30.GL_UNSIGNED_BYTE, 0);
            GLES30.glBindBuffer(GLES30.GL_PIXEL_PACK_BUFFER, 0);

            renderedSnapshot = Bitmap.createBitmap(outputSpace.width, outputSpace.height,
                    Config.ARGB_8888);
            // NOTE: this will block until rendering to outputBuffer is complete
            GLES30Helper.copyPboToBitmap(copyBuffer, renderedSnapshot);

            return true;
        }

        private boolean saveToDisk() {
            if (renderedSnapshot == null) {
                Log.e(TAG, "Rendered snapshot not available");
                return false;
            }

            outputFile = Util.generateMediaFile(Util.MediaFileTypes.JPEG);
            if (outputFile == null) {
                Log.e(TAG, "Failed generating media file for snapshot");
                return false;
            }

            try {
                FileOutputStream fos = new FileOutputStream(outputFile);
                renderedSnapshot.compress(CompressFormat.JPEG, 100, fos);
                fos.flush();
                fos.close();
            } catch (IOException e) {
                Log.e(TAG, "Failed writing compressed snapshot to disk", e);
                return false;
            }

            renderedSnapshot.recycle();

            Util.notifyMediaScannerOfContent(context, Uri.fromFile(outputFile));

            Log.i(TAG, "Saved snapshot to '" + outputFile + "'");

            return true;
        }
    }
}
