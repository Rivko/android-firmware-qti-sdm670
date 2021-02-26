/*=============================================================================
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*===========================================================================

 EDIT HISTORY FOR FILE

 when         who      what, where, why
 ----------   ---      -------------------------------------------------------
 12/26/2016   ysreekan Removed Firmware lock related sources

 ===========================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import java.util.Random;
import java.util.concurrent.locks.ReentrantLock;
import java.io.*;
import java.net.*;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;
import com.qualcomm.qti.securemsm.mdtp.ServerSigner.ASN1.MdtpMsgBuilder;
import com.qualcomm.qti.securemsm.mdtp.ServerSigner.CMS.CMSBuilder;
import com.qualcomm.qti.securemsm.mdtp.ServerSigner.PKI.PKILoader;

public class ServerService extends Service{

	/** Log output **/
	private final String TAG = "ServerService";
	private final String TAG_CLIENT_LISTENER = "ServerClientListener";
	private final String TAG_WEB_LISTENER = "ServerWebListener";

	/** Networking **/
	private ServerSocket serverSocketClient;
	private ServerSocket serverSocketWeb;
	private Socket clientSocket, webSocket; /** Servers Sockets **/
	private final int clientPort = 60000;
	private final int webInterfacePort = 60001;

	/** Multi-Threading **/
	private Thread clientListener; 	/** Server Listener Thread **/
	private Thread webListener; 	/** Server Listener Thread **/
	ReentrantLock parseLock;
	ReentrantLock dbLock;

	/** Pending Events **/
	boolean isPendingLock, isPendingUnlock;
	MdtpMessage pendingHeader;
	Object pendingMsg;

	/** Server Status & DB **/
	private DataBase mServerDB = null;

	/** Server runtime DB **/
	private class DataBase {
		String UserName;
		String UserPassword;
		String MdtpPin; //IsvPin
		String DeviceIMEI;
		String DeviceHWRecoveryPIN;
		String IsvDeviceID;
		String IsvFriendlyName;
		byte[] UserToken;
		MdtpStateDemo DeviceState;

		public void defaults(){
			UserName = "SafeSwitch@qti.qualcomm.com";
			UserPassword = "12345678";
			MdtpPin = "null";
			DeviceIMEI = "null";
			DeviceHWRecoveryPIN = "null";
			IsvDeviceID = "null";
			IsvFriendlyName = "QTI ISV";
			UserToken = new byte[16];
			DeviceState = new MdtpStateDemo();
			DeviceState.mSystemState = MdtpStateDemo.MdtpSystemStateDemo.DISABLED;
			DeviceState.mAppState.mSimLocked = false;
			DeviceState.mAppState.mEmergencyOnly = false;
		}
	}
	/***************/

	/** Server persistent DB **/
	private static String USER_NAME_FILE = "user_name";
	private static String USER_PASSWORD_FILE = "user_password";
	private static String USER_DEVICE_STATE_FILE = "user_device_state";
	private static String USER_TOEKN_FILE = "user_token";
	private static String USER_PIN_FILE = "user_pin";
	private static String USER_DEVICE_IMEI_FILE = "user_device_imei";
	private static String USER_DEVICE_HW_PIN = "user_device_hw_pin";
	private static String USER_DEVICE_ISV_ID = "user_device_isv_id";
	/***************/

	@Override
	public int onStartCommand (Intent intent, int flags, int startId){
		Log.d(TAG, "Server onStartCommand");
		initServer();
		Log.d(TAG, "MDTP Server: onStartCommand: initThread done");
		startClientListener();
		startWebListener();
		Log.d(TAG, "MDTP Server: onStartCommand: start listeners done");

		return START_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}

	/** Init server components **/
	void initServer(){
		Log.d(TAG, "MDTP Server: initServer");
		isPendingLock = false;
		isPendingUnlock = false;
		pendingHeader = null;
		pendingMsg = null;
		parseLock = new ReentrantLock();
		dbLock = new ReentrantLock();
		mServerDB = new DataBase();
		mServerDB.defaults();
		loadServerDB();
		PKILoader.load(getBaseContext());
	}

	/** Start server thread for ISV application communication **/
	void startClientListener() {
		Log.d(TAG, "MDTP Server: startClientListener");

		Runnable r = new Runnable(){
			@Override
			public void run() {
				Log.d(TAG_CLIENT_LISTENER, "Client Listener Thread is running");

				try {
					serverSocketClient = new ServerSocket(clientPort);
				} catch (Exception e) {
					e.printStackTrace();
					throw new RuntimeException(e);
				}
				Log.d(TAG_CLIENT_LISTENER, "MDTP Waiting for client on port "+ serverSocketClient.getLocalPort() + "...");
				while(true)
				{
					try {
						serverSocketClient.setSoTimeout(500);
						clientSocket = serverSocketClient.accept();
						Log.d(TAG_CLIENT_LISTENER, "MDTP Just connected to " + clientSocket.getRemoteSocketAddress());
						ObjectInputStream input = new ObjectInputStream(clientSocket.getInputStream());
						ObjectOutputStream output = new ObjectOutputStream(clientSocket.getOutputStream());
						parseMessage(input,output);
						//clientSocket.close();
					} catch(SocketTimeoutException s) {
						 continue;
					} catch(Exception e) {
						 e.printStackTrace();
						 throw new RuntimeException(e);
					}
					storeServerDB();
				}

			}

		};

		clientListener = new Thread(r);
		clientListener.start();
	}

	/** Start server thread for Web Interface communication **/
	void startWebListener() {
		Log.d(TAG, "MDTP Server: startWebListener");

		Runnable r = new Runnable(){
			@Override
			public void run() {
				Log.d(TAG_WEB_LISTENER, "MDTP Web Listener Thread is running");
				try {
					serverSocketWeb = new ServerSocket(webInterfacePort);
				} catch (Exception e) {
					e.printStackTrace();
					throw new RuntimeException(e);
				}
				Log.d(TAG_WEB_LISTENER, "MDTP Waiting for web on port "+ serverSocketWeb.getLocalPort() + "...");
				while(true)
				{
					try {
						serverSocketWeb.setSoTimeout(500);
						webSocket = serverSocketWeb.accept();
						Log.d(TAG_WEB_LISTENER, "MDTP Just connected to " + webSocket.getRemoteSocketAddress());
						ObjectInputStream input = new ObjectInputStream(webSocket.getInputStream());
						ObjectOutputStream output = new ObjectOutputStream(webSocket.getOutputStream());
						parseMessage(input, output);
						//webSocket.close();
					} catch(SocketTimeoutException s) {
						 continue;
					} catch(Exception e) {
						e.printStackTrace();
						throw new RuntimeException(e);
					}
					storeServerDB();
				}

			}

		};

		webListener = new Thread(r);
		webListener.start();
	}

	/** Parse and handle incoming messages **/
	private void parseMessage(ObjectInputStream input,ObjectOutputStream output) throws RuntimeException, OptionalDataException, ClassNotFoundException, IOException{
		parseLock.lock();
		if (input == null) return;
		MdtpMessage header = (MdtpMessage) input.readObject();
		if( header == null){
			Log.d(TAG, "MDTP Server: parseMessage: NULL error");
			return;
		}

		if(header.getMessageType().equals("SignUpRequest") ){
			handleSignUpRequest(input, output);
		}
		if(header.getMessageType().equals("WebLogInRequest")){
			handleWebLoginRequest(input, output);
		}
		if(header.getMessageType().equals("ActivationRequest")){
			handleActivationRequest(input, output);
		}
		if(header.getMessageType().equals("DeactivationRequest")){
			handleDeactivationRequest(input, output);
		}
		if(header.getMessageType().equals("UpdateStatus")){
			handleUpdatetatus(input, output, header);
		}
		if(header.getMessageType().equals("UpdateStatusRequest")){
			handleStatusRequest(input, output, header);
		}
		if(header.getMessageType().equals("RemoteLockRequest")){
			handleRemoteLockRequest(input, output);
		}
		if(header.getMessageType().equals("RemoteLockResponse")){
			handleRemoteLockResponse(input, output);
		}
		if(header.getMessageType().equals("RemoteRestoreRequest")){
			handleRemoteRestoreRequest(input, output);
		}
		if(header.getMessageType().equals("WebRestoreRequest")){
			handleWebRestoreRequest(input, output);
		}
		if(header.getMessageType().equals("StopPendingRequests")){
			handleStopPendingRequests(input, output);
		}
		if(header.getMessageType().equals("PollServer")){
			handlePollServerMessage(input, output);
		}

		parseLock.unlock();
	}

	/** Loads the server's DB from filesystem to runtime variables **/
	private void loadServerDB(){
		FileInputStream fis;
		DataInputStream dis;
		ObjectInputStream ois;
		dbLock.lock();
		Log.d(TAG, "Server: loadServerDB: start");
		try {
			fis= getBaseContext().openFileInput(USER_NAME_FILE);
			dis = new DataInputStream(fis);
			mServerDB.UserName = dis.readUTF();
			fis.close();

			fis= getBaseContext().openFileInput(USER_PASSWORD_FILE);
			dis = new DataInputStream(fis);
			mServerDB.UserPassword = dis.readUTF();
			fis.close();

			fis= getBaseContext().openFileInput(USER_PIN_FILE);
			dis = new DataInputStream(fis);
			mServerDB.MdtpPin = dis.readUTF();
			fis.close();

			fis= getBaseContext().openFileInput(USER_DEVICE_IMEI_FILE);
			dis = new DataInputStream(fis);
			mServerDB.DeviceIMEI = dis.readUTF();
			fis.close();

			fis= getBaseContext().openFileInput(USER_TOEKN_FILE);
			dis = new DataInputStream(fis);
			dis.readFully(mServerDB.UserToken, 0, 16);
			fis.close();

			fis= getBaseContext().openFileInput(USER_DEVICE_HW_PIN);
			dis = new DataInputStream(fis);
			mServerDB.DeviceHWRecoveryPIN= dis.readUTF();
			fis.close();

			fis= getBaseContext().openFileInput(USER_DEVICE_ISV_ID);
			dis = new DataInputStream(fis);
			mServerDB.IsvDeviceID= dis.readUTF();
			fis.close();

			fis= getBaseContext().openFileInput(USER_DEVICE_STATE_FILE);
			ois = new ObjectInputStream(fis);
			mServerDB.DeviceState= (MdtpStateDemo)ois.readObject();
			fis.close();

			Log.d(TAG, "Server: loadServerDB: Completed");
		} catch (FileNotFoundException e) {
			Log.d(TAG, "Server: FileNotFoundException: setting default DB");
			storeServerDB();
		} catch (Exception e) {
			Log.d(TAG, "Server: loadServerDB: IOException");
			e.printStackTrace();
		}
		dbLock.unlock();
	}

	/** Store the server's runtime DB to filesystem **/
	private void storeServerDB(){
		FileOutputStream fos;
		DataOutputStream dos;
		ObjectOutputStream oos;
		dbLock.lock();
		try {
			Log.d(TAG, "Server: storeServerDB: Start");
			fos= getBaseContext().openFileOutput(USER_NAME_FILE, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.writeUTF(mServerDB.UserName);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_PASSWORD_FILE, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.writeUTF(mServerDB.UserPassword);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_PIN_FILE, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.writeUTF(mServerDB.MdtpPin);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_DEVICE_IMEI_FILE, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.writeUTF(mServerDB.DeviceIMEI);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_TOEKN_FILE, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.write(mServerDB.UserToken, 0, 16);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_DEVICE_HW_PIN, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.writeUTF(mServerDB.DeviceHWRecoveryPIN);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_DEVICE_ISV_ID, Context.MODE_PRIVATE);
			dos = new DataOutputStream(fos);
			dos.writeUTF(mServerDB.IsvDeviceID);
			fos.close();

			fos= getBaseContext().openFileOutput(USER_DEVICE_STATE_FILE, Context.MODE_PRIVATE);
			oos = new ObjectOutputStream(fos);
			oos.writeObject(mServerDB.DeviceState);
			fos.close();

			Log.d(TAG, "Server: storeServerDB: Completed");
		} catch (FileNotFoundException e) {
			Log.d(TAG, "Server: FileNotFoundException: store state");
			e.printStackTrace();
		} catch (IOException e) {
			Log.d(TAG, "Server: IOException: store state");
			e.printStackTrace();
		}
		dbLock.unlock();
	}

	/** Generate a 8 digit random value **/
	public String randomPin() {
		return String.format("%08d", new Random().nextInt(99999999));
	}

	/** SignUp requests handler **/
	private void handleSignUpRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "MDTP Server: Incoming SignUpRequest message");
		MdtpMessage.SignUpRequest msg = (MdtpMessage.SignUpRequest)input.readObject();
		mServerDB.UserName =  msg.getUsername();
		mServerDB.UserPassword = msg.getPassword();
		MdtpMessage newMsg = new MdtpMessage("SignUpResponse", "Server", "Client");
		MdtpMessage.SignUpResponse response = newMsg.new SignUpResponse(true);
		output.writeObject(newMsg);
		output.writeObject(response);
		output.flush();
	}

	/** Web interface login requests handler **/
	private void handleWebLoginRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "MDTP Server: Incoming WebLogInRequest message");
		MdtpMessage.WebLogInRequest msg = (MdtpMessage.WebLogInRequest)input.readObject();
		MdtpMessage newMsg = new MdtpMessage("WebLogInResponse", "Server", "WebInterface");
		MdtpMessage.WebLogInResponse response = null;
		if (!mServerDB.UserName.equals(msg.getUsername()) || !mServerDB.UserPassword.equals(msg.getPassword()) )
			response = newMsg.new WebLogInResponse(false);
		else
			response = newMsg.new WebLogInResponse(true);

		output.writeObject(newMsg);
		output.writeObject(response);
		output.flush();
	}

	/** Activation requests handler **/
	private void handleActivationRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "MDTP Server: Incoming ActivationRequest message");
		MdtpMessage newMsg;
		MdtpMessage.SignedActivationResponse response;
		MdtpMessage.ActivationRequest msg = (MdtpMessage.ActivationRequest)input.readObject();
		if (!mServerDB.UserName.equals(msg.getUsername()) || !mServerDB.UserPassword.equals(msg.getPassword()) ){
			Log.d(TAG, "MDTP Server: Wrong user credentials");
			newMsg = new MdtpMessage("SignedActivationResponse", "Server", "Client");
			response = newMsg.new SignedActivationResponse(false, msg.useSimLock());
		}else{
			mServerDB.UserToken = msg.getUserToken();
			mServerDB.DeviceIMEI = msg.getIMEI();
			mServerDB.MdtpPin = randomPin();
			mServerDB.DeviceHWRecoveryPIN = msg.getHWRecoveryPin();
			mServerDB.IsvDeviceID = msg.getIsvDeviceID();

			newMsg = new MdtpMessage("SignedActivationResponse", "Server", "Client");
			response = newMsg.new SignedActivationResponse(true, msg.useSimLock());
			/** buildSetStateMsg(boolean active, boolean firmwareLockSelect, boolean simLockSelect,byte[] idToken, byte[] isvPin, byte[] isvDeviceId, byte[] isvFriendlyName) **/
			response.setActivationMsg(CMSBuilder.create(
				MdtpMsgBuilder.buildSetStateMsg(
					true, msg.useSimLock(), mServerDB.UserToken,
					mServerDB.MdtpPin.getBytes(), mServerDB.IsvDeviceID.getBytes(),
					mServerDB.IsvFriendlyName.getBytes()
					)
				)
			);
		}

		output.writeObject(newMsg);
		output.writeObject(response);
		output.flush();
	}

	/** Deactivaion requests handler **/
	private void handleDeactivationRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "MDTP Server: Incoming DeactivationRequest message");
		MdtpMessage newMsg;
		MdtpMessage.SignedDeactivationResponse response;
		MdtpMessage.DeactivationRequest msg = (MdtpMessage.DeactivationRequest)input.readObject();
		if (	( !mServerDB.UserName.equals(msg.getUsername()) ) ||
				( !mServerDB.UserPassword.equals(msg.getPassword())) ) {
			Log.d(TAG, "Wrong user credentials");
			newMsg = new MdtpMessage("SignedDeactivationResponse", "Server", "Client");
			response = newMsg.new SignedDeactivationResponse(false);
		}else{
			newMsg = new MdtpMessage("SignedDeactivationResponse", "Server", "Client");
			response = newMsg.new SignedDeactivationResponse(true);
			/** Deactivation message - SET_STATE(activated=0, fwlock=1, simlock=0/1) */
			response.setDeactivationMsg(CMSBuilder.create(
				MdtpMsgBuilder.buildSetStateMsg(
					false, false, mServerDB.UserToken,
					new byte[0], new byte[0], new byte[0]
					)
				)
			);
		}

		output.writeObject(newMsg);
		output.writeObject(response);
		output.flush();
	}

	/** Devices status change updates requests handler **/
	private void handleUpdatetatus(ObjectInputStream input,ObjectOutputStream output, MdtpMessage header) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "MDTP Server: Incoming UpdateStatus message");
		MdtpMessage.UpdateStatus msg = (MdtpMessage.UpdateStatus)input.readObject();
		mServerDB.DeviceState = msg.getStatus();
		MdtpMessage newheader = new MdtpMessage("Acknowledgment", "Server", header.getMessageSource());
		MdtpMessage.Acknowledgment response = newheader.new Acknowledgment("Server", header.getMessageSource());
		output.writeObject(newheader);
		output.writeObject(response);
		output.flush();
	}

	/** Devices statues requests handler **/
	private void handleStatusRequest(ObjectInputStream input,ObjectOutputStream output, MdtpMessage header) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "MDTP Server: Incoming UpdateStatusRequest message");
		MdtpMessage newheader = new MdtpMessage("UpdateStateToWeb", "Server", header.getMessageSource());
		MdtpMessage.UpdateStateToWeb response = newheader.new UpdateStateToWeb();
		response.setStatus(mServerDB.DeviceState);
		response.setLockingStatus(isPendingLock);
		response.setUnlockingStatus(isPendingUnlock);
		response.setPin(mServerDB.MdtpPin);
		response.setHWRecoveryPin(mServerDB.DeviceHWRecoveryPIN);
		response.setIsvDeviceID(mServerDB.IsvDeviceID);
		response.setIsvFreindlyName(mServerDB.IsvFriendlyName);
		output.writeObject(newheader);
		output.writeObject(response);
		output.flush();
	}

	/** Remote kill requests (from web interface) handler **/
	private void handleRemoteLockRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		MdtpMessage.RemoteLockRequest msg = (MdtpMessage.RemoteLockRequest)input.readObject();
		MdtpMessage newheader = new MdtpMessage("RemoteLockMessage", "Server", "Client");
		MdtpMessage.RemoteLockMessage response = newheader.new RemoteLockMessage();
		response.setKillMsg(CMSBuilder.create(MdtpMsgBuilder.buildSetKillMsg(true, true, mServerDB.UserToken)));

		isPendingLock = true;
		pendingHeader = newheader;
		pendingMsg = response;

		newheader = new MdtpMessage("UpdateStateToWeb", "Server", "WebInterface");
		MdtpMessage.UpdateStateToWeb responseStatus = newheader.new UpdateStateToWeb();
		responseStatus.setStatus(mServerDB.DeviceState);
		responseStatus.setLockingStatus(isPendingLock);
		responseStatus.setPin(mServerDB.MdtpPin);
		responseStatus.setUnlockingStatus(false);
		output.writeObject(newheader);
		output.writeObject(responseStatus);
		output.flush();
	}

	private void handleRemoteLockResponse(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Server: Incoming RemoteLockResponse message");
		MdtpMessage.RemoteLockResponse msg = (MdtpMessage.RemoteLockResponse)input.readObject();
		if(msg.getLockResult()){
			mServerDB.DeviceState.mAppState.mEmergencyOnly = false;
			Log.d(TAG, "Server: Device has been locked");
		}
		MdtpMessage newheader = new MdtpMessage("Acknowledgment", "Server", "WebInterface");
		MdtpMessage.Acknowledgment response = newheader.new Acknowledgment("Server","WebInterface");
		output.writeObject(newheader);
		output.writeObject(response);
		output.flush();
	}

	/** Restore requests (from ISV application) handler **/
	private void handleRemoteRestoreRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Server: Incoming RemoteRestoreRequest message");
		MdtpMessage.RemoteRestoreRequest msg = (MdtpMessage.RemoteRestoreRequest)input.readObject();
		MdtpMessage newheader;
		MdtpMessage.RemoteRestoreMessage response;

		if (	( !mServerDB.UserName.equals(msg.getUsername()) ) ||
				( !mServerDB.UserPassword.equals(msg.getPassword())) ) {
			newheader = new MdtpMessage("RemoteRestoreMessage", "Server", "Client");
			response = newheader.new RemoteRestoreMessage(false);
		}else {
			newheader = new MdtpMessage("RemoteRestoreMessage", "Server", "Client");
			response = newheader.new RemoteRestoreMessage(true);
			response.setRestoreMsg(CMSBuilder.create(MdtpMsgBuilder.buildSetKillMsg(false, true, mServerDB.UserToken)));
		}

		output.writeObject(newheader);
		output.writeObject(response);
		output.flush();
	}

	/** Remote Kill requests (from web interface) handler **/
	private void handleWebRestoreRequest(ObjectInputStream input,ObjectOutputStream output) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Server: Incoming WebRestoreRequest message");
		MdtpMessage.WebRestoreRequest msg = (MdtpMessage.WebRestoreRequest)input.readObject();
		MdtpMessage newheader  = new MdtpMessage("RemoteRestoreMessage", "Server", "Client");
		MdtpMessage.RemoteRestoreMessage response = newheader.new RemoteRestoreMessage(true);
		response.setRestoreMsg(CMSBuilder.create(MdtpMsgBuilder.buildSetKillMsg(false, true, mServerDB.UserToken)));

		isPendingUnlock = true;
		pendingHeader = newheader;
		pendingMsg = response;

		newheader = new MdtpMessage("UpdateStateToWeb", "Server", "WebInterface");
		MdtpMessage.UpdateStateToWeb responseStatus = newheader.new UpdateStateToWeb();
		responseStatus.setStatus(mServerDB.DeviceState);
		responseStatus.setLockingStatus(false);
		responseStatus.setUnlockingStatus(isPendingUnlock);
		responseStatus.setPin(mServerDB.MdtpPin);
		output.writeObject(newheader);
		output.writeObject(responseStatus);
		output.flush();
	}

	/** Stop pending requests (from web interface) handler **/
	private void handleStopPendingRequests(ObjectInputStream input, ObjectOutputStream output)throws OptionalDataException, ClassNotFoundException, IOException{
		MdtpMessage.StopPendingRequests msg = (MdtpMessage.StopPendingRequests)input.readObject();
		isPendingUnlock = false;
		isPendingLock = false;
		pendingHeader = null;
		pendingMsg = null;
		MdtpMessage newheader = new MdtpMessage("UpdateStateToWeb", "Server", "WebInterface");
		MdtpMessage.UpdateStateToWeb responseStatus = newheader.new UpdateStateToWeb();
		responseStatus.setStatus(mServerDB.DeviceState);
		responseStatus.setLockingStatus(false);
		responseStatus.setUnlockingStatus(false);
		responseStatus.setPin(mServerDB.MdtpPin);
		output.writeObject(newheader);
		output.writeObject(responseStatus);
		output.flush();
	}

	/** Poll for pending requests handler **/
	private void handlePollServerMessage(ObjectInputStream input, ObjectOutputStream output)throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Server: Incoming PollServer message");
		MdtpMessage.PollServer msg = (MdtpMessage.PollServer)input.readObject();
		if(isPendingLock || isPendingUnlock){
			output.writeObject(pendingHeader);
			output.writeObject(pendingMsg);
			output.flush();
		}else{
			MdtpMessage newHeader = new MdtpMessage("Acknowledgment", "Server", "Client");
			MdtpMessage.Acknowledgment response =  newHeader.new Acknowledgment("Server", "Client");
			output.writeObject(newHeader);
			output.writeObject(response);
			output.flush();
			return;
		}
		if(isPendingLock)
			mServerDB.DeviceState.mAppState.mEmergencyOnly = false;
		else
			mServerDB.DeviceState.mAppState.mEmergencyOnly = true;

		isPendingUnlock = false;
		isPendingLock = false;
		pendingHeader = null;
		pendingMsg = null;
	}

}
