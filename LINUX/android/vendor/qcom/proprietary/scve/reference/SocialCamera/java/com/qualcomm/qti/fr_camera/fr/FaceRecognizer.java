/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.fr;

import android.content.Context;
import android.util.Log;

import com.qualcomm.qti.facerecognition.FaceInfo;
import com.qualcomm.qti.facerecognition.FaceRecognition;
import com.qualcomm.qti.facerecognition.FaceRecognitionResult;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Locale;
import java.util.Objects;

/**
 * This class provides a convenience wrapper around {@link FaceRecognition}
 */
public class FaceRecognizer implements AutoCloseable {
    /**
     * The SCVE mode in which to run facial recognition
     */
    public enum ScveMode {
        DEFAULT(FaceRecognition.SCVE_MODE_DEFAULT),
        CPU_OFF_LOAD(FaceRecognition.SCVE_MODE_CPU_OFFLOAD),
        PERFORMANCE(FaceRecognition.SCVE_MODE_PERFORMANCE),
        LOW_POWER(FaceRecognition.SCVE_MODE_LOW_POWER);

        private final int scveMode;

        ScveMode(int scveMode) {
            this.scveMode = scveMode;
        }
    }

    /**
     * The frame color format of the provided image frames
     */
    public enum FrameFormat {
        /** Grayscale, 8bpp */
        GRAYSCALE(FaceRecognition.SCVE_COLORFORMAT_GREY_8BIT),

        /** YCbCr 4:2:0 semi-planar (NV12), 12bpp */
        NV12(FaceRecognition.SCVE_COLORFORMAT_NV12),

        /** YCrCb 4:2:0 semi-planar (NV21), 12bpp */
        NV21(FaceRecognition.SCVE_COLORFORMAT_NV21);

        private final int scveColorFormat;

        FrameFormat(int scveColorFormat) {
            this.scveColorFormat = scveColorFormat;
        }
    }

    public enum FaceRecoMode {
        /** For offline processing of large image collection where
            real time feedback is not critical */
        STILL_IMAGE(FaceRecognition.SCVE_FACERECOGNITION_STILL_IMAGE),

        /** For camera previe mode where real time feedback is very
            critical */
        VIDEO(FaceRecognition.SCVE_FACERECOGNITION_VIDEO);

        private final int scveFaceRecoMode;

        FaceRecoMode(int scveFaceRecoMode) {
            this.scveFaceRecoMode = scveFaceRecoMode;
        }
    }

    private static final String TAG = "SocialCamera";
    private static final String FR_DATABASE_NAME = "frdb.dat";
    private static final int FR_DATABASE_MAX_USERS = 1000;
    private static final int FR_MAX_NUM_RESULTS = 10; // Max recognized faces for a single request

    private final ScveMode scveMode;
    private final FrameFormat format;
    private final FaceRecoMode frMode;
    private final int width;
    private final int height;
    private final int lumaStride;
    private final int chromaStride;
    private final int maxFacesPerImage;
    private final File frDatabasePath;
    private final FaceRecognition faceRecognition;
    private final int minFrameBytes;
    private final FaceInfo[] faceInfo;
    private final FaceRecognitionResult[] faceRecognitionResults =
            new FaceRecognitionResult[FR_MAX_NUM_RESULTS];
    private boolean closed;

    /**
     * Construct a new {@link FaceRecognizer}
     * @param context the active {@link Context}
     * @param scveMode the {@link ScveMode} in which to run facial recognition
     * @param frMode the {@link FaceRecoMode} in which to run facial recognition
     * @param format the {@link FrameFormat} of the input image frames
     * @param width the width of the input image frames, in px
     * @param height the height of the input image frames, in px
     * @param strides the strides of the input image frame planes, in bytes
     * @param maxFacesPerImage the maximum number of faces per input image frame to
     *                         detect/recognize
     * @throws NullPointerException if context, scveMode, format, or strides are null
     * @throws IllegalArgumentException if width or height are <= 0, if maxFacesPerImage is < 0,
     *         or if the strides are invalid for the selected {@link FrameFormat}
     * @throws FaceRecognizerException for all SCVE facial recognition initialization errors
     */
    public FaceRecognizer(Context context, ScveMode scveMode, FaceRecoMode frMode,
                          FrameFormat format, int width, int height, int[] strides,
                          int maxFacesPerImage) {
        Objects.requireNonNull(context, "context may not be null");
        Objects.requireNonNull(scveMode, "scveMode may not be null");
        Objects.requireNonNull(frMode, "frMode may not be null");
        Objects.requireNonNull(format, "format may not be null");
        Objects.requireNonNull(strides, "strides may not be null");

        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("width and height must be > 0");
        } else if (maxFacesPerImage <= 0) {
            throw new IllegalArgumentException("maxFacesPerImage must be > 0");
        }

        // Validate strides
        switch (format) {
            case GRAYSCALE:
                if (strides.length != 1 || strides[0] < width) {
                    throw new IllegalArgumentException("Invalid strides for GRAYSCALE; must be " +
                            "of length 1, and stride[0] must be >= width");
                }
                lumaStride = strides[0];
                chromaStride = 0;
                minFrameBytes = strides[0] * height;
                break;

            case NV12:
            case NV21:
                if (strides.length != 2 || strides[0] < width ||
                        strides[1] < ((width + 1) / 2) * 2) {
                    throw new IllegalArgumentException("Invalid strides for NV12/NV21; must be " +
                            "of length 2, stride[0] must be >= width, and stride[1] must be >= " +
                            "width rounded up to the next multiple of 2");
                }
                lumaStride = strides[0];
                chromaStride = strides[1];
                minFrameBytes = strides[0] * height + strides[1] * ((height + 1) / 2);
                break;

            default:
                throw new IllegalStateException("Unexpected format: " + format.name());
        }

        this.scveMode = scveMode;
        this.frMode = frMode;
        this.format = format;
        this.width = width;
        this.height = height;
        this.maxFacesPerImage = maxFacesPerImage;

        frDatabasePath = new File(context.getFilesDir(), FR_DATABASE_NAME);

        faceRecognition = new FaceRecognition();
        initializeScve();

        faceInfo = new FaceInfo[maxFacesPerImage];
        for (int i = 0; i < faceInfo.length; i++) {
            faceInfo[i] = new FaceInfo();
        }

        for (int i = 0; i < faceRecognitionResults.length; i++) {
            faceRecognitionResults[i] = new FaceRecognitionResult();
        }
    }

    /**
     * Close the current {@link FaceRecognizer}. After invoking this method, no further methods may
     * be invoked on this {@link FaceRecognizer} instance.
     */
    @Override
    public void close() {
        if (!closed) {
            closed = true;
            destroyScve();
        }
    }

    /**
     * Detect (and optionally recognize) faces in the provided image
     * @param frame the image frame to process. It should match the properties specified during
     *              construction of this {@link FaceRecognizer}.
     * @param recognize true to perform recognition on faces after detection, false to just perform
     *                  face detection
     * @return an array of {@link Face}s. If recognition was performed, each {@link Face} will
     *         contain recognition scores.
     * @throws NullPointerException if frame is null
     * @throws IllegalArgumentException if frame does not contain sufficient bytes (the minimum
     *         number of bytes depends on the frame properties specified during object construction)
     * @throws FaceRecognizerException if {@link #close()} has been invoked, or if an error occurs
     *         during SCVE facial detection or recognition
     */
    public Face[] detectFaces(ByteBuffer frame, boolean recognize) {
        Objects.requireNonNull(frame, "frame may not be null");

        if (recognize && faceRecognition.nGetNumDatabaseUsers() == 0) {
            Log.v(TAG, "Face database has no users, ignoring recognize flag");
            recognize = false;
        }

        if (frame.limit() < minFrameBytes) {
            throw new IllegalArgumentException("frame.length must be at least " + minFrameBytes);
        } else if (closed) {
            throw new FaceRecognizerException("FaceRecognizer has been closed");
        }

        int result = faceRecognition.nDetectFaces(frame, faceInfo);
        if (result < 0) {
            throw new FaceRecognizerException("Failed detecting faces, error=" + result);
        }

        Face[] faces = new Face[result];
        for (int i = 0; i < result; i++) {
            int numRecognitions = 0;
            if (recognize) {
                numRecognitions = faceRecognition.nRecognizeFace(frame, faceInfo[i],
                        faceRecognitionResults.length, faceRecognitionResults);
                if (numRecognitions < 0) {
                    Log.e(TAG, "Failed recognizing faces, error=" + numRecognitions);
                }
            }

            faces[i] = new Face(faceInfo[i], faceRecognitionResults, numRecognitions);
        }

        return faces;
    }

    /**
     * Enroll a {@link Face} in the facial recognition database
     * @param frame the image frame containing the {@link Face} to enroll
     * @param face the {@link Face} to be enrolled
     * @return an identifier for the enrolled {@link Face}
     * @throws NullPointerException if frame or face are null
     * @throws IllegalArgumentException if frame does not contain sufficient bytes (the minimum
     *         number of bytes depends on the frame properties specified during object construction)
     * @throws FaceRecognizerException if {@link #close()} has been invoked, or if an error occurs
     *         during SCVE facial enrollment
     */
    public String enrollFace(ByteBuffer frame, Face face) {
        // N.B. persistentId must generate exactly 20 bytes when converted to a byte[]
        String persistentId = String.format(Locale.US, "fr_%017d",
                (long)(Math.random() * 100000000000000000l));
        return enrollFace(frame, face, persistentId);
    }

    /**
     * Enroll a {@link Face} in the facial recognition database, associating it with another
     * enrolled face with the specified ID. This can be used to enroll multiple angles/images of a
     * face using the same ID.
     * @param frame the image frame containing the {@link Face} to enroll
     * @param face the {@link Face} to be enrolled
     * @param id the ID of an already-enrolled {@link Face} (as returned by
     *           {@link #enrollFace(ByteBuffer, Face)})
     * @return an identifier for the enrolled {@link Face}
     * @throws NullPointerException if frame or face are null
     * @throws IllegalArgumentException if frame does not contain sufficient bytes (the minimum
     *         number of bytes depends on the frame properties specified during object
     *         construction), or if id is not a valid ID previously returned by
     *         {@link #enrollFace(ByteBuffer, Face)}
     * @throws FaceRecognizerException if {@link #close()} has been invoked, or if an error occurs
     *         during SCVE facial enrollment
     */
    public String enrollFace(ByteBuffer frame, Face face, String id) {
        Objects.requireNonNull(frame, "frame may not be null");
        Objects.requireNonNull(face, "face may not be null");
        Objects.requireNonNull(id, "id may not be null");

        byte[] persistentId = id.getBytes(StandardCharsets.UTF_8);
        if (persistentId.length != 20) {
            throw new IllegalArgumentException("id must be exactly 20 bytes in length");
        }

        if (frame.limit() < minFrameBytes) {
            throw new IllegalArgumentException("frame.length must be at least " + minFrameBytes);
        } else if (closed) {
            throw new FaceRecognizerException("FaceRecognizer has been closed");
        }

        int result = faceRecognition.nEnrollFace(frame, face.createFaceInfo(), persistentId);
        if (result == 0) {
            result = faceRecognition.nSaveDatabase(frDatabasePath.getPath());
        }
        if (result < 0) {
            throw new FaceRecognizerException("Failed enrolling face, error=" + result);
        }

        return id;
    }

    /**
     * Clear the {@link FaceRecognizer} database of all previously enrolled faces
     */
    public void removeAllFaces() {
        destroyScve();
        //noinspection ResultOfMethodCallIgnored
        frDatabasePath.delete();
        initializeScve();
    }

    /**
     * Remove the specified face from the {@link FaceRecognizer} database
     * @param id the ID of an already-enrolled {@link Face} (as returned by
     *           {@link #enrollFace(ByteBuffer, Face)})
     * @throws IllegalArgumentException if id is null, or if id is not a valid ID previously
     *         returned by {@link #enrollFace(ByteBuffer, Face)}
     */
    public void removeFace(String id) {
        Objects.requireNonNull(id, "id may not be null");

        byte[] persistentId = id.getBytes(StandardCharsets.UTF_8);
        if (persistentId.length != 20) {
            throw new IllegalArgumentException("id must be exactly 20 bytes in length");
        }

        // N.B. ignore return, nothing actionable we can do on an error
        faceRecognition.nRemoveFromDatabase(persistentId);
        faceRecognition.nSaveDatabase(frDatabasePath.getPath());
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        close();
    }

    private void initializeScve() {
        int result = faceRecognition.nCreateScveContext(scveMode.scveMode);
        if (result != FaceRecognition.SCVE_SUCCESS) {
            throw new FaceRecognizerException("Failed to create SCVE context, error=" + result);
        }

        result = faceRecognition.nCreateFaceRecognition(frDatabasePath.getPath(),
                frMode.scveFaceRecoMode);
        if (result != FaceRecognition.SCVE_SUCCESS) {
            throw new FaceRecognizerException("Failed to create SCVE face recognition, error=" +
                    result);
        }

        result = faceRecognition.nInit(format.scveColorFormat, width, height, lumaStride,
                chromaStride, maxFacesPerImage, FR_DATABASE_MAX_USERS);
        if (result != FaceRecognition.SCVE_SUCCESS) {
            throw new FaceRecognizerException("Failed to initialize SCVE face recognition, error=" +
                    result);
        }
    }

    private void destroyScve() {
        faceRecognition.nFreeFaceRecognition();
        faceRecognition.nFreeScveContext();
    }
}
