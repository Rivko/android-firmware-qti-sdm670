#ifndef __HALCLK_H__
#define __HALCLK_H__
/*
==============================================================================

FILE:         HALclk.h

DESCRIPTION:

==============================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/Library/HALclk.h#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/18/11   vs      Ported from the latest clock driver.


==============================================================================
            Copyright (c) 2010-2017 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include <HALcomdef.h>
#include <CoreVerify.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

#ifndef MIN
  #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
  #define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


/*
 * Macro to generate the hardware version number from a given major
 * and minor number.  Used when comparing against return value of
 * HAL_clk_GetChipVersion(). For example to check if this is at least
 * version 1.2:
 *   if (HAL_clk_GetChipVersion() >= HAL_CLK_CHIP_VERSION(1,2))
 */
#define HAL_CLK_CHIP_VERSION(major, minor)  (((major) << 16) | (minor))


/*
 * Value used to indicate clock has no debug mux connection.
 */
#define HAL_CLK_NO_DEBUG_MUX_SEL  0xFFFFFFFF


/* ============================================================================
**    Typedefs
** ==========================================================================*/

/*
 * Forward declaration of structs b/c of the cross references within these
 * structs
 */
typedef struct HAL_clk_ClockDescType       HAL_clk_ClockDescType;
typedef struct HAL_clk_ClockDomainDescType HAL_clk_ClockDomainDescType;
typedef struct HAL_clk_SourceDescType      HAL_clk_SourceDescType;
typedef struct HAL_clk_PowerDomainDescType HAL_clk_PowerDomainDescType;
typedef struct HAL_clk_DividerDescType     HAL_clk_DividerDescType;
typedef struct HAL_clk_DebugMuxDescType    HAL_clk_DebugMuxDescType;


/*
 * HAL_clk_ClockConfigType
 *
 * Generic configurations used for clocks.  Each clock will support
 * a different set of these enumerations.
 *
 */
typedef enum
{
  HAL_CLK_CONFIG_NULL                                   = 0,

  /*
   * Using lower half as clock config values that will be passed
   * by the client via top level driver interface.
   */
  HAL_CLK_CONFIG_MMSS_SOURCE_CSI0                       = 1,
  HAL_CLK_CONFIG_MMSS_SOURCE_CSI1                       = 2,
  HAL_CLK_CONFIG_MMSS_SOURCE_CSI2                       = 3,
  HAL_CLK_CONFIG_CORE_MEM_ON                            = 4,
  HAL_CLK_CONFIG_CORE_MEM_OFF                           = 5,
  HAL_CLK_CONFIG_PERIPH_MEM_ON                          = 6,
  HAL_CLK_CONFIG_PERIPH_MEM_OFF                         = 7,
  HAL_CLK_CONFIG_HW_CTL_ON                              = 8,
  HAL_CLK_CONFIG_HW_CTL_OFF                             = 9,

  HAL_CLK_CONFIG_FORCE_MEM_CORE_DISABLED                = 13,
  HAL_CLK_CONFIG_FORCE_MEM_CORE_ENABLED                 = 14,
  HAL_CLK_CONFIG_FORCE_MEM_PERIPH_ENABLED               = 15,
  HAL_CLK_CONFIG_FORCE_MEM_CORE_AND_MEM_PERIPH_ENABLED  = 16,

  /*
   * Using upper half as clock config values that will be internal
   * to our driver.
   */
  HAL_CLK_CONFIG_ASYNC_MODE = 1000,
  HAL_CLK_CONFIG_SYNC_MODE,
  HAL_CLK_CONFIG_1X_MODE,
  HAL_CLK_CONFIG_2X_MODE,

  HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
  HAL_CLK_CONFIG_Q6SS_CORE_LOW_JITTER_PLL,
  HAL_CLK_CONFIG_Q6SS_CORE_PLL_MAIN,

  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_MUX,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_PLL2,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_HSPLL1,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_HSPLL2,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_RAMP_CLOCK,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_JTAG_TCK,
  HAL_CLK_CONFIG_ACPU_CORE_SOURCE_MUX_SYNC_TO_AXI,
  HAL_CLK_CONFIG_ACPU_SLEEP_USE_SLEEP_EN,
  HAL_CLK_CONFIG_ACPU_SLEEP_MASK_CORE_CLK,
  HAL_CLK_CONFIG_ACPU_SLEEP_WAIT_FOR_AXI_CLK,
  HAL_CLK_CONFIG_ACPU_NO_POWER_DOWN_REQ_ON_SWFI,
  HAL_CLK_CONFIG_ACPU_POWER_DOWN_REQ_ON_SWFI,
  HAL_CLK_CONFIG_ACPU_POWER_UP_REQ_ON_INTERRUPT,
  HAL_CLK_CONFIG_ACPU_NO_POWER_UP_REQ_ON_INTERRUPT,

  HAL_CLK_CONFIG_VFE_VIDEO,
  HAL_CLK_CONFIG_VFE_CAMERA,
  HAL_CLK_CONFIG_VFE_EXTERNAL,

  HAL_ENUM_32BITS(CLK_CONFIG)
} HAL_clk_ClockConfigType;



/*
 * HAL_clk_ClockAPIType
 *
 * Programming interfaces available for controlling clocks.  Used when the
 * driver needs to verify if the functionality is available for a given
 * clock.  See HAL_clk_IsClockSupported.
 */
typedef enum
{
  HAL_CLK_CLOCK_API_ENABLE,
  HAL_CLK_CLOCK_API_DISABLE,
  HAL_CLK_CLOCK_API_RESET,
  HAL_CLK_CLOCK_API_INVERT,
  HAL_CLK_CLOCK_API_ISON,
  HAL_CLK_CLOCK_API_ISENABLED,
  HAL_CLK_CLOCK_API_CONFIG,
  HAL_CLK_CLOCK_API_CONFIGDIVIDER,
  HAL_CLK_CLOCK_API_CALCFREQ,

  HAL_ENUM_32BITS(CLK_CLOCK_API)
} HAL_clk_ClockAPIType;


/*
 * HAL_clk_ClockDomainAPIType
 *
 * Programming interfaces available for controlling clock domains.  Used when the
 * driver needs to verify if the functionality is available for a given
 * clock domain.  See HAL_clk_IsClockDomainSupported.
 */
typedef enum
{
  HAL_CLK_CLOCK_DOMAIN_API_ISON,

  HAL_ENUM_32BITS(CLK_DOMAIN_API),
} HAL_clk_ClockDomainAPIType;

/*
 * HAL_clk_PowerDomainAPIType
 *
 * Programming interfaces available for controlling power domains.  Used when the
 * driver needs to verify if the functionality is available for a given
 * power domain.  See HAL_clk_IsPowerDomainSupported.
 */
typedef enum
{
  HAL_CLK_POWER_DOMAIN_API_ENABLE,
  HAL_CLK_POWER_DOMAIN_API_DISABLE,
  HAL_CLK_POWER_DOMAIN_API_ISENABLED,
  HAL_CLK_POWER_DOMAIN_API_ISON,
  HAL_CLK_POWER_DOMAIN_API_ENABLE_HW,
  HAL_CLK_POWER_DOMAIN_API_DISABLE_HW,

  HAL_ENUM_32BITS(CLK_POWER_DOMAIN_API)
} HAL_clk_PowerDomainAPIType;


/*
 * HAL_clk_ClockMuxConfigType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  eSource  - The source to use.
 *  nDiv2x   - The integer (2 * divider) value to use.
 *  nM       - The M value for any M/N counter, or 0 to bypass.
 *  nN       - The N value for any M/N counter.
 *  nDFSIdx  - The DFS index this config is used for. 0xFF if not used in DFS.
 */
typedef struct
{
  uint8   nMuxSel;
  uint8   nDiv2x;
  uint16  nM;
  uint16  nN;
  uint8   nDFSIdx;
} HAL_clk_ClockMuxConfigType;


/*
 * HAL_clk_SourceConfigType
 *
 * Configuration enumerations for sources.
 *
 */
typedef enum
{
  HAL_CLK_CONFIG_SOURCE_NULL = 0,

  HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE,
  HAL_CLK_CONFIG_PLL_FSM_MODE_DISABLE,
  HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_TEST_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_TEST_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_EVEN_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_EVEN_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_ODD_OUTPUT_ENABLE,
  HAL_CLK_CONFIG_PLL_ODD_OUTPUT_DISABLE,
  HAL_CLK_CONFIG_PLL_DROOP_DETECTION_DISABLE,
  HAL_CLK_CONFIG_PLL_DROOP_DETECTION_ENABLE,
  HAL_CLK_CONFIG_PLL_OUT_INV_EN_DISABLE,
  HAL_CLK_CONFIG_PLL_OUT_INV_EN_ENABLE,
  HAL_CLK_CONFIG_PLL_MASTER_EN_DISABLE,
  HAL_CLK_CONFIG_PLL_MASTER_EN_ENABLE,
  HAL_CLK_CONFIG_PLL_BIST_EN_DISABLE,
  HAL_CLK_CONFIG_PLL_BIST_EN_ENABLE,

  HAL_CLK_CONFIG_XO_CHIP_BUFFER_USING_EXT_CLK_SRC,
  HAL_CLK_CONFIG_XO_USING_EXT_CRYSTAL,
  HAL_CLK_CONFIG_XO_GAIN_0,
  HAL_CLK_CONFIG_XO_GAIN_1,
  HAL_CLK_CONFIG_XO_GAIN_2,
  HAL_CLK_CONFIG_XO_GAIN_3,
  HAL_CLK_CONFIG_XO_GAIN_4,
  HAL_CLK_CONFIG_XO_GAIN_5,
  HAL_CLK_CONFIG_XO_GAIN_6,
  HAL_CLK_CONFIG_XO_GAIN_7,

  HAL_CLK_CONFIG_XO_DOUBLER_DCC,

  HAL_ENUM_32BITS(CLK_SOURCE_CONFIG)
} HAL_clk_SourceConfigType;


/*
 * HAL_clk_PLLPostDivType
 *
 * PLL-specific post dividers.
 */
typedef union
{
  struct
  {
    uint8 nEven;
    uint8 nOdd;
  } Fabia;

  struct
  {
    uint8 nMain;
  } Renovo;

  struct
  {
    uint8 nMain;
  } Agera;

} HAL_clk_PLLPostDivType;


/*
 * HAL_clk_PLLFracModeType
 *
 * PLL fractional modes.
 */
typedef enum
{
  HAL_CLK_PLL_FRAC_MODE_UNKNOWN,
  HAL_CLK_PLL_FRAC_MODE_ALPHA,
  HAL_CLK_PLL_FRAC_MODE_MN,
} HAL_clk_PLLFracModeType;


/*
 * HAL_clk_PLLConfigType
 *
 * Parameters used for configuring a source that is a PLL.
 *
 *  nSource               - The source to use for the PLL.
 *  nMode                 - The PLL mode to use.
 *  nPreDiv               - The pre-divider value (generally 1 or 2).
 *  nL                    - The L value for the PLL.  The PLL output frequency is derived
 *                          as out_freq = ((in_freq / nPreDiv) * (L + M/N)) / nPostDiv.
 *  nFracVal              - Fractional value, interpreted based on eFracMode.
 *  eFracMode             - Fractional mode.
 *  PostDivs              - Post dividers.
 */
typedef struct
{
  uint32                    nSource;
  uint32                    nMode;
  uint32                    nPreDiv;
  uint32                    nL;
  uint32                    nFracVal;
  HAL_clk_PLLFracModeType   eFracMode;
  HAL_clk_PLLPostDivType    PostDivs;
} HAL_clk_PLLConfigType;


/*
 * HAL_clk_FootswitchConfigType
 *
 *  CORE_FORCE_ON         - Force the core to remain on when the clock
 *                          is disabled.  This is generally the default.
 *  CORE_ALLOW_OFF        - Allow the core logic to turn off when the
 *                          core clock turns off.
 *  PERIPHERAL_FORCE_ON   - Force the peripheral logic to remain on
 *                          when the clock is disabled.
 *  PERIPHERAL_ALLOW_OFF  - Allow the peripheral logic to turn off
 *                          when the core clock turns off.  This is
 *                          generally the default.
 *  PERIPHERAL_FORCE_OFF  - Force the peripheral logic to remain off
 *                          even when the clock is on.
 *  PERIPHERAL_ALLOW_ON   - Allow the peripheral logic to turn on
 *                          when the clock is on.  This is
 *                          generally the default.
 */
typedef enum
{
  HAL_CLK_FOOTSWITCH_CONFIG_CORE_FORCE_ON,
  HAL_CLK_FOOTSWITCH_CONFIG_CORE_ALLOW_OFF,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_ON,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_ALLOW_OFF,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_FORCE_OFF,
  HAL_CLK_FOOTSWITCH_CONFIG_PERIPHERAL_ALLOW_ON,
  HAL_ENUM_32BITS(CLK_FOOTSWITCH_CONFIG)
} HAL_clk_FootswitchConfigType;


/*
 * HAL_clk_Context
 *
 * HAL clock driver context data.
 *
 *  BusyWait      - Pointer to busy wait function.
 *                  Assumption: Will be implemented by the top level driver
 *                  whether in DAl or via clk_busy_wait. If this function
 *                  pointer is NULL then HAL modules that require a
 *                  wait/pause may fail - this will be functional failure
 *                  and not compile time failure.
 *
 *  nChipVersion  - Version of the chip. Compare against
 *                  HAL_CLK_CHIP_VERSION(major, minor).
 *  nChipId       - ID of the chip.
 *  nChipFamily   - Family of the chip.
 */
typedef struct
{
  void (*BusyWait) ( uint32 uSeconds );
  uint32 nChipVersion;
  uint32 nChipId;
  uint32 nChipFamily;
} HAL_clk_ContextType;


/*
 * HAL_clk_SourceConfigMode
 *
 *  This enumeration defines the behavior when configuring a PLL.
 *
 *  NORMAL
 *   - Configure the PLL using every field in the source config
 *     structure. This is also known as legacy mode config.
 *
 *  SLEW
 *   - Dynamically re-configure the PLL using only the L/Alpha/AlphaU
 *     fields in the source config structure.  This config mode cannot
 *     re-configure to a frequency that requires a VCO change.
 *
 *  PENDING
 *   - Same as NORMAL mode except we do not trigger the re-configuration
 *     since we expect the HW to latch on to the new configuration
 *     values upon being re-enabled by HW (e.g. SPM).  This mode is
 *     useful for the case where the PLL must be configured to be
 *     enabled by HW at the calibration frequency when exiting sleep.
 */
typedef enum
{
  HAL_CLK_SOURCE_CONFIG_MODE_NORMAL,
  HAL_CLK_SOURCE_CONFIG_MODE_SLEW,
  HAL_CLK_SOURCE_CONFIG_MODE_PENDING,

  HAL_CLK_NUM_OF_SOURCE_CONFIG_MODES,

  HAL_ENUM_32BITS(CLK_SOURCE_CONFIG_MODES)
} HAL_clk_SourceConfigMode;


/*
 * HAL_clk_SourceDisableModeType
 *
 *  This enumeration defines the behavior when enabling/disabling a PLL.
 *
 *  NORMAL
 *    - Turns the BIAS circuitry off.
 *    - Perform a normal bringup/shutdown of the PLL.
 *
 *  STANDBY
 *    - Leaves the BIAS circuitry on.
 *    - The saved calibration results in faster enable times at the expense of
 *      higher power usage.
 *
 *  SAVE
 *    - Turns the BIAS circuitry off.
 *    - Save the PLL calibration data to non-volatile storage.
 *      The calibration data is restored when enabling the PLL to skip the
 *      calibration step.
 *
 *  FREEZE
 *    - Disable the PLL output.
 *    - Set the PLL freeze bit.
 *    - Calibration is maintained.
 */
typedef enum
{
  HAL_CLK_SOURCE_DISABLE_MODE_NORMAL            = 0,
  HAL_CLK_SOURCE_DISABLE_MODE_STANDBY           = 1,
  HAL_CLK_SOURCE_DISABLE_MODE_SAVE              = 2,
  HAL_CLK_SOURCE_DISABLE_MODE_FREEZE            = 3,
  HAL_ENUM_32BITS(CLK_SOURCE_DISABLE_MODES)
} HAL_clk_SourceDisableModeType;


/*
 * HAL_clk_VRegLevelType
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
  HAL_CLK_VREG_LEVEL_OFF              = 0,
  HAL_CLK_VREG_LEVEL_RETENTION_MINUS  = HAL_CLK_VREG_LEVEL_OFF,
  HAL_CLK_VREG_LEVEL_RETENTION        = 16,
  HAL_CLK_VREG_LEVEL_RETENTION_PLUS   = HAL_CLK_VREG_LEVEL_RETENTION,
  HAL_CLK_VREG_LEVEL_LOW_MIN          = 48,
  HAL_CLK_VREG_LEVEL_LOW_MINUS        = 64,
  HAL_CLK_VREG_LEVEL_LOW              = 128,
  HAL_CLK_VREG_LEVEL_LOW_PLUS         = 192,
  HAL_CLK_VREG_LEVEL_NOMINAL_MINUS    = HAL_CLK_VREG_LEVEL_LOW_PLUS,
  HAL_CLK_VREG_LEVEL_NOMINAL          = 256,
  HAL_CLK_VREG_LEVEL_NOMINAL_PLUS     = 320,
  HAL_CLK_VREG_LEVEL_HIGH_MINUS       = 336,
  HAL_CLK_VREG_LEVEL_HIGH             = 384,
  HAL_CLK_VREG_LEVEL_HIGH_PLUS        = HAL_CLK_VREG_LEVEL_HIGH,
  HAL_CLK_VREG_LEVEL_TURBO_MINUS      = 416,
  HAL_CLK_VREG_LEVEL_TURBO            = HAL_CLK_VREG_LEVEL_TURBO_MINUS,
  HAL_CLK_VREG_LEVEL_TURBO_PLUS       = HAL_CLK_VREG_LEVEL_TURBO,

  HAL_CLK_VREG_MAX_LEVEL              = HAL_CLK_VREG_LEVEL_TURBO_PLUS,
  HAL_CLK_VREG_NUM_LEVELS             = 17,

  HAL_ENUM_32BITS(CLK_VREG_LEVEL)
} HAL_clk_VRegLevelType;


/*============================================================================

               FUNCTION DECLARATIONS FOR MODULE

============================================================================*/


/* ============================================================================
**    Mandatory Functions
** ==========================================================================*/

void    HAL_clk_Init                    ( HAL_clk_ContextType *pHALClkCtxt );

void    HAL_clk_EnableClock             ( HAL_clk_ClockDescType *pmClockDesc );
void    HAL_clk_DisableClock            ( HAL_clk_ClockDescType *pmClockDesc );
void    HAL_clk_ConfigClock             ( HAL_clk_ClockDescType *pmClockDesc, HAL_clk_ClockConfigType eConfig );
void    HAL_clk_ConfigClockDivider      ( HAL_clk_ClockDescType *pmClockDesc, uint32 nDiv );
void    HAL_clk_ConfigFootswitch        ( HAL_clk_ClockDescType *pmClockDesc, HAL_clk_FootswitchConfigType eFSConfig );
void    HAL_clk_ResetClock              ( HAL_clk_ClockDescType *pmClockDesc, boolean bAssert );
void    HAL_clk_ResetClockBranch        ( HAL_clk_ClockDescType *pmClockDesc, boolean bAssert );
boolean HAL_clk_IsClockSupported        ( HAL_clk_ClockDescType *pmClockDesc, HAL_clk_ClockAPIType eAPI );
boolean HAL_clk_IsClockEnabled          ( HAL_clk_ClockDescType *pmClockDesc );
boolean HAL_clk_IsClockOn               ( HAL_clk_ClockDescType *pmClockDesc );
boolean HAL_clk_IsClockReset            ( HAL_clk_ClockDescType *pmClockDesc );
void    HAL_clk_InvertClock             ( HAL_clk_ClockDescType *pmClockDesc, boolean bInvert );

void    HAL_clk_CalcClockFreq           ( HAL_clk_ClockDescType *pmClockDesc, uint32 *pnFreqHz );
void    HAL_clk_DetectClockConfig       ( HAL_clk_ClockDescType *pmClockDesc, uint32 *pnConfig, boolean *pbValid );
void    HAL_clk_DetectClockDivider      ( HAL_clk_ClockDescType *pmClockDesc, uint32 *pnDiv );

void    HAL_clk_ConfigClockMux          ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, const HAL_clk_ClockMuxConfigType *pmConfig );
void    HAL_clk_DetectClockMuxConfig    ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig );
boolean HAL_clk_IsClockDomainOn         ( HAL_clk_ClockDomainDescType *pmClockDomainDesc );
boolean HAL_clk_IsClockDomainSupported  ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, HAL_clk_ClockDomainAPIType eAPI );
boolean HAL_clk_IsClockMuxConfigEqual   ( const HAL_clk_ClockMuxConfigType *pConfigA, const HAL_clk_ClockMuxConfigType *pConfigB );
void    HAL_clk_UpdateRootEnable        ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, boolean bAssert );
void    HAL_clk_EnableDFS               ( HAL_clk_ClockDomainDescType *pmClockDomainDesc );
boolean HAL_clk_IsClockDomainDFSEnabled ( HAL_clk_ClockDomainDescType *pmClockDomainDesc );
void    HAL_clk_ConfigDFSMux            ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig );
void    HAL_clk_DetectDFSMuxConfig      ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig, uint32 nPerfLevel );

boolean HAL_clk_EnableSource            ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
void    HAL_clk_DisableSource           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
boolean HAL_clk_EnableSourceVote        ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_DisableSourceVote       ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_ConfigSource            ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
boolean HAL_clk_IsSourceEnabled         ( HAL_clk_SourceDescType *pSource );
boolean HAL_clk_IsSourceVoteEnabled     ( HAL_clk_SourceDescType *pSource );
boolean HAL_clk_ConfigPLL               ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode );
void    HAL_clk_DetectPLLConfig         ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
boolean HAL_clk_IsPLLConfigEqual        ( const HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pConfigA, const HAL_clk_PLLConfigType *pConfigB );

void    HAL_clk_EnablePowerDomain       ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
void    HAL_clk_DisablePowerDomain      ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
void    HAL_clk_EnablePowerDomainHW     ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
void    HAL_clk_DisablePowerDomainHW    ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
boolean HAL_clk_IsPowerDomainSupported  ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc,  HAL_clk_PowerDomainAPIType eAPI );
boolean HAL_clk_IsPowerDomainEnabled    ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
boolean HAL_clk_IsPowerDomainOn         ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );

void HAL_clk_ConfigDivider              ( HAL_clk_DividerDescType *pmDividerDesc, uint32 nDiv );

uint32  HAL_clk_GetChipVersion          ( void );
uint32  HAL_clk_GetChipId               ( void );
uint32  HAL_clk_GetChipFamily           ( void );

boolean HAL_clk_ValidateRegSetTimeout   ( uintnt nAddr, uint32 nMask, uint32 nTimeoutUS );
boolean HAL_clk_ValidateRegClearTimeout ( uintnt nAddr, uint32 nMask, uint32 nTimeoutUS );

void HAL_clk_ConfigDebugMux             ( HAL_clk_DebugMuxDescType *pmDebugMux, uint32 nMuxSel );
void HAL_clk_CalcDebugMuxFreq           ( HAL_clk_DebugMuxDescType *pmDebugMux, uint32 *pnFreqHz );

/*
==============================================================================

               DOCUMENTATION FOR MODULE

==============================================================================

HAL_clk_Init( )

Description:
  Initialize the clock HAL data structures and clock hardware.

==============================================================================

HAL_clk_EnableClock( )

Description:
  Enable the given clock.  Note that the source should be enabled before
  invoking this function.

Parameters:
  eClock - The clock to enable.

==============================================================================

HAL_clk_DisableClock( )

Description:
  Disable the given clock.

Parameters:
  eClock - The clock to disable.

==============================================================================

HAL_clk_ConfigClock( )

Description:
  Configures the given clock to the given configuration.  Typically the
  configuration will be a clock rate.  The argument is a clock-dependent
  enumeration.

Parameters:
  eClock  - The clock to configure.
  nConfig - The configuration to use (i.e. HAL_clk_UARTConfigType)

==============================================================================

HAL_clk_ConfigClockDivider( )

Description:
  Configures the given clock with the given divider value.

Parameters:
  nDiv     - The divider value to use (in mathematical notation as opposed
             to raw register field value, i.e. "1" = div-1)

==============================================================================

HAL_clk_ConfigFootswitch( )

Description:
  Configures the given clock with the given footswitch setting.

Parameters:
  eFSConfig - Footswitch configuration.

==============================================================================

HAL_clk_ResetClock( )

Description:
  Asserts or de-asserts the reset signal to the given clock hardware block.

Parameters:
  pmClockDesc  - The handle of the clock to reset.
  bAssert      - Whether to assert or de-assert the reset signal.

==============================================================================

HAL_clk_IsClockSupported( )

Description:
  Checks if the given clock API is supported for the given clock.

Parameters:
  pmClockDesc  - The handle of the clock in question.
  eAPI         - Which API to verify support of.

Return:
  boolean - TRUE if the clock API is supported.

==============================================================================

HAL_clk_IsClockEnabled( )

Description:
  Checks if the given clock is enabled.  Note that this simply means the
  HW gate is enabled, not that the clock is actually toggling.  To see if
  the clock is active, use IsClockOn.

Parameters:
  pmClockDesc - The handle of the clock to check.

Return:
  boolean - TRUE if the clock is enabled.

==============================================================================

HAL_clk_IsClockOn( )

Description:
  Checks if the given clock is running.  If the clock source is not active
  then even if the clock is enabled it may not be "on".

Parameters:
  pmClockDesc - The handle of the clock to check.

Return:
  boolean - TRUE if the clock is on.

==============================================================================

HAL_clk_IsClockReset( )

Description:
  Checks if the given clock is in reset.

Parameters:
  pmClockDesc - The handle of the clock to check.

Return:
  boolean - TRUE if the clock is in reset.

==============================================================================

HAL_clk_InvertClock( )

Description:
  Inverts the clock signal.

Parameters:
  pmClockDesc  - The handle of the clock to check.
  bInvert      - TRUE to invert the clock, FALSE to remove inversion.

==============================================================================

HAL_clk_CalcClockFreq( )

Description:
  This function calculates the current clock frequency for the given clock.
  This is done using internal loop back clock test registers which may not
  be supported on a particular target.

Parameters:
  pmClockDesc  - The handle of the clock to check.
  pnFreqHz     - The calculated clock frequency in Hz.

Return:
  See pnFreqHz.

==============================================================================

HAL_clk_DetectClockConfig( )

Description:
  Determines the current clock configuration based on the hardware
  settings.  Note that most clocks probably do not implement this
  functionality.

Parameters:
  pmClockDesc - The handle of the clock to check.

Return:
  pnConfig - The determined configuration.
  pbValid  - Whether or not the configuration was successfully determined.
             If the current hardware state is not in a supported
             configuration this will be FALSE and pnConfig should be
             ignored.

==============================================================================

HAL_clk_DetectClockDivider( )

Description:
  Determines the current clock configuration based on the hardware
  settings.  Note that most clocks probably do not implement this
  functionality.

Parameters:
  pmClockDesc - The handle of the clock to check.

Return:
  pnDiv - The determined divider value.


==============================================================================

HAL_clk_SaveClockState( )

Description:
  Saves the current state of the clocks.  Does not save source or rail state.
  Is only guaranteed to save the state that is lost during
  HAL_clk_ResetClockState, not necessarily lost during power collapse.
  This function is intended to be used before TCXO shutdown when all active
  clocks must be disabled via HAL_clk_ResetClockState.

Parameters:
  None

Return:
  None

See Also:
  HAL_clk_Save, HAL_clk_ResetClockState, HAL_clk_RestoreClockState

==============================================================================

HAL_clk_ResetClockState( )

Description:
  Resets (disables) the clocks.  Does not touch source or rail state.
  This function is intended to be used before TCXO shutdown when all active
  clocks must be disabled.

Parameters:
  None

Return:
  None

See Also:
  HAL_clk_SaveClockState, HAL_clk_RestoreClockState

==============================================================================

HAL_clk_RestoreClockState( )

Description:
  Restores the clock state saved with HAL_clk_SaveClockState.
  This function is intended to be used after TCXO shutdown to restore clocks
  disabled with HAL_clk_ResetClockState.

Parameters:
  None

Return:
  None

See Also:
  HAL_clk_SaveClockState, HAL_clk_ResetClockState

==============================================================================

HAL_clk_ClockSleepEnable()

Description:
  Enables sleep register for the given clock.  This will halt the clock when
  corresponding processor goes to SWFI/halt state.

Parameters:
  pmClockDesc - The handle of the clock to check.

Return:
  None

==============================================================================

HAL_clk_ClockSleepReset()

Description:
  Resets sleep registers.  This function will vary based on the target since
  sleep registers are target specific.

Parameters:
  None

Return:
  None

==============================================================================

HAL_clk_ConfigClockMux

Description:
  Configures clock MUX for the given clock domain.

Parameters:
  pmClockDomainDesc  - The handle to clock domain to configure.
  pmConfig           - The configuration to use.

Return:
  None

==============================================================================

HAL_clk_DetectClockMuxConfig

Description:
  Detects clock MUX configuration for the given clock domain.

Parameters:
  pmClockDomainDesc - The handle to clock domain to check

Return:
  pmConfig - The determined configuration

==============================================================================

HAL_clk_IsClockDomainSupported

Description:
  Checks if the given clock domain is supported.

Parameters:
  pmClockDomainDesc - The handle to clock domain to check

Return:
  boolean - TRUE if the clock domain is supported.

==============================================================================

HAL_clk_IsClockDomainOn

Description:
  Specifies if the clock domain is on or not.

Parameters:
  pmClockDomainDesc - The handle to clock domain to check

Return:
  boolean - TRUE if the clock domain is on, FALSE otherwise.

==============================================================================


HAL_clk_EnableSource( )

Description:
  Enable the given source.  Does nothing if the source is not supported.

Parameters:
  eSource - The source to enable.
  eMode   - Mode used to enable the source
  pData   - Pointer to data depending on eMode used.

Return:
  boolean - TRUE if the source successfully locked and is enabled.

==============================================================================

HAL_clk_DisableSource( )

Description:
  Disable the given source.  Does nothing if the source is not supported.

Parameters:
  eSource - The source to disable.
  eMode   - Mode used to disable the source
  pData   - Pointer to data depending on eMode used.

==============================================================================

HAL_clk_EnableSourceVote( )

Description:
  Enables the given source vote.

Parameters:
  eSource - The source to enable vote for.

Return:
  boolean - TRUE if the source successfully locked and is enabled.

==============================================================================

HAL_clk_DisableSourceVote( )

Description:
  Disables the given source vote.

Parameters:
  eSource - The source to disable vote for.

==============================================================================

HAL_clk_ConfigSource( )

Description:
  Configures the given source to the given configuration.

Parameters:
  eSource - The source to configure.
  nConfig - The generic configuration to use, the actual type will depend
            on the type of source being configured.

==============================================================================

HAL_clk_IsSourceEnabled( )

Description:
  Checks if the given source is enabled.

Parameters:
  eSource - The source to check.

Return:
  boolean - TRUE if the source is enabled.

==============================================================================

HAL_clk_IsSourceVoteEnabled( )

Description:
  Checks if the given source vote is enabled.

Parameters:
  eSource - The source to check.

Return:
  boolean - TRUE if the source vote is enabled.

==============================================================================

HAL_clk_ConfigPLL( )

Description:
  Configures the given source, which must be a PLL, to the given
  configuration.

Parameters:
  eSource  - The source to configure (must be a PLL).
  pmConfig - The configuration to use.

Return:
  boolean - TRUE if the source was configured according to the parameters.
            Also, if a config parameter requested slewing then this return
            value indicates whether the source locked successfully.

==============================================================================

HAL_clk_DetectPLLConfig( )

Description:
  Detects the current configuration of the given PLL source.  The eSource
  field of the result will be set to HAL_CLK_SOURCE_NULL on failure.

Parameters:
  eSource  - The source whose configuration to detect (must be a PLL).
  pmConfig - The detected configuration.

==============================================================================

HAL_clk_EnablePowerDomain( )

Description:
  Enable the given power domain.  Does nothing if the power domain is not
  supported. Note that this is merely the clock portion of the power domain
  control.  Generally bus and possibly PMIC interactions will also be required.

Parameters:
  pmPowerDomainDesc - The handle to the power domain to enable.

==============================================================================

HAL_clk_DisablePowerDomain( )

Description:
  Disable the given power domain.  Does nothing if the power domain is not
  supported.  Note that this is merely the clock portion of the power domain
  control.  Generally bus and possibly PMIC interactions will also be required.

Parameters:
  pmPowerDomainDesc - The handle to the power domain to disable.

==============================================================================

HAL_clk_IsPowerDomainSupported( )

Description:
  Checks if the given power domain is supported.

Parameters:
 pmPowerDomainDesc - The handle to the power domain to check.

Return:
  boolean - TRUE if the power domain is supported on this target.

==============================================================================

HAL_clk_IsPowerDomainEnabled( )

Description:
  Checks if the given power domain is enabled.

Parameters:
  pmPowerDomainDesc - The handle to the power domain to check.

Return:
  boolean - TRUE if the power domain is enabled.

==============================================================================

HAL_clk_IsPowerDomainOn( )

Description:
  Checks if the given power domain is on.

Parameters:
  pmPowerDomainDesc - The handle to the power domain to check.

Return:
  boolean - TRUE if the power domain is on.

==============================================================================

HAL_clk_EnablePowerDomainHW( )

Description:
  Enable the given power domain using a HW control option.

Parameters:
  pmPowerDomainDesc - The handle to the power domain to enable.

==============================================================================

HAL_clk_DisablePowerDomainHW( )

Description:
  Disable the given power domain using a HW control option.

Parameters:
  pmPowerDomainDesc - The handle to the power domain to disable.

==============================================================================

HAL_clk_GetNextClockInDomain ( )

Description:
  Returns a pointer to the queried (based on nClockCount) clock of this domain.

Parameters:
  pmClockDomainDesc  - The handle to the clock domain.
  nClockIndex        - The clock for which to get HAL handle for.

Return:
  pmClockDesc - The handle to the queried (based on nClockCount) clock of this domain.

==============================================================================

HAL_clk_GetNextClockDomain( )

Description:
  Returns a pointer to the next (or first) clock domain.

Parameters:
  pmClockDomainDesc - The handle to the clock domain.

Return:
  pNextClockDomainHandle - The handle to the next (or first) clock domain.

==============================================================================

HAL_clk_GetNextPowerDomain( )

Description:
  Returns a pointer to the next (or first) power domain.

Parameters:
  pmPowerDomainDesc - The handle to the power domain.

Return:
  pNextPowerDomainHandle - The handle to the next (or first) power domain.

==============================================================================

HAL_clk_GetClockName( )

Description:
  Returns a name of the given clock.

Parameters:
  pmClockDesc - The handle to the clock.

Return:
  szClockName  - The name of the clock.

==============================================================================

HAL_clk_GetPowerDomainName( )

Description:
  Returns the name of the given power domain.

Parameters:
  pmPowerDomainDesc - The handle to the power domain.

Return:
  szPowerDomainName  - The name of the power domain.

==============================================================================

HAL_clk_GetChipVersion( )

Description:
  Returns the chip version.

==============================================================================

HAL_clk_GetChipId( )

Description:
  Returns the chip id.

==============================================================================

HAL_clk_GetChipFamily( )

Description:
  Returns the chip family.

==============================================================================

HAL_clk_ValidateRegSetTimeout( )

Description:
  Returns whether the register field got set within the timeout.

Parameters:
  nAddr - the register address
  nMask - the mask of the register field being set.
  nTimeoutUS - the timeout while waiting for the register field to set.

Return:
  boolean - TRUE if the register field set
          - FALSE if the register field didn't set before the timeout.

==============================================================================

HAL_clk_ValidateRegClearTimeout( )

Description:
  Returns whether the register field got cleared within the timeout.

  nAddr - the register address
  nMask - the mask of the register field being cleared.
  nTimeoutUS - the timeout while waiting for the register field to clear.

Return:
  boolean - TRUE if the register field cleared
          - FALSE if the register field didn't clear before the timeout.

==============================================================================

*/

#endif /* !HALCLK_H */

