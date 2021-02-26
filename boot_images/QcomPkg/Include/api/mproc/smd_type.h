#ifndef SMD_TYPE_H
#define SMD_TYPE_H

/**
 * @file smd_type.h
 *
 * Public data types and definitions used by SMD
 */

/*==============================================================================
     Copyright (c) 2012 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/12   pa      Added SMD_STATUS_SIZE_MISMATCH.
08/22/12   bt      Move SMD error codes to smd_type.h.
01/20/12   bm      Add PROC(n)_RPM edges.
10/31/11   bm      Moved SMD_CHANNEL_NAME_SIZE_MAX to smd_type.h to 
                   expose it as a public macro.
05/03/11   tl      Created separate header file for SMD data types
===========================================================================*/

/*-------------------------------------------------------------------------
  Defines
-------------------------------------------------------------------------*/

/** @addtogroup smd
@{ */
/** SMD_CHANNEL_NAME_SIZE_MAX must be a multiple of 4 for structure
   alignment in shared memory. */ 
#define SMD_CHANNEL_NAME_SIZE_MAX    20

/** Standard size of a small FIFO buffer (1 KB). */
#define SMD_MIN_FIFO       (1 * 1024) 

/** Default FIFO size for an SMD buffer (8 KB). */
#define SMD_STANDARD_FIFO  (8 * 1024)

/** Standard size of a large FIFO buffer (128 KB). This is the largest size 
  permitted by the SMD. */
#define SMD_MAX_FIFO       (128 * 1024)

/** @name API Return Error Codes
  These are the error codes returned by some SMD public APIs, and their 
  meaning, which the client can use to determine the success of the function, 
  or what went wrong.
@{ */
#define SMD_STATUS_SUCCESS        0   /**< The function succeeded. */
#define SMD_STATUS_ERROR          -1  /**< Generic error. */
#define SMD_STATUS_INVALID_PARAM  -2  /**< Invalid argument passed in. */
#define SMD_STATUS_NOT_INIT       -3  /**< SMD has not been initialized enough 
                                           to handle this command. */
#define SMD_STATUS_SIZE_MISMATCH  -4  /**< Requested port size does not match
                                           existing port size */
/** @} */ /* end_name_group API Return Error Codes */

/*--------------------------------------------------------------------------
  Enumerated types
---------------------------------------------------------------------------*/

/* smd_channel_type */
/** SMD channel connection directions.
*/
typedef enum
{
  SMD_CHANNEL_TYPE_FIRST,                    /**< &nbsp; */
  SMD_APPS_MODEM = SMD_CHANNEL_TYPE_FIRST,   /**< &nbsp; */
  SMD_APPS_QDSP,                             /**< &nbsp; */
  SMD_MODEM_QDSP,                            /**< &nbsp; */
  SMD_APPS_DSPS,                             /**< &nbsp; */
  SMD_MODEM_DSPS,                            /**< &nbsp; */
  SMD_QDSP_DSPS,                             /**< &nbsp; */
  SMD_APPS_RIVA,                             /**< &nbsp; */
  SMD_MODEM_RIVA,                            /**< &nbsp; */
  SMD_QDSP_RIVA,                             /**< &nbsp; */
  SMD_DSPS_RIVA,                             /**< &nbsp; */
  SMD_APPS_Q6FW,                             /**< &nbsp; */
  SMD_MODEM_Q6FW,                            /**< &nbsp; */
  SMD_QDSP_Q6FW,                             /**< &nbsp; */
  SMD_DSPS_Q6FW,                             /**< &nbsp; */
  SMD_RIVA_Q6FW,                             /**< &nbsp; */
  SMD_APPS_RPM,                              /**< &nbsp; */
  SMD_MODEM_RPM,                             /**< &nbsp; */
  SMD_QDSP_RPM,                              /**< &nbsp; */
  SMD_RIVA_RPM,                              /**< &nbsp; */
  SMD_CHANNEL_TYPE_LAST,                     /**< &nbsp; */
  SMD_INVALID_CHANNEL_TYPE                   /**< &nbsp; */
} smd_channel_type;

/** @} */ /* end_addtogroup smd */

#endif /* SMD_TYPE_H */
