#ifndef __HALCLKINTERNAL_H__
#define __HALCLKINTERNAL_H__
/*
==============================================================================

FILE:         HALclkInternal.h

DESCRIPTION:
  Internal definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/Library/HALclkInternal.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/18/14   jb      Add parent clock source
09/11/12   ll      Added PLL Type (SR, SR2) field to PLL Context typedef
07/18/11   vs      Ported from the latest clock driver.

==============================================================================
      Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <HALclk.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/

/*
 * ENCODE_NOT_2D / ENCODE_NOT_N_MINUS_M
 *
 * Macros to return the inverted value of the 2D field or (N - M)
 * in a type 1 mux structure.  Used to prepare the value for writing
 * to the hardware register field.
 */
#define ENCODE_NOT_2D(mux)         (~(mux)->nN)
#define ENCODE_NOT_N_MINUS_M(mux)  (~((mux)->nN - (mux)->nM))

/*
 * DECODE_NOT_2D / DECODE_NOT_N_MINUS_M
 *
 * Macros to  unwind the inverted value of the 2D field or (N - M)
 * read from a type 1 mux register.  Used to find a match for a mux
 * config with a BSP entry.
 */
#define DECODE_NOT_2D(nNot2DVal)                     (~(nNot2DVal))
#define DECODE_NOT_N_MINUS_M(nNotNMinusMVal, nMVal)  (~(nNotNMinusMVal) + nMVal)

/*
 * DIVIDER
 *
 * Macro to return the normalized divider for a given mux structure.
 * Just subtracts 1 unless the value is already 0.
 */
#define DIVIDER(mux)  ((mux)->nDivider ? (mux)->nDivider - 1 : 0)

/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

/*
 * HAL_CLK_FMSK
 *
 * Define a register mask data structure given the register name
 * and field.
 */
#define HAL_CLK_FMSK(io, field)   \
  {                               \
    HWIO_ADDR(io),                \
    HWIO_FMSK(io, field)          \
  }


/*
 * HAL_CLK_FMSKI
 *
 * Define a register mask data structure given the index, register name
 * and field.
 */
#define HAL_CLK_FMSKI(io,index,field)   \
  {                               \
    HWIO_ADDR(io,index),         \
    HWIO_FMSK(io, field)          \
  }


/*
 * HAL_CLK_FMSKM
 *
 * Define a register mask data structure given the register name
 * and a mask.
 */
#define HAL_CLK_FMSKM(io, mask)   \
  {                               \
    HWIO_ADDR(io),                \
    mask                          \
  }


/*
 * Define an invalid source index for error detection.
 */
#define HAL_CLK_SOURCE_INDEX_INVALID 0xFFFFFFFF


/* ============================================================================
**    Typedefs
** ==========================================================================*/

/*
 * HAL_clk_RegisterMaskType
 *
 * Contains a register address and mask, used for setting and clearing
 * a given set of bits.
 *
 * nAddr - Address of the register (32-bit).
 * nMask - 32-bit mask.
 */
typedef struct
{
  uintnt nAddr;
  uint32 nMask;
} HAL_clk_RegisterMaskType;


/*
 * HAL_clk_ClockRegistersType
 *
 * nBranchAddr  - Clock branch address/offset.
 * nResetAddr   - Clock block reset address/offset.
 * mHWVotable   - Register/mask for HW votable configuration setup.
 */
typedef struct
{
  uintnt                    nBranchAddr;
  uintnt                    nResetAddr;
  HAL_clk_RegisterMaskType  mHWVotable;
} HAL_clk_ClockRegistersType;


/*
 * HAL_clk_ClockControlType
 *
 * Set of function pointers used to control a clock.
 *
 * Enable           - Enable the given clock.
 * Disable          - Disable the given clock.
 * IsEnabled        - Check if the given clock is enabled.
 * IsOn             - Check if the given clock is on.
 * Reset            - Assert or deassert the reset for given clock group.
 * Config           - Configure the given clock.
 * DetectConfig     - Return the current configuration for the clock.
 * ConfigDivider    - Set the divider for the clock in question.
 * DetectDivider    - Return the current divider for the clock in question.
 * ConfigFootswitch - Configure the clock footswitch.
 * Invert           - Inverts the clock signal.
 * ResetBranch      - Assert or deassert the reset for given clock branch.
 */
typedef struct
{
  void    (*Enable)           ( HAL_clk_ClockDescType  *pmClockDesc );
  void    (*Disable)          ( HAL_clk_ClockDescType  *pmClockDesc );
  boolean (*IsEnabled)        ( HAL_clk_ClockDescType  *pmClockDesc );
  boolean (*IsOn)             ( HAL_clk_ClockDescType  *pmClockDesc );
  void    (*Reset)            ( HAL_clk_ClockDescType  *pmClockDesc, boolean bAssert );
  boolean (*IsReset)          ( HAL_clk_ClockDescType  *pmClockDesc );
  void    (*Config)           ( HAL_clk_ClockDescType  *pmClockDesc, HAL_clk_ClockConfigType eConfig );
  void    (*DetectConfig)     ( HAL_clk_ClockDescType  *pmClockDesc, uint32 *pnConfig, boolean *pbValid );
  void    (*ConfigDivider)    ( HAL_clk_ClockDescType  *pmClockDesc, uint32 nDiv );
  void    (*DetectDivider)    ( HAL_clk_ClockDescType  *pmClockDesc, uint32 *pnDiv );
  void    (*ConfigFootswitch) ( HAL_clk_ClockDescType  *pmClockDesc, HAL_clk_FootswitchConfigType eConfig );
  void    (*Invert)           ( HAL_clk_ClockDescType  *pmClockDesc, boolean bInvert );
  void    (*ResetBranch)      ( HAL_clk_ClockDescType  *pmClockDesc, boolean bAssert );
} HAL_clk_ClockControlType;


/*
 * HAL_clk_SourceControlType
 *
 * Set of function pointers used to control a clock source.
 *
 * Enable          - Enable the given source.
 * Disable         - Disable the given source.
 * IsEnabled       - Check if the given source is enabled.
 * Config          - Configure the given source.
 * ConfigPLL       - Configure the given PLL source parameters.
 * DetectPLLConfig - Get the current configuration for the source.
 * EnableVote      - Enable vote the given source.
 * DisableVote     - Disable vote the given source.
 * IsVoteEnabled   - Check if the given source vote is enabled.
 */
typedef struct
{
  boolean (*Enable)          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
  void    (*Disable)         ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
  boolean (*IsEnabled)       ( HAL_clk_SourceDescType *pSource );
  void    (*Config)          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
  boolean (*ConfigPLL)       ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode );
  void    (*DetectPLLConfig) ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
  boolean (*EnableVote)      ( HAL_clk_SourceDescType *pSource );
  void    (*DisableVote)     ( HAL_clk_SourceDescType *pSource );
  boolean (*IsVoteEnabled)   ( HAL_clk_SourceDescType *pSource );
  boolean (*IsPLLConfigEqual)( const HAL_clk_PLLConfigType  *pConfigA, const HAL_clk_PLLConfigType  *pConfigB);
} HAL_clk_SourceControlType;


/*
 * HAL_clk_ClockDomainControlType
 *
 * ConfigMux         - Configure the given clock mux.
 * DetectMuxConfig   - Return the current configuration of the mux for the clock.
 * IsOn              - Returns whether the RCG domain is on or not.
 * RootEnable        - Assert/deassert the given clock mux CGR_ROOT_EN bit.
 */
typedef struct
{
  void    (*ConfigMux)          ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, const HAL_clk_ClockMuxConfigType *pmConfig );
  void    (*DetectMuxConfig)    ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig );
  boolean (*IsOn)               ( HAL_clk_ClockDomainDescType *pmClkDomainDesc );
  void    (*RootEnable)         ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, boolean bAssert );
  void    (*EnableDFS)          ( HAL_clk_ClockDomainDescType *pmClkDomainDesc );
  boolean (*IsDFSEnabled)       ( HAL_clk_ClockDomainDescType *pmClkDomainDesc );
  void    (*ConfigDFSMux)       ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig );
  void    (*DetectDFSMuxConfig) ( HAL_clk_ClockDomainDescType *pmClkDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig, uint32 nPerfLevel );
} HAL_clk_ClockDomainControlType;


/*
 * HAL_clk_PowerDomainControlType
 *
 * Enable     - Enable power domain.
 * Disable    - Disable power domain.
 * IsEnabled  - Check whether power domain is enabled.
 * IsOn       - Check whether power domain is on.
 * EnableHW   - Enable HW Control of the power domain.
 * DisableHW  - Revert back to SW Control of the power domain.
 */
typedef struct
{
  void    (*Enable)     ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  void    (*Disable)    ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  boolean (*IsEnabled)  ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  boolean (*IsOn)       ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  void    (*EnableHW)   ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
  void    (*DisableHW)  ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
} HAL_clk_PowerDomainControlType;


/*
 * HAL_clk_DividerControlType
 *
 * ConfigDivider - Configure the divide value.
 * DetectDivider - Detect the divide value.
 */
typedef struct
{
  void    (*ConfigDivider)  ( HAL_clk_DividerDescType *pmDividerDesc, uint32 nDiv );
  void    (*DetectDivider)  ( HAL_clk_DividerDescType *pmDividerDesc, uint32 *pnDiv );
} HAL_clk_DividerControlType;


/*
 * HAL_clk_DebugMuxControlType
 *
 * ConfigMux  - Configure the debug mux.
 * CalcFreq   - Calculate frequency of the configured mux input.
 */
typedef struct
{
  void    (*ConfigMux)      ( HAL_clk_DebugMuxDescType *pmDebugMux, uint32 nMuxSel );
  void    (*CalcFreq)       ( HAL_clk_DebugMuxDescType *pmDebugMux, uint32 *pnFreqHz );
} HAL_clk_DebugMuxControlType;


/*
 * HAL_clk_ClockDescType
 *
 * Description of a clock, including required data to control the
 * clock.
 *
 * mRegisters  - (optional) Set of registers used by the
 *               common clock control functions (HAL_clk_ClockControl*).
 * pmControl   - Pointer to the control functions for this clock.  If NULL
 *               then the clock is not supported.
 * nDebugMuxSel - Input select value for this clock on its debug mux.
 *
 */
struct HAL_clk_ClockDescType
{
  HAL_clk_ClockRegistersType  mRegisters;
  HAL_clk_ClockControlType   *pmControl;
  uint32                      nDebugMuxSel;
};


/*
 * HAL_clk_ClockDomainDescType
 *
 * Description of a clock domain, including required data to control the
 * clock domain.
 *
 * nCGRAddr          - (First) Clock generation register address/offset.
 * pmControl         - Pointer to the control functions for this domain.
 * nMNDWidth         - Width of MND counter.
 * nDFSStates        - Max number of DFS states.
 */
struct HAL_clk_ClockDomainDescType
{
  uintnt                           nCGRAddr;
  HAL_clk_ClockDomainControlType  *pmControl;
  uint8                            nMNDWidth;
  uint8                            nDFSStates;
};


/*
 * HAL_clk_SourceDescType
 *
 * Description of a clock source, including control functions.
 *
 * nAddr          - Source base address.
 * VoterRegister  - Register/mask for HW votable configuration setup.
 * pmControl      - Pointer to the control functions for this source.  If NULL
 *                  then the source is not supported.
 */
struct HAL_clk_SourceDescType
{
  uintnt                       nAddr;
  HAL_clk_RegisterMaskType     VoterRegister;
  HAL_clk_SourceControlType   *pmControl;
};


/*
 * HAL_clk_PowerDomainDescType
 *
 * Description of a power domain, including control functions.
 *
 * nGDSCRAddr           - Global distributed switch controller (GDSC) register address.
 * pmControl            - Pointer to the control functions for this power domain.  If
 *                        NULL then the power domain is not supported.
 * pmClockMemCore       - Pointer to the optional list of clocks used to control MEM CORE
 *                        sleep behavior.
 * pmClockMemPeriph     - Pointer to the optional list of clocks used to control MEM PERIPH
 *                        sleep behavior.
 */
struct HAL_clk_PowerDomainDescType
{
  uintnt                           nGDSCRAddr;
  HAL_clk_PowerDomainControlType  *pmControl;
  HAL_clk_ClockDescType          **pmClockMemCore;
  HAL_clk_ClockDescType          **pmClockMemPeriph;
};


/*
 * HAL_clk_DividerDescType
 *
 * Description of a clock divider, including control functions.
 *
 * nAddr      - Clock divider base address.
 * pmControl  - Pointer to the control functions for this power domain. If
 *              NULL then the power domain is not supported.
 */
struct HAL_clk_DividerDescType
{
  uintnt                      nAddr;
  HAL_clk_DividerControlType *pmControl;
};


/*
 * HAL_clk_DebugMuxDescType
 *
 * Description of a debug mux, including required data to control the
 * clock.
 *
 * pmControl   - Pointer to the control functions for this debug mux. If NULL
 *               then the debug mux is not supported.
 */
struct HAL_clk_DebugMuxDescType
{
  HAL_clk_DebugMuxControlType *pmControl;
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


boolean HAL_clk_WaitForClockOn   ( HAL_clk_ClockDescType  *pmClockDesc );
boolean HAL_clk_WaitForClockOff  ( HAL_clk_ClockDescType  *pmClockDesc );
void    HAL_clk_BusyWait         ( uint32 uSec );

#endif /* !HALCLKINTERNAL_H */

