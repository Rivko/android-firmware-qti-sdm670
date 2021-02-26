#ifndef SIORS232_MDD_DATA_H
#define SIORS232_MDD_DATA_H

/*===========================================================================

           S I O   D E V I C E   L A Y E R   M D D   (D A T A)   H E A D E R

DESCRIPTION
   This is the header file for the common code used by the SIO UART driver. 

Copyright (c) 2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/inc/siors232_mdd_data.h#1 $

when         who       what, where, why
-----------  --------  ----------------------------------------------------------
07-Jul-2008  cmihalik  Initial revision.
===========================================================================*/

#include "siors232_mdd.h"

void       siors_mdd_scan_data_autodetect_mode_rx_isr( sio_mdd_port_type*, dsm_item_type* );
void       siors_mdd_scan_data_rawdata_mode_rx_isr( sio_mdd_port_type*, dsm_item_type* );
void       siors_mdd_setup_uart_autodetect( sio_mdd_port_type* );

#endif 

