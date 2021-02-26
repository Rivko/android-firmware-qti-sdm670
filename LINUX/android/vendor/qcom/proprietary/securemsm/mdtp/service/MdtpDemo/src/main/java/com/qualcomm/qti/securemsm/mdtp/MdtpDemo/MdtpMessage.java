/*=============================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp.MdtpDemo;

import java.io.Serializable;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpSystemState;
import com.qualcomm.qti.securemsm.mdtp.MdtpApi.MdtpAppState;

public class MdtpMessage implements Serializable {

	private static final long serialVersionUID = 1L;

	/***** Mdtp Message Header *****/
	private String messageType;
	private String Source;
	private String Destination;

	public MdtpMessage(String type, String source, String dest) {
		this.messageType = type;
		this.Source = source;
		this.Destination = dest;
	}

	public String getMessageType(){
		return this.messageType;
	}

	public String getMessageSource(){
		return this.Source;
	}

	public String getMessageDest(){
		return this.Destination;
	}


	/***** Message Types *****/

	/** Sign Up Message Request **/
	public class SignUpRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private String Username;
		private String Password;

		public SignUpRequest(String username, String pass) {
			super("SignUpRequset", "Client", "Server");
			this.Username = username;
			this.Password = pass;
		}

		public String getUsername(){
			return this.Username;
		}

		public String getPassword(){
			return this.Password;
		}
	}

	/** Sign Up Message Response **/
	public class SignUpResponse extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private boolean Response;

		public SignUpResponse(boolean response) {
			super("SignUpResponse", "Server", "Client");
			this.Response = response;
		}

		public boolean getResponse(){
			return this.Response;
		}
	}

	/** WebInterface Log In Message Request **/
	public class WebLogInRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private String Username;
		private String Password;

		public WebLogInRequest(String username, String pass) {
			super("WebLogInRequest", "WebInterface", "Server");
			this.Username = username;
			this.Password = pass;
		}

		public String getUsername(){
			return this.Username;
		}

		public String getPassword(){
			return this.Password;
		}
	}

	/** WebInterface Log In Message Response **/
	public class WebLogInResponse extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private boolean Response;

		public WebLogInResponse(boolean response) {
			super("WebLogInResponse", "Server", "WebInterface");
			this.Response = response;
		}

		public boolean getResponse(){
			return this.Response;
		}
	}

	/** Activation Request **/
	public class ActivationRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;

		private String userName;
		private String password;
		private String DeviceIMEI; // User's device unique ID (IMEI).
		private byte[] UserToken; // ID token
		private boolean SimLock;
		private String IsvDeviceID;
		private String HWRecoveryPin;

		public ActivationRequest(String username, String password) {
			super("ActivationRequest", "Client", "Server");

			this.userName = username;
			this.password = password;
			this.SimLock = false;
			this.UserToken = null;
			this.DeviceIMEI = null;
			this.IsvDeviceID = null;
			this.HWRecoveryPin = null;
			/*TODO:
				this.DeviceIMEI = get_device_IMEI();
			*/
		}

		public String getUsername(){
			return this.userName;
		}
		public String getPassword(){
			return this.password;
		}
		public byte[] getUserToken(){
			return this.UserToken;
		}
		public String getIMEI(){
			return this.DeviceIMEI;
		}
		public boolean useSimLock(){
			return this.SimLock;
		}
		public String getIsvDeviceID(){
			return this.IsvDeviceID;
		}
		public String getHWRecoveryPin(){
			return this.HWRecoveryPin;
		}

		public void setUserToken(byte[] s){
			this.UserToken = s;
		}
		public void setIMEI(String s){
			this.DeviceIMEI = s;
		}
		public void setSimLock(boolean b){
			this.SimLock = b;
		}
		public void setIsvDeviceID(String s){
			this.IsvDeviceID = s;
		}
		public void setHWRecoveryPin(String s){
			this.HWRecoveryPin = s;
		}
	}

	/** Signed Activation Response **/
	public class SignedActivationResponse extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		byte[] activationMsg;
		byte[] simLockMsg;
		boolean isValidRequest;

		public SignedActivationResponse(boolean isValid, boolean useSimLock) {
			super("SignedActivationResponse", "Server", "Client");
			isValidRequest = isValid;
			if(!isValidRequest){
				return;
			}
		}
		public boolean getIsValidRequest(){
			return this.isValidRequest;
		}
		public byte[] getActivationMsg(){
			return this.activationMsg;
		}
		public byte[] getSimLockMsg(){
			return this.simLockMsg;
		}
		public void setActivationMsg(byte[] signedActivationMsg ){
			this.activationMsg = signedActivationMsg;
		}
	}

	/** Deactivation Request **/
	public class DeactivationRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private String Username;
		private String Password;

		public DeactivationRequest(String username, String password) {
			super("DeactivationRequest", "Client", "Server");
			this.Username = username;
			this.Password = password;
		}

		public String getUsername(){
			return this.Username;
		}
		public String getPassword(){
			return this.Password;
		}

	}

	/** Signed Deactivation Response **/
	public class SignedDeactivationResponse extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private byte[] deactivationMsg;
		private boolean isValidRequest;

		public SignedDeactivationResponse(boolean isValid) {
			super("SignedDeactivationResponse", "Server", "Client");
			isValidRequest = isValid;
		}

		public byte[] getDeactivationMsg(){
			return this.deactivationMsg;
		}
		public boolean getIsValidRequest(){
			return this.isValidRequest;
		}
		public void setDeactivationMsg(byte[] signedDeactivationMsg){
			this.deactivationMsg = signedDeactivationMsg;
		}
	}

	/** Update MDTP Status  **/
	public class UpdateStatus extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private MdtpStateDemo mState;
		private String pin;
		private String HWRecoveryPin;

		public UpdateStatus(String Source, String Dest){
			super("UpdateStatus", Source, Dest);
			MdtpStateDemo mState = new MdtpStateDemo();
			String pin = null;
			String HWRecoveryPin = null;
		}

		public MdtpStateDemo getStatus(){
			return mState;
		}
		public String getPin(){
			return this.pin;
		}
		public String getHWRecoveryPin(){
			return this.HWRecoveryPin;
		}

		public void setStatus(MdtpStateDemo deviceState){
			mState = deviceState;
		}
		public void setPin(String s){
			this.pin = s;
		}
		public void setHWRecoveryPin(String s){
			this.HWRecoveryPin = s;
		}


	}

	/** Update MDTP Status to Web  **/
	public class UpdateStateToWeb extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private MdtpStateDemo mState;
		private boolean isDuringLock;
		private boolean isDuringUnlock;
		private String MdtpPin;
		private String HWRecoveryPin;
		private String IsvDeviceID;
		private String IsvFreindlyName;

		public UpdateStateToWeb(){
			super("UpdateStatus", "Server", "WebInterface");
			this.isDuringLock = false;
			this.isDuringUnlock = false;
			mState = new MdtpStateDemo();
			MdtpPin = null;
			HWRecoveryPin = null;
			IsvDeviceID = null;
			IsvFreindlyName = null;
		}

		public MdtpStateDemo getStatus(){
			return mState;
		}
		public boolean getLockingStatus(){
			return this.isDuringLock;
		}
		public boolean getUnlockingStatus(){
			return this.isDuringUnlock;
		}
		public String getPin(){
			return this.MdtpPin;
		}
		public String getHWRecoveryPin(){
			return this.HWRecoveryPin;
		}
		public String getIsvDeviceID(){
			return this.IsvDeviceID;
		}
		public String getIsvFreindlyName(){
			return this.IsvFreindlyName;
		}

		public void setStatus(MdtpStateDemo deviceState){
			mState = deviceState;
		}
		public void setLockingStatus(boolean b){
			this.isDuringLock = b;
		}
		public void setUnlockingStatus(boolean b){
			this.isDuringUnlock = b;
		}
		public void setPin(String s){
			this.MdtpPin = s;
		}
		public void setHWRecoveryPin(String s){
			this.HWRecoveryPin = s;
		}
		public void setIsvDeviceID(String s){
			this.IsvDeviceID = s;
		}
		public void setIsvFreindlyName(String s){
			this.IsvFreindlyName = s;
		}

	}

	/** Update MDTP Status Request **/
	public class UpdateStatusRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;

		public UpdateStatusRequest(String Source){
			super("UpdateStatusRequest", Source, "Server");
		}
	}

	/** Remote Lock (Kill) Request **/
	public class RemoteLockRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;

		public RemoteLockRequest(){
			super("RemoteLockRequest", "WebInterface", "Server");
		}
	}

	/** Remote Lock (Kill) Message **/
	public class RemoteLockMessage extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private byte[] killMsg;

		public RemoteLockMessage(){
			super("RemoteLockMessage", "Server", "Client");
			killMsg = null;
		}
		public byte[] getKillMsg(){
			return this.killMsg;
		}
		public void setKillMsg(byte[] msg){
			this.killMsg = msg;
		}
	}

	/** Remote Lock (Kill) Response **/
	public class RemoteLockResponse extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private boolean lockResult;

		public RemoteLockResponse(boolean Result, String Source, String Dest){
			super("RemoteLockResponse", Source, Dest);
			this.lockResult = Result;
		}

		public boolean getLockResult(){
			return this.lockResult;
		}
	}

	/** Remote Restore (Unlock) Request **/
	public class RemoteRestoreRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private String deviceIMEI;
		private String username;
		private String password;
		public RemoteRestoreRequest(String IMEI, String user, String pass){
			super("RemoteRestoreRequest", "Client", "Server");
			this.deviceIMEI = IMEI;
			this.username = user;
			this.password = pass;
		}

		public String getIMEI(){
			return deviceIMEI;
		}
		public String getPassword(){
			return this.password;
		}
		public String getUsername(){
			return this.username;
		}
	}

	/** Web Restore (Unlock) Request **/
	public class WebRestoreRequest extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		public WebRestoreRequest(){
			super("WebRestoreRequest", "WebInterface", "Server");

		}
	}

	/** Remote Restore (Unlock) Message **/
	public class RemoteRestoreMessage extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private byte[] restoreMsg;
		private boolean restoreStatus;

		public RemoteRestoreMessage(boolean status){
			super("RemoteRestoreMessage", "Server", "Client");
			restoreStatus = status;
		}

		public byte[] getRestoreMsg(){
			return this.restoreMsg;
		}
		public void setRestoreMsg(byte[] msg){
			this.restoreMsg = msg;
		}
		public boolean getRestoreStatus(){
			return 	restoreStatus;
		}
	}

	/** Remote Restore (Unlock) Response **/
	public class RemoteRestoreResponse extends MdtpMessage {
		private static final long serialVersionUID = 1L;
		private String restoreResult;

		public RemoteRestoreResponse(String Result, String Source, String Dest){
			super("RemoteRestoreResponse", Source, Dest);
			this.restoreResult = Result;
		}

		public String getLockResult(){
			return this.restoreResult;
		}
	}

	/** General Acknowledgment **/
	public class Acknowledgment extends MdtpMessage {
		private static final long serialVersionUID = 1L;

		public Acknowledgment(String Source, String Dest){
			super("Acknowledgment", Source, Dest);
		}
	}

	/** Client to server poll message **/
	public class PollServer extends MdtpMessage {
		private static final long serialVersionUID = 1L;

		public PollServer(){
			super("PollServer", "Client", "Server");
		}
	}

	/** Stop Pending Requests Message **/
	public class StopPendingRequests extends MdtpMessage {
		private static final long serialVersionUID = 1L;

		public StopPendingRequests(){
			super("StopPendingRequests", "WebInterface", "Server");
		}
	}

}
