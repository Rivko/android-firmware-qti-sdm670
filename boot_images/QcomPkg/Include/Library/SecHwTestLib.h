#ifndef __SECHWTESTLIB_H__
#define __SECHWTESTLIB_H__

/** @file  
                             SecHwTestLib.h

  This is for function declaration of Debug Mode Checks

  Copyright (c) 2015 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/15   sk      Added Prodmode HOB
03/04/15   sk      Added definitions in header
01/08/15   sk      Initial version
===========================================================================*/

#include <PiPei.h>
#include <Pi/PiHob.h>

#define TZEXEC_SVC_WATERMARK_ID        0x0040000
#define AES256_KEY_SIZE                32 // output size == AES key size
#define AES128_KEY_SIZE                16
#define AES_IV_SIZE                    16

/* Status codes */

#define RETURN_SUCCESS                     0
#define RETURN_ENCRYPTION_FAILED           1
#define RETURN_PARAMETER_CHECK_FAILED      2
#define RETURN_SERVICE_UNAVAILABLE         3

//InformationType
#define ADAPTER_INFO_PLATFORM_SECURITY_GUID \
   {0x6be272c7, 0x1320, 0x4ccd, { 0x90, 0x17, 0xd4, 0x61, 0x2c, 0x01, 0x2b, 0x25 }}
  
extern EFI_GUID gQcomProdmodeInfoGuid;

#define PLATFORM_SECURITY_VERSION_VNEXTCS           0x00000003

#define PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE   0x00000001 // IHV
#define PLATFORM_SECURITY_ROLE_PLATFORM_IBV         0x00000002
#define PLATFORM_SECURITY_ROLE_IMPLEMENTOR_OEM      0x00000003
#define PLATFORM_SECURITY_ROLE_IMPLEMENTOR_ODM      0x00000004

// The below flags are all condensed to 1 bit to make it simpler
/*
#define FLAG_SECURE_BOOT                            0x01
#define FLAG_RPMB_CHECK                             0x02
#define FLAG_SHK_CHECK                              0x04
#define FLAG_ROLLBACK_CHECK                         0x08
#define FLAG_DUMMY_KEY_CHECK                        0x10
#define FLAG_PRODUCTION_DEVICE_CHECK                0x20
#define FLAG_PRODUCTION_MODE_CHECK                  0x40
*/
#define FLAG_SECURE_BOOT                            0x01
#define FLAG_RPMB_CHECK                             0x01
#define FLAG_SHK_CHECK                              0x01
#define FLAG_ROLLBACK_CHECK                         0x01
#define FLAG_DUMMY_KEY_CHECK                        0x01
#define FLAG_PRODUCTION_DEVICE_CHECK                0x01
#define FLAG_PRODUCTION_MODE_CHECK                  0x01



/* FUNCTION DECLARATIONS */
BOOLEAN DummyKeyCheck(VOID);
BOOLEAN GetProdmodeInfo(VOID);
BOOLEAN GetDebugPolicyFlag(VOID);

/* -------------------------------------- INTERFACE STRUCT -------------------------------------- */
/*
Version 
Return PLATFORM_SECURITY_VERSION_VNEXTCS.

Role 
The role of the publisher of this interface. 
Reference platform designers such as IHVs and IBVs are expected to return 
PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE and PLATFORM_SECURITY_ROLE_PLATFORM_IBV respectively. 
If the test modules from the designers are unable to fully verify all security features, 
then the platform implementers, OEMs and ODMs, will need to publish this interface 
with a role of Implementer.

ImplementationID 
Human readable vendor, model, & version of this implementation. 
For example “SiliconVendorX Chip1234 v1” and “BIOSvendorY BIOSz v2”.

SecurityFeaturesSize 
The size in bytes of the SecurityFeaturesRequired and SecurityFeaturesEnabled arrays. 
The arrays must be the same size.

SecurityFeaturesRequired 
IHV-defined bitfield corresponding to all security features which must be implemented 
to meet the security requirements defined by PLATFORM_SECURITY_VERSION Version. 
For example, 7 features may be required to be implemented to satisfy Version, 
a value of 0b01111111 might be reported. NOTE: Not checked by MSFT

SecurityFeaturesImplemented 
Publisher-defined bitfield corresponding to all security features which have implemented 
programmatic tests in this module. NOTE: Not checked by MSFT

SecurityFeaturesVerified 
Publisher-defined bitfield corresponding to all security features which have been verified 
implemented by this implementation. NOTE: This is the only 1 bit (LSB) checked by MSFT. 

ErrorString
A Null-terminated string, one failure per line (CR/LF terminated), 
with a unique identifier that the OEM/ODM can use to locate the documentation which will describe 
the steps to remediate the failure – a URL to the documentation is recommended. For example, 
“0x4827 JTAG not disabled http://somewhere.net/docs/remediate4827.html \r\n
0x2744 Platform Secure Boot key not provisioined http://somewhere.net/docs/remediate2744.html”
*/
typedef struct 
{
  UINT32  Version;
  UINT32  Role;
  CHAR16  ImplementationID[256];
  UINT32  SecurityFeaturesSize;
  UINT8   SecurityFeaturesRequired[2]; //Ignored for non-IHV
  UINT8   SecurityFeaturesImplemented[2];
  UINT8   SecurityFeaturesVerified[2];
  CHAR16  ErrorString[512];
} ADAPTER_INFO_PLATFORM_SECURITY, *PADAPTER_INFO_PLATFORM_SECURITY;
/* -------------------------------------- END, INTERFACE STRUCT -------------------------------------- */


/*------------------------------------------------------------
-------------------------------------------------------------*/ 
typedef struct WATER_MARK_REQ_S
{
   /* Command ID */
   UINT32 commandId;       
} __attribute__ ((packed)) WATER_MARK_REQ_T ,*PWATER_MARK_REQ_T;

typedef struct WATER_MARK_RSP_S
{    
   /* Command ID */
    UINT32 commandId;   
   /* error return */
    UINT32  status;
  /* output buffer, AES256 encrypted output size, output size == key size with padding*/
  UINT8 buf[AES256_KEY_SIZE]; 
} __attribute__ ((packed)) WATER_MARK_RSP_T, *PWATER_MARK_RSP_T;


/* Functions */
/**
  This function registers the Platform Security Adapter callback

  @param[in]  VOID
  @retval EFI_SUCCESS            The callback is registered.
  @retval EFI_NOT_FOUND          The AdapterInformationRegisterProtocol is not found.
  @retval EFI_DEVICE_ERROR       The device reported an error.
  @retval EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources.
**/
EFI_STATUS RegisterSecurityAdapter(VOID);


#endif /* __SECHWTESTLIB_H__ */

