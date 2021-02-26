#ifndef _QSOCKET_H_
#define _QSOCKET_H_
/******************************************************************************
  @file    qsocket.h
  @brief   Generic Socket-like interface

  DESCRIPTION

  Provides a generic socket like interface currently used for IPC Router.
  If this becomes a generic interface the documentation must change 
  appropriately

  Certain Operating systems might need special setup. Please refer
  to qsocket_<os>.h if it exists
  
  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/
#include "stdlib.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
                            SOCKET TYPES
============================================================================*/

/* Datagram data socket */
#define QSOCK_DGRAM  1
/* Stream-like socket */

#define QSOCK_STREAM 2

/*============================================================================
                          SOCKET SEND/RECV FLAGS
============================================================================*/

/* Refrain from reading the next available packet but return the next
 * packet size instead. */
#define QMSG_PEEK      1

/* Send will block on flow control, recv will block if there
 * are no packets to receive */
#define QMSG_DONTWAIT  2

/* Truncate the message if the buffer it too long and return
 * the full length of the buffer */
#define QMSG_TRUNC     4

/*============================================================================
                          SOCKET OPTION LEVEL DEFINES
============================================================================*/
/* Generic non-protocol specific socket options */
#define QSOL_SOCKET          1

/*============================================================================
                          SOCKET OPTION DEFINES
  Generic non-protocol specific options valid with LEVEL = QSOL_SOCKET
============================================================================*/

/* Set the read timeout. Option value is of type 'int' containing 
 * the timeout for reads in milli-seconds.
 * 0 - Non-blocking writes by default.
 * -1 - Block for infinity
 * x>0 - block for x milli-seconds */
#define QSO_RCVTIMEO         1

/* Set the write timeout. Option value is of type 'int' containing 
 * the timeout for writes in milli-seconds.
 * 0 - Non-blocking writes by default.
 * -1 - Block for infinity
 * x>0 - block for x milli-seconds */
#define QSO_SNDTIMEO         2


/*============================================================================
                            SHUTDOWN TYPES
============================================================================*/
/* Close RX path for the socket */
#define QSHUT_RD              0

/* Close TX for the socket */
#define QSHUT_WR              1

/* Close both RX and TX of the socket */
#define QSHUT_RDWR            2

/*============================================================================
                            POLL EVENT TYPES
============================================================================*/

/* Input event */
#define QPOLLIN    1

/* Output event -- Unused, provided for completeness */
#define QPOLLOUT   2

/* Remote side has hungup on this port */
#define QPOLLHUP   4

/* Invalid parameters/arguments/operations */
#define QPOLLNVAL 8

/*============================================================================
                                ERROR CODES
============================================================================*/
/* Bad socket descriptor */
#define QEBADF                     -1
/* The connection is reset */
#define QECONNRESET                -2
/* This is not a connected socket and a destination is required */
#define QEDESTADDRREQ              -3 
/* Generic access fault */
#define QEFAULT                    -4
/* Invalid parameters */
#define QEINVAL                    -5
/* Is a connected socket but an addr was provided */
#define QEISCONN                   -6
/* message size too large to be transmitted as one chunk */
#define QEMSGSIZE                  -7
/* No TX queue buffer remaining */
#define QENOBUFS                   -8
/* Memory allocation failed */
#define QENOMEM                    -9
/* Socket is not connected and no target was provided */
#define QENOTCONN                  QEDESTADDRREQ
/* This operation or flags are not supported */
#define QENOTSUPP                  -11
/* Transmission would require the call to block */
#define QEAGAIN                    -12
/* Operation would block but the user expected it to not block */
#define QEWOULDBLOCK               QEAGAIN
/* The recv was refused as the buffer size was insufficient */
#define QEWOULDTRUNC               -13
/* Address Family is not supported  */
#define QEAFNOTSUPP                -14
#define QEAFNOSUPPORT              QEAFNOTSUPP
/* Operation not supported */
#define QEOPNOTSUPP                -15
/* Operating timed out */
#define QETIMEDOUT                 -16
/* Host is unreachable */
#define QEHOSTUNREACH              -17
/* Access error */
#define QEACCES                    -18
/* No message available to receive */
#define QENOMSG                    -19

/*============================================================================
                             TYPES
============================================================================*/

struct qsockaddr {
  unsigned short sa_family;
  char sa_data[14];
};

typedef size_t qsocklen_t;


typedef size_t qnfds_t;

/* Input to pollfd */
struct qpollfd {
  int fd;      /* File descriptor */
  int events;  /* Interested events */
  int revents; /* Returned events */
};

/*============================================================================
                        FUNCTION PROTOTYPES
============================================================================*/

/*===========================================================================
  FUNCTION  qsocket
===========================================================================*/
/*!
@brief

  This function returns a socket descriptor

@param[in]   domain    The address family of the socket (Currently only
                       AF_IPC_ROUTER is supported)
@param[in]   type      Type of socket. See SOCKET TYPES for more info
@param[in]   protocol  Family specific protocol (Currently unused)

@return
  Positive socket descriptor upon success, negative error code on failure.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int qsocket(int domain, int type, int protocol);

/*===========================================================================
  FUNCTION  qconnect
===========================================================================*/
/*!
@brief

  Connect the socket to the specified destination endpoint. Considering 
  IPC Router is a connection-less protocol, this does not result in
  an actual connection, but just associates the socket to the remote
  endpoint. Thus, if the remote endpoint terminates, the sender will
  detect a failure.

@param[in]   fd        Socket descriptor
@param[in]   addr      Address of the destination endpoint
@param[in]   addrlen   Length of the address

@return
  0 on success, negative error code on failure.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int qconnect(int fd, struct qsockaddr *addr, qsocklen_t addrlen);

/*===========================================================================
  FUNCTION  qbind
===========================================================================*/
/*!
@brief

  Binds a socket to a specific name.
  Since all ports in IPC Router are ephemeral ports, it is not allowed
  to bind to a specified physical port ID (Like UDP/IP).

@param[in]   fd        Socket descriptor
@param[in]   addr      Name to bind with the socket
@param[in]   addrlen   Length of the address

@return
  0 on success, negative error code on failure.

@note

  - Dependencies
    - Note that the 'type' field of the NAME _must_ be allocated and reserved
      by the Qualcomm Interface Control board else it is highly likely that
      the users might be in conflict with names allocated to other users.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qbind(int fd, struct qsockaddr *addr, qsocklen_t addrlen);

/*===========================================================================
  FUNCTION  qsendto
===========================================================================*/
/*!
@brief

  Sends a message to a specific remote address

@param[in]   fd        Socket descriptor
@param[in]   buf       Pointer to the buffer
@param[in]   len       Length of the buffer
@param[in]   flags     Flags - See SOCKET SEND/RECV FLAGS for more info
@param[in]   addr      Address of the destination
@param[in]   addrlen   Length of the address

@return
  Transmitted length on success, negative error code on failure.
  If the destination is flow controlled, and QMSG_DONTWAIT is
  set in flags, then the function can return QEAGAIN

  If the address is of type IPCR_ADDR_NAME, the message is multicast
  to all ports which has bound to the specified name.

@note
  - Dependencies
    - None

  - Retry logic (After receiving an error of QEAGAIN)
    - If QMSG_DONTWAIT is set in the flags, the user must retry
      after blocking and receiving the:
      * QPOLLOUT event (In the case of a connected socket)'
      * QPOLLIN event in the case of a unconnected socket and
        qrecvfrom() returns a zero length message with the unblocked
        destination in the output address.

  - Side Effects
    - Function can block.
*/
/*=========================================================================*/
int qsendto(int fd, void *buf, size_t len, int flags, 
             struct qsockaddr *addr, qsocklen_t addrlen);

/*===========================================================================
  FUNCTION  qsend
===========================================================================*/
/*!
@brief

  Sends a message to the connected address

@param[in]   fd        Socket descriptor
@param[in]   buf       Pointer to the buffer
@param[in]   len       Length of the buffer
@param[in]   flags     Flags - See SOCKET SEND/RECV FLAGS for more info

@return
  Transmitted length on success, negative error code on failure.
  If the destination is flow controlled, and QMSG_DONTWAIT is
  set in flags, then the function can return QEAGAIN

  If the address is of type IPCR_ADDR_NAME, the message is multicast
  to all ports which has bound to the specified name.

@note
  - Dependencies
    - None

  - Retry logic (After receiving an error of QEAGAIN)
    - If QMSG_DONTWAIT is set in the flags, the user must retry
      after blocking and receiving the:
      * QPOLLOUT event (In the case of a connected socket)'
      * QPOLLIN event in the case of a unconnected socket and
        qrecvfrom() returns a zero length message with the unblocked
        destination in the output address.

  - Dependencies
    - qconnect() must have been called to associate this socket
      with a destination address

  - When the connected endpoint terminates (normally or abnormally), the
    function would unblock and return QENOTCONN/QEDESTADDRREQ.

  - Side Effects
    - Function can block.
*/
/*=========================================================================*/
int qsend(int fd, void *buf, size_t len, int flags);

/*===========================================================================
  FUNCTION  qrecvfrom
===========================================================================*/
/*!
@brief

  Receives a message from a remote address

@param[in]   fd        Socket descriptor
@param[in]   buf       Pointer to the buffer
@param[in]   len       Length of the buffer
@param[in]   flags     Flags - See SOCKET SEND/RECV FLAGS for more info
@param[out]  addr      Address of the sender
@param[inout]addrlen   Input: Size of the passed in buffer for address
                       Output: Size of the address filled by the framework.
                       (Can be NULL if addr is also NULL).

@return
  Received packet size in bytes, negative error code in failure.

@note

  - Dependencies
    - None

  - Retry logic
    - If QMSG_DONTWAIT is set in the flags, the user must retry
      after blocking and receiving the QPOLLIN event using qpoll()

  - Side Effects
    - None
*/
/*=========================================================================*/
int qrecvfrom(int fd, void *buf, size_t len, int flags, 
                struct qsockaddr *addr, qsocklen_t *addrlen);

/*===========================================================================
  FUNCTION  qrecv
===========================================================================*/
/*!
@brief

  Receives a message from the connected address address

@param[in]   fd        Socket descriptor
@param[in]   buf       Pointer to the buffer
@param[in]   len       Length of the buffer
@param[in]   flags     Flags - See SOCKET SEND/RECV FLAGS for more info

@return
  Received packet size in bytes, negative error code in failure.

@note

  - Dependencies
    - qconnect() must have been called to associate this socket
      with a destination address

  - Retry logic
    - If QMSG_DONTWAIT is set in the flags, the user must retry
      after blocking and receiving the QPOLLIN event using qpoll()

  - When the connected endpoint terminates (normally or abnormally), the
    function would unblock and return QENOTCONN/QEDESTADDRREQ.

  - Side Effects
    - None
*/
/*=========================================================================*/
int qrecv(int fd, void *buf, size_t len, int flags);

/*===========================================================================
  FUNCTION  qgetsockopt
===========================================================================*/
/*!
@brief

  Gets an option value

@param[in]   fd        Socket descriptor
@param[in]   level     Level of the option. Currently the only supported
                       level is QSOL_IPC_ROUTER
@param[in]   optname   Option name to get
@param[in]   optval    Buffer to place the option
@param[inout]optlen    In: Length of the buffer passed into qgetsockopt
                       Out: Length of the filled in options

@return
  0 on success, negative error code on failure.

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int qgetsockopt(int fd, int level, int optname, void *optval, qsocklen_t *optlen);

/*===========================================================================
  FUNCTION  qsetsockopt
===========================================================================*/
/*!
@brief

  Sets an option on a socket

@param[in]   fd        Socket descriptor
@param[in]   level     Level of the option. Currently the only supported
                       level is QSOL_IPC_ROUTER
@param[in]   optname   Option name to get
@param[in]   optval    Buffer to place the option
@param[in]   optlen    Length of the buffer passed into qsetsockopt

@return
  0 on success, negative error code on failure.

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int qsetsockopt(int fd, int level, int optname, void *optval, qsocklen_t optlen);

/*===========================================================================
  FUNCTION  qpoll
===========================================================================*/
/*!
@brief

  Blocks on requested events on the provided socket descriptors

@param[in]   pfd       Array of poll info (See qpollfd for more info)
@param[in]   num       Number of sockets to wait on (Len of pfd array)
@param[in]   timeout_ms Timeout in milli-seconds:
                        -1 = Infinite
                        0  = poll, return immediately if there are no events
                        val > 0, timeout

@return
  Total number of socket descriptors which have events on them on success
  0 if there were no events, and the function unblocked after the timeout.
  Negative error code on failure.

@note

  - Dependencies
    - None

  - Side Effects
    - Blocks waiting for events
*/
/*=========================================================================*/
int qpoll(struct qpollfd *pfd, qnfds_t num, int timeout_ms);

/*===========================================================================
  FUNCTION  qclose
===========================================================================*/
/*!
@brief

  Closes the socket

@param[in]   fd        Socket descriptor

@return
  0 on success, negative error code on failure

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int qclose(int fd);

/*===========================================================================
  FUNCTION  qshutdown
===========================================================================*/
/*!
@brief

  Shuts down a socket

@param[in]   fd        Socket descriptor
@param[in]   how       QSHUTRD (or 0) - Stop receiving packets
                       QSHUTWR (or 1) - Stop transmitting packets
                       QSHUTRDWR (or 2) - Stop both receiving and transmitting

@return
  0 on success  negative error code on failure.

@note

  - Dependencies
    - None

  - Side Effects
    - shutting down both RD and WR will have the same effect as qclose()
*/
/*=========================================================================*/
int qshutdown(int fd, int how);


#ifdef __cplusplus
}
#endif

#endif
