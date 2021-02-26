/*
 * Copyright (c) 2011-2013, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.android.profile;

import static com.android.profile.ProfileConst.AUDIO_PROLILE_MODE;
import static com.android.profile.ProfileConst.DEFAULT_BRIGHTNESS;
import static com.android.profile.ProfileConst.DEFAULT_VOLUME_RATIO;
import static com.android.profile.ProfileConst.DISABLE_CONNECTIVITY;
import static com.android.profile.ProfileConst.ENABLE_CONNECTIVITY;
import static com.android.profile.ProfileConst.LOG;
import static com.android.profile.ProfileConst.PROFILE_GENERAL;
import static com.android.profile.ProfileConst.PROFILE_MEETING;
import static com.android.profile.ProfileConst.PROFILE_OUTING;
import static com.android.profile.ProfileConst.PROFILE_SILENT;
import static com.android.profile.ProfileDataBaseAdapter.BLUETOOTH;
import static com.android.profile.ProfileDataBaseAdapter.BRIGHTNESS;
import static com.android.profile.ProfileDataBaseAdapter.DATA;
import static com.android.profile.ProfileDataBaseAdapter.GPS_LOCATION;
import static com.android.profile.ProfileDataBaseAdapter.NETWORK_LOCATION;
import static com.android.profile.ProfileDataBaseAdapter.PROFILE_NAME;
import static com.android.profile.ProfileDataBaseAdapter.RINGTONE1;
import static com.android.profile.ProfileDataBaseAdapter.RINGTONE2;
import static com.android.profile.ProfileDataBaseAdapter.RING_VOLUME;
import static com.android.profile.ProfileDataBaseAdapter.SILENT;
import static com.android.profile.ProfileDataBaseAdapter.VIBRATE;
import static com.android.profile.ProfileDataBaseAdapter.WIFI;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.location.LocationManager;
import android.media.AudioManager;
import android.media.RingtoneManager;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.provider.Settings.System;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.wrapper.ConnectivityManager_Wrapper;
import com.qualcomm.qti.wrapper.Settings_Wrapper;
import com.qualcomm.qti.wrapper.TelephonyManager_Wrapper;

public class ProfileApplyManager {

    private static final String TAG = "ProfileApplyManager";

    private Context mContext;
    private ProfileDataBaseAdapter mDataBaseAdapter = null;
    private Profile mProfile;

    private int maxRingVolume = 7;
    private final int MAX_VOLUME = 0;
    private final int DEFAULT_VOLUME = 1;
    private final int ZERO_VOLUME = 2;

    private AudioManager mAudioManager = null;
    private ConnectivityManager mConnectivityManager = null;
    private WifiManager mWifiManager = null;
    private BluetoothAdapter mBluetoothAdapter = null;

    private Callback mCallback = null;

    interface Callback {
        void updataWindowBrightness(int value);
    }

    public ProfileApplyManager(Context context) {
        mContext = context;
        if (mDataBaseAdapter == null) {
            mDataBaseAdapter = new ProfileDataBaseAdapter(context);
            mDataBaseAdapter.openDataBase();
        }
        getService();

        if (mDataBaseAdapter.fetchAllProfiles().getCount() <= 0) {
            resetProfileDataBase(true);
        }
    }

    public ProfileApplyManager(Context context, Callback callback) {
        this(context);
        mCallback = callback;
    }

    private void getService() {
        mAudioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        mConnectivityManager =
            (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public void clearDataBaseAdapter() {
        if (mDataBaseAdapter != null) {
            mDataBaseAdapter.closeDataBase();
            mDataBaseAdapter = null;
        }
    }

    public AudioManager getAudioManager() {
        return mAudioManager;
    }

    public ConnectivityManager getConnectivityManager() {
        return mConnectivityManager;
    }

    public WifiManager getWifiManager() {
        return mWifiManager;
    }

    public BluetoothAdapter getBluetoothAdapter() {
        return mBluetoothAdapter;
    }

    public Profile getProfile() {
        return mProfile;
    }

    public void resetProfileDataBase(boolean newFlashed) {

        int def_ring_volume = mContext.getResources().getInteger(R.integer.def_volume);
        int def_brightness = DEFAULT_BRIGHTNESS;
        String def_ringtone1 = null;
        String def_ringtone2 = null;

        final SharedPreferences mSharedPreferences = mContext.getSharedPreferences(
                "com.android.profile_preferences", 0);
        mDataBaseAdapter.deleteAllProfiles();
        if (newFlashed) {
            Profile mProfile = getCurrentSettings();
            def_brightness = mProfile.getBrightness();
            def_ringtone1 = getCurrentRingtone(0);
            def_ringtone2 = getCurrentRingtone(1);

            SharedPreferences.Editor editor = mSharedPreferences.edit();
            editor.putInt(ProfileUtils.KEY_DEFAUT_RINGVOLUME, def_ring_volume);
            editor.putInt(ProfileUtils.KEY_DEFAUT_BRIGHTNESS, def_brightness);
            editor.putString(ProfileUtils.KEY_DEFAUT_RINGTONE1, def_ringtone1);
            editor.putString(ProfileUtils.KEY_DEFAUT_RINGTONE2, def_ringtone2);
            editor.commit();
        } else {
            def_ring_volume = mSharedPreferences.getInt(ProfileUtils.KEY_DEFAUT_RINGVOLUME,
                    def_ring_volume);
            def_brightness = mSharedPreferences.getInt(ProfileUtils.KEY_DEFAUT_BRIGHTNESS,
                    def_brightness);
            def_ringtone1 = mSharedPreferences.getString(ProfileUtils.KEY_DEFAUT_RINGTONE1,
                    def_ringtone1);
            def_ringtone2 = mSharedPreferences.getString(ProfileUtils.KEY_DEFAUT_RINGTONE2,
                    def_ringtone2);
        }
        // Profile(String profile_name, boolean silent, boolean vibrate, String
        // ringtone1,
        // String ringtone2, int ring_volume, boolean data, boolean wifi,
        // boolean bluetooth,
        // boolean gpsLocation, boolean networkLocation, int brightness)
        Profile generalProfile = new Profile(PROFILE_GENERAL, false, false,
                def_ringtone1, def_ringtone2, def_ring_volume, true, false, false, false, false,
                def_brightness);
        Profile meetingProfile = new Profile(PROFILE_MEETING, true, true,
                def_ringtone1, def_ringtone2, 0, true, false, false, false, false, def_brightness);
        Profile outingProfile = new Profile(PROFILE_OUTING, false, true, def_ringtone1,
                def_ringtone2, maxRingVolume, true, false, false, false, false, (int) (0.7 * 255));
        Profile silentProfile = new Profile(PROFILE_SILENT, true, false, def_ringtone1,
                def_ringtone2, 0, true, false, false, false, false, def_brightness);
        //Profile powersaveProfile = new Profile(PROFILE_POWERSAVE, false, false, def_ringtone1,
        //        def_ringtone2, def_ring_volume, false, false, false, false, false, 1);

        //Please note: If change the default profiles, please also update DEFAULT_PROFILE_COUNT
        mDataBaseAdapter.insertProfile(generalProfile);
        mDataBaseAdapter.insertProfile(meetingProfile);
        mDataBaseAdapter.insertProfile(outingProfile);
        mDataBaseAdapter.insertProfile(silentProfile);
        //mDataBaseAdapter.insertProfile(powersaveProfile);
    }

    public Profile getCurrentSettings() {

        Profile mProfile = new Profile();
        boolean silentCur = false;
        boolean vibrateCur = false;
        int ring_volumeCur;
        boolean dataCur = true;
        boolean wifiCur = false;
        boolean bluetoothCur = false;
        boolean gpsLocationCur = false;
        boolean networkLocationCur = false;
        int brightnessCur = DEFAULT_BRIGHTNESS;
        String ringtone1Cur = null;
        String ringtone2Cur = null;

        // whether in silent
        silentCur = false;
        if (mAudioManager.getRingerMode() == AudioManager.RINGER_MODE_SILENT
                || mAudioManager.getRingerMode() == AudioManager.RINGER_MODE_VIBRATE) {
            silentCur = true;
        }

        if (LOG) {
            if (mAudioManager.getRingerMode() == AudioManager.RINGER_MODE_VIBRATE)
                logd("RingerMode=Vibrate");
            if (mAudioManager.getRingerMode() == AudioManager.RINGER_MODE_SILENT)
                logd("RingerMode=Silent");
            if (mAudioManager.getRingerMode() == AudioManager.RINGER_MODE_NORMAL)
                logd("RingerMode=Normal");

            if (mAudioManager.getVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER)
                == AudioManager.VIBRATE_SETTING_ONLY_SILENT)
                logd("VIBRATE_TYPE_RINGER=VIBRATE_SETTING_ONLY_SILENT");
            if (mAudioManager.getVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER)
                == AudioManager.VIBRATE_SETTING_ON)
                logd("VIBRATE_TYPE_RINGER=VIBRATE_SETTING_ON");
            if (mAudioManager.getVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER)
                == AudioManager.VIBRATE_SETTING_OFF)
                logd("VIBRATE_TYPE_RINGER=VIBRATE_SETTING_OFF");

            if (mAudioManager.getVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION)
                == AudioManager.VIBRATE_SETTING_ONLY_SILENT)
                logd("VIBRATE_TYPE_NOTIFICATION=VIBRATE_SETTING_ONLY_SILENT");
            if (mAudioManager.getVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION)
                == AudioManager.VIBRATE_SETTING_ON)
                logd("VIBRATE_TYPE_NOTIFICATION=VIBRATE_SETTING_ON");
            if (mAudioManager.getVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION)
                == AudioManager.VIBRATE_SETTING_OFF)
                logd("VIBRATE_TYPE_NOTIFICATION=VIBRATE_SETTING_OFF");

            boolean vibeInSilent = (Settings.System.getInt(mContext.getContentResolver(),
                    Settings_Wrapper.VIBRATE_IN_SILENT, 1) == 1);
            logd("vibeInSilent=" + vibeInSilent);
        }
        // Fetch the value of vibrator according to method soundSetting which
        // was set before.
        vibrateCur = Settings.System.getInt(mContext.getContentResolver(),
                Settings_Wrapper.VIBRATE_IN_SILENT, 1) == 1;

        ConnectivityManager_Wrapper cmWrapper = new ConnectivityManager_Wrapper(mContext);
        if (cmWrapper != null) {
            dataCur = cmWrapper.getMobileDataEnabled();
        }

        if (mWifiManager != null) {
            int wifiState = mWifiManager.getWifiState();
            if (wifiState == WifiManager.WIFI_STATE_ENABLED
                    || wifiState == WifiManager.WIFI_STATE_ENABLING)
                wifiCur = true;
            else
                wifiCur = false;
        }
        if (mBluetoothAdapter != null) {
            int bluetoothState = mBluetoothAdapter.getState();
            if (bluetoothState == BluetoothAdapter.STATE_ON
                    || bluetoothState == BluetoothAdapter.STATE_TURNING_ON)
                bluetoothCur = true;
            else
                bluetoothCur = false;
        }

        gpsLocationCur = Settings.Secure.isLocationProviderEnabled(mContext.getContentResolver(),
                LocationManager.GPS_PROVIDER);

        networkLocationCur = Settings.Secure.isLocationProviderEnabled(mContext
                .getContentResolver(), LocationManager.NETWORK_PROVIDER);

        try {
            brightnessCur = Settings.System.getInt(mContext.getContentResolver(),
                    "screen_brightness");
        } catch (SettingNotFoundException e) {
            e.printStackTrace();
        }

        ring_volumeCur = mAudioManager.getStreamVolume(AudioManager.STREAM_RING);

        ringtone1Cur = getCurrentRingtone(0);
        ringtone2Cur = getCurrentRingtone(1);

        mProfile.setSilent(silentCur);
        mProfile.setVibrate(vibrateCur);
        mProfile.setRingtone1(ringtone1Cur);
        mProfile.setRingtone2(ringtone2Cur);
        mProfile.setRingVolume(ring_volumeCur);
        mProfile.setData(dataCur);
        mProfile.setWifi(wifiCur);
        mProfile.setBluetooth(bluetoothCur);
        mProfile.setGpsLocation(gpsLocationCur);
        mProfile.setNetworkLocation(networkLocationCur);
        mProfile.setBrightness(brightnessCur);
        return mProfile;
    }

    public String getCurrentRingtone(int slot) {

        int ringtoneTypes[] = {
                RingtoneManager.TYPE_RINGTONE, ProfileSetting.TYPE_RINGTONE_2
        };

        if (slot < 0 || slot > 1)
            return null;
        Uri ring = RingtoneManager.getActualDefaultRingtoneUri(mContext, ringtoneTypes[slot]);
        if (ring == null)
            return null;
        return ring.toString();
    }

    /**
     * Apply the current profile
     *
     * @param profileId the profile id in the table of profile database
     */
    public void applyProfile(int profileId) {
        if (profileId == -1) {
            return;
        }

        if (mDataBaseAdapter == null) {
            mDataBaseAdapter = new ProfileDataBaseAdapter(mContext);
            mDataBaseAdapter.openDataBase();
        }

        Cursor cursor = mDataBaseAdapter.fetchProfile(profileId);
        if (cursor == null || !cursor.moveToFirst()){
            return;
        }

        // get profile
        mProfile = getSavedSettings(cursor);
        cursor.close();

        // Set profile
        soundSetting(mProfile.isSilent(), mProfile.isVibrate(), mProfile.getRingVolume());
        dataSetting(mProfile.isData());
        wifiSetting(mProfile.isWifi());
        bluetoothSetting(mProfile.isBluetooth());
        gpsLocationSetting(mProfile.isGpsLocation());
        networkLocationSetting(mProfile.isNetworkLocation());
        brightnessSetting(mProfile.getBrightness());
        ringtoneSetting(mProfile.getRingtone1(), mProfile.getRingtone2());

        displayToast(ProfileUtils.getShowProfileName(mContext, mProfile.getProfileName())
                + " " + mContext.getString(R.string.profile_enable));

        // setSelectedId(profileId);

        // mModifyInThisApp = true;
        ProfileConst.MODIFY_IN_THIS_APP = true;
    }

    private Profile getSavedSettings(Cursor mCursor) {

        // logd("");
        Profile mProfile = new Profile();
        String profile_name = "new";
        boolean silent = false;
        boolean vibrate = true;
        String ringtone1 = null;
        String ringtone2 = null;
        int ring_volume = 0;
        boolean data = true;
        boolean wifi = false;
        boolean bluetooth = false;
        boolean gpsLocation = false;
        boolean networkLocation = false;
        int brightness = 20;

        profile_name = mCursor.getString(mCursor.getColumnIndex(PROFILE_NAME));
        silent = mCursor.getInt(mCursor.getColumnIndex(SILENT)) > 0 ? true : false;
        vibrate = mCursor.getInt(mCursor.getColumnIndex(VIBRATE)) > 0 ? true : false;
        ringtone1 = mCursor.getString(mCursor.getColumnIndex(RINGTONE1));
        ringtone2 = mCursor.getString(mCursor.getColumnIndex(RINGTONE2));
        ring_volume = mCursor.getInt(mCursor.getColumnIndex(RING_VOLUME));
        data = mCursor.getInt(mCursor.getColumnIndex(DATA)) > 0 ? true : false;
        wifi = mCursor.getInt(mCursor.getColumnIndex(WIFI)) > 0 ? true : false;
        bluetooth = mCursor.getInt(mCursor.getColumnIndex(BLUETOOTH)) > 0 ? true : false;
        gpsLocation = mCursor.getInt(mCursor.getColumnIndex(GPS_LOCATION)) > 0 ? true : false;
        networkLocation = mCursor.getInt(mCursor.getColumnIndex(NETWORK_LOCATION)) > 0 ? true
                : false;
        brightness = mCursor.getInt(mCursor.getColumnIndex(BRIGHTNESS));

        mProfile.setProfileName(profile_name);
        mProfile.setSilent(silent);
        mProfile.setVibrate(vibrate);
        mProfile.setRingtone1(ringtone1);
        mProfile.setRingtone2(ringtone2);
        mProfile.setRingVolume(ring_volume);
        mProfile.setData(data);
        mProfile.setWifi(wifi);
        mProfile.setBluetooth(bluetooth);
        mProfile.setGpsLocation(gpsLocation);
        mProfile.setNetworkLocation(networkLocation);
        mProfile.setBrightness(brightness);

        return mProfile;
    }

    private void soundSetting(boolean mSilent, boolean mVibrate, int mVolume) {

        int vibInSilent = 1;

        // Variable to sync the vibrate of system settings with profile manager,
        // 1 enable vibrate, 0 disable.
        Log.d(TAG, "mSilent:" + mSilent + "|mVibrate:" + mVibrate + "|mVolume:"
                + mVolume);
        int vibInRinging = 1;
        if (mSilent) {
            // muteAllSound(true);
            setOtherVolume(ZERO_VOLUME);
            if (mVibrate) { // only vibrate
                mAudioManager.setRingerMode(AudioManager.RINGER_MODE_VIBRATE);
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER,
                        AudioManager.VIBRATE_SETTING_ON);
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION,
                        AudioManager.VIBRATE_SETTING_ON);
                vibInSilent = 1;
                vibInRinging = 1;
            } else
            // neither vibrate nor ringer
            {
                mAudioManager.setRingerMode(AudioManager.RINGER_MODE_SILENT);
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER,
                        AudioManager.VIBRATE_SETTING_OFF);
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION,
                        AudioManager.VIBRATE_SETTING_OFF);
                vibInSilent = 0;
                vibInRinging = 0;
            }

        } else {
            // muteAllSound(false);
            //when it's not slient all sounds,
            //needn't set the music and alarm sound volume by profile
            //setOtherVolume(DEFAULT_VOLUME);

            // both vibrate and ringer
            mAudioManager.setRingerMode(AudioManager.RINGER_MODE_NORMAL);
            if (mVibrate) {
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER,
                        AudioManager.VIBRATE_SETTING_ON);
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION,
                        AudioManager.VIBRATE_SETTING_ON);
                vibInSilent = 1;
                vibInRinging = 1;
            } else {
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_RINGER,
                        AudioManager.VIBRATE_SETTING_OFF);
                mAudioManager.setVibrateSetting(AudioManager.VIBRATE_TYPE_NOTIFICATION,
                        AudioManager.VIBRATE_SETTING_OFF);
                vibInSilent = 0;
                vibInRinging = 0;
            }
            // ring_volume & notification_volume setting
            if (ProfileFunctionConfig.isRingVolumeCtrlEnabled()) {
                mAudioManager.setStreamVolume(AudioManager.STREAM_RING, mVolume,
                        AudioManager.FLAG_REMOVE_SOUND_AND_VIBRATE);
                mAudioManager.setStreamVolume(AudioManager.STREAM_NOTIFICATION, mVolume,
                        AudioManager.FLAG_REMOVE_SOUND_AND_VIBRATE);
            }
        }
        Settings.System
                .putInt(mContext.getContentResolver(),
                    Settings_Wrapper.VIBRATE_IN_SILENT, vibInSilent);
        Settings.System
                .putInt(mContext.getContentResolver(),
                    Settings.System.VIBRATE_WHEN_RINGING, vibInRinging);
        Settings.System
                .putInt(mContext.getContentResolver(),
                    ProfileSetting.VIBRATE_WHEN_RINGING2, vibInRinging);
    }

    private void setOtherVolume(int mode) {

        int music_volume = 0, alarm_volume = 0;
        switch (mode) {
        case ZERO_VOLUME:
            break;
        case DEFAULT_VOLUME:
            music_volume = Math.round(DEFAULT_VOLUME_RATIO
                    * mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC));
            alarm_volume = Math.round(DEFAULT_VOLUME_RATIO
                    * mAudioManager.getStreamMaxVolume(AudioManager.STREAM_ALARM));
            setMusicAlarmVolume(music_volume, alarm_volume);
            break;
        case MAX_VOLUME:
            music_volume = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
            alarm_volume = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_ALARM);
            setMusicAlarmVolume(music_volume, alarm_volume);
            break;
        default:
            break;
        }
    }

    private void setMusicAlarmVolume(int musicVolume, int alarmVolume) {
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, musicVolume,
                AudioManager.FLAG_REMOVE_SOUND_AND_VIBRATE);
        mAudioManager.setStreamVolume(AudioManager.STREAM_ALARM, alarmVolume,
                AudioManager.FLAG_REMOVE_SOUND_AND_VIBRATE);
    }

    private void dataSetting(boolean data) {

        if (!ProfileFunctionConfig.isDataCtrlEnabled())
            return;
        if (data == DISABLE_CONNECTIVITY)
            setDataEnabled(DISABLE_CONNECTIVITY);
        else
            setDataEnabled(ENABLE_CONNECTIVITY);

    }

    // Should be unmuted by the same object, so it is not recommended.
    // private void muteAllSound(boolean mute) {
    // mAudioManager.setStreamMute(AudioManager.STREAM_MUSIC, mute);
    // mAudioManager.setStreamMute(AudioManager.STREAM_ALARM, mute);
    // mAudioManager.setStreamMute(AudioManager.STREAM_RING, mute);
    // mAudioManager.setStreamMute(AudioManager.STREAM_NOTIFICATION, mute);
    // mAudioManager.setStreamMute(AudioManager.STREAM_SYSTEM, mute);
    // }

    public void setDataEnabled(boolean para) {
        TelephonyManager_Wrapper wrapperInstance
                = TelephonyManager_Wrapper.getDefaultWrapperInstance();
        if (wrapperInstance != null) {
            wrapperInstance.setDataEnabled(para);
        } else {
            Log.e(TAG, "phone=null");
        }
    }

    private void wifiSetting(boolean wifi) {

        boolean airplane = (System.getInt(mContext.getContentResolver(),
            System.AIRPLANE_MODE_ON, 0) != 0);
        if (airplane) {
            Log.i(TAG, "airplane on");
            return;
        }
        if (!ProfileFunctionConfig.isWifiCtrlEnabled())
            return;
        if (mWifiManager != null)
            mWifiManager.setWifiEnabled(wifi);
        else
            Log.e(TAG, "mWifiManager=null");
    }

    private void bluetoothSetting(boolean bluetooth) {

        if (!ProfileFunctionConfig.isBluetoothCtrlEnabled())
            return;
        if (mBluetoothAdapter != null) {
            if (bluetooth)
                mBluetoothAdapter.enable();
            else
                mBluetoothAdapter.disable();
        }
    }

    private void gpsLocationSetting(boolean gpsLocation) {

        if (!ProfileFunctionConfig.isGpsLocationCtrlEnabled())
            return;

        try {
            Settings.Secure.setLocationProviderEnabled(mContext.getContentResolver(),
                    LocationManager.GPS_PROVIDER, gpsLocation);
        } catch (Exception e) {
        }
    }

    private void networkLocationSetting(boolean networkLocation) {

        if (!ProfileFunctionConfig.isNetworkLocationCtrlEnabled())
            return;

        try {
            Settings.Secure.setLocationProviderEnabled(mContext.getContentResolver(),
                    LocationManager.NETWORK_PROVIDER, networkLocation);
        } catch (Exception e) {

        }
    }

    private void brightnessSetting(int brightness) {

        Settings.System.putInt(mContext.getContentResolver(),
            Settings.System.SCREEN_BRIGHTNESS_MODE, Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
        if (!android.provider.Settings.System.putInt(mContext.getContentResolver(),
                android.provider.Settings.System.SCREEN_BRIGHTNESS, brightness))
            Log.e(TAG, "SCREEN_BRIGHTNESS writing error");
        Uri localUri = Settings.System.getUriFor("screen_brightness");
        mContext.getContentResolver().notifyChange(localUri, null);
        int value;
        value = brightness;
        if (brightness <= 0)
            value = 1;
        else if (brightness > 255)
            value = 255;
        value = brightness <= 0 ? 1 : brightness;
        if (mCallback != null) {
            mCallback.updataWindowBrightness(value);
        }
        // WindowManager.LayoutParams mParam = mContext.getWindow().getAttributes();
        // mParam.screenBrightness = value / 255.0f; // 0.0 - 1.0
        // mContext.getWindow().setAttributes(mParam);
    }

    private void ringtoneSetting(String ringtone1, String ringtong2) {
        if (!ProfileFunctionConfig.isRingtoneCtrlEnabled())
            return;
        Uri uri1 = (ringtone1 != null) ? Uri.parse(ringtone1) : null;
        Uri uri2 = (ringtong2 != null) ? Uri.parse(ringtong2) : null;
        //only can set default ringtone, can not set slot2.
        RingtoneManager.setActualDefaultRingtoneUri(mContext, RingtoneManager.TYPE_RINGTONE, uri1);
        //RingtoneManager.setActualRingtoneUriBySubId(this, SUB_1, uri1);
        //RingtoneManager.setActualRingtoneUriBySubId(this, SUB_2, uri2);
    }

    public void displayToast(Object s) {

        Toast.makeText(mContext, s + "", Toast.LENGTH_SHORT).show();
    }

    void setSelectedId(int id) {

        // ProfileUtils.saveIntValue(mContext, ProfileUtils.KEY_SELECTED_ID, id);
        Settings.Global.putInt(mContext.getContentResolver(), AUDIO_PROLILE_MODE, id);
    }

    int getSelectedId() {

        // return ProfileUtils.getIntValueSaved(mContext, ProfileUtils.KEY_SELECTED_ID, -1);
        return Settings.Global.getInt(mContext.getContentResolver(), AUDIO_PROLILE_MODE, -1);
    }

    private void logd(Object s) {

        if (!LOG)
            return;
        Thread mThread = Thread.currentThread();
        StackTraceElement[] mStackTrace = mThread.getStackTrace();
        String mMethodName = mStackTrace[3].getMethodName();

        s = "[" + mMethodName + "] " + s;
        Log.d(TAG, s + "");
    }

}