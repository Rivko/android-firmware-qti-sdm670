/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import android.content.*;
import android.os.Environment;
import android.util.Log;

public class BootupReceiver extends BroadcastReceiver {
	private final static String TAG = "BootupReceiver";

	@Override
	public void onReceive(Context context, Intent intent) {
		Log.v(TAG, "Voicewakeup autostartup");
		new Thread(new StartRunnable(context)).start();
	}

	private class StartRunnable implements Runnable {
		private final Context mContext;

		public StartRunnable(Context context) {
			mContext = context;
		}

		public void run() {
			while(true) {
				String state = Environment.getExternalStorageState();
				Log.v(TAG, "getExternalStorageState ... - " + state);
				if(Environment.MEDIA_CHECKING.equals(state)) {
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				} else {
					Intent intent = new Intent("com.qualcomm.qti.sva.VwuService.class");
					intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
					//mContext.startService(intent);
					break;
				}
			}
		}
	}

}
