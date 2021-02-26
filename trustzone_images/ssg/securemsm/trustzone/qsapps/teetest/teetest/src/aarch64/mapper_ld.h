/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
#ifndef _TEETEST64_MAPPER_LD_H_
#define _TEETEST64_MAPPER_LD_H_

#define TZ_VAS_LAST_ADDRESS VAS_LAST_ADDRESS

int mmu_map
(
  mmu_ctx_t* const ctx,
  v_addr_t v_addr,
  p_addr_t p_addr,
  size_t   sz_in_kb,
  tz_cfg_t msk
);

#endif /* _TEETEST64_MAPPER_LD_H_ */
