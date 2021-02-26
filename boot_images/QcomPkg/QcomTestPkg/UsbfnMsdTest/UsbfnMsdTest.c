/** @file UsbfnMsdTest.c

 Tests for USB Mass Storage class

 Copyright (c) 2010-2012,2014-2015 QUALCOMM Technologies Inc. All rights reserved.

 **/

/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/30/15   ar      Remove NULL HandleFilter in GetSDBlkIoHandle()
 04/30/14   ar      Cleanup for NDEBUG build  
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 11/07/11   cching  Initial check-in.

 =============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIUsbMsdPeripheral.h>
#include <Protocol/BlockIo.h>
#include <Guid/EventGroup.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include "UsbError.h"

#define NUM_LUN_IN_TEST 2

// Terminate output with a newline, and eliminate the need for double parens.
#define DBG(EfiD, Str, args...) \
  AsciiPrint ("%a: " Str "\n",__FUNCTION__, ##args);

// For debug stack info.
#define FNC_ENTER_MSG() DBG (EFI_D_INFO, "++")
#define FNC_LEAVE_MSG() DBG (EFI_D_INFO, "--")

#define ERR_CHK(ErrStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, ErrStr, __VA_ARGS__); \
      goto ON_EXIT; \
    } \
  } while (0)

// Highly undesirable, but continue anyway.
#define WRN_CHK(WrnStr, ...) \
  do { \
    if (EFI_ERROR (Status)) { \
      DBG (EFI_D_ERROR, WrnStr, __VA_ARGS__); \
    } \
  } while (0)

extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiSdRemovableGuid;

static EFI_USB_MSD_PERIPHERAL_PROTOCOL* UsbMsdProtocol;
static HandleInfo HandleInfoListEmmc[16]; //contains the partitions in eMMC
static UINTN NumEmmcHandles = 0;
static HandleInfo HandleInfoListSdCard[16];
static UINTN NumSdHandles = 0;

EFI_STATUS AssignEmmcPartitionToLun(UINT8 LunNum)
{
  EFI_STATUS Status;
  EFI_BLOCK_IO_PROTOCOL *BlkIo = NULL;
  UINTN Index = 0;

  //
  // TODO: Currently there are two handles looks identical. Need to have a way to filter out
  // and select correct eMMC (BOOTME) 
  // Choose the 2nd handle
  //
  if(0 < NumEmmcHandles){
        BlkIo = HandleInfoListEmmc[NumEmmcHandles - 1].BlkIo;
  }

  if(NULL == BlkIo){
    DBG(EFI_D_INFO, "Cannot find BlkIo handle of eMMC specified");
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }
  DBG(EFI_D_INFO, "Assign HandleInfoListEmmc[%d].BlkIo 0x%x to LUN %d", Index, BlkIo, LunNum);
  Status = UsbMsdProtocol->AssignBlkIoHandle(UsbMsdProtocol, BlkIo, LunNum);

  ON_EXIT:
  return Status;
}

EFI_STATUS AssignSdCardBlkIoToLun(UINT8 LunNum)
{
  EFI_STATUS Status;
  EFI_BLOCK_IO_PROTOCOL *BlkIo = NULL;
  UINTN Index = 0;

  for (Index = 0; Index < NumSdHandles; ++Index) {
    if (TRUE == HandleInfoListSdCard[Index].BlkIo->Media->LogicalPartition) {
      BlkIo = HandleInfoListSdCard[Index].BlkIo;
      break;
    }
  }

  if(NULL == BlkIo){
        DBG(EFI_D_INFO, "Cannot find BlkIo handle of eMMC specified");
        Status = EFI_UNSUPPORTED;
        goto ON_EXIT;
  }
  DBG(EFI_D_INFO, "Assign HandleInfoListSdCard[%d].BlkIo 0x%x to LUN %d", Index, BlkIo, LunNum);
  Status = UsbMsdProtocol->AssignBlkIoHandle(UsbMsdProtocol, BlkIo, LunNum);

  ON_EXIT:
  return Status;
}

EFI_STATUS GetSDBlkIoHandle()
{
  UINT32 Attribs = 0;
  UINT32 MaxHandles;
  EFI_BLOCK_IO_PROTOCOL *BlkIo;
  PartiSelectFilter HandleFilter;
  EFI_STATUS Status;
  UINTN index = 0;

  Attribs |= BLK_IO_SEL_MEDIA_TYPE_REMOVABLE;

  MaxHandles = sizeof(HandleInfoListSdCard) / sizeof(*HandleInfoListSdCard);
  HandleFilter.PartitionType = 0;
  HandleFilter.RootDeviceType = &gEfiSdRemovableGuid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoListSdCard, &MaxHandles);

  DBG(EFI_D_INFO, "status %p, num of Block IO : %d", Status, MaxHandles);

  if ((Status != EFI_SUCCESS) || (MaxHandles == 0))
    return EFI_NOT_FOUND;

  for (index = 0; index < MaxHandles; ++index) {

    BlkIo = HandleInfoListSdCard[index].BlkIo;

    DBG(
        EFI_D_INFO,
        "SD card Handle 0x%x, MediaId 0x%x, RemoveableMedia 0x%x, "
        "MediaPresent 0x%x, LogicalPartition 0x%x, ReadOnly 0x%x, "
        "WriteCaching 0x%x, BlockSize 0x%x, IoAligh 0x%x, LastBlock 0x%x\n",
        BlkIo, BlkIo->Media->MediaId, BlkIo->Media->RemovableMedia, BlkIo->Media->MediaPresent, BlkIo->Media->LogicalPartition, BlkIo->Media->ReadOnly, BlkIo->Media->WriteCaching, BlkIo->Media->BlockSize, BlkIo->Media->IoAlign, BlkIo->Media->LastBlock);

  }

  NumSdHandles = MaxHandles;

  return EFI_SUCCESS;

}

EFI_STATUS GetEmmcPartitionBlkIoHandle()
{
  UINT32 Attribs = 0;
  UINT32 MaxHandles;
  EFI_BLOCK_IO_PROTOCOL *BlkIo;
  EFI_STATUS Status;
  UINTN index = 0;
  PartiSelectFilter HandleFilter;

  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
  Attribs |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;

  MaxHandles = sizeof(HandleInfoListEmmc) / sizeof(*HandleInfoListEmmc);
  HandleFilter.PartitionType = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoListEmmc, &MaxHandles);

  DBG(EFI_D_INFO, "status %p, num of Block IO : %d", Status, MaxHandles);

  if ((Status != EFI_SUCCESS) || (MaxHandles == 0))
    return EFI_NOT_FOUND;

  for (index = 0; index < MaxHandles; ++index) {

    BlkIo = HandleInfoListEmmc[index].BlkIo;

    DBG(
        EFI_D_INFO,
        "Emmc Partition Handle 0x%x, MediaId 0x%x, RemoveableMedia 0x%x, "
        "MediaPresent 0x%x, LogicalPartition 0x%x, ReadOnly 0x%x, "
        "WriteCaching 0x%x, BlockSize 0x%x, IoAligh 0x%x, LastBlock 0x%x\n",
        BlkIo, BlkIo->Media->MediaId, BlkIo->Media->RemovableMedia, BlkIo->Media->MediaPresent, BlkIo->Media->LogicalPartition, BlkIo->Media->ReadOnly, BlkIo->Media->WriteCaching, BlkIo->Media->BlockSize, BlkIo->Media->IoAlign, BlkIo->Media->LastBlock);
  }

  NumEmmcHandles = MaxHandles;

  return EFI_SUCCESS;
}

/**
 The user Entry Point for Application. The user code starts with this function
 as the real entry point for the application.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS       The entry point is executed successfully.
 @retval other             Some error occurs when executing this entry point.

 **/
EFI_STATUS
EFIAPI
UefiMain( IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN Index = 0;
  UINT8 NumOfLunsSupported = 0;
  EFI_STATUS Status = EFI_SUCCESS;


  TEST_START("UsbfnMsdTest");

  // Locate the USB MSD protocol.
  Status = gBS->LocateProtocol(&gEfiUsbMsdPeripheralProtocolGuid, NULL,
      (VOID **) &UsbMsdProtocol);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  // Get handles for SD card
  Status = GetSDBlkIoHandle();
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  // Get handles for eMMC partitions
  Status = GetEmmcPartitionBlkIoHandle();
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  //
  // Query how many LUNs can be supported
  //
  DBG(EFI_D_INFO,"Query how many LUNs can be supported");
  Status = UsbMsdProtocol->QueryMaxLun(UsbMsdProtocol, &NumOfLunsSupported);
  DBG(EFI_D_ERROR,"Num of LUNs supported = %d",NumOfLunsSupported);

  //
  // Assign the SD card BlkIo Handle to LUN2. The operation should fail
  // since the device can only support LUN0 and LUN2
  //
  DBG(EFI_D_INFO,"Assign the SD card BlkIo handle to LUN 2. "
    "The operation should fail since only LUN 0 & 1 is supported");
  Status = AssignSdCardBlkIoToLun(NumOfLunsSupported);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  //
  // Assign the SD card BlkIo handle to LUN0
  //
  DBG(EFI_D_INFO,"Assign the SD card BlkIo handle to LUN0");
  Status = AssignSdCardBlkIoToLun(0);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  //
  // Now Assign SD blkio handle to LUN2. The operation should fail since SD
  // is currently assoicated with LUN1
  //
  DBG(EFI_D_INFO," Now assign the same SD BLKIO handle to LUN1. "
    "The operation should fail since a handle can only be associated with one LUN");
  Status = AssignSdCardBlkIoToLun(1);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  //
  // Assign one eMMC partition BlkIo handle to LUN1
  //
  DBG(EFI_D_INFO,"Assign one eMMC partition BlkIo handle to LUN1");
  Status = AssignEmmcPartitionToLun(1);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);


  DBG(EFI_D_INFO,"Check host PC, two removable disks should appear");

  //
  // Stall 60 second before disable the LUNs
  //
  DBG(EFI_D_INFO,"Wait 60 second before testing the ability to disable the LUNs.");
  gBS->Stall(60 * 1000000);

  //
  // Remove the mapping for LUNs
  //
  DBG(EFI_D_INFO,"Remove the mapping for LUNs");

  for (Index = 0; Index < NUM_LUN_IN_TEST; ++Index) {
    Status = UsbMsdProtocol->AssignBlkIoHandle(UsbMsdProtocol, NULL, Index);
    USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);
  }

  DBG(EFI_D_INFO,"Check host PC, two removable disks should disappear");


  //
  // Stall 60 second before disable the LUNs
  //
  DBG(EFI_D_INFO,"Wait 60 second before testing the ability to re-enable the LUNs.");
  gBS->Stall(60 * 1000000);

  //
  // exchange the mapping
  //
  DBG(EFI_D_INFO,"Exchange the LUN mapping.");
  DBG(EFI_D_INFO,"Assign the SD card BlkIo handle to LUN1");
  Status = AssignSdCardBlkIoToLun(1);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  //
  // Remap SD blkio to LUN1
  //
  DBG(EFI_D_INFO,"Assign the eMMC BlkIo handle to LUN0");
  Status = AssignEmmcPartitionToLun(0);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);

  DBG(EFI_D_INFO,"Check host PC, two removable disks should appear, and driver"
    " letter should be exchanged");

  //
  // Stall 60 second before doing another test
  //
  DBG(EFI_D_INFO,"Wait for 60 second");
  gBS->Stall(60 * 1000000);

  //
  // Now Assign third BLKIO to LUN 1. The operation should fail since
  // LUN 1 is currently associated with the SD blkio handle
  //
  DBG(EFI_D_INFO,"Now assign another the eMMC BlkIo handle to LUN1. "
    "The operation should fail since LUN 1 is in use");
  Status = UsbMsdProtocol->AssignBlkIoHandle(UsbMsdProtocol, HandleInfoListEmmc[0].BlkIo, 1);
  USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);


  //
  // Test ends. Remove all association
  //
  DBG(EFI_D_INFO,"Remove all association before the test ends");
  for (Index = 0; Index < NUM_LUN_IN_TEST; ++Index) {
    Status = UsbMsdProtocol->AssignBlkIoHandle(UsbMsdProtocol, NULL, Index);
    USB_ASSERT_EFI_ERROR_RETURN (Status, EFI_DEVICE_ERROR);
  }


  TestStatus("UsbfnMsdTest", Status);
  TEST_STOP("UsbfnMsdTest");

  return Status;
}
