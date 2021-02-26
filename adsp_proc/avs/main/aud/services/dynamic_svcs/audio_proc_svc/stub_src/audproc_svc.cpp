/*==========================================================================
ELite Source File

This file implement Audio Post Processing Dynamic Service logic.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------

=========================================================================== */
/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "AudDynaPPSvc.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

ADSPResult AudPP_AddStaticTopologies(void);
ADSPResult AudPP_AddStaticTopologiesAndModules(void);
ADSPResult aud_add_static_topos_and_mods_to_cmn_db(void);
ADSPResult aud_add_static_topos_to_cmn_db(void);

#ifdef __cplusplus
}
#endif //__cplusplus

ADSPResult AudPP_CreateSvc (const AudPPSvcInitParams_t *pParams, void **handle, void **far_handle)
{
   return ADSP_EOK;
}

ADSPResult AudPP_AddStaticTopologies(void)
{
   return ADSP_EOK;
}

ADSPResult AudPP_AddStaticTopologiesAndModules(void)
{
   return ADSP_EOK;
}

ADSPResult aud_add_static_topos_and_mods_to_cmn_db(void)
{
   return ADSP_EOK;
}

ADSPResult aud_add_static_topos_to_cmn_db(void)
{
   return ADSP_EOK;
}