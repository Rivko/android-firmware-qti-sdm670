#ifndef __AFE_SLIMBUS_SLAVE_CFG_I_H__
#define __AFE_SLIMBUS_SLAVE_CFG_I_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusSlaveCfg_i.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusSlaveCfg-i.h

DESCRIPTION: Main Interface to the AFE Slimbus slave device configuration

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusSlaveCfg_i.h#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
03-20-2011  mspk  Initial Draft


============================================================================*/
#include "qurt_elite.h"
#include "ddislimbus.h"
#include "AFESlimbusDriver.h"
#include "AFESlimbusDriverUtils_i.h"

/*=====================================================================
 Macros
 ======================================================================*/
// slave port water mark level (0: 6bytes, 1: 9bytes, 2: 12 bytes, 3: 15 bytes)
#define SLAVE_PORT_WATER_MARK_VALUE  						2
#define SLAVE_PORT_ENABLE	    							1
#define SLAVE_PORT_DISABLE									0

// below details are taken from SLIMBUS slave SWI

#define SB_PGD_TX_PORT_MULTI_CHANNEL_0_START_PORT_ID   0
#define SB_PGD_TX_PORT_MULTI_CHANNEL_0_END_PORT_ID     7

#define SB_PGD_TX_PORT_MULTI_CHANNEL_1_START_PORT_ID   8
#define SB_PGD_TX_PORT_MULTI_CHANNEL_1_END_PORT_ID     15

#define SB_PGD_RX_PORT_MULTI_CHANNEL_0_START_PORT_ID   16
#define SB_PGD_RX_PORT_MULTI_CHANNEL_0_END_PORT_ID     23

#define SB_PGD_RX_PORT_MULTI_CHANNEL_1_START_PORT_ID   24
#define SB_PGD_RX_PORT_MULTI_CHANNEL_1_END_PORT_ID     31


#define SB_PGD_MAX_NUMBER_OF_TX_SLAVE_DEV_PORTS     		 16
#define SB_PGD_MAX_NUMBER_OF_RX_SLAVE_DEV_PORTS     		 16

/*=====================================================================
 Typedef's
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/
ADSPResult afe_slimbus_slave_port_config(afe_slimbus_state_struct_t *p_dev_state, int16_t multi_ch_grouping);
ADSPResult afe_slimbus_internal_slave_port_cfg(afe_slimbus_state_struct_t *p_dev_state, int16_t multi_ch_grouping);

#endif /*#ifndef __AFE_SLIMBUS_SLAVE_CFG_H__*/
