/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.Manifest;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Parcelable;
import android.preference.PreferenceManager;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.view.ContextThemeWrapper;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.qti.threedr.CannedDataTest.CannedDataIO;
import com.qualcomm.qti.threedr.Preferences;
import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.VOConfig;
import com.qualcomm.qti.threedr.VO_Configuration;
import com.qualcomm.qti.threedr.newscan.NewScanActivity;
import com.qualcomm.qti.threedr.utils.AboutActivity;
import com.qualcomm.qti.threedr.utils.Constants;
import com.qualcomm.qti.threedr.utils.ContextMenuItemClickListener;

import java.io.File;

public class GalleryViewActivity extends AppCompatActivity implements ContextMenuItemClickListener {

    private static final int MODELVIEWER_REQUESTCODE = 100;
    public static final int OPERATION_MODEL_ADDED = 0;
    public static final int OPERATION_MODEL_RENAMED = 1;
    public static final int OPERATION_MODEL_DELETED = 2;
    public static final String OPERATIONPERFORMED = "OPERATION_PERFORMED";
    MainFragment mainFragment;
    private static final String TAG = GalleryViewActivity.class.getSimpleName();
    MenuItem deleteMenu;
    MenuItem shareMenu;
    Toolbar toolbar;
    //boolean depth_scanning_available = false;
    FloatingActionButton fab;
    Parcelable recyclerviewScrollPosition;

    private static final float BATTERY_MIN_LEVEL = 70f; //minimum battery percentage level 0-100 to start a scan

    public static boolean isDeveloperMode = false;


    private void NotGranted() {
        Toast.makeText(this, "Enable Camera and Storage permissions to use this app.", Toast.LENGTH_LONG).show();
        finish();
    }

    private void checkForPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            int[] permissions = new int[2];
            permissions[0] = checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE);
            permissions[1] = checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);

            if (false == BrowserUtils.AllGranted(permissions)) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0x13);
            }
        }
    }

    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case 0x13:
                if (false == BrowserUtils.AllGranted(grantResults)) {
                    NotGranted();
                } else {
                    if (mainFragment != null && mainFragment.isVisible()) {
                        createNecessaryDirectories();
                        mainFragment.adapter.reload();
                    }
                }
                break;
        }
    }

    /**
     * Creates the necessary folders if they done exist
     */
    private void createNecessaryDirectories() {
        File calibFolder = new File(Constants.CALIBRATION_FOLDER);
        if (!calibFolder.exists()) {
            calibFolder.mkdirs();
        }

        File dumpFolder = new File(Constants.DUMP_FOLDER);
        if (!dumpFolder.exists()) {
            dumpFolder.mkdirs();
        }

        File modelFolder = new File(Constants.MODEL_FOLDER);
        if (!modelFolder.exists()) {
            modelFolder.mkdirs();
        }

        File cannedDataFolder = new File(Constants.CANNED_DATA_FOLDER);
        if (!cannedDataFolder.exists()) {
            cannedDataFolder.mkdirs();
        }
    }

    private float getBatteryPct() {
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent batteryStatus = this.registerReceiver(null, ifilter);
        int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

        return level * 100f / (float)scale;

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gallery_view);
        toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);



        try {
            ActivityInfo activityInfo = getPackageManager().getActivityInfo(
                    this.getComponentName(), PackageManager.GET_META_DATA);
            Bundle aBundle = activityInfo.metaData;
            if (aBundle != null) {
                isDeveloperMode = aBundle.getBoolean("com.qualcomm.qti.modelbrowser.GalleryViewActivity.isDeveloperMode", false);
                if (isDeveloperMode) {
                    SettingsActivity.setPrefDontShowTutuorialCheckbox(this, true);
                }
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        if (savedInstanceState == null) {
            mainFragment = new MainFragment();
            getSupportFragmentManager().beginTransaction()
                    .add(R.id.fragmentContainer, mainFragment).commit();
        } else {
            mainFragment = (MainFragment) (getSupportFragmentManager().getFragments().get(0));
        }

        PreferenceManager.setDefaultValues(this, R.xml.prefs, false);

        checkForPermissions();
        createNecessaryDirectories();



        fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                float batteryPct = getBatteryPct();

                VOConfig voConfig = Preferences.getVOConfig(getApplicationContext());
                boolean[] vo_config = voConfig.ToArray();
                int battery = VO_Configuration.ConfigName.BATTERY_OVERRIDE;
                boolean batteryOverride = vo_config[battery];
                if (batteryPct < BATTERY_MIN_LEVEL && batteryOverride == false) {
                    final AlertDialog.Builder builder = new AlertDialog.Builder(GalleryViewActivity.this);
                    builder.setMessage(getString(R.string.battery_string_1) + " "
                            + (int)batteryPct
                            + getString(R.string.battery_string_2) + " "
                            + (int)BATTERY_MIN_LEVEL + "%.");
                    builder.setNeutralButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which){

                        }
                    });

                    final AlertDialog dialog = builder.create();
                    dialog.show();

                }
                else {
                    Intent intent = new Intent(getApplicationContext(), NewScanActivity.class);
                    startActivity(intent);
                }
            }
        });


    }

    public void setSelectionMode(boolean isSelectionMode) {
        if (!isSelectionMode) {
            setTitle(R.string.app_name);
            getSupportActionBar().setDisplayHomeAsUpEnabled(false);
            fab.setVisibility(View.VISIBLE);
            /*
            if (depth_scanning_available)
                fab.setVisibility(View.VISIBLE);
            else
                fab.setVisibility(View.GONE);\
            */
        } else {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setHomeAsUpIndicator(R.drawable.ic_close_24dp);
            setTitle(mainFragment.adapter.nSelected + " Selected");
            fab.setVisibility(View.GONE);

        }
    }

    void startViewer(Intent intent) {
        recyclerviewScrollPosition = mainFragment.getRecycleViewState();
        startActivityForResult(intent, MODELVIEWER_REQUESTCODE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == MODELVIEWER_REQUESTCODE) {
            Log.i(TAG, "result code is " + resultCode);
            if (resultCode == RESULT_OK) {
                int operationPreformed = data.getIntExtra(GalleryViewActivity.OPERATIONPERFORMED, -1);
                int pos = data.getIntExtra(BrowserUtils.EXTRA_MODELINFO_POS, -1);
                switch (operationPreformed) {
                    case OPERATION_MODEL_ADDED: //Added
                        //We wont come here for reload after new add because onActivityResult wont
                        // get called as new scan takes a different path.
                        Log.i(TAG, "New model added");
                        mainFragment.reload();
                        break;
                    case OPERATION_MODEL_RENAMED:
                        Log.i(TAG, "model renamed at pos " + pos);
                        mainFragment.adapter.updateItemForRename(pos);
                        break;
                    case OPERATION_MODEL_DELETED: //deleted
                        Log.i(TAG, "model deleted at pos " + pos);
                        mainFragment.adapter.remove(pos);
                        break;
                    default:
                        Log.i(TAG, "case default");
                }
            }

        }
        mainFragment.setRecycleViewState(recyclerviewScrollPosition);
    }

    public void setNSelected(int n) {
        setTitle(n + " Selected");
    }

    @Override
    public void onBackPressed() {
        Log.i(TAG, "onBack pressed");
        if (mainFragment != null && mainFragment.getSelectionMode()) {
            mainFragment.setMode(-1);
            invalidateOptionsMenu();
            Parcelable recycleviewState = mainFragment.getRecycleViewState();
            mainFragment.adapter.notifyDataSetChanged();
            mainFragment.setRecycleViewState(recycleviewState);

        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        if (mainFragment != null && mainFragment.getSelectionMode()) {
            getMenuInflater().inflate(R.menu.menu_selection_mode, menu);
            shareMenu = menu.findItem(R.id.action_share);
            deleteMenu = menu.findItem(R.id.action_delete);
            setSelectionMode(true);
            enableMenuItems(mainFragment.adapter.nSelected > 0);
        } else {
            getMenuInflater().inflate(R.menu.menu_gallery_view, menu);
            setSelectionMode(false);
        }
        return true;
    }

    public void enableMenuItems(boolean enable) {
        enableShareMenuItem(enable);
        enableDeleteMenuItem(enable);
    }

    public void enableDeleteMenuItem(boolean enable) {
        if (mainFragment != null && mainFragment.getSelectionMode() && deleteMenu != null) {
            deleteMenu.setEnabled(enable);
        }
    }

    public void enableShareMenuItem(boolean enable) {
        if (mainFragment != null && mainFragment.getSelectionMode() && shareMenu != null) {
            shareMenu.setEnabled(enable);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        boolean bHandled = true;

        switch (id) {
            case android.R.id.home:
                onBackPressed();
                break;
            case R.id.action_sortByDate:
                if (mainFragment != null) {
                    mainFragment.setMode(1);
                    mainFragment.reload();
                }
                break;
            case R.id.action_sortByName:
                if (mainFragment != null) {
                    mainFragment.setMode(0);
                    mainFragment.reload();
                }
                break;
            case R.id.action_select:
                if (mainFragment != null) {
                    mainFragment.setMode(2);
                    invalidateOptionsMenu();
                    //To save the recyclerview scroll state and then will restore this saved state after notifydatasetchange.
                    Parcelable recyclerViewScrollState = mainFragment.getRecycleViewState();
                    mainFragment.adapter.notifyDataSetChanged();
                    mainFragment.setRecycleViewState(recyclerViewScrollState);

                }
                break;
            case R.id.action_share:
                Log.i(TAG, "action share clicked");
                break;
            case R.id.action_video_share:
                Log.i(TAG, "action video share clicked");
                mainFragment.actionVideoShare();
                break;
            case R.id.action_raw_share:
                Log.i(TAG, "action raw share clicked");
                mainFragment.actionRawShare();
                break;
            case R.id.action_delete:
                Log.i(TAG, "action delete clicked");
                mainFragment.actionDelete();
                break;
            case R.id.action_settings:
                Intent intent = new Intent();
                intent.setClass(this, SettingsActivity.class);
                startActivity(intent);
                break;
            case R.id.action_about:
                Intent aboutIntent = new Intent(this, AboutActivity.class);
                startActivity(aboutIntent);
                break;

            default:
                bHandled = super.onOptionsItemSelected(item);
                break;
        }

        return bHandled;
    }

    @Override
    public void onContextMenuItemClicked(int i) {
        if (i == R.id.action_delete) {
            if (mainFragment != null) {
                mainFragment.reload();
            }
        }
    }

}
