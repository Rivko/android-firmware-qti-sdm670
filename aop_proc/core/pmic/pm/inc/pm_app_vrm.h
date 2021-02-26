#ifndef __PM_APP_VRM_H__
#define __PM_APP_VRM_H__

/*! \file pm_app_vrm.h
 *  \n
 *  \brief This header file contains application level PMIC VRM resource related apis and definitions. 
 *  \n  
 *  \n &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/inc/pm_app_vrm.h#1 $
 
when        who     what, where, why
--------    ---     ----------------------------------------------------------
11/23/16    kt       Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/**
 * @brief PMIC VRM API used peripheral types 
 *       (might make more sense to make this part of VRM API headers. Revisit this)
 */
typedef enum 
{
    PM_VRM_PERIPH_NONE,
    PM_VRM_PERIPH_SMPS,
    PM_VRM_PERIPH_LDO,
    PM_VRM_PERIPH_BOB,
    PM_VRM_PERIPH_VS,
    PM_VRM_PERIPH_CLK,
    PM_VRM_PERIPH_PBS_CLIENT,
    PM_VRM_PERIPH_GPIO,
    PM_VRM_PERIPH_SOC,  // dummy periph just for VRM SOC resource
    PM_VRM_PERIPH_SPMI, // dummy periph just for work-around
    PM_VRM_PERIPH_VREF, 
    PM_VRM_PERIPH_XO,
    PM_VRM_PERIPH_VREG,
    PM_VRM_PERIPH_INVALID,
}pm_vrm_periph_type;


/**
*  @brief PMIC VRM API used peripheral info struct.
* 
*  pmic_index PMIC index (starts from 0)
* 
*  periph_type PMIC peripheral type (refer to pm_vrm_periph_type
*  enum above)
* 
*  periph_index PMIC peripheral index (e.g. For SMPS1, index is
*  0)
 */
typedef struct
{
    uint32 pmic_index;
    pm_vrm_periph_type periph_type;
    uint32 periph_index;
}pm_vrm_periph_info;


/**
*  @brief PMIC VRM pm_app_vrm_pbs_wait_seq_done API used struct
*         to return the PBS sequnece completion/done status.
* 
*  pmic_index PMIC index for which the ret value is updated
*  ret_val PBS seq return value after sequence is done
 */
typedef struct
{
    uint32 pmic_index;
    uint8 ret_val;
}pm_vrm_pbs_seq_status;

/*===========================================================================

                        API

===========================================================================*/
/** @addtogroup pm_app_vrm
@{ */

/**
* An app level API to get the VRM VREG resource settling status. This 
* API settles for the given estimated time and returns VREG_READY or 
* VREG_OK status for power rails. 
*  
* @param[in] periph_info PMIC peripheral info (refer to pm_vrm_periph_info struct above)
* @param[in] estimated_time_us estimated settling time for the resource.
* @param[out] vreg_status VREG READY/OK status after est settling time
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type 
pm_app_vrm_vreg_settle_status(pm_vrm_periph_info *periph_info, uint32 estimated_time_us, boolean *vreg_status);

/**
* An app level API to clear and return the VRM VREG resource OCP
* status.
*  
* @param[in] periph_info PMIC peripheral info (refer to pm_vrm_periph_info struct above)
* @param[out] ocp_status VREG OCP status
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type 
pm_app_vrm_vreg_ocp_status_clear(pm_vrm_periph_info *periph_info, boolean *ocp_status);

/**
* An app level API to trigger the PBS software trigger sequence 
* and wait for PBS software sequence completion and return the 
* sequence done status. 
*  
* @param[in] periph_info PMIC peripheral info (refer to pm_vrm_periph_info struct above)
* @param[in] trig_arg PMIC PBS software trigger argument to 
*                     indicate sequence in execution
* @param[out] seq_status PMIC PBS software sequence status
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type 
pm_app_vrm_pbs_sw_trigger(pm_vrm_periph_info *periph_info, uint8 trig_arg, pm_vrm_pbs_seq_status *seq_status);

/** @} */ /* end_addtogroup pm_app_vrm */
#endif /* __PM_APP_VRM_H__ */
