#ifndef BARE_METAL_INFO_H
#define BARE_METAL_INFO_H

/*! \file bare_metal_info.h
 *
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===================================================================
			        EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/pmic_devices/common/src/bare_metal_info.h#1 $ 
$Author: pwbldsvc $ 

when		who		what, where, why
--------	---		------------------------------------------------

====================================================================*/
/*===================================================================
                        INCLUDE FILES
====================================================================*/
#include "pm.h"   


/*===================================================================
                        DEFINITIONS
====================================================================*/


typedef unsigned short pm_register_address_type ; 
typedef unsigned char  pm_register_data_type ;
typedef unsigned char  pm_register_mask_type ;
typedef unsigned short pm_register_16_bit_data_type ; 
typedef unsigned short pm_register_16_bit_mask_type ; 


typedef struct RegisterInfoType
{
    pm_register_address_type    addr ;
    pm_register_mask_type       mask;
    pm_register_data_type       data;
} RegisterInfoType ;
#endif // BARE_METAL_INFO_H
