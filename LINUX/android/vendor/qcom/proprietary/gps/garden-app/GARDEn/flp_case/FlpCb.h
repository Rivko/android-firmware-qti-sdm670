/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef FLPCB_H
#define FLPCB_H
#include "FlpCase.h"

namespace garden {

class gnsscase;
class FlpCb : public GnssCbBase {
public:
    FlpCb(GnssCase* gCase);
    ~FlpCb() {}
    void batchingCb(size_t num_locations, Location* location) override;
    void startBatchingCb(LocationError error) override;
    void stopBatchingCb(LocationError error) override;
    void updateBatchingCb(LocationError error) override;
    void getBatchedLocationCb(LocationError error) override;
    void startTrackingCb(LocationError error) override;
    void stopTrackingCb(LocationError error) override;
    void updateTrackingCb(LocationError error) override;

private:
    GnssCase* mFlpCase = nullptr;
};

} //namespace garden


#endif //FLPCB_H
