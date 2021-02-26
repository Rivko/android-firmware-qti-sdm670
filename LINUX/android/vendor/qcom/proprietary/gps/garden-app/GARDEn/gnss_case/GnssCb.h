/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GNSSCB_H
#define GNSSCB_H
#include "GnssCase.h"

namespace garden {

class GnssCase;
class GnssCb : public GnssCbBase {
public:
    GnssCb(GnssCase* gCase);
    ~GnssCb();
    void gnssLocationCb(Location& location) override;
    void gnssStatusCb(uint32_t status) override;
    void gnssSvStatusCb(GnssSvNotification& svNotify) override;
    void gnssNmeaCb(uint64_t timestamp, const char* nmea, int length) override;
    void gnssMeasurementsCb(GnssMeasurementsNotification& gnssMeasurementsNotification) override;
    void gnssDataCb(GnssDataNotification& dataNotify) override;
    void gnssNiNotifyCb(uint32_t id, GnssNiNotification& notification) override;
    void gnssLocInfoNotifCb(GnssLocationInfoNotification gnssLocInfoNotif) override;

    void setNiParam(int niCount, int niResPatCount, int* networkInitiatedResponse);

private:
    GnssCase* mGnssCase = nullptr;
    int mNiCount;
    std::vector<int> mNetworkInitiatedResponse;
};



} //namespace garden


#endif //GNSSCB_H
