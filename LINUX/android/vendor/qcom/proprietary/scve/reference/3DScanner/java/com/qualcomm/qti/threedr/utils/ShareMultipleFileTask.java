/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;

import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by mithun on 4/18/16.
 */
public class ShareMultipleFileTask extends AsyncTask<ShareMultipleFileTask.Param, Integer, ArrayList<Uri>> {
    private static final String TAG = ShareMultipleFileTask.class.getSimpleName();

    public interface Listener {
        void onPreExecute();

        void onProgressUpdate(int numDeleted);

        void onResult(ArrayList<Uri> uris);
    }

    public static class Param {
        List<File> selectedFolderList;
        FilenameFilter filter;

        public Param(List<File> selectedFolderList, FilenameFilter filter) {
            this.selectedFolderList = selectedFolderList;
            this.filter = filter;
        }
    }

    Listener listener;

    public ShareMultipleFileTask(Listener listener) {
        this.listener = listener;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        if (listener != null) {
            listener.onPreExecute();
        }
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
        super.onProgressUpdate(values);
        if (listener != null) {
            listener.onProgressUpdate(values[0]);
        }
    }

    @Override
    protected void onPostExecute(ArrayList<Uri> uris) {
        if (listener != null)
            listener.onResult(uris);
    }

    @Override
    protected ArrayList<Uri> doInBackground(Param... params) {
        List<File> listOfFolders = params[0].selectedFolderList;
        FilenameFilter filter = params[0].filter;

        ArrayList<Uri> uris = new ArrayList<Uri>();
        if (listOfFolders != null) {
            int count = 1;
            for (File file : listOfFolders) {
                publishProgress(count++);
                String zippedFileName = BrowserUtils.zipModel(file, filter);
                File zipFile = new File(zippedFileName);
                if (zipFile.exists()) {
                    uris.add(Uri.fromFile(zipFile));
                    Log.e(TAG, "zipFile = " + zipFile.getAbsolutePath());
                }
            }
        }
        return uris;
    }
}