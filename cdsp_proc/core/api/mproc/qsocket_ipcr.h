#ifndef _QSOCKET_IPCR_H_
#define _QSOCKET_IPCR_H_
/******************************************************************************
  @file    qsocket_ipcr.h
  @brief   IPC Router protocol specific definitions

  DESCRIPTION

  IPC Router: 
  IPC Router is a connection-less datagram protocol covering the Network
  and Transport layers of the OSI model. It provides a packet delimited
  end-to-end flow control. Any streaming functionalities provided by
  IPC Router are implemented on top of the datagram methods. IPC Router
  also provides a reliable transport to users as it expects reliability
  from it's link layer.

  IPC Router provides a control endpoint to allow services/clients to
  listen on events from the distributed name service.

  IPC Router also provides a distributed name service in order to provide
  location transparency to it's clients

  This header provides a set of socket-like API to to communicate
  over IPC Router.

  Certain Operating systems might need special setup. Please refer
  to qsocket_<os>.h if it exists
  
  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/
#include "qsocket.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
                            FAMILY
============================================================================*/
#define AF_IPC_ROUTER    27

/*============================================================================
                          SOCKET OPTION DEFINES
 IPC Router protocol specific socket options (LEVEL = QSOL_IPC_ROUTER)
============================================================================*/

/* IPC Router specific socket options */
#define QSOL_IPC_ROUTER      0

/* Sets the current socket as a control port so it
 * may receive events from the name service */
#define QSO_IPCR_SET_CONTROL_PORT   1

/* This option is valid only if the QSO_IPCR_SET_CONTROL_PORT
 * option is set on the socket. This will limit the name server
 * events such as NEW_NAME and REMOVE_NAME to a specific type.
 * The option value is a 32 bit integer (uint32_t) which will
 * specify the 'service' field of the name we are interested in */
#define QSO_IPCR_SET_SERVICE_PREF      2

/*============================================================================
                         SOCKET GETADDRINFO FLAGS
============================================================================*/

/* Flags to ipcr_getaddrinfo which asks the interface
 * to ignore the provided instance field of the name to lookup */
#define IPCR_FLAGS_ANY_INSTANCE  1

/*============================================================================
                             TYPES
============================================================================*/

/* NAME bound by an IPC Router socket */
typedef struct {
  uint32_t service;
  uint32_t instance;
} ipcr_name_t;

/* PORT ADDRESS of an IPC Router socket */
typedef struct {
  uint32_t node_id; /* Processor ID */
  uint32_t port_id; /* Port ID */
} ipcr_port_t;

/* TYPE Of address (non-specific name or port)  */
#define IPCR_ADDR_PORT     0
#define IPCR_ADDR_NAME     1
#define IPCR_ADDR_SNAME    2

/* IPC Router protocol address type */
struct qsockaddr_ipcr {
  unsigned short sa_family; /* IPCR_FAMILY */
  struct ipcr_addr {
    unsigned char addrtype; /* IPCR_ADDR_PORT or IPCR_ADDR_NAME */
    union {
      ipcr_port_t port_addr; /* Phy address */
      ipcr_name_t port_name; /* Name */
      char        port_sname[32]; /* String name */
    } addr;
  } address; /* IPCR Address type */
  unsigned char reserved;
};

/* Type returned by name lookups */
typedef struct {
  ipcr_name_t name;
  ipcr_port_t port;
} ipcr_addrinfo_t;

/* Types of events which can be received
 * from the CTRL socket */
#define IPCR_CTRL_NEW_SERVICE    4
#define IPCR_CTRL_REMOVE_SERVICE 5
#define IPCR_CTRL_REMOVE_PORT    6

/* Control message format received from
 * the control socket */
typedef union {
  uint32_t   msg_type;
  struct {
    uint32_t   msg_type;
    uint32_t   service;
    uint32_t   instance;
    uint32_t   node_id;
    uint32_t   port_id;
  } srv;
  struct {
    uint32_t   msg_type;
    uint32_t   node_id;
    uint32_t   port_id;
  } cli;
} ipcr_ctrl_msg;


/*============================================================================
               PROTOCOL SPECIFIC HELPER METHODS
============================================================================*/

/*===========================================================================
  FUNCTION  ipcr_find_name
===========================================================================*/
/*!
@brief

 Finds all ports with the associated name

@param[in]   fd        Caller's file descriptor
@param[in]   name      Name of the port
@param[out]  addrs     (Optional, can be NULL, num_entries is assumed
                       to be 0) Pointer to array of addresses to store 
                       the address of found ports with matching name.
@param[out]  instances (Optional, can be NULL) Pointer to array of 
                       instance IDs of the names found (Useful if
                       IPCR_FLAGS_ANY_INSTANCE is used to get the
                       instance ID of the found address). Note, the
                       array must allow the same number of entries 
                       as 'addrs'
@param[inout]num_entires (Optional, can be NULL, input assumed to be 0)
                         Input the size of the array 'addrs' and
                         if provided, 'instances'.
                         Output, the number of entries filled.
@param[in]   flags     0 for an exact match on the provided
                       TYPE and INSTANCE pair.
                       IPCR_FLAGS_ANY_INSTANCE to match just
                       the TYPE.

@return
  Number of services found upon success, negative error code on failure.

@note

  - Dependencies
    - The user must hold a valid file descriptor toward IPC Router.
      (return of qsocket(AF_IPC_ROUTER, *, *)

  - Side Effects
    - None
*/
/*=========================================================================*/
int ipcr_find_name(int fd, ipcr_name_t *name, struct qsockaddr_ipcr *addrs, 
                    unsigned int *instances, unsigned int *num_entries, 
                    unsigned int flags);

/*===========================================================================
  FUNCTION  ipcr_find_sname
===========================================================================*/
/*!
@brief

 Finds all ports with the associated name

@param[in]   fd        Caller's file descriptor
@param[in]   name      Name of the port
@param[out]  addrs     (Optional, can be NULL, num_entries is assumed
                       to be 0) Pointer to array of addresses to store 
                       the address of found ports with matching name.
@param[inout]num_entires (Optional, can be NULL, input assumed to be 0)
                         Input the size of the array 'addrs' and
                         if provided, 'instances'.
                         Output, the number of entries filled.
@param[in]   flags     0 (Currently unused)

@return
  Number of services found upon success, negative error code on failure.

@note

  - Dependencies
    - The user must hold a valid file descriptor toward IPC Router.
      (return of qsocket(AF_IPC_ROUTER, *, *)

  - Side Effects
    - None
*/
/*=========================================================================*/
int ipcr_find_sname(int fd, char *name, struct qsockaddr_ipcr *addrs, 
                    unsigned int *num_entries, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
