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

public class ClientService extends Service {

	/** Log output **/
	private final String TAG = "ClientService";

	/** Off screen poller Thread **/
	private Thread clientPoller;

	private ServerAPI mServerApi = null;

	@Override
	public int onStartCommand (Intent intent, int flags, int startId){
		Log.d(TAG, "Client Service onStartCommand");
		mServerApi = ServerAPI.getInstance(getBaseContext());
		startPollingServer();
		return START_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	void startPollingServer(){
		Log.d(TAG, "MDTP: startPollingServer");
		Runnable r = new Runnable(){
			@Override
			public void run() {
				while(true){
					try {
						if (mServerApi.getDeviceState().mSystemState != MdtpStateDemo.MdtpSystemStateDemo.ACTIVE){
							Log.d(TAG, "MDTP: startPollingServer - stopping service");
							stopSelf();
							break;
						}
						MdtpApiSingleton.getInstance().lockAPI(getBaseContext());
						/** Bound to Mdtp service since app will unboud from it on exit **/
						if (!MdtpApiSingleton.getInstance().isInitialized())
							MdtpApiSingleton.getInstance().start(getBaseContext());
						if (!MdtpApiSingleton.getInstance().isBound())
							MdtpApiSingleton.getInstance().bind(getBaseContext());

						/** Check if application has successfully bound to MDTP service */
						int i;
						/** wait for 50*100 (5 seconds) **/
						final int maxWaitIter = 50;
						for (i = 0; (!MdtpApiSingleton.getInstance().isBound()) && (i < maxWaitIter); i++) {
							Log.d(TAG, "Waiting for server bind...");
							Thread.sleep(100);
						}
						/** Could not bound to service */
						if (i == maxWaitIter) {
							MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
							Log.d(TAG, "Timeout while binding to the service");
							continue;
						}
						Log.d(TAG, "Succeffully binded to the service");

						/** Poll server **/
						MdtpMessage MsgHeader = new MdtpMessage("PollServer", "Client", "Server");
						MdtpMessage.PollServer pollMsg = MsgHeader.new PollServer();
						Log.d(TAG, "MDTP: polling server...");
						mServerApi.sendMessageToServer(MsgHeader, pollMsg, 60000);

						/** Poll for pending messages every 15 secs **/
						MdtpApiSingleton.getInstance().unlockAPI(getBaseContext());
						Thread.sleep(15000);
					} catch (Exception e) {
						throw new RuntimeException();
					}

				}

			}
		};

		clientPoller = new Thread(r);
		clientPoller.start();
	}
}
