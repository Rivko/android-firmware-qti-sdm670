/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.gsma.services.nfc;

import android.content.Context;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.drawable.Drawable;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import android.nfc.cardemulation.CardEmulation;

public final class AidGroup
{
    private static final String TAG = "GsmaAidGroup";
    final static boolean DBG = true;
    String mDescription;
    String mCategory;
    ArrayList<String> mAidList;

    protected AidGroup (String description, String category)
    {
        if (DBG) Log.d(TAG, "AidGroup():" + description + ", " + category);
        mDescription = description;
        mCategory = category;
        mAidList = new ArrayList<String>();
    }

    // Add a new AID to the current group.
    public void addNewAid(String aid)
    {
        if (DBG) Log.d(TAG, "addNewAid():" + aid.toUpperCase());
        if (!(CardEmulation.isValidAid(aid))) {
            throw new IllegalArgumentException("Ilegal Argument passed");
        }
        mAidList.add(aid.toUpperCase());
    }

    //Return the category of the group of AIDs.
    public String getCategory()
    {
        if (DBG) Log.d(TAG, "getCategory()");
        return mCategory;
    }

    //Return the description of the group of AIDs
    public String getDescription ()
    {
        if (DBG) Log.d(TAG, "getDescription()");
        return mDescription;
    }

    //Remove an AID from the current group.
    public void removeAid(String aid)
    {
        if (DBG) Log.d(TAG, "removeAid():" + aid);
        if (!(CardEmulation.isValidAid(aid))) {
            throw new IllegalArgumentException("Ilegal Argument passed");
        }
        mAidList.remove(aid);
    }

    //undocumented method
    //return AIDs from the current group.
    public String[] getAids()
    {
        if (DBG) Log.d(TAG, "getAids()");
        String[] aids = new String[mAidList.size()];
        return mAidList.toArray(aids);
    }
}
