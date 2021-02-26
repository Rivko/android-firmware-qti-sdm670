/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import java.io.*;
import java.net.*;
import java.util.Enumeration;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.content.pm.PackageManager;
import android.content.ComponentName;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;
import android.app.StatusBarManager;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

public class ServerAPI {
	private static String TAG = "ClientServerAPI";

	private static String SERVER_IP_FILE = "server_IP_file";
	private static String DEVICE_STATE = "device_state";
	private static String MDTP_PIN = "mdtp_pin";
	private static String MDTP_FUSE = "mdtp_fuse";
	private static String MDTP_SERVER_ON_BOOT_CLIENT = "mdtp_start_Server_on_boot_client";
	private static String serverIP;
	private Context ctx;

	/** Server API Singleton */
	private static ServerAPI instance = null;

	/** Get reference to singleton **/
	public static ServerAPI getInstance(Context context) {
		if(instance == null) {
			Log.d(TAG, "Instantiate ServerAPI");
			instance = new ServerAPI();
			if(context == null){
				throw new RuntimeException();
			}
			instance.ctx = context;
			try {
				FileInputStream server_ip = instance.ctx.openFileInput(SERVER_IP_FILE);
				DataInputStream server_ip_data = new DataInputStream(server_ip);
				serverIP = server_ip_data.readUTF();
				server_ip.close();
			} catch (Exception e) {
				serverIP = instance.getLocalIpAddress();
				e.printStackTrace();
			}
		}
		return instance;
	}

	/** Get local IP, used for init server ip **/
	private String getLocalIpAddress(){
		try{
			for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
				en.hasMoreElements();) {

				NetworkInterface intf = (NetworkInterface) en.nextElement();
				for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses();
					enumIpAddr.hasMoreElements();) {
						InetAddress inetAddress = (InetAddress) enumIpAddr.nextElement();
						if(	(!inetAddress.isLoopbackAddress()) &&
							(inetAddress instanceof Inet4Address)){
								return inetAddress.getHostAddress().toString();
						}
				}
			}
		} catch (Exception e) {
			/** Return defualt IP **/
			Log.d(TAG, "MDTP ServerAPI - could not find network interface");
		}
		return "0.0.0.0";
	}

	/** Try to send message to the server **/
	private String communicateServer(MdtpMessage header, Object Msg, int port) {
		Socket client = null;
		String res = "Failure";

		if(Msg == null || header == null) {
			Log.d(TAG,"communicateServer: NULL msg");
			return "Failure";
		}
		try {
			Log.d(TAG,"Connecting to MDTP server on " + InetAddress.getByName(serverIP) +"/" + port);
			client = new Socket (InetAddress.getByName(serverIP), port);

			Log.d(TAG, "Just connected to " + client.getRemoteSocketAddress());
			ObjectOutputStream out = new ObjectOutputStream(client.getOutputStream());
			out.writeObject(header);
			out.writeObject(Msg);
			out.flush();
			Log.d(TAG,"MDTP: communicateServer: Message Sent - "+ header.getMessageType());
			res = parseServerResponse(client);
			client.close();
		} catch(IOException e) {
			e.printStackTrace();
			return "Network Error";
		} catch (Exception e) {
			e.printStackTrace();
			throw new RuntimeException();
		}
		return res;
	}

	/** Try to send message to the server
		Gives a few number of chances to connect with server
	**/
	public String sendMessageToServer(MdtpMessage header, Object Msg, int port){
		int counts = 5;
		String retval = "Failure";
		do {
			retval = communicateServer(header, Msg, port);
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				return "Failure";
			}
			if(--counts == 0){
				return "Network Error";
			}
		} while (retval.equals("Network Error"));

		return retval;
	}

	/** Parse and handle the server message **/
	public String parseServerResponse(Socket socket) throws StreamCorruptedException, IOException, ClassNotFoundException{
		Log.d(TAG,"parseServerResponse");
		ObjectInputStream input = null;
		MdtpMessage header = null;

		if (socket == null)
			return "Failure";

		input = new ObjectInputStream(socket.getInputStream());
		header = (MdtpMessage) input.readObject();

		if (header == null)
			return "Failure";

		/** SignUpResponse **/
		if(header.getMessageType().equals("SignUpResponse")){
			return handleSignUpResponse(input);
		}
		/** SignedActivationResponse **/
		if(header.getMessageType().equals("SignedActivationResponse")){
			return handleSignedActivationResponse(input);
		}
		/** SignedDeactivationResponse **/
		if(header.getMessageType().equals("SignedDeactivationResponse")){
			return handleSignedDeactivationResponse(input);
		}
		/** RemoteLockMessage **/
		if(header.getMessageType().equals("RemoteLockMessage")){
			if(handleRemoteLockMessage(input).equals("Failure")){
				MdtpMessage MsgHeader = new MdtpMessage("UpdateStatus", "Client", "Server");
				MdtpMessage.UpdateStatus updateStatus = MsgHeader.new UpdateStatus("Client", "Server");
				MdtpState state = MdtpApiSingleton.getInstance().getState();
				updateStatus.setStatus(new MdtpStateDemo(state));
				return communicateServer(MsgHeader, updateStatus, 60000);
			}else{
				return "Success";
			}
		}
		/** RemoteRestoreMessage **/
		if(header.getMessageType().equals("RemoteRestoreMessage")){
			if(handleRemoteRestoreMessage(input).equals("Failure")){
				MdtpMessage MsgHeader = new MdtpMessage("UpdateStatus", "Client", "Server");
				MdtpMessage.UpdateStatus updateStatus = MsgHeader.new UpdateStatus("Client", "Server");
				MdtpState state = MdtpApiSingleton.getInstance().getState();
				updateStatus.setStatus(new MdtpStateDemo(state));
				communicateServer(MsgHeader, updateStatus, 60000);
				return "Failure";
			}else{
				return "Success";
			}
		}
		/** Acknowledgment **/
		if(header.getMessageType().equals("Acknowledgment")){
			return handleAcknowledgment(input);
		}

		return "Failure";
	}

	/** SignUp response handler **/
	private String handleSignUpResponse(ObjectInputStream input) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Client: Incoming SignUpResponse message");
		MdtpMessage.SignUpResponse msg = null;
		msg = (MdtpMessage.SignUpResponse) input.readObject();
		if ( msg == null || !msg.getResponse() ){
			Log.d(TAG, "Cannot create new account");
			return "Failure";
		}else{
			Log.d(TAG, "Created a new SafeSwitch Account!!");
			return "Success";
		}
	}

	/** Signed activation message handler **/
	private String handleSignedActivationResponse(ObjectInputStream input) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Client: Incoming SignedActivationResponse message");
		MdtpMessage.SignedActivationResponse msg = null;
		MdtpState state;
		msg = (MdtpMessage.SignedActivationResponse) input.readObject();
		if(!msg.isValidRequest){
			Log.d(TAG, "Client: Activation Failed");
			return "Failure";
		}

		try{
			for (int i = 0; (!MdtpApiSingleton.getInstance().isBound()) && (i < 5); i++)
				Thread.sleep(100);
		} catch (InterruptedException e) {
			return "Failure";
		}
		Log.d(TAG, "Mdtp Client: MdtpApiSingleton is bound");

		try{
			MdtpApiSingleton.getInstance().processSignedMsg(msg.getActivationMsg());
		}
		catch(RuntimeException e){
			return "Failure";
		}

		state = MdtpApiSingleton.getInstance().getState();
		setDeviceState(new MdtpStateDemo(state));
		return "Success";
	}

	/** Signed deactivation message handler **/
	private String handleSignedDeactivationResponse(ObjectInputStream input) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Client: Incoming SignedDeactivationResponse message");
		MdtpMessage.SignedDeactivationResponse msg = null;
		MdtpState state;
		msg = (MdtpMessage.SignedDeactivationResponse) input.readObject();
		if(!msg.getIsValidRequest()){
			Log.d(TAG, "Client: Deactivation Failed");
			return "Failure";
		}

		try{
			for (int i = 0; (MdtpApiSingleton.getInstance().isBound() == false) && (i < 5); i++)
			Thread.sleep(100);
		} catch (InterruptedException e) {
			return "Failure";
		}

		try{
			MdtpApiSingleton.getInstance().processSignedMsg(msg.getDeactivationMsg());
		}catch(RuntimeException e){
			return "Failure";
		}

		state = MdtpApiSingleton.getInstance().getState();
		setDeviceState(new MdtpStateDemo(state));
		return "Success";
	}

	/** Signed kill message handler **/
	private String handleRemoteLockMessage(ObjectInputStream input) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Client: Incoming RemoteLockMessage message");
		MdtpMessage.RemoteLockMessage msg = null;
		MdtpState state;
		msg = (MdtpMessage.RemoteLockMessage) input.readObject();
		try{
			MdtpApiSingleton.getInstance().processSignedMsg(msg.getKillMsg());
			state = MdtpApiSingleton.getInstance().getState();
		}catch(RuntimeException e){
			return "Failure";
		}
		Intent launchNextActivity = new Intent(ctx, LockScreenActivity.class);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		launchNextActivity.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);

		ComponentName lockerComponent =
			new ComponentName("com.qualcomm.qti.securemsm.mdtp.MdtpDemo",
				"com.qualcomm.qti.securemsm.mdtp.MdtpDemo.LockScreenActivity");
		ctx.getPackageManager().setComponentEnabledSetting(lockerComponent,
			PackageManager.COMPONENT_ENABLED_STATE_ENABLED, 1);

		setDeviceState(new MdtpStateDemo(state));
		ctx.startActivity(launchNextActivity);
		return "Success";
	}

	/** Signed restore message handler **/
	private String handleRemoteRestoreMessage(ObjectInputStream input) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Client: Incoming RemoteRestoreMessage message");
		MdtpMessage.RemoteRestoreMessage msg = null;
		MdtpState state;
		msg = (MdtpMessage.RemoteRestoreMessage) input.readObject();
		boolean result = msg.getRestoreStatus();
		if(result != true){
			return "Failure";
		}

		try{
			MdtpApiSingleton.getInstance().processSignedMsg(msg.getRestoreMsg());
			state = MdtpApiSingleton.getInstance().getState();
		}catch(RuntimeException e){
			return "Failure";
		}
		setDeviceState(new MdtpStateDemo(state));
		DeviceLockUtil mDeviceLockUtil;
		mDeviceLockUtil = DeviceLockUtil.getInstance(ctx);
		mDeviceLockUtil.unlockDevice();
		return "Success";
	}

	/** Acknowledgment message handler **/
	private String handleAcknowledgment(ObjectInputStream input) throws OptionalDataException, ClassNotFoundException, IOException{
		Log.d(TAG, "Client: Incoming Acknowledgment message");
		MdtpMessage.Acknowledgment msg = (MdtpMessage.Acknowledgment) input.readObject();
		return "Success";
	}

	/** Write to a UTF syntaxed file **/
	private void updateUTFInternalState(String str, String filename){
		try{
			FileOutputStream fos = ctx.openFileOutput(filename, Context.MODE_PRIVATE);
			DataOutputStream dos = new DataOutputStream(fos);
			dos.writeUTF(str);
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}
	}

	/** Read from a UTF syntaxed file **/
	private String readUTFInternalState(String filename){
		String res;
		try{
			FileInputStream fis = ctx.openFileInput(filename);
			DataInputStream dis = new DataInputStream(fis);
			res = dis.readUTF();
			fis.close();
		} catch (FileNotFoundException e) {
			Log.d(TAG, "readInternalState :FileNotFoundException");
			return null;
		} catch (IOException e) {
			Log.d(TAG, "readInternalState :IOException");
			throw new RuntimeException(e);
		}
		return res;
	}

	/** Set the server's IP address **/
	public void setServerIP(String s){
		serverIP = s;
		updateUTFInternalState(serverIP, SERVER_IP_FILE);
	}

	/** Get the server's IP address **/
	public String getServerIP(){
		String res = serverIP;
		if(res !=null)
			return res;

		return readUTFInternalState(SERVER_IP_FILE);
	}

	/** Get the device's state **/
	public MdtpStateDemo getDeviceState(){
		MdtpStateDemo state = new MdtpStateDemo();
		try{
			FileInputStream fis = ctx.openFileInput(DEVICE_STATE);
			ObjectInputStream dis = new ObjectInputStream(fis);
			state = (MdtpStateDemo)dis.readObject();
			fis.close();
		} catch (FileNotFoundException e) {
			Log.d(TAG, "FileNotFoundException");
			state.mSystemState = MdtpStateDemo.MdtpSystemStateDemo.DISABLED;
			state.mAppState.mSimLocked = false;
			state.mAppState.mEmergencyOnly = false;
		} catch (Exception e) {
			Log.d(TAG, "IOException");
			throw new RuntimeException(e);
		}
		return state;
	}

	/** Set the device's state **/
	public void setDeviceState(MdtpStateDemo state){
		try{
			FileOutputStream fos = ctx.openFileOutput(DEVICE_STATE, Context.MODE_PRIVATE);
			ObjectOutputStream dos = new ObjectOutputStream(fos);
			dos.writeObject(state);
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}
	}

	/** Check if a full connection to MDTP server has been established **/
	public boolean isServerConnectionEstablished(){
		boolean fuse = false;
		try{
			FileInputStream fis = ctx.openFileInput(MDTP_FUSE);
			DataInputStream dis = new DataInputStream(fis);
			fuse = dis.readBoolean();
			fis.close();
		} catch (FileNotFoundException e) {
			return false;
		} catch (IOException e) {
			Log.d(TAG, "IOException");
			throw new RuntimeException(e);
		}
		return fuse;
	}

	/** Set the client-server connection status **/
	public void setServerConnectionStatus(boolean fuse){
		try{
			FileOutputStream fos = ctx.openFileOutput(MDTP_FUSE, Context.MODE_PRIVATE);
			DataOutputStream dos = new DataOutputStream(fos);
			dos.writeBoolean(fuse);
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}
	}

	/** Check if to load server on boot **/
	public boolean getLoadServerOnBoot(){
		boolean fuse = false;
		try{
			FileInputStream fis = ctx.openFileInput(MDTP_SERVER_ON_BOOT_CLIENT);
			DataInputStream dis = new DataInputStream(fis);
			fuse = dis.readBoolean();
			fis.close();
		} catch (FileNotFoundException e) {
			return false;
		} catch (IOException e) {
			Log.d(TAG, "IOException");
			throw new RuntimeException(e);
		}
		return fuse;
	}

	/** Set if to load server on boot **/
	public void setLoadServerOnBoot(boolean load){
		try{
			FileOutputStream fos = ctx.openFileOutput(MDTP_SERVER_ON_BOOT_CLIENT, Context.MODE_PRIVATE);
			DataOutputStream dos = new DataOutputStream(fos);
			dos.writeBoolean(load);
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}
	}
}
