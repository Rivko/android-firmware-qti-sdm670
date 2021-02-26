//===========================================================================
//
// FILE:         QUPAC_670_Access.xml
//
// DESCRIPTION:  This file lists access permission for all QUPS
//
//===========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.tz/2.0.2/settings/buses/qup_accesscontrol/qupv3/config/670/QUPAC_Access.c#3 $
//
// when       who     what, where, why
// 11/11/19   PCR     Added suppor for sheldon platform
// 05/13/19   NM      Change Touch Instance(QUPV3_1_SE1) FIFO Mode.
// 11/06/17   SKA     Change QUP1 SE1 to GSI mode for Secure touch use case.
// 10/09/17   MJS     Give trustzone option for exclusive access during use cases.
// 09/01/17   DPK     Initial revision for SDM670
//
//===========================================================================
//             Copyright (c) 2017,2019 QUALCOMM Technologies, Incorporated.
//                    All Rights Reserved.
//                QUALCOMM Confidential & Proprietary
//===========================================================================
#include "QupACCommonIds.h"

/* OEMs are expected to modify this .c to suit their board design. The uAC 
   specifies the owners of the SE resource. It is initially populated
   according to System IO GPIO allocation */

//All SEs have to be listed below. Any SE not present cannot be accesssed by any subsystem. 
//It's designed to be flexible enough to list only available SEs on a particular platform.

const QUPv3_se_security_permissions_type qupv3_perms_default[] =
{
  /*   PeriphID,         ProtocolID,               Mode, NsOwner,bAllowFifo,bLoad,bModExcl  */
  { QUPV3_0_SE0, QUPV3_PROTOCOL_SPI,     QUPV3_MODE_GSI, AC_TZ,       FALSE, TRUE, TRUE }, // NFC eSE
  /*QUPV3_0_SE1*/
  /*QUPV3_0_SE2*/
  { QUPV3_0_SE3, QUPV3_PROTOCOL_I2C,     QUPV3_MODE_GSI, AC_HLOS,      TRUE, TRUE,FALSE }, // NFC 
  /*QUPV3_0_SE4*/
  /*QUPV3_0_SE5*/
  { QUPV3_0_SE6, QUPV3_PROTOCOL_UART_4W, QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // Cherokee BT HCI
  /*QUPV3_0_SE7*/
  { QUPV3_1_SE0, QUPV3_PROTOCOL_SPI,     QUPV3_MODE_GSI, AC_HLOS,      TRUE, TRUE,FALSE }, // WCD9340
  { QUPV3_1_SE1, QUPV3_PROTOCOL_I2C,     QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // Legacy touch
  { QUPV3_1_SE2, QUPV3_PROTOCOL_I2C,     QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // Haptics (sensors hub)
  /*QUPV3_1_SE3*/
  { QUPV3_1_SE4, QUPV3_PROTOCOL_UART_2W, QUPV3_MODE_FIFO,AC_HLOS,      TRUE,FALSE,FALSE }, // Debug
  /*QUPV3_1_SE5*/
  /*QUPV3_1_SE6*/
  { QUPV3_1_SE7, QUPV3_PROTOCOL_SPI,     QUPV3_MODE_GSI, AC_TZ,       FALSE, TRUE, TRUE }, // Fingerprint
};

const uint32 qupv3_perms_size_default = sizeof(qupv3_perms_default)/sizeof(qupv3_perms_default[0]);

const QUPv3_se_security_permissions_type qupv3_perms_sheldon[] =
{
  /*   PeriphID,         ProtocolID,               Mode, NsOwner,bAllowFifo,bLoad,bModExcl  */
  /*QUPV3_0_SE0*/
  /*QUPV3_0_SE1*/
  /*QUPV3_0_SE2*/
  /*QUPV3_0_SE3*/
  /*QUPV3_0_SE4*/
  /*QUPV3_0_SE5*/
  { QUPV3_0_SE6, QUPV3_PROTOCOL_UART_4W, QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // Cherokee BT HCI
  /*QUPV3_0_SE7*/
  { QUPV3_1_SE0, QUPV3_PROTOCOL_SPI,     QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // WCD9340
  { QUPV3_1_SE1, QUPV3_PROTOCOL_I2C,     QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // TUSB1064 DP switch 
  { QUPV3_1_SE2, QUPV3_PROTOCOL_I2C,     QUPV3_MODE_FIFO,AC_HLOS,      TRUE, TRUE,FALSE }, // LP4x BUCK CTRL I2C
  /*QUPV3_1_SE3*/
  { QUPV3_1_SE4, QUPV3_PROTOCOL_UART_2W, QUPV3_MODE_FIFO,AC_HLOS,      TRUE,FALSE,FALSE }, // Debug
  /*QUPV3_1_SE5*/
  /*QUPV3_1_SE6*/
  /*QUPV3_1_SE7*/
};

const uint32 qupv3_perms_size_sheldon = sizeof(qupv3_perms_sheldon)/sizeof(qupv3_perms_sheldon[0]);


const QUPv3_se_security_permissions_type qupv3_perms_rumi[] =
{
  /*   PeriphID,         ProtocolID,            Mode,    NsOwner,bAllowFifo,bLoad  */
  /*QUPV3_0_SE0*/
  /*QUPV3_0_SE1*/
  /*QUPV3_0_SE2*/
  /*QUPV3_0_SE3*/
  /*QUPV3_0_SE4*/
  /*QUPV3_0_SE5*/
  /*QUPV3_0_SE6*/
  /*QUPV3_0_SE7*/
  /*QUPV3_1_SE0*/
  { QUPV3_1_SE1, QUPV3_PROTOCOL_UART_2W, QUPV3_MODE_GSI, AC_HLOS,      FALSE,FALSE }, // Debug
  { QUPV3_1_SE2, QUPV3_PROTOCOL_SPI,     QUPV3_MODE_GSI, AC_HLOS,      TRUE, TRUE }, // Flash
  /*QUPV3_1_SE3*/
  /*QUPV3_1_SE4*/
  { QUPV3_1_SE5, QUPV3_PROTOCOL_I2C,     QUPV3_MODE_GSI, AC_HLOS,      TRUE, TRUE }, // Haptics (sensors hub)
  /*QUPV3_1_SE6*/
  /*QUPV3_1_SE7*/
};

const uint32 qupv3_perms_size_rumi = sizeof(qupv3_perms_rumi)/sizeof(qupv3_perms_rumi[0]);

const QUPv3_gpii_security_permissions_type qupv3_gpii_perms[] =
{
  { QUPV3_0_GPII0,  AC_ADSP_Q6_ELF },
  { QUPV3_0_GPII1,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_0_GPII2,  AC_ADSP_Q6_ELF },
  { QUPV3_0_GPII3,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_0_GPII4,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_0_GPII5,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_0_GPII6,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_0_GPII7,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_0_GPII8,  AC_TZ },
  { QUPV3_0_GPII9,  AC_TZ },
  { QUPV3_0_GPII10, AC_ADSP_Q6_ELF },
  { QUPV3_0_GPII11, AC_ADSP_Q6_ELF },
  { QUPV3_0_GPII12, AC_MSS_MSA }, 
  { QUPV3_1_GPII0,  AC_ADSP_Q6_ELF },
  { QUPV3_1_GPII1,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_1_GPII2,  AC_ADSP_Q6_ELF },
  { QUPV3_1_GPII3,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_1_GPII4,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_1_GPII5,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_1_GPII6,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_1_GPII7,  AC_HLOS,        AC_HLOS_GSI },
  { QUPV3_1_GPII8,  AC_TZ },
  { QUPV3_1_GPII9,  AC_TZ },
  { QUPV3_1_GPII10, AC_ADSP_Q6_ELF },
  { QUPV3_1_GPII11, AC_ADSP_Q6_ELF },
  { QUPV3_1_GPII12, AC_MSS_MSA },
};

const uint32 qupv3_gpii_perms_size = sizeof(qupv3_gpii_perms)/sizeof(qupv3_gpii_perms[0]);


