/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Time Services Diagnostics Packet Processing
        T I M E   U N I T   T E S T   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for Time Service operations.

Copyright (c) 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.qdsp6/2.1/services/time/src/time_lpass_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
01/07/09   pc      Created.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include <stringl/stringl.h>
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "memheap.h"
#include "msg.h"
#include "time_diag.h"
#include "timetick.h"
#include "time_timetick.h"
  /* test purpose */
#include "qw.h"

/****************************************************************************\
Example for Command compilation :-
===============================

 - Below is the diag packet req/resp structure for reference :-
   ---------------------------------------------------------
typedef struct
{
  diagpkt_subsys_header_type header;        - Sub System header
  uint16 command;                           - Which command
  uint16 command_data[DIAG_TIME_DATA_SIZE]; - Command parameters
                                              -to be used by function
  uint16 return_stat;                       - Return Status

}TIME_SERVICES_TEST_F_(req_type/resp_type);
  ----------------------------------------------------------

 - Sample Command building:-
   -----------------------
diagpkt_subsys_header_type header (4 bytes)
------------------------------------------------
Command Class is default   - 1 byte  : 0x4B
Subsystem id for Time_Diag - 1 byte  : 0x49(73 for Time Services)
??                         - 2 bytes : 0x00 0x00
------------------------------------------------

uint16 command (2 bytes)
-----------------------------------------------
Command ID                 - 2 bytes: 0x00 0x00 (Eg for TIME_SET)
-----------------------------------------------

uint16 command_data[DIAG_TIME_DATA_SIZE] (14 bytes max)
-----------------------------------------------
0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00 0x14 0x00 0x1E 0x00 0x03 00 - 14 bytes
 # Eg for time_set_julian()
   0xD9 0x07 -> 0x07D9 = 2009 (Year)
   0x02 0x00 -> 0x0002 =    2 (Month)
   0x05 0x00 -> 0x0005 =    5 (Day)
   0x0A 0x00 -> 0x000A =   10 (Hour)
   0x14 0x00 -> 0x0014 =   20 (Mins)
   0x1E 0x00 -> 0x001E =   30 (Secs)
   0x03 0x00 -> 0x0003 =    3 (day of week)
-----------------------------------------------

uint16 return_stat (2 bytes)
----------------------------------------------------------------
Return Status used in response packets "0x00 0x00", if succesful
----------------------------------------------------------------
If
Tx: 0x4B 0x49 0x00 0x00 0x00 0x00 0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00
                                  0x14 0x00 0x1E 0x00 0x01 0x00
then expected
Rx: 0x4B 0x49 0x00 0x00 0x00 0x00 0xD9 0x07 0x02 0x00 0x05 0x00 0x0A 0x00
                                  0x14 0x00 0x1E 0x00 0x01 0x00 0x00 0x00

NOTE : For Commands that donot require any payload, 14 byte command data
       need not be sent
\****************************************************************************/

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

#define LPASS_OFFSET                 0

#define LPASS_TIMER1_CREATE          LPASS_OFFSET+1
#define LPASS_TIMER2_CREATE          LPASS_OFFSET+2
#define LPASS_TIMER3_CREATE          LPASS_OFFSET+3
#define LPASS_TIMER4_CREATE          LPASS_OFFSET+4
#define LPASS_TIMER5_CREATE          LPASS_OFFSET+5
#define LPASS_TIMER6_PRINT_ERR       LPASS_OFFSET+6

extern qurt_anysignal_t qurt_timer_test_signal;
extern void qurt_print_error_messages(void);

/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/



/*===========================================================================

 FUNCTION TIME_SERVICES_TEST

DESCRIPTION
 This function would test the Time services.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/
PACK(void *) time_services_test 
(
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  TIME_SERVICES_TEST_F_req_type *req =(TIME_SERVICES_TEST_F_req_type *) req_pkt;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* TIME_SERVICES_TEST_F_rsp_type *rsp; */
  const int rsp_len = sizeof( TIME_SERVICES_TEST_F_rsp_type );

  PACK(void *) rsp_ptr = NULL;

  /* Allocate the same length as the request. */
  rsp_ptr = (TIME_SERVICES_TEST_F_rsp_type *)diagpkt_subsys_alloc
                                         ( DIAG_SUBSYS_TIME,
                                           TIME_SERVICES_TEST_F,
                                           rsp_len );

  if (rsp_ptr != NULL)
  {
    memscpy ((void *) rsp_ptr, pkt_len, (void *) req_pkt, pkt_len);
  }

  switch ( req->command )
  {

    /*normal creation and deletion of timer*/
    case LPASS_TIMER1_CREATE :

    qurt_anysignal_set(&qurt_timer_test_signal,LPASS_TIMER1_CREATE);

    break;

     /*creating an absolute timer*/
     case LPASS_TIMER2_CREATE:

    qurt_anysignal_set(&qurt_timer_test_signal,LPASS_TIMER2_CREATE);  

    break;

     case LPASS_TIMER3_CREATE:

     qurt_anysignal_set(&qurt_timer_test_signal,0x4);    

     break;

  case LPASS_TIMER4_CREATE:

     qurt_anysignal_set(&qurt_timer_test_signal,0x8);

      break;

  case LPASS_TIMER5_CREATE:
      qurt_anysignal_set(&qurt_timer_test_signal,0x10);
      break;
  case LPASS_TIMER6_PRINT_ERR:
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Not doing anything ");
      break;
  default :
      MSG(MSG_SSID_ATS, MSG_LEGACY_HIGH,"LPASS_TIMER_DEBUG_time_lpass_diag: Error in command");
     break;
  }

  (void)pkt_len;

  return (rsp_ptr);

} /* time_services_test */


/*===========================================================================

              FUNCTION DISPATCH TABLE FOR DIAG COMMANDS

DESCPRITIION  
  TIME_SERVICES_TEST_F needs to be defined to a value in diagcmd.h if
no subsys ID is used to register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static const diagpkt_user_table_entry_type time_services_test_tbl[] =
{
  {TIME_SERVICES_TEST_F,TIME_SERVICES_TEST_F,time_services_test}
};


/*===========================================================================

FUNCTION TIME_SERVICES_TEST_INIT

DESCRIPTION
  Registers the diag packet function dispatch table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void time_services_test_init (void)
{
 /* Could use the following registration without specifying the processor */
    //DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_TIME, time_services_test_tbl);
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_QDSP6_PROC, DIAG_SUBSYS_TIME,  
        time_services_test_tbl);

}

