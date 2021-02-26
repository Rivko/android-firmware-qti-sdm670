/** 
  @file  EFIDisplayUtils.h
  @brief UEFI display apps boot loader definitions.
*/
/*=============================================================================
    Copyright (c) 2016-2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __EFIDISPLAYUTILS_H__
#define __EFIDISPLAYUTILS_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_displayUtils_constants  
@{ */
/**
  Protocol version.
*/
#define DISPLAY_UTILS_REVISION 0x0000000000020000

/** @} */ /* end_addtogroup efi_displayUtils_constants */

/*  Protocol GUID definition */
/** @ingroup efi_displayUtils_protocol */

// {c0dd69ac-76ba-11e6-ab24-1fc7f5575f19}
#define EFI_DISPLAYUTILS_PROTOCOL_GUID \
    {0xc0dd69ac, 0x76ba, 0x11e6, {0xab, 0x24, 0x1f, 0xc7, 0xf5, 0x57, 0x5f, 0x19}}


/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the Display Utils Protocol GUID.
*/
extern EFI_GUID gQcomDisplayUtilsProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_DISPLAY_UTILS_PROTOCOL  EFI_QCOM_DISPLAY_UTILS_PROTOCOL;
/** @endcond */

/** @addtogroup efi_displayUtils_data_types 
@{ */
/** 
  Display Utils property types
*/
typedef enum
{
  EFI_DISPLAY_UTILS_DEVICE_TREE_ADDR = 0x0, /**< Device tree base address: (Type: VOID*) */
  EFI_DISPLAY_UTILS_PANEL_OVERRIDE,         /**< Panel override string (Type: CHAR8*) */
  EFI_DISPLAY_UTILS_SUPPORTED_PANELS,       /**< Newline separated list of supported panels (Type: CHAR16*) */
  EFI_DISPLAY_UTILS_PANEL_CONFIG,           /**< Panel raw configuration */
} EFI_DISPLAY_UTILS_PROPERTY_TYPE;

/** 
  Parameter structure for EFI_DISPLAY_UTILS_PANEL_LIST
*/
typedef struct
{
  UINT32   PanelCount;                  /**< Number of panels in PanelList */
  UINT32   Length;                      /**< String length of PanelList */
  CHAR8   *PanelList;                   /**< Newline separated list of supported panels */
} EFI_DISPLAY_UTILS_PANEL_LIST_PARAM;

/** 
  Display Utils render logo flags
*/
typedef enum
{
  DISPLAY_UTILS_RENDERLOGO_CLEARSCREEN       = 0x00000001,  /**< Clear Screen flag */
  DISPLAY_UTILS_RENDERLOGO_POSITION_OVERRIDE = 0x00000002,  /**< Position Override flag */
} EFI_DISPLAY_UTILS_RENDERLOGO_FLAGS;

/** 
  Display Utils render logo configurations structure
*/
typedef struct
{
  UINT32 uDisplayId;                                        /**< Display ID */
  UINT32 uFlags;                                            /**< Render Logo Override flags */
  UINT32 xPosition;                                         /**< Override x Position of Logo */
  UINT32 yPosition;                                         /**< Override y Position of Logo */
} EFI_DISPLAY_UTILS_RENDERLOGO_CONFIG;

/**
  Parameter structure for EFI_DISPLAY_UTILS_PANEL_CONFIG
*/
typedef struct
{
  UINT32   uPanelIndex;                 /**< Panel index */
  CHAR8   *pPanelConfig;                /**< Panel configuration */
  UINT32   uPanelConfigLength;          /**< Panel configuration length */
} EFI_DISPLAY_UTILS_PANEL_CONFIG_PARAMS;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_DISPLAY_UTILS_SET_PROPERTY */ 
/** @ingroup efi_displayUtils_control
  @par Summary
  Set a property on Display UEFI

  @param[in] Type                Property type
  @param[in] Data                Pointer to data structure
  @param[in] DataSize            Size of data in bytes
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_UTILS_SET_PROPERTY)(
  IN EFI_DISPLAY_UTILS_PROPERTY_TYPE   Type,
  IN VOID                             *Data,
  IN UINT32                            DataSize
);

/* EFI_DISPLAY_UTILS_GET_PROPERTY */ 
/** @ingroup efi_displayUtils_control
  @par Summary
  Gets the value of a property from Display UEFI.
  If Data is NULL or DataSize is smaller then function 
  returns EFI_BUFFER_TOO_SMALL with DataSize set to 
  the required buffer size

  @param[in] Type                Property type
  @param[out] Data               Pointer to data structure
  @param[in/out] DataSize        [in] Max size of data 
                                 [out] Actual size of data 
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
  EFI_BUFFER_TOO_SMALL     : Buffer to small to copy data into
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_UTILS_GET_PROPERTY)(
  IN     EFI_DISPLAY_UTILS_PROPERTY_TYPE   Type,
  OUT    VOID                             *Data,
  IN OUT UINT32                           *DataSize
);

/* EFI_DISPLAY_UTILS_RENDER_LOGO */ 
/** @ingroup efi_displayUtils_control
  @par Summary
  Render logo function implementation.

  @param[in] psRenderLogoConfig  Configurations of clearing screen and overriding position.
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_UTILS_RENDER_LOGO)(
  IN     EFI_DISPLAY_UTILS_RENDERLOGO_CONFIG *psRenderLogoConfig
);

/* EFI_DISPLAY_UTILS_SET_MODE */ 
/** @ingroup efi_displayUtils_control
  @par Summary
  Set display mode can be used for primary and externl

  @param[in] DisplayId     Primary(0) or external (1)
  @param[in] ModeNum       display mode index
  @param[in] Flags         extra info
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_UTILS_SET_MODE)(
  IN UINT32                            DisplayId,
  IN UINT32                            ModeNum,
  IN UINT32                            Flags
);


/* EFI_DISPLAY_UTILS_SET_VARIABLE */ 
/** @ingroup efi_displayUtils_control
  @par Summary
  Variable set function implementation.

  @param[in]  pVariableName              The variable name that need to set.
  @param[in]  pVariableValue             The variable value.
  @param[in]  uDataLength                The length of the variable value.
  @param[in]  uFlags                     The flag to set variable.
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_UTILS_SET_VARIABLE)(
  CHAR16                          *pVariableName,
  UINT8                           *pVariableValue,
  UINTN                            uDataLength,
  UINTN                            uFlags
);


/* EFI_DISPLAY_UTILS_GET_VARIABLE */ 
/** @ingroup efi_displayUtils_control
  @par Summary
  Variable get function implementation.

  @param[in]   pVariableName              The variable name that need to get.
  @param[out]  pVariableValue             The variable value.
  @param[out]  pDataLength                The length of the variable value.
  @param[in]   uFlags                     The flag to get variable.
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.
  EFI_UNSUPPORTED          : Parameter passed is not supported. 
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_DISPLAY_UTILS_GET_VARIABLE)(
  CHAR16                          *pVariableName,
  UINT8                           *pVariableValue,
  UINTN                           *pDataLength,
  UINTN                            uFlags
);
/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_displayUtils_protocol
  @par Summary
  Qualcomm Display Utils Protocol interface.

  @par Parameters
  @inputprotoparams{} 
*/
struct _EFI_QCOM_DISPLAY_UTILS_PROTOCOL
{
  UINT64                                           Revision;
  EFI_DISPLAY_UTILS_SET_PROPERTY                   DisplayUtilsSetProperty; 
  EFI_DISPLAY_UTILS_GET_PROPERTY                   DisplayUtilsGetProperty;
  EFI_DISPLAY_UTILS_RENDER_LOGO                    DisplayUtilsRenderLogo;
  EFI_DISPLAY_UTILS_SET_MODE                       DisplayUtilsSetMode;
  EFI_DISPLAY_UTILS_SET_VARIABLE                   DisplayUtilsSetVariable;
  EFI_DISPLAY_UTILS_GET_VARIABLE                   DisplayUtilsGetVariable;
};

#endif  /* __EFIDISPLAYUTILS_H__ */
