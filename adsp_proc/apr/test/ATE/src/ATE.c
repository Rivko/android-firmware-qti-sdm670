/*===========================================================================

@file   APRGeneralTest.cpp
@brief

Copyright (c) 2011 by QUALCOMM Technologies, Inc.
All Rights Reserved. QUALCOMM Proprietary and Confidential.

Edit History:

$Header: //components/rel/apr.common/2.0/test/ATE/src/ATE.c#1 $
$DateTime: 2017/10/06 13:36:34 $
$Author:
$Change: 14557094 $
$Revision: #1 $

===========================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR FILE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.
    when        who         what, where, why
    --------    -----       ---------------------------------------------------
    01/29/14    rajas       Initial version

    =========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ATE_Log.h"
#include "ATE.h"


/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/


/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/


/*****************************************************************************
 * Function Declarations                                                     *
 ****************************************************************************/


/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/


/******************************************************************************
* Public API
******************************************************************************/

int32_t ate_init(void)
{
    int32_t result;

    ateNOTE_INFO("Initializing APR ..");
    result = apr_init();
    ateEXPECT_EQ(result, APR_EOK, "APR Init failed, res = 0x%x", result);

    return result;
}

int32_t ate_deinit(void)
{
    int32_t result;

    ateNOTE_INFO("DeInitializing APR ..");
    result = apr_deinit();
	ateEXPECT_EQ(result, APR_EOK, "APR De-init failed, res = 0x%x", result);

    return result;
}


/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/

/* General Test Cases */

void ate_get_version(void)
{
    int32_t rc;
    char_t ver_buf[31];
    uint32_t ver_actual_size;
    apr_cmd_get_version_t args;

    ateNOTE_INFO("Starting GetVersion testcase ..");

    args.buffer = ver_buf;
    args.buf_size = sizeof( ver_buf );
    args.ret_size = &ver_actual_size;

    ateNOTE_INFO("Querying APR Version..");
    rc = apr_call( APR_CMDID_GET_VERSION, &args, sizeof( args ) );
	ateASSERT_EQ(rc, APR_EOK, "GetVersion failed, res=0x%x", rc);

    ateNOTE1_INFO("Local APR version is: %s", ver_buf);

    return;
}


static int32_t my_service_dispatch_cb(aprv2_packet_t *packet, void *dispatch_data)
{
    ateNOTE1_INFO("Recvd dispatch callback, packet=0x%x", packet);

    return APR_EOK;
}

void ate_allocate_free(uint32_t size)
{
    int32_t rc;
    char_t my_dns[] = "qcom.domain.mydnsname";
    uint16_t my_addr = 0;
    uint32_t my_handle = 0;
    aprv2_packet_t *ret_packet = NULL;

    ateNOTE_INFO("Starting AllocateFree testcase ..");

    rc = __aprv2_cmd_register2(&my_handle, my_dns, sizeof(my_dns), 0, my_service_dispatch_cb, NULL, &my_addr);
    ateASSERT_EQ(rc, APR_EOK, "register2 cmd failed, res=0x%x", rc);
    ateASSERT_NE(my_handle, 0, "Recvd invalid handle", 0);
    ateASSERT_NE(my_addr, 0, "Recvd invalid address", 0);

    ateNOTE1_INFO("Allocating packet of size..: %d", size);
    rc = __aprv2_cmd_alloc(my_handle, APRV2_ALLOC_TYPE_COMMAND, size, &ret_packet);
    ateEXPECT_EQ(rc, APR_EOK, "alloc cmd failed, res=0x%x", rc);
    ateEXPECT_NE(ret_packet, NULL, "Recvd invalid packet: NULL", 0);

    if (NULL != ret_packet)
    {
        ateNOTE1_INFO("Freeing packet..0x%x", ret_packet);
        rc = __aprv2_cmd_free(my_handle, ret_packet);
        ateEXPECT_EQ(rc, APR_EOK, "free cmd failed, res=0x%x", rc);
    }

    rc = __aprv2_cmd_deregister(my_handle);
    ateASSERT_EQ(rc, APR_EOK, "deregister cmd failed, res=0x%x", rc);

    return;
}

void ate_query_by_dns(void)
{
    int32_t rc;
    char_t my_dns[] = "qcom.domain.mydnsname";
    uint16_t my_addr = 0;
    uint16_t my_lookup_addr = 0;
    uint32_t my_handle = 0;

    ateNOTE_INFO("Starting QueryByDNS testcase ..");

    ateNOTE_INFO("Registering a DNS name..");
    rc = __aprv2_cmd_register2(&my_handle, my_dns,
            sizeof(my_dns), 0, my_service_dispatch_cb, NULL, &my_addr);
    ateASSERT_EQ(rc, APR_EOK, "register2 cmd failed, res=0x%x", rc);
    ateNOTE2_INFO("Recvd Handle: 0x%x, Addess: 0x%x", my_handle, my_addr);
    ateASSERT_NE(my_handle, 0, "Recvd invalid handle", 0);
    ateASSERT_NE(my_addr, 0, "Recvd invalid address", 0);

    ateNOTE_INFO("Querying by the DNS name..");
    rc = __aprv2_cmd_local_dns_lookup(my_dns, sizeof(my_dns), &my_lookup_addr);
    ateASSERT_EQ(rc, APR_EOK, "lookup cmd failed, res=0x%x", rc);
    ateNOTE1_INFO("DNSLookup returned address..: 0x%x", my_lookup_addr);
    ateASSERT_EQ(my_lookup_addr, my_addr, "Recvd incorrect address..", 0);

    ateNOTE_INFO("De-registering..");
    rc = __aprv2_cmd_deregister(my_handle);
    ateASSERT_EQ(rc, APR_EOK, "deregister cmd failed, res=0x%x", rc);

    return;
}

void ate_query_by_handle(void)
{
    int32_t rc;
    char_t my_dns[] = "qcom.domain.mydnsname";
    uint16_t my_addr = 0;
    uint16_t my_query_addr = 0;
    uint32_t my_handle = 0;

    ateNOTE_INFO("Starting QueryByHandle testcase ..");

    ateNOTE_INFO("Registering a DNS name..");
    rc = __aprv2_cmd_register2(&my_handle, my_dns,
            sizeof(my_dns), 0, my_service_dispatch_cb, NULL, &my_addr);
    ateASSERT_EQ(rc, APR_EOK, "register2 cmd failed, res=0x%x", rc);
    ateNOTE2_INFO("Recvd Handle: 0x%x, Addess: 0x%x", my_handle, my_addr);
    ateASSERT_NE(my_handle, 0, "Recvd invalid handle", 0);
    ateASSERT_NE(my_addr, 0, "Recvd invalid address", 0);

    ateNOTE_INFO("Querying using handle..");
    rc = __aprv2_cmd_query_address(my_handle, &my_query_addr);
    ateASSERT_EQ(rc, APR_EOK, "query cmd failed, res=0x%x", rc);
    ateNOTE1_INFO("Query returned address..: 0x%x", my_query_addr);
    ateASSERT_EQ(my_query_addr, my_addr, "Recvd incorrect address..", 0);

    ateNOTE_INFO("De-registering..");
    rc = __aprv2_cmd_deregister(my_handle);
    ateASSERT_EQ(rc, APR_EOK, "deregister cmd failed, res=0x%x", rc);

    return;
}

void ate_register2_by_name(char_t *dns_name, uint32_t dns_name_size)
{
    int32_t rc;
    uint16_t my_addr = 0;
    uint32_t my_handle = 0;

    ateNOTE_INFO("Starting Register2ByName testcase ..");

    ateNOTE1_INFO("Registering DNS name: %s", dns_name);
    rc = __aprv2_cmd_register2(&my_handle, dns_name,
            dns_name_size, 0, my_service_dispatch_cb, NULL, &my_addr);
    ateASSERT_EQ(rc, APR_EOK, "register2 cmd failed, res=0x%x", rc);
    ateNOTE2_INFO("Recvd Handle: 0x%x, Addess: 0x%x", my_handle, my_addr);
    ateASSERT_NE(my_handle, 0, "Recvd invalid handle", 0);
    ateASSERT_NE(my_addr, 0, "Recvd invalid address", 0);

    ateNOTE_INFO("De-registering..");
    rc = __aprv2_cmd_deregister(my_handle);
    ateASSERT_EQ(rc, APR_EOK, "deregister cmd failed, res=0x%x", rc);

    return;
}

void ate_register2_by_addr(void)
{
    int32_t rc;
    uint16_t my_addr = 0;
    uint32_t my_handle = 0;

    ateNOTE_INFO("Starting Register2ByAddr testcase ..");

    ateNOTE_INFO("Registering dynamic client..");
    rc = __aprv2_cmd_register2(&my_handle, NULL,
            0, 0, my_service_dispatch_cb, NULL, &my_addr);
    ateASSERT_EQ(rc, APR_EOK, "register2 cmd failed, res=0x%x", rc);
    ateNOTE2_INFO("Recvd Handle: 0x%x, Addess: 0x%x", my_handle, my_addr);
    ateASSERT_NE(my_handle, 0, "Recvd invalid handle", 0);
    ateASSERT_NE(my_addr, 0, "Recvd invalid address", 0);

    ateNOTE_INFO("De-registering..");
    rc = __aprv2_cmd_deregister(my_handle);
    ateASSERT_EQ(rc, APR_EOK, "deregister cmd failed, res=0x%x", rc);

    return;
}
