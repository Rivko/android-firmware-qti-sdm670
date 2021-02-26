/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.utils;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.util.Hashtable;
import java.util.Vector;

public class LogFileDump {
    public final static int SKIP_MOTION = 1;
    public final static int SKIP_LAST_NOT_DONE = 2;
    public final static int SKIP_FOCUSING = 3;

    private int mPreviewWidth;
    private int mPreviewHeight;
    private int mPictureWidth;
    private int mPictureHeight;
    private int mFocusMode;

    // interesting or not
    private Vector<Integer> mInterestingFramesId;

    // 1 for high motion; 2 for not completing last action;
    private Vector<Integer> mSkipReason;

    // eval frame id
    private Vector<Integer> mEvalFramesId;

    // post processing result
    private Vector<Integer> mPostProcessingResult;

    // ROI, index, startframeid, endframeid, roi
    private Vector<Integer> mRois;

    public void init(int prew, int preh, int picw, int pich, int fmode) {
        mPreviewWidth = prew;
        mPreviewHeight = preh;
        mPictureWidth = picw;
        mPictureHeight = pich;
        mFocusMode = fmode;

        mInterestingFramesId = new Vector<Integer>();
        mSkipReason = new Vector<Integer>();
        mEvalFramesId = new Vector<Integer>();
        mRois = new Vector<Integer>();
    }

    public Vector<Integer> getInterestingFramesId()
    {
        return mInterestingFramesId;
    }

    public Vector<Integer> getSkipReason()
    {
        return mSkipReason;
    }

    public Vector<Integer> getEvalFramesId()
    {
        return mEvalFramesId;
    }

    public void setPostProcessingResult(Vector<Integer> result)
    {
        mPostProcessingResult = (Vector<Integer>) result.clone();
    }

    public Vector<Integer> getRois()
    {
        return mRois;
    }

    public void writeFile(String name) {
        File file = new File(name);

        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(file));

            StringBuilder sb = new StringBuilder();
            sb.append("preview width:");
            sb.append(mPreviewWidth);
            sb.append(" height:");
            sb.append(mPreviewHeight);
            sb.append('\n');

            sb.append("picture width:");
            sb.append(mPictureWidth);
            sb.append(" height:");
            sb.append(mPictureHeight);
            sb.append('\n');

            sb.append("focus mode:");
            sb.append(mFocusMode);
            sb.append('\n');

            sb.append("interesting frame id:\n");
            for(int i = 0; i < mInterestingFramesId.size(); i++) {
                sb.append(mInterestingFramesId.elementAt(i).intValue());
                sb.append('\n');
            }

            sb.append("skip reason:\n");
            for(int i = 0; i < mSkipReason.size(); i++) {
                sb.append(mSkipReason.elementAt(i).intValue());
                sb.append('\n');
            }

            sb.append("eval frame id:\n");
            for(int i = 0; i < mEvalFramesId.size(); i++) {
                sb.append(mEvalFramesId.elementAt(i).intValue());
                sb.append('\n');
            }

            sb.append("post processing result:\n");
            for(int i = 0; i < mPostProcessingResult.size(); i++) {
                sb.append(mPostProcessingResult.elementAt(i).intValue());
                sb.append('\n');
            }

            sb.append("roi data: index start end x y w h\n");
            for(int i = 0; i < mRois.size()/7; i++) {
                for(int j = 0; j < 7; j++) {
                    sb.append(mRois.elementAt(i * 7 + j).intValue());
                    sb.append(' ');
                }
                sb.append('\n');
            }

            writer.append(sb);
            writer.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
