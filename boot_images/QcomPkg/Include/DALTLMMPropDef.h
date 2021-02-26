#ifndef __DALTLMMPROPDEF_H__
#define __DALTLMMPROPDEF_H__
/*
============================================================================

                 D A L T L M M   S T A T E   S T R U C T U R E
                             H E A D E R   F I L E

FILE:         DALTLMMDefines.h

DESCRIPTION:  
              This file contains definitions for use solely with the
              DALTLMM Properties XML file.  It's use is not recommended by
              any drivers and must not be used in any DALTLMM APIs.

============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/30/10   dcf     Created the initial revision. 
 
============================================================================
             Copyright (c) 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
*/


/*==========================================================================

                     DEFINITIONS FOR THIS MODULE

==========================================================================*/



/*
 * The following definitions are used solely in the TLMM.xml files to 
 * set up initialization configuration for each GPIO.  Definitions are 
 * based on the DAL_GPIO_CFG_OUT() macro in the DDITlmm.h file.  The below 
 * definitions incorporate the shifted values from this macro in order 
 * to accommodate the XML preprocessor mechanism for DAL Properties. 
 *  
 * These are based on DAL_GPIO_VERSION defined as 0x20000000 with 
 * DALTLMM_INTERFACE_VERSION defined as DALINTERFACE_VERSION(1,1) 
*/


#define DALTLMM_INPUT            0          /* Defines 0 for direction INPUT */
#define DALTLMM_OUTPUT           0x4000     /* Defines 1 for direction OUTPUT */
#define DALTLMM_NO_PULL          0          /* Defines 0 for NO-PULL */
#define DALTLMM_PULL_DOWN        0x8000     /* Defines 1 for PULL-DOWN */
#define DALTLMM_KEEPER           0x10000    /* Defines 2 for KEEPER */
#define DALTLMM_PULL_UP          0x18000    /* Defines 3 for PULL-UP */
#define DALTLMM_OUT_LOW          0x400000   /* OUTPUT LOW value in HAL format */
#define DALTLMM_OUT_HIGH         0x600000   /* OUTPUT HIGH value in HAL format */
#define DALTLMM_PRG_YES          0x10000000 /* Program this GPIO */
#define DALTLMM_PRG_NO           0x0        /* Do not program this GPIO. */

#endif /* __DALTLMMPROPDEF_H__ */

