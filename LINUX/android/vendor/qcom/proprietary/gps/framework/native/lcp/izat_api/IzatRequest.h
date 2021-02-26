/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IzatRequest

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATREQUEST_H__
#define __IZAT_MANAGER_IZATREQUEST_H__

#include <comdef.h>
#include "IzatTypes.h"

namespace izat_manager {

/**
 * @brief IzatRequest class
 * @details This class is used to create/set location requests.
 */
class IzatRequest {
public:

    typedef IzatStreamType IzatProviderType;

    // Create a default request
    /**
     * @brief Create a default location request.
     * @details Create a default location request.
     * @return Pointer to an instance of IzatRequest created on heap.
     */
    static IzatRequest *  create () {
        return new IzatRequest ();
    }

    /**
     * @brief Create location provider based request.
     * @details Create location provider based request;
     *          This method only supports 2 provider types. IZAT_STREAM_GNSS and IZAT_STREAM_NETWORK
     *          In order to create a request for IZAT_STREAM_FUSED use createCriteriaRequest method below.
     *
     *
     * @param provider Type of provider.
     * @param timeBetweenFixes Time between fixes.
     * @param distanceBetweenFixes Distance between fixes.
     * @param singleShot Boolean flag to indicate whether it is singleshot (true) or tracking (false) request..
     * @return Pointer to an instance of IzatRequest created on heap.
     */
    static IzatRequest * createProviderRequest (IzatProviderType provider, int64 timeBetweenFixes, float  distanceBetweenFixes, bool singleShot)
    {
        if (distanceBetweenFixes < 0) distanceBetweenFixes = 0;
        IzatHorizontalAccuracy acc;
        if (provider == IZAT_STREAM_GNSS) {
            acc = IZAT_HORIZONTAL_FINE;
        }
        else {
            acc = IZAT_HORIZONTAL_BLOCK;
        }

        IzatRequest * request = new IzatRequest ();
        if (request) {
            request->setProvider (provider);
            request->setHorizontalAccuracy (acc);
            request->setInterval (timeBetweenFixes);
            request->setDistance (distanceBetweenFixes);
            if (singleShot) request->setNumUpdates (1);
        }
        return request;
    }

    /**
     * @brief Create Criteria based request.
     * @details Create criteria based request;
     *          Default provider for this request is IZAT_STREAM_FUSED.
     *
     *
     * @param horizontalAccuracy Horizontal accuracy.
     * @param altitudeAccuracy Altitude accuracy.
     * @param bearingAccuracy Bearing Accuracy.
     * @param timeBetweenFixes Time between fixes.
     * @param distanceBetweenFixes Distance between fixes
     * @param singleShot Boolean flag to indicate whether it is singleshot (true) or tracking (false) request.
     * @return Pointer to an instance of IzatRequest created on heap.
     */
    static IzatRequest * createCriteriaRequest (IzatHorizontalAccuracy horizontalAccuracy, IzatOtherAccuracy altitudeAccuracy, IzatOtherAccuracy bearingAccuracy, int64 timeBetweenFixes, float  distanceBetweenFixes, bool singleShot)
    {
        (void) bearingAccuracy;
        (void) altitudeAccuracy;
        if (distanceBetweenFixes < 0) distanceBetweenFixes = 0;
        IzatRequest * request = new  IzatRequest ();
        if (request) {
            request->setHorizontalAccuracy (horizontalAccuracy);
            request->setInterval (timeBetweenFixes);
            request->setDistance (distanceBetweenFixes);
            if (singleShot) request->setNumUpdates (1);
        }
        return request;
    }

    /**
     * @brief Default contructor
     * @details Default constructor
     *
     */
    IzatRequest ()
    : mProvider (IZAT_STREAM_FUSED),
      mNumUpdates (INTEGER_MAX_VAL),
      mSuggestedResponseTimeForFirstFix (0),
      mTimeIntervalBetweenFixes (0),
      mSmallestDistanceBetweenFixes (0),
      mSuggestedHorizontalAccuracy (IZAT_HORIZONTAL_NONE),
      mSuggestedAltitudeAccuracy (IZAT_OTHER_NONE),
      mSuggestedBearingAccuracy (IZAT_OTHER_NONE) {}

    /**
     * @brief Destructor.
     * @details Destructor.
     */
    virtual ~IzatRequest () {}

    /**
     * @brief Gets provider.
     * @details Gets provider.
     * @return Instance of IzatProviderType.
     */
    IzatProviderType getProvider () const {
        return mProvider;
    }

    /**
     * @brief Gets number of location updates.
     * @details Gets number of location updates.
     * @return Number of location updates.
     */
    int32 getNumUpdates () const {
        return mNumUpdates;
    }

    /**
     * @brief Gets time for first fix.
     * @details Gets time for first fix.
     * @return Time for first fix.
     */
    int64 getTimeForFirstFix () const {
        return mSuggestedResponseTimeForFirstFix;
    }

    /**
     * @brief Gets interval between fixes.
     * @details Gets interval between fixes.
     * @return Interval between fixes.
     */
    int64 getInterval () const {
        return mTimeIntervalBetweenFixes;
    }

    /**
     * @brief Gets distance between fixes.
     * @details Gets distance between fixes.
     * @return Distance between fixes.
     */
    float getDistance () const {
        return mSmallestDistanceBetweenFixes;
    }

    /**
     * @brief Gets horizontal accuracy.
     * @details Gets horizontal accuracy.
     * @return Horizontal accuracy.
     */
    IzatHorizontalAccuracy getHorizontalAccuracy () const {
        return mSuggestedHorizontalAccuracy;
    }

    /**
     * @brief Gets altitude accuracy.
     * @details Gets altitude accuracy.
     * @return Altitude accuracy.
     */
    IzatOtherAccuracy getAltitudeAccuracy () const {
        return mSuggestedAltitudeAccuracy;
    }

    /**
     * @brief Gets Bearing accuracy.
     * @details Gets Bearing accuracy.
     * @return Bearing accuracy.
     */
    IzatOtherAccuracy getBearingAccuracy () const {
        return mSuggestedBearingAccuracy;
    }

    /**
     * @brief Sets provider.
     * @details Sets provider.
     *
     * @param provider Type of provider.
     */
    void setProvider (IzatProviderType provider) {
        mProvider = provider;
    }

    /**
     * @brief Sets number of location updates expected
     * @details Sets number of location updates expected;
     *          By default this will be set to a MAX value which means that the client will keep receiving
     *          location updates after it has requested for it and until it removes itself from it.
     *          Setting this to any other value such as 1 will indicate to the client that it will receive
     *          only one location update. But care must be taken by the client to remove itself after it has received
     *          this one update.
     *
     * @param numUpdates Number of location updates expected.
     */
    void setNumUpdates (int32 numUpdates) {
        mNumUpdates = numUpdates;
    }

    /**
     * @brief Sets time for first fix.
     * @details Sets time for first fix.
     *
     * @param time time for first fix.
     */
    void setTimeForFirstFix (int64 time) {
        mSuggestedResponseTimeForFirstFix = time;
    }

    /**
     * @brief Sets interval between fixes.
     * @details Sets interval between fixes.
     *
     * @param time Interval between fixes.
     */
    void setInterval (int64 time) {
        mTimeIntervalBetweenFixes = time;
    }

    /**
     * @brief Sets distance between fixes.
     * @details Sets distance between fixes.
     *
     * @param distance Distance between fixes.
     */
    void setDistance (float distance) {
        mSmallestDistanceBetweenFixes = distance;
    }

    /**
     * @brief Sets Horizonstal Accuracy.
     * @details Sets Horizonstal Accuracy.
     *
     * @param accuracy Horizontal accuracy.
     */
    void setHorizontalAccuracy (IzatHorizontalAccuracy accuracy) {
        mSuggestedHorizontalAccuracy = accuracy;
    }

    /**
     * @brief Sets Altitude accuracy.
     * @details Sets Altitude accuracy.
     *
     * @param accuracy Altitude accuracy.
     */
    void setAltitudeAccuracy (IzatOtherAccuracy accuracy) {
        mSuggestedAltitudeAccuracy = accuracy;
    }

    /**
     * @brief Sets Bearing accuracy.
     * @details Sets Bearing accuracy.
     *
     * @param accuracy Bearing acccuracy.
     */
    void setBearingAccuracy (IzatOtherAccuracy accuracy) {
        mSuggestedBearingAccuracy = accuracy;
    }

    /**
     * Expected number of location updates is intialized to this value.
     */
    static const int32 INTEGER_MAX_VAL = 2147483647;

private:

    /**
     * Provider Type.
     */
    IzatProviderType         mProvider;

    /**
     * Number of location updates requested.
     */
    int32                    mNumUpdates;

    /**
     * Response time for first fix in milliseconds
     */
    int64                    mSuggestedResponseTimeForFirstFix;

    /**
     * Time interval between fixes in milliseconds
     */
    int64                    mTimeIntervalBetweenFixes;

    /**
     * Smallest Distance between fixes.
     */
    float                    mSmallestDistanceBetweenFixes;

    /**
     * Horizontal accuracy.
     */
    IzatHorizontalAccuracy   mSuggestedHorizontalAccuracy;

    /**
     * Altitude accuracy.
     */
    IzatOtherAccuracy        mSuggestedAltitudeAccuracy;

    /**
     * Bearing accuracy.
     */
    IzatOtherAccuracy        mSuggestedBearingAccuracy;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATREQUEST_H__
