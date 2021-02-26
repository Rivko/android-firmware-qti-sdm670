/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  GENERAL DESCRIPTION
  ComboNetworkProvider is a location provider that facilitates
  combining location results from proprietary and framework providers
  to give out the best location results.

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_COMBONETWORKPROVIDER_H__
#define __IZAT_MANAGER_COMBONETWORKPROVIDER_H__

#include "LocTimer.h"
#include "LocationProvider.h"
#include <IDataItemCore.h>

namespace izat_manager
{

class ComboNetworkProvider : public LocationProvider {

public:

    static ILocationProvider* getInstance(const struct s_IzatContext* izatContext);
    static int destroyInstance();
    void transitionPreferredProviderToQNP();

    // ILocationProvider overrides
    int setRequest(const LocationRequest *request);
    void enable();
    void disable();

    // ILocationResponse overrides
    void reportLocation(const LocationReport *report,
        const ILocationProvider *providerSrc = NULL);
    void reportError(const LocationError *error,
        const ILocationProvider *providerSrc = NULL);

    // IDataItemObserver overrides
    virtual void getName (std::string & name) { name = "ComboNetworkProvider";}
    virtual void notify(const std::list<IDataItemCore *> & dlist);

private:
    typedef enum {
        NLP_MODE_NONE = 0,
        NLP_MODE_GNP_ONLY = 1,
        NLP_MODE_QNP_ONLY = 2,
        NLP_MODE_COMBO = 3,
        NLP_MODE_QNP_PREFER = 4,
        NLP_MODE_LAST = 5
    } NlpMode;

    typedef enum {
        COMBO_STATE_DISABLED,
        COMBO_STATE_IDLE,
        COMBO_STATE_TRACKING
    } ComboTrackingState;

    typedef enum {
        PROVIDER_NONE,
        PROVIDER_QNP,
        PROVIDER_OSNP
    } ProviderType;

    class Timer : public LocTimer {
        ComboNetworkProvider* mClient;
    public:
        inline Timer(ComboNetworkProvider* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };
    friend class Timer;

    // Static member variables
    static ILocationProvider* mComboNetworkProvider;

    ILocationProvider* mQNP;
    ILocationProvider* mOSNP;

    NlpMode mNlpMode, mEmergencyNlpMode, mNlpModeInUse;
    ComboTrackingState mProviderState;
    bool mInEmergencyMode;
    bool mAwaitingQuickQnpFix;
    ProviderType mPreferredProvider;
    int mAdaptiveCounter;
    bool mToleranceExpired;
    uint32_t mCurrentTimeBetweenFixInMsec;

    const unsigned int mToleranceTimeFirstInMsec;
    const unsigned int mToleranceTimeAfterInMsec;
    const unsigned int mNlpThreshold;
    const unsigned int mAccuracyMultiple;
    const bool mNlpComboModeUsesQnpWithNoEulaConsent;
    const unsigned int mQuickFixWaitTimeMaxInMsec;
    const unsigned int mQuickFixAccuracyMinInMeters;

    Timer mToleranceTimer;
    ILocationResponse *mWiperObj;

    LocationReport mQNPLocReport;
    LocationReport mFrameworkLocReport;
    LocationError  mQNPErrorReport;
    LocationError  mFrameworkErrorReport;
    LocationError  mLocationError;

    bool mEULAAccepted;
    LocationRequest mCurrentRequest;

    ComboNetworkProvider(const struct s_IzatContext * izatContext,
        ILocationProvider* pQNP, ILocationProvider* pOSNP, NlpMode mode,
        NlpMode emergencyNlpMode, unsigned int toleranceTimeFirst,
        unsigned int toleranceTimeAfter, unsigned int nlpThreshold,
        unsigned int accuracyMultiple, unsigned int nlpComboModeUsesQnpWithNoEulaConsent,
        unsigned int quickFixWaitTimeMaxInMsec,
        unsigned int quickFixAccuracyMinInMeters);
    ~ComboNetworkProvider();

    void startTrackingSession(const LocationRequest *request);
    void updateTrackingSession(const LocationRequest *request);
    void stopTrackingSession(const LocationRequest *request);
    void resetForNextInterval(uint32_t interval);
    void resetToleranceTimer(uint32_t interval);
    void consolidateFirstFixesAndReport();
    void reportBestLocation(const LocationReport & report);
    void reportIntermediateLocation(LocationReport report);
    void increamentAdaptiveCounter();
    void decreamentAdaptiveCounter();
    void adjustAdaptiveCounter();

    void handleEULAUpdate(bool eulaAccepted);
    bool useQNPInQNPPreferredMode();

    struct switchNlpModeMsg : public LocMsg {
        ComboNetworkProvider* mCombo;
        LocationRequest mLocRequest;

        switchNlpModeMsg(ComboNetworkProvider *pCombo,
            const LocationRequest *request) : mCombo(pCombo) {
            memcpy(&mLocRequest, request, sizeof(LocationRequest));
        }
        virtual void proc() const;
    };

    struct requestLocationMsg : public LocMsg {
        ComboNetworkProvider* mCombo;
        LocationRequest mLocRequest;

        requestLocationMsg(ComboNetworkProvider *pCombo,
            const LocationRequest *request) : mCombo(pCombo) {
            memcpy(&mLocRequest, request, sizeof(LocationRequest));
        }
        virtual void proc() const;
    };

    struct toleranceTimerMsg : public LocMsg {
        ComboNetworkProvider* mCombo;

        inline toleranceTimerMsg(ComboNetworkProvider *pCombo) :
            mCombo(pCombo) {}
        virtual void proc() const;
        inline ~toleranceTimerMsg() {}
    };

    struct reportLocationMsg : public LocMsg {
        ComboNetworkProvider* mCombo;
        LocationReport mLocationReport;

        reportLocationMsg(ComboNetworkProvider *pCombo,
            const LocationReport* locReport) :
            mCombo(pCombo), mLocationReport(*locReport) {
        }
        virtual void proc() const;
        inline ~reportLocationMsg() {}
    };

    struct handleQNPLocReportMsg : public LocMsg {
        ComboNetworkProvider* mCombo;

        inline handleQNPLocReportMsg(ComboNetworkProvider *pCombo) :
            mCombo(pCombo) {}
        virtual void proc() const;
        inline handleQNPLocReportMsg() {}
    };

    struct handleFrameworkLocReportMsg : public LocMsg {
        ComboNetworkProvider* mCombo;

        inline handleFrameworkLocReportMsg(ComboNetworkProvider *pCombo) :
            mCombo(pCombo) {}
        virtual void proc() const;
        inline handleFrameworkLocReportMsg() {}
    };

    struct injectLocationInIdleMsg : public LocMsg {
        ComboNetworkProvider* mCombo;
        LocationReport mLocationReport;

        injectLocationInIdleMsg(ComboNetworkProvider *pCombo,
            const LocationReport* locReport) :
            mCombo(pCombo), mLocationReport(*locReport) {
        }
        virtual void proc() const;
        inline ~injectLocationInIdleMsg() {}
    };


    struct subscribeEULAMsg : public LocMsg {
        ComboNetworkProvider* mCombo;

        subscribeEULAMsg(ComboNetworkProvider *pCombo) : mCombo(pCombo) {}
        virtual void proc() const;
        inline ~subscribeEULAMsg() {}
    };

    struct handleOsObserverUpdateMsg : public LocMsg {
        ComboNetworkProvider* mCombo;
        std::list <IDataItemCore *> mDataItemList;

        handleOsObserverUpdateMsg(ComboNetworkProvider *pCombo,
            const std::list<IDataItemCore *> & dataItemList);

        ~handleOsObserverUpdateMsg();
        virtual void proc() const;
    };
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_COMBONETWORKPROVIDER_H__
