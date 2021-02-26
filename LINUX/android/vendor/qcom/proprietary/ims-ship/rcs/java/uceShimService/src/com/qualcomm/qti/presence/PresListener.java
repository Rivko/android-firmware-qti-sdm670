/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.presence;


//import com.android.ims.internal.uce.presence.*;
import java.util.ArrayList;
import java.util.Iterator;

import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.presence.PresCmdId;
import com.android.ims.internal.uce.presence.PresResInstanceInfo;
import com.qualcomm.qti.uceservice.V1_0.IPresenceListener;
import com.qualcomm.qti.uceservice.V1_0.PresPublishTriggerType;
import com.qualcomm.qti.uceservice.V1_0.PresSubscriptionState;
import com.qualcomm.qti.uceservice.V1_0.PresenceCmdId;
import com.qualcomm.qti.uceservice.V1_0.ResInstantceState;
import com.qualcomm.qti.uceservice.V1_0.UceStatusCode;

import com.qualcomm.qti.translator.*;

import android.os.DeadObjectException;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.RemoteException;
import android.util.Log;

import com.qualcomm.qti.uceShimService.UceThreadHandler;

public class PresListener extends IPresenceListener.Stub
{
    com.android.ims.internal.uce.presence.IPresenceListener mAidlPresenceListener = null;
    private HidlToAidlTranslator translateToAidlObj = null;
    private HandlerThread presListenerThread = new HandlerThread("QPresServiceListenerThread");
    private Handler presListenerHandler;
    private UceThreadHandler mPresThreadHandler = null;

    private final static String logTAG = "AIDL-Shim:PresListener:";

    public void setAidlPresListener(com.android.ims.internal.uce.presence.IPresenceListener aidlPresListener) {
	mAidlPresenceListener = aidlPresListener;
    }
    public void setCreatePresenceHandler(UceThreadHandler handler) {
        this.mPresThreadHandler = handler;
    }

    @Override
    public void onPresenceCreated(long serviceHandle) {
        Log.d(logTAG,"onPresenceCreated:  start inside thread [" + serviceHandle + "]");
        mPresThreadHandler.setHandle((int)serviceHandle);
        mPresThreadHandler.resume();
        mPresThreadHandler = null;
    }

    @Override
    public void getVersionCb(String pVersion)
            throws RemoteException {

        PresListenerRunnable runnable = new PresListenerRunnable();
        runnable.mAidlPresenceListener = mAidlPresenceListener;
        runnable.m_pVersion = pVersion;
        runnable.methodName = "getVersionCb";

        postToHandler(runnable , "getVersionCb" );
    }

    @Override
    public void serviceAvailable(int statusCode)
                 throws RemoteException {

        PresListenerRunnable runnable = new PresListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlPresenceListener = mAidlPresenceListener;
        runnable.m_statusCode = new StatusCode();
        runnable.m_statusCode.setStatusCode(translateToAidlObj.getAidlStatusCode(statusCode));
        runnable.methodName = "serviceAvailable";

        postToHandler(runnable , "serviceAvailable");
    }

    @Override
    public void serviceUnAvailable(int statusCode)
                          throws RemoteException {

        PresListenerRunnable runnable = new PresListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.mAidlPresenceListener = mAidlPresenceListener;
        runnable.methodName = "serviceUnAvailable";
        runnable.m_statusCode = new StatusCode();
        runnable.m_statusCode.setStatusCode(translateToAidlObj.getAidlStatusCode(statusCode));

        postToHandler(runnable , "serviceUnAvailable");
    }

    @Override
    public void onPublishTrigger(int publishTrigger)
                throws RemoteException {

        PresListenerRunnable runnable = new PresListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.methodName = "onPublishTrigger";
        runnable.mAidlPresenceListener = mAidlPresenceListener;
        runnable.m_publishTrigger = new com.android.ims.internal.uce.presence.PresPublishTriggerType();
        runnable.m_publishTrigger.setPublishTrigeerType(translateToAidlObj.getAidlPresenceTriggerTypeValue(publishTrigger));
        postToHandler(runnable , "onPublishTrigger");
    }

    @Override
    public void cmdStatus(
            com.qualcomm.qti.uceservice.V1_0.PresCmdStatus pCmdStatus) throws RemoteException {
        Log.d(logTAG,"cmdStatus: start");

        com.android.ims.internal.uce.presence.PresCmdStatus cmdStatus = new com.android.ims.internal.uce.presence.PresCmdStatus();
        translateToAidlObj = new HidlToAidlTranslator();

        cmdStatus = translateToAidlObj.getAidlPresCmdStatus(pCmdStatus);

        PresListenerRunnable runnable = new PresListenerRunnable();
        runnable.methodName = "cmdStatus";
        runnable.mAidlPresenceListener = mAidlPresenceListener;
        runnable.m_pCmdStatus = cmdStatus;

        postToHandler(runnable , "cmdStatus");
    }

    @Override
    public void onSipResponseReceived(
            com.qualcomm.qti.uceservice.V1_0.PresSipResponse pSipResponse) throws RemoteException {

        com.android.ims.internal.uce.presence.PresCmdId presCmdId = new com.android.ims.internal.uce.presence.PresCmdId();
        com.android.ims.internal.uce.presence.PresSipResponse sipResponse = new com.android.ims.internal.uce.presence.PresSipResponse();
        translateToAidlObj = new HidlToAidlTranslator();

        sipResponse = translateToAidlObj.getAidlPresSipResponse(pSipResponse);

        PresListenerRunnable runnable = new PresListenerRunnable();
        runnable.methodName = "onSipResponseReceived";
        runnable.mAidlPresenceListener = mAidlPresenceListener;
        runnable.m_pSipResponse = sipResponse;

        postToHandler(runnable , "onSipResponseReceived");
    }

    @Override
    public void onCapInfoReceived(
            String pPresentityURI,
            ArrayList<com.qualcomm.qti.uceservice.V1_0.PresTupleInfo> pTupleInfoArray)
            throws RemoteException {
        PresListenerRunnable runnable = new PresListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.methodName = "onCapInfoReceived";
        runnable.mAidlPresenceListener = mAidlPresenceListener;

	int sizeofTupleArray = pTupleInfoArray.size();
	runnable.m_pTupleInfo = new com.android.ims.internal.uce.presence.PresTupleInfo[sizeofTupleArray];

	runnable.m_pTupleInfo = translateToAidlObj.getAidlTupleInfoArray(pTupleInfoArray);
        runnable.m_pPresentityURI = pPresentityURI;

        postToHandler(runnable , "onCapInfoReceived");
    }

    @Override
    public void onListCapInfoReceived(
            com.qualcomm.qti.uceservice.V1_0.PresRlmiInfo pRlmiInfo,
			ArrayList<com.qualcomm.qti.uceservice.V1_0.PresResInfo> pResInfoArray)
            throws RemoteException {

        PresListenerRunnable runnable = new PresListenerRunnable();
        translateToAidlObj = new HidlToAidlTranslator();

        runnable.methodName = "onListCapInfoReceived";
        runnable.mAidlPresenceListener = mAidlPresenceListener;

        runnable.m_pRlmiInfo = new com.android.ims.internal.uce.presence.PresRlmiInfo();
        runnable.m_pRlmiInfo = translateToAidlObj.getAidlPresRlmiInfo(pRlmiInfo);

        int sizeOfResInfoArray = pResInfoArray.size();
        runnable.m_pResInfo = new com.android.ims.internal.uce.presence.PresResInfo[sizeOfResInfoArray];
        runnable.m_pResInfo = translateToAidlObj.getAidlPresResInfo(pResInfoArray);

        postToHandler(runnable , "onListCapInfoReceived");
    }

	@Override
    public void onUnpublishSent() {
        PresListenerRunnable runnable = new PresListenerRunnable();
        runnable.methodName = "onUnPublishSent";
        runnable.mAidlPresenceListener = mAidlPresenceListener;
        postToHandler(runnable , "onUnPublishSent");

    }

    private void postToHandler(Runnable runnable, String methodName)
    {
        try
        {
            if(presListenerThread == null)
            {
                Log.d(logTAG, ": "+ methodName +" : thread is null");
                presListenerThread = new HandlerThread("QPresServiceListenerThread");
                presListenerThread.start();
                Log.d(logTAG, ": "+ methodName +" : after thread start");
                presListenerHandler = new Handler(presListenerThread.getLooper());
                presListenerHandler.post(runnable);
                Log.d(logTAG, ": "+ methodName +" : after posting runnable to handler");
            }
            else if (presListenerThread.getState() == Thread.State.NEW)
            {
                Log.d(logTAG, ": "+ methodName +" : thread state is new");
                presListenerThread.start();
                Log.d(logTAG, ": "+ methodName +" : after thread start");
                presListenerHandler = new Handler(presListenerThread.getLooper());
                presListenerHandler.post(runnable);
                Log.d(logTAG, ": "+ methodName +" : after posting runnable to handler");
            }
            else if (presListenerThread.getState() == Thread.State.WAITING)
            {
                Log.d(logTAG, ": "+ methodName +" : thread state is wating");
                presListenerHandler.post(runnable);
                Log.d(logTAG, ": "+ methodName +" : after posting runnable to handler");
            }
            else if (presListenerThread.getState() == Thread.State.TERMINATED)
            {
                Log.d(logTAG, ": "+ methodName +" : thread state is terminated");
                presListenerThread = null;
                presListenerThread = new HandlerThread("QPresServiceListenerThread");
                presListenerThread.start();
                Log.d(logTAG, ": "+ methodName +" : after thread start");
                presListenerHandler = new Handler(presListenerThread.getLooper());
                presListenerHandler.post(runnable);
                Log.d(logTAG, ": "+ methodName +" : after posting runnable to handler");
            }
            else if(presListenerThread.getState() == Thread.State.BLOCKED)
            {
                Log.d(logTAG, ": "+ methodName +" : thread state is blocked");
                presListenerHandler.post(runnable);
                Log.d(logTAG, ": "+ methodName +" : after posting runnable to handler");
            }
            else
            {
                Log.d(logTAG, ": "+ methodName +" : before posting runnable to handler : thread state "+ presListenerThread.getState());
                presListenerHandler.post(runnable);
                Log.d(logTAG, ": "+ methodName +" : after posting runnable to handler");
            }
        }
        catch(Exception e)
        {
            Log.d(logTAG, ": "+ methodName +" : Exception during posting runnable to handler"+e);
            e.printStackTrace();
        }
    }

  private class PresListenerRunnable implements Runnable {
      public StatusCode m_statusCode = null;
      public String m_pVersion = null;
      public com.android.ims.internal.uce.presence.PresPublishTriggerType m_publishTrigger = null;
      public com.android.ims.internal.uce.presence.PresCmdId m_presCmdId = null;
      public com.android.ims.internal.uce.presence.PresCmdStatus m_pCmdStatus = null;
      public com.android.ims.internal.uce.presence.PresCapInfo m_pMyCapInfo = null;
      public com.android.ims.internal.uce.presence.PresSipResponse m_pSipResponse = null;
      public String m_pPresentityURI = null;
      public com.android.ims.internal.uce.presence.PresResInstanceInfo m_pInstanceInfo = null;
      public com.android.ims.internal.uce.presence.PresTupleInfo m_pTupleInfo [];
      public com.android.ims.internal.uce.presence.PresRlmiInfo m_pRlmiInfo = null;
      public com.android.ims.internal.uce.presence.PresResInfo m_pResInfo[];
      public ResInstantceState m_resInstState = null;

      public String methodName;

      com.android.ims.internal.uce.presence.IPresenceListener mAidlPresenceListener = null;

      @Override
      public void run() {
          try {
          if (methodName.equals("getVersionCb")) {
              mAidlPresenceListener.getVersionCb(m_pVersion);
          }
          if (methodName.equals("serviceAvailable")) {
              mAidlPresenceListener.serviceAvailable(m_statusCode);
          }
          if (methodName.equals("serviceUnAvailable")) {
              mAidlPresenceListener.serviceUnAvailable(m_statusCode);
          }
          if (methodName.equals("onPublishTrigger")) {
              mAidlPresenceListener.publishTriggering(m_publishTrigger);
          }
          if (methodName.equals("cmdStatus")) {
              mAidlPresenceListener.cmdStatus(m_pCmdStatus);
          }
          if (methodName.equals("onSipResponseReceived")) {
              mAidlPresenceListener.sipResponseReceived(m_pSipResponse);
          }
          if (methodName.equals("onCapInfoReceived")) {
              mAidlPresenceListener.capInfoReceived(m_pPresentityURI, m_pTupleInfo);
          }
          if (methodName.equals("onListCapInfoReceived")) {
              mAidlPresenceListener.listCapInfoReceived(m_pRlmiInfo, m_pResInfo);
          }
          if (methodName.equals("onUnPublishSent")) {
              mAidlPresenceListener.unpublishMessageSent();
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
