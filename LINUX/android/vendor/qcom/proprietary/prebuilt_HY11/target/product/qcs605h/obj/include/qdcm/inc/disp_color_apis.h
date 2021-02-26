/*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
*/

#ifndef __DISP_COLOR_APIS_H__
#define __DISP_COLOR_APIS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/types.h>

#define DISP_APIS_OFFSET 0
enum DISP_APIS_PUBLIC {
  DISP_APIS_INIT = 0x0,
  DISP_APIS_DEINIT,
  DISP_APIS_GET_NUM_DISPLAYS,
  DISP_APIS_GET_DISPLAY_INFO,
  DISP_APIS_GET_GLOBAL_COLOR_BALANCE_RANGE,
  DISP_APIS_SET_GLOBAL_COLOR_BALANCE,
  DISP_APIS_GET_GLOBAL_COLOR_BALANCE,
  DISP_APIS_GET_NUM_DISPLAY_MODES,
  DISP_APIS_GET_DISPLAY_MODES,
  DISP_APIS_GET_ACTIVE_DISPLAY_MODE,
  DISP_APIS_SET_ACTIVE_DISPLAY_MODE,
  DISP_APIS_SAVE_DISPLAY_MODE,
  DISP_APIS_DELETE_DISPLAY_MODE,
  DISP_APIS_SET_DEFAULT_DISPLAY_MODE,
  DISP_APIS_GET_DEFAULT_DISPLAY_MODE,
  DISP_APIS_GET_ADAPT_BL_QL_LEVEL,
  DISP_APIS_SET_ADAPT_BL_QL_LEVEL,
  DISP_APIS_GET_ADAPT_BL_SCALE,
  DISP_APIS_GET_SVI_STRENGTH_RANGE,
  DISP_APIS_GET_SVI_STRENGTH,
  DISP_APIS_SET_SVI_STRENGTH,
  DISP_APIS_GET_GLOBAL_PA_RANGE,
  DISP_APIS_GET_GLOBAL_PA_CONFIG,
  DISP_APIS_SET_GLOBAL_PA_CONFIG,
  DISP_APIS_GET_FEATURE_VERSION,
  DISP_APIS_SET_USR_COLOR_BALANCE_CONFIG,
  DISP_APIS_GET_USR_COLOR_BALANCE_CONFIG,
  DISP_APIS_SET_USR_EBR_CONFIG,
  DISP_APIS_GET_USR_EBR_CONFIG,
  DISP_APIS_GET_PANEL_BRIGHTNESS_LEVEL_EXT,
  DISP_APIS_SET_PANEL_BRIGHTNESS_LEVEL_EXT,
  DISP_APIS_NUM_PUBLIC_APIS
};

typedef uint64_t DISPAPI_HANDLE;

#define MAX_MODE_NAME_LENGTH 64
#define PANEL_NAME_LENGTH 256

/*
The following macros are used to identify success or error conditions
  DISP_API_SUCCESS          -- No errors
  DISP_API_ERR_PERM_DENIED  -- Error : unable to process the request
  DISP_API_ERR_INVALID_PARM -- Error : Invalid Paramter
  DISP_API_ERR_NOT_SUPPORTED-- Error : Not Supported
  DISP_API_ERR_TIMEOUT      -- Error : Call timed out, device busy
  DISP_API_ERR_UNDEFINED    -- Error : Undefined or unknown parameter specified
  DISP_API_ERR_NOTACTIVE    -- Error : Functionality is not enabled or active
*/
#define DISP_API_SUCCESS 0
#define DISP_API_ERR_PERM_DENIED -1
#define DISP_API_ERR_INVALID_PARM -2
#define DISP_API_ERR_NOT_SUPPORTED -3
#define DISP_API_ERR_TIMEOUT -4
#define DISP_API_ERR_UNDEFINED -5

/*
struct disp_range -- Generic structure for defining supported ranges as intergers
  max_val  -- Maximum supported value
  min_val  -- Minimum supported value
  step_val -- Step (increment) value for parameter setting.
*/
struct disp_range {
  int32_t max_val;
  int32_t min_val;
  uint32_t step_val;
};

/*
struct disp_range_float -- Generic structure for defining supported ranges as floats
  max_val  -- Maximum supported value
  min_val  -- Minimum supported value
  step_val -- Step (increment) value for parameter setting.
*/
struct disp_range_float {
  float max_val;
  float min_val;
  float step_val;
};

/*
This enum includes all the APIs supported.
  DISP_GLOBAL_COLOR_BALANCE_API -- Color Balance
  DISP_COLOR_MODE_SEL_API       -- Color Mode Selection : Active mode, Number of Modes, etc
  DISP_COLOR_MODE_MANAGEMENT_API-- Color Mode Management : Create/update, Save/delete
  DISP_ADAPTIVE_BACKLIGHT_API   -- Adaptive Backlight
  DISP_GLOBAL_PICT_ADJ_API      -- Global Picture Adjustment
  DISP_GLOBAL_MEM_COLOR_ADJ_API -- Global Memory Color
  DISP_SUNLIGHT_VISIBILITY_API  -- Sunlight Visibility
  DISP_SOURCE_COLOR_INFO_API    -- Source Color Information
*/
typedef enum {
  DISP_API_GLOBAL_COLOR_BALANCE_API = 0,
  DISP_API_COLOR_MODE_SEL_API,
  DISP_API_COLOR_MODE_MANAGEMENT_API,
  DISP_API_ADAPTIVE_BACKLIGHT_API,
  DISP_API_GLOBAL_PICT_ADJ_API,
  DISP_API_GLOBAL_MEM_COLOR_ADJ_API,
  DISP_API_SUNLIGHT_VISIBILITY_API,
  DISP_API_SOURCE_COLOR_INFO_API,
  DISP_API_MAX,
  DISP_API_FORCE32BIT = 0x7FFFFFFF
} display_api_type;

/*
This enum is used as part of the display mode management, it is used to describe the display mode
types
  DISP_MODE_OEM  -- Mode is created by OEM (device vendor), these modes cannot be deleted
                    or modified
  DISP_MODE_USER -- Mode is creted by the device user, these modes can be deleted or modified.
  DISP_MODE_ALL  -- Represents all the possible type of modes.
*/
typedef enum {
  DISP_MODE_OEM = 0,
  DISP_MODE_USER,
  DISP_MODE_ALL,
  DISP_MODE_MAX,
  DISP_MODE_FORCE32BIT = 0x7FFFFFFF
} disp_mode_type;

/*
This enum is used as part of the Adaptive Backlight management, it is used to express the level of
processing to be applied
  ADAPT_BL_QUAL_LOW     -- Apply a low level of processing, this level the most power savings
  but with the most quality degradation
  ADAPT_BL_QUAL_MEDIUM  -- Apply a mediem level of processing, this level is a mid-point between
                           the Low and High Level
  ADAPT_BL_QUAL_LOW     -- Apply a high level of processing, this level the least power savings
                           but with the least quality degradation
  ADAPT_BL_QUAL_AUTO    -- Automatically applies different processing between YUV (video) and
                           RGB content.  For YUV content the Low level will be applied,
                           for all other content the High level will be applied.
*/
typedef enum {
  ADAPT_BL_QUAL_LOW = 0,
  ADAPT_BL_QUAL_MEDIUM,
  ADAPT_BL_QUAL_HIGH,
  ADAPT_BL_QUAL_AUTO,
  ADAPT_BL_QUAL_MAX,
  ADAPT_BL_QUAL_FORCE32BIT = 0x7FFFFFFF
} disp_adapt_bl_ql_level;

/*
struct disp_mode -- structure for display color mode
  id        -- Mode Identifier, zero based
  name      -- Name of the mode, UTF-8 encoding
  name_len  -- Length of the mode name
  type      -- Mode type : OEM or User defined color mode
*/
struct disp_mode {
  int32_t id;
  disp_mode_type type;
  uint32_t name_len;
  char *name;
};

/*
struct disp_pa_config_data -- structure for storing global PA data
  hue        -- Hue setting in units degrees
  saturation -- Saturation setting in units of percentage
  value      -- Value setting in units of percentage
  contrast   -- Contrast setting in units of percentage
  sat_thresh -- Saturation Threshold in units of percentage
*/
struct disp_pa_config_data {
  int32_t hue;
  float saturation;
  float value;
  float contrast;
  float sat_thresh;
};

/*
struct disp_pa_config -- structure for storing global PA config
  flags -- Reserved
  data  -- PA parameter structure
*/
struct disp_pa_config {
  uint32_t flags;
  struct disp_pa_config_data data;
};

/*
struct disp_pa_range -- structure for getting the ranges of global PA data
  flags         -- Reserved
  hue           -- Supported range and step sizes for hue
  saturation    -- Supported range and step sizes for saturation
  value         -- Supported range and step sizes for value
  contrast      -- Supported range and step sizes for contrast
  sat_threshold -- Supported range and step sizes for sat_threshold
*/
struct disp_pa_range {
  uint32_t flags;
  struct disp_range hue;
  struct disp_range_float saturation;
  struct disp_range_float value;
  struct disp_range_float contrast;
  struct disp_range_float sat_threshold;
};

/*
This enum includes the value gamma values for source content definitions
  DISP_GAMMA_CUSTOM      -- Gamma value of custom defined
  DISP_GAMMA_1P0         -- Gamma value of 1.0
  DISP_GAMMA_2P2         -- Gamma value of 2.2
  DISP_GAMMA_2P6         -- Gamma value of 2.6
  DISP_GAMMA_REC601      -- Gamma value associate with REC601
  DISP_GAMMA_REC709      -- Gamma value associated with REC709
  DISP_GAMMA_SRGB        -- Gamma value associated with sRGB
*/
typedef enum {
  DISP_GAMMA_CUSTOM = 0,
  DISP_GAMMA_1P0,
  DISP_GAMMA_2P2,
  DISP_GAMMA_2P6,
  DISP_GAMMA_REC601,
  DISP_GAMMA_REC709,
  DISP_GAMMA_SRGB,
  DISP_GAMMA_MAX,
  DISP_GAMMA_FORCE32 = 0x7FFFFFFF
} disp_gamma_type;

/*
  GC_FLAGS_USE_8_BIT_ROUNDING_OUTPUT -- Enables 8-bit rounding at the PGC output.
*/
#define DISP_GC_FLAGS_USE_8_BIT_ROUNDING_OUTPUT 0x00000001
#define GC_ENTRIES_SIZE 1024
#define DISP_IGC_FLAGS_STRENGTH_FIELD_MASK 0x000F0000
#define DISP_IGC_FLAGS_STRENGTH_FIELD_SHIFT 16

/*
struct lut_info -- structure for defining the various igc or gc lut information
  flags         -- DISP_GC_FLAGS_USE_8_BIT_ROUNDING_OUTPUT for GC configuration
                -- DISP_IGC_FLAGS_STRENGTH_FIELD for IGC configuration
  num_entries   -- Identifies the number of lut values in the table
  gamma         -- Reserved. Must be set to 0x0
  entries       -- Table buffer holding the lut values
*/
struct lut_info {
  uint32_t flags;
  uint32_t num_entries;
  disp_gamma_type gamma;
  uint32_t r_entries[GC_ENTRIES_SIZE];
  uint32_t g_entries[GC_ENTRIES_SIZE];
  uint32_t b_entries[GC_ENTRIES_SIZE];
};

/*
struct pcc_coeff -- structure for providing the PCC (Polynomial Color Correction)
coefficients

    Rout = Rc + RcRin + RgGin + RbBin + RrrRinRin + RggGinGin + RbbRinRin +
           RrgRinGin + RgbGinBin + RrbRinBin + RrgbRinGinBin

    Gout = Gc + GcRin + GgGin + GbBin + GrrRinRin + GggGinGin + GbbRinRin +
           GrgRinGin + GgbGinBin + GrbRinBin + GrgbRinGinBin

    Bout = Bc + BcRin + BgGin + BbBin + BrrRinRin + BggGinGin + BbbRinRin +
           BrgRinGin + BgbGinBin + BrbRinBin + BrgbRinGinBin

  X = R, G or B
  c -- Xc coefficent
  r -- Xr coefficent
  g -- Xg coefficent
  b -- Xb coefficent
  rr -- Xrr coefficent
  gg -- Xgg coefficent
  bb -- Xbb coefficent
  rg -- Xrg coefficent
  gb -- Xgb coefficent
  rb -- Xrb coefficent
  rgb -- Xrgb coefficent
*/
struct pcc_coeff {
  double c;
  double r;
  double g;
  double b;
  double rr;
  double gg;
  double bb;
  double rg;
  double gb;
  double rb;
  double rgb;
};

/*
struct pcc_coeff_data -- structure for providing per color component coefficients
  flags -- Reserved
  r     -- Red component coefficents
  g     -- Green component coefficents
  b     -- Blue component coefficents
*/
struct pcc_coeff_data {
  uint32_t flags;
  struct pcc_coeff r;
  struct pcc_coeff g;
  struct pcc_coeff b;
};

/*
The following macros are to be used as define feature Identifers to be used
as part of the versioning queries.
  FEATURE_VER_SW_API                 -- For describing the version of the overall SW interface
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_PA_API              -- For describing the version of the PA support
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_MEMCOLOR_API        -- For describing the version of the Memory Color support
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_COLORBAL_API        -- For describing the version of the Color Balance support
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_SAVEMODES_API       -- For describing the version of the Save Modes support
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_ADAPT_BACKLIGHT_API -- For describing the version of the Adaptive Backlight support
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_SVI_API             -- For describing the version of the SVI support
                                        - 1.0.0 : Initial Release
  FEATURE_VER_SW_DISP_INFO_API  - For describing the version of the Display Info Support
                                        - 1.0.0 : Initial Release

 Note: The range 0xF0000000->0xFFFFFFFF is reserved.
*/
#define FEATURE_VER_SW_API 0x00000000
#define FEATURE_VER_SW_PA_API 0x00000001
#define FEATURE_VER_SW_MEMCOLOR_API 0x00000002
#define FEATURE_VER_SW_COLORBAL_API 0x00000003
#define FEATURE_VER_SW_SAVEMODES_API 0x00000004
#define FEATURE_VER_SW_ADAPT_BACKLIGHT_API 0x00000005
#define FEATURE_VER_SW_SVI_API 0x00000006
#define FEATURE_VER_SW_DISP_INFO_API 0x00000007

/*
The following macros are to be used as define feature Identifers to be used
as part of the versioning queries.

  FEATURE_VER_HW_IGC_GLOBAL     - For describing the version of the Global IGC HW
  FEATURE_VER_HW_GC_GLOBAL      - For describing the version of the Global GC HW
  FEATURE_VER_HW_PCC_GLOBAL     - For describing the version of the Global PCC HW
  FEATURE_VER_SW_IGC_GC_API     - For describing the version of the IGC\GC Support
  FEATURE_VER_SW_PCC_API        - For describing the version of the PCC Support
*/
#define FEATURE_VER_HW_IGC_GLOBAL 0xF000000D
#define FEATURE_VER_HW_GC_GLOBAL  0xF000000E
#define FEATURE_VER_HW_PCC_GLOBAL 0xF0000008
#define FEATURE_VER_SW_IGC_GC_API 0xF1000005
#define FEATURE_VER_SW_PCC_API    0xF1000007

/*
struct version_info -- structure for version information
  major -- Major version changes are not backward compatible.
  minor –- Expanded or deprecated functionality.  Minor version adds or removes
           functionality while maintaining backwards compatibility.
  step  -- Reserved, shall specify 0
*/
struct version_info {
  uint8_t major;
  uint8_t minor;
  uint16_t step;
};

/*
This enum includes the possible display interface types
  DISP_INTF_DSI0     -- DSI Interface 0
  DISP_INTF_DSI1     -- DSI Interface 1
  DISP_INTF_HDMI    -- HDMI Interface
  DISP_INTF_MHL     -- MHL Interface
  DISP_INTF_VIRTUAL -- Virtual : Memory buffer writeback
*/
typedef enum {
  DISP_INTF_DSI0 = 0,
  DISP_INTF_DSI1,
  DISP_INTF_HDMI,
  DISP_INTF_MHL,
  DISP_INTF_VIRTUAL,
  DISP_INTF_MAX,
  DISP_INTF_FORCE32 = 0x7FFFFFFF
} disp_intf;

/*
struct display_info_info -- structure for display information
  flags     -- Reserved
  layer_id  -- Unique and opaque identifier for the display.  This identifier will be
               used for all subsequent calls.
  width     -- Width of the layer in units of pixels
  height    -- Height of the layer in units of lines
  active_status -- State of display : 0 = disabled (OFF), 1 = Enabled (ON)
  intf      -- Display interface Type

*/
struct display_info {
  uint32_t flags;
  uint32_t disp_id;
  uint32_t width;
  uint32_t height;
  uint32_t active_status;
  uint32_t intf;
  char panel_name[PANEL_NAME_LENGTH];
};

/*
   DISP_USR_CB_FLAGS_USE_DEFAULT_MODE
      - Apply the disp_usr_color_balance_config on top of default color mode.
   DISP_USR_CB_FLAGS_USE_UPDATED_CB_IN_SAVE_MODE
      - Use any updates applied to the color-modules with this data in
        creating new mode, using disp_api_save_display_mode.
*/
#define DISP_USR_CB_FLAGS_USE_DEFAULT_MODE             0x00000001
#define DISP_USR_CB_FLAGS_USE_UPDATED_CB_IN_SAVE_MODE  0x00000002

/*
struct disp_usr_color_balance_config -- structure for user color balance configuration
  flags -- flags for providing additional information. Use DISP_USR_CB_FLAGS_* definitions
  red   -- double value denoting red coefficient normalized to the range [0-1]
  green -- double value denoting green coefficient normalized to the range [0-1]
  blue  -- double value denoting blue coefficient normalized to the range [0-1]
*/
struct disp_usr_color_balance_config {
  uint32_t flags;
  double red;
  double green;
  double blue;
};

/*
   DISP_USR_EBR_FLAGS_USE_DEFAULT_MODE
      - Apply the disp_usr_ebr_config on top of default color mode.
   DISP_USR_EBR_FLAGS_USE_CURRENT_ACTIVE_MODE
      - Apply the disp_usr_ebr_config on top of current active color mode

   DEFAULT_MODE and CURRENT_ACTIVE_MODE flags are exclusive. Driver selects
   DEFAULT_MODE if user enables both DEFAULT_MODE and CURRENT_ACTIVE_MODE flags.

   DISP_USR_EBR_FLAGS_USE_UPDATED_EBR_IN_SAVE_MODE
      - Use any updates applied to the color-modules with this data in
        creating new mode, using disp_api_save_display_mode.
*/
#define DISP_USR_EBR_FLAGS_USE_DEFAULT_MODE             0x00000001
#define DISP_USR_EBR_FLAGS_USE_CURRENT_ACTIVE_MODE      0x00000002
#define DISP_USR_EBR_FLAGS_USE_UPDATED_EBR_IN_SAVE_MODE 0x00000004

/*
struct disp_usr_ebr_config -- structure for user EBR configuration
  flags -- flags for providing additional information. Use DISP_USR_EBR_FLAGS_* definitions
  exposure_level (E)  -- exposure level. Range [0-255]
  eye_prot_level (B)  -- eye protection percentage. Range [0-100]
  red_tint_level (R)  -- red tint percentage. Range [0-100]
*/
struct disp_usr_ebr_config {
  uint32_t flags;
  uint32_t exposure_level;
  uint32_t eye_prot_level;
  uint32_t red_tint_level;
};

/*
struct disp_mode_info -- structure to represent color mode information
mode_id -- denotes enumerated mode id
is_default -- boolean representing if this is default mode
disp_id -- display id
mode_name -- mode's title in char array
color_gamut -- mode's color gamut in char array
*/
struct disp_mode_info {
  int32_t mode_id;
  bool is_default;
  int32_t disp_id;
  char mode_name[MAX_MODE_NAME_LENGTH];
  char color_gamut[MAX_MODE_NAME_LENGTH];
};

/*=========================================================================
FUNCTION
  disp_api_init()

DESCRIPTION
  This API initializes the display API library.

  This function must be called before calling any display APIs.
  The function returns a context handle that must be used on all subsequent
  calls.

  hctx  -- returns context handle on a successful call
  flags -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_init(DISPAPI_HANDLE *hctx, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_deinit()

DESCRIPTION
  This API de-initializes the display API library.

  hctx  -- Context handle.  This handle will no longer be valid after a
           successful deinit.
  flags -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_deinit(DISPAPI_HANDLE hctx, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_get_num_displays()

DESCRIPTION
  This API retrieves number of available displays.

  hctx -- Context handle.
  num_layers -- active layers
  flags -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_num_displays(DISPAPI_HANDLE hctx, uint32_t *num_disps, uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_get_display_info()

DESCRIPTION
  This API retrieves per display information.

  This call is used to discover the per display information and its associated ID.

  hctx -- Context handle.
  index -- No. index of physical display under query
  info -- display info

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_display_info(DISPAPI_HANDLE hctx, uint32_t index, struct display_info *info);

/*=========================================================================
FUNCTION
  disp_api_get_global_color_balance_range()

DESCRIPTION
  This API retrieves the supported color balance range information.

  hctx    -- Context handle.
  disp_id -- Display ID type
  range   -- Color Balance Range information

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_color_balance_range(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                                struct disp_range *range);

/*=========================================================================
FUNCTION
  disp_api_set_global_color_balance()

DESCRIPTION
  This API adjusts the color balance between cold and warm.

  hctx     -- Context handle.
  disp_id  -- Display ID type
  warmness -- Color balance adjustement, the larger the value the warmer the color
  flags    -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_global_color_balance(DISPAPI_HANDLE hctx, uint32_t disp_id, int32_t warmness,
                                          uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_get_global_color_balance()

DESCRIPTION
  This API retrieves the active settings for color balance.

  hctx      -- Context handle.
  disp_id   -- display ID type
  warmness  -- Color balance adjustement, the larger the value the warmer the color
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_color_balance(DISPAPI_HANDLE hctx, uint32_t disp_id, int32_t *warmness,
                                          uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_get_num_display_modes()

DESCRIPTION
  This API can retrieves the number of display color modes available for a given display ID.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  mode_type -- System defined mode type or user defined mode type
  mode_cnt  -- Number of display color modes
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_num_display_modes(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                       disp_mode_type mode_type, int32_t *mode_cnt,
                                       uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_get_display_modes()

DESCRIPTION
  This API retrieves the list of display color modes available for a given display ID.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  mode_type -- OEM defined mode type or user defined mode type
  modes     -- List of display color modes, including ID, name, and etc.
  mode_cnt  -- Number of display color modes; this variable indicates the number of
               modes being requested to be retrieved and the amount of buffer memory
               provided (modes) to contain the mode information.
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_display_modes(DISPAPI_HANDLE hctx, uint32_t disp_id, disp_mode_type mode_type,
                                   struct disp_mode *modes, int32_t mode_cnt, uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_get_active_display_mode()

DESCRIPTION
  This API retrieves the ID of the active display color mode for a given display ID.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  mode_id   -- Active Mode ID.
  mask      -- Bit mask that indicates which features are modified on top of the last applied mode
                0 - not modified, 1 - modified
                bit 0    -- Color balance adjustment
                bit 1    -- Global PA
                bit 2    -- Global Memory Color
                bit 3:31 -- Reserved
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_active_display_mode(DISPAPI_HANDLE hctx, uint32_t disp_id, int32_t *mode_id,
                                         uint32_t *mask, uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_set_active_display_mode()

DESCRIPTION
  This API applies a display color mode to become the active mode.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  mode_id   -- Display color mode ID, if the ID does not match an existing mode no update
               is applied.
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_active_display_mode(DISPAPI_HANDLE hctx, uint32_t disp_id, int32_t mode_id,
                                         uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_save_display_mode()

DESCRIPTION
  This API saves the current active settings as a display mode.

  hctx          -- Context handle.
  disp_id       -- Display ID type
  mode_name     -- Name string identifying the mode to be saved
  mode_name_len -- Name string length
  mode_id       -- A unique ID representing the saved mode;
                   If the caller of this API passes a negative ID, then this API will assign
                   an positive integer as the ID of the saved mode, and return to caller;
                   if the caller of this API passes a positive ID or zero, then this API will
                   check whether this ID already exists or not. If it is, then update this
                   mode; otherwise, create a new mode with this ID.
  flags         -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_save_display_mode(DISPAPI_HANDLE hctx, uint32_t disp_id, char *mode_name,
                                   uint32_t mode_name_len, int32_t *mode_id, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_delete_display_mode()

DESCRIPTION
  This API deletes a display mode based on the mode ID.

  OEM modes and the currently active mode cannot be deleted.

  hctx    -- Context handle.
  mode_id -- Display mode ID to be deleted
  flags   -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_delete_display_mode(DISPAPI_HANDLE hctx, int32_t mode_id, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_set_default_display_mode()

DESCRIPTION
  This API sets a display mode as the default mode for a given display type based on input mode ID.

  hctx    -- Context handle.
  disp_id -- Display ID type
  mode_id -- Display mode ID to be set as the default.
  flags   -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_default_display_mode(DISPAPI_HANDLE hctx, uint32_t disp_id, int32_t mode_id,
                                          uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_get_default_display_mode()

DESCRIPTION
  This API retrieves the the default display mode ID.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  mode_id   -- Display color mode ID
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_default_display_mode(DISPAPI_HANDLE hctx, uint32_t disp_id, int32_t *mode_id,
                                          uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_get_adapt_bl_ql_level()

DESCRIPTION
  This API retrieves the adaptive backlight quality level.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  state     -- State of mode : 0 = exit, 1 = enter
  level     -- Quality level setting
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_adapt_bl_ql_level(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t *enable,
                                       disp_adapt_bl_ql_level *level, uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_set_adapt_bl_ql_level()

DESCRIPTION
  This API sets the quality level of adaptive backlight.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  state     -- State of mode : 0 = exit, 1 = enter
  level     -- Quality level setting
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_adapt_bl_ql_level(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t enable,
                                       disp_adapt_bl_ql_level level, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_get_adapt_bl_scale()

DESCRIPTION
  This API gets the BL scale as percentage.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  scale     -- Backlight setting scale setting in units of percentage
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_adapt_bl_scale(DISPAPI_HANDLE hctx, uint32_t disp_id, float *scale,
                                    uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_get_sunlight_visibility_strength_range()

DESCRIPTION
  This API retrieves the supported range for the sunlight visibility strength.

  hctx    -- Context handle.
  disp_id -- Display ID type
  range   -- Supported range and step sizes of for SVI

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_sunlight_visibility_strength_range(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                                        struct disp_range *range);

/*=========================================================================
FUNCTION
  disp_api_get_sunlight_visibility_strength()

DESCRIPTION
  This API retrieves the current sunlight visibility strength.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  state     -- State of mode : 0 = exit, 1 = enter
  level     -- Strength setting in units of percentage
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_sunlight_visibility_strength(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                                  uint32_t *enable, int32_t *level,
                                                  uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_set_sunlight_visibility_strength()

DESCRIPTION
  This API sets the sunlight visibility strength.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  state     -- State of mode : 0 = exit, 1 = enter
  level     -- Strength setting in units of percentage
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_sunlight_visibility_strength(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                                  uint32_t enable, int32_t level, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_get_global_pa_range()

DESCRIPTION
  This API retrieves the global picture adjustment ranges for hue, saturation, value
  and contrast.

  hctx    -- Context handle.
  disp_id -- Display ID type
  range   -- Supported range information for PA.

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_pa_range(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                     struct disp_pa_range *range);

/*=========================================================================
FUNCTION
  disp_api_get_global_pa_config()

DESCRIPTION
  This API retrieves the current global picture adjustment configuration.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  enable    -- state of mode : 0 = disable, 1 = enable
  cfg       -- PA configuration information

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_pa_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t *enable,
                                      struct disp_pa_config *cfg);

/*=========================================================================
FUNCTION
  disp_api_set_global_pa_config()

DESCRIPTION
  This API sets global picture adjustment configuration.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- PA configuration information

  Parameter Usage:
  1: enable = 1 & cfg = non-NULL => enable functionality and apply configuration specified via cfg
  2: enable = 1 & cfg = NULL => enable functionality and apply previous (cached) configuration
  3: enable = 0 & cfg = non-NULL => disable functionality and cache configuration specified via cfg
  4: enable = 0 & cfg = NULL => disable functionality

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_global_pa_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t enable,
                                      struct disp_pa_config *cfg);

/*=========================================================================
FUNCTION
  disp_api_get_feature_version()

DESCRIPTION
  This API retrieves the feature version information for a specific functionality.

  hctx          -- Context handle.
  feature_id    -- Feature ID functionality being queried
  ver           -- Version information
                   A version value of 0.0.0 specifies an unsupported feature.
  flags         -- Reserved.

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_feature_version(DISPAPI_HANDLE hctx, uint32_t feature_id,
                                     struct version_info *ver, uint32_t *flags);

/*=========================================================================
FUNCTION
  disp_api_set_global_gclut_config()

DESCRIPTION
  This API sets the global gc lut Info.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- gc lut Info

  Parameter Usage:
  1: enable = 1 & cfg = non-NULL => enable functionality and apply configuration specified via cfg
  2: enable = 1 & cfg = NULL => enable functionality and apply previous (cached) configuration
  3: enable = 0 & cfg = non-NULL => disable functionality and cache configuration specified via cfg
  4: enable = 0 & cfg = NULL => disable functionality

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_global_gclut_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t enable,
                                         struct lut_info *cfg);

/*=========================================================================
FUNCTION
  disp_api_set_global_igclut_config()

DESCRIPTION
  This API sets the global igc lut Info - post mixer.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- gc lut Info

  Parameter Usage:
  1: enable = 1 & cfg = non-NULL => enable functionality and apply configuration specified via cfg
  2: enable = 1 & cfg = NULL => enable functionality and apply previous (cached) configuration
  3: enable = 0 & cfg = non-NULL => disable functionality and cache configuration specified via cfg
  4: enable = 0 & cfg = NULL => disable functionality

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_global_igclut_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t enable,
                                          struct lut_info *cfg);

/*=========================================================================
FUNCTION
  disp_api_set_global_pcc_config()

DESCRIPTION
  This API sets the global PCC Info - post mixer.

  hctx      -- Context handle.
  disp_id   -- display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- PCC data

  Parameter Usage:
  1: enable = 1 & cfg = non-NULL => enable functionality and apply configuration specified via cfg
  2: enable = 1 & cfg = NULL => enable functionality and apply previous (cached) configuration
  3: enable = 0 & cfg = non-NULL => disable functionality and cache configuration specified via cfg
  4: enable = 0 & cfg = NULL => disable functionality

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_global_pcc_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t enable,
                                       struct pcc_coeff_data *cfg);

/*=========================================================================
FUNCTION
  disp_api_get_global_gclut_config()

DESCRIPTION
  This API retrieves the global gc lut info.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- gc lut Info

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_gclut_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t *enable,
                                         struct lut_info *cfg);

/*=========================================================================
FUNCTION
  disp_api_get_global_igclut_config()

DESCRIPTION
  This API retrieves the global igc lut info - post mixer.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- gc lut Info

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_igclut_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t *enable,
                                          struct lut_info *cfg);

/*=========================================================================
FUNCTION
  disp_api_get_global_pcc_config()

DESCRIPTION
  This API retrieves the global PCC info - post mixer.

  hctx      -- Context handle.
  disp_id   -- display ID type
  enable    -- State of mode : 0 = disable, 1 = enable
  cfg       -- PCC configuration data

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_global_pcc_config(DISPAPI_HANDLE hctx, uint32_t disp_id, uint32_t *enable,
                                       struct pcc_coeff_data *cfg);

/*=========================================================================
FUNCTION
  disp_api_set_usr_color_balance_config()

DESCRIPTION
  This API adjusts the color balance between cold and warm based on user configuration.

  hctx    -- Context handle.
  disp_id -- display ID type: primary, external, or WIFI display
  config  -- pointer to structure containing user color balance configuration;

RETURN VALUE
  ZERO - success.
  NON-ZERO - fail
=========================================================================*/
int32_t disp_api_set_usr_color_balance_config(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                              struct disp_usr_color_balance_config* config);

/*=========================================================================
FUNCTION
  disp_api_get_usr_color_balance_config()

DESCRIPTION
  This API returns the current user color balance configuration applied on target.

  hctx    -- Context handle.
  disp_id -- display ID type: primary, external, or WIFI display
  config  -- pointer to structure containing color balance configuration;

RETURN VALUE
  ZERO - success.
  NON-ZERO - fail
=========================================================================*/
int32_t disp_api_get_usr_color_balance_config(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                              struct disp_usr_color_balance_config* config);

/*=========================================================================
FUNCTION
  disp_api_set_usr_ebr_config()

DESCRIPTION
  This API adjusts the exposure, brightness and red-tint configuration.

  hctx    -- Context handle.
  disp_id -- display ID type: primary, external, or WIFI display
  config  -- pointer to structure containing user EBR configuration;

RETURN VALUE
  ZERO - success.
  NON-ZERO - fail
=========================================================================*/
int32_t disp_api_set_usr_ebr_config(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                    struct disp_usr_ebr_config* config);

/*=========================================================================
FUNCTION
  disp_api_get_usr_ebr_config()

DESCRIPTION
  This API returns the current user EBR applied on target.

  hctx    -- Context handle.
  disp_id -- display ID type: primary, external, or WIFI display
  config  -- pointer to structure containing EBR configuration;

RETURN VALUE
  ZERO - success.
  NON-ZERO - fail
=========================================================================*/
int32_t disp_api_get_usr_ebr_config(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                    struct disp_usr_ebr_config* config);
/*==========================================================================
FUNCTION
  disp_api_set_panel_brightness_level_ext()

DESCRIPTION
  This API sets the panel's brightness level.

  hctx      -- Context handle.
  disp_id   -- Display ID type.
  value     -- Brightness level
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_set_panel_brightness_level_ext(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                                uint32_t value, uint32_t flags);

/*=========================================================================
FUNCTION
  disp_api_get_panel_brightness_level_ext()

DESCRIPTION
  This API retrieves the panel's brightness level.

  hctx      -- Context handle.
  disp_id   -- Display ID type
  value     -- Brightness level
  flags     -- Reserved

RETURN VALUE
  ZERO (SUCCESS)- success.
  Negative Value - error\failure
=========================================================================*/
int32_t disp_api_get_panel_brightness_level_ext(DISPAPI_HANDLE hctx, uint32_t disp_id,
                                                uint32_t *value, uint32_t *flags);

#ifdef __cplusplus
}
#endif

#endif  // __DISP_COLOR_APIS_H__
