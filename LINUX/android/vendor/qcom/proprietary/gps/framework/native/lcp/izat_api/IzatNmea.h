/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IzatNmea

  Copyright  (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc..
=============================================================================*/

#ifndef __IZATNMEA_H__
#define __IZATNMEA_H__

#include <comdef.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <string.h>

namespace izat_manager {

using namespace std;

/**
 * @brief IzatNmea class
 * @details IzatNmea class containing nmea related information
 *
 */
class IzatNmea {
public:

    /**
     * @brief Default contructor
     * @details Default constructor
     */
    inline IzatNmea () {
        reset ();
    }

    /**
     * @brief Constructor
     * @details Constructor
     *
     * @param rhs Reference to IzatNmea
     */
    inline IzatNmea (const IzatNmea & rhs)
          :mHasNmeaString(rhs.mHasNmeaString),
          mNmeaStr(rhs.mNmeaStr) { }
    /**
     * @brief Constructor from UlpNmea
     * @details Constructor
     *
     * @param ulpNmea  Reference to UlpNmea
     */
    inline IzatNmea (const UlpNmea *ulpNmea)
          :mHasNmeaString(FALSE){
        if(ulpNmea) {
            mHasNmeaString = TRUE;
            mNmeaStr.assign((const char *)&ulpNmea->nmea_str[0]);
        }
    }

    /**
     * @brief assignment operator
     * @details assignment operator
     *
     * @param rhs Reference to IzatNmea
     * @returs reference to IzatNmea
     */
    inline IzatNmea & operator= (const IzatNmea & rhs) {
        makeCopy (rhs);
        return *this;
    }

    /**
     * @brief Destructor
     * @details Destructor
     */
    inline virtual ~IzatNmea () {
        reset ();
    };

    /**
     * @brief Reset
     * @details Reset
     */
    inline void reset () {

        mHasNmeaString = false;
    }

    /**
     * @brief Check if nmea is valid
     * @details Check if nmea is valid
     * @return true if valid or false if not valid
     */
    inline bool isValid () const {
        return  (mHasNmeaString);
    }

    /**
     * @brief get nmea string
     * @details returns the nmea string information
     * @return nmea string
     */
    inline std::string getNmea () const {
        return  (mNmeaStr);
    }

private:

    /**
     * Boolean flag to indicate the presence NMEA string
     */
    bool mHasNmeaString;

    /**
     * NMEA String
     */
    std::string mNmeaStr;

    /**
     * @brief Copy method
     * @details Copy method
     *
     * @param rhs Reference to IzatNmea indcating where to copy from.
     */
    inline void makeCopy  (const IzatNmea & rhs) {
        mHasNmeaString = rhs.mHasNmeaString;
        mNmeaStr.assign(rhs.mNmeaStr);
    }
};

}// namespace izat_manager

#endif // #ifndef __IZATNMEA_H___
