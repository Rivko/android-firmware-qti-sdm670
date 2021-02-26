/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include <InstallKeybox.h>

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "cutils/properties.h"

#include "qmi_client.h"
#include "qmi_client_instance_defs.h"
#include "device_management_service_v01.h"
#include "common_v01.h"
#include "qmi_cci_target_ext.h"

#define QMI_CLIENT_SRV_REG_TIMEOUT   (5000)
#define QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT (5000)

using namespace std;

int InstallKeybox::provisionDeviceID(void) {
    qmi_client_error_type qmi_client_error = 0;
    dms_get_device_serial_numbers_resp_msg_v01 qmi_response;
    qmi_idl_service_object_type service_object = dms_get_service_object_v01();
    qmi_client_os_params os_params;
    bool disconnect = false;
    qmi_client_type client_handle = NULL;
    bool with_modem = true;

    memset( &qmi_response, 0, sizeof( qmi_response ) );
    if(!service_object) {
        cerr << "service_object NULL" << endl;
        return -1;
    }

    qmi_client_error = qmi_client_init_instance(service_object,
            QMI_CLIENT_INSTANCE_ANY,
            NULL,
            NULL,
            &os_params,
            QMI_CLIENT_SRV_REG_TIMEOUT,
            &client_handle);
    if(qmi_client_error != 0) {
        if(qmi_client_error == QMI_TIMEOUT_ERR){
            with_modem = false;
        }else{
            cerr << "qmi_client_init_instance error" << qmi_client_error << endl;
            return -1;
        }
    }
    disconnect = true;
    // ** fetch data
    if (with_modem){
        qmi_client_error = qmi_client_send_msg_sync(client_handle,
                                                       QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
                                                       NULL,
                                                       0,  // empty request payload
                                                       (void*) &qmi_response,
                                                       sizeof( qmi_response ),
                                                       QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT );

        if ( 0 != qmi_client_error )
        {
            cerr << "qmi_client_send_msg_sync error" << qmi_client_error << endl;
            if(disconnect)
                qmi_client_release(client_handle);
            return -1;
        }
    }


    char paramstr[PROPERTY_VALUE_MAX];
    property_get("ro.product.brand", paramstr, "");
    Brand.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.device", paramstr, "");
    Device.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.name", paramstr, "");
    Product.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.serialno", paramstr, "");
    SerialNum.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.manufacturer", paramstr, "");
    Manufacturer.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    property_get("ro.product.model", paramstr, "");
    Model.assign(paramstr, strlen(paramstr));
    memset(paramstr, 0, PROPERTY_VALUE_MAX);

    if (with_modem){
        Imei.assign(qmi_response.imei, strlen(qmi_response.imei));
        Meid.assign(qmi_response.meid, strlen(qmi_response.meid));
    }

    cerr << "Brand: " << Brand << endl;
    cerr << "Device: " << Device << endl;
    cerr << "Product: " << Product << endl;
    cerr << "SerialNum: " << SerialNum << endl;
    cerr << "Manufacturer: " << Manufacturer << endl;
    cerr << "Model: " << Model << endl;
    cerr << "Imei: " << Imei << endl;
    cerr << "Meid: " << Meid << endl;

    KeymasterKeyProvision keymaster_provision;

    int result = keymaster_provision.KeyMasterProvisionInit();
    if (result) {
        cerr << "KeyMasterProvisionInit error" << result << endl;
        if(disconnect)
            qmi_client_release(client_handle);
        return -1;
    }

    result = keymaster_provision.KeyMasterProvisionDeviceId(Brand, Device,
            Product, SerialNum, Manufacturer, Model, Imei, Meid);
    if (result) {
        cerr << "KeyMasterProvisionDeviceId error" << result << endl;
        if(disconnect)
            qmi_client_release(client_handle);
        return -1;
    }

    return 0;
}
