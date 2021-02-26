/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.preferences;

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

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercaptureui.preferences.camera2.Camera2PreferenceAntiBanding;
import com.qualcomm.qti.clevercaptureui.preferences.camera2.Camera2PreferenceFocusMode;
import com.qualcomm.qti.clevercaptureui.preferences.camera2.Camera2PreferencePictureSize;
import com.qualcomm.qti.clevercaptureui.preferences.camera2.Camera2PreferencePreviewSize;
import com.qualcomm.qti.clevercaptureui.preferences.legacycamera.CameraPreferenceAntiBanding;
import com.qualcomm.qti.clevercaptureui.preferences.legacycamera.CameraPreferenceFocusMode;
import com.qualcomm.qti.clevercaptureui.preferences.legacycamera.CameraPreferencePictureSize;
import com.qualcomm.qti.clevercaptureui.preferences.legacycamera.CameraPreferencePreviewSize;

/**
 * This class exposes a settings framework for use by the CleverCapture app to control the camera and
 * SCVE settings
 */
@SuppressWarnings("deprecation")
public abstract class CameraPreferenceManager {

    private static final String CAMERA_SELECTED_ID = "camerapref_camera_id";
    private static final String PANO_SCVE_MODE_KEY = "panopref_scve_mode";
    private static final String PANO_ZOOM_CFG_KEY = "panopref_zoom_cfg";
    private static final String PANO_MOTION_CFG_KEY = "panopref_motion_cfg";
    private static final String PANO_FOCUS_CFG_KEY = "panopref_focus_cfg";
    private static final String CC_BURST_NUMBER_KEY = "ccpref_burst_number";
    private static final String CC_BURST_INTERVAL_KEY = "ccpref_burst_interval";
    private static final String PANO_IMAGE_COMPRESS_LEVEL_KEY = "panopref_image_compress_level";
    private static final String PANO_LOCK_AE_AWB_KEY = "panopref_lock_ae_awb";
    private static final String CC_ANTIBANDING_KEY = "ccpref_antibanding";
    private static final String CC_VIDEO_STABILIZATION_KEY = "ccpref_video_stabilization";
    /**
     * This method will create and add a set of {@link Preference}s for the legacy Camera APIs to
     * the given {@link PreferenceScreen}
     * @param ps {@link PreferenceScreen} to which to add {@link Camera} and CleverCapture settings
     * @param cameraId ID of the {@link Camera} for which {@link Preference}s should be created
     * @param p {@link Parameters} for the given camera ID
     */
    public static void addLegacyCameraPreferenceEntries(PreferenceScreen ps, int cameraId,
            Parameters p) {
        Context c = ps.getContext();

        PreferenceCategory cameraPrefs = new PreferenceCategory(c);
        cameraPrefs.setTitle(R.string.camera_settings);
        ps.addPreference(cameraPrefs);

        cameraPrefs.addPreference(new CameraPreferencePreviewSize(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferencePictureSize(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferenceFocusMode(c, cameraId, p).
                createPreferenceEntry());
        cameraPrefs.addPreference(new CameraPreferenceAntiBanding(c, cameraId, p).
                createPreferenceEntry());
        addCommonPreferenceEntries(c, ps, cameraPrefs);
    }

    /**
     * This method will create and add a set of {@link Preference}s for the new Camera2 APIs to the
     * given {@link PreferenceScreen}
     * @param ps {@link PreferenceScreen} to which to add {@link Camera} and CleverCapture settings
     * @param cameraId ID of the {@link Camera} for which {@link Preference}s should be created
     * @param characteristics {@link CameraCharacteristics} for the given camera ID
     */
    public static void addCamera2PreferenceEntries(PreferenceScreen ps, int cameraId,
            CameraCharacteristics characteristics) {
        Context c = ps.getContext();

        PreferenceCategory cameraPrefs = new PreferenceCategory(c);
        cameraPrefs.setTitle(R.string.camera_settings);
        ps.addPreference(cameraPrefs);

        cameraPrefs.addPreference(new Camera2PreferencePreviewSize(c, cameraId, characteristics).
                createPreferenceEntry());
        cameraPrefs.addPreference(new Camera2PreferencePictureSize(c, cameraId, characteristics).
                createPreferenceEntry());
        cameraPrefs.addPreference(new Camera2PreferenceFocusMode(c, cameraId, characteristics).
                createPreferenceEntry());
        cameraPrefs.addPreference(new Camera2PreferenceAntiBanding(c, cameraId, characteristics).
                createPreferenceEntry());

        addCommonPreferenceEntries(c, ps, cameraPrefs);
    }

    private static void addCommonPreferenceEntries(Context c, PreferenceScreen ps, PreferenceCategory cameraPrefs) {
        cameraPrefs.addPreference(createListPreferenceFromResources(c, PANO_ZOOM_CFG_KEY, 0,
                R.string.pano_settings_zoom_cfg,
                R.array.pano_settings_zoom_cfg_entries,
                R.array.pano_settings_zoom_cfg_values,
                R.string.pano_settings_zoom_cfg_default));

        cameraPrefs.addPreference(createListPreferenceFromResources(c, PANO_LOCK_AE_AWB_KEY, 0,
                R.string.pano_settings_enable_ae_awb_lock,
                R.array.pano_settings_enable_ae_awb_lock_entries,
                R.array.pano_settings_enable_ae_awb_lock_values,
                R.string.pano_settings_enable_ae_awb_lock_default));

        cameraPrefs.addPreference(createListPreferenceFromResources(c, PANO_IMAGE_COMPRESS_LEVEL_KEY, 0,
                R.string.pano_settings_image_compress_level,
                R.array.pano_settings_image_compress_level_entries,
                R.array.pano_settings_image_compress_level_values,
                R.string.pano_settings_image_compress_level_default));

        cameraPrefs.addPreference(createListPreferenceFromResources(c, CC_VIDEO_STABILIZATION_KEY, 0,
                R.string.camera_settings_video_stabilization,
                R.array.camera_settings_video_stabilization_entries,
                R.array.camera_settings_video_stabilization_values,
                R.string.camera_settings_video_stabilization_default));

        PreferenceCategory panoPrefs = new PreferenceCategory(c);
        panoPrefs.setTitle(R.string.pano_settings);
        ps.addPreference(panoPrefs);

        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_SCVE_MODE_KEY, 0,
                R.string.pano_settings_scve_mode,
                R.array.pano_settings_scve_mode_entries,
                R.array.pano_settings_scve_mode_values,
                R.string.pano_settings_scve_mode_default));

        //panoPrefs.addPreference(createListPreferenceFromResources(c, CC_BURST_NUMBER, 0,
        //        R.string.cc_settings_burst_number,
        //        R.array.cc_settings_burst_number_entries,
        //        R.array.cc_settings_burst_number_values,
        //        R.string.cc_settings_burst_number_default));

        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_MOTION_CFG_KEY, 0,
                R.string.pano_settings_motion_cfg,
                R.array.pano_settings_motion_cfg_entries,
                R.array.pano_settings_motion_cfg_values,
                R.string.pano_settings_motion_cfg_default));

        panoPrefs.addPreference(createListPreferenceFromResources(c, PANO_FOCUS_CFG_KEY, 0,
                R.string.pano_settings_focus_cfg,
                R.array.pano_settings_focus_cfg_entries,
                R.array.pano_settings_focus_cfg_values,
                R.string.pano_settings_focus_cfg_default));
    }

    /**
     * Apply the {@link Preference}s to the given {@link Parameters} instance
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @param cameraId ID of the {@link Camera} to which {@link Preference}s should be applied
     * @param p {@link Parameters} for the given {@link Camera} ID
     */
    public static void applyLegacyCameraPreferences(Context ctx, int cameraId, Parameters p) {
        new CameraPreferencePictureSize(ctx, cameraId, p).apply();
        new CameraPreferencePreviewSize(ctx, cameraId, p).apply();
        new CameraPreferenceFocusMode(ctx, cameraId, p).apply();
        new CameraPreferenceAntiBanding(ctx, cameraId, p).apply();
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
        new Camera2PreferenceFocusMode(ctx, cameraId, characteristics).apply(builder);
        new Camera2PreferenceAntiBanding(ctx, cameraId, characteristics).apply(builder);
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
        PreferenceManager.getDefaultSharedPreferences(ctx).edit().putInt(CAMERA_SELECTED_ID,
                cameraId).apply();
    }

    /**
     * Get the selected preview size for the Camera2 APIs
     * @param ctx {@link Context} from which to load the picture size
     * @param cameraId the selected {@link Camera} ID
     * @param cc {@link CameraCharacteristics} o
     * @return
     */
    public static Size getCamera2PreviewSize(Context ctx, int cameraId, CameraCharacteristics cc) {
        return new Camera2PreferencePreviewSize(ctx, cameraId, cc).getPreviewSize();
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

    public static int getZoomCfg(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_ZOOM_CFG_KEY, 0),
                ctx.getResources().getString(R.string.pano_settings_zoom_cfg_default));
        return Integer.parseInt(val);
    }

    public static int getMotionCfg(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_MOTION_CFG_KEY, 0),
                ctx.getResources().getString(R.string.pano_settings_motion_cfg_default));
        return Integer.parseInt(val);
    }

    public static int getFocusCfg(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_FOCUS_CFG_KEY, 0),
                ctx.getResources().getString(R.string.pano_settings_focus_cfg_default));
        return Integer.parseInt(val);
    }

    public static int getVideoStabilizationCfg(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(CC_VIDEO_STABILIZATION_KEY, 0),
                ctx.getResources().getString(R.string.camera_settings_video_stabilization_default));
        return Integer.parseInt(val);
    }

    public static int getBurstNumber(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(CC_BURST_NUMBER_KEY, 0),
                ctx.getResources().getString(R.string.cc_settings_burst_number_default));
        return Integer.parseInt(val);
    }

    public static int getBurstInterval(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(CC_BURST_INTERVAL_KEY, 0),
                ctx.getResources().getString(R.string.cc_settings_burst_interval_default));
        return Integer.parseInt(val);
    }

    /**
     * Get the preferred image compress level
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return a preferred image compress level
     */
    public static int getImageCompressLevel(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_IMAGE_COMPRESS_LEVEL_KEY, 0),
                ctx.getResources().getString(R.string.pano_settings_image_compress_level_default));
        return Integer.parseInt(val);
    }

    /**
     * Get user preference whether to enable Ae/Af
     * @param ctx {@link Context}
     * @return true if enable AeAF and false otherwise
     */
    public static boolean getEnableAeAWB(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(
                CameraPreference.constructPreferenceKey(PANO_LOCK_AE_AWB_KEY, 0),
                ctx.getResources().getString(R.string.pano_settings_enable_ae_awb_lock_default));
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
            int cameraId, int titleResId, int entriesResId, int valuesResId,
            int defaultValueResId) {
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
