#ifndef DRV_MAPPING_H
#define DRV_MAPPING_H

#include "comdef.h"

typedef enum {
    RSC_DRV_GLOBAL_TZ            = 0,
    RSC_DRV_GLOBAL_HYP           = 1,
    RSC_DRV_GLOBAL_HLOS          = 2,
    RSC_DRV_GLOBAL_SECPROC       = 3,
    RSC_DRV_GLOBAL_AUDIO         = 4,
    RSC_DRV_GLOBAL_SENSORS       = 5,
    RSC_DRV_GLOBAL_AOP           = 6,
    RSC_DRV_GLOBAL_DEBUG         = 7,
    RSC_DRV_GLOBAL_GPU           = 8,
    RSC_DRV_GLOBAL_DISPLAY       = 9,
    RSC_DRV_GLOBAL_COMPUTE_DSP   = 10,
    RSC_DRV_GLOBAL_MODEM_SW      = 11,
    RSC_DRV_GLOBAL_MODEM_HW      = 12,
    RSC_DRV_GLOBAL_ARC_CPRF      = 13,
    RSC_DRV_GLOBAL_BCM_CD0       = 14,
    RSC_DRV_GLOBAL_BCM           = 15,
    RSC_DRV_GLOBAL_RESERVED,
    RSC_DRV_GLOBAL_MAX,
} RSCSW_DRV_GLOBAL_V1_MAPPING;

#endif // DRV_MAPPING_H

