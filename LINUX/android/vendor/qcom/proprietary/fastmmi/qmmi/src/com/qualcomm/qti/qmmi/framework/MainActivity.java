/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.service.diag.MmiDiagService;
import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;
import java.util.ArrayList;
import java.util.List;
import com.qualcomm.qti.qmmi.model.HidlManager;


public class MainActivity extends Activity implements CasesContract.View {
    private static final String SYS_BOOTMODE_PROPERTY = "sys.boot_mode";


    private MainPresenter mPresenter;
    private ListView listView;
    private CasesAdapter mListAdapter;
    private TextView mStatusiew;
    private PackageInfo mPackageInfo;
    static int selected_position = 0;
    Context mContext;

    //For RunAll feature
    private MenuItem mRunAllMenu;
    private boolean mAllRunning = false;
    private List<TestCase> mRunAllCaseList;
    private int mCurIndex;
    private static final int REQUEST_RUN_ALL_CASES = 100;
    private static final int REQUEST_START_CASES_ACTIVITY = 200;

    public static int WAIT_BEFORE_REBOOT_MS = 200;

    BroadcastReceiver uiUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            LogUtils.logi( "start to update ui,action:" + intent.getAction());
            if (Utils.ACTION_DIAG_UPDATE_TESTCAST_LIST.equals(intent.getAction())) {
                refreshUI();
                startAllServices(mPresenter.getTestCaseList());
                mListAdapter.notifyDataSetChanged();
            } else if (Utils.ACTION_DIAG_UPDATE_TESTCAST_RESULT.equals(intent.getAction())) {
                mListAdapter.notifyDataSetChanged();
                mPresenter.updateResult();
            } else if (Utils.ACTION_DIAG_START_TESTCAST.equals(intent.getAction())) {
                String caseName = intent.getExtras().getString(Utils.BUNDLE_KEY_CASE_NAME);
                TestCase testCase = mPresenter.getTestCaseByName(caseName);
                if (testCase != null) {
                    startCaseActivity(testCase.getActivityClass(), testCase);
                }
            } else if (Utils.ACTION_UPDATE_BACKGROUND_CASE_RESULT.equals(intent.getAction())) {
                //Stop Case
                String caseName = intent.getExtras().getString(Utils.BUNDLE_KEY_CASE_NAME);
                mPresenter.stopCase(mPresenter.getTestCaseByName(caseName));
                updateAutoCaseResult(intent);
            }
        }
    };

    private void updateAutoCaseResult(Intent intent) {
        Bundle bundle = intent.getExtras();
        int state = bundle.getInt(Utils.BUNDLE_KEY_RESULT);
        String caseName = bundle.getString(Utils.BUNDLE_KEY_CASE_NAME);
        mPresenter.updateCaseResult(mPresenter.getTestCaseByName(caseName), state);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //Cant show Actionbar when use FEATURE_LEFT_ICON
        //requestWindowFeature(Window.FEATURE_LEFT_ICON);
        //getWindow().setFeatureDrawableResource(Window.FEATURE_LEFT_ICON, R.drawable.q);

        mContext = getApplicationContext();
        //Keep screen on.
        Utils.wakeUpAndKeepScreenOn(this);

        setContentView(R.layout.activity_main);

        mStatusiew = (TextView) findViewById(R.id.status);
        listView = (ListView) findViewById(R.id.items_list);

        mListAdapter = new CasesAdapter(new ArrayList<TestCase>(0), mItemListener, mContext);
        listView.setAdapter(mListAdapter);

        /**Get version*/
        try {
            PackageManager pm = getPackageManager();
            mPackageInfo = pm.getPackageInfo(getPackageName(), PackageManager.GET_CONFIGURATIONS);
        } catch (PackageManager.NameNotFoundException e) {
            LogUtils.loge("get version failed");
        }

        mPresenter = new MainPresenter(MainActivity.this, getApplicationContext());
        mPresenter.start();
        mPresenter.setPackageInfo(mPackageInfo);

        /**register broadcast*/
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(Utils.ACTION_DIAG_UPDATE_TESTCAST_LIST);
        mIntentFilter.addAction(Utils.ACTION_DIAG_UPDATE_TESTCAST_RESULT);
        mIntentFilter.addAction(Utils.ACTION_DIAG_START_TESTCAST);
        mIntentFilter.addAction(Utils.ACTION_UPDATE_BACKGROUND_CASE_RESULT);
        registerReceiver(uiUpdateReceiver, mIntentFilter);

        /**prepare*/
        prepare();
    }

    private void prepare() {
        //copy the raw data file into data/ .../files
        LogUtils.logi("prepare start");
        FileUtils.copy(getResources().openRawResourceFd(R.raw.qualsound), Utils.SOUND_FILE_PATH.trim());
        FileUtils.copy(getResources().openRawResourceFd(R.raw.tone_single_1khz), Utils.TONE_1KHZ_FILE_PATH.trim());
        LogUtils.logi("end to prepare");
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    private void restoreRunAllCaseList(){
        if ( mAllRunning && mRunAllCaseList == null){
            LogUtils.logi( "Restore RunAll CaseList");
            mRunAllCaseList = mPresenter.getTestCaseList();
        }
    }

    @Override
    protected void onStop() {
        // TODO Auto-generated method stub
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(uiUpdateReceiver);
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        LogUtils.logi( "onActivityResult: requestCode :" + requestCode + " resultCode:" + resultCode);
        if (requestCode == REQUEST_RUN_ALL_CASES){
            if (resultCode == RESULT_OK){
                refreshUI();
                runAll();
            }
        } else if(requestCode == REQUEST_START_CASES_ACTIVITY){
            refreshUI();
        }
    }

    @Override
    public void updateStatus(String status) {
        mStatusiew.setText(status);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        mRunAllMenu = menu.findItem(R.id.menu_run_all);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_run_all:
                //Run All
                handleRunStopAll();
                break;
            case R.id.menu_settings:
                break;
            case R.id.menu_refresh:
                refreshUI();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private void updateRunAllUI(boolean runAll){
        if (runAll){
            mAllRunning = true;
            mCurIndex = 0;
            mRunAllMenu.setTitle(R.string.menu_stop_run);
        } else{
            mAllRunning = false;
            mCurIndex = 0;
            mRunAllMenu.setTitle(R.string.menu_run_all);
        }
    }

    private void runAll() {
        restoreRunAllCaseList();

        if ( !checkIfRunning() ){
            return;
        }

        LogUtils.logi("runAll - Case index : " + mCurIndex);
        if (mRunAllCaseList != null) {
            TestCase testCase = mRunAllCaseList.get(mCurIndex);
            if (testCase != null) {
                LogUtils.logi("runAll - Case Name : " + testCase.getName());
                LogUtils.logi("runAll - Case Auto : " + testCase.getAuto());
                //For Cases: Auto is true, then run test case service
                if (testCase.getAuto()){
                    LogUtils.logi("runAll - run service : " + testCase.getServiceClass().getName());
                    testCase.setLaunchMode(TestCase.LAUNCH_MODE_BACKGROUND);
                    mPresenter.runCase(testCase);
                    mCurIndex++;
                    runAll();
                }else{
                    LogUtils.logi("autoRun - run activity : " + testCase.getActivityClass().getName());
                    startCaseActivityForResult(testCase);
                    mCurIndex++;
                }
            }
        }
    }

    private void stopAllRunningCases() {
        if ( !mAllRunning ){
            return;
        }

        restoreRunAllCaseList();
        //Just need stop auto true case, since foreground case already stop when exit.
        for(int i = 0; i < mCurIndex; i++){
            TestCase testCase = mRunAllCaseList.get(i);
            if (mAllRunning && testCase != null && testCase.getAuto()){
                mPresenter.stopCase(testCase);
            }
        }
    }

    private boolean checkIfRunning(){
        //Check if all cases are all done.
        if ( mRunAllCaseList != null && mCurIndex >= mRunAllCaseList.size() ){
            //Should not call here, auto cases should call stop in itself to run complete.
            //stopAllRunningCases();
            updateRunAllUI(false);
            return false;
        } else if ( !mAllRunning ) {
            return false;
        }
        return true;
    }

    private void refreshUI(){
        //refresh test case listview
        //TODO :check if duplicate with Presenter.updateResult(), confirm the data source of adapter.
        showCases(mPresenter.getTestCaseList());
        //refresh case status bar
        mPresenter.updateResult();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        // TODO Auto-generated method stub
        super.onSaveInstanceState(outState);
    }

    private void handleRunStopAll(){

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setIcon(R.mipmap.ic_launcher);

        if (!mAllRunning){
            builder.setTitle(R.string.run_all_dlg_title);
            builder.setMessage(R.string.run_all_dlg_start);
            builder.setPositiveButton(R.string.postive_button, new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface dialog, int which) {
                    // TODO Auto-generated method stub
                    updateRunAllUI(true);
                    mRunAllCaseList = mPresenter.getTestCaseList();
                    runAll();
                }
            });
            builder.setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface dialog, int which) {
                    // TODO Auto-generated method stub
                    return;
                }
            });
        } /*Stop */ else {
            builder.setTitle(R.string.stop_run_dlg_title);
            builder.setMessage(R.string.stop_run_dlg_stop);
            builder.setPositiveButton(R.string.postive_button, new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface dialog, int which) {
                    // TODO Auto-generated method stub
                    stopAllRunningCases();
                    updateRunAllUI(false);
                }
            });
            builder.setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface dialog, int which) {
                    // TODO Auto-generated method stub
                    return;
                }
            });
        }

        builder.setCancelable(true);
        builder.show();
    }

    public void onReset(View v) {
        LogUtils.logi( "Reset start");
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setIcon(R.mipmap.ic_launcher);
        builder.setTitle(R.string.reset_dialog);
        builder.setMessage(R.string.reset_dialog_message);

        //Button event
        builder.setPositiveButton(R.string.postive_button, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                mPresenter.clearResult();
                refreshUI();
                Toast.makeText(getApplicationContext(), R.string.reset_toast_postive, Toast.LENGTH_SHORT).show();
            }
        });
        builder.setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                Toast.makeText(getApplicationContext(), R.string.reset_toast_negative, Toast.LENGTH_SHORT).show();
            }
        });

        //Set to cancel
        builder.setCancelable(true);
        AlertDialog dialog = builder.create();
        dialog.show();
    }

    public void onReboot(View v) {
        LogUtils.logi( "reboot start");

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setIcon(R.mipmap.ic_launcher);
        builder.setTitle(R.string.reboot_dialog);

        final String[] items = this.getResources().getStringArray(R.array.reboot_items);
        builder.setSingleChoiceItems(items, 0, new DialogInterface.OnClickListener() {

            public void onClick(DialogInterface dialog, int which) {
                selected_position = which;
            }
        });

        //Button event
        builder.setPositiveButton(R.string.postive_button, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {

                switch (selected_position) {
                    case 0:
                        powerOff(mContext);
                        Toast.makeText(getApplicationContext(), R.string.reboot_toast_poweroff, Toast.LENGTH_SHORT).show();
                        break;
                    case 1:
                        rebootToAndroid(mContext);
                        Toast.makeText(getApplicationContext(), R.string.reboot_toast_reboot_android, Toast.LENGTH_SHORT).show();
                        break;
                    case 2:
                        rebootToFFBM(mContext);
                        Toast.makeText(getApplicationContext(), R.string.reboot_toast_reboot_ffbm, Toast.LENGTH_SHORT).show();
                        break;
                    case 3:
                        rebootToQMMI(mContext);
                        Toast.makeText(getApplicationContext(), R.string.reboot_toast_reboot_qmmi, Toast.LENGTH_SHORT).show();
                        break;
                    case 4:
                        Toast.makeText(getApplicationContext(), R.string.reboot_toast_poweroff_ship_mode, Toast.LENGTH_SHORT).show();
                        powerOffToShipMode(mContext);
                        break;
                    default:
                        break;
                }
                LogUtils.logi( "clicked:" + items[selected_position]);

            }
        });
        builder.setNegativeButton(R.string.negative_button, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                Toast.makeText(getApplicationContext(), R.string.reboot_toast_negative, Toast.LENGTH_SHORT).show();
            }
        });

        //Set to cancel
        builder.setCancelable(true);
        AlertDialog dialog = builder.create();
        dialog.show();
    }

    /**
     * Implement reboot to Qmmi immediately.
     */
    private static void rebootToQMMI(Context context) {
        SystemProperties.set(SYS_BOOTMODE_PROPERTY, "qmmi");
        SystemClock.sleep(WAIT_BEFORE_REBOOT_MS);
        reboot(context);
    }

    /**
     * Implement reboot to android immediately.
     */
    private static void rebootToAndroid(Context context) {
        SystemProperties.set(SYS_BOOTMODE_PROPERTY, "normal");
        SystemClock.sleep(WAIT_BEFORE_REBOOT_MS);
        reboot(context);
    }

    /**
     * Implement reboot to FFBM immediately.
     */
    private static void rebootToFFBM(Context context) {
        SystemProperties.set(SYS_BOOTMODE_PROPERTY, "ffbm");
        SystemClock.sleep(WAIT_BEFORE_REBOOT_MS);
        reboot(context);
    }

    /**
     * Power off to ship mode immediately.
     * @param context
     */
    private static void powerOffToShipMode(Context context){

        HidlManager hidlManager = HidlManager.getInstance();
        if (hidlManager == null) {
            LogUtils.loge("Not support");
            return;
        }

        boolean ret = hidlManager.enterShipMode();

        if (ret){
            powerOff(context);
        } else{
            Toast.makeText(context, R.string.reboot_toast_set_mode_fail, Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Implement reboot function immediately.
     */
    private static void reboot(Context context) {
        Intent requestReboot = new Intent(
                Intent.ACTION_REBOOT);
        requestReboot.putExtra(Intent.EXTRA_KEY_CONFIRM, false);
        requestReboot.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(requestReboot);
    }

    /**
     * Implement power off function immediately.
     */
    private static void powerOff(Context context) {
        Intent requestShutdown = new Intent(
                Intent.ACTION_REQUEST_SHUTDOWN);
        requestShutdown.putExtra(Intent.EXTRA_KEY_CONFIRM, false);
        requestShutdown.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(requestShutdown);
    }

    CaseItemListener mItemListener = new CaseItemListener() {
        @Override
        public void onCaseClick(TestCase clickedCase) {
            startCaseActivity(clickedCase.getActivityClass(), clickedCase);
        }
    };

    public interface CaseItemListener {
        void onCaseClick(TestCase clickedCase);
    }

    @Override
    public void showCases(List<TestCase> cases) {
        mListAdapter.replaceData(cases);
    }

    @Override
    public void updateResult() {
        LogUtils.logi("updateResult");
        mListAdapter.notifyDataSetChanged();
    }

    @Override
    public void startAllServices(List<TestCase> cases) {
        /*
        *  Start diag service
        * */
        startDiagService();

        /*
         *  Start case service
         * */
        MyThread myThread = new MyThread(cases);
        myThread.start();
    }


    public static class CasesAdapter extends BaseAdapter {
        private List<TestCase> mCases;
        private CaseItemListener mItemListener;
        private Context mContext;

        public CasesAdapter(List<TestCase> cases, CaseItemListener itemListener, Context context) {
            setList(cases);
            mItemListener = itemListener;
            mContext = context;
        }

        public void replaceData(List<TestCase> cases) {
            setList(cases);
        }

        @Override
        public int getCount() {
            return mCases != null ? mCases.size() : 0;
        }

        @Override
        public TestCase getItem(int i) {
            return mCases.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            View rowView = view;
            if (rowView == null) {
                LayoutInflater inflater = LayoutInflater.from(viewGroup.getContext());
                rowView = inflater.inflate(R.layout.case_item, viewGroup, false);
            }

            final TestCase testCase = getItem(i);

            TextView titleTV = (TextView) rowView.findViewById(R.id.title);
            int resId = mContext.getResources().getIdentifier(testCase.getName(), "string", mContext.getPackageName());
            titleTV.setText(String.valueOf(i + 1) + ". " + mContext.getResources().getText(resId));

            ImageView imageView = (ImageView) rowView.findViewById(R.id.result);
            LogUtils.logi("caseName:" + testCase.getName() + " state:" + testCase.getResult());
            if (testCase.getResult() == TestCase.STATE_PASS) {
                imageView.setImageResource(R.drawable.passpic);
            } else if (testCase.getResult() == TestCase.STATE_FAIL) {
                imageView.setImageResource(R.drawable.failpic);
            } else {
                imageView.setImageDrawable(null);
            }

            rowView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    mItemListener.onCaseClick(testCase);
                }
            });
            return rowView;
        }

        private void setList(List<TestCase> cases) {
            mCases = cases;
            notifyDataSetChanged();
        }
    }

    public void startCaseActivity(Class<?> activityClass, TestCase testCase) {
        if (activityClass == null) {
            return;
        }
        Bundle bundle = new Bundle();
        Intent intent = new Intent(MainActivity.this, activityClass);
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, testCase.getName());
        intent.putExtras(bundle);
        startActivityForResult(intent,REQUEST_START_CASES_ACTIVITY);
    }

    public void startCaseActivityForResult(TestCase testCase) {
        LogUtils.logi( "startCaseActivityForResult: name: " + testCase.getName());
        Class<?> activityClass = testCase.getActivityClass();
        LogUtils.logi( "startCaseActivityForResult: activityClass: " + activityClass.getName());
        if (activityClass == null) {
            return;
        }
        Bundle bundle = new Bundle();
        Intent intent = new Intent(MainActivity.this, activityClass);
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, testCase.getName());
        intent.putExtras(bundle);
        startActivityForResult(intent,REQUEST_RUN_ALL_CASES);
    }

    public void startDiagService() {
        LogUtils.logi( "startDiagService start");
        Intent intent = new Intent(this, MmiDiagService.class);
        intent.setPackage(this.getPackageName());
        //set the base path used by diag native:/data/user/0/com.qualcomm.qti.qmmi/files/
        intent.putExtra("fileBaseDir", getFilesDir().getAbsolutePath() + "/");
        startService(intent);
    }

    private class MyThread extends Thread {
        List<TestCase> cases;

        public MyThread(List<TestCase> cases) {
            this.cases = cases;
        }

        @Override
        public void run() {
            try {
                for (TestCase testCase : cases) {
                    mPresenter.startCaseService((Context)(MainActivity.this),testCase);
                }
            } catch (Exception ex) {
                LogUtils.loge(ex);
            }
        }
    }
}
