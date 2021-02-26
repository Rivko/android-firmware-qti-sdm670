/** 
 @file PcieCfgRdWr.c

  Implements Pcie Configuration Read Write routines

  Copyright (c) 2016, Qualcomm Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ----    ----------------------------------------------------------
 07/25/16   ts      Initiail version for Pci config read write 8/16/32
=============================================================================*/

#include <Library/IoLib.h>
#include <Library/PcieConfigLib.h>

/***************************************************************************//**
 * @fn PcieConfigLibPciRead8
 * @brief Read one byte from config space
 *
 * Reads an 8 bit value from PCI configuration address
 *
 * @param[in] PciAddress     Address to read from
 *
 * @retval 8 bit value read.
 ******************************************************************************/
UINT8
EFIAPI
PcieConfigLibPciRead8 (
  IN UINTN PciAddress
)
{
  UINT32 Value;

  Value = MmioRead32 (PciAddress & ~ (0x3));
  Value >>= ( (PciAddress & 0x3) * 8);
  return (UINT8) (Value & 0xff);
}

/***************************************************************************//**
 * @fn PcieConfigLibPciRead16
 * @brief Read one word from config space
 *
 * Reads a 16 bit value from PCI configuration address
 *
 * @param[in] PciAddress     Address to read from
 *
 * @retval 8 bit value read.
 ******************************************************************************/
UINT16
EFIAPI
PcieConfigLibPciRead16  (
  IN UINTN PciAddress
)
{
  UINT32 Value;

  Value = MmioRead32 (PciAddress & ~ (0x3));
  Value >>= ( (PciAddress & 0x3) * 8);
  return (UINT16) (Value & 0xffff);
}

/***************************************************************************//**
 * @fn PcieConfigLibPciRead32
 * @brief Read one double word from config space
 *
 * Reads a 32 bit value from PCI configuration address
 *
 * @param[in] PciAddress     Address to read from
 *
 * @retval 8 bit value read.
 ******************************************************************************/
UINT32
EFIAPI
PcieConfigLibPciRead32 (
  IN UINTN PciAddress
)
{
  return MmioRead32 (PciAddress);
}

/***************************************************************************//**
 * @fn PcieConfigLibPciWrite8
 * @brief Write one byte to config space
 *
 * Wtites a given 8-bit balue to a PCI config space address
 *
 * @param[in] PciAddress     Address to read from
 * @param[in] Value          8 bit value to write
 *
 * @retval 8 bit value written.
 ******************************************************************************/
UINT8
EFIAPI
PcieConfigLibPciWrite8 (
  IN UINTN PciAddress,
  IN UINT8 Value
)
{
  UINT32 WriteValue;

  WriteValue = (UINT32) Value << ( (PciAddress & 0x3) * 8);
  MmioAndThenOr32 (PciAddress & ~ (0x3),
                   ~ ( (UINT32) 0xff << ( (PciAddress & 0x3) * 8)),
                   WriteValue);
  return Value;
}

/***************************************************************************//**
 * @fn PcieConfigLibPciWrite16
 * @brief Write one word to config space
 *
 * Writes a given 16 bit value to a PCI configuration address
 *
 * @param[in] PciAddress     Address to read write to
 * @param[in] Value          16 bit value to write
 *
 * @retval 16 bit value written.
 ******************************************************************************/
UINT16
EFIAPI
PcieConfigLibPciWrite16 (
  IN UINTN PciAddress,
  IN UINT16 Value
)
{
  UINT32 WriteValue;

  WriteValue = (UINT32) Value << ( (PciAddress & 0x3) * 8);
  MmioAndThenOr32 (PciAddress & ~ (0x3),
                   ~ ( (UINT32) 0xffff << ( (PciAddress & 0x3) * 8)),
                   WriteValue);
  return Value;
}

/***************************************************************************//**
 * @fn PcieConfigLibPciWrite32
 * @brief Write one double word to config space
 *
 * Writes a given 32 bit value to a PCI configuration address
 *
 * @param[in] PciAddress     Address to write to
 * @param[in] Value          32 bit value to write
 *
 * @retval 32 bit value written.
 ******************************************************************************/
UINT32
EFIAPI
PcieConfigLibPciWrite32 (
  IN UINTN PciAddress,
  IN UINT32 Value
)
{
  return MmioWrite32 (PciAddress, Value);
}
