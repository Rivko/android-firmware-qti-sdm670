/**
@file module_mgr.h
@brief This is the API file for module_mgr

Module Manager has following responsibilities
- it contains list of static and dynamic modules.
- at boot-up it reads the acdb file
-            it loads modules by using acdb file and the list of static and dynamic modules.

- It also exposes APIs for reading modules from memory blob sent in (DE)REGISTER_MODULES cmd.

- it interacts with AMDB.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/module_mgmt/module_mgr/inc/module_mgr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/12/2015 rbhatnk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/

#ifndef _MODULE_MGR_H
#define _MODULE_MGR_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "mmdefs.h"
#include "adsp_error_codes.h"


ADSPResult mm_register_all_static_modules();
ADSPResult mm_register_modules_based_on_acdb();
ADSPResult mm_register_modules_from_reg_api(void *addr, uint32_t size, bool_t is_out_of_band, uint32_t client_id);
ADSPResult mm_remove_modules_from_dereg_api(void *addr, uint32_t size, bool_t is_out_of_band, uint32_t client_id);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_MODULE_MGR_H
