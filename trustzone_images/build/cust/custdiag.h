#ifndef CUSTDIAG_H
#define CUSTDIAG_H
/*===========================================================================

DESCRIPTION
  Configuration for DIAG.

  Copyright (c) 2010  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custdiag.h#1 $ $DateTime: 2018/02/06 03:02:41 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/23/10   mad     DIAG_IS2000 is not defined for IMAGE_APPS_PROC 
07/12/10   mad     Changes for apps-only diag bring-up 
06/22/10   sg      Moved the definitions of DIAGDIAG_START_STRESS_TEST_F to 
                   diagdiag_common.c
06/22/10   is      Added DIAG_VOC_TASK
06/10/10   is      Core image feature flag cleanup
06/03/10   JV      Added feature 'DIAG_DOG_SUPPORT'
06/02/10   vs      Added FEATURE_TOOLSDIAG_ESN
05/10/10   is      Added DIAG_CM_AVAIL
02/26/10   sg      Added DIAG_TX_SLEEP_THRESHOLD_DEFAULT
                   DIAG_TX_SLEEP_TIME_DEFAULT
02/24/06   vg      Added FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
02/24/06   hn      Removed old FEATURE_DIAG_PACKET_COUPLING
04/22/05   adm     Added FEATURE_SMD also for diagcomm_fwd compilation check
11/22/04    cr     Only compile diagcomm_fwd for multi-processor builds.
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/*---------------------------------------------------------------------------
                             Diagnostics
---------------------------------------------------------------------------*/

  /* ---------------------------------------------------------------------
   *                     Common Feature Flags
   * --------------------------------------------------------------------- */

#ifdef IMAGE_APPS_PROC
  #define DIAG_IMAGE_APPS_PROC
  #define DIAG_SIO_USB
  #define DIAG_APPS_ONLY_BRINGUP  /* New feature to identify APPS ONLY diag, as opposed to say Diag on single-proc targets */
  #define FEATURE_TOOLSDIAG_INIT
  #define DIAG_STANDALONE
#endif
  
#ifdef IMAGE_MODEM_PROC
  #define DIAG_IMAGE_MODEM_PROC
#endif  

#ifdef IMAGE_QDSP6_PROC
  #define DIAG_IMAGE_QDSP6_PROC
#endif
   
   
/* Enable diagnv */
#define FEATURE_DIAG_NV

/* Diag should check validity of address when doing memory ops.*/
#define FEATURE_DIAG_HW_ADDR_CHECK

/* Manually register dispatch tables. */
#define FEATURE_DIAG_MANUAL_DISPATCH_TABLE

#define FEATURE_DIAG_MP_MASTER_APPS

#if !defined(FEATURE_THIN_UI)
#define FEATURE_DIAG_8K_QDSP6
#endif

#if defined(IMAGE_MODEM_PROC)
  #define FEATURE_TOOLSDIAG_INIT
  #define FEATURE_TOOLSDIAG_ESN
#endif

#define DIAG_TX_SLEEP_THRESHOLD_DEFAULT (1024*16)

#define DIAG_TX_SLEEP_TIME_DEFAULT 10

#define DIAG_DOG_SUPPORT

  /* ---------------------------------------------------------------------
   *                    Core Image Feature Flags
   * --------------------------------------------------------------------- */
#ifdef FEATURE_COREIMG

  /* COREIMG Feature flags can be added here */

#else /* #ifdef FEATURE_COREIMG */

  /* ---------------------------------------------------------------------
   *                 Non-Core Image Feature Flags
   * --------------------------------------------------------------------- */

#if defined (FEATURE_IS2000) && !defined (IMAGE_APPS_PROC)
  #define DIAG_IS2000
#endif /* FEATURE_IS2000 */
    
#ifdef FEATURE_ADSP_DIAG
  #define FEATURE_DIAG_ADSP_CALLBACK
#endif /* FEATURE_ADSP_DIAG */

#if defined(IMAGE_MODEM_PROC) && defined(FEATURE_WCDMA)
  #define FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#endif

#if !defined(FEATURE_GSM_PLT) && !defined(FEATURE_HWTC) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_LTE_PLT)
  #define DIAG_CM_AVAIL
#endif 

#ifdef FEATURE_WCDMA
  #define DIAG_WCDMA
#endif

#ifdef FEATURE_GSM
  #define DIAG_GSM
#endif

#ifdef FEATURE_VOC_TASK
  #define DIAG_VOC_TASK
#endif

#endif /* #ifndef FEATURE_COREIMG */
#endif /* CUSTDIAG_H */
