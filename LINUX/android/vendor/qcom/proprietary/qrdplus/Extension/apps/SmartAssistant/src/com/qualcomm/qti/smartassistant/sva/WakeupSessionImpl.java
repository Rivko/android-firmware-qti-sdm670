/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import android.content.Context;
import android.hardware.soundtrigger.SoundTrigger;
import android.hardware.soundtrigger.SoundTriggerModule;

import com.qualcomm.listen.ListenSoundModel;
import com.qualcomm.listen.ListenTypes;
import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.utils.SharedPreferenceUtils;
import com.qualcomm.qti.smartassistant.utils.FileUtils;
import com.qualcomm.qti.smartassistant.wrapper.AudioSystemWrapper;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.CopyOnWriteArrayList;

public class WakeupSessionImpl implements IWakeupSession, SoundTrigger.StatusListener {
    private SoundTriggerModule mSTModule = null;
    private boolean mSTServiceEnabled = false;
    private List<IWakeupListener> mWakeupListeners;
    private Context mAppContext;
    private UUID mUuid = null;
    private UUID mVendorUuid = null;


    @Override
    public int establishSvaSession(String smName, Context context) {
        int result;
        mAppContext = context;
        if (null == smName) {
            LogUtils.e("establishSvaSession: invalid input param smName == null");
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        result = loadSM(smName);
        if (SvaApiConst.RET_CODE_OK != result) {
            LogUtils.e("establishSvaSession: loadSM fail result = " + result);
            return result;
        }

        result = startRecognition(smName);
        if (SvaApiConst.RET_CODE_OK != result) {
            LogUtils.e("establishSvaSession: startRecognition fail result = " + result);
            return result;
        }

        return result;
    }

    @Override
    public int terminateSvaSession(String smName) {
        int result;
        if (null == smName) {
            LogUtils.e("terminateSvaSession: invalid input param smName is null");
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        // stopRecognition first
        result = stopRecognition(smName);
        if (SvaApiConst.RET_CODE_OK != result) {
            LogUtils.e("terminateSvaSession: stopRecognition fail result = " + result);
            return result;
        }

        //then unload SM
        result = unloadSM(smName);
        if (SvaApiConst.RET_CODE_OK != result) {
            LogUtils.e("terminateSvaSession: unloadSM fail result = " + result);
            return result;
        }

        return result;
    }

    @Override
    public void releaseAllSvaSessions() {
        LogUtils.d("releaseAllSvaSessions: enter");
        detachModule();

        // set all sva sessions to init state
        SmSessionManager.getInstance().reinitSmSessions();

        LogUtils.d("releaseAllSvaSessions: exit");
    }

    @Override
    public void addWakeupListener(IWakeupListener listener) {
        mWakeupListeners.add(listener);

    }

    @Override
    public void removeWakeupListener(IWakeupListener listener) {
        mWakeupListeners.remove(listener);
    }

    @Override
    public int restartRecognition(int smHandle) {
        int result;

        if (-1 == smHandle) {
            LogUtils.e("restartRecognition: invalid input param smHandle is -1");
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        if (SmSession.SmState.STARTED
                == SmSessionManager.getInstance().getSessionState(smHandle)) {
            LogUtils.e("restartRecognition: already restarted, return success directly");
            result = SvaApiConst.RET_CODE_OK;
            return result;
        }

        if (SmSession.SmState.STOPPED
                != SmSessionManager.getInstance().getSessionState(smHandle)) {
            LogUtils.e("restartRecognition: fail for wrong state");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        // get the related recognition config and restart the recognition
        SoundTrigger.RecognitionConfig stRecognitionConfig
                = SmSessionManager.getInstance().getRecognitionConfig(smHandle);
        if (null == stRecognitionConfig) {
            LogUtils.e("restartRecognition: getRecognitionConfig fail");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        int ret = mSTModule.startRecognition(smHandle, stRecognitionConfig);
        LogUtils.d("restartRecognition: mSTModule.startRecognition return value = " + ret);
        if (SoundTrigger.STATUS_OK != ret) {
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        } else {
            // set smHandle session state to STARTED
            SmSession smSession = SmSessionManager.getInstance().getSmSession(smHandle);
            if (null != smSession) {
                smSession.setSessionState(SmSession.SmState.STARTED);
            }

            result = SvaApiConst.RET_CODE_OK;
            return result;
        }
    }

    @Override
    public SmSession querySessionInfo(String smName) {
        if (null == smName) {
            LogUtils.d("querySessionInfo: invalid input param smName is null");
            return null;
        }

        int smHandle = SmSessionManager.getInstance().getSmHandle(smName);
        if (-1 == smHandle) {
            LogUtils.d("querySessionInfo: fail for no match smHandle");
            return null;
        }

        return SmSessionManager.getInstance().getSmSession(smHandle);
    }

    @Override
    public void onRecognition(SoundTrigger.RecognitionEvent recognitionEvent) {
        LogUtils.d("Impl onRecognition: recognitionEvent = " + recognitionEvent);

        // set smHandle 's session state to loaded
        int smHandle = recognitionEvent.soundModelHandle;
        SmSession smSession = SmSessionManager.getInstance().getSmSession(smHandle);
        if (null != smSession) {
            smSession.setSessionState(SmSession.SmState.STOPPED);
        }

        for (IWakeupListener listener : mWakeupListeners) {
            listener.onRecognition(recognitionEvent);
        }
    }

    @Override
    public void onSoundModelUpdate(SoundTrigger.SoundModelEvent soundModelEvent) {
        LogUtils.d("onSoundModelUpdate is recalled, do nothing");
    }

    @Override
    public void onServiceStateChange(int state) {
        LogUtils.d("Impl onServiceStateChange: state = " + state);
        setSTServiceEnabled((SoundTrigger.SERVICE_STATE_ENABLED == state) ? true : false);
        for (IWakeupListener listener : mWakeupListeners) {
            listener.onServiceStatusChange(state);
        }
    }

    @Override
    public void onServiceDied() {
        LogUtils.d("Impl onServiceDied: ST Service died");
        for (IWakeupListener listener : mWakeupListeners) {
            listener.onServiceDied();
        }
    }

    //implement WakeupSessionImpl as singleton
    private static final class SingletonHolder {
        private static final WakeupSessionImpl INSTANCE = new WakeupSessionImpl();

        private SingletonHolder() {

        }
    }

    private WakeupSessionImpl() {
        mWakeupListeners = new CopyOnWriteArrayList();
        if (!attachModule()) {
            LogUtils.e("WakeupSessionImpl: attachModule failed");
        }
    }

    public static WakeupSessionImpl getInstance() {
        return SingletonHolder.INSTANCE;
    }

    private void setSTServiceEnabled(boolean enabled) {
        mSTServiceEnabled = enabled;
    }

    private boolean getSTServiceEnabled() {
        return mSTServiceEnabled;
    }

    private boolean attachModule() {
        ArrayList<SoundTrigger.ModuleProperties> moduleProperties
                = new ArrayList<SoundTrigger.ModuleProperties>();
        SoundTrigger.listModules(moduleProperties);
        if (moduleProperties.size() < 1) {
            LogUtils.e("attachModule: no available modules");
            return false;
        }

        mVendorUuid = moduleProperties.get(0).uuid;

        LogUtils.d("attachModule: moduleProperties.get(0) = " + moduleProperties.get(0));
        mSTModule = SoundTrigger.attachModule(moduleProperties.get(0).id, this, null);
        if (null == mSTModule) {
            LogUtils.e("attachModule: low layer error");
            return false;
        }

        return true;
    }

    private int loadSM(String smName) {
        int result;
        if (null == smName) {
            LogUtils.e("loadSM: invalid input param smName = " + smName);
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        String filePath = FileUtils.getSMFilePath(smName);
        if (!FileUtils.isFileExist(filePath)) {
            LogUtils.e("loadSM: filePath = " + filePath + " not exist");
            result = SvaApiConst.RET_CODE_SM_FILE_NOT_EXIST;
            return result;
        }

        if (null == mSTModule) {
            LogUtils.d("loadSM: mSTModule is null, need retry attachModule");
            if (!attachModule()) {
                LogUtils.e("loadSM: retry attachModule fail");
                result = SvaApiConst.RET_CODE_FAILURE;
                return result;
            }
        }

        // if the SM have been loaded, return success directly
        int soundModalHandle = SmSessionManager.getInstance().getSmHandle(smName);
        if (-1 != soundModalHandle
                && (SmSession.SmState.LOADED
                == SmSessionManager.getInstance().getSessionState(soundModalHandle))) {
            LogUtils.d("loadSM: smName = " + smName + " already has been loaded");
            result = SvaApiConst.RET_CODE_OK;
            return result;
        }

        //determine recognition mode
        int recognitionMode = SoundTrigger.RECOGNITION_MODE_VOICE_TRIGGER;
        if (FileUtils.getIsUdmSm(smName) && SharedPreferenceUtils.getIsUserVerificationEnabled(mAppContext)) {
            recognitionMode += SoundTrigger.RECOGNITION_MODE_USER_IDENTIFICATION;
        }

        // inform HAL: the number of sessions that will be used.
        AudioSystemWrapper.setParameters("SVA_NUM_SESSIONS=1");

        ByteBuffer smBuffer = FileUtils.getSMData(smName);
        if (null == smBuffer) {
            LogUtils.e("loadSM: getSMData fail");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        //query SM info: support MKU SM and PDK+Multiple Users SM
        ListenTypes.SoundModelInfo smInfo = ListenSoundModel.query(smBuffer);
        if (null == smInfo) {
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        ListenTypes.SVASoundModelInfo v2SmInfo = (ListenTypes.SVASoundModelInfo) smInfo;
        ListenTypes.KeywordInfo[] kwInfoArray = v2SmInfo.keywordInfo;
        SoundTrigger.Keyphrase[] stKpArray = new SoundTrigger.Keyphrase[kwInfoArray.length];
        SoundTrigger.KeyphraseRecognitionExtra[] stKpRecognitionExtraArray
                = new SoundTrigger.KeyphraseRecognitionExtra[kwInfoArray.length];

        // create ArrayList of kpsUsersArray where the index in the ArrayList
        // is used to create the userIdList
        ArrayList<String> kpsUsersArray = new ArrayList<String>();

        // first add all keyphrase to kpsUsersArray
        for (ListenTypes.KeywordInfo kwItem : kwInfoArray) {
            kpsUsersArray.add(kwItem.keywordPhrase);
        }

        // then add all user|keyphrase to kpsUsersArray
        for (String userName : v2SmInfo.userNames) {
            // obtain the user info in the KeywordInfo, then compare
            // and judge whether the keyword has been training by
            // the user
            for (ListenTypes.KeywordInfo kwItem : kwInfoArray) {
                for (String trainedUserName : kwItem.activeUsers) {
                    if (userName.equals(trainedUserName)) {
                        kpsUsersArray.add(trainedUserName + "|" + kwItem.keywordPhrase);
                    }
                }
            }
        }

        // for all keyphrase, create SoundTrigger.Keyphrase object
        for (int j = 0; j < kwInfoArray.length; j++) {
            ListenTypes.KeywordInfo kwInfo = kwInfoArray[j];
            int[] userIdList = new int[kwInfo.activeUsers.length];
            int userIdListItemIndex = 0;
            int kpsUsersArrItemIndex = -1;
            for (String trainedUserName : kwInfo.activeUsers) {
                kpsUsersArrItemIndex
                        = kpsUsersArray.indexOf(trainedUserName + "|" + kwInfo.keywordPhrase);
                if (-1 == kpsUsersArrItemIndex) {
                    LogUtils.e("loadSM: cannot match index");
                    result = SvaApiConst.RET_CODE_INVALID_PARAM;
                    return result;
                }

                userIdList[userIdListItemIndex++] = kpsUsersArrItemIndex;
            }

            SoundTrigger.Keyphrase stKeyphrase
                    = new SoundTrigger.Keyphrase(j, recognitionMode, "en_US",
                    kwInfo.keywordPhrase, userIdList);
            stKpArray[j] = stKeyphrase;

            // create KeyphraseRecognitionExtra object
            SoundTrigger.ConfidenceLevel[] stConfidenceLevelArray
                    = new SoundTrigger.ConfidenceLevel[kwInfo.activeUsers.length];
            if (kwInfo.activeUsers.length != stKeyphrase.users.length) {
                LogUtils.d("loadSM: kwInfo.activeUsers.length != stKeyphrase.users.length");
                result = SvaApiConst.RET_CODE_INVALID_PARAM;
                return result;
            }

            // for all users
            for (int k = 0; k < kwInfo.activeUsers.length; k++) {
                String activeUserName = kwInfo.activeUsers[k];
                int userConfidenceLevel = FileUtils.getUserConfidenceLevelSetting();
                SoundTrigger.ConfidenceLevel stConfidenceLevel
                        = new SoundTrigger.ConfidenceLevel(stKeyphrase.users[k],
                        userConfidenceLevel);
                stConfidenceLevelArray[k] = stConfidenceLevel;
            }

            SoundTrigger.KeyphraseRecognitionExtra stKpRecognitionExtra
                    = new SoundTrigger.KeyphraseRecognitionExtra(j, 0,
                    FileUtils.getKeyphraseConfidenceLevelSetting(), stConfidenceLevelArray);
            stKpRecognitionExtraArray[j] = stKpRecognitionExtra;
        }

        mUuid = new UUID(0, 0);
        smBuffer.rewind();
        byte[] smByteArray = new byte[smBuffer.remaining()];
        smBuffer.get(smByteArray);

        SoundTrigger.KeyphraseSoundModel stKpSm
                = new SoundTrigger.KeyphraseSoundModel(mUuid, mVendorUuid, smByteArray, stKpArray);
        int[] smHandleArray = new int[20];

        // call loadSoundModel
        int ret = mSTModule.loadSoundModel(stKpSm, smHandleArray);
        LogUtils.d("loadSM: mSTModule.loadSoundModel return value = " + ret);
        if (SoundTrigger.STATUS_NO_INIT == ret) {
            result = SvaApiConst.RET_CODE_AUDIO_SERVER_NOT_READY;
            return result;
        } else if (SoundTrigger.STATUS_OK == ret) {
            int smHandle = smHandleArray[0];
            SmSessionManager.getInstance().addLoadedSmSession(smName,
                    smHandle, stKpRecognitionExtraArray);
            result = SvaApiConst.RET_CODE_OK;
            return result;
        } else {
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }
    }

    private int startRecognition(String smName) {
        int result;

        //check the input param
        if (null == smName) {
            LogUtils.e("startRecognition: invalid input param smName = " + smName);
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        //judge the SM whether loaded or not
        int smHandle = SmSessionManager.getInstance().getSmHandle(smName);
        if (-1 == smHandle) {
            LogUtils.e("startRecognition: fail for SM not loaded first");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        // if the SM have been started, return success directly
        if (SmSession.SmState.STARTED
                == SmSessionManager.getInstance().getSessionState(smHandle)) {
            LogUtils.d("startRecognition: smName = " + smName + " already has been started");
            result = SvaApiConst.RET_CODE_OK;
            return result;
        }

        boolean bCanTransit = SmSessionManager.getInstance()
                .isStateTransitional(smHandle, SmSession.SmState.STARTED);
        if (!bCanTransit) {
            LogUtils.e("startRecognition: fail for wrong state transition");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        SoundTrigger.KeyphraseRecognitionExtra[] stKpRecognitionExtraArray
                = SmSessionManager.getInstance().getKpRecognitionExtras(smHandle);
        if (null == stKpRecognitionExtraArray) {
            LogUtils.e("startRecognition: fail when getKpRecognitionExtras");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        boolean bCaptureRequest = false;
        if (SharedPreferenceUtils.getIsLABEnabled(mAppContext)) {
            bCaptureRequest = true;
        }

        SoundTrigger.RecognitionConfig stRecognitionConfig
                = new SoundTrigger.RecognitionConfig(bCaptureRequest, false,
                stKpRecognitionExtraArray, null);

        int ret = mSTModule.startRecognition(smHandle, stRecognitionConfig);
        LogUtils.d("startRecognition: mSTModule.startRecognition return value = " + ret);
        if (SoundTrigger.STATUS_OK == ret) {
            result = SvaApiConst.RET_CODE_OK;
            SmSessionManager.getInstance().addStartedSmSession(smHandle, stRecognitionConfig);
            return result;
        } else {
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }
    }

    private int stopRecognition(String smName) {
        int result;
        if (null == smName) {
            LogUtils.e("stopRecognition: invalid input param smName = " + smName);
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        int smHandle = SmSessionManager.getInstance().getSmHandle(smName);
        if (-1 == smHandle) {
            LogUtils.e("stopRecognition: fail for have not match smHandle");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        // if the SM has been stopped, just return success directly
        if (SmSession.SmState.STOPPED
                == SmSessionManager.getInstance().getSessionState(smHandle)) {
            LogUtils.e("stopRecognition: smName = " + smName + " already has been stopped");
            result = SvaApiConst.RET_CODE_OK;
            return result;
        }

        boolean bCanTransit = SmSessionManager.getInstance().isStateTransitional(smHandle,
                SmSession.SmState.STOPPED);
        if (!bCanTransit) {
            LogUtils.e("stopRecognition: fail for wrong state");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        int ret = mSTModule.stopRecognition(smHandle);
        LogUtils.d("stopRecognition: mSTModule.stopRecognition return value = " + ret);
        if (SoundTrigger.STATUS_OK == ret) {
            result = SvaApiConst.RET_CODE_OK;
            SmSessionManager.getInstance().addStoppedSmSession(smHandle);
            return result;
        } else {
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }
    }

    private int unloadSM(String smName) {
        int result;
        if (null == smName) {
            LogUtils.e("unloadSM: invalid input param smName = " + smName);
            result = SvaApiConst.RET_CODE_INVALID_PARAM;
            return result;
        }

        int smHandle = SmSessionManager.getInstance().getSmHandle(smName);
        if (-1 == smHandle) {
            LogUtils.e("unloadSM: fail for no match smHandle");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        // if the SM has been unloaded, return success directly
        if (SmSession.SmState.UNLOADED
                == SmSessionManager.getInstance().getSessionState(smHandle)) {
            LogUtils.e("unloadSM: smName = " + smName + " already has been unloaded");
            result = SvaApiConst.RET_CODE_OK;
            return result;
        }

        boolean bCanTransit
                = SmSessionManager.getInstance().isStateTransitional(smHandle,
                SmSession.SmState.UNLOADED);
        if (!bCanTransit) {
            LogUtils.e("unloadSM: fail for wrong state");
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }

        int ret = mSTModule.unloadSoundModel(smHandle);
        LogUtils.d("unloadSM: mSTModule.unloadSoundModel return value = " + ret);
        if (SoundTrigger.STATUS_OK == ret) {
            result = SvaApiConst.RET_CODE_OK;
            SmSessionManager.getInstance().addUnloadedSmSession(smHandle);
            return result;
        } else {
            result = SvaApiConst.RET_CODE_FAILURE;
            return result;
        }
    }

    private void detachModule() {
        LogUtils.d("detachModule: enter");
        if (null != mSTModule) {
            LogUtils.d("detachModule: mSTModule.detach() is called");
            mSTModule.detach();
            mSTModule = null;
        }
        LogUtils.d("detachModule: exit");
    }
}
