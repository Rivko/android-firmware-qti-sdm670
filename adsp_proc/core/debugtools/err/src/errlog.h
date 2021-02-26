#ifndef ERRLOG_H
#define ERRLOG_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  E R R

DESCRIPTION
  This header file contains the definitions of log structure for core dump
 
Copyright (c) 2007 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/errlog.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
07/25/07   tbg     Initial revision (pulled code from rexlog.h)

===========================================================================*/


/* -----------------------------------------------------------------------
** Includes 
** ----------------------------------------------------------------------- */
#include "comdef.h"
#ifdef ERR_HW_ARMV8
#include "errlog_armv8.h"
#elif ERR_HW_QDSP6
#include "errlog_q6.h"
#endif 

#include "DDITimetick.h"
#include "err.h"

#include "erri_sizes.h"   /* For err specific size Macros */
#include "err_crumb_trail_mask.h"
#include "err_qurt.h"
#include "tms_utils_msg.h"

/* -----------------------------------------------------------------------
** MACROs
** ----------------------------------------------------------------------- */

#ifndef QURT_THREAD_ATTR_NAME_MAXLEN
#define QURT_THREAD_ATTR_NAME_MAXLEN 16
#endif /* QURT_THREAD_ATTR_NAME_MAXLEN */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

typedef enum
{
  ERR_ARCH_UNKNOWN = 0,
  ERR_ARCH_ARM,
  ERR_ARCH_QDSP6,
  SIZEOF_ERR_ARCH_TYPE
} err_arch_type;

typedef enum
{
  ERR_OS_UNKNOWN = 0,
  ERR_OS_REX,
  ERR_OS_L4, 
  ERR_OS_BLAST, 
  ERR_OS_QURT, 
  SIZEOF_ERR_OS_TYPE
} err_os_type;

#ifndef UINT16_MAX
#define UINT16_MAX 0xFFFF
#endif /* UINT16_MAX */

/* Defines for stack undeflow /overflow */
typedef enum
{
  STACK_NO_ERROR      = 1<<0,
  STACK_OVERFLOW      = 1<<1,
  STACK_UNDERFLOW     = 1<<2,
  STACK_ERR_NO_INFO   = 1<<3,
  STACK_MAX_SIZE      = UINT16_MAX
} err_stack_type;

#define NUM_QDSP6_GP_REGS 32

typedef struct
{
  err_arch_type type;
  uint32 version;
  union arch_coredump_union regs;
} arch_coredump_type;

/* update this version whenever QDSP6 arch_coredump_type changes */
#define ERR_ARCH_COREDUMP_VER  2
#define ERR_ARCH_COREDUMP_TYPE ERR_ARCH_QDSP6
#define ERR_ARCH_QDSP_INUSE
 
/************************************************************************
 *                        OS_COREDUMP_TYPES
 ************************************************************************/

/****************
 *    QURT
 ****************/

#define ERR_OS_TCB_TYPE void

typedef struct
{
  err_os_type type;
  uint32 version;  
  ERR_OS_TCB_TYPE* tcb_ptr; 
} os_coredump_type;

/* update this version whenever L4 os_coredump_type changes */
#define ERR_OS_COREDUMP_VER  1
#define ERR_OS_COREDUMP_TYPE ERR_OS_QURT

/************************************************************************
 *                         ERR_COREDUMP_TYPE
 ************************************************************************/

#define ERR_LOG_NUM_PARAMS 3

typedef struct
{
  err_cb_info_t         info;
  DalTimetickTime64Type start_time;
} err_cb_external_type;

typedef struct
{
  char qc_image_version_string[ERR_IMAGE_VERSION_SIZE];
  char image_variant_string[ERR_IMAGE_VERSION_SIZE];
  char oem_image_version_string[ERR_IMAGE_VERSION_SIZE];
  char oem_image_uuid_string[ERR_IMAGE_VERSION_SIZE];
} image_coredump_type;

typedef struct
{
  uint32                            version;
  uint32                            linenum;
  uint64                            timestamp;  // Not used, remove it in next version
  uint64                            uptime;
  DalTimetickTime64Type             err_handler_start_time;
  DalTimetickTime64Type             err_handler_end_time;
  char                              filename[ERR_LOG_MAX_FILE_LEN];
  char                              message[ERR_LOG_MAX_MSG_LEN];
  uint32                            param[ERR_LOG_NUM_PARAMS];
  char                              aux_msg[ERR_LOG_MAX_MSG_LEN];
  char                              int_msg[ERR_LOG_MAX_MSG_LEN];
  err_cb_external_type              err_cb[ERR_CB_MAX];
  void*                             err_cb_current;
  err_stack_type                    stack_error;
  uint32                            crumb_trail_bmsk;
  const err_const_type *            compressed_ptr;
  unsigned int                      tid;
  unsigned int                      pd_id;
  char                              tcb_name[QURT_THREAD_ATTR_NAME_MAXLEN];
  boolean                           is_exception;
} err_coredump_type;

/* update this version whenever err_coredump_type changes */
#define ERR_COREDUMP_VER  6

/************************************************************************
 *                           COREDUMP_TYPE
 ************************************************************************/


typedef struct
{
  uint32 version;
  arch_coredump_type arch;
  os_coredump_type os;
  err_coredump_type err;
  image_coredump_type image;
} coredump_type;

 /* update this version whenever coredump_type changes */
#define ERR_COREDUMP_VERSION  2
 
/************************************************************************
 *                           FATAL_PARAM_TYPE
 ************************************************************************/
typedef unsigned long err_param_t;

typedef struct {
  unsigned int                      line;    
  const char *                      file_name; 
  const char *                      format;
  err_param_t                       param1;
  err_param_t                       param2;
  err_param_t                       param3;   
  /* orignal (compressed) msg_const */
  const                             err_const_type* msg_const_ptr; 
  uint32                            array[SIZEOF_ARCH_COREDUMP_REGISTERS];
  uint32                            crumb_trail_bmsk;
  ERR_MUTEX_TYPE                    err_fatal_mutex;
  char                              aux_msg[ERR_LOG_MAX_MSG_LEN];
  boolean                           err_fatal_mutex_init;
  boolean                           err_services_init_complete;
  boolean                           err_reentrancy_flag;
} err_fatal_params_type;

/*===========================================================================
FUNCTION ERR_LOG_INIT

DESCRIPTION
  Initialize buffer to be used for writing log information.
  Calling the function subsequent times will reset the location and size
  of the buffer.

PARAMETERS
  char *buff     Pointer to buffer
  uint32 size    Size of buffer

RETURNS
 
============================================================================*/
void err_log_init(char *buff, uint32 size);

/*===========================================================================
FUNCTION ERR_GENERATE_LOG

DESCRIPTION
  This is the function called by error services when a log report needs to
  be generated.

  err_log_init() must be called prior to calling this function.

PARAMETERS
  None

RETURNS 
  uint32          Number of bytes written to buffer.

============================================================================*/

uint32 err_generate_log(void);

/*===========================================================================
MACRO ERR_CRUMB_TRAIL_BMSK

DESCRIPTION
  Used to log state of err_handling

PARAMETERS
  None

RETURNS 
  uint32          Number of bytes written to buffer.

============================================================================*/
extern coredump_type coredump;
extern err_fatal_params_type err_fatal_params;

#define ERR_CRUMB_TRAIL_BMSK(x)\
do {\
    (coredump.err.crumb_trail_bmsk |= (x));\
    asm volatile ("dccleana(%0)" : : "r" ((qurt_addr_t )( &(coredump.err.crumb_trail_bmsk) )));\
} while (0)

#define ERR_CRUMB_TRAIL_BMSK_ISLAND(x)\
do {\
    err_fatal_params.crumb_trail_bmsk |= (x);\
} while (0)

#endif /* ERRLOG_H */
