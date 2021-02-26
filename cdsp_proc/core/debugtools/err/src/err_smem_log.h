#ifndef ERR_SMEM_LOG_H
#define ERR_SMEM_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     S M E M    L O G G I N G

GENERAL DESCRIPTION
  This module provides extended smem logging in case of fatal and 
  non-fatal errors.  This module is not a task, but rather a set of
  procedures which run in the context of the calling task.

Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_smem_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     File created for error extended smem logging

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


/*===========================================================================
			  FUNCTION PROTOTYPES
===========================================================================*/


/*===========================================================================
FUNCTION ERR_SMEM_CALCULATE_OFFSET

DESCRIPTION
  This function calculates the offset for USER PD to start writing to SMEM buffer from.

DEPENDENCIES
  Needs to be called only from USER PD.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void err_smem_calculate_offset(void);


/*===========================================================================
FUNCTION ERR_SMEM_WRITE_TIMESTAMP_STRING

DESCRIPTION
  Writes the timestamp string from the Root PD.

DEPENDENCIES
  Needs to be called only from Root PD.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void err_smem_write_timestamp_string(const char * engg_timestampstring_string,
                                     const char * qcom_timestampstring_string);

/*===========================================================================
FUNCTION ERR_SMEM_LOG_IMAGE_VERSION

DESCRIPTION
  Writes the image version information to the SMEM buffer from the Root PD.

DEPENDENCIES
  Needs to be called only from Root PD. 
  User PD may not have the image versioning available.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void err_smem_log_image_version(void);


/*===========================================================================
FUNCTION err_smem_log_init

DESCRIPTION
  This function initialized the smem log for writing extended smem info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
void err_smem_log_init (void);

#endif /* ERR_SMEM_LOG_H */

