#ifndef __TSFWUPGRADELIB_H__
#define __TSFWUPGRADELIB_H__

/** @file
                              TsFwUpgradeLib.h

  TsFwUpgradeDxe driver is responsible for doing firmware updation to TSC. It is composed
  of two parts: platform(target) independent and platform dependent. The platform
  independent part is same across different platforms, and platform dependent
  part vary on different platform based on the underlying touch screen controller 
  that exists on a particular platform .

  This file defines the interfaces both parts will follow.

  Copyright (c) 2012 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/13    dm       Changed GPIO lines for MSM8974
09/11/12    dm       Added HID Descriptor structure and macro
08/09/12    dm       Added required constants, structures
06/08/12    sr       Initial version.
===========================================================================*/

#pragma once

//typedefs
typedef unsigned char UCHAR;
typedef UCHAR* PUCHAR;
typedef unsigned long ULONG;
typedef UINT8 *PUINT8;
typedef UINT16 *PUINT16;
typedef VOID * PVOID;
typedef char CHAR;

#define CHANGE_LINE 61
#define RESET_LINE 60
#define I2CMODE_LINE 57

//#define HIDI2C

//Max Frame size for Atmel mXT1386 Chip set
#define MAX_FRAME_SIZE 532 

//I2C frequency
#define I2C_TS_FREQ 100

//Max object type number
#define OBJECT_TYPE_MAX 99

// string to check cfg format is raw or not
#define  RAW_FORMAT_STRING "OBP_RAW V1"

//command to be written in T6 object to store cfg data in NV memory
#define T6_BACKUPNV_WRITE_VALUE 0x55

//T6 object value
#define GEN_COMMANDPROCESSOR_T6  6

//minimum header size expected for object in cfg file (raw format)
#define MIN_OBJ_HEADER_SIZE     15

//#define SKIP_LINE(CfgPtr)   for(;CfgPtr && *CfgPtr!='\n';CfgPtr++); CfgPtr++;
//Only for test
#define SKIP_LINE(CfgPtr)   for(;CfgPtr && *CfgPtr!='\n';CfgPtr++); CfgPtr++;

//check for Bootloader mode
#define BOOTLOADER_MODE_TEST_BITS        ((1 << 7) | (1 << 6))

//check for Extended mode ID
#define BOOTLOADER_EXTENDED_MODE_ID_CHECK_BIT       (1 << 5)

//check for APP CRC FAIL Mode
#define APP_CRC_FAIL_MODE_TEST_BITS (1 << 6)

//check for controller ready to receive data
#define CONTROLLER_IS_READY_TO_RECIEVE_FRAME_DATA_TEST_BIT7       ((1 << 7))
#define CONTROLLER_IS_READY_TO_RECIEVE_FRAME_DATA_TEST_BIT6       ((1 << 6))

//Helper Macros

//checking given char is hex or not
#define IsHexChar(_char) (((_char >= 'A') && (_char <= 'Z')) || ((_char >= '0') && (_char <= '9')))

//checking for given char is number or not
#define IsHexNumericChar(_char) ((_char >= '0') && (_char <= '9'))

//checking given char is alpha char or not
#define IsHexAlphaChar(_char) ((_char >= 'A') && (_char <= 'Z'))

//this macro generated 16 bit in little endian format
#define GENERATE_16_BIT(MSB,LSB)   (UINT16)((((UINT16)(MSB)) << 8) | (LSB))

/* TS controller Operation Status */
typedef enum
{
  E_SUCCESS,
  E_FAILURE,
  E_BACKUP_FAILED,
  E_READ_FAILED,
  E_WRITE_FAILED,
  E_RESTORE_FAILED,
  E_DATA_MISMATCH,
  STATUS_INTERNAL_ERROR
} TS_STATUS;

//HID Descriptor structure
typedef struct _HID_DESCRIPTOR
{
  UINT16	wHIDDescLength;
  UINT16	bcdVersion;
  UINT16	wReportDescLength;
  UINT16	wReportDescRegister;
  UINT16	wInputRegister;
  UINT16	wMaxInputLength;
  UINT16	wOutputRegister;
  UINT16	wMaxOutputLength;
  UINT16	wCommandRegister;
  UINT16	wDataRegister;
  UINT16	wVendorID;
  UINT16	wProductID;
  UINT16	wVersionID;
  UINT8	  RESERVED [4];
} HID_DESCRIPTOR, *PHID_DESCRIPTOR;

// FW upgrade status
typedef enum _FWUPGSTATUS
{
  Success = 0,
  RecoverableError,
  NonRecoverableError
} FWUPGSTATUS;

//Different Bootloader States
typedef enum _AT_FW_UPDATE_STATE
{
   ENTER_BOOTLOADER_MODE = 0,
	APPLICATION_UPDTATE_UNLOCK_MODE,
	READY_CONTROLLER_TO_RECIEVE_FRAME_DATA,
	SEND_FRAME_DATA_TO_CONTROLLER,
	READY_CONTROLLER_TO_FRAME_CRC_CHECK,
	FRAME_CRC_CHECK_RESULT,
	FRAME_CRC_PASSED,
	FRAME_CRC_FAILED,
	FIRMWARE_UPDATE_PROCEDURE_DONE,
	FIRMWARE_WAIT_FOR_READY_INTERRUPT_TO_EXIT
}AT_FW_UPDATE_STATE, *PAT_FW_UPDATE_STATE;

//Data Frame Structure used in the ATMEL Firmware Update File
typedef struct _AT_FIRMWARE_UPDATE_FRAME {
   //2 Byte Length field(FrameLength[0] will hold the LSB and FrameLength[1] will contain the MSB of the Frame Length) 
   //: Value details the number of bytes in the frame i.e Encrypted Frame Data Size + 2 Byte CRC. 
	UINT8 FrameLength[2];				
	//Encrypted Frame Data
   UINT8 FrameData[MAX_FRAME_SIZE-4];	
   //2 Byte CRC( FrameCRC[0] will hold the LSB and FrameCRC[1] will contain the MSB of the Frame CRC)
	UINT8 FrameCRC[2];					
}AT_FIRMWARE_UPDATE_FRAME,*PAT_FIRMWARE_UPDATE_FRAME;

//Touch controller device ID structure
typedef struct _AT_ID_INFO {
    union {
        struct {                    // Offset
            UINT8 Family;           // 0
            UINT8 Variant;          // 1
            UINT8 Version;          // 2
            UINT8 Build;            // 3
            UINT8 MatrixXSize;      // 4
            UINT8 MatrixYSize;      // 5
            UINT8 NumberOfObjects;  // 6
        } Field;
        UINT8 RawMem[7];
    };
} AT_ID_INFO, *PAT_ID_INFO;

//Touch controller Object element structure
typedef struct _AT_TYPE_ELEMENT {
    union {
        struct {                    // Offset
            UINT8 Type;             // 0
            UINT8 StartposLsb;      // 1
            UINT8 StartposMsb;      // 2
            UINT8 SizeMinus1;       // 3
            UINT8 InstancesMinus1;  // 4
            UINT8 IdsPerInstance;   // 5
        } Field;
        UINT8 RawMem[6];
    };
} AT_TYPE_ELEMENT, *PAT_TYPE_ELEMENT;

//object Instance structure
typedef struct _AT_OBJECT_INSTANCE {
    PAT_TYPE_ELEMENT    pObjectType;    // Pointer to object type
    UINT16              RegAddress;     // Start address of this instance
    BOOLEAN             UpdatePending;  // Is there an update we need to commit
    PVOID               pConfig;        // Pointer to configuration struct 
} AT_OBJECT_INSTANCE, *PAT_OBJECT_INSTANCE;

EFI_STATUS
PerformFWUpgradeProc(PUCHAR Buffer, UINTN PayloadSize);

EFI_STATUS TsFwUpgradeInit(VOID);


#endif /* __TSFWUPGRADELIB_H__ */
