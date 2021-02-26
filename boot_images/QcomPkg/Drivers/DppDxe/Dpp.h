/** 
  @file  Dpp.h 
  @brief DPP definitions.
**/
/*=============================================================================
  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Confidential and Proprietary.
=============================================================================*/
  
/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/16/15   mic	   Updated to support 64 bit arch 
07/16/13   mic     Added FAT-based DPP protocol support
03/25/12   jd      Initial revision
=============================================================================*/

#ifndef __DPP_H__
#define __DPP_H__

/*! 
 *      DPP Layout 
 *  
 *      +------------------------------+
 *      |    DPP Header                |
 *      +------------------------------+
 *      |    DPP Directory Header      |
 *      +------------------------------+
 *      |    DPP Directory Entry 0     |
 *      +------------------------------+
 *      |    ......                    |
 *      +------------------------------+
 *      |    DPP Directory Entry N - 1 |
 *      +------------------------------+
 *      |                              |
 *      |                              |
 *      |    Device Provision Data     |
 *      |                              |
 *      |                              |
 *      +------------------------------+
 *  
 */

#pragma pack (push, 1)

/*!
 * Data structure that describes the DPP Partition status
 */
typedef struct _DPP_PART_STATUS
{
  BOOLEAN Initialized;
} DPP_PART_STATUS, *PDPP_PART_STATUS;

/*!
 * DPP Partition Type
 */
typedef enum _DPP_PARTITION_TYPE 
{
  DPP_PARTITION_TYPE_UNKNOWN = 0,
  DPP_PARTITION_TYPE_BLOCK,
  DPP_PARTITION_TYPE_FAT,  
  DPP_PARTITION_TYPE_MAX
} DPP_PARTITION_TYPE;

/*!
 * DPP Provision Data Type
 */
typedef enum _DPP_DATA_TYPE 
{
  DPP_DATA_TYPE_QCOM = 0,
  DPP_DATA_TYPE_OEM,
  DPP_DATA_TYPE_MAX
} DPP_DATA_TYPE;

/*!
 * DPP Provision Data Entry Constants
 */
#define DPP_DATA_TYPE_MAX_LENGTH    4
#define DPP_DATA_NAME_MAX_LENGTH    256
#define DPP_MAX_DATA_ENTRIES        200

/*!
 * DPP Data Status
 */
typedef enum _DPP_DATA_STATUS 
{
  DPP_DATA_INVALID = 0,
  DPP_DATA_VALID
} DPP_DATA_STATUS;

/*!
 * Data structure that describes the Provision Data Entires
 */
typedef struct _DPP_DATA_ENTRY
{
  UINT16  Name[DPP_DATA_NAME_MAX_LENGTH+1];   /* Provision Data Name, unicode */
  UINT16  Type[DPP_DATA_TYPE_MAX_LENGTH+1];   /* Provision Data Type, unicode */
  UINT32  AllocatedSize;                      /* Space allocated */
  UINT32  UsedSize;                           /* Space currently used */
  UINT32  Valid;                              /* Indicate if it is a valid entry */
  UINT32  Offset;                             /* Offset of associated Provision Data within DPP data space*/
  UINT8   Reserved[484];                      /* Reserved */
} DPP_DATA_ENTRY, *PDPP_DATA_ENTRY;

/*!
 * Data structure that describes the DPP Directory Header
 */
typedef struct _DPP_DIRECTORY_HEADER
{
  UINT32  Size;                           /* Directory Header size */
  UINT32  DataEntriesOffset;              /* Offset of 1st data entry within DPP */
  UINT32  DataEntriesAllocated;           /* Max number of Provision Data entries allowed */
  UINT32  DataEntriesUsed;                /* Number of Provision Data entries currently in use */
  UINT32  DataSpaceOffset;                /* Offset of 1st provision data within DPP */     
  UINT32  DataSpaceAllocated;             /* Allocated provision data space in bytes */
  UINT32  DataSpaceUsed;                  /* Used provision data space in bytes */
  UINT8   Reserved[36];                   /* Reserved */
} DPP_DIRECTORY_HEADER, *PDPP_DIRECTORY_HEADER; 

/*!
 * Data structure that describes the DPP Directory
 */
typedef struct _DPP_DIRECTORY
{
  DPP_DIRECTORY_HEADER    Header;                     /* Header */
  DPP_DATA_ENTRY          ProvisionDataEntries[1];    /* Provision Data Entries */
} DPP_DIRECTORY, *PDPP_DIRECTORY;

/*!
 * DPP Header Constants
 */
#define DPP_SIGNATURE   0x20505044  /* 'DPP ' */
#define DPP_REVISION    0x00010000

/*!
 * Data structure that describes the DPP header
 */
typedef struct _DPP_HEADER
{
  UINT32  Signature;       /* Signature */
  UINT32  Revision;        /* Revision */
  UINT32  Size;            /* DPP header size */
  UINT32  Reserved1;       /* Reserved */
  UINT32  DirectoryOffset; /* Offset of Provisioning Data Directory within DPP */
  UINT32  BlockSize;       /* DPP block size */
  UINT64  PartSize;        /* DPP size*/
  UINT8   Reserved2[32];   /* Reserved */
} DPP_HEADER, *PDPP_HEADER;

/*!
 * DPP LBAs
 */
#define DPP_HEADER_LBA              0

#pragma pack (pop)

#endif	/* __DPP_H__ */

