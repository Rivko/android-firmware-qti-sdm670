/**
 * Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.Window;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.os.PowerManager;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import android.util.Log;

import java.lang.reflect.Field;
import java.util.ArrayList;

public class ConfigurationActivity extends Activity 
    implements DialogInterface.OnClickListener{

    private static final String KEY_NO_TITILE = "no_title";
    private static final String KEY_VISIBLE_CONTAINER_ID = "visible_container_id";

    private boolean mNoTitle = false;
    private int mVisibleContainerId = -1;

    private MenuItem mUItem = null;
    private Intent mIntent = null;
    private RadioPreferenceFragment mFragment = null;
    public  static ArrayList<Activity> activities = new ArrayList<Activity>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        initFragment();
        if (savedInstanceState != null) {
            mNoTitle = savedInstanceState.getBoolean(KEY_NO_TITILE, false);
            mVisibleContainerId = savedInstanceState.getInt(KEY_VISIBLE_CONTAINER_ID, -1);
        }

        if (mVisibleContainerId < 0) {
            finish();
            return;
        }

        if (mNoTitle) {
            requestWindowFeature(Window.FEATURE_NO_TITLE);
        }

        try {
            ViewConfiguration mconfig = ViewConfiguration.get(this);
            Field menuKeyField = ViewConfiguration.class.getDeclaredField("sHasPermanentMenuKey");
            if(menuKeyField != null) {
                menuKeyField.setAccessible(true);
                menuKeyField.setBoolean(mconfig, false);
            }
        } catch (Exception ex) {
        }
        setContentView(R.layout.configuration_activity);
        addActivity((Activity)this);
        findViewById(mVisibleContainerId).setVisibility(View.VISIBLE);
    }

    private void addActivity(Activity activity) {
        activities.add(activity);
    }

    public static void onDestroyfinish() {
        for (Activity activity : activities) {
            activity.finish();
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        outState.putBoolean(KEY_NO_TITILE, mNoTitle);
        outState.putInt(KEY_VISIBLE_CONTAINER_ID, mVisibleContainerId);
        super.onSaveInstanceState(outState);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.carrierconfig_menu, menu);
        mUItem = menu.findItem(R.id.enable_trigger);
        mUItem.setCheckable(true);
        if(Utils.getValue(Utils.PROP_KEY_TRIGGER,false))
            mUItem.setChecked(true);
        else
            mUItem.setChecked(false);
        if(mIntent == null){
            mIntent = new Intent(Utils.ACTION_SET_TRIGGER_PROPERTY);
        }
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (mFragment != null) {
            switch (item.getItemId()) {
                case R.id.standard_view:
                    mFragment.onDisplayModeChanged(Utils.STANDARD_VIEW);
                    return true;
                case R.id.hierarchy_view:
                    mFragment.onDisplayModeChanged(Utils.HIERARCHY_VIEW);
                    return true;
                case R.id.enable_trigger:
                    setMenuItemStatus(item);
                    //showAlertDialog();
                default:
                    break;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                PowerManager pm =
                    (PowerManager) ConfigurationActivity.this.getSystemService(Context.POWER_SERVICE);
                pm.reboot("");
                break;
            case DialogInterface.BUTTON_NEGATIVE:
                setMenuItemStatus(mUItem);
                break;
        }
    }

    private void setMenuItemStatus(MenuItem mItem)
    {
        if(mItem.isChecked()){
            Utils.sendMyBroadcast(ConfigurationActivity.this,mIntent,String.valueOf(false));
            mItem.setChecked(false);
        }
        else{
            Utils.sendMyBroadcast(ConfigurationActivity.this,mIntent,String.valueOf(true));
            mItem.setChecked(true);
        }
    }

    private void showAlertDialog(){
        final AlertDialog.Builder mAlertDialog = 
                new AlertDialog.Builder(ConfigurationActivity.this);
        mAlertDialog.setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(R.string.alert_enable_trigger)
                    .setMessage(R.string.alert_trigger_text)
                    .setPositiveButton(android.R.string.ok, this)
                    .setNegativeButton(android.R.string.cancel, this)
                    .create().show();
    }
    private void initFragment() {
        String action = getIntent().getAction();
        if (Intent.ACTION_MAIN.equals(action)) {
            if (getIntent().hasCategory(StartActivityReceiver.CODE_SPEC_SWITCH_L)) {
                getFragmentManager().beginTransaction()
                        .add(R.id.first, new MultiSimConfigFragmentL())
                        .commit();
                getFragmentManager().beginTransaction()
                        .add(R.id.second, new CarrierConfigFragmentL())
                        .commit();
            } else {
                getFragmentManager().beginTransaction()
                        .add(R.id.first, new MultiSimConfigFragment())
                        .commit();
                mFragment = new CarrierConfigFragment();
                getFragmentManager().beginTransaction()
                        .add(R.id.second, mFragment)
                        .commit();
            }

            mNoTitle = false;
            mVisibleContainerId = R.id.two_fragment;
        } else if (Utils.ACTION_CARRIER_RESET.equals(action)){
            getFragmentManager().beginTransaction()
                    .add(R.id.one_fragment, CarrierResetFragment.newInstance())
                    .commit();

            mNoTitle = false;
            mVisibleContainerId = R.id.one_fragment;
        } else {
            ArrayList<Carrier> list =
                    getIntent().getParcelableArrayListExtra(Utils.EXTRA_CARRIER_LIST);
            if (list == null || list.size() < 1) return;

            if (Utils.ACTION_TRIGGER_WELCOME.equals(action)) {
                getFragmentManager().beginTransaction()
                        .add(R.id.one_fragment, TriggerWelcomeFragment.newInstance(list))
                        .commit();
            } else if (Utils.ACTION_TRIGGER_START.equals(action)) {
                getFragmentManager().beginTransaction()
                           .add(R.id.one_fragment,TriggerStartFragment.newInstance(list))
                           .commit();
            } else if (Utils.ACTION_TRIGGER_REBOOT.equals(action)) {
                getFragmentManager().beginTransaction()
                           .add(R.id.one_fragment,TriggerRebootFragment.newInstance(list))
                           .commit();
            } else if (Utils.ACTION_TRIGGER_CONFIGUAGE_LATER.equals(action)) {
                getFragmentManager().beginTransaction()
                           .add(R.id.one_fragment,TriggerConfigureLaterFragment.newInstance(list))
                           .commit();
            }
            mNoTitle = true;
            mVisibleContainerId = R.id.one_fragment;
        }
    }

}
