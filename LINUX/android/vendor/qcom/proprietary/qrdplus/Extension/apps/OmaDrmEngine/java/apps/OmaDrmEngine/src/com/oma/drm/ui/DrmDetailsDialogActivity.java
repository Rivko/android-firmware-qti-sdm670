/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm.ui;

import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.oma.drm.DrmMediaInterface;
import com.oma.drm.OmaDrmInfo;
import com.oma.drm.R;
import com.oma.drm.util.OmaDrmUtils;

public class DrmDetailsDialogActivity extends Activity implements
        DialogInterface.OnCancelListener {

    private static final String TAG = "OmaDrm:DrmDetailsDiaAct";

    public static final String KEY_DRM_INFO = "drm_info";

    private OmaDrmInfo mOmaDrminfo;

    DrmMediaInterface mDrmMedia;
    ArrayList<String> mInfoArray = new ArrayList<String>();
    Uri mUri;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();
        if (intent == null) {
            Log.d(TAG, "Intent is null finish DrmDetailsDialogActivity");
            finish();
        }
        mUri = intent.getData();
        mOmaDrminfo = (OmaDrmInfo) intent.getParcelableExtra(KEY_DRM_INFO);
        if (!initialize()) {
            finish();
            return;
        }
        show();
    }

    private boolean initialize() {
        mOmaDrminfo = OmaDrmUtils.getOmaDrmInfo(this, mUri);
        if (mOmaDrminfo == null) {
            Log.e(TAG, "No info found : " + mUri);
            return false;
        }
        String path = OmaDrmUtils.getFilePath(this, mUri);
        mInfoArray.addAll(mOmaDrminfo.getInfoAsList(this));
        mInfoArray.add(getResources().getString(R.string.drm_text_title)
                + OmaDrmUtils.getFileName(path));
        mInfoArray.add(getResources().getString(R.string.drm_text_location)
                + path);

        if (!mOmaDrminfo.rightsPresent()) {
            Log.d(TAG, "Rights not valid finish DrmDetailsDialogActivity");
            finish();
        }

        if (OmaDrmUtils.DEBUG) {
            if (mOmaDrminfo != null) {
                dump();
            }
        }
        return true;
    }

    public void show() {
        ListView detailsList = (ListView) LayoutInflater.from(this).inflate(
                R.layout.details_list, null, false);
        detailsList.setDivider(null);
        detailsList.setAdapter(new ArrayAdapter<>(this, R.layout.details,
                mInfoArray));

        Dialog mDialog = new AlertDialog.Builder(this)
                .setIconAttribute(android.R.attr.alertDialogIcon)
                .setView(detailsList)
                .setTitle(R.string.drm_details_title)
                .setCancelable(true)
                .setOnCancelListener(this)
                .setPositiveButton(R.string.dialog_close,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog,
                                    int whichButton) {
                                onCancel(dialog);
                            }
                        }).show();
    }

    @Override
    public void onCancel(DialogInterface dialog) {
        finish();
    }

    private void dump() {
        Log.d(TAG, "MimeType >>> " + mOmaDrminfo.mMimeType);
        Log.d(TAG, "File size >>> " + mOmaDrminfo.mFileSize);
        Log.d(TAG, "Uid >>> " + mOmaDrminfo.mUid);
        Log.d(TAG, "TYPE >>> " + mOmaDrminfo.mPermission.mType);
        Log.d(TAG, "Permission count >>> " + mOmaDrminfo.mPermission.mCount);
        Log.d(TAG, "Permission enddate >>> " + mOmaDrminfo.mPermission.mEnddate);
        Log.d(TAG, "Permission startdate >>> "
                + mOmaDrminfo.mPermission.mStartDate);
        Log.d(TAG, "Permission interval >>> "
                + mOmaDrminfo.mPermission.mInterval);
    }
}
