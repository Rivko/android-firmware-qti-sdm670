/** 
 @file PcieLink.c

  Implements link initialization related routines.

  Copyright (c) 2014-2017, Qualcomm Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---    ------------------------------------------------------------
 05/22/17  tselvam  TurnOff port clocks when link training failed
 05/05/17  tselvam  Included support to perform RMW on PHY init sequence
 04/21/17 tselvam  Optimized by removing macro implementation for register
                   writes and polling functionality
 02/10/17 tselvam  Implementing version dependent PHY init sequence writes
 10/07/16 tselvam  Change in the Phy init sequence by enabling pipe clock after
                   PLL locked in
 08/10/16 tselvam  Added PHY power down sequence and returned EFI_SUCCESS from
                   L1ss init to prevent PciHostBridgeDxe image from unloading,
                   so that the ACPI variable can be updated during callback
 07/15/16 tselvam  Formatted strings to %a in debug prints & Updated the global
                   with updated info if Link training fails for a root port
 07/08/16 tselvam  code refactoring
 06/08/16 tselvam  Validated PCIe library on MSM8998
 06/08/16 tselvam  Performed PCIe PHY init, moved all the GPIO inits right
                   before link training and enabled L1 for EP only if it is
                   supported for the End point
 02/08/16 tselvam  Added pcie xml config support
 11/12/15 tselvam  KW reivew #6754 & #6755
 09/03/15 tselvam  MSM8998 Support Included
 08/07/15 tselvam  Updated with new PHY seq for V3.0 h/w
 07/01/15 tselvam  Fixed the 7e bug check in HLOS by reverting the Limit region value
                   to just cover until the first 4KB of EP config space.
 06/22/15 tselvam  Fixed the compile error while 32-bit uefi.mbn build.
 06/15/15 tselvam  Used clock reset API and also updated unified code.
                   Implemented PCIe capability walk for L1ss enabling.
 05/13/15 tselvam  Unified code for multi platforms
 03/18/15 tselvam  Fixed Copyrights header
 03/09/15 tselvam  reduced some delays to improve boot time and include warning
                   messages and fixed running pcieTest more than once by adding
                   gcc bcr reset sequences.
 03/05/15 tselvam  written class code register value in PCIe_Setup_L1ss fn.
 01/26/15 tselvam  Updated as per 8994 changes
 12/15/14 tselvam  Changes for 8996 for all three root ports
 10/07/14 ahalabi  Changes for 8996
 08/04/14 nk       Changed MemLabel
 06/16/14 hozefak  Add PcieInit protocol
 05/20/14 hozefak  Edit to bring in changes for 8994 PHY
 05/07/14 hozefak  Turn on PCie during exitbootservices
 04/18/14 hozefak  First checkin for 8994
=============================================================================*/

#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/BaseLib.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFINpa.h>
#include <Library/PcieConfigLib.h>

#include "PcieLink.h"
#include "PcieBsp.h"

/***************************************************************************//**
 * @fn PcieConfigLibTurnOnExternalClock
 * @brief Turn ON external clock before polling for PHY status
 *
 * This function will enable the external clock
 *
 * @param[in] external clock string name embedded in pcieClkDesc_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibTurnOnExternalClock(
    CONST pcieClkDesc_t * ClockDesc)
{
  EFI_CLOCK_PROTOCOL    * pClockProtocol;
  EFI_STATUS              Status = EFI_SUCCESS;

  /* Locate the clock protocol */
  Status = gBS->LocateProtocol(&gEfiClockProtocolGuid, NULL,
      (VOID **)&pClockProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Clock LocateProtocol failed\r\n", __func__));
    return Status;
  }

  /*
   * Make sure external clock is enabled in GCC ctrl register
   * before we even poll for PHYSTATUS bit in the next step
   */
  Status = PcieConfigLibEnableClocks (pClockProtocol, ClockDesc->external);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Enable failed\r\n", __func__));
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibRegWriteConditional
 * @brief Write PHY init sequences
 *
 * This function will into PHY init sequences provided
 *
 * @param[in] phy_seq - array of phy init sequence
 * @param[in] mem - memory base addresses of corresponding regisers in phy_seq
 *
 * @return None
 ******************************************************************************/
STATIC inline VOID PcieConfigLibRegWriteConditional(
    CONST pciePhyRegSeq_t * phy_seq,
    CONST memRegions_t * mem)
{
  UINT32 Index = 0;
  UINT64 Addr;
  UINT32 Operation;;
  UINT32 Offset;
  UINT32 Mask;
  UINT32 Value;
  UINT32 Temp;

  if (mem == NULL || phy_seq == NULL)
    return;

  /* Assign the value of operation once before the while loop */
  Operation = phy_seq[Index].opflag;

  /*
   * This while loop will work on the Operation flag from
   * the PHY init sequence array and perform one of the following:
   *
   * o Delay of specified micro seconds
   * o Read Modify write of a specified register with mask and value
   * o Write into the specified register
   */
  while(Operation != PHY_REG_NOP)
  {
    Addr = mem[phy_seq[Index].base].pa;
    Offset = phy_seq[Index].offset;
    Value = phy_seq[Index].val;

    switch (Operation)
    {
    case PHY_REG_DELAY:
      gBS->Stall(Value);
    break;
    case PHY_REG_RMW:
      Mask = phy_seq[Index].mask; 
      Temp = MmioRead32(Addr + Offset);
      Temp = Temp & ~Mask;
      Temp = Temp | Value;
      MmioWrite32(Addr + Offset, Temp);
    break;
    case PHY_REG_WRI:
      MmioWrite32(Addr + Offset, Value);
    break;
    case PHY_REG_NOP:
    default:
    break;
    }

    Index++;
    Operation = phy_seq[Index].opflag;
  }
}

/***************************************************************************//**
 * @fn PcieConfigLibWriteRegisterSequence
 * @brief Write into sequence of registers
 *
 * This function will write into sequence of registers from different
 * register regions to bring the PHY out of reset
 *
 * @param[in] Root port index in portInfo_t struct
 * @param[in] Complete phy init sequence in pciePhyAllVerSeq_t pointer
 * @param[in] Clock names in pcieClkDesc_t pointer
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibWriteRegisterSequence(
    UINT32 RootPortIndex,
    CONST pciePhyAllVerSeq_t * Phy,
    CONST pcieClkDesc_t * ClockDesc)
{
  UINT32                  version_index = 0;
  UINT32                  reg_index = 0;
  rootPort_t            * pcie_root_port_info;
  CONST memRegions_t    * mem;
  EFI_CHIPINFO_PROTOCOL * pChipInfoProtocol;
  EFIChipInfoVersionType  chipInfoVersion;
  CONST pciePhyRegSeq_t * phy_seq;
  EFI_STATUS              Status = EFI_SUCCESS;

  /*
   * Check if the PHY sequence is NULL
   */
  if(Phy == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Complete PHY init sequence is NULL\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  /*
   * Return Success if there is no entry to write into
   */
  if (Phy[version_index].version == Pcie_Qcom_SoC_Ver_STOP)
  {
    return EFI_SUCCESS;
  }

  /*
   * Get the Root port info and Check if it is NULL
   */
  pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  /*
   * Check if the provided RootPortIndex is valid and available
   */
  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: RootPortIndex out of range\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  /*
   * Get the memory regions base address
   */
  mem = pcie_root_port_info->port_info[RootPortIndex].mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem regiosn is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid,NULL,(VOID **) &pChipInfoProtocol);
  if (EFI_ERROR (Status))
  {    
    DEBUG(( EFI_D_WARN, "%a: ChipInfo Locate Failed\r\n", __func__));
    return Status ;
  }    

  Status = pChipInfoProtocol->GetChipVersion(pChipInfoProtocol, &chipInfoVersion);
  if (EFI_ERROR (Status))
  {    
    DEBUG(( EFI_D_WARN, "%a: Failed to get version info\r\n", __func__));
    return Status ;
  }

  while (Phy[version_index].version != Pcie_Qcom_SoC_Ver_STOP)
  {
    phy_seq = Phy[version_index].phy_seq;
    switch(Phy[version_index].version)
    {
      default:
        /*
         * If the version numbering in the PHY init sequence is
         * less than or equal to the actual chipset version
         * number read from the h/w, program the sequence.
         *
         * Version independent sequences are tied to a version
         * number "0x00000000" in the PHY init sequences to denote
         * it is applicable for all versions of the SoC
         */
        if (Phy[version_index].version <= chipInfoVersion)
        {
          PcieConfigLibRegWriteConditional(phy_seq, mem);
        }
        break;
      case Pcie_Qcom_SoC_Ver_POLL:
        /*
         * Polling sequence is empty, better dont call PollBitMask
         */
        if (phy_seq[reg_index].base == invalid_reg)
        {
          break;
        }

        /*
         * Once the PHY PLL is enabled make sure that the
         * external pipe clock is being enabled via gcc clock
         * device driver 
         */
        if (ClockDesc != NULL && ClockDesc->external != NULL)
        {
          PcieConfigLibTurnOnExternalClock(ClockDesc);
        }

        /*
         * Check for the bit condition to be TRUE or FALSE
         */
        if( !PcieConfigLibPollBitMsk(mem[phy_seq[reg_index].base].pa + phy_seq[reg_index].offset,
              phy_seq[reg_index].mask,
              (BOOLEAN)phy_seq[reg_index].val,
              PCIE_PHY_UP_TIMEOUT_COUNT_MAX_15_MILLI_SEC))
        {
          DEBUG(( EFI_D_WARN, "%a: PHY not ready\r\n", __func__));
          return EFI_NOT_READY;
        }

        break;
    }

    version_index++;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibPollBitMsk
 * @brief Poll for the status until the Condition is satisifed
 *
 * Polls the "Addr" for bit specified in "Mask" with an interval of "Delay" for
 * each iteration until the "Condition" specified for that bit is met
 *
 * @param[in] Root port index in portInfo_t struct
 * @param[in] UINT64 Addr - Address to be read register from 
 * @param[in] UINT32 Mask - Bit Mask to be used for checking the condition
 * @param[in] BOOLEAN Condition - TRUE/FALSE condition to be met for bit mask
 * @param[in] UINT32 Delay - Delay in micro seconds between each iteration
 *
 * @return TRUE if the condition is met else FALSE
 ******************************************************************************/
BOOLEAN PcieConfigLibPollBitMsk (
    UINT64 Addr,
    UINT32 Mask,
    BOOLEAN Condition,
    UINT32 Delay)
{ 
  INT32 CountVariable = INIT_TIMEOUT;
  volatile UINT32 StatusBit;
  UINT32 Shift = __builtin_ctz((Mask));

  /*
   * Read once before the condition check, as there is a posibilty
   * that the first read of the status register will have the
   * condition met and we dont have to stall for even one cycle
   */
  StatusBit = MmioRead32(Addr);
  StatusBit = (StatusBit & (Mask)) >> Shift;

  while ( (StatusBit != (Condition)) && (CountVariable >= 0))
  {
    gBS->Stall(Delay);
    StatusBit = MmioRead32(Addr);
    StatusBit = (StatusBit & (Mask)) >> Shift;
    CountVariable--;
  }
  
  /*
   * Return "TRUE" if CountVariable is "Non-Zero":
   *  This denotes CountVariable did not exhausted and we could able to poll
   *  for the satisfaction condition before the time out
   *
   * Return "FALSE" if CountVariable is "Zero":
   *  This denotes CountVariable did exhausted and we could not able to poll
   *  for the satisfaction condition before the time out
   */
  return ((CountVariable>=0)?TRUE:FALSE);
}

/***************************************************************************//**
 * @fn PcieConfigLibInitializePHY
 * @brief Bring PHY out of reset
 *
 * This function will bring the PCIe PHY out of reset
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibInitializePHY(UINT32 RootPortIndex)
{
  CONST pcieBsp_t * pcie_bsp_info = PcieConfigLibGetPcieBspInfo();
  rootPort_t      * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  portInfo_t      * port_info;
  CONST pciePhyAllVerSeq_t * phy_seq;
  EFI_STATUS Status = EFI_SUCCESS;

  if (pcie_bsp_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: pcie_bsp_info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: RootPortIndex out of range\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  /* Call Common PHY init only for Root port 0 */
  if (RootPortIndex == 0)
  {
    /*
     * Get init sequence for common PHY
     */
    phy_seq = pcie_bsp_info->phy_init_seq;
    if(phy_seq != NULL)
    {
      Status = PcieConfigLibWriteRegisterSequence(RootPortIndex,
                                                  phy_seq,
                                                  pcie_bsp_info->phy_clk);
      if EFI_ERROR (Status)
      {
        DEBUG(( EFI_D_WARN, "%a: Common PHY init Failed \r\n", __func__));
        return Status ;
      }
    }
  }

  /*
   * Get the port info struct base for corresponding root port index
   */
  port_info = &pcie_root_port_info->port_info[RootPortIndex];

  /*
   * Get init sequence for port PHY
   */
  phy_seq = port_info->port_phy_init_seq;
  Status = PcieConfigLibWriteRegisterSequence(RootPortIndex,
                                              phy_seq,
                                              port_info->clk);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Port PHY init Failed \r\n", __func__));
    return Status ;
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibDeInitializePHY
 * @brief Power down PHY
 *
 * This function will power down port PHY
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibDeInitializePHY(UINT32 RootPortIndex)
{
  rootPort_t            * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  portInfo_t            * port;

  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: RootPortIndex out of range\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  port = &pcie_root_port_info->port_info[RootPortIndex];

  PcieConfigLibRegWriteConditional(port->port_phy_deinit_seq, port->mem);

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibTrainLink
 * @brief Perform Link Training
 *
 * This function will perform LTSSM_EN and do link training with connected
 * endpoint for the root port specified. This function will also update the
 * rootPort_t with available number of root ports.
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibTrainLink (UINT32 RootPortIndex)
{
  UINT32                  index = 0;
  UINT32                  temp;
  rootPort_t            * pcie_root_port_info;
  CONST memRegions_t    * mem;
  CONST gpioPinConfig_t * gpio;
  portInfo_t            * port;
  EFI_STATUS              Status = EFI_SUCCESS;

  pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: RootPortIndex out of range\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  port = &pcie_root_port_info->port_info[RootPortIndex]; 
  if (port == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  /*
   * Get the memory regions base address
   */
  mem = port->mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem regiosn is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  /*
   * Bring the End-points out of reset
   * and configure the clk_req gpio
   */
  gpio = port->gpio_cfg;
  while(gpio[index].type != NONE)
  {
    Status = PcieConfigLibConfigureGPIOs(&gpio[index]);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: Configure GPIO failed@%d\r\n",
              __func__, index));
      return Status;
    }
    index++;
  }

  /*
   * Link Training:
   * Write and spin into the LTSSM_EN bit to make 
   * sure it is written into the register.
   */
  do
  {
    MmioOr32((HWIO_OFFS(PCIE_0_PCIE20_PARF_LTSSM) + mem[parf_reg].pa),
             HWIO_PCIE_0_PCIE20_PARF_LTSSM_LTSSM_EN_BMSK);
    temp = MmioRead32((HWIO_OFFS(PCIE_0_PCIE20_PARF_LTSSM) + mem[parf_reg].pa));
  } while (!(temp & HWIO_PCIE_0_PCIE20_PARF_LTSSM_LTSSM_EN_BMSK));

  if( PcieConfigLibPollBitMsk(
             (HWIO_OFFS(PCIE_0_PCIE20_ELBI_SYS_STTS) + mem[elbi_reg].pa),
             HWIO_PCIE_0_PCIE20_ELBI_SYS_STTS_XMLH_LINK_UP_BMSK,
             TRUE,
             PCIE_LINK_UP_TIMEOUT_COUNT_MAX_150_MILLI_SEC))
  {
    port->link->trained = TRUE;
  }
  else
  {
    port->link->trained = FALSE;

    /*
     * Turn OFF the port PHY and clocks
     */
    Status = PcieConfigLibDeInitializePHY(RootPortIndex);
    if (EFI_ERROR(Status))
    {
      DEBUG(( EFI_D_WARN, "%a: Disable Port Clock failed\r\n", __func__));
      return Status;
    }

    Status = PcieConfigLibTurnOffPortClocks(RootPortIndex);
    if (EFI_ERROR(Status))
    {
      DEBUG(( EFI_D_WARN, "%a: Disable Port Clock failed\r\n", __func__));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn LocateCapabilityRegBlock
 * @brief Locate capcbility register block
 *
 * Locate capability register block per capability ID.
 *
 * @param[in]  CapID     , 16-bit PCIe extended capability ID
 * @param[in]  RegBase   , configuration register base of the device
 * @param[out] CapOffset , offset found based on CapID
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS
LocateCapabilityRegBlock (IN UINT16 CapID,
                          IN UINTN RegBase,
                          OUT UINT8 * CapOffset)
{
  UINT8   CapabilityPtr;
  UINT32  CapabilityEntry;
  UINT8   CapabilityID;

  if(CapOffset == NULL)
  {
    DEBUG(( EFI_D_WARN, "LocateCapabilityRegBlock: CapOffset is NULL\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  CapabilityPtr = (UINT8)MmioRead32 (RegBase + PCI_CAPBILITY_POINTER_OFFSET);

  while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)){
    CapabilityEntry = MmioRead32 (RegBase + CapabilityPtr);

    CapabilityID = (UINT8) CapabilityEntry;

    if (CapabilityID == CapID) {
      *CapOffset = CapabilityPtr;
      return EFI_SUCCESS;
    }

    CapabilityPtr = (UINT8) (CapabilityEntry >> 8);
  }

  return EFI_NOT_FOUND;
}

/***************************************************************************//**
 * @fn LocatePciExpressCapabilityRegBlock
 * @brief Locate Express capcbility register block
 *
 * Find the capability pointer offset for the specified
 * capability by walking through the capability list.
 *
 * @param[in]  CapID     , 16-bit PCIe extended capability ID
 * @param[in]  RegBase   , configuration register base of the device
 * @param[out] CapOffset , offset found based on CapID
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS LocatePciExpressCapabilityRegBlock (IN UINT16 CapID,
                                                      IN UINTN RegBase,
                                                      OUT UINT16 * CapOffset)
{
  UINT16 CapabilityPtr;
  UINT32 CapabilityEntry;
  UINT16 CapabilityID;

  CapabilityPtr = EFI_PCIE_CAPABILITY_BASE_OFFSET;

  if(CapOffset == NULL)
  {
    DEBUG(( EFI_D_WARN, "LocatePciExpressCapabilityRegBlock: CapOffset is NULL\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  do
  {
    /*
     * Mask it to DWORD alignment as per PCI spec
     */
    CapabilityPtr &= 0xFFC;
    CapabilityEntry = MmioRead32 (RegBase + CapabilityPtr);
    if (CapabilityEntry == 0xFFFFFFFF)
    {
      DEBUG(( EFI_D_WARN, "%a: Cap Reg not accessible\r\n", __func__));
      return EFI_NOT_FOUND;
    }
    CapabilityID = (UINT16) CapabilityEntry;

    if (CapabilityID == CapID)
    {
      *CapOffset = CapabilityPtr;
      return EFI_SUCCESS;
    }

    CapabilityPtr = (CapabilityEntry & HWIO_PCIE_0_L1SUB_CAP_HEADER_REG_NEXT_OFFSET_BMSK)
                                     >> HWIO_PCIE_0_L1SUB_CAP_HEADER_REG_NEXT_OFFSET_SHFT;
  }while(CapabilityPtr != 0);

  return EFI_NOT_FOUND;
}

/***************************************************************************//**
 * @fn PcieConfigLibSetupL1ss
 * @brief Configure L1ss
 *
 * This function will configure link parameters for L1ss
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibSetupL1ss(UINT32 RootPortIndex)
{
  rootPort_t         * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  CONST memRegions_t * mem;
  portInfo_t         * port;
  UINT32              TempReg;
  UINT16              L1SubCtrlBase = 0;
  UINT8               PcieCapBaseRp = 0;
  UINT8               PcieCapBaseEp = 0;
  UINTN               EndPointConfigAddr;
  EFI_STATUS          Status = EFI_SUCCESS;
  
  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: No RootPorts Left to configure\r\n", __func__));
    return EFI_SUCCESS;
  }

  if (pcie_root_port_info->port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }
  
  port = &pcie_root_port_info->port_info[RootPortIndex];
  
  /*
   * Get the memory regions base address
   */
  mem = port->mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem regiosn is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (port->link->trained)
  {
    Status = LocatePciExpressCapabilityRegBlock(EFI_PCIE_L1SS_PM_CAP_ID,
                                                mem[dbi_reg].pa,
                                                &L1SubCtrlBase);
    if EFI_ERROR (Status)
    {
      /*
       * DEBUG(( EFI_D_INFO, "%a: CapOffset not found for %d root complex\r\n",
       *        __func__, RootPortIndex));
       */
      goto L1ssSetupFunctionEnd;
    }

    /* L1_2_PCIPM_EN | L1_1_PCIPM_EN | L1_2_ASPM_EN | L1_1_ASPM_EN */
    TempReg = 0x0000000F;
    MmioOr32(mem[dbi_reg].pa + L1SubCtrlBase + control1_reg_offset, TempReg);

    Status = LocateCapabilityRegBlock(EFI_PCI_PCIE_CAP_ID,
                                      mem[dbi_reg].pa,
                                      &PcieCapBaseRp);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: PCIe CapOffset not found for %d root complex\r\n", 
              __func__, RootPortIndex));
      goto L1ssSetupFunctionEnd;
    }

    /* Get the endpoint base address from the iATU viewport registers */
    MmioWrite32((HWIO_OFFS(PCIE_0_IATU_VIEWPORT_REG) + mem[dbi_reg].pa), 1);
    EndPointConfigAddr = MmioRead32(
      (HWIO_OFFS(PCIE_0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0) + mem[dbi_reg].pa));

    Status = LocateCapabilityRegBlock(EFI_PCI_PCIE_CAP_ID,
                                                  EndPointConfigAddr,
                                                  &PcieCapBaseEp);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: PCIe CapOffset not found for EP of %d root complex\r\n", 
              __func__, RootPortIndex));
      goto L1ssSetupFunctionEnd;
    }

    /*
     * Perform Common Clock Configuration and Link Retrain
     * before enabling ASPM
     */

    /* Enable Common Clock Configuration for Root port */
    MmioOr16(mem[dbi_reg].pa + PcieCapBaseRp + link_control_offset,
      HWIO_PCIE_0_LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_COMMON_CLK_CONFIG_BMSK);
    /* Enable Common Clock Configuration for End Point */
    MmioOr16(EndPointConfigAddr + PcieCapBaseEp + link_control_offset,
      HWIO_PCIE_0_LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_COMMON_CLK_CONFIG_BMSK);
    /* Re-Train the link */
    MmioOr16(mem[dbi_reg].pa + PcieCapBaseRp + link_control_offset,
      HWIO_PCIE_0_LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_RETRAIN_LINK_BMSK);
    /*
     * Wait for link to retrain
     */
    PcieConfigLibPollBitMsk(
      mem[dbi_reg].pa + PcieCapBaseRp + link_control_offset,
      HWIO_PCIE_0_LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_TRAINING_BMSK,
      FALSE,
      100);

    /* PCIE_1_TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1.PCIE_CAP_LTR_EN Enable */
    TempReg =  HWIO_PCIE_0_TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_LTR_EN_BMSK;
    MmioOr32(mem[dbi_reg].pa + PcieCapBaseRp + device_control2_offset, TempReg);

    /* Enable DBI_RO_WR_EN to access CS1 region */
    MmioWrite32 ((HWIO_OFFS(PCIE_0_MISC_CONTROL_1_REG) + mem[dbi_reg].pa),
      HWIO_PCIE_0_MISC_CONTROL_1_REG_DBI_RO_WR_EN_BMSK);
    MemoryFence();

    /* Optionality Compliance */
    TempReg = HWIO_PCIE_0_LINK_CAPABILITIES_REG_PCIE_CAP_ASPM_OPT_COMPLIANCE_BMSK;
    /* clear AS_LINK_PM_SUPT */
    TempReg = TempReg & (~(UINT32)(HWIO_PCIE_0_LINK_CAPABILITIES_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_BMSK));
    /* Disable L0s & Enable L1 */
    TempReg = TempReg | (ASPM_L0s_DS_L1_EN << HWIO_PCIE_0_LINK_CAPABILITIES_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_SHFT);
    MmioOr32 (mem[dbi_reg].pa + PcieCapBaseRp + link_capabilities_offset, TempReg);

    /* Disable DBI_RO_WR_EN bit to access CS1 region */
    MmioWrite32 ((HWIO_OFFS(PCIE_0_MISC_CONTROL_1_REG) + mem[dbi_reg].pa),
                 !(HWIO_PCIE_0_MISC_CONTROL_1_REG_DBI_RO_WR_EN_BMSK));
    MemoryFence();

    /* Read the link capability register of End point */
    TempReg = MmioRead32(EndPointConfigAddr + PcieCapBaseEp + link_capabilities_offset);
    /* Maskout everything except L1 ASPM support bit */
    TempReg &= (ASPM_L0s_DS_L1_EN << HWIO_PCIE_0_LINK_CAPABILITIES_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_SHFT);
    /* Configure L1.1 and L1.2 for End point only if L1 is reported in cap register */
    if(TempReg)
    {
      Status = LocatePciExpressCapabilityRegBlock(EFI_PCIE_L1SS_PM_CAP_ID,
                                                  EndPointConfigAddr,
                                                  &L1SubCtrlBase);
      if EFI_ERROR (Status)
      {
    	/*
         * DEBUG(( EFI_D_INFO, "%a: L1ss CapOffset not found for EP of %d root complex\r\n",
         *      __func__, RootPortIndex));
         */
        goto L1ssSetupFunctionEnd;
      }

      /* L1_2_PCIPM_EN | L1_1_PCIPM_EN | L1_2_ASPM_EN | L1_1_ASPM_EN */
      TempReg = 0x0000000F;
      MmioOr32(EndPointConfigAddr + L1SubCtrlBase + control1_reg_offset, TempReg);

      /* PCIE_1_TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1.PCIE_CAP_LTR_EN Enable */
      TempReg =  HWIO_PCIE_0_TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_LTR_EN_BMSK;
      MmioOr32(EndPointConfigAddr + PcieCapBaseEp + device_control2_offset, TempReg);
    }    
  }

L1ssSetupFunctionEnd:
  return EFI_SUCCESS;
}
