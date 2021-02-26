/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef ULP_CASE_H
#define ULP_CASE_H

#include <GnssCase.h>
#include <ulp_service.h>

namespace garden {

class UlpCase : public GnssCase {

#define ULP_CASE_BASE GNSS_CASE_MAX
typedef enum {
    ULP_CASE_LOCATION   = (ULP_CASE_BASE << 0),
    ULP_CASE_TIMEOUT    = (ULP_CASE_BASE << 1),
    ULP_CASE_MAX        = (ULP_CASE_BASE << 2),
} ULP_CASE_FLAG;

public:
    UlpCase();
    virtual ~UlpCase();
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();

    void setupPhoneContext(uint32_t context);
    void updatePhoneContext();
    void nlpResponse();
    void updateCriteriaAndSleep(UlpLocationCriteria& criteria,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void updateCriteriaWithSource(uint32_t action, uint32_t source,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void updateCriteriaWithAccuracy(uint32_t action, uint32_t accuracy,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void addCriteriaWithSource(uint32_t source,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void removeCriteriaWithSource(uint32_t source,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void addCriteriaWithAccuracy(uint32_t accuracy,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void removeCriteriaWithAccuracy(uint32_t accuracy,
            uint32_t type, uint32_t interval, uint32_t timeout);
    void startAndSleep(uint32_t timeout);
    void stopAndSleep(uint32_t timeout);
    void expectLocation(uint32_t timeout);
    void expectLocation();

    // Callbacks
    static pthread_t createThreadCb(const char *name, void (*start)(void *), void *arg);
    static void locationCb(UlpLocation* location,
            GpsLocationExtended* locationExtended, enum loc_sess_status status);
    static void statusCb(const LocGpsStatusValue status);
    static void nmeaCb(const UlpNmea *pNmeaStr);
    static void svInfoCb(const GnssSvNotification* svNotify, uint16_t source);
    static void requestPhoneContextCb(UlpPhoneContextRequest *req);
    static void networkLocationRequestCb(UlpNetworkRequestPos* req);

    // Cases
    GARDEN_RESULT ulpCase1();
    GARDEN_RESULT ulpCase2();
    GARDEN_RESULT ulpCase3();
    GARDEN_RESULT ulpCase4();
    GARDEN_RESULT ulpCase5();
    GARDEN_RESULT ulpCase6();
    GARDEN_RESULT ulpCase7();
    GARDEN_RESULT ulpCase8();
    GARDEN_RESULT ulpCase9();
    GARDEN_RESULT ulpCase10();
    GARDEN_RESULT ulpCase11();
    GARDEN_RESULT ulpCase12();

    static GARDEN_RESULT init();
    static void addCase(int idx);

public:
    static const UlpEngineInterface* pUlpEngineInterface;
    static const UlpPhoneContextInterface* pUlpPhoneContextInterface;
    static const UlpNetworkInterface* pUlpNetworkInterface;

private:
    static UlpCase* mInstance;
    uint32_t mPhoneContext;
    GardenTimer mTimer;
};

} // namespace garden
#endif // ULP_CASE_H
