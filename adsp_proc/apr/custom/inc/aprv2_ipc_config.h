#ifndef __APRV2_IPC_CONFIG_H__
#define __APRV2_IPC_CONFIG_H__
/*=============================================================================
  Copyright (C) 2010-2016,2018-2019 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

===============================================================================
                                 Edit History
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //source/qcom/qct/multimedia2/apr/apr2/mainline/main/latest/datalink/apr_glink/src/apr_glink.c
  $Author: svutukur $

when         who   what, where, why
----------   ---   ------------------------------------------------------------

=============================================================================*/
#include "apr_comdef.h"
#include "aprv2_packet.h"
#include "apr_glink.h"
#include "aprv2_pl_config.h"
#include "aprv2_ids_domains.h"
#include "aprv2_mpd_i.h"
#include "glink.h"
//Typedef structures and function pointers
typedef struct aprv2_ipc_send_t aprv2_ipc_send_t;
typedef struct apr_port_info_t apr_port_info_t;
//typedef int32_t ( *aprv2_ipc_init_fn_t ) ( );
typedef int32_t ( *aprv2_ipc_dispatch_fn_t ) (uint32_t port_index, void* buf, uint32_t packet_len );

//Function declarations...
APR_INTERNAL int32_t apr_ipc_write_fn(uint32_t index,void *buf, uint32_t length);
APR_INTERNAL int32_t apr_diag_send_fn(uint32_t port_handle, void *buf, uint32_t length);
APR_INTERNAL int32_t aprv2_ipc_local(uint32_t port_handle,void *buf, uint32_t length);
APR_INTERNAL int32_t aprv2_ipc_unsupport_domain(uint32_t port_handle,void *buf, uint32_t length);
APR_INTERNAL int32_t aprv2_ipc_mpd_send(uint32_t port_handle, void *packet, uint32_t length);

//Structure headers
struct aprv2_ipc_send_t
{
  uint16_t domain_id; //store domain id for readability
  aprv2_ipc_dispatch_fn_t dispatch_data; //store the call back function here...
  int32_t port_handle_index;
};

struct apr_port_info_t {
  char_t *remote_ss;
  char_t *port_name;
  int32_t channel_id;
  int32_t index;
};

//Static structs and variables
static apr_port_info_t *ipc_ports_pointer;
static aprv2_ipc_send_t *aprv2_ipc_send_dispatch_table;

#define APRV2_REMOTE_SS_MODEM     ("mpss")
#define APRV2_REMOTE_SS_LPASS     ("lpass")
#define APRV2_REMOTE_SS_APPS      ("apss")
#define APRV2_REMOTE_SS_CDSP      ("cdsp")
#define APRV2_REMOTE_SS_SDSP      ("sdsp")

#define APRV2_IPC_AUDIO_PORT_NAME ( "apr_audio_svc" )
#define APRV2_IPC_APPS2_PORT_NAME ( "apr_apps2" )
/* "apr_voice_svc" used for communicating with ADSP2 */
#define APRV2_IPC_MODEM_PORT_NAME ( "apr_voice_svc" )
#define APRV2_IPC_VOICE_PORT_NAME ( "apr_voice_svc" )
#define APRV2_IPC_CDSP_PORT_NAME ( "apr_cdsp_adsp" )
#define APRV2_IPC_SDSP_PORT_NAME ( "apr_sdsp_adsp" )


//Modem goes here
#ifdef modem_proc  //if we are in modem

#ifdef APR_AUDIO_IN_ROOTPD // modem_proc

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_MODEM_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V )

//ipc layer config
#define APR_AUDIO_IN_ROOTPD 1
//#define APR_UPD 0
//#define APR_RPD 0
#define GLINK_PORTS 3
#define DIAG_PORTS 1 

//Used for ipc send/dispatch table.
static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
{APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,apr_diag_send_fn,NULL}, //diagsend
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS_V,apr_ipc_write_fn, 0},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,apr_ipc_write_fn,2},
};

// List of remote subsystems that this subsystem connects to.
static char_t *glink_remote_ss_list[] = {
  APRV2_REMOTE_SS_APPS,
  APRV2_REMOTE_SS_MODEM
};

static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {  
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_AUDIO_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID,0}, //apps
    {APRV2_REMOTE_SS_MODEM, APRV2_IPC_MODEM_PORT_NAME, APR_GLINK_MODEM_REMOTE_SS_ID, 1}, //mpss
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_APPS2_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID, 2}, //apps2
};
#endif // modem_proc APR_SPD_AUD_RPD

#ifdef APR_SPD // modem_proc

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_MODEM_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_MODEM_V )

//ipc layer config
#define APR_SPD 1
//#define APR_UPD 0
//#define APR_RPD 0
#define GLINK_PORTS 3
#define DIAG_PORTS 1 

//Used for ipc send/dispatch table.
static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
{APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,apr_diag_send_fn,NULL}, //diagsend
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,apr_ipc_write_fn,1},
 {APRV2_IDS_DOMAIN_ID_APPS_V,apr_ipc_write_fn, 0},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,aprv2_ipc_unsupport_domain,NULL},
};

// List of remote subsystems that this subsystem connects to.
static char_t *glink_remote_ss_list[] = {
  APRV2_REMOTE_SS_APPS,
  APRV2_REMOTE_SS_LPASS
};

static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {  
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_VOICE_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID,0},
    {APRV2_REMOTE_SS_LPASS, APRV2_IPC_VOICE_PORT_NAME, APR_GLINK_QDSP_REMOTE_SS_ID, 1},
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_APPS2_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID, 2}, 
};
#endif // modem_proc APR_SPD

#ifdef APR_RPD // modem_proc

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_MODEM_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V )

//ipc layer config
//#define APR_UPD 0
//#define APR_SPD 0
#define APR_RPD 1
#define GLINK_PORTS 3
#define DIAG_PORTS 1
  
static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
 {APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,apr_diag_send_fn,NULL}, //diagsend
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS_V,apr_ipc_write_fn, 0},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,apr_ipc_write_fn,2},
};

// List of remote subsystems that this subsystem connects to.
static char_t *glink_remote_ss_list[] = {
  APRV2_REMOTE_SS_APPS,
  APRV2_REMOTE_SS_MODEM
};
 
static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {  
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_AUDIO_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID,0}, //apps
    {APRV2_REMOTE_SS_MODEM, APRV2_IPC_MODEM_PORT_NAME, APR_GLINK_MODEM_REMOTE_SS_ID, 1}, //mpss
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_APPS2_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID, 2}, //apps2
};
#endif //bear root pd ends

#ifdef APR_UPD // modem_proc

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_MODEM_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V )

//ipc layer config
#define APR_UPD 1
//#define APR_SPD 0
//#define APR_RPD 0
#define GLINK_PORTS 0
#define DIAG_PORTS 0
  
static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
 {APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS_V,aprv2_ipc_mpd_send, NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,aprv2_ipc_mpd_send,NULL},
};
 
static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {};
 
#endif // modem_proc APR_UPD

#endif  //modem_proc end

/* ADSP MODE BEGINS HERE */
#ifdef adsp_proc

#ifdef APR_UPD   // adsp_proc

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V )

//ipc layer config
#define APR_UPD 1
//#define APR_RPD 0
//#define APR_SPD 0
#define GLINK_PORTS 0
#define DIAG_PORTS 0
  
static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
 {APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS_V,aprv2_ipc_mpd_send, NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_SDSP_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_CDSP_V,aprv2_ipc_mpd_send,NULL},
};

static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {};

#endif  // adsp_proc APR_UPD

#ifdef APR_RPD // adsp_proc 

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_ADSP_V ) //This is need if we have more than two default domainids
#define PDR_FEATURE 1
//ipc layer config
#define APR_RPD 1
//#define APR_SPD 0
//#define APR_UPD 0
#define GLINK_PORTS 5
#define DIAG_PORTS 0

static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
 {APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM_V,apr_ipc_write_fn,1},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,aprv2_ipc_mpd_send,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS_V,apr_ipc_write_fn, 0},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,apr_ipc_write_fn,2},
 {APRV2_IDS_DOMAIN_ID_SDSP_V,apr_ipc_write_fn,4},
 {APRV2_IDS_DOMAIN_ID_CDSP_V,apr_ipc_write_fn,3},
};

static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {  
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_AUDIO_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID,0},
    {APRV2_REMOTE_SS_MODEM, APRV2_IPC_MODEM_PORT_NAME, APR_GLINK_MODEM_REMOTE_SS_ID, 1},
    {APRV2_REMOTE_SS_APPS, APRV2_IPC_APPS2_PORT_NAME, APR_GLINK_APPS_REMOTE_SS_ID, 2},
    {APRV2_REMOTE_SS_CDSP, APRV2_IPC_CDSP_PORT_NAME, APR_GLINK_CDSP_REMOTE_SS_ID, 3},
    {APRV2_REMOTE_SS_SDSP, APRV2_IPC_SDSP_PORT_NAME, APR_GLINK_SDSP_REMOTE_SS_ID, 4},
};

// List of remote subsystems that this subsystem connects to.
static char_t *glink_remote_ss_list[] = {
  APRV2_REMOTE_SS_APPS,
  APRV2_REMOTE_SS_MODEM,
  APRV2_REMOTE_SS_CDSP,
  APRV2_REMOTE_SS_SDSP
};

#endif //adsp root pd ends
#endif //adsp mode ends

#ifdef cdsp_proc //cdsp starts here...

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_CDSP_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_CDSP_V ) //This is need if we have more than two default domainids

//ipc layer config
#define APR_RPD 1
//#define APR_SPD 0
//#define APR_UPD 0
#define GLINK_PORTS 1
#define DIAG_PORTS 0

static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
 {APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_unsupport_domain,1},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,apr_ipc_write_fn,0},
 {APRV2_IDS_DOMAIN_ID_APPS_V,aprv2_ipc_unsupport_domain, 0},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,aprv2_ipc_unsupport_domain,2},
 {APRV2_IDS_DOMAIN_ID_SDSP_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_CDSP_V,aprv2_ipc_local,NULL},
};

// List of remote subsystems that this subsystem connects to.
static char_t *glink_remote_ss_list[] = {
  APRV2_REMOTE_SS_LPASS
};

static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {  
    {APRV2_REMOTE_SS_LPASS, APRV2_IPC_CDSP_PORT_NAME, APR_GLINK_QDSP_REMOTE_SS_ID, 0}
};

#endif //cdsp ends here...

#ifdef slpi_proc //cdsp starts here...

#define APRV2_PL_DEFAULT_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_SDSP_V )
#define APRV2_PL_DEFAULT2_DOMAIN_ID_V ( APRV2_IDS_DOMAIN_ID_SDSP_V ) //This is need if we have more than two default domainids

//ipc layer config
#define APR_RPD 1
//#define APR_SPD 0
//#define APR_UPD 0
#define GLINK_PORTS 1
#define DIAG_PORTS 0

static aprv2_ipc_send_t aprv2_ipc_glink_send_dispatch_table[ ] = {
 {APRV2_IDS_DOMAIN_ID_RESERVED_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_SIM_V,aprv2_ipc_local,NULL},
 {APRV2_IDS_DOMAIN_ID_PC_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_MODEM_V,aprv2_ipc_unsupport_domain,1},
 {APRV2_IDS_DOMAIN_ID_ADSP_V,apr_ipc_write_fn,0},
 {APRV2_IDS_DOMAIN_ID_APPS_V,aprv2_ipc_unsupport_domain, 0},
 {APRV2_IDS_DOMAIN_ID_MODEM2_V,aprv2_ipc_unsupport_domain,NULL},
 {APRV2_IDS_DOMAIN_ID_APPS2_V,aprv2_ipc_unsupport_domain,2},
 {APRV2_IDS_DOMAIN_ID_SDSP_V,aprv2_ipc_local,NULL},
};

// List of remote subsystems that this subsystem connects to.
static char_t *glink_remote_ss_list[] = {
  APRV2_REMOTE_SS_LPASS
};

static apr_port_info_t glink_port_data[ GLINK_PORTS ] = {  
    {APRV2_REMOTE_SS_LPASS, APRV2_IPC_SDSP_PORT_NAME, APR_GLINK_QDSP_REMOTE_SS_ID, 0}
};

#endif //sdsp ends here...

#endif /*  __APRV2_PL_CONFIG_H__ */

