/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.Utils;

import android.app.ListActivity;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ResolveInfo;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;


public class SelectActionActivity extends ListActivity {
    private final static String TAG = "ListenLog.SelectActionActivity";

    //UI elements
    private TextView uiKeyphraseName;
    private TextView uiInstructionalText;

    private String keyphraseName = null;
    private ArrayList<String> actions = new ArrayList<String>();
    private ArrayList<Intent> actionIntents = new ArrayList<Intent>();
    private ArrayList<Drawable> appIcons = new ArrayList<Drawable>();
    private boolean isModeTraining = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");
        setContentView(R.layout.activity_user_select_action);

        // Get keyphrase name and mode.
        Bundle extras = this.getIntent().getExtras();
        if (null != extras) {
            // Get mode to determine why selectAction was called.
            if (extras.containsKey(Global.EXTRA_SELECTACTION_MODE)) {
                if (extras.getString(Global.EXTRA_SELECTACTION_MODE).equals(
                        Global.MODE_SELECTACTION_TRAINING)) {
                    isModeTraining = true;
                    Log.v(TAG, "onCreate: isModeTraining is true");
                }
            }

            if (extras.containsKey(Global.EXTRA_DATA_KEYPHRASE_NAME)) {
                keyphraseName = extras.getString(Global.EXTRA_DATA_KEYPHRASE_NAME);
                Log.v(TAG, "onCreate: intent extra EXTRA_KEYPHRASE_NAME= " + keyphraseName);
            }
        } else {
            Utils.openAlertDialog(SelectActionActivity.this, TAG,
                    getString(R.string.error), getString(R.string.cannot_continue));
            return;
        }

        // Set UI text
        uiKeyphraseName = (TextView)findViewById(R.id.selectaction_tv_keyphrasename);
        uiInstructionalText = (TextView)findViewById(R.id.selectaction_tv_instruction);

        uiKeyphraseName.setText(getString(R.string.selectaction_keyphrasename) + " " + keyphraseName);
        uiInstructionalText.setText(R.string.selectaction_instruction);

        /** Fill the actions **/
        // Add a "none" action in each of the lists.
        actions.add("None");
        appIcons.add(new ColorDrawable(Color.TRANSPARENT));
        actionIntents.add(null);

        final PackageManager packageManager = getPackageManager();
        Intent mainIntent = new Intent(Intent.ACTION_MAIN, null);
        mainIntent.addCategory(Intent.CATEGORY_LAUNCHER);

        List<ResolveInfo> launcherAppInfos = packageManager.queryIntentActivities(mainIntent, 0);
        Collections.sort(launcherAppInfos, new ResolveInfo.DisplayNameComparator(packageManager));

        List<ApplicationInfo> appInfos = new ArrayList<ApplicationInfo>();
        for (ResolveInfo packageInfo : launcherAppInfos) {
             try {
                appInfos.add(packageManager.getApplicationInfo(packageInfo.activityInfo.packageName, 0));
            } catch (NameNotFoundException e) {
                Log.v(TAG, "onCreate: could not find packageInfo for package= " +
                        packageInfo.activityInfo.packageName);
                e.printStackTrace();
            }
        }

        for (ApplicationInfo appInfo : appInfos) {
            actions.add((packageManager.getApplicationLabel(appInfo)).toString());
            appIcons.add(packageManager.getApplicationIcon(appInfo));
            actionIntents.add(packageManager.getLaunchIntentForPackage(appInfo.packageName));
            /*Log.v(TAG, "onCreate: actionIntents added intent.getComponent()= " +
                    packageManager.getLaunchIntentForPackage(appInfo.packageName).getComponent().
                    getClassName());*/
        }
        Log.v(TAG, "onCreate: actions.size()= " + actions.size());
        Log.v(TAG, "onCreate: appIcons.size()= " + appIcons.size());
        Log.v(TAG, "onCreate: actionIntents.size()= " + actionIntents.size());

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, R.layout.listitem_user_selectaction,
                R.id.listitem_selectaction_appname, actions.toArray(new String[actions.size()])) {
            @Override
            public View getView(int position, View convertView, android.view.ViewGroup parent) {
                View view = super.getView(position, convertView, parent);
                ImageView appIcon = (ImageView)view.findViewById(R.id.listitem_selectaction_appicon);
                TextView appName = (TextView)view.findViewById(R.id.listitem_selectaction_appname);

                appName.setText(actions.get(position));
                appIcon.setImageDrawable(appIcons.get(position));
                return view;
            };
        };
        setListAdapter(adapter);


        ListView listView = getListView();
        listView.setTextFilterEnabled(true);

        listView.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String keyphraseAction = actions.get(position);
                Intent keyphraseActionIntent = actionIntents.get(position);
                if (isModeTraining) {
                    Global.getInstance().getSmRepo().setTempTrainingComKeyphrase(keyphraseName,
                            keyphraseAction, keyphraseActionIntent, true);
                    // SelectActionActivity called to get keyphraseAction as a part of training.
                    // Call TrainingActivity with selected keyphraseAction.
                    Intent intent = new Intent(SelectActionActivity.this, TrainingTipsActivity.class);
                    intent.putExtra(Global.EXTRA_DATA_KEYPHRASE_NAME, keyphraseName);
                    intent.putExtra(Global.TAG_TRAINING_IS_UDK, true);
                    startActivity(intent);
                } else {
                    // SelectActionActivity called for setting keyphraseAction on existing
                    // keyphrase. Save keyphraseAction and return to calling activity.
                    Global.getInstance().getSmRepo().setKeyphraseAction(keyphraseName,
                            keyphraseAction, keyphraseActionIntent);
                    setResult(RESULT_OK);
                    finish();
                }
            }
        });
    }

    @Override
    protected void onResume() {
        Log.v(TAG, "onResume");
        Global.getInstance().incrementNumActivitiesShowing();
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.v(TAG, "onPause");
        Global.getInstance().decrementNumActivitiesShowing();
        super.onPause();
    }
}
