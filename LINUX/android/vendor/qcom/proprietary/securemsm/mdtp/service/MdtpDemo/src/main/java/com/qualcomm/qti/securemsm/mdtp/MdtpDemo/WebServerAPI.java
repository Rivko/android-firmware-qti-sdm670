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
import android.util.Log;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApiImpl;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class WebServerAPI {

	private static String TAG = "WebServerAPI";
	private static String MDTP_SERVER_ON_BOOT_WEB = "mdtp_start_Server_on_boot_web";
	private MdtpStateDemo device_state = null;
	private boolean pending_lock = false;
	private boolean pending_unlock = false;
	private String mdtp_pin = null;
	private String mdtp_hw_pin = null;
	private String isv_device_id = null;
	private String isv_name = null;
	private Context ctx;
	private final int webInterfacePort = 60001;

	/** Web Server API Singleton*/
	private static WebServerAPI instance = null;

	/**
	* Get reference to singleton
	*/
	public static WebServerAPI getInstance(Context context) {
		if(instance == null) {
			Log.d(TAG, "MDTP: Instantiate WebServerAPI");
			instance = new WebServerAPI();
			if(context == null){
				throw new RuntimeException();
			}
			instance.ctx = context;
		}
		return instance;
	}

	public String communicateServer(MdtpMessage header, Object Msg) {
		Socket client = null;
		String res = "Failure";

		if(Msg == null) {
			Log.d(TAG,"MDTP: communicateServer: NULL msg");
			return "Failure";
		}
		try {
			Log.d(TAG,"Connecting to MDTP server on port " + webInterfacePort);
			client = new Socket(InetAddress.getByName(getLocalIpAddress()), webInterfacePort);
			Log.d(TAG, "MDTP: Just connected to " + client.getRemoteSocketAddress());

			ObjectOutputStream out = new ObjectOutputStream(client.getOutputStream());
			ObjectInputStream input = new ObjectInputStream(client.getInputStream());
			out.writeObject(header);
			out.writeObject(Msg);
			out.flush();
			res = parseServerResponse(input);
			client.close();
		} catch(UnknownHostException e) {
			Log.d(TAG,"MDTP: communicateServer: UnknownHostException");
			e.printStackTrace();
			return "Network Error";
		} catch(IOException e) {
			Log.d(TAG,"communicateServer: Could not connect to server");
			e.printStackTrace();
			return "Network Error";
		} catch (ClassNotFoundException e) {
			Log.d(TAG,"MDTP: communicateServer: ClassNotFoundException");
			e.printStackTrace();
		} catch (Exception e) {
			Log.d(TAG,"MDTP: communicateServer: Exception");
			e.printStackTrace();
		}
		return res;
	}

	private String parseServerResponse(ObjectInputStream input) throws StreamCorruptedException, IOException, ClassNotFoundException{
		Log.d(TAG,"MDTP: parseServerResponseWeb");
		MdtpMessage header = (MdtpMessage) input.readObject();

		if (header == null) {
			Log.d(TAG,"MDTP: parseServerResponseWeb - NULL header");
			return "Failure";
		}
		Log.d(TAG,"MDTP: communicateServer: Message Recived - "+ header.getMessageType());
		/** WebLogInResponse **/
		if(header.getMessageType().equals("WebLogInResponse")){
			MdtpMessage.WebLogInResponse msg = null;
			msg = (MdtpMessage.WebLogInResponse) input.readObject();
			if ( msg == null || !msg.getResponse() ){
				Log.d(TAG, "Cannot login web account");
				return "Failure";
			}else{
				Log.d(TAG, "Logged in to web interface!");
				return "Success";
			}
		}
		/** UpdateStateToWeb **/
		if(header.getMessageType().equals("UpdateStateToWeb")){
			MdtpMessage.UpdateStateToWeb msg = null;
			msg = (MdtpMessage.UpdateStateToWeb) input.readObject();
			if (msg == null){
				Log.d(TAG, "MDTP: Cannot get device info");
				return "Failure";
			}
			device_state = msg.getStatus();
			pending_lock = msg.getLockingStatus();
			pending_unlock = msg.getUnlockingStatus();

			if(msg.getPin() != null)
				mdtp_pin = msg.getPin();
			else
				mdtp_pin = "NULL";

			if(msg.getHWRecoveryPin() != null)
				mdtp_hw_pin = msg.getHWRecoveryPin();
			else
				mdtp_hw_pin = "NULL";

			if(msg.getIsvDeviceID() != null)
				isv_device_id = msg.getIsvDeviceID();
			else
				isv_device_id = "Unknown";

			if(msg.getIsvFreindlyName() != null)
				isv_name = msg.getIsvFreindlyName();
			else
				isv_name = "Unknown";

			return "Success";
		}
		/** Acknowledgment **/
		if(header.getMessageType().equals("Acknowledgment")){
			MdtpMessage.Acknowledgment msg = (MdtpMessage.Acknowledgment) input.readObject();
			return "Success";
		}
		Log.d(TAG, "MDTP: Got Nothing");
		return "Failure";
	}

	public MdtpStateDemo getStatus(){
		return device_state;
	}
	public boolean getLockingStatus(){
		return pending_lock;
	}
	public boolean getUnlockingStatus(){
		return pending_unlock;
	}
	public String getMdtpPin(){
		return mdtp_pin;
	}
	public String getMdtpHWRecoveryPin(){
		return mdtp_hw_pin;
	}
	public String getIsvDeviceID(){
		return isv_device_id;
	}
	public String getIsvFreindlyName(){
		return isv_name;
	}

	public String getLocalIpAddress() throws Exception {
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
		return "0.0.0.0";
	}

	/** Check if to load server on boot **/
	public boolean getLoadServerOnBoot(){
		boolean fuse = false;
		try{
			FileInputStream fis = ctx.openFileInput(MDTP_SERVER_ON_BOOT_WEB);
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
			FileOutputStream fos = ctx.openFileOutput(MDTP_SERVER_ON_BOOT_WEB, Context.MODE_PRIVATE);
			DataOutputStream dos = new DataOutputStream(fos);
			dos.writeBoolean(load);
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}
	}
}
