/*==============================================================================
  FILE:         rsc_config.h

  OVERVIEW:     This file contains target specific data related to the
                RSC sequences

  DEPENDENCIES: None
                
                Copyright (c) 2017-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "comdef.h"

/*==========================================================================
 *                          DEFINITIONS
 *=========================================================================*/
/* Child offsets */
#define Q6SS_RSCC_SSC_LPASS_BUS_ISO_SEQ	               0x0
#define Q6SS_RSCC_SSC_LPASS_BUS_DEISO_SEQ	           0x12
#define Q6SS_RSCC_SSC_LPASS_SHORT_APCR_SEQ_LPASS_SSC   0x22
#define Q6SS_RSCC_SSC_LPASS_LONG_APCR_SEQ_TCS	       0x5a
#define Q6SS_RSCC_SSC_LPASS_LONG_APCR_SEQ_NOTCS	       0xd0

#define Q6SS_RSCC_SSC_LPASS_NUMSEQWORDS 79
#define Q6SS_RSCC_SSC_LPASS_NUMBRANCHES 0

#define RSC_NUM_CHILD_LOW_POWER_MODES       5


/* Parent offsets */
#define SSC_LPASS_RSCP_LONG_SEQ             0x0
#define SSC_LPASS_RSCP_EXIT_SLEEP           0x28

#define SSC_LPASS_RSCP_NUMSEQWORDS           11
#define SSC_LPASS_RSCP_NUMBRANCHES            2

#define RSC_NUM_PARENT_LOW_POWER_MODES        1
