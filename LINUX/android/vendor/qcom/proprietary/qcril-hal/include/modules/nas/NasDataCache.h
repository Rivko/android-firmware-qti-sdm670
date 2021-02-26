/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include <mutex>
#include "modules/data/DataCommon.h"

class NasDataCache {

  /* Structures */
  public:
          static NasDataCache& getInstance();
          ~NasDataCache();
          /*
          enum PreferredDataTechType {
                  PREF_DATA_TECH_INVALID  = -1,
                  PREF_DATA_TECH_UNKNOWN  = 0,
                  PREF_DATA_TECH_FIRST    = PREF_DATA_TECH_UNKNOWN,
                  PREF_DATA_TECH_CDMA     = 1,
                  PREF_DATA_TECH_EVDO     = 2,
                  PREF_DATA_TECH_GSM      = 3,
                  PREF_DATA_TECH_UMTS     = 4,
                  PREF_DATA_TECH_EHRPD    = 5,
                  PREF_DATA_TECH_LTE      = 6,
                  PREF_DATA_TECH_TDSCDMA  = 7,
                  PREF_DATA_TECH_MAX
          };
          */

  private:
          std::mutex cacheMutex;
          unsigned int numCalls;
          //PreferredDataTechType prefDataTech;


  /* Methods */
  private:
        NasDataCache();

  public:
          void setNumDataCalls(unsigned int nCalls);
          unsigned int getNumDataCalls();

          /*
          void setPreferredDataTech(qcril_arb_pref_data_tech_e_type dataTech);
          PreferredDataTechType getPreferredDataTech();
          */
};
