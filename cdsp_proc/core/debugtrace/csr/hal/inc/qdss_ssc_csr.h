#ifndef QDSS_SSC_CSR_H
#define QDSS_SSC_CSR_H

/*=============================================================================

FILE:         qdss_ssc_csr.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "DALStdDef.h"

void HAL_qdss_ssc_csr_HalConfigInit(uint32 base_addr);
void qdss_ssc_csr_LoadTimestamp(uint64 timeval); 
void HAL_qdss_ssc_csr_EnableTimestampGenerator(void);
void HAL_qdss_ssc_tsgen_Enable(void);
void qdss_ssc_ts_lpi_request(uint8 request);


#endif //QDSS_SSC_CSR_H
