/**
  @file FruLib.h

  @brief Functions to read and write to Fru through SSIF

  Fru is implemented as according to Platform Management FRU Information Storage Definition v1.0 rev 1.2

  Copyright (c) 2015 by Qualcomm Technologies, Inc.
  All Rights Reserved.

 **/

/*=======================================================================
  Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
02/12/12   dgurjar  created
========================================================================*/

#include <Library/DebugLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SSIF.h>

extern EFI_GUID gQcomSsifProtocolGuid;

#define FRU_SPECIFICATION_VERSION         0x1

#define TYPE_CODE_BINARY_OR_UNSPECIFIED   0x0
#define TYPE_CODE_BCD_PLUS                0x1
#define TYPE_CODE_6_BIT_ASCII             0x2
#define TYPE_CODE_8_BIT_ASCII             0x3

#define END_OF_FIELD                      0xC

#define DEFAULT_FRU_DEVICE                0x0

const static CHAR8 *Bcd_Plus = "0123456789 -.///";

typedef struct FruField {
  struct {
    UINT8 Length  : 6;
    UINT8 Type    : 2;
  } TypeLength;
  CHAR8 *Data;
  UINTN AsciiLength;
} FruField;

typedef struct FruHeader {
  UINT8   Version;
  struct {
    UINT8 Internal;
    UINT8 Chassis;
    UINT8 Board;
    UINT8 Product;
    UINT8 Multi;
  } Offsets;
  UINT8 Pad;
  UINT8 Checksum;
} FruHeader;

typedef struct FruAreaInternalUse {
  UINT8   Version;
  UINT8   *Data;
} FruAreaInternalUse;

typedef struct FruAreaChassis {
  UINT8     AreaVersion;
  UINT8     AreaLength;
  UINT8     ChassisType;
  FruField  PartNumber;
  FruField  SerialNumber;
  FruField  *AdditionalFields;
} FruAreaChassis;

typedef struct FruAreaBoard {
  UINT8     AreaVersion;
  UINT8     AreaLength;
  UINT8     LanguageCode;
  UINT8     ManufactureDate[3];
  FruField  Manufacturer;
  FruField  ProductName;
  FruField  SerialNumber;
  FruField  PartNumber;
  FruField  FruFileID;
  FruField  *AdditionalFields;
} FruAreaBoard;

typedef struct FruAreaProduct {
  UINT8     AreaVersion;
  UINT8     AreaLength;
  UINT8     LanguageCode;
  FruField  ManufacturerName;
  FruField  ProductName;
  FruField  PartModelNumber;
  FruField  SerialNumber;
  FruField  AssetTag;
  FruField  FruFileID;
  FruField  *AdditionalFields;
} FruAreaProduct;



/**
  Read the FruHeader and update gFruHeader with the data
  The header is located at offset 0 on the specified device

  @param[in]  FruDeviceId     The device id of the header we are reading

  @retval EFI_SUCCESS         Header read and global updated
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
ReadFruHeader (
  IN  UINT8   FruDeviceId
);

/**
  Write the FruHeader and update gFruHeader with the data if successful
  The header is located at offset 0 on the specified device

  @param[in]  FruDeviceId     The device id of the header we are reading

  @retval EFI_SUCCESS         Header written and global updated
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
WriteFruHeader (
  IN  UINT8     FruDeviceId,
  IN  FruHeader Header
);

/**
  Given a Fru Header structure and lengths of all of its fields, determines if
  the header is valid

  @param[in]  FruHeader       The header data structure
  @param[in]  *AreaLength     Length of Area *

  @retval EFI_SUCCESS         Area read successfully
  @retval other               Unable to read Fru data

**/
static
EFI_STATUS
VerifyFruHeader (
  IN  FruHeader        Header,
  IN  OPTIONAL UINTN   InternalAreaLength,
  IN  OPTIONAL UINTN   ChassisAreaLength,
  IN  OPTIONAL UINTN   BoardAreaLength,
  IN  OPTIONAL UINTN   ProductAreaLength,
  IN  OPTIONAL UINTN   MultiAreaLength
);

/**
  Given a Buffer and an offset, parses a FruField from that location in the buffer. Updates the Offset to point to the location after the field

  @param[in]       Buffer          The raw data
  @param[in]       Offset          Offset into data marking the start of a FruField
  @param[out]      Offset          Offset into data marking the end of a FruField
  @param[out]      FieldData       Parsed and Allocated data structure

  @retval EFI_SUCCESS         Field parsed successfully
  @retval other               Unable parse data

**/
static
EFI_STATUS
ReadFruField (
  IN      UINT8     *Buffer,
  IN OUT  UINTN     *Offset,
  OUT     FruField  *FieldData
);

/**
  Given a Buffer and an offset, parses a FruField from that location in the buffer. Updates the Offset to point to the location after the field

  @param[in]       Buffer          Buffer we want written
  @param[in]       Offset          Offset into data marking the start of a FruField
  @param[out]      Offset          Offset into data marking the end of a FruField
  @param[in]       FieldData       The field we want written at the given offset

  @retval EFI_SUCCESS         Field parsed successfully
  @retval other               Unable parse data

**/
static
EFI_STATUS
WriteFruField (
  IN OUT  UINT8     *Buffer,
  IN OUT  UINTN     *Offset,
  IN      FruField  FieldData
);


/**
  Given an offset and a device id a length and a buffer, writes the area buffer to the Fru at the  offset

  @param[in]  FruDeviceId     The device id of the header we are reading
  @param[in]  AreaOffset      The offset to read from
  @param[in]  Data            The data to write ta, NULL if nothing was read
  @param[in]  AreaLength      The length of the area buffer

  @retval EFI_SUCCESS         Area read successfully
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
WriteAreaData (
  IN  UINT8   FruDeviceId,
  IN  UINT16  AreaOffset,
  IN  UINT8   *Data,
  IN  UINTN   AreaLength
);


/**
  Given an offset and a device id, reads the Fru area at that location and returns
  it in a dynamically allocated buffer

  @param[in]  FruDeviceId     The device id of the header we are reading
  @param[in]  AreaOffset      The offset to read from
  @param[out] Data            The read allocated data, NULL if nothing was read

  @retval EFI_SUCCESS         Area read successfully
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
ReadAreaData (
  IN  UINT8   FruDeviceId,
  IN  UINT16  AreaOffset,
  OUT UINT8   **Data
);

/**
  Given an offset a device id and offset, reads the Board Info Area from that offset

  @param[in]  FruDeviceId     The device id of the header we are reading
  @param[in]  AreaOffset      Offset we want to write to

  @retval EFI_SUCCESS         Area read successfully
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
ReadBoardInfoArea (
  IN UINT8  FruDeviceId,
  IN UINT16 AreaOffset
);

/**
  Given an offset a device id and offset, writes the he Chassis Info Area to that offset

  @param[in]  FruDeviceId     The device id of the header we are reading
  @param[in]  AreaOffset      Offset we want to write to
  @param[in]  Chassis         The FruAreaChassis we want to write

  @retval EFI_SUCCESS         Area read successfully
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
WriteFruAreaBoard (
  IN UINTN FruDeviceId,
  IN UINTN AreaOffset,
  IN FruAreaBoard Board
);

/**
  Given an offset a device id and offset, reads the Chassis Info Area from that offset

  @param[in]  FruDeviceId     The device id of the header we are reading
  @param[in]  AreaOffset      Offset we want to write to

  @retval EFI_SUCCESS         Area read successfully
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
ReadChassisInfoArea (
  IN UINT8 DeviceId,
  IN UINT16 AreaOffset
);

/**
  Given an offset a device id and offset, writes the he Chassis Info Area to that offset

  @param[in]  FruDeviceId     The device id of the header we are reading
  @param[in]  AreaOffset      Offset we want to write to
  @param[in]  Chassis         The FruAreaChassis we want to write

  @retval EFI_SUCCESS         Area read successfully
  @retval EFI_NOT_FOUND       gSsifProtocol uninitialized
  @retval other               Unable to read Fru header through Ssif

**/
static
EFI_STATUS
WriteFruAreaChassis (
  IN UINTN FruDeviceId,
  IN UINTN AreaOffset,
  IN FruAreaChassis Chassis
);

/**
  Given a device id reads all relevant Fru information into global buffers

  @param[in]  FruDeviceId     The device id of the header we are reading

  @retval EFI_SUCCESS         Area read successfully
  @retval other               Unable to read Fru data

**/
EFI_STATUS
ReadFruData (
  UINT8   FruDeviceId
);


/**
  Given a device id reads all relevant Fru information into global buffers

  @param[in]  FruDeviceId     The device id of the header we are reading

  @retval EFI_SUCCESS         Area read successfully
  @retval other               Unable to read Fru data

**/
EFI_STATUS
WriteFruData (
  IN  UINT8                     FruDeviceId,
  IN  FruHeader                 Header,
  IN  OPTIONAL FruAreaChassis   *ChassisArea,
  IN  OPTIONAL FruAreaBoard     *BoardArea,
  IN  OPTIONAL FruAreaProduct   *ProductArea
);

EFI_STATUS
GetFruAreaChassis (
  FruAreaChassis *ChassisArea
);

EFI_STATUS
GetFruAreaBoard (
  FruAreaBoard *BoardArea
);

EFI_STATUS
GetFruAreaProduct (
  FruAreaProduct *ProductArea
);
