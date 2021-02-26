/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_user.c
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#include <stdlib.h>
#include "qurt.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi.h"
#include "dangergen.h"
#include "dangergen_qdi_constants.h"


typedef struct
{
    dangergen_handle_type dangergen_handle; // must be first field
    int qdi_handle;
} dangergen_handle_wrapper_type;


dangergen_result_type dangergen_open(dangergen_open_params_type* params,
    dangergen_handle_type* handle_out)
{
    dangergen_result_type result = DANGERGEN_ERROR;
    int qdi_handle;

    if(params == NULL || handle_out == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    qdi_handle = qurt_qdi_open(DANGERGEN_QDI_DRIVER_NAME);
    if(qdi_handle >= 0)
    {
        dangergen_handle_wrapper_type* wrapper = 
            malloc(sizeof(dangergen_handle_wrapper_type));
        if(wrapper)
        {
            result = qurt_qdi_handle_invoke(qdi_handle, DANGERGEN_QDI_OPEN,
                params, &wrapper->dangergen_handle);
            if(result == DANGERGEN_SUCCESS)
            {
                wrapper->qdi_handle = qdi_handle;
                // The caller's 'handle' is actually a pointer to a wrapper
                // structure which has the dangergen handle and qdi handle
                *handle_out = wrapper;
            }
            else
            {
                qurt_qdi_close(qdi_handle);
                free(wrapper);
            }
        }
        else
        {
           qurt_qdi_close(qdi_handle);
        }
    }

    return result;

}


dangergen_result_type dangergen_close(dangergen_handle_type h)
{

    dangergen_result_type result = DANGERGEN_ERROR;
    dangergen_handle_wrapper_type* wrapper = (dangergen_handle_wrapper_type*) h;

    if(h == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    result = qurt_qdi_handle_invoke(wrapper->qdi_handle, DANGERGEN_QDI_CLOSE,
        wrapper->dangergen_handle);
    if(result == DANGERGEN_SUCCESS)
    {
        qurt_qdi_close(wrapper->qdi_handle);
        free(wrapper);
    }

    return result;

}


dangergen_result_type dangergen_configure_watchdog(dangergen_handle_type h,
    dangergen_watchdog_params_type* params)
{

    dangergen_handle_wrapper_type* wrapper = (dangergen_handle_wrapper_type*) h;

    if(h == NULL || params == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    return qurt_qdi_handle_invoke(wrapper->qdi_handle, DANGERGEN_QDI_CONFIGURE,
        wrapper->dangergen_handle, params);

}


dangergen_result_type dangergen_enable(dangergen_handle_type h)
{

    dangergen_handle_wrapper_type* wrapper = (dangergen_handle_wrapper_type*) h;

    if(h == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    return qurt_qdi_handle_invoke(wrapper->qdi_handle, DANGERGEN_QDI_ENABLE,
        wrapper->dangergen_handle);

}

dangergen_result_type dangergen_switch_mode(dangergen_handle_type h,
    dangergen_generator_enable_mode mode)
{

    dangergen_handle_wrapper_type* wrapper = (dangergen_handle_wrapper_type*) h;

    if((h == NULL) ||
       (mode >= DANGERGEN_ENABLE_MODE_MAX))
        return DANGERGEN_ERROR_BADPARAM;

    return qurt_qdi_handle_invoke(wrapper->qdi_handle, DANGERGEN_QDI_ENABLE,
        wrapper->dangergen_handle, mode);

}

dangergen_result_type dangergen_disable(dangergen_handle_type h)
{

    dangergen_handle_wrapper_type* wrapper = (dangergen_handle_wrapper_type*) h;

    if(h == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    return qurt_qdi_handle_invoke(wrapper->qdi_handle, DANGERGEN_QDI_DISABLE,
        wrapper->dangergen_handle);

}


dangergen_result_type dangergen_refresh(dangergen_handle_type h)
{

    dangergen_handle_wrapper_type* wrapper = (dangergen_handle_wrapper_type*) h;

    if(h == NULL)
        return DANGERGEN_ERROR_BADPARAM;

    return qurt_qdi_handle_invoke(wrapper->qdi_handle, DANGERGEN_QDI_REFRESH,
        wrapper->dangergen_handle);

}


