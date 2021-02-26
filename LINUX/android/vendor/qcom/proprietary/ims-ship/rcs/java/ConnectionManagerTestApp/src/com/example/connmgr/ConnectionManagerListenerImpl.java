/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.example.connmgr;

import java.net.InetAddress;
import java.net.Socket;
import javax.net.SocketFactory;

import android.util.Log;
import android.net.ConnectivityManager;
import android.content.Context;
import android.net.NetworkInfo;

import com.qualcomm.qti.imscmservice.V1_0.IImsCmServiceListener;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_CONFIG_DATA;
import com.example.connmgr.ConnectionGlobalData;


public class ConnectionManagerListenerImpl extends IImsCmServiceListener.Stub  {
    final static String TAG = "UI_ConnMgr_ConnectionManagerListenerImpl";

    /*onStatusChange---- This callback is invoked when there
      is a change in the status of the IMS Connection Manager.
    */
    public void onStatusChange(int status)  {
        Log.d(TAG, "onConnectionManagerStatusChange status is "+status);
        if ( status == 2 )
        {
            ConnectionGlobalData.connMgrInitialised = 1 ;
            Log.d(TAG, "onConnectionManagerStatusChange ConnectionManager is initialised");
            Log.d(TAG, "testing - sending packets across device");
            Log.d(TAG, "handleTestSendingPacketsViaSocket entered ");
            String localHost = "10.242.114.49";
            int localport = 54321;
            String packet = "Data Packet to be sent to remote.";
            Log.d(TAG, "localHost Ip address " + localHost);
            String remoteIP = "10.242.166.1";
            int remoteport = 60450;  // Got from the SIP Message.
            ConnectivityManager mConnectivityMgr;
            try{

              if( ConnectionGlobalData.mContext == null)
              {
                Log.d(TAG, "ConnectionGlobalData.mContext is null");
              }
              mConnectivityMgr = (ConnectivityManager) ConnectionGlobalData.mContext
                                                   .getSystemService(Context.CONNECTIVITY_SERVICE);

              if( mConnectivityMgr != null){
                NetworkInfo nInfo = mConnectivityMgr.getActiveNetworkInfo();
                if(nInfo != null)
                {
                  Log.d(TAG, "getActiveNetworkInfo " + nInfo.toString());
                }
                else
                {
                  Log.d(TAG, "getActiveNetworkInfo nInfo is null, no active network found");
                }
                if(ConnectionGlobalData.IMSPDNConnected == true)
                {
                  Log.d(TAG, "Creating and sending message to remoteIP " +
                  InetAddress.getByName(remoteIP) +
                  " from host "+
                  InetAddress.getByName(localHost));
                  Socket soc = SocketFactory.getDefault().createSocket(
                    InetAddress.getByName(remoteIP),
                    remoteport,
                    InetAddress.getByName(localHost),
                    localport);
                    soc.getOutputStream().write(packet.getBytes());
                }
                else
                {
                  Log.d(TAG, "IMS PDN is not connected");
                }
              }
              else
              {
                Log.d(TAG, "Connectivity Manager is NULL");
              }
            }
            catch(Exception ex)
            {
              ex.printStackTrace();
            }
        }
    }

    /*onConfigurationChange---- Callback invoked when the configurations are updated*/
    public void onConfigurationChange(IMSCM_CONFIG_DATA configData, int userData) {

      if(configData != null)
      {
        for (int i=0; i<=1; i++)
        {
          if(ConnectionGlobalData.userDataArray[i] == userData)
          {
            Log.d(TAG, "onConfigurationChange: on Sub :"+ i);
            Log.d(TAG, "onConfigurationChange: userData :"+ userData);
          }
        }
        ConnectionGlobalData.configData = configData;
        if (configData.userConfig != null)
        {
          ConnectionGlobalData.userConfigData = configData.userConfig;
          Log.d(TAG, "userConfig" +
                ConnectionGlobalData.userConfigData.iUEClientPort + "\n" +
                ConnectionGlobalData.userConfigData.iUEServerPort +"\n"+
                ConnectionGlobalData.userConfigData.pAssociatedURI + "\n"+
                ConnectionGlobalData.userConfigData.pUEPublicIPAddress + "\n"+
                ConnectionGlobalData.userConfigData.iUEPublicPort + "\n"+
                ConnectionGlobalData.userConfigData.pSipPublicUserId + "\n"+
                ConnectionGlobalData.userConfigData.pSipPrivateUserId + "\n"+
                ConnectionGlobalData.userConfigData.pSipHomeDomain + "\n"+
                ConnectionGlobalData.userConfigData.pUEPubGruu +"\n"+
                ConnectionGlobalData.userConfigData.pLocalHostIPAddress + "\n"+
                ConnectionGlobalData.userConfigData.eIpType + "\n"+
                ConnectionGlobalData.userConfigData.pIMEIStr + "\n"+
                ConnectionGlobalData.userConfigData.iUEOldSAClientPort);
          Log.d(TAG, "userConfig via toString()" +
                ConnectionGlobalData.userConfigData.toString());
        }
        if (configData.deviceConfig != null)
        {
          ConnectionGlobalData.deviceConfigData = configData.deviceConfig;
          Log.d(TAG, "deviceConfig" +
                ConnectionGlobalData.deviceConfigData.bUEBehindNAT + "\n" +
                ConnectionGlobalData.deviceConfigData.bIpSecEnabled + "\n" +
                ConnectionGlobalData.deviceConfigData.bCompactFormEnabled + "\n" +
                ConnectionGlobalData.deviceConfigData.bKeepAliveEnableStatus + "\n" +
                ConnectionGlobalData.deviceConfigData.bGruuEnabled + "\n" +
                ConnectionGlobalData.deviceConfigData.pStrSipOutBoundProxyName + "\n" +
                ConnectionGlobalData.deviceConfigData.iSipOutBoundProxyPort + "\n" +
                ConnectionGlobalData.deviceConfigData.iPCSCFClientPort + "\n" +
                ConnectionGlobalData.deviceConfigData.iPCSCFServerPort + "\n" +
                ConnectionGlobalData.deviceConfigData.pArrAuthChallenge + "\n" +
                ConnectionGlobalData.deviceConfigData.pArrNC + "\n" +
                ConnectionGlobalData.deviceConfigData.pServiceRoute + "\n" +
                ConnectionGlobalData.deviceConfigData.pSecurityVerify + "\n" +
                ConnectionGlobalData.deviceConfigData.iPCSCFOldSAClientPort + "\n" +
                ConnectionGlobalData.deviceConfigData.iTCPThresholdValue);
          Log.d(TAG, "deviceConfig via toString()" +
                ConnectionGlobalData.deviceConfigData.toString());
        }
        if (configData.autoConfig != null)
        {
          ConnectionGlobalData.autoConfigData = configData.autoConfig;
          Log.d(TAG, "autoConfig" +
                ConnectionGlobalData.autoConfigData.configType + "\n" +
                ConnectionGlobalData.autoConfigData.autoConfigXml);
          Log.d(TAG, "autoConfig via toString()" +
                ConnectionGlobalData.autoConfigData.toString());
        }
        else if(configData.userConfig == null)
        {
          Log.d(TAG, "userConfig is NULL");
        }
        else if(configData.deviceConfig == null)
        {
          Log.d(TAG, "deviceConfig is NULL");
        }
        else if(configData.autoConfig == null)
        {
          Log.d(TAG, "autoConfig is NULL");
        }
      }
      else
      {
        Log.d(TAG, "configData is null");
      }

    Log.d(TAG, "onConfigurationChange config data via toString()" +
          ConnectionGlobalData.configData.toString());
    }

    /*onCommandStatus---- Callback invoked to notify the clients the status of request placed.*/
    public void onCommandStatus(int userData, int status) {
      for (int i=0; i<=1; i++)
      {
        if(ConnectionGlobalData.userDataArray[i] == userData)
        {
          Log.d(TAG, "onCommandStatus: on Sub :"+ i);
          Log.d(TAG, "onCommandStatus: userData :"+ userData);
          Log.d(TAG, "onCommandStatus: status :"+ status);
        }
      }
    }

    /*onServiceReady---- Callback invoked upon successful service creation.*/
    public void onServiceReady(long serviceHandle, int userData, int status) {
      //iterate through array userDataArray and based on index : map iccid to servicehandle
      for (int i=0; i<=1; i++)
      {
        if(ConnectionGlobalData.userDataArray[i] == userData)
        {
          ConnectionGlobalData.connMgrInitialised = 1 ;
          ConnectionGlobalData.serviceHandleArray[i] = serviceHandle;
          Log.d(TAG+i, "onServiceReady: serviceHandle :"+ ConnectionGlobalData.serviceHandleArray[i]);
          Log.d(TAG+i, "onServiceReady: userData :"+ userData);
          Log.d(TAG+i, "onServiceReady: status :"+ status);

        }
      }
    }
}
