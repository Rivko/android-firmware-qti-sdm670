/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.support.v4.widget.SlidingPaneLayout;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.WindowManager;
import android.widget.EditText;

import com.qti.csm.R;
import com.qti.csm.encryption.system.KeyManagement;
import com.qti.csm.encryption.system.ProcessInfo;
import com.qti.csm.encryption.system.SystemInfo;
import com.qti.csm.utils.UtilsSystem;

import java.lang.reflect.Field;

import static com.qti.csm.encryption.system.ProcessInfo.logd;
import static com.qti.csm.encryption.system.SystemInfo.ENC_CHECK;
import static com.qti.csm.encryption.system.SystemInfo.MODE_DECRYPT;
import static com.qti.csm.encryption.system.SystemInfo.MODE_ENCRYPT;

public class FileEncrypt extends FragmentActivity implements
        EncryptProcess.OnOperationClickListener {

    private Context mContext;
    FileExplorerFragment mFileExplorerFragment;
    EncryptProcess mOperationFragment;
    SlidingPaneLayout mSlidingPaneLayout;
    Runnable loadKeyRunnable = new Runnable() {

        @Override
        public void run() {
            if (MainApp.getInstance().mInternalKey == null) {
                MainApp.getInstance().mInternalKey = EncryptKey
                        .loadKeyFromFile(EncryptKey.INTERNAL_INDEX);
                logd("mInternalKey: " + MainApp.getInstance().mInternalKey);
            }
            if (MainApp.getInstance().mSdcardKey == null) {
                MainApp.getInstance().mSdcardKey = EncryptKey
                        .loadKeyFromFile(EncryptKey.SDCARD_INDEX);
            }
        }
    };

    @Override
    protected void onResume() {
        logd("");
        refreshUI();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("");
        registerReceiver(mBroadcastReceiver, intentFilter);
        super.onResume();
    }

    @Override
    protected void onPause() {
        logd("");
        unregisterReceiver(mBroadcastReceiver);
        super.onPause();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (UtilsSystem.isMonkeyRunning()) {
            return;
        }
        init();
    }

    @Override
    protected void onDestroy() {
        logd("");
        MainApp.getInstance().clearPasswordCache();
        super.onDestroy();
    };

    private void refreshUI() {
        logd("");
    }

    void init() {
        mContext = getApplicationContext();
        getActionBar().setDisplayShowHomeEnabled(true);
        FileExplorerFragment.init(MODE_ENCRYPT, ENC_CHECK);

        setContentView(R.layout.file_list);
        mSlidingPaneLayout = (SlidingPaneLayout) findViewById(R.id.encryptdata);
        mSlidingPaneLayout.setPanelSlideListener(mPanelSlideListener);
        mSlidingPaneLayout.openPane();
        mSlidingPaneLayout.setSliderFadeColor(0x77ffffff);

        mFileExplorerFragment = (FileExplorerFragment) getSupportFragmentManager()
                .findFragmentById(R.id.file_explorer_fragment);
        mOperationFragment = (EncryptProcess) getFragmentManager()
                .findFragmentById(R.id.operation_fragment);

        MainApp.getInstance().scheduledExecutorService.execute(loadKeyRunnable);
    }

    BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            logd(action);
        };
    };

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        boolean isRootPath = false;

        if (mFileExplorerFragment != null) {
            isRootPath = mFileExplorerFragment.onKeyDown(keyCode, event);
            if (!isRootPath) {
                return true;
            }
        }
        if (isRootPath && !mSlidingPaneLayout.isOpen()) {
            mSlidingPaneLayout.smoothSlideOpen();
            logd("Go back to OperationFragment");
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void onOperationClick(int mode) {
        logd(mode);
        switch (mode) {
            case MODE_ENCRYPT:
                setTitle(R.string.encrypt);
                FileExplorerFragment.init(MODE_ENCRYPT, ENC_CHECK);
                mSlidingPaneLayout.smoothSlideClosed();
                break;
            case MODE_DECRYPT:
                if (MainApp.getInstance().mInternalKey == null) {
                    ProcessInfo.toast(this, getString(R.string.need_password_toast));

                    Intent intent = new Intent(this, KeyManagement.class);
                    intent.putExtra("mode", SystemInfo.INTENT_CREATE_PASSWORD);
                    startActivity(intent);
                    return;
                }
                if (MainApp.getInstance().mInternalPassword == null) {
                    showDialog(DIALOG_LOGIN);
                    return;
                }
                setTitle(R.string.decrypt);
                FileExplorerFragment.init(MODE_DECRYPT, ENC_CHECK);
                mSlidingPaneLayout.smoothSlideClosed();
                break;

            default:
                break;
        }

        mFileExplorerFragment.refreshExplorer();
    }

    SlidingPaneLayout.PanelSlideListener mPanelSlideListener = new SlidingPaneLayout.PanelSlideListener() {

        @Override
        public void onPanelSlide(View arg0, float arg1) {
        }

        @Override
        public void onPanelOpened(View arg0) {
            setTitle(R.string.encryptdata);
            logd("");
        }

        @Override
        public void onPanelClosed(View arg0) {
            logd("");
            switch (FileExplorerFragment.getFileExplorerMode()) {

                case MODE_ENCRYPT:
                    setTitle(R.string.encrypt);
                    break;

                case MODE_DECRYPT:
                    setTitle(R.string.decrypt);
                    break;

                default:
                    break;
            }

        }
    };

    private static final int DIALOG_LOGIN = 0;
    private static boolean changePwd = false;

    @Override
    protected Dialog onCreateDialog(int id, Bundle args) {
        logd(id);
        switch (id) {
            case DIALOG_LOGIN:
                final View loginDialogView = LayoutInflater.from(
                        FileEncrypt.this).inflate(R.layout.user_check,
                        null);

                AlertDialog loginDialog = new AlertDialog.Builder(this)
                        .setTitle(R.string.login)
                        .setView(loginDialogView)
                        .setOnCancelListener(new DialogInterface.OnCancelListener() {

                            @Override
                            public void onCancel(DialogInterface dialog) {
                                finish();
                            }
                        })
                        .setPositiveButton(R.string.ok,
                                new DialogInterface.OnClickListener() {

                                    public void onClick(DialogInterface dialog,
                                                        int whichButton) {
                                        boolean dismissDialog = true;

                                        String password = ((EditText) loginDialogView
                                                .findViewById(R.id.login_password))
                                                .getText().toString();

                                        if (!password.equals(EncryptKey
                                                .loadPassword(EncryptKey.INTERNAL_INDEX))) {
                                            ProcessInfo.toast(
                                                    mContext,
                                                    getString(R.string.password_wrong));
                                            dismissDialog = false;
                                        } else {
                                            dismissDialog = true;
                                            if (changePwd == true) {
                                                Intent intent = new Intent(FileEncrypt.this,
                                                        KeyManagement.class);
                                                intent.putExtra("mode", SystemInfo.INTENT_CHANGE_PASSWORD);
                                                changePwd = false;
                                                startActivity(intent);
                                            } else {
                                                setTitle(R.string.decrypt);
                                                FileExplorerFragment.init(MODE_DECRYPT,
                                                        ENC_CHECK);
                                                mSlidingPaneLayout.smoothSlideClosed();
                                            }
                                        }

                                        try {
                                            Field field = dialog.getClass()
                                                    .getSuperclass()
                                                    .getDeclaredField("mShowing");
                                            field.setAccessible(true);
                                            field.set(dialog, dismissDialog);
                                        } catch (Exception e) {
                                            ProcessInfo.loge(e);
                                        }
                                    }
                                })
                        .setNegativeButton(R.string.cancel,
                                new DialogInterface.OnClickListener() {

                                    public void onClick(DialogInterface dialog,
                                                        int whichButton) {
                                        MainApp.getInstance().mInternalPassword = null;
                                        MainApp.getInstance().mSdcardPassword = null;
                                        try {
                                            Field field = dialog.getClass()
                                                    .getSuperclass()
                                                    .getDeclaredField("mShowing");
                                            field.setAccessible(true);
                                            field.set(dialog, true);
                                        } catch (Exception e) {
                                            ProcessInfo.loge(e);
                                        }
                                    }
                                }).create();
                loginDialog
                        .getWindow()
                        .setSoftInputMode(
                                WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE
                                        | WindowManager.LayoutParams.SOFT_INPUT_STATE_HIDDEN);
                return loginDialog;
            default:
                break;
        }
        return null;
    }

    private static final int MENU_CHANGE_PASSWORD = Menu.FIRST;

    public boolean onCreateOptionsMenu(Menu menu) {
        logd("");
        SubMenu addMenu = menu.addSubMenu(0, MENU_CHANGE_PASSWORD, 0,
                R.string.change_password);
        addMenu.setIcon(android.R.drawable.ic_menu_add);
        if (MainApp.getInstance().mInternalKey == null) {
            menu.setGroupEnabled(0, false);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case MENU_CHANGE_PASSWORD:
                changePwd = true;
                showDialog(DIALOG_LOGIN);
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        if (MainApp.getInstance().mInternalKey != null) {
            menu.setGroupEnabled(0, true);
        }
        return true;
    }
}
