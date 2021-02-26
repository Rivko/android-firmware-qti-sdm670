/** @file SBITestInterface.h

  SBI UEFI Test Interface

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/12/11   unr     Added save and restore functionality to test descriptors
 03/02/11   unr     Initial revision for UEFI SBI

=============================================================================*/

#ifndef __SBITESTINTERFACE_H__
#define __SBITESTINTERFACE_H__

#define SBI_TEST_DEV_NAME_MAX  12

/**
  @brief
  SBI read or write type
 */
typedef enum
{
  SBI_TEST_READ,
  SBI_TEST_WRITE,
  SBI_TEST_SAVE,
  SBI_TEST_RESTORE
} SBI_TEST_READ_WRITE_TYPE;

typedef UINT32 SBI_TEST_READ_WRITE_ADDR;
typedef UINT32 SBI_TEST_READ_WRITE_DATA;

/**
  @brief
  Structure defining SBI Transfer Descriptor
 */
typedef struct
{
  SBI_TEST_READ_WRITE_TYPE      type;   /**<-- Read or Write */
  SBI_TEST_READ_WRITE_ADDR      addr;   /**<-- Address */
  SBI_TEST_READ_WRITE_DATA      data;   /**<-- Data to write or data expected
                                               during read */
} SBI_TEST_TRANSFER_DESCRIPTOR;

/**
  @brief
  Structure defining SBI Test Descriptor
 */
typedef struct
{
  UINT8                         device[SBI_TEST_DEV_NAME_MAX];
  UINT32                        num_descriptors;
  SBI_TEST_TRANSFER_DESCRIPTOR *descriptor;
} SBI_TEST_DESCRIPTOR;

/**
  UINT32 SBITestGetNumDevices ()

  @brief
  Gets the number of SBI devices that need to be tested

  @retval SBI_TEST_NUM_DEVICES

 */
UINT32 SBITestGetNumDevices (void);

/**
  SBI_TEST_DESCRIPTOR *SBIGetTestDescriptor (UINT32 device_index)

  @brief
  Gets the test descriptor for the device at device_index

  @param[in]  device_index          From 0 to (SBI_TEST_NUM_DEVICES - 1)

  @retval Pointer to SBI_TEST_DESCRIPTOR on success and NULL on failure

 */
SBI_TEST_DESCRIPTOR *SBIGetTestDescriptor (UINT32 device_index);

/**
  UINT8 *SBIGetDeviceName (UINT32 device_index)

  @brief
  Gets the device name for the device at device_index

  @param[in]  device_index          From 0 to (SBI_TEST_NUM_DEVICES - 1)

  @retval Pointer to device name on success and NULL on failure

 */
UINT8 *SBIGetDeviceName (UINT32 device_index);

#endif	/* __SBITESTINTERFACE_H__ */
