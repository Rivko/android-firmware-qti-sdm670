#ifndef HWIO_UFS_V1_H
#define HWIO_UFS_V1_H

/**********************************************************************
 *
 * Copyright (c) 2012-2016 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE

when         who     what, where, why
----------   ---     ---------------------------------------------------
2016-11-04   jt      Get base address from msmhwiobase.h   
2016-08-22   jt      Update for 8998v2 
2016-07-22   jt      Adjust for Napali 
2012-12-18   rh      Initial creation 
=======================================================================*/

#define UFSHCI_OFFSET_ADDR                               0x4000

/* Define the required HWIO address mapping function */
#define HWIO_UFSHCI_CAP                                  (0x0000 + UFSHCI_OFFSET_ADDR)  
#define HWIO_UFSHCI_VER                                  (0x0008 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_HCDDID                               (0x0010 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_HCPMID                               (0x0014 + UFSHCI_OFFSET_ADDR)

#define HWIO_UFSHCI_IS                                   (0x0020 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_IE                                   (0x0024 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_HCS                                  (0x0030 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_HCE                                  (0x0034 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UECPA                                (0x0038 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UECDL                                (0x003C + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UECN                                 (0x0040 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UECT                                 (0x0044 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UECDME                               (0x0048 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTRIACR                              (0x004C + UFSHCI_OFFSET_ADDR)

#define HWIO_UFSHCI_UTRLBA                               (0x0050 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTRLBAU                              (0x0054 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTRLDBR                              (0x0058 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTRLCLR                              (0x005C + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTRLRSR                              (0x0060 + UFSHCI_OFFSET_ADDR)

#define HWIO_UFSHCI_UTMRLBA                              (0x0070 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTMRLBAU                             (0x0074 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTMRLDBR                             (0x0078 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTMRLCLR                             (0x007C + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UTMRLRSR                             (0x0080 + UFSHCI_OFFSET_ADDR)

#define HWIO_UFSHCI_UICCMDR                              (0x0090 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UCMDARG1                             (0x0094 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UCMDARG2                             (0x0098 + UFSHCI_OFFSET_ADDR)
#define HWIO_UFSHCI_UCMDARG3                             (0x009C + UFSHCI_OFFSET_ADDR)

#define HWIO_VENSPEC_UFS_CFG1                            (0x00DC + UFSHCI_OFFSET_ADDR)
#define HWIO_VENSPEC_UFS_CFG2                            (0x00E0 + UFSHCI_OFFSET_ADDR)
#define HWIO_VENSPEC_UFS_HW_VERSION                      (0x00E4 + UFSHCI_OFFSET_ADDR)
#define HWIO_VENSPEC_UFS_SYS1CLK_1US                     (0x00C0 + UFSHCI_OFFSET_ADDR)
#define HWIO_VENSPEC_TX_SYMBOL_CLK_NS_US                 (0x00C4 + UFSHCI_OFFSET_ADDR)
#define HWIO_VENSPEC_REG_PA_ERR_CODE                     (0x00CC + UFSHCI_OFFSET_ADDR)
#define HWIO_VENSPEC_REG_PA_LINK_STARTUP_TIMER           (0x00d8 + UFSHCI_OFFSET_ADDR)



/* Bit field of UFSHCI_CAP register */
#define BMSK_NUTRS                                       (0x1f << 0)
#define BMSK_NUTMRS                                      (0x07 << 16)
#define BMSK_64AS                                        (0x01 << 24)
#define BMSK_OODDS                                       (0x01 << 25)
#define BMSK_UICDMETMS                                   (0x01 << 26)

/* Bit field of UFSHCI_IS register */
#define BMSK_UTRCS                                       (0x01 << 0)
#define BMSK_UDEPRI                                      (0x01 << 1)
#define BMSK_UE                                          (0x01 << 2)
#define BMSK_UTMS                                        (0x01 << 3)
#define BMSK_UPMS                                        (0x01 << 4)
#define BMSK_UHXS                                        (0x01 << 5)
#define BMSK_UHES                                        (0x01 << 6)
#define BMSK_ULLS                                        (0x01 << 7)
#define BMSK_ULSS                                        (0x01 << 8)
#define BMSK_UTMRCS                                      (0x01 << 9)
#define BMSK_UCCS                                        (0x01 << 10)
#define BMSK_DFES                                        (0x01 << 11)
#define BMSK_UTPES                                       (0x01 << 12)
#define BMSK_HCFES                                       (0x01 << 16)
#define BMSK_SBFES                                       (0x01 << 17)

/* Bit field of UFSHCI_HCS register */
#define BMSK_UPMCRS                                      (0x03 << 8)
#define UPMCRS_PWR_OK                                    (0)
#define UPMCRS_PWR_LOCAL                                 (1)
#define UPMCRS_PWR_REMOTE                                (2)
#define UPMCRS_PWR_BUSY                                  (3)
#define UPMCRS_PWR_ERROR_CAP                             (4)
#define UPMCRS_PWR_FATAL_ERROR                           (5)
#define BMSK_UCRDY                                       (0x01 << 3)
#define BMSK_UTMRLRDY                                    (0x01 << 2)
#define BMSK_UTRLRDY                                     (0x01 << 1)
#define BMSK_DP                                          (0x01 << 0)

/* Bit field of UFSHCI_HCE register */
#define BMSK_HCE                                         (0x01 << 0)

/* Bit field of UFSHCI_UTRIACR register */
#define BMSK_IAEN                                        (0x01 << 31)
#define BMSK_IAPWEN                                      (0x01 << 24)
#define BMSK_IASB                                        (0x01 << 20)
#define BMSK_CTR                                         (0x01 << 16)
#define BMSK_IACTH                                       (0x3f << 8)
#define BMSK_IATOVAL                                     (0xff << 0)

/* Bit field of UFSHCI_UTRLRSR register */
#define BMSK_UTRLRSR                                     (0x01 << 0)

/* Bit field of UFSHCI_UTMRLRSR register */
#define BMSK_UTMRLRSR                                    (0x01 << 0)

/* Bit field of UFSHCI_UICCMDR register */
#define UICCMDR_DME_GET                                  0x01
#define UICCMDR_DME_SET                                  0x02
#define UICCMDR_DME_PEER_GET                             0x03
#define UICCMDR_DME_PEER_SET                             0x04
#define UICCMDR_DME_POWERON                              0x10
#define UICCMDR_DME_POWEROFF                             0x11
#define UICCMDR_DME_ENABLE                               0x12
#define UICCMDR_DME_RESET                                0x14
#define UICCMDR_DME_ENDPOINTRESET                        0x15
#define UICCMDR_DME_LINKSTARTUP                          0x16
#define UICCMDR_DME_HIBERNATE_ENTER                      0x17
#define UICCMDR_DME_HIBERNATE_EXIT                       0x18
#define UICCMDR_DME_TEST_MODE                            0x1a
       
/* Bit field of QC_UFS_CFG1 register */
#define BMSK_UTP_PHY_SOFT_RESET                          0x02

/* Bit field of QC_UFS_SYS1CLK_1US register */
#define BMSK_SYS1CLK_1US_REG                             0x1ff
#define SHFT_SYS1CLK_1US_REG                             0

/* Bit field of QC_UFS_TX_SYMBOL_CLK_NS_US register */
#define BMSK_TX_SYMBOL_CLK_1US_REG                       0x1ff
#define SHFT_TX_SYMBOL_CLK_1US_REG                       0
#define BMSK_CLK_NS_REG                                  (0xfff << 10)
#define SHFT_CLK_NS_REG                                  (10)

/* Define the required HWIO input/out macro mapping */

#define HWIO_REG_IN(base, reg)                           \
        (in_dword((base + HWIO_##reg)))
#define HWIO_REG_OUT(base, reg, v)                       \
        (out_dword((base + HWIO_##reg),v))
#define HWIO_REG_OUTM(base, reg, m, v)                   \
        (out_dword((base + HWIO_##reg),  \
        ((in_dword((base + HWIO_##reg)) & (m)) | (v))))

/* Concept - Caller must supply a register base addree, register name
 * and value  */

#endif /* HWIO_UFS_V1_H */

