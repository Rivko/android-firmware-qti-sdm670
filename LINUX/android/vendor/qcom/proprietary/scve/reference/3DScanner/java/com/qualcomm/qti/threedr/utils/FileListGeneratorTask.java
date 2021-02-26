/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.os.AsyncTask;

import java.io.File;

public class FileListGeneratorTask extends AsyncTask<FileListGeneratorTask.Param, Void, String[]> {
    public interface Listener {
        void onResult(String[] ModelNames);
    }

    public static class Param {
        File folder;
        String extension;

        public Param(File f, String extn) {
            folder = f;
            extension = extn;
        }
    }

    ;

    Listener listener;

    public FileListGeneratorTask(Listener listener) {
        this.listener = listener;
    }

    @Override
    protected void onPostExecute(String[] modelNames) {
        listener.onResult(modelNames);
    }

    @Override
    protected String[] doInBackground(Param... params) {
        return GetListOfModelNames(params[0].folder, params[0].extension);

    }

    String[] GetListOfModelNames(File folder, String extension) {
        String[] listOfModelNames = folder.list(new FileNameExtensionFilter(extension));
        return listOfModelNames;
    }
}
