#ifndef QDSS_CTRL_PRIV_H
#define QDSS_CTRL_PRIV_H

/*=============================================================================

FILE:         qdss_ctrl_priv.h

DESCRIPTION:  Private data structures used by the qdss ctrl subsystem

================================================================================
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/

#include "DALSys.h"
#include "npa.h"
#include "tracer.h"

#define QDSS_CTRL_SUCCESS          0
#define QDSS_CTRL_ERROR            1

struct qdss_ctrl_s {
   DALSYSSyncHandle hSync;
   tracer_client_handle_t hTracer;
   uint32 test_data_count;
   boolean etm_port_enabled;
   uint8 etm_state;
   npa_client_handle npa_client;
};

extern struct qdss_ctrl_s qdss_ctrl_ctxt;

void qdss_ctrl_clk_on(void);
void qdss_ctrl_clk_off(void);

#endif //QDSS_CTRL_PRIV_H

