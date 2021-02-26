#ifndef DIAGCOMM_IO_UDP_H 
#define DIAGCOMM_IO_UDP_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2015-2016 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_io_udp.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/12/16   ph      SMD references removal and code clean up.
05/03/16   ph      Changed UDP_WRITE_TIMEOUT to infinite.
04/08/15   ps      Created file for Diag over sockets


===========================================================================*/
#include "diagcomm_io.h"
#include "diagpkt.h"


typedef struct
{
  char buf[3*DIAG_MAX_RX_PKT_SIZ];
  uint32 used;
}diag_udp_read_buf_type;

#define UDP_WRITE_TIMEOUT -1 //Infinite timeout


void diagcomm_io_udp_init(void);
void diagcomm_io_udp_open_init(diagcomm_io_conn_type * conn);
int diagcomm_io_udp_open(diagcomm_io_conn_type * conn);

int diagcomm_io_udp_send(diagcomm_io_conn_type * conn,void *buf, unsigned int total_pkt_length);
void diagcomm_io_udp_close(diagcomm_io_conn_type * conn);
int diagcomm_io_udp_get_srv_addr (ipcr_name_t name, struct qsockaddr_ipcr *srv_addr, int timeout);


#endif //DIAGCOMM_IO_UDP_H
