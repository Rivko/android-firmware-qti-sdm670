/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.options;

import com.qualcomm.qti.uceservice.V1_0.IOptionsListener;
import com.qualcomm.qti.uceservice.V1_0.UceStatusCode;
import com.qualcomm.qti.uceservice.V1_0.OptionsSipResponse;
import com.qualcomm.qti.uceservice.V1_0.OptionsCapabilityInfo;
import com.qualcomm.qti.uceservice.V1_0.OptionsCmdId;
import com.qualcomm.qti.uceservice.V1_0.OptionsCmdStatus;
import com.qualcomm.qti.uceservice.V1_0.OptionsCapabilityInfo;

import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.common.CapInfo;

import com.qualcomm.qti.translator.*;

import android.os.RemoteException;
import android.util.Log;
import android.os.Handler;
import android.os.HandlerThread;

import static java.lang.Math.toIntExact;

import com.qualcomm.qti.uceShimService.UceThreadHandler;

public class OptionsListener extends IOptionsListener.Stub
{
    com.android.ims.internal.uce.options.IOptionsListener mAidlOptionsServiceListener = null;

    private HidlToAidlTranslator translateToAidlObj = null;
    private HandlerThread cdListenerThread = new HandlerThread("QCdServiceListenerThread");
    private Handler cdListenerHandler;
    private UceThreadHandler mOptionThreadHandler = null;

    private final static String logTAG = "RCSService Options-Listener";

    public void setOptionsHandler(UceThreadHandler handler) {
        this.mOptionThreadHandler = handler;
    }


    public void setAidlOptionsServiceListener(com.android.ims.internal.uce.options.IOptionsListener objOptionsServiceListener) {
        Log.d(logTAG, "setIQCDServiceListener start ");
        mAidlOptionsServiceListener = objOptionsServiceListener;

    }

    public void onOptionsCreated(long serviceHandle) {
        Log.d(logTAG,"onOptionsCreated:  start inside thread [" + serviceHandle + "]");
        mOptionThreadHandler.setHandle((int)serviceHandle);
        mOptionThreadHandler.resume();
        mOptionThreadHandler = null;
    }

    public void getVersionCb( String pVersion ) {

        OptionsListenerRunnable runnable = new OptionsListenerRunnable();
        runnable.mAidlOptionsListener = mAidlOptionsServiceListener;
        runnable.m_Version = pVersion;
        runnable.methodName = "getVersionCb";

        postToHandler(runnable , "getVersionCb" );
    }

    public void serviceAvailable(int statusCode) throws RemoteException {

        OptionsListenerRunnable runnable = new OptionsListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlOptionsListener = mAidlOptionsServiceListener;
        runnable.m_statusCode = new StatusCode();
        runnable.m_statusCode.setStatusCode(translateToAidlObj.getAidlStatusCode(statusCode));
        runnable.methodName = "serviceAvailable";

        postToHandler(runnable , "serviceAvailable" );
    }

    public void serviceUnAvailable(int statusCode) throws RemoteException {

        OptionsListenerRunnable runnable = new OptionsListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlOptionsListener = mAidlOptionsServiceListener;
        runnable.m_statusCode = new StatusCode();
        runnable.m_statusCode.setStatusCode(translateToAidlObj.getAidlStatusCode(statusCode));
        runnable.methodName = "serviceUnAvailable";

        postToHandler(runnable , "serviceUnavailable" );
    }

    public void sipResponseReceived( String URI,
                                      OptionsSipResponse cdSipResponse, OptionsCapabilityInfo CDCapInfo)
                throws RemoteException {

        OptionsListenerRunnable runnable = new OptionsListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlOptionsListener = mAidlOptionsServiceListener;
        runnable.m_URI = URI;
        runnable.m_cdSipResponse = new com.android.ims.internal.uce.options.OptionsSipResponse();
        runnable.m_CDCapInfo = new com.android.ims.internal.uce.options.OptionsCapInfo();
        runnable.m_cdSipResponse = translateToAidlObj.getAidlOptionsSipResponse(cdSipResponse);
        runnable.m_CDCapInfo = translateToAidlObj.getAidlOptionsCapInfo(CDCapInfo);
        runnable.methodName = "sipResponseReceived";

        postToHandler(runnable , "QCDServiceListener_CDUpdate" );
    }

    public void cmdStatus(OptionsCmdStatus CdCMDStatus) {

        OptionsListenerRunnable runnable = new OptionsListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlOptionsListener = mAidlOptionsServiceListener;
        runnable.m_CdCMDStatus = new com.android.ims.internal.uce.options.OptionsCmdStatus();
        runnable.m_CdCMDStatus = translateToAidlObj.getAidlOptionsCmdStatus(CdCMDStatus);
        runnable.methodName = "cmdStatus";

        postToHandler(runnable , "cmdStatus" );
    }

    public void incomingOptions(String sUri,
                         OptionsCapabilityInfo CDCapInfo, short tID) {

        OptionsListenerRunnable runnable = new OptionsListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlOptionsListener = mAidlOptionsServiceListener;
        runnable.m_URI = sUri;
        runnable.m_tID = (int)tID;
        runnable.m_CDCapInfo = new com.android.ims.internal.uce.options.OptionsCapInfo();
        runnable.m_CDCapInfo = translateToAidlObj.getAidlOptionsCapInfo(CDCapInfo);
        runnable.methodName = "incomingOptions";

        postToHandler(runnable , "QCDServiceListener_IncomingOptions" );
    }

    private void postToHandler(Runnable runnable, String methodName) {
        try {
            if(cdListenerThread == null) {
                Log.d(logTAG, ": "+ methodName +" : thread is null");
                cdListenerThread = new HandlerThread("QCdServiceListenerThread");
                cdListenerThread.start();
                cdListenerHandler = new Handler(cdListenerThread.getLooper());
                cdListenerHandler.post(runnable);

            } else if (cdListenerThread.getState() == Thread.State.NEW) {
                Log.d(logTAG, ": "+ methodName +" : thread state is new");
                cdListenerThread.start();
                cdListenerHandler = new Handler(cdListenerThread.getLooper());
                cdListenerHandler.post(runnable);

            } else if (cdListenerThread.getState() == Thread.State.WAITING) {
                Log.d(logTAG, ": "+ methodName +" : thread state is wating");
                cdListenerHandler.post(runnable);

            } else if (cdListenerThread.getState() == Thread.State.TERMINATED) {
                Log.d(logTAG, ": "+ methodName +" : thread state is terminated");
                cdListenerThread = null;
                cdListenerThread = new HandlerThread("QCdServiceListenerThread");
                cdListenerThread.start();
                cdListenerHandler = new Handler(cdListenerThread.getLooper());
                cdListenerHandler.post(runnable);

            } else if(cdListenerThread.getState() == Thread.State.BLOCKED) {
                Log.d(logTAG, ": "+ methodName +" : thread state is blocked");
                cdListenerHandler.post(runnable);

            } else {
                Log.d(logTAG, ": "+ methodName +" : before posting runnable to handler : thread state "+ cdListenerThread.getState());
                cdListenerHandler.post(runnable);
            }
        } catch(Exception e) {
            Log.d(logTAG, ": "+ methodName +" : Exception during posting runnable to handler"+e);
            e.printStackTrace();
        }
    }

    private class OptionsListenerRunnable implements Runnable {

        public StatusCode m_statusCode = null;
        public String m_Version = null;
        public String m_URI = null;
        public int m_tID;
        public com.android.ims.internal.uce.options.OptionsSipResponse m_cdSipResponse = null;
        public com.android.ims.internal.uce.options.OptionsCmdStatus m_CdCMDStatus = null;
        public com.android.ims.internal.uce.options.OptionsCapInfo m_CDCapInfo = null;

        public String methodName;

        com.android.ims.internal.uce.options.IOptionsListener mAidlOptionsListener = null;

        @Override
        public void run() {
            try {
                if (methodName.equals("getVersionCb")) {
                    mAidlOptionsListener.getVersionCb(m_Version);
                }
                if (methodName.equals("serviceAvailable")) {
                    mAidlOptionsListener.serviceAvailable(m_statusCode);
                }
                if (methodName.equals("serviceUnAvailable")) {
                    mAidlOptionsListener.serviceUnavailable(m_statusCode);
                }
                if (methodName.equals("sipResponseReceived")) {
                    mAidlOptionsListener.sipResponseReceived(m_URI,
                                                              m_cdSipResponse, m_CDCapInfo);
                }
                if (methodName.equals("cmdStatus")) {
                    mAidlOptionsListener.cmdStatus(m_CdCMDStatus);
                }
                if (methodName.equals("incomingOptions")) {
                    mAidlOptionsListener.incomingOptions(m_URI, m_CDCapInfo, m_tID);
                }
            } catch (RemoteException remot) {
                Log.d(logTAG,"RemoteException client does not exits");
            } catch (NullPointerException nullpointer) {
                Log.d(logTAG, "NullPointerException client is null");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };
}
