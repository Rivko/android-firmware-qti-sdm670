/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     S M E M    L O G G I N G

GENERAL DESCRIPTION
  This module provides extended smem logging in case of fatal and 
  non-fatal errors.  This module is not a task, but rather a set of
  procedures which run in the context of the calling task.

Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_smem_log.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     File created for error extended smem logging

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "erri.h"
#include "tms_utils.h"
#include "smem.h"
#include "err_smem_log.h"

typedef enum{
 sp,
 fp,
 pc,
 ssr,
 badva,
 lr,
 SIZEOF_ARCH_COREDUMP_SMEM_REGISTERS
}coredump_smem_register_enum_type;

static char *arch_coredump_smem_reg_label[SIZEOF_ARCH_COREDUMP_SMEM_REGISTERS] = {
  "SP",
  "FP",
  "PC",
  "SSR",
  "BADVA",
  "LR"
 };

char * err_smem_log_buf_ptr;
static char *smem_buf_ptr=NULL;
static uint32 smem_buf_unused=0;
static boolean smem_buf_init = FALSE;
static uint32 smem_written;

/* Code under ERR_F3_TRACE_TO_SMEM flag is deprecated from internal CRMs 
   since A-family targets, however code is available in case if any 
   customer is using this.

   Code under this macro is used to dump error related information in below 
   SMEM_ERR_CRASH_LOG SMEM item of size ERR_DATA_MAX_SIZE.
*/
#if defined(ERR_F3_TRACE_TO_SMEM)
  static char *err_smem_log_buf;
  static err_data_type err_data;
  static void err_store_info(void);
#endif /* ERR_F3_TRACE_TO_SMEM */

/* Size for enhanced SMEM error logging */
#define ERR_SMEM_BUFFER_SIZE 0x1000

/* To identify which process got crashed first, and update detail of that process only in SMEM */
DalTimetickTime64Type err_handling_prev_time = 0;

/* version information for the smem */
struct error_smem_version_s
{
uint32 major_ver;
uint32 minor_ver;
};

static const struct error_smem_version_s err_smem_version = { 1,0 };

static void err_info_to_smem_buffer(void);

/*===========================================================================
MACRO WRITE_BUF
 
DESCRIPTION
  MACRO to be used when printing to the log buffer.  Should be called using
  the same arguments you would pass to a printf() call.  Checks first to see
  if space is available in the buffer, then passes the buffer location, size,
  and VA_ARGS parameters to the tms_utils_fmt function. Upon returning, the
  various buffer pionters and size variables are updated as needed.
============================================================================*/
#define SMEM_WRITE_BUF(...) \
   if(smem_buf_unused != 0) { \
	 smem_written = tms_utils_fmt(smem_buf_ptr, smem_buf_unused, __VA_ARGS__); \
	 if(smem_written > TMS_UTILS_BUF_SZ_ZERO){\
		smem_buf_ptr+=(smem_written-1); \
	 }\
	 smem_buf_unused-=MIN(smem_written,smem_buf_unused); \
   }


 /*===========================================================================
FUNCTION ERR_LOG_REGISTERS (Static)

DESCRIPTION
  Logs register information to buffer
============================================================================*/
static void err_smem_log_registers(void)
{
  uint32 i;
  uint32 regs[SIZEOF_ARCH_COREDUMP_SMEM_REGISTERS]= {0};

  /* Get the register values that we need for the smem logging */
  regs[sp] = coredump.arch.regs.name.sp;
  regs[fp] = coredump.arch.regs.name.fp;
  regs[pc] = coredump.arch.regs.name.pc;
  regs[ssr] = coredump.arch.regs.name.ssr;
  regs[badva] = coredump.arch.regs.name.badva;
  regs[lr] = coredump.arch.regs.name.lr;

  SMEM_WRITE_BUF("Register:\n");

  for(i=0; i<SIZEOF_ARCH_COREDUMP_SMEM_REGISTERS; i++)
  {
	/* Print out register names and values */
	SMEM_WRITE_BUF("%12s : 0x%08x\n", arch_coredump_smem_reg_label[i], (unsigned int)regs[i]);
  }

  SMEM_WRITE_BUF("\n");

  return;
}


/*===========================================================================
FUNCTION ERR_SMEM_LOG_STACK (Static)

DESCRIPTION
  Logs stack information to buffer, printing both address and data.
  Will not print stack addresses that fall outside of the stack range based
  on the TCB of the faulting task (in the case no TCB is reported, the TCB
  of the current task is used)
============================================================================*/
static void err_smem_log_stack(void)
{
  uint32 stack_base = 0; /* lower start address of the stack */
  uint32 stack_end = 0;   /* higher adress, i.e. range of the stack */
  uint32 *sp;
  uint32 i;

  /* thread attribute */
  qurt_thread_attr_t *attr = NULL;
  attr = qurt_system_stm_thread_attr_get (coredump.err.tid);

  if (attr != NULL)
  {
    /* Stack variables */
    stack_base = (uint32)attr->stack_addr;      /* base address of the stack */
    stack_end = stack_base + attr->stack_size;  /* Range of the stack */
  }

  sp = (uint32*)coredump.arch.regs.name.sp;

  /* Dump the stack overflow / underflow info */
  if (coredump.err.stack_error & STACK_UNDERFLOW)
  {
    SMEM_WRITE_BUF("Stack Underflow detected, or the stack variables are corrupted\n");
  }

  if (coredump.err.stack_error & STACK_OVERFLOW)
  {
    SMEM_WRITE_BUF("Stack Overflow detected, or the stack variables are corrupted\n");
  }

  /* Dump the stack */
  if ( (sp != NULL) && (attr != NULL) )
  {
    if((sp > (uint32*)stack_base) && (sp < (uint32*)stack_end))
    {
	  SMEM_WRITE_BUF("Stack Dump\n");
	  SMEM_WRITE_BUF("from : 0x%08x\n",(unsigned int)sp);
	  SMEM_WRITE_BUF("to   : 0x%08x\n",(unsigned int)stack_end);
	  for(i=0; (i<attr->stack_size) && (sp<(uint32*)stack_end); i++)
	  {
		  SMEM_WRITE_BUF("%08x\n",(unsigned int)*sp);
		  sp++;
	  }
	}
	else
	{
	  SMEM_WRITE_BUF("Stack pointer (0x%08x) out of range of current tcb stack limits\n",(unsigned int)sp);
	  SMEM_WRITE_BUF("Stack data not dumped.\n");
	}
  }
  else
  {
	/* No stack poitner recorded. This could be due to a dog timeout (no rex_core),
	   or some other error that bypassed the saving of registers. */
	SMEM_WRITE_BUF("No stack pointer recorded (or SP is NULL)...stack data not dumped.\n");
  }

  SMEM_WRITE_BUF("\n");

  return;
}

/*===========================================================================
FUNCTION ERR_SMEM_LOG_THREAD_INFO (Static)

DESCRIPTION
  Logs thread related information to buffer
============================================================================*/
static void err_smem_log_thread_info(void)
{
  SMEM_WRITE_BUF("Thread ID      : 0x%08x\n",(unsigned int)coredump.err.tid);
  SMEM_WRITE_BUF("Thread name    : %s\n",coredump.err.tcb_name);
  SMEM_WRITE_BUF("Process ID     : %d\n",coredump.err.pd_id);
  /* Report time information */
  SMEM_WRITE_BUF("Start Timetick : 0x%08x%08x\n",
                 (unsigned int)(coredump.err.err_handler_start_time >> 32),
                 (unsigned int)(coredump.err.err_handler_start_time & 0xFFFFFFFF) );  
  SMEM_WRITE_BUF("\n");
}

/*===========================================================================

FUNCTION err_smem_log_write_null

DESCRIPTION
  This function writes NULL at the end of SMEM buffer after logging is complete.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void err_smem_log_write_null (void)
{
  if (smem_buf_unused == 0) 
  {
  /* The entire smem buffer is written, so write at the end */
  err_smem_log_buf_ptr[ERR_SMEM_BUFFER_SIZE - 1] = (char)0;
  }
  else
  {
  /* Write at the end of the current used smem buffer */
  *smem_buf_ptr = (char)0;
  }
}


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
void err_smem_calculate_offset(void)
{
  char * traversal_pointer = NULL;
  uint32 index = 0;

  if (err_smem_log_buf_ptr != NULL)
  {
   traversal_pointer = err_smem_log_buf_ptr;

   /* Run in a loop until NULL if found */
   while ( (index < ERR_SMEM_BUFFER_SIZE) &&
   	       (traversal_pointer[index] != 0) )
   	{
   	 index++;
   	}

   /* Initialize the SMEM_WRITE_BUF macro offset's based on this found index */
   smem_written = index;
   if(smem_written > TMS_UTILS_BUF_SZ_ZERO)
   {
	smem_buf_ptr+=(smem_written);
   }
   smem_buf_unused-=MIN(smem_written,smem_buf_unused);
  }

  return;
}

/*===========================================================================

FUNCTION err_smem_log_init_setup

DESCRIPTION
  This function initialized the smem WRITE_BUF macro.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
static void err_smem_log_err_smem_ver(void)
{
  /* Write the err smem version information at the starting */
  SMEM_WRITE_BUF("err_smem_ver.%u.%u\n",
                  (unsigned int)err_smem_version.major_ver, 
                  (unsigned int)err_smem_version.minor_ver);
  return;
}

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
                                           const char * qcom_timestampstring_string )
{
  char err_no_val[] = "No Value";

  if (err_smem_log_buf_ptr == NULL)
  {
   return;
  }

  /* Do the memset of the smem log buffer.
     * Since we want to do the memset only once & from the root PD only.
     */
  memset((void*)err_smem_log_buf_ptr,0x0,ERR_SMEM_BUFFER_SIZE);

  /* We want to update the err smem version 
     * information at the begining of the smem buffer.
     */
  err_smem_log_err_smem_ver();  

  if(engg_timestampstring_string[0]!=0)
  {
	SMEM_WRITE_BUF("%s\n", engg_timestampstring_string);
  }
  else
  {
	SMEM_WRITE_BUF("engg_timestampstring : %s\n", err_no_val);
  }

  
  if(qcom_timestampstring_string[0]!=0)
  {
	SMEM_WRITE_BUF("%s\n", qcom_timestampstring_string);
  }
  else
  {
	SMEM_WRITE_BUF("qcom_timestampstring : %s\n", err_no_val);
  }

  SMEM_WRITE_BUF("\n");
  return;
}

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
void err_smem_log_image_version(void)
{
//  uint32 up_time;
//  uint32 up_h, up_m, up_s;
  char err_no_val[] = "No Value";

  if (err_smem_log_buf_ptr == NULL)
  {
   return;
  }

  if(coredump.image.qc_image_version_string[0]!=0)
  {
	SMEM_WRITE_BUF("QC Image Version: %s\n", coredump.image.qc_image_version_string);
  }
  else
  {
	SMEM_WRITE_BUF("QC Image Version: %s\n", err_no_val);
  }

  
  if(coredump.image.image_variant_string[0]!=0)
  {
	SMEM_WRITE_BUF("Image Variant : %s\n", coredump.image.image_variant_string);
  }
  else
  {
	SMEM_WRITE_BUF("Image Variant : %s\n", err_no_val);
  }

  if(coredump.image.oem_image_version_string[0]!=0)
  {
	SMEM_WRITE_BUF("Oem Image Version: %s\n", coredump.image.oem_image_version_string);
  }
  else
  {
	SMEM_WRITE_BUF("Oem Image Version: %s\n", err_no_val);
  }


  if(coredump.image.oem_image_uuid_string[0]!=0)
  {
	SMEM_WRITE_BUF("Oem Image UUID: %s\n", coredump.image.oem_image_uuid_string);
  }
  else
  {
	SMEM_WRITE_BUF("Oem Image UUID: %s\n", err_no_val);
  }

  SMEM_WRITE_BUF("\n");
  return;
}


/*===========================================================================

FUNCTION err_smem_log_init_setup

DESCRIPTION
  This function initialized the smem WRITE_BUF macro.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
static void err_smem_log_init_setup(char *buff, uint32 size)
{
  if( buff != 0 )
  {
    // buf_base_ptr = buff; // Base address of buffer 
    smem_buf_ptr = buff;      // Current position in buffer
    smem_buf_unused = size;   // Total unused size of buffer (bytes)
    smem_buf_init=TRUE;
  }
}

/*===========================================================================

FUNCTION err_smem_log_init

DESCRIPTION
  This function initialized the smem log for writing extended smem info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
void err_smem_log_init (void)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  int32 smem_rv;
  smem_alloc_params_type smem_params;

  smem_params.remote_host = SMEM_APPS;
  smem_params.smem_type = ERR_LOG_SMEM_ITEM;
  smem_params.size = ERR_SMEM_BUFFER_SIZE;
  smem_params.buffer = NULL;
  smem_params.flags = SMEM_ALLOC_FLAG_NONE;

  smem_rv = smem_alloc_ex(&smem_params);
  if(SMEM_STATUS_SUCCESS != smem_rv)
  {
    TMS_MSG_ERROR_1("SMEM alloc for ERR_LOG_SMEM_ITEM failed, rv=%d", smem_rv);
    goto next;
  }
  else
  {
    /* Initialize the err smem buffer */
    err_smem_log_buf_ptr = smem_params.buffer;
    err_smem_log_init_setup(err_smem_log_buf_ptr, ERR_SMEM_BUFFER_SIZE);	 
  }

  /* Error info to SMEM buffer callback registeration with error handling */
  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = err_info_to_smem_buffer;
  cb_info.bucket = ERRCB_BUCKET_NORMAL;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS != rv)
    TMS_MSG_ERROR_1("Registeration of Error info to SMEM buffer cb failed, rv = %d", rv);


next:
  /***************************************************************************
            Code under ERR_F3_TRACE_TO_SMEM flag is deprecated!!
  ***************************************************************************/
  #if defined(ERR_F3_TRACE_TO_SMEM)

  err_smem_log_buf = (char *)smem_alloc(SMEM_ERR_CRASH_LOG, ERR_DATA_MAX_SIZE);

  /* err_store_info callback registeration with error handling */
  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = err_store_info;
  cb_info.bucket = ERRCB_BUCKET_NORMAL;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS != rv)
    TMS_MSG_ERROR_1("Registeration of err_store_info cb failed, rv = %d", rv);

  #endif // ERR_F3_TRACE_TO_SMEM
  return;
}

/*===========================================================================

FUNCTION err_info_to_smem_buffer

DESCRIPTION
  This function logs below listed error information in shared memory buffer , which will be read by HLOS kernel
   to shown in kernle logs(Dmsgs):
   
-	Time of crash (m-d-y h:m:S):
-	Uptime (h:m:S): 
-	Build ID: 
-	Thread id: 
-	Thread_name: 
-	Registers: 
	ELR :
	BADVA :
	SSR :
-	PD :
-	Call stack:

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/

static void err_info_to_smem_buffer (void)
{
  /* Check which process got crashed first & buffer is not NULL */
  if ( (err_smem_log_buf_ptr != NULL)
        && (coredump.err.err_handler_start_time > 0)
        && ( (err_handling_prev_time == 0) 
              || (err_handling_prev_time > coredump.err.err_handler_start_time)
              )
        )
  {
    ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_SMEM_LOG_PRE);

    err_smem_log_thread_info();

    err_smem_log_registers();

    err_smem_log_stack();

    err_smem_log_write_null();

    err_handling_prev_time = coredump.err.err_handler_start_time;	 
  }
}

/***************************************************************************
        Code under ERR_F3_TRACE_TO_SMEM flag is deprecated!!
***************************************************************************/
#if defined(ERR_F3_TRACE_TO_SMEM)

/*===========================================================================

FUNCTION ERR_STORE_INFO

DESCRIPTION
  Writes some crash related information to a designated RAM buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
============================================================================*/
#define ERR_DATA_MAGIC_NUMBER       (uint64)0x5252452041544144ULL
static void err_store_info (void)
{
  char *buf = NULL;

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_STORE_INFO_PRE);
  
  /* If magic numbers are present at the front of the buffer, then previous
   * data has not been written out yet. Return without doing anything in
   * this case. -- this is probably obsolete without nzi, but cheap protection
   */
  if(err_data.err_log.flag == ERR_DATA_MAGIC_NUMBER)
  {
    /* Buffer is already full */
    return;
  }
  err_data.err_log.flag = ERR_DATA_MAGIC_NUMBER;

  /* Copy data needed to store NV log */
  err_data.err_log.nv_log.line_num = coredump.err.linenum;
  err_data.err_log.nv_log.fatal = TRUE;
  (void) strlcpy((char *)err_data.err_log.nv_log.file_name,
					       (char *)coredump.err.filename,
                 MIN( sizeof(err_data.err_log.nv_log.file_name), sizeof(coredump.err.filename) ));

  buf = (char *) err_data.err_log.data;

  /* Interrupts are disabled at this point -- need to make sure we don't
   * timeout.  Use force_kick to avoid intlock attempt in isr (possibly)
   */
  dog_force_kick();
  (void) memset (buf, 0x00, ERR_DATA_MAX_SIZE);
  err_log_init(buf, ERR_DATA_MAX_SIZE);
  err_data.err_log.length = err_generate_log();

  /* If we have ptr to ERR smem log region, copy log buffer */
  if(err_smem_log_buf!=NULL)
  {
    memscpy((void*)err_smem_log_buf, ERR_DATA_MAX_SIZE, (void*)buf, ERR_DATA_MAX_SIZE);
  }
  //actual f3 trace is handled exclusively by diag now (using err_crash_cb_register)

  return;
} /* err_store_info */

#endif // ERR_F3_TRACE_TO_SMEM

