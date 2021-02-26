
/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#pragma once

#define APSS_HM_BASE                         0x17800000
#define APSS_HM_BASE_PHYS                    0x17800000
#define APCS_GIC500_GICD_REG_BASE            (APSS_HM_BASE       + 0x00200000)
#define APCS_ALIAS0_GIC500_GICR_REG_BASE     (APSS_HM_BASE       + 0x00300000)

#define APCS_QGICDR_R0_R0_QGICDR_GICR_REG_BASE_PHYS  (APCS_ALIAS0_GIC500_GICR_REG_BASE)
#define APCS_QGICDR_QGICDR_GICD_REG_BASE_PHYS        (APCS_GIC500_GICD_REG_BASE)

/*---------------------------------------------------------------------------
  QGIC Base Address.
---------------------------------------------------------------------------*/
#define HAL_QGIC_GICD_BASE_ADDR   APCS_GIC500_GICD_REG_BASE
#define HAL_QGIC_GICC_BASE_ADDR   0x17C00000
#define HAL_QGIC_GICH_BASE_ADDR   0
#define HAL_QGIC_GICV_BASE_ADDR   0
#define HAL_QGIC_GICR_BASE_ADDR   APCS_ALIAS0_GIC500_GICR_REG_BASE

#define IsGICv3Enabled()          1
#define GetGICCBaseAddr()         HAL_QGIC_GICC_BASE_ADDR

#define HWIO_APCS_QGICDR_GICD_IPRIORITYRn_ADDR(n) (HAL_QGIC_GICD_BASE_ADDR    + 0x00000400 + 0x4 * (n))
#define HWIO_APCS_QGICDR_GICD_ICENABLERn_ADDR(n)  (HAL_QGIC_GICD_BASE_ADDR    + 0x00000180 + 0x4 * (n))
#define HWIO_APCS_QGICDR_GICD_ISPENDRn_ADDR(n)    (HAL_QGIC_GICD_BASE_ADDR    + 0x00000200 + 0x4 * (n))
#define HWIO_APCS_QGICDR_GICD_ISENABLERn_ADDR(n)  (HAL_QGIC_GICD_BASE_ADDR    + 0x00000100 + 0x4 * (n))
#define HWIO_APCS_QGICDR_GICD_ITARGETSRn_ADDR(n)  (HAL_QGIC_GICD_BASE_ADDR    + 0x00000800 + 0x4 * (n))
#define HWIO_APCS_QGICDR_GICD_ISPENDRn_ADDR(n)    (HAL_QGIC_GICD_BASE_ADDR    + 0x00000200 + 0x4 * (n))

