#ifndef DALGPIOMgrINTERFACE_H
#define DALGPIOMgrINTERFACE_H
/*==============================================================================

FILE:      GPIOMgrInterface.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright © 2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*------------------------------------------------------------------------------
Include files
------------------------------------------------------------------------------*/

#include "DDIGPIOInt.h"

/*------------------------------------------------------------------------------
Defining Internal Structures
------------------------------------------------------------------------------*/

#define GPIOMGR_PD_NONE     0x7F
#define GPIOMGR_UNUSED_GPIO 0x7FFF

#define GPIOMGR_FLAG_INUSE    0x01
#define GPIOMGR_FLAG_RESERVED   0x10

/**
  This enum is used to define the destination subsystem for the 
  GPIO Manager. This is required in order to handle subsystems 
  which support multiple types of GPIOs, so we want to make sure 
  that the correct direct connects are used. 
 */
typedef enum{
  GPIOMGR_DEVICE_MODEM = GPIOINT_DEVICE_MODEM,     /**< The Modem device is usually the QDSP6 processor. */
  GPIOMGR_DEVICE_SPS = GPIOINT_DEVICE_SPS,         /**< The SPS device usually is the Sensors processor. */
  GPIOMGR_DEVICE_LPA_DSP = GPIOINT_DEVICE_LPA_DSP, /**< The LPA DSP device is usually the LPASS processor */
  GPIOMGR_DEVICE_RPM = GPIOINT_DEVICE_RPM,         /**< The RPM device usually is the resource power manager processor */
  GPIOMGR_DEVICE_APPS = GPIOINT_DEVICE_APPS,       /**< The APPS device usually is the applications processor */
  GPIOMGR_DEVICE_WCN = GPIOINT_DEVICE_WCN,         /**< The WCN device usually is the RIVA processor */
  GPIOMGR_DEVICE_DSP = GPIOINT_DEVICE_DSP,         /**< On legacy 9k devices the DSP is the modem processor */
  GPIOMGR_DEVICE_TURING = GPIOINT_DEVICE_TURING,   /**< The Turing device is the computer DSP processor. */
  GPIOMGR_DEVICE_NONE = GPIOINT_DEVICE_NONE,       /**< The default target processor value. */
/** @cond */
  PLACEHOLDER_GPIOMgrProcessorType = 0x7f
/** @endcond */
}GPIOMgrProcessorType;


typedef struct
{
  uint16 nDirConnID;
  uint16 nInterruptID;
  uint16 nGPIO; /* GetDirConn */
  uint16 nReservedPD;
}
GPIOMgrConfigMapType;

typedef struct
{
  uint32 nPD;
  uint32 nTargetProc;
}
GPIOMgrConfigPDType;


#endif /*DALGPIOMgrINTERFACE_H*/


