#ifndef SYS_M_DIAG_H
#define SYS_M_DIAG_H
/*===========================================================================

           S Y S _ M _ D I A G . H

DESCRIPTION
  Diag interface for system monitor

Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   nk      Added changes to introduce delay through QXDM command before entering into STM 
09/26/14   dav     File Created

===========================================================================*/
#include "diagpkt.h"
#include "diagcmd.h"
#include "sys_m_diag.h"
#include "tms_diag.h"
#include "sys_m.h"
#include "timer.h"
#include "timetick.h"

#define SYS_M_DIAG_PROC_ID DIAG_MODEM_PROC 

typedef enum{
  SYS_M_PERIPHERAL_RESTART,
  SYS_M_SYSTEM_RESTART,
  SYS_M_PERIPHERAL_RESTART_WITH_RES_CODE,
  SYS_M_SYSTEM_RESTART_WITH_RES_CODE,
  SIZEOF_SYS_M_RESTART_TYPE
}sys_m_restart_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
  sys_m_restart_type restart_type;
  uint32 delay;
}sys_m_restart_req_type;

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 delay_secs;
} sys_m_shutdown_delay_req_type;


typedef struct diag_info_s
{
  sys_m_restart_type    type;
  uint32                 delay;
} diag_info_t;

PACK(void *) sys_m_request_restart_diag (PACK(void *) req_ptr, uint16 pkt_len);

PACK(void *) sys_m_shutdown_delay_diag (PACK(void *) req_ptr, uint16 pkt_len);


#endif	/* SYS_M_DIAG_H  */
