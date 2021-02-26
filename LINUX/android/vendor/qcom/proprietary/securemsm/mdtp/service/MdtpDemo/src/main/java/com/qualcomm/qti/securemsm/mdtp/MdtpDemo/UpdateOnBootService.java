/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class UpdateOnBootService extends Service {

	/** Log output **/
	private final String TAG = "UpdateOnBootService";

	/** Off screen poller Thread **/
	private Thread helperThread;

	private ServerAPI mServerApi = null;

	@Override
	public int onStartCommand (Intent intent, int flags, int startId){
		Log.d(TAG, "UpdateOnBootService onStartCommand");
		mServerApi = ServerAPI.getInstance(getBaseContext());
		updateServer();
		return START_NOT_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	void updateServer(){
		Log.d(TAG, "MDTP: updateServer");

		Runnable r = new Runnable(){
			@Override
			public void run() {
				/** Bound to Mdtp service since app will unboud from it on exit **/
				MdtpState state = new MdtpState();
				try{
					MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
					MdtpApiSingleton.getInstance().start(getBaseContext());
					Log.d(TAG, "Mdtp singelton start executed...");
					MdtpApiSingleton.getInstance().bind(getBaseContext());
					Log.d(TAG, "Mdtp singelton bind executed...");

					/** Check if application has successfully bound to MDTP service */
					int i;
					// wait for 50*100 (5 seconds) - service initialization can take time on boot
					final int maxWaitIter = 50;
					for (i = 0; (MdtpApiSingleton.getInstance().isBound() == false) && (i < maxWaitIter); i++) {
						Log.d(TAG, "Waiting for server bind...");
						Thread.sleep(100);
					}
					/** not bound to service */
					if (i == maxWaitIter) {
						Log.d(TAG, "Timeout while binding to the service");
						throw new RuntimeException();
					}
					Log.d(TAG, "Succeffully binded to the service");

					/** Update server for status **/
					MdtpMessage MsgHeader = new MdtpMessage("UpdateStatus", "Client", "Server");
					MdtpMessage.UpdateStatus updateStatus = MsgHeader.new UpdateStatus("Client", "Server");
					state = MdtpApiSingleton.getInstance().getState();
					Log.d(TAG, "Got mdtp status after bootloader...");
					updateStatus.setStatus(new MdtpStateDemo(state));

					String retval;
					// 100 Chances to reach server
					int count = 100;
					do{
						Thread.sleep(200);
						if(--count == 0){
							Log.d(TAG, "No more chances to reach server...");
							break;
						}
						retval = mServerApi.sendMessageToServer(MsgHeader, updateStatus, 60000);
					} while(retval.equals("Network Error"));

					MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
				} catch (Exception e) {
					Log.d(TAG, "Exception while accesing the service " + e.toString());
					throw new RuntimeException();
				}
				if(state.mSystemState == MdtpSystemState.ACTIVE){
					Log.d(TAG,"MDTP: Mdtp is activated");
					Intent clientListenetrIntent = new Intent(UpdateOnBootService.this, ClientService.class);
					startService(clientListenetrIntent);
				}else{
					Log.d(TAG,"MDTP: Mdtp is not activated");
				}
			}
		};
		helperThread = new Thread(r);
		helperThread.start();
	}
}
