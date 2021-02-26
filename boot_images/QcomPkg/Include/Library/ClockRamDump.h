/**
==============================================================================
  @file ClockRamDump.c

  Clock functions for the XBL RAM Dump image, which primarily supports a 
  video splash screen.

==============================================================================

                             Edit History


when      who           what, where, why
--------  ------------  ------------------------------------------------------
06/07/16  bc            Initial MSM8998 revision, new file

==============================================================================
            Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"

/*=========================================================================
                       MACRO DEFINITIONS
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

/* FUNCTION: Clock_RD_GetClockID
 *   Input Parameter: char *sClockName -- clock name to return the ID
 *   Return Value : 
 *     Success : A clock ID in the range 0 .. number_of_clocks
 *     Failure : -1
 */
int     Clock_RD_GetClockID( const char *sClockName);

/* FUNCTION: Clock_RD_EnableClock
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=Success, FALSE=Failure or bad parameter
 * 
 * This assumes a standard CBCR (non-votable) with CLK_EN in bit 0 */
boolean Clock_RD_EnableClock( int nClockID );

/* FUNCTION: Clock_RD_DisableClock
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=Success, FALSE=Failure or bad parameter
 * 
 * This assumes a standard CBCR (non-votable) with CLK_EN in bit 0 */
boolean Clock_RD_DisableClock( int nClockID );

/* FUNCTION: Clock_RD_IsClockEnabled
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=clock is enabled, FALSE=clock is disable
 *
 * Note: This only tells you the state of the control.  After you enable a clock, it takes
 * a few cycles for it to turn on which will be indicated by IsClockOn().  If something
 * goes wrong with a clock enable, then it could be Enabled but not On, or Disabled but not Off.
 *
 * This assumes a standard CBCR (non-votable) with CLK_EN in bit 0 */
boolean Clock_RD_IsClockEnabled( int nClockID );

/* FUNCTION: Clock_RD_IsClockOn
 *   Input Parameter: int nClockID -- Use GetClockID() to return this for the clock in question 
 *   Return Value : TRUE=clock is on, FALSE=clock is off
 * 
 * This assumes a standard CBCR (non-votable) with CLK_OFF in bit 31 */
boolean Clock_RD_IsClockOn( int nClockID );

/* FUNCTION: Clock_RD_SetClockFreqKHz
 *   Input Parameter: nClockID -- Use GetClockID() to return this for the clock in question 
 *   Input Parameter: nFreqKHz -- The frequency to use.  If this frequency is either
 *     0 or 19200, CXO will be used.  Otherwise, the one frequency in the table is used.
 *   Return Value : TRUE=Success, FALSE=Failed
 * 
 * This assumes a standard CBCR (non-votable) with CLK_OFF in bit 31 */
boolean Clock_RD_SetClockFreqKHz( int nClockID, uint32 nFreqKHz );

/* FUNCTION: Clock_RD_SetExternalSource
 *   Input Parameter: nClockID -- Use GetClockID() to return this for the clock in question 
 *   Input Parameter: nSource, nDivider, nM, nN, nD : This gives the client the ability 
 *    to choose their external source with a divider, similar to the DDIClock function.
 *
 *   Return Value : TRUE=Success, FALSE=Failed
 */
boolean Clock_RD_SelectExternalSource( int nClockID, uint32 nFreqHz, uint32 nSource, uint32  nDivider,  uint32  nM,  uint32  nN,  uint32  n2D);

/* FUNCTION: Clock_RD_GetClockPowerDomainID
 *   Input Parameter: char *sClockName -- clock domain to return the ID
 *   Return Value : 
 *     Success : A clock ID in the range 0x1000 .. 0x1000+number_of_domains
 *     Failure : -1
 *   The Domain IDs are offset by DOMAIN_OFFSET (0x1000) to keep these distinct
 *   from the clock IDs which start at 0.
 */
int Clock_RD_GetClockPowerDomainID( const char *sClockDomain );

/* FUNCTION: Clock_RD_EnableClockPowerDomain
 *   Input Parameter: int nClockDomainID -- Use GetClockPowerDomainID() to get a valid ID
 *   Return Value : TRUE=Success, FALSE=Failed
 */
boolean Clock_RD_EnableClockPowerDomain( int nClockDomainID );

/* FUNCTION: Clock_RD_DisableClockPowerDomain
 *   Input Parameter: int nClockDomainID -- Use GetClockPowerDomainID() to get a valid ID
 *   Return Value : TRUE=Success, FALSE=Failed
 */
boolean Clock_RD_DisableClockPowerDomain( int nClockDomainID );

