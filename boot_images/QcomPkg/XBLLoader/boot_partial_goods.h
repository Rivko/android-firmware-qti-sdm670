#ifndef BOOT_PARTIAL_GOODS_H
#define BOOT_PARTIAL_GOODS_H

/*===========================================================================

                     Boot Partial Goods Interface

GENERAL DESCRIPTION
 Contains common defintions for functions pertaining to partial goods.

Copyright 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/15   ck      Added boot_pg_retrieve_l3_interleave_status
02/06/15   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================

                           PUBLIC MACROS

===========================================================================*/
/* Settling time in uS that is needed between register writes. */
#define BOOT_SW_PG_DELAY_US                     10000


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
**  Function :  boot_pg_retrieve_cpu_status
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function parses the partial goods cpu fuses to determine what cpus
*   are valid.  Each bit in the return value indicates a single cpu.  LSB
*   equals cpu 0.  High or '1' means the cpu is valid.  Low or '0' means the
*   cpu is not valid.
*
* @par Dependencies
*   None
*
* @retval
*   uint64 64 bit value representing all cpu status
* 
* @par Side Effects
*   None
* 
*/
uint64 boot_pg_retrieve_cpu_status(void);


/*===========================================================================
**  Function :  boot_pg_retrieve_l3_status
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function parses the partial goods l3 interleave fuses to determine
*   what L3 blocks are valid.  Each L3 block consists of two interleaves.
*   If either of the interleaves are valid then the L3 block is considered
*   valid.  Each bit in the return value indicates a single L3 block.  LSB
*   equals L3 block 0.  High or '1' means the block is valid.  Low or '0' means
*   the block is not valid.
*
* @par Dependencies
*   None
*
* @retval
*   uint64 64 bit value representing all L3 block status
* 
* @par Side Effects
*   None
* 
*/
uint64 boot_pg_retrieve_l3_status(void);


/*===========================================================================
**  Function :  boot_pg_retrieve_l3_interleave_status
** ==========================================================================
*/
/*!
* 
* @brief  
*   This function parses the partial goods l3 interleave fuses to determine
*   what L3 interleaves are valid.  Each L3 block consists of two interleaves.
*   Each bit in the return value indicates a single L3 interleave.  LSB
*   equals L3 block 0, interleave 0.  High or '1' means the interleave is
*   valid.  Low or '0' means the interleave is not valid.
*
* @par Dependencies
*   None
*
* @retval
*   uint64 64 bit value representing all L3 interleave status
* 
* @par Side Effects
*   None
* 
*/
uint64 boot_pg_retrieve_l3_interleave_status(void);


#endif /* BOOT_PARTIAL_GOODS_H */
