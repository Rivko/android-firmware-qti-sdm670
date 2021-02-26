/*
 * graphite_errorcodes.h
 *
 * This header contains error codes returned by Graphite server
 * in response to command sent from Graphite Client.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GRAPHITE_ERRORCODES_H__
#define __GRAPHITE_ERRORCODES_H__

/*
 * Graphite error codes.
 * These error codes will be returned from Graphite server in response
 * to the commands sent from Graphite client.
 */

/* Success. The operation completed with no errors. */
#define GRAPHITE_EOK                                           ( 0x00000000 )
/* General failure. */
#define GRAPHITE_EFAILED                                       ( 0x00000001 )
/* Bad operation parameter. */
#define GRAPHITE_EBADPARAM                                     ( 0x00000002 )
/* Unsupported routine or operation. */
#define GRAPHITE_EUNSUPPORTED                                  ( 0x00000003 )
/* Unsupported version. */
#define GRAPHITE_EVERSION                                      ( 0x00000004 )
/* Unexpected problem encountered. */
#define GRAPHITE_EUNEXPECTED                                   ( 0x00000005 )
/* Unhandled problem occurred. */
#define GRAPHITE_EPANIC                                        ( 0x00000006 )
/* Unable to allocate resource. */
#define GRAPHITE_ENORESOURCE                                   ( 0x00000007 )
/* Invalid handle. */
#define GRAPHITE_EHANDLE                                       ( 0x00000008 )
/* Operation is already processed. */
#define GRAPHITE_EALREADY                                      ( 0x00000009 )
/* Operation is not ready to be processed. */
#define GRAPHITE_ENOTREADY                                     ( 0x0000000A )
/* Operation is pending completion. */
#define GRAPHITE_EPENDING                                      ( 0x0000000B )
/* Operation could not be accepted or processed. */
#define GRAPHITE_EBUSY                                         ( 0x0000000C )
/* Operation was aborted due to an error. */
#define GRAPHITE_EABORTED                                      ( 0x0000000D )
/* Operation was preempted by a higher priority. */
#define GRAPHITE_EPREEMPTED                                    ( 0x0000000E )
/* Operation requests an intervention to complete. */
#define GRAPHITE_ECONTINUE                                     ( 0x0000000F )
/* Operation requests an immediate intervention to complete. */
#define GRAPHITE_EIMMEDIATE                                    ( 0x00000010 )
/* Operation was not implemented. */
#define GRAPHITE_ENOTIMPL                                      ( 0x00000011 )
/* Operation needs more data or resources. */
#define GRAPHITE_ENEEDMORE                                     ( 0x00000012 )
/* Operation does not have memory. */
#define GRAPHITE_ENOMEMORY                                     ( 0x00000014 )
/* Item does not exist. */
#define GRAPHITE_ENOTEXIST                                     ( 0x00000015 )
/* Operation is finished. */
#define GRAPHITE_ETERMINATED                                   ( 0x00000016 )

#endif /* __GRAPHITE_ERRORCODES_H__ */
