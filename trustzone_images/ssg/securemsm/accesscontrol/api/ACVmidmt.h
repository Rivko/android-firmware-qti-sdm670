#ifndef TZBSP_VMIDMT_H
#define TZBSP_VMIDMT_H

/*===========================================================================
  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ============================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/accesscontrol/api/ACVmidmt.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  05/18/12   sg      First version.
  ============================================================================*/

#include <HALvmidmt.h>
#include <stdbool.h>
#include "ACCommon.h"
#include "tz_syscall_pub.h"

#define TZBSP_VMID_NOACCESS     0
#define TZBSP_VMID_VMID_0       0
#define TZBSP_VMID_RPM          1
#define TZBSP_VMID_TZ           2
#define TZBSP_VMID_AP           3
#define TZBSP_VMID_MSS          4
#define TZBSP_VMID_LPASS        5
#define TZBSP_VMID_CP           6
#define TZBSP_VMID_VIDEO        7
#define TZBSP_VMID_ZAP_SHADER   8
#define TZBSP_VMID_MDSS         9
/* Space for VMID 10 */
#define TZBSP_VMID_SSC          11

/* These values were obtained by the IPA team, and are documented in the
 ** 'IPAv2 Pipes - Istari SMMU VMID configuration' spreadsheet
 ** these shouldn't be changed as these are used by IPA to generate SID */
#define TZBSP_VMID_IPA_AP       (0b1100) /* 12 */
#define TZBSP_VMID_IPA_UC_PIPE  (0b1101) /* 13 */
#define TZBSP_VMID_IPA_WIFI     TZBSP_VMID_IPA_UC_PIPE
#define TZBSP_VMID_IPA_UC       (0b1110) /* 14*/

/* 9x35, 9x45 IPA VMID need special care as per below description from IPA h/w team.
 **
 ** IPA uC handles power collapse save and restore producer. Without XPU and VMIDMT 
 ** retention capability in Elessar IPA uC restores their state. 
 ** This means that uC firmware could use any VMID value (by reprogramming the VMIDMT).
 ** Thus security team required that IPA HW always sets VMID bit 4 as high so that it will
 ** not be able to use ‘regular’ system VMIDs. (same IPA impl was carried over to 9x45)
 */
#define TZBSP_VMID_IPA_AP_MDM       0x13 /* BIT 4 SET | TZBSP_VMID_AP */
#define TZBSP_VMID_IPA_UC_PIPE_MDM  0x14 /* BIT 4 SET | TZBSP_VMID_MSS */
#define TZBSP_VMID_IPA_UC_MDM       0x15 /* BIT 4 SET & new VMID */

#define TZBSP_VMID_WLAN         15

/* For Napali QUPv3, there is a GSI mode for HLOS and a normal mode
 * The GSI mode has a different SID
 * Also on Napali v1 and possibly v2 we have TZ VMID programmed in HW
 * as 0x0 and it cannot be configured, hence we will have to handle it specially with the below
 * new VMID
 */
#define TZBSP_VMID_AP_GSI       0x16
#define TZBSP_VMID_QUP_TZ       0x0

#define TZBSP_VMID_NOACCESS_BIT     (1<<TZBSP_VMID_NOACCESS)
#define TZBSP_VMID_VMID_0_BIT       (1<<TZBSP_VMID_VMID_0)
#define TZBSP_VMID_TZ_BIT           (1<<TZBSP_VMID_TZ)
#define TZBSP_VMID_RPM_BIT          (1<<TZBSP_VMID_RPM)
#define TZBSP_VMID_LPASS_BIT        (1<<TZBSP_VMID_LPASS)
#define TZBSP_VMID_MSS_BIT          (1<<TZBSP_VMID_MSS)
#define TZBSP_VMID_AP_BIT           (1<<TZBSP_VMID_AP)
#define TZBSP_VMID_CP_BIT           (1<<TZBSP_VMID_CP)
#define TZBSP_VMID_VIDEO_BIT        (1<<TZBSP_VMID_VIDEO)
#define TZBSP_VMID_MDSS_BIT         (1<<TZBSP_VMID_MDSS)
#define TZBSP_VMID_SSC_BIT          (1<<TZBSP_VMID_SSC)

#define TZBSP_VMID_IPA_AP_BIT       (1 << TZBSP_VMID_IPA_AP)
#define TZBSP_VMID_IPA_UC_PIPE_BIT  (1 << TZBSP_VMID_IPA_UC_PIPE)
#define TZBSP_VMID_IPA_UC_BIT       (1 << TZBSP_VMID_IPA_UC)

#define TZBSP_VMID_IPA_AP_MDM_BIT       (1 << TZBSP_VMID_IPA_AP_MDM)
#define TZBSP_VMID_IPA_UC_PIPE_MDM_BIT  (1 << TZBSP_VMID_IPA_UC_PIPE_MDM)
#define TZBSP_VMID_IPA_UC_MDM_BIT       (1 << TZBSP_VMID_IPA_UC_MDM)

#define TZBSP_VMID_WLAN_BIT         (1<<TZBSP_VMID_WLAN)
#define TZBSP_VMID_ZAP_SHADER_BIT   (1<<TZBSP_VMID_ZAP_SHADER)
#define TZBSP_VMID_IPA_WIFI_BIT     (1<<TZBSP_VMID_IPA_WIFI)

#define TZBSP_ALL_VMID              ((~TZBSP_VMID_NOACCESS) & (~TZBSP_DOMAIN_MASK))


/* Domain definitions. Start downwards from 31 so it doesn't conflict with VMIDs */
#define TZBSP_DOMAIN_MASK 0xFF000000 /* Reserve 8 bits for domains */
#define TZBSP_DOMAIN_MSA   31
#define TZBSP_DOMAIN_HYP   30
#define TZBSP_DOMAIN_SP    29
#define TZBSP_DOMAIN_TZ    28


/* Domain definitions */
#define TZBSP_DOMAIN_MSA_BIT   (1U << TZBSP_DOMAIN_MSA)
#define TZBSP_DOMAIN_HYP_BIT   (1U << TZBSP_DOMAIN_HYP)
#define TZBSP_DOMAIN_SP_BIT    (1U << TZBSP_DOMAIN_SP)
#define TZBSP_DOMAIN_TZ_BIT    (1U << TZBSP_DOMAIN_TZ)


/**
 * API to map a vmid. Allows only specific instances to be remapped
 *
 * @param [in] master VMIDMT instance
 * @param [in] index  Index for which VMID has to be mapped
 * @param [in] vmid   VMID to be mapped
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
int tzbsp_vmidmt_map_vmid(HAL_vmidmt_InstanceType master,
        uint8 index, uint32 vmid, bool secure);

/**
 * API to map a vmid. Allows only specific instances to be remapped
 *
 * @param [in] eVmidmtInstance  VMIDMT instance
 * @param [in] pInternalSidList List of internal SIDs to be mapped 
 * @param [in] vmid   VMID to be mapped to 
 *
 * @return \c E_SUCCESS if successful, error code otherwise.
 */
AC_ERROR_CODE ACMapSidToVmidmt(HAL_vmidmt_InstanceType eVmidmtInstance,
        uint32 *pInternalSidList,
        uint32 uNumInternalSids,
        ACVirtualMachineId  uDestinationVm);

#endif /* TZBSP_VMIDMT_H */
