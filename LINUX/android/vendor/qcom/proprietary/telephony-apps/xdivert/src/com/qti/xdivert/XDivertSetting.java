/**
 * Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qti.xdivert;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.UserHandle;
import android.preference.PreferenceScreen;
import android.telephony.TelephonyManager;
import android.telephony.PhoneNumberUtils;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import android.view.MenuItem;
import java.util.List;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;

// SIM records have msisdn.Hence directly process XDivert feature

public class XDivertSetting extends TimeConsumingPreferenceActivity {
    private static final String LOG_TAG = "XDivertSetting";

    private static final String BUTTON_XDIVERT = "xdivert_checkbox";

    private static final int EVENT_PROCESS_XDIVERT_CHECK_BOX = 1;

    private XDivertCheckBoxPreference mXDivertButton;
    private static XDivertUtility sXDivertUtility;
    public static final int LINE_NUMBERS = 1;
    private int mNumPhones;
    String[] mLine1Numbers;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        // Make sure we are running as the primary user only
        if (UserHandle.myUserId() != UserHandle.USER_OWNER) {
            Toast.makeText(this, R.string.xdivert_setting_primary_user_only,
                Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        sXDivertUtility = XDivertUtility.init(getApplicationContext());

        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED);
        filter.addAction(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED);
        registerReceiver(sXDivertUtility, filter);

        addPreferencesFromResource(R.xml.xdivert);
        mNumPhones = TelephonyManager.getDefault().getPhoneCount();
        mLine1Numbers = new String[mNumPhones];
        processXDivert();
   }

   @Override
   protected void onDestroy() {
      super.onDestroy();
      unregisterReceiver(sXDivertUtility);
   }



    private boolean isSlotActive(int slotId) {
        boolean slotActive = false;
	SubscriptionManager mgr = SubscriptionManager.from(XDivertUtility.getContext());
        List<SubscriptionInfo> activeSubList = mgr.getActiveSubscriptionInfoList();
        if (activeSubList != null) {
            for (SubscriptionInfo subInfoRecord : activeSubList) {
                if (subInfoRecord.getSimSlotIndex() == slotId) {
                    slotActive = true;
                     break;
                }
            }
        }
        return slotActive;
    }

    private boolean isAllSubActive() {
        for (int i = 0; i < mNumPhones; i++) {
            if (!isSlotActive(i)) return false;
        }
        return true;
    }

    private boolean isAnySubCdma() {
        for (int i = 0; i < mNumPhones; i++) {
            Phone phone = PhoneFactory.getPhone(i);
            if (phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) return true;
        }
        return false;
    }

    private boolean isValidLine1Number(String[] line1Numbers) {
        for (int i = 0; i < mNumPhones; i++) {
            if (TextUtils.isEmpty(line1Numbers[i])) return false;
        }
        return true;
    }

    private void processXDivert() {
        String[] line1Numbers = new String[mNumPhones];
        for (int i = 0; i < mNumPhones; i++) {
            Phone phone = PhoneFactory.getPhone(i);
            String msisdn = phone.getLine1Number();  // may be null or empty
            if (!TextUtils.isEmpty(msisdn)) {
                //Populate the line1Numbers only if it is not null
               line1Numbers[i] = PhoneNumberUtils.formatNumber(msisdn);
            }

            Log.d(LOG_TAG, "SUB:" + i + " phonetype = " + phone.getPhoneType()
                    + " isSlotActive = " + isSlotActive(i)
                    + " line1Number = " + line1Numbers[i]);
        }
        if (!isAllSubActive()) {
            //Is a subscription is deactived/or only one SIM is present,
            //dialog would be displayed stating the same.
            displayAlertDialog(R.string.xdivert_sub_absent);
        } else if (isAnySubCdma()) {
            //X-Divert is not supported for CDMA phone.Hence for C+G / C+C,
            //dialog would be displayed stating the same.
            displayAlertDialog(R.string.xdivert_not_supported);
        } else if (!isValidLine1Number(line1Numbers)) {
            //SIM records does not have msisdn, hence ask user to enter
            //the phone numbers.
            Intent intent = new Intent();
            intent.setClass(this, XDivertPhoneNumbers.class);
            startActivityForResult(intent, LINE_NUMBERS);
        } else {
            //SIM records have msisdn.Hence directly process
            //XDivert feature
            mLine1Numbers = line1Numbers;
            mHandler.sendMessage(mHandler.obtainMessage(EVENT_PROCESS_XDIVERT_CHECK_BOX));
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
         Log.d(LOG_TAG, "requestCode: "+ requestCode + " resultCode: " + resultCode);
        if (requestCode == LINE_NUMBERS) {
            if (resultCode == RESULT_OK) {
                mLine1Numbers  = data.getStringArrayExtra(XDivertUtility.LINE1_NUMBERS);
                Log.d(LOG_TAG, "numbers: "+ mLine1Numbers);
                mHandler.sendMessage(mHandler.obtainMessage(EVENT_PROCESS_XDIVERT_CHECK_BOX));
            } else {
                finish();
            }
        }
    }

    Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_PROCESS_XDIVERT_CHECK_BOX:
                 Log.d(LOG_TAG, "EVENT_PROCESS_XDIVERT_CHECK_BOX: "+ mLine1Numbers);
                processXDivertCheckBox(mLine1Numbers);
                break;
            }
        }
    };
    private void displayAlertDialog(int resId) {
        new AlertDialog.Builder(this).setMessage(resId)
            .setTitle(R.string.xdivert_title)
            .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        Log.d(LOG_TAG, "X-Divert onClick");
                        finish();
                    }
                })
            .show()
            .setOnDismissListener(new DialogInterface.OnDismissListener() {
                    public void onDismiss(DialogInterface dialog) {
                        Log.d(LOG_TAG, "X-Divert onDismiss");
                        finish();
                    }
            });
    }

    private void processXDivertCheckBox(String[] line1Numbers) {
        Log.d(LOG_TAG,"processXDivertCheckBox line1Numbers = "
                + java.util.Arrays.toString(line1Numbers));
        PreferenceScreen prefSet = getPreferenceScreen();
        mXDivertButton = (XDivertCheckBoxPreference) prefSet.findPreference(BUTTON_XDIVERT);
        mXDivertButton.init(this, false, line1Numbers);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (android.R.id.home == item.getItemId()) {  // See ActionBar#setDisplayHomeAsUpEnabled()
            onBackPressed();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

}
