/**
  @file bamconfig_props.c
  @brief
  This file contains implementation of the interfaces that provide
  target specific base addresses & irq vector data to the BAM driver.

*/
/*
===============================================================================

                             Edit History

 $Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/22/14   SS      Cleaned up for 8996
10/28/13   SS      Added 8994 support
12/04/12   SS      Added tgtcfg ptr to support binary compatibility
08/24/12   NK      Fix warnings and added dal config image detection 
04/15/11   SS      Created

===============================================================================
                   Copyright (c) 2012-2016 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
/* If DAL_APPS_IMAGE set the local define for the appropriate config */
#ifdef DAL_CONFIG_IMAGE_APPS
#ifndef BAM_APPS_CONFIG
#define BAM_APPS_CONFIG
#endif
#endif

#include "bamtgtcfg.h"
#ifdef BAM_XML_PROPS
#ifdef BAM_ADSP_CONFIG
#include "bamtgtcfgdata_lpass.h"
#elif BAM_MODEM_CONFIG
#include "bamtgtcfgdata_mpss.h"
#elif BAM_TZOS_CONFIG
#include "bamtgtcfgdata.h"
#else
#error "Invalid header file included"
#endif
#endif


