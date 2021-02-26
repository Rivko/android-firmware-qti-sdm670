#ifndef __VRM_XOB_H__
#define __VRM_XOB_H__

/*! \file vrm_xob.h
*
*  \brief This header file contains internal VRM XO buffer related definitions.
*  \n
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/xob/vrm_xob.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_types.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Internal API Functions
//******************************************************************************
                            
/**
 * @brief Assigns resource id for requested VRM XOB resource.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] rsrc_names rsrc names struct contains the rsrc 
 *                       name, rsrc alias name if any and rsrc
 *                       parent name if any.
 * 
 * @param[in] num_children Number of children if rsrc has any.
 * 
 * @param[out] rsrc_id assigned rsrc id is returned.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_xob_assign_rsrc_id(vrm_info* vrm_ptr, vrm_rsrc_names* rsrc_names, uint32 num_children, uint32* rsrc_id);

#endif
