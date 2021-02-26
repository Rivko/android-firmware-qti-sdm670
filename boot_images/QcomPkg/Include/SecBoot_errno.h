
/** @file SecBoot_errno.h

DESCRIPTION:
  This contains the definition of the return codes (error numbers).
  Functions using this definition either return an error code, or set
  a global variable to the appropriate value.


  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
**/

/*===========================================================================

                         EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef USE_ARM
//     #include "pbl_error_handler.h"
#endif

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/
#ifndef SECBOOT_ERRNO_H
#define SECBOOT_ERRNO_H

typedef enum
{
  /*** Generic outcomes of operations ***/
  E_SUCCESS            =  0,    /* Operation successful                  */
  E_FAILURE            =  1,    /* Operation failed due to unknown err   */
  E_NOT_ALLOWED        =  2,    /* Operation currently not allowed       */
  E_NOT_AVAILABLE      =  3,    /* Operation currently not available     */
  E_NOT_SUPPORTED      =  4,    /* Operation not yet implemented         */
  E_CANCELED           =  5,    /* Operation was scheduled but canceled  */
  E_ABORTED            =  6,    /* Operation was started but interrp'd   */
  E_INTERRUPTED        =  7,    /* Operation was started but interrp'd   */
  E_DEADLOCK           =  8,    /* Operation would cause a deadlock      */
  E_AGAIN              =  9,    /* Caller should retry operation         */
  E_RESET              = 10,    /* Executing module was reset            */
  E_WOULD_BLOCK        = 11,    /* nonblocking IO operation would block  */
  E_IN_PROGRESS        = 12,    /* Operation in progress                 */
  E_ALREADY_DONE       = 13,    /* Operation already completed           */

  /*** Memory management related error conditions ***/
  E_NO_DSM_ITEMS       = 14,    /* Out of DSM items                      */
  E_NO_MEMORY          = 15,    /* Allocation from a memory pool failed  */

  /*** Parameter or data parsing related error conditions ***/
  E_INVALID_ARG        = 16,    /* Arg is not recognized                 */
  E_OUT_OF_RANGE       = 17,    /* Arg value is out of range             */
  E_BAD_ADDRESS        = 18,    /* Ptr arg is bad address                */
  E_NO_DATA            = 19,    /* Expected data, received none          */
  E_BAD_DATA           = 20,    /* Data failed sanity check, e.g. CRC    */
  E_DATA_INVALID       = 21,    /* Data is correct, but contents invalid */
  E_DATA_EXPIRED       = 22,    /* Data is not yet or not any more valid */
  E_DATA_TOO_LARGE     = 23,    /* Data is too large to process          */

  /*** Hardware related error conditions ***/
  E_NO_DEV             = 24,    /* Hardware resource not available       */
  E_DEV_FAILURE        = 25,    /* Hardware failure                      */
  E_NV_READ_ERR        = 26,    /* Operation failed due to NV read err   */
  E_NV_WRITE_ERR       = 27,    /* Operation failed due to NV write err  */
  E_EFS_ERROR          = 28,    /* EFS interface error.                  */
  E_DSP_ERROR          = 29,    /* DSP returned error on cmd (-1)        */

  /*** Protocol operation related error conditions ***/
  E_TIMER_EXP          = 30,    /* Operation not completed (timer exp)   */
  E_VERSION_MISMATCH   = 31,    /* Unexpected software or protocol ver.  */
  E_TASK_SIG_ERR       = 32,    /* Signal unknown or no handler          */
  E_TASK_Q_FULL        = 33,    /* Task queue is full                    */
  E_PROT_Q_FULL        = 34,    /* Protocol queue is full                */
  E_PROT_TX_Q_FULL     = 35,    /* Protocol tx data queue is full        */
  E_PROT_RX_Q_FULL     = 36,    /* Protocol rx data queue is full        */
  E_PROT_UNKN_CMD      = 37,    /* Protocol doesn't understand cmd       */
  E_PROT_UNKN_IND      = 38,    /* Protocol doesn't understand ind       */
  E_PROT_UNKN_MSG      = 39,    /* Protocol doesn't understand msg       */
  E_PROT_UNKN_SIG      = 40,    /* Protocol doesn't understand signal    */

  /*** Networking related error conditions ***/
  E_NO_NET             = 100,   /* Network is not connected (up)         */
  E_NOT_SOCKET         = 101,   /* Socket operation on non-socket        */
  E_NO_PROTO_OPT       = 102,   /* Network protocol not available        */
  E_SHUTDOWN           = 103,   /* Socket is being shut down             */
  E_CONN_REFUSED       = 104,   /* Connection refused by peer            */
  E_CONN_ABORTED       = 105,   /* Connection aborted                    */
  E_IS_CONNECTED       = 106,   /* Connection is already established     */
  E_NET_UNREACH        = 107,   /* Network destination is unreachable    */
  E_HOST_UNREACH       = 108,   /* Host destination is unreachable       */
  E_NO_ROUTE           = 109,   /* No route to destination               */
  E_ADDR_IN_USE        = 110,   /* IP address is already in use          */
  E_INVALID_TAG        = 111,
  E_UNUSED                         = 0x7FFFFFFF
} errno_enum_type;


#endif /* SECBOOT_ERRNO_H */

