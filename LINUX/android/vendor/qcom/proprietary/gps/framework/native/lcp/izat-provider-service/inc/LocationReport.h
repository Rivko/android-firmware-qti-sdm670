/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  LocationReport

  Copyright  (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_LOCATIONREPORT_H__
#define __IZAT_MANAGER_LOCATIONREPORT_H__

#include "IzatLocation.h"
#include "WiperData.h"

namespace izat_manager
{

class LocationReport : public IzatLocation {
public:
    typedef enum ProcessorSrc {
        ProcessorSrc_AP,
        ProcessorSrc_Modem
    } ProcessorSrc;

    bool mHasHorizontalConfidence;
    uint8 mHorizontalConfidence;

    bool mHasHorizontalReliability;
    ReliabilityValue mHorizontalReliability;

    bool mHasVerticalConfidence;
    uint8 mVerticalConfidence;

    bool mHasVerticalReliability;
    ReliabilityValue mVerticalReliability;

    bool mHasPositionSource;
    PositionSourceType mPositionSource;

    bool mHasProcessorSource;
    ProcessorSrc mProcessorSource;

    bool mHasPositionSourceProvider;
    PositionSourceProviderType mPositionSourceProvider;

    bool mHasPropagatedFix;
    bool mIsPropagatedFix;

    bool mIsIntermediateFix;

    LocationReport() {
        resetInternal();
    }

    LocationReport(const LocationReport &locReport) :
        IzatLocation(locReport) {
        makeCopy(locReport);
    }

    LocationReport(const IzatLocation &izatLoc) :
        IzatLocation(izatLoc) {
        resetInternal();
    }

    LocationReport & operator=(const LocationReport &locReport) {
        IzatLocation::operator=(locReport);
        makeCopy(locReport);
        return *this;
    }

    virtual ~LocationReport() {
        resetInternal();
    };

    void reset()
    {
        IzatLocation::reset();
        resetInternal();
    }

    void stringify (string & valueStr) const {
        if (isValid ()) {
            IzatLocation :: stringify (valueStr);
            if (mHasPositionSource) {
                char t[10];
                snprintf (t, 10, "%d", mPositionSource);
                string positionSource (t);
                valueStr += ", Position Source (GNSS-0, CELLID-1, ENH_CELLID-2, WIFI-3, TERRESTRIAL-4, GNSS_TERRESTRIAL_HYBRID-5, OTHER-6): ";
                valueStr += positionSource;
            }

            if (mHasProcessorSource) {
                char t[10];
                snprintf (t, 10, "%d", mProcessorSource);
                string processorSource (t);
                valueStr += ", Processor Source (AP - 0, Modem - 1): ";
                valueStr += processorSource;
            }

            if (mHasPropagatedFix) {
                valueStr += ", Propagated Fix: ";
                valueStr += mIsPropagatedFix ? "True" : "False";
            }

            if (mHasPositionSourceProvider) {
                char t[10];
                snprintf (t, 10, "%d", mPositionSourceProvider);
                string positionSourceProvider (t);
                valueStr += ", Source Provider (External - 0, INTERNAL - 1): ";
                valueStr += positionSourceProvider;
            }
        } else {
            valueStr += "Not Valid";
        }
    }
protected:

private:
    void makeCopy(const LocationReport & locReport) {
        mHasHorizontalConfidence = locReport.mHasHorizontalConfidence;
        mHorizontalConfidence = locReport.mHorizontalConfidence;

        mHasHorizontalReliability = locReport.mHasHorizontalReliability;
        mHorizontalReliability = locReport.mHorizontalReliability;

        mHasVerticalConfidence = locReport.mHasVerticalConfidence;
        mVerticalConfidence = locReport.mVerticalConfidence;

        mHasVerticalReliability = locReport.mHasVerticalReliability;
        mVerticalReliability = locReport.mVerticalReliability;

        mHasPositionSource = locReport.mHasPositionSource;
        mPositionSource = locReport.mPositionSource;

        mHasProcessorSource = locReport.mHasProcessorSource;
        mProcessorSource = locReport.mProcessorSource;

        mHasPositionSourceProvider = locReport.mHasPositionSourceProvider;
        mPositionSourceProvider = locReport.mPositionSourceProvider;

        mHasPropagatedFix = locReport.mHasPropagatedFix;
        mIsPropagatedFix = locReport.mIsPropagatedFix;

        mIsIntermediateFix = locReport.mIsIntermediateFix;
    }

    void resetInternal() {
        mHasHorizontalConfidence = false;
        mHasHorizontalReliability = false;
        mHasVerticalConfidence = false;
        mHasVerticalReliability = false;
        mHasPositionSource = false;
        mHasProcessorSource = false;
        mHasPositionSourceProvider = false;
        mHasPropagatedFix = false;
        mIsIntermediateFix = false;
    }
};

}// namespace izatmanager

#endif // #ifndef __IZAT_MANAGER_LOCATIONREPORT_H__
