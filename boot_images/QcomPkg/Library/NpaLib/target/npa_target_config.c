/*=============================================================================
@file npa_target_config.c

NPA target configuration

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

=============================================================================*/
#include "npa_config.h"

#define REQUEST_THREAD_PRIORITY 236
#define SCHEDULER_THREAD_PRIORITY 237

#ifdef __cplusplus
  #define EXTERN_CONST extern const
  extern "C" {
#else
  #define EXTERN_CONST const
#endif

EXTERN_CONST npa_config npa_config_data =
{
  32768,                        /* Log Buffer Size */

  /* pool configuration */
  { 30, 10 },                   /* resources */
  { 100, 10 },                  /* clients */
  { 10, 10 },                   /* events */
  { 30, 10 },                   /* links */
  { 50, 10 },                   /* event queue elements */
  { 32, 16 },                   /* list elements */

  /* Workloop configuration */
  { REQUEST_THREAD_PRIORITY, 32 }, /* Async Request Workloop */
  { 0,  1 },                       /* Async Event Workloop */
  &npa_config_data /* table end - points to self */
};

/* Holder for Resource Override Data.  Uncomment and fill
 * in if there is a need to override defaults.
 */
EXTERN_CONST npa_resource_config npa_resource_overrides_data[] = 
{
  /* name, request, fork, join */
  { (const char*)0, { 0, 0, 0 } }  /* Always last entry */
};

#ifdef __cplusplus
}
#endif
