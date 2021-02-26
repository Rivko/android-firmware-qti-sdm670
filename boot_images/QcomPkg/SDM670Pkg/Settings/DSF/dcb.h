#ifndef __DCB_H__
#define __DCB_H__


/*==============================================================================
              Copyright (c) 2017, Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "HALhwio.h"
#include <string.h>
#include "ddr_common.h"
#include "HAL_SNS_DDR.h"
#include "target_config.h"

/*==============================================================================
                                  MACROS
==============================================================================*/
#define DCB_MAX_SIZE ((1024*13)+4) // must be mutiple of 4, must not be multiple of 512

/*==============================================================================
                                  TYPES
==============================================================================*/
// Indices for DCB header
enum
{
    DCB_CRC_IDX,
    DCB_SIZE_IDX,
    DSF_VERSION_IDX,
    DDR_RUNTIME_IDX,
    AOP_PARAMS_IDX,
    TRAINING_PARAMS_IDX,
    ECDT_PRE_DEVICE_INIT_IDX,
    ECDT_POST_DEVICE_INIT_IDX,
    DDRSS_CFG_IDX,
    MEMNOC_CFG_IDX,
    SHRM_CFG_IDX,
    MCCC_MSTR_CFG_IDX,
    MCCC_SLV_CFG_IDX,
    DQ_PHY_CFG_IDX,
    CA_PHY_CFG_IDX,
    DDR_CC_CFG_IDX,
    MC_CFG_IDX,
    LLCC_CFG_IDX,
    DQ_PHY_REGS_IDX,
    CA_PHY_REGS_IDX,
    DDI_INPUT_IDX,
    DCB_IDX_MAX,
};

#endif
