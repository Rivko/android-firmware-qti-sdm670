/*==============================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.
All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "chre/core/gnss_manager.h"

#include "chre/core/event_loop_manager.h"
#include "chre/util/macros.h"
#include "chre/platform/assert.h"
#include "chre/platform/fatal_error.h"

#include <chre/pal/qc_geofence.h>

using chre::EventLoopManager;
using chre::EventLoopManagerSingleton;
using chre::memoryAlloc;
using chre::memoryFree;

namespace qcgf {

    static void gf_heapFree(uint16_t eventType, void *eventData)
    {
        FARF(LOW, "gf_heapFree eventType=%d", eventType);
        switch (eventType)
        {
        case CHRE_EVENT_GNSS_GF_RESPONSE:
            qcGeofenceResponseEventType*  pRespEvent;
            pRespEvent = (qcGeofenceResponseEventType*)eventData;
            if (pRespEvent->count != 0) {
                memoryFree((void*)pRespEvent->err);
                memoryFree((void*)pRespEvent->ids);
            }
            break;

        case CHRE_EVENT_GNSS_GF_BREACH:
            qcGeofenceBreachEventType*  pBreachEvent;
            pBreachEvent = (qcGeofenceBreachEventType*)eventData;
            if (pBreachEvent->count != 0) {
                memoryFree((void*)pBreachEvent->ids);
            }
            break;
        }
        memoryFree(eventData);
    }

    void test_gf_geofenceResponse(qcGeofenceResponseEventType responseEvent)
    {
        qcGeofenceResponseEventType*  pRespEvent;
        uint8_t*                    pErr;
        uint32_t*                   pIds;

        FARF(LOW, "test_gf_geofenceResponse count %d",
            responseEvent.count);

        pRespEvent = (qcGeofenceResponseEventType*)memoryAlloc(sizeof(qcGeofenceResponseEventType));
        if (NULL == pRespEvent) {
            FATAL_ERROR("memoryAlloc failed for pRespEvent");
            return;
        }
        pRespEvent->count = responseEvent.count;
        pErr = (uint8_t*)memoryAlloc((pRespEvent->count)*sizeof(uint8_t));
        if (NULL == pErr) {
            FATAL_ERROR("memoryAlloc failed for pErr");
            return;
        }
        pIds = (uint32_t*)memoryAlloc((pRespEvent->count)*sizeof(uint32_t));
        if (NULL == pIds) {
            FATAL_ERROR("memoryAlloc failed for pIds");
            return;
        }

        memcpy(pErr, responseEvent.err, (pRespEvent->count)*sizeof(uint8_t));
        memcpy(pIds, responseEvent.ids, (pRespEvent->count)*sizeof(uint32_t));

        pRespEvent->err = pErr;
        pRespEvent->ids = pIds;
        for (uint32_t i = 0; i < pRespEvent->count; i++) {
            FARF(LOW, "    err[%d]=%d id[%d]=%d",
                i, *(pRespEvent->err + i),
                i, *(pRespEvent->ids + i));
        }

        bool eventPosted = EventLoopManagerSingleton::get()->getEventLoop()
            .postEvent(CHRE_EVENT_GNSS_GF_RESPONSE, pRespEvent, gf_heapFree,
                chre::kSystemInstanceId, chre::kBroadcastInstanceId);
        if (!eventPosted) {
            FATAL_ERROR("Failed to send Geofence response event");
        }
    }

    void test_gf_geofenceBreach(qcGeofenceBreachEventType breachEvent)
    {
        qcGeofenceBreachEventType*  pBreachEvent;
        uint32_t*                   pIds;

        FARF(LOW, "test_gf_geofenceBreach count %d timestamp=%lld",
            breachEvent.count, breachEvent.timestamp);

        for (uint32_t i = 0; i < breachEvent.count; i++) {
            FARF(LOW, "    ids[%d]=%d", i, *(breachEvent.ids + i));
        }
        FARF(LOW, "    latitude=%lf", breachEvent.location.latitude);
        FARF(LOW, "    longitude=%lf", breachEvent.location.longitude);
        FARF(LOW, "    altitude=%f", breachEvent.location.altitude);
        FARF(LOW, "    speed=%f", breachEvent.location.speed);
        FARF(LOW, "    bearing=%f", breachEvent.location.bearing);
        FARF(LOW, "    accuracy=%f", breachEvent.location.accuracy);
        FARF(LOW, "    flags=0X%X", breachEvent.location.flags);
        FARF(LOW, "    type=%d", breachEvent.breach_type);

        pBreachEvent = (qcGeofenceBreachEventType*)memoryAlloc(sizeof(qcGeofenceBreachEventType));
        if (NULL == pBreachEvent) {
            FATAL_ERROR("memoryAlloc failed for pBreachEvent");
            return;
        }
        pIds = (uint32_t*)memoryAlloc((pBreachEvent->count)*sizeof(uint32_t));
        if (NULL == pIds) {
            FATAL_ERROR("memoryAlloc failed for pIds");
            return;
        }

        pBreachEvent->count = breachEvent.count;
        memcpy(pIds, breachEvent.ids, (pBreachEvent->count)*sizeof(uint32_t));
        memcpy(&(pBreachEvent->location), &(breachEvent.location), sizeof(qcGeofenceLocationType));
        pBreachEvent->breach_type = breachEvent.breach_type;
        pBreachEvent->timestamp = breachEvent.timestamp;

        bool eventPosted = EventLoopManagerSingleton::get()->getEventLoop()
            .postEvent(CHRE_EVENT_GNSS_GF_BREACH, pBreachEvent, NULL,
                chre::kSystemInstanceId, chre::kBroadcastInstanceId);
        if (!eventPosted) {
            FATAL_ERROR("Failed to send Geofence breach event");
        }
    }

    DLL_EXPORT uint8_t chreQcGeofenceInit() {
        bool success = true;

        qcGeofenceCallbacksType geofence_callbacks = {
            sizeof(qcGeofenceCallbacksType),
            test_gf_geofenceResponse,
            test_gf_geofenceBreach,
        };

        chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);
        success = nanoapp->registerForBroadcastEvent(CHRE_EVENT_GNSS_GF_RESPONSE);
        if (!success) {
            LOGE("Failed to register nanoapp for Geofence Response events");
        }
        success = nanoapp->registerForBroadcastEvent(CHRE_EVENT_GNSS_GF_BREACH);
        if (!success) {
            LOGE("Failed to register nanoapp for Geofence Breach events");
        }
        return qcGeofenceInit(&geofence_callbacks);
    }

    DLL_EXPORT uint8_t chreQcGeofenceDeinit() {
        bool success = true;

        chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);

        success = nanoapp->unregisterForBroadcastEvent(CHRE_EVENT_GNSS_GF_RESPONSE);
        if (!success) {
            LOGE("Failed to unregister nanoapp for Geofence Response events");
        }
        success = nanoapp->unregisterForBroadcastEvent(CHRE_EVENT_GNSS_GF_BREACH);
        if (!success) {
            LOGE("Failed to unregister nanoapp for Geofence Breach events");
        }
        return qcGeofenceDeinit();
    }

    DLL_EXPORT uint8_t chreQcGeofenceAdd(size_t count,
        qcGeofenceOptionsType const* pOptions,
        qcGeofenceDataType const* pData,
        uint32_t** pIdArray)
    {
        return qcGeofenceAdd(count, pOptions, pData, pIdArray);
    }

    DLL_EXPORT uint8_t chreQcGeofenceRemove(size_t count, uint32_t const* pIDs)
    {
        return qcGeofenceRemove(count, pIDs);
    }

    DLL_EXPORT uint8_t chreQcGeofenceModify(size_t count, uint32_t const* pIDs,
        qcGeofenceOptionsType const* options)
    {
        return qcGeofenceModify(count, pIDs, options);
    }

    DLL_EXPORT uint8_t chreQcGeofencePause(size_t count, uint32_t const* pIDs)
    {
        return qcGeofencePause(count, pIDs);
    }

    DLL_EXPORT uint8_t chreQcGeofenceResume(size_t count, uint32_t const* pIDs)
    {
        return qcGeofenceResume(count, pIDs);
    }
}