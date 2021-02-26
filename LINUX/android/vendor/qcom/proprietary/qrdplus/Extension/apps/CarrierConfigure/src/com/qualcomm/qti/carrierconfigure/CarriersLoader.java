/**
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.carrierconfigure;

import android.content.AsyncTaskLoader;
import android.content.Context;
import android.os.RemoteException;
import android.util.Log;

import com.qualcomm.qti.accesscache.ICarrierAccessCacheService;
import com.qualcomm.qti.carrierconfigure.Carrier.CarriersStorage;
import com.qualcomm.qti.loadcarrier.ILoadCarrierService;

import java.util.HashMap;

public class CarriersLoader extends AsyncTaskLoader<CarriersStorage> {
    // Save the result.
    private CarriersStorage mCarriersStorage = null;

    // Save the state.
    private boolean mParsedPreset = false;
    private boolean mParsedSdCard = false;
    private boolean mParsedCache = false;

    private ILoadCarrierService mService;
    private ICarrierAccessCacheService mAccessCacheService;

    public CarriersLoader(Context context, ILoadCarrierService service,
            ICarrierAccessCacheService accessCacheService) {
        super(context);
        mService = service;
        mAccessCacheService = accessCacheService;
    }

    @Override
    protected void onReset() {
        super.onReset();
        if (mCarriersStorage != null) mCarriersStorage.reset();
        mParsedPreset = false;
        mParsedSdCard = false;
        mParsedCache = false;
    }

    @Override
    protected void onStartLoading() {
        if (mParsedPreset && (mService == null || mParsedSdCard)) {
            // Have a result available, deliver it immediately.
            // deliverResult(mCarriersStorage);
        }

        if (takeContentChanged() || !mParsedPreset || (mService != null && !mParsedSdCard)
                || (mAccessCacheService != null && !mParsedCache)) {
            forceLoad();
        }
    }

    @SuppressWarnings("unchecked")
    @Override
    public CarriersStorage loadInBackground() {
        if (mCarriersStorage == null) {
            mCarriersStorage = new CarriersStorage();
        }

        // Load the preset carriers, and we will load the carriers on the SD card if we get the
        // service changed notify. Then restart loading.
        if (!mParsedPreset) {
             Carrier.preConfigspeccfgFile();
             Carrier.parsePresetCarriers(mCarriersStorage);
             mParsedPreset = true;
        }
        if (!mParsedSdCard && mService != null) {
            // Get the carrier list from SD card maybe expend most time, we will deliver
            // the result first. After get parse finish, it will deliver the result again.
            // deliverResult(mCarriersStorage);

            try {
                Carrier.parseCarriers(
                        (HashMap<String, String>) mService.getSdcardCarrierList(),
                        mCarriersStorage);
                mParsedSdCard = true;
            } catch (RemoteException e) {
                Log.e("CarriersLoader", "Found the LoadCarrierService, but catch RemoteException: "
                        + e.getMessage());
            }
        }
        if (!mParsedCache && mAccessCacheService != null) {
            try {
                Carrier.parseCarriers(
                        (HashMap<String, String>) mAccessCacheService.getCacheCarrierList(),
                        mCarriersStorage);
                mParsedCache = true;
            } catch (RemoteException e) {
                Log.e("CarriersLoader", "Found the AccessCacheService, but catch RemoteException: "
                        + e.getMessage());
            }
        }

        return mCarriersStorage;
    }

    public void notifyCarrierLoadServiceChanged(ILoadCarrierService service,
            ICarrierAccessCacheService accessCacheService) {
        // Update the service value.
        mService = service;
        mAccessCacheService = accessCacheService;

        // Start loading process.
        startLoading();
    }

    public boolean isParsingFinished() {
        return mParsedPreset && mParsedSdCard && mParsedCache;
    }
}
