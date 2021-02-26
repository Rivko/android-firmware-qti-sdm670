/**
@file  EFIWinAcpiUpdate.h
@brief Win DSDT Update UEFI Protocol definitions.
*/
/*=============================================================================
Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when         who     what, where, why
--------     ---     -----------------------------------------------------------
02/17/2017   vinitb  Added qccdi option related changes
01/30/2017   vinitb  Added sensors/gpu option related changes
09/12/2016   ltg     Updated as per new design
09/08/2016   mfc     Initial revision

=============================================================================*/

#ifndef __EFI_WIN_ACPI_UPDATE_H__
#define __EFI_WIN_ACPI_UPDATE_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_WIN_ACPI_UPDATE_constants
Protocol version. */
#define EFI_WIN_ACPI_UPDATE_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
#define EFI_WIN_ACPI_UPDATE_PROTOCOL_GUID \
	    {0x9349b7b6, 0xb924, 0x4321, {0x8e, 0x6d, 0x5, 0x6b, 0x3f, 0xfa, 0xc3, 0x80 }}

/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/** @cond */
/* External reference to the WinAcpiUpdate Protocol GUID. */
extern EFI_GUID gEfiWinAcpiUpdateProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/* Protocol declaration */
typedef struct _EFI_WIN_ACPI_UPDATE_PROTOCOL EFI_WIN_ACPI_UPDATE_PROTOCOL;
/** @endcond */

/** @cond */
/** Type definition for the #EFI_WIN_ACPI_TYPE enumeration. */
typedef enum _EFI_FEATURE_TYPE EFI_FEATURE_TYPE;
/** @endcond */
/**
Feature information. The information is part of the input
to SETFEATUREVARIABLE and GETFEATUREVARIABLE APIs.
*/
enum _EFI_FEATURE_TYPE
{
	EFI_FEATURE_INVALID = -1,
	EFI_FEATURE_FRONT_CAMERA = 0,
	EFI_FEATURE_BACK_CAMERA = 1,
	EFI_FEATURE_BT = 2,
	EFI_FEATURE_AUDIO = 3,
	EFI_FEATURE_MODEM = 4,
	EFI_FEATURE_USB = 5,
	EFI_FEATURE_SENSORS = 6,
	EFI_FEATURE_GPU = 7,
	EFI_FEATURE_WLAN = 8,
	EFI_FEATURE_QCCDI = 9,

	EFI_FEATURE_MAX
};


/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_SETFEATUREVARIABLE */
/** @ingroup efi_setfeaturevariable
@par Summary
Takes data as specified by feture type and updates the configuration
variable bit mask in local memory.

@param[in]  This              Pointer to the WIN_ACPI_UPDATE_PROTOCOL instance.
@param[in]  Type              Enum type specifying the feature.
@param[in]  Enable            Boolean value,
TRUE: Enabling the feature.
FALSE: Disabling the feature.

@par Description
This function takes in the feature type, enables the flag, and saves the
configuration in the local memory of the protocol. To commit the
configuration to storage, caller has to call the commit API.

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- Parameter is invalid. \n
EFI_XXXX              -- Other errors; refer to @xhyperref{R1,[R1]} for
details.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_SETFEATUREVARIABLE)(
IN  EFI_WIN_ACPI_UPDATE_PROTOCOL		*This,
IN  EFI_FEATURE_TYPE				Type,
IN  BOOLEAN							Enable
);


/* EFI_GETFEATUREVARIABLE */
/** @ingroup efi_getfeaturevariable
@par Summary
Takes data as specified by feature type and returns the configuration for
that feature.

@param[in]  This              Pointer to the WIN_ACPI_UPDATE_PROTOCOL instance.
@param[in]  Type              Enum type specifying the feature.

@param[out] Enable            Boolean value,
TRUE: feature enabled.
FALSE: feature disabled.

@par Description
This function queries the status of the provided feature type.

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_INVALID_PARAMETER -- Parameter is invalid. \n
EFI_XXXX              -- Other errors; refer to @xhyperref{R1,[R1]} for
details.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_GETFEATUREVARIABLE)(
IN  EFI_WIN_ACPI_UPDATE_PROTOCOL    *This,
IN  EFI_FEATURE_TYPE            Type,
OUT  BOOLEAN                    *Enable
);

/* EFI_COMMITFEATUREVARIABLE */
/** @ingroup efi_commitfeaturevariable
@par Summary
This API saves the local configuration mask to a UEFI variable.
Reboot is requried to take the configuration into effect.

@param[in]  This              Pointer to the WIN_ACPI_UPDATE_PROTOCOL instance.

@par Description
This function saves the local configuration bit mask to the UEFI variable.

@return
EFI_SUCCESS           -- Function completed successfully. \n
EFI_XXXX              -- Other errors; refer to @xhyperref{R1,[R1]} for
details.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_COMMITFEATUREVARIABLE)(
IN  EFI_WIN_ACPI_UPDATE_PROTOCOL  *This
);

/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_win_acpi_update_protocol
@par Summary
EFI_WIN_ACPI_UPDATE_PROTOCOL Protocol interface.
*/
struct _EFI_WIN_ACPI_UPDATE_PROTOCOL {
	UINT64							Revision;
	EFI_GETFEATUREVARIABLE			GetFeatureVariable;
	EFI_SETFEATUREVARIABLE			SetFeatureVariable;
	EFI_COMMITFEATUREVARIABLE		CommitFeatureVariable;
};

#endif	/* __EFI_WIN_ACPI_UPDATE_H__ */

