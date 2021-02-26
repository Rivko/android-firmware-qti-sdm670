#ifndef TZBLSPAC_H
#define TZBLSPAC_H

/*===========================================================================
         Copyright (c) 2015-2017 by QUALCOMM Technologies, Incorporated.  
              All Rights Reserved.
            QUALCOMM Confidential & Proprietary
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE


  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  05/18/17   MJS     API restructure
  04/11/17   MJS     Firmware loading enabled
  12/10/14   sk      Created
  =============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/* Forward declaration */
struct PILSubsys_s;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
int tzbsp_blsp_initaccess();
int tzbsp_ssc_blsp_initaccess();
int tzbsp_sensor_blsp_initaccess();
//int tzbsp_blsp_modify_ownership(uint32 blsp_periph_num, tzbsp_blsp_owner_ss_type owner_ss);
int tzbsp_qupv3_bring_up(struct PILSubsys_s *subsys);

#endif /* TZBLSPAC_H */

