#ifndef ERRNO_CODES_H
#define ERRNO_CODES_H
/**
  @file errno_codes.h
  @brief This file contains the definition of the return codes (error numbers).

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_error_codes" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

               E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This file contains the definition of the return codes (error numbers).
 

Copyright (c) 2002-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/services/errno_codes.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
04/30/03    gr     Updated with more codes.
04/24/02    gr     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/
/** @addtogroup utils_error_codes
@{ */

/*---------------------------------------------------------------------------
  #undef the macros defined here to prevent a redefinition warning in case
  of collision with definitions in system header files.
---------------------------------------------------------------------------*/
#undef EEOF
#undef EINTR
#undef EBADF
#undef EFAULT
#undef EWOULDBLOCK
#undef EAGAIN
#undef EAFNOSUPPORT
#undef EPROTOTYPE
#undef ESOCKNOSUPPORT
#undef EPROTONOSUPPORT
#undef EMFILE
#undef EOPNOTSUPP
#undef EADDRINUSE
#undef EDESTADDRREQ
#undef EINPROGRESS
#undef EISCONN
#undef EIPADDRCHANGED
#undef ENOTCONN
#undef ECONNREFUSED
#undef ETIMEDOUT
#undef ECONNRESET
#undef ECONNABORTED
#undef EPIPE
#undef ENETDOWN
#undef EMAPP
#undef EBADAPP
#undef SOCKEXIST
#undef EINVAL
#undef ENOMEMORY
#undef ENOMEM
#undef ENOENT
#undef ESRCH
#undef EHOSTNOTFOUND
#undef ETRYAGAIN
#undef ENORECOVERY
#undef ENOADDRESS
#undef EMSGTRUNC
#undef ENOROUTE
#undef ESHUTDOWN
#undef E2BIG
#undef ENOSYS
#undef EMSGSIZE
#undef EALREADY
#undef EDEADLK
#undef ENOLINK
#undef EIDRM
#undef EEXIST
#undef EUSERS
#undef EOVERFLOW
#undef EISNAM
#undef ELOOP
#undef EISDIR
#undef ENODEV
#undef ENOTEMPTY
#undef ESPIPE
#undef ENOPROTOOPT
#undef ENETISCONN
#undef ENETINPROGRESS
#undef ENONET
#undef ENETCLOSEINPROGRESS
#undef ENETEXIST
#undef ENETUNREACH
#undef ENOTDIR
#undef ENODATA
#undef EFBIG
#undef EADDRNOTAVAIL
#undef EROFS
#undef EACCES
#undef ENAMETOOLONG
#undef ENOSPC
#undef EHOSTUNREACH
#undef ENOBUFS
#undef ENFILE
#undef ETIME
#undef EBADFD
#undef EMLINK
#undef ENOTSOCK
#undef EBUSY
#undef ENETRESET
#undef ECOMM
#undef EDQUOT
#undef EPROTO
#undef EHOSTDOWN
#undef EPERM
#undef EIO
#undef ENXIO
#undef EDEADLOCK
#undef ENONET

/* This set of errno codes have values that match the values in ds_errno.h.
*/
#define EEOF                   0      /**< End of the file. */
#define EINTR                 99      /**< Interruption during a system call
                                           produced a fault. */
#define EBADF                100      /**< Invalid socket descriptor. */
#define EFAULT               101      /**< Invalid buffer or argument. */
#define EWOULDBLOCK          102      /**< Operation would block. */
/* @cond */
#define EAGAIN               EWOULDBLOCK
/* @endcond */
#define EAFNOSUPPORT         103      /**< Address family is not supported. */
#define EPROTOTYPE           104      /**< Wrong protocol for the socket 
                                           type. */
#define ESOCKNOSUPPORT       105      /**< Socket parameter is not supported. */
#define EPROTONOSUPPORT      106      /**< Protocol is not supported. */
#define EMFILE               107      /**< No more sockets are available. */
#define EOPNOTSUPP           108      /**< Operation is not supported. */
#define EADDRINUSE           109      /**< Address is already in use. */
#define EDESTADDRREQ         110      /**< Destination address is required. */
#define EINPROGRESS          111      /**< Connect is in progress. */
#define EISCONN              112      /**< Connection is already 
                                           established. */
#define EIPADDRCHANGED       113      /**< IP address is changed, causing a 
                                           TCP reset. */
#define ENOTCONN             114      /**< Socket is not connected. */
#define ECONNREFUSED         115      /**< Connection attempt was refused. */
#define ETIMEDOUT            116      /**< Connection attempt is timed out. */
#define ECONNRESET           117      /**< Connection was reset. */
#define ECONNABORTED         118      /**< Connection was aborted. */
#define EPIPE                119      /**< Broken pipe. */
#define ENETDOWN             120      /**< Network is unavailable. */
#define EMAPP                121      /**< No more applications are 
                                           available. */
#define EBADAPP              122      /**< Invalid application ID. */
#define SOCKEXIST            123      /**< Sockets already exist. */
#define EINVAL               124      /**< Invalid operation. */

/*---------------------------------------------------------------------------
                          DNS Lookup Error Messages
---------------------------------------------------------------------------*/
/** @name DNS Lookup Error Messages
@{ */
#define ENAMEERR             125      /**< Domain name error. */
#define ENAMEMISS            126      /**< Domain name was not found. */
#define ENETERR              127      /**< Network is not opened. */
#define ENOMEMORY            128      /**< Out of memory. */
#define EINUSE               129      /**< Resolver is in use. */
#define ENOMEM         ENOMEMORY      /**< Out of memory. */
/** @} */ /* end_name_group DNS Lookup Error Messages */

/*---------------------------------------------------------------------------
                        dss_sendto() Error Conditions
---------------------------------------------------------------------------*/
#define EMSGSIZE             130     /**< Message is too large. */

/*---------------------------------------------------------------------------
                            Socket Option Errors
---------------------------------------------------------------------------*/
#define ENOPROTOOPT          131  /**< Option is unknown at the indicated 
                                       level. */

/*---------------------------------------------------------------------------
                        More DNS Lookup Error Messages
---------------------------------------------------------------------------*/
/** @name DNS Lookup Error Messages
@{ */
#define EHOSTNOTFOUND        ENAMEERR /**< Host was not found. */
#define ETRYAGAIN            EINUSE   /**< Temporary error. */
#define ENORECOVERY          132      /**< Unrecoverable error. */
#define ENOADDRESS           133      /**< No address for the domain name. */
/** @} */ /* end_name_group DNS Lookup Error Messages */

/*---------------------------------------------------------------------------
                    More Transport Error Condition Values
---------------------------------------------------------------------------*/
#define EMSGTRUNC            134  /**< Message was truncated; the supplied 
                                       buffer is too small. */

/*---------------------------------------------------------------------------
                      IP Routing Error Condition Values
---------------------------------------------------------------------------*/
#define ENOROUTE             135  /**< No route to the destination was 
                                       found. */

/*---------------------------------------------------------------------------
                       Shutdown Error Condition Values
---------------------------------------------------------------------------*/
#define ESHUTDOWN            136      /**< Cannot send after a transport
                                           endpoint shutdown. */

/*---------------------------------------------------------------------------
                           Network Subsystem Errors
---------------------------------------------------------------------------*/
/** @name Network Subsystem Errors
@{ */
#define ENETISCONN           200 /**< Subsystem is established and 
                                      available. */
#define ENETINPROGRESS       201 /**< Subsystem establishment is in 
                                      progress. */
#define ENONET               202 /**< Network is unavailable. */
#define ENETCLOSEINPROGRESS  203 /**< Point-to-Point Protocol is closing. */
#define ENETEXIST            204 /**< Network subsystem resources already 
                                      exist. */
/** @} */ /* end_name_group Network Subsystem Errors */

/* These errno codes do not exist in ds_errno.h.
*/
#define ENOENT               800      /**< No such file or directory. */
#define E2BIG                801      /**< Argument list is too long. */
#define ENOSYS               802      /**< Function is not implemented. */
#define EALREADY             803      /**< Operation is already in progress. */
#define EDEADLK              804      /**< Resource deadlock will occur. */
#define ENOLINK              805      /**< Link has been severed. */
#define EEXIST               806      /**< File exists. */
#define EUSERS               807      /**< Too many users. */
#define EOVERFLOW            808      /**< Value is too large for the defined
                                           data type. */
#define EISNAM               809      /**< Is a named type file. */
#define ELOOP                810      /**< Too many symbolic links were 
                                           encountered. */
#define EISDIR               811      /**< Is a directory. */
#define ENODEV               812      /**< No such device. */
#define ENOTEMPTY            813      /**< Directory is not empty. */
#define ESPIPE               814      /**< Illegal seek. */
#define ENETUNREACH          815      /**< Network is unreachable. */
#define ENOTDIR              816      /**< Not a directory. */
#define ENODATA              817      /**< No data is available. */
#define EFBIG                818      /**< File is too large. */
#define EADDRNOTAVAIL        819      /**< Cannot assign the requested 
                                           address. */
#define EROFS                820      /**< Read-only file system. */
#define EACCES               821      /**< Permission is denied. */
#define ENAMETOOLONG         822      /**< File name is too long. */
#define ENOSPC               823      /**< No space is left on the device. */
#define EHOSTUNREACH         824      /**< No route to the host. */
#define ENOBUFS              825      /**< No buffer space is available. */
#define ENFILE               826      /**< File table overflow occurred. */
#define ETIME                827      /**< Timer has expired. */
#define EBADFD               828      /**< File descriptor is in a bad state. */
#define EMLINK               829      /**< Too many links. */
#define ENOTSOCK             830      /**< Socket operation is on a 
                                           nonsocket. */
#define EBUSY                831      /**< Device or resource is busy. */
#define ENETRESET            832      /**< Network dropped the connection
                                           because of a reset. */
#define ECOMM                833      /**< Communication error occurred on 
                                           send. */
#define EDQUOT               834      /**< Quota was exceeded. */
#define EPROTO               835      /**< Protocol error. */
#define EHOSTDOWN            836      /**< Host is down. */
#define EPERM                837      /**< Operation is not permitted. */
#define EIO                  838      /**< I/O error. */
#define ENXIO                839      /**< No such device or address. */
/** @cond */
#define EDEADLOCK            EDEADLK
/** @endcond */

/** @} */ /* end_addtogroup utils_error_codes */
#endif /* ERRNO_CODES_H */

