/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera.preferences;

import android.content.Context;
import android.content.SharedPreferences;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.preference.Preference;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.util.Log;
import android.util.Size;

import com.qualcomm.qti.fr_camera.R;

import java.util.ArrayList;
import java.util.Objects;

/**
 * This class is the main entrypoint into managing {@link CameraDevice} preferences. It can be used
 * to both create {@link Preference}s to configure the {@link CameraDevice}, and to apply the
 * configuration to the current {@link CameraDevice}.
 */
public class CameraPreferenceManager {
    /**
     * Classes may implement this interface to be notified when various camera setting events occur
     */
    public interface EventListener {
        /**
         * This method will be invoked when a preference which affects
         * {@link CameraCaptureSession}s changes
         */
        void onCaptureSessionPreferenceChanged();

        /**
         * This method will be invoked when a preference which only affects non-preview
         * {@link CaptureRequest}s changes
         */
        void onPictureCaptureRequestPreferenceChanged();

        /**
         * This method will be invoked when a preference which affects both preview and non-preview
         * {@link CaptureRequest}s changes
         */
        void onCaptureRequestPreferenceChanged();
    }

    private static final String TAG = "SocialCamera";
    private static final String DEBUG_PREF_SAVE_PICTURES = "debug_pref_save_pictures";
    private static final String DEBUG_PREF_SAVE_PREVIEW_FRAMES = "debug_pref_save_preview_frames";
    private static final String DEBUG_PREF_ALLOW_DROP_PREVIEW_FRAMES =
            "debug_pref_allow_drop_preview_frames";

    private final Context context;
    private final ArrayList<CameraPreference> prefs = new ArrayList<>();
    private final CameraPreferencePictureSize pictureSizePreference;
    private final CameraPreferencePreviewSize previewSizePreference;
    private final CameraPreferenceFlash flashPreference;

    private EventListener listener;

    /**
     * Construct a new {@link CameraPreferenceManager}
     * @param context the active {@link Context}
     * @param cameraId the ID of the {@link CameraDevice} to manage
     * @throws NullPointerException if context or cameraId are null
     * @throws IllegalArgumentException if cameraId is an empty string, or if it specifies an
     *         invalid or disabled {@link CameraDevice}.
     */
    public CameraPreferenceManager(Context context, String cameraId) {
        Objects.requireNonNull(context, "context may not be null");
        Objects.requireNonNull(cameraId, "cameraId may not be null");
        if (cameraId.isEmpty()) {
            throw new IllegalArgumentException("cameraId may not be empty");
        }

        this.context = context;

        CameraManager cameraManager = (CameraManager)context.getSystemService(
                Context.CAMERA_SERVICE);
        CameraCharacteristics characteristics;
        try {
            characteristics = cameraManager.getCameraCharacteristics(cameraId);
        } catch (CameraAccessException e) {
            throw new IllegalArgumentException("Specified camera [" + cameraId +
                    "] is not valid", e);
        }

        pictureSizePreference = new CameraPreferencePictureSize(context, cameraId, characteristics);
        previewSizePreference = new CameraPreferencePreviewSize(context, cameraId, characteristics);
        flashPreference = new CameraPreferenceFlash(context, cameraId, characteristics);

        prefs.add(pictureSizePreference);
        prefs.add(previewSizePreference);
        prefs.add(new CameraPreferenceSceneMode(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceAutoExposure(context, cameraId, characteristics)); // N.B. This must come before flash mode, which depends on AE being configured
        prefs.add(new CameraPreferenceExposureCompensation(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceAutoFocus(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceAutoWhiteBalance(context, cameraId, characteristics));
        prefs.add(flashPreference);
        prefs.add(new CameraPreferenceNoiseReduction(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceLensShadingCorrection(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceAberrationCorrection(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceEdgeEnhancement(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceHotPixelCorrection(context, cameraId, characteristics));
        prefs.add(new CameraPreferenceStabilization(context, cameraId, characteristics));
    }

    /**
     * Set the {@link EventListener} to notify when {@link CameraPreferenceManager} events occur
     * @param listener the {@link EventListener} to notify, or null to clear a previously set
     *                 listener
     */
    public void setEventListener(EventListener listener) {
        if (this.listener != null) {
            PreferenceManager.getDefaultSharedPreferences(context)
                    .unregisterOnSharedPreferenceChangeListener(sharedPrefListener);
        }

        this.listener = listener;

        if (this.listener != null) {
            PreferenceManager.getDefaultSharedPreferences(context)
                    .registerOnSharedPreferenceChangeListener(sharedPrefListener);
        }
    }

    /**
     * Add camera {@link Preference}s for the current {@link CameraDevice} to the provided
     * {@link PreferenceScreen}
     * @param preferenceGroup the {@link PreferenceGroup} to which to add {@link Preference}s
     * @throws NullPointerException if preferenceGroup is null
     */
    public void addCameraPreferences(PreferenceGroup preferenceGroup) {
        Objects.requireNonNull(preferenceGroup, "preferenceGroup may not be null");

        for (CameraPreference p : prefs) {
            if (p.isSupported()) {
                preferenceGroup.addPreference(p.createPreference());
            }
        }
    }

    /**
     * Add debug {@link Preference}s for the current {@link CameraDevice} to the provided
     * {@link PreferenceScreen}
     * @param preferenceGroup the {@link PreferenceGroup} to which to add {@link Preference}s
     * @throws NullPointerException if preferenceGroup is null
     */
    public void addDebugPreferences(PreferenceGroup preferenceGroup) {
        Objects.requireNonNull(preferenceGroup, "preferenceGroup may not be null");

        //SwitchPreference prefDebugSavePictures = createSwitchPreference(
        //        DEBUG_PREF_SAVE_PICTURES,
        //        R.string.debug_pref_save_pictures,
        //        R.string.debug_pref_save_pictures_desc,
        //        R.bool.debug_pref_save_pictures_default_value);
        //preferenceGroup.addPreference(prefDebugSavePictures);

        SwitchPreference prefDebugSavePreviewFrames = createSwitchPreference(
                DEBUG_PREF_SAVE_PREVIEW_FRAMES,
                R.string.debug_pref_save_preview_frames,
                R.string.debug_pref_save_preview_frames_desc,
                R.bool.debug_pref_save_preview_frames_default_value);
        preferenceGroup.addPreference(prefDebugSavePreviewFrames);

        SwitchPreference prefDebugAllowDropPreviewFrames = createSwitchPreference(
                DEBUG_PREF_ALLOW_DROP_PREVIEW_FRAMES,
                R.string.debug_pref_allow_drop_preview_frames,
                R.string.debug_pref_allow_drop_preview_frames_desc,
                R.bool.debug_pref_allow_drop_preview_frames_default_value);
        preferenceGroup.addPreference(prefDebugAllowDropPreviewFrames);
        prefDebugAllowDropPreviewFrames.setDependency(prefDebugSavePreviewFrames.getKey());
    }

    /**
     * Apply the current configuration to the provided {@link CaptureRequest.Builder}
     * @param request the {@link CaptureRequest.Builder} to configure
     * @param isPreviewRequest true if request is for camera preview, else false
     * @throws NullPointerException if request is null
     */
    public void applyCameraPreferences(CaptureRequest.Builder request, boolean isPreviewRequest) {
        Objects.requireNonNull(request, "request may not be null");

        Log.v(TAG, "Applying camera preferences to a " +
                (isPreviewRequest ? "preview" : "capture") + " request");

        for (CameraPreference p : prefs) {
            p.apply(request, isPreviewRequest);
        }
    }

    /**
     * Get the requested picture size
     * @return a {@link Size} containing the users requested picture size
     */
    public Size getPictureSize() {
        return pictureSizePreference.getSize();
    }

    /**
     * Get the requested {@link PreviewSize} mode
     * @return the requested {@link PreviewSize} mode
     */
    public PreviewSize getPreviewSize() {
        return previewSizePreference.getPreviewSize();
    }

    /**
     * Get the currently active {@link FlashMode} for captured pictures
     * @return the active {@link FlashMode} value, or {@link FlashMode#NOT_SUPPORTED} if this
     *         {@link CameraDevice} does not have a flash
     */
    public FlashMode getFlashMode() {
        return flashPreference.getFlashMode();
    }

    /**
     * Toggle the active {@link FlashMode} for captured pictures to the next supported
     * {@link FlashMode}
     * @return the newly active {@link FlashMode} value, or {@link FlashMode#NOT_SUPPORTED} if this
     *         {@link CameraDevice} does not have a flash
     */
    public FlashMode toggleFlashMode() {
        return flashPreference.toggleFlashMode();
    }

    /**
     * Test whether pictures should be saved to the SDCard (for debug purposes)
     * @return true if pictures should be saved for debug purposes, else false
     */
    public boolean getDebugSavePictures() {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(DEBUG_PREF_SAVE_PICTURES, context.getResources().getBoolean(
                        R.bool.debug_pref_save_pictures_default_value));
    }

    /**
     * Test whether preview frames should be saved to the SDCard (for debug purposes)
     * @return true if preview frames should be saved for debug purposes, else false
     */
    public boolean getDebugSavePreviewFrames() {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(DEBUG_PREF_SAVE_PREVIEW_FRAMES, context.getResources().getBoolean(
                        R.bool.debug_pref_save_preview_frames_default_value));
    }

    /**
     * Test whether, if preview frames are being saved, it is allowed to skip saving preview frames
     * whenever necessary to maintain a high framerate
     * @return true if preview frames may be skipped to maintain maximum framerate, else false
     */
    public boolean getDebugAllowSkipPreviewFrames() {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(DEBUG_PREF_ALLOW_DROP_PREVIEW_FRAMES, context.getResources().getBoolean(
                        R.bool.debug_pref_allow_drop_preview_frames_default_value));
    }

    private SwitchPreference createSwitchPreference(String key, int titleResId, int summaryResId,
                                                    int defaultValueResId) {
        SwitchPreference pref = new SwitchPreference(context);
        pref.setKey(key);
        pref.setTitle(titleResId);
        pref.setSummary(summaryResId);
        pref.setDefaultValue(context.getResources().getBoolean(defaultValueResId));
        return pref;
    }

    private final SharedPreferences.OnSharedPreferenceChangeListener sharedPrefListener =
            new SharedPreferences.OnSharedPreferenceChangeListener() {
        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
                                              String key) {
            if (listener == null) {
                return;
            }

            if (key.equals(pictureSizePreference.getPreferenceKey())) {
                listener.onCaptureSessionPreferenceChanged();
            } else if (key.equals(flashPreference.getPreferenceKey())) {
                listener.onPictureCaptureRequestPreferenceChanged();
            } else {
                for (CameraPreference p : prefs) {
                    if (key.equals(p.getPreferenceKey())) {
                        listener.onCaptureRequestPreferenceChanged();
                        break;
                    }
                }
            }
        }
    };
}
