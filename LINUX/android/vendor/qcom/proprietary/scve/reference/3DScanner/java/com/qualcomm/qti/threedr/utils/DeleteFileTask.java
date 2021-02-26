/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.os.AsyncTask;

import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;

import java.io.File;
import java.util.List;

public class DeleteFileTask extends AsyncTask<DeleteFileTask.Param, Integer, Void> {
    public interface Listener {
        void onPreExecute();

        void onProgressUpdate(int numDeleted);

        void onResult();
    }

    public static class Param {
        List<File> selectedFolderList;

        public Param(List<File> selectedFolderList) {
            this.selectedFolderList = selectedFolderList;
        }
    }

    Listener listener;

    public DeleteFileTask(Listener listener) {
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
    protected void onPostExecute(Void v) {
        if (listener != null)
            listener.onResult();
    }

    @Override
    protected Void doInBackground(Param... params) {
        List<File> listOfFolders = params[0].selectedFolderList;

        if (listOfFolders != null) {
            for (int i = 0; i < listOfFolders.size(); i++) {
                publishProgress(i + 1);
                BrowserUtils.delete(listOfFolders.get(i));
            }
        }
        return null;
    }


}
