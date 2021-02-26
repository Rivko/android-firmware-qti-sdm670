#ifndef CLOCK_H
#define CLOCK_H

/*
===========================================================================
*/
/**
  @file clock.h
  @brief Public interface include file for accessing the clock driver.

  The clock.h file is the public API interface to the clock driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Clock_API_mainpage.dox file contains all file/group descriptions that 
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      Clock_API_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/systemdrivers/clock/clock.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  01/20/17   vph     Support cap trace resource at fix voltage corner
  08/11/16   vph     Initial version
  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/
#include "comdef.h"
#include "clock_target.h"

/*=========================================================================
      Type Definitions
==========================================================================*/
#define CLOCK_SUCCESS   0
#define CLOCK_ERROR     -1

/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF
/**
 * Clock QDSS levels
 */
typedef enum
{
  CLOCK_QDSS_LEVEL_OFF,         /* clocks set to CXO, and off             */
  CLOCK_QDSS_LEVEL_DYNAMIC,     /* clocks scales dynamic with System HUB  */
  CLOCK_QDSS_LEVEL_TURBO,       /* clocks set to TURBO speed              */
  CLOCK_QDSS_LEVEL_NOMINAL,     /* clocks set to NOMINAL speed            */
  CLOCK_QDSS_LEVEL_SVS_L1,      /* clocks set to SVS_L1 speed             */
  CLOCK_QDSS_LEVEL_SVS,         /* clocks set to SVS speed                */
  CLOCK_QDSS_LEVEL_LOW_SVS,     /* clocks set to LOW_SVS speed            */
  CLOCK_QDSS_LEVEL_MIN_SVS,     /* clocks set to MIN_SVS speed            */
  CLOCK_QDSS_LEVEL_MAX_LEVEL = CLOCK_QDSS_LEVEL_MIN_SVS,
  CLOCK_QDSS_LEVEL_NUM_LEVELS
} ClockQDSSLevelType;

typedef enum
{
  VDD_OFF = 0x0,
  VDD_RET = 0x10,
  MIN_SVS = 0x30,
  LOW_SVS = 0x40,
  SVS = 0x80,
  SVS_L1 = 0xC0,
  NOMINAL = 0x100,
  NOMINAL_L1 = 0x140,
  NOMINAL_L2 = 0x150,
  TURBO = 0x180,
  TURBO_L1 = 0x1A0,
  MAX = TURBO_L1,
  VDD_INVALID = -1,
  VDD_NUM_LEVELS = 11,
  CLOCK_ENUM_32BITS(VDD)
}voltage_rail;

typedef enum
{
  GCC,
  DDRCC,
  NA = 0xFF,
  CLOCK_ENUM_32BITS(MODE)
}clk_mode;

typedef enum
{
  PMIC_CFG_0,
  PMIC_CFG_1,
  PMIC_CFG_2,
  PMIC_CFG_3,
  PMIC_CFG_4,
  PMIC_CFG_5,
  PMIC_CFG_6,
  PMIC_CFG_7,
  PMIC_CFG_8,
  CLOCK_ENUM_32BITS(PMIC)
}ddr_automode;

typedef struct
{
  uint8 clk_idx;
  uint32 freq_khz;
  uint32 clk_period;
  boolean freq_enable;
  clk_mode mode;
  voltage_rail vddcx;
  voltage_rail vddmx;
  voltage_rail vdda;
  ddr_automode ddr_automode_cfg;
  uint8 max_up_idx;
  uint8 min_down_idx;
  boolean double_freq_change;
}freq_state;


/**
 * DDR configure settings data from XBL
 */
typedef struct
{
  uint8 nNumMCStates;
  uint8 nNumSHUBStates;
  freq_state *pMCCfg;
  freq_state *pSHUBCfg;
} DDRCfgType;


/*=========================================================================
      Interface Declarations
==========================================================================*/

/** @addtogroup clock_functions
@{ */

/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/**
  Performs clock initialization in limited environments.

  This function is the normal driver init function for AOP.

  @return
  None

  @dependencies
  Varies based on the image under consideration; usually, there are 
  no dependencies.
*/

void Clock_Init (void);


/* ============================================================================
**  Function : Clock_PostInit
** ============================================================================
*/
/*!
    Clock driver post init API which is used for register QMP driver for QDSS 
    messages.

    @param 
      None
    @return
      None

    @dependencies
      qmp_msg_init - Need to call after qmp_msg_init

    @sa Non
*/
void Clock_PostInit( void );


/* =========================================================================
**  Function : Clock_SwitchToHWCtrlMode
** =========================================================================*/
/**
  Transfer clock power domain and PLLs from SW to HW mode.

  @return
  None

  @dependencies
  None
*/
void Clock_SwitchToHWCtrlMode (void);


/* =========================================================================
**  Function : Clock_SetQUPPerfLevel
** =========================================================================*/
/**
  Configure QUPv3 performance level.

  @return
    CLOCK_SUCCESS - Set configuration success
    CLOCK_ERROR   - Set configuration fails

  @dependencies
  None
*/

int32 Clock_SetQUPPerfLevel( uint32 nlevel );


/* =========================================================================
**  Function : Clock_TargetInit
** =========================================================================*/
/**
  Perform target specific clock configuration
  @param
    None
  @return
    None

  @dependencies
    None
*/

void Clock_TargetInit( void );

/* =========================================================================
**  Function : Clock_QDSSSetPerfLevel
** =========================================================================*/
/**
  Set QDSS performance level.
  @param
    eLevel - QDSS performance level
  @return
    None

  @dependencies
    None
*/

void Clock_QDSSSetPerfLevel( ClockQDSSLevelType eLevel );

/* =========================================================================
**  Function : Clock_ClampGPU
** =========================================================================*/
/**
  This is for SDM845v1 only.  It is a work-around for QDSS.
  This function is  for bringing the GPU out of IO clamp to restore the QDSS timestamp network.

  @param
    bool bEnableClamp: true: when VDD_GFX is turned off to clamp the GPU.
                       false: when VDD_GFX is turned on, this unclamps the GPU IOs. 

  @return
    None

  @dependencies
    None
*/
void Clock_ClampGPU( bool bEnableClamp);

/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/**
  This function is used for disable all necessary AOP PLLs and clocks before
  chip sleep.

  @param
    None
  @return
    None

  @dependencies
    None
*/
void Clock_ProcessorSleep( void );


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/**
  This function is used for restore all necessary AOP PLLs and clocks that
  were disabled during chip sleep.

  @param
    None
  @return
    None

  @dependencies
    None
*/
void Clock_ProcessorRestore( void );

/* =========================================================================
**  Function : Clock_ProcessorReduce
** =========================================================================*/
/**
  This function is used for doing clock reductions before CPU halt

  @param
    boolean enable : true reduces the CPU clock, false restores it to full speed
  @return
    None

  @dependencies
    None
*/
void Clock_ProcessorReduce( boolean enable );

/* =========================================================================
**  Function : Clock_GetBootConfigCP
** =========================================================================*/
/**
  This function is used for getting clock performance level settings in XBL

  @param
    None
  @return
    Pointer to Clock configure settings structure

  @dependencies
    None
*/
ClockCfgCPType* Clock_GetBootConfigCP( void );


/* =========================================================================
**  Function : Clock_GetBootConfigDDR
** =========================================================================*/
/**
  This function is used for getting DDR configure settings that have been
  setting in XBL

  @param
    None
  @return
    Pointer to DDR settings structure

  @dependencies
    None
*/
DDRCfgType* Clock_GetBootConfigDDR( void );

/** @} */ /* end_addtogroup clock_functions */

#endif /* !CLOCK_H */
