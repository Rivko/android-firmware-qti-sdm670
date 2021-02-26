/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class BootServer extends BroadcastReceiver{
	private final String TAG = "BootServer";
	private static ServerAPI mServerApi = null;
	private static WebServerAPI mWebServerApi = null;

	public void onReceive(Context context, Intent intent) {
		Log.d(TAG,"MDTP BootServer: on recieve");
		mServerApi = ServerAPI.getInstance(context);
		mWebServerApi = WebServerAPI.getInstance(context);
		MdtpStateDemo state = mServerApi.getDeviceState();
		if (intent.getAction().equalsIgnoreCase(Intent.ACTION_BOOT_COMPLETED)) {
			Log.d(TAG,"MDTP: ACTION_BOOT_COMPLETED");
			if(mWebServerApi.getLoadServerOnBoot() || mServerApi.getLoadServerOnBoot()){
				/** Start Server **/
				Intent serviceIntent = new Intent(context, ServerService.class);
				context.startService(serviceIntent);
			}
			/** Update status to server if needed **/
			if(mServerApi.isServerConnectionEstablished()){
				Log.d(TAG,"MDTP: connection has been established, need to update server");
				Intent updateOnBootIntent = new Intent(context, UpdateOnBootService.class);
				context.startService(updateOnBootIntent);
			}else{
				Log.d(TAG,"MDTP: server connection off - deactivated or unregistered");
				/** Start Client's poller if mdtp is activated **/
				if(state.mSystemState == MdtpStateDemo.MdtpSystemStateDemo.ACTIVE){
					Log.d(TAG,"MDTP: Mdtp is activated");
					Intent clientListenetrIntent = new Intent(context, ClientService.class);
					context.startService(clientListenetrIntent);
				}else{
					Log.d(TAG,"MDTP: Mdtp is not activated");
				}
			}

		}

	}

}
