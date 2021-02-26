/*===========================================================================
           ual_aud_dev_arbi.cpp

DESCRIPTION: UAL wrapper around audio device arbitration.
For further documentation, refer to
mm-audio/audio-dev-arbi/aud_dev_arbi_client_if.h

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*----------------------------------------------------------------------------
   Include files
----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <dlfcn.h>
#include "usf_log.h"
#include "ual_aud_dev_arbi_defs.h"

/*----------------------------------------------------------------------------
   Macros and constants
----------------------------------------------------------------------------*/
#undef LOG_TAG
#define LOG_TAG  "UAL_AUD_DEV_ARBI"

// Definitions match these in mm-audio/audio-dev-arbi/aud_dev_arbi_client_if.h

typedef void (*aud_dev_arbi_event_cb_t)(audio_devices_t, aud_dev_arbi_event_t,
                                        aud_dev_arbi_event_cb_resp_flags_t*);


typedef int (init_fn_t)(aud_dev_arbi_client_handle_t*,
                        aud_dev_arbi_event_cb_t);
typedef int (deinit_fn_t)(aud_dev_arbi_client_handle_t);
typedef int (register_fn_t)(aud_dev_arbi_client_handle_t,
                            aud_dev_arbi_client_t,
                            audio_devices_t);
typedef int (acquire_fn_t)(aud_dev_arbi_client_handle_t,
                           audio_devices_t,
                           aud_dev_arbi_client_cmd_result_cb_t,
                           uint32_t user_data);
typedef int (release_fn_t)(aud_dev_arbi_client_handle_t,
                           audio_devices_t aud_dev,
                           aud_dev_arbi_client_cmd_result_cb_t,
                           uint32_t user_data);

/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/
static aud_dev_arbi_client_handle_t s_client_handle = NULL;

static void *s_lib_handle = NULL;
static init_fn_t *s_init_fp = NULL;
static deinit_fn_t *s_deinit_fp = NULL;
static register_fn_t *s_register_fp = NULL;
static acquire_fn_t *s_acquire_fp = NULL;
static release_fn_t *s_release_fp = NULL;

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_unload_library
==============================================================================*/
static void ual_aud_dev_arbi_unload_library
(
  void
)
{
  if (s_lib_handle != NULL)
  {
    (void)dlclose(s_lib_handle);
    s_lib_handle = NULL;
  }

  s_init_fp = NULL;
  s_deinit_fp = NULL;
  s_register_fp = NULL;
  s_acquire_fp = NULL;
  s_release_fp = NULL;

}

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_load_library
==============================================================================*/
static void ual_aud_dev_arbi_load_library
(
  void
)
{
  s_lib_handle = dlopen("libaudiodevarb.so", RTLD_NOW);
  if (s_lib_handle != NULL)
  {
    s_init_fp = (init_fn_t*)dlsym(s_lib_handle, "aud_dev_arbi_client_init");
    s_deinit_fp = (deinit_fn_t*)dlsym(s_lib_handle, "aud_dev_arbi_client_deinit");
    s_register_fp = (register_fn_t*)dlsym(s_lib_handle, "aud_dev_arbi_client_register");
    s_acquire_fp = (acquire_fn_t*)dlsym(s_lib_handle, "aud_dev_arbi_client_acquire");
    s_release_fp = (release_fn_t*)dlsym(s_lib_handle, "aud_dev_arbi_client_release");

    if ((s_init_fp == NULL) ||
        (s_deinit_fp == NULL) ||
        (s_register_fp == NULL) ||
        (s_acquire_fp == NULL) ||
        (s_release_fp == NULL))
    {
      ALOGE("%s: error loading symbols from library", __func__);
      ual_aud_dev_arbi_unload_library();
    }
  }
}

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_init
==============================================================================*/
int ual_aud_dev_arbi_init
(
  aud_dev_arbi_event_cb_t evt_cb
)
{
  if (NULL == s_lib_handle)
  {
    ual_aud_dev_arbi_load_library();
  }

  if (s_lib_handle != NULL)
  {
    return s_init_fp(&s_client_handle, evt_cb);
  }
  else
  {
    return 0;
  }
}

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_deinit
==============================================================================*/
int ual_aud_dev_arbi_deinit
(
  void
)
{
  if (s_lib_handle != NULL)
  {
    s_deinit_fp(s_client_handle);
    s_client_handle = NULL;
    ual_aud_dev_arbi_unload_library();
  }

  return 0;
}

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_register
==============================================================================*/
int ual_aud_dev_arbi_register
(
  aud_dev_arbi_client_t client,
  audio_devices_t aud_dev
)
{
  if ((s_lib_handle != NULL)  && (s_client_handle != NULL))
  {
    return s_register_fp(s_client_handle, client, aud_dev);
  }
  else
  {
    return 0;
  }
}

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_acquire
==============================================================================*/
int ual_aud_dev_arbi_acquire
(
  audio_devices_t aud_dev,
  aud_dev_arbi_client_cmd_result_cb_t result_cb,
  uint32_t user_data
)
{
  if ((s_lib_handle != NULL) && (s_client_handle != NULL))
  {
    return s_acquire_fp(s_client_handle, aud_dev, result_cb, user_data);
  }
  else
  {
    return 0;
  }
}

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_release
==============================================================================*/
int ual_aud_dev_arbi_release
(
  audio_devices_t aud_dev,
  aud_dev_arbi_client_cmd_result_cb_t result_cb,
  uint32_t user_data
)
{
  if ((s_lib_handle != NULL) && (s_client_handle != NULL))
  {
    return s_release_fp(s_client_handle, aud_dev, result_cb, user_data);
  }
  else
  {
    return 0;
  }
}
