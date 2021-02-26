/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import com.qualcomm.qti.qmmi.bean.TestCase;

import java.util.List;

public class ResultThread {

    final Object mSyncLock = new Object();
    private List<TestCase> mTestCaseList;
    private String filePath;

    public ResultThread() {
    }

    public ResultThread(List<TestCase> mTestCaseList, String filePath) {
        this.mTestCaseList = mTestCaseList;
        this.filePath = filePath;

    }

    public void setTestCaseList(List<TestCase> mTestCaseList) {
        this.mTestCaseList = mTestCaseList;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    //** flush the result to file
    public void flushResult() {
        synchronized (mSyncLock) {
            mSyncLock.notify();
        }
    }

    public void start() {
        Thread t = new Thread(new PersistRunnable(mSyncLock));
        t.start();
    }

    private class PersistRunnable implements Runnable {
        private Object obj;

        public PersistRunnable(Object obj) {
            this.obj = obj;
        }

        public void run() {
            for (; ; ) {
                synchronized (obj) {
                    try {
                        System.out.println("wait notify come to start" + Thread.currentThread().getName());
                        obj.wait();
                        ResultParser.saveResultToFile(filePath, mTestCaseList);
                    } catch (Exception exc) {
                        exc.printStackTrace(System.out);
                    }
                }
            }
        }
    }

}
