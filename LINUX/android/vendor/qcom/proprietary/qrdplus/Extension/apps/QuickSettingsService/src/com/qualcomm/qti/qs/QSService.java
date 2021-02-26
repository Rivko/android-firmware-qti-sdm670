/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.qs;

import android.app.Service;
import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.os.IBinder;
import android.os.Binder;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qualcomm.qti.qs.tiles.ApnTile;
import com.qualcomm.qti.qs.tiles.DataTile;
import com.qualcomm.qti.qs.tiles.DdsTile;
import com.qualcomm.qti.qs.tiles.QSTile;
import com.qualcomm.qti.qs.tiles.RoamingTile;

import java.util.ArrayList;
import java.util.List;

public class QSService extends Service
{
    private Context mContext;
    private List<QSTile> mQSTiles;
    private BroadcastReceiver mReceiver;
    private ConnectivityManager mConnectivityManager;
    private TelephonyManager mTelephonyManager;
    private final String TAG = "QSService";
    private final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    private List<String> loadTileIntents() {
        String tileList = mContext.getString(R.string.qs_intents);
        ArrayList<String> tiles = new ArrayList<String>();
        for (String tile : tileList.split(",")) {
            tile = tile.trim();
            if (tile.isEmpty()) continue;
            tiles.add(tile);
        }
        return tiles;
    }

    private QSTile createTile(String action) {
        if (action.endsWith("apntile")) return new ApnTile(mContext, action);
        else if (action.endsWith("ddstile") && mTelephonyManager.isMultiSimEnabled())
            return new DdsTile(mContext, action);
        else if (action.endsWith("roamingtile")) return new RoamingTile(mContext, action);
        else if (action.endsWith("datatile") &&
                        mConnectivityManager.isNetworkSupported(ConnectivityManager.TYPE_MOBILE))
            return new DataTile(mContext, action);
        else Log.w(TAG, "bad tile action: " + action);

        return null;
    }

    @Override
    public void onCreate()
    {
        mContext = getApplicationContext();
        mQSTiles = new ArrayList<QSTile>();
        mConnectivityManager = (ConnectivityManager)
                mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        mTelephonyManager = (TelephonyManager)
                mContext.getSystemService(Context.TELEPHONY_SERVICE);
        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if(Intent.ACTION_LOCALE_CHANGED.equals(intent.getAction())){
                    for (QSTile tile : mQSTiles) {
                        tile.handleUpdateState();
                    }
                }else {
                    for (QSTile tile : mQSTiles) {
                        if((intent.getAction()).
                                equals(tile.getOnClickAction())){
                            tile.handleClick();
                        }
                    }
               }
            }};
    }

    @Override
    public int onStartCommand (Intent intent, int flags, int startId)
    {
        for (String action : loadTileIntents()) {
             QSTile tile = createTile(action);
             if(tile != null) {
                 mQSTiles.add(tile);
             }
        }

        IntentFilter filter = new IntentFilter();
        for (QSTile tile : mQSTiles) {
            tile.handleUpdateState();
            filter.addAction(tile.getOnClickAction());
        }
        filter.addAction(Intent.ACTION_LOCALE_CHANGED);

        mContext.registerReceiver(mReceiver, filter);

        return Service.START_REDELIVER_INTENT;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // We don't provide binding, so return null
        return null;
    }

    @Override
    public void onDestroy() {
        mContext.unregisterReceiver(mReceiver);
    }
}
