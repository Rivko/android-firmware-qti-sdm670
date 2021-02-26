/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.uicccontact;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.UserHandle;
import android.text.TextUtils;
import android.util.SparseArray;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.uicc.AdnRecord;
import com.android.internal.telephony.uicc.IccRefreshResponse;
import com.qualcomm.qti.internal.telephony.QtiRilInterface;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * {@hide}
 */
public final class QtiSimPhoneBookAdnRecordCache extends Handler {
    //***** Instance Variables
    private static final String LOG_TAG = "QtiSimPhoneBookAdnRecordCache";
    private static final boolean DBG = true;
    // member variables
    protected final CommandsInterface mCi;
    protected int mPhoneId;
    protected Context mContext;
    //max adn count
    private int mAdnCount = 0;
    //valid adn count
    private int mValidAdnCount = 0;
    private int mEmailCount = 0;
    private int mValidEmailCount = 0;
    private int mAddNumCount = 0;
    private int mValidAddNumCount = 0;
    private int mMaxNameLen = 0;
    private int mMaxNumberLen = 0;
    private int mMaxEmailLen = 0;
    private int mMaxAnrLen = 0;
    private int mRecCount = 0;
    private Object mLock = new Object();
    private ArrayList<AdnRecord> mSimPbRecords;
    private boolean mRefreshAdnCache = false;
    private QtiRilInterface mQtiRilInterface;

    // People waiting for ADN-like files to be loaded
    ArrayList<Message> mAdnLoadingWaiters = new ArrayList<Message>();

    // People waiting for adn record to be updated
    Message mAdnUpdatingWaiter = null;

    //EXT file Used/free records.
    SparseArray<int[]> extRecList = new SparseArray<int[]>();

    //***** Event Constants
    static final int EVENT_INIT_ADN_DONE = 1;
    static final int EVENT_QUERY_ADN_RECORD_DONE = 2;
    static final int EVENT_LOAD_ADN_RECORD_DONE = 3;
    static final int EVENT_LOAD_ALL_ADN_LIKE_DONE = 4;
    static final int EVENT_UPDATE_ADN_RECORD_DONE = 5;
    static final int EVENT_SIM_REFRESH = 6;

    //***** Constructor
    public QtiSimPhoneBookAdnRecordCache(Context context,  int phoneId,  CommandsInterface ci) {
        mCi = ci;
        mSimPbRecords = new ArrayList<AdnRecord>();
        mPhoneId = phoneId;
        mContext = context;

        mQtiRilInterface = QtiRilInterface.getInstance(context);
        mQtiRilInterface.registerForAdnInitDone(this, EVENT_INIT_ADN_DONE, null);
        mCi.registerForIccRefresh(this, EVENT_SIM_REFRESH, null);

        IntentFilter intentFilter = new IntentFilter(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        context.registerReceiver(sReceiver, intentFilter);
    }

    public void reset() {
        mAdnLoadingWaiters.clear();
        clearUpdatingWriter();

        mSimPbRecords.clear();
        mRecCount = 0;
        mRefreshAdnCache = false;
    }

    private void clearUpdatingWriter() {
        sendErrorResponse(mAdnUpdatingWaiter, "QtiSimPhoneBookAdnRecordCache reset");
        mAdnUpdatingWaiter = null;
    }

    private void sendErrorResponse(Message response, String errString) {
        if (response != null) {
            Exception e = new RuntimeException(errString);
            AsyncResult.forMessage(response).exception = e;
            response.sendToTarget();
        }
    }

    private void
    notifyAndClearWaiters() {
        if (mAdnLoadingWaiters == null) {
            return;
        }

        for (int i = 0, s = mAdnLoadingWaiters.size() ; i < s ; i++) {
            Message response = mAdnLoadingWaiters.get(i);

            if ((response != null)) {
                AsyncResult.forMessage(response).result = mSimPbRecords;
                response.sendToTarget();
            }
        }

        mAdnLoadingWaiters.clear();
    }

    public void queryAdnRecord () {
        mRecCount = 0;
        mAdnCount = 0;
        mValidAdnCount = 0;
        mEmailCount = 0;
        mAddNumCount = 0;

        log("start to queryAdnRecord");

        mQtiRilInterface.registerForAdnRecordsInfo(this, EVENT_LOAD_ADN_RECORD_DONE, null);
        mQtiRilInterface.getAdnRecord(
                obtainMessage(EVENT_QUERY_ADN_RECORD_DONE),
                mPhoneId);

        try {
            mLock.wait();
        } catch (InterruptedException e) {
            Rlog.e(LOG_TAG, "Interrupted Exception in queryAdnRecord");
        }

        mQtiRilInterface.unregisterForAdnRecordsInfo(this);
    }


    public void
    requestLoadAllAdnLike(Message response) {
        if (!mQtiRilInterface.isServiceReady()) {
            log("Oem hook service is not ready yet ");
            sendErrorResponse(response, "Oem hook service is not ready yet");
            return;
        }

        if (mAdnLoadingWaiters != null) {
            mAdnLoadingWaiters.add(response);
        }

        synchronized (mLock) {
            if (!mSimPbRecords.isEmpty()) {
                log("ADN cache has already filled in");
                if (mRefreshAdnCache) {
                    mRefreshAdnCache = false;
                    refreshAdnCache();
                } else {
                    notifyAndClearWaiters();
                }

                return;
            }

            queryAdnRecord();
        }
    }

    public void updateSimPbAdnBySearch(AdnRecord oldAdn, AdnRecord newAdn, Message response) {
        ArrayList<AdnRecord> oldAdnList = mSimPbRecords;

        if (!mQtiRilInterface.isServiceReady()) {
            log("Oem hook service is not ready yet ");
            sendErrorResponse(response, "Oem hook service is not ready yet");
            return;
        }

        synchronized (mLock) {
            if (!mSimPbRecords.isEmpty()) {
                log("ADN cache has already filled in");
                if (mRefreshAdnCache) {
                    mRefreshAdnCache = false;
                    refreshAdnCache();
                }
            } else {
                queryAdnRecord();
            }
        }

        if (oldAdnList == null) {
            sendErrorResponse(response, "Sim PhoneBook Adn list not exist");
            return;
        }

        int index = -1;
        int count = 1;
        if (oldAdn.isEmpty() && !newAdn.isEmpty()) {
            //add contact
            index = 0;
        } else {
            //delete or update contact
            for (Iterator<AdnRecord> it = oldAdnList.iterator(); it.hasNext();) {
                if (oldAdn.isEqual(it.next())) {
                    index = count;
                    break;
                }
                count++;
            }
        }
        if (index == -1) {
            sendErrorResponse(response, "Sim PhoneBook Adn record don't exist for " + oldAdn);
            return;
        }

        if(index == 0 && mValidAdnCount == mAdnCount) {
            sendErrorResponse(response, "Sim PhoneBook Adn record is full");
            return;
        }

        int recordIndex = (index == 0) ? 0 : oldAdnList.get(index-1).getRecordNumber();

        QtiSimPhoneBookAdnRecord updateAdn = new QtiSimPhoneBookAdnRecord();
        updateAdn.mRecordIndex = recordIndex;
        updateAdn.mAlphaTag = newAdn.getAlphaTag();
        updateAdn.mNumber = newAdn.getNumber();
        if(newAdn.getEmails() != null) {
            updateAdn.mEmails = newAdn.getEmails();
            updateAdn.mEmailCount = updateAdn.mEmails.length;
        }
        if(newAdn.getAdditionalNumbers() != null) {
            updateAdn.mAdNumbers = newAdn.getAdditionalNumbers();
            updateAdn.mAdNumCount = updateAdn.mAdNumbers.length;
        }

        if (mAdnUpdatingWaiter != null) {
            sendErrorResponse(response, "Have pending update for Sim PhoneBook Adn");
            return;
        }

        mAdnUpdatingWaiter = response;

        mQtiRilInterface.updateAdnRecord(
                    updateAdn,
                    obtainMessage(EVENT_UPDATE_ADN_RECORD_DONE, index, 0, newAdn),
                    mPhoneId);
    }

    private final BroadcastReceiver sReceiver = new  BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                                            SubscriptionManager.INVALID_SIM_SLOT_INDEX);
                String simStatus = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                if (IccCardConstants.INTENT_VALUE_ICC_ABSENT.equals(simStatus)
                && mPhoneId == phoneId) {
                    log("ACTION_SIM_STATE_CHANGED intent received simStatus: " + simStatus
            + "phoneId: " + phoneId);
                    invalidateAdnCache();
                }
            }
        }
    };

    //***** Overridden from Handler

    @Override
    public void
    handleMessage(Message msg) {
        AsyncResult ar = (AsyncResult)msg.obj;
        int efid;

        switch(msg.what) {
            case EVENT_INIT_ADN_DONE:
                ar = (AsyncResult)msg.obj;
                log("Initialized ADN done");
                if (ar.exception == null) {
                    invalidateAdnCache();
                } else {
                    log("Init ADN done Exception: " + ar.exception);
                }

                break;

            case EVENT_QUERY_ADN_RECORD_DONE:
                log("Querying ADN record done");
                if (ar.exception != null) {
                    synchronized (mLock) {
                        mLock.notify();
                    }

                    for (Message response : mAdnLoadingWaiters) {
                        sendErrorResponse(response, "Query adn record failed" + ar.exception);
                    }
                    mAdnLoadingWaiters.clear();
                    break;
                }
                mAdnCount = ((int[]) (ar.result))[0];
                mValidAdnCount = ((int[]) (ar.result))[1];
                mEmailCount = ((int[]) (ar.result))[2];
                mValidEmailCount = ((int[]) (ar.result))[3];
                mAddNumCount = ((int[]) (ar.result))[4];
                mValidAddNumCount = ((int[]) (ar.result))[5];
                mMaxNameLen = ((int[]) (ar.result))[6];
                mMaxNumberLen = ((int[]) (ar.result))[7];
                mMaxEmailLen = ((int[]) (ar.result))[8];
                mMaxAnrLen = ((int[]) (ar.result))[9];
                log("Max ADN count is: " + mAdnCount
                    + ", Valid ADN count is: " + mValidAdnCount
                    + ", Email count is: " + mEmailCount
                    + ", Valid email count is: " + mValidEmailCount
                    + ", Add number count is: " + mAddNumCount
                    + ", Valid add number count is: " + mValidAddNumCount
                    + ", Max name length is: " + mMaxNameLen
                    + ", Max number length is: " + mMaxNumberLen
                    + ", Max email length is: " + mMaxEmailLen
                    + ", Valid anr length is: " + mMaxAnrLen);

                if(mValidAdnCount == 0 || mRecCount == mValidAdnCount) {
                    sendMessage(obtainMessage(EVENT_LOAD_ALL_ADN_LIKE_DONE));
                }
                break;

            case EVENT_LOAD_ADN_RECORD_DONE:
                log("Loading ADN record done");
                if (ar.exception != null) {
                    break;
                }

                QtiSimPhoneBookAdnRecord[] AdnRecordsGroup = (QtiSimPhoneBookAdnRecord[])(ar.result);
                for (int i = 0 ; i < AdnRecordsGroup.length ; i++) {
                    if (AdnRecordsGroup[i] != null) {
                        mSimPbRecords.add(new AdnRecord(0,
                                                AdnRecordsGroup[i].getRecordIndex(),
                                                AdnRecordsGroup[i].getAlphaTag(),
                                                AdnRecordsGroup[i].getNumber(),
                                                AdnRecordsGroup[i].getEmails(),
                                                AdnRecordsGroup[i].getAdNumbers()));
                        mRecCount ++;
                    }
                }

                if(mRecCount == mValidAdnCount) {
                    sendMessage(obtainMessage(EVENT_LOAD_ALL_ADN_LIKE_DONE));
                }
                break;

            case EVENT_LOAD_ALL_ADN_LIKE_DONE:
                log("Loading all ADN records done");
                synchronized (mLock) {
                    mLock.notify();
                }

                notifyAndClearWaiters();
                break;

            case EVENT_UPDATE_ADN_RECORD_DONE:
                log("Update ADN record done");
                Exception e = null;

                if (ar.exception == null) {
                    int index = msg.arg1;
                    AdnRecord adn = (AdnRecord) (ar.userObj);
                    int recordIndex = ((int[]) (ar.result))[0];

                    if(index == 0) {
                        //add contact
                        log("Record number for added ADN is " + recordIndex);
                        adn.setRecordNumber(recordIndex);
                        mSimPbRecords.add(adn);
                        mValidAdnCount ++;
                    } else if (adn.isEmpty()){
                        //delete contact
                        int adnRecordIndex = mSimPbRecords.get(index - 1).getRecordNumber();
                        log("Record number for deleted ADN is " + adnRecordIndex);
                        if(recordIndex == adnRecordIndex) {
                            mSimPbRecords.remove(index - 1);
                            mValidAdnCount --;
                        } else {
                            e = new RuntimeException(
                                "The index for deleted ADN record did not match");
                        }
                    } else {
                        //Change contact
                        int adnRecordIndex = mSimPbRecords.get(index - 1).getRecordNumber();
                        log("Record number for changed ADN is " + adnRecordIndex);
                        if(recordIndex == adnRecordIndex) {
                            adn.setRecordNumber(recordIndex);
                            mSimPbRecords.set(index - 1, adn);
                        } else {
                            e = new RuntimeException(
                                "The index for changed ADN record did not match");
                        }
                    }
                } else {
                   e = new RuntimeException("Update adn record failed",
                                ar.exception);
                }

                if (mAdnUpdatingWaiter != null) {
                    AsyncResult.forMessage(mAdnUpdatingWaiter, null, e);
                    mAdnUpdatingWaiter.sendToTarget();
                    mAdnUpdatingWaiter = null;
                }
                break;

            case EVENT_SIM_REFRESH:
                ar = (AsyncResult)msg.obj;
                log("SIM REFRESH occurred");
                if (ar.exception == null) {
                    IccRefreshResponse refreshRsp = (IccRefreshResponse)ar.result;
                    if (refreshRsp == null) {
                        if (DBG) log("IccRefreshResponse received is null");
                        break;
                    }

                    if(refreshRsp.refreshResult == IccRefreshResponse.REFRESH_RESULT_FILE_UPDATE ||
                        refreshRsp.refreshResult == IccRefreshResponse.REFRESH_RESULT_INIT) {
                          invalidateAdnCache();
                    }
                } else {
                    log("SIM refresh Exception: " + ar.exception);
                }
                break;
        }

    }

    public int getAdnCount() {
        return mAdnCount;
    }

    public int getUsedAdnCount() {
        return mValidAdnCount;
    }

    public int getEmailCount() {
        return mEmailCount;
    }

    public int getUsedEmailCount() {
        return mValidEmailCount;
    }

    public int getAnrCount() {
        return mAddNumCount;
    }

    public int getUsedAnrCount() {
        return mValidAddNumCount;
    }

    public int getMaxNameLen() {
        return mMaxNameLen;
    }

    public int getMaxNumberLen() {
        return mMaxNumberLen;
    }

    public int getMaxEmailLen() {
        return mMaxEmailLen;
    }

    public int getMaxAnrLen() {
        return mMaxAnrLen;
    }

    private void log(String msg) {
        if(DBG) Rlog.d(LOG_TAG, msg);
    }

    public void invalidateAdnCache() {
        log("invalidateAdnCache");
        mRefreshAdnCache = true;
    }

    private void refreshAdnCache() {
        log("refreshAdnCache");
        mSimPbRecords.clear();
        queryAdnRecord();
    }
}
