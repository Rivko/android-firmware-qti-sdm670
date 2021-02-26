/*
============================================================================

FILE:          rs_driver.h

DESCRIPTION:   Resampler HW driver internal header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================

============================================================================
  EDIT HISTORY FOR MODULE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-25   qm   Initial version.
============================================================================
*/
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef RS_MMPM_H
#define RS_MMPM_H

#include "hwd_mmpm.h"

//ADSPPM related functions
ADSPResult rs_mmpm_register(void);
ADSPResult rs_mmpm_deregister(void);
ADSPResult rs_mmpm_misc_static_vote(void);
ADSPResult rs_mmpm_misc_static_devote(void);
ADSPResult rs_mmpm_misc_dynamic_vote_clks(int16 job_id);
ADSPResult rs_mmpm_misc_dynamic_devote_clks(int16 job_id);
ADSPResult rs_mmpm_enable_core_clks(uint32 core_clk);
ADSPResult rs_mmpm_disable_core_clks(void);


#endif

#ifdef __cplusplus
}
#endif //__cplusplus


