#ifndef PROFDIAG_H
#define PROFDIAG_H
/*===========================================================================

DESCRIPTION
  Definitions of DIAG subsystem packets for configuring task profile,
  memory debug, PC profile, and code coverage 
 
Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //components/rel/core.qdsp6/2.1.c4/services/prof/inc/prof_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


#include "diagcmd.h"
#include "diagpkt.h"
#include "customer.h"
#include "comdef.h"
#include "memheap.h"

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#define PROF_DIAG_PERF_MON_F 0

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*============================================================================

PACKET   Task Performance Monitoring

ID       DIAG_SUBSYS_PROF_PERFMON_F

PURPOSE  Sent from the DM to the DMSS requesting the DMSS to either start or
         stop task profiling.

         action: A value of 1 indicates that collection of Performance
                 monitoring information should be started. A value of 2
                 indicates that collection should be stopped. Starting
                 collection will reset previously collected information.

         ver: version number in order to address any changes in the format
                 in future.

         evt0 .. evt 3: One of the 4 event numbers requested to be monitored.
                 These event numbers correspond to the Event numbers
                 specified in QDSP6 System Architecture Specification. 

         pmon_period: The periodicity for performance monitoring and
                 reporting performance information (i.e sending out the
                 LOG_BLAST_PM_C log packet) in milliseconds. For instance,
                 if this field has a value of 500, task profiling information
                 will be sent out every 500 milliseconds.

RESPONSE The DMSS sends back a packet containing the result of the request
         (success or failure, with 1 = success and 0 = failure)

============================================================================*/


/** @brief Request structure to enable performance monitor through QXDM */
typedef PACK(struct )
{
  diagpkt_subsys_header_type header;
      /**< Sub System header                       */      
  uint8  enable_flag;
      /**< Enable Flag                             */      
  uint16 profile_duration;
      /**< duration(msec) between two events       */      
} prof_diag_perf_mon_req_t;

/** @brief Response structure to acknowledge performance monitor enablement
when enabled through QXDM 
*/
typedef PACK(struct) 
{
  diagpkt_subsys_header_type header;
      /**< Sub System header                       */        
} prof_diag_perf_mon_rsp_t;

/* use for ADSP perf meter */
DIAGPKT_SUBSYS_REQ_DEFINE( PROF, PERFMETER_F )
  uint8  enable_flag;
  uint16 profile_duration;
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, PERFMETER_F )
   uint32 status;
DIAGPKT_SUBSYS_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE( PROF, TASKPROFILE_F )
   uint32 action;
   uint32 task_period;
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, TASKPROFILE_F )
   uint32 status;
DIAGPKT_SUBSYS_RSP_END

/*============================================================================

PACKET   Task Performance Monitoring

ID       DIAG_SUBSYS_REX_PERFMON_F

PURPOSE  Sent from the DM to the DMSS requesting the DMSS to either start or
         stop task profiling.

         action: A value of 1 indicates that collection of Performance
                 monitoring information should be started. A value of 2
                 indicates that collection should be stopped. Starting
                 collection will reset previously collected information.

         ver: version number in order to address any changes in the format
                 in future.

         evt0 .. evt 3: One of the 4 event numbers requested to be monitored.
                 These event numbers correspond to the Event numbers
                 specified in QDSP6 System Architecture Specification. 

         pmon_period: The periodicity for performance monitoring and
                 reporting performance information (i.e sending out the
                 LOG_BLAST_PM_C log packet) in milliseconds. For instance,
                 if this field has a value of 500, task profiling information
                 will be sent out every 500 milliseconds.

RESPONSE The DMSS sends back a packet containing the result of the request
         (success or failure, with 1 = success and 0 = failure)

============================================================================*/
DIAGPKT_SUBSYS_REQ_DEFINE( PROF, PERFMON_F )
   uint32 action;
   uint8 ver;
   uint8 rsvd[3];
   uint8 evt0;
   uint8 evt1;
   uint8 evt2;
   uint8 evt3;
   uint32 pmon_period;
DIAGPKT_SUBSYS_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE( PROF, PERFMON_F )
   uint32 status;
DIAGPKT_SUBSYS_RSP_END

/* =========================================================================
** PROFILING SUBSYS DIAG TABLE
** ========================================================================= */
#if defined(SLPI_PROF)
   #define DIAG_QURT_PERFMON_F          0x0504
   #define DIAG_PERFMETER_F             0x0503
   #define DIAG_TASKPROFILE_F           0x0502
   #define DIAG_BLAST_PERFMON_F         0x0501 /* support legacy log packet */
#elif defined(CDSP_PROF)
   #define DIAG_QURT_PERFMON_F          0x0404
   #define DIAG_PERFMETER_F             0x0403
   #define DIAG_TASKPROFILE_F           0x0402
   #define DIAG_BLAST_PERFMON_F         0x0401 /* support legacy log packet */
#elif defined(ADSP_PROF)
   #define DIAG_QURT_PERFMON_F          0x0404
   #define DIAG_PERFMETER_F             0x0403
   #define DIAG_TASKPROFILE_F           0x0402
   #define DIAG_BLAST_PERFMON_F         0x0401 /* support legacy log packet */
#elif defined(MPSS_PROF)
   #define DIAG_QURT_PERFMON_F          0x0204
   #define DIAG_PERFMETER_F             0x0203
   #define DIAG_TASKPROFILE_F           0x0202
   #define DIAG_BLAST_PERFMON_F         0x0201 /* support legacy log packet */
#endif

#endif /* PROF_DIAG_H */
