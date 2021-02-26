#ifndef SMMUDYNAMICSID_H
#define SMMUDYNAMICSID_H

#include "ACCommon.h"
#include "SMMUTypes.h"
#include "IxErrno.h"


typedef enum
{
  SMMU_INSTANCE_VIDEO      = 0,
  SMMU_INSTANCE_MDP        = 1,
  SMMU_INSTANCE_LPASS_Q6   = 2,
  SMMU_INSTANCE_LPASS_CORE = 6,
  SMMU_INSTANCE_GPU        = 18,
  SMMU_INSTANCE_ROT        = 21,
  SMMU_INSTANCE_VFE        = 22,
  SMMU_INSTANCE_ANOC0      = 23,
  SMMU_INSTANCE_ANOC1      = 24,
  SMMU_INSTANCE_ANOC2      = 25,
  SMMU_INSTANCE_CPP        = 26,
  SMMU_INSTANCE_JPEG       = 27,
  SMMU_INSTANCE_COUNT,
  SMMU_INSTANCE_MAX        = 0x7FFFFFFF,
} SMMU_Instance_e_type;


uint64 SMMU_SetDynamicSID2VMID( SMMU_Instance_e_type esmmu_instance, uint32 *sid_list,
                                uint32 num_entries, ACVirtualMachineId dest_vmid);

int SMMU_TZ_SetDynamicSID2VMID( SMMU_Instance_e_type esmmu_instance, uint32 *sid_list,
                                uint32 num_entries, ACVirtualMachineId dest_vmid);


#endif
