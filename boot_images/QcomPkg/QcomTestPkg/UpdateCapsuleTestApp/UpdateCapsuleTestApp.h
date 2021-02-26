/** @file UpdateCapsuleTestApp.h
   
  Tests for capsules.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/29/11   jthompso Initial version
=============================================================================*/

#include <Uefi.h>

#define MAX_CAPSULE_SIZE 3*1024*1024  //3//    3MB
#define MAX_CAPSULE_COUNT 8

typedef enum {
    FW_TYPE_UNKNOWN,
    FW_TYPE_SYSTEM,
    FW_TYPE_DEVICE,
    FW_TYPE_DRIVER
} ESRT_FWTYPE;

typedef enum {
    FW_LAST_ATMPT_STATUS_SUCCESS,
    FW_LAST_ATMPT_STATUS_UNSUCCESSFUL,
    FW_LAST_ATMPT_STATUS_INSUFF_RESOURCE,
    FW_LAST_ATMPT_STATUS_INCORRECT_VERSION,
    FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT,
    FW_LAST_ATMPT_STATUS_AUTHEN_ERROR,
} ESRT_LAST_ATTEMPT_STATUS;
typedef struct
{
    EFI_CAPSULE_BLOCK_DESCRIPTOR List[2048];
}SCATTERGATHERLIST;


//Only give 1MB for capsules.
typedef struct
{
    EFI_CAPSULE_HEADER  Header;
    UINT8               data[MAX_CAPSULE_SIZE-sizeof(EFI_CAPSULE_HEADER)];
}CAPSULE_PLACEHOLDER;


typedef struct
{
    SCATTERGATHERLIST       ScatterGatherLists[MAX_CAPSULE_COUNT];
    CAPSULE_PLACEHOLDER     Capsules[MAX_CAPSULE_COUNT];

}CAPSULE_MAP;


typedef struct
{
    UINT8 data[EFI_PAGE_SIZE];

}EFI_PAGE;

typedef struct
{
    EFI_PAGE Pages[2048];

}PAGED_CAPSULE;

typedef struct
{

    EFI_GUID FWClass ;
    UINT32 FWType;
    UINT32 FWVer;
    UINT32 FWLeastCompVer;
    UINT32 CapsuleFlag;
    UINT32 LastAttemptVer;
    UINT32 LastAttemptStatus;

}ESRT_ENTRY;

typedef struct
{
    UINT32 FWResrcCnt;
    UINT32 FWResrcMax;
    UINT64 FWResrcVer;

}ESRT_TABLE_HEADER;
