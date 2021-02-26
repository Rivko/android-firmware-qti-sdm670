/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;

import com.qualcomm.qti.smartassistant.service.Messages;
import com.qualcomm.qti.smartassistant.training.SoundModelTrainer;
import com.qualcomm.qti.smartassistant.training.TrainingAssistant;
import com.qualcomm.qti.smartassistant.widget.UserDialog;
import com.qualcomm.qti.smartassistant.utils.SharedPreferenceUtils;

import java.util.ArrayList;

public class AssistantSettingsActivity extends ServiceActivity implements View.OnClickListener{

    public static final int REQUEST_TRAINING_CODE = 3;
    private static final String TAG = "AssistantSettingsActivity";
    private TextView mAppVersionView;
    private View mAddUserView;
    private Switch mOneshotSwitch;
    private Switch mUserVerificationSwitch;
    private Switch mAssistantSwitch;
    private LinearLayout mUserArea;
    private ListView mUserList;
    private UserListAdapter mUserAdapter;
    private ProgressDialog mProgressDialog = null;

    UserDialog mAddUserDialog, mEditUserDialog;
    AlertDialog mDelConfirmDialog;

    private boolean mAssistantStatus = false;
    private boolean mAssistantStatusBeforeTraining = false;
    private boolean mOneshotStatus = false;
    private boolean mUserVerificationStatus = false;


    private SoundModelTrainer mModelTrainer;
    private TextView mOneShotLabel;
    private TextView mUserVerificationLabel;
    private TextView mAddUserLabel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_assistant_settings);

        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        initializeUserInterface();
        updateViewsDisplay();

        mModelTrainer = new SoundModelTrainer(TrainingAssistant.DEFAULT_KEY_PHRASE);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void initializeUserInterface(){
        mAppVersionView = findViewById(R.id.app_version);
        mAddUserView = findViewById(R.id.add_user_layout);
        mAssistantSwitch = findViewById(R.id.assistant_switch);
        mOneshotSwitch = findViewById(R.id.one_shot_switch);
        mUserVerificationSwitch = findViewById(R.id.user_verification_switch);
        mOneShotLabel = findViewById(R.id.one_shot_label);
        mUserVerificationLabel = findViewById(R.id.user_verification_label);
        mUserArea = findViewById(R.id.users_list_area);
        mUserList = findViewById(R.id.user_list);
        ArrayList<String> users = getCurrentUsers();
        mUserAdapter = new UserListAdapter(this, users);
        mUserList.setAdapter(mUserAdapter);
        mAddUserLabel = findViewById(R.id.add_user_label);

        mAddUserView.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.add_user_layout:
                UserDialog.Builder builder = new UserDialog.Builder(
                        AssistantSettingsActivity.this);
                builder.setTitle(R.string.add_user_dialog_title);
                builder.setHint(R.string.user_edit_hint);
                builder.setPositiveButton(R.string.user_alert_training_text,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                Intent intent = new Intent(AssistantSettingsActivity.this,
                                        TrainingActivity.class);
                                intent.putExtra(TrainingAssistant.TAG_TRAINING_KEYPHRASE, TrainingAssistant.DEFAULT_KEY_PHRASE);
                                intent.putExtra(TrainingAssistant.TAG_TRAINING_USER, mAddUserDialog.getEditContent());
                                intent.putExtra(TrainingAssistant.TAG_TRAINING_IS_UDK, true);
                                dialogInterface.dismiss();
                                sendRequest(Messages.STOP_RECOGNITION_REQUEST, null);
                                mAssistantStatusBeforeTraining = mAssistantStatus;
                                AssistantSettingsActivity.this.startActivityForResult(intent, REQUEST_TRAINING_CODE);
                            }
                        });

                builder.setNegtiveButton(R.string.user_alert_cancel_text,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                dialogInterface.dismiss();
                            }
                        });
                mAddUserDialog = builder.create();
                mAddUserDialog.show();
                break;
        }
    }

    private void updateViewsDisplay(){
        updateAddUserView();
        updateSwitchesDisplay();
        updateAppVersionInfo();
    }

    private void updateAddUserView() {
        if (mUserAdapter.getCount() < TrainingAssistant.NUM_MAX_USERS) {
            mAddUserView.setVisibility(View.VISIBLE);
        } else {
            mAddUserView.setVisibility(View.GONE);
        }
    }

    private ArrayList<String> getCurrentUsers() {
        return SharedPreferenceUtils.getUserListData(this);
    }

    private void updateSwitchesDisplay() {
        mAssistantStatus = SharedPreferenceUtils.getAssistantEnabled(this);
        mOneshotStatus = SharedPreferenceUtils.getIsLABEnabled(this);
        mUserVerificationStatus = SharedPreferenceUtils.getIsUserVerificationEnabled(this);

        mAssistantSwitch.setChecked(mAssistantStatus);
        mOneshotSwitch.setChecked(mOneshotStatus);
        mUserVerificationSwitch.setChecked(mUserVerificationStatus);
        updateSwitchesStatus();

        mAssistantSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean state) {
                if (state) {
                    sendRequest(Messages.START_RECOGNITION_REQUEST, null);
                    mProgressDialog = ProgressDialog.show(AssistantSettingsActivity.this,
                            "Opening", "Please wait", true, false);
                } else {
                    sendRequest(Messages.STOP_RECOGNITION_REQUEST, null);
                    mProgressDialog = ProgressDialog.show(AssistantSettingsActivity.this,
                            "Closing", "Please wait", true, false);
                }
            }
        });

        mOneshotSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean state) {
                SharedPreferenceUtils.setIsLABEnabled(AssistantSettingsActivity.this, state);
            }
        });

        mUserVerificationSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean state) {
                SharedPreferenceUtils.setIsUserVerificationEnabled(AssistantSettingsActivity.this,
                        state);
                if (state) {
                    mUserArea.setVisibility(View.VISIBLE);
                } else {
                    mUserArea.setVisibility(View.GONE);
                }
            }
        });
        mUserList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, final int position, long l) {
                //TODO: start the dialog to rename or delete.
                String username = mUserAdapter.getItem(position);

                UserDialog.Builder builder = new UserDialog.Builder(
                        AssistantSettingsActivity.this);
                builder.setTitle(R.string.edit_user_dialog_title);
                builder.setEditText(username);
                builder.setEditable(false);
                builder.setPositiveButton(R.string.user_alert_ok_text,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                String newName = mEditUserDialog.getEditContent();
                                if (null != newName && newName.length() > 0) {
                                    mUserAdapter.updateItemName(position, newName);
                                    SharedPreferenceUtils.renameUserData(AssistantSettingsActivity.this,
                                            position, newName);
                                }
                                mEditUserDialog.dismiss();
                            }
                        });
                builder.setNegtiveButton(R.string.user_alert_delete_text,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                startConfirmDialog(position);
                                mEditUserDialog.dismiss();
                            }
                        });
                mEditUserDialog = builder.create();
                mEditUserDialog.show();
            }
        });
    }

    private void updateAppVersionInfo() {
        String version = getAppVersion();
        String versionStr = getResources().getString(R.string.application_version) + version;
        mAppVersionView.setText(versionStr);
    }

    private String getAppVersion() {
        try {
            return getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }

    private void startConfirmDialog(final int position) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View view = View.inflate(this, R.layout.dialog_user_delete_confirm, null);
        builder.setView(view);
        builder.setCancelable(false);
        TextView title = view.findViewById(R.id.delete_alert_title);
        title.setText(R.string.remove_user_dialog_title);
        Button pBtn = view.findViewById(R.id.del_positiveButton);
        Button nBtn = view.findViewById(R.id.del_negativeButton);
        pBtn.setText(R.string.dialog_yes);
        nBtn.setText(R.string.dialog_no);

        pBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String pleaseWaitStr = getString(R.string.home_dialog_pleasewait);
                String RemoveUserStr = getString(R.string.removing_dialog_deletesoundmodel);
                mProgressDialog = ProgressDialog.show(AssistantSettingsActivity.this,
                        RemoveUserStr, pleaseWaitStr);

                //TODO:
                String realUserName = SharedPreferenceUtils.getRealUserName(AssistantSettingsActivity.this,
                        position);
                new RemoveUserTask(realUserName, position).execute(realUserName);
                mDelConfirmDialog.dismiss();
            }
        });

        nBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mDelConfirmDialog.dismiss();
            }
        });
        mDelConfirmDialog = builder.create();
        mDelConfirmDialog.show();
    }



    private void clearUsers() {
        SharedPreferenceUtils.clearUsers(this);
        mUserAdapter.clear();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.e("juwei", "onActivityResult, resultcode = " + resultCode);
        if (REQUEST_TRAINING_CODE == requestCode) {
            if (RESULT_OK == resultCode) {
                String name = data.getStringExtra(TrainingActivity.RESULT_CONTENT);
                Log.e("juwei", "training good name = " + name);
                if (name != null) {
                    mUserAdapter.addItemData(name);
                    SharedPreferenceUtils.addUserListData(this, name);
                    updateAddUserView();
                }
            }
            if (mAssistantStatusBeforeTraining){
                sendRequest(Messages.START_RECOGNITION_REQUEST, null);
            }
        }
    }

    private class RemoveUserTask extends AsyncTask<String, Void, Boolean> {
        private String sUserName;
        private int sPosition;
        public RemoveUserTask(String username, int position) {
            sUserName = username;
            sPosition = position;
        }

        @Override
        protected Boolean doInBackground(String...strings) {
            return mModelTrainer.deleteUserFromSoundModel(sUserName);
        }

        @Override
        protected void onPostExecute(Boolean result) {
            if (mProgressDialog != null && mProgressDialog.isShowing()) {
                mProgressDialog.dismiss();
            }
            if (result) {
                mUserAdapter.removeItem(sPosition);
                SharedPreferenceUtils.removeUser(AssistantSettingsActivity.this,
                        sPosition);
                updateAddUserView();
            }
        }
    }

    @Override
    protected void handleStartRecognition() {
        //TODO: change the switch status
        mAssistantStatus = true;
        updateSwitchesStatus();
        SharedPreferenceUtils.setAssistantEnabled(AssistantSettingsActivity.this, true);
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            mProgressDialog.dismiss();
        }
    }

    @Override
    protected void handleStopRecognition() {
        //TODO: change the swtich status
        mAssistantStatus = false;
        updateSwitchesStatus();
        SharedPreferenceUtils.setAssistantEnabled(AssistantSettingsActivity.this, false);
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            mProgressDialog.dismiss();
        }
    }

    private void updateSwitchesStatus(){
        if (mAssistantStatus){
            mOneshotSwitch.setEnabled(false);
            mUserVerificationSwitch.setEnabled(false);
            mOneShotLabel.setTextColor(getResources().getColor(android.R.color.darker_gray));
            mUserVerificationLabel.setTextColor(getResources().getColor(android.R.color.darker_gray));
            mAddUserLabel.setTextColor(getResources().getColor(android.R.color.darker_gray));
            mAddUserView.setEnabled(false);
            mUserList.setEnabled(false);
            mUserArea.setVisibility(View.GONE);
        }else {
            mOneshotSwitch.setEnabled(true);
            mUserVerificationSwitch.setEnabled(true);
            mOneShotLabel.setTextColor(getResources().getColor(android.R.color.black));
            mUserVerificationLabel.setTextColor(getResources().getColor(android.R.color.black));
            mAddUserLabel.setTextColor(getResources().getColor(android.R.color.black));
            mAddUserView.setEnabled(true);
            mUserList.setEnabled(true);
        }
        mUserAdapter.notifyDataSetChanged();
        if (mUserVerificationSwitch.isChecked()) {
            mUserArea.setVisibility(View.VISIBLE);
        } else {
            mUserArea.setVisibility(View.GONE);
        }
    }

    public class UserListAdapter extends BaseAdapter {

        private ArrayList<String> mUsers;
        private Context mContext;


        public UserListAdapter(Context context, ArrayList<String> users) {
            mContext = context;
            mUsers = users;
        }


        @Override
        public int getCount() {
            return mUsers.size();
        }

        @Override
        public String getItem(int i) {
            return mUsers.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            Holder holder;
            if(null == view) {
                LayoutInflater inflater = (LayoutInflater)mContext.getSystemService(
                        Context.LAYOUT_INFLATER_SERVICE);
                view = inflater.inflate(R.layout.user_sound_model_item, null);
                holder = new Holder();
                holder.text = view.findViewById(R.id.user_title);


                view.setTag(holder);
            } else {
                holder = (Holder)view.getTag();
            }

            holder.text.setText(mUsers.get(i));
            return view;

        }

        private class Holder{
            public TextView text;
        }

        public void updateItemName(int position, String s) {
            mUsers.set(position, s);
            notifyDataSetChanged();
        }

        public void removeItem(int position) {
            mUsers.remove(position);
            notifyDataSetChanged();
        }

        public void addItemData(String s) {
            mUsers.add(s);
            notifyDataSetChanged();
        }

        public ArrayList<String> getData() {
            return mUsers;
        }

        public void clear() {
            mUsers.clear();
            notifyDataSetChanged();
        }


    }
}
