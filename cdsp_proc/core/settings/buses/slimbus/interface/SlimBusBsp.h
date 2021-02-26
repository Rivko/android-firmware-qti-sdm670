#ifndef SLIMBUSBSP_H
#define SLIMBUSBSP_H
/*
===========================================================================

FILE:         SlimBusBsp.h

DESCRIPTION:  This file defines data structures and macros used by 
              the SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/settings/buses/slimbus/interface/SlimBusBsp.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/15/17   DPK     Corrected the usage of the variable type.
08/31/17   DPK     Support to know which tlmm hold the slimbus gpio lines
02/07/17   MJS     Initial revision.

===========================================================================
             Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

//#include "ddislimbus.h"

#define SLIMBUS_ENUM_ADDR_BUF_SIZE 6

#define SB_MAX_GPIOS          4
#define SB_MAX_MASTER_PORTS  24
#define SB_MAX_CLK_GEAR      10

typedef uint8 SlimBusLogicalAddrType;

typedef enum
{
  SB_VOLTAGE_MIN = 0,
  SB_VOLTAGE_SVS2 = 1,
  SB_VOLTAGE_SVS = 2,
  SB_VOLTAGE_NOMINAL = 3
} SlimBusVoltageType;

typedef struct
{
  /* Structure version */
  uint32 uStructVer;
  /* String name for this driver instance */
  const char *pszInstName;
  /* Base enumeration address for the master device */
  uint8 uaMasterEA[SLIMBUS_ENUM_ADDR_BUF_SIZE];
  /* HWIO base address string */
  char *pszHwioBase;
  /* Offset from the HWIO base address */
  uint32 uHwioBaseOffset;
  /* HWIO base physical address */
  uint32 uHwioBase;
  /* Handle to the BAM device handle */
  uint32 hBamDev;
  /* The interrupt ID */
  uint32 uIntId;
  /* The BAM interrupt ID */
  uint32 uBamIntId;
  /* The EE number for this driver */
  uint32 uMyEE;
  /* The GPIOs to configure */
  uint32 uaGpioConfig[SB_MAX_GPIOS];
  /* The GPIO interrupt number to configure */
  uint32 uGpioIntNum;
  /* Array specifying the number of endpoints per port */
  uint8 uaNumEndPoints[SB_MAX_MASTER_PORTS];
  /* Array mapping clock gear to required voltage vote */
  /* Note: to simplify things, array starts at CG zero instead of one,
           in case unsigned int value of 0 is ever used (even though
           it is not valid), and to prevent need to do +1/-1 to the 
           array index */
  SlimBusVoltageType uaVoltageVote[SB_MAX_CLK_GEAR+1];
  /* To know the SLIMBUS GPIO lines are part of LPI TLMM */
  boolean bIsLpiTlmm;  
} SlimBusBSPType;

typedef struct 
{
    SlimBusLogicalAddrType LA;
    uint8 uaEA[SLIMBUS_ENUM_ADDR_BUF_SIZE];
    uint8 uDataLineMask;
} SlimBusDeviceDalProps;


#endif /* SLIMBUSBSP_H */

