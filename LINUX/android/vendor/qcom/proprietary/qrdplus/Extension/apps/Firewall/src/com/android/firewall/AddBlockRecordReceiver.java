/*
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.firewall;

import android.content.BroadcastReceiver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.android.firewall.FirewallProvider;
import com.android.firewall.BlockRecord;
import com.suntek.mway.rcs.client.aidl.constant.Parameter;

public class AddBlockRecordReceiver extends BroadcastReceiver {

    private static final String TAG = "AddBlockRecordReceiver";

    public static final String ADD_CALL_BLOCK_RECORD = "com.android.firewall.ADD_CALL_BLOCK_RECORD";
    public static final String ADD_RCS_MESSAGE_BLOCK_RECORD = "com.suntek.mway.rcs.app.service.ACTION_MESSAGE_FIREWALL_BLOCK_RECORD";
    public static final String ADD_SMS_BLOCK_RECORD = "com.android.firewall.ADD_SMS_BLOCK_RECORD";
    public static final String ADD_MMS_BLOCK_RECORD = "com.android.firewall.ADD_MMS_BLOCK_RECORD";
    public static final String BLOCK_NUMBER = "number";
    public static final String MMS_BLOCK_CONTENT = "content";
    private static final int MSG_NUMBER_LEN= 11;

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, "Action = " + intent.getAction());
        if (ADD_CALL_BLOCK_RECORD.equals(intent.getAction())) {
            Log.i(TAG, "Action CALL");
            String number = intent.getStringExtra(BLOCK_NUMBER);
            long date = intent.getLongExtra(BlockRecord.DATE, System.currentTimeMillis());
            int sub_id =intent.getIntExtra(BlockRecord.SUB_ID, -1);
            ContentValues values =new ContentValues();
            values.put(BlockRecord.CONTACT, number);
            values.put(BlockRecord.BLOCK_TYPE, BlockRecord.BLOCK_TYPE_CALL);
            values.put(BlockRecord.DATE, date);
            values.put(BlockRecord.SUB_ID, sub_id);
            context.getContentResolver().insert(BlockRecord.CONTENT_URI, values);
        } else if (ADD_RCS_MESSAGE_BLOCK_RECORD.equals(intent.getAction())) {
            Log.i(TAG, "Action RCS_MESSAGE ");
            Long msgID =intent.getLongExtra(Parameter.EXTRA_ID,-1);
            String number = intent.getStringExtra(Parameter.EXTRA_NUMBER);
            if (number.indexOf("+86") != -1) {
                number = number.substring(3);
            }
            String content = intent.getStringExtra(Parameter.EXTRA_CONTENT);
            int msgtype = intent.getIntExtra(Parameter.EXTRA_MESSAGE_TYPE, -1);
            int burn = intent.getIntExtra(Parameter.EXTRA_BARCYCLE, -1);

            ContentValues values = new ContentValues();
            values.put(BlockRecord.CONTACT, number);
            values.put(BlockRecord.SUB_ID, msgID);
            values.put(BlockRecord.BLOCK_TYPE, BlockRecord.BLOCK_TYPE_RCS_MESSAGE);
            values.put(BlockRecord.DATA, content);
            values.put(BlockRecord.MSG_TYPE, msgtype);
            values.put(BlockRecord.BURN, burn);
            context.getContentResolver().insert(BlockRecord.CONTENT_URI, values);
        } else if (ADD_SMS_BLOCK_RECORD.equals(intent.getAction())) {
            Log.i(TAG, "Action SMS ");
            String number = intent.getStringExtra(BLOCK_NUMBER);
            long date = intent.getLongExtra(BlockRecord.DATE, System.currentTimeMillis());
            String content = intent.getStringExtra(MMS_BLOCK_CONTENT);
            ContentValues values = new ContentValues();
            values.put(BlockRecord.CONTACT, number);
            values.put(BlockRecord.DATE, date);
            values.put(BlockRecord.BLOCK_TYPE, BlockRecord.BLOCK_TYPE_SMS);
            values.put(BlockRecord.DATA, content);
            context.getContentResolver().insert(BlockRecord.CONTENT_URI, values);

        } else if (ADD_MMS_BLOCK_RECORD.equals(intent.getAction())) {
            Log.i(TAG, "Action MMS ");
            String number = intent.getStringExtra(BLOCK_NUMBER);
            long date = intent.getLongExtra(BlockRecord.DATE, System.currentTimeMillis());
            byte[] data = intent.getByteArrayExtra(BlockRecord.DATA);
            long subId = intent.getLongExtra(BlockRecord.SUB_ID, -1);
            ContentValues values = new ContentValues();
            values.put(BlockRecord.BLOCK_TYPE, BlockRecord.BLOCK_TYPE_MMS);
            values.put(BlockRecord.CONTACT, number);
            values.put(BlockRecord.DATE, date);
            context.getContentResolver().insert(BlockRecord.CONTENT_URI, values);
        }

    }
}
