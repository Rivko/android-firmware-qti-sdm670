/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IzatSvInfo


  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZATSVINFO_H__
#define __IZATSVINFO_H__

#include <comdef.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include "IzatTypes.h"
#include <gps_extended_c.h>
#include <loc_gps.h>

namespace izat_manager {

using namespace std;

/**
 * @brief IzatSvInfo class
 * @details IzatSvInfo class containing svinfo related information
 *
 */
class IzatSvInfo {
public:

    /**
     * @brief Default contructor
     * @details Default constructor
     */
    inline IzatSvInfo () {}

    /**
     * @brief Constructor
     * @details Constructor
     *
     * @param rhs Reference to IzatSvInfo
     */
    inline IzatSvInfo (const IzatSvInfo & rhs) {
        makeCopy (rhs);
    }

    /**
     * @brief assignment operator
     * @details assignment operator
     *
     * @param rhs Reference to IzatSvInfo
     * @returs reference to IzatSvInfo
     */
    inline IzatSvInfo & operator= (const IzatSvInfo & rhs) {
        makeCopy (rhs);
        return *this;
    }

    /**
     * @brief Destructor
     * @details Destructor
     */
    inline virtual ~IzatSvInfo () {}

    /**
     * GnssSvNotification
     */
    GnssSvNotification mSvNotify;

private:
    /**
     * @brief Copy method
     * @details Copy method
     *
     * @param rhs Reference to IzatSvInfo indcating where to copy from.
     */
    void makeCopy  (const IzatSvInfo & rhs) {
        mSvNotify = rhs.mSvNotify;
    }
};

}// namespace izat_manager

#endif // #ifndef __IZATSVINFO_H__
