/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_CB_H
#define GNSS_MULTI_CLIENT_CB_H
#include "GnssMultiClientCase.h"
#include "GnssCbBase.h"

namespace garden {

class GnssMultiClientCase;
class GnssMultiClientCb : public GnssCbBase {
public:
    GnssMultiClientCb(GnssMultiClientCase* gCase);
    ~GnssMultiClientCb();
    void gnssLocationCb(Location& location) override;
    void gnssStatusCb(uint32_t status) override;
    void gnssSetCapabilitiesCb(uint32_t capabilities) override;
    void gnssSvStatusCb(GnssSvNotification& svNotify) override;

private:
    GnssMultiClientCase* mGnssCase = nullptr;
};



} //namespace garden


#endif //GNSS_MULTI_CLIENT_CB_H
