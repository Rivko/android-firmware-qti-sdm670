/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_qdi.c
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#include <stdlib.h>
#include "qurt.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "dangergen.h"
#include "dangergen_qdi.h"
#include "dangergen_qdi_constants.h"
#include "dangergen_internal.h"
#include "dangergen_inlines.h"


typedef struct
{
    qurt_qdi_obj_t qdi_obj; // must be first field
    dangergen_driver_type* self;
    dangergen_handle_type handle;
} dangergen_qdi_obj_type;


void dangergen_release_qdi_obj(qurt_qdi_obj_t* obj)
{
    dangergen_qdi_obj_type* dangergen_qdi_obj = (dangergen_qdi_obj_type*) obj;
    free(dangergen_qdi_obj);
}

static int dangergen_qdi_invocation(
    int client_handle,
    qurt_qdi_obj_t *obj,
    int method,
    qurt_qdi_arg_t a1,
    qurt_qdi_arg_t a2,
    qurt_qdi_arg_t a3,
    qurt_qdi_arg_t a4,
    qurt_qdi_arg_t a5,
    qurt_qdi_arg_t a6,
    qurt_qdi_arg_t a7,
    qurt_qdi_arg_t a8,
    qurt_qdi_arg_t a9)
{

    dangergen_qdi_obj_type* dangergen_qdi = (dangergen_qdi_obj_type*) obj;

    int result = DANGERGEN_ERROR;

    switch(method)
    {
        case DANGERGEN_QDI_OPEN:
        {
            dangergen_open_params_type params;
            dangergen_open_params_type* in_params =
                (dangergen_open_params_type*) a1.ptr;
            if(qurt_qdi_copy_from_user(client_handle, &params, in_params,
                sizeof(dangergen_open_params_type)) >= 0)
            {
                dangergen_handle_type h_local;
                dangergen_handle_type* h_out = (dangergen_handle_type*) a2.ptr;
                if(dangergen_is_successful(dangergen_open(&params, &h_local)))
                {
                    // Write handle back to user space
                    if(qurt_qdi_copy_to_user(client_handle, h_out, &h_local,
                        sizeof(dangergen_handle_type)) >= 0)
                    {
                        // Save the new handle; to be used in case of
                        // unexpected close of QDI handle
                        dangergen_qdi->handle = h_local;
                        result = DANGERGEN_SUCCESS;
                    }
                    else
                    {
                        dangergen_close(h_local);
                    }
                }
            }
            break;
        }
        case DANGERGEN_QDI_CLOSE:
        {
            dangergen_handle_type h = (dangergen_handle_type) a1.num;
            result = dangergen_close(h);
            dangergen_qdi->handle = NULL;
            break;
        }
        case DANGERGEN_QDI_CONFIGURE:
        {
            dangergen_watchdog_params_type params;
            dangergen_watchdog_params_type* in_params =
                (dangergen_watchdog_params_type*) a2.ptr;
            dangergen_handle_type h = (dangergen_handle_type) a1.num;
            if(qurt_qdi_copy_from_user(client_handle, &params, in_params,
                sizeof(dangergen_watchdog_params_type)) >= 0)
            {
                result = dangergen_configure_watchdog(h, &params);
            }
            break;
        }
        case DANGERGEN_QDI_ENABLE:
        {
            dangergen_handle_type h = (dangergen_handle_type) a1.num;
            result = dangergen_enable(h);
            break;
        }
        case DANGERGEN_QDI_DISABLE:
        {
            dangergen_handle_type h = (dangergen_handle_type) a1.num;
            result = dangergen_disable(h);
            break;
        }
        case DANGERGEN_QDI_REFRESH:
        {
            dangergen_handle_type h = (dangergen_handle_type) a1.num;
            result = dangergen_refresh(h);
            break;
        }
        case DANGERGEN_QDI_SWITCH_MODE:
        {
            dangergen_handle_type h = (dangergen_handle_type) a1.num;
            dangergen_generator_enable_mode mode = (dangergen_generator_enable_mode)a2.num;
            result = dangergen_switch_mode (h, mode);
            break;
        }
        default:
        {
            result = qurt_qdi_method_default(client_handle, obj, method,
                a1, a2, a3, a4, a5, a6, a7, a8, a9);
            break;
        }
    }

    return result;

}

static int dangergen_qdi_opener_invocation(
    int client_handle,
    qurt_qdi_obj_t *obj,
    int method,
    qurt_qdi_arg_t a1,
    qurt_qdi_arg_t a2,
    qurt_qdi_arg_t a3,
    qurt_qdi_arg_t a4,
    qurt_qdi_arg_t a5,
    qurt_qdi_arg_t a6,
    qurt_qdi_arg_t a7,
    qurt_qdi_arg_t a8,
    qurt_qdi_arg_t a9)
{

    dangergen_qdi_obj_type* dangergen_qdi = (dangergen_qdi_obj_type*) obj;
    dangergen_driver_type* self = dangergen_qdi->self;

    int result = 0;

    switch(method)
    {
        case QDI_OPEN:
        {
            dangergen_qdi_obj_type* dangergen_qdi_obj =
                malloc(sizeof(dangergen_qdi_obj_type));
            if(dangergen_qdi_obj)
            {
                dangergen_qdi_obj->qdi_obj.invoke = dangergen_qdi_invocation;
                dangergen_qdi_obj->qdi_obj.refcnt = QDI_REFCNT_INIT;
                dangergen_qdi_obj->qdi_obj.release = dangergen_release_qdi_obj;
                dangergen_qdi_obj->self = self;
                result = qurt_qdi_new_handle_from_obj_t(client_handle,
                    &dangergen_qdi_obj->qdi_obj);
            }
            break;
        }
        case QDI_CLOSE:
        {
            // Handle unexpected close
            result = dangergen_close(dangergen_qdi->handle);
            dangergen_release_qdi_obj(obj); // check this
            break;
        }
        default:
        {
            result = qurt_qdi_method_default(client_handle, obj, method,
                a1, a2, a3, a4, a5, a6, a7, a8, a9);
            break;
        }
    }

    return result;

}

static dangergen_qdi_obj_type dangergen_qdi_opener =
{
    {
        dangergen_qdi_opener_invocation,
        QDI_REFCNT_PERM,
        0 // never released
    },
    NULL
};


dangergen_result_type dangergen_qdi_init(dangergen_driver_type* self)
{

    dangergen_result_type result = DANGERGEN_ERROR;

    if(self == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    dangergen_qdi_opener.self = self;

    if(qurt_qdi_devname_register(DANGERGEN_QDI_DRIVER_NAME,
        &dangergen_qdi_opener.qdi_obj) == 0)
    {
        result = DANGERGEN_SUCCESS;
    }

    return result;

}

