#ifndef DIAGLOGI_H
#define DIAGLOGI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services internal header file

General Description
  Internal declarations to support data logging.

Initializing and Sequencing Requirements 
  'log_init()' must be called once during initialization prior to use.

Copyright (c) 2007-2014,2016-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                          Edit History 
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/17   sp      Added support for extended logs in userPD 
05/12/16   ph     SMD references removal and code clean up.
10/31/14   rs      Removed EQUIP_ID_MAX.
11/22/13   sr      Removed PACK from pointer references 
09/27/13   xy      Removed code under FEATURE_WINCE  
11/09/12   is      Support for preset masks
08/31/12   sr      Support for mask retrieval command 
10/07/11   is      Support sending log mask per equipment id
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
02/01/11   sg      Dual mask changes
02/25/10   sg      Merging DCM Rtos and Wm files
07/27/09   mad     Included qw.h, changed #pragma pack usage
01/10/08   mad     Added copyright and file description.
12/5/07    as      Created

===========================================================================*/


#include "comdef.h"
#include "log_codes.h"
#include "log.h"
#include "diagtune.h"      /* For def. of LOG_EQUIP_ID_1_LAST_CODE etc */
#include "qw.h"            /* for qword */

/* -------------------------------------------------------------------------
 * Definitions and Declarations
 * ------------------------------------------------------------------------- */

typedef PACK( struct)
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} log_header_type;



typedef struct{
  log_code_type log_code;           /* stores the log_code */
  log_on_demand log_on_demand_ptr;  /* stores the function pointer */
}log_on_demand_list_item_type;


typedef enum
{
  LOG_INIT_STATE = 0,
  LOG_NORMAL_STATE,
  LOG_FLUSH_STATE, /* Pending flush operation. */
  LOG_PANIC_STATE /* Panic mode flush in progress */

} log_state_enum_type;


#define LOG_DIAGPKT_OFFSET FPOS(diag_log_rsp_type, log)

/* -------------------------------------------------------------------------
 * Definitions for last log code per equipment ID.
 * If it is undefined, it is defined to 0.  digatune.h need only to 
 * contain values for included equipment IDs.
 * ------------------------------------------------------------------------- */
#if !defined (LOG_EQUIP_ID_0_LAST_CODE)
#define LOG_EQUIP_ID_0_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_1_LAST_CODE)
#define LOG_EQUIP_ID_1_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_2_LAST_CODE)
#define LOG_EQUIP_ID_2_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_3_LAST_CODE)
#define LOG_EQUIP_ID_3_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_4_LAST_CODE)
#define LOG_EQUIP_ID_4_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_5_LAST_CODE)
#define LOG_EQUIP_ID_5_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_6_LAST_CODE)
#define LOG_EQUIP_ID_6_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_7_LAST_CODE)
#define LOG_EQUIP_ID_7_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_8_LAST_CODE)
#define LOG_EQUIP_ID_8_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_9_LAST_CODE)
#define LOG_EQUIP_ID_9_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_10_LAST_CODE)
#define LOG_EQUIP_ID_10_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_11_LAST_CODE)
#define LOG_EQUIP_ID_11_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_12_LAST_CODE)
#define LOG_EQUIP_ID_12_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_13_LAST_CODE)
#define LOG_EQUIP_ID_13_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_14_LAST_CODE)
#define LOG_EQUIP_ID_14_LAST_CODE 0
#endif

#if !defined (LOG_EQUIP_ID_15_LAST_CODE)
#define LOG_EQUIP_ID_15_LAST_CODE 0
#endif


/* -------------------------------------------------------------------------
 * Logging mask implementation details.
 *
 * The logging mask stores a bit for every code within the range specified
 * in log_codes.h.   Each equipment ID has a mask that is represented
 * as an array of bytes.  All of this are listed in an array of bytes
 * of size 'LOG_MASK_SIZE'.  An offset into this array is used to determine
 * the start of the mask associated with a particular equipment ID.
 *
 * The range is inclusive, meaning the beginning (0) and end value
 * ('LOG_EQUIP_ID_xxx_LAST_ITEM') are included in the range.  Therefore, all 
 * equipment IDs have at least 1 byte (range 0-0).
 *
 * 'LOG_MASK_ARRAY_INDEX()' determine the index into the mask for a given
 * equipment ID.
 *
 * 'LOG_MASK_BIT_MASK()' gives the bit of the code within its byte in the
 * mask.
 *
 * 'LOG_GET_EQUIP_ID()' retuns the equipment ID of a given log code.
 * 
 * 'LOG_GET_ITEM_NUM()' returns the item number of a given log code.
 *
 * 'log_mask_enabled()' returns non-zero if a code is enabled.
 *
 * 'log_set_equip_id()' sets the equipment ID in a log code.
 *
 * ------------------------------------------------------------------------- */

#define LOG_MASK_ARRAY_INDEX(xx_item) ((xx_item) >> 3)

#define LOG_MASK_BIT_MASK(xx_item) (0x01 << ((xx_item) & 7))

#define LOG_GET_EQUIP_ID(xx_code) ((((log_code_type) (xx_code)) >> 12) & 0x000F)

#define LOG_GET_ITEM_NUM(xx_code) (((log_code_type) (xx_code)) & 0x0FFF)

/* This computes the number of bytes in the log mask array. */
#define LOG_MASK_SIZE (\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_0_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_1_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_2_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_3_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_4_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_5_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_6_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_7_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_8_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_9_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_10_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_11_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_12_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_13_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_14_LAST_CODE)) +\
  1 + LOG_MASK_ARRAY_INDEX(LOG_GET_ITEM_NUM(LOG_EQUIP_ID_15_LAST_CODE)))


#ifndef LOG_ON_DEMAND_SIZE
 #define  LOG_ON_DEMAND_SIZE  64
#endif

/* Number of bits in the frame number that is inserted into the timestamp,
** and the mask to extract these bits.
*/
#define LOG_FN_NUM_BITS 10
#define LOG_FN_MASK 0x3ff

void diag_switch_log_preset_mask( void );
boolean log_config_mask (int code, boolean  flag, byte stream_id, uint8 preset_id);
void log_config_mstr_mask (int code, boolean  flag, byte stream_id, uint8 preset_id);

void * log_pkt_get_local_equipid_range ( void * req_pkt_ptr, uint16 pkt_len);
void * log_pkt_get_local_masks (void * req_pkt_ptr, uint16 pkt_len);


#endif  /* DIAGLOGI_H */
