#ifndef QDSS_HWEVENT_CONFIG_H
#define QDSS_HWEVENT_CONFIG_H

/*=============================================================================

FILE:         qdss_hwevent_config.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#define SSC_HWEVENTS_MUXSEL_BEGIN 0x05F02028
#define SSC_HWEVENTS_MUXSEL_END   0x05F02104

static __inline void qdss_hwe_setreg_remap(uint32 *paddr, uint32 *pvalue) 
{
   if ((*paddr >= SSC_HWEVENTS_MUXSEL_BEGIN) &&
       (*paddr <= SSC_HWEVENTS_MUXSEL_END)) {
      *pvalue |= (1<<31);
   }

   //Remapping needed because MUX SEL registers are not
   //contiguous and tool does not have a way to handle it.
   if (0x05F020A0 == *paddr) {
      *paddr = 0x05F02100;
   }

   else if (0x05F020A4 == *paddr) {
      *paddr = 0x05F02104;
   }
}

#endif //QDSS_HWEVENT_CONFIG_H
