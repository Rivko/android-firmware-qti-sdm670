/*=============================================================================

                err_qdi.h  --  Header File

GENERAL DESCRIPTION
      ERR QDI Layer Header File

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_qdi.h#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
007/31/14   din     File created to support Error on User PD.
=============================================================================*/
#ifndef ERR_QDI_H
#define ERR_QDI_H

#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt.h"
#include "comdef.h"
#include "erri.h"
#include "erri_sizes.h"
#include "DDITimetick.h"
#include "timer.h"


#define ERR_DRIVER_NAME                    "/dev/err_qdi_pd"

#define ERR_QDI_SEND_PDR_INFO              (0+QDI_PRIVATE)
#define ERR_QDI_INFORM_PFR                 (1+QDI_PRIVATE)
#define ERR_QDI_ERR_HANDLING_DONE          (2+QDI_PRIVATE)
#define ERR_QDI_COMMUNICATE_USER_CB        (3+QDI_PRIVATE) 

#define ERR_QDI_DEVICE_NAME_LEN            QURT_MAX_NAME_LEN

typedef enum
{
  ERR_QDI_USERPD_NOT_INITIALIZED = 0,
  ERR_QDI_USERPD_INITIALIZED,
  ERR_QDI_USERPD_ERR_INITIATED,
  ERR_QDI_USERPD_ERR_HANDLING_COMPLETED,
  ERR_QDI_USERPD_ERR_PD_EXIT_STARTED,
  ERR_QDI_USERPD_ERR_PD_EXIT_COMPLETED
} err_qdi_userpd_state_e;


typedef struct
{
  void                            *cb;
  DalTimetickTime64Type           start_tick;
  DalTimetickTime64Type           end_tick;
} err_qdi_cb_bucket_user_pd_t;

typedef struct
{
  void                            *stack_addr;
  uint32                          stack_size;
  err_qdi_cb_bucket_user_pd_t     user_cb[ERRCB_BUCKET_MAX];
}err_qdi_cb_user_pd_t;

typedef struct err_qdi_internal_struct
{
  int                     client_handle;
  char                    pd_name[ERR_QDI_DEVICE_NAME_LEN];
  uint32                  pid;
  char                    pfr[ERR_QDI_PFR_LEN];
  uint32                  pfr_len;
  DalTimetickTime64Type   start_time;
  DalTimetickTime64Type   end_time;
  void *                  pdr_record_data;
  err_qdi_userpd_state_e  state;
  timer_type              pdtimeout_timer;
  err_qdi_cb_user_pd_t    user_cb;
  boolean                 pdtimeout_timer_set;
  boolean                 pdr_enabled;
} err_qdi_internal_s;

typedef enum err_qdi_enum {
  ERR_QDI_SUCCESS = 0,
  ERR_QDI_FAIL,
  ERR_QDI_INVALID_PARAMETERS,
  ERR_QDI_NOT_INITIALIZED
}err_qdi_e;

#endif /* ERR_QDI_H */
