/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.example.connmgr;

import android.content.Context;

import com.qualcomm.qti.imscmservice.V1_0.IImsCmServiceListener;
import com.qualcomm.qti.imscmservice.V1_0.IImsCmService;
import com.qualcomm.qti.imscmservice.V1_0.IImsCMConnection;
import com.qualcomm.qti.imscmservice.V1_0.IImsCMConnectionListener;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_CONFIG_DATA;
import com.qualcomm.qti.imscmservice.V1_0.QIMSCM_USER_CONFIG;
import com.qualcomm.qti.imscmservice.V1_0.QIMS_CM_DEVICE_CONFIG;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_AUTOCONFIG_DATA;
import com.qualcomm.qti.imscmservice.V1_0.IMSCM_ConfigType;
import java.util.HashMap;


public class ConnectionGlobalData  {
    final static String TAG = "UI_ConnMgr_ConnectionGlobalData";
    public static final String SM_FEATURE_TAG =
        "+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.oma.cpm.largemsg\"";

    public static final String IM_FEATURE_TAG =
        "+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.oma.cpm.session\"";

    public static final String FT_FEATURE_TAG =
        "+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.oma.cpm.filetransfer\"";

    public static final String PRESENCE_FEATURE_TAG =
        "+g.3gpp.iari-ref=\"urn%3Aurn-7%3A3gpp-application.ims.iari.rcse.dp\"";

    public static final String  INVALID_FEATURE_TAG3 =
        "+g.3gpp.icsi-ref=\"urn%3Aurn-7%3A3gpp-service.ims.icsi.oma.cpm.anyinvalidtag\"";

    public static IImsCmService connMgr = null;
    public static boolean IMSPDNConnected = false;
    public static int connMgrInitialised = 0 ;
    public static int connSmInitialisedArray[] = {0, 0};
    public static int connImInitialisedArray[] = {0, 0};
    public static int connInvalidInitialisedArray[] = {0, 0};
    public static Context mContext = null;
    public static int userDataArray[] = {6789, 9560}; //random value for sub0 and sub1
    public static long serviceHandleArray[] = {0, 0};
    public static int activeWorkingSub = 0;
    public static IMSCM_CONFIG_DATA configData = null;
    public static QIMSCM_USER_CONFIG userConfigData = null;
    public static QIMS_CM_DEVICE_CONFIG deviceConfigData = null;
    public static IMSCM_AUTOCONFIG_DATA autoConfigData = null;
    public static int configType = IMSCM_ConfigType.IMSCM_USER_CONFIG;
    public static int autoConfigType = IMSCM_ConfigType.IMSCM_AUTO_CONFIG;
    public static short responseCode = 404;

    public static HashMap<Integer, IImsCMConnection> connSmMap =
        new HashMap<Integer, IImsCMConnection>();
    public static HashMap<Integer, IImsCMConnection> connImMap =
        new HashMap<Integer, IImsCMConnection>();
    public static HashMap<Integer, IImsCMConnection> connInvalidMap =
        new HashMap<Integer, IImsCMConnection>();

    public static HashMap<Integer, IImsCmServiceListener> connMgrListenerMap =
        new HashMap<Integer, IImsCmServiceListener>();

    public static HashMap<Integer, IImsCMConnectionListener> connSmListenerMap =
        new HashMap<Integer, IImsCMConnectionListener>();
    public static HashMap<Integer, IImsCMConnectionListener> connImListenerMap =
        new HashMap<Integer, IImsCMConnectionListener>();
    public static HashMap<Integer, IImsCMConnectionListener> connInvalidListenerMap =
        new HashMap<Integer, IImsCMConnectionListener>();

}
