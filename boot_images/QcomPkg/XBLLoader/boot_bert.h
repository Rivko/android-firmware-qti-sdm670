#ifndef BOOT_BERT_H
#define BOOT_BERT_H

/*===========================================================================

                        Boot BERT Record
                          Header File 

DESCRIPTION
 Contains common defintion used by boot BERT record.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
06/05/15   as      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"


/*===========================================================================

                     PUBLIC TYPE DEFINITIONS

===========================================================================*/

typedef struct
{
  uint64* bert_cookie_addr;
  uint8*  bert_record_addr;
}bert_record_type;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define SBL_BOOT_BERT_COOKIE        0x7A12EF1AD94337B0

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_bert_check

** ==========================================================================
*/
/*!
* 
* @brief
*   This function will check to see if the bert record should be processed.
*  
* @param[in] bl_shared_data Pointer to the shared data 
*
* @par Dependencies
*   Cookies must be present in IRAM.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_bert_check( bl_shared_data_type *bl_shared_data );


#endif /* BOOT_BERT_H */
