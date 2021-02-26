/*
   DESCRIPTION
   This file contains the defintions of cvd diag utility

   Copyright (C) 2016 , 2018 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_diag.c#2 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "msg.h"
#include "err.h"

/* VSD APIs. */
#include "cvd_diag_api.h"
#include "cvd_diag_i.h"
#include "drv_api.h"


/*****************************************************************************
 * GLOBALS                                                              *
 ****************************************************************************/

static char_t cvd_diag_my_dns[] = "qcom.audio.cvddiag";
static uint16_t cvd_diag_my_addr;
  /**< CVD DIAG address is set at initialization. */

static char_t cvd_diag_mvm_dns[] = "qcom.audio.mvm";
static uint16_t cvd_diag_mvm_addr;

/* APR Resource */
static uint32_t cvd_diag_apr_handle;

/* function to be implemented by VDS */
extern VOID_DIAG_PACKED_PTR vds_diag_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16_t pkt_len
);

static const diagpkt_user_table_entry_type cvd_diag_tbl[] =
{
  { CVD_DIAG_COMMAND, CVD_DIAG_COMMAND, cvd_diag_cmd_processing },
   /**< call-back function to be called for CVD commands.*/
  { VDS_DIAG_COMMAND, VDS_DIAG_COMMAND, vds_diag_cmd_processing },
   /**< call-back function to be called for VDS commands.*/
};

/****************************************************************************
 * INTERNAL ROUTINES                                                 *
 ****************************************************************************/


VOID_DIAG_PACKED_PTR cvd_diag_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16_t pkt_len
)
{
  int32_t rc = APR_EOK;
  cvd_diag_rsp_t* rsp_pkt = NULL;
  cvd_diag_cmd_req_t* diag_req_pkt = ( cvd_diag_cmd_req_t* ) req_pkt ;
  uint8_t cmd = CVD_DIAG_ITEST_CMD_INITIAL_VALUE ;
  if( pkt_len == (sizeof( diagpkt_subsys_header_type ) + sizeof(uint8_t)) )
  {
    cmd = diag_req_pkt->cmd;
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "cvd_diag_cmd_processing(): Packet length of diag request packet is invalid");
  }
  vss_icommon_evt_toggle_csm_t param;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "cvd_diag_cmd_processing(): Command:(%d)", cmd );

  switch ( cmd ) 
  {
    case CVD_DIAG_ITEST_CMD_ENABLE_CSM:
    {
      param.status = TRUE;
      rc = __aprv2_cmd_alloc_send(
             cvd_diag_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
             cvd_diag_my_addr, APR_NULL_V,
             cvd_diag_mvm_addr, APR_NULL_V,
             NULL, VSS_ICOMMON_EVT_TOGGLE_CSM, 
             &param, sizeof( vss_icommon_evt_toggle_csm_t ) );

      CVD_DIAG_PANIC_ON_ERROR( rc );
    }
    break;
      
    case CVD_DIAG_ITEST_CMD_DISABLE_CSM:
    {
      param.status = FALSE;
      rc = __aprv2_cmd_alloc_send(
             cvd_diag_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
             cvd_diag_my_addr, APR_NULL_V,
             cvd_diag_mvm_addr, APR_NULL_V,
             NULL, VSS_ICOMMON_EVT_TOGGLE_CSM,
             &param, sizeof( vss_icommon_evt_toggle_csm_t ) );

      CVD_DIAG_PANIC_ON_ERROR( rc );
    }
    break;

  case CVD_DIAG_ITEST_CMD_INITIAL_VALUE:
  default:
      rc = APR_EUNSUPPORTED;
      break;
  }

  rsp_pkt = ( cvd_diag_rsp_t* )diagpkt_subsys_alloc( DIAG_SUBSYS_CVD,
                                 CVD_DIAG_COMMAND, sizeof( cvd_diag_rsp_t ) );
  if ( rsp_pkt == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "cvd_diag_cmd_processing(): Diag Packet could not be allocated");
    return NULL;
  }

  rsp_pkt->result = rc;
  diagpkt_commit( ( void* )rsp_pkt );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "cvd_diag_cmd_processing(): Rsp(0x%08x)",  rsp_pkt->result );

  return rsp_pkt;
}

static int32_t cvd_diag_register_with_diag ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "cvd_diag_register_with_diag: Registering to DIAG_SUBSYS_CVD" );
  DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_CVD, cvd_diag_tbl );

  return APR_EOK;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/
static int32_t cvd_diag_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  ( void ) __aprv2_cmd_free( cvd_diag_apr_handle, packet );
  return APR_EOK;
}

static int32_t cvd_diag_init ( void )
{
  int32_t rc;
  rc = __aprv2_cmd_register2(
           &cvd_diag_apr_handle, cvd_diag_my_dns, sizeof( cvd_diag_my_dns ), 0,
           cvd_diag_isr_dispatch_fn, NULL, &cvd_diag_my_addr );

  ( void ) __aprv2_cmd_local_dns_lookup(
               cvd_diag_mvm_dns, sizeof( cvd_diag_mvm_dns ), &cvd_diag_mvm_addr );

  rc = cvd_diag_register_with_diag( );
  return rc;
}

static int32_t cvd_diag_deinit ( void )
{ 
   /* Release the APR resource. */
    ( void ) __aprv2_cmd_deregister( cvd_diag_apr_handle );
    return APR_EOK;
}

APR_EXTERNAL int32_t cvd_diag_call (
  uint32_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
    case DRV_CMDID_INIT:
      rc = cvd_diag_init( );
      break;

    case DRV_CMDID_DEINIT:
      rc = cvd_diag_deinit();
      break;

    default:
      rc = APR_EUNSUPPORTED;
      break;
  }
  return rc;
}
