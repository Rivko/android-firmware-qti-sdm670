/**
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 **/

package com.qualcomm.qti.modemtestmode;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.android.internal.telephony.TelephonyProperties;

import com.qualcomm.qti.modemtestmode.IMbnTestService;

public class MbnFileLoad extends Activity {
    private final String TAG = "MbnFileLoad";
    private final String LOCATION = "location";
    private final String LOCATION_HINT = "location_hint";

    private final int MENU_LOCATION = 0;
    private final int EVENT_QCRIL_HOOK_READY = 1;
    private final int EVENT_LOAD_MBN_FILE = 2;

    private final int PHONE_STORAGE = 0;
    private final int SD_CARD = 1;

    private MbnFileManager mFileMgr;
    private Context mContext;
    private TextView mLocationHint;
    private TextView mConfigHint;
    private Button mBackButton;
    private Button mLoadButton;
    private ListView mFileListView;
    private ArrayAdapter<String> mArrayAdapter;
    private ArrayList<String> mDirContent;
    private int mLocationChoice = 0;
    private int mMbnFileIndex = 0;
    private int mLoadedMbnNumber = 0;
    private int mLoadedMbnSuccess = 0;
    private boolean mIsLastDir = false;
    private int mMultiModeNumber = 1;
    private ProgressDialog mProgressDialog;
    private IMbnTestService mMbnTestService = null;

    private Handler mHandler = new Handler() {

        @Override
        public void dispatchMessage(Message msg) {
            switch(msg.what) {
            case EVENT_QCRIL_HOOK_READY:
                log("EVENT_QCRIL_HOOK_READY");
                MbnAppGlobals.getInstance().unregisterQcRilHookReady(mHandler);
                setActivityView();
                break;

            case EVENT_LOAD_MBN_FILE:
                log("EVENT_LOAD_MBN_FILE");
                loadMbnFile(msg.arg1);
                break;

            default:
                log("Unexpected event:" + msg.what);
                break;
            }
        }

    };

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            log("Received: " + action);

            // TODO why need two broadcasts?
            if (MbnTestUtils.PDC_CONFIG_CLEARED_ACTION.equals(action)) {
                byte[] result = intent.getByteArrayExtra(MbnTestUtils.PDC_ACTIVATED);
                int sub = intent.getIntExtra(MbnTestUtils.SUB_ID, MbnTestUtils.DEFAULT_SUB);
                ByteBuffer payload = ByteBuffer.wrap(result);
                payload.order(ByteOrder.nativeOrder());
                int error = payload.get();
                log("Sub:" + sub + " activated:" + new String(result) + " error code:" + error);
                switch (error) {
                case MbnTestUtils.CLEANUP_SUCCESS:
                case MbnTestUtils.CLEANUP_ALREADY:
                    mMbnFileIndex = 0;
                    mLoadedMbnNumber = 0;
                    mLoadedMbnSuccess = 0;

                    if (mDirContent == null || mDirContent.size() == 0) {
                        log("No Mbn Files");
                        MbnTestUtils.showToast(MbnFileLoad.this, "No Mbn Files");
                        return;
                    }
                    Message msg = mHandler.obtainMessage(EVENT_LOAD_MBN_FILE);
                    msg.sendToTarget();
                    break;
                case MbnTestUtils.CLEANUP_FAILED:
                    MbnTestUtils.showToast(MbnFileLoad.this, "Fail to cleanup configs");
                    break;
                default:
                    break;
                }
            } else if (MbnTestUtils.PDC_DATA_ACTION.equals(action)) {
                byte[] result = intent.getByteArrayExtra(MbnTestUtils.PDC_ACTIVATED);
                int sub = intent.getIntExtra(MbnTestUtils.SUB_ID, MbnTestUtils.DEFAULT_SUB);
                ByteBuffer payload = ByteBuffer.wrap(result);
                payload.order(ByteOrder.nativeOrder());
                int error = payload.get();
                log("Sub:" + sub + " activated:" + new String(result) + " error code:" + error);
                if (error == MbnTestUtils.LOAD_MBN_SUCCESS) {
                    mLoadedMbnSuccess++;
                }
                Message msg = mHandler.obtainMessage(EVENT_LOAD_MBN_FILE);
                msg.arg1 = mLoadedMbnSuccess;
                msg.sendToTarget();
            }
        }

    };

    private ServiceConnection mMbnServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mMbnTestService = (IMbnTestService) IMbnTestService.Stub.asInterface(service);
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mMbnTestService = null;
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mbn_file_load);
        mContext = this;
        int titleResId = MbnTestUtils.isCurrentSwMbnMode() ?
                R.string.mbn_file_load_sw : R.string.mbn_file_load_hw;
        setTitle(getResources().getString(titleResId));
        MbnAppGlobals.getInstance().registerQcRilHookReady(mHandler,
                EVENT_QCRIL_HOOK_READY, null);
        bindService(new Intent(this, MbnTestService.class),
                mMbnServiceConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter();
        filter.addAction(MbnTestUtils.PDC_DATA_ACTION);
        filter.addAction(MbnTestUtils.PDC_CONFIG_CLEARED_ACTION);
        registerReceiver(mBroadcastReceiver, filter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mBroadcastReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mMbnServiceConnection);
    }

    private void setActivityView() {
        SharedPreferences sp = getSharedPreferences(getPackageName(), Context.MODE_PRIVATE);
        String location = sp.getString(LOCATION, MbnTestUtils.getGeneralMbnPath());
        String[] normalConfig = getResources().getStringArray(R.array.normal_config);
        String[] abnormalConfig = getResources().getStringArray(R.array.abnormal_config);
        String[] hw_ss_Config = getResources().getStringArray(R.array.hw_ss_config);
        String[] hw_7_1_Config = getResources().getStringArray(R.array.hw_7_1_config);
        String[] hw_7_5_Config = getResources().getStringArray(R.array.hw_7_5_config);

        log("Mbn path is: " + location);

        mFileMgr = new MbnFileManager(normalConfig, abnormalConfig, hw_ss_Config, hw_7_1_Config,
                hw_7_5_Config);
        if (!TextUtils.isEmpty(location)) {
            mDirContent = mFileMgr.setRootDir(location);
        } else {
            mDirContent = mFileMgr.setRootDir(MbnTestUtils.getGeneralMbnPath());
        }

        String locationHint = sp.getString(LOCATION_HINT, getString(R.string.phone_storage));

        mLocationHint = (TextView) findViewById(R.id.location_hint);
        mLocationHint.setText(locationHint);

        mConfigHint = (TextView) findViewById(R.id.config_hint);
        mConfigHint.setText(normalConfig[0]);

        setMbnFilesListView();

        mBackButton = (Button) findViewById(R.id.back_button);
        mBackButton.setOnClickListener(new BackButtonClicker());
        showBackButton();

        mLoadButton = (Button) findViewById(R.id.load_button);
        mLoadButton.setOnClickListener(new LoadButtonClicker());
        showLoadButton();
    }

    class BackButtonClicker implements Button.OnClickListener {

        @Override
        public void onClick(View v) {
            mDirContent = mFileMgr.handleKeyDown(KeyEvent.KEYCODE_BACK);
            if (mDirContent == null) {
                finish();
            }
            if (mIsLastDir) {
                mIsLastDir = false;
                mFileListView.setAdapter(null);
                mArrayAdapter = new ArrayAdapter<String>(MbnFileLoad.this,
                        android.R.layout.simple_list_item_single_choice, mDirContent);
                mFileListView.setAdapter(mArrayAdapter);
                mFileListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
            }
            mArrayAdapter.notifyDataSetChanged();
            int choice = mFileMgr.getPreviousChoice();
            mFileListView.setItemChecked(choice, true);
            mFileMgr.setCurrentChoice(choice);
            showBackButton();
            showLoadButton();
        }

    }

    class LoadButtonClicker implements Button.OnClickListener {

        @Override
        public void onClick(View v) {
            if (mFileMgr.isLastDir()) {
                if (MbnTestUtils.isCurrentSwMbnMode()) {
                    boolean ret = MbnAppGlobals.getInstance().cleanUpConfigs();
                    if (ret == false) {
                        log("Fail to cleanUpConfigs");
                        MbnTestUtils.showToast(MbnFileLoad.this, "Failed to clean up configs");
                    }
                } else {
                    handleHwMbnFileSelection();
                }
            } else {
                mDirContent= mFileMgr.getNextDir();
                if (mFileMgr.isLastDir()) {
                    // TODO, need use single choice
                    mIsLastDir = true;
                    mFileListView.setAdapter(null);
                    mArrayAdapter = new ArrayAdapter<String>(MbnFileLoad.this,
                            android.R.layout.simple_list_item_1, mDirContent);
                    mFileListView.setAdapter(mArrayAdapter);
                    mFileListView.setChoiceMode(ListView.CHOICE_MODE_NONE);
                } else {
                    if (mDirContent != null) {
                        mFileListView.setItemChecked(0, true);
                        mFileMgr.setCurrentChoice(0);
                    }
                }
                mArrayAdapter.notifyDataSetChanged();
                showBackButton();
                showLoadButton();
            }
        }

    }

    public void handleHwMbnFileSelection() {
        boolean ret = MbnAppGlobals.getInstance().deactivateAllConfigs();
        if (ret == false) {
            log("Fail to deactivateAllConfigs");
            MbnTestUtils.showToast(MbnFileLoad.this, "Failed to deAct All Configs");
        }
        mMbnFileIndex = 0;
        mLoadedMbnNumber = 0;
        mLoadedMbnSuccess = 0;

        if (mDirContent == null || mDirContent.size() == 0) {
            log("No Mbn Files");
            MbnTestUtils.showToast(MbnFileLoad.this, "No Mbn Files");
            return;
        }
        Message msg = mHandler.obtainMessage(EVENT_LOAD_MBN_FILE);
        msg.sendToTarget();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case MENU_LOCATION:
            rootPathSelectDialog();
            return true;
        default:
            break;
        }
        return false;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add(0, MENU_LOCATION, 0, R.string.location_configuration);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        mDirContent = mFileMgr.handleKeyDown(keyCode);
        if (mDirContent == null) {
            finish();
        }
        mArrayAdapter.notifyDataSetChanged();
        int position = mFileMgr.getPreviousChoice();
        mFileListView.setItemChecked(position, true);
        mFileMgr.setCurrentChoice(position);
        showBackButton();
        return true;
    }

    private void setMbnFilesListView() {
        mFileListView = (ListView) findViewById(R.id.mbn_file_list);
        mArrayAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_single_choice, mDirContent);
        mFileListView.setAdapter(mArrayAdapter);
        mFileListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        if (mDirContent != null && mDirContent.size() != 0) {
            mFileListView.setItemChecked(0, true);
            mFileMgr.setCurrentChoice(0);
        }
        mFileListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                mFileMgr.setCurrentChoice(position);
            }
        });
    }

    private void showBackButton() {
        if (mFileMgr.isRootDir()) {
            mBackButton.setVisibility(View.INVISIBLE);
        } else {
            mBackButton.setVisibility(View.VISIBLE);
        }
        mConfigHint.setText(mFileMgr.getCurrentConfig());
    }

    private void showLoadButton() {
        if (mDirContent != null && mDirContent.size() != 0) {
            mLoadButton.setVisibility(View.VISIBLE);
        } else {
            mLoadButton.setVisibility(View.INVISIBLE);
        }
    }

    private void rootPathSelectDialog() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.location_configuration)
        .setSingleChoiceItems(R.array.locations, mLocationChoice,
                new OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                mLocationChoice = which;
            }
        })
        .setPositiveButton(R.string.ok, new OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                switch (mLocationChoice) {
                case PHONE_STORAGE:
                    resetRootPath(MbnTestUtils.getGeneralMbnPath(),
                            MbnFileLoad.this.getString(R.string.phone_storage));
                    break;
                case SD_CARD:
                    String path = MbnFileManager.getSDPath(MbnFileLoad.this);
                    if (path == null) {
                        //TODO need show prompt
                        MbnTestUtils.showToast(MbnFileLoad.this, "Failed to get SD CARD path");
                        log("Fail to get SD path");
                    } else {
                        resetRootPath(path, MbnFileLoad.this.getString(R.string.sdcard));
                    }
                    break;
                default:
                    break;
                }
            }
        })
        .setNegativeButton(R.string.cancel, new OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        builder.show();

    }

    private void resetRootPath(String location, String hint) {
        if (!TextUtils.isEmpty(location)) {
            SharedPreferences sp = getSharedPreferences(getPackageName(),
                    Context.MODE_PRIVATE);
            sp.edit().putString(LOCATION, location)
            .putString(LOCATION_HINT, hint)
            .commit();
            mLocationHint.setText(hint);
            mDirContent = mFileMgr.setRootDir(location);
            showBackButton();
            showLoadButton();
            mArrayAdapter.notifyDataSetChanged();
        }
    }

    private void loadMbnFile(int success) {
        log("Total size:" + mDirContent.size() + " mMbnFileIndex:" +
                mMbnFileIndex + " success:" + success);
        if (mMbnFileIndex >= mDirContent.size()) {
            //HW MBN check
            if (MbnTestUtils.isCurrentSwMbnMode()) {
                showQuitDialog(mContext.getString(R.string.loaded) + mLoadedMbnNumber + "\n" +
                        mContext.getString(R.string.success) + success);
            } else {
                if (mLoadedMbnNumber == mLoadedMbnSuccess) {
                    //SUCCESS case.
                    showHwMbnActivationDialog();
                } else {
                    MbnTestUtils.showToast(mContext,
                            mContext.getString(R.string.fail_to_activate_mbn));
                }
            }
            return;
        }

        for (; mMbnFileIndex < mDirContent.size(); ) {
            String fileName = mFileMgr.getCurAbsolutelyPath()+mDirContent.get(mMbnFileIndex);
            if (fileName.endsWith(MbnTestUtils.MBN_FILE_SUFFIX)) {
                String destName;
                String string = mDirContent.get(mMbnFileIndex);
                String lastName = string.substring(string.lastIndexOf('/') + 1);
                if (MbnTestUtils.mbnNeedToGo()) {
                    destName = MbnTestUtils.MBN_TO_GO_DIR+lastName;
                    File dest = new File(destName);
                    try {
                        dest.delete();
                        dest.createNewFile();
                        Runtime.getRuntime().exec("chmod 644 " + destName);
                        Runtime.getRuntime().exec("chown radio.radio " + destName);
                    } catch (IOException e) {
                        log("can't create file:" + destName + " error:" +  e);
                        mMbnFileIndex++;
                        continue;
                    }
                    log("copy " + fileName + " to " + destName);
                    if (!MbnFileManager.copyWithChannels(new File(fileName), dest, true)) {
                        MbnTestUtils.showToast(mContext, mContext.getString(
                                R.string.fail_to_copy_file));
                        mMbnFileIndex++;
                        continue;
                    }
                    try {
                        Thread.sleep(500);
                    }  catch (Exception e) {
                        log("Got exception while sleep after copying file");
                    }
                } else {
                    destName = fileName;
                }

                if (MbnTestUtils.isCurrentSwMbnMode()) {
                    MbnAppGlobals.getInstance().setupMbnConfig(destName,
                            MbnTestUtils.APP_MBN_ID_PREFIX+fileName, 0);
                } else {
                    handleSetupHwMbnConfig(destName, MbnTestUtils.APP_MBN_ID_PREFIX+fileName);
                }
                mLoadedMbnNumber++;
                mMbnFileIndex++;
                break;
            } else {
                mMbnFileIndex++;
            }
        }
    }

    private void handleSetupHwMbnConfig(String destName, String config) {
        int sub_mask = 1;
        if (config.toLowerCase().contains("ss") || config.toLowerCase().contains("singlesim")) {
            mMultiModeNumber = 1;
        } else if (config.toLowerCase().contains("dsds") ||
                config.toLowerCase().contains("dsda")) {
            mMultiModeNumber = 2;
        }

        if (mMultiModeNumber== 2) sub_mask = 1 + 2;

        MbnAppGlobals.getInstance().setupMbnConfig(destName, config, sub_mask);
    }

    private void showProgressDialog() {
        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage(getResources().getString(R.string.Activating_hint));
        mProgressDialog.setCancelable(false);
        mProgressDialog.show();
    }

    private class PostSettingThread implements Runnable {
        public PostSettingThread() {
            log("PostSettingThread: Start");
        }

        public void run() {
            int sub_mask = 1;
            String mode = mMultiModeNumber == 1 ? "ssss" : "dsds";
            if (mMultiModeNumber== 2) sub_mask = 1 + 2;

            try {
                mMbnTestService.setProperty(TelephonyProperties.PROPERTY_MULTI_SIM_CONFIG, mode);
                Thread.sleep(15000);
            } catch (Exception e) {
                log("PostSettingThread: Got exception");
            }
            log("PostSettingThread: Start reboot");
            mProgressDialog.dismiss();
            MbnTestUtils.rebootSystem(mContext);
        }
    }

    private void showHwMbnActivationDialog(){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.activate_hint)
        .setMessage(R.string.activate_description)
        .setPositiveButton(R.string.activate,
                new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                showProgressDialog();
                new Thread(new PostSettingThread()).start();
            }
        })
        .setNegativeButton(R.string.cancel,
                new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                MbnFileLoad.this.finish();
            }
        });
        builder.show();
    }

    private void showQuitDialog(String msg){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(R.string.exit_mbnfileload)
        .setMessage(msg)
        .setPositiveButton(R.string.quit,
                new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                MbnFileLoad.this.finish();
            }
        })
        .setNegativeButton(R.string.cancel,
                new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        builder.show();
    }

    private void log(String msg) {
        Log.d(TAG, "MbnTest_" + msg);
    }

}
