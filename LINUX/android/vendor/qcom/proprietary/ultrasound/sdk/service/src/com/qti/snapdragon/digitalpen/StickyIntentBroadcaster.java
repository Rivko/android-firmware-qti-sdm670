/*===========================================================================
                           StickyIntentBroadcaster.java

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

package com.qti.snapdragon.digitalpen;

import java.util.ArrayList;
import java.util.List;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.UserHandle;

public class StickyIntentBroadcaster {

    private Context context;
    private boolean bootCompleted;
    private List<Intent> queuedIntents = new ArrayList<Intent>();
    private BroadcastReceiver bootCompletedReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            synchronized (StickyIntentBroadcaster.this) {
                bootCompleted = true;
                for (Intent queuedIntent : queuedIntents) {
                    broadcast(queuedIntent);
                }
                context.unregisterReceiver(this);
            }
        }
    };

    public StickyIntentBroadcaster(Context context) {
        this.context = context;
        context.registerReceiver(bootCompletedReceiver, new IntentFilter(
                Intent.ACTION_BOOT_COMPLETED));
    }

    synchronized public void broadcastAfterBootCompleted(Intent intent) {
        if (bootCompleted) {
            broadcast(intent);
        } else {
            queuedIntents.add(intent);
        }
    }

    private void broadcast(Intent intent) {
        context.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

}
