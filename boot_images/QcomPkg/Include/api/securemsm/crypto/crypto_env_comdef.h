#ifndef UC_COMDEF_H
#define UC_COMDEF_H


/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/securemsm/crypto/crypto_env_comdef.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/12   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdint.h>
#include "string.h"
#include "boot_comdef.h"


typedef enum
{
  /*** Generic outcomes of operations ***/
  UC_E_SUCCESS            =  0,    /* Operation successful                  */
  UC_E_FAILURE            =  1,    /* Operation failed due to unknown err   */
  UC_E_NOT_ALLOWED        =  2,    /* Operation currently not allowed       */
  UC_E_NOT_AVAILABLE      =  3,    /* Operation currently not available     */
  UC_E_NOT_SUPPORTED      =  4,    /* Operation not yet implemented         */
  UC_E_CANCELED           =  5,    /* Operation was scheduled but canceled  */
  UC_E_ABORTED            =  6,    /* Operation was started but interrp'd   */
  UC_E_INTERRUPTED        =  7,    /* Operation was started but interrp'd   */
  UC_E_DEADLOCK           =  8,    /* Operation would cause a deadlock      */
  UC_E_AGAIN              =  9,    /* Caller should retry operation         */
  UC_E_RESET              = 10,    /* Executing module was reset            */
  UC_E_WOULD_BLOCK        = 11,    /* nonblocking IO operation would block  */
  UC_E_IN_PROGRESS        = 12,    /* Operation in progress                 */
  UC_E_ALREADY_DONE       = 13,    /* Operation already completed           */

  /*** Memory management related error conditions ***/
  UC_E_NO_DSM_ITEMS       = 14,    /* Out of DSM items                      */
  UC_E_NO_MEMORY          = 15,    /* Allocation from a memory pool failed  */

  /*** Parameter or data parsing related error conditions ***/
  UC_E_INVALID_ARG        = 16,    /* Arg is not recognized                 */
  UC_E_OUT_OF_RANGE       = 17,    /* Arg value is out of range             */
  UC_E_BAD_ADDRESS        = 18,    /* Ptr arg is bad address                */
  UC_E_NO_DATA            = 19,    /* Expected data, received none          */
  UC_E_BAD_DATA           = 20,    /* Data failed sanity check, e.g. CRC    */
  UC_E_DATA_INVALID       = 21,    /* Data is correct, but contents invalid */
  UC_E_DATA_EXPIRED       = 22,    /* Data is not yet or not any more valid */
  UC_E_DATA_TOO_LARGE     = 23,    /* Data is too large to process          */

  /*** Hardware related error conditions ***/
  UC_E_NO_DEV             = 24,    /* Hardware resource not available       */
  UC_E_DEV_FAILURE        = 25,    /* Hardware failure                      */
  UC_E_NV_READ_ERR        = 26,    /* Operation failed due to NV read err   */
  UC_E_NV_WRITUC_E_ERR       = 27,    /* Operation failed due to NV write err  */
  UC_E_EFS_ERROR          = 28,    /* EFS interface error.                  */
  UC_E_DSP_ERROR          = 29,    /* DSP returned error on cmd (-1)        */

  /*** Protocol operation related error conditions ***/
  UC_E_TIMER_EXP          = 30,    /* Operation not completed (timer exp)   */
  UC_E_VERSION_MISMATCH   = 31,    /* Unexpected software or protocol ver.  */
  UC_E_TASK_SIG_ERR       = 32,    /* Signal unknown or no handler          */
  UC_E_TASK_Q_FULL        = 33,    /* Task queue is full                    */
  UC_E_PROT_Q_FULL        = 34,    /* Protocol queue is full                */
  UC_E_PROT_TX_Q_FULL     = 35,    /* Protocol tx data queue is full        */
  UC_E_PROT_RX_Q_FULL     = 36,    /* Protocol rx data queue is full        */
  UC_E_PROT_UNKN_CMD      = 37,    /* Protocol doesn't understand cmd       */
  UC_E_PROT_UNKN_IND      = 38,    /* Protocol doesn't understand ind       */
  UC_E_PROT_UNKN_MSG      = 39,    /* Protocol doesn't understand msg       */
  UC_E_PROT_UNKN_SIG      = 40,    /* Protocol doesn't understand signal    */

  /*** File related error conditions ***/
  UC_E_NO_ENTRY           = 41,    /* No such file or directory             */
  UC_E_FILENAMUC_E_TOO_LONG  = 42,    /* File name too long                    */
  UC_E_DIR_NOT_EMPTY      = 43,    /* Directory not empty                   */
  UC_E_IS_DIR             = 44,    /* Is a directory                        */
  UC_E_EOF                = 45,    /* End Of File                           */
  UC_E_XDEV               = 46,    /* Attempt to cross device               */
  UC_E_BAD_F              = 47,    /* Bad file descriptor                   */
  UC_E_MAX_OPEN_FILES     = 48,    /* Too many open files                   */
  UC_E_BAD_FMT            = 49,    /* Bad formatted media                   */
  UC_E_FILUC_E_EXISTS        = 50,    /* File already exists                   */
  UC_E_EFS_FULL           = 51,    /* No space left on device               */
  UC_E_NOT_DIR            = 52,    /* Not a directory                       */
  UC_E_NOT_EFS_ITEM       = 53,    /* Not an EFS2 item                      */
  UC_E_ALREADY_OPEN       = 54,    /* File/Directory already open           */
  UC_E_BUSY               = 55,    /* Device/Item is busy                   */
  UC_E_OUT_OF_NODES       = 56,    /* Too many symbolic links encountered   */

  /*** SQL related error conditions ***/
  UC_E_SQL_INTERNAL       = 57,    /* An internal logic error in SQLite     */
  UC_E_SQL_BUSY           = 58,    /* The database file is locked           */
  UC_E_SQL_LOCKED         = 59,    /* A table in the database is locked     */
  UC_E_SQL_READONLY       = 60,    /* Attempt to write a readonly database  */
  UC_E_SQL_IOERR          = 61,    /* Some kind of disk I/O error occurred  */
  UC_E_SQL_CORRUPT        = 62,    /* The database disk image is malformed  */
  UC_E_SQL_NOTFOUND       = 63,    /* Table or record not found             */
  UC_E_SQL_FULL           = 64,    /* Insertion failed because DB is full   */
  UC_E_SQL_CANTOPEN       = 65,    /* Unable to open database file          */
  UC_E_SQL_PROTOCOL       = 66,    /* Database lock protocol error          */
  UC_E_SQL_EMPTY          = 67,    /* Database is empty                     */
  UC_E_SQL_SCHEMA         = 68,    /* The database schema changed           */
  UC_E_SQL_CONSTRAINT     = 69,    /* Abort due to constraint violation     */
  UC_E_SQL_MISMATCH       = 70,    /* Data type mismatch                    */
  UC_E_SQL_MISUSE         = 71,    /* Library used incorrectly              */
  UC_E_SQL_NOLFS          = 72,    /* uses OS features not supported        */
  UC_E_SQL_FORMAT         = 73,    /* Auxiliary database format error       */
  UC_E_SQL_NOTADB         = 74,    /* file opened that is not a db file     */
  UC_E_SQL_ROW            = 75,    /* sqlite3_step() has another row ready  */
  UC_E_SQL_DONE           = 76,    /* sqlite3_step() has finished executing */
  UC_E_NO_ATTR            = 77,    /* No Attr found                         */
  UC_E_INVALID_ITEM       = 78,    /* Invalid Item found                    */
  /*** Networking related error conditions ***/
  UC_E_NO_NET             = 100,   /* Network is not connected (up)         */
  UC_E_NOT_SOCKET         = 101,   /* Socket operation on non-socket        */
  UC_E_NO_PROTO_OPT       = 102,   /* Network protocol not available        */
  UC_E_SHUTDOWN           = 103,   /* Socket is being shut down             */
  UC_E_CONN_REFUSED       = 104,   /* Connection refused by peer            */
  UC_E_CONN_ABORTED       = 105,   /* Connection aborted                    */
  UC_E_IS_CONNECTED       = 106,   /* Connection is already established     */
  UC_E_NET_UNREACH        = 107,   /* Network destination is unreachable    */
  UC_E_HOST_UNREACH       = 108,   /* Host destination is unreachable       */
  UC_E_NO_ROUTE           = 109,   /* No route to destination               */
  UC_E_ADDR_IN_USE        = 110,   /* IP address is already in use          */

  /*** BCMCS related error conditions ***/
  UC_E_DB_INVALID         = 111,   /* Entire database is invalid            */
  UC_E_FLOW_NOT_IN_ZONE   = 112,   /* The flow is not in the specified zone */
  UC_E_ZONUC_E_NOT_IN_DB     = 113,   /* The specified zone is not in database */
  UC_E_BUF_OVERFLOW       = 114,   /* The output buffer is too small        */
  //UC_E_EOF              = 115,   /* End of file is reached                */
  UC_E_DB_OVERFLOW        = 116,   /* The db is full, no further updates    */
  UC_E_DB_NO_OVERWRITE    = 117,   /* value exists and overwrite is false   */
  UC_E_NOT_FOUND          = 118,   /* Data not found                        */
  UC_E_NO_RIGHTS          = 119,   /* No Rights/Permission to access data   */
  UC_E_HASH_VERIF_FAIL    = 120,   /* Hash Verification on DCF Failed       */

  /*** HTTP related error conditions ***/
  UC_E_CONTINUE               = 200,   /* Server response status code of continue */

  UC_E_RESERVED           = 0x7FFFFFFF
} sw_crypto_errno_enum_type;

typedef int ret_type;
/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/* The following definitions are the same accross platforms.  This first
** group are the sanctioned types.
*/

typedef unsigned int boolean_t;

#if 0 // included boot_comdef.h 
#define B_FALSE 0
#define B_TRUE 1

#ifndef _UINT64_DEFINED
  typedef  uint64_t   uint64;      /* Unsigned 64 bit value */
  #define _UINT64_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  uint32_t  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  uint16_t     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  uint8_t      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  int64_t    int64;       /* Signed 64 bit value */
#define _INT64_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  int32_t    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  int16_t       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  int8_t        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

typedef unsigned long           uintnt;
#endif
/* This group are the deprecated types.  Their use should be
** discontinued and new code should use the types above
*/
#if 0
#ifndef _BYTE_DEFINED
typedef  unsigned char     byte;         /* Unsigned 8  bit value type. */
  #define _BYTE_DEFINED
#endif

#ifndef _WORD_DEFINED
typedef  unsigned short    word;         /* Unsigned 16 bit value type. */
  #define _WORD_DEFINED
#endif

#ifndef _UINT1_DEFINED
typedef  unsigned char     uint1;        /* Unsigned 8  bit value type. */
  #define _UINT1_DEFINED
#endif

#ifndef _UINT2_DEFINED
typedef  unsigned short    uint2;        /* Unsigned 16 bit value type. */
  #define _UINT2_DEFINED
#endif

#ifndef _INT1_DEFINED
typedef  signed char       int1;         /* Signed 8  bit value type. */
  #define _INT1_DEFINED
#endif

#ifndef _INT2_DEFINED
typedef  signed short      int2;         /* Signed 16 bit value type. */
  #define _INT2_DEFINED
#endif

#ifndef _INT4_DEFINED
typedef  long int          int4;         /* Signed 32 bit value type. */
  #define _INT4_DEFINED
#endif

#ifndef _SINT15_DEFINED
typedef  signed short      sint15;       /* Signed 16 bit value */
  #define _SINT15_DEFINED
#endif

#ifndef _SINT7_DEFINED
typedef  signed char       sint7;        /* Signed 8  bit value */
  #define _SINT7_DEFINED
#endif
#endif

#ifndef NULL
  #define NULL  0
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define htonll(x) \
((uint64)((((uint64)(x) & 0x00000000000000ffll) << 56) | \
  (((uint64)(x) & 0x000000000000ff00ll) << 40) | \
  (((uint64)(x) & 0x0000000000ff0000ll) << 24) | \
                  (((uint64)(x) & 0x00000000ff000000ll) <<  8) | \
                  (((uint64)(x) & 0x000000ff00000000ll) >>  8) | \
                  (((uint64)(x) & 0x0000ff0000000000ll) >> 24) | \
  (((uint64)(x) & 0x00ff000000000000ll) >> 40) | \
  (((uint64)(x) & 0xff00000000000000ull) >> 56) ))

#define htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

#define htons(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))

#define ntohll(x) \
((uint64)((((uint64)(x) & 0x00000000000000ffll) << 56) | \
  (((uint64)(x) & 0x000000000000ff00ll) << 40) | \
  (((uint64)(x) & 0x0000000000ff0000ll) << 24) | \
                  (((uint64)(x) & 0x00000000ff000000ll) <<  8) | \
                  (((uint64)(x) & 0x000000ff00000000ll) >>  8) | \
                  (((uint64)(x) & 0x0000ff0000000000ll) >> 24) | \
  (((uint64)(x) & 0x00ff000000000000ll) >> 40) | \
  (((uint64)(x) & 0xff00000000000000ull) >> 56) ))

#define ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

#define ntohs(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
        (((uint16)(x) & 0xff00) >> 8)))

#endif // UC_COMDEF_H
