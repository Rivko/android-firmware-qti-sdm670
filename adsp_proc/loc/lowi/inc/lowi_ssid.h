#ifndef __LOWI_SSID_H__
#define __LOWI_SSID_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI SSID Interface Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for
  LOWI SSID

Copyright (c) 2012 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#if APSS
#include <inc/lowi_defines.h>
#include <inc/lowi_const.h>
#else
#include "lowi_const.h"
#include "lowi_service_v01.h"
#endif


namespace qc_loc_fw
{

/**
 * Class for SSID
 */
class LOWISsid
{
public:

  /**
   * Constructor
   */
  LOWISsid ();

  /**
   * Checks if the SSID is valid
   * @return true for valid, false otherwise
   */
  bool isSSIDValid() const;

  /**
   * Sets SSID
   * @param unsigned char* SSID to be set
   * @param int   Length of the SSID
   * @return 0 for success, non zero for error
   */
  int setSSID(const unsigned char * const ssid, const int length);

#if !APSS
  /**
   * Sets SSID
   * @param sQmiLowiSsid_v01* SSID
   * @return 0 for success, non zero for error
   */
  int setSSID (sQmiLowiSsid_v01* ssid);
#endif

  /**
   * Gets SSID
   * @param [out] unsigned char* SSID to be retrieved
   * @param [out] int*   Length of the SSID
   * @return 0 for success, non zero for error
   */
  int getSSID(unsigned char * const pSsid, int * const pLength) const;

  /**
   * Compares the ssid with the current
   * @param LOWISsid& LOWISsid to compare with
   * @return 0 for equal, non zero otherwise
   */
  int compareTo (const LOWISsid & ssid);

#if !APSS
  /**
   *  Returns the supported version number which depends on the request
   * version number
   */
  eSupportedServiceVersion getSupportedVersion ();
#endif

private:
  static const char* const TAG;
#if APSS
  bool m_isSsidValid;
  int m_ssid_length;
  unsigned char m_ssid[32];
#else
  /** SSID */
  sQmiLowiSsid_v01 ssid_v01;

  /** Supported service version
   *  This version depends on the QMI service major version
   */
  eSupportedServiceVersion  mSupportedVersion;
#endif
};

} // namespace qc_loc_fw

#endif //#ifndef __LOWI_SSID_H__
