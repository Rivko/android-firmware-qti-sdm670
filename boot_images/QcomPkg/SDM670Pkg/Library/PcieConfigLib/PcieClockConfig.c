/** 
 @file PcieClockConfig.c

  Handles all the Pcie related clocks and power domains.

  Copyright (c) 2016-2017, Qualcomm Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   -------  ----------------------------------------------------------
 07/26/17   mk       Added support for IoMMU driver to enable IO Coherency
 05/22/17   tselvam  Removed SMMU workaround from PCIe library code
 02/10/17   tselvam  Enabled IO coherency based on a flag from devcfg
 01/12/17   tselvam  Turning Off AGNOC clocks to unblock XO shutdown in HLOS
 12/07/16   tselvam  Programmed SMMU SMR and S2CR register using MASK instead
                     of programming all the registers with SID
 10/07/16   tselvam  Handled Reset, Enable of external and gcc controlled 
                     clocks seperately
 08/10/16   tselvam  Disabled clock power domain along with port clocks
                     during de initialization
 07/28/16   tselvam  Added SMMU configuration
 07/15/16   tselvam  Formatted strings to %a in debug prints
 06/27/16   tselvam  SpunOff from PciLink.c to create PcieClockConfig.c
=============================================================================*/

#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/BaseLib.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFINpa.h>
#include <Library/PcieConfigLib.h>
#include <HALbootHWIO.h>
#include <Library/PcdLib.h>
#include <Protocol/EFIHALIOMMUProtocol.h>

/*NPA headers*/
#include "npa.h"
#include "pmapp_npa.h"
/* PCIe lib haeders */
#include "PcieLink.h"
#include "PcieBsp.h"

/* handle for the npa client. */
STATIC npa_client_handle PcieReqClient = NULL;

/***************************************************************************//**
 * @fn PcieConfigLibDisableClocks
 * @brief Disable Clocks
 *
 * This function will disable clocks from the clocks list
 * 
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] A const char * string of clock power domain name
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibDisableClocks (
        EFI_CLOCK_PROTOCOL    *pClockProtocol,
        CONST char ** ClockList)
{
  UINTN                ClockId;
  UINT32               ClockIndex = 0;
  BOOLEAN              ClockEnabled = FALSE;
  EFI_STATUS           Status = EFI_SUCCESS;
  
  if (pClockProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Protocol is NULL %a\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  if (ClockList == NULL)
  {
    /*
     * DEBUG(( EFI_D_INFO, "%a: No Action Required!!!\r\n", __func__));
     */
    return EFI_SUCCESS;
  }
  
  while (ClockList[ClockIndex] != NULL)
  {
    Status = pClockProtocol->GetClockID(pClockProtocol, ClockList[ClockIndex], &ClockId);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: GetClockID failed %a\r\n",
              __func__, ClockList[ClockIndex]));
      return Status;
    }
    
    Status = pClockProtocol->IsClockEnabled(pClockProtocol, ClockId, &ClockEnabled);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: IsClockEnabled failed %a\r\n",
              __func__, ClockList[ClockIndex]));
      return Status;
    }
    
    if (ClockEnabled == TRUE)
    {
      Status = pClockProtocol->DisableClock(pClockProtocol, ClockId);
      if EFI_ERROR (Status)
      {
        DEBUG(( EFI_D_WARN, "%a: DisableClock failed %a\r\n",
                __func__, ClockList[ClockIndex]));
        return Status;
      }    	
    }

    ClockIndex++;  
  }
  
  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibVoteIcbClkBW
 * @brief vore for ICB bandwidth
 *
 * This is a self aware function, based on the parameter pcieIcbClkBW_t type's,
 * npa_hdl member, will decide whether to create / destroy an npa client handle.
 *
 * @param[in] pointer to pcieIcbClkBW_t
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibVoteIcbClkBW (
        pcieIcbClkBW_t * icb_bw)
{
  if (icb_bw == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: icb_bw is NULL\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  /* If client doesn't exist, create one */
  if (icb_bw->npa_hdl == NULL)
  {

    icb_bw->npa_hdl = npa_create_sync_client_ex( "/icb/arbiter",
                                                 icb_bw->npa_client_name,
                                                 NPA_CLIENT_VECTOR,
                                                 icb_bw->ms_size,
                                                 (void *)icb_bw->ms_list );

    if (icb_bw->npa_hdl == NULL)
    {
      DEBUG(( EFI_D_WARN, "%a: NULL Client for%a\r\n", __func__,
	    icb_bw->npa_client_name));
      return EFI_LOAD_ERROR;
    }

    npa_issue_vector_request( icb_bw->npa_hdl, 
                              icb_bw->req_size/sizeof(npa_resource_state), 
                              (npa_resource_state *)icb_bw->req_list );
  }
  /* There is a client, destroy it if someone called us */
  else
  {
    npa_complete_request(icb_bw->npa_hdl);
    npa_destroy_client(icb_bw->npa_hdl);
    icb_bw->npa_hdl = NULL;
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibDisableClockPower
 * @brief Disble the clock power domain
 *
 * Disable clock power domain
 *
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] A const char * string of clock power domain name
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibDisableClockPower (
        EFI_CLOCK_PROTOCOL    *pClockProtocol,
        CONST char * PowerDomain)
{
  UINTN      uClockPowerDomainId;
  EFI_STATUS Status;

  if (pClockProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Protocol is NULL %a\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Status = pClockProtocol->GetClockPowerDomainID(pClockProtocol,
                                                 PowerDomain,
                                                 &uClockPowerDomainId);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Get Failed %a\r\n", __func__, PowerDomain));
    return Status;
  }
  Status =  pClockProtocol->DisableClockPowerDomain(pClockProtocol,
                                                   uClockPowerDomainId);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Disable Failed for %a\r\n", __func__, PowerDomain));
    return Status;
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibTurnOffPortClocks
 * @brief Turn Off PCIe port Clocks
 *
 * This function will turn off all the port clocks
 *
 * @param[in] Root port index in portInfo_t struct to disable the clocks 
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibTurnOffPortClocks (UINT32 RootPortIndex)
{
  EFI_CLOCK_PROTOCOL * pClockProtocol;
  rootPort_t         * PcieRootPortInfo = PcieConfigLibGetPciRootPortInfo();
  portInfo_t         * PortInfo;
  CONST char        ** ClockList;
  EFI_STATUS           Status;
  
  if (PcieRootPortInfo->port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Library not initialized\r\n", __func__));
    return RETURN_LOAD_ERROR;
  }

  if (RootPortIndex >= PcieRootPortInfo->num_rp)
  {
      DEBUG(( EFI_D_WARN, "%a: RootPortIndex is out of range\r\n", __func__));
      return EFI_INVALID_PARAMETER;
  }

  PortInfo = &PcieRootPortInfo->port_info[RootPortIndex];
  
  if(PortInfo->clk == NULL)
  {
    /*
     * If there are no clocks to disable
     * return EFI_SUCCESS
     */
    return EFI_SUCCESS;
  }
  
  Status = gBS->LocateProtocol(&gEfiClockProtocolGuid,
                               NULL,
                               (VOID **)&pClockProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Clock LocateProtocol failed\r\n", __func__));
    return Status;
  }
  
  /* Disable gcc controlled port clocks */
  ClockList = PortInfo->clk->gcc;
  Status = PcieConfigLibDisableClocks (pClockProtocol, ClockList);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: gcc port clocks failed\r\n", __func__));
    return Status;
  }

  /* Disable external port clocks */
  ClockList = PortInfo->clk->external;
  Status = PcieConfigLibDisableClocks (pClockProtocol, ClockList);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: external port clocks failed\r\n", __func__));
    return Status;
  }

  /* Destroy Client handle for ICB bandwidth vote */
  Status = PcieConfigLibVoteIcbClkBW(PortInfo->icb_bw);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: ICB vote failed\r\n", __func__));
    return Status;
  }

  /* Disable Clock power domain */
  Status = PcieConfigLibDisableClockPower (pClockProtocol, PortInfo->pwr);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: port power domain failed\r\n", __func__));
    return Status;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibTurnOffCommonClocks
 * @brief Turn Off PCIe Common Clocks
 *
 * This function will turn off all the common clocks if any
 *
 * @param[in] void 
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibTurnOffCommonClocks (VOID)
{
  EFI_CLOCK_PROTOCOL * pClockProtocol;
  CONST pcieBsp_t    * PcieBspInfo = PcieConfigLibGetPcieBspInfo();
  CONST char        ** ClockList;
  EFI_STATUS           Status = EFI_SUCCESS;

  if(PcieBspInfo == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Invalid BSP Info pointer\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  if (PcieBspInfo->phy_clk == NULL)
  {
    /*
     * If there are no clocks to disable
     * return EFI_SUCCESS
     */
    return EFI_SUCCESS;
  }
  
  Status = gBS->LocateProtocol(&gEfiClockProtocolGuid,
                               NULL,
                               (VOID **)&pClockProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Clock LocateProtocol failed\r\n", __func__));
    return Status;
  }
 
  /* Disable common gcc controlled clocks */
  ClockList = PcieBspInfo->phy_clk->gcc;
  Status = PcieConfigLibDisableClocks (pClockProtocol, ClockList);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Disable gcc clocks failed\r\n", __func__));
    return Status;
  }

  /* Disable external common clocks */
  ClockList = PcieBspInfo->phy_clk->external;
  Status = PcieConfigLibDisableClocks (pClockProtocol, ClockList);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Disable external clocks failed\r\n", __func__));
    return Status;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibSetAuxClockFreq
 * @brief Set Aux Clock Freq
 *
 * Try and set the correct aux clock frequency as requested
 *
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] Pointer to auxClkSettings_t type
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibSetAuxClockFreq (
        EFI_CLOCK_PROTOCOL    *pClockProtocol,
        CONST auxClkSettings_t * aux_clk)
{
  UINTN      ClockId;
  UINT32     nFreqHz;
  EFI_STATUS Status = EFI_SUCCESS;

  if (pClockProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Protocol is NULL \r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Status = pClockProtocol->GetClockID(pClockProtocol,
                                      aux_clk->name,
                                      &ClockId);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Get Clock ID Failed\r\n", __func__));
    return Status;
  }

  //Try exact frequency match
  Status = pClockProtocol->SetClockFreqHz(pClockProtocol, ClockId,
                                         aux_clk->freq,
                                         EFI_CLOCK_FREQUENCY_HZ_CLOSEST,
                                         &nFreqHz);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: SetClockFreqHz failed\r\n", __func__));
    return Status;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibEnableClockPower
 * @brief Enable the clock power domain
 *
 * Enables clock power domain
 *
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] A const char * string of clock power domain name
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibEnableClockPower (
        EFI_CLOCK_PROTOCOL    *pClockProtocol,
        CONST char * PowerDomain)
{
  UINTN      uClockPowerDomainId;
  EFI_STATUS Status = EFI_SUCCESS;

  if (pClockProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Protocol is NULL\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Status = pClockProtocol->GetClockPowerDomainID(pClockProtocol,
                                                 PowerDomain,
                                                 &uClockPowerDomainId);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Get Failed %a\r\n", __func__, PowerDomain));
    return Status;
  }

  Status =  pClockProtocol->EnableClockPowerDomain(pClockProtocol,
                                                   uClockPowerDomainId);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Enable Failed for %a\r\n", __func__, PowerDomain));
    return Status;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibResetClocks
 * @brief Reset the clocks
 *
 * Resets the list of clocks provided
 *
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] An array of const char * string list of clock names to reset
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibResetClocks (
        EFI_CLOCK_PROTOCOL    *pClockProtocol,
        CONST char ** ResetClockList)
{
  UINT32     ClockIndex = 0;
  UINTN      ClockId;
  EFI_STATUS Status = EFI_SUCCESS;

  if (pClockProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Protocol is NULL \r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  /*
   * check if ResetClockList is NULL
   * if so, there is no clock needed to be enabled
   */
  if (ResetClockList == NULL)
  {
    /*
     * DEBUG(( EFI_D_INFO, "%a: No Action Required!!!\r\n", __func__));
     */
    return EFI_SUCCESS;
  }

  while (ResetClockList[ClockIndex] != NULL)
  {
    Status = pClockProtocol->GetClockID(pClockProtocol, ResetClockList[ClockIndex], &ClockId);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: GetClockID failed %a\r\n",
	    __func__, ResetClockList[ClockIndex]));
      return Status;
    }

    Status = pClockProtocol->ResetClock(pClockProtocol, ClockId, EFI_CLOCK_RESET_ASSERT);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: ResetClock assert failed %a\r\n",
	    __func__, ResetClockList[ClockIndex]));
      return Status;
    }

    gBS->Stall(500);

    Status = pClockProtocol->ResetClock(pClockProtocol, ClockId, EFI_CLOCK_RESET_DEASSERT);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: ResetClock deassert failed %a\r\n",
	    __func__, ResetClockList[ClockIndex]));
      return Status;
    }

    ClockIndex++;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibEnableClocks
 * @brief Enable the clocks
 *
 * Resets and Enabled clocks provided in clock descriptor
 *
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] An array of const char * string list of clock names to Enable
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibEnableClocks (
        EFI_CLOCK_PROTOCOL  * pClockProtocol,
        CONST char ** EnableClockList)
{
  UINTN      ClockId;
  UINT32     ClockIndex = 0;
  BOOLEAN    clockEnabled = FALSE;
  EFI_STATUS Status = EFI_SUCCESS;

  if (pClockProtocol == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: Clock Protocol is NULL %a\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  /*
   * check if EnableClockList is NULL
   * if so, there is no clock needed to be enabled
   */
  if (EnableClockList == NULL)
  {
    /*
     * DEBUG(( EFI_D_INFO, "%a: No Action Required!!!\r\n", __func__));
     */
    return EFI_SUCCESS;
  }

  while (EnableClockList[ClockIndex] != NULL)
  {
    Status = pClockProtocol->GetClockID(pClockProtocol, EnableClockList[ClockIndex], &ClockId);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: GetClockID failed %a\r\n",
              __func__, EnableClockList[ClockIndex]));
      return Status;
    }

    Status = pClockProtocol->IsClockEnabled(pClockProtocol, ClockId, &clockEnabled);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: IsClockEnabled failed %a\r\n",
              __func__, EnableClockList[ClockIndex]));
      return Status;
    }

    /*
     * If this clock was not enabled previously
     */
    if (clockEnabled == FALSE)
    {
      Status = pClockProtocol->EnableClock(pClockProtocol, ClockId);
      if EFI_ERROR (Status)
      {
        DEBUG(( EFI_D_WARN, "%a: EnableClock failed %a\r\n",
                __func__, EnableClockList[ClockIndex]));
        return Status;
      }
    }

    ClockIndex++;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibConfigureSMMU
 * @brief Configure the SMMU
 *
 * This function performs the following operations:
 *  o-> Enabled clock power domain for SMMU
 *  o-> Configure SMRn, S2CRn, CBAR0, and PARF BDF to SID registers
 *
 * @param[in] UINT32 RootPortIndex, root port index to be configured
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibConfigureSMMU(UINT32 RootPortIndex)
{
  UINT32               BDFIndex;
  UINT16               BDF;
  rootPort_t         * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  portInfo_t         * port = &pcie_root_port_info->port_info[RootPortIndex];
  EFI_HAL_IOMMU_PROTOCOL * pHALIOMMUProtocol;
  HAL_IOMMU_fptable_t * fp_table = NULL;
  VOID                * p_pcie_domain_hdl;
  EFI_STATUS           Status = EFI_SUCCESS;

  /*
   * Write PARF BDF to SID registers
   */
  BDFIndex = 0;
  do
  {
    BDF = (((UINT16)BDFIndex) << 8);
    /* OFFSET to GEN3 and GEN2 controller are same for this register,
     * so using the value for GEN2.
     *
     * TODO: Find a solution to not touch this register with macro directly
     */
    MmioWrite32(port->mem[parf_reg].pa + HWIO_PCIE_0_PCIE20_PARF_BDF_TRANSLATE_n_OFFS(BDFIndex), BDF);
    BDFIndex = BDFIndex + 1;
  }while(BDFIndex <= 0xf);

  /* Locate UEFI IOMMU protocol */
  Status = gBS->LocateProtocol(&gEfiHalIommuProtocolGuid,
                               NULL,
                               (VOID **)&pHALIOMMUProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: IOMMU LocateProtocol failed\r\n", __func__));
    return Status;
  }

  /* Get the function pointer handle for IOMMU config */
  pHALIOMMUProtocol->_HAL_IOMMU_protocol_init(&fp_table);

  /* Create Domain for PCIe */
  if(HAL_IOMMU_ERR_OK != fp_table->domain_create_func(&p_pcie_domain_hdl)) {
    DEBUG(( EFI_D_WARN, "%a: IOMMU domain create failed\r\n", __func__));
    return EFI_NOT_STARTED;
  }

  /* Configure the rootport for Stage one bypass with CCA enabled for IOCoherency */
  if(HAL_IOMMU_ERR_OK != fp_table->config_bypass_domain_func(p_pcie_domain_hdl, HAL_IOMMU_AARCH64_CCA_DOMAIN_TYPE)) {
    DEBUG(( EFI_D_WARN, "%a: IOMMU domain configure failed\r\n", __func__));
    return EFI_UNSUPPORTED;
  }

  /* Attach the device to the domain */
  if (HAL_IOMMU_ERR_OK != fp_table->domain_attach_func(p_pcie_domain_hdl,
                                                       (void *)port->iommu_hdl, 0x0, 0x0))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibConfigureClocks
 * @brief Configure the PCIe clocks
 *
 * This function performs the following operations:
 *  o-> Enabled clock power domain
 *  o-> Resets and Enables all the clocks
 *  o-> sets the Aux clock frequency
 *
 * @param[in] void
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibConfigureClocks (UINT32 RootPortIndex)
{
  EFI_CLOCK_PROTOCOL  * pClockProtocol = NULL;
  CONST pcieBsp_t     * pcie_bsp_info = PcieConfigLibGetPcieBspInfo();
  rootPort_t          * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  portInfo_t          * port = &pcie_root_port_info->port_info[RootPortIndex];
  CONST pcieClkDesc_t * clk;
  EFI_STATUS            Status = EFI_SUCCESS;

  Status = gBS->LocateProtocol(&gEfiClockProtocolGuid,
                               NULL,
                               (VOID **)&pClockProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG(( EFI_D_WARN, "%a: Clock LocateProtocol failed\r\n", __func__));
    return Status;
  }

  /*
   * Turn ON clock power domain
   */
  Status = PcieConfigLibEnableClockPower(pClockProtocol, port->pwr);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Enable power Domain failed\r\n", __func__));
    return Status;
  }

  /*
   * Vote for ICB bandwidth
   */
  Status = PcieConfigLibVoteIcbClkBW(port->icb_bw);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: ICB vote failed\r\n", __func__));
    return Status;
  }

  clk = pcie_bsp_info->phy_clk;
  if (clk != NULL)
  {
    /*
     * Reset common PHY clock if any
     */
    Status = PcieConfigLibResetClocks(pClockProtocol, clk->reset);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: Reset common clock failed \r\n", __func__));
      return Status;
    }

    /*
     * Turn ON common PHY clock if any
     */
    Status = PcieConfigLibEnableClocks(pClockProtocol, clk->gcc);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: Enable PHY clocks failed\r\n", __func__));
      return Status;
    }
  }

  clk = port->clk;
  if (clk != NULL)
  {
    /*
     * Reset port PHY clock if any
     */
    Status = PcieConfigLibResetClocks(pClockProtocol, clk->reset);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: Reset port clock failed \r\n", __func__));
      return Status;
    }

    /*
     * Turn ON port PHY clocks 
     */
    Status = PcieConfigLibEnableClocks(pClockProtocol, clk->gcc);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: Enable port clocks failed\r\n", __func__));
      return Status;
    }
  }

  /*
   * Configure the AUX clock to requested frequency 
   */
  Status = PcieConfigLibSetAuxClockFreq(pClockProtocol, port->aux_clk);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Aux Clock Set failed\r\n", __func__));
    return Status;
  }

  /*
   * Configure the SMMU clocks and BDF/SID entries
   */
  Status = PcieConfigLibConfigureSMMU(RootPortIndex);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: Configure SMMU Failed\r\n", __func__));
    return Status;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibInitializeLDO
 * @brief Configure the PCIe LDOs
 *
 * Vote for the LDOs necessary for PCIe to be operational
 *
 * @param[in] void
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibInitializeLDO(VOID)
{
  if (PcieReqClient == NULL)
  {
    PcieReqClient = npa_create_sync_client(PMIC_NPA_GROUP_ID_PCIE, 
                                           "pcie",
                                           NPA_CLIENT_REQUIRED);
    if (PcieReqClient == NULL)
    {
      DEBUG(( EFI_D_WARN, "%a: pcie npa_create_sync_client returned NULL\r\n",
              __func__));
      return EFI_DEVICE_ERROR;
    }
  
    /* Sends Activate to the driver */
    npa_issue_required_request(PcieReqClient, 
                               PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL);
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibDeInitializeLDO
 * @brief Disable the PCIe LDOs
 *
 * Vote for the LDOs to be turned OFF
 *
 * @param[in] void
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibDeInitializeLDO(VOID)
{
  if (PcieReqClient != NULL)
  {
    npa_issue_required_request(PcieReqClient, PMIC_NPA_MODE_ID_PCIE_OFF);
    npa_complete_request(PcieReqClient);
    npa_destroy_client(PcieReqClient);
    PcieReqClient = NULL;
  }

  return EFI_SUCCESS;
}
