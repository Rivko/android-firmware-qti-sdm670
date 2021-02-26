#ifndef CUSTPMIC_H
#define CUSTPMIC_H
/*===========================================================================

DESCRIPTION
  Configuration for PMIC3 using MSM6250

  Copyright (c) 2010  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custpmic.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/19/11   dy      Readd FEATURE_PMIC_RTC
05/03/11   vk      Undefine PM_IMAGE_MODEM_PROC for BOOT IMAGE
04/20/11   hs      Removed obsolete features.
03/24/2011 vk      undef PM_TEST_MODE. All stripped features should be undef in the build
10/18/2010 mpt	   added #define PM_TEST_MODE	// This must be stripped
===========================================================================*/

/* -----------------------------------------------------------------------
** drivers/pm
** ----------------------------------------------------------------------- */

/* PMIC feature */
#define FEATURE_PMIC

/* Enable the PMIC real time clock */
#define FEATURE_PMIC_RTC                  

#define FEATURE_PMIC_32K_XTAL

#if defined(FEATURE_PMIC)

      /* Enables the PMIC TCXO management. It allows DMSS to turn TCXO ON/OFF by
      toggling the GPIO connected to PMIC TCXO_EN pin */
      #define FEATURE_PMIC_TCXO_CONTROL
    
      /* Enable the 32K external crystal. RTC will not work
      with the internal RC clock. */
      #define FEATURE_PMIC_32K_XTAL
    
      #define FEATURE_PMIC_INTERNAL_IC_TEMPERATURE
       
  /* not used in PMIC code, used by core/iodevices/hs/src/hs_mb6250.h */ 
      #define FEATURE_PMIC_LCDKBD_LED_DRIVER
     
      #define FEATURE_PMIC_PROCCOMM_SUPPORT
    
    /* used by sndhwg2.c */
      #undef FEATURE_PMIC_SPEAKER
    
    /* used by sndhwg2.c and vocoder */
      #undef FEATURE_PMIC_SPEAKER_ON_OFF_DELAY

    
#endif /* FEATURE_PMIC */
    

/*************************************************************************/
/************************* VBATT DRIVER SECTION **************************/
/*************************************************************************/

/* Enable the ADC functionality, required for filters */
#define FEATURE_ADC_FOR_VBATT_AVAILABLE 

#define FEATURE_PMIC_TARGET_MSM8660_ONLY

#endif /* CUSTPMIC_H */


