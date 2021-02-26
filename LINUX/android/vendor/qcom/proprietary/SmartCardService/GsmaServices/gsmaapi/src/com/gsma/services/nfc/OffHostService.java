/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.gsma.services.nfc;

import java.util.List;
import java.util.ArrayList;
import java.lang.Class;
import java.lang.reflect.Field;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.drawable.*;
import android.content.Context;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.drawable.Drawable;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import com.gsma.services.nfc.AidGroup;
import com.gsma.services.utils.InsufficientResourcesException;
import android.content.pm.PackageManager.NameNotFoundException;

public final class OffHostService
{
    private static final String TAG = "GsmaOffHostService";
    final static boolean DBG = true;

    String mDescription;
    String mSeName;
    NfcController mNfcController;
    ArrayList<AidGroup> mAidGroupList;
    Drawable mBanner;
    int mBannerResId;

    OffHostService (String description, String SEName, NfcController nfcController)
    {
        if (DBG) Log.d(TAG, "OffHostService():" + description + ", " + SEName);
        mDescription = description;
        mSeName = SEName;
        mBannerResId = 0x00;
        mNfcController = nfcController;
        mAidGroupList = new ArrayList<AidGroup>();
    }

    // Update the Android Framework with all pending updates.
    public void commit() throws InsufficientResourcesException
    {
        boolean status = false;
        if (DBG) Log.d(TAG, "commit()");
        status= mNfcController.commitOffHostService(this);
        if(status == false)
            throw new InsufficientResourcesException("Routing Table is Full, Cannot Commit");
    }

    // Create a new empty group of AIDs for the "Off-Host" service.
    public AidGroup defineAidGroup(String description, String category) throws IllegalArgumentException
    {
        if (DBG) Log.d(TAG, "defineAidGroup() " + description + ", " + category);
        try {
            AidGroup aidGroup = new AidGroup (description, category);
            mAidGroupList.add(aidGroup);
            return aidGroup;
        }
        catch (IllegalArgumentException e) {
            Log.d(TAG, "defineAidGroup() IllegalArgumentException..." + e.getMessage());
            throw new IllegalArgumentException("method is invoked with null or illegal argument");
        }
    }

    //Delete an existing AID group from the "Off-Host" service.
    public void  deleteAidGroup(AidGroup group) throws IllegalArgumentException
    {
        if (DBG) Log.d(TAG, "deleteAidGroup() " + group.getDescription() + ", " + group.getCategory());
        try {
            mAidGroupList.remove(group);
        }
        catch (IllegalArgumentException e) {
            Log.d(TAG, "deleteAidGroup() IllegalArgumentException..." + e.getMessage());
            throw new IllegalArgumentException("method is invoked with null or illegal argument");
        }
    }

    //Return a list of the AID groups linked to the "Off-Host" service.
    public AidGroup[] getAidGroups()
    {
        if (DBG) Log.d(TAG, "getAidGroups()");
        AidGroup[] aidGroups = new AidGroup[mAidGroupList.size()];
        return mAidGroupList.toArray(aidGroups);
    }

    //Return the banner linked to the "Off-Host" service.
    public Drawable getBanner ()
    {
        if (DBG) Log.d(TAG, "getBanner()");
        return mBanner;
    }

    //Return the description of the "Off-Host" service.
    public String getDescription ()
    {
        if (DBG) Log.d(TAG, "getDescription()");
        return mDescription;
    }

    //Return the Secure Element name which holds the "Off-Host" service.
    public String getLocation ()
    {
        if (DBG) Log.d(TAG, "getLocation()");
        return mSeName;
    }

    //Set a banner for the "Off-Host" service.
    public void setBanner(Drawable banner) throws IllegalArgumentException
    {
        if (DBG) Log.d(TAG, "setBanner()");
        //mBanner = banner;
        PackageManager pManager = mNfcController.mContext.getPackageManager();
        final String packName = mNfcController.mContext.getPackageName();
        if (DBG) Log.d(TAG, "setBanner() Resources packName: " + packName);
        try {
            for (int i = 0; i < Class.forName(packName + ".R").getClasses().length; i++) {
                if (Class.forName(packName + ".R").getClasses()[i].getName().split("\\$")[1].equals("drawable")) {
                    if (Class.forName(packName + ".R").getClasses()[i] != null) {
                        Field[] f = Class.forName(packName + ".R").getClasses()[i].getDeclaredFields();
                        for (int counter = 0, max = f.length; counter < max; counter++) {
                            int resId = f[counter].getInt(null);
                            Drawable d = pManager.getResourcesForApplication(packName).getDrawable(resId);
                            if ( (d.getConstantState().equals(banner.getConstantState())) ) {
                                mBannerResId = resId;
                                mBanner = d;
                                if (DBG) Log.d(TAG, "setBanner() Resources GOT THE DRAWABLE On loop "
                                               + String.valueOf(counter) + "got resId : " + String.valueOf(resId));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
        catch (IllegalArgumentException e) {
            Log.d(TAG, "setBanner() IllegalArgument exception ..." + e.getMessage());
            throw new IllegalArgumentException("method is invoked with null or illegal argument");
        }
        catch (NameNotFoundException e) {
            Log.d(TAG, "setBanner() NameNotFoundException exception ..." + e.getMessage());
            throw new IllegalArgumentException("method is invoked with null or illegal argument");
        }
        catch (Exception e) {
            Log.d(TAG, "setBanner() other exception ..." + e.getMessage());
        }
        if(mBannerResId == 0x00) {
            Log.d(TAG, "bannerId  set to 0");
            mBannerResId = -1;
            mBanner = banner;
        }

    }

    //Set a banner for the "Off-Host" service.
    public void setBanner(int bannerResId) throws Resources.NotFoundException
    {
        if (DBG) Log.d(TAG, "setBanner() with " + String.valueOf(bannerResId));
        mBannerResId = bannerResId;
        PackageManager pManager = mNfcController.mContext.getPackageManager();
        final String packName = mNfcController.mContext.getPackageName();
        if (mBannerResId > 0) {
            try {
                if (DBG) Log.d(TAG, "setBanner(): getDrawable() with mBannerResId=" + String.valueOf(mBannerResId));
                mBanner = pManager.getResourcesForApplication(packName).getDrawable(mBannerResId);
            }
            catch (Exception e) {
                Log.e(TAG, "Exception : " + e.getMessage());
                throw new Resources.NotFoundException("resourceID does not exist");
            }
        }
    }

    // undocumented API
    //Return the banner linked to the "Off-Host" service.
    protected int getBannerResId ()
    {
        if (DBG) Log.d(TAG, "getBannerResId() returning : " + String.valueOf(mBannerResId));
        return mBannerResId;
    }
}
