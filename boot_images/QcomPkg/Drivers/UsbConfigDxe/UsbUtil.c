/** @file UsbConfig.c

  QCOM_USB_CONFIG_PROTOCOL driver utility functions.

  Copyright (c) 2011, 2013-2014, 2016, QUALCOMM Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/17/16   ck       Fix the CLK protocol usage
 06/05/14   amitg    MSM 8994 Bring Up, Added disable clock function
 02/22/13   amitg    Added support for the SSPhy register read/write
 03/30/11   sniedzie Created from PlatConfig.
 05/11/11   sbryksin Added platform/core type selection for UsbFn
 08/11/11   sbryksin Changes for 8960v2 w/ HSIC
 11/02/11   sbryksin Added initial APQ8064 revision
=============================================================================*/

#include <Base.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UsbConfigLib.h>
#include "HAL_hsusb.h"
#include "HAL_ssusb.h"
#include <Protocol/EFIClock.h>

extern EFI_CLOCK_PROTOCOL                    *ClockProtocol;

/**
  Reads PHY register through ULPI viewport

  @param  RegAddr                Address of register to be read from.
  @param  Data                   Pointer to store read value.

  @retval EFI_SUCCESS            Data read successfully.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       ULPI cmd failed

**/
EFI_STATUS
UlpiPhyRead (
  IN   UINT32 BaseAddr,
  IN   UINT8  RegAddr,
  OUT  UINT8  *Data
  )
{

  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       i       =  0;

  FNC_ENTER_MSG ();

  ARG_CHK (Data);

  HAL_HSUSB_WRITE (BaseAddr, HAL_HSUSB_ULPI_VIEWPORT_ADDR,
             HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK
             | (RegAddr << HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_SHFT));

  // Keep reading until run bit goes to 0
  for (i = 0; i < ULPI_PHY_MAX_RW_VERIFY_ATTEMPTS; ++i) {
    UINT32 ReadData = 0;

    ReadData = HAL_HSUSB_READ (BaseAddr, HAL_HSUSB_ULPI_VIEWPORT_ADDR);

    if (!(ReadData & HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK)) {
      *Data = (UINT8) (
                (ReadData & HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_BMSK)
                >> HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_SHFT
                );

      goto ON_EXIT;
    }
  }

  Status = EFI_DEVICE_ERROR;
  WRN_CHK ("failed to read ULPI PHY register x%02x", RegAddr);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}



/**
  Writes Data to the specified register over ULPI viewport.

  @param  RegAddr                Address of register to be written to.
  @param  Data                   Data to be written.

  @retval EFI_SUCCESS            Data written successfully.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       ULPI cmd failed

**/
EFI_STATUS
UlpiPhyWrite (
  IN UINT32  BaseAddr,
  IN UINT8   RegAddr,
  IN UINT8   Data
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       i       =  0;

  FNC_ENTER_MSG ();

  HAL_HSUSB_WRITE (BaseAddr, HAL_HSUSB_ULPI_VIEWPORT_ADDR,
             HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK
           | HAL_HSUSB_ULPI_VIEWPORT_ULPIRW_BMSK
           | (RegAddr << HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_SHFT)
           | Data );

  //wait until run bit goes to 0
  for (i = 0; i < ULPI_PHY_MAX_RW_VERIFY_ATTEMPTS; ++i) {
    if (!(HAL_HSUSB_READ (BaseAddr, HAL_HSUSB_ULPI_VIEWPORT_ADDR)
          & HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK)) {
      goto ON_EXIT;
    }
  }

  Status = EFI_DEVICE_ERROR;
  WRN_CHK ( "failed to write ULPI PHY register x%02x with data %02x",
    RegAddr, Data);

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Write masked Data to the specified register over ULPI viewport.

  @param  RegAddr                Address of register to be written to.
  @param  Data                   Data to set.
  @param  Bmsk                   Bitmask to set.

  @retval EFI_SUCCESS            Data written successfully.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       ULPI cmd failed

**/
EFI_STATUS
UlpiPhyWriteBmsk (
  IN UINT32  BaseAddr,
  IN UINT8   RegAddr,
  IN UINT8   Data,
  IN UINT8   Bmsk
  )
{
  EFI_STATUS  Status    =  EFI_SUCCESS;
  UINT8       ReadData  =  0;

  FNC_ENTER_MSG ();

  Status = UlpiPhyRead (BaseAddr, RegAddr, &ReadData);
  ERR_CHK ( "failed to read ULPI PHY register x%02x", RegAddr);

  Status =  UlpiPhyWrite (BaseAddr,RegAddr,
              (ReadData & ~Bmsk) | (Data & Bmsk));

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}

/**
  Reads PHY register through SS Phy

  @param  RegAddr                Address of register to be read from.
  @param  Data                   Pointer to store read value.

  @retval EFI_SUCCESS            Data read successfully.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       ULPI cmd failed

**/

EFI_STATUS SSUSBPhyRead (
  IN   UINT32 BaseAddr,
  IN   UINT32  RegAddr,
  OUT  UINT16  *Data
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  INT32       cnt    = 0;

  FNC_ENTER_MSG ();

  ARG_CHK (Data);

  //4.14.2  Read Sequence to SS PHY Control Register
  //1.    Write SS PHY register address to SS_CR_DATA_IN_REG bits of USB30_QSCRATCH_SS_CR_PROTOCOL_DATA_IN register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), RegAddr);

  //2.    Write 1 to bit SS_CR_CAP_ADDR_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(USB30_QSCRATCH_REG_BASE_OFFS),
                   HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_RMSK);

  //3.Read bit SS_CR_CAP_ADDR_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR register until it is cleared to 0.
  while (((HAL_HSUSB_READ (BaseAddr, HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(USB30_QSCRATCH_REG_BASE_OFFS)))
           & HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_RMSK)
          && (cnt++ < SS_PHY_MAX_RW_VERIFY_ATTEMPTS) );

  if (cnt >= SS_PHY_MAX_RW_VERIFY_ATTEMPTS)
  {
      Status = EFI_DEVICE_ERROR;
      WRN_CHK ( "failed to read PIPE3 PHY register x%02x with data %02x",RegAddr, Data);
      goto ON_EXIT;
  }

  //4. Write 1 to bit SS_CR_READ_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_READ register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_READ_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), HAL_SSUSB_CR_PROTOCOL_READ_RMSK);

  //5.    Read bit SS_CR_READ_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_READ register until it is cleared to 0.
  while (((HAL_HSUSB_READ (BaseAddr,  HAL_SSUSB_CR_PROTOCOL_READ_ADDR (USB30_QSCRATCH_REG_BASE_OFFS)))
         & HAL_SSUSB_CR_PROTOCOL_READ_RMSK)
         && (cnt++ < SS_PHY_MAX_RW_VERIFY_ATTEMPTS) );

  if (cnt >= SS_PHY_MAX_RW_VERIFY_ATTEMPTS)
  {
      Status = EFI_DEVICE_ERROR;
      WRN_CHK ( "failed to read PIPE3 PHY register x%02x with data %02x",RegAddr, Data);
      goto ON_EXIT;
  }

  //6.    Read the PHY register data from bits SS_CR_DATA_OUT_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_DATA_OUT.
  *Data = (UINT16) (HAL_HSUSB_READ (BaseAddr,
                    HAL_SSUSB_CR_PROTOCOL_DATA_OUT_ADDR(USB30_QSCRATCH_REG_BASE_OFFS))
                    );

  ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;

}


/**
  Write PHY register through SS Phy

  @param  RegAddr                Address of register to be read from.
  @param  Data                   Pointer to store read value.

  @retval EFI_SUCCESS            Data read successfully.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       PIPE3 cmd failed

4.14.1  Write Sequence to SS PHY Control Register
1.  Write SS PHY register address to SS_CR_DATA_IN_REG bits of USB30_QSCRATCH_SS_CR_PROTOCOL_DATA_IN register.
2.  Write 1 to bit SS_CR_CAP_ADDR_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR register.
3.  Read bit SS_CR_CAP_ADDR_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR register until it is cleared to 0.
4.  Write the write data to SS_CR_DATA_IN_REG bits of USB30_QSCRATCH_SS_CR_PROTOCOL_DATA_IN register.
5.  Write 1 to bit SS_CR_CAP_DATA_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_DATA register.
6.  Read bit SS_CR_CAP_DATA_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_DATA register until it is cleared to 0.
7.  Write 1 to bit SS_CR_WRITE_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_WRITE register.
8.  Read bit SS_CR_WRITE_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_WRITE register until it is cleared to 0.
**/

EFI_STATUS SSUSBPhyWrite (
  IN UINT32  BaseAddr,
  IN UINT32   RegAddr,
  IN UINT16   Data
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  INT32       cnt    = 0;

  FNC_ENTER_MSG ();

  ARG_CHK (Data);

  //4.14.1  Write Sequence to SS PHY Control Register
  //1. Write SS PHY register address to SS_CR_DATA_IN_REG bits of USB30_QSCRATCH_SS_CR_PROTOCOL_DATA_IN register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), RegAddr);

  //2. Write 1 to bit SS_CR_CAP_ADDR_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_RMSK);

  //3.    Read bit SS_CR_CAP_ADDR_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR register until it is cleared to 0.
  while (((HAL_HSUSB_READ (BaseAddr, HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(USB30_QSCRATCH_REG_BASE_OFFS)))
          & HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_RMSK)
         && (cnt++ < SS_PHY_MAX_RW_VERIFY_ATTEMPTS) );

  if (cnt >= SS_PHY_MAX_RW_VERIFY_ATTEMPTS)
  {
      Status = EFI_DEVICE_ERROR;
      WRN_CHK ( "failed to write PIPE3 PHY register x%02x with data %02x",RegAddr, Data);
      goto ON_EXIT;
  }

  //4. Write the write data to SS_CR_DATA_IN_REG bits of USB30_QSCRATCH_SS_CR_PROTOCOL_DATA_IN register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), Data);

  //5. Write 1 to bit SS_CR_CAP_DATA_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_DATA register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), HAL_SSUSB_CR_PROTOCOL_CAP_DATA_RMSK);

  // 6.  Read bit SS_CR_CAP_DATA_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_CAP_DATA register until it is cleared to 0.
  while (((HAL_HSUSB_READ (BaseAddr, HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(USB30_QSCRATCH_REG_BASE_OFFS)))
         & HAL_SSUSB_CR_PROTOCOL_CAP_DATA_RMSK)
         && (cnt++ < SS_PHY_MAX_RW_VERIFY_ATTEMPTS) );

  if (cnt >= SS_PHY_MAX_RW_VERIFY_ATTEMPTS)
  {
      Status = EFI_DEVICE_ERROR;
      WRN_CHK ( "failed to write PIPE3 PHY register x%02x with data %02x",RegAddr, Data);
      goto ON_EXIT;
  }

  // 7.  Write 1 to bit SS_CR_WRITE_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_WRITE register.
  HAL_HSUSB_WRITE (BaseAddr, HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(USB30_QSCRATCH_REG_BASE_OFFS), HAL_SSUSB_CR_PROTOCOL_WRITE_RMSK);
  // 8.  Read bit SS_CR_WRITE_REG of USB30_QSCRATCH_SS_CR_PROTOCOL_WRITE register until it is cleared to 0.
  while (((HAL_HSUSB_READ (BaseAddr, HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(USB30_QSCRATCH_REG_BASE_OFFS)))
          & HAL_SSUSB_CR_PROTOCOL_WRITE_RMSK)
          && (cnt++ < SS_PHY_MAX_RW_VERIFY_ATTEMPTS) );

  if (cnt >= SS_PHY_MAX_RW_VERIFY_ATTEMPTS)
  {
      Status = EFI_DEVICE_ERROR;
      WRN_CHK ( "failed to write PIPE3 PHY register x%02x with data %02x",RegAddr, Data);
      goto ON_EXIT;
  }
  ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;

}

/**
  Enables specified clock.

  @param  ClockName              Clock name as specified in
                                 QcomPkg/Application/ClockApp/ClockTestData.c.
  @param  minFrequency           Minimal Clock frequency
  @param  divide                 Frequency divide, has to be larger than 1 to take effect

  @retval EFI_SUCCESS            Clock is enabled.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       Clock is not on.
  @retval EFI_PROTOCOL_ERROR     Unable to get clock ID.

**/
EFI_STATUS
EFIAPI
EnableClock (
  IN   CONST CHAR8  *ClockName,
  IN   UINT32       minFrequency,
  IN   UINT32       divide
  )
{
  // Initialize locals.
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       ClockId;
  UINT32      FreqHz;

  FNC_ENTER_MSG ();

  if( !ClockProtocol || !ClockName ){
    DBG (EFI_D_ERROR, "Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Get clock ID.
  Status = ClockProtocol->GetClockID (ClockProtocol, ClockName, &ClockId);

  ERR_CHK ("failed to get clock ID for %a", ClockName);

  if (minFrequency)
  {
    Status = ClockProtocol->SetClockFreqHz (ClockProtocol, ClockId, minFrequency,
             EFI_CLOCK_FREQUENCY_HZ_AT_LEAST, &FreqHz);
    WRN_CHK( "failed to set %a clock frequency (%d)", ClockName, minFrequency);
    DBG (EFI_D_INFO, "%a clock frequency @(%d)", ClockName, FreqHz);
  }

  // Enable clock.
  Status = ClockProtocol->EnableClock (ClockProtocol, ClockId);
  ERR_CHK ("failed to enable %a clock", ClockName);

  if ( divide > 1 ) {
    Status = ClockProtocol->SetClockDivider (ClockProtocol, ClockId, divide);
    WRN_CHK ("failed to divide clock %a", ClockName);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Disables specified clock.

  @param  ClockName              Clock name as specified in
                                 QcomPkg/Application/ClockApp/ClockTestData.c.

  @retval EFI_SUCCESS            Clock is enabled.
  @retval EFI_INVALID_PARAMETER  One or more arguments has an unsupported value.
  @retval EFI_DEVICE_ERROR       Clock is not on.
  @retval EFI_PROTOCOL_ERROR     Unable to get clock ID.

**/
EFI_STATUS
EFIAPI
DisableClock (
  IN   CONST CHAR8  *ClockName
  )
{
  // Initialize locals.
  EFI_STATUS  Status  =  EFI_SUCCESS;
  BOOLEAN     IsEnabled =  FALSE;
  UINTN       ClockId;

  FNC_ENTER_MSG ();

  if( !ClockProtocol || !ClockName ){
    DBG (EFI_D_ERROR, "Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // Get clock ID.
  Status = ClockProtocol->GetClockID (ClockProtocol, ClockName, &ClockId);
  ERR_CHK ("failed to get clock ID for %a", ClockName);

  // Disable clock.
  Status = ClockProtocol->DisableClock (ClockProtocol, ClockId);
  ERR_CHK ("failed to disable %a clock", ClockName);

  //wait 1ms for clock to be disabled
  gBS->Stall (10000);
  
  // Get clock state. If the clock is enabled, report the error.
  // The state of being ON or OFF is not the same as being ENABLED or DISABLED. 
  // A clock needs to be turned on first,  before we can enable or disable it.
  // After we DISABLE a clock, we should check whether the clock is still ENABLED and not 
  // whether it is still ON.
  Status = ClockProtocol->IsClockEnabled (ClockProtocol, ClockId, &IsEnabled);
  ERR_CHK ("failed to determine if %a clock is on", ClockName);

  if (IsEnabled) {
    Status = EFI_DEVICE_ERROR;
    ERR_CHK ("clock remains enabled for %a", ClockName);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


