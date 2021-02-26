/*
 * Copyright (c) 2011-2013,2016, QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.android.profile;

import static com.android.profile.ProfileConst.EDIT_PROFILE;
import static com.android.profile.ProfileConst.LOG;
import static com.android.profile.ProfileConst.NEW_PROFILE;
import static com.android.profile.ProfileDataBaseAdapter.BLUETOOTH;
import static com.android.profile.ProfileDataBaseAdapter.BRIGHTNESS;
import static com.android.profile.ProfileDataBaseAdapter.DATA;
import static com.android.profile.ProfileDataBaseAdapter.GPS_LOCATION;
import static com.android.profile.ProfileDataBaseAdapter.ID;
import static com.android.profile.ProfileDataBaseAdapter.NETWORK_LOCATION;
import static com.android.profile.ProfileDataBaseAdapter.PROFILE_NAME;
import static com.android.profile.ProfileDataBaseAdapter.RINGTONE1;
import static com.android.profile.ProfileDataBaseAdapter.RINGTONE2;
import static com.android.profile.ProfileDataBaseAdapter.RING_VOLUME;
import static com.android.profile.ProfileDataBaseAdapter.SILENT;
import static com.android.profile.ProfileDataBaseAdapter.VIBRATE;
import static com.android.profile.ProfileDataBaseAdapter.WIFI;
import static com.android.profile.ProfileItemView.STATE_CURRENT_PROFILE;
import static com.android.profile.ProfileItemView.STATE_SAVED_PROFILE;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.ContentObserver;
import android.database.Cursor;
import android.media.AudioManager;
import android.net.ConnectivityManager;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.text.format.Time;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

public class ProfileMain extends Activity {

    static String TAG = "ProfileMain";

    private Profile mProfileCur = new Profile();
    List<Profile> mProfileList = new ArrayList<Profile>();
    private int savedProfileNum;
    private final int CODE_REQUEST_SETTING = 1;
    private final int CODE_REQUEST_NEW_PROFILE = 2;

    private ListView mListView = null;

    final int MENU_ADD = Menu.FIRST;

    final int MENU_RESET = Menu.FIRST + 1;
    ProfileDataBaseAdapter mDataBaseAdapter = null;
    private static final int DIALOG_EDIT = 1;
    private static final int DIALOG_RESET = 2;
    private static final int DIALOG_DELETE = 3;

    static final int PROFILE_EDIT = 0;
    static final int PROFILE_DELETE = 1;
    static final int PROFILE_SAVE = 2;
    private static AudioManager mAudioManager = null;
    private static ConnectivityManager mConnectivityManager = null;

    private static WifiManager mWifiManager = null;
    private static BluetoothAdapter mBluetoothAdapter = null;

    private static Context mContext = null;
    private LayoutInflater mInflater = null;
    private static ProfileFunctionConfig mProfileFunctionConfig = null;
    private ProfileApplyManager mProfileApplyManager = null;

    private final int DEFAULT_PROFILE_COUNT = 4;

    private boolean mModifyInThisApp = false;
    private ContentObserver mBrightnessObserver =
            new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange) {
            // For distinguish if the bar changes the brightness
            mModifyInThisApp = false;
            int brightnessCur = -1;
            try {
                brightnessCur = Settings.System.getInt(mContext.getContentResolver(),
                        "screen_brightness");
            } catch (SettingNotFoundException e) {
                e.printStackTrace();
            }

            if (!nullJudge(mDataBaseAdapter, "mDataBaseAdapter")) {
                Cursor mCursor = mDataBaseAdapter.fetchAllProfiles();
                int brightnessIndex = mCursor.getColumnIndex(BRIGHTNESS);
                int i;
                for (mCursor.moveToFirst(), i = 0; !(mCursor.isAfterLast());
                    mCursor.moveToNext(), i++) {
                    if (brightnessCur == mCursor.getInt(brightnessIndex)) {
                        mModifyInThisApp = true;
                        break;
                    }
                }
                mCursor.close();
            }

            if (!mModifyInThisApp) {
                setSelectedId(-1);
                notifyChanged();
            }
        }
    };

    private ContentObserver mSelectIdObserver =
            new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange) {
            notifyChanged();
        }
    };

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {

        AdapterView.AdapterContextMenuInfo mMenuInfo =
                (AdapterView.AdapterContextMenuInfo) menuInfo;
        ProfileItemView mView = (ProfileItemView) mMenuInfo.targetView;
        int position = mMenuInfo.position;
        menu.setHeaderTitle(getString(R.string.dialog_edit_title));
        switch (mView.getState()) {
        case STATE_SAVED_PROFILE:
            menu.add(0, PROFILE_EDIT, 0, getString(R.string.edit));
            if (position >= DEFAULT_PROFILE_COUNT)
                menu.add(0, PROFILE_DELETE, 0, getString(R.string.delete));
            break;
        case STATE_CURRENT_PROFILE:
            menu.add(0, PROFILE_EDIT, 0, getString(R.string.edit));
            // menu.add(0, PROFILE_SAVE, 0, getString(R.string.save));
            break;
        default:
            break;
        }
        super.onCreateContextMenu(menu, v, menuInfo);
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {

        AdapterView.AdapterContextMenuInfo mMenuInfo = (AdapterView.AdapterContextMenuInfo) item
                .getMenuInfo();
        ProfileItemView mView = (ProfileItemView) mMenuInfo.targetView;

        switch (item.getItemId()) {
        case PROFILE_EDIT:
            ProfileSetting.id = mView.getId();
            Intent intent = new Intent(ProfileMain.this, ProfileSetting.class);
            intent.putExtra("action", EDIT_PROFILE);
            startActivityForResult(intent, CODE_REQUEST_SETTING);
            break;
        case PROFILE_DELETE:
            ProfileSetting.id = mView.getId();
            showDialog(DIALOG_DELETE);
            break;
        case PROFILE_SAVE:
            Profile mProfile = getCurrentSettings();
            mProfile.setProfileName(getString(R.string.default_name));
            mDataBaseAdapter.insertProfile(mProfile);
            getAllProfiles();
            notifyChanged();
            break;
        default:
            break;
        }
        return super.onContextItemSelected(item);
    }

    private void init(Context context) {

        mContext = context;
        mProfileFunctionConfig = new ProfileFunctionConfig(context);
        mProfileFunctionConfig.getConfigFromXml(R.xml.profile_function_config);

        mProfileApplyManager = new ProfileApplyManager(context,
            new ProfileApplyManager.Callback() {

            public void updataWindowBrightness(int value) {
                WindowManager.LayoutParams mParam = getWindow().getAttributes();
                mParam.screenBrightness = value / 255.0f; // 0.0 - 1.0
                getWindow().setAttributes(mParam);
                Log.i(TAG, "updataWindowBrightness");
            }
        });
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {

        logd("");
        init(this);
        super.onCreate(savedInstanceState);
        // ====open db====
        if (mDataBaseAdapter == null) {
            mDataBaseAdapter = new ProfileDataBaseAdapter(this);
            mDataBaseAdapter.openDataBase();
        }
        nullJudge(mDataBaseAdapter, "mDataBaseAdapter");

        getService();

        // ====show the default profiles ===
        if (mDataBaseAdapter.fetchAllProfiles().getCount() <= 0) {
            resetProfileDataBase(true);
        }
        setContentView(R.layout.main_layout);
        mListView = (ListView) findViewById(R.id.list);
        mListView.setAdapter(mBaseAdapter);
        registerForContextMenu(mListView);
        mListView.setOnItemClickListener(new OnItemClickListener() {

            public void onItemClick(AdapterView<?> arg0, View view, int position, long id) {
                // position
                // 0-based

                int profileId = mProfileList.get(position).getId();

                applyProfile(profileId);

                ProfileMain.this.finish();
            }
        });

    }// onCreate

    private void getService() {

        nullJudge(mProfileApplyManager, "mProfileApplyManager");
        mAudioManager = mProfileApplyManager.getAudioManager();
        nullJudge(mAudioManager, "mAudioManager");
        mConnectivityManager = mProfileApplyManager.getConnectivityManager();
        nullJudge(mConnectivityManager, "mConnectivityManager");
        mWifiManager = mProfileApplyManager.getWifiManager();
        nullJudge(mWifiManager, "mWifiManager");
        mBluetoothAdapter = mProfileApplyManager.getBluetoothAdapter();
        nullJudge(mBluetoothAdapter, "mBluetoothAdapter");
        mInflater = LayoutInflater.from(mContext);
    }

    public void getAllProfiles() {

        logd("");
        if (nullJudge(mDataBaseAdapter, "mDataBaseAdapter"))
            return;
        Cursor mCursor = mDataBaseAdapter.fetchAllProfiles();
        int idIndex = mCursor.getColumnIndex(ID);
        int nameIndex = mCursor.getColumnIndex(PROFILE_NAME);
        int silentIndex = mCursor.getColumnIndex(SILENT);
        int vibrateIndex = mCursor.getColumnIndex(VIBRATE);
        int volumeIndex = mCursor.getColumnIndex(RING_VOLUME);
        int dataIndex = mCursor.getColumnIndex(DATA);
        int wifiIndex = mCursor.getColumnIndex(WIFI);
        int bluetoothIndex = mCursor.getColumnIndex(BLUETOOTH);
        int gpsLocationIndex = mCursor.getColumnIndex(GPS_LOCATION);
        int networkLocationIndex = mCursor.getColumnIndex(NETWORK_LOCATION);
        int brightnessIndex = mCursor.getColumnIndex(BRIGHTNESS);
        int ringtone1Index = mCursor.getColumnIndex(RINGTONE1);
        int ringtone2Index = mCursor.getColumnIndex(RINGTONE2);

        savedProfileNum = mCursor.getCount();
        logd("savedProfileNum=" + savedProfileNum);
        int i;
        mProfileList.clear();
        for (mCursor.moveToFirst(), i = 0; !(mCursor.isAfterLast()); mCursor.moveToNext(), i++) {
            mProfileList.add(new Profile());
            mProfileList.get(i).setId(mCursor.getInt(idIndex));
            mProfileList.get(i).setProfileName(mCursor.getString(nameIndex));
            mProfileList.get(i).setSilent(mCursor.getInt(silentIndex) > 0 ? true : false);
            mProfileList.get(i).setVibrate(mCursor.getInt(vibrateIndex) > 0 ? true : false);
            mProfileList.get(i).setRingVolume(mCursor.getInt(volumeIndex));
            mProfileList.get(i).setData(mCursor.getInt(dataIndex) > 0 ? true : false);
            mProfileList.get(i).setWifi(mCursor.getInt(wifiIndex) > 0 ? true : false);
            mProfileList.get(i).setBluetooth(mCursor.getInt(bluetoothIndex) > 0 ? true : false);
            mProfileList.get(i).setGpsLocation(mCursor.getInt(gpsLocationIndex) > 0 ? true : false);
            mProfileList.get(i).setNetworkLocation(
                    mCursor.getInt(networkLocationIndex) > 0 ? true : false);
            mProfileList.get(i).setBrightness(mCursor.getInt(brightnessIndex));
            mProfileList.get(i).setRingtone1(mCursor.getString(ringtone1Index));
            mProfileList.get(i).setRingtone2(mCursor.getString(ringtone2Index));
            mProfileList.get(i).setMatchCurProfile(false);// reset to false
        }
        mCursor.close();

        if (!mModifyInThisApp) {
            boolean isMatch = false;
            for (i = 0; i < savedProfileNum; i++) {
                isMatch = matchCurSetting(mProfileList.get(i));
                mProfileList.get(i).setMatchCurProfile(isMatch);
                if (isMatch)
                    break;
            }
            if (!isMatch) {
                setSelectedId(-1);
            }
        }
    }

    /**
     * Apply the current profile
     *
     * @param profileId the profile id in the table of profile database
     */
    private void applyProfile(int profileId) {
        nullJudge(mProfileApplyManager, "mProfileApplyManager");
        mProfileApplyManager.applyProfile(profileId);
        mProfileApplyManager.clearDataBaseAdapter();

        setSelectedId(profileId);
        mModifyInThisApp = true;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        SubMenu addMenu = menu.addSubMenu(0, MENU_ADD, 0, R.string.menu_add);
        addMenu.setIcon(android.R.drawable.ic_menu_add);

        SubMenu resetMenu = menu.addSubMenu(0, MENU_RESET, 0, R.string.menu_reset);
        resetMenu.setIcon(android.R.drawable.ic_menu_revert);
        return true;

    }

    void resetProfileDataBase(boolean newFlashed) {
        nullJudge(mProfileApplyManager, "mProfileApplyManager");
        mProfileApplyManager.resetProfileDataBase(newFlashed);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {
        case MENU_ADD:
            Intent intent = new Intent(ProfileMain.this, ProfileSetting.class);
            intent.putExtra("action", NEW_PROFILE);
            startActivityForResult(intent, CODE_REQUEST_NEW_PROFILE);
            break;
        case MENU_RESET:
            showDialog(DIALOG_RESET);
            break;

        default:
            break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onResume() {

        logd("");
        // IntentFilter filter = new IntentFilter();
        // filter.addAction(AudioManager.VOLUME_CHANGED_ACTION);
        // registerReceiver(mReceiver, filter);

        if (mDataBaseAdapter == null) {
            mDataBaseAdapter = new ProfileDataBaseAdapter(this);
            mDataBaseAdapter.openDataBase();
        }
        mProfileCur = getCurrentSettings();
        getAllProfiles();
        notifyChanged();
        super.onResume();

        mContext.getContentResolver().registerContentObserver(
                Settings.System.getUriFor(Settings.System.SCREEN_BRIGHTNESS),
                true, mBrightnessObserver);
        if (mContext.getResources().getBoolean(R.bool.config_autoprofile_enable)) {
            mContext.getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(ProfileConst.AUDIO_PROLILE_MODE),
                true, mSelectIdObserver);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        logd(resultCode);
        if (requestCode == CODE_REQUEST_SETTING) {
            if (resultCode != RESULT_CANCELED) {
                if (data != null){
                    int profileId = data.getIntExtra(ProfileConst.KEY_PROFILE_ID, -1);
                    setSelectedId(profileId);
                    applyProfile(profileId);
                }
            }
        }

        // Back form new profile
        if (requestCode == CODE_REQUEST_NEW_PROFILE) {
            if (resultCode != RESULT_CANCELED) {
                // Get the profile database id
                if (data != null) {
                    long profileId = data.getLongExtra(ProfileConst.KEY_PROFILE_ID, -1);

                    setSelectedId((int) profileId);
                    // Active the new profile
                    applyProfile((int) profileId);
                }
            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {

        logd("");
        if (mDataBaseAdapter != null) {
            mDataBaseAdapter.closeDataBase();
            mDataBaseAdapter = null;
        }

        mModifyInThisApp = false;
        mContext.getContentResolver().unregisterContentObserver(mBrightnessObserver);
        if (mContext.getResources().getBoolean(R.bool.config_autoprofile_enable)) {
            mContext.getContentResolver().unregisterContentObserver(mSelectIdObserver);
        }
        super.onPause();
    }

    @Override
    protected void onStop() {

        logd("");
        super.onStop();
    }

    public void notifyChanged() {

        logd("========");
        mBaseAdapter.notifyDataSetChanged();
    }

    BaseAdapter mBaseAdapter = new BaseAdapter() {

        public Object getItem(int arg0) {

            return null;
        }

        public long getItemId(int arg0) {

            return 0;
        }

        public View getView(int index, View convertView, ViewGroup parent) {

            // convertView is the last data

            if (convertView == null)
                convertView = mInflater.inflate(R.layout.main_list_row, null);

            ProfileItemView mProfileItemView = (ProfileItemView) convertView;
            Profile mProfile = mProfileList.get(index);
            logd((index + 1) + ":" + ProfileUtils.getShowProfileName(mContext,
                mProfile.getProfileName()));

            // ImageView imageView = (ImageView)
            // convertView.findViewById(R.id.profile_image);
            TextView nameView = (TextView) convertView.findViewById(R.id.profile_name);
            RadioButton checkButton = (RadioButton) convertView.findViewById(R.id.choice);
            // imageView.setImageResource(R.drawable.menu_add);

            nameView.setText("" + ProfileUtils.getShowProfileName(mContext,
                mProfile.getProfileName()));
            mProfileItemView.setId(mProfile.getId());
            mProfileItemView.setState(STATE_SAVED_PROFILE);

            if (matchCurSetting(mProfile) || mProfile.getId() == getSelectedId()) {
                checkButton.setChecked(true);
                logd("Match:" + ProfileUtils.getShowProfileName(mContext,
                    mProfile.getProfileName()));
                if (index == savedProfileNum) {
                    mProfileItemView.setState(STATE_CURRENT_PROFILE);
                }
            } else {
                checkButton.setChecked(false);
            }
            return convertView;
        }

        public int getCount() {

            if (mProfileList != null)
                return mProfileList.size();
            else
                return 0;

        };
    };

    public void displayToast(Object s) {

        Toast.makeText(this, s + "", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected Dialog onCreateDialog(int dialog) {

        switch (dialog) {
        case DIALOG_EDIT:
            return new AlertDialog.Builder(ProfileMain.this).setTitle(R.string.dialog_edit_title)
                    .setItems(R.array.dialog_edit_item, new DialogInterface.OnClickListener() {

                        public void onClick(DialogInterface dialog, int which) {

                            switch (which) {
                            case PROFILE_EDIT:
                                Intent intent = new Intent(ProfileMain.this, ProfileSetting.class);
                                intent.putExtra("action", EDIT_PROFILE);
                                startActivity(intent);
                                break;
                            case PROFILE_DELETE:
                                if (mDataBaseAdapter.deleteProfile(ProfileSetting.id) == false)
                                    loge("Delete Failure");
                                getAllProfiles();
                                notifyChanged();
                                break;
                            default:
                                break;
                            }
                        }
                    }).create();
        case DIALOG_RESET:
            return new AlertDialog.Builder(ProfileMain.this).setTitle(R.string.dialog_reset_title)
                    .setMessage(R.string.dialog_reset_message).setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {

                                public void onClick(DialogInterface dialog, int whichButton) {

                                    resetProfileDataBase(false);
                                    //apply GENERAL profile
                                    applyProfile(mProfileList.get(0).getId());
                                    mProfileCur = getCurrentSettings();
                                    getAllProfiles();
                                    notifyChanged();
                                }
                            }).setNegativeButton(R.string.cancel,
                            new DialogInterface.OnClickListener() {

                                public void onClick(DialogInterface dialog, int whichButton) {

                                }
                            }).create();
        case DIALOG_DELETE:
            return new AlertDialog.Builder(ProfileMain.this).setMessage(R.string.deleteConfirmation)
                .setPositiveButton(R.string.ok,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                if (mDataBaseAdapter.deleteProfile(ProfileSetting.id) == false) {
                                    loge("Delete Failure");
                                }
                                else if (ProfileSetting.id == getSelectedId()){
                                    //delete success, apply GENERAL profile
                                    applyProfile(mProfileList.get(0).getId());
                                }
                                getAllProfiles();
                                notifyChanged();
                            }
                        }).setNegativeButton(R.string.cancel,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                            }
                        }).create();
        }
        return null;
    }

    @SuppressWarnings("unused")
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context arg0, Intent arg1) {

            if (LOG) {
                loge(arg1.getAction());
                // loge(arg1.getExtra(AudioManager.EXTRA_VOLUME_STREAM_TYPE));
            }
        }

    };

    private Profile getCurrentSettings() {
        nullJudge(mProfileApplyManager, "mProfileApplyManager");
        return mProfileApplyManager.getCurrentSettings();
    }

    private boolean matchCurSetting(Profile mProfile) {

        if (getSelectedId() != mProfile.getId())
            return false;
        logd("ID match success, to match profile config>>");
        /** Current Profile */
        boolean silentCur = mProfileCur.isSilent();
        boolean vibrateCur = mProfileCur.isVibrate();
        int ring_volumeCur = mProfileCur.getRingVolume();
        boolean dataCur = mProfileCur.isData();
        boolean wifiCur = mProfileCur.isWifi();
        boolean bluetoothCur = mProfileCur.isBluetooth();
        boolean gpsLocationCur = mProfileCur.isGpsLocation();
        boolean networkLocationCur = mProfileCur.isNetworkLocation();
        int brightnessCur = mProfileCur.getBrightness();
        String ringtone1Cur = mProfileCur.getRingtone1();
        String ringtone2Cur = mProfileCur.getRingtone2();

        // If on BRIGHTNESS_MODE_AUTOMATIC, makes it always mismatch
        if (Settings.System
                .getInt(getContentResolver(),
                    Settings.System.SCREEN_BRIGHTNESS_MODE, -1) ==
                Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC)
            brightnessCur = -1;

        /** Matching Profile */
        boolean silent = mProfile.isSilent();
        boolean vibrate = mProfile.isVibrate();
        int ring_volume = mProfile.getRingVolume();
        boolean data = mProfile.isData();
        boolean wifi = mProfile.isWifi();
        boolean bluetooth = mProfile.isBluetooth();
        boolean gpsLocation = mProfile.isGpsLocation();
        boolean networkLocation = mProfile.isNetworkLocation();
        int brightness = mProfile.getBrightness();
        String ringtone1 = mProfile.getRingtone1();
        String ringtone2 = mProfile.getRingtone2();

        /** Function Enable or not */
        boolean silentCtrlEnabled = ProfileFunctionConfig.isSilentCtrlEnabled();
        boolean vibrateCtrlEnabled = ProfileFunctionConfig.isVibrateCtrlEnabled();
        boolean ringVolumeCtrlEnabled = ProfileFunctionConfig.isRingVolumeCtrlEnabled();
        boolean dataCtrlEnabled = ProfileFunctionConfig.isDataCtrlEnabled();
        boolean wifiCtrlEnabled = ProfileFunctionConfig.isWifiCtrlEnabled();
        boolean bluetoothCtrlEnabled = ProfileFunctionConfig.isBluetoothCtrlEnabled();
        boolean gpsLocationCtrlEnabled = ProfileFunctionConfig.isGpsLocationCtrlEnabled();
        boolean networkLocationCtrlEnabled = ProfileFunctionConfig.isNetworkLocationCtrlEnabled();
        boolean brightnessCtrlEnabled = ProfileFunctionConfig.isBrightnessCtrlEnabled();
        boolean ringtoneCtrlEnabled = ProfileFunctionConfig.isRingtoneCtrlEnabled();

        //add to disable the flag of silentCtrlEnabled when
        //slient is false to match with the flag of silentCur
        if (false == silent) {
            silentCtrlEnabled = false;
        }
        else {
            silentCtrlEnabled = true;
        }

        // boolean value
        // if silentCtrl is disabled, !silentCtrlEnabled will be always true,
        // so it is equal to not check this item
        if ((silentCur == silent || !silentCtrlEnabled)
                && (vibrateCur == vibrate || !vibrateCtrlEnabled)
                && (brightnessCur == brightness || !brightnessCtrlEnabled)
                && (dataCur == data || !dataCtrlEnabled) && (wifiCur == wifi || !wifiCtrlEnabled)
                && (bluetoothCur == bluetooth || !bluetoothCtrlEnabled)
                && (gpsLocationCur == gpsLocation || !gpsLocationCtrlEnabled)
                && (networkLocationCur == networkLocation || !networkLocationCtrlEnabled)) {
            // non-boolean value
            // ringtone
            if (ringtoneCtrlEnabled) {
                if ((ringtone1 == null && ringtone1Cur != null)
                        || (ringtone2 == null && ringtone2Cur != null)
                        || (ringtone1Cur == null && ringtone1 != null)
                        || (ringtone2Cur == null && ringtone2 != null))
                    return false;
                if ((ringtone1 != null && ringtone1Cur != null && (ringtone1
                        .compareTo(ringtone1Cur) != 0))
                        || (ringtone2 != null && ringtone2Cur != null && (ringtone2
                                .compareTo(ringtone2Cur)) != 0))
                    return false;
            }
            // ringVolume
            if (ringVolumeCtrlEnabled) {

                if (ring_volumeCur == ring_volume)
                    return true;
                else if (silentCur == true)
                    return true;
            } else
                return true;

        }
        return false;
    }

    // using when get reference
    private <T> boolean nullJudge(T ref, String pos) {

        if (ref == null) {
            loge(pos + "=null");
            ProfileMain.this.finish();
            return true;
        }else
            return false;
    }

    private static void loge(Object e) {

        if (!LOG)
            return;
        Thread mThread = Thread.currentThread();
        StackTraceElement[] mStackTrace = mThread.getStackTrace();
        String mMethodName = mStackTrace[3].getMethodName();
        e = "[" + mMethodName + "] " + e;
        Log.e(TAG, e + "");
    }

    private static void logd(Object s) {

        if (!LOG)
            return;
        Thread mThread = Thread.currentThread();
        StackTraceElement[] mStackTrace = mThread.getStackTrace();
        String mMethodName = mStackTrace[3].getMethodName();

        s = "[" + mMethodName + "] " + s;
        Log.d(TAG, s + "");
    }

    void setSelectedId(int id) {
        nullJudge(mProfileApplyManager, "mProfileApplyManager");
        mProfileApplyManager.setSelectedId(id);
    }

    int getSelectedId() {
        nullJudge(mProfileApplyManager, "mProfileApplyManager");
        return mProfileApplyManager.getSelectedId();
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN || keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
            mModifyInThisApp = false;
            mProfileCur = getCurrentSettings();
            getAllProfiles();
            notifyChanged();
        }
        return super.onKeyUp(keyCode, event);
    }
}
