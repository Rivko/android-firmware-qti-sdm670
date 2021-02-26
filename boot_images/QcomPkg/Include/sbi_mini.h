#ifndef SBI_MINI_H
#define SBI_MINI_H
/*
===========================================================================

FILE:         sbi_mini.h

DESCRIPTION:  This file contains the definition of the mini SBI interface.
              This interface should only be used from contexts where it is
              impossible to use the full driver, such as when DDR is stalled.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
08/05/11   UNR     Replaced comdef.h with HALcomdef.h for UEFI
04/29/11   MJS     Add forwarding clock API extension.
04/27/11   MJS     Add BT SSBI ID.
06/17/10   MJS     Mini driver API extension for tiny images.
06/15/10   MJS     Add read support.
02/10/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/
#ifndef _ARM_ASM_
#include "HALcomdef.h"
#endif
#include "DalSBIProps.h"

/* SSBI bus IDs */ 
#define SBI_MINI_PMIC_SSBI_ID  0
#define SBI_MINI_PMIC_SSBI_2_ID  1
#define SBI_MINI_BT_SSBI_ID 2

/* Total number of bus IDs */
#define SBI_MINI_NUM_ID          3

/* SSBI modes */
#define SBI_MINI_SSBI_1_0_MODE   0
#define SBI_MINI_SSBI_2_0_MODE   1


#ifndef _ARM_ASM_
/* ===========================================================================

sbi_mini_init()

Description:
  This function initializes the mini SBI driver for the specified bus.

Parameters:
  uBusId  - SBI bus identifier

Return:
  None

======================================================================== */
extern void sbi_mini_init( uint32 uBusId );

/* ===========================================================================

sbi_mini_lock_bus()
   
Description:
  This function locks the SBI bus.  The SBI bus must be locked before
  bus transactions can be performed.

Parameters:
  uBusId  - SBI bus identifier

Return:
  TRUE if successful, FALSE if an error occurred

======================================================================== */
extern boolean sbi_mini_lock_bus( uint32 uBusId );

/*===========================================================================

sbi_mini_unlock_bus()

Description:
  This function unlocks the SBI bus.  The SBI bus must be unlocked after
  bus transactions are performed.

Parameters:
  uBusId  - SBI bus identifier

Return:
  TRUE if successful, FALSE if an error occurred

===========================================================================*/
extern boolean sbi_mini_unlock_bus( uint32 uBusId );

/*===========================================================================

sbi_mini_write_byte()

Description:
  This function writes one byte to the SBI bus.  The write is performed
  using the currently configured SBI transfer mode (e.g. SSBI 1.0 or SSBI 2.0).

Parameters:
  uBusId  - SBI bus identifier
  usAddr  - SBI address to write
  ucData  - Data byte to write

Notes:
  SSBI transfer mode (1.0 vs 2.0) is not changed by this function call

Return:
  TRUE if successful, FALSE if an error occurred

===========================================================================*/
extern boolean sbi_mini_write_byte(uint32 uBusId, uint16 usAddr, uint8 ucData);

/*===========================================================================

sbi_mini_read_byte()

Description:
  This function reads one byte from the SBI bus.  The read is performed
  using the currently configured SBI transfer mode (e.g. SSBI 1.0 or SSBI 2.0).

Parameters:
  uBusId   - SBI bus identifier
  usAddr   - SBI address to read
  pucData  - Pointer to location to store data byte to read.  For the 
             assembly implementation, the read data will also be returned
             in R1.

Notes:
  SSBI transfer mode (1.0 vs 2.0) is not changed by this function call

Return:
  TRUE if successful, FALSE if an error occurred (For the assembly
  implementation, the read data will also be returned in R1.)

===========================================================================*/
extern boolean sbi_mini_read_byte(uint32 uBusId, uint16 usAddr, uint8 *pucData);

/*===========================================================================

sbi_mini_set_transfer_mode()

Description:
  This function sets the SSBI mode and address width.

Parameters:
  uBusId  - SBI bus identifier
  uMode   - SSBI mode (see SBI_MINI_*_MODE)
  uWidth  - SSBI address width (only applies to SSBI 2.0 mode)

Return:
  TRUE if successful, FALSE if an error occurred

===========================================================================*/
extern boolean sbi_mini_set_transfer_mode(uint32 uBusId, uint32 uMode, uint32 uWidth);

/*===========================================================================

sbi_mini_set_sampling_delay()

Description:
  This function sets the SSBI read sampling delay.

Parameters:
  uBusId  - SBI bus identifier
  uDelay  - SSBI samlping delay (see SBI_SAMP_DEL_* defined in DalSBIProps.h)

Return:
  TRUE if successful, FALSE if an error occurred

===========================================================================*/
extern boolean sbi_mini_set_sampling_delay(uint32 uBusId, uint32 uDelay);

/*===========================================================================

sbi_mini_set_fclk_mode()

Description:
  This function sets the SSBI forwarding clock mode, including whether 
  the fowarding clock is enabled and whether hardware clock gating is
  enabled or not.

Parameters:
  uBusId    - SBI bus identifier
  bEnable   - TRUE to enable forwarding clock output, FALSE to disable
  bHwGating - TRUE to enable hardware clock gating of the fowarding clock,
              FALSE to disable

Return:
  TRUE if successful, FALSE if an error occurred

===========================================================================*/
extern boolean sbi_mini_set_fclk_mode(uint32 uBusId, boolean bEnable, boolean bHwGating);

#endif /* _ARM_ASM_ */

#endif /* SBI_MINI_H */

