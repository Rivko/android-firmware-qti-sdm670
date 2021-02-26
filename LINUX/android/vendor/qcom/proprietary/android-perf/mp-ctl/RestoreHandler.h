/******************************************************************************
  @file    RestoreHandler.h
  @brief   Implementation of nodes restoration

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __RESET_HANDLER__H_
#define __RESET_HANDLER__H_

#include "ResourceInfo.h"
#include "OptsData.h"
#include "Target.h"

class ResetHandler {
    private:
        static ResetHandler mResetHandler;

    private:
        ResetHandler();
        ResetHandler(ResetHandler const& rh);
        ResetHandler& operator=(ResetHandler const& rh);

    public:
        ~ResetHandler();

        int Init();

        static void reset_to_default_values(OptsData &d);
        static void reset_freq_to_default(OptsData &d);
        static void reset_cores_status(OptsData &d);
        static void reset_sched_boost(OptsData &d);
        static void reset_cpu_nodes(int cpu);

        static ResetHandler &getInstance() {
            return mResetHandler;
        }

};

#endif /*__RESET_HANDLER__H_*/
