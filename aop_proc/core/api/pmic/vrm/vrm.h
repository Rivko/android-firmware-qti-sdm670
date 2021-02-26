#ifndef __VRM_H__
#define __VRM_H__

/*! \file vrm.h
*
*  \brief This header file contains VRM driver related definitions.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/pmic/vrm/vrm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "comdef.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_LSHIFT(pos) (0x1 << pos)

/**
 * @brief VRM API error return type
 */
typedef enum 
{
    VRM_SUCCESS               = 0,
    VRM_FAILURE               = VRM_LSHIFT(0),
    VRM_NOT_INITIALIZED       = VRM_LSHIFT(1),
    VRM_NULL_PTR_ERROR        = VRM_LSHIFT(2),
    VRM_INVALID_PARAM         = VRM_LSHIFT(3),
    VRM_OUT_OF_BOUND_ERROR    = VRM_LSHIFT(4),
    VRM_DEP_MAP_ERROR         = VRM_LSHIFT(5),
    VRM_SEQ_NOT_SUPPORTED     = VRM_LSHIFT(6),
    VRM_CMD_DB_ERROR          = VRM_LSHIFT(7),
    VRM_SETTLING_ERROR        = VRM_LSHIFT(8),
    VRM_RINIT_ERROR           = VRM_LSHIFT(9),
    VRM_RESULT_INVALID        = VRM_LSHIFT(30),
}vrm_result;

//******************************************************************************
// Public API Functions
//******************************************************************************
                            
/**
 * @brief Updates ARC DRV mode vote in VRM votetable for the 
 *        given resource.
 * 
 * @param[in] rsrc_name: VRM resource name (used in CmdDB) 
 * 
 * @param[in] mode: Mode value to be updated 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_arc_drv_mode_update(const char* rsrc_name, uint32 mode);

#endif
