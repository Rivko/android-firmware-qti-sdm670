#ifndef CUSTSEC_H
#define CUSTSEC_H
/*===========================================================================

            " C u s t - S E C "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Secure Services Livrary.

  Copyright (c) 2001-2006  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custsec.h#1 $
$DateTime: 2018/02/06 03:02:41 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/11   ejt     Turned off FEATURE_SEC_SSL_TESTAPP
09/03/10   ejt     Original file

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                        Security Services Modules
---------------------------------------------------------------------------*/

/* General cryptographic library */
#define FEATURE_SEC

/* IPSec protocol for IPv4 and VPN remote access client */

#ifdef IMAGE_MODEM_PROC
   /* secrnd task */
   #define FEATURE_SECRND
#endif

/* SFS */
#define FEATURE_SEC_SFS

/* SSL */
#define FEATURE_SEC_SSL

/* Turn it on when smetest is checked in for 8960 target */
//#define FEATURE_SEC_TESTAPPS

/*------------------------------*/
/* SEC Services Initialization  */  
/*------------------------------*/

//#define FEATURE_IXTIMESVC

//#define FEATURE_IXFILESVC

/*---------------------------------------------------------------------------
                    Cryptographic library configuration
---------------------------------------------------------------------------*/

#ifdef FEATURE_SEC

  /* Enable X.509 certificate handling */
  #define FEATURE_SEC_X509

  /* Enable storage of root certs in EFS */
  #define FEATURE_SEC_EFS_CA

  /* ARM implementations of the algorithms */
  #define FEATURE_SEC_ARM_CRYPTO

  #ifdef IMAGE_MODEM_PROC
   /* HW implementations of the algorithms */
   #define FEATURE_SEC_HW_CRYPTO
  #endif

  /* Enabled for CMI builds */
  #define FEATURE_SEC_CMI

  #define FEATURE_SEC_NO_HW_KEY_READ

  /* ---------------------------------------------------- */
  /* ARM platform-specific comfiguration                  */
  /* ---------------------------------------------------- */
  #ifdef FEATURE_SEC_ARM_CRYPTO
  #endif

  /* Crypto test app */
  #ifdef FEATURE_SEC_TESTAPPS
  #define SECAPITEST_TIMETEST
  #define FEATURE_SEC_CRYPT_TESTAPP
  #ifdef FEATURE_SEC_SSL
//#define FEATURE_SEC_SSL_TESTAPP
  #endif
  #else
  #undef FEATURE_SEC_CRYPT_TESTAPP
  #endif

  /* SEC test diag event reporting support */
  #define FEATURE_SECTEST_DIAG_EVENTS

  /* SEC crypto layer diag event reporting support */
  #undef FEATURE_SECCRYPT_DIAG_EVENTS

#endif  /*FEATURE_SEC */

#ifdef FEATURE_SEC_SFS
#include "custsfs.h"
#endif

#ifdef FEATURE_SEC_IPSEC
#include "custipsec.h"
#endif

#endif /* CUSTSEC_H */
