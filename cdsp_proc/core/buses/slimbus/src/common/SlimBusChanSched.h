#ifndef SLIMBUSCHANSCHED_H
#define SLIMBUSCHANSCHED_H
/*
===========================================================================

FILE:         SlimBusChanSched.h

DESCRIPTION:  This file contains the SLIMbus core driver  
              function prototypes for channel scheduling.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusChanSched.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/30/13   MJS     Support for spcifying minimum clock gear for scheduling.
12/11/11   MJS     Remove references to DAL.
01/21/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011, 2013 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/


/**
 * @brief Outer loop for performing data channel bandwidth 
 *        allocation
 *
 * This function implements the outer loop for performing data 
 * channel bandwidth allocation 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[out] puSubFrmCode  Location to store the scheduled 
 *       subframe code
 * @param[in,out] puClkGear  Location to receive the minimum
 *       clock gear and store the scheduled clock gear
 * 
 * @return SB_SUCCESS if bandwidth allocation was performed 
 *         sucessfully, or and error code if there an allocation
 *         error 
 */
extern SBResult SlimBus_DoBandwidthAlloc
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 *puSubFrmCode,
  uint32 *puClkGear
);

#endif /* SLIMBUSCHANSCHED_H */

