/*
   Copyright (c) 2013-2014, 2016 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/vocsvc/src/vocsvc_avtimer.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "msg.h"
#include "err.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"

#ifndef WINSIM
#include "avtimer_drv_api.h"
#endif /* WINSIM */

#include "vocsvc_avtimer_api.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
#define AVTIMER_CLIENT_NAME ( "CVDTIMER" )

#define AVTIMER_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

/****************************************************************************
 * DEFINATIONS                                                              *
 ****************************************************************************/

typedef void* vocsvc_avtimer_drv_handle_t;

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

static vocsvc_avtimer_drv_handle_t avtimer_drv_handle = NULL;
static uint32_t avtimer_handle_ref_count = 0;

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

APR_EXTERNAL int32_t vocsvc_avtimer_open_handle (
  void
)
{
  uint32_t rc = APR_EOK;
  avtimer_open_param_t avtimer_params;

  avtimer_params.flag = 0;
  avtimer_params.client_name = AVTIMER_CLIENT_NAME;

  
  if ( avtimer_handle_ref_count == 0 )
  {
    /* Open the avtimer_drv_handle for the very first request. */
    rc = avtimer_drv_hw_open( &avtimer_drv_handle, &avtimer_params );
    AVTIMER_PANIC_ON_ERROR( rc );
    avtimer_handle_ref_count++;
  }
  else
  {
    /* Increment avtimer_handle_ref_count on subsequent open handle request. */
    avtimer_handle_ref_count++;
  }

#ifndef SIM_DEFINED
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_avtimer_open_handle(): rc = " \
         "0x%d, avtimer_drv_handle = 0x%8X",
         rc, avtimer_drv_handle );
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_avtimer_open_handle(): " \
         "avtimer_handle_ref_count = 0x%d",
         avtimer_handle_ref_count );
#endif /* SIM_DEFINED */

  return rc;
}

APR_EXTERNAL int32_t vocsvc_avtimer_close_handle (
  void
)
{
  uint32_t rc = APR_EOK;
    
  if ( avtimer_handle_ref_count == 1 )
  {
    /* Close the avtimer_drv_handle when the reference count goes zero. */
    rc = avtimer_drv_hw_close( avtimer_drv_handle );
    AVTIMER_PANIC_ON_ERROR( rc );
    avtimer_drv_handle = NULL;
    avtimer_handle_ref_count--;
  }
  else
  {
    /* Decrement avtimer_handle_ref_count on subsequent close handle request. */
    avtimer_handle_ref_count--;
  }

#ifndef SIM_DEFINED    
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_avtimer_close_handle(): rc " \
         "= 0x%d avtimer_handle "
         "reference count = 0x%d", rc, avtimer_handle_ref_count );
#endif /* SIM_DEFINED */

  return rc;
}

APR_EXTERNAL int32_t vocsvc_avtimer_get_time (
  vocsvc_avtimer_timestamp_t* avtimer_timestamp
)
{
  uint64_t avtimer_timestamp_us = 0;

  avtimer_timestamp_us = avtimer_drv_get_time( avtimer_drv_handle );
  avtimer_timestamp->timestamp_us = avtimer_timestamp_us;

#ifndef SIM_DEFINED
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "vocsvc_avtimer_get_time(): " \
         "time_lsw: 0x%08X, time_msw: 0x%08X",
         ((uint32_t)(avtimer_timestamp->timestamp_us)),
         ( ( uint32_t ) ( avtimer_timestamp->timestamp_us >> 32 ) ) );
#endif /* SIM_DEFINED */

  return APR_EOK;
}
