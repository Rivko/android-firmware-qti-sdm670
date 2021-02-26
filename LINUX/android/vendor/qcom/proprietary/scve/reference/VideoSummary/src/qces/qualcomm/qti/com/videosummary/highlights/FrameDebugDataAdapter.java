/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.highlights;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoDebug.FrameDebugData;

/**
 * Created by matthewf on 8/3/15.
 */
public class FrameDebugDataAdapter extends ArrayAdapter<FrameDebugData> {

    private Context mContext;

    public FrameDebugDataAdapter(Context context, ArrayList<FrameDebugData> frameData) {
        super(context, 0, frameData);
        mContext = context;
    }

    // create a new ImageView for each item referenced by the Adapter
    public View getView(int position, View convertView, ViewGroup parent) {

        FrameDebugData frameData = getItem(position);

        ImageView imView;

        if (convertView == null) {
            // if it's not recycled, initialize some attributes
            convertView = LayoutInflater.from(mContext).inflate(R.layout.frame_info_layout,parent,false);
        }

        imView = (ImageView) convertView.findViewById(R.id.frame_imageView);
        imView.setImageBitmap(frameData.bmp);

        TextView tv1 = (TextView) convertView.findViewById(R.id.text_view1);
        tv1.setText("Frame: " + Integer.toString(frameData.frame_number));

        TextView tv2 = (TextView) convertView.findViewById(R.id.text_view2);
        tv2.setText(frameData.scores[0] + "," + frameData.scores[1] + "," + frameData.scores[2]);

        return convertView;

    }

}
