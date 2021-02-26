/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.qti.csm.R;
import com.qti.csm.encryption.system.SystemInfo;

import java.io.File;
import java.util.ArrayList;

public class FileExplorerAdapter extends BaseAdapter {
    private ArrayList<File> mFileList;
    private LayoutInflater mLayoutInflater;
    private Activity mActivity;
    private Bitmap mFileIcon;
    private Bitmap mFolderIcon;

    public FileExplorerAdapter(Activity activity, ArrayList<File> fileList) {
        mFileList = fileList;
        mLayoutInflater = LayoutInflater.from(activity);
        mActivity = activity;
        mFolderIcon = BitmapFactory.decodeResource(activity.getResources(),
                R.drawable.directory);
        mFileIcon = BitmapFactory.decodeResource(activity.getResources(),
                R.drawable.listfile);
    }

    @Override
    public Object getItem(int index) {
        return mFileList.get(index);
    }

    @Override
    public int getCount() {
        return mFileList.size();
    }

    @Override
    public long getItemId(int id) {
        return id;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewData viewData = null;
        if (convertView == null) {
            convertView = mLayoutInflater
                    .inflate(R.layout.file_item_view, null);
            viewData = new ViewData();
            viewData.fileIcon = (ImageView) convertView
                    .findViewById(R.id.file_icon);
            viewData.lockIcon = (ImageView) convertView
                    .findViewById(R.id.lock_icon);
            viewData.fileName = (TextView) convertView
                    .findViewById(R.id.file_name);
            convertView.setTag(viewData);
        } else {
            viewData = (ViewData) convertView.getTag();
        }

        File file = mFileList.get(position);
        if (file.isFile()) {
            viewData.fileIcon.setImageBitmap(mFileIcon);
            if (file.isFile()
                    && file.getName().endsWith(SystemInfo.ENCRYPT_FILE_SUFFIX)) {
                viewData.lockIcon.setVisibility(View.VISIBLE);
            } else {
                viewData.lockIcon.setVisibility(View.INVISIBLE);
            }
        } else {
            viewData.fileIcon.setImageBitmap(mFolderIcon);
            viewData.lockIcon.setVisibility(View.INVISIBLE);
        }
        viewData.fileName.setText(file.getName());
        return convertView;
    }

    private class ViewData {
        ImageView fileIcon;
        ImageView lockIcon;
        TextView fileName;
    }

}
