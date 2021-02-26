#ifndef DIAG_LSM_F3_TRACEI_H
#define DIAG_LSM_F3_TRACEI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Diag F3 Save Trace

GENERAL DESCRIPTION
  Contains headers for FEATURE_SAVE_DEBUG_TRACE 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/f3_trace/src/Diag_LSM_F3_Trace.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/18/13   ph      Remove intermediate buffer in F3 Trace as part of optimization.
01/25/13   sg      File created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qw.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define DIAG_F3_TRACE_BUFFER_CORRUPTED -2
#define DIAG_F3_TRACE_FAILURE          -1
#define DIAG_F3_TRACE_SUCCESS           0

#ifdef FEATURE_SAVE_TRACE_ON_BY_DEFAULT
   /* Allow targets to define these values in other header files if needed.
    * Values will not be redefined if they already exist. */
   #ifndef DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL
          #define DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL 0x1
   #endif

   #ifndef DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL
      #define DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL 0xFF
   #endif
#else //!FEATURE_SAVE_TRACE_ON_BY_DEFAULT
   /* Undefine values if FEATURE_SAVE_TRACE_ON_BY_DEFAULT is not enabled
    * and redefine to 0x0. */
   #ifdef DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL
      #undef DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL
   #endif
   #define DIAG_F3_TRACE_CONTROL_MASK_DEFAULT_VAL 0x0

   #ifdef DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL
      #undef DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL
   #endif
   #define DIAG_F3_TRACE_DETAIL_MASK_DEFAULT_VAL 0x0
#endif //FEATURE_SAVE_TRACE_ON_BY_DEFAULT


#define MAX_8BIT_VALUE                      255
#define MAX_16BIT_VALUE                     65535
#define MAX_24BIT_VALUE                     16777215

#define DIAG_F3_TRACE_SAVETIME 0x2
#define DIAG_F3_TRACE_SAVEVARS 0x1

/* Format of diag_f3_trace_hdr_type:
 *  header_flag: 32 bit value indicating the start of a message
 *
 *  config: 16 bit field containing the following information:
 *  Bits 15:13    Type Field (Event, Msg, etc.)
 *  Bits 11:8     Number of arguments Field
 *  Bits  7:4     Message Type Field (Standard MSG/Qshrink MSG)
 *  Bits  3:0     Details Field
 *                Bit 3:2 "Byte Sized Arguments" Field
 *                Bit 1   Save Time flag
 *                Bit 0   Save Vars flag
 */
#define DIAG_F3_TRACE_VALID_HEADER_FLAG 0xA55A6BB6
typedef PACK(struct)
{
   uint32 header_flag;
   uint16 config;
} diag_f3_trace_hdr_type;

typedef PACK(struct)
{
   uint16 lo;        /* Most significant of the lower 32 bits */
   uint32 hi;        /* Most significant bytes of a qword timestamp */
} diag_f3_trace_tstamp_type;

#define DIAG_F3_TRACE_MAX_ID_LENGTH 40

typedef struct
{
   uint64 timestamp;
   uint32 buffer_ptr;
   uint8  build[DIAG_F3_TRACE_MAX_ID_LENGTH];
} diag_f3_trace_id_type;

typedef uint32 msg_arg_type;

enum
{
   DIAG_F3_TRACE_STANDARD = 0,
   DIAG_F3_TRACE_QSHRINK,
   DIAG_F3_TRACE_QSHRINK20,
   DIAG_F3_TRACE_LAST
};


#define DIAG_F3_TRACE_MSG_BIT 0x1
#define DIAG_F3_TRACE_DIAG_MASK_BIT 0x8

/*===========================================================================

FUNCTION DIAG_F3_TRACE_INIT

DESCRIPTION
  Initialization function for this module.  Registers the packet handlers.
  Reads the diag_debug NV Item to set the run time mask.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diag_f3_trace_init(void);

void diag_f3_save_msg(const void * constant_data_ptr, msg_arg_type * args, uint8 nargs, void * msg, uint32 type);

#endif /* DIAG_LSM_F3_TRACEI_H */
