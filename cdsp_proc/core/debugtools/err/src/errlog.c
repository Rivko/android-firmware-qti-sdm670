/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Error Logging Code

General Description
  The routines in this file handle writing a simple text log with crash
  information to a designated buffer in memory.  The information in these
  logs should in most cases be enough to help identify where a crash
  occurred, but it is not anticipated that there will be enough information
  available in the log to fully debug the crash itself.

Copyright (c) 2009 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/errlog.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/09   tbg     File Created

===========================================================================*/


#include "comdef.h"
#include "errlog.h"
#include "err.h"
#include "erri.h"
#include "stdlib.h"
#include "time_jul.h"
#include "time_svc.h"
#include "mobile.h"
#include "rex.h"

#include "tms_utils.h"

extern coredump_type coredump;

/* Misc labels for log printing.
   Should match any correspoding structs in errlog.h
 */

static char *arch_coredump_type_label[SIZEOF_ERR_ARCH_TYPE] = {
  "ERR_ARCH_UNKNOWN",
  "ERR_ARCH_ARM",
  "ERR_ARCH_QDSP6"
};

#define COREDUMP_SP_REG QDSP6_SP
static char *arch_coredump_register_label[SIZEOF_ARCH_COREDUMP_REGISTERS] = {
  "QDSP6_GP_R0",
  "QDSP6_GP_R1",
  "QDSP6_GP_R2",
  "QDSP6_GP_R3",
  "QDSP6_GP_R4",
  "QDSP6_GP_R5",
  "QDSP6_GP_R6",
  "QDSP6_GP_R7",
  "QDSP6_GP_R8",
  "QDSP6_GP_R9",
  "QDSP6_GP_R10",
  "QDSP6_GP_R11",
  "QDSP6_GP_R12",
  "QDSP6_GP_R13",
  "QDSP6_GP_R14",
  "QDSP6_GP_R15",
  "QDSP6_GP_R16",
  "QDSP6_GP_R17",
  "QDSP6_GP_R18",
  "QDSP6_GP_R19",
  "QDSP6_GP_R20",
  "QDSP6_GP_R21",
  "QDSP6_GP_R22",
  "QDSP6_GP_R23",
  "QDSP6_GP_R24",
  "QDSP6_GP_R25",
  "QDSP6_GP_R26",
  "QDSP6_GP_R27",
  "QDSP6_GP_R28",
  "QDSP6_SP",
  "QDSP6_FP",
  "QDSP6_LR",
  "QDSP6_PC",
  "QDSP6_USR",
  "QDSP6_UGP",
  "QDSP6_GP",
  "QDSP6_LC0",
  "QDSP6_LC1",
  "QDSP6_SA0",
  "QDSP6_SA1",
  "QDSP6_M0",
  "QDSP6_M1",
  "QDSP6_P",
  "QDSP6_SGP",
  "QDSP6_SSR",
  "QDSP6_IMASK",
  "QDSP6_BADVA",
  "QDSP6_ELR",
  "QDSP6_TID",
  "QDSP6_EVB",
  "QDSP6_IPEND",
  "QDSP6_SYSCFG",
  "QDSP6_MODECTL",
  "QDSP6_REV",
  "QDSP6_TLBHI",
  "QDSP6_TLBLO",
  "QDSP6_TLBIDX",
  "QDSP6_DIAG",
  "QDSP6_IAD",
  "QDSP6_IEL",
  "QDSP6_IAHL",
  "QDSP6_PCYCLEHI",
  "QDSP6_PCYCLELO"
  };

/* Maximum number of words dumped from the stack */
#define ERR_STACK_DUMP_SIZE 0x64 

/* Version number of errlog format */
#define EXT_STORE_VERSION 3

static boolean buf_init=FALSE;
//static char *buf_base_ptr=NULL;
static char *buf_ptr=NULL;
static uint32 buf_size=0;
static uint32 buf_unused=0;
static uint32 written;

/*===========================================================================
MACRO WRITE_BUF

DESCRIPTION
  MACRO to be used when printing to the log buffer.  Should be called using
  the same arguments you would pass to a printf() call.  Checks first to see
  if space is available in the buffer, then passes the buffer location, size,
  and VA_ARGS parameters to the tms_utils_fmt function. Upon returning, the
  various buffer pionters and size variables are updated as needed.
============================================================================*/
#define WRITE_BUF(...) \
  if(buf_unused != 0) { \
    written = tms_utils_fmt(buf_ptr, buf_unused, __VA_ARGS__); \
    if(written > TMS_UTILS_BUF_SZ_ZERO){\
       /* tms_utils_fmt returns total number of bytes written, including null\
          term.  Set in pointer to point at NULL so we overwrite it if \
	  further writes occur */\
       buf_ptr+=(written-1); \
    }\
    buf_unused-=MIN(written,buf_unused); \
  }

/*===========================================================================
FUNCTION ERR_LOG_REGISTERS (Static)

DESCRIPTION
  Logs register information to buffer
============================================================================*/
static void err_log_registers(void)
{
  uint32 i;
  uint32 *regs=(uint32*)&coredump.arch.regs.array[0];

  WRITE_BUF("Register values from coredump:\n");

  for(i=0; i<SIZEOF_ARCH_COREDUMP_REGISTERS; i++)
  {
	/* Print out register names and values */
	WRITE_BUF("%12s : 0x%08lx\n", arch_coredump_register_label[i], (unsigned long)regs[i]);
  }

  WRITE_BUF("\n");

  return;
}

/*===========================================================================
FUNCTION ERR_LOG_STACK (Static)

DESCRIPTION
  Logs stack information to buffer, printing both address and data.
  Will not print stack addresses that fall outside of the stack range based
  on the TCB of the faulting task (in the case no TCB is reported, the TCB
  of the current task is used)
============================================================================*/
static void err_log_stack(void)
{
  uint32 i;
  uint32 *sp, stack_end;
  rex_tcb_type *tcb_ptr = (rex_tcb_type*)coredump.os.tcb_ptr;
  rex_task_attributes_type attr;
	rex_stack_pointer_type stack_start_ptr = NULL;
	rex_stack_size_type stack_size = 0;

  /* Get stack pointer from rex_core */
  sp=(uint32*)coredump.arch.regs.array[COREDUMP_SP_REG];

  if (sp!=0)
  {
	if(tcb_ptr == 0)
	{
	  /* Use tcb_ptr from rex_core if available, otherwise get current task tcb_ptr */
	   tcb_ptr = rex_self();
	}

  rex_task_get_attr(tcb_ptr, &attr);
  rex_task_attr_get_stack_attr(&attr, &stack_start_ptr, &stack_size);

	/* Get the address of the end of the stack for this task, and make sure
       we don't read beyond it.*/
	stack_end = (uint32)(stack_start_ptr) + stack_size;

  /*Copy the stack.  Only do the copy if the stack pointer is within the 
     stack for the TCB*/
  if((sp < (uint32*)stack_end) && (sp > (uint32*)stack_start_ptr))
  {
	  WRITE_BUF("Stack Dump (from 0x%08lx):\n",(unsigned long)sp);
	  WRITE_BUF("Addr        Data\n");
	  for(i=0; (i<ERR_STACK_DUMP_SIZE) && (sp<(uint32*)stack_end); i++)
	  {
		  WRITE_BUF("0x%08lx  0x%08lx\n",(unsigned long)sp,(unsigned long)*sp);
		  sp++;
	  }
	}
	else
	{
	  WRITE_BUF("Stack pointer (0x%08lx) out of range of current tcb stack limits\n",(unsigned long)sp);
	  WRITE_BUF("Stack data not dumped.\n");
	}
  }
  else
  {
	/* No stack poitner recorded. This could be due to a dog timeout (no rex_core),
	   or some other error that bypassed the saving of registers. */
	WRITE_BUF("No stack pointer recorded (or SP is NULL)...stack data not dumped.\n");
  }

  WRITE_BUF("\n");
  return;
}

/*===========================================================================
FUNCTION ERR_LOG_OS (Static)

DESCRIPTION
  Function to log information relevant to the OS being used.
  Currently assumes REX/L4 and accesses minimal information from the TCB.

============================================================================*/
static void err_log_os(void)
{
  rex_tcb_type *tcb_ptr = (rex_tcb_type*)coredump.os.tcb_ptr;
  char rex_task_name[REX_TASK_NAME_LEN+1];

  if(tcb_ptr == 0)
  {
	  /* Use tcb_ptr from coredump if available, otherwise get current task tcb_ptr */
	  tcb_ptr = rex_self();
  }
  rex_get_task_name(tcb_ptr, rex_task_name, sizeof(rex_task_name), NULL);
  WRITE_BUF("REX_TCB ptr: 0x%08lx\n",(uint32)tcb_ptr);
  WRITE_BUF("tcb.task_name: %s\n",rex_task_name);

  WRITE_BUF("\n");
}

/*===========================================================================
FUNCTION ERR_LOG_ARCH (Static)

DESCRIPTION
  Function to log information relevant to CPU architecture being used.
  Currently suppots only ARM.
============================================================================*/
static void err_log_arch(void)
{
  WRITE_BUF("Coredump ARCH type is: %s\n",
    arch_coredump_type_label[coredump.arch.type]);
  /* Call helper functions to print registers and stack */
  err_log_registers();
  err_log_stack();
}

/*===========================================================================
FUNCTION ERR_LOG_ERR (Static)

DESCRIPTION
  Function to log general information relevant to the error being handled.
  Includes information such as error line number and file, time of crash, etc.
============================================================================*/

static char err_no_val[] = "No Value";

static void err_log_err(void)
{
  uint32 up_time;
  uint32 up_h, up_m, up_s;

  if(coredump.err.filename[0]!=0)
  {
	WRITE_BUF("Error in file %s, line %u\n",
      coredump.err.filename, (unsigned int)coredump.err.linenum);
  }
  else
  {
        WRITE_BUF("Error in file %s, line %u\n", err_no_val, 
                   (unsigned int)coredump.err.linenum);
  }
  if(coredump.err.message[0]!=0)
  {
	WRITE_BUF("Error message: %s\n", coredump.err.message);
  }
  else
  {
        WRITE_BUF("Error message: %s\n", err_no_val);
  }

  if(coredump.err.aux_msg[0]!=0)
  {
	WRITE_BUF("Error aux message: %s\n", coredump.err.aux_msg);
  }
  else
  {
	WRITE_BUF("Error aux message: %s\n", err_no_val);	 
  }

  if(coredump.image.qc_image_version_string[0]!=0)
  {
	WRITE_BUF("QC Image Version: %s\n", coredump.image.qc_image_version_string);
  }
  else
  {
	WRITE_BUF("QC Image Version: %s\n", err_no_val);
  }

  
  if(coredump.image.image_variant_string[0]!=0)
  {
	WRITE_BUF("Image Variant : %s\n", coredump.image.image_variant_string);
  }
  else
  {
	WRITE_BUF("Image Variant : %s\n", err_no_val);
  }

  if(coredump.image.oem_image_version_string[0]!=0)
  {
	WRITE_BUF("Oem Image Version: %s\n", coredump.image.oem_image_version_string);
  }
  else
  {
	WRITE_BUF("Oem Image Version: %s\n", err_no_val);
  }


  if(coredump.image.oem_image_uuid_string[0]!=0)
  {
	WRITE_BUF("Oem Image UUID: %s\n", coredump.image.oem_image_uuid_string);
  }
  else
  {
	WRITE_BUF("Oem Image UUID: %s\n", err_no_val);
  }

  /* Report time information */

  WRITE_BUF("Start Timetick : %lu\n", (unsigned long)coredump.err.err_handler_start_time);
  WRITE_BUF("End   Timetick : %lu\n", (unsigned long)coredump.err.err_handler_end_time);

  #ifndef FEATURE_ERR_Q6_CLIENT
  {
    time_julian_type sys_time;
    
    time_get_julian(&sys_time);
    WRITE_BUF("Time of crash (m-d-y h:m:s): %02d-%02d-%d %d:%02d:%02d\n",sys_time.month,sys_time.day,sys_time.year,sys_time.hour,sys_time.minute,sys_time.second);
  }
  #endif

  up_time=time_get_uptime_secs();
  up_h=up_time/3600;
  up_m=(up_time-(up_h*3600))/60;
  up_s=up_time-(up_h*3600)-(up_m*60);
  WRITE_BUF("Uptime (h:m:s): %ld:%02ld:%02ld\n",up_h,up_m,up_s);

  /* Report Build ID */
  WRITE_BUF("Build ID: %s\n", mob_sw_rev);

  WRITE_BUF("\n");
  return;
}

/*===========================================================================
FUNCTION ERR_LOG_HEADER (Static)

DESCRIPTION
  Function to place any needed header information that should go at the
  top of the log report. 
============================================================================*/
static void err_log_header(void)
{
  WRITE_BUF("ERR crash log report.  Version %d.\n\n",EXT_STORE_VERSION);
  return;
}

/*===========================================================================
FUNCTION ERR_LOG_PRINT_TO_BUF (Static)

DESCRIPTION
  This function is for giving external functions access to print to the log
  report.  It is static and only accessible if passed as a pointer by the
  errlog code to the external function.

  String passed in will only be logged if there is room in the buffer.

PARAMETERS
  char *buf      Pointer to buffer

RETURNS 
  None.

============================================================================*/
static void err_log_print_to_buf(char * buf)
{
  WRITE_BUF("%s", buf);
  return;
}

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
void err_log_init(char *buff, uint32 size)
{
  if( buff != 0 )
  {
    // buf_base_ptr = buff; // Base address of buffer 
    buf_ptr = buff;      // Current position in buffer
    buf_size = size;     // Total size of buffer (bytes)
    buf_unused = size;   // Total unused size of buffer (bytes)
    buf_init=TRUE;
  }
}

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
uint32 err_generate_log(void)
{
  if ((buf_init!=TRUE) || (buf_unused==0))
  {
	return 0;
  }

  err_log_header();
  err_log_err();
  err_log_os();
  err_log_arch();
  
  /* Use err_log_print_to_buf here instead of WRITE_BUF macro
   * to remove compiler warning when err_log_print_to_buf not
   * otherwise referenced. It works the same. */
  err_log_print_to_buf("End of crash log report.\n");

  return (buf_size - buf_unused);
}
