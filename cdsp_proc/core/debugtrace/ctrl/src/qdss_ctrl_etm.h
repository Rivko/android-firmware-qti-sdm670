#ifndef QDSS_CTRL_ETM_H
#define QDSS_CTRL_ETM_H

/*=============================================================================

FILE:         qdss_ctrl_etm.h

DESCRIPTION:

================================================================================
              Copyright (c) 2013-2016 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <stddef.h>

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// ETM configuration formats available.
#define QDSS_CTRL_ETM_CONFIG_FORMAT_ID_DSP    1
#define QDSS_CTRL_ETM_CONFIG_FORMAT_ID_xxx    2

// ETM configuration format of this code
#define QDSS_CTRL_ETM_CONFIG_FORMAT_ID  QDSS_CTRL_ETM_CONFIG_FORMAT_ID_DSP
#define QDSS_CTRL_ETM_CONFIG_FORMAT_NAME  "dsp"


// ETM configurable parameter identifiers.
#define QDSS_CTRL_ETM_PARAM_ID_DSP_NONE    0
#define QDSS_CTRL_ETM_PARAM_ID_DSP_MODE    1
#define QDSS_CTRL_ETM_PARAM_ID_DSP_ROUTE   2
#define QDSS_CTRL_ETM_PARAM_ID_DSP_FILTER  3
#define QDSS_CTRL_ETM_PARAM_ID_DSP_UNKNOWN 0xFF

/*---------------------------------------------------------------------------
 * Function Declarations
 * ------------------------------------------------------------------------*/

void qdss_ctrl_etm_init_config_def(void);

int qdss_ctrl_etm_get_config(const char *pget_str,
                             char *presp_str,
                             size_t resp_max_len);
int qdss_ctrl_etm_set_config(const char *pset_str);

int qdss_ctrl_etm_get_param(uint16 param_id, uint32 *pval);
int qdss_ctrl_etm_set_param(uint16 param_id, uint32 val);

int qdss_ctrl_etm_enable(void);
int qdss_ctrl_etm_disable(void);


#endif //QDSS_CTRL_ETM_H
