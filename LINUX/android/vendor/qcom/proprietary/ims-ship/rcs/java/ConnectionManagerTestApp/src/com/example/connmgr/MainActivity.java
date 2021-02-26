/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.example.connmgr;

import java.util.*;
import java.text.SimpleDateFormat;
import java.text.*;
import java.lang.*;

import java.net.InetAddress;
import java.net.Socket;
import javax.net.SocketFactory;

import android.net.ConnectivityManager;
import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.AsyncTask;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Button;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionInfo;

import com.qualcomm.qti.imscmservice.V1_0.IImsCmService;
import com.qualcomm.qti.imscmservice.V1_0.IImsCmServiceListener;
import com.qualcomm.qti.imscmservice.V1_0.IImsCMConnectionListener;
import com.qualcomm.qti.imscmservice.V1_0.IImsCMConnection;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_CONN_MESSAGE;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_MESSAGE_TYPE;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_SIP_PROTOCOL;
import com.qualcomm.qti.imscmservice.V1_0.QIMSCM_USER_CONFIG;
import com.qualcomm.qti.imscmservice.V1_0.QIMS_CM_DEVICE_CONFIG;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_CONFIG_DATA;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_AUTOCONFIG_DATA;
import com.example.connmgr.ConnectionGlobalData;



public class MainActivity extends Activity {
    final static String TAG = "UI_ConnMgr_MainActivity";
    Button button;
    Button buttonCreatecon;
    static int nSetStatus = 1;
    private String requestNwAction = "android.cne.action.REQUEST_NETWORK";

    private String mIccId = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initButtons();

        ConnectionGlobalData.mContext = getApplicationContext();

        if(ConnectionGlobalData.mContext == null) {
            Log.e(TAG, "Context is NULL");
        }

        Log.d(TAG, "before creating intent filter");

        IntentFilter filter = new IntentFilter();

        Log.d(TAG, "after creating intent filter before add action " +
              requestNwAction);

        filter.addAction(requestNwAction);
        Log.d(TAG, "after add action " + requestNwAction);

        BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
            //Log.d(TAG, "new Broadcast receiver before onReceive definition");
            @Override
            public void onReceive(Context c, Intent intent) {
                if(intent.getAction() == requestNwAction) {
                    Log.d(TAG, "Received request network Intent");
                    new RequestNetworkTask(ConnectionGlobalData.mContext).execute();
                }
                else
                {
                    Log.d(TAG, "Received request network Intent but it is not equal to " +
                          requestNwAction);
                }
            }
        };

        Log.d(TAG, "before registerReceiver with mIntenetReceiver, filter");
        ConnectionGlobalData.mContext.registerReceiver(mIntentReceiver, filter);
        Log.d(TAG, "after registerReceiver with mIntenetReceiver, filter");
    }

    private void getDefaultIccId() {
        //check if this is multiSIM.
        SubscriptionManager subMgr =  SubscriptionManager.from(getApplicationContext());
        List<SubscriptionInfo> subInfoList = subMgr.getActiveSubscriptionInfoList();
        if (subInfoList.size() > 1) {
            Log.i(TAG, "RCSStartRCSService Detected MultiSim");
            //return;
        }
        mIccId = subInfoList.get(0).getIccId();
        Log.i(TAG, "getDefaultIccId default mIccId [" + mIccId +"]");
    }

    public void initButtons()
    {
        getDefaultIccId();
        initCreateConnectionManagerButton();
        initActivateSub0Button();
        initActivateSub1Button();
        initCreateSMConnectionButton();
        initCreateIMConnectionButton();
        initCreateInvalidConnectionButton();
        initTriggerRegistrationButton();
        initTriggerDeRegistrationButton();
        initSendSMMessageButton();
        initSendIMMessageButton();
        initCloseSMConnectionButton();
        initCloseIMConnectionButton();
        initCloseConnectionManagerButton();
        initGetConfigurationManagerButton();
        initMethodResponseButton();
        initRegRestorationButton();
        initTestSendingPacketsViaSocket();
        initAutoconfigButton();
        initsetStatusButton();
        initsendMsgButton();
    }
    private void initsendMsgButton() {
    // TODO Auto-generated method stub
    Button sendMsgButton = (Button) findViewById(R.id.sendMsg);
    handlesendMsgButtonClick(sendMsgButton);
    }

    private void initAutoconfigButton() {
    Button autoconfigButton = (Button) findViewById(R.id.autoconfig);
    handleAutoconfigButtonClick(autoconfigButton);
    }

    private void initsetStatusButton() {
    // TODO Auto-generated method stub
    Button setStatusButton = (Button) findViewById(R.id.setStatus);
    handlesetStatusButtonClick(setStatusButton);
    }

    private void handlesendMsgButtonClick(Button sendMsgButton) {
    sendMsgButton.setOnClickListener(new OnClickListener() {
    @Override
      public void onClick(View arg0) {
        Log.d(TAG, "sendMsgButton click");
        try{
            if (ConnectionGlobalData.connMgr!= null &&
               ConnectionGlobalData.connSmMap.get(ConnectionGlobalData.activeWorkingSub) != null )
            {
               if ( ConnectionGlobalData.connMgrInitialised == 1 &&
                    ConnectionGlobalData.connSmInitialisedArray[ConnectionGlobalData.activeWorkingSub] == 1)
               {
                   int iRetValue ;
                   Log.d(TAG, "sendmsg");
                   /*Fill in the message related params listed below*/
                   String pstringMsg = "MESSAGE sip:310008984630745@ims.vz.net "+
                                       "SIP/2.0\r\n"+
                                       "From: \"TestUE1\" "+
                                       "<sip:11111@ims.cingularme.com>;"+
                                       "tag=3476455352\r\n"+
                                       "To: <sip:11111@ims.cingularme.com>\r\n"+
                                       "CSeq: 255229831 MESSAGE\r\n"+
                                       "Call-ID: 3476455304_1570329456@10.242.114.221\r\n"+
                                       "Via: SIP/2.0/UDP 10.242.114.221:5060;"+
                                       "branch=z9hG4bK504163233\r\n"+
                                       "Max-Forwards: 70\r\n"+
                                       "Route: <sip:10.252.42.41:5060;lr>\r\n"+
                                       "P-Access-Network-Info: 3GPP-E-UTRAN-FDD; "+
                                       "utran-cell-id-3gpp=3114800000011C000\r\n"+
                                       "Accept-Contact: *;+g.3gpp.iari-ref=\""+
                                       "urn:urn-7:3gpp-service.ims.icsi.oma.cpm.msg\"\r\n"+
                                       "Content-Type: text/plain\r\n"+
                                       "Expires: 2345\r\nContent"+
                                       "-Length: 8\r\n\r\nSM_MESSAGE\r\n\r\n";

                   IMSCM_CONN_MESSAGE msg= new IMSCM_CONN_MESSAGE();
                   /* Refers to the SIP request/response call ID value as per RFC 3261 */
                   msg.pCallId = "3476455304_1570329456@10.242.114.221";
                   byte[] bArr = pstringMsg.getBytes();
                   for(Byte b : bArr) {
                      msg.pMessage.add(b);
                   }

                   /*Message destination;
                     for SIP requests, an outbound proxy address can be specified;
                     for SIP responses, a header address can be used
                   */
                   msg.pOutboundProxy = "10.242.166.1";
                   /* protocol can be TCP,UDP,NONE */
                   msg.eProtocol = IMSCM_SIP_PROTOCOL.IMSCM_SIP_PROTOCOL_UDP;
                   msg.iRemotePort = (byte) 5060;//set remote port
                   /* Message Type : REQUEST,RESPONSE,NONE */
                   msg.eMessageType = IMSCM_MESSAGE_TYPE.IMSCM_MESSAGE_TYPE_REQUEST;
                   /* 0 -request ; 1-response; 2-invalidmax*/
                   iRetValue=ConnectionGlobalData.connSmMap.get(
                       ConnectionGlobalData.activeWorkingSub).sendMessage(
                           msg,
                           ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                   msg.pMessage.clear();

                   Log.d(TAG, "sendMsgButton RetValue "+ iRetValue);
               }
               else
               {
                   Log.d(TAG, "setStatus Connection Manager/ SM connection is not REGISTERED ");
               }
            }
            else
            {
               Log.d(TAG, " setStatus Connection Manager/ SM connection is NULL");
            }
        }catch (Exception e){
            Log.d(TAG, "Exception raised" + e);
        }
    }});
    }

    private void handleAutoconfigButtonClick(Button autoconfigButton) {
        autoconfigButton.setOnClickListener(new OnClickListener() {
        @Override
        public void onClick(View arg0) {
            Log.d(TAG, "autoconfigButton click");
            try
            {
              if (ConnectionGlobalData.connMgr!= null){
                if (ConnectionGlobalData.connMgrInitialised == 1)
                {
                  Log.d(TAG, "handleAutoconfigButtonClick : Calling Autoconfig: iccid "
                        + ConnectionGlobalData.activeWorkingSub);
                  /* autoConfigType = IMSCM_ConfigType.IMSCM_AUTO_CONFIG */
                  int status = ConnectionGlobalData.connMgr.getConfiguration(
                                    ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                    ConnectionGlobalData.autoConfigType,
                                    ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                }
                else
                {
                  Log.d(TAG, "iccid " + ConnectionGlobalData.activeWorkingSub +
                        " :handleAutoconfigButtonClick error connection manager is not initilised");
                }
            } else {
              Log.d(TAG, "handleAutoconfigButtonClick ConnectionGlobalData.connMgr is NULL");
            }}catch (Exception e){
               Log.d(TAG, "Exception raised" + e);
            }
        }});
    }

    private void handlesetStatusButtonClick(Button setStatusButton) {
        setStatusButton.setOnClickListener(new OnClickListener() {

        @Override
        public void onClick(View arg0) {
            Log.d(TAG, "setStatusButton click : iccid "+ ConnectionGlobalData.activeWorkingSub);
            try {
                if (ConnectionGlobalData.connMgr!= null &&
                   ConnectionGlobalData.connSmMap.get(ConnectionGlobalData.activeWorkingSub) != null )
                {
                   if ( ConnectionGlobalData.connMgrInitialised == 1 )
                   {
                       int iRetValue =0;
                       Log.d(TAG, "iccid" + ConnectionGlobalData.activeWorkingSub);
                       Log.d(TAG, ":setStatus = " + nSetStatus);
                       /*Sets the current status of the service(feature tag of the service),
                         which is session based and is using the current connection.
                         0 means none ; 1 means active ;2 means hold ; 3 means inactive
                       */
                       iRetValue=ConnectionGlobalData.connSmMap.get(
                           ConnectionGlobalData.activeWorkingSub).setStatus("+g.3gpp.iari-ref=\"urn:"+
                                                                       "urn-7:3gpp-service."+
                                                                       "ims.icsi.oma.cpm.msg\"",
                                                                       nSetStatus);
                       if( nSetStatus == 1 )
                           nSetStatus = 3;
                       else
                           nSetStatus = 1;
                       Log.d(TAG, "setstatus RetValue "+ iRetValue);
                   }
                   else
                   {
                       Log.d(TAG, "setStatus Connection Manager/ SM connection "+
                                  "is not REGISTERED 222");
                   }
               }
               else
               {
                   Log.d(TAG, " setStatus Connection Manager/ SM connection is NULL");
               }
           } catch (Exception e) {
               Log.d(TAG, "Exception raised" + e);
           }
        }});
    }

    private void initRegRestorationButton() {
        // TODO Auto-generated method stub
        Button connMgrButton = (Button) findViewById(R.id.regRestoration);
        //handleRegRestorationButtonClick(connMgrButton);
    }

/*
    private void handleRegRestorationButtonClick(Button connMgrButton) {
      connMgrButton.setOnClickListener(new OnClickListener() {
      @Override
      public void onClick(View arg0) {
        Log.d(TAG, "handleRegRestorationButtonClick click");
          if (ConnectionGlobalData.connMgr!= null)
          {
           if (ConnectionGlobalData.connMgrInitialised == 1)
           {
             int iRetValue ;
             Log.d(TAG, "RegRestoration");
             //iRetValue = ConnectionGlobalData.connMgr.RegRestoration("LTE", 200);
             //Call the function with method and responce code
             iRetValue = ConnectionGlobalData.connMgr.regRestoration("INVITE", 403);
             Log.d(TAG, "RegRestoration RetValue "+ iRetValue);
           }
           else
           {
             Log.d(TAG, "create IM connection error connection manager is not initilised");
           }
          } else {
           Log.d(TAG, "handleRegRestorationButtonClick ConnectionGlobalData.connMgr is NULL");
          }
      }});
  }*/

    private void initCreateConnectionManagerButton()
    {
        Button connMgrButton = (Button) findViewById(R.id.CreateManager);
        handleCreateConnectionManagerButtonClick(connMgrButton);
    }

    private void initActivateSub0Button()
    {
        Button connMgrButton = (Button) findViewById(R.id.ActivateSub0);
        handleActivateSub0ButtonClick(connMgrButton);
    }

    private void initActivateSub1Button()
    {
        Button connMgrButton = (Button) findViewById(R.id.ActivateSub1);
        handleActivateSub1ButtonClick(connMgrButton);
    }

    private void initCreateSMConnectionButton()
    {
        Button connButton = (Button) findViewById(R.id.CreateSMConn);
        handleCreateSMConnectionButtonClick(connButton);
    }

    private void initCreateIMConnectionButton()
    {
        Button connButton = (Button) findViewById(R.id.CreateIMConn);
        handleCreateIMConnectionButtonClick(connButton);
    }

    private void initCreateInvalidConnectionButton()
    {
        Button connButton = (Button) findViewById(R.id.CreateInvalidConn);
        handleCreateInvalidConnectionButtonClick(connButton);
    }

    private void initTriggerRegistrationButton()
    {
        Button connButton = (Button) findViewById(R.id.TriggerRegistration);
        handleTriggerRegistrationButtonClick(connButton);
    }

    private void initTriggerDeRegistrationButton()
    {
        Button connButton = (Button) findViewById(R.id.TriggerDeRegistration);
        handleTriggerDeRegistrationButtonClick(connButton);
    }

    private void initSendSMMessageButton()
    {
        Button connButton = (Button) findViewById(R.id.SendSMMessage);
        handleSendSMMessageButtonClick(connButton);
    }

    private void initSendIMMessageButton()
    {
        Button connButton = (Button) findViewById(R.id.SendIMMessage);
        handleSendIMMessageButtonClick(connButton);
    }

    private void initCloseSMConnectionButton()
    {
        Button connButton = (Button) findViewById(R.id.CloseSMConn);
        handleCloseSMConnectionButtonClick(connButton);
    }

    private void initCloseIMConnectionButton()
    {
        Button connButton = (Button) findViewById(R.id.CloseIMConn);
        handleCloseIMConnectionButtonClick(connButton);
    }

    private void initCloseConnectionManagerButton()
    {
        Button connMgrButton = (Button) findViewById(R.id.CloseManager);
        handleCloseConnectionManagerButtonClick(connMgrButton);
    }

    private void initGetConfigurationManagerButton()
    {
        Button getConfigurationButton = (Button) findViewById(R.id.GetConfiguration);
        handleGetConfigurationButtonClick(getConfigurationButton);
    }

    private void initMethodResponseButton()
    {
        Button methodResponseButton = (Button) findViewById(R.id.MethodResponse);
        handleMethodResponseButtonClick(methodResponseButton);
    }

    private void initTestSendingPacketsViaSocket()
    {
        Button socketSendButton = (Button) findViewById(R.id.socketSend);
        handleTestSendingPacketsViaSocket(socketSendButton);
    }

    private void handleTestSendingPacketsViaSocket(Button socketSendButton)
    {
        socketSendButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
               Log.d(TAG, "handleTestSendingPacketsViaSocket entered ");
               /*click getConfigurationbutton before sending packets via socket and
                 assign value directly from onConfigChange Callback
                 Callback populates: IMSCM_USER_CONFIG,
                                     IMSCM_DEVICE_CONFIG,
                                     IMSCM_AUTO_CONFIG
               */
               String localHost = ConnectionGlobalData.userConfigData.pLocalHostIPAddress;
               int localport = 54321;
               String packet = "Data Packet to be sent to remote.";
               Log.d(TAG, "localHost Ip address " + localHost);
               /*Got from the SIP Message. -- for testing use ice server address */
               String remoteIP = "10.242.166.1";
               int remoteport = 60450;  // Got from the SIP Message.
               //InetAddress remoteInetIp = InetAddress.getByName(remoteIP);
               //byte[] bytes = remoteInetIp.getAddress();
               ConnectivityManager mConnectivityMgr;
               try{
                   if( ConnectionGlobalData.mContext == null)
                   {
                       Log.d(TAG, "ConnectionGlobalData.mContext is null");
                   }
                   mConnectivityMgr =
                       (ConnectivityManager) ConnectionGlobalData.mContext.getSystemService(
                                                                     Context.CONNECTIVITY_SERVICE);
                   if( mConnectivityMgr != null){
                        /* if(! ((Object) mConnectivityMgr)
                           .requestRouteToHostAddress(
                                     ConnectivityManager.TYPE_MOBILE,
                                     InetAddress.getByName(remoteIP)) )
                        {
                            Log.d(TAG, "Cannot establish route to " +
                                       InetAddress.getByName(remoteIP));
                        }
                        else
                        {
                            Log.d(TAG, "Route Established to " +
                                       InetAddress.getByName(remoteIP));
                        }*/

                       Log.d(TAG, "Creating and sending message to remoteIP " +
                             InetAddress.getByName(remoteIP) +
                             " from host "+
                             InetAddress.getByName(localHost));

                       Socket soc = SocketFactory.getDefault()
                                               .createSocket(InetAddress.getByName(remoteIP),
                                                             remoteport,
                                                             InetAddress.getByName(localHost),
                                                             localport);
                       soc.getOutputStream().write(packet.getBytes());
                   }
                   else
                   {
                       Log.d(TAG, "Connectivity Manager is NULL");
                   }
               }catch(Exception ex)
               {
                   ex.printStackTrace();
               }
           }});
    }

    private void handleGetConfigurationButtonClick(Button getConfigurationButton)
    {
        getConfigurationButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
               try {
                   /* returns asynchronous callback onConfigChange in the listener */
                   int status =
                       ConnectionGlobalData.connMgr.getConfiguration(
                                                               ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                                               ConnectionGlobalData.configType,
                                                               ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                   Log.d(TAG, "ImsCmServiceImpl.getConfiguration called: iccid:"+
                         ConnectionGlobalData.activeWorkingSub);
               }catch(Exception e){
                   Log.d(TAG, "Exception raised" + e);
               }
           }});
    }

    private void handleMethodResponseButtonClick(Button methodResponseButton)
    {
        methodResponseButton.setOnClickListener(new OnClickListener() {
          public void onClick(View v) {
            try {
              if (ConnectionGlobalData.connMgr!= null &&
                  ConnectionGlobalData.connMgrInitialised == 1){
                  int iRetValue;
                  Log.d(TAG, "MethodResponse: iccid: " +
                                 ConnectionGlobalData.activeWorkingSub);
                  iRetValue = ConnectionGlobalData.connMgr.methodResponse(
                                                           ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                                           "INVITE",
                                                           ConnectionGlobalData.responseCode,
                                                           ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                  Log.d(TAG, "MethodResponse RetValue "+ iRetValue);

              } else
              {
                 Log.d(TAG, "create MethodResponse error connection manager "+
                            "is not initilised : iccid: " +
                            ConnectionGlobalData.activeWorkingSub);
              }
            } catch (Exception e) {
                Log.d(TAG, "Exception raised" + e);
            }
          }});
    }

    private void handleCreateConnectionManagerButtonClick(Button connMgrButton) {
        connMgrButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
               try {
                   /*startService---- Starts the native Android service */
                   ConnectionGlobalData.connMgr =
                       IImsCmService.getService("qti.ims.connectionmanagerservice");
                   Log.d(TAG, "ImsCmServiceImpl.InitializeService called");

                   SubscriptionManager subMgr =  SubscriptionManager.from(getApplicationContext());
                   //create CM for all active subs
                   List<SubscriptionInfo> subInfoList = subMgr.getActiveSubscriptionInfoList();
                   for( int i=0; i < subInfoList.size(); i++) {
                       IImsCmServiceListener connMgrListener = new ConnectionManagerListenerImpl();
                       ConnectionGlobalData.connMgrListenerMap.put(i, connMgrListener);
                       mIccId = subInfoList.get(i).getIccId();
                   /* calls onServiceReady Callback and populates serviceHandle */
                   int status = ConnectionGlobalData.connMgr.InitializeService(
                                                              mIccId,
                                                              ConnectionGlobalData.connMgrListenerMap.get(i),
                                                              ConnectionGlobalData.userDataArray[i]);
                       Log.d(TAG+mIccId, "ImsCmServiceImpl connMgr status: " + status);
                   }
                } catch (Exception e) {
                    Log.d(TAG, "Exception raised" + e);
                }
           }});
    }

    private void handleActivateSub0ButtonClick(Button connMgrButton) {
        connMgrButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
               try {
                   Log.d(TAG, "ActivateSub0 called");
                   ConnectionGlobalData.activeWorkingSub = 0;
               } catch (Exception e) {
                   Log.d(TAG, "Exception raised" + e);
               }
           }});
    }

    private void handleActivateSub1ButtonClick(Button connMgrButton) {
        connMgrButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
               try {
                   Log.d(TAG, "ActivateSub1 called");
                   ConnectionGlobalData.activeWorkingSub = 1;
               } catch (Exception e) {
                   Log.d(TAG, "Exception raised" + e);
               }
           }});
    }

    private void handleCreateSMConnectionButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
               try{
                   if (ConnectionGlobalData.connMgr!= null){
                       if ( ConnectionGlobalData.connMgrInitialised == 1)
                       {
                           Log.d(TAG, "createconnection: iccid: " +
                                 ConnectionGlobalData.activeWorkingSub);
                           /*Creates new native IMS Connection object.
                             In this case SM feature tag is used */
                           ConnectionGlobalData.connSmListenerMap.put(ConnectionGlobalData.activeWorkingSub,
                                      new ConnectionListenerImpl(ConnectionGlobalData.activeWorkingSub));
                           IImsCMConnection connSm =
                               ConnectionGlobalData.connMgr.createConnection(
                                   ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                   ConnectionGlobalData.connSmListenerMap.get(ConnectionGlobalData.activeWorkingSub),
                                   ConnectionGlobalData.SM_FEATURE_TAG);
                           ConnectionGlobalData.connSmMap.put(ConnectionGlobalData.activeWorkingSub, connSm);
                           if ( ConnectionGlobalData.connSmMap.get(ConnectionGlobalData.activeWorkingSub) != null &&
                                ConnectionGlobalData.connSmListenerMap.get(ConnectionGlobalData.activeWorkingSub) != null )
                           {
                               ConnectionGlobalData.connSmMap.get(
                                  ConnectionGlobalData.activeWorkingSub).addListener(
                                      ConnectionGlobalData.connSmListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                           }else{
                              Log.d(TAG, "connSm/connSmListener is NULL - iccid" +
                                    ConnectionGlobalData.activeWorkingSub);
                           }
                       }else{
                           Log.d(TAG, "createconnection is not initialised ");
                       }
                   }}catch (Exception e){
                       Log.d(TAG,"Exception raised" + e);
                   }}});
    }

    private void handleCreateIMConnectionButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try {
                    if (ConnectionGlobalData.connMgr!= null){
                        if ( ConnectionGlobalData.connMgrInitialised == 1){
                           Log.d(TAG, "create IM connection : iccid : "+
                                 ConnectionGlobalData.activeWorkingSub);
                           /*Creates new native IMS Connection object.
                             In this case IM feature tag is used*/
                           ConnectionGlobalData.connImListenerMap.put(ConnectionGlobalData.activeWorkingSub,
                           new ConnectionListenerImpl(ConnectionGlobalData.activeWorkingSub));
                           IImsCMConnection connIm =
                               ConnectionGlobalData.connMgr.createConnection(
                                   ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                   ConnectionGlobalData.connImListenerMap.get(ConnectionGlobalData.activeWorkingSub),
                                   ConnectionGlobalData.IM_FEATURE_TAG);
                           ConnectionGlobalData.connImMap.put(ConnectionGlobalData.activeWorkingSub, connIm);

                           if ( ConnectionGlobalData.connImMap.get(ConnectionGlobalData.activeWorkingSub) != null &&
                                ConnectionGlobalData.connImListenerMap.get(ConnectionGlobalData.activeWorkingSub)!= null)
                           {
                               ConnectionGlobalData.connImMap.get(ConnectionGlobalData.activeWorkingSub).addListener(
                                   ConnectionGlobalData.connImListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                           }else{
                               Log.d(TAG, "create IM connection Connection IM/"+
                                          "connImListener is null : iccid :" +
                                     ConnectionGlobalData.activeWorkingSub);
                           }
                        }else{
                           Log.d(TAG, "create IM connection Connection Manager is not initialised");
                        }
                    }else{
                       Log.d(TAG, "connMgr is NULL");
                    }} catch (Exception e) {
                       Log.d(TAG, "Exception raised" + e);
                   }}});
    }

    private void handleCreateInvalidConnectionButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
           public void onClick(View v) {
                try {
                    if (ConnectionGlobalData.connMgr!= null){
                        if ( ConnectionGlobalData.connMgrInitialised == 1){
                           Log.d(TAG, "create Invalidconnection: iccid: " +
                                 ConnectionGlobalData.activeWorkingSub);
                           /*Creates new native IMS Connection object.
                             In this case SM feature tag is used*/
                           ConnectionGlobalData.connInvalidListenerMap.put(ConnectionGlobalData.activeWorkingSub,
                               new ConnectionListenerImpl(ConnectionGlobalData.activeWorkingSub));
                           IImsCMConnection connInvalid =
                               ConnectionGlobalData.connMgr.createConnection(
                                   ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                   ConnectionGlobalData.connInvalidListenerMap.get(ConnectionGlobalData.activeWorkingSub),
                                   ConnectionGlobalData.INVALID_FEATURE_TAG3);
                           ConnectionGlobalData.connInvalidMap.put(ConnectionGlobalData.activeWorkingSub, connInvalid);

                            if ( ConnectionGlobalData.connInvalidMap.get(ConnectionGlobalData.activeWorkingSub) != null &&
                                ConnectionGlobalData.connInvalidListenerMap.get(ConnectionGlobalData.activeWorkingSub)!= null)
                            {
                               ConnectionGlobalData.connInvalidMap.get(ConnectionGlobalData.activeWorkingSub).addListener(
                                   ConnectionGlobalData.connInvalidListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                            }
                            else{
                               Log.d(TAG, "create Invalid connection Connection/"+
                                          "connInvalidListener is null : iccid: "+
                                     ConnectionGlobalData.activeWorkingSub);
                            }
                        }else{
                           Log.d(TAG, "create Invalid connection - Connection Manager "+
                                      "is not initialised");
                        }
                    }else{
                       Log.d(TAG, "connMgr is NULL");
                    }} catch (Exception e) {
                       Log.d(TAG, "Exception raised" + e);
                    }
            }});
    }

    private void handleTriggerRegistrationButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    if (ConnectionGlobalData.connMgr!= null){
                        if (ConnectionGlobalData.connMgrInitialised == 1)
                        {
                            int iRetValue ;
                            Log.d(TAG, "TriggerRegistration: iccid:" +
                                  ConnectionGlobalData.activeWorkingSub);
                            /*triggerRegistration---- Triggers registration.
                              This must be done after all the connections are created,
                              enabling registration triggering with all the required
                              feature tags simultaneously*/
                            iRetValue = ConnectionGlobalData.connMgr.triggerRegistration(
                                ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                            Log.d(TAG, "TriggerRegistration RetValue "+ iRetValue);
                        }
                        else
                        {
                            Log.d(TAG, "create IM connection error connection manager "+
                                       "is not initilised : iccid: " +
                                  ConnectionGlobalData.activeWorkingSub);
                        }
                    }}catch (Exception e){
                        Log.d(TAG, "Exception raised" + e);
                    }}});
    }

    private void handleTriggerDeRegistrationButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    if (ConnectionGlobalData.connMgr!= null){
                        if (ConnectionGlobalData.connMgrInitialised == 1)
                        {
                            int iRetValue ;
                            Log.d(TAG, "TriggerDeRegistration: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            /*triggerDeRegistration---- Triggers a deregistration.
                              This API removes all FTs, performs a PDN release,
                              and brings up the PDN*/
                            iRetValue = ConnectionGlobalData.connMgr.triggerDeRegistration(
                                ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                            Log.d(TAG, "TriggerDeRegistration RetValue "+ iRetValue);
                        }
                        else{
                            Log.d(TAG, "create IM connection error connection manager "+
                                       "is not initilised: iccid :"+
                                  ConnectionGlobalData.activeWorkingSub);
                        }
                    }}catch (Exception e){
                        Log.d(TAG, "Exception raised" + e);
                    }}});
    }

    private void handleSendSMMessageButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    if (ConnectionGlobalData.connMgr!= null &&
                        ConnectionGlobalData.connSmMap.get(ConnectionGlobalData.activeWorkingSub) != null )
                    {
                        if ( ConnectionGlobalData.connMgrInitialised == 1 &&
                             ConnectionGlobalData.connSmInitialisedArray[ConnectionGlobalData.activeWorkingSub] == 1)
                        {
                            int messageId = (int) (new Date().getTime()/1000);
                            Log.d(TAG, "Send SM Message: iccid :"+
                                  ConnectionGlobalData.activeWorkingSub);
                            String msgString = "MESSAGE sip:22222@ims.cingularme.com SIP/2.0\r\n"+
                                               "From: \"TestUE1\" <sip:11111@ims.cingularme.com>;"+
                                               "tag=3486455352\r\n"+
                                               "To: <sip:22222@ims.cingularme.com>\r\n"+
                                               "CSeq: 255229832 MESSAGE\r\n"+
                                               "Call-ID: 3486455304_1570329456@"+
                                               "[2002:c023:9c17:401e::3]\r\n"+
                                               "Via: SIP/2.0/UDP[2002:c023:9c17:401e::3]:5060;"+
                                               "branch=z9hG4bK504163233\r\n"+
                                               "Max-Forwards: 70\r\n"+
                                               "Route: <sip:[2002:c023:9c17:401e::3]:5060;lr>\r\n"+
                                               "P-Access-Network-Info: 3GPP-E-UTRAN-FDD; "+
                                               "utran-cell-id-3gpp=3114800000011C000\r\n"+
                                               "Accept-Contact: *;+g.3gpp.icsi-ref=\"urn:urn-7:"+
                                               "3gpp-service.ims.icsi.oma.cpm.session\"\r\n"+
                                               "Content-Type: text/plain\r\n"+
                                               "Expires: 2345\r\n"+
                                               "Content-Length: 8\r\n\r\nSM_MESSAGE\r\n\r\n";

                            IMSCM_CONN_MESSAGE msg = new IMSCM_CONN_MESSAGE();
                            /* Refers to the SIP request/response call ID value as per RFC 3261 */
                            msg.pCallId = "3486455304_1570329456@[2002:c023:9c17:401e::3]";

                            /* Message Type : REQUEST,RESPONSE,NONE */
                            msg.eMessageType = IMSCM_MESSAGE_TYPE.IMSCM_MESSAGE_TYPE_REQUEST;

                            /* protocol can be TCP,UDP,NONE */
                            msg.eProtocol = IMSCM_SIP_PROTOCOL.IMSCM_SIP_PROTOCOL_UDP;

                            /*Message destination; for SIP requests, an outbound
                              proxy address can be specified;
                              for SIP responses, a header address can be used*/
                            msg.pOutboundProxy = "[2002:c023:9c17:401e::3]";
                            msg.iRemotePort = (byte) 5060;//set remote port
                            byte[] bArr = msgString.getBytes();
                            for(Byte b : bArr) {
                                msg.pMessage.add(b);
                            }
                            int ret_status = ConnectionGlobalData.connSmMap.get(
                                ConnectionGlobalData.activeWorkingSub).sendMessage(
                                    msg,ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                            msg.pMessage.clear();
                            Log.d(TAG, "Send SM Message : status is " + ret_status);
                        }
                        else
                        {
                            Log.d(TAG, "SendSM Connection Manager/ SM connection "+
                                       "is not REGISTERED : iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                        }
                    }else{
                        Log.d(TAG, " SendSM Connection Manager/ SM connection is NULL");
                    }}catch (Exception e){
                        Log.d(TAG, "Exception raised" + e);
                    }}});
    }

    private void handleSendIMMessageButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    if (ConnectionGlobalData.connMgr!= null &&
                        ConnectionGlobalData.connImMap.get(ConnectionGlobalData.activeWorkingSub) != null )
                    {
                        if ( ConnectionGlobalData.connMgrInitialised == 1)
                        {
                            String msgString = "MESSAGE sip:22222@ims.cingularme.com SIP/2.0\r\n"+
                                               "From: \"TestUE1\" <sip:11111@ims.cingularme.com>;"+
                                               "tag=3486455352\r\n"+
                                               "To: <sip:22222@ims.cingularme.com>\r\n"+
                                               "CSeq: 255229832 MESSAGE\r\n"+
                                               "Call-ID: 3486455304_1570329456@"+
                                               "[2002:c023:9c17:401e::3]\r\n"+
                                               "Via: SIP/2.0/UDP[2002:c023:9c17:401e::3]:5060;"+
                                               "branch=z9hG4bK504163233\r\n"+
                                               "Max-Forwards: 70\r\n"+
                                               "Route: <sip:[2002:c023:9c17:401e::3]:5060;"+
                                               "lr>\r\nP-Access-Network-Info: 3GPP-E-UTRAN-FDD;"+
                                               " utran-cell-id-3gpp=3114800000011C000\r\n"+
                                               "Accept-Contact: *;+g.3gpp.icsi-ref=\"urn:"+
                                               "urn-7:3gpp-service.ims.icsi.oma.cpm.session\"\r\n"+
                                               "Content-Type: text/plain\r\n"+
                                               "Expires: 2345\r\nContent-Length: 8\r\n\r\n"+
                                               "IM_MESSAGE\r\n\r\n";

                            IMSCM_CONN_MESSAGE msg = new IMSCM_CONN_MESSAGE();
                            msg.pCallId = "3486455304_1570329456@[2002:c023:9c17:401e::3]";
                            msg.eMessageType = IMSCM_MESSAGE_TYPE.IMSCM_MESSAGE_TYPE_REQUEST;
                            msg.eProtocol = IMSCM_SIP_PROTOCOL.IMSCM_SIP_PROTOCOL_UDP;
                            msg.pOutboundProxy = "[2002:c023:9c17:401e::3]";
                            msg.iRemotePort = (byte) 5060;
                            byte[] bArr = msgString.getBytes();
                            for(Byte b : bArr) {
                                msg.pMessage.add(b);
                            }

                            ConnectionGlobalData.connImMap.get(
                                ConnectionGlobalData.activeWorkingSub).sendMessage(
                                    msg,
                                    ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                            Log.d(TAG, "Send IM Message: iccid: "+
                                  ConnectionGlobalData.activeWorkingSub);
                            msg.pMessage.clear();
                            Log.d(TAG, "Send IM Message");
                        }else{
                            Log.d(TAG, "Send IM Message");
                        }
                    }
                }catch (Exception e){
                    Log.d(TAG, "Exception raised" + e);
                }
            }});
    }

    private void handleCloseConnectionManagerButtonClick(Button connMgrButton) {
        connMgrButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    Log.d(TAG, "CloseConnectionManagerButtonClick: iccid:"+
                          ConnectionGlobalData.activeWorkingSub);
                    if (ConnectionGlobalData.connMgr!= null){
                        if (ConnectionGlobalData.connMgrListenerMap.get(ConnectionGlobalData.activeWorkingSub)!= null )
                        {
                            Log.d(TAG, "ImsCmServiceImpl removeListener called: iccid"+
                                  ConnectionGlobalData.activeWorkingSub);
                            Log.d(TAG, "ImsCmServiceImpl.removeListener"+
                                  ConnectionGlobalData.connMgrListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                            ConnectionGlobalData.connMgr.removeListener(
                                ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                ConnectionGlobalData.connMgrListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                        }
                        Log.d(TAG, "ConnectionManagerNativeImpl.close  called: iccid :"+
                              ConnectionGlobalData.activeWorkingSub);
                        /* Closes the Connection Manager.
                           Closing the manager forces pending connection objects to be
                           immediately deleted regardless of what state they are in*/
                        ConnectionGlobalData.connMgr.closeService(
                            ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub]);
                    }
                    else
                    {
                        Log.d(TAG, "CloseConnectionManagerButtonClick/connection Mgr is NULL: iccid :"+
                              ConnectionGlobalData.activeWorkingSub);
                    }
                    Log.d(TAG, "CloseConnectionManagerButtonClick end: iccid"+
                          ConnectionGlobalData.activeWorkingSub);
                }catch (Exception e){
                    Log.d(TAG, "Exception raised" + e);
                }}});
    }

    private void handleCloseSMConnectionButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    if (ConnectionGlobalData.connMgr!= null &&
                        ConnectionGlobalData.connSmMap.get(ConnectionGlobalData.activeWorkingSub) != null )
                    {
                        if ( ConnectionGlobalData.connMgrInitialised == 1)
                        {
                            Log.d(TAG, "SM closeAllTransactions: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            /*closeAllTransactions---- Terminates all transactions
                              being handled by the connection object*/
                            ConnectionGlobalData.connSmMap.get(
                                ConnectionGlobalData.activeWorkingSub).closeAllTransactions(
                                    ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);
                            Log.d(TAG, "Remove SM listener: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            if (ConnectionGlobalData.connSmListenerMap.get(ConnectionGlobalData.activeWorkingSub)!= null )
                                ConnectionGlobalData.connSmMap.get(
                                    ConnectionGlobalData.activeWorkingSub).removeListener(
                                        ConnectionGlobalData.connSmListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                            Log.d(TAG, "Close SM connection: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            /*ConnectionNativeImpl.close---- Closes the connection and
                              triggers deregistration of the associated URI*/
                            ConnectionGlobalData.connMgr.closeConnection(
                                ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                ConnectionGlobalData.connSmMap.get(ConnectionGlobalData.activeWorkingSub));
                        }
                        else{
                            Log.d(TAG, "CloseSMConnectionButtonClick Connection Manager "+
                                       "is not initialised: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                        }
                    }else{
                        Log.d(TAG, "CloseSMConnectionButtonClick/connection Mgr or "+
                                   "SM Conn is NULL: iccid:"+
                              ConnectionGlobalData.activeWorkingSub);
                    }
                }catch (Exception e){
                    Log.d(TAG, "Exception raised" + e);
                }}});
    }

    private void handleCloseIMConnectionButtonClick(Button connButton) {
        connButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                try{
                    if (ConnectionGlobalData.connMgr!= null &&
                        ConnectionGlobalData.connImMap.get(ConnectionGlobalData.activeWorkingSub) != null )
                    {
                        if ( ConnectionGlobalData.connMgrInitialised == 1)
                        {
                            Log.d(TAG, "Close IM closeAllTransactions: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            /*closeAllTransactions---- Terminates all transactions
                              being handled by the connection object*/
                            ConnectionGlobalData.connImMap.get(
                                ConnectionGlobalData.activeWorkingSub).closeAllTransactions(
                                ConnectionGlobalData.userDataArray[ConnectionGlobalData.activeWorkingSub]);

                            Log.d(TAG, "Remove IM listener: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            if ( ConnectionGlobalData.connImListenerMap.get(ConnectionGlobalData.activeWorkingSub)!= null ){
                                ConnectionGlobalData.connImMap.get(
                                    ConnectionGlobalData.activeWorkingSub).removeListener(
                                        ConnectionGlobalData.connImListenerMap.get(ConnectionGlobalData.activeWorkingSub));
                            }
                            Log.d(TAG, "Close IM connection: iccid:"+
                                  ConnectionGlobalData.activeWorkingSub);
                            ConnectionGlobalData.connMgr.closeConnection(
                                ConnectionGlobalData.serviceHandleArray[ConnectionGlobalData.activeWorkingSub],
                                ConnectionGlobalData.connImMap.get(ConnectionGlobalData.activeWorkingSub));
                        }else{
                            Log.d(TAG, "Close IM connection");
                        }
                    }
                }catch (Exception e){
                    Log.d(TAG, "Exception raised" + e);
                }}});
    }
}

class RequestNetworkTask extends AsyncTask<String, Void, String> {
    private NetworkRequest mIMSRequest;
    private ConnectivityManager cm;
    private Context mContext;
    private String DEBUG_TAG = "CM_IMS_PDN";
    boolean isProcessingDone;
    boolean isAvailable = false;
    private String startDataAction = "android.cne.action.START_DATA";

    public RequestNetworkTask(Context context) {
        mContext = context;
    }

    @Override
    protected String doInBackground(String... params) {
        Log.d(DEBUG_TAG, "Processing is started");
        isProcessingDone = false;
        createNetwork(mContext);
/*      IntentFilter filter = new IntentFilter();
        filter.addAction(startDataAction);
        BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context c, Intent intent) {
                if(intent.getAction() == startDataAction) {
                    Log.d(DEBUG_TAG, "Received start data intent");
                    doData();
                }
            }
        };
        mContext.registerReceiver(mIntentReceiver, filter);
*/        while(isProcessingDone != true) {
        }
        Log.d(DEBUG_TAG, "Processing is done");
        return null;
    }

    public void createNetwork(Context context) {
        Log.d(DEBUG_TAG, "Trying to request network");
        mIMSRequest = new NetworkRequest.Builder()
                        .addCapability(NetworkCapabilities.NET_CAPABILITY_IMS)
                        .addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR)
                        .build();
        cm = (ConnectivityManager)mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        cm.requestNetwork(mIMSRequest, mIMSNetworkCallback);
    }

    private NetworkCallback mIMSNetworkCallback = new NetworkCallback(){
        //Log.d(DEBUG_TAG, "Trying to create network callback");
        @Override
        public void onAvailable(Network network) {
            Log.d(DEBUG_TAG, "Network onAvailable start");
            Log.d(DEBUG_TAG, "Network: " + network.toString());
            boolean flag = ConnectivityManager.setProcessDefaultNetwork(network);
            if(flag) {
                Log.d(DEBUG_TAG, "onAvailable: bind the process to this network");
            }
            else
            {
                Log.d(DEBUG_TAG, "onAvailable: bind the process to this network failed");
            }
            //doData();
            if(!isAvailable) {
                isAvailable = true;

                ConnectionGlobalData.IMSPDNConnected = true;
                try {
                    //doTcpData();
                    //doUdpData();
                } catch(Exception e) {
                    e.printStackTrace();
                }
            }
        }

        @Override
        public void onLost(Network network) {
            Log.d(DEBUG_TAG, "Network Lost: " + network.toString());
            ConnectionGlobalData.IMSPDNConnected = false;
        }
    };
}
