/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.widget;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.qualcomm.qti.smartassistant.R;
import com.qualcomm.qti.smartassistant.asr.CandidateInfo;
import com.qualcomm.qti.smartassistant.service.device.SmsSender;
import com.qualcomm.qti.smartassistant.utils.LogUtils;

import java.util.ArrayList;
import java.util.List;

public class ContactSelectWindow extends PopupWindow {

    private static final String TAG = "ContactSelectWindow";
    private List<CandidateInfo> mCandidateInfos = new ArrayList<>();
    private View mContentView;
    private ContactAdapter mContactAdapter;
    private final String mMessage;
    private final Context mContext;

    public ContactSelectWindow(Activity context, List<CandidateInfo> candidateInfos,
                               String  message) {
        super(context);
        mContext = context;
        mMessage = message;
        mCandidateInfos.addAll(candidateInfos);
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mContentView = inflater.inflate(R.layout.select_contact_pop_layout, null);
        setContentView(mContentView);
        initListView(mContentView, context);
        setWidth(ViewGroup.LayoutParams.MATCH_PARENT);
        setHeight(ViewGroup.LayoutParams.MATCH_PARENT);
        setFocusable(true);
        setAnimationStyle(R.style.contact_select_popuwindow);
        ColorDrawable dw = new ColorDrawable(0xb0000000);
        setBackgroundDrawable(dw);
        mContentView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
            }
        });
    }

    public void showWindowOrExecuteCommand(View parent){
        if (mCandidateInfos.size() > 1
                || (mCandidateInfos.size() == 1 && mCandidateInfos.get(0).getPhones().size() > 1)){
            showAtLocation(parent,Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL, 0, 0);
        }else if (mCandidateInfos.size() == 1 && mCandidateInfos.get(0).getPhones().size() == 1){
            CandidateInfo.Phone phone = mCandidateInfos.get(0).getPhones().get(0);
            if (!TextUtils.isEmpty(mMessage)){
                sendSMS(phone.getNumber(), mMessage);
            }else {
                phoneCallByNumber(phone.getNumber());
            }
        }
    }

    private void initListView(View contentView, Context context){
        ListView listView = contentView.findViewById(R.id.contactList);
        mContactAdapter = new ContactAdapter(context, mCandidateInfos);
        listView.setAdapter(mContactAdapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                CandidateInfo.Phone phone = (CandidateInfo.Phone) mContactAdapter.getItem(position);
                LogUtils.d(TAG, "onItemClick phone = "+phone.getNumber());
                if (!TextUtils.isEmpty(mMessage)){
                    sendSMS(phone.getNumber(), mMessage);
                }else {
                    phoneCallByNumber(phone.getNumber());
                }
                dismiss();
            }
        });
    }

    private void sendSMS(String phoneNumber, String message){
        SmsSender smsSender = new SmsSender(mContext);
        smsSender.sendMessage(phoneNumber, message);
    }

    private void phoneCallByNumber(String phoneNumber){
        Uri uri = Uri.parse("tel:"+phoneNumber);
        Intent intent = new Intent(Intent.ACTION_CALL, uri);
        mContext.startActivity(intent);
    }

    public class ContactAdapter extends BaseAdapter {
        private static final int TYPE_HEADER = 0;
        private static final int TYPE_ITEM = 1;
        private List<CandidateInfo> mList;
        private LayoutInflater inflater;

        public ContactAdapter(Context context, List<CandidateInfo> list) {
            mList = list;
            inflater = LayoutInflater.from(context);
        }

        @Override
        public int getCount() {
            int count = 0;
            if (mList != null) {
                for (CandidateInfo candidateInfo : mList) {
                    count += candidateInfo.size();
                }
            }
            return count;
        }

        @Override
        public Object getItem(int position) {
            int head = 0;
            for (CandidateInfo candidateInfo : mList) {
                int size = candidateInfo.size();
                int current = position - head;
                if (current < size) {
                    return candidateInfo.getItem(current);
                }
                head += size;
            }
            return null;
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.contact_layout, parent, false);
                viewHolder.contactName = (TextView) convertView.findViewById(R.id.contactName);
                viewHolder.phoneNumber = (TextView) convertView.findViewById(R.id.phoneNumber);
                viewHolder.phoneType = (TextView) convertView.findViewById(R.id.phoneType);
                viewHolder.contact_phone_info_layout = (RelativeLayout) convertView.
                        findViewById(R.id.contact_phone_info_layout);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            switch (getItemViewType(position)) {
                case TYPE_HEADER:
                    viewHolder.contactName.setText((CharSequence) getItem(position));
                    viewHolder.contact_phone_info_layout.setVisibility(View.GONE);
                    break;
                case TYPE_ITEM:
                    viewHolder.contact_phone_info_layout.setVisibility(View.VISIBLE);
                    CandidateInfo.Phone phone = (CandidateInfo.Phone) getItem(position);
                    viewHolder.phoneNumber.setText(phone.getNumber());
                    viewHolder.phoneType.setText(phone.getTypeLabel());
                    break;
            }

            return convertView;
        }

        @Override
        public int getViewTypeCount() {
            return 2;
        }

        @Override
        public int getItemViewType(int position) {
            int head = 0;
            for (CandidateInfo candidateInfo : mList) {
                int size = candidateInfo.size();
                int current = position - head;
                if (current == 0) {
                    return TYPE_HEADER;
                }
                head += size;
            }
            return TYPE_ITEM;
        }

        @Override
        public boolean isEnabled(int position) {
            return getItemViewType(position) != TYPE_HEADER;
        }

        @Override
        public boolean areAllItemsEnabled() {
            return false;
        }
        private class ViewHolder {
            TextView contactName;
            TextView phoneNumber;
            TextView phoneType;
            RelativeLayout contact_phone_info_layout;
        }
    }
}
