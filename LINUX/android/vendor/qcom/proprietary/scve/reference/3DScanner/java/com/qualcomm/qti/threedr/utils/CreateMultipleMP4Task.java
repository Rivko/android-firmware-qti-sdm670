/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.os.AsyncTask;

import com.qualcomm.qti.threedr.MeshViewer.MeshViewerUtils;
import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;
import com.qualcomm.qti.threedr.modelbrowser.ModelInfo;

import java.util.ArrayList;

/**
 * Created by matthewf on 5/3/16.
 */
public class CreateMultipleMP4Task extends AsyncTask<CreateMultipleMP4Task.Param, Integer, Void> {

    private static final String TAG = CreateMultipleMP4Task.class.getSimpleName();

    public interface Listener {
        void onPreExecute();

        void onProgressUpdate(int progress);

        void onResult();

    }

    public static class Param {
        ArrayList<ModelInfo> models;

        public Param(ArrayList<ModelInfo> arrayListModels) {
            models = arrayListModels;
        }
    }

    Listener listener;

    public CreateMultipleMP4Task(Listener listener) {
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
    protected void onPostExecute(Void aVoid) {
        super.onPostExecute(aVoid);
        if (listener != null) {
            listener.onResult();
        }
    }

    @Override
    protected Void doInBackground(Param... params) {

        for (int i = 0; i < params[0].models.size(); i++)
        {
            MeshViewerUtils.setupGLandMP4forMeshModel(params[0].models.get(i),
                    false,
                    null);

            for (int j = 0; j < MeshViewerUtils.TOTAL_FRAMES; j++) {
                MeshViewerUtils.renderAndEncodeSingleFrame(j);
                publishProgress(j);
            }

            MeshViewerUtils.cleanupAfterEncoding();
            publishProgress(BrowserUtils.PROGRESS_CODE_INCREMENT_MODEL);
        }

        return null;
    }

}