#ifndef CLOCKDRIVER_H
#define CLOCKDRIVER_H
/*
===========================================================================
*/
/**
  @file ClockDriver.h

  Internal header file for the Clock Driver structures and APIs.
*/
/*
===========================================================================

  Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  10/07/16   vph     Initial code
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

/*=========================================================================
      References
==========================================================================*/

/*=========================================================================
      Definitions
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/
/*
 * ClockSourceType
 *
 * A type to be used for clock sources in the configuration data and functions.
 */

typedef enum {
  SRC_CXO = 0,
  SRC_GPLL0,
  SRC_GPLL0_EVEN,
  SRC_GPLL1,
  SRC_GPLL2,
  SRC_GPLL3,
  SRC_GPLL4,
  SRC_GPLL5,
  SRC_GPLL6,
  SRC_AOPLL0,
  SRC_AOPLL0_EVEN,
  SRC_AOPLL1,
  SRC_TOTAL
} ClockSourceType;

/*
 * ClockConfigMuxType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  nCMDCGRAddr - The address of the CMD RCGR register.
 *  eSource     - The source to use.
 *  nDiv2x      - The integer (2 * divider) value to use.
 */
typedef struct 
{
  uint32 nCMDRCGRAddr;
  uint8 eSource;
  uint8 nDiv2x;
} ClockConfigMuxType;


/* ============================================================================
**  Function : Clock_CopyConfigFromShared
** ============================================================================
*/
/*!
    Copy shared boot configure setting in SMEM for used in AOP

    @param 
      None

    @return
      None

    @dependencies
      None.

    @sa 
      None
*/
void Clock_CopyConfigFromShared( void );

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
int32 Clock_EnableSource( uint8 eSource );

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
int32 Clock_DisableSource( uint8 eSource );

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
int32 Clock_ConfigMux ( ClockConfigMuxType* pMux );

#endif /* !CLOCKDRIVER_H */
