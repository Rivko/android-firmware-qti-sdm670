/******************************************************************************
* File: dsp_cache.S
*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
******************************************************************************/

.global sns_llcm_cache_invalidate
.type   sns_llcm_cache_invalidate, @function
   sns_llcm_cache_invalidate:
   r3 = lsr(r1,#5)
   loop0(1f,r3)
   .falign
1:
   {
      dcinva(r0)
      r0 = add(r0,#32)
   }:endloop0
   jumpr r31
.size sns_llcm_cache_invalidate, .-sns_llcm_cache_invalidate


.global sns_llcm_cache_clean
.type   sns_llcm_cache_clean, @function
   sns_llcm_cache_clean:
   {
      p0 = cmp.eq(r1,#0)
      if (p0.new) jump:nt 2f
      r3 = lsr(r1,#5)
   }
   loop0(1f,r3)
   .falign
1:
   {
      dccleana(r0)
      r0 = add(r0,#32)
   }:endloop0
2:
   syncht
   jumpr r31
.size sns_llcm_cache_clean, .-sns_llcm_cache_clean
