/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import android.hardware.soundtrigger.SoundTrigger;

import com.qualcomm.qti.smartassistant.utils.LogUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class SmSessionManager {
    private Map<Integer, SmSession> mSmSessions = new HashMap<Integer, SmSession>();
    private ArrayList<String> mLoadedSmNameList = new ArrayList<String>();
    public final static int MAX_SESSION_NUM = 8;

    //implement SmSessionManager as singleton
    private static final class SingletonHolder {
        private static final SmSessionManager INSTANCE = new SmSessionManager();

        private SingletonHolder() {

        }
    }

    private SmSessionManager() {
        for (int i = 0; i < MAX_SESSION_NUM; i++) {
            mLoadedSmNameList.add(null);
        }
    }

    public static SmSessionManager getInstance() {
        return SingletonHolder.INSTANCE;
    }

    public void reinitSmSessions() {
        mSmSessions = new HashMap<Integer, SmSession>();
        mLoadedSmNameList = new ArrayList<String>();
        for (int i = 0; i <= MAX_SESSION_NUM; i++) {
            mLoadedSmNameList.add(null);
        }
    }

    private int addSmNameToList(String smName) {
        for (int i = 0; i < mLoadedSmNameList.size(); i++) {
            if (null == mLoadedSmNameList.get(i)) {
                mLoadedSmNameList.set(i, smName);
                return i;
            }
        }
        return -1;
    }

    private boolean removeSmNameFromList(String smName) {
        for (int i = 0; i <= mLoadedSmNameList.size(); i++) {
            if (null != mLoadedSmNameList.get(i)
                    && mLoadedSmNameList.get(i).equalsIgnoreCase(smName)) {
                mLoadedSmNameList.set(i, null);
                return true;
            }
        }

        LogUtils.e("removeSmNameFromList: do not find smName from the list");
        return false;
    }


    public void addLoadedSmSession(String smName, int smHandle,
                                   SoundTrigger.KeyphraseRecognitionExtra[] recognitionExtras) {
        if (null == smName
                || smHandle < 0
                || null == recognitionExtras) {
            LogUtils.e("addLoadedSmSession: invalid input params");
            return;
        }

        int id = addSmNameToList(smName);
        mSmSessions.put(smHandle,
                new SmSession(smName, recognitionExtras, SmSession.SmState.LOADED, id));
        if (mSmSessions.get(smHandle).mSessionId == -1) {
            LogUtils.e("addLoadedSmSession: session num reach max, fail to add loaded SM to list");
        }
    }

    public void addStartedSmSession(int smHandle,
                                    SoundTrigger.RecognitionConfig recognitionConfig) {
        if (!mSmSessions.containsKey(smHandle)) {
            LogUtils.e("addStartedSmSession: invalid input param");
            return;
        }

        mSmSessions.get(smHandle).setRecognitionConfig(recognitionConfig);
        mSmSessions.get(smHandle).setSessionState(SmSession.SmState.STARTED);
    }

    public void addStoppedSmSession(int smHandle) {
        if (!mSmSessions.containsKey(smHandle)) {
            LogUtils.e("addStoppedSmSession: invalid input param");
            return;
        }
        mSmSessions.get(smHandle).setSessionState(SmSession.SmState.STOPPED);
        mSmSessions.get(smHandle).removeRecognitionConfig();
    }

    public void addUnloadedSmSession(int smHandle) {
        //remove session obj
        SmSession smSession = mSmSessions.remove(smHandle);
        if (null == smSession) {
            LogUtils.e("addUnloadedSmSession: cannot remove session obj with smHandle = "
                    + smHandle);
        }

        //remove smName from the mLoadedSmNameList
        boolean bSuccess = removeSmNameFromList(smSession.mSmName);
        if (!bSuccess) {
            LogUtils.d("addUnloadedSmSession: remove SM name from list failed");
        }
    }

    public String getSmName(int smHandle) {
        return mSmSessions.containsKey(smHandle) ? mSmSessions.get(smHandle).getSmName() : null;
    }

    public SmSession.SmState getSessionState(int smHandle) {
        return mSmSessions.containsKey(smHandle) ? mSmSessions.get(smHandle).getSessionState() : null;
    }

    public SoundTrigger.KeyphraseRecognitionExtra[] getKpRecognitionExtras(int smHandle) {
        return mSmSessions.containsKey(smHandle)
                ? mSmSessions.get(smHandle).getKpRecognitionExtra()
                : null;
    }

    public SoundTrigger.RecognitionConfig getRecognitionConfig(int smHandle) {
        return mSmSessions.containsKey(smHandle)
                ? mSmSessions.get(smHandle).getRecognitionConfig() : null;
    }

    public int getSessionId(int smHandle) {
        return mSmSessions.containsKey(smHandle) ? mSmSessions.get(smHandle).getSessionId() : null;
    }

    public int getSmHandle(String smName) {
        int retSmHandle = -1;

        if (null == smName) {
            LogUtils.e("getSmHandle: invalid input param");
            return retSmHandle;
        }

        for (Map.Entry<Integer, SmSession> entry : mSmSessions.entrySet()) {
            if (entry.getValue().getSmName().equalsIgnoreCase(smName)) {
                retSmHandle = entry.getKey();
            }
        }

        if (-1 == retSmHandle) {
            LogUtils.e("getSmHandle: do not find out match record");
            return retSmHandle;
        }

        return retSmHandle;
    }

    private boolean isStateTransitional(SmSession.SmState fromState, SmSession.SmState toState) {
        switch (toState) {
            case UNLOADED:
                if (SmSession.SmState.LOADED == fromState
                        || SmSession.SmState.STOPPED == fromState) {
                    return true;
                }
                break;
            case LOADED:
                if (SmSession.SmState.UNLOADED == fromState) {
                    return true;
                }
                break;
            case STARTED:
                if (SmSession.SmState.LOADED == fromState
                        || SmSession.SmState.STOPPED == fromState) {
                    return true;
                }
                break;
            case STOPPED:
                if (SmSession.SmState.STARTED == fromState) {
                    return true;
                }
                break;
            default:
                LogUtils.e("isStateTransitional: unrecognized state" + toState);
                break;
        }

        return false;
    }

    public boolean isStateTransitional(int smHandle, SmSession.SmState toState) {
        if (-1 == smHandle) {
            LogUtils.e("isStateTransitional: invalid smHandle");
            return false;
        }

        SmSession.SmState fromState = getSessionState(smHandle);
        return isStateTransitional(fromState, toState);
    }

    public ArrayList<Integer> getAllStartedSessionsSmHandles() {
        ArrayList<Integer> smHandleList = new ArrayList<Integer>();

        for (Map.Entry<Integer, SmSession> entry : mSmSessions.entrySet()) {
            if (entry.getValue().getSessionState().equals(SmSession.SmState.STARTED)) {
                LogUtils.d("getAllStartedSessionsSmHandles: added smHandle = " + entry.getKey());
                smHandleList.add(entry.getKey());
            }
        }

        return smHandleList;
    }

    public ArrayList<String> getLoadedSmNameList() {
        return mLoadedSmNameList;
    }

    public SmSession getSmSession(int smHandle) {
        if (!mSmSessions.containsKey(smHandle)) {
            LogUtils.e("getSmSession: invalid input param: smHandle = " + smHandle);
            return null;
        }

        return mSmSessions.get(smHandle);
    }

}
