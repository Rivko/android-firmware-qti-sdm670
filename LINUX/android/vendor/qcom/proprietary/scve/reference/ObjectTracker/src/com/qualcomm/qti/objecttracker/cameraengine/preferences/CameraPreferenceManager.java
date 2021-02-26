/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttracker.cameraengine.preferences;

import java.util.ArrayList;
import java.util.Collection;

import com.qualcomm.qti.objecttracker.tracker.OperationMode;
import com.qualcomm.qti.objecttracker.tracker.Precision;
import com.qualcomm.qti.objecttrackerdemo.R;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceCategory;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;

/**
 * This class exposes a settings framework for use by an app to control the camera and SCVE settings
 */
@SuppressWarnings("deprecation")
public abstract class CameraPreferenceManager {

    private static final String CAMERA_SELECTED_ID_KEY = "camerapref_camera_id";
    private static final String SCVE_MODE_KEY = "scvepref_scve_mode";
    private static final String SCVE_TRACKER_PRECISION_KEY = "scvepref_tracker_precision";
    private static final String SCVE_AE_AF_CONTROLLER_KEY = "scvepref_ae_af_controller";
    private static final String DEBUG_OSD_KEY = "debugpref_osd";

    /**
     * This method will create and add a set of {@link Preference}s to the given {@link
     * PreferenceScreen}
     * @param ps {@link PreferenceScreen} to which to add {@link Camera} settings
     * @param cameraId ID of the {@link Camera} for which {@link Preference}s should be created
     * @param p {@link Parameters} for the given {@link Camera} ID
     */
    public static void addCameraPreferenceEntries(PreferenceScreen ps, int cameraId, Parameters p) {
        Context c = ps.getContext();

        PreferenceCategory cameraPrefs = new PreferenceCategory(c);
        cameraPrefs.setTitle(R.string.camera_settings);
        ps.addPreference(cameraPrefs);

        PreferenceCategory scvePrefs = new PreferenceCategory(c);
        scvePrefs.setTitle(R.string.scve_settings);
        ps.addPreference(scvePrefs);

        PreferenceCategory debugPrefs = new PreferenceCategory(c);
        debugPrefs.setTitle(R.string.debug_settings);
        ps.addPreference(debugPrefs);

        for (CameraPreference pref : createCameraPreferences(c, cameraId, p)) {
            cameraPrefs.addPreference(pref.createPreferenceEntry());
        }

        scvePrefs.addPreference(createListPreferenceFromResources(c, SCVE_MODE_KEY,
                R.string.scve_settings_scve_mode,
                R.array.scve_settings_scve_mode_entries,
                R.array.scve_settings_scve_mode_values,
                R.string.scve_settings_scve_mode_default));
        scvePrefs.addPreference(createListPreferenceFromResources(c, SCVE_TRACKER_PRECISION_KEY,
                R.string.scve_settings_tracker_precision,
                R.array.scve_settings_tracker_precision_entries,
                R.array.scve_settings_tracker_precision_values,
                R.string.scve_settings_tracker_precision_default));
        scvePrefs.addPreference(createListPreferenceFromResources(c, SCVE_AE_AF_CONTROLLER_KEY,
                R.string.scve_settings_ae_af_controller,
                R.array.scve_settings_ae_af_controller_entries,
                R.array.scve_settings_ae_af_controller_values,
                R.string.scve_settings_ae_af_controller_default));

        debugPrefs.addPreference(createListPreferenceFromResources(c, DEBUG_OSD_KEY,
                R.string.debug_settings_osd,
                R.array.debug_settings_osd_entries,
                R.array.debug_settings_osd_values,
                R.string.debug_settings_osd_default));
    }

    /**
     * Apply the {@link Preference}s to the given {@link Parameters} instance
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @param cameraId ID of the {@link Camera} to which {@link Preference}s should be applied
     * @param p {@link Parameters} for the given {@link Camera} ID
     */
    public static void applyCameraPreferences(Context ctx, int cameraId, Parameters p) {
        for (CameraPreference pref : createCameraPreferences(ctx, cameraId, p)) {
            pref.apply();
        }
    }

    /**
     * Get the ID of the selected {@link Camera}
     * @param ctx {@link Context} from which to load the {@link Camera} ID
     * @return the selected {@link Camera} ID
     */
    public static int getSelectedCameraId(Context ctx) {
        return PreferenceManager.getDefaultSharedPreferences(ctx).getInt(CAMERA_SELECTED_ID_KEY, 0);
    }

    /**
     * Set the ID of the selected {@link Camera}
     * @param ctx {@link Context} to which to save the {@link Camera} ID
     * @param cameraId {@link Camera} ID to save
     */
    public static void setSelectedCameraId(Context ctx, int cameraId) {
        PreferenceManager.getDefaultSharedPreferences(ctx).edit().putInt(CAMERA_SELECTED_ID_KEY,
                cameraId).apply();
    }

    /**
     * Get the SCVE mode from the camera settings
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return an {@link OperationMode} value
     */
    public static OperationMode getScveMode(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(SCVE_MODE_KEY,
                ctx.getResources().getString(R.string.scve_settings_scve_mode_default));
        return OperationMode.valueOf(val);
    }

    /**
     * Get the SCVE object tracker precision from the camera settings
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return a {@link Precision} value
     */
    public static Precision getScveObjectTrackerPrecision(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(SCVE_TRACKER_PRECISION_KEY,
                ctx.getResources().getString(R.string.scve_settings_tracker_precision_default));
        return Precision.valueOf(val);
    }

    /**
     * Get whether the AE/AF controller should be enabled
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return true if the AE/AF controller should be enabled, else false
     */
    public static boolean isAeAfControllerEnabled(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(SCVE_AE_AF_CONTROLLER_KEY,
                ctx.getResources().getString(R.string.scve_settings_ae_af_controller_default));
        return Boolean.parseBoolean(val);
    }

    /**
     * Get whether the on-screen diagnostics display should be enabled
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @return true if the on-screen diagnostics should be enabled, else false
     */
    public static boolean isDebugOnScreenDiagnosticsEnabled(Context ctx) {
        String val = PreferenceManager.getDefaultSharedPreferences(ctx).getString(DEBUG_OSD_KEY,
                ctx.getResources().getString(R.string.debug_settings_osd_default));
        return Boolean.parseBoolean(val);
    }

    /**
     * Create a set of {@link CameraPreference} objects for the specified camera
     * @param ctx {@link Context} from which to load {@link Preference} values
     * @param cameraId ID of the {@link Camera} to which {@link Preference}s should be applied
     * @param p {@link Parameters} for the given {@link Camera} ID
     * @return a new {@link Collection} of {@link CameraPreference}s
     */
    private static Collection<CameraPreference> createCameraPreferences(Context ctx, int cameraId,
            Parameters p) {
        ArrayList<CameraPreference> list = new ArrayList<CameraPreference>();
        list.add(new CameraPreferencePreviewResolution(ctx, cameraId, p));
        list.add(new CameraPreferenceExposureCompensation(ctx, cameraId, p));
        list.add(new CameraPreferenceWhiteBalance(ctx, cameraId, p));
        list.add(new CameraPreferenceSceneMode(ctx, cameraId, p));
        list.add(new CameraPreferenceFocusMode(ctx, cameraId, p));
        if (CameraPreferenceISO.isSupported(p)) {
            list.add(new CameraPreferenceISO(ctx, cameraId, p));
        }

        return list;
    }

    /**
     * Helper method to construct a {@link ListPreference} from a series of resources
     * @param ctx {@link Context} from which to store/load this {@link ListPreference}'s value
     * @param key Key value to use when storing/loading this setting
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
    private static ListPreference createListPreferenceFromResources(Context ctx, String key,
            int titleResId, int entriesResId, int valuesResId, int defaultValueResId) {
        ListPreference lp = new ListPreference(ctx);
        lp.setKey(key);
        lp.setTitle(titleResId);
        lp.setEntries(entriesResId);
        lp.setEntryValues(valuesResId);
        lp.setDefaultValue(ctx.getResources().getString(defaultValueResId));
        lp.setSummary("%s");
        return lp;
    }
}
