/**
@file AdspCoreSvc_DynamicLoader.h
@brief This file declares the APIs for the Dynamic Loader module.

The dynamic loader module spawns a thread to load the Audio and
Voice shared objects. This module should be run at startup.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/19/13    DG       Created file.
==========================================================================*/

/*------------------------------------------------------------------------
   Copyright (c) 2013, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.
--------------------------------------------------------------------------*/

#ifndef ADSP_CORE_SVC_DYNAMIC_LOADER_H
#define ADSP_CORE_SVC_DYNAMIC_LOADER_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

typedef struct _adsp_dynamic_loader_t adsp_dynamic_loader_t;

/**
  Creates the dynamic loader structure.

  @param[in] qurt_elite_channel_t Pointer to the channel that will be notified when
                          the thread is about to exit.

  @return Pointer to the dynamic loader. NULL if there was an error.

  @dependencies
  None.
*/
adsp_dynamic_loader_t *adsp_dynamic_loader_create(qurt_elite_channel_t *thread_exit_notification_channel);

/**
  Destroys the dynamic loader structure.

  @param[in] obj_ptr Pointer to the structure to be destroyed.

  @return
  None.

  @dependencies
  None.
*/
void adsp_dynamic_loader_destroy(adsp_dynamic_loader_t *obj_ptr);

/**
  Gets the signal mask for the thread exit notification signal.

  @param[in] obj_ptr Pointer to the dynamic loader structure.

  @return
  None.

  @dependencies
  None.
*/
uint32_t adsp_dynamic_loader_get_signal_mask(adsp_dynamic_loader_t *obj_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ADSP_CORE_SVC_DYNAMIC_LOADER_H

