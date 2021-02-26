/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef FLP_CASE_H
#define FLP_CASE_H

#include "GeofenceCase.h"

namespace garden {

#define FLP_CASE_BASE GEOFENCE_CASE_MAX
typedef enum {
    FLP_CASE_START     = (FLP_CASE_BASE << 0),
    FLP_CASE_UPDATE    = (FLP_CASE_BASE << 1),
    FLP_CASE_STOP      = (FLP_CASE_BASE << 2),
    FLP_CASE_GET       = (FLP_CASE_BASE << 3),
    FLP_CASE_MAX       = (FLP_CASE_BASE << 4),
} FLP_CASE_FLAG;

class FlpCase : public GnssCase {
public:
    FlpCase() = default;
    virtual ~FlpCase() {}
    virtual GARDEN_RESULT preRun();
    virtual GARDEN_RESULT postRun();
    GARDEN_RESULT menuCase();
    GARDEN_RESULT loadFromFileCase(std::string file);
    void startFlpSession(uint32_t mode, LocationOptions &options, int timeout);

private:
    void getBatchSize();
    void startBatching();
    void updateBatching();
    void stopBatching();
    void getBatchedLocation();
    void batchingCleanUp();
    void injectLocation();
};

} // namespace garden
#endif // FLP_CASE_H
