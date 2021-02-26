/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import java.util.ArrayList;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.sva.R;

public class VoiceRequestListAdapter extends BaseAdapter {
    private final static String TAG = "ListenLog.VoiceRequestListAdapter";
    private Context context;
    private ArrayList<VoiceRequestItem> voiceRequestsInList;

    public VoiceRequestListAdapter(Context context,
            ArrayList<VoiceRequestItem> inVoiceRequestsList) {
        this.context = context;
        this.voiceRequestsInList = inVoiceRequestsList;
    }

    @Override
    public int getCount() {
        return voiceRequestsInList.size();
    }

    @Override
    public Object getItem(int position) {
        return voiceRequestsInList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    /*@Override
    public void onClick(View v) {
        return;
    }*/

    /*@Override
    public void OnClickListener(new OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
            return;
        }
    });*/

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Log.v(TAG, "getView: entered");
        RowViewHolder rowViewHolder;

        if (null == convertView) {
            LayoutInflater inflater = (LayoutInflater)context.getSystemService(
                    Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.listitem_voicerequest, null);

            rowViewHolder = new RowViewHolder();
            convertView.setTag(rowViewHolder);

            rowViewHolder.uiSmName = (TextView)
                    convertView.findViewById(R.id.listitem_voicerequest_smname);
            rowViewHolder.uiRecordTime = (TextView)
                    convertView.findViewById(R.id.listitem_voicerequest_recordtime);

            View uiListItemLayout = convertView.findViewById(R.id.listitem_voicerequest_toplayout);

            uiListItemLayout.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    RowViewHolder currentRowViewHolder = (RowViewHolder)v.getTag();
                    showSmSelectionLimitToast(currentRowViewHolder.item.getSoundModelName());
                }
            });

        } else {
            rowViewHolder = (RowViewHolder)convertView.getTag();
        }

        rowViewHolder.item = voiceRequestsInList.get(position);
        rowViewHolder.uiSmName.setText(rowViewHolder.item.getSoundModelName());
        rowViewHolder.uiRecordTime.setText(rowViewHolder.item.getRecordTime());
        return convertView;
    }

    public void showSmSelectionLimitToast(String inSmName) {
        Toast.makeText(this.context, inSmName, Toast.LENGTH_LONG).show();
    }

    public interface OnItemActionListener {
        public void onOptionClicked(View v, VoiceRequestItem item);
    }

    private class RowViewHolder {
        public TextView uiSmName;
        public TextView uiRecordTime;
        public VoiceRequestItem item;
    }

    public static class VoiceRequestItem implements Comparable<VoiceRequestItem> {
        private String soundModelName;
        private String recordTime;

        public VoiceRequestItem(String inSoundModelName, String inTime) {
            this.soundModelName = inSoundModelName;
            this.recordTime = inTime;
        }

        public String getSoundModelName() {
            return soundModelName;
        }

        public String getRecordTime() {
            return recordTime;
        }

        //TODO sort by time
        @Override
        public int compareTo(VoiceRequestItem another) {
            return this.getSoundModelName().compareToIgnoreCase(another.getSoundModelName());
        }
    }
}

