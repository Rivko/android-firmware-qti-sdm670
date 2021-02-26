#ifndef __CLOCKBOOT_H__
#define __CLOCKBOOT_H__

/** @file ClockBoot.h

  This file contains the external interface definitions for the clock regime device driver.
  This driver is support for the Krait Second Primary Boot Loader.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdrivers/ClockBoot.h#1 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  02/08/17  vphan   Support QUP V3 clock set frequency
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

=============================================================================*/

/*=========================================================================
      Includes
==========================================================================*/

#include "com_dtypes.h"
#include "railway.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF

/**
@brief List of boot speeds that the clock driver supports.

MIN - XO frequency perf level.
NOMINAL - Highest frequency at nominal voltage level.
MAX - Max frequency support by the frequency plan.
DEFAULT - Same as NOMINAL.

*/
typedef enum
{
  CLOCK_BOOT_PERF_NONE,
  CLOCK_BOOT_PERF_NOMINAL,
  CLOCK_BOOT_PERF_TURBO,
  CLOCK_BOOT_PERF_DEFAULT,
  CLOCK_BOOT_PERF_NUM,
  CLOCK_ENUM_32BITS(BOOT_PERF)
} ClockBootPerfLevelType;

/**
@brief List of SDC clocks that the clock driver supports.
*/
typedef enum
{
   CLK_SDC0,
   CLK_SDC1,
   CLK_SDC2,
   CLK_SDC3,
   CLK_SDC4,
   CLK_SDC5,
   CLK_SDC_NUM_CLKS,
   CLK_SDC_NONE = CLK_SDC_NUM_CLKS,
} ClockSDCType;

/**
@brief List of QUPv3 clocks that the clock driver supports.
*/
typedef enum
{
   CLK_QUPV3_NONE,
   CLK_QUPV3_WRAP0_S0,
   CLK_QUPV3_WRAP0_S1,
   CLK_QUPV3_WRAP0_S2,
   CLK_QUPV3_WRAP0_S3,
   CLK_QUPV3_WRAP0_S4,
   CLK_QUPV3_WRAP0_S5,
   CLK_QUPV3_WRAP0_S6,
   CLK_QUPV3_WRAP0_S7,
   CLK_QUPV3_WRAP1_S0,
   CLK_QUPV3_WRAP1_S1,
   CLK_QUPV3_WRAP1_S2,
   CLK_QUPV3_WRAP1_S3,
   CLK_QUPV3_WRAP1_S4,
   CLK_QUPV3_WRAP1_S5,
   CLK_QUPV3_WRAP1_S6,
   CLK_QUPV3_WRAP1_S7,
   CLK_QUPV3_NUM_CLKS
} ClockQUPV3Type;

/*
 * ClockUSB30PipeClkSrcType
 *
 * Select USB30 PIPE Clk Source Type
 */
typedef enum
{
  CLOCK_USB30_SRC_USB3_PHY = 0,
  CLOCK_USB30_SRC_CORE_BI_PLL_TEST_SE,
  CLOCK_USB30_SRC_CXO,
  CLOCK_USB30_SRC_TCK,
  CLOCK_USB30_SRC_INVALID
} ClockUSB30PipeClkSrcType;

/*
 * BootLogClockType
 *
 * List of clocks whose frequency can be queried via external interface.
 */
typedef enum
{
   CLK_BOOT_LOG_APPS_CLUSTER_0,
   CLK_BOOT_LOG_DDR,
   CLK_BOOT_LOG_NUM_CLOCKS,
   CLOCK_ENUM_32BITS(CLK_BOOT_LOG_TYPE)
} ClockBootLogQueryType;

typedef struct
{
  uint32 pll_dec_start;
  uint32 pll_div_frac_start3;
  uint32 pll_plllock_cmp1;
  uint32 pll_plllock_cmp2;
  uint32 pll_vco_count1;
  uint32 pll_vco_count2;
  uint32 pll_pll_lpf2_postdiv;
  uint32 pll_kvco_code;
} ClockDSFConfigType;


/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.
*/
boolean Clock_PreDDRInit( uint32 ddr_type );


/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.
*/
boolean Clock_PreDDRInitEx( uint32 ddr_type );

/* ============================================================================
**  Function : Clock_Init
** ============================================================================
*/
/*!

    This function turns on the required clocks and configures
    Fabric and Krait speeds depending on the System Fabric and
    CPU boot performance level.

    @param eSysPerfLevel   -  [in] Fabric and DDR performance level to initialize.
    @param eCPUPerfLevel   -  [in] Scropion CPU performance level to initialize.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/

boolean Clock_Init(ClockBootPerfLevelType eSysPerfLevel,
                    ClockBootPerfLevelType eCPUPerfLevel);

/* ============================================================================
**  Function : Clock_SetSysPerfLevel
** ============================================================================
*/
/**
  Configure System and Application Fabric to a perf level.

  @param eSysPerfLevel [in]  -  Fabric and DDR performance level to configure.

  @return
  TRUE -- Fabric was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetSysPerfLevel(ClockBootPerfLevelType eSysPerfLevel);

/* ============================================================================
**  Function : Clock_SetCPUPerfLevel
** ============================================================================
*/
/**
  Configure Krait CPU to a specific perf level.

  @param eCPUPerfLevel [in] - CPU performance level.

  @return
  TRUE -- CPU was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetCPUPerfLevel(ClockBootPerfLevelType eCPUPerfLevel);

/* ============================================================================
**  Function : Clock_EnableL2
** ============================================================================
*/
/*!
    Configure L2 cache to a specific perf level.

    @param eL2PerfLevel   -  [IN] CPU performance level

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetL2PerfLevel(ClockBootPerfLevelType eL2PerfLevel);

/* ============================================================================
**  Function : Clock_SetSDCClockFrequency
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param eClockPerfLevel   -  [IN] Clock frequency level
           eClock            -  [IN] SDC clock to configure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetSDCClockFrequency(ClockBootPerfLevelType ePerfLevel,
                                    ClockSDCType eClock);


/* ============================================================================
**  Function : Clock_SetSDCClockFrequencyExt
** ============================================================================
*/
/*!
    Configure SDC clock to a specific perf level.

    @param nFreqKHz          -  [IN] Frequency in KHz
           eClock            -  [IN] SDC clock to configure
    @return
        freq - Returns the frequency which may be rounded up if the
          requested frequency was not in the clock plan.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_SetSDCClockFrequencyExt( uint32 nFreqKHz, ClockSDCType eClock);


/* ============================================================================
**  Function : Clock_SetQUPV3Frequency
** ============================================================================
*/
/*!
    Set QUP V3 clock frequency.

    @param 
      eClock   - QUPV3 index 
      nFreqKHz   Clock frequency in KHz

    @return
    TRUE -- Set frequency success
    FALSE -- Set frequency failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetQUPV3Frequency(ClockQUPV3Type eClock, uint32 nFreqKHz);


/* ============================================================================
**  Function : Clock_DisableQUPV3Clock
** ============================================================================
*/
/*!
    Disable QUP V3 clock.

    @param 
      eClock   - QUPV3 index 

    @return
      TRUE -- Disable success
      FALSE -- Disable failed.

    @dependencies
      None.

    @sa None
*/
boolean Clock_DisableQUPV3Clock(ClockQUPV3Type eClock);


/* ============================================================================
**  Function : Clock_InitForDownloadMode
** ============================================================================
*/
/*!
    This will initialize clocks for the RAMs in the system that are to be saved
    during crash dump procedures in the downloader.

    @param
      None
    @return
      None

    @dependencies
    None.

    @sa None
*/

void Clock_InitForDownloadMode(void);

/* ============================================================================
**  Function : Clock_InitUSB
** ============================================================================
*/
/*!
    Configure USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_InitUSB(void);


/* ============================================================================
**  Function : Clock_InitUSB30
** ============================================================================
*/
/*!
    Configure USB3.0 clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_InitUSB30(void);


/* ============================================================================
**  Function : Clock_Usb30EnableSWCollapse
** ============================================================================
*/
/*!
    Enable SW Collapse for USB30

    @param Enable/Disable.
    @return
    TRUE always

    @dependencies
    None.

    @sa None
   */
boolean Clock_Usb30EnableSWCollapse(boolean enable);

/* ============================================================================
**  Function : Clock_Usb30GetSWCollapse
** ============================================================================
*/
/*!
    Returns the status of SW Collapse for USB30

    @param None
    @return
    TRUE if enabled
    FALSE if disabled

    @dependencies
    None.

    @sa None
*/
boolean Clock_Usb30GetSWCollapse(void);

/* ============================================================================
**  Function : Clock_ExitBoot
** ============================================================================
*/
/*!

    This function turns off clocks that were used during boot, but are not
    needed after boot.  This should be called at the end of boot.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    Call this last in the boot sequence.

*/
boolean Clock_ExitBoot(void);

/* ============================================================================
**  Function : Clock_DebugInit
** ============================================================================
*/
/*!

    This function is called very early in boot.  It is used for work-arounds that
    need to be done before JTAG attaches at the SBL "hold focus button" spin loop.

    @return
    TRUE -- Clean up was successful.
    FALSE -- Clean up failed.

   @dependencies
    You cannot use busywait in this function.

*/
boolean Clock_DebugInit(void);

/* ============================================================================
**  Function : Clock_DisableUSB
** ============================================================================
*/
/*!
    Disable USB clocks.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisableUSB(void);

/* ============================================================================
**  Function : Clock_USB30_PipeClkSrcSel
** ============================================================================
*/
/*!
    Select USB3 PIPE clock source.

    @param eSrc -  [IN] Clock source choice for the USB3.0 PHY
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.
    @dependencies  None.
    @sa None
*/
boolean Clock_USB30_PipeClkSrcSel(ClockUSB30PipeClkSrcType eSrc);

/* ============================================================================
**  Function : Clock_DDRSpeed
** ============================================================================
*/
/*!
    Return the DDR clock rate in kilohertz.  This clock rate is the bus speed.
    It is not the controller clock (2X) clock for targets that use Legacy mode.

    @param None.
    @return
      The speed configured in Clock_PreDDRInit() of the BIMC/DDR clock.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_DDRSpeed(void);

/* ============================================================================
**  Function : Clock_GetClockFrequency
** ============================================================================
*/
/*!
    Get the requested clock frequency in hertz.

    @param None.
    @return
      True iff the requested clock frequency is placed in pnFrequencyHz.

    @dependencies
    None.

    @sa None
*/
boolean Clock_GetClockFrequency(ClockBootLogQueryType eBootLogClock,
                                uint32 *pnFrequencyHz);

/* ========================================================================
**  Function : Clock_I2CInit
** ======================================================================*/
/*
    Description: Configure all clocks needed for EEPROM to be used Pre DDR.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_I2CInit(void);

/* ========================================================================
**  Function : Clock_EnableQPICForDownloadMode
** ======================================================================*/
/*
    Description: Enable QPIC clocks.
                 The function is used in Download Mode where QPIC clocks are
                 not enabeld by PBL.

    @param None
    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnableQPICForDownloadMode( void );


/* ========================================================================
**  Function : Clock_SetBIMCSpeed
** ======================================================================*/
/*
    Description: API for setting DDR frequency

    @param nFreqKHz - DDR frequency in KHz

    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetBIMCSpeed(uint32 nFreqKHz );


/* ========================================================================
**  Function : Clock_UFSInit
** ======================================================================*/
/*
    Description: Enable UFS clocks, and set it to certain performance levels

    @param ePerfLevel
    @return TRUE on success.

    @dependencies
    None.

    @sa None
*/
boolean Clock_UFSInit( ClockBootPerfLevelType ePerfLevel );


/* ============================================================================
**  Function : Clock_InitRPM
** ============================================================================
*/
/*!

    This function does any initialization before RPM launches.  SMEM is now
    usable.

    @return
    TRUE -- Success
    FALSE -- Failure

   @dependencies
    Requires SMEM

*/
boolean Clock_InitRPM( void );


/* ============================================================================
**  Function : Clock_InitVSense
** ============================================================================
*/
/*!
    This function enables the Voltage Sensor clocks, and configure them run
    at max frequency level at NOMINAL voltage.

    @return
      TRUE -- Initialization was successful.
      FALSE -- Initialization failed.

   @dependencies
     None.

*/
boolean Clock_InitVSense( void );

/* ============================================================================
**  Function : Clock_ShutdownVSense
** ============================================================================
*/
/*!
    This function disables the voltage sensor clocks.

    @return
      TRUE -- Initialization was successful.
      FALSE -- Initialization failed.

   @dependencies
      None.

*/
boolean Clock_ShutdownVSense( void );

/* ============================================================================
**  Function : Clock_DDRPrepareChange
** ============================================================================
*/
/*!
    This function prepares for a DDR clock switch.  It increases voltage and locks
    the unused PLL to prepare for the switch.

    @param nMDDRChannel - Choose a channel (0-5) to prepare the clock switch.
    @param nFreqKHz - A frequency from the clock plan (as retrieved from Clock_DDRQuery())

    @return TRUE on success.

   @dependencies
      None.

*/
boolean Clock_DDRPrepareChange
(
  uint32 nMDDRChannel,
  uint32 nFreqKHz
);

/* ============================================================================
**  Function : Clock_DDRApplyChange
** ============================================================================
*/
/*!
    This function actually switches the clock switch that was started by
      Clock_DDRPrepareChange().

    @param nMDDRChannel - Choose a channel (0-5) to apply the clock switch.

    @return TRUE on success.

   @dependencies
      None.

*/
boolean Clock_DDRApplyChange
(
 uint32 nMDDRChannel
);

/* ============================================================================
**  Function : Clock_DDRCleanUpChange
** ============================================================================
*/
/*!
    This function completes the clock switch.  It applies a drop in voltage if
    needed and turns off the unused PLL.

    @param nMDDRChannel - Choose a channel (0-5) to apply the clock switch.

    @return TRUE on success.

   @dependencies
      None.

*/
boolean Clock_DDRCleanUpChange
(
 uint32 nMDDRChannel
);

/* ============================================================================
**  Function : Clock_LMhInit
** ============================================================================
*/
/*  Initialize for the current limiter calibration.
 *  Power on A57 at Turbo Voltage(A57 operation and stability is not a requirement)
    L2 headswitch needs to be closed as well.

    @param nA57uV  -  [IN] requested voltage in microvolts
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_LMhInit
(
  uint32 nA57uV
);

/* ============================================================================
**  Function : Clock_LMhPreCalibration
** ============================================================================
*/
/*  Set the A57 cores at the maximum clock rate.

    @param  nFreqKHz : The frequency to set in KHz
    @return : rounded freqeuncy that was set, or 0 on failure in kHz.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_LMhPreCalibration
(
  uint32 nFreqKHz
);

/* ============================================================================
**  Function : Clock_LMhDeInit
** ============================================================================
*/
/*  Disable the PLL, turn of the L2 head switch, power off the rail.

    @param  None.
    @return
    TRUE -- Success.
    FALSE -- Reserved, not used.

    @dependencies
    None.

    @sa None
*/
boolean Clock_LMhDeInit
(
  void
);

#endif  /* __CLOCKBOOT_H__ */
