/**
 * Copyright (c) 2012-2014, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qrd.omadownload;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.pm.PackageManager;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.os.Bundle;
import android.os.SystemProperties;
import android.provider.Downloads;
import android.util.Log;
import android.widget.Toast;
import com.qrd.omadownload.OmaDownloadMonitorService;
import com.qrd.omadownload.R;
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;

public class OmaDownloadActivity extends Activity implements OnClickListener , OnCancelListener{
    private static final String LOG_TAG = "OMADL_ACTIVITY";
    public static final String ACTION_OMADL_CONFIRM = "android.intent.omadownload.ddconfirm";
    public static final String ACTION_OMADL_REDIRECT = "android.intent.omadownload.urldirect";
    public static final String EXTRA_OBJECTURL = "objecturl";
    public static final String EXTRA_OBJECTSIZE = "objectsize";
    public static final String EXTRA_OBJECTTYPE = "objecttype";
    public static final String EXTRA_OBJECTALLMIMETYPE = "objectallmimetype";
    public static final String EXTRA_OBJECTNAME = "objectname";
    public static final String EXTRA_OBJECTVENDOR = "objectvendor";
    public static final String EXTRA_OBJECTDESCRIPTION = "objectdescription";
    public static final String EXTRA_OBJECTINSTALLNOTIFYURL = "objectinstallnotifyurl";
    public static final String EXTRA_OBJECTNEXTURL = "objectnexturl";
    public static final String EXTRA_MONITOR_MM_URI = "mon_mm_uri";
    public static final String EXTRA_MONITOR_DD_URI = "mon_dd_uri";
    public static final String EXTRA_MONITOR_DD_SRC_URL = "mon_dd_url";
    private String mObjecturl = null;
    private String mObjectname = null;
    private String mObjectsize = null;
    private String mObjecttype = null;
    private String mObjectvendor = null;
    private String mObjectInstallNotifyUrl = null;

    private String mObjectdescription = null;
    private String mObjectnexturl = null;
    private DialogInterface mResultDialog = null;
    private DialogInterface mConfirmDialog = null;

    public static final String MIMETYPE_DRM_MESSAGE = "application/vnd.oma.drm.message";
    public static final String MIMETYPE_DRM_CONTENT = "application/vnd.oma.drm.content";
    public static final String EXTRA_CONTENT_URI = "mon_content_uri";
    public static final String EXTRA_DATA = "mon_data";
    private int statusReport;
    public  static boolean sIsOmaDlErrorHandlingEnabled;
    private String action;
    private String scheme;
    private static final int REQUEST_WRITE_EXTERNAL_STORAGE = 10;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sIsOmaDlErrorHandlingEnabled = SystemProperties.getBoolean(
                "persist.omadlhandleer.enable", false)
                || getResources().getBoolean(R.bool.enableOMADLErrorHandling);
    }

    @Override
    protected void onResume() {
        super.onResume();
        action = getIntent().getAction();
        scheme = getIntent().getScheme() != null ? getIntent().getScheme():"";
        grantPermissions();
    }

    private void handleDownload() {
        if (action.equals(OmaDownloadActivity.ACTION_OMADL_CONFIRM))
            handleDDConfirm((Context)this, getIntent());
        else if (action.equals(OmaDownloadActivity.ACTION_OMADL_REDIRECT))
            handleDDRedirect((Context)this, getIntent());
        else if (action.equals(Intent.ACTION_VIEW) && (scheme.equals("http")
                || scheme.equals("https")))
            handleDDDownload(this, getIntent().getData());
        else if (action.equals(Intent.ACTION_VIEW) && scheme.equals("content"))
            downloadMMFile(this, getIntent().getData());
        else
            finish();
    }

    /**
     * check for nongranted dangerous permissions and request
     */
    private void grantPermissions() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[] { Manifest.permission.WRITE_EXTERNAL_STORAGE },
                    REQUEST_WRITE_EXTERNAL_STORAGE);
        } else {
            handleDownload();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
            String[] permissions, int[] grantResults) {
        switch (requestCode) {
        case REQUEST_WRITE_EXTERNAL_STORAGE:
            if (grantResults.length > 0
                    && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                handleDownload();
            } else {
                // permission denied, finish activity
                finish();
            }
            break;
        }
    }

    public void onClick(DialogInterface dialog, int which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            if (dialog != mResultDialog) {
                handleMMDownload(this);
            } else {
                handleMMNextUrl(this);
            }
        } else if (sIsOmaDlErrorHandlingEnabled) {
            if (which == DialogInterface.BUTTON_NEGATIVE) {
                statusReport = OmaDownloadMonitorService.STATUS_USER_CANCELLED;
                if (mObjectInstallNotifyUrl != null) {
                    new Thread(new Runnable() {
                        @Override
                         public void run() {
                            OmaDownloadMonitorService
                                    .postInstallNotification(OmaDownloadActivity.this,
                                            mObjectInstallNotifyUrl, statusReport, null, null);
                        }
                    }).start();
                }
            }
        }
        finish();
    }

    public void onCancel(DialogInterface dialog) {
        finish();
    }

    private void handleDDRedirect(Context ctx, Intent intent) {
        Log.i(LOG_TAG, "handleDDRedirect");
        mObjectnexturl = (String)intent.getExtra(EXTRA_OBJECTNEXTURL);
        if (mResultDialog != null)
            mResultDialog.dismiss();
        if (mConfirmDialog != null)
            mConfirmDialog.dismiss();
        mResultDialog = new AlertDialog.Builder(ctx)
        .setTitle(R.string.download_complete)
        .setMessage(R.string.download_sucessful)
        .setPositiveButton(R.string.next_page, this)
        .setNegativeButton(R.string.cancel, this)
        .setOnCancelListener(this)
        .show();
    }

    private void handleDDConfirm(Context ctx, Intent intent) {
        Log.i(LOG_TAG, "handleDDConfirm");
        mObjecturl = (String)intent.getExtra(EXTRA_OBJECTURL);
        mObjectname = (String)intent.getExtra(EXTRA_OBJECTNAME);
        mObjectsize = (String)intent.getExtra(EXTRA_OBJECTSIZE);
        mObjecttype = (String)intent.getExtra(EXTRA_OBJECTTYPE);
        String mAllMimetype = (String)intent.getExtra(EXTRA_OBJECTALLMIMETYPE);
        mObjectvendor = (String)intent.getExtra(EXTRA_OBJECTVENDOR);
        mObjectdescription = (String)intent.getExtra(EXTRA_OBJECTDESCRIPTION);
        if (sIsOmaDlErrorHandlingEnabled)
        mObjectInstallNotifyUrl = (String)intent.getExtra(EXTRA_OBJECTINSTALLNOTIFYURL);
        String ddContent = (mObjectname != null ? (getText(R.string.filename) + mObjectname + "\n") : "")
              + (mAllMimetype != null ? (getText(R.string.filetype) + mAllMimetype + "\n") : "")
              + (mObjectsize != null ? (getText(R.string.filesize) + mObjectsize + "\n") : "")
              + (mObjectvendor != null ? (getText(R.string.filevendor) + mObjectvendor + "\n") : "")
              + (mObjectdescription != null ? (getText(R.string.description) + mObjectdescription + "\n") : "");
        if (mResultDialog != null)
            mResultDialog.dismiss();
        if (mConfirmDialog != null)
            mConfirmDialog.dismiss();
        if ((getResources().getBoolean(R.bool.enableShowOMADLConfirmDialogue))) {
            mConfirmDialog = new AlertDialog.Builder(ctx)
                    .setTitle(R.string.download_info)
                    .setMessage(ddContent)
                    .setPositiveButton(R.string.download, this)
                    .setNegativeButton(R.string.cancel, this)
                    .setOnCancelListener(this).show();
        } else {
            if (mConfirmDialog != null) {
                mConfirmDialog.dismiss();
            }
            handleMMDownload(ctx);
            Toast.makeText(this, R.string.downloading, Toast.LENGTH_SHORT)
                    .show();
        }
    }

    private Uri handleDownload(Context ctx, Uri data, String filename, String mimetype) {
        Log.i(LOG_TAG, "handleDownload");
        String status = Environment.getExternalStorageState();
        String url = data.toString();
        if (!status.equals(Environment.MEDIA_MOUNTED)) {
            Toast.makeText(this, R.string.cannot_download, Toast.LENGTH_SHORT)
            .show();
            return null;
        }
        ContentValues values = new ContentValues();
        values.put(Downloads.Impl.COLUMN_NOTIFICATION_PACKAGE, getPackageName());
        values.put(Downloads.Impl.COLUMN_IS_PUBLIC_API, true);
        values.put(Downloads.Impl.COLUMN_URI, url);
        values.put(Downloads.Impl.COLUMN_VISIBILITY,
            Downloads.Impl.VISIBILITY_VISIBLE_NOTIFY_COMPLETED);
        values.put(Downloads.Impl.COLUMN_DESTINATION,
            Downloads.Impl.DESTINATION_EXTERNAL);
        if (mimetype != null)
            values.put(Downloads.Impl.COLUMN_MIME_TYPE, mimetype);
        if (filename != null)
            values.put(Downloads.Impl.COLUMN_FILE_NAME_HINT, filename);
        values.put(Downloads.Impl.COLUMN_DESCRIPTION, data.getHost());

        Log.e(LOG_TAG, "inser value: url is:" + url + "host:" + data.getHost() + "ctx:"
            + ctx + " resolver:" + ctx.getContentResolver());

        return ctx.getContentResolver().insert(Downloads.Impl.CONTENT_URI, values);
    }

    private void handleDDDownload(Context ctx, Uri data) {
        Log.i(LOG_TAG,"handleDDDownload method");
        String filename = null;
        String url = data.toString();
        if (!url.endsWith("/")) {
            int index = url.lastIndexOf('/') + 1;
            if (index > 0) {
                filename = url.substring(index);
            }
        }
        if (filename == null)
            filename = "temp_download";
        if(!filename.endsWith(".dd"))
            filename = filename + ".dd";
        Uri ddUri = handleDownload(ctx, data, filename, "application/vnd.oma.dd+xml");
        if (ddUri != null)
        {
        startService(new Intent(this, OmaDownloadMonitorService.class).putExtra(
                EXTRA_MONITOR_DD_URI, ddUri.toString()).putExtra(EXTRA_MONITOR_DD_SRC_URL,
                data.toString()));
        }
        finish();
    }

    private void handleMMDownload(Context ctx) {
        Log.i(LOG_TAG,"handleMMDownload method");
        String filename = null;
        if (mObjectname != null) {
            filename = mObjectname.replaceAll("[/\\\\, ]", "_");
        }

        if (filename == null) {
            if (!mObjecturl.endsWith("/")) {
                int index = mObjecturl.lastIndexOf('/') + 1;
                if (index > 0) {
                    filename = mObjecturl.substring(index);
                }
            }
        }
        if (filename == null)
            filename = "temp_download";
        if(filename.indexOf(".") < 0) {
            String fileExt = "dat";
            if (mObjecttype != null) {
                if (mObjecttype.equals(MIMETYPE_DRM_MESSAGE)) {
                    fileExt = "dm";
                } else if (mObjecttype.equals(MIMETYPE_DRM_CONTENT)) {
                    fileExt = "dcf";
                } else {
                    int index = mObjecttype.lastIndexOf('/') + 1;
                    if (index > 0)
                       fileExt = mObjecttype.substring(index);
                }
            }
            filename = filename + "." + fileExt;
        }
        Uri mmUri = handleDownload(ctx, Uri.parse(mObjecturl), filename,
            (mObjecttype != null ? mObjecttype : null));
        if (mmUri != null)
            startService(new Intent(this, OmaDownloadMonitorService.class).putExtra(
                EXTRA_MONITOR_MM_URI, mmUri.toString()));
        finish();
    }

    private void handleMMNextUrl(Context ctx) {
        if (mObjectnexturl != null)
            ctx.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(mObjectnexturl)));
        finish();
    }

    private void downloadMMFile(Context ctx, Uri dataUri) {
        Log.i(LOG_TAG, "downloadMMFile method");
        Cursor mmObjectMonitorCur = ctx
                .getContentResolver()
                .query(dataUri,
                        new String[] { Downloads.Impl.COLUMN_URI },
                               null, null, null);
        final int ddUriIndex = mmObjectMonitorCur
                .getColumnIndexOrThrow(Downloads.Impl.COLUMN_URI);
        String uriString = "";
        String dataString = "";
        if (mmObjectMonitorCur != null
                && mmObjectMonitorCur.moveToFirst()) {
            uriString  = mmObjectMonitorCur.getString(ddUriIndex);
            dataString = dataUri.toString();
        }
        if (uriString != null) {
            startService(new Intent(this, OmaDownloadMonitorService.class).putExtra(
                    EXTRA_CONTENT_URI, uriString).putExtra(EXTRA_DATA, dataString));
        }
        finish();
    }

}
