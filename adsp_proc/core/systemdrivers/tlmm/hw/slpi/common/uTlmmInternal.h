#ifndef __UTLMMINTERNAL_H__
#define __UTLMMINTERNAL_H__

/*===============================================================================

                    T O P  L E V E L  M O D E  M U X  (TLMM) 

FILE:         uTlmmInternal.h

DESCRIPTION:  
              Internal definitions for use by the uTlmm driver and the DALTLMM
              driver.

=================================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/tlmm/hw/slpi/common/uTlmmInternal.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------
03/12/15   dcf     Created. 

=================================================================================
             Copyright © 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=================================================================================
*/

#include "comdef.h"


/*==========================================================================

                     LOCAL DATA FOR THIS MODULE

==========================================================================*/

/**
 * uTlmmInitType:  The initialization data structure for the uTlmm driver.
 *
 * Elements:
 *   nTlmmBaseAddr  - The TLMM base address for HW register access.
 *   nTlmmNumGpio   - The Total supported GPIO for uTlmm.
 *   anLowPowerCfgs - An array of low-power GPIO configurations populated by
 *                    the DALTLMM driver.
 */

typedef struct
{
  uint32 nTlmmBaseAddr;
  uint32 nTlmmNumGpio;
  uint32 anLowPowerCfgs[32];
}uTlmmInitType;


#endif /* #ifndef __UTLMMINTERNAL_H__ */

