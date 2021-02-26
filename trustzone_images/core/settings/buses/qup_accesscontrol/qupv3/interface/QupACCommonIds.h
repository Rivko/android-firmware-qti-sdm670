#ifndef QUPAC_COMMONIDS_H
#define QUPAC_COMMONIDS_H

/*===========================================================================
         Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  
              All Rights Reserved.
            QUALCOMM Confidential & Proprietary
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE


  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  11/01/17   MJS     Add support for Hana.
  10/09/17   MJS     Give trustzone option for exclusive access during use cases.
  06/16/17   MJS     Updates to GSI SID access control
  04/28/17   MJS     Add AC_HLOS_GSI
  04/25/17   MJS     Disable DFS for UART serial engines
  04/11/17   MJS     Add AC_LPASS
  03/03/17   MJS     Initial revision
  =============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/** Defines for QUP access ids */
#define AC_NONE  0
#define AC_TZ  1
#define AC_HLOS_GSI 2
#define AC_HLOS  3
#define AC_HYP  4
#define AC_SSC_Q6_ELF 5
#define AC_ADSP_Q6_ELF 6 // Single 
#define AC_SSC_HLOS 7   // ??, may be we combine this with other SSC one //
#define AC_CP_TOUCH 8
#define AC_CP_BITSTREAM 9
#define AC_CP_PIXEL 10
#define AC_CP_NON_PIXEL 11
#define AC_VIDEO_FW 12
#define AC_CP_CAMERA 13
#define AC_HLOS_UNMAPPED 14
#define AC_MSS_MSA 15
#define AC_MSS_NONMSA 16
#define AC_UNMAPPED 17
#define AC_LPASS 18
#define BLSP_AC_LAST 19
#define AC_DEFAULT 0xFF// Default as in retain whatever in SMMU static config table

/** QUPV3 serial engine IDs */
typedef enum {
   PERIPH_ENUM_MIN = 0,
   QUPV3_0_SE0 = PERIPH_ENUM_MIN,
   QUPV3_0_SE1,
   QUPV3_0_SE2,
   QUPV3_0_SE3,
   QUPV3_0_SE4,
   QUPV3_0_SE5,
   QUPV3_0_SE6,
   QUPV3_0_SE7,
   QUPV3_1_SE0,
   QUPV3_1_SE1,
   QUPV3_1_SE2,
   QUPV3_1_SE3,
   QUPV3_1_SE4,
   QUPV3_1_SE5,
   QUPV3_1_SE6,
   QUPV3_1_SE7,
   QUPV3_2_SE0,
   QUPV3_2_SE1,
   QUPV3_2_SE2,
   QUPV3_2_SE3,
   QUPV3_2_SE4,
   QUPV3_2_SE5,
   QUPV3_2_SE6,
   QUPV3_2_SE7,
   QUPV3_SSC_SE0,
   QUPV3_SSC_SE1,
   QUPV3_SSC_SE2,
   QUPV3_SSC_SE3,
   QUPV3_SSC_SE4,
   QUPV3_SSC_SE5,
   QUPV3_SSC_SE6,
   QUPV3_SSC_SE7,
   QUPV3_SE_END,
   QUPV3_GPII_START = QUPV3_SE_END,
   QUPV3_0_GPII0 = QUPV3_GPII_START,
   QUPV3_0_GPII1,
   QUPV3_0_GPII2,
   QUPV3_0_GPII3,
   QUPV3_0_GPII4,
   QUPV3_0_GPII5,
   QUPV3_0_GPII6,
   QUPV3_0_GPII7,
   QUPV3_0_GPII8,
   QUPV3_0_GPII9,
   QUPV3_0_GPII10,
   QUPV3_0_GPII11,
   QUPV3_0_GPII12,
   QUPV3_0_GPII13,
   QUPV3_0_GPII14,
   QUPV3_0_GPII15,
   QUPV3_1_GPII0,
   QUPV3_1_GPII1,
   QUPV3_1_GPII2,
   QUPV3_1_GPII3,
   QUPV3_1_GPII4,
   QUPV3_1_GPII5,
   QUPV3_1_GPII6,
   QUPV3_1_GPII7,
   QUPV3_1_GPII8,
   QUPV3_1_GPII9,
   QUPV3_1_GPII10,
   QUPV3_1_GPII11,
   QUPV3_1_GPII12,
   QUPV3_1_GPII13,
   QUPV3_1_GPII14,
   QUPV3_1_GPII15,
   QUPV3_2_GPII0,
   QUPV3_2_GPII1,
   QUPV3_2_GPII2,
   QUPV3_2_GPII3,
   QUPV3_2_GPII4,
   QUPV3_2_GPII5,
   QUPV3_2_GPII6,
   QUPV3_2_GPII7,
   QUPV3_2_GPII8,
   QUPV3_2_GPII9,
   QUPV3_2_GPII10,
   QUPV3_2_GPII11,
   QUPV3_2_GPII12,
   QUPV3_2_GPII13,
   QUPV3_2_GPII14,
   QUPV3_2_GPII15,
   QUPV3_SSC_GPII0,
   QUPV3_SSC_GPII1,
   QUPV3_SSC_GPII2,
   QUPV3_SSC_GPII3,
   QUPV3_SSC_GPII4,
   QUPV3_SSC_GPII5,
   QUPV3_SSC_GPII6,
   QUPV3_SSC_GPII7,
   QUPV3_SSC_GPII8,
   QUPV3_SSC_GPII9,
   QUPV3_SSC_GPII10,
   QUPV3_SSC_GPII11,
   QUPV3_SSC_GPII12,
   QUPV3_SSC_GPII13,
   QUPV3_SSC_GPII14,
   QUPV3_SSC_GPII15,
   PERIPH_MAX_ID
} QUPV3_PERIPHID;

#define QUPV3_PROTOCOL_TO_HW(proto) ((proto)&0xf)

/** QUPv3 protocols */
typedef enum
{
  QUPV3_PROTOCOL_NONE = 0,
  QUPV3_PROTOCOL_SPI  = 1,
  QUPV3_PROTOCOL_UART = 2,
  QUPV3_PROTOCOL_UART_2W = QUPV3_PROTOCOL_UART,
  QUPV3_PROTOCOL_I2C  = 3,
  QUPV3_PROTOCOL_I3C  = 4,
  QUPV3_PROTOCOL_MAX,
  QUPV3_PROTOCOL_UART_4W = QUPV3_PROTOCOL_UART + 16,
  QUPV3_PROTOCOL_UINT32 = 0x7fffffff
} QUPv3_protocol_type;

/** QUPv3 FIFO/DMA access modes */
typedef enum
{
  QUPV3_MODE_FIFO = 0,
  QUPV3_MODE_CPU_DMA = 1,
  QUPV3_MODE_GSI = 2,
  QUPV3_MODE_MAX,
  QUPV3_MODE_UINT32 = 0x7fffffff
} QUPv3_mode_type;

/** QUPv3 serial engine configuration and permissions devcfg
    data structure */
typedef struct
{
  QUPV3_PERIPHID periph;        /* The SE peripheral to config and assign */
  QUPv3_protocol_type protocol; /* The protocol to use for this SE */
  QUPv3_mode_type mode;         /* The DMA/FIFO mode to use for this SE */
  uint32 uAC;                   /* Access control perms for this SE */
  boolean bAllowFifo;           /* Whether to allow FIFO mode for this SE */
  boolean bLoad;                /* Whether to load the firmware for this SE */
  boolean bModExclusive;        /* Whether TZ should have exclusive access when 
                                   a modify ownership operation has been 
                                   performed (TRUE), or whether only the GPIO
                                   ownership should be modified (FALSE) */
} QUPv3_se_security_permissions_type;

/** QUPv3 GPII permissions devcfg data structure  */
typedef struct
{
  QUPV3_PERIPHID periph;        /* The GPII peripheral to assign */
  uint32 uAC;                   /* The access control perms for this GPII */
  uint32 uGsiAC;                /* The GSI SID access control perms for this GPII */
} QUPv3_gpii_security_permissions_type;

/** QUPv3 serial engine hardware resource structure */
typedef struct
{
  QUPV3_PERIPHID periph;        /* The SE peripheral */
  uint8 *pBase;                 /* HWIO base address for this SE */
  const char *sClockName;       /* String name for the SE clock */
} QUPv3_se_resource_type;

/** QUPv3 GPII hardware resource structure */
typedef struct
{
  QUPV3_PERIPHID periph;        /* The GPII peripheral */
} QUPv3_gpii_resource_type;

/** QUPv3 common hardware resource structure  */
typedef struct
{
  uint8 *pBase;                 /* HWIO base address for this QUPv3 */
  const char *sDeviceName;      /* String name for the AC device (non-sec) */
  const char *sSecDeviceName;   /* String name for the AC device (sec) */
} QUPv3_common_resource_type;

/** Firmware data structure storage for devcfg */
typedef struct
{
   const void *pFwData;               /* Pointer to the firmware segment data struct */
   const uint32 FwDataSize;           /* Size of the firmware segment data struct */
} QUPv3_devcfg_fw_type;

#endif

