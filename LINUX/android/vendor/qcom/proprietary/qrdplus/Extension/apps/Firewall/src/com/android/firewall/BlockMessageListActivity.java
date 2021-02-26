/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.CursorAdapter;
import android.widget.ListView;
import android.widget.PopupMenu;
import android.widget.TextView;
import android.widget.Toast;

import com.android.firewall.BlockRecord;
import com.suntek.mway.rcs.client.aidl.constant.Constants;

import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.List;


public class BlockMessageListActivity extends Activity{
    public static final String NUMBER_KEY = "number_key";
    private static final int CALL_MENU = 0;
    private static final int MESSAGE_MENU = 1;
    private static final int DELETE_MENU = 2;
    private static final int BLOCK_TYPE_CALL = 1;
    private static final int BLOCK_TYPE_SMS = 2;
    private static final int BLOCK_TYPE_MMS = 3;
    private ListView smsBlockRecordListview;
    private TextView emptySmsText;
    private Context mContext;
    private Cursor smsCursor;
    private long selectId = -1;
    private String number;
    public static final int RCS_NOT_A_BURN_MESSAGE = 0;

    private void showMenu(View view) {
        PopupMenu popup = new PopupMenu(BlockMessageListActivity.this, view);
        Menu menu = popup.getMenu();

        menu.add(0, CALL_MENU, 0,
                R.string.block_record_call);
        menu.add(0, MESSAGE_MENU, 0,
                R.string.block_record_message);
        menu.add(0, DELETE_MENU, 0,
                R.string.block_record_delete);
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                onContextItemSelected(item);
                return true;
            }
        });
        popup.show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.sms_block_list);
        number = getIntent().getStringExtra(NUMBER_KEY);
        mContext = this;
        if (TextUtils.isEmpty(number)) {
            Toast.makeText(this, R.string.number_is_empty, Toast.LENGTH_LONG).show();
            finish();
        }
        init();
    }

    public void init() {

        smsBlockRecordListview = (ListView)findViewById(R.id.smsBlockRecordListview);
        emptySmsText = (TextView) findViewById(R.id.sms_record_is_empty);
        initListView();
    }

    private void initListView() {
        if(number.startsWith("+") || number.startsWith("00")){
            int len = number.length();
            if (len > 11) {
                number = number.substring(len - 11, len);
            }
        }
        smsCursor = getContentResolver().query(
                BlockRecord.CONTENT_URI, new String[]{
                        "_id","block_type","contact","date","data","msg_type","sub_id","burn"
                    },
                " block_type > "+"0" + " AND " + "contact" + " LIKE '%" + number + "'",null, null);

        smsBlockRecordListview.setEmptyView(emptySmsText);

        SmsBlockListCursorAdapter smsAdapter = new SmsBlockListCursorAdapter(
                this, smsCursor, true);

        smsBlockRecordListview.setAdapter(smsAdapter);
        final List<String> menuList = Arrays
                .asList(BlockMessageListActivity.this.getResources().getStringArray(
                        R.array.block_record_menu_array));
        smsBlockRecordListview.setOnItemClickListener(new OnItemClickListener() {

                    @Override
                    public void onItemClick(AdapterView<?> arg0, View view, int arg2, long arg3) {
                        smsCursor.moveToPosition(arg2);
                        selectId = smsCursor.getLong(smsCursor
                                .getColumnIndexOrThrow(BlockRecord._ID));
                        showMenu(view);
                    }
                });
    }

    class SmsBlockListCursorAdapter extends CursorAdapter {
        LayoutInflater mInflater;
        private int indexOfNumber, indexOfMsg, indexOfTime, indexOfMsgType, indexOfBurn,
                indexOfBlockType;

        public SmsBlockListCursorAdapter(Context context, Cursor c,
                boolean autoRequery) {
            super(context, c, autoRequery);

            mInflater = (LayoutInflater) context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            getColumnIndices(c);
            c.setNotificationUri(getContentResolver(),
                    BlockRecord.CONTENT_URI);
        }

        private void getColumnIndices(Cursor cursor) {
            if (cursor != null) {
                indexOfNumber = cursor.getColumnIndexOrThrow(BlockRecord.CONTACT);
                indexOfMsg = cursor.getColumnIndexOrThrow(BlockRecord.DATA);
                indexOfTime = cursor.getColumnIndexOrThrow(BlockRecord.DATE);
                indexOfMsgType = cursor.getColumnIndexOrThrow(BlockRecord.MSG_TYPE);
                indexOfBurn = cursor.getColumnIndexOrThrow(BlockRecord.BURN);
                indexOfBlockType = cursor.getColumnIndexOrThrow(BlockRecord.BLOCK_TYPE);
            }
        }

        @Override
        public int getCount() {
            if (smsCursor.getCount() != 0
                    && smsBlockRecordListview.getVisibility() == View.GONE) {
                smsBlockRecordListview.setVisibility(View.VISIBLE);
                emptySmsText.setVisibility(View.GONE);

            } else if (smsBlockRecordListview.getVisibility() == View.VISIBLE
                    && smsCursor.getCount() == 0) {
                smsBlockRecordListview.setVisibility(View.GONE);
                emptySmsText.setVisibility(View.VISIBLE);

            }
            return super.getCount();
        }

        @Override
        public void bindView(View arg0, Context arg1, Cursor arg2) {

            TextView sms_msg = (TextView) arg0.findViewById(R.id.msg);
            TextView time = (TextView) arg0.findViewById(R.id.time);

            String sms_msgStr = arg2.getString(indexOfMsg);
            String timeStr = arg2.getString(indexOfTime);
            int rcs_msg_type = arg2.getInt(indexOfMsgType);
            int burn_number = arg2.getInt(indexOfBurn);
            int block_type = arg2.getInt(indexOfBlockType);
            if (block_type == BlockRecord.BLOCK_TYPE_MMS) {
                sms_msg.setText(R.string.mms_string);
            } else if (burn_number > 0) {
                sms_msg.setText(R.string.msg_type_burnMessage);
            } else {
                switch (rcs_msg_type) {
                    case Constants.MessageConstants.CONST_MESSAGE_IMAGE:
                        sms_msg.setText(R.string.msg_type_image);
                        break;
                    case Constants.MessageConstants.CONST_MESSAGE_AUDIO:
                        sms_msg.setText(R.string.msg_type_audio);
                        break;
                    case Constants.MessageConstants.CONST_MESSAGE_VIDEO:
                        sms_msg.setText(R.string.msg_type_video);
                        break;
                    case Constants.MessageConstants.CONST_MESSAGE_MAP:
                        sms_msg.setText(R.string.msg_type_location);
                        break;
                    case Constants.MessageConstants.CONST_MESSAGE_CONTACT:
                        sms_msg.setText(R.string.msg_type_contact);
                        break;
                    case Constants.MessageConstants.CONST_MESSAGE_PAID_EMOTICON:
                        sms_msg.setText(R.string.msg_type_paid_emoticon);
                        break;
                    default:
                        sms_msg.setText(sms_msgStr);
                        break;
                }
            }
            if (!TextUtils.isEmpty(timeStr)) {
                time.setText(getTimeStr(Long.parseLong(timeStr)));
            }
        }

        @Override
        public View newView(Context arg0, Cursor arg1, ViewGroup arg2) {

            return mInflater.inflate(R.layout.sms_block_list_item, arg2, false);

        }
    }

    private String getTimeStr(long time) {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        return format.format(new Date(time));
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case CALL_MENU: {
                Intent intent = new Intent(Intent.ACTION_CALL, Uri.parse("tel:" + number));
                startActivity(intent);
                break;
            }

            case MESSAGE_MENU: {
                Intent intent = new Intent(Intent.ACTION_SENDTO, Uri.parse("smsto:" + number));
                startActivity(intent);
                return true;
            }

            case DELETE_MENU:
                getContentResolver().delete(BlockRecord.CONTENT_URI, " _id = ?  ", new String[] {
                        String.valueOf(selectId)
                });
                return true;
        }
        return false;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (smsCursor != null && !smsCursor.isClosed()) {
            smsCursor.close();
        }
    }
}
