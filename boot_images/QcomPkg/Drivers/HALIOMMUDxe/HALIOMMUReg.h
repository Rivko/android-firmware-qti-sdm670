#ifndef __HAL_IOMMU_REG_H__
#define __HAL_IOMMU_REG_H__
/*=============================================================================

                       S M M U  H A L  L I B R A R Y

FILE:      HALIOMMUReg.h
   System MMU Hardware Abstraction Layer (HAL)

GENERAL DESCRIPTION
  This file contains data structures needed internall by the HAL IOMMU library

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2010 - 2015
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUReg.h#1 $
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $
$Change: 14402555 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/10   kcr     Initial Version
=============================================================================*/

/*-----------------------------------------------------------------------------
 * Include Files
 *---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
#include "HALIOMMUTypes.h"
#include "HALIOMMUImplDef.h"

/*-----------------------------------------------------------------------------
 * Definitions
 *---------------------------------------------------------------------------*/
 
#define HAL_IOMMU_GLOBAL_REGION0_OFFSET 0x0     // global 0
#define HAL_IOMMU_GLOBAL_REGION1_OFFSET 0x1000  // global 1
#define HAL_IOMMU_GLOBAL_REGION2_OFFSET 0x2000  // impl def 0
#define HAL_IOMMU_GLOBAL_REGION3_OFFSET 0x3000  // perf monitor
#define HAL_IOMMU_GLOBAL_REGION4_OFFSET 0x4000  // ssd
#define HAL_IOMMU_GLOBAL_REGION5_OFFSET 0x6000  // impl def 1

#define HAL_IOMMU_CTX_BANK_SIZE         0x00001000

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 0 
 *---------------------------------------------------------------------------*/
#define HAL_IOMMU_CR0_OFFSET           0x00000000
#define HAL_IOMMU_SCR1_OFFSET          0x00000004
#define HAL_IOMMU_CR2_OFFSET           0x00000008
#define HAL_IOMMU_ACR_OFFSET           0x00000010
#define HAL_IOMMU_IDR0_OFFSET          0x00000020
#define HAL_IOMMU_IDR1_OFFSET          0x00000024
#define HAL_IOMMU_IDR2_OFFSET          0x00000028
#define HAL_IOMMU_IDR7_OFFSET          0x0000003C
#define HAL_IOMMU_GFAR0_OFFSET         0x00000040
#define HAL_IOMMU_GFAR1_OFFSET         0x00000044
#define HAL_IOMMU_GFSR_OFFSET          0x00000048
#define HAL_IOMMU_GFSRRESTORE_OFFSET   0x0000004C
#define HAL_IOMMU_GFSYNR0_OFFSET       0x00000050
#define HAL_IOMMU_GFSYNR1_OFFSET       0x00000054
#define HAL_IOMMU_GFSYNR2_OFFSET       0x00000058
#define HAL_IOMMU_STLBIALL_OFFSET      0x00000060
#define HAL_IOMMU_TLBIVMID_OFFSET      0x00000064
#define HAL_IOMMU_TLBIALLNSNH_OFFSET   0x00000068
#define HAL_IOMMU_TLBGSYNC_OFFSET      0x00000070
#define HAL_IOMMU_TLBGSTATUS_OFFSET    0x00000074
#define HAL_IOMMU_TLBIVAH0_OFFSET      0x00000078
#define HAL_IOMMU_TLBIVAH1_OFFSET      0x0000007C
#define HAL_IOMMU_TRANSBUF_READSW_OFFSET 0x00000080
#define HAL_IOMMU_TRANSBUF_READSWSTATUS_OFFSET 0x00000084
#define HAL_IOMMU_TRANSBUF_TR0_OFFSET  0x00000088
#define HAL_IOMMU_TRANSBUF_TR1_OFFSET  0x0000008C
#define HAL_IOMMU_TRANSBUF_TR2_OFFSET  0x00000090
#define HAL_IOMMU_TRANSBUF_DR0_OFFSET  0x00000094
#define HAL_IOMMU_TRANSBUF_DR1_OFFSET  0x00000098
#define HAL_IOMMU_TRANSBUF_DR2_OFFSET  0x0000009C
#define HAL_IOMMU_TLBIVALH64_0_OFFSET  0x000000B0
#define HAL_IOMMU_TLBIVALH64_1_OFFSET  0x000000B4
#define HAL_IOMMU_TLBIVMIDS1_OFFSET    0x000000B8
#define HAL_IOMMU_TLBIVAH64_0_OFFSET   0x000000C0
#define HAL_IOMMU_TLBIVAH64_1_OFFSET   0x000000C4
#define HAL_IOMMU_GATS1UR0_OFFSET      0x00000100
#define HAL_IOMMU_GATS1PW1_OFFSET      0x0000011C
#define HAL_IOMMU_GATS12UR0_OFFSET     0x00000120
#define HAL_IOMMU_GATS12UR1_OFFSET     0x00000124
#define HAL_IOMMU_GATS12UW0_OFFSET     0x00000128
#define HAL_IOMMU_GATS12UW1_OFFSET     0x0000012C
#define HAL_IOMMU_GATS12PR0_OFFSET     0x00000130
#define HAL_IOMMU_GATS12PR1_OFFSET     0x00000134
#define HAL_IOMMU_GATS12PW0_OFFSET     0x00000138
#define HAL_IOMMU_GATS12PW1_OFFSET     0x0000013C
#define HAL_IOMMU_GPAR0_OFFSET         0x00000180
#define HAL_IOMMU_GPAR1_OFFSET         0x00000184
#define HAL_IOMMU_GATSR_OFFSET         0x00000188
#define HAL_IOMMU_NSCR0_OFFSET         0x00000400
#define HAL_IOMMU_NSCR2_OFFSET         0x00000408
#define HAL_IOMMU_NSACR_OFFSET         0x00000410
#define HAL_IOMMU_NSGFAR0_OFFSET       0x00000440
#define HAL_IOMMU_NSGFAR1_OFFSET       0x00000444
#define HAL_IOMMU_NSGFSR_OFFSET        0x00000448
#define HAL_IOMMU_NSGFSRRESTORE_OFFSET 0x0000044C
#define HAL_IOMMU_NSGFSYNR0_OFFSET     0x00000450
#define HAL_IOMMU_NSGFSYNR1_OFFSET     0x00000454
#define HAL_IOMMU_NSGFSYNR2_OFFSET     0x00000458
#define HAL_IOMMU_NSTLBGSYNC_OFFSET    0x00000470
#define HAL_IOMMU_NSTLBGSTATUS_OFFSET  0x00000474
#define HAL_IOMMU_NSGATSR_OFFSET       0x00000588
#define HAL_IOMMU_SMR0_OFFSET          0x00000800
#define HAL_IOMMU_SMR127_OFFSET        0x000009FC
#define HAL_IOMMU_S2CR0_OFFSET         0x00000C00
#define HAL_IOMMU_S2CR127_OFFSET       0x00000DFC
#define HAL_IOMMU_PIDR4                0x00000FD0
#define HAL_IOMMU_CIDR3                0x00000FFC

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 1 
 *---------------------------------------------------------------------------*/
#define HAL_IOMMU_CBAR0_OFFSET         0x00000000
#define HAL_IOMMU_CBAR127_OFFSET       0x000001FC
#define HAL_IOMMU_CBFRSYNRA0_OFFSET    0x00000400
#define HAL_IOMMU_CBFRSYNRA127_OFFSET  0x000005FC
#define HAL_IOMMU_CBA2R0_OFFSET        0x00000800
#define HAL_IOMMU_CBA2R127_OFFSET      0x000009FC

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 2 
 *---------------------------------------------------------------------------*/
#define HAL_IOMMU_AS2CR0_OFFSET        0x00000000
// TODO: probably need to add more defines for HLOS virtualization

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 3
 *---------------------------------------------------------------------------*/
// Performance Monitors
// TODO: probably need to add more defines for HLOS virtualization

/*-----------------------------------------------------------------------------
 * Register Offsets inside the global region 4
 *---------------------------------------------------------------------------*/
#define HAL_IOMMU_SSDR0_OFFSET         0x00000000

/*-----------------------------------------------------------------------------
 * Register Offsets inside the context bank regions 
 *---------------------------------------------------------------------------*/
#define HAL_IOMMU_CBN_SCTLR_OFFSET          0x00000000
#define HAL_IOMMU_CBN_ACTLR_OFFSET          0x00000004
#define HAL_IOMMU_CBN_RESUME_OFFSET         0x00000008
#define HAL_IOMMU_CBN_TCR2_OFFSET           0x00000010
#define HAL_IOMMU_CBN_TTBR0_0_OFFSET        0x00000020
#define HAL_IOMMU_CBN_TTBR0_1_OFFSET        0x00000024
#define HAL_IOMMU_CBN_TTBR1_0_OFFSET        0x00000028
#define HAL_IOMMU_CBN_TTBR1_1_OFFSET        0x0000002C
#define HAL_IOMMU_CBN_TTBCR_OFFSET          0x00000030
#define HAL_IOMMU_CBN_CONTEXTIDR_OFFSET     0x00000034
#define HAL_IOMMU_CBN_PRRR_OFFSET           0x00000038
#define HAL_IOMMU_CBN_NMRR_OFFSET           0x0000003C
#define HAL_IOMMU_CBN_MAIR0_OFFSET          0x00000038
#define HAL_IOMMU_CBN_MAIR1_OFFSET          0x0000003C
#define HAL_IOMMU_CBN_PAR0_OFFSET           0x00000050
#define HAL_IOMMU_CBN_PAR1_OFFSET           0x00000054
#define HAL_IOMMU_CBN_FSR_OFFSET            0x00000058
#define HAL_IOMMU_CBN_FSR_RESTORE_OFFSET    0x0000005C
#define HAL_IOMMU_CBN_FAR0_OFFSET           0x00000060
#define HAL_IOMMU_CBN_FAR1_OFFSET           0x00000064
#define HAL_IOMMU_CBN_FSYN0_OFFSET          0x00000068
#define HAL_IOMMU_CBN_FSYN1_OFFSET          0x0000006C
#define HAL_IOMMU_CBN_IPAFAR0_OFFSET        0x00000070
#define HAL_IOMMU_CBN_IPAFAR1_OFFSET        0x00000074
#define HAL_IOMMU_CBN_TLBIALL_OFFSET        0x00000618
#define HAL_IOMMU_CBN_TLBIVA_OFFSET         0x00000600
#define HAL_IOMMU_CBN_TLBIIPAS2_OFFSET      0x00000630
#define HAL_IOMMU_CBN_TLBSYNC_OFFSET        0x000007F0
#define HAL_IOMMU_CBN_TLBSTATUS_OFFSET      0x000007F4
#define HAL_IOMMU_CBN_LOCAL_HALT_CTRL_OFFSET      0x00000D08

/*-----------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------*/
typedef union HAL_IOMMU_Secure_IDR0_t
{
  struct {
    unsigned NUMSMRG:8;
    unsigned EXIDS:1;
    unsigned NUMSIDB:4;
    unsigned BTM:1;
    unsigned CTTW:1;
    unsigned reserved:1;
    unsigned NUMIRPT:8;
    unsigned PTFS:2;
    unsigned ATOSNS:1;
    unsigned SMS:1;
    unsigned NTS:1;
    unsigned S2TS:1;
    unsigned S1TS:1;
    unsigned SES:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_Secure_IDR0_t;

typedef union HAL_IOMMU_Secure_IDR1_t
{
  struct {
    unsigned NUMCB:8;
    unsigned NUMSSDNDXB:4;
    unsigned SSDTP:2;
    unsigned reserved:1;
    unsigned SMCD:1;
    unsigned NUMS2CB:8;
    unsigned reserved2:4;
    unsigned NUMPAGENDXB:3;
    unsigned PAGESIZE:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_Secure_IDR1_t;

typedef union HAL_IOMMU_CR0_t
{
  struct {
    unsigned CLIENTPD:1;
    unsigned GFRE:1;
    unsigned GFIE:1;
    unsigned EXIDENABLE:1;
    unsigned GCFGFRE:1;
    unsigned GCFGFIE:1;
    unsigned TRANSIENTCFG:2;
    unsigned STALLD:1;
    unsigned GSE:1;
    unsigned USFCFG:1;
    unsigned VMIDPNE:1;
    unsigned PTM:1;
    unsigned FB:1;
    unsigned BSU:2;
    unsigned MEMATTR:4;
    unsigned MTCFG:1;
    unsigned SMCFCFG:1;
    unsigned SHCFG:2;
    unsigned RACFG:2;
    unsigned WACFG:2;
    unsigned NSCFG:2;
    unsigned reserved2:2;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CR0_t;

typedef union HAL_IOMMU_CR2_t
{
  struct {
    unsigned BPVMID:8;
    unsigned reserved:24;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CR2_t;

typedef union HAL_IOMMU_ACR_t
{
  struct {
    unsigned REQPRIORITY:2;
    unsigned reserved1:2;
    unsigned REQPRIORITYCFG:1;
    unsigned G0SCRATCHS:4;
    unsigned G0SCRATCHNS:19;
    unsigned BPRCOSH:1;
    unsigned BPRCISH:1;
    unsigned BPRCNSH:1;
    unsigned reserved2:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_ACR_t;

typedef union HAL_IOMMU_SCR1_t
{
  struct {
    unsigned NSNUMCBO:8;
    unsigned NSNUMSMRGO:8;
    unsigned NSNUMIRPTO:8;
    unsigned GASRAE:1;
    unsigned GEFRO:1;
    unsigned SIF:1;
    unsigned SPMEN:1;
    unsigned reserved:4;
  } formatted;
  unsigned raw;
} HAL_IOMMU_SCR1_t;

typedef struct HAL_IOMMU_GFSR_t
{
  unsigned raw;
} HAL_IOMMU_GFSR_t;

typedef union HAL_IOMMU_CBn_SCTLR_t
{
  struct {
    union {
      struct {
        unsigned M:1;
        unsigned TRE:1;
        unsigned AFE:1;
        unsigned AFFD:1;
        unsigned E:1;
        unsigned CFRE:1;
        unsigned CFIE:1;
        unsigned CFCFG:1;
        unsigned HUPCF:1;
        unsigned WXN:1;
        unsigned UWXN:1;
        unsigned reserved:1;
        unsigned ASIDPNE:1;
        unsigned reserved2:1;
        unsigned TRANSIENTCFG:2;
        unsigned MEMATTR:4;
        unsigned MTCFG:1;
        unsigned reserved3:1;
        unsigned SHCFG:2;
        unsigned RACFG:2;
        unsigned WACFG:2;
        unsigned NSCFG:2;
        unsigned reserved4:2;
      } s1;
      struct {
        unsigned M:1;
        unsigned TRE:1;
        unsigned AFE:1;
        unsigned AFFD:1;
        unsigned E:1;
        unsigned CFRE:1;
        unsigned CFIE:1;
        unsigned CFCFG:1;
        unsigned HUPCF:1;
        unsigned reserved1:4;
        unsigned PTW:1;
        unsigned BSU:2;
        unsigned MEMATTR:4;
        unsigned reserved2:1;
        unsigned FB:1;
        unsigned SHCFG:2;
        unsigned RACFG:2;
        unsigned WACFG:2;
        unsigned reserved3:4;
      } s2;
    } translation_stage;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBn_SCTLR_t;

typedef union HAL_IOMMU_CBARn_t
{
  struct {
    union {
      // stage 1 context with stage 2 bypass
      struct {
        unsigned VMID:8;
        unsigned BPSHCFG:2;
        unsigned HYPC:1;
        unsigned FB:1;
        unsigned MEMATTR:4;
        unsigned TYPE:2;
        unsigned BSU:2;
        unsigned RACFG:2;
        unsigned WACFG:2;
        unsigned IRPTNDX:8;
      } type_s1;
      // stage 2 context
      struct {
        unsigned VMID:8;
        unsigned reserved1:8;
        unsigned TYPE:2;
        unsigned SBZ:2;
        unsigned reserved2:4;
        unsigned IRPTNDX:8;
      } type_s2;
      // stage 1 context with stage 2 context (nested)
      struct {
        unsigned VMID:8;
        unsigned CBNDX:8;
        unsigned TYPE:2;
        unsigned SBZ:2;
        unsigned reserved2:4;
        unsigned IRPTNDX:8;
      } type_s2_nested;
    } cbar_type;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBARn_t;

typedef union HAL_IOMMU_CBA2Rn_t
{
  struct {
    unsigned VA64:1;
    unsigned MONC:1;
    unsigned reserved:30;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBA2Rn_t;

typedef union HAL_IOMMU_S2CRn_t
{
  struct {
    unsigned FIELD:8;
    unsigned SHCFG:2;
    unsigned EXIDVALID:1;
    unsigned MTCFG:1;
    unsigned MEMATTR:4;
    unsigned TYPE:2;
    unsigned NSCFG:2;
    unsigned RACFG:2;
    unsigned WACFG:2;
    unsigned PRIVCFG:2;
    unsigned INSTCFG:2;
    unsigned TRANSIENTCFG:2;
    unsigned reserved2:2;
  } formatted;
  unsigned raw;
} HAL_IOMMU_S2CRn_t;

typedef union HAL_IOMMU_AS2CRn_t
{
  struct {
    unsigned REQPRIORITY:2;
    unsigned reserved:2;
    unsigned REQPRIORITYCFG:1;
    unsigned reserved1:23;
    unsigned BPRCOSH:1;
    unsigned BPRCISH:1;
    unsigned BPRCNSH:1;
    unsigned reserved2:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_AS2CRn_t;

typedef union HAL_IOMMU_SMRn_t
{
  struct {
    union {
      struct {
        unsigned ID:15;
        unsigned reserved:1;
        unsigned MASK:15;
        unsigned VALID:1;
      } type_exid0;
      struct {
        unsigned ID:16;
        unsigned MASK:16;
      } type_exid1;
    } smr_type;
  } formatted;
  unsigned raw;
} HAL_IOMMU_SMRn_t;

typedef union HAL_IOMMU_CBn_FSR_t
{
  struct {
    unsigned reserved:1;
    unsigned TF:1;
    unsigned AFF:1;
    unsigned PF:1;
    unsigned EF:1;
    unsigned TLBMCF:1;
    unsigned TLBLKF:1;
    unsigned reserved2:23;
    unsigned SS:1;
    unsigned MULTI:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBn_FSR_t;

typedef union HAL_IOMMU_CBn_TTBCR_t
{
  struct {
    union {
      struct {
        union {
          // ARMv7 VMSA - short descriptor
          struct {
            unsigned T0SZ:3;
            unsigned SBZ:1;
            unsigned PD0:1;
            unsigned PD1:1;
            unsigned reserved:8;
            unsigned NSCFG0:1;
            unsigned reserved2:15;
            unsigned NSCFG1:1;
            unsigned EAE:1;
          } eae_0;
          // LPAE
          struct {
            union {
              struct {
                unsigned T0SZ:3;
                unsigned reserved1:4;
                unsigned EPD0:1;
                unsigned IRGN0:2;
                unsigned ORGN0:2;
                unsigned SH0:2;
                unsigned NSCFG0:1;
                unsigned reserved2:1;
                unsigned T1SZ:3;
                unsigned reserved3:3;
                unsigned A1:1;
                unsigned EPD1:1;
                unsigned IRGN1:2;
                unsigned ORGN1:2;
                unsigned SH1:2;
                unsigned NSCFG1:1;
                unsigned EAE:1;
              } s1;
              // S2 HAL_IOMMU_CBn_TCR
              struct {
                unsigned T0SZ:4;
                unsigned reserved1:2;
                unsigned SL0:2;
                unsigned IRGN0:2;
                unsigned ORGN0:2;
                unsigned SH0:2;
                unsigned reserved2:17;
                unsigned EAE:1;
              } s2;
            } translation_stage;
          } eae_1;
        } eae_state;
      } va64_0;
      struct {
        union {
          struct {
            unsigned T0SZ:6;
            unsigned reserved:1;
            unsigned EPD0:1;
            unsigned IRGN0:2;
            unsigned ORGN0:2;
            unsigned SH0:2;
            unsigned TG0:2;
            unsigned T1SZ:6;
            unsigned A1:1;
            unsigned EPD1:1;
            unsigned IRGN1:2;
            unsigned ORGN1:2;
            unsigned SH1:2;
            unsigned TG1:2;
          } s1;
          struct {
            unsigned T0SZ:6;
            unsigned SL0:2;
            unsigned IRGN0:2;
            unsigned ORGN0:2;
            unsigned SH0:2;
            unsigned TG0:2;
            unsigned PASIZE:3;
            unsigned reserved:13;
          } s2;
        } translation_stage;
      } va64_1;
    } va64_type;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBn_TTBCR_t;

typedef union HAL_IOMMU_CBn_TCR2_t
{
  struct 
  {
    unsigned PASIZE:3;
    unsigned reserved1:1;
    unsigned AS:1;
    unsigned TBI0:1;
    unsigned TBI1:1;
    unsigned reserved2:7;
    unsigned NSCFG0:1;
    unsigned SEP:3;
    unsigned reserved3:12;
    unsigned NSCFG1:1;
    unsigned reserved4:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBn_TCR2_t;

typedef union HAL_IOMMU_CBn_ACTLR_t
{
  struct {
    unsigned REQPRIORITY:2;
    unsigned reserved1:2;
    unsigned REQPRIORITYCFG:1;
    unsigned PRR_ENABLE:1;
    unsigned reserved2:2;
    unsigned PRIVCFG:2;
    unsigned reserved3:18;
    unsigned BPRCOSH:1;
    unsigned BPRCISH:1;
    unsigned BPRCNSH:1;
    unsigned reserved4:1;
  } formatted;
  unsigned raw;
} HAL_IOMMU_CBn_ACTLR_t;

typedef union HAL_IOMMU_REG_DYNCLK_CTRL_t
{
  struct {
    unsigned FORCE_CLK_ON:1;
    unsigned reserved1:3;
    unsigned HYST_CNT:7;
    unsigned reserved2:21;
  } formatted;
  unsigned raw;
} HAL_IOMMU_REG_DYNCLK_CTRL_t;

typedef union HAL_IOMMU_MICRO_MMU_CTRL_t
{
  struct {
    unsigned INDEX:3;
    unsigned IDLE:1;
    unsigned GSE_FORCE:1;
    unsigned TC1_BFB_AFF_TF_ISCACHED:1;
    unsigned TC2_BFB_AFF_TF_ISCACHED:1;
    unsigned MSCH_REQ_FIX_PRY_ENABLE:1;
    unsigned reserved:24;
  } formatted;
  unsigned raw;
} HAL_IOMMU_MICRO_MMU_CTRL_t;


/**
* HAL IOMMU API Handle definition
*/

/* Global context bank HAL APIs */

HAL_IOMMUResult_t   HAL_IOMMU_GetSCTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_SCTLR_t *sctlr);
HAL_IOMMUResult_t   HAL_IOMMU_SetSCTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_SCTLR_t sctlr);
HAL_IOMMUResult_t   HAL_IOMMU_GetFSR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_FSR_t *fsr);
HAL_IOMMUResult_t   HAL_IOMMU_SetFSRRestore(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_FSR_t fsr);
HAL_IOMMUResult_t   HAL_IOMMU_GetFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *far);
HAL_IOMMUResult_t   HAL_IOMMU_GetFAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *far0);
HAL_IOMMUResult_t   HAL_IOMMU_GetFAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *far1);
HAL_IOMMUResult_t   HAL_IOMMU_SetFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 far);
HAL_IOMMUResult_t   HAL_IOMMU_SetFAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 far0);
HAL_IOMMUResult_t   HAL_IOMMU_SetFAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 far1);
HAL_IOMMUResult_t   HAL_IOMMU_GetFSYN0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *fsyn0);
HAL_IOMMUResult_t   HAL_IOMMU_GetFSYN1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *fsyn1);
HAL_IOMMUResult_t   HAL_IOMMU_SetFSYN0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 fsyn0);
HAL_IOMMUResult_t   HAL_IOMMU_SetFSYN1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 fsyn1);
HAL_IOMMUResult_t   HAL_IOMMU_GetIPAFAR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ipafar);
HAL_IOMMUResult_t   HAL_IOMMU_GetPAR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *par0);
HAL_IOMMUResult_t   HAL_IOMMU_GetPAR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *par1);
HAL_IOMMUResult_t   HAL_IOMMU_GetPRRR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *prrr);
HAL_IOMMUResult_t   HAL_IOMMU_GetNMRR(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *nmrr);
HAL_IOMMUResult_t   HAL_IOMMU_SetMAIR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 mair0);
HAL_IOMMUResult_t   HAL_IOMMU_SetMAIR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 mair1);
HAL_IOMMUResult_t   HAL_IOMMU_GetMAIR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *mair0);
HAL_IOMMUResult_t   HAL_IOMMU_GetMAIR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 *mair1);
HAL_IOMMUResult_t   HAL_IOMMU_SetAsid(uint32 baseVirtAddr, uint32 ctxBankNumber, uint8 asid);
HAL_IOMMUResult_t   HAL_IOMMU_SetTTBR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ttAddr);
HAL_IOMMUResult_t   HAL_IOMMU_GetTTBR0(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ttAddr);
HAL_IOMMUResult_t   HAL_IOMMU_SetTTBR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ttAddr);
HAL_IOMMUResult_t   HAL_IOMMU_GetTTBR1(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 *ttAddr);
HAL_IOMMUResult_t   HAL_IOMMU_GetTTBCR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TTBCR_t *ttbcr);
HAL_IOMMUResult_t   HAL_IOMMU_SetTTBCR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TTBCR_t ttbcr);
HAL_IOMMUResult_t   HAL_IOMMU_GetTCR2(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TCR2_t *tcr2);
HAL_IOMMUResult_t   HAL_IOMMU_SetTCR2(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_TCR2_t tcr2);
HAL_IOMMUResult_t   HAL_IOMMU_GetACTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_ACTLR_t *actlr);
HAL_IOMMUResult_t   HAL_IOMMU_SetACTLR(uint32 baseVirtAddr, uint32 ctxBankNumber, HAL_IOMMU_CBn_ACTLR_t actlr);
HAL_IOMMUResult_t   HAL_IOMMU_GetCBARn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBARn_t *cbar);
HAL_IOMMUResult_t   HAL_IOMMU_SetCBARn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBARn_t cbar);
HAL_IOMMUResult_t   HAL_IOMMU_GetCBFRSYNRAn(uint32 baseVirtAddr, uint32 index, uint32 *cbfrsynra);
HAL_IOMMUResult_t   HAL_IOMMU_SetCBFRSYNRAn(uint32 baseVirtAddr, uint32 index, uint32 cbfrsynra);
HAL_IOMMUResult_t   HAL_IOMMU_GetCBA2Rn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBA2Rn_t *cba2r);
HAL_IOMMUResult_t   HAL_IOMMU_SetCBA2Rn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_CBA2Rn_t cba2r);
HAL_IOMMUResult_t   HAL_IOMMU_GetS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_S2CRn_t *s2cr);
HAL_IOMMUResult_t   HAL_IOMMU_SetS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_S2CRn_t s2cr);
HAL_IOMMUResult_t   HAL_IOMMU_GetAS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_AS2CRn_t *as2cr);
HAL_IOMMUResult_t   HAL_IOMMU_SetAS2CRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_AS2CRn_t as2cr);
HAL_IOMMUResult_t   HAL_IOMMU_SetSMRn(uint32 baseVirtAddr, uint32 index, uint32 id);
HAL_IOMMUResult_t   HAL_IOMMU_SetSMRn_raw(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_SMRn_t smr);
HAL_IOMMUResult_t   HAL_IOMMU_GetSMRn(uint32 baseVirtAddr, uint32 index, HAL_IOMMU_SMRn_t *smr);
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureIDR0(uint32 baseVirtAddr, HAL_IOMMU_Secure_IDR0_t *SecureIdr0);
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureIDR1(uint32 baseVirtAddr, HAL_IOMMU_Secure_IDR1_t *SecureIdr1);
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t *secureCR0);
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t secureCR0);
HAL_IOMMUResult_t   HAL_IOMMU_GetNonSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t *nonSecureCR0);
HAL_IOMMUResult_t   HAL_IOMMU_SetNonSecureCR0(uint32 baseVirtAddr, HAL_IOMMU_CR0_t nonSecureCR0);
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t *secureACR);
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t secureACR);
HAL_IOMMUResult_t   HAL_IOMMU_GetNonSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t *nonSecureACR);
HAL_IOMMUResult_t   HAL_IOMMU_SetNonSecureACR(uint32 baseVirtAddr, HAL_IOMMU_ACR_t nonSecureACR);
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t *secureCR2);
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t secureCR2);
HAL_IOMMUResult_t   HAL_IOMMU_GetNonSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t *nonSecureCR2);
HAL_IOMMUResult_t   HAL_IOMMU_SetNonSecureCR2(uint32 baseVirtAddr, HAL_IOMMU_CR2_t nonSecureCR2);
HAL_IOMMUResult_t   HAL_IOMMU_GetSecureCR1(uint32 baseVirtAddr, HAL_IOMMU_SCR1_t *secureCR1);
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureCR1(uint32 baseVirtAddr, HAL_IOMMU_SCR1_t secureCR1);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFAR(uint32 baseVirtAddr, uint64 *gfAddr);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFAR0(uint32 baseVirtAddr, uint32 *gfAddr0);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFAR1(uint32 baseVirtAddr, uint32 *gfAddr1);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFAR(uint32 baseVirtAddr, uint64 gfAddr);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFAR0(uint32 baseVirtAddr, uint32 gfAddr0);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFAR1(uint32 baseVirtAddr, uint32 gfAddr1);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFAR(uint32 baseVirtAddr, uint64 *nsgfAddr);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFAR0(uint32 baseVirtAddr, uint32 *nsgfAddr0);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFAR1(uint32 baseVirtAddr, uint32 *nsgfAddr1);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFAR(uint32 baseVirtAddr, uint64 nsgfAddr);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFAR0(uint32 baseVirtAddr, uint32 nsgfAddr0);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFAR1(uint32 baseVirtAddr, uint32 nsgfAddr1);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFSR(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t *gfsr);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSR(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t *nsgfsr);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFSRRestore(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t gfsr);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSRRestore(uint32 baseVirtAddr, HAL_IOMMU_GFSR_t nsgfsr);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFSYNR0(uint32 baseVirtAddr, uint32 *gfsynr0);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFSYNR1(uint32 baseVirtAddr, uint32 *gfsynr1);
HAL_IOMMUResult_t   HAL_IOMMU_GetGFSYNR2(uint32 baseVirtAddr, uint32 *gfsynr2);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFSYNR0(uint32 baseVirtAddr, uint32 gfsynr0);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFSYNR1(uint32 baseVirtAddr, uint32 gfsynr1);
HAL_IOMMUResult_t   HAL_IOMMU_SetGFSYNR2(uint32 baseVirtAddr, uint32 gfsynr2);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSYNR0(uint32 baseVirtAddr, uint32 *nsgfsynr0);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSYNR1(uint32 baseVirtAddr, uint32 *nsgfsynr1);
HAL_IOMMUResult_t   HAL_IOMMU_GetNSGFSYNR2(uint32 baseVirtAddr, uint32 *nsgfsynr2);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSYNR0(uint32 baseVirtAddr, uint32 nsgfsynr0);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSYNR1(uint32 baseVirtAddr, uint32 nsgfsynr1);
HAL_IOMMUResult_t   HAL_IOMMU_SetNSGFSYNR2(uint32 baseVirtAddr, uint32 nsgfsynr2);
HAL_IOMMUResult_t   HAL_IOMMU_SetSecureIndex(uint32 baseVirtAddr, uint32 index);
HAL_IOMMUResult_t   HAL_IOMMU_ClearSecureIndex(uint32 baseVirtAddr, uint32 index);
HAL_IOMMUResult_t   HAL_IOMMU_GlobalInvalidateSecureTLBAll(uint32 baseVirtAddr); 
HAL_IOMMUResult_t   HAL_IOMMU_GlobalInvalidateNonSecureTLBVMID(uint32 baseVirtAddr, uint32 uVMID);
HAL_IOMMUResult_t   HAL_IOMMU_GlobalInvalidateNonSecureNonHypTLBAll(uint32 baseVirtAddr);
HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBAll(uint32 baseVirtAddr, uint32 ctxBankNumber);
HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBVA(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 vaddr);
HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBIPAS2(uint32 baseVirtAddr, uint32 ctxBankNumber, uint64 ipaAddr);
HAL_IOMMUResult_t   HAL_IOMMU_InvalidateCBTLBSync(uint32 baseVirtAddr, uint32 ctxBankNumber);
HAL_IOMMUResult_t   HAL_IOMMU_SetResumeStalledTransaction(uint32 baseVirtAddr, uint32 ctxBankNumber, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegDynClkCtrl(uint32 baseVirtAddr, HAL_IOMMU_REG_DYNCLK_CTRL_t dynClkCtrl);
HAL_IOMMUResult_t   HAL_IOMMU_GetRegDynClkCtrl(uint32 baseVirtAddr, HAL_IOMMU_REG_DYNCLK_CTRL_t *dynClkCtrl);
HAL_IOMMUResult_t   HAL_IOMMU_SetMicroMMUCtrl(uint32 baseVirtAddr, HAL_IOMMU_MICRO_MMU_CTRL_t uMMUCtrl);
HAL_IOMMUResult_t   HAL_IOMMU_GetMicroMMUCtrl(uint32 baseVirtAddr, HAL_IOMMU_MICRO_MMU_CTRL_t *uMMUCtrl);
HAL_IOMMUResult_t   HAL_IOMMU_SetHTWAReqCtlr(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetHUMCfg(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetFilterCfg(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTLBIFilterCfg(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1SPTLBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1SPTLBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BPTLBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BPTLBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1L0BFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1L0BFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1GBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1GBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC2GBBFBLowerPtrs_0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC2GBBFBLowerPtrsn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1SPTLBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BPTLBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1GBBFBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBSRAMBaseAddr0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1BTLBWidth0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1MBBFBWidth0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTC1KBBFBWidth0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetSS2TBn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetNestedPrefetchEnable0(uint32 baseVirtAddr, uint64 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetNestedPrefetchEnablen(uint32 baseVirtAddr, uint32 index, uint64 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetNestedPrefetchCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetSlotManagerDemandReqCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetSlotManagerNestedReqCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUClkCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUQOSARBCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUQOSARBFPCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUQOSTransSchedulerReservation(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUQOSMSDemandHTWReservation(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUQOSMSPrefetchHTWReservation(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTLBISCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetTHBSctrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_GetDebugTHBCtrl(uint32 baseVirtAddr, uint32 *value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStartFilterCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStartFilterCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBLogFilterCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBLogFilterVmid(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStopFilterCtrl(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBStopFilterState(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetDebugTHBWCmd(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUOTReservationn(uint32 baseVirtAddr, uint32 index, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUPLVLCtrl0(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUuPLVLCtrl1(uint32 baseVirtAddr, uint32 value);
HAL_IOMMUResult_t   HAL_IOMMU_SetRegHAL_IOMMUPLVLHTWCtrln(uint32 baseVirtAddr, uint32 index, uint32 value);

/*-----------------------------------------------------------------------------
 * Inline Helper Functions
 *---------------------------------------------------------------------------*/
static __inline uint32 HAL_IOMMU_CTX_BANK0_OFFSET(uint32 baseAddr)
{
  HAL_IOMMU_Secure_IDR1_t uIDR1Val = {{0}};
  uint32 uPageSize = 0;
  uint32 uNumPage = 0;

  (void)HAL_IOMMU_GetSecureIDR1(baseAddr, &uIDR1Val);
  uPageSize = (uIDR1Val.formatted.PAGESIZE == 0) ? 0x1000 : 0x10000;
  uNumPage = 1 << (uIDR1Val.formatted.NUMPAGENDXB + 1);
  return (uPageSize * uNumPage);
}

#ifdef __cplusplus
}
#endif

#endif /* __HAL_IOMMU_REG_H__ */
