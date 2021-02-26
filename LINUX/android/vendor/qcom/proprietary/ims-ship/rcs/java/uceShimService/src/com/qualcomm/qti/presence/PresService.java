/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.presence;

import com.android.ims.internal.uce.common.UceLong;
import com.android.ims.internal.uce.common.StatusCode;
import com.android.ims.internal.uce.presence.*;

import com.qualcomm.qti.uceservice.V1_0.UceStatus;
import com.qualcomm.qti.uceservice.V1_0.UceStatusCode;
import com.qualcomm.qti.uceservice.V1_0.PresenceCapabilityInfo;
import com.qualcomm.qti.uceservice.V1_0.PresServiceInfo;

import com.qualcomm.qti.translator.*;

import android.os.RemoteException;
import android.os.Parcel;
import android.util.Log;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import java.util.Map;
import java.util.HashMap;

import com.qualcomm.qti.uceShimService.RCSService;


public class PresService extends IPresenceService.Stub {

    private final static String logTAG = "QRCSService PresService";
    private AidlToHidlTranslator translateToHidlObj = null;
    private HidlToAidlTranslator translateToAidlObj = null;
    private static Map<Integer, com.qualcomm.qti.uceservice.V1_0.IPresenceService> presHandleMap =
                          new HashMap<Integer, com.qualcomm.qti.uceservice.V1_0.IPresenceService>();
    // maintatin list of HIDL Listeners
    private static List<PresListener> hidlListenerlist = new ArrayList<PresListener>();

    public void setServiceHandle (int serviceHandle) {
        com.qualcomm.qti.uceservice.V1_0.IPresenceService localHidlPresenceService = null;
        try {
            localHidlPresenceService = RCSService.mHidlService.getPresenceService(serviceHandle);
        }catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
        }
        if(localHidlPresenceService != null) {
            presHandleMap.put(serviceHandle, localHidlPresenceService);
        }
    }
    public StatusCode getVersion(int pPresServiceHdl)
            throws RemoteException {

        StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();
        try {

        UceStatus pStatus = presHandleMap.get(pPresServiceHdl).getVersion(pPresServiceHdl);
        retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
        return retStatus;
    }

    public void addHidlListener(PresListener hidlListener) {
        hidlListenerlist.add(hidlListener);
    }

    public StatusCode addListener(int pPresServiceHdl,
            IPresenceListener pPresServiceListener,
            UceLong pPresServiceListenerHdl) throws RemoteException {

        StatusCode retStatus = new StatusCode();
        retStatus.setStatusCode(StatusCode.UCE_FAILURE);
        translateToAidlObj = new HidlToAidlTranslator();

        long clientHandle = pPresServiceListenerHdl.getUceLong();
        PresListener hidlPresListener = new PresListener();
        addHidlListener(hidlPresListener);
        hidlPresListener.setAidlPresListener(pPresServiceListener);
        try {

        UceStatus pStatus = presHandleMap.get(pPresServiceHdl)
		                       .addListener((long)pPresServiceHdl,
							                 hidlPresListener, clientHandle);
        retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
        return retStatus;
    }

    public StatusCode removeListener(int pPresServiceHdl,
            UceLong pPresServiceListenerHdl) throws RemoteException {

        StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();

        long presServiceListenerHdl = pPresServiceListenerHdl.getUceLong();
        try {

        UceStatus pStatus = presHandleMap.get(pPresServiceHdl)
		                      .removeListener(pPresServiceHdl, presServiceListenerHdl);
        retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
        return retStatus;
    }
//no o/p type mentioned for reEnableService
    public StatusCode reenableService(int pPresServiceHdl,
	                                  int pUserData) throws RemoteException {

        StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();
        UceStatus pStatus = null;
	try {
        pStatus = presHandleMap.get(pPresServiceHdl).reEnableService(pPresServiceHdl, pUserData);
        //hidlPresenceService.reEnableService(pPresServiceHdl, pUserData);
        retStatus.setStatusCode(pStatus.status); // to replace with
        //translateToAidlObj.getAidlStatusCode (pStatus.status)
        return retStatus;
        }
        catch( RemoteException | RuntimeException e ) {
         Log.w(logTAG, "Unexpected remote exception", e);
         throw e;
        }
    }

    public StatusCode publishMyCap(int pPresServiceHdl,
            PresCapInfo  pMyCapInfo, int pUserData) throws RemoteException {
        StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();
	translateToHidlObj = new AidlToHidlTranslator();

        PresenceCapabilityInfo pHidlCapInfo = new PresenceCapabilityInfo();
        pHidlCapInfo = translateToHidlObj.getHidlPresCapInfo(pMyCapInfo);

        try {
        UceStatus pStatus = presHandleMap.get(pPresServiceHdl)
		                    .publishCapability(pPresServiceHdl,
							                   pHidlCapInfo, pUserData);
	retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
	return retStatus;
    }

    public StatusCode getContactCap(int pPresServiceHdl,
            String pRemoteURI, int pUserData) throws RemoteException {

        StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();
        try {

        UceStatus pStatus = presHandleMap.get(pPresServiceHdl)
		                         .getContactCapability(pPresServiceHdl,
								                       pRemoteURI, pUserData);
	retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
	return retStatus;
    }

    public StatusCode getContactListCap(int pPresServiceHdl,
            String[] pRemoteURIList, int pUserData)
            throws RemoteException {

	StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();

	ArrayList<String> stringRemoteURIList = new ArrayList<String>(Arrays.asList(pRemoteURIList));
        try {

        UceStatus pStatus = presHandleMap.get(pPresServiceHdl)
		                        .getContactListCapability((long)pPresServiceHdl,
								                           stringRemoteURIList,
               (long) pUserData);
	retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
        return retStatus;
    }

    public StatusCode setNewFeatureTag(int pPresServiceHdl, String pFeatureTag,
            com.android.ims.internal.uce.presence.PresServiceInfo pServiceInfo, int pUserData)
            throws RemoteException {

	StatusCode retStatus = new StatusCode();
	retStatus.setStatusCode(StatusCode.UCE_FAILURE);
	translateToAidlObj = new HidlToAidlTranslator();
	translateToHidlObj = new AidlToHidlTranslator();

	PresServiceInfo hidlPresSvcInfo = new PresServiceInfo();
	hidlPresSvcInfo = translateToHidlObj.getHidlPresSvcInfo(pServiceInfo);
        try {

        UceStatus pStatus = presHandleMap.get(pPresServiceHdl)
		                       .setNewFeatureTag((long)pPresServiceHdl, pFeatureTag,
                hidlPresSvcInfo, (long)pUserData);
	retStatus.setStatusCode(translateToAidlObj.getAidlStatusCode(pStatus.status));
        } catch(RemoteException | RuntimeException e) {
           Log.w(logTAG, "Unexpected remote exception", e);
           throw e;
        }
	return retStatus;
    }
}
