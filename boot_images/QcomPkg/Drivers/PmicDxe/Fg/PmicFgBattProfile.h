#ifndef __PMICFGBATTPROFILE_H__
#define __PMICFGBATTPROFILE_H__

 /*! @file PmicFgBattProfile.h
 *
 * PMIC Battery Profile UEFI definitions.
 *
 * Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*=============================================================================
                              EDIT HISTORY


 when          who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/30/17   sm        Adding profile rev to battery profile info
 01/23/17   sm        Input parameter list change for PmicFgBattProfileInit()
 05/16/16   va        Update for loading profile as a common platform
 03/22/16   va        Update to make FG Protocol compile for WP and remove not necessary code
 01/27/16   sm        Added parameter to PmicFgBattProfileInit()
 10/26/15   va        Rslow Clear - profile index fix
 04/08/15   va        Rslow WA Changes 
 03/13/15   al        Adding changes for optimization
 01/05/15   va        Added Multiple Battery Profile and Profile Parser Status API
 09/23/14   al        Moving API declaration from source file to header file 
 09/25/14   va        Update for New Battery Profile Format
 05/16/14   va        New file.
=============================================================================*/

/**
  PMIC Platform interface
 */
#include <Protocol/EFIPmicFg.h>

/*===========================================================================
                               MACROS
===========================================================================*/
#define MAX_NUM_OF_BATT_PROFILE         8 // Number profile that can exists in profile file
#define MAX_VARNAME_SIZE                    30
#define BREAK_IF_ERROR(Status) if (EFI_SUCCESS != Status ) { break;} 

/*=========================================================================
                            GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/

/*!
 *  \brief Structure used for storing Battery Profile
*/

/* Battery profile addr data value */
typedef struct _FgBattAddrData{
  UINT32 Addr;
  UINT32 Addr2;
  UINT32 Offset;
  UINT32 Data;
}FgBattAddrData;

typedef struct _FgBattThermCoff{
  UINT32 ThermC1Coeff;
  UINT32 ThermC2Coeff;
  UINT32 ThermC3Coeff;
}FgBattThermCoff;

/* Battery profile params*/
typedef struct _FgBattProfileParams{
  UINT32 BatteryId;
  UINT32 FloatVoltage;
  UINT32 NomBattCapacity;
  UINT32 ThermistorValue;
  UINT32 CheckSum;
  UINT8  *BatteryName;
  UINT32 ProfileRev;
  FgBattThermCoff ThermCoff;
}FgBattProfileParams;

/* Battery profile Data */
typedef struct _FgBattProfileData{
  FgBattAddrData     *addrData;
  FgBattProfileParams Pp; /*ProfileParams*/
}FgBattProfileData;

/* Battery profile Number */
typedef enum _FgBattProfileNum{
 FG_BATTPROFILE_NUMSEL_1,/*Profile Number */
 FG_BATTPROFILE_NUMSEL_2, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_3, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_4, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_5, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_6, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_7, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_8, /*Profile Number */
 FG_BATTPROFILE_NUMSEL_MAX /*MAx Profile Number */
}FgBattProfileNumSel;

/* Battery profile complete Info */
typedef struct _FgBattProfile{
  FgBattProfileData Pd[MAX_NUM_OF_BATT_PROFILE];/* One Profile complete Information */
  UINT32 ProfileCount;/*Total number of profile available */
  UINT32 SelectedProfile;/*Selected profile and flashed in FG SRAM */
}FgBattProfile;

typedef enum 
{
  UEFI_VARIABLE_OP_GET,
  UEFI_VARIABLE_OP_SET,
  UEFI_VARIABLE_OP_INVALID,
}PmUefiVariableOpType;

/*!
 * \enum EFI_PM_FG_PARSER_STATUS_TYPE
 *  \brief Enumeration for Profile Parser Status
 */
 typedef enum _EFI_PM_FG_PROFILE_PARSER_STATUS
{
  PM_PARSER_STATUS_GOOD_MATCH,
  PM_PARSER_STATUS_NO_ID_MATCH,
  PM_PARSER_STATUS_MEM_ALLOC_ERROR,
  PM_PARSER_STATUS_PARSE_ERROR,
  PM_PARSER_STATUS_OTHER_ERROR,
  PM_PARSER_STATUS_CONFIG_ERROR,
  PM_PARSER_STATUS_BAD_PARAM,
  PM_PARSER_STATUS_DUMMY_BATTERY
} EFI_PM_FG_PROFILE_PARSER_STATUS;

typedef enum
{
  EFI_PM_FG_BATT_TYPE_NORMAL,
  EFI_PM_FG_BATT_TYPE_SMART,
  EFI_PM_FG_BATT_BATT_EMULATOR,
  EFI_PM_FG_BATT_TYPE_DEBUG_BOARD,
  EFI_PM_FG_BATT_TYPE_UNKNOWN,
  EFI_PM_FG_BATT_TYPE_INVALID
}PM_FG_BATT_TYPE;


EFI_STATUS PmicFgBattProfileInit(EFI_PM_FG_CFGDATA_TYPE *pFgConfigData);

EFI_PM_FG_PROFILE_PARSER_STATUS PmicBattProfileParseDataBuffer (IN UINT8 *pBufferData, IN  UINTN DataBufferSize);

EFI_STATUS PmicFgBattHandleProfile(UINT32 PmicDeviceIndex);

VOID       EFI_PmicBattProfileStrToUpper(char *Str, UINT32 StrSize);

VOID       EFI_PmicBattProfileProcessVariable(char *VarName);

EFI_STATUS EFI_PmicBattProfileStrCat(char *Dest, char *Src);

/* EFI_STATUS PmicFgBattProfileReadProfileParams( UINT32 PmicDeviceIndex, BOOLEAN ReadProfile, OPTIONAL UINT32 ProfileIndex); */

EFI_STATUS PmicUefiVariable(UINT16 *variable, UINTN* Value, PmUefiVariableOpType OpType /*TRUE for Set and FALSE for Get*/);

/* EFI_STATUS PmicFgBattProfileGetProfileParams(OUT FgBattProfileParams *Pp); */

#endif /* __PMICFGBATTPROFILE_H__ */

