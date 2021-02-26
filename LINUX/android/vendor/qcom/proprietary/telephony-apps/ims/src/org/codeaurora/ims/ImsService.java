/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import com.android.internal.telephony.ims.ImsResolver;
import com.qualcomm.ims.vt.ImsVideoGlobals;

import org.codeaurora.ims.QtiImsExtManager;
import org.codeaurora.ims.utils.QtiCarrierConfigHelper;
import com.qualcomm.ims.utils.Log;

import android.content.Intent;
import android.os.IBinder;
import android.os.ServiceManager;
import android.telephony.TelephonyManager;
import android.telephony.ims.feature.MMTelFeature;
import android.telephony.ims.feature.RcsFeature;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.ims.ImsResolver;
import com.qualcomm.ims.utils.Log;
import com.qualcomm.ims.vt.ImsVideoGlobals;

import org.codeaurora.ims.utils.QtiImsExtUtils;

import java.util.ArrayList;
import java.util.List;

public class ImsService extends android.telephony.ims.ImsService {

    private static final String LOG_TAG = "ImsService";
    private static final int INVALID_SLOT_ID = -1;
    private static final int UNINITIALIZED_VALUE = -1;

    private ImsServiceSub mServiceSub[];
    private ImsSenderRxr mSenderRxrs[];
    // Slot id --> table
    private List<ImsServiceSub> mServiceSubSlotMap = new ArrayList<>();

    private int mNumPhonesCache = UNINITIALIZED_VALUE;
    private ImsSubController mSubController;
    private QtiImsExt mQtiImsExt;

    private int getNumSlots() {
        if (mNumPhonesCache == UNINITIALIZED_VALUE) {
            mNumPhonesCache = TelephonyManager.getDefault().getPhoneCount();
        }
        return mNumPhonesCache;
    }

    private void setupImsSubscriptionMap() {
        final int numSlots = getNumSlots();
        Log.i(this, "setupImsSubscriptionMap :: numSlots=" + numSlots);
        for (int i = 0; i < numSlots; i++) {
            mServiceSubSlotMap.add(i, mServiceSub[i]);
        }
    }

    private void setup() {
        QtiCarrierConfigHelper.getInstance().setup(this);
        final int numSlots = getNumSlots();
        mServiceSub = new ImsServiceSub[numSlots];
        mSenderRxrs = new ImsSenderRxr[numSlots];
        for (int i = 0; i < numSlots; i++) {
            mSenderRxrs[i] = new ImsSenderRxr(this, i);
        }
        mSubController = new ImsSubController(this, mSenderRxrs);
        for (int i = 0; i < numSlots; i++) {
            Phone phone = PhoneFactory.getPhone(i);
            mServiceSub[i] = new ImsServiceSub(this, i, mSenderRxrs[i], mSubController, phone.mCi);
        }
        setupImsSubscriptionMap();

        // TODO: Support DSDA devices.
        final int defaultSub = 0;
        ImsVideoGlobals.init(mServiceSubSlotMap.get(defaultSub), this);
        // TODO: For multi-IMS stacks, ImsVideoGlobals needs to be refactored
        //       to use the correct ImsServicSub.

        // Initialize the QtiImsExt
        mQtiImsExt = new QtiImsExt(this, mServiceSub);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i (this, "ImsService created!");
        setup();

        // Adding QtiImsExtBinder to ServiceManager
        ServiceManager.addService(QtiImsExtManager.SERVICE_ID, mQtiImsExt.getBinder());
        // NOTE: Since there is no explicit 'removeService()' API in ServiceManager,
        // the clients of the QTI IMS Service will need to gracefully handle the
        // QTI binder being null, when onDestroy() is called. They would need to
        // call the linkToDeath method.
    }

    @Override
    public IBinder onBind(Intent intent) {
        if (ImsResolver.SERVICE_INTERFACE.equals(intent.getAction())) {
            Log.d(this, "Returning mImsServiceController for ImsService binding");
            return mImsServiceController;
        }
        Log.e(this, "Invalid Intent action in onBind: " + intent.getAction());
        return null;
    }

    @Override
    public MMTelFeature onCreateEmergencyMMTelImsFeature(int slotId) {
        if(slotId > INVALID_SLOT_ID && slotId < getNumSlots()) {
            return mServiceSubSlotMap.get(slotId);
        }
        return null;
    }

    @Override
    public MMTelFeature onCreateMMTelImsFeature(int slotId) {
        if(slotId > INVALID_SLOT_ID && slotId < getNumSlots()) {
            return mServiceSubSlotMap.get(slotId);
        }
        return null;
    }

    @Override
    public RcsFeature onCreateRcsFeature(int slotId) {
        // Not supported.
        return null;
    }

    @Override
    public void onDestroy() {
        Log.i(this, "Ims Service Destroyed Successfully...");
        for (int i = 0; i < getNumSlots(); i++) {
            mServiceSub[i].dispose();
            mServiceSub[i] = null;
            mSenderRxrs[i] = null;
        }
        mServiceSubSlotMap.clear();
        mServiceSub = null;
        mSenderRxrs = null;
        mSubController = null;
        QtiCarrierConfigHelper.getInstance().teardown();
        super.onDestroy();
    }
}
