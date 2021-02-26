/** @file FwFileIdentifiers.h
   
  Headerfile for FileNames.c, part of FwCommonLib

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/08/08     mic         Added maximum update attempts support
 2012/08/08     rsb         Removed unused file paths and moved update specific
                            files to FWUPDATE folder in GPP FAT
 2012/06/22     mic         Added support for OPM keys
 2012/06/21     rs          Adding support for FPDT ACPI table
 2012/06/12     rs          Added support for BreakChangeNumber
 2012/03/10     rs          Added SSD Keystore file path
 2011/10/12     jthompso    Initial version
 2012/01/25     jd          Added file path for FactoryTool commit and result files

=============================================================================*/
#ifndef __QCOM_FW_HANDLES_FILES_H__
#define __QCOM_FW_HANDLES_FILES_H__


#include <Guid/FileInfo.h>
#include <Include/Library/UefiBootServicesTableLib.h>
#include <Include/Library/FwCommonLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/FirmwareVolume2.h>


//FileNames
#define QCOM_FW_UPDATE_FILE_UPDATE_APP_PATH                 L"QCPLAT.EFI"
#define QCOM_FW_UPDATE_FILE_UPDATE_APP_BCK_PATH             L"QCPLAT$$.EFI"
#define QCOM_FW_UPDATE_FILE_FLAG_LEGACY_UPDATE_COMMIT_PATH  L"FCT_COMM.FLG"
#define QCOM_FW_UPDATE_FILE_FLAG_FACTORY_PROVISIONED_PATH   L"FCT_PROV.FLG"
#define QCOM_FW_UPDATE_FILE_FLAG_FACTORY_PATH               L"\\DPP\\FACTORY.FLG"
#define QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE_PATH          L"\\FWUPDATE\\UPDATE.STA"
#define QCOM_FW_UPDATE_FILE_FACTORY_RESULT_FILE_PATH        L"\\DPP\\FACTORY.RES"
#define QCOM_FW_UPDATE_PAYLOAD_APP_FILE_PATH                L"QPAYLOAD.EFI"
#define QCOM_FW_UPDATE_FILE_BP_GPT_FILE_PATH                L"\\FWUPDATE\\BPT.TBL"
#define QCOM_FW_UPDATE_FILE_GPP_GPT_FILE_PATH               L"\\FWUPDATE\\GPP.TBL"
#define QCOM_FW_UPDATE_ACPI_CSRT_FILE_PATH                  L"\\ACPI\\CSRT.ACP"
#define QCOM_FW_UPDATE_ACPI_TPM2_FILE_PATH                  L"\\ACPI\\TPM2.ACP"
#define QCOM_FW_UPDATE_ACPI_BGRT_FILE_PATH                  L"\\ACPI\\BGRT.ACP"
#define QCOM_FW_UPDATE_ACPI_LOGO1_FILE_PATH                 L"\\LOGO1.BMP"
#define QCOM_FW_UPDATE_ACPI_DBG2_FILE_PATH                  L"\\ACPI\\DBG2.ACP"
#define QCOM_FW_UPDATE_ACPI_DBGP_FILE_PATH                  L"\\ACPI\\DBGP.ACP"
#define QCOM_FW_UPDATE_ACPI_DSDT_FILE_PATH                  L"\\ACPI\\DSDT.AML"
#define QCOM_FW_UPDATE_ACPI_FACP_FILE_PATH                  L"\\ACPI\\FACP.ACP"
#define QCOM_FW_UPDATE_ACPI_FACS_FILE_PATH                  L"\\ACPI\\FACS.ACP"
#define QCOM_FW_UPDATE_ACPI_FPDT_FILE_PATH                  L"\\ACPI\\FPDT.ACP"
#define QCOM_FW_UPDATE_ACPI_MADT_FILE_PATH                  L"\\ACPI\\MADT.ACP"
#define QCOM_FW_UPDATE_ACPI_CSRT_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\CSRT.ACP"
#define QCOM_FW_UPDATE_ACPI_TPM2_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\TPM2.ACP"
#define QCOM_FW_UPDATE_ACPI_BGRT_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\BGRT.ACP"
#define QCOM_FW_UPDATE_ACPI_LOGO1_BCK_FILE_PATH             L"\\FWUPDATE\\LOGO1$$.BMP"
#define QCOM_FW_UPDATE_ACPI_DBG2_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\DBG2.ACP"
#define QCOM_FW_UPDATE_ACPI_DBGP_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\DBGP.ACP"
#define QCOM_FW_UPDATE_ACPI_DSDT_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\DSDT.AML"
#define QCOM_FW_UPDATE_ACPI_FACP_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\FACP.ACP"
#define QCOM_FW_UPDATE_ACPI_FACS_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\FACS.ACP"
#define QCOM_FW_UPDATE_ACPI_FPDT_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\FPDT.ACP"
#define QCOM_FW_UPDATE_ACPI_MADT_BCK_FILE_PATH              L"\\FWUPDATE\\ACPIBCK\\MADT.ACP"
#define QCOM_FW_UPDATE_OA3_MSDM_ORIG_FILE_PATH              L"\\DPP\\MSDM.ACP" 
#define QCOM_FW_UPDATE_OA3_MSDM_DEST_FILE_PATH              L"\\ACPI\\MSDM.ACP" 
#define QCOM_FW_UPDATE_DPP_QCOM_DIR_FILE_PATH               L"\\DPP\\QCOM\\" 
#define QCOM_FW_UPDATE_DPP_OEM_DIR_FILE_PATH                L"\\DPP\\OEM\\" 
#define QCOM_FW_UPDATE_SSD_KEYSTORE_FILE_PATH               L"\\DPP\\SSD.KEY" 
#define QCOM_FW_UPDATE_BREAKING_CHANGE_NUM_FILE_PATH        L"\\FWUPDATE\\BKCHANGE.NUM"
#define QCOM_FW_UPDATE_OPM_PUB_KEY_FILE_PATH                L"\\DPP\\OPM_PUB.PROVISION"
#define QCOM_FW_UPDATE_OPM_PUB_KEY_BCK_FILE_PATH            L"\\FWUPDATE\\OPMBCK\\OPM_PUB.PRO"
#define QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIV_KEY_FILE_PATH     L"\\DPP\\OPM_PRIV.PROVISION"
#define QCOM_FW_UPDATE_OPM_ENCRYPTED_PRIV_KEY_BCK_FILE_PATH L"\\FWUPDATE\\OPMBCK\\OPM_PRIV.PRO"
#define QCOM_FW_UPDATE_FILE_ATTEMPT_COUNT_FILE_PATH         L"\\FWUPDATE\\ATTEMPT.CNT"
//0A85A45E-915F-49DB-8BD5-5337861F8082
#define QCOM_FW_UPDATE_FILE_SBL1_IN_PAYLOAD_GUID \
  { \
    0x0A85A45E, 0x915F, 0x49DB, {0x8B, 0xD5, 0x53, 0x37, 0x86, 0x1F, 0x80, 0x82} \
  }

//E7BF4F3F-7DC9-40C0-9DF2-CE2EC5CEACEF
#define QCOM_FW_UPDATE_FILE_SBL2_IN_PAYLOAD_GUID \
  { \
    0xE7BF4F3F, 0x7DC9, 0x40C0, {0x9D, 0xF2, 0xCE, 0x2E, 0xC5, 0xCE, 0xAC, 0xEF} \
  }

//8BA7FEBE-AB44-411D-86E7-A6F2DE7E3F40
//New: 16DACD4A-4476-4320-94 E0-1F 50 32 58 D0 5B
//OLD: B4D5C1CE-549D-4FF3-83 AA-23 86 81 E3 F8 A5
#define QCOM_FW_UPDATE_FILE_SBL3_IN_PAYLOAD_GUID \
  { \
    /*0xB4D5C1CE, 0x549D, 0x4FF3, {0x83, 0xAA, 0x23, 0x86, 0x81, 0xE3, 0xF8, 0xA5 }*/ \
    /*0x16DACD4A, 0x4476, 0x4320, {0x94, 0xE0, 0x1F, 0x50, 0x32, 0x58, 0xD0, 0x5B } */\
    0x8BA7FEBE, 0xAB44, 0x411D, {0x86, 0xE7, 0xA6, 0xF2, 0xDE, 0x7E, 0x3F, 0x40} \
  }

//8A8BD280-F35E-48E1-A891-BF0BB855831E
#define QCOM_FW_UPDATE_FILE_RPM_IN_PAYLOAD_GUID \
  { \
    0x8A8BD280, 0xF35E, 0x48E1, {0xA8, 0x91, 0xBF, 0x0B, 0xB8, 0x55, 0x83, 0x1E} \
  }

//497FBC93-5784-4B8C-8F01-2AF50FB19239
#define QCOM_FW_UPDATE_FILE_TZ_IN_PAYLOAD_GUID \
  { \
    0x497FBC93, 0x5784, 0x4B8C, {0x8F, 0x01, 0x2A, 0xF5, 0x0F, 0xB1, 0x92, 0x39} \
  }
//F7A7DF2A-A845-4C17-94B8-85FD9CD022D7
#define QCOM_FW_UPDATE_FILE_WINSECAPP_IN_PAYLOAD_GUID \
  { \
    0xF7A7DF2A, 0xA845, 0x4C17, {0x94, 0xB8, 0x85, 0xFD, 0x9C, 0xD0, 0x22, 0xD7} \
  }

//31C5C241-D6CE-4E7A-B400-9C35571B2EA9
#define QCOM_FW_UPDATE_FILE_UEFI_IN_PAYLOAD_GUID \
  { \
    0x31C5C241, 0xD6CE, 0x4E7A, {0xB4, 0x00, 0x9C, 0x35, 0x57, 0x1B, 0x2E, 0xA9} \
  }

//{B9D29A25-8BD7-4487-B1 F9-AA CF 6F 40 CA B4}
#define QCOM_FW_UPDATE_FILE_UPDATE_APP_IN_PAYLOAD_GUID \
  { \
    0xB9D29A25, 0x8BD7, 0x4487, {0xB1, 0xF9, 0xAA, 0xCF, 0x6F, 0x40, 0xCA, 0xB4} \
  }


/*{642F3381-0327-4E0D-A7C1-A2C7D2C45812}*/
#define QCOM_FW_UPDATE_ACPI_CSRT_FILE_IN_PAYLOAD_GUID \
  { \
    0x642F3381, 0x0327, 0x4E0D, { 0xA7, 0xC1, 0xA2, 0xC7, 0xD2, 0xC4, 0x58, 0x12 } \
  }

/*{D298A7FE-C5CC-4C54-9CBC-9A59FA47F3AB}*/
#define QCOM_FW_UPDATE_ACPI_TPM2_FILE_IN_PAYLOAD_GUID \
  { \
    0xD298A7FE, 0xC5CC, 0x4C54, { 0x9C, 0xBC, 0x9A, 0x59, 0xFA, 0x47, 0xF3, 0xAB } \
  }

/*{8E230A44-9617-40BC-B18D-256795E55526}*/
#define QCOM_FW_UPDATE_ACPI_BGRT_FILE_IN_PAYLOAD_GUID \
  { \
    0x8E230A44, 0x9617, 0x40BC, { 0xB1, 0x8D, 0x25, 0x67, 0x95, 0xE5, 0x55, 0x26 } \
  }

/*{8E230A44-9617-40BC-B18D-256795E55526}*/
#define QCOM_FW_UPDATE_ACPI_LOGO1_FILE_IN_PAYLOAD_GUID \
  { \
    0x8aa7def2, 0x4b2e, 0x470c, { 0xba, 0xe2, 0x5, 0x7c, 0xac, 0xa3, 0x27, 0xdb } \
  }

/* {D8E02C2D-9310-47E6-8B92-C7A3564C488A}*/

#define QCOM_FW_UPDATE_ACPI_DBG2_FILE_IN_PAYLOAD_GUID \
  { \
    0xD8E02C2D, 0x9310, 0x47E6, { 0x8B, 0x92, 0xC7, 0xA3, 0x56, 0x4C, 0x48, 0x8A } \
  }

/*{5A6FC8AA-505C-41BF-94E7-71458ABD27FE}*/
#define QCOM_FW_UPDATE_ACPI_DBGP_FILE_IN_PAYLOAD_GUID \
  { \
    0x5A6FC8AA, 0x505C, 0x41BF, { 0x94, 0xE7, 0x71, 0x45, 0x8A, 0xBD, 0x27, 0xFE } \
  }

/*{044AF707-CDE8-4D15-B811-594BDABEB1FD}*/
#define QCOM_FW_UPDATE_ACPI_DSDT_FILE_IN_PAYLOAD_GUID \
  { \
    0x044AF707, 0xCDE8, 0x4D15, { 0xB8, 0x11, 0x59, 0x4B, 0xDA, 0xBE, 0xB1, 0xFD } \
  }

/*{24FC010F-AA0F-4310-AC48-E259FBD07AB0}*/
#define QCOM_FW_UPDATE_ACPI_FACP_FILE_IN_PAYLOAD_GUID \
  { \
    0x24FC010F, 0xAA0F, 0x4310, { 0xAC, 0x48, 0xE2, 0x59, 0xFB, 0xD0, 0x7A, 0xB0 } \
  }

/*{E455D6FD-16A5-45D2-8E07-1F5C992ABE28}*/
#define QCOM_FW_UPDATE_ACPI_FACS_FILE_IN_PAYLOAD_GUID \
  { \
    0xE455D6FD, 0x16A5, 0x45D2, { 0x8E, 0x07, 0x1F, 0x5C, 0x99, 0x2A, 0xBE, 0x28 } \
  }

/*{CA6BECA3-CD6D-44F4-AFB2-65076B81AD54}*/
#define QCOM_FW_UPDATE_ACPI_MADT_FILE_IN_PAYLOAD_GUID \
  { \
    0xCA6BECA3, 0xCD6D, 0x44F4, { 0xAF, 0xB2, 0x65, 0x07, 0x6B, 0x81, 0xAD, 0x54 } \
  }

/*{F760AEEB-B172-4522-AFB2-ECCDC523B598}*/
#define QCOM_FW_UPDATE_ACPI_FPDT_FILE_IN_PAYLOAD_GUID \
  { \
    0xF760AEEB, 0xB172, 0x4522, { 0xAF, 0xB2, 0xEC, 0xCD, 0xC5, 0x23, 0xB5, 0x98 } \
  }

/*{C7340E65-0D5D-43D6-ABB7-39751D5EC8E7}*/
#define QCOM_FW_UPDATE_FILE_VERSION_METADATA_IN_PAYLOAD_GUID \
  { \
    0xC7340E65, 0x0D5D, 0x43D6, { 0xAB, 0xB7, 0x39, 0x75, 0x1D, 0x5E, 0xC8, 0xE7 } \
  }

/*{01620DA3-F273-4401-9821-1D0E5169D8DA}*/
#define QCOM_FW_UPDATE_OPM_PUB_KEY_IN_PAYLOAD_GUID \
  { \
    0x01620DA3, 0xF273, 0x4401, { 0x98, 0x21, 0x1D, 0x0E, 0x51, 0x69, 0xD8, 0xDA } \
  }

/*{3998E865-A733-4812-97D7-4BC973EA3442}*/
#define QCOM_FW_UPDATE_OPM_PRIV_KEY_PAYLOAD_GUID \
  { \
    0x3998E865, 0xA733, 0x4812, { 0x97, 0xD7, 0x4B, 0xC9, 0x73, 0xEA, 0x34, 0x42 } \
  }

#define QCOM_FW_UPDATE_NULL_GUID \
  { \
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } \
  }




typedef struct{
      QCOM_FW_UPDATE_FW_FILE    File;
      UINT16                    *FileName;
      EFI_GUID                  FileGuid;

}QCOM_FW_UPDATE_FILE_IDENTIFIER;

/**
 * Returns pointer to a 
 *  filename string.
 *  
 *  
 * @param Name - File enum
 * @param NameString - Output - Pointer to namestring.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFileName(
    IN QCOM_FW_UPDATE_FW_FILE   Name,
    OUT UINT16                  **NameString

    );


/**
 * Returns pointer to a 
 *  file guid.
 *  
 *  
 * @param Name - File enum
 * @param NameString - Output - Pointer to FileGuid.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFileGuid(
    IN QCOM_FW_UPDATE_FW_FILE   Name,
    OUT const EFI_GUID          **FileGuid

    );

#endif
