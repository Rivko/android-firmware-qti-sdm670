/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.fr;

import android.content.Context;
import android.preference.MultiSelectListPreference;
import android.preference.Preference;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.preference.SwitchPreference;
import android.widget.Toast;

import com.qualcomm.qti.fr_camera.R;

import java.text.Collator;
import java.util.Comparator;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

/**
 * {@link FacePreferences} provides methods for creating {@link Preference}s which allows
 * controlling the {@link FaceRecognizer} database from a settings screen
 */
public class FacePreferences {
    private static final String DEBUG_PREF_FR_VERBOSE_LOGGING = "debug_pref_fr_verbose_logging";
    private static final String DEBUG_PREF_FR_CPU_OFFLOAD = "debug_pref_fr_cpu_offload";

    // Not constructable
    private FacePreferences() {}

    /**
     * Add {@link Preference}s for removing face(s) from the {@link FaceRecognizer} database to the
     * specified {@link PreferenceGroup}
     * @param preferenceGroup a {@link PreferenceGroup} to which to add {@link FaceRecognizer}
     *                        database control {@link Preference}s
     * @throws NullPointerException if preferenceGroup is null
     */
    public static void addFaceRecognizerPreferences(PreferenceGroup preferenceGroup) {
        Objects.requireNonNull(preferenceGroup, "preferenceGroup may not be null");

        final Context context = preferenceGroup.getContext();
        final UserDatabase userDatabase = UserDatabase.getInstance(context);

        final MultiSelectListPreference removeUsersPref = new MultiSelectListPreference(context);
        removeUsersPref.setTitle(R.string.prefs_fr_db_remove_user_title);
        updateUserList(userDatabase, removeUsersPref);
        removeUsersPref.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                @SuppressWarnings("unchecked")
                Set<String> removeUsers = (Set<String>) newValue;

                FaceRecognizer fr = createFaceRecognizer(context);
                for (String id : removeUsers) {
                    userDatabase.removeUser(id);
                    fr.removeFace(id);
                }
                fr.close();

                updateUserList(userDatabase, removeUsersPref);

                return false;
            }
        });
        preferenceGroup.addPreference(removeUsersPref);

        final Preference removeAllUsersPref = new Preference(context);
        removeAllUsersPref.setTitle(R.string.prefs_fr_clear_db_title);
        removeAllUsersPref.setSummary(R.string.prefs_fr_clear_db_summary);
        removeAllUsersPref.setOnPreferenceClickListener(
                new Preference.OnPreferenceClickListener() {
                    @Override
                    public boolean onPreferenceClick(Preference preference) {
                        UserDatabase userDatabase = UserDatabase.getInstance(context);
                        userDatabase.removeAllUsers();

                        FaceRecognizer fr = createFaceRecognizer(context);
                        fr.removeAllFaces();
                        fr.close();

                        updateUserList(userDatabase, removeUsersPref);

                        Toast.makeText(context, R.string.camera_settings_fr_database_cleared,
                                Toast.LENGTH_SHORT).show();

                        return true;
                    }
                });
        preferenceGroup.addPreference(removeAllUsersPref);
    }

    /**
     * Add {@link FaceRecognizer} debug preferences to the provided {@link PreferenceGroup}
     * @param preferenceGroup the {@link PreferenceGroup} to which to add {@link FaceRecognizer}
     *                        debug preferences
     * @throws NullPointerException if preferenceGroup is null
     */
    public static void addDebugPreferences(PreferenceGroup preferenceGroup) {
        Objects.requireNonNull(preferenceGroup, "preferenceGroup may not be null");

        final Context context = preferenceGroup.getContext();

        SwitchPreference pref = new SwitchPreference(context);
        pref.setKey(DEBUG_PREF_FR_VERBOSE_LOGGING);
        pref.setTitle(R.string.debug_pref_fr_verbose_logging);
        pref.setDefaultValue(context.getResources().getBoolean(
                R.bool.debug_pref_fr_verbose_logging_default_value));
        preferenceGroup.addPreference(pref);

        SwitchPreference cpuOffloadPref = new SwitchPreference(context);
        cpuOffloadPref.setKey(DEBUG_PREF_FR_CPU_OFFLOAD);
        cpuOffloadPref.setTitle(R.string.debug_pref_fr_cpu_offload_mode);
        cpuOffloadPref.setDefaultValue(context.getResources().getBoolean(
                R.bool.debug_pref_fr_cpu_offload_mode_default_value));
        preferenceGroup.addPreference(cpuOffloadPref);
    }

    /**
     * Test whether {@link FaceRecognizer} verbose logging is enabled
     * @param context the active {@link Context}
     * @return true if {@link FaceRecognizer} verbose logging is enabled, else false
     * @throws NullPointerException if context is null
     */
    public static boolean isVerboseLoggingEnabled(Context context) {
        Objects.requireNonNull(context, "context may not be null");

        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(DEBUG_PREF_FR_VERBOSE_LOGGING, context.getResources().getBoolean(
                        R.bool.debug_pref_fr_verbose_logging_default_value));
    }

    public static boolean isCPUOffloadEnabled(Context context) {
        Objects.requireNonNull(context, "context may not be null");
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(DEBUG_PREF_FR_CPU_OFFLOAD, context.getResources().getBoolean(
                        R.bool.debug_pref_fr_cpu_offload_mode_default_value));

    }

    private static FaceRecognizer createFaceRecognizer(Context context) {
        return new FaceRecognizer(context, FaceRecognizer.ScveMode.DEFAULT,
                FaceRecognizer.FaceRecoMode.STILL_IMAGE, FaceRecognizer.FrameFormat.GRAYSCALE, FaceRecognizerWorker.PreviousWidth, FaceRecognizerWorker.PreviousHeight,
                new int[]{FaceRecognizerWorker.PreviousRowStride}, 30);
    }

    private static void updateUserList(UserDatabase userDatabase, MultiSelectListPreference lp) {
        Map<String, UserDatabase.UserRecord> users = userDatabase.getAllUserRecords();

        SortedSet<User> sortedUsers = new TreeSet<>(new UserComparator());
        for (Map.Entry<String, UserDatabase.UserRecord> e : users.entrySet()) {
            sortedUsers.add(new User(e.getKey(), e.getValue().name));
        }

        CharSequence[] entries = new CharSequence[sortedUsers.size()];
        CharSequence[] entryValues = new CharSequence[entries.length];
        int i = 0;
        for (User u : sortedUsers) {
            entries[i] = u.name;
            entryValues[i++] = u.id;
        }

        lp.setEntries(entries);
        lp.setEntryValues(entryValues);
    }

    private static class User {
        private final String id;
        private final CharSequence name;

        private User(String id, CharSequence name) {
            this.id = id;
            this.name = name;
        }
    }

    private static class UserComparator implements Comparator<User> {
        private static final Collator LOCALE_COLLATOR = Collator.getInstance();

        @Override
        public int compare(User lhs, User rhs) {
            if (lhs == rhs) {
                return 0;
            } else if (lhs == null) {
                return -1;
            } else if (rhs == null) {
                return 1;
            } else {
                return LOCALE_COLLATOR.compare(lhs.name.toString(), rhs.name.toString());
            }
        }
    }
}
