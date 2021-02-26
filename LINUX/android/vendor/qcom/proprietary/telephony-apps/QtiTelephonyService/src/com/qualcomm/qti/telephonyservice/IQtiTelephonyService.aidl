/******************************************************************************
 * @file    IQtiTelephonyService.aidl
 * @brief   This interface describes APIs for the service which supports Telephony requirement
 *          to make apis accessible to downloadable clients.
 *          To begin with user authentication apis are added which allows clients to generate keys
 *          for user authentication by Generic Bootstrapping Architecture(GBA)
 *          based on AKA protocol.
 *          Clients should bind to the service name
 *          "com.qualcomm.qti.telephonyservice.IQtiTelephonyService"
 *          Permission "com.qualcomm.qti.permission.USE_QTI_TELEPHONY_SERVICE" is required to bind
 *          to the QtiTelephonyService.
 *          Permission "com.qualcomm.qti.permission.ACCESS_USER_AUTHENTICATION_APIS" is required to
 *          access the user authentication apis.
 *
 * @version 1.0
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.telephonyservice;

import com.qualcomm.qti.telephonyservice.KsNafResponse;

interface IQtiTelephonyService {

    /**
     * getVersion will be used by client to get the version(non-negative integer value)
     * of the interface implemented by QtiTelephony service.
     * Version is incremented for every IQtiTelephonyService.aidl change.
     * Backward compatibility will be maintained.
     *
     * @return
     *     Version of the interface implemented by QtiTelephony service.
     */
    String getVersion();

    /**
     * gbaInit will be used by client to perform bootstrapping procedures(GBA)
     * to generate keys.
     *
     * @param securityProtocol
     *    Security protocol identifier NULL terminated string of 5 octets.
     *    See 3GPP TS 33.220 Annex H.
     * @param nafFullyQualifiedDomainName
     *    NAF fully qualified domain name with maximum size of 255 bytes.
     * @param slotId
     *    Slot identifier in which SIM is inserted.
     *    Value 0(Slot 1) 1(Slot 2) 3(Slot 3)
     * @param applicationType
     *    Application Type of the card application.
     *    Value 0(UNKNOWN) 1(SIM) 2(USIM) 3(RUIM) 4(CSIM) 5(ISIM)
     * @param forceBootStrapping
     *    true=force bootstrapping, false=do not force bootstrapping.
     *
     * @return
     *     KsNafResponse
     *     Response object defined in KsNafResponse.aidl.
     */
    KsNafResponse gbaInit(in byte[] securityProtocol, in String nafFullyQualifiedDomainName,
                            in int slotId, in int applicationType, in boolean forceBootStrapping);

    /**
     * getImpi will be used by client to read IMPI.
     *
     * @param slotId
     *    Slot identifier in which SIM is inserted.
     *    Value 0(Slot 1) 1(Slot 2) 3(Slot 3)
     * @param applicationType
     *    Application Type of the card application.
     *    Value 0(UNKNOWN) 1(SIM) 2(USIM) 3(RUIM) 4(CSIM) 5(ISIM)
     * @param secure
     *    true=return encrypted IMPI value, false=return plain text IMPI value
     *
     * @return
     *     byte[]
     *     impi
     */
     byte[] getImpi(in int slotId, in int applicationType, in boolean secure);
}
