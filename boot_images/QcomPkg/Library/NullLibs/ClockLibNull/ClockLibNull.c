/** @file ClockLibNull.c
  
  Stub functions for ClockLib

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/03/16   kpa     Added HAL_clk_SourceType, Clock_InitUSB
 05/06/15   plc     Add new clock stubs
 05/30/14   kpa     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ClockBoot.h"
//#include "Drivers/ClockDxe/ClockDriver.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#include "Library/HALclkSharedLib/HALavs.h"
#ifdef CLOCK_ULOG_STUB
  #include "ClockULogStub.h"
#else
  #include "ULogFront.h"
#endif
#include "ClockDefs.h"
//#include "drivers/ClockDxe/ClockBSP.h"
#include "HALclk.h"

#ifdef CLOCK_NPA_STUB
#include "ClockNPAStub.h"
#else
  #include "npa.h"
  #include "npa_resource.h"
#endif


/*==========================================================================

                                  DEFINES

===========================================================================*/
typedef enum
{
  HAL_CLK_SOURCE_NULL             = 0,

  /*
   * XO
   */
  HAL_CLK_SOURCE_XO               = 1,

  /*
   * GCC PLL
   */
  HAL_CLK_SOURCE_GPLL0,
  HAL_CLK_SOURCE_GPLL1,
  HAL_CLK_SOURCE_GPLL2,
  HAL_CLK_SOURCE_GPLL3,
  HAL_CLK_SOURCE_GPLL4,
  HAL_CLK_SOURCE_GPLL5,

  /*
   * MMSS PLL
   */
  HAL_CLK_SOURCE_MMPLL0,
  HAL_CLK_SOURCE_MMPLL1,
  HAL_CLK_SOURCE_MMPLL2,
  HAL_CLK_SOURCE_MMPLL3,
  HAL_CLK_SOURCE_MMPLL4,
  HAL_CLK_SOURCE_MMPLL5,
  HAL_CLK_SOURCE_MMPLL10,

  /*
   * External source
   */
  HAL_CLK_SOURCE_EXTERNAL,
  HAL_CLK_NUM_OF_EXTERNAL_SOURCES,

  /*
   * Raw
   */
  HAL_CLK_SOURCE_RAW0,
  HAL_CLK_SOURCE_RAW1,
  HAL_CLK_SOURCE_RAW2,

  /*
   * Misc
   */
  HAL_CLK_SOURCE_SLEEPCLK,
  HAL_CLK_SOURCE_GROUND,
  HAL_CLK_SOURCE_GPLL0_DIV2,

  HAL_CLK_NUM_OF_SOURCES,

  HAL_ENUM_32BITS(CLK_SOURCE)
} HAL_clk_SourceType;

/*
 * Clock handle type
 * NOTE: this can also be typedef uint32 * HAL_clk_ClockHandleType
 */
typedef void *HAL_clk_ClockHandleType;

/*
 * Clock domain handle type
 * NOTE: this can also be typedef uint32 * HAL_clk_ClockDomainHandleType
 */
typedef void *HAL_clk_ClockDomainHandleType;

/*
 * Power domain handle type
 * NOTE: this can also be typedef uint32 * HAL_clk_PowerDomainHandleType
 */
typedef void *HAL_clk_PowerDomainHandleType;

/*
 * ClockHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 *  aeChipInfoId    - Array of chip id's that the configuration supports.
 *                    NULL indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType    eChipInfoFamily;
  const DalChipInfoIdType *aeChipInfoId;

} ClockHWVersionType;


/*
 * ClockSourceFreqConfigType
 *
 * Frequency configuration information for a PLL.
 *
 *  nFreqHz     - Frequency in Hz for this configuration.
 *  HALConfig   - Configuration to pass to the HAL.
 *  eVRegLevel  - Voltage level for this configuration.
 *  HWVersion   - Version of the chip HW this configuration is for.
 *  bLastEntry  - Boolean indicator if this is the last entry.
 */
typedef struct
{
  uint32                nFreqHz;
  HAL_clk_PLLConfigType HALConfig;
  ClockVRegLevelType    eVRegLevel;
  ClockHWVersionType    HWVersion;
  boolean               bLastEntry;
} ClockSourceFreqConfigType;


/*
 * ClockSourceConfigType
 *
 * Configuration information for a PLL.
 *
 *  eSource           - Source ENUM value.
 *  szName            - Name of source.
 *  nConfigMask       - Mask of misc PLL configurations such as aux out,
 *                      bist out, early out, etc.
 *  pSourceFreqConfig - Pointer to array of source configurations.
 *  pCalibrationFreqConfig  - Pointer to the calibration configuration.
 *  eDisableMode            - PLL disable/enable mode.
 */
typedef struct
{
  HAL_clk_SourceType         eSource;
  const char                *szName;
  uint32                     nConfigMask;
  ClockSourceFreqConfigType *pSourceFreqConfig;
  ClockSourceFreqConfigType *pCalibrationFreqConfig;
  HAL_clk_SourceDisableModeType  eDisableMode;
} ClockSourceConfigType;


/*
 * ClockMuxConfigType
 *
 * Configuration information for a clock mux.
 *
 *  nFreqHz           - Frequency in Hz for this configuration.
 *  HALConfig         - Configuration to pass to the HAL.
 *  eVRegLevel        - Voltage level for this configuration.
 *  HWVersion         - Version of the chip HW this configuration is for.
 *  pSourceFreqConfig - Optional pointer to a PLL to configure for this.
 */
typedef struct
{
  uint32                     nFreqHz;
  HAL_clk_ClockMuxConfigType HALConfig;
  ClockVRegLevelType         eVRegLevel;
  ClockHWVersionType         HWVersion;
  ClockSourceFreqConfigType *pSourceFreqConfig;
} ClockMuxConfigType;


/*
 * ClockLogType
 *
 * Default configuration for the clock log.
 *
 *  nLogSize         - Log size in bytes.  Must be a power of 2.
 *  nGlobalLogFlags  - Global log flags.
 */
typedef struct
{
  uint32      nLogSize;
  uint8       nGlobalLogFlags;
} ClockLogType;


/*
 * ClockXOVoterType
 *
 * Structure to hold clocks which need to explicitly hold a source.
 */
typedef struct
{
  const char *szName;
} ClockXOVoterType;


/*
 * ClockPropertyValueType
 *
 * Generic clock property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *ClockPropertyValueType;


/*
 * ClockPropertyType
 *
 * Generic clock property structure, the BSP data in non-DAL environments
 * will consist of an array of these.
 *
 *  szName - Name of the property
 *  Value  - Property value
 */
typedef struct
{
  const char            *szName;
  ClockPropertyValueType Value;
} ClockPropertyType;



/*
 * ClockAVSCoreCfgType
 *
 * BSP Data structure for describing delay synthesizer to CPU frequency
 * requirements.
 *
 * nDelaySynth    - The synthesizer value to program to the coprocessor.
 * nMinVdd        - The lowest voltage that AVS can set automatically at this frequency.
 * nPerfVdd       - The performance level associated voltage in uV.
 */
typedef struct
{
  uint32 nDelaySynth;
  uint32 nMinVdd;
  uint32 nPerfVdd;
}ClockAVSCoreFreqCfgType;


/*
 * ClockAVSCoreConfigType
 *
 * BSP Data structure for initializing an AVS core.
 *
 * nPhysicalBase    - The physical base address for this AVS core.
 * eCore            - The Core ID for this instance.
 * ePMIC            - The PMIC type for this core.
 * eStepSize        - The step size of voltage adjustment requests.
 * nVddRequestDelay - The delay between HW voltage requests.
 * nClockDiv        - The clock divider.
 * HWVersion        - The version of the chip HW this configuration is for.
 */
typedef struct
{
  uint32               nPhysicalBase;
  HAL_avs_CoreType     eCore;
  HAL_avs_PmicType     ePMIC;
  HAL_avs_StepSizeType eStepSize;
  uint32               nVddRequestDelay;
  uint32               nClockDiv;
  ClockHWVersionType   HWVersion;
}ClockAVSCoreConfigType;


/*
 * ClockFlagNodeType
 *
 * List of nodes that init flags can be applied to.
 *
 *  CLOCK        - Flags are to be applied to the specified clock.
 *  CLOCK_DOMAIN - Flags are to be applied to the specified domain.
 *  SOURCE       - Flags are to be applied to the specified source.
 *  POWER_DOMAIN - Flags are to be applied to the specified power domain.
 *
 */
typedef enum
{
  CLOCK_FLAG_NODE_TYPE_NONE          = 0,
  CLOCK_FLAG_NODE_TYPE_CLOCK         = 1,
  CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN  = 2,
  CLOCK_FLAG_NODE_TYPE_SOURCE        = 3,
  CLOCK_FLAG_NODE_TYPE_POWER_DOMAIN  = 4,
  CLOCK_FLAG_NODE_TYPE_MAX_SUPPORTED = 5
} ClockFlagNodeType;


/*
 * ClockFlagInitType
 *
 * BSP Data structure for initializing the flags of an individual clock,
 * clock domain, power domain, or source.
 *
 *  eNodeType - The node to which the flags should be applied.
 *  id        - The ID to lookup this node info.
 *              (This type is dependent upon the eNodeType).
 *  nFlag   - Init value for the clock's flags.
 */
typedef struct
{
  ClockFlagNodeType  eNodeType;
  void              *pID;
  uint32             nFlag;
} ClockFlagInitType;


/*
 * ClockNameListType
 *
 * Generic type for lists consisting only of clock names.
 *
 * szName - Name of the clock in the list.
 */
typedef struct
{
  const char  *szName;
} ClockNameListType;

/**
 * Clock voltage regular configuration type.
 */
typedef struct ClockVRegConfigType
{
  uint32                         nClientCount[CLOCK_VREG_NUM_LEVELS]; /**< Number of active clients */
  ClockVRegLevelType             eMinLevel;                           /**< Minimum vreg level */
  ClockVRegLevelType             eInitLevel;                          /**< Initial vreg level */
  npa_client_handle              NPAHandle;                           /**< NPA client handle */
} ClockVRegConfigType;


/**
 * Clock voltage request type.
 */
typedef struct ClockVRegRequestType
{
  ClockVRegLevelType eVRegLevel;
  boolean            bSuppressible;
} ClockVRegRequestType;


/**
 * Clock source type.
 */
typedef struct ClockSourceNodeType ClockSourceNodeType;
struct ClockSourceNodeType
{
  HAL_clk_SourceType             eSource;                     /**< Source enum value. */
  const char                    *szName;                      /**< Source name string */
  ClockSourceConfigType         *pBSPConfig;                  /**< Pointer to BSP data */
  npa_client_handle              NPAHandle;                   /**< NPA client handle */
  ClockSourceFreqConfigType     *pActiveFreqConfig;           /**< Pointer to active config */
  uint32                         nReferenceCount;             /**< Number of active non-CPU domains */
  uint32                         nReferenceCountSuppressible; /**< Number of active suppressible domains */
  uint32                         nReferenceCountExternal;     /**< Number of active external enable requests */
  ClockVRegRequestType           VRegRequest;                 /**< Voltage request */
  uint32                         nFlags;                      /**< Flags for the source */
  uint32                         nCalibration;                /**< Source calibration data */
  ClockSourceNodeType           *pSource;                     /**< Pointer to source's source node */
};


/**
 * Clock domain type.
 */
typedef struct
{
  ClockMuxConfigType            *pBSPConfig;                  /**< Pointer to BSP data */
  HAL_clk_ClockDomainHandleType  HALHandle;                   /**< Handle to HAL data */
  ClockMuxConfigType            *pActiveMuxConfig;            /**< Pointer to active config */
  uint32                         nReferenceCount;             /**< Number of active clocks */
  uint32                         nReferenceCountSuppressible; /**< Number of active suppressible domains */
  ClockVRegRequestType           VRegRequest;                 /**< Voltage request */
  uint32                         nFlags;                      /**< Flags for the domain */
  ClockSourceNodeType           *pSource;                     /**< Pointer to source node */
} ClockDomainNodeType;


/**
 * Clock type.
 */
typedef struct
{
  const char                    *szName;              /**< Clock name string */
  HAL_clk_ClockHandleType        HALHandle;           /**< Handle to HAL data */
  uint32                         nReferenceCount;     /**< Number of active client requests */
  uint32                         nFlags;              /**< Flags for the clock */
  ClockDomainNodeType           *pDomain;             /**< Pointer to domain node */
} ClockNodeType;


/**
 * Power domain type.
 */
typedef struct
{
  const char                    *szName;              /**< Power domain name string */
  HAL_clk_PowerDomainHandleType  HALHandle;           /**< Handle to HAL data */
  uint32                         nReferenceCount;     /**< Number of active client requests */
  boolean                        bEnabled;            /**< Enabled flag */
  uint32                         nReferenceCountHW;   /**< Number of active client requests */
  boolean                        bEnabledHW;          /**< Enabled HW Control flag */
  uint32                         nFlags;              /**< Flags for the power domain */
} ClockPowerDomainNodeType;


/**
 * Clock driver context.
 */
typedef struct
{
  DALSYSSyncObj                  LockObj;                               /**< Sync object storage */
  DALSYSSyncHandle               hLock;                                 /**< Handle to our mutex */

  ULogHandle                     hClockLog;                             /**< Handle to our ULOG log */
  uint32                         nGlobalFlags;                          /**< Global flags for the driver */

  ULogHandle                     hClockBISTLog;                         /**< Handle to our BIST ULOG log */
  boolean                        bBISTEnabled;                          /**< Flag used to enable the BIST */
  uint32                         nBISTLogSize;                          /**< Size of the BIST ULOG log */
  ClockNameListType             *pBISTExceptions;                       /**< Pointer to the BIST Exceptions List */

  ULogHandle                     hClockDebugLog;                        /**< Handle to our Debug ULOG log */

  boolean                        bDCVSEnabled;                          /**< DCVS state */
  boolean                        bCPREnabled;                           /**< CPR state */
  void                          *pImageCtxt;                            /**< Image context */

  uint8                          anSourceIndex[HAL_CLK_NUM_OF_SOURCES]; /**< Array of source indices */
  uint32                         nNumSources;                           /**< Number of supported sources */
  ClockSourceNodeType           *aSources;                              /**< Array of supported sources */
  uint32                         nNumClockDomains;                      /**< Number of supported clock domains */
  ClockDomainNodeType           *aClockDomains;                         /**< Array of supported clock domains */
  uint32                         nNumClocks;                            /**< Number of supported clocks */
  ClockNodeType                 *aClocks;                               /**< Array of supported clocks */
  uint32                         nNumPowerDomains;                      /**< Number of supported power domains */
  ClockPowerDomainNodeType      *aPowerDomains;                         /**< Array of supported power domains */
  ClockVRegConfigType            VRegConfig;                            /**< Voltage configuration */
  ClockVRegConfigType            VRegConfigSuppressible;                /**< Suppressible voltage configuration */
  boolean                        bSuspendVoltageRequest;                /**< Voltage suspension state */
  boolean                        bQdssEnabled;                          /**< QDSS enabled status */

  LM_CpuUpdateFuncPtr            fpCpuUpdate;                           /**< LM CPU CX Voltage Corner Update callback function */
  LM_GpuUpdateFuncPtr            fpGpuUpdate;                           /**< LM GPU CX Voltage Corner Update callback function */
} ClockDrvCtxt;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


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
                    ClockBootPerfLevelType eCPUPerfLevel)
{
  return TRUE;
}


/* ========================================================================
**  Function : Clock_PreDDRInit
** ======================================================================*/
/*
    Description: Clock_PreDDRInitEx() was added in 8974 to provide a separate API
    for emergency download (EDL) without including Mxdroop work around. Adding wrapper 
    function to avoid compilation erros in 8x26 and 8x10.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_PreDDRInit( uint32 ddr_type )
{
  return TRUE;
}


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
boolean Clock_I2CInit( void )
{
  return TRUE;
}

/* ============================================================================
**  Function : Clock_InitForDownloadMode
** ============================================================================
*/
/*!
    Configure clocks for download.  Enable every RAM we want to dump.

    @param None.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
void Clock_InitForDownloadMode(void)
{
}


/* ============================================================================
**  Function : Clock_ExitBoot
** ============================================================================
*/
/*!

    This function turns off clocks that were used during boot, but are not
    needed after boot.  This should be called at the end of boot.

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

   @dependencies
    None.

*/
boolean Clock_ExitBoot(void)
{
  return TRUE;
}


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
boolean Clock_DebugInit(void)
{
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SetSysPerfLevel
** ============================================================================
*/
/**
  Configure System and Application Fabric to a perf level.

  @param eSysPerfLevel [in]  -  System NOC and DDR performance level to configure.

  @return
  TRUE -- Fabric was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetSysPerfLevel
(
  ClockBootPerfLevelType eSysPerfLevel
)
{
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SetL2PerfLevel
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
boolean Clock_SetL2PerfLevel(ClockBootPerfLevelType eL2PerfLevel)
{
  return TRUE;
}


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

boolean Clock_SetCPUPerfLevel(ClockBootPerfLevelType eCPUPerfLevel)
{
  return TRUE;
}


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
uint32 Clock_DDRSpeed()
{
  return 0x0;
}

/* =========================================================================
**  Function : Clock_EnterLowPowerMode
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnterLowPowerMode
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult           eReturnedResult = DAL_SUCCESS;
  return(eReturnedResult);
}

/* =========================================================================
**  Function : Clock_ExitLowPowerMode
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ExitLowPowerMode
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult           eReturnedResult = DAL_SUCCESS;
  return(eReturnedResult);
}

/*=========================================================================
**  Function : Clock_NormalizeChipInfo
** =========================================================================*/
/*
  See ClockDriver.h
*/

void Clock_NormalizeChipInfo
(
  HAL_clk_ContextType *HALClkCtxt
)
{
}

/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/*
  See DalClock.h
*/

DALResult Clock_InitImage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;
} /* END Clock_InitVoltage */

/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_VoltageRequest
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockVRegRequestType  *pCurrentRequest,
  ClockVRegRequestType  *pNewRequest
)
{
  return DAL_SUCCESS;
} /* END Clock_VoltageRequest */

/* ============================================================================
**  Function : Clock_InitRPM()
** ============================================================================
*/
/**
  Perform any initialization needed just before RPM starts.  SMEM is available
  at this time.

  @param 
  @return
  TRUE -- Initialization succeeded.
  FALSE -- Initialization failed.

  @dependencies
  None.

  @sa
  Clock_Init.
  boot_smem_init
*/
boolean Clock_InitRPM( void )
{
  return TRUE;
}

/* ============================================================================
**  Function : Clock_GetClockFrequency
** ============================================================================
*/
/*!
    Get the requested clock frequency in hertz.

    @param
      eBootLogClock [in]  - targeted clock
      pnFrequencyHz [out] - frequency of clock in hertz

    @return
      True iff the requested clock frequency is placed in pnFrequencyHz.

    @dependencies
    None.

    @sa None
*/

boolean Clock_GetClockFrequency(ClockBootLogQueryType eBootLogClock,
                                uint32 *pnFrequencyHz)
{
  boolean ret = TRUE;
  return ret;
}

/* =========================================================================
**  Function : Clock_SetBIMCSpeed
** =========================================================================*/
/*!
    Switch DDR and the related BIMC roots to the requested frequency

    @param -  nFreqHz [in] Frequency to search for in the table
    @return - FALSE on fail, TRUE on success

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetBIMCSpeed(uint32 nFreqKHz )
{
  return TRUE;
}

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
boolean Clock_InitVSense( void )
{
  return TRUE;
}

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
boolean Clock_ShutdownVSense( void )
{
  return TRUE;
}

/* =========================================================================
**  Function : Clock_EnableAVS
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableAVS
(
  ClockDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;
}

/* ========================================================================
**  Function : Clock_UFSInit
** ======================================================================*/
/*!
    Get the requested clock frequency in hertz.

    @param
      ePerfLevel [in]  - Performance level

    @return
      True - Success init UFS clocks

    @dependencies
    None.

    @sa None
*/

boolean Clock_UFSInit( ClockBootPerfLevelType ePerfLevel )
{
  return TRUE;
}

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
boolean Clock_InitUSB(void)
{
  return TRUE;
} /* END Clock_InitUSB */


