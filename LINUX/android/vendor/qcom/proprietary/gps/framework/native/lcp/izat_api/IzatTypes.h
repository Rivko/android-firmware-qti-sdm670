/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Types

  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATTYPES_H__
#define __IZAT_MANAGER_IZATTYPES_H__

namespace izat_manager {

/**
 * Bit Mask for location listeners.
 */
typedef unsigned short int IzatListenerMask;

/**
 * Enum of supported streams.
 * Each elements of enum represent a bit mask value.
 * Do not add values with multiple bits set.
 *
 * Note of caution:
 * If new value is being added to this enum (after 0x4),
 * update the logic in onSetRequestJNI() method to derive
 * providerNameLookUpTable array access index.
 */
typedef enum IzatStreamType {

    /**
     * Fused location stream.
     */
    IZAT_STREAM_FUSED = 0x1,

    /**
     * Network location stream.
     */
    IZAT_STREAM_NETWORK = 0x2,

    /**
     * GNSS location stream.
     */
    IZAT_STREAM_GNSS  = 0x4,

    /**
     * GNSS NMEA stream.
     */
    IZAT_STREAM_NMEA  = 0x8,

    /**
     * GNSS Raw location stream.
     */
    IZAT_STREAM_DR  = 0x10,

    /**
     * GNSS SV Info stream.
     */
    IZAT_STREAM_GNSS_SVINFO  = 0x20,

    /**
     * GNSS Raw SV Info stream.
     */
    IZAT_STREAM_DR_SVINFO  = 0x40,


    IZAT_STREAM_ALL = (IZAT_STREAM_DR_SVINFO << 1) - 1

} IzatStreamType;

typedef enum IzatLocationStatus {
    /**
     * Final location
     */
    IZAT_LOCATION_STATUS_FINAL = 0x0,

    /**
      * Intermediate location
      */
    IZAT_LOCATION_STATUS_INTERMEDIATE = 0x1
} IzatLocationStatus;

/**
 * Izat Provider status Enumeration
 */
typedef enum IzatProviderStatus {

    /**
     * Out of service.
     */
    IZAT_PROVIDER_OUT_OF_SERVICE = 0,

    /**
     * Temporarily unavailable.
     */
    IZAT_PROVIDER_TEMPORARILY_UNAVAILABLE,

    /**
     * Available.
     */
    IZAT_PROVIDER_AVAILABLE,

    /**
     * GNSS status none
     */
    IZAT_PROVIDER_GNSS_STATUS_NONE,

    /**
     * GNSS Session BEGIN
     */
    IZAT_PROVIDER_GNSS_STATUS_SESSION_BEGIN,

    /**
     * GNSS Session END
     */
    IZAT_PROVIDER_GNSS_STATUS_SESSION_END,

    /**
     * GNSS Engine ON
     */
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_ON,

    /**
     * GNSS Engine OFF
     */
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_OFF,



} IzatProviderStatus;

/**
 * Enumeration of Horizontal accuracy to be used for specifying accuracy of Latitude and Longitudes
 */
typedef enum IzatHorizontalAccuracy {

    /**
     * High power consumption.
     */
    IZAT_HORIZONTAL_FINE = 0,

    /**
     * Medium power consumption.
     */
    IZAT_HORIZONTAL_BLOCK,

    /**
     * Low power consumption.
     */
    IZAT_HORIZONTAL_CITY,

    /**
     * Zero power consumption.
     */
    IZAT_HORIZONTAL_NONE

} IzatHorizontalAccuracy;

/**
 * Enumeration of accuracy to be used for specifying accuracy of Bearing and or Altitude.
 */
typedef enum IzatOtherAccuracy {

    /**
     * High accuracy.
     */
    IZAT_OTHER_HIGH = 0,

    /**
     * Low accuracy.
     */
    IZAT_OTHER_LOW,

    /**
     * No Accuracy requirement.
     */
    IZAT_OTHER_NONE

} IzatOtherAccuracy;

typedef enum IzatNetworkPositionSourceType {
    /**
     * Network position is from Cell-Tower
     */
    IZAT_NETWORK_POSITION_FROM_CELL = 0,

    /**
     *  Network position is from WIFI Access Points
     */
    IZAT_NETWORK_POSITION_FROM_WIFI = 1
} IzatNetworkPositionSourceType;

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATTYPES_H__
