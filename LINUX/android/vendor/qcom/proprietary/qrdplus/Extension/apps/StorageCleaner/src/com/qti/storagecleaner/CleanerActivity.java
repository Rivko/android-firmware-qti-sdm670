/* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.storagecleaner;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.XmlResourceParser;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.ArrayList;

public class CleanerActivity extends Activity implements View.OnClickListener {

    private static final String TAG = "CleanerActivity";
    private ListView mListView;
    private Button mStartButton;
    private ListAdaper mListAdapter;
    private CleanerInfo dataStorageInfo;
    private CleanerInfo sharedStorageInfo;
    private boolean mCleanAppCache = false;
    private static final String START_CLEAR_APPLICATION_CACHE = "com.qti.datastoragecleanerservice.CLEAR_APPLICATION_STORAGE";
    private static final String DATA_SERVICE_BIND_ACTION = "com.qti.datastoragecleanerservice.BIND";
    private static final String DATA_SERVICE_PACKAGE_NAME = "com.qti.datastoragecleanerservice";

    private static final String TAG_CLEANER = "cleaner";
    private static final String TAG_PATH = "path";
    private static final String TAG_STORAGE = "storage";
    private static final String TAG_PATTERN = "pattern";

    private static final String DATA_STORAGE = "data";
    private static final String SHARED_STORAGE = "shared";
    private static final String APP_STORAGE = "app";

    private AlertDialog mProcessDialog;
    private boolean mStartCleanShare = false;
    private boolean mStartCleanData = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cleaner);
        getActionBar().setDisplayShowHomeEnabled(false);
        getActionBar().setDisplayHomeAsUpEnabled(true);
        mListView = (ListView) findViewById(R.id.listView);
        mStartButton = (Button) findViewById(R.id.start_button);
        mStartButton.setOnClickListener(this);
        loadItems();
        bindCleanerService();
    }

    private void bindCleanerService() {
        Intent dataintent = new Intent(DATA_SERVICE_BIND_ACTION)
                .setPackage(DATA_SERVICE_PACKAGE_NAME);
        bindService(dataintent, dataConnectService, Context.BIND_AUTO_CREATE);

        Intent intent = new Intent(this, StorageCleanerService.class);
        bindService(intent, shareConnectService, Context.BIND_AUTO_CREATE);

    }
    private IStorageCleanerService mshareService = null;
    private ICleanerCallBack mCleanerCallback = new ICleanerCallBack.Stub() {

        @Override
        public void onCleanFinish() throws RemoteException {
            if (mStartCleanData && mStartCleanShare) {
                mStartCleanData = false;
                return;
            }
            mStartCleanShare = false;
            if (mProcessDialog != null && mProcessDialog.isShowing()) {
                mProcessDialog.dismiss();
            }
            Toast.makeText(CleanerActivity.this, R.string.clean_finish, Toast.LENGTH_SHORT).show();

        }
    };
    private ServiceConnection shareConnectService = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            Log.d(TAG, "onServiceConnected ibinder is " + iBinder);
            mshareService = IStorageCleanerService.Stub.asInterface(iBinder);;
            try {
                mshareService.registCleanerCallBack(mCleanerCallback);
                if (mStartCleanShare) {
                    mshareService.startCleaner(sharedStorageInfo.path, sharedStorageInfo.pattern);
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {

        }
    };

    private IStorageCleanerService mDataService = null;
    private ServiceConnection dataConnectService = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            Log.d(TAG, "onServiceConnected ibinder is " + iBinder);
            mDataService = IStorageCleanerService.Stub.asInterface(iBinder);
            try {
                mDataService.registCleanerCallBack(mCleanerCallback);
                if (mStartCleanData) {
                    mDataService.startCleaner(dataStorageInfo.path, dataStorageInfo.pattern);
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {

        }
    };
    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(shareConnectService);
        unbindService(dataConnectService);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void loadItems() {
        new AsyncTask<Void, Void, ArrayList<Cleaner>>() {

            @Override
            protected ArrayList<Cleaner> doInBackground(Void... params) {
                ArrayList cleanerList = new ArrayList();
                XmlResourceParser cleanerParser = getResources().getXml(R.xml.cleaner);
                try {
                    int eventType = cleanerParser.getEventType();
                    Cleaner cleaner = null;
                    while (eventType != XmlPullParser.END_DOCUMENT) {
                        switch (eventType) {
                            case XmlPullParser.START_DOCUMENT:
                                break;
                            case XmlPullParser.START_TAG:
                                String tagName = cleanerParser.getName();
                                if (TAG_CLEANER.equalsIgnoreCase(tagName)) {
                                    cleaner = new Cleaner();
                                    cleaner.name_id = cleanerParser.getAttributeResourceValue(0, 0);
                                } else if (TAG_PATH.equals(tagName)) {
                                    if (cleaner != null) {
                                        cleaner.path = cleanerParser.nextText();
                                    }
                                } else if (TAG_STORAGE.equals(tagName)) {
                                    if (cleaner != null) {
                                        cleaner.storage = cleanerParser.nextText();
                                    }
                                } else if (TAG_PATTERN.equals(tagName)) {
                                    if (cleaner != null) {
                                        cleaner.pattern = cleanerParser.nextText();
                                    }
                                }
                                break;
                            case XmlPullParser.END_TAG:
                                tagName = cleanerParser.getName();
                                if (TAG_CLEANER.equalsIgnoreCase(tagName)) {
                                    cleanerList.add(cleaner);
                                    cleaner = null;
                                }
                                break;
                            case XmlPullParser.END_DOCUMENT:
                                break;
                        }
                        cleanerParser.next();
                        eventType = cleanerParser.getEventType();
                    }
                } catch (XmlPullParserException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                return cleanerList;
            }

            @Override
            protected void onPostExecute(ArrayList<Cleaner> cleaners) {
                mListAdapter = new ListAdaper(cleaners);
                mListView.setAdapter(mListAdapter);
            }
        }.execute();
    }

    @Override
    public void onClick(View v) {
        showProcessDialog();
        for (Cleaner cleaner : mListAdapter.mItems) {
            if (cleaner.isChecked) {
                if (cleaner.storage.equalsIgnoreCase(DATA_STORAGE)) {
                    if (dataStorageInfo == null) {
                        dataStorageInfo = new CleanerInfo();
                        dataStorageInfo.path =  cleaner.path;
                        dataStorageInfo.pattern = cleaner.pattern;
                    } else {
                        dataStorageInfo.path += ";" + cleaner.path;
                        dataStorageInfo.pattern += ";" + cleaner.pattern;
                    }
                } else if (cleaner.storage.equalsIgnoreCase(SHARED_STORAGE)) {
                    if (sharedStorageInfo == null) {
                        sharedStorageInfo = new CleanerInfo();
                        sharedStorageInfo.path = cleaner.path;
                        sharedStorageInfo.pattern = cleaner.pattern;
                    } else {
                        sharedStorageInfo.path += ";" + cleaner.path;
                        sharedStorageInfo.pattern += ";" + cleaner.pattern;
                    }
                } else if (cleaner.storage.equalsIgnoreCase(APP_STORAGE)) {
                    mCleanAppCache = true;
                }
            }
        }
        combinStorageInfo();
        if (sharedStorageInfo != null) {
            mStartCleanShare = true;
            if (mshareService != null) {
                try {
                    mshareService.startCleaner(sharedStorageInfo.path, sharedStorageInfo.pattern);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            } else {
                bindCleanerSharedService();
            }
        }

        if (mCleanAppCache) {
            startService(new Intent(START_CLEAR_APPLICATION_CACHE));
        }
        if (dataStorageInfo != null) {
            mStartCleanData = true;
            if (mDataService != null) {
                try {
                    mDataService.startCleaner(dataStorageInfo.path, dataStorageInfo.pattern);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            } else {
                bindCleanerDataService();
            }
        }

    }

    private void bindCleanerSharedService() {
        Intent intent = new Intent(this, StorageCleanerService.class);
        bindService(intent, shareConnectService, Context.BIND_AUTO_CREATE);
    }

    private void bindCleanerDataService() {
        Intent dataintent = new Intent(DATA_SERVICE_BIND_ACTION)
                .setPackage(DATA_SERVICE_PACKAGE_NAME);
        bindService(dataintent, dataConnectService, Context.BIND_AUTO_CREATE);
    }

    private void showProcessDialog() {
        if (mProcessDialog == null) {
            mProcessDialog = new ProgressDialog(this);
            mProcessDialog.setTitle(R.string.clean_title);
            mProcessDialog.setMessage(getString(R.string.clean_content));
            mProcessDialog.setCancelable(false);
        }
        mProcessDialog.show();
    }

    private void combinStorageInfo() {
        combinStorageInfo(dataStorageInfo);
        combinStorageInfo(sharedStorageInfo);
    }

    private void combinStorageInfo(CleanerInfo info) {
        if (info == null) return;
        String[] path = info.path.split(";");
        String[] pattern = info.pattern.split(";");
        String[][] patterns = new String[pattern.length][];
        for (int i = 0; i < pattern.length; i++) {
            patterns[i] = pattern[i].split(",");
        }

        if(path.length != pattern.length) {
            throw new RuntimeException("Wrong");
        }
        String final_path = path[0];
        for (int i = 0; i < path.length; i++) {
            String[] temp_path = final_path.split("/");
            String[] temp_path1 = path[i].split("/");
            for (int j = 0; j < (temp_path.length < temp_path1.length ?
                    temp_path.length : temp_path1.length); j++) {
                boolean handled = false;
                if (!temp_path[j].equalsIgnoreCase(temp_path1[j])) {
                    String prefix = final_path.substring(final_path.indexOf(temp_path[j]));
                    for (int k = i - 1; k >= 0; k--) {
                        for (int n = 0; n < patterns[k].length; n++)
                            patterns[k][n] = prefix + "/" + patterns[k][n];
                    }
                    for (int n = 0; n < patterns[i].length; n++)
                        patterns[i][n] = path[i].substring(path[i].indexOf(temp_path1[j])) +
                                "/" + patterns[i][n];
                    final_path = final_path.substring(0, final_path.indexOf(temp_path[j]));
                    handled = true;
                    break;
                }
                if (!handled && temp_path.length != temp_path1.length) {
                    if (temp_path.length < temp_path1.length) {
                        for (int n = 0; n < patterns[i].length; n++)
                            patterns[i][n] = path[i].substring(path[i].indexOf(temp_path1[j])) +
                                    "/" + patterns[i][n];
                    } else {
                        String prefix = final_path.substring(final_path.indexOf(temp_path[j]));
                        for (int k = i - 1; k >= 0; k--) {
                            for (int n = 0; n < patterns[k].length; n++)
                                patterns[k][n] = prefix + "/" + patterns[k][n];
                        }
                    }
                }
            }
        }
        info.path = final_path;
        StringBuilder s = new StringBuilder();
        for (int i = 0; i < patterns.length; i++) {
            for (int j = 0; j < patterns[i].length; j++) {
                if (i == 0 && j == 0) {
                    s.append(patterns[i][j]);
                } else {
                    s.append(",");
                    s.append(patterns[i][j]);
                }
            }
        }
        info.pattern = s.toString();
    }


    private class Cleaner {
        int name_id;
        String path;
        String storage;
        String pattern;
        boolean isChecked = true;
    }

    private class CleanerInfo {
        String path;
        String pattern;
    }

    private class ListAdaper extends BaseAdapter{
        private final ArrayList<Cleaner> mItems;

        public ListAdaper(ArrayList<Cleaner> cleaners) {
            mItems = cleaners;
        }

        @Override
        public int getCount() {
            return mItems.size();
        }

        @Override
        public Object getItem(int position) {
            return mItems.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            Cleaner item = mItems.get(position);
            View v;
            if (convertView == null) {
                v = getLayoutInflater().inflate(R.layout.cleaner_item_view, null);
            } else {
                v = convertView;
            }
            TextView tv = (TextView) v.findViewById(R.id.textView);
            tv.setText(item.name_id);
            final CheckBox cb = (CheckBox) v.findViewById(R.id.checkBox);
            cb.setChecked(item.isChecked);
            cb.setTag(item);
            cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    Cleaner cleaner = (Cleaner) cb.getTag();
                    cleaner.isChecked = isChecked;
                    if (isItemSelected()) {
                        mStartButton.setEnabled(true);
                    } else {
                        mStartButton.setEnabled(false);
                    }
                }
            });
            return v;
        }

        private boolean isItemSelected() {
            for (Cleaner cleaner : mItems) {
                if (cleaner.isChecked) {
                    return true;
                }
            }
            return false;
        }
    }
}
