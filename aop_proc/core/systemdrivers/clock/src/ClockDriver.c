/*
===========================================================================
*/
/**
  @file ClockDriver.c

  Interface implementation file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/clock/src/ClockDriver.c#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $


  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  01/20/17   vph     Support cap trace resource at fix voltage corner
  08/11/16   vph     Initial version
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/
#include "clock.h"
#include "ClockDriver.h"
#include "HALhwio.h"
#include "ClockHWIO.h"
#include "rinit.h"
#include "CoreVerify.h"
#include "json_parser.h"
#include "qmp_msg_driver.h"
#include "target_aop_qdss.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/
#define CLOCK_CMD_CFG_UPDATE_FMSK     HWIO_FMSK(GCC_SYS_NOC_CMD_RCGR, UPDATE)
#define CLOCK_CFG_REG_OFFSET         (HWIO_ADDR(GCC_SYS_NOC_CFG_RCGR)-HWIO_ADDR(GCC_SYS_NOC_CMD_RCGR))
#define CLOCK_CFG_RCGR_SRC_SEL_FMSK   HWIO_FMSK(GCC_SYS_NOC_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_RCGR_SRC_SEL_SHFT   HWIO_SHFT(GCC_SYS_NOC_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_RCGR_SRC_DIV_FMSK   HWIO_FMSK(GCC_SYS_NOC_CFG_RCGR, SRC_DIV)
#define CLOCK_CFG_RCGR_SRC_DIV_SHFT   HWIO_SHFT(GCC_SYS_NOC_CFG_RCGR, SRC_DIV)

/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

#define SIZE 10
/*=========================================================================
      Type Definitions
==========================================================================*/

/* Reference number for tracking PLL enable */
static int32 nPLLNumRef[SRC_TOTAL];
static uint32 nQDSSSubVal[NUM_OF_SUBSYSTEMS];

/*=========================================================================
      Functions
==========================================================================*/
extern boolean Clock_SourceMapToMux(ClockConfigMuxType *pConfig, uint32 *nMuxValue );
extern boolean Clock_SourceMapToBitIndex( uint32 eSource, uint32 *nBitIndex );


/* ============================================================================
**  Function : Clock_QDSSAggrVote
** ============================================================================
*/
/*!
    Clock API for aggregate all the sub-systems vote for QDSS clocks

    @param 
      sub - Sub-system
      val - Sub-system vote value

    @return
      Aggregation to highest vote value of all sub-systems

    @dependencies
      None.

    @sa 
      None
*/
static uint32 Clock_QDSSAggrVote( EDEGES sub, uint32 val )
{
  uint32 nAggrVal = 0;
  uint32 idx;

  nQDSSSubVal[sub] = val;

  for ( idx=0; idx<NUM_OF_SUBSYSTEMS; idx++ )
  {
    if ( nQDSSSubVal[idx] > nAggrVal )
      nAggrVal = nQDSSSubVal[idx];
  }
  return nAggrVal;

}


/* ============================================================================
**  Function : Clock_ProcessMsg
** ============================================================================
*/
/*!
    Clock API for processing QMP (Qualcomm Message Protocol) messages.
    Currently, this API is only processing QDSS QMP messages.

    @param 
      msg - json format message
      sub - Sub-system

    @return
      None

    @dependencies
      None.

    @sa 
      None
*/
static void Clock_ProcessMsg( char* msg, EDEGES sub )
{
  char val[SIZE];
  uint32 nAggrVal, nVal;

  /* QDSS message */
  if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "res", strlen("res"), val, SIZE) )
  {
    if ( strcmp(val, "qdss") == 0 )
    {
      if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "val", strlen("val"), val, SIZE) )
      {
        nVal = atoi(val);
        nAggrVal = Clock_QDSSAggrVote( sub, nVal ); 

        if ( nVal != 0 )
        {
          aop_qdss_add_client( sub );
          Clock_QDSSSetPerfLevel( (ClockQDSSLevelType) nAggrVal );
        }
        else
        {
          Clock_QDSSSetPerfLevel( (ClockQDSSLevelType) nAggrVal );
          aop_qdss_remove_client( sub );
        }
      }
    }
  }
}

/* ============================================================================
**  Function : Clock_Init
** ============================================================================
*/
/*!
    Clock driver initialzed

    @param 
      None
    @return
      None

    @dependencies
    None.

    @sa None
*/
void Clock_Init 
(
  void
)
{
  /* Initialize the clocks DFS */
  int ret_val = rinit_module_init ("clock");
  CORE_VERIFY (ret_val == RINIT_SUCCESS);

  Clock_CopyConfigFromShared();
  Clock_TargetInit();

  /* Clear all sub-system votes */
  memset ( nQDSSSubVal, 0, sizeof(uint32) * NUM_OF_SUBSYSTEMS );

} /* END Clock_Init */


/* ============================================================================
**  Function : Clock_PostInit
** ============================================================================
*/
/*!
    Clock driver post init API which is used for register QMP driver for messages.

    @param 
      None
    @return
      None

    @dependencies
      qmp_msg_init - Need to call after qmp_msg_init

    @sa Non
*/
void Clock_PostInit 
(
  void
)
{
  CORE_VERIFY ( MSG_SUCESS == qmp_msg_register_cb(Clock_ProcessMsg, "clock") );
} /* END Clock_Init */


/* ============================================================================
**  Function : Clock_EnableSource
** ============================================================================
*/
/*!
    Enable a PLL source.  These PLLs are configure and shared with RPMh.  Clock
    driver can only request GPLL_ENA_VOTE for enable PLL

    @param 
      eSource -  [IN] PLL source
    @return
      CLOCK_SUCCESS -- PLL source was enabled successful.
      CLCOK_ERR)R   -- PLL source is not supported.

    @dependencies
    None.

    @sa None
*/
int32 Clock_EnableSource( uint8 eSource )
{
  uint32 nBitIndex;
  uint32 nValue;

  if ( eSource >= SRC_TOTAL )
    return CLOCK_ERROR;

  /* Don't need to enable CXO source */
  if ( eSource == SRC_CXO )
    return CLOCK_SUCCESS;

  /* Map Source to Source Index */
  if ( !Clock_SourceMapToBitIndex((uint32)eSource, &nBitIndex) )
    return CLOCK_ERROR;

  if ( nPLLNumRef[nBitIndex] == 0 )
  {
    nValue = (1 << nBitIndex);
    HWIO_OUTM( GCC_RPM_GPLL_ENA_VOTE, nValue, nValue );
  }

  nPLLNumRef[nBitIndex]++;

  return CLOCK_SUCCESS;
}

/* ============================================================================
**  Function : Clock_DisableSource
** ============================================================================
*/
/*!
    Disable a PLL source.  These PLLs are configure and shared with RPMh.  Clock
    driver can only remove GPLL_ENA_VOTE for disable PLL

    @param 
      eSource -  [IN] PLL source
    @return
      CLOCK_SUCCESS -- PLL source was disabled successful.
      CLCOK_ERR)R   -- PLL source is not supported.

    @dependencies
    None.

    @sa None
*/
int32 Clock_DisableSource( uint8 eSource )
{
  uint32 nBitIndex;

  if ( eSource >= SRC_TOTAL )
    return CLOCK_ERROR;

  /* Don't need to disable CXO source */
  if ( eSource == SRC_CXO )
    return CLOCK_SUCCESS;

  /* Map Source to Source Index */
  if ( !Clock_SourceMapToBitIndex((uint32)eSource, &nBitIndex) )
    return CLOCK_ERROR;

  if ( nPLLNumRef[nBitIndex] > 0 )
    nPLLNumRef[nBitIndex]--;

  if ( nPLLNumRef[nBitIndex] == 0 )
  {
    HWIO_OUTM( GCC_RPM_GPLL_ENA_VOTE, (1 << nBitIndex), 0 );
  }

  return CLOCK_SUCCESS;
}


/* =========================================================================
**  Function : Clock_ConfigMux
** =========================================================================*/
/*!
    Configure a clock mux. 

    @param pConfig -  [IN] Clock mux config structure
    @return
    CLOCK_SUCCESS -- Configuration mux is successful program.
    CLOCK_ERROR   -- Source configuration is not supported.

    @dependencies
    None.

    @sa None
*/
int32 Clock_ConfigMux (ClockConfigMuxType *pConfig)
{
  uintnt nCmdRCGRAddr, nCfgRCGRAddr;
  uint32 nCmdRCGRVal,  nCfgRCGRVal;
  uint32 nSource;
  
  /* Get Source mux index */
  if ( !Clock_SourceMapToMux(pConfig, &nSource ) )
    return CLOCK_ERROR;

  nCmdRCGRAddr = pConfig->nCMDRCGRAddr;
  nCmdRCGRVal  = in_dword(nCmdRCGRAddr);
  nCfgRCGRAddr = pConfig->nCMDRCGRAddr + CLOCK_CFG_REG_OFFSET; 
  nCfgRCGRVal  = in_dword(nCfgRCGRAddr);

  /* Clear the fields. */
  nCfgRCGRVal &= ~(CLOCK_CFG_RCGR_SRC_SEL_FMSK | CLOCK_CFG_RCGR_SRC_DIV_FMSK);

  /* Program the source and divider values. */
  nCfgRCGRVal |= (nSource << CLOCK_CFG_RCGR_SRC_SEL_SHFT)
                  & CLOCK_CFG_RCGR_SRC_SEL_FMSK;
  nCfgRCGRVal |= ((HALF_DIVIDER(pConfig) << CLOCK_CFG_RCGR_SRC_DIV_SHFT)
                  & CLOCK_CFG_RCGR_SRC_DIV_FMSK);

  /* Write the final CFG register value. */
  out_dword(nCfgRCGRAddr, nCfgRCGRVal);

  /* Trigger the update. */
  nCmdRCGRVal |= CLOCK_CMD_CFG_UPDATE_FMSK;
  out_dword(nCmdRCGRAddr, nCmdRCGRVal);

  /* Wait until update finishes. */
  while ( in_dword(nCmdRCGRAddr) & CLOCK_CMD_CFG_UPDATE_FMSK );

  return CLOCK_SUCCESS;
}

