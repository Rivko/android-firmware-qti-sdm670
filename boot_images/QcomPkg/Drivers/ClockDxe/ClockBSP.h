#ifndef __CLOCKBSP_H__
#define __CLOCKBSP_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R

        B O A R D   S U P P O R T   P A C K A G E   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the BSP interface definitions for the
  MSM DAL Clock Device Driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2010-2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/ClockDxe/ClockBSP.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/30/10   vs      Initial release.

==========================================================================*/

/*=========================================================================
      Includes
==========================================================================*/


#include "Include/ClockDefs.h"
#include "HALclk.h"
#include "HALclkInternal.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#ifdef CLOCK_NPA_STUB
  #include "ClockNPAStub.h"
#else
  #include "npa.h"
  #include "npa_resource.h"
#endif


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

#define SIZEOF_ARRAY(a) (sizeof((a))/sizeof((a)[0]))


/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_BSP_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF


/*
 * Macros for compare support BSP within the range of HW version
 * BSP HW VERSION:
 *     Bits[ 0: 7] - Max HW version minor bits
 *     Bits[15: 8] - Max HW version major bits
 *     Bits[23:16] - Min HW version minor bits
 *     Bits[31:24] - Min HW version major bits
*/

#define BSP_HW_VER(nMinMajor, nMinMinor, nMaxMajor, nMaxMinor)                 \
            ( ((nMinMajor<<24)& 0xFF000000) | ((nMinMinor<<16) & 0x00FF0000) | \
              ((nMaxMajor<<8) & 0x0000FF00) |  (nMaxMinor&0xFF) )

#define BSP_HW_VER_MIN( nBSPVersion ) ( ((nBSPVersion & 0xFF000000)>>8) | \
                                        ((nBSPVersion & 0x00FF0000)>>16) )
#define BSP_HW_VER_MAX( nBSPVersion ) ( ((nBSPVersion & 0x0000FF00)<<8) | \
                                         (nBSPVersion & 0x000000FF) )


/*
 * Macros for consolidating PLL configuration in a single mask.
 */
#define CLOCK_CONFIG_PLL_FSM_MODE_ENABLE        (1 << 0)
#define CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE      (1 << 1)
#define CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE     (1 << 2)
#define CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE    (1 << 3)
#define CLOCK_CONFIG_PLL_XO_DOUBLER_DCC         (1 << 4)
#define CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE     (1 << 5)
#define CLOCK_CONFIG_PLL_ODD_OUTPUT_ENABLE      (1 << 6)


/**
 * Unified flag bits
 *
 * NOTE: These flags can be used in any node that supports the
 * nFlags field.
 *
 *  LOG_STATE_CHANGE - Enable logging of enable/disable calls
 *    to a particular node.
 *  LOG_FREQ_CHANGE - Enable logging of frequency changes to
 *    a particular node.
 *  LOG_CONFIG_CHANGE - Enable logging of configuration changes
 *    to a particular node.
 *  LOG_RESET - Enable logging of resets to a particular node.
 *  LOG_INVERT_CHANGE - Enables logging of clock inversion
 *    changes.
 *  LOG_SLEEP_CHANGE - Enable logging of sleep state changes.
 *  DO_NOT_DISABLE - Ignore requests to disable a particular
 *    node.
 *  SUPPRESSIBLE - Indicates XO shutdown will not be held off
 *    while the clock is on.
 *  DOMAIN_HOLD_VREG_REQUEST - Hold the voltage request even
 *    when the clock is off. When set this means the voltage
 *    request will be based solely on the configured clock
 *    frequency and will not be released when the clock is
 *    disabled.
 *  DOMAIN_DYNAMIC_CONFIG - Indicates this domain was configured
 *    dynamically and memory should be freed when the domain is
 *    reconfigured.
 *  AUTOGATE - Indicates this clock has the autogating feature
 *    enabled.
 *  BIST_FAILURE_ANTICIPATED - Indicates this clock may fail the
 *    Built-In-Self-Test due to configuration complexities, and
 *    negative results should not be treated as a hard failure.
 *  XO_VOTE - Indicates this clock should explicitly vote for
 *    XO when enabled.
 *  NO_REFERENCE_COUNTING - Indicates that reference counting for
 *    this clock has been disabled. In such cases, the reference
 *    counter is limited to the range between 0 and 1.
 *  BRANCH_ONLY_CONTROL - Indicates that when controlling this
 *    clock all source and voltage processing is bypassed and
 *    only the clock branch control is performed. Source and
 *    voltage processing is performed only when configuring
 *    this clock to be under BRANCH_ONLY_CONTROL.
 *  SUPPRESS_VOLTAGE_CONTROL - Indicates that voltage control
 *    is being performed externally and all voltage control
 *    requests need to be ignored. This is usually a very
 *    special scenario (e.g., crash dump collection in WA/WP).
 *  SUPPORTS_SLEWING - Indicates that this PLL can be
 *    dynamically re-configured without having to first
 *    be disabled and then re-enabled.
 *  SOURCE_CALIBRATED - Indicates that a PLL has been calibrated.
 *  BYPASS_DEPENDENCIES - Indicates that the resource's requirements
 *    are always satisfied or managed by an external agent.
 *    The clock driver will not make requests for this type
 *    of resource.
 *  STUB_HW_RUMI - Indicates that requests on this resource should
 *    avoid accessing HW on RUMI.
 *  CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF - Indicates that the SW should wait for
 *    a root-off signal before disabling the clock source.
 *  DETECT_SUBSOURCE - Detect the subsource configuration to match against the
 *    source configuration defined in the BSP.
 */
#define CLOCK_FLAG_LOG_STATE_CHANGE           PUBLIC_CLOCK_FLAG_LOG_STATE_CHANGE
#define CLOCK_FLAG_LOG_FREQ_CHANGE            PUBLIC_CLOCK_FLAG_LOG_FREQ_CHANGE
#define CLOCK_FLAG_LOG_CONFIG_CHANGE          PUBLIC_CLOCK_FLAG_LOG_CONFIG_CHANGE
#define CLOCK_FLAG_LOG_RESET                  PUBLIC_CLOCK_FLAG_LOG_RESET
#define CLOCK_FLAG_LOG_INVERT_CHANGE          PUBLIC_CLOCK_FLAG_LOG_INVERT_CHANGE
#define CLOCK_FLAG_LOG_SLEEP_CHANGE           PUBLIC_CLOCK_FLAG_LOG_SLEEP_CHANGE
// Reserved for future logging                PUBLIC_CLOCK_FLAG_RESERVED_0x00000040
// Reserved for future logging                PUBLIC_CLOCK_FLAG_RESERVED_0x00000080
#define CLOCK_FLAG_DO_NOT_DISABLE             PUBLIC_CLOCK_FLAG_RESERVED_0x00000100
#define CLOCK_FLAG_SUPPRESSIBLE               PUBLIC_CLOCK_FLAG_RESERVED_0x00000200
#define CLOCK_FLAG_DOMAIN_HOLD_VREG_REQUEST   PUBLIC_CLOCK_FLAG_RESERVED_0x00000400
#define CLOCK_FLAG_DOMAIN_DYNAMIC_CONFIG      PUBLIC_CLOCK_FLAG_RESERVED_0x00000800
#define CLOCK_FLAG_AUTOGATE                   PUBLIC_CLOCK_FLAG_RESERVED_0x00001000
#define CLOCK_FLAG_BIST_FAILURE_ANTICIPATED   PUBLIC_CLOCK_FLAG_RESERVED_0x00002000
#define CLOCK_FLAG_INTERNAL_CONTROL           PUBLIC_CLOCK_FLAG_RESERVED_0x00004000
#define CLOCK_FLAG_XO_VOTE                    PUBLIC_CLOCK_FLAG_RESERVED_0x00008000
#define CLOCK_FLAG_NO_REFERENCE_COUNTING      PUBLIC_CLOCK_FLAG_RESERVED_0x00010000
#define CLOCK_FLAG_BRANCH_ONLY_CONTROL        PUBLIC_CLOCK_FLAG_RESERVED_0x00020000
#define CLOCK_FLAG_SUPPRESS_VOLTAGE_CONTROL   PUBLIC_CLOCK_FLAG_RESERVED_0x00040000
#define CLOCK_FLAG_SUPPORTS_SLEWING           PUBLIC_CLOCK_FLAG_RESERVED_0x00080000
#define CLOCK_FLAG_SOURCE_CALIBRATED          PUBLIC_CLOCK_FLAG_RESERVED_0x00100000
#define CLOCK_FLAG_BYPASS_DEPENDENCIES        PUBLIC_CLOCK_FLAG_RESERVED_0x00200000
#define CLOCK_FLAG_LEAVE_ALWAYS_ON            PUBLIC_CLOCK_FLAG_RESERVED_0x00400000
// Reserved for future allocation             PUBLIC_CLOCK_FLAG_RESERVED_0x00800000
#define CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE    PUBLIC_CLOCK_FLAG_RESERVED_0x01000000
#define CLOCK_FLAG_STUB_HW_RUMI               PUBLIC_CLOCK_FLAG_RESERVED_0x02000000
#define CLOCK_FLAG_CPU_SOURCE_PLL             PUBLIC_CLOCK_FLAG_RESERVED_0x04000000
#define CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF   PUBLIC_CLOCK_FLAG_RESERVED_0x08000000
#define CLOCK_FLAG_EXPLICIT_ACCESS_ONLY       PUBLIC_CLOCK_FLAG_RESERVED_0x10000000
#define CLOCK_FLAG_READ_ONLY                  PUBLIC_CLOCK_FLAG_RESERVED_0x20000000
#define CLOCK_FLAG_DFS_CONTROLLED             PUBLIC_CLOCK_FLAG_RESERVED_0x40000000
#define CLOCK_FLAG_FORCE_ROOT_ENABLE          PUBLIC_CLOCK_FLAG_RESERVED_0x80000000

#define CLOCK_LOGGING_FLAGS               \
        (                                 \
          CLOCK_FLAG_LOG_STATE_CHANGE   | \
          CLOCK_FLAG_LOG_FREQ_CHANGE    | \
          CLOCK_FLAG_LOG_CONFIG_CHANGE  | \
          CLOCK_FLAG_LOG_RESET          | \
          CLOCK_FLAG_LOG_INVERT_CHANGE  | \
          CLOCK_FLAG_LOG_SLEEP_CHANGE     \
        )

/**
 * Global flag bits
 *
 * NOTE: These flags are to be used only in the pBSP->nFlags
 * field of the driver context structure.
 *
 *  LOG_x_STATE_CHANGE - Enable logging of enable/disable
 *    calls to all clocks, sources, or power domains as
 *    specified by x.
 *  LOG_CLOCK_FREQ_CHANGE - Enable logging of frequency changes
 *    to all clocks.
 *  LOG_CLOCK_CONFIG_CHANGE - Enable logging of configuration
 *    changes to all clocks.
 *  LOG_CLOCK_RESET - Enable logging of resets to all clocks.
 *  LOG_VOLTAGE_CHANGE - Enable logging of vdd requirement changes.
 *  DO_NOT_DISABLE_x - Ignore requests to disable all clocks,
 *    sources, or power domains as specified by x.
 *  STUB_API - Stubs out entire API (DAL and NPA nodes expected).
 *  LOG_CLOCK_SLEEP_CHANGE - Enable logging of sleep state changes.
 *  BIST_IN_PROGRESS - Indicates that a Built-In Self Test is running.
 *  LOG_MISC_STATUS - Enable logging of miscellaneous status info.
 *  CPU_BYPASS_RANGE_CHECKING - Bypass DCVS requests range checking.
 */
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE          PUBLIC_CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE           PUBLIC_CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE         PUBLIC_CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET                 PUBLIC_CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE         PUBLIC_CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE
#define CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE   PUBLIC_CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE
#define CLOCK_GLOBAL_FLAG_LOG_VOLTAGE_CHANGE              PUBLIC_CLOCK_GLOBAL_FLAG_LOG_VOLTAGE_CHANGE
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_FREQ_CHANGE          PUBLIC_CLOCK_GLOBAL_FLAG_LOG_SOURCE_FREQ_CHANGE
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_CLOCKS           PUBLIC_CLOCK_GLOBAL_FLAG_RESERVED_0x00000100
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_SOURCES          PUBLIC_CLOCK_GLOBAL_FLAG_RESERVED_0x00000200
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_POWER_DOMAINS    PUBLIC_CLOCK_GLOBAL_FLAG_RESERVED_0x00000400
#define CLOCK_GLOBAL_FLAG_STUB_API                        PUBLIC_CLOCK_GLOBAL_FLAG_RESERVED_0x00000800
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_SLEEP_CHANGE          PUBLIC_CLOCK_GLOBAL_FLAG_LOG_CLOCK_SLEEP_CHANGE
#define CLOCK_GLOBAL_FLAG_BIST_IN_PROGRESS                PUBLIC_CLOCK_GLOBAL_FLAG_BIST_IN_PROGRESS
#define CLOCK_GLOBAL_FLAG_CPU_TEST_IN_PROGRESS            PUBLIC_CLOCK_GLOBAL_FLAG_CPU_TEST_IN_PROGRESS
#define CLOCK_GLOBAL_FLAG_LOG_MISC_STATUS                 PUBLIC_CLOCK_GLOBAL_FLAG_LOG_MISC_STATUS
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_INVERT_CHANGE         PUBLIC_CLOCK_GLOBAL_FLAG_LOG_CLOCK_INVERT_CHANGE
#define CLOCK_GLOBAL_FLAG_CPU_BYPASS_RANGE_CHECKING       PUBLIC_CLOCK_GLOBAL_FLAG_CPU_BYPASS_RANGE_CHECKING

#define CLOCK_GLOBAL_LOGGING_FLAGS                          \
        (                                                   \
          CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE        | \
          CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE         | \
          CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE       | \
          CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET               | \
          CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE       | \
          CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE | \
          CLOCK_GLOBAL_FLAG_LOG_VOLTAGE_CHANGE            | \
          CLOCK_GLOBAL_FLAG_LOG_SOURCE_FREQ_CHANGE        | \
          CLOCK_GLOBAL_FLAG_LOG_CLOCK_SLEEP_CHANGE        | \
          CLOCK_GLOBAL_FLAG_LOG_MISC_STATUS               | \
          CLOCK_GLOBAL_FLAG_BIST_IN_PROGRESS              | \
          CLOCK_GLOBAL_FLAG_CPU_TEST_IN_PROGRESS          | \
          CLOCK_GLOBAL_FLAG_LOG_CLOCK_INVERT_CHANGE         \
        )


/*
 * Macro used to terminate ClockSourceMapType arrays.
 */
#define CLOCK_SOURCE_MAP_END 0xFF


/*
 * Macro used to terminate ClockDebugMuxInputDivType arrays.
 */
#define CLOCK_DEBUG_MUX_INPUT_DIV_END 0xFFFF


/*
 * Forward prototypes.
 */
typedef struct ClockSourceNodeType ClockSourceNodeType;


/*
 * ClockVRegLevelType
 *
 * List of voltage levels that a client can request.  Note that the
 * enumeration values map to pmic NPA node request values.
 *
 *  OFF             - No vote
 *  RETENTION_MINUS - Lowered retention voltage level.
 *  RETENTION       - Retention voltage level.
 *  RETENTION_PLUS  - Elevated retention voltage level.
 *  LOW_MIN         - Lowest operational voltage level.
 *  LOW_MINUS       - Lowered low voltage level.
 *  LOW             - Low voltage level.
 *  LOW_PLUS        - Elevated low voltage level.
 *  NOMINAL_MINUS   - Lowered nominal voltage level.
 *  NOMINAL         - Nominal voltage level.
 *  NOMINAL_PLUS    - Elevated nominal voltage level.
 *  HIGH_MINUS      - Lowered high voltage level.
 *  HIGH            - High voltage level.
 *  HIGH_PLUS       - Elevated high voltage level.
 *  TURBO_MINUS     - Lowered turbo voltage level.
 *  TURBO           - Turbo voltage level.
 *  TURBO_PLUS      - Elevated turbo voltage level.
 */
typedef enum
{
  CLOCK_VREG_LEVEL_OFF              = HAL_CLK_VREG_LEVEL_OFF,
  CLOCK_VREG_LEVEL_RETENTION_MINUS  = HAL_CLK_VREG_LEVEL_RETENTION_MINUS,
  CLOCK_VREG_LEVEL_RETENTION        = HAL_CLK_VREG_LEVEL_RETENTION,
  CLOCK_VREG_LEVEL_RETENTION_PLUS   = HAL_CLK_VREG_LEVEL_RETENTION_PLUS,
  CLOCK_VREG_LEVEL_LOW_MIN          = HAL_CLK_VREG_LEVEL_LOW_MIN,
  CLOCK_VREG_LEVEL_LOW_MINUS        = HAL_CLK_VREG_LEVEL_LOW_MINUS,
  CLOCK_VREG_LEVEL_LOW              = HAL_CLK_VREG_LEVEL_LOW,
  CLOCK_VREG_LEVEL_LOW_PLUS         = HAL_CLK_VREG_LEVEL_LOW_PLUS,
  CLOCK_VREG_LEVEL_NOMINAL_MINUS    = HAL_CLK_VREG_LEVEL_NOMINAL_MINUS,
  CLOCK_VREG_LEVEL_NOMINAL          = HAL_CLK_VREG_LEVEL_NOMINAL,
  CLOCK_VREG_LEVEL_NOMINAL_PLUS     = HAL_CLK_VREG_LEVEL_NOMINAL_PLUS,
  CLOCK_VREG_LEVEL_HIGH_MINUS       = HAL_CLK_VREG_LEVEL_HIGH_MINUS,
  CLOCK_VREG_LEVEL_HIGH             = HAL_CLK_VREG_LEVEL_HIGH,
  CLOCK_VREG_LEVEL_HIGH_PLUS        = HAL_CLK_VREG_LEVEL_HIGH_PLUS,
  CLOCK_VREG_LEVEL_TURBO_MINUS      = HAL_CLK_VREG_LEVEL_TURBO_MINUS,
  CLOCK_VREG_LEVEL_TURBO            = HAL_CLK_VREG_LEVEL_TURBO,
  CLOCK_VREG_LEVEL_TURBO_PLUS       = HAL_CLK_VREG_LEVEL_TURBO_PLUS,

  CLOCK_VREG_MAX_LEVEL              = HAL_CLK_VREG_MAX_LEVEL,
  CLOCK_VREG_NUM_LEVELS             = HAL_CLK_VREG_NUM_LEVELS,
  CLOCK_BSP_ENUM_32BITS(VREG_LEVEL)
} ClockVRegLevelType;


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
 *  pSource     - Pointer to source used by this configuration.
 *  HALConfig   - Configuration to pass to the HAL.
 *  eVRegLevel  - Voltage level for this configuration.
 *  HWVersion   - Version of the chip HW this configuration is for.
 */
typedef struct
{
  uint32                 nFreqHz;
  ClockSourceNodeType   *pSource;
  HAL_clk_PLLConfigType  HALConfig;
  ClockVRegLevelType     eVRegLevel;
  ClockHWVersionType     HWVersion;
} ClockSourceFreqConfigType;


/*
 * ClockMuxConfigType
 *
 * Configuration information for a clock mux.
 *
 *  nFreqHz           - Frequency in Hz for this configuration.
 *  pSource           - Pointer to source used by this configuration.
 *  HALConfig         - Configuration to pass to the HAL.
 *  eVRegLevel        - Voltage level for this configuration.
 *  HWVersion         - Version of the chip HW this configuration is for.
 *  pSourceFreqConfig - Optional pointer to a PLL to configure for this.
 */
typedef struct
{
  uint32                           nFreqHz;
  ClockSourceNodeType             *pSource;
  HAL_clk_ClockMuxConfigType       HALConfig;
  ClockVRegLevelType               eVRegLevel;
  ClockHWVersionType               HWVersion;
  const ClockSourceFreqConfigType *pSourceFreqConfig;
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
  uint32 nLogSize;
  uint32 nGlobalLogFlags;
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
 * ClockStubType
 *
 * BSP data structure for stubbing out the clock driver on various platforms.
 *
 * bRUMI   - Stub boolean for RUMI.
 * bVirtio - Stub boolean for Virtio.
 */
typedef struct
{
  boolean bRUMI;
  boolean bVirtio;
} ClockStubType;


/*
 * ClockBase
 *
 * Structure for a register base mapping.
 *
 * nPhysAddr  - Physical address to be mapped.
 * nSize      - Size of region to be mapped.
 * pnVirtAddr - Mapped virtual address.
 */
typedef struct ClockBase
{
  uintnt  nPhysAddr;
  uint32  nSize;
  uintnt *pnVirtAddr;
} ClockBaseType;


/**
 * Structure to track corner reference counts.
 */
typedef struct ClockCornerRefCount
{
  ClockVRegLevelType eCorner;   /**< Voltage corner being voted for. */
  uint32             nRefCount; /**< Number of votes on eCorner. */
} ClockCornerRefCountType;


/**
 * Clock voltage rail configuration type.
 */
typedef struct ClockVRegConfigType
{
  ClockCornerRefCountType aCornerRefCounts[CLOCK_VREG_NUM_LEVELS];  /**< Number of active clients */
  ClockVRegLevelType      eMinLevel;                                /**< Minimum vreg level */
  npa_client_handle       NPAHandle;                                /**< NPA client handle */
} ClockVRegConfigType;


/**
 * Clock voltage rail type.
 */
typedef struct ClockRail
{
  const char          *szName;                  /**< Rail name */
  ClockVRegConfigType  VRegConfig;              /**< Voltage configuration */
  ClockVRegConfigType  VRegConfigSuppressible;  /**< Suppressible voltage configuration */
  boolean              bSuspendVoltageRequest;  /**< Voltage suspension state */
} ClockRailType;


/**
 * Clock voltage request type.
 */
typedef struct ClockVRegRequestType
{
  ClockVRegLevelType  eVRegLevel;
  boolean             bSuppressible;
} ClockVRegRequestType;


/**
 * Clock source type.
 */
struct ClockSourceNodeType
{
  const char                      *szName;                        /**< Source name. */
  uint32                           nFlags;                        /**< Source flags. */
  const ClockSourceFreqConfigType *aBSP;                          /**< Frequency configuration array. */
  uint32                           nBSPLen;                       /**< Length of aBSP. */
  ClockRailType                   *pRail;                         /**< Voltage rail powering this source. */
  HAL_clk_SourceDescType           HALSource;                     /**< HAL source structure. */
  uint32                           nConfigMask;                   /**< Configuration mask */
  HAL_clk_SourceDisableModeType    eDisableMode;                  /**< Disable mode. */
  uint32                           nReferenceCount;               /**< Number of active non-CPU domains */
  uint32                           nReferenceCountSuppressible;   /**< Number of active suppressible domains */
  uint32                           nReferenceCountExternal;       /**< Number of active external enable requests */
  ClockVRegRequestType             VRegRequest;                   /**< Voltage request */
  ClockSourceNodeType             *pSource;                       /**< Pointer to source's source node */
  const ClockSourceFreqConfigType *pActiveFreqConfig;             /**< Pointer to active config */
  const ClockSourceFreqConfigType *pCalibrationFreqConfig;        /**< Pointer to calibration config */
  uint32                           nCalibrationFreqConfigLen;
  npa_client_handle                NPAHandle;                     /**< NPA client handle for required requests. */
  npa_client_handle                NPAHandleSuppressible;         /**< NPA client handle for suppressible requests. */
  uint32                           nCalibration;                  /**< Source calibration data */
};


/**
 * ClockFMaxType
 */
typedef struct
{
  uint32              nMaxFreqHz; /**< Max frequency. */
  ClockVRegLevelType  eVRegLevel; /**< Corner required by nMaxFreqHz */
  ClockHWVersionType  HWVersion;  /**< HW version this max frequency applies to. */
} ClockFMaxType;


/**
 * Clock source map type.
 */
typedef struct ClockSourceMap
{
  uint32               nMuxSel;   /**< Mux select value. */
  uint32               nFreqHz;   /**< Frequency of clock selected by nMuxSel. */
  ClockSourceNodeType *pSource;   /**< Source node associated with nMuxSel. */
  ClockHWVersionType   HWVersion; /**< HW version this source map applies to. */
} ClockSourceMapType;


/**
 * Structure to track input-specific divide values.
 */
typedef struct ClockDebugMuxInputDiv
{
  uint16 nMuxSel;   /**< Input mux select value. */
  uint16 nDivider;  /**< Divider for input specified by nMuxSel. */
} ClockDebugMuxInputDivType;


/**
 * Clock debug mux type.
 */
typedef struct ClockDebugMux ClockDebugMuxType;
struct ClockDebugMux
{
  const char                *szName;      /**< Debug mux name */
  ClockDebugMuxType         *pParent;     /**< Pointer to parent mux. */
  uint32                     nParentSel;  /**< Value to select this mux in pParent. */
  uint32                     nDivider;    /**< Global divider applied to all inputs on this mux. */
  ClockDebugMuxInputDivType *anInputDivs; /**< Additional dividers that apply only to specific inputs. */
  HAL_clk_DebugMuxDescType   HALDebugMux; /**< HAL debug mux structure. */
};


/**
 * Clock domain type.
 */
typedef struct
{
  ClockMuxConfigType          *aBSP;                          /**< Frequency configuration array. */
  uint32                       nBSPLen;                       /**< Length of aBSP. */
  ClockRailType               *pRail;                         /**< Voltage rail powering this domain. */
  ClockSourceNodeType         *pSource;                       /**< Pointer to source node */
  uint32                       nFlags;                        /**< Domain flags. */
  HAL_clk_ClockDomainDescType  HALDomain;                     /**< HAL domain structure. */
  const ClockMuxConfigType    *pActiveMuxConfig;              /**< Pointer to active config */
  uint32                       nReferenceCount;               /**< Number of active clocks */
  uint32                       nReferenceCountSuppressible;   /**< Number of active suppressible domains */
  ClockVRegRequestType         VRegRequest;                   /**< Voltage request */
  const ClockFMaxType         *aFMax;                         /**< Max frequency array. */
  const ClockSourceMapType    *aSourceMap;                    /**< Source map array. */
} ClockDomainNodeType;


/**
 * Clock type.
 */
typedef struct
{
  const char            *szName;                            /**< Clock name. */
  ClockDomainNodeType   *pDomain;                           /**< Domain this clock belongs to. */
  uint32                 nFlags;                            /**< Clock flags. */
  HAL_clk_ClockDescType  HALClock;                          /**< HAL clock structure. */
  uint32                 nReferenceCount;                   /**< Number of active client requests */
  ClockDebugMuxType     *pDebugMux;                         /**< Pointer to debug mux this clock is connected to. */
  uint32                 nUniqueId;                         /**< Unique number used to identify this clock in logs. */
} ClockNodeType;


/**
 * Power domain type.
 */
typedef struct
{
  const char                  *szName;                      /**< Power domain name. */
  uint32                       nFlags;                      /**< Power domain flags. */
  HAL_clk_PowerDomainDescType  HALPowerDomain;              /**< HAL power domain structure. */
  uint32                       nReferenceCount;             /**< Number of active client requests */
  uint32                       nReferenceCountHW;           /**< Number of active client requests */
} ClockPowerDomainNodeType;


/**
 * Clock divider type.
 */
typedef struct ClockDivider
{
  const char              *szName;      /**< Clock divider name. */
  uint32                   nFlags;      /**< Divider flags. */
  HAL_clk_DividerDescType  HALDivider;  /**< HAL clock divider structure. */
} ClockDividerNodeType;


/**
 * Clock NPA resource context.
 */
typedef struct ClockNPACtxt
{
  const char    *szClockName; /**< Name of clock to control. */
  ClockNodeType *pClock;      /**< Clock node to control. */
} ClockNPACtxtType;


/**
 * Clock NPA node definition.
 */
typedef struct ClockNPANode
{
  npa_node_definition  Node;          /**< NPA node. */
  npa_resource_state  *anInitStates;  /**< Initial resource states. */
} ClockNPANodeType;


/**
 * Main clock BSP type.
 */
typedef struct ClockBSP
{
  ClockBaseType              *aBases;           /**< Array of clock bases. */
  uint32                      nNumBases;        /**< Length of aBases. */
  ClockRailType             **aRails;           /**< Array of voltage rails. */
  uint32                      nNumRails;        /**< Length of aRails. */
  ClockNodeType              *aClocks;          /**< Array of clocks. */
  uint32                      nNumClocks;       /**< Length of aClocks. */
  ClockDomainNodeType       **aDomains;         /**< Array of clock domains. */
  uint32                      nNumDomains;      /**< Length of aDomains. */
  ClockSourceNodeType       **aSources;         /**< Array of clock sources. */
  uint32                      nNumSources;      /**< Length of aSources. */
  ClockDividerNodeType       *aDividers;        /**< Array of clock dividers .*/
  uint32                      nNumDividers;     /**< Length of aDividers. */
  ClockPowerDomainNodeType   *aPowerDomains;    /**< Array of power domains. */
  uint32                      nNumPowerDomains; /**< Length of aPowerDomains. */
  uint32                      nFlags;           /**< Clock driver flags. */
  ClockNPANodeType           *pGenericNPANode;  /**< Generic NPA node. */
  uint32                      nLogFlags;        /**< Clock driver log flags. */
  HAL_clk_ClockMuxConfigType  SafeMuxConfig;    /**< Default safe mux configuration. */
  ClockDebugMuxType         **aDebugMuxes;      /**< Array of debug muxes. */
  uint32                      nNumDebugMuxes;   /**< Length of aDebugMuxes. */
} ClockBSPType;


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


#endif  /* __CLOCKBSP_H__ */

