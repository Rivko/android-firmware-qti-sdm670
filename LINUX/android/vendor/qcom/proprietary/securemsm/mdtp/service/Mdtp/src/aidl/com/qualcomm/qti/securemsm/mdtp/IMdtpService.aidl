/*=============================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.qti.securemsm.mdtp;


interface IMdtpService {
    int initialize();
    int shutdown();
    int getState(out int[] systemState, out int[] appState);
    int getVersion(out int[] libVersion, out int[] tzVersion);
    int processSignedMsg(in byte[] message);
    int deactivateLocal(in byte[] pin);
    int getIdToken(out byte[] idToken);
    int updateCrl(in byte[] crl, in byte[] certificate);
    int getHwRecoveryPin(out byte[] pin);
}
