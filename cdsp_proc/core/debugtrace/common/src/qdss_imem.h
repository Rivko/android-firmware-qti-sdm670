#ifndef QDSS_IMEM_H
#define QDSS_IMEM_H

/*=============================================================================

FILE:         qdss_imem.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "DALSysTypes.h"
#include "HALhwio.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*
qdss imem base is at an offset from the chip imem base, 
which is also typically non-zero, So 0 is always is invalid
*/
#define QDSS_INVALID_IMEM_ADDR  0

#define QDSS_SYSTEST_MAGIC       0x51440000 /*Ascii 'QD' is used as the magic*/
#define QDSS_SYSTEST_MAGIC_MASK  0xFFFF0000 /*mask for testing/setting magic*/

#define QDSS_SYSTEST_RPM_INIT       0x1     /* Initialize QDSS in RPM */
#define QDSS_SYSTEST_EVENTS_ENABLE  0x2     /* Enable systest events */

/*---------------------------------------------------------------------------
 * Functions
 * ------------------------------------------------------------------------*/
/**
  @brief Return the base address for QDSS in the IMEM address space

  The address returned is the virtual address where it is needed.

  @return If a valid address cannot be obtained for this chip, this funtion
          retuns QDSS_INVALID_IMEM_ADDR(0).
 */

uint32 qdss_imem_base(void);



/**
@brief Get system initialization time test control word

If there is a valid system initialization test control word, return it
Otherwise return 0

@return See above. A return value of 0 means we did not have a valid
       systest_ctl word with the magic
*/

static __inline  uint32 qdss_get_systest_ctl(uint32 systest_ctl_addr) 
{
   if (QDSS_INVALID_IMEM_ADDR != systest_ctl_addr ) {
      uint32 systest_ctl=__inpdw(systest_ctl_addr);
      if (QDSS_SYSTEST_MAGIC == (QDSS_SYSTEST_MAGIC_MASK & systest_ctl)) {
         return systest_ctl;
      }
   }
   return 0;
}



#endif //QDSS_IMEM_H
