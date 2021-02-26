/*==============================================================================
  FILE:         rsc_config.h

  OVERVIEW:     This file contains target specific data related to the
                RSC sequences

  DEPENDENCIES: None
                
                Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "comdef.h"

/*==========================================================================
 *                          DEFINITIONS
 *=========================================================================*/
/* Child offsets */
#define Q6SS_RSCC_CSS_SHORT_APCR_SEQ_CSS          0x0
#define Q6SS_RSCC_CSS_LONG_APCR_SEQ_NOTCS         0x40
#define Q6SS_RSCC_CSS_NUMSEQWORDS                 47
#define Q6SS_RSCC_CSS_NUMBRANCHES                 0

/* Parent offsets */
#define TURING_RSCP_PDC_HS_PWR_COL                0x0
#define TURING_RSCP_NUMSEQWORDS                   8
#define TURING_RSCP_NUMBRANCHES                   0
