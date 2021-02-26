#ifndef __PMIC_SBL_PON_TARGET_H__
#define __PMIC_SBL_PON_TARGET_H__

/*! \file pm_sbl_pon_target.h
*  \n
*  \brief This file contains PMIC device initialization function and globals declaration.
*  \n
*  \n &copy; Copyright 2016 Qualcomm Technologies Inc, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/16   aab     Added multiple common function used by app level API and target specific functions
03/28/16   aab     Added pm_tgt_get_gp1_cfg()
10/08/15   aab     Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "com_dtypes.h"
#include "pm_config_target.h"
#include "pm_pon.h"



/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
//API used to get pmic specific PS HOLD configuration
pm_err_flag_type 
pm_tgt_get_pshold_reset_cfg( uint32                      pmic_index,
                             pm_app_pon_reset_cfg_type   app_pshold_cfg,
                             pm_pon_reset_cfg_type*      pshold_cfg);


//API used to get  pmic specific PON Reset configuration
pm_err_flag_type
pm_tgt_get_kpdpwr_reset_cfg( uint32                        pmic_index,
                             pm_app_pon_reset_cfg_type     app_kpdpwr_reset_cfg,
                             pm_pon_reset_cfg_type*        kpdpwr_reset_cfg);


//API used to get pmic specific PON S3 timer multiplier
pm_err_flag_type 
pm_tgt_get_pon_s3_reset_timer_multiplier( uint32                        pmic_index, 
                                          pm_app_pon_reset_source_type  app_pon_s3_reset_source, 
                                          uint32*                       pm_s3_reset_timer_multiplier );

//API used to do target specific PSHOLD configuration
pm_err_flag_type
pm_pon_tgt_specific_pshold_cfg(uint32 pmic_index, pm_app_pon_reset_cfg_type pon_cfg);


//API used to do target specific PON Reset configuration 
pm_err_flag_type 
pm_pon_tgt_specific_kpd_reset_cfg(uint32 pmic_index, pm_app_pon_reset_cfg_type app_pon_reset_cfg);


#endif //__PMIC_SBL_PON_TARGET_H__
