#ifndef SMMUDYNAMICSID_H
#define SMMUDYNAMICSID_H


#include "SMMUTypes.h"
#include "IxErrno.h"


typedef enum
{
  SMMU_INSTANCE_APPS	   = 0,
  SMMU_INSTANCE_GPU 	   = 1,
  SMMU_INSTANCE_COUNT	   = 2,
  SMMU_INSTANCE_MAX        = 0x7FFFFFFF,
} SMMU_Instance_e_type;

// syscall stubbed for 24 ** Cleam this up and add in chipset/24 later on.
int smmu_tz_dyn_gpu_aper_reg_syscall(uint32_t reg_cb_pair_1, uint32_t reg_cb_pair_2, 
                                      uint32_t reg_cb_pair_3, uint32_t reg_cb_pair_4);

#endif
