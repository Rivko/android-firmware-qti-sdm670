/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic common routines

General Description
  This file contains routines that are common to both root PD and user PD.


Copyright (c) 2017 by QUALCOMM Technologies, Incorporated. 
All Rights Reserved. Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/common/src/diag_pd_common.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/17   ph      Tagging the data with dynamic diag IDs.
===========================================================================*/
#include "customer.h"

#include "comdef.h"
#include <stdio.h>
#include "stringl.h"
#include "diag_v.h"
#include "diagdiag_v.h"


/*===========================================================================
FUNCTION   diag_get_pd_name

DESCRIPTION
    This function retrieves the PD type for a given process name obtained by 
    service registry. On Successful call, current userPD [AUDIO/WLAN/SENSOR]
    is returned else DIAG_PD_NONE is returned.

DEPENDENCIES
    TMS API call should give the correct domain information "mdm/modem/audio_pd"

RETURN VALUE
    NONE

SIDE EFFECTS
    NONE
===========================================================================*/
diag_pd_type diag_get_pd_name(char* name )
{
  char lname[MAX_PNAME_LEN];
  char *token = NULL;
  char *saveptr = NULL;
  const char *delimter = "/\\";
  int i, size;
  diag_pd_type diag_pd = DIAG_PD_NONE;
  
  strlcpy(lname, name, MAX_PNAME_LEN);
  
  size = sizeof(diag_pd_table) / sizeof(diag_pd_table[0]);

  token = strtok_r(lname, delimter, &saveptr);
  while ( token != NULL )
  {
    for (i = 0; i < size; i++)
    {
      if ( strncmp(
                   diag_pd_table[i].pd,
                   token,
                   DIAG_MAX_PD_NAME_SIZE) == 0 )
      {
        diag_pd = diag_pd_table[i].pd_type;
        break;
      }
    }
    token = strtok_r(NULL, delimter, &saveptr);
  }
  
  return diag_pd;
 }