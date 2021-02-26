/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceCategory;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.util.Size;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.preferences.camera2.Camera2PreferenceExposureCompensation;
import com.qualcomm.qti.panoramaui.preferences.camera2.Camera2PreferencePictureSize;
import com.qualcomm.qti.panoramaui.preferences.camera2.Camera2PreferenceWhiteBalance;
import com.qualcomm.qti.panoramaui.preferences.legacycamera.CameraPreferenceExposureCompensation;
import com.qualcomm.qti.panoramaui.preferences.legacycamera.CameraPreferenceISO;
import com.qualcomm.qti.panoramaui.preferences.legacycamera.CameraPreferencePictureSize;
import com.qualcomm.qti.panoramaui.preferences.legacycamera.CameraPreferenceWhiteBalance;
import com.qualcomm.qti.panoramaui.utils.AppConfig;

/**
 * This class exposes a settings framework for use by the Panorama app to control the camera and
 * SCVE settings
 */
@SuppressWarnings("deprecation")
public abstract class CameraPreferenceManager {

    private static final String CAMERA_SELECTED_ID = "camerapref_camera_id";
    private static final String PANO_SCVE_MODE_KEY = "panopref_scve_mode";
    private static final String PANO_IMAGE_COMPRESS_LEVEL = "panopref_image_compress_level";
    private static final String PANO_LOCK_AE_AWB = "panopref_lock_ae_awb";
    private static final String PANO_PROJECTION_TYPE = "panopref_projection_type";
    private static final String DEBUG_ENABLE_VERBOSE_LOGGING = "debugpref_enable_verbose_logging";
    private static final String DEBUG_SAVE_PREVIEW_FRAMES_KEY = "debugpref_save_preview_frames";
    private static final String DEBUG_SAVE_KEY_FRAMES_KEY = "debugpref_save_key_frames";

    /**
     * This method will create and add a set of {@link Preference}s for the legacy Camera APIs to
     * the given {@link PreferenceScreen}
     * @param ps {@link PreferenceScreen} to which to add {@link Camera} and Panorama settings
     * @param cameraId ID of the {@link Camera} for which {@link Preference}s should be created
     * @param p {@link Parameters} for the given camera ID
     */
    public static void addLegacyCameraPreferenceEntries(PreferenceScreen ps, int cameraId,
            Parameters p) {
        Context c = ps.getContext();

        PreferenceCategory cameraPrefs = new PreferenceCategory(c);
        cameraPrefs.setTitle(R.string.camera_settings);
        ps.addPreference(cameraPrefs);

        cameraPrefs.addPreference(new CameraPreferencePictureSize(c, cameraId, p)
                .createPreferenceEntry());
        /* NOTE: removed because default camera config is considered ideal
        cameraPrefs.addPreference(new CameraPreferenceExposureCompensation(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferenceWhiteBalance(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferenceSceneMode(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferenceFocusMode(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferenceISO(c, cameraId, p).
                createPreferenceEntry());
        */

        addCommonPreferenceEntries(c, ps);
    }

    /**
     * This method will create and add a set of {@link Preference}s for the new Camera2 APIs to the
     * given {@link PreferenceScreen}
     * @param ps {@link PreferenceScreen} to which to add {@link Camera} and Panorama settings
     * @param cameraId ID of the {@link Camera} for which {@link Preference}s should be created
     * @param characteristics {@link CameraCharacteristics} for the given camera ID
     */
    public static void addCamera2PreferenceEntries(PreferenceScreen ps, int cameraId,
            CameraCharacteristics characteristics) {
        Context c = ps.getContext();

        PreferenceCategory cameraPrefs = new PreferenceCategory(c);
        cameraPrefs.setTitle(R.string.camera_settings);
        ps.addPreference(cameraPrefs);

        cameraPrefs.addPreference(new Camera2PreferencePictureSize(c, cameraId, characteristics)
                .createPreferenceEntry());
        /* NOTE: removed because default camera config is considered ideal
        cameraPrefs.addPreference(new Camera2PreferenceExposureCompensation(c, cameraId, characteristics).
                createPreferenceEntry());
        cameraPrefs.addPreference(new Camera2PreferenceWhiteBalance(c, cameraId, characteristics).
                createPreferenceEntry());
        cameraPrefs.addPreference(new Camera2PreferenceSceneMode(c, cameraId, characteristics).
                createPreferenceEntry());
        cameraPrefs.addPreference(new Camera2PreferenceFocusMode(c, cameraId, characteristics).
                createPreferenceEntry());
        */

        addCommonPreferenceEntries(c, ps);
    }

    private static void addCommonPreferenceEntries(Context c, PreferenceScreen ps) {
        PreferenceCategory panoPrefs = new PreferenceCategory(c);
        panoPrefs.setTitle(R.string.pano_settings);
        ps.addPreference(panoPrefs);
        if (!AppConfig.EVAL_ENABLED) {

        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_SCVE_MODE_KEY, 0,
                    R.string.pano_settings_scve_mode, R.array.pano_settings_scve_mode_entries,
                    R.array.pano_settings_scve_mode_values, R.string.pano_settings_scve_mode_default));
        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_IMAGE_COMPRESS_LEVEL, 0,
                R.string.pano_settings_image_compress_level,
                R.array.pano_settings_image_compress_level_entries,
                R.array.pano_settings_image_compress_level_values,
                R.string.pano_settings_image_compress_level_default));
        }
        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_LOCK_AE_AWB, 0,
                R.string.pano_settings_enable_3a_lock,
                R.array.pano_settings_enable_3a_lock_entries,
                R.array.pano_settings_enable_3a_lock_values,
                R.string.pano_settings_enable_3a_lock_default));
        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_PROJECTION_TYPE, 0,
                R.string.pano_settings_projection_type,
                R.array.pano_settings_projection_type_entries,
                R.array.pano_settings_projection_type_values,
                R.string.pano_settings_projection_type_default));
        if (!AppConfig.EVAL_ENABLED) {
            PreferenceCategory debugPrefs = new PreferenceCategory(c);
            debugPrefs.setTitle(R.string.debug_settings);
            ps.addPreference(debugPrefs);

            debugPrefs.addPreference(createListPreferenceFromResources(c,
                    DEBUG_ENABLE_VERBOSE_LOGGING, 0, R.string.debug_settings_enable_loggings,
                R.array.debug_settings_enable_loggings_entries,
                R.array.debug_settings_enable_loggings_values,
                R.string.debug_settings_enable_loggings_default));
            debugPrefs.addPreference(createListPreferenceFromResources(c,
                    DEBUG_SAVE_PREVIEW_FRAMES_KEY, 0, R.string.debug_settings_save_preview_frames,
                R.array.debug_settings_save_preview_frames_entries,
                R.array.debug_settings_save_preview_frames_values,
                R.string.debug_settings_save_preview_frames_default));
            debugPrefs.addPreference(createListPreferenceFromResources(c,
                    DEBUG_SAVE_KEY_FRAMES_KEY, 0, R.string.debug_settings_save_key_frames,
                R.array.debug_settings_save_key_frames_entries,
                R.array.debug_settings_save_key_frames_values,
                R.string.debug_settings_save_key_frames_default));
        }
    }

    /**
     * Apply the {@link Preference}s to the given {@link Parameters} instance
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @param cameraId ID of the {@link Camera} to which {@link Preference}s should be applied
     * @param p {@link Parameters} for the given {@link Camera} ID
     */
    public static void applyLegacyCameraPreferences(Context ctx, int cameraId, Parameters p) {
        new CameraPreferencePictureSize(ctx, cameraId, p).apply();
        new CameraPreferenceExposureCompensation(ctx, cameraId, p).apply();
        new CameraPreferenceWhiteBalance(ctx, cameraId, p).apply();
        //new CameraPreferenceSceneMode(ctx, cameraId, p).apply();
        //new CameraPreferenceFocusMode(ctx, cameraId, p).apply();
        new CameraPreferenceISO(ctx, cameraId, p).apply();
    }

    /**
     * Apply the {@link Preference}s to the given {@link CaptureRequest.Builder} instance
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @param cameraId ID of the {@link Camera} to which {@link Preference}s should be applied
     * @param characteristics {@link CameraCharacteristics} for the specified camera ID
     * @param builder {@link CaptureRequest.Builder} to apply the preferences to
     */
    public static void applyCamera2CameraPreferences(Context ctx, int cameraId,
            CameraCharacteristics characteristics, CaptureRequest.Builder builder) {
        new Camera2PreferenceExposureCompensation(ctx, cameraId, characteristics).apply(builder);
        new Camera2PreferenceWhiteBalance(ctx, cameraId, characteristics).apply(builder);
        //new Camera2PreferenceSceneMode(ctx, cameraId, characteristics).apply(builder);
        //new Camera2PreferenceFocusMode(ctx, cameraId, characteristics).apply(builder);
    }

    /**
     * Get the ID of the selected {@link Camera}
     * @param ctx {@link Context} from which to load the {@link Camera} ID
     * @return the selected {@link Camera} ID
     */
    public static int getSelectedCameraId(Context ctx) {
        return PreferenceManager.getDefaultSharedPreferences(ctx).getInt(CAMERA_SELECTED_ID, 0);
    }

    /**
     * Set the ID of the selected {@link Camera}
     * @param ctx {@link Context} to which to save the {@link Camera} ID
     * @param cameraId {@link Camera} ID to save
     */
    public static void setSelectedCameraId(Context ctx, int cameraId) {
        PreferenceManager.getDefaultSharedPreferences(ctx).edit()
                .putInt(CAMERA_SELECTED_ID, cameraId).apply();
    }

    /**
     * Get the selected picture size for the Camera2 APIs
     * @param ctx {@link Context} from which to load the picture size
     * @param cameraId the selected {@link Camera} ID
     * @param cc {@link CameraCharacteristics} o
     * @return
     */
    public static Size getCamera2PictureSize(Context ctx, int cameraId, CameraCharacteristics cc) {
        return new Camera2PreferencePictureSize(ctx, cameraId, cc).getPictureSize();
    }

    /**
     * Get the SCVE mode from the Panorama settings
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return a SCVE_MODE_* value
     */
    public static int getScveMode(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_SCVE_MODE_KEY, 0),
                ctx.getResources().getString(R.string.pano_settings_scve_mode_default));
        return Integer.parseInt(val);
    }

    /**
     * Get user preference whether to enable Ae/Af
     * @param ctx {@link Context}
     * @return true if enable AeAF and false otherwise
     */
    public static boolean getEnable3A(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_LOCK_AE_AWB, 0),
                ctx.getResources().getString(R.string.pano_settings_enable_3a_lock_default));
        return Boolean.parseBoolean(val);
    }

    /**
     * Get the projection type from the Panorama settings
     *
     * @param ctx
     *            {@link Context} from which to load {@link Preference} values
     * @return projection type value
     */

    public static int getProjectionType(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_PROJECTION_TYPE, 0),
                ctx.getResources().getString(R.string.pano_settings_projection_type_default));
        return Integer.parseInt(val);
    }
    /**
     * Enable/Disable logging functions for debug purpose
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return true for Yes and false for No
     */
    public static boolean getEnableLogging(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(DEBUG_ENABLE_VERBOSE_LOGGING, 0),
                ctx.getResources().getString(R.string.debug_settings_enable_loggings_default));
        return Boolean.parseBoolean(val);
    }

    /**
     * Get the preferred image compress level
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return a preferred image compress level
     */
    public static int getImageCompressLevel(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_IMAGE_COMPRESS_LEVEL, 0),
                ctx.getResources().getString(R.string.pano_settings_image_compress_level_default));
        return Integer.parseInt(val);
    }

    /**
     * Get whether to save preview frames to the SD card (for debug purposes)
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return true if preview frames should be saved, else false
     */
    public static boolean getSavePreviewFrames(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(DEBUG_SAVE_PREVIEW_FRAMES_KEY, 0),
                ctx.getResources().getString(R.string.debug_settings_save_preview_frames_default));
        return Boolean.parseBoolean(val);
    }

    /**
     * Get whether to save key frames to the SD card (for debug purposes)
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return true if key frames should be saved, else false
     */
    public static boolean getSaveKeyFrames(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(DEBUG_SAVE_KEY_FRAMES_KEY, 0),
                ctx.getResources().getString(R.string.debug_settings_save_key_frames_default));
        return Boolean.parseBoolean(val);
    }

    /**
     * Helper method to construct a {@link ListPreference} from a series of resources
     * @param ctx {@link Context} from which to store/load this {@link ListPreference}'s value
     * @param baseKey Base key value to use when storing/loading this setting
     * @param cameraId {@link Camera} ID to which this setting applies
     * @param titleResId Resource ID of a {@link String} to use as the human-readable title of this
     *      {@link ListPreference}
     * @param entriesResId Resource ID of an array of {@link String}s to use as the human-readable
     *      set of entries in this {@link ListPreference}
     * @param valuesResId Resource ID of an array of {@link String}s to use as the values
     *      corresponding to the entries in entriesResId. This array must be of the same length as
     *      entriesResId
     * @param defaultValueResId Resource ID of a {@link String} to use as the default value of this
     *      {@link ListPreference}. It must be the same as of the values in valuesResId.
     * @return a {@link ListPreference} representing the desired setting
     */
    private static ListPreference createListPreferenceFromResources(Context ctx, String baseKey,
            int cameraId, int titleResId, int entriesResId, int valuesResId, int defaultValueResId) {
        ListPreference lp = new ListPreference(ctx);
        lp.setKey(CameraPreference.constructPreferenceKey(baseKey, cameraId));
        lp.setTitle(titleResId);
        lp.setEntries(entriesResId);
        lp.setEntryValues(valuesResId);
        lp.setDefaultValue(ctx.getResources().getString(defaultValueResId));
        lp.setSummary("%s");
        return lp;
    }
}
