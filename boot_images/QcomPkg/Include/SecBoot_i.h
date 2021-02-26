/** @file SecBoot_i.h

 
  Copyright (c) 2011-2012, 2015 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------
01/21/15   sk      Changed typedef for compilation
12/10/12   shl     Warning fix
12/12/11   shl     Initial version, ported from Brew       

============================================================================*/

/*

DESCRIPTION: This is the secure boot internal header file.

 Hierarchy of dependencies in files for the secure boot process

                      secboot.h (this file, the entry point for this API)
                          |
                      secboot.c--------- secboot_i.h/c (internal, helper
                                                        functions)
                          |
               +--------------+-------+----------------------+
               |                      |                      |
  secboot_x509.h/c       secboot_rsa_math.h/c        secboot_sha1.h/c
   ( x509 parser)       (RSA operations)        (SHA1 interfaces)
         |
         |
  secboot_asn1.h/c
   ( asn1 library)
*/


#ifndef SECBOOT_I_H_
#define SECBOOT_I_H_


//#define DEBUG

/* Define the platform here ... */
#if defined(WIN32) || defined(_WIN32)
  #define USE_WINDOWS 1
#else
  #define USE_ARM     1
  //#define USE_UNIX    1
#endif

#if !defined(USE_ARM) && !defined(USE_WINDOWS)
  #error "Must define platform"
#endif
// End of platform identification

/*===========================================================================

                              INCLUDE FILES

============================================================================*/

#ifdef USE_WINDOWS
  #include <stdlib.h> // debugging for printfs
  #include <stdio.h>
#endif

#ifdef USE_ARM
  /*
  #include "msg.h" // for MSG_LOW etc
  #include "comdef.h" // for common definitions like uint32 boolean etc
  */
  #include "com_dtypes.h"
#endif

#include "SecBoot_errno.h"
#include "SecBoot_types.h"
#include "SecBoot_util.h"
/*============================================================================
                               Definitions
=============================================================================*/
#define MEMSET  qmemset
#define MEMCPY  qmemcpy
#define MEMCMP  bByteCompare
#define MEMMOVE qmemcpy
#define BLOAD8  bLoad8
#define BSTOR8  bStor8

#ifdef USE_WINDOWS
  #define TRUE       1
  #define FALSE      0
  typedef unsigned __int64 uint64;
  typedef unsigned __int32 uint32;
  typedef unsigned __int16 uint16;
  typedef unsigned __int8  uint8;
  typedef signed __int64 int64;
  typedef signed __int32 int32;
  typedef signed __int16 int16;
  typedef signed __int8  int8;
//  typedef int boolean;
#else // its the ARM target, USE__ARM
  //sk: Removing below for compilation fix for 8996
  //typedef unsigned long long uint64;
#endif

/* we stick to a small size to save memory whereever possible */
#define NUM_CERTS            10

#define ATTEST_CERT_INDEX        0
#define CA_CERT_INDEX            1
#define ROOT_CERT_INDEX          2

// Macros and Hacks for ARM <--> Windows movement.
#ifdef USE_WINDOWS
  /* All the same */
  #define MSG_HIGH MSG_ERROR
  #define MSG_MED MSG_ERROR
  #define MSG_LOW MSG_ERROR
  #define MSG_ERROR(text, msg_param1, msg_param2, msg_param3) \
          { printf(text,msg_param1,msg_param2,msg_param3);printf("\n"); }
  #define MSG_FATAL(text, msg_param1, msg_param2, msg_param3) \
          { printf("\nFatal Error: Exiting ...\n"); printf(text, \
            msg_param1, msg_param2, msg_param3); exit(1); }
  /* Remove the following at compile time */
  #define ASSERT(exp)
  #define TRACE(x,y,z)
  #define LOCAL
#endif

/* In PBL we don't have MSG anymore ... so now removing them */
#ifdef USE_ARM
  /* All the same */
  #define MSG_HIGH MSG_ERROR
  #define MSG_MED MSG_ERROR
  #define MSG_LOW MSG_ERROR
  #define MSG_ERROR(text, msg_param1, msg_param2, msg_param3)
  #define MSG_FATAL(text, msg_param1, msg_param2, msg_param3)
  /* Remove the following at compile time */
//  #define ASSERT(exp)
//  #define TRACE(x,y,z)

#endif

#ifdef LOCAL
#undef LOCAL
#endif /* LOCAL */

#define LOCAL static

#endif
