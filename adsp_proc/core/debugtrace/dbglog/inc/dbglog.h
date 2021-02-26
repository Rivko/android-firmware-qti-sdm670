#ifndef DBGLOG_H
#define DBGLOG_H

/*=============================================================================

FILE:         dbglog.h

DESCRIPTION:  Wrapper around Uart logging API provided by buses team

===============================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include <stdio.h>
#include "comdef.h"


int dbglog_uart(char *logbuf, int size);


/*
 * UDBG_LOG - Logs a string to UART
 *            Can be used from uImage
 * Parameters
 *     _buffer - string buffer
 *     _size   - size of the string to be logged
 */

#define UDBG_LOG(_buffer,_size)                 \
   do {                                         \
      dbglog_uart(_buffer,_size);               \
      dbglog_uart("\r\n",2);                    \
   }while (0) 


   
/*
 * UDBG_LOG_NO_LINEFEED - Logs the exact string passed to UART
 *                        Caller should take care of including linefeed("\r\n")
 *                               at the end of the string
 *                        Makes exactly one UART call to avoid split messages
 *                        Can be used from uImage
 *                              
 * Parameters
 *     _buffer - string buffer
 *     _size   - size of the string to be logged
 */

#define UDBG_LOG_NO_LINEFEED(_buffer,_size)     \
   do {                                         \
      dbglog_uart(_buffer,_size);               \
   }while (0) 
   
   

/*
 * DBG_LOG - CANNOT BE USED FROM UIMAGE
 *         - Supports printf style logging to UART
 *         
 * Parameters
 *        Similar to printf
 */
#define DBG_LOG_BUF_SIZE 128
#define DBG_LOG(...)                                                 \
   do {                                                              \
      char _log_buf[DBG_LOG_BUF_SIZE];                               \
      int _msg_size=snprintf(_log_buf,DBG_LOG_BUF_SIZE,__VA_ARGS__); \
      UDBG_LOG(_log_buf,_msg_size);                                  \
   }while (0) 



/*
 * U_MSG_ID_LOG - Logs a string message followed by an id
 *              - Can be used from uImage
 *Parameters
 *         msg - A string
 *         id  - A uint32 id
 */

#define U_MSG_ID_LOG(msg, id)                                           \
   do {                                                                 \
      char _log_buf[DBG_LOG_BUF_SIZE];                                  \
      char *_log_start=_log_buf+DBG_LOG_BUF_SIZE-1;                     \
      uint32 evt_id=id;                                                 \
      char *msg_start=msg;                                              \
      char *msg_p;                                                      \
      *_log_start='\n';                                                 \
      --_log_start;                                                     \
      *_log_start='\r';                                                 \
      while(evt_id!=0) {                                                \
         --_log_start;                                                  \
         *_log_start='0'+(evt_id%10);                                   \
         evt_id/=10;                                                    \
      }                                                                 \
      msg_p=msg_start+strlen(msg)-1;                                    \
      while(msg_p>=msg_start) {                                         \
         --_log_start;                                                  \
         *_log_start=*msg_p;                                            \
         --msg_p;                                                       \
      }                                                                 \
      dbglog_uart(_log_start, DBG_LOG_BUF_SIZE-(_log_start-_log_buf));  \
   }while (0) 

#endif //DBGLOG_H
