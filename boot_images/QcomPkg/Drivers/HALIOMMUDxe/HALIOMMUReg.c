/*=========================================================================

                       S M M U  H A L  L I B R A R Y

FILE:      HALHAL_IOMMU.c
   System HAL_IOMMU Hardware Abstraction Layer (HAL)

GENERAL DESCRIPTION
  This file contains hardware abstraction layer routines for managing HAL_IOMMU instances
  for multimedia 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
Copyright (c) 2010 - 2015
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUReg.c#1 $
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $
$Change: 14402555 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/06/12   spa     Initial Version
==========================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "HALIOMMUReg.h"
#include "HALIOMMUImplDef.h"
#include "HALIOMMUTypes.h"

/**
* HAL_IOMMU API Handle definition
*/

uint32 HAL_IOMMU_IN(uint32 hal_iommu_base, uint32 reg_space_base, uint32 offset)
{
	return (*((volatile uint32 *)((uint64)(hal_iommu_base + reg_space_base + offset))));
}

void
HAL_IOMMU_OUT(uint32 hal_iommu_base, uint32 reg_space_base, uint32 offset, uint32 value)
{
	*((uint32 *)((uint64)(hal_iommu_base + reg_space_base + offset))) = value;
}

uint64 HAL_IOMMU_IN64(uint32 hal_iommu_base, uint32 reg_space_base, uint32 offset)
{
	return (*((volatile uint64 *)((uint64)(hal_iommu_base + reg_space_base + offset))));
}

void
HAL_IOMMU_OUT64(uint32 hal_iommu_base, uint32 reg_space_base, uint32 offset, uint64 value)
{
	*((uint64 *)((uint64)(hal_iommu_base + reg_space_base + offset))) = value;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNMRR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *nmrr)
{
   *nmrr = HAL_IOMMU_IN(baseVirtAddr,
             (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
             HAL_IOMMU_CBN_NMRR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetPRRR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *prrr)
{
   *prrr = HAL_IOMMU_IN(baseVirtAddr,
             (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
             HAL_IOMMU_CBN_PRRR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetMAIR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 mair0)
{
  HAL_IOMMU_OUT(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
               HAL_IOMMU_CBN_MAIR0_OFFSET, mair0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetMAIR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 mair1)
{
  HAL_IOMMU_OUT(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
               HAL_IOMMU_CBN_MAIR1_OFFSET, mair1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetMAIR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *mair0)
{
   *mair0 = HAL_IOMMU_IN(baseVirtAddr,
                        (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                        HAL_IOMMU_CBN_MAIR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetMAIR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *mair1)
{
   *mair1 = HAL_IOMMU_IN(baseVirtAddr,
                        (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                        HAL_IOMMU_CBN_MAIR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetSCTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_SCTLR_t *sctlr)
{
   sctlr->raw = HAL_IOMMU_IN(baseVirtAddr,
                  (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                  HAL_IOMMU_CBN_SCTLR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSCTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_SCTLR_t sctlr)
{
   HAL_IOMMU_OUT(baseVirtAddr,
                (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_SCTLR_OFFSET, sctlr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetCBARn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBARn_t *cbar)
{
   cbar->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION1_OFFSET + HAL_IOMMU_CBAR0_OFFSET, (index * 4));
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetCBARn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBARn_t cbar)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION1_OFFSET + HAL_IOMMU_CBAR0_OFFSET, (index * 4), cbar.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetCBFRSYNRAn(uint32 baseVirtAddr, uint32 index, uint32 *cbfrsynra)
{
   *cbfrsynra = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION1_OFFSET + HAL_IOMMU_CBFRSYNRA0_OFFSET, (index * 4));
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetCBFRSYNRAn(uint32 baseVirtAddr, uint32 index, uint32 cbfrsynra)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION1_OFFSET + HAL_IOMMU_CBFRSYNRA0_OFFSET, (index * 4), cbfrsynra);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetCBA2Rn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBA2Rn_t *cba2r)
{
   cba2r->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION1_OFFSET + HAL_IOMMU_CBA2R0_OFFSET, (index * 4));
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetCBA2Rn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBA2Rn_t cba2r)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION1_OFFSET + HAL_IOMMU_CBA2R0_OFFSET, (index * 4), cba2r.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_S2CRn_t *s2cr)
{
   s2cr->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_S2CR0_OFFSET, (index * 4));
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_S2CRn_t s2cr)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_S2CR0_OFFSET, (index * 4), s2cr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetAS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_AS2CRn_t *as2cr)
{
   as2cr->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION2_OFFSET, HAL_IOMMU_AS2CR0_OFFSET + (index * 4));
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetAS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_AS2CRn_t as2cr)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION2_OFFSET, HAL_IOMMU_AS2CR0_OFFSET + (index * 4), as2cr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSMRn(uint32 baseVirtAddr, uint32 index, uint32 id)
{
   HAL_IOMMU_SMRn_t smr;

   smr.formatted.smr_type.type_exid0.ID = id & 0x7FFF;
   smr.formatted.smr_type.type_exid0.MASK = 0;
   smr.formatted.smr_type.type_exid0.VALID = 1;
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_SMR0_OFFSET, (index * 4), smr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSMRn_raw(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_SMRn_t smr)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_SMR0_OFFSET, (index * 4), smr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSMRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_SMRn_t *smr)
{
	smr->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_SMR0_OFFSET, (index * 4));
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureIDR0(uint32 baseVirtAddr, HAL_IOMMU_Secure_IDR0_t *SecureIdr0)
{
   SecureIdr0->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_IDR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureIDR1(uint32 baseVirtAddr, HAL_IOMMU_Secure_IDR1_t *SecureIdr1)
{
   SecureIdr1->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_IDR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t *secureCR0)
{
   secureCR0->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_CR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t secureCR0)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_CR0_OFFSET, secureCR0.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetNonSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t *nonSecureCR0)
{
   nonSecureCR0->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSCR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetNonSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t nonSecureCR0)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSCR0_OFFSET, nonSecureCR0.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t *secureACR)
{
   secureACR->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_ACR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t secureACR)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_ACR_OFFSET, secureACR.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetNonSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t *nonSecureACR)
{
   nonSecureACR->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSACR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetNonSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t nonSecureACR)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSACR_OFFSET, nonSecureACR.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t *secureCR2)
{
   secureCR2->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_CR2_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t secureCR2)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_CR2_OFFSET, secureCR2.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetNonSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t *nonSecureCR2)
{
   nonSecureCR2->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSCR2_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetNonSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t nonSecureCR2)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSCR2_OFFSET, nonSecureCR2.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureCR1(uint32 baseVirtAddr, HAL_IOMMU_SCR1_t *secureCR1)
{
   secureCR1->raw    =  HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_SCR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureCR1(uint32 baseVirtAddr, HAL_IOMMU_SCR1_t secureCR1)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_SCR1_OFFSET, secureCR1.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFAR(uint32 baseVirtAddr, uint64 *gfAddr)
{
   *gfAddr = HAL_IOMMU_IN64(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFAR0(uint32 baseVirtAddr, uint32 *gfAddr0)
{
   *gfAddr0 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFAR1(uint32 baseVirtAddr, uint32 *gfAddr1)
{
   *gfAddr1 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFAR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFAR(uint32 baseVirtAddr, uint64 gfAddr)
{
  HAL_IOMMU_OUT64(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFAR0_OFFSET, gfAddr);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFAR0(uint32 baseVirtAddr, uint32 gfAddr0)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFAR0_OFFSET, gfAddr0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFAR1(uint32 baseVirtAddr, uint32 gfAddr1)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFAR1_OFFSET, gfAddr1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFAR(uint32 baseVirtAddr, uint64 *nsgfAddr)
{
   *nsgfAddr = HAL_IOMMU_IN64(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFAR0(uint32 baseVirtAddr, uint32 *nsgfAddr0)
{
   *nsgfAddr0 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFAR1(uint32 baseVirtAddr, uint32 *nsgfAddr1)
{
   *nsgfAddr1 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFAR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFAR(uint32 baseVirtAddr, uint64 nsgfAddr)
{
  HAL_IOMMU_OUT64(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFAR0_OFFSET, nsgfAddr);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFAR0(uint32 baseVirtAddr, uint32 nsgfAddr0)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFAR0_OFFSET, nsgfAddr0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFAR1(uint32 baseVirtAddr, uint32 nsgfAddr1)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFAR1_OFFSET, nsgfAddr1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFSR(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t *gfsr)
{
   gfsr->raw = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSR(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t *nsgfsr)
{
   nsgfsr->raw = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFSRRestore(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t gfsr)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSRRESTORE_OFFSET, gfsr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSRRestore(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t nsgfsr)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSRRESTORE_OFFSET, nsgfsr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFSYNR0(uint32 baseVirtAddr, uint32 *gfsynr0)
{
   *gfsynr0 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSYNR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFSYNR1(uint32 baseVirtAddr, uint32 *gfsynr1)
{
   *gfsynr1 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSYNR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetGFSYNR2(uint32 baseVirtAddr, uint32 *gfsynr2)
{
   *gfsynr2 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSYNR2_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFSYNR0(uint32 baseVirtAddr, uint32 gfsynr0)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSYNR0_OFFSET, gfsynr0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFSYNR1(uint32 baseVirtAddr, uint32 gfsynr1)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSYNR1_OFFSET, gfsynr1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetGFSYNR2(uint32 baseVirtAddr, uint32 gfsynr2)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_GFSYNR2_OFFSET, gfsynr2);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSYNR0(uint32 baseVirtAddr, uint32 *nsgfsynr0)
{
   *nsgfsynr0 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSYNR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSYNR1(uint32 baseVirtAddr, uint32 *nsgfsynr1)
{
   *nsgfsynr1 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSYNR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSYNR2(uint32 baseVirtAddr, uint32 *nsgfsynr2)
{
   *nsgfsynr2 = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSYNR2_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSYNR0(uint32 baseVirtAddr, uint32 nsgfsynr0)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSYNR0_OFFSET, nsgfsynr0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSYNR1(uint32 baseVirtAddr, uint32 nsgfsynr1)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSYNR1_OFFSET, nsgfsynr1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSYNR2(uint32 baseVirtAddr, uint32 nsgfsynr2)
{
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_NSGFSYNR2_OFFSET, nsgfsynr2);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetSecureIndex(uint32 baseVirtAddr, uint32 index)
{
	uint32 ssdRegVal;
	uint32 ssdRegBit;
	uint32 ssdRegNum;
	uint32 ssdRegMask;
	ssdRegNum = index / 32;
	ssdRegBit = index % 32;
	ssdRegMask = ~((uint32)1 << ssdRegBit);
	ssdRegVal = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION4_OFFSET, HAL_IOMMU_SSDR0_OFFSET + (ssdRegNum*4));
	ssdRegVal = ssdRegVal & ssdRegMask;
	HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION4_OFFSET, HAL_IOMMU_SSDR0_OFFSET + (ssdRegNum*4), ssdRegVal);
	return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_ClearSecureIndex(uint32 baseVirtAddr, uint32 index)
{
	uint32 ssdRegVal;
	uint32 ssdRegBit;
	uint32 ssdRegNum;
	uint32 ssdRegMask;
	ssdRegNum = index / 32;
	ssdRegBit = index % 32;
	ssdRegMask = (1 << ssdRegBit);
	ssdRegVal = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION4_OFFSET, HAL_IOMMU_SSDR0_OFFSET + (ssdRegNum*4));
	ssdRegVal = ssdRegVal | ssdRegMask;
	HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION4_OFFSET, HAL_IOMMU_SSDR0_OFFSET + (ssdRegNum*4), ssdRegVal);
	return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GlobalInvalidateSecureTLBAll(uint32 baseVirtAddr) 
{
	volatile uint32 dummy = 0;
	HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_STLBIALL_OFFSET, dummy);

	HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBGSYNC_OFFSET, dummy);

	do {
		dummy = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBGSTATUS_OFFSET);
	} while (dummy);

	return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t HAL_IOMMU_GlobalInvalidateNonSecureTLBVMID(uint32 baseVirtAddr, uint32 uVMID)
{
  volatile uint32 dummy = 0;

  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBIVMID_OFFSET, uVMID);
  HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBGSYNC_OFFSET, dummy);
  do
  {
    dummy = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBGSTATUS_OFFSET);
  } while (dummy);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GlobalInvalidateNonSecureNonHypTLBAll(uint32 baseVirtAddr)
{
	volatile uint32 dummy = 0;
	HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBIALLNSNH_OFFSET, dummy);

	HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBGSYNC_OFFSET, dummy);

	do {
		dummy = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION0_OFFSET, HAL_IOMMU_TLBGSTATUS_OFFSET);
	} while (dummy);

	return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBAll(uint32 baseVirtAddr, uint32 ctxBankNumber) 
{
	volatile uint32 dummy = 0;
	HAL_IOMMU_OUT(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBIALL_OFFSET, dummy);

	HAL_IOMMU_OUT(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBSYNC_OFFSET, dummy);

	do {
		dummy = HAL_IOMMU_IN(baseVirtAddr,
      (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
      HAL_IOMMU_CBN_TLBSTATUS_OFFSET);
	} while (dummy);

	return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBVA(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 vaddr)
{
	volatile uint32 dummy = 0;
	HAL_IOMMU_OUT64(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBIVA_OFFSET, vaddr);

	HAL_IOMMU_OUT(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBSYNC_OFFSET, dummy);

	do {
		dummy = HAL_IOMMU_IN(baseVirtAddr,
      (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
      HAL_IOMMU_CBN_TLBSTATUS_OFFSET);
	} while (dummy);

	return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBIPAS2(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ipaAddr)
{
  volatile uint32 dummy = 0;
  HAL_IOMMU_OUT64(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBIIPAS2_OFFSET, ipaAddr);

  HAL_IOMMU_OUT(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBSYNC_OFFSET, dummy);

  do {
    dummy = HAL_IOMMU_IN(baseVirtAddr,
              (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
              HAL_IOMMU_CBN_TLBSTATUS_OFFSET);
  } while (dummy);

  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBSync(uint32 baseVirtAddr, uint32 ctxBankNumber)
{
  volatile uint32 dummy = 0;

  HAL_IOMMU_OUT(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_TLBSYNC_OFFSET, dummy);

  do {
    dummy = HAL_IOMMU_IN(baseVirtAddr,
              (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
              HAL_IOMMU_CBN_TLBSTATUS_OFFSET);
  } while (dummy);

  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetResumeStalledTransaction(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 value)
{
  HAL_IOMMU_OUT(baseVirtAddr,
    (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
    HAL_IOMMU_CBN_RESUME_OFFSET, value);

  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetFSR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_FSR_t *fsr)
{
   fsr->raw = HAL_IOMMU_IN(baseVirtAddr,
                (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_FSR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetFSRRestore(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_FSR_t fsr)
{
   HAL_IOMMU_OUT(baseVirtAddr,
                (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_FSR_RESTORE_OFFSET, fsr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *far0)
{
   *far0 = HAL_IOMMU_IN64(baseVirtAddr,
            (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
            HAL_IOMMU_CBN_FAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetFAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *far0)
{
   *far0 = HAL_IOMMU_IN(baseVirtAddr,
             (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
             HAL_IOMMU_CBN_FAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetFAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *far1)
{
   *far1 = HAL_IOMMU_IN(baseVirtAddr,
             (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
             HAL_IOMMU_CBN_FAR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 far0)
{
  HAL_IOMMU_OUT64(baseVirtAddr,
                 (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                  HAL_IOMMU_CBN_FAR0_OFFSET, far0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetFAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 far0)
{
  HAL_IOMMU_OUT(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_FAR0_OFFSET, far0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetFAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 far1)
{
  HAL_IOMMU_OUT(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_FAR1_OFFSET, far1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetFSYN0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *fsyn0)
{
   *fsyn0 = HAL_IOMMU_IN(baseVirtAddr,
              (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
              HAL_IOMMU_CBN_FSYN0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetFSYN1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *fsyn1)
{
   *fsyn1 = HAL_IOMMU_IN(baseVirtAddr,
              (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
              HAL_IOMMU_CBN_FSYN1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetFSYN0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 fsyn0)
{
  HAL_IOMMU_OUT(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_FSYN0_OFFSET, fsyn0);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetFSYN1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 fsyn1)
{
  HAL_IOMMU_OUT(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                HAL_IOMMU_CBN_FSYN1_OFFSET, fsyn1);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetIPAFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ipafar)
{
  *ipafar = HAL_IOMMU_IN64(baseVirtAddr,
               (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
               HAL_IOMMU_CBN_IPAFAR0_OFFSET);
  return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetPAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *par0)
{
   *par0 = HAL_IOMMU_IN(baseVirtAddr,
             (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
             HAL_IOMMU_CBN_PAR0_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetPAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *par1)
{
   *par1 = HAL_IOMMU_IN(baseVirtAddr,
             (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
             HAL_IOMMU_CBN_PAR1_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}



HAL_IOMMUResult_t   HAL_IOMMU_SetAsid(uint32 baseVirtAddr, uint32 ctxBankNumber, uint8 asid)
{
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_CONTEXTIDR_OFFSET, asid);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTTBR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ttAddr)
{
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_TTBR0_0_OFFSET, (uint32)ttAddr);
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_TTBR0_1_OFFSET, (uint32)(ttAddr >> 32));

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetTTBR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ttAddr)
{
  // lower 32 bits
  *ttAddr = HAL_IOMMU_IN(baseVirtAddr,
                        (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                        HAL_IOMMU_CBN_TTBR0_0_OFFSET);

  // upper 32 bits
  *ttAddr |= ((uint64)HAL_IOMMU_IN(baseVirtAddr,
                      (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                      HAL_IOMMU_CBN_TTBR0_1_OFFSET) << 32);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTTBR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ttAddr)
{
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_TTBR1_0_OFFSET, (uint32)ttAddr);
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_TTBR1_1_OFFSET, (uint32)(ttAddr >> 32));

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetTTBR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ttAddr)
{
  // lower 32 bits
  *ttAddr = HAL_IOMMU_IN(baseVirtAddr,
                        (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                        HAL_IOMMU_CBN_TTBR1_0_OFFSET);

  // upper 32 bits
  *ttAddr |= ((uint64)HAL_IOMMU_IN(baseVirtAddr,
                      (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                      HAL_IOMMU_CBN_TTBR1_1_OFFSET) << 32);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetTTBCR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TTBCR_t *ttbcr)
{
   ttbcr->raw = HAL_IOMMU_IN(baseVirtAddr,
                  (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                  HAL_IOMMU_CBN_TTBCR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetTTBCR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TTBCR_t ttbcr)
{
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_TTBCR_OFFSET, ttbcr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_GetTCR2(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TCR2_t *tcr2)
{
   tcr2->raw = HAL_IOMMU_IN(baseVirtAddr,
                 (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                 HAL_IOMMU_CBN_TCR2_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTCR2(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TCR2_t tcr2)
{
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_TCR2_OFFSET, tcr2.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetACTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_ACTLR_t *actlr)
{
   actlr->raw = HAL_IOMMU_IN(baseVirtAddr,
                  (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
                  HAL_IOMMU_CBN_ACTLR_OFFSET);
   return HAL_IOMMU_ERR_HAL_SUCCESS;
}
HAL_IOMMUResult_t   HAL_IOMMU_SetACTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_ACTLR_t actlr)
{
   HAL_IOMMU_OUT(baseVirtAddr,
     (ctxBankNumber * HAL_IOMMU_CTX_BANK_SIZE) + HAL_IOMMU_CTX_BANK0_OFFSET(baseVirtAddr),
     HAL_IOMMU_CBN_ACTLR_OFFSET, actlr.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetHTWAReqCtlr(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_HTW_AREQ_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetHUMCfg(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_HUM_CFG_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetFilterCfg(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_FILTER_CFG_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTLBIFilterCfg(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_TLBI_FILTER_CFG_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1SPTLBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_SP_TLB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1SPTLBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_SP_TLB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BPTLBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BP_TLB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BPTLBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BP_TLB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BTLB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BTLB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1L0BFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_L0_BFB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1L0BFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_L0_BFB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1GBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_GB_BFB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1GBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_GB_BFB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_MB_BFB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_MB_BFB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_KB_BFB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_KB_BFB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC2GBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC2_GB_BFB_LOWER_POINTERS_0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC2GBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC2_GB_BFB_LOWER_POINTERSN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1SPTLBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_SP_TLB_SRAM_BASE_ADDR0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BPTLBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BP_TLB_SRAM_BASE_ADDR0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BTLB_SRAM_BASE_ADDR0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1GBBFBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_GB_BFB_SRAM_BASE_ADDR0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_MB_BFB_SRAM_BASE_ADDR0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_KB_BFB_SRAM_BASE_ADDR0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBWidth0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_BTLB_WIDTH0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBWidth0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_MB_BFB_WIDTH0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBWidth0(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_TC1_KB_BFB_WIDTH0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetSS2TBn(uint32 baseVirtAddr, uint32 index, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_SS2TBN_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNestedPrefetchEnable0(uint32 baseVirtAddr, uint64 value)
{
   HAL_IOMMU_OUT64(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                  HAL_IOMMU_IMPLDEF_NESTED_PREFETCH_ENABLE0_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNestedPrefetchEnablen(uint32 baseVirtAddr, uint32 index, uint64 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                  HAL_IOMMU_IMPLDEF_NESTED_PREFETCH_ENABLE0_OFFSET + (4 * index), value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetNestedPrefetchCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_NESTED_PREFETCH_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetSlotManagerDemandReqCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_SLOT_MANAGER_DEMAND_REQ_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetSlotManagerNestedReqCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_SLOT_MANAGER_NESTED_REQ_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUClkCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_REG_HAL_IOMMU_CLK_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetTLBISCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_TLBIS_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}


HAL_IOMMUResult_t   HAL_IOMMU_SetTHBSctrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_THB_SCTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetDebugTHBCtrl(uint32 baseVirtAddr, uint32 *value)
{
   *value = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_CTRL_OFFSET);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStartFilterCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_START_FILTER_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBLogFilterCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_LOG_FILTER_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBLogFilterVmid(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_LOG_FILTER_VMID_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStopFilterCtrl(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_STOP_FILTER_CTRL_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStopFilterState(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_STOP_FILTER_STATE_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBWCmd(uint32 baseVirtAddr, uint32 value)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_DEBUG_THB_WCMD_OFFSET, value);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetRegDynClkCtrl(uint32 baseVirtAddr, HAL_IOMMU_REG_DYNCLK_CTRL_t dynClkCtrl)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_REG_DYNCLK_CTRL_OFFSET, dynClkCtrl.raw);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetRegDynClkCtrl(uint32 baseVirtAddr, HAL_IOMMU_REG_DYNCLK_CTRL_t *dynClkCtrl)
{
   dynClkCtrl->raw = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                                 HAL_IOMMU_IMPLDEF_REG_DYNCLK_CTRL_OFFSET);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_SetMicroMMUCtrl(uint32 baseVirtAddr, HAL_IOMMU_MICRO_MMU_CTRL_t uMMUCtrl)
{
   HAL_IOMMU_OUT(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                HAL_IOMMU_IMPLDEF_MICRO_MMU_CTRL_OFFSET, uMMUCtrl.raw);


   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

HAL_IOMMUResult_t   HAL_IOMMU_GetMicroMMUCtrl(uint32 baseVirtAddr, HAL_IOMMU_MICRO_MMU_CTRL_t *uMMUCtrl)
{
   uMMUCtrl->raw = HAL_IOMMU_IN(baseVirtAddr, HAL_IOMMU_GLOBAL_REGION5_OFFSET,
                               HAL_IOMMU_IMPLDEF_MICRO_MMU_CTRL_OFFSET);

   return HAL_IOMMU_ERR_HAL_SUCCESS;
}

