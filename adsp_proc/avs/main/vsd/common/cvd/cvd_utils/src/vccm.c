/*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/vccm.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include "msg.h"
#include "err.h"

#ifndef WINSIM
  #include "mmpm.h"
#endif /* !WINSIM */

#include "cvd_devcfg_parser_i.h"
#include "vccm_api.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define VCCM_NUM_RSCS_PER_MMPM_REQUEST_V ( 2 )
#define VCCM_NUM_OF_CLIENTS_V ( 3 )
#define VCCM_CLIENT_NAME_SIZE ( 16 )
#define VCCM_MMPM_CLIENT_ID_NONE ( 0xFFFFFFFF )

#define VCCM_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

#if ( ADSPPM_INTEGRATION == 1 )
static char_t vccm_clients[VCCM_NUM_OF_CLIENTS_V][VCCM_CLIENT_NAME_SIZE] =
                { "VCCM_CVS",
                  "VCCM_CVP",
                  "VCCM_MVM"
                };
static uint32_t vccm_mmpm_client_id[VCCM_NUM_OF_CLIENTS_V] =
                  {
                    VCCM_MMPM_CLIENT_ID_NONE,
                    VCCM_MMPM_CLIENT_ID_NONE,
                    VCCM_MMPM_CLIENT_ID_NONE 
                  };
#endif

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/


APR_INTERNAL uint32_t vccm_mmpm_register(
  uint32_t client_id 
)
{
  uint32_t rc = APR_EOK;

#if ( ADSPPM_INTEGRATION == 1 )

  MmpmRegParamType register_param;

  if ( VCCM_MMPM_CLIENT_ID_NONE == vccm_mmpm_client_id[ client_id ] )
  {
    register_param.rev = MMPM_REVISION;
    register_param.coreId = MMPM_CORE_ID_LPASS_ADSP;
    register_param.instanceId = MMPM_CORE_INSTANCE_0;
    register_param.pClientName = vccm_clients[client_id];
    register_param.pwrCtrlFlag = PWR_CTRL_NONE;
    register_param.callBackFlag = CALLBACK_NONE;
    register_param.MMPM_Callback = NULL;
    register_param.cbFcnStackSize = 0;

    vccm_mmpm_client_id[ client_id ] = MMPM_Register_Ext( &register_param );
    if ( vccm_mmpm_client_id[ client_id ] == 0 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vccm_mmpm_register(): Failed to register %s with MMPM.",
             vccm_clients[client_id] );
      vccm_mmpm_client_id[ client_id ] = VCCM_MMPM_CLIENT_ID_NONE;
      rc = APR_EFAILED;
      VCCM_PANIC_ON_ERROR( rc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vccm_mmpm_register(): Successfully registered %s to MMPM."
             "vccm_mmpm_client_id = (0x%08X)", vccm_clients[client_id], 
             vccm_mmpm_client_id[ client_id ] );
    }
  }
#endif

  return rc;
}

APR_INTERNAL uint32_t vccm_mmpm_deregister(
  uint32_t client_id
)
{
  uint32_t rc = APR_EOK;

#if ( ADSPPM_INTEGRATION == 1 )

  MMPM_STATUS mmpm_rc;

  if( VCCM_MMPM_CLIENT_ID_NONE != vccm_mmpm_client_id[ client_id ] )
  {
    mmpm_rc = MMPM_Deregister_Ext( vccm_mmpm_client_id[ client_id ] );
    if ( mmpm_rc != MMPM_STATUS_SUCCESS )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vccm_mmpm_deregister(): Failed to de-register from MMPM for "
             "client ID %d, mmpm_rc = %d ",
             vccm_mmpm_client_id[ client_id ], mmpm_rc );
      rc = APR_EFAILED;
      VCCM_PANIC_ON_ERROR( rc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vccm_mmpm_deregister(): Successfully de-registered %s from "
             "MMPM, vccm_mmpm_client_id = (0x%08X)",
             vccm_clients[client_id], vccm_mmpm_client_id[ client_id ] );
      vccm_mmpm_client_id[ client_id ] = VCCM_MMPM_CLIENT_ID_NONE;
    }
  }
#endif

  return rc;
}

APR_INTERNAL uint32_t vccm_mmpm_request_clocks (
  uint32_t client_id,
  uint32_t mpps_req
)
{
  uint32_t rc = APR_EOK;

  #if ( ADSPPM_INTEGRATION == 1 )
  
    MmpmMppsReqType mpps;
    MmpmGenBwReqType bw_array;
    MmpmGenBwValType bw;
  
    MMPM_STATUS mmpm_global_rc;
    MMPM_STATUS mmpm_individual_rc[ VCCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
    MmpmRscParamType rsc_param[ VCCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
    MmpmRscExtParamType request_param;

   /* Query the Q6 core clock from CVD_DEVCFG. */
    if ( VCCM_CLIENT_ID_MVM == client_id )
    {
      ( void ) cvd_devcfg_parser_get_max_q6_core_clock( &mpps.adspFloorClock );
       mpps.mppsTotal = mpps_req;
    }
    else
    {
      ( void ) cvd_devcfg_parser_get_min_q6_core_clock( &mpps.adspFloorClock );
       mpps.mppsTotal = mpps_req;
    }

  /* Query the bus clock from CVD_DEVCFG. */
  ( void ) cvd_devcfg_parser_get_max_bw( &bw.bwValue.busBwValue.bwBytePerSec );
    bw.busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    bw.busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
    bw.bwValue.busBwValue.usagePercentage = 100;
    bw.bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;

    /* Update the bw request params array. */
    bw_array.numOfBw = 1;
    bw_array.pBandWidthArray = &bw;

    rsc_param[ 0 ].rscId = MMPM_RSC_ID_MPPS;
    rsc_param[ 0 ].rscParam.pMppsReq = &mpps;

    rsc_param[ 1 ].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;
    rsc_param[ 1 ].rscParam.pGenBwReq = &bw_array;

    request_param.apiType = MMPM_API_TYPE_SYNC;
    request_param.numOfReq = VCCM_NUM_RSCS_PER_MMPM_REQUEST_V;
    request_param.pReqArray = rsc_param;
    request_param.pStsArray = mmpm_individual_rc;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vccm_mmpm_request_clocks(): client ID: %d, MPSS: %d, ADSP Floor clock: %dMHz",
            vccm_mmpm_client_id[ client_id ], mpps.mppsTotal, mpps.adspFloorClock );

    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vccm_mmpm_request_clocks(): BW req master port: %d, Slave port: %d" \
           "Bytes per sec: %d, Usage percentage: %d, Usage type: %d", 
           bw.busRoute.masterPort, bw.busRoute.slavePort, bw.bwValue.busBwValue.bwBytePerSec,
           bw.bwValue.busBwValue.usagePercentage, bw.bwValue.busBwValue.usageType );

    mmpm_global_rc = MMPM_Request_Ext( vccm_mmpm_client_id[ client_id ],
                                       &request_param );
    if ( mmpm_global_rc != MMPM_STATUS_SUCCESS )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vccm_mmpm_request_clocks(): MMPM request failed. MIPS req " 
             "rc = %d, BW req rc = %d", mmpm_individual_rc[0],
             mmpm_individual_rc[1] );

      rc = APR_EFAILED;
      VCCM_PANIC_ON_ERROR( rc );
    }
  #endif

  return rc;
}

APR_INTERNAL uint32_t vccm_mmpm_release_clocks ( 
  uint32_t client_id
)
{
  uint32_t rc = APR_EOK;

#if ( ADSPPM_INTEGRATION == 1 )

  MMPM_STATUS mmpm_global_rc;
  MMPM_STATUS mmpm_individual_rc[ VCCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
  MmpmRscParamType rsc_param[ VCCM_NUM_RSCS_PER_MMPM_REQUEST_V ];
  MmpmRscExtParamType release_param;

  /* MMPM release only requires resource IDs to be indicated, not the resource
   * values such as MIPS number. All the resources under the resource IDs will
   * be released. */
  rsc_param[ 0 ].rscId =  MMPM_RSC_ID_MPPS;
  rsc_param[ 1 ].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;

  release_param.apiType = MMPM_API_TYPE_SYNC;
  release_param.numOfReq = VCCM_NUM_RSCS_PER_MMPM_REQUEST_V;
  release_param.pReqArray = rsc_param;
  release_param.pStsArray = mmpm_individual_rc;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vccm_mmpm_release_clocks(): Releasing MMPM resources for "
         "client ID = %d", vccm_mmpm_client_id[ client_id ] );

  mmpm_global_rc = MMPM_Release_Ext( vccm_mmpm_client_id[ client_id ],
                                     &release_param );
  if ( mmpm_global_rc != MMPM_STATUS_SUCCESS )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vccm_mmpm_release_clocks(): MMPM release failed. MIPS release " 
           "rc = %d, BW release rc = %d", mmpm_individual_rc[0],
           mmpm_individual_rc[1] );

    rc = APR_EFAILED;
    VCCM_PANIC_ON_ERROR( rc );
  }
#endif

  return rc;
}

