/******************************************************************************NS

  @file    bt_qmi_dms_client.c
  @brief

  DESCRIPTION

 ******************************************************************************/
/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================*/

#if defined(BT_QSOC_GET_ITEMS_FROM_NV)

#include "bt_qmi_dms_client.h"

#include "string.h"
#undef LOGD
#undef LOGE
#undef LOGI
#undef LOGV

#define LOGD ALOGD
#define LOGI ALOGI
#define LOGE ALOGE
#define LOGV ALOGV

static qmi_client_type dms_qmi_client;
static qmi_client_os_params dms_qmi_client_os_params;
static int qmi_handle;
static boolean dms_init_done = FALSE;


/* Android system property for fetching the modem type */
#define QMI_UIM_PROPERTY_BASEBAND               "ro.baseband"

/* Android system property values for various modem types */
#define QMI_UIM_PROP_BASEBAND_VALUE_SVLTE_1     "svlte1"
#define QMI_UIM_PROP_BASEBAND_VALUE_SVLTE_2A    "svlte2a"
#define QMI_UIM_PROP_BASEBAND_VALUE_CSFB        "csfb"
#define QMI_UIM_PROP_BASEBAND_VALUE_SGLTE       "sglte"
#define QMI_UIM_PROP_BASEBAND_VALUE_SGLTE2      "sglte2"
#define QMI_UIM_PROP_BASEBAND_VALUE_MSM         "msm"
#define QMI_UIM_PROP_BASEBAND_VALUE_APQ         "apq"
#define QMI_UIM_PROP_BASEBAND_VALUE_MDMUSB      "mdm"
#define QMI_UIM_PROP_BASEBAND_VALUE_DSDA        "dsda"
#define QMI_UIM_PROP_BASEBAND_VALUE_DSDA_2      "dsda2"


boolean bt_qmi_dms_init()
{
    qmi_client_error_type qmi_client_err = QMI_NO_ERR;
    qmi_idl_service_object_type dms_service;
    char prop_value[PROPERTY_VALUE_MAX];
    qmi_client_qmux_instance_type qmi_modem_port;
    int timeout = 4;

    /* Find out the modem type */
    memset(prop_value, 0x00, sizeof(prop_value));
    property_get(QMI_UIM_PROPERTY_BASEBAND, prop_value, "");

    /*check if it's APQ only device */
    if (strcmp(prop_value, QMI_UIM_PROP_BASEBAND_VALUE_APQ) == 0)
    {
        LOGE("%s:Read BD address from Modem NV is not supported for APQ devices \n",
                                                                      __func__);
        return FALSE;
    }

    dms_service = dms_get_service_object_v01();
    if(dms_service == NULL) {
        LOGE("%s: Not able to get dms service handle\n", __func__);
        return FALSE;
    }

    qmi_modem_port = QMI_CLIENT_INSTANCE_ANY;

    qmi_client_err = qmi_client_init_instance(dms_service, qmi_modem_port,
                NULL, NULL, &dms_qmi_client_os_params, timeout, &dms_qmi_client);

    if(qmi_client_err != QMI_NO_ERR) {
        LOGE("%s :Error while Initializing qmi_client_init_instance: %d\n",
                __func__, qmi_client_err);
        return FALSE;
    }

    dms_init_done = TRUE;
    return TRUE;
}

boolean qmi_dms_get_bt_address(unsigned char *pBdAddr)
{
    qmi_client_error_type qmi_client_err;
    dms_get_mac_address_req_msg_v01 addr_req;
    dms_get_mac_address_resp_msg_v01 addr_resp;

    if((dms_init_done == FALSE) || (pBdAddr == NULL)) {
        return FALSE;
    }
    /* clear the request content */
    memset(&addr_req, 0, sizeof(addr_req));

    /*Request to get the Bluetooth address */
    addr_req.device = DMS_DEVICE_MAC_BT_V01;
    qmi_client_err = qmi_client_send_msg_sync(dms_qmi_client,
            QMI_DMS_GET_MAC_ADDRESS_REQ_V01, &addr_req, sizeof(addr_req), &addr_resp,
            sizeof(addr_resp), DMS_QMI_TIMEOUT);

    if(qmi_client_err != QMI_NO_ERR){
        LOGE("%s: Error:  %d\n", __func__, qmi_client_err);
        return FALSE;
    }

    LOGE("%s: addr_resp.mac_address_valid %d addr_resp.mac_address_len %d \n",
            __func__, addr_resp.mac_address_valid,addr_resp.mac_address_len );

    if (addr_resp.mac_address_valid && (addr_resp.mac_address_len == BD_ADDR_SIZE)) {
        memcpy(pBdAddr, addr_resp.mac_address, addr_resp.mac_address_len);
        LOGE("%s: Succesfully Read BT address\n", __func__);
        return TRUE;
    }
    return FALSE;
}

boolean qmi_dms_get_refclock(unsigned long *ref_clock)
{
    *ref_clock = 0;
    return FALSE;
}

boolean qmi_dms_get_clk_sharing(boolean *clock_sharing)
{
    *clock_sharing = FALSE;
    return FALSE;
}

boolean is_modem_bt_nv_supported(modem_bt_nv_items_type nv_item_type)
{
    boolean ret = FALSE;
    switch(nv_item_type)
    {
        case MODEM_NV_BD_ADDR:
            ret = TRUE;
            break;
        case MODEM_NV_BT_SOC_REFCLOCK_TYPE:
        case MODEM_NV_BT_SOC_CLK_SHARING_TYPE:
        default:
            ret = FALSE;
    }
    return ret;
}

void cleanup_qmi_dms()
{
    qmi_client_error_type qmi_client_err;
    LOGV("->%s", __func__);

    if(dms_init_done == FALSE) {
        return;
    }

    qmi_client_err = qmi_client_release(dms_qmi_client);
    if(qmi_client_err != QMI_NO_ERR){
        LOGE("Error: while releasing qmi_client : %d\n", qmi_client_err);
    }

    LOGV("<-%s", __func__);
}

#endif
