/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.telephonyservice.tests;

import java.util.Arrays;

import android.os.IBinder;
import android.test.ServiceTestCase;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;

import com.qualcomm.qti.telephonyservice.IQtiTelephonyService;
import com.qualcomm.qti.telephonyservice.QtiTelephonyService;
import com.qualcomm.qti.telephonyservice.KsNafResponse;

import android.test.suitebuilder.annotation.Suppress;

/*
 *  To run the test -
 *  1. Remove @Suppress annotation
 *  2. Compile the tests to generate the test apk
 */
public class QtiTelephonyServiceTest extends ServiceTestCase<QtiTelephonyService> {

    private IQtiTelephonyService mQtiTelephonyService;

    public QtiTelephonyServiceTest() {
        super(QtiTelephonyService.class);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();

        Intent startQtiTelephonyServiceIntent = new Intent();
        startQtiTelephonyServiceIntent.setClass(getContext(), QtiTelephonyService.class);

        IBinder service = bindService(startQtiTelephonyServiceIntent);
        assertNotNull(service);

        mQtiTelephonyService = IQtiTelephonyService.Stub.asInterface(service);
        assertNotNull(mQtiTelephonyService);
    }

    public void testQtiTelephonyServiceVersion() throws RemoteException {
        String version = mQtiTelephonyService.getVersion();
        assertNotNull(version);
        assertFalse(version.isEmpty());
    }

    public void testGbaInitIsimApplicationId() throws RemoteException {
        int slotId = 0;
        int applicationType = 5;
        byte[] securityProtocol = new byte[]{0x01, 0x02, 0x03, 0x04, 0x05};
        String nafFullyQualifiedDomainName = "www.xyz.com";
        boolean forceBootStrapping = true;

        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(securityProtocol,
                                                                    nafFullyQualifiedDomainName,
                                                                    slotId,
                                                                    applicationType,
                                                                    forceBootStrapping);
        assertNotNull(ksNafresponse);
    }

    public void testGbaInitUsimApplicationId() throws RemoteException {
        int slotId = 0;
        int applicationType = 2;
        byte[] securityProtocol = new byte[]{0x01, 0x02, 0x03, 0x04, 0x05};
        String nafFullyQualifiedDomainName = "www.xyz.com";
        boolean forceBootStrapping = true;

        KsNafResponse ksNafresponse = mQtiTelephonyService.gbaInit(securityProtocol,
                                                                    nafFullyQualifiedDomainName,
                                                                    slotId,
                                                                    applicationType,
                                                                    forceBootStrapping);
        assertNotNull(ksNafresponse);
    }

    public void testSecureImpiIsimApplicationId() throws RemoteException {
        int slotId = 0;
        int applicationType = 5;
        byte[] secureImpi = mQtiTelephonyService.getImpi(slotId, applicationType, true);
        assertFalse(secureImpi.length == 0);
    }

    public void testImpiIsimApplicationId() throws RemoteException {
        int slotId = 0;
        int applicationType = 5;
        byte[] secureImpi = mQtiTelephonyService.getImpi(slotId, applicationType, false);
        assertFalse(secureImpi.length == 0);
    }
}
