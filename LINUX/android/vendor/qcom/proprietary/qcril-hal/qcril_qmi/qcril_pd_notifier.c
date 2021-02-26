/******************************************************************************
#  Copyright (c) 2016, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_pd_notifier.c

  @brief
  Get notifications of PD status

*/


#include <stdint.h>
#include "ref_counted.h"
#include "qcril_pd_notifier.h"

#ifdef QCRIL_USE_PD_NOTIFIER
#include <libpdmapper.h>
#include <libpdnotifier.h>
#include <service_registry_notifier_v01.h>
#define SERVNAME_LENGTH (sizeof(((SR_DL_Handle *)NULL)->service_name))
#define NOTIFIER_NAME_LENGTH (sizeof(((PD_Notifier_Handle *)NULL)->serviceName))
#define NOTIFIER_CLIENT_LENGTH (sizeof(((PD_Notifier_Handle *)NULL)->clientName))
#endif
#include "qcrili.h"
#ifndef QCRIL_NOTUSED
#define QCRIL_NOTUSED(x) (void) x
#endif


struct qcril_qmi_pd_entry
{
    REF_COUNTED_DECL;
    char *notifier_string;
    int instance;
    int service_data_valid;
    int service_data;
    uint32_t entry_number;
    struct qcril_qmi_pd_handle *owner;
    qcril_qmi_pd_notifier_cb cb;
    void *userdata;
    qcril_qmi_pd_state state;
#ifdef QCRIL_USE_PD_NOTIFIER
    char name[SERVNAME_LENGTH];
    char client_name[NOTIFIER_CLIENT_LENGTH];
    PD_Notifier_Handle *pd_monitor_handle;
#endif
};

struct qcril_qmi_pd_handle
{
#ifdef QCRIL_USE_PD_NOTIFIER
    char service[SERVNAME_LENGTH];
    char client_name[NOTIFIER_CLIENT_LENGTH];
    SR_DL_Handle *pd_mapper_handle;
#endif

    ssize_t n_entries;
    qcril_qmi_pd_entry **pd_entry;
    void *pd_monitor_cb;
    uint8_t available;
};

typedef enum qcril_qmi_pd_notifier_error
{
    QCRIL_QMI_PD_NOTIFIER_SUCCESS,
    QCRIL_QMI_PD_NOTIFIER_E_GENERIC,
    QCRIL_QMI_PD_NOTIFIER_E_NOMEM,
    QCRIL_QMI_PD_NOTIFIER_E_INVALID_ARG,
} qcril_qmi_pd_notifier_error;

#ifdef QCRIL_USE_PD_NOTIFIER
qcril_qmi_pd_state pd_event_to_qcril_qmi_pd_state(enum pd_event event)
{
    qcril_qmi_pd_state ret;
    switch(event) {
        case EVENT_PD_DOWN:
            ret = QCRIL_QMI_PD_DOWN;
            break;
        case EVENT_PD_UP:
            ret = QCRIL_QMI_PD_UP;
            break;
        default:
            ret = QCRIL_QMI_PD_UNKNOWN;
            break;
    }
    return ret;
}

qcril_qmi_pd_state pd_state_to_qcril_qmi_pd_state(pd_state state)
{
    qcril_qmi_pd_state ret = QCRIL_QMI_PD_UNKNOWN;

    switch(state) {
        case SERVREG_NOTIF_SERVICE_STATE_DOWN_V01:
            ret = QCRIL_QMI_PD_DOWN;
            break;
        case SERVREG_NOTIF_SERVICE_STATE_UP_V01:
            ret = QCRIL_QMI_PD_UP;
            break;
        default:
            ret = QCRIL_QMI_PD_UNKNOWN;
    }
    return ret;
}

static
void qcril_qmi_pd_notifier_callback(void *userdata, enum pd_event event)
{
    qcril_qmi_pd_entry *entry = (qcril_qmi_pd_entry *)userdata;
    qcril_qmi_pd_state state = pd_event_to_qcril_qmi_pd_state(event);

    if (entry)
    {
        if (entry->cb)
        {
            entry->cb(entry, state, entry->userdata);
        }
        entry->state = state;
    }
}

static
void qcril_qmi_pd_entry_free(void *strct, ref_counted *rfcnt)
{
    qcril_qmi_pd_entry *entry = (qcril_qmi_pd_entry *)strct;
    QCRIL_NOTUSED(rfcnt);

    free(entry->notifier_string);
    qcril_free(entry);
}

static qcril_qmi_pd_entry *
qcril_qmi_pd_notifier_entry_new
(
    qcril_qmi_pd_handle *handle,
    uint32 entry,
    qcril_qmi_pd_state state
)
{
    enum SR_Result_Enum mrc;
    qcril_qmi_pd_entry *ret = NULL;
    qcril_qmi_pd_notifier_error error = QCRIL_QMI_PD_NOTIFIER_SUCCESS;
    char *name;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        ret = qcril_malloc(
                sizeof(qcril_qmi_pd_entry));
        if (!ret)
        {
            QCRIL_LOG_ERROR("Memory allocation error while allocating entry");
            error = QCRIL_QMI_PD_NOTIFIER_E_NOMEM;
            break;
        }

        REF_COUNTED_INIT(ret, qcril_qmi_pd_entry_free);

        // TODO: Query for initial state of PD
        ret->state = state;
        ret->entry_number = entry;
        ret->owner = handle;
        mrc = servreg_get_entry(handle->pd_mapper_handle,
                              &name,
                              &ret->instance,
                              &ret->service_data_valid,
                              &ret->service_data,
                              entry);

        if (mrc != SR_RESULT_SUCCESS)
        {
            QCRIL_LOG_ERROR("Error obtaining entry");
            error = QCRIL_QMI_PD_NOTIFIER_E_GENERIC;
            break;
        }

        strlcpy(ret->name, name, sizeof(ret->name));
        mrc = servreg_get_data_for_notifier(
                handle->pd_mapper_handle,
                &ret->notifier_string,
                &ret->instance,
                entry);

        if (mrc != SR_RESULT_SUCCESS)
        {
            QCRIL_LOG_ERROR("Error getting data for notifier");
            break;
        }

        strlcpy(ret->client_name, handle->client_name, sizeof(ret->client_name));
    } while(0);
    if (error)
    {
        REF_COUNTED_REPL(ret, (qcril_qmi_pd_entry *)NULL);
    }
 
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

qcril_qmi_pd_notifier_error
qcril_qmi_pd_notifier_set_entries(qcril_qmi_pd_handle *handle)
{
    ssize_t i;
    void *entries;
    qcril_qmi_pd_notifier_error error = QCRIL_QMI_PD_NOTIFIER_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if ( !handle)
        {
            QCRIL_LOG_ERROR("Invalid handle");
            error = QCRIL_QMI_PD_NOTIFIER_E_INVALID_ARG;
            break;
        }

        handle->n_entries = servreg_get_numentries(handle->pd_mapper_handle);

        if (handle->n_entries < 0)
        {
            break;
        }

        handle->pd_entry = qcril_malloc(
                sizeof(qcril_qmi_pd_entry *) * handle->n_entries);

        if (!handle->pd_entry)
        {
            QCRIL_LOG_ERROR("Memory allocation error");
            error = QCRIL_QMI_PD_NOTIFIER_E_NOMEM;
            break;
        }

        for (i = 0; i < handle->n_entries; i++)
        {
            handle->pd_entry[i] = qcril_qmi_pd_notifier_entry_new(
                    handle,
                    i,
                    QCRIL_QMI_PD_UNKNOWN);
            if (!handle->pd_entry[i])
            {
                QCRIL_LOG_ERROR("Error creating entry");
                error = QCRIL_QMI_PD_NOTIFIER_E_GENERIC;
                break;
            }

        }
        if (i < handle->n_entries) break;

    } while (0);

    if (error != QCRIL_QMI_PD_NOTIFIER_SUCCESS)
    {
        if(handle && handle->pd_entry) {
            for(i = 0; i < handle->n_entries; i++)
            {
                REF_COUNTED_REPL(handle->pd_entry[i], (qcril_qmi_pd_entry *)NULL);
            }
            entries = handle->pd_entry;
            handle->pd_entry = NULL;
            qcril_free(entries);
        }
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET(error);
    return error;
}
#endif

qcril_qmi_pd_handle * qcril_qmi_pd_notifier_new(const char *service, const char *client_name)
{
    qcril_qmi_pd_notifier_error error = QCRIL_QMI_PD_NOTIFIER_SUCCESS;
    qcril_qmi_pd_handle *pd_handle = NULL;
    qcril_qmi_pd_notifier_error nrc;
#ifdef QCRIL_USE_PD_NOTIFIER
    enum SR_Result_Enum mrc;
#endif
    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!service)
        {
            QCRIL_LOG_ERROR("Invalid handle");
            error = QCRIL_QMI_PD_NOTIFIER_E_INVALID_ARG;
            break;
        }

        pd_handle = qcril_malloc(sizeof(*pd_handle));
        if (!pd_handle)
        {
            QCRIL_LOG_ERROR("Memory allocation failure");
            error = QCRIL_QMI_PD_NOTIFIER_E_NOMEM;
            break;
        }

#ifdef QCRIL_USE_PD_NOTIFIER
        strlcpy(pd_handle->service, service, sizeof(pd_handle->service));
        strlcpy(pd_handle->client_name,client_name, sizeof(pd_handle->client_name));
        pd_handle->pd_mapper_handle = servreg_alloc_DLHandle();
        if (!pd_handle->pd_mapper_handle)
        {
            QCRIL_LOG_ERROR("Memory allocation failure");
            error = QCRIL_QMI_PD_NOTIFIER_E_NOMEM;
            break;
        }

        mrc = servreg_get_domainlist((char *)service, pd_handle->pd_mapper_handle);
        if (mrc != SR_RESULT_SUCCESS)
        {
            QCRIL_LOG_ERROR("Error getting domain list (%d)", mrc);
            error = QCRIL_QMI_PD_NOTIFIER_E_GENERIC;
            break;
        }

        nrc = qcril_qmi_pd_notifier_set_entries(pd_handle);
        if (nrc != QCRIL_QMI_PD_NOTIFIER_SUCCESS)
        {
            QCRIL_LOG_ERROR("Error creating entries (%d)", nrc);
            error = QCRIL_QMI_PD_NOTIFIER_E_GENERIC;
            break;
        }
        pd_handle->available = TRUE;
#else
        QCRIL_NOTUSED(nrc);
        QCRIL_NOTUSED(client_name);
        pd_handle->available = FALSE;
#endif
    }while(0);

    if (error)
    {
#ifdef QCRIL_USE_PD_NOTIFIER
        if (pd_handle)
        {
            servreg_free_DLHandle(pd_handle->pd_mapper_handle);
        }
#endif
        if (pd_handle)
        {
            qcril_free(pd_handle);
            pd_handle = NULL;
        }
    }
    QCRIL_LOG_FUNC_RETURN();
    return pd_handle;
}

uint8_t qcril_pd_notifier_available
(
    qcril_qmi_pd_handle *handle
)
{
    return handle->available;
}

uint32_t qcril_pd_notifier_get_n_entries(qcril_qmi_pd_handle *handle)
{
    uint32_t ret = 0;

    ret = (unsigned int)handle->n_entries;

    return ret;
}

qcril_qmi_pd_entry *qcril_pd_notifier_get_entry(qcril_qmi_pd_handle *handle, int32_t i)
{
    qcril_qmi_pd_entry *ret = NULL;

    do
    {
        if (!handle) break;
        if (i >= handle->n_entries) break;

        ret = handle->pd_entry[i];
        REF_COUNTED_INCR(ret);
    } while (0);

    return ret;
}

void qcril_pd_notifier_release_entry(qcril_qmi_pd_handle *handle, int32_t i)
{
    do
    {
        if (!handle) break;
        if (!handle->pd_entry[i]) break;

        REF_COUNTED_REPL(handle->pd_entry[i], (qcril_qmi_pd_entry *)NULL);
    } while (0);
}

void qcril_pd_notifier_put_entry(qcril_qmi_pd_entry *entry)
{
    if (entry)
    {
        REF_COUNTED_DECR(entry);
    }
}

void qcril_pd_notifier_register_for_entry
(
    qcril_qmi_pd_entry *entry,
    qcril_qmi_pd_notifier_cb cb,
    void *userdata
)
{
    QCRIL_LOG_FUNC_ENTRY();
    if (entry)
    {
#ifdef QCRIL_USE_PD_NOTIFIER
        if (!entry->pd_monitor_handle)
        {
            REF_COUNTED_INCR(entry);
            entry->cb = cb;
            entry->userdata = userdata;
            entry->pd_monitor_handle =
                    pd_notifier_alloc(
                            entry->name,
                            entry->client_name,
                            entry->instance,
                            qcril_qmi_pd_notifier_callback,
                            entry);
            if (!entry->pd_monitor_handle)
            {
                QCRIL_LOG_ERROR("Unable to allocate pd_notifier_handle for \"%s\"", entry->name);
                REF_COUNTED_DECR(entry);
            }
        }
#else
    QCRIL_NOTUSED(cb);
    QCRIL_NOTUSED(userdata);
#endif
    }
    QCRIL_LOG_FUNC_RETURN();
}

void qcril_pd_notifier_unregister_for_entry(qcril_qmi_pd_entry *entry)
{
#ifdef QCRIL_USE_PD_NOTIFIER
    PD_Notifier_Handle *tmp;
#endif

    if (entry)
    {
#ifdef QCRIL_USE_PD_NOTIFIER
        if (!entry->pd_monitor_handle)
        {
            tmp = entry->pd_monitor_handle;
            entry->pd_monitor_handle = NULL;
            pd_notifier_free(tmp);
            REF_COUNTED_DECR(entry);
        }
#endif
    }
}

static qcril_qmi_pd_entry *find_entry
(
    qcril_qmi_pd_handle *handle,
    const char *domain
)
{
    int32_t i;
    qcril_qmi_pd_entry *entry = NULL;

#ifdef QCRIL_USE_PD_NOTIFIER
    for (i = 0; i < handle->n_entries; i++)
    {
        if (!strcmp(handle->pd_entry[i]->name, domain)) {
            entry = handle->pd_entry[i];
            break;
        }
    }
#else
    QCRIL_NOTUSED(handle);
    QCRIL_NOTUSED(domain);
    QCRIL_NOTUSED(i);
#endif

    return entry;
}

void qcril_pd_notifier_register_for
(
    qcril_qmi_pd_handle *handle,
    const char *domain,
    qcril_qmi_pd_notifier_cb cb,
    void *userdata
)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_pd_entry * entry = NULL;
    do
    {
        if (!handle) {
            QCRIL_LOG_ERROR("Null handle");
            break;
        }

        entry = find_entry(handle, domain);

        if (entry)
        {
            qcril_pd_notifier_register_for_entry(entry, cb, userdata);
        }
        else
        {
            QCRIL_LOG_ERROR("Couldn't find entry for domain %s", domain);
        }
    } while(0);
    QCRIL_LOG_FUNC_RETURN();
}

void qcril_pd_notifier_register_for_all(qcril_qmi_pd_handle *handle, qcril_qmi_pd_notifier_cb cb, void *userdata)
{
    int32_t i;
    do
    {
        for (i = 0; i < handle->n_entries; i++)
        {
            if (!handle->pd_entry[i]) continue;

            qcril_pd_notifier_register_for_entry(handle->pd_entry[i], cb, userdata);
        }
    } while(0);
}

void qcril_pd_notifier_unregister_for_all(qcril_qmi_pd_handle *handle)
{
    int32_t i;
    do
    {
        if (!handle)
        {
            break;
        }
        for (i = 0; i < handle->n_entries; i++)
        {
            if (handle->pd_entry[i])
            {
                qcril_pd_notifier_unregister_for_entry(handle->pd_entry[i]);
            }
        }
    } while (0);
}

void qcril_pd_notifier_start_listening_all(qcril_qmi_pd_handle *handle)
{
#ifdef QCRIL_USE_PD_NOTIFIER
    pd_rcode rc;
    pd_state state;
#endif
    int32_t i;
    do
    {
        for (i = 0; i < handle->n_entries; i++)
        {
#ifdef QCRIL_USE_PD_NOTIFIER
            rc = pd_notifier_register(handle->pd_entry[i]->pd_monitor_handle, &state);
            handle->pd_entry[i]->state = pd_state_to_qcril_qmi_pd_state(state);
#endif
        }
    } while(0);
}

void qcril_pd_notifier_stop_listening_all(qcril_qmi_pd_handle *handle)
{
#ifdef QCRIL_USE_PD_NOTIFIER
    pd_rcode rc;
#endif
    int32_t i;
    do
    {
        for (i = 0; i < handle->n_entries; i++)
        {
#ifdef QCRIL_USE_PD_NOTIFIER
            rc = pd_notifier_deregister(handle->pd_entry[i]->pd_monitor_handle);
#endif
        }
    } while(0);
}

void qcril_pd_notifier_stop_listening
(
    qcril_qmi_pd_handle *handle,
    const char *domain
)
{
    qcril_qmi_pd_entry *entry = NULL;

    entry = find_entry(handle, domain);

    if (entry)
    {
#ifdef QCRIL_USE_PD_NOTIFIER
        pd_rcode rc;
        rc = pd_notifier_deregister(entry->pd_monitor_handle);
        if (rc != PD_NOTIFIER_SUCCESS)
        {
            QCRIL_LOG_ERROR("Error in pd_notifier_deregister %d", rc);
        }
#endif
    }
}
void qcril_pd_notifier_start_listening
(
    qcril_qmi_pd_handle *handle,
    const char *domain
)
{
    qcril_qmi_pd_entry *entry = NULL;

    QCRIL_LOG_FUNC_ENTRY();
    entry = find_entry(handle, domain);
    if (entry)
    {
#ifdef QCRIL_USE_PD_NOTIFIER
        pd_rcode rc;
        pd_state state;
        rc = pd_notifier_register(entry->pd_monitor_handle, &state);
        if (rc != PD_NOTIFIER_SUCCESS)
        {
            QCRIL_LOG_ERROR("Error in pd_notifier_register %d", rc);
        }
        else
        {
            entry->state = pd_state_to_qcril_qmi_pd_state(state);
            if (entry->cb)
            {
                entry->cb(entry, entry->state, entry->userdata);
            }
            QCRIL_LOG_ERROR("Updated to state %d for domain %s", entry->state, domain);
        }
#endif
    }
    else
    {
        QCRIL_LOG_ERROR("Unable to find entry for \"%s\"", domain);
    }
    QCRIL_LOG_FUNC_RETURN();
}

void qcril_qmi_pd_entry_get_data
(
    qcril_qmi_pd_entry *entry,
    const char **name,
    const char **notifier_string,
    int *instance,
    int *svc_data_valid,
    int *svc_data,
    void **userdata
)
{
    do
    {
        if (!entry)
        {
            break;
        }

#ifdef QCRIL_USE_PD_NOTIFIER
        if (name)
        {
            *name = entry->name;
        }
#else
        QCRIL_NOTUSED(name);
#endif

        if (notifier_string)
        {
            *notifier_string = entry->notifier_string;
        }

        if (instance)
        {
            *instance = entry->instance;
        }

        if (svc_data_valid)
        {
            *svc_data_valid = entry->service_data_valid;
        }

        if (svc_data)
        {
            *svc_data = entry->service_data;
        }

        if (userdata)
        {
            *userdata = entry->userdata;
        }
    } while (0);
}
