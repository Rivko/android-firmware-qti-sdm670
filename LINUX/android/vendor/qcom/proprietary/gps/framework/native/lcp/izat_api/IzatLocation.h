/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IzatLocation


  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATLOCATION_H__
#define __IZAT_MANAGER_IZATLOCATION_H__

#include <comdef.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include "IzatTypes.h"

namespace izat_manager {

using namespace std;

/**
 * @brief IzatLocation class
 * @details IzatLocation class containing location related information
 *
 */
class IzatLocation {
public:

    /**
     * @brief Default contructor
     * @details Default constructor
     */
    IzatLocation () {
        reset ();
    }

    /**
     * @brief Constructor
     * @details Constructor
     *
     * @param rhs Reference to IzatLocation
     */
    IzatLocation (const IzatLocation & rhs) {
        makeCopy (rhs);
    }

    /**
     * @brief assignment operator
     * @details assignment operator
     *
     * @param rhs Reference to IzatLocation
     * @returs reference to IzatLocation
     */
    IzatLocation & operator= (const IzatLocation & rhs) {
        makeCopy (rhs);
        return *this;
    }

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IzatLocation () {
        reset ();
    };

    /**
     * @brief Reset
     * @details Reset
     */
    void reset () {

        mHasUtcTimestampInMsec = false;
        mHasElapsedRealTimeInNanoSecs = false;
        mHasLatitude = false;
        mHasLongitude = false;
        mHasHorizontalAccuracy = false;
        mHasAltitudeWrtEllipsoid = false;
        mHasAltitudeWrtMeanSeaLevel = false;
        mHasBearing = false;
        mHasSpeed = false;
        mHasAltitudeMeanSeaLevel = false;
        mHasDop = false;
        mHasMagneticDeviation = false;
        mHasVertUnc = false;
        mHasSpeedUnc = false;
        mHasBearingUnc = false;
        mHasHorizontalReliability = false;
        mHasVerticalReliability = false;
        mHasHorUncEllipseSemiMajor = false;
        mHasHorUncEllipseSemiMinor = false;
        mHasHorUncEllipseOrientAzimuth = false;
        mHasNetworkPositionSource = false;
        mHasPositionDynamics = false;
        mHasGpsTime = false;
    }

    /**
     * @brief Check if location is valid
     * @details Check if location is valid
     * @return true if valid or false if not valid
     */
    bool isValid () const {
        return  (mHasLatitude && mHasLongitude && mHasHorizontalAccuracy);
    }

    /**
     * @brief Convert contents to string
     * @details Convert contents to string
     *
     * @param valueStr reference to string where the final string value is stored.
     */
    void stringify (string & valueStr) const {
        valueStr.clear ();
        if (isValid ()) {
            valueStr += "Latitude: ";
            char t[50];
            memset (t, '\0', 50);
            snprintf (t, 50, "%f", mLatitude);
            string latitude (t);
            memset (t, '\0', 50);
            valueStr += latitude;
            valueStr += ", Longitude: ";
            snprintf (t, 50, "%f", mLongitude);
            string longitude (t);
            memset (t, '\0', 50);
            valueStr += longitude;
            valueStr += ", Horizontal Acc: ";
            snprintf (t, 50, "%f", mHorizontalAccuracy);
            string horizontalAccuracy (t);
            memset (t, '\0', 50);
            valueStr += horizontalAccuracy;

            if (mHasVertUnc) {
                valueStr += ", Vertical Uncertainity: ";
                snprintf (t, 50, "%f", mVertUnc);
                string vertUnc (t);
                memset(t, '\0', 50);
                valueStr += vertUnc;
            }

            if (mHasBearing) {
                valueStr += ", Bearing: ";
                snprintf (t, 50, "%f", mBearing);
                string bearing (t);
                memset (t, '\0', 50);
                valueStr += bearing;
            }

            if (mHasBearingUnc) {
                valueStr += ", Bearing Uncertainity: ";
                snprintf (t, 50, "%f", mBearingUnc);
                string bearingUnc(t);
                memset (t, '\0', 50);
                valueStr += bearingUnc;
            }

            if (mHasSpeed) {
                valueStr += ", Speed: ";
                snprintf (t, 50, "%f", mSpeed);
                string speed (t);
                memset(t, '\0', 50);
                valueStr += speed;
            }

            if (mHasSpeedUnc) {
                valueStr += ", Speed Uncertainity: ";
                snprintf (t, 50, "%f", mSpeedUnc);
                string speedUnc (t);
                memset(t, '\0', 50);
                valueStr += speedUnc;
            }

            if (mHasNetworkPositionSource) {
                valueStr += ", (Network Position Source: CELL:0 WIFI:1) ";
                snprintf (t, 50, "%d", mNetworkPositionSource);
                string source (t);
                memset(t, '\0', 50);
                valueStr += source;
            }
        } else {
            valueStr += "Position Invalid";
        }
    }

    /**
     * Boolean flag to indicate the presence of UTC time stamp
     */
    bool mHasUtcTimestampInMsec;

    /**
     * UTC time stamp in milliseconds
     */
    int64 mUtcTimestampInMsec;

    /**
     * Boolean flag to indicate the presence of elapsed real time nano seconds
     */
    bool mHasElapsedRealTimeInNanoSecs;

    /**
     * Elapsed real time in nano seconds
     */
    int64 mElapsedRealTimeInNanoSecs;

    /**
     * Boolean flag to indicate the presence of latitude
     */
    bool mHasLatitude;
    /**
     * Latitude
     */
    double mLatitude;

    /**
     * Boolean flag to indicate the presence of longitude
     */
    bool mHasLongitude;

    /**
     * Longitude
     */
    double mLongitude;

    /**
     * Boolean flag to indicate the presence of horizontal accuracy
     */
    bool mHasHorizontalAccuracy;

    /**
     * Horizontal accuracy
     */
    float mHorizontalAccuracy;

    /**
     * Boolean flag to indicate the presence of Altitude with respect to ellipsoid
     */
    bool mHasAltitudeWrtEllipsoid;

    /**
     * Altitude with respect to ellipsoid
     */
    double mAltitudeWrtEllipsoid;

    /**
     * Boolean flag to indicate the presence of Altitude with respect to sea level
     */
    bool mHasAltitudeWrtMeanSeaLevel;

    /**
     * Altitude with respect to sea level
     */
    double mAltitudeWrtMeanSeaLevel;

    /**
     * Boolean flag to indicate the presence of Bearing
     */
    bool mHasBearing;

    /**
     * Bearing
     */
    float mBearing;

    /**
     * Boolean flag to indicate the presence of Speed
     */
    bool mHasSpeed;

    /**
     * Speed
     */
    float mSpeed;

    /**
     * Position Source
     */
    uint16_t        mPositionSource;

    /**
     * Boolean flag to indicate the presence of Altitude wrt mean sea level
     */
    bool mHasAltitudeMeanSeaLevel;

    /**
    * Contains the Altitude wrt mean sea level
    */
    float           mAltitudeMeanSeaLevel;

    /**
     * Boolean flag to indicate the presence of pdop
     */
    bool mHasDop;

    /**
    *Contains Position Dilusion of Precision.
    */
    float           mPdop;

    /**
    * Contains Horizontal Dilusion of Precision.
    */
    float           mHdop;

    /**
    * Contains Vertical Dilusion of Precision.
    */
    float           mVdop;

    /**
     * Boolean flag to indicate the presence of MagneticDeviation
     */
    bool mHasMagneticDeviation;

    /**
    * Contains Magnetic Deviation.
    */
    float           mMagneticDeviation;

    /**
     * Boolean flag to indicate the presence of VertUnc
     */
    bool mHasVertUnc;

    /**
    * vertical uncertainty in meters
    */
    float           mVertUnc;

    /**
     * Boolean flag to indicate the presence of SpeedUnc
     */
    bool mHasSpeedUnc;

    /**
    * speed uncertainty in m/s
    */
    float           mSpeedUnc;

    /**
     * Boolean flag to indicate the presence of BearingUnc
     */
    bool mHasBearingUnc;

    /**
    * heading uncertainty in degrees (0 to 359.999)
    */
    float           mBearingUnc;

    /**
     * Boolean flag to indicate the presence of HorizontalReliability
     */
     bool mHasHorizontalReliability;

    /**
    * horizontal reliability.
    */
    uint16_t  mHorizontalReliability;

    /**
     * Boolean flag to indicate the presence of VerticalReliability
     */
     bool mHasVerticalReliability;

    /**
    * vertical reliability.
    */
    uint16_t  mVerticalReliability;
    /**
     * Boolean flag to indicate the presence of HorUncEllipseSemiMajor
     */
     bool mHasHorUncEllipseSemiMajor;

    /*
    * Horizontal Elliptical Uncertainty (Semi-Major Axis)
    */
    float            mHorUncEllipseSemiMajor;

    /**
     * Boolean flag to indicate the presence of HorUncEllipseSemiMinor
     */
     bool mHasHorUncEllipseSemiMinor;

    /*
    * Horizontal Elliptical Uncertainty (Semi-Minor Axis)
    */
    float            mHorUncEllipseSemiMinor;

    /**
     * Boolean flag to indicate the presence of HorUncEllipseOrientAzimuth
     */
     bool mHasHorUncEllipseOrientAzimuth;

    /*
    * Elliptical Horizontal Uncertainty HorUncEllipseOrientAzimuth
    */
    float            mHorUncEllipseOrientAzimuth;

    /**
      * Boolean flag to indicate presence of network position source type
      */
    bool mHasNetworkPositionSource;

    /**
      * Network position source
      */
    IzatNetworkPositionSourceType mNetworkPositionSource;

    /**
      * Boolean flag to indicate presence of nav solution mask
      */
    bool mHasNavSolutionMask;

    /**
      * Nav Solution Mask
      */
    uint32_t mNavSolutionMask;

    /**
      * Boolean flag to indicate presence of position tech mask
      */
    bool mHasPositionTechMask;

    /**
      * Position Tech Mask
      */
    uint32_t mPositionTechMask;

    /**
      * Boolean flag to indicate presence of Position body frame data info
      */
    bool mHasPositionDynamics;

    /**
      * Body dynamics data field mask
      */
    uint32_t mPosDatamask;

    /**
      * Rate of Speed change
      */
    float mLongAccel;

    /**
      * Accel Accuracy
      */
    float mLatAccel;

    /**
      * vertical Accel Accuracy
      */
    float mVertAccel;

    /**
      * Yaw rate Accuracy
      */
    float mYawRate;

    /**
      * Pitch In deg
      */
    float mPitch;

    /**
      * Boolean flag to indicate presence of gpsTime info
      */
    bool mHasGpsTime;

    /**
      * gps week
      */
    uint16_t mGpsWeek;

    /**
      * Amount of time into the current GPS week (Milliseconds)
      */
    uint32_t mGpsTimeOfWeekMs;

    /**
      * Boolean flag to indicate presence of Dilution of precision
      */
    bool mHasExtDOP;

    /**
      * Position dilution of precision.
      */
    float mExtPdop;

    /**
      * Horizontal dilution of precision.
      */
    float mExtHdop;

    /**
      * Vertical dilution of precision.
      */
    float mExtVdop;

    /**
      * geometric  dilution of precision.
      */
    float mExtGdop;

    /**
      * time dilution of precision.
      */
    float mExtTdop;

private:
    /**
     * @brief Copy method
     * @details Copy method
     *
     * @param rhs Reference to IzatLocation indcating where to copy from.
     */
    void makeCopy  (const IzatLocation & rhs) {
        mHasUtcTimestampInMsec = rhs.mHasUtcTimestampInMsec;
        mUtcTimestampInMsec = rhs.mUtcTimestampInMsec;

        mHasElapsedRealTimeInNanoSecs = rhs.mHasElapsedRealTimeInNanoSecs;
        mElapsedRealTimeInNanoSecs = rhs.mElapsedRealTimeInNanoSecs;

        mHasLatitude = rhs.mHasLatitude;
        mLatitude = rhs.mLatitude;

        mHasLongitude= rhs.mHasLongitude;
        mLongitude = rhs.mLongitude;

        mHasHorizontalAccuracy = rhs.mHasHorizontalAccuracy;
        mHorizontalAccuracy = rhs.mHorizontalAccuracy;

        mHasAltitudeWrtEllipsoid = rhs.mHasAltitudeWrtEllipsoid;
        mAltitudeWrtEllipsoid = rhs.mAltitudeWrtEllipsoid;

        mHasAltitudeWrtMeanSeaLevel = rhs.mHasAltitudeWrtMeanSeaLevel;
        mAltitudeWrtMeanSeaLevel = rhs.mAltitudeWrtMeanSeaLevel;

        mHasBearing = rhs.mHasBearing;
        mBearing = rhs.mBearing;

        mHasSpeed = rhs.mHasSpeed;
        mSpeed = rhs.mSpeed;
        mPositionSource = rhs.mPositionSource;

        mHasAltitudeMeanSeaLevel = rhs.mHasAltitudeMeanSeaLevel;
        mAltitudeMeanSeaLevel = rhs.mAltitudeMeanSeaLevel;

        mHasDop = rhs.mHasDop;
        mPdop = rhs.mPdop;
        mHdop = rhs.mHdop;
        mVdop = rhs.mVdop;

        mHasMagneticDeviation = rhs.mHasMagneticDeviation;
        mMagneticDeviation = rhs.mMagneticDeviation;

        mHasVertUnc = rhs.mHasVertUnc;
        mVertUnc = rhs.mVertUnc;

        mHasSpeedUnc = rhs.mHasSpeedUnc;
        mSpeedUnc = rhs.mSpeedUnc;

        mHasBearingUnc = rhs.mHasBearingUnc;
        mBearingUnc = rhs.mBearingUnc;

        mHasHorizontalReliability = rhs.mHasHorizontalReliability;
        mHorizontalReliability = rhs.mHorizontalReliability;

        mHasVerticalReliability = rhs.mHasVerticalReliability;
        mVerticalReliability = rhs.mVerticalReliability;

        mHasHorUncEllipseSemiMajor = rhs.mHasHorUncEllipseSemiMajor;
        mHorUncEllipseSemiMajor = rhs.mHorUncEllipseSemiMajor;

        mHasHorUncEllipseSemiMinor = rhs.mHasHorUncEllipseSemiMinor;
        mHorUncEllipseSemiMinor= rhs.mHorUncEllipseSemiMinor;

        mHasHorUncEllipseOrientAzimuth = rhs.mHasHorUncEllipseOrientAzimuth;
        mHorUncEllipseOrientAzimuth = rhs.mHorUncEllipseOrientAzimuth;

        mHasNetworkPositionSource = rhs.mHasNetworkPositionSource;
        mNetworkPositionSource = rhs.mNetworkPositionSource;

        mHasPositionTechMask = rhs.mHasPositionTechMask;
        mPositionTechMask = rhs.mPositionTechMask;

        mHasNavSolutionMask = rhs.mHasNavSolutionMask;
        mNavSolutionMask = rhs.mNavSolutionMask;

        mHasPositionDynamics = rhs.mHasPositionDynamics;
        mPosDatamask = rhs.mPosDatamask;
        mLongAccel = rhs.mLongAccel;
        mLatAccel = rhs.mLatAccel;
        mVertAccel = rhs.mVertAccel;
        mYawRate = rhs.mYawRate;
        mPitch = rhs.mPitch;

        mHasGpsTime = rhs.mHasGpsTime;
        mGpsWeek = rhs.mGpsWeek;
        mGpsTimeOfWeekMs = rhs.mGpsTimeOfWeekMs;

        mHasExtDOP = rhs.mHasExtDOP;
        mExtPdop = rhs.mExtPdop;
        mExtHdop = rhs.mExtHdop;
        mExtVdop = rhs.mExtVdop;
        mExtGdop = rhs.mExtGdop;
        mExtTdop = rhs.mExtTdop;

    }
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATLOCATION_H__
