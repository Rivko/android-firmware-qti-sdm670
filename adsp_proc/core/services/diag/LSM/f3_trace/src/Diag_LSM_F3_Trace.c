
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diag F3 Trace Saving

GENERAL DESCRIPTION

 Contains functionality to support F3 message trace retrieval from process space.


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2013-2015, 2017 by QUALCOMM Technologies, Incorporated. All Rights Reserved.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/f3_trace/src/Diag_LSM_F3_Trace.c#2 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
08/14/17   sp     Fix start up bug in diag task when hardware threads reduced 
06/18/15   sa     Added support for time sync feature.
09/19/14   sr     Changed the flag diag_f3_trace_wrap_flag to volatile  
03/11/14   sr     Resolved compiler warnings 
02/18/13   ph     Resolved concurrent case where index goes out of bounds.
12/19/13   rh     Save message args individually instead of as a group
11/18/13   ph     Remove intermediate buffer in F3 Trace as part of optimization.
01/25/13   sg     Created File
===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "osal.h"
#include "Diag_LSM_F3_Trace.h" // internal
#include "Diag_LSMi.h"
#include "err.h"
#include "msg.h"
#include "Diag_LSM.h"
#include "diagstub.h"
#include <stringl.h>

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define DIAG_F3_TRACE_GET_TYPE(x)     ((x) >> 13)
#define DIAG_F3_TRACE_MSG_NUM_ARGS(x) (((x) >> 8) & 0x1F)
#define DIAG_F3_TRACE_MSG_TYPE(x) (((x) >> 4) & 0xF)
#define DIAG_F3_TRACE_BYTE_SIZED_ARGS(x) (((x) & 0xC) >> 2)
#define DIAG_F3_TRACE_GET_DETAILS(x)  ((x) & 0xF)
#define DIAG_F3_TRACE_IS_HEADER_VALID(x) ((x)==diag_F3_TRACE_VALID_HEADER_FLAG)

// MIGRATION NOTE: recover_f3.cmm depends upon err_f3_trace_wrap_flag
volatile uint8                   diag_f3_trace_wrap_flag;
static uint32                  diag_f3_trace_buffer_head;
static diag_f3_trace_id_type    diag_f3_trace_buffer_id;
uint8  *diag_f3_trace_buffer = NULL;

typedef struct
{
   void* const_data_ptr; /* can be ptr to msg_const_type or msg_qsr_const_type */
   diag_f3_trace_tstamp_type tstamp;

} msg_debug_store_type;

static void diag_f3_trace_to_buffer(uint32, uint8*, int); // forward reference

static boolean diag_f3_trace_buffer_init(void); // forward reference

static int diag_f3_trace_buffer_initialized = FALSE;
static int diag_f3_trace_buffer_init_begin = FALSE;
static unsigned int diag_f3_trace_buffer_length = 0;
/*Default values*/
static uint8 diag_f3_trace_control_mask = DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL;
static uint8 diag_f3_trace_detail_mask = DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL;

extern boolean diag_time_initialized_lsm;

/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get_LSM)(qword time);

extern boolean diag_time_initialized_lsm;

/*===========================================================================

FUNCTION DIAG_LSM_F3_TRACE_INIT

DESCRIPTION
  Initializes the buffer provided by client for F3 trace saving


RETURN VALUE
  SUCCESS
  FAILURE if the client has provided invalid buffer

SIDE EFFECTS
  None
===========================================================================*/
boolean diag_lsm_f3_trace_init(uint8 *address, unsigned int length)
{
   diag_f3_trace_buffer = (uint8 *)address;
   diag_f3_trace_buffer_length = length;

   if ((diag_f3_trace_buffer)&&(diag_f3_trace_buffer_length>0))
   {
      diag_f3_trace_buffer_init();
      return TRUE;
   }
   else
   {
      printf("Invalid buffer provided");
      return FALSE;
   }

   /* Initilize the time API to use if not initialized by this time */
   if(!diag_time_initialized_lsm)
   {
      diag_time_init_LSM();
   }

}
/*===========================================================================

FUNCTION DIAG_F3_TRACE_TO_BUFFER

DESCRIPTION
  Saves the buffer to the RAM buffer containing trace information.

DEPENDENCIES
  Interrupts must be locked.  FEATURE_ERR_EXTENDED_STORE must be defined.

RETURN VALUE
  If < 0, failure occurred.  Otherwise, success.

SIDE EFFECTS
  None
===========================================================================*/
static void diag_f3_trace_to_buffer(uint32 index, uint8 *src, int length)
{
   int i;

   if (index >= diag_f3_trace_buffer_length)
   {
      index -= diag_f3_trace_buffer_length;
   }

   /* Save to the circular buffer according to where the head points */
   for (i = 0; i < length; i++)
   {
      diag_f3_trace_buffer[index] = src[i];
      index++;
      if (index >= diag_f3_trace_buffer_length)
      {
         index -= diag_f3_trace_buffer_length;
      }
   }

   return;
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_BUFFER_INIT

DESCRIPTION
  Initializes the circular buffer indices.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean diag_f3_trace_buffer_init(void)
{
   /* Grab init_begin flag to make sure only one entity initializes the buffer */
   if (!osal_atomic_compare_and_set((osal_atomic_word_t *)&diag_f3_trace_buffer_init_begin, FALSE, TRUE)) return (FALSE);

   /* Set up the buffer indices */
   diag_f3_trace_buffer_head = 0;
   diag_f3_trace_wrap_flag = FALSE;

   /* Initialize build ID associated with this run */
   /* (timestamp set when file is saved) */
   diag_f3_trace_buffer_id.timestamp = 0;
   diag_f3_trace_buffer_id.buffer_ptr = (uint32)(&diag_f3_trace_buffer_id);
   memset(diag_f3_trace_buffer_id.build, 0, DIAG_F3_TRACE_MAX_ID_LENGTH);
   strlcpy((char *)diag_f3_trace_buffer_id.build, "LPASS\0",
           DIAG_F3_TRACE_MAX_ID_LENGTH);

   /* Clear out buffer before we start putting data in it */
   memset(diag_f3_trace_buffer, 0, diag_f3_trace_buffer_length);

   /* Once initialized to true, the trace records will be allowed to save */
   diag_f3_trace_buffer_initialized = TRUE;

   return (TRUE);
}
/*===========================================================================

FUNCTION DIAG_F3_TRACE_BUFFER_INIT

DESCRIPTION
  Initializes the circular buffer indices.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean diag_lsm_f3_trace_deinit(void)
{

   diag_f3_trace_buffer_init_begin = FALSE;

   /* Set up the buffer indices */
   diag_f3_trace_buffer_head = 0;
   diag_f3_trace_wrap_flag = FALSE;

   /* Clear out buffer */
   if (diag_f3_trace_buffer) memset(diag_f3_trace_buffer, 0, diag_f3_trace_buffer_length);

   /* Once initialized to true, the trace records will be allowed to save */
   diag_f3_trace_buffer_initialized = FALSE;
   diag_lsm_f3_trace_init_control_mask_config(0);
   diag_lsm_f3_trace_init_detail_mask_config(0);
   return (TRUE);
}

static uint32 diag_f3_trace_secure_offset(uint32 length)
{
   uint32 old_head;
   uint32 new_head;

   do
   {
      old_head = diag_f3_trace_buffer_head;
      new_head = old_head + length;
      if (new_head >= diag_f3_trace_buffer_length)
      {
         new_head -= diag_f3_trace_buffer_length;
         diag_f3_trace_wrap_flag = TRUE;
      }
   }
   while (!osal_atomic_compare_and_set((osal_atomic_word_t *)&diag_f3_trace_buffer_head, old_head, new_head));

   return (old_head);
}

/*===========================================================================

FUNCTION DIAG_F3_TRACE_SET_UP_MSG_HEADER

DESCRIPTION
  Creates the header for a F3 message record in the trace buffer.  The
  header structure is as follows:
  Total = 16 bits
    * 3 bits = Record type
    * 5 bits = number of arguments
    * 6 bits = reserved
    * 1 bit  = savetime
    * 1 bit  = savevars

DEPENDENCIES
  None

RETURN VALUE
  Header for the F3 record

SIDE EFFECTS
  None

===========================================================================*/
static void diag_f3_trace_set_up_msg_header(int nargs, char savevars, char savetime, char byte_sized, uint32 type, diag_f3_trace_hdr_type *hdr)
{
   /* Zero out config data to start */
   hdr->config = 0;

   /* Add config bits to header */
   hdr->config |= (DIAG_F3_TRACE_MSG_BIT << 13);
   hdr->config |= (nargs & 0x1F) << 8;
/*lint -save -e{734} */
   byte_sized = byte_sized << 2;
/*lint -restore */
   hdr->config |= (byte_sized | savevars | savetime);
   hdr->config |= (type & 0xF) << 4;

   /* Write header flag to header */
   hdr->header_flag = DIAG_F3_TRACE_VALID_HEADER_FLAG;
}

// THIS IS THE PRIMARY "PUBLIC" API

void diag_f3_save_msg(const void *constant_data_ptr, msg_arg_type *args, uint8 nargs, void *msg, uint32 type)
{
   /* Sanity checks */

   /* Basic data checks */
   if ((diag_f3_trace_buffer == NULL) || (diag_f3_trace_buffer_length == 0))
       return;

   if ((constant_data_ptr == NULL) || (type >= DIAG_F3_TRACE_LAST))
       return;

   /* Is F3 trace msg saving enabled? */
   if (!(diag_f3_trace_control_mask & DIAG_F3_TRACE_MSG_BIT))
      return;

   /* Check masks based on DIAG_F3_TRACE_DIAG_MASK_BIT */
   if (diag_f3_trace_control_mask & DIAG_F3_TRACE_DIAG_MASK_BIT)
   {
      /* Use DIAG masks in this case...so just check to see if msg was created */
      if (!msg)
        return;
   }
   else
   {
      /* Not using DIAG masks in this case,
         so compare F3 trace detail mask with ss_mask */
      uint32 mask;
      if (type == DIAG_F3_TRACE_STANDARD)
         mask = ((msg_const_type *)constant_data_ptr)->desc.ss_mask;
      else
         mask = ((msg_qsr_const_type *)constant_data_ptr)->desc.ss_mask;

      if (!((diag_f3_trace_detail_mask >> 3) & mask))
          return;
   }

   /* Make sure buffer has been initialized (needed prior to intial entry) */
   if (diag_f3_trace_buffer_initialized == FALSE)
   {
      /* Abort if failure/conflict detected during buffer init */
      if (!diag_f3_trace_buffer_init())
         return;
   }

   /* Proceed with saving msg to buffer */
   {
      int i = 0;
      uint32 record_length=0;
      diag_f3_trace_hdr_type record_hdr;
      char savetime = 0, savevars = 0;
      int byte_sized = 3;
      int bytes_to_copy = 0;
      uint32 write_offset = 0;

      savetime = diag_f3_trace_detail_mask & DIAG_F3_TRACE_SAVETIME;
      savevars = diag_f3_trace_detail_mask & DIAG_F3_TRACE_SAVEVARS;

      if (savevars)
      {
         /* Calculate bytes needed to store the largest argument. */

         byte_sized = 3;
         for (i = 0; i < nargs; i++)
         {
            if (args[i] > MAX_24BIT_VALUE)
            {
               byte_sized = 0;
               break; /* Can't get larger; bail out of loop now */
            }
            else if ((args[i] > MAX_16BIT_VALUE) && (byte_sized > 1))
            {
               byte_sized = 1;
            }
            else if ((args[i] > MAX_8BIT_VALUE) && (byte_sized > 2))
            {
               byte_sized = 2;
            }
         }
      }
      else
      {
         /* Not saving any variables, so set nargs to 0 */
         nargs = 0;
      }

      /*Calculate the trace record_length needed i.e. Add Size of header,
      size of msg_const_type, size of timestamp type and size of arguments if any */

      /*Add the size of header */
      record_length += sizeof(diag_f3_trace_hdr_type);

      /*Size of the address of the msg_const_type*/
      record_length += sizeof(void*);

      /* Add size to save timestamp to the record */
      if(savetime)
      {
        record_length += sizeof(diag_f3_trace_tstamp_type);
      }

      /* Size of arguments to save */
      if(savevars)
      {
        bytes_to_copy = (sizeof(msg_arg_type)-(uint32)byte_sized);
        record_length += bytes_to_copy * nargs;
      }

      /* Secure an address to save the message to */
      write_offset = diag_f3_trace_secure_offset(record_length);

      /* Save to the circular buffer according to where the head points */
      /* Set up msg header */

      diag_f3_trace_set_up_msg_header(nargs, savetime, savevars, (char)byte_sized, type, &record_hdr);
      /* Save record to the main F3 trace buffer*/
      diag_f3_trace_to_buffer(write_offset, (uint8*) &record_hdr, sizeof(diag_f3_trace_hdr_type));
      /*Increment the offset by size of the header */
      write_offset+=sizeof(diag_f3_trace_hdr_type);

      /* Save address of the msg_const_type */
      /* ptr size is same for standard and Qshrink messages */

      /* Save record to the main F3 trace buffer*/
      diag_f3_trace_to_buffer(write_offset, (uint8*) &constant_data_ptr, sizeof(void*));

      /*Increment the offset by size of the address */
      write_offset+=sizeof(void*);

      /* Save timestamp to the record */
      if (savetime)
      {
         qword tstamp = {0, 0};     /* Time-stamp to be used with this message       */
         diag_f3_trace_tstamp_type shortened_ts;

         if (msg)
         {
            /* Get the timestamp from the created F3 message */
            if (type == DIAG_F3_TRACE_STANDARD)
            {
               qw_equ(tstamp, ((msg_ext_store_type *)msg)->hdr.ts);
            }
            else
            {
               qw_equ(tstamp, ((msg_qsr_store_type *)msg)->hdr.ts);
            }
         }

         else
         {
            /* Message was dropped and not sent to diag.  Generate own ts */
            if (diag_time_initialized_lsm)
              diag_time_get_LSM(tstamp);                              //lint !e545
         }

         /* Get rid of the last two bytes of the timestamp */
         shortened_ts.hi = qw_hi(tstamp);
         shortened_ts.lo = qw_lo(tstamp) >> HALF_WORD_BITS;
        /* Save record to the main F3 trace buffer*/
        diag_f3_trace_to_buffer(write_offset, (uint8*) &shortened_ts, sizeof(diag_f3_trace_tstamp_type));
        /*Increment the offset by size of the timestamp type */
        write_offset+=sizeof(diag_f3_trace_tstamp_type);
      }
      /* Save arguments to the record */
      if (savevars)
      {
         for (i = 0; i < nargs; i++)
         {
        /* Save record to the main F3 trace buffer*/
            diag_f3_trace_to_buffer(write_offset, (uint8*) &(args[i]), bytes_to_copy);

            write_offset+=bytes_to_copy;
         }
      }
         }
      }


/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT_CONTROL_CONFIG

DESCRIPTION
  Initializes the configuration values.

DEPENDENCIES
  This function is not mutex protected.  It is assumed it will only be
  called from serialized init functions (rcinit).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void diag_lsm_f3_trace_init_control_mask_config(uint8 control)
{
   diag_f3_trace_control_mask = (control & 0xFF);
}
/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT_DETAIL_CONFIG

DESCRIPTION
  Initializes the configuration values.

DEPENDENCIES
  This function is not mutex protected.  It is assumed it will only be
  called from serialized init functions (rcinit).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void diag_lsm_f3_trace_init_detail_mask_config(uint8 detail)
{
   diag_f3_trace_detail_mask = (detail & 0xFF);
}

