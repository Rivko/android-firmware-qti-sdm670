/**
 * @file:  cpr_smem_vp.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/src/cpr_smem_vp.c#1 $
 * $Change: 14402555 $
 */
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cpr_logs.h"
#include "cpr_utils.h"
#include "cpr_smem_internal.h"
#include <string.h>

static uint32 *currOffset;
static cpr_smem_hdr *hdr;

#define INCREMENT_OFFSET(size)                                                   \
do{                                                                              \
    *currOffset += (size);                                                        \
    if(*currOffset >= hdr->smemSize)                                              \
        CPR_LOG_FATAL("Exceeded memory (0x%x/0x%x)", *currOffset, hdr->smemSize); \
}while(0)

#define GET_BUFFER()    (((uint8*)hdr) + *currOffset)


static boolean cpr_smem_write_version(cpr_rail_state* state,
        const cpr_version *ver)
{
    if (!ver)
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s cpr version @ (offset: 0x%x)",
            rail->name, *currOffset);

    cpr_smem_version *s_ver = (cpr_smem_version *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_version));

    s_ver->foundry = (uint32)ver->foundry;
    s_ver->min     = ver->min;
    s_ver->max     = ver->max;

    return true;
}

static void cpr_smem_read_version(cpr_version *ver)
{
    CPR_ASSERT(ver);

    cpr_smem_version *s_ver = (cpr_smem_version *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_version));

    ver->foundry = (cpr_foundry_id)s_ver->foundry;
    ver->min     = s_ver->min;
    ver->max     = s_ver->max;
}

static boolean __cpr_smem_write_voltage_mode(cpr_rail_state* state,
        cpr_voltage_mode *list, int count)
{
    int i = 0;

    if (!count)
        return false;

    uint32* s_modes = (uint32 *) GET_BUFFER();
    INCREMENT_OFFSET((sizeof(uint32) * count));

    for (i = 0; i < count; i++)
        s_modes[i] = (uint32)list[i];

    return true;
}

static void __cpr_smem_read_voltage_mode(cpr_voltage_mode *list, int count)
{
    int i = 0;

    uint32* s_modes = (uint32 *)GET_BUFFER();
    INCREMENT_OFFSET((sizeof(uint32) * count));

    for (i = 0; i < count; i++)
        list[i] = (cpr_voltage_mode) s_modes[i];
}

static boolean cpr_smem_write_supported_voltage_mode(cpr_rail_state* state,
        cpr_voltage_plan *vp)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s supportediModes[%d] starting @ offset: 0x%x)",
            rail->name, vp->modesCount, *currOffset);

    return __cpr_smem_write_voltage_mode(state, vp->supportedModes,
            vp->modesCount);
}

static void cpr_smem_read_supported_voltage_mode(cpr_voltage_plan *vp)
{
    CPR_ASSERT(vp->modesCount);

    vp->supportedModes = cpr_image_malloc(sizeof(cpr_voltage_mode) *
            vp->modesCount);
    CPR_ASSERT(vp->supportedModes);

    __cpr_smem_read_voltage_mode(vp->supportedModes, vp->modesCount);
}

static boolean cpr_smem_write_temp_adj_cfg(cpr_rail_state* state,
        cpr_voltage_plan *vp)
{
    int i = 0;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s temerature adj config @ (offset: 0x%x)",
            rail->name, *currOffset);

    if (!(vp->tempAdjCfg))
        return false;

    cpr_smem_temp_adj_cfg* s_tempAdjCfg = (cpr_smem_temp_adj_cfg*)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_temp_adj_cfg));

    cpr_temp_adj_cfg* tempAdjCfg = vp->tempAdjCfg;

    s_tempAdjCfg->tempSensorStartId = tempAdjCfg->tempSensorStartId;
    s_tempAdjCfg->tempSensorEndId = tempAdjCfg->tempSensorEndId;

    for (i = 0; i < 3; i++)
        s_tempAdjCfg->tempPoints[i] = tempAdjCfg->tempPoints[i];

    return true;
}

static void cpr_smem_read_temp_adj_cfg(cpr_voltage_plan *vp)
{
    int i = 0;

    cpr_smem_temp_adj_cfg* s_tempAdjCfg = (cpr_smem_temp_adj_cfg*)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_temp_adj_cfg));

    vp->tempAdjCfg = cpr_image_malloc(sizeof(cpr_temp_adj_cfg));
    cpr_temp_adj_cfg* tempAdjCfg = vp->tempAdjCfg;

    tempAdjCfg->tempSensorStartId = s_tempAdjCfg->tempSensorStartId;
    tempAdjCfg->tempSensorEndId = s_tempAdjCfg->tempSensorEndId;

    for (i = 0; i < 3; i++)
        tempAdjCfg->tempPoints[i] = s_tempAdjCfg->tempPoints[i];
}

static boolean cpr_smem_write_kvs(cpr_rail_state* state, cpr_kv_cfg *kvCfg)
{
    int i = 0;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s kv's[%d] @ (offset: 0x%x)",
            rail->name, kvCfg->count, *currOffset);

    if (!(kvCfg->count))
        return false;

    for(i = 0; i < kvCfg->count; i++)
    {
        cpr_smem_kv* s_kvs = (cpr_smem_kv *)GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_kv));

        cpr_kv *kvs = &(kvCfg->kvs[i]);

        s_kvs->ro = kvs->ro;
        s_kvs->kv = kvs->kv;
    }

    return true;
}

static void cpr_smem_read_kvs(cpr_kv_cfg *kvCfg)
{
    int i = 0;

    CPR_ASSERT(kvCfg->count);

    kvCfg->kvs = cpr_image_malloc(sizeof(cpr_kv) * kvCfg->count);
    CPR_ASSERT(kvCfg->kvs);

    cpr_smem_kv* s_kvs = (cpr_smem_kv *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_kv) * kvCfg->count);

    for(i = 0; i < kvCfg->count; i++)
    {
        cpr_kv *kvs = &(kvCfg->kvs[i]);
        cpr_smem_kv *ss_kvs = &(s_kvs[i]);

        kvs->ro = ss_kvs->ro;
        kvs->kv = ss_kvs->kv;
    }
}

static boolean cpr_smem_write_kv_cfg(cpr_rail_state* state,
        cpr_voltage_plan *vp)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s kv config @ (offset: 0x%x)",
            rail->name, *currOffset);

    cpr_smem_kv_cfg *s_kvCfg = (cpr_smem_kv_cfg *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_kv_cfg));

    cpr_kv_cfg *kvCfg = vp->kvCfg;

    s_kvCfg->count = kvCfg->count;
    s_kvCfg->kvs = cpr_smem_write_kvs(state, kvCfg);

    return true;
}

static void cpr_smem_read_kv_cfg(cpr_voltage_plan *vp)
{
    cpr_smem_kv_cfg *s_kvCfg = (cpr_smem_kv_cfg *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_kv_cfg));

    vp->kvCfg = cpr_image_malloc(sizeof(cpr_kv_cfg));
    CPR_ASSERT(vp->kvCfg);

    cpr_kv_cfg *kvCfg = vp->kvCfg;

    kvCfg->count = s_kvCfg->count;
    cpr_smem_read_kvs(kvCfg);
}

static boolean cpr_smem_write_margin_data(cpr_rail_state* state,
        cpr_margin_cfg *margins)
{
    int i = 0;
    if (!(margins->count))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s margin data[%d] @ (offset: 0x%x)",
            rail->name, margins->count, *currOffset);

    for (i = 0; i < margins->count; i++) {
        cpr_smem_margin_data *s_data = (cpr_smem_margin_data *)GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_margin_data));

        cpr_margin_data *data = &(margins->data[i]);

        s_data->cprRevMin = data->cprRevMin;
        s_data->cprRevMax = data->cprRevMax;
        s_data->openLoop = data->openLoop;
        s_data->closedLoop = data->closedLoop;
        s_data->maxFloorToCeil = data->maxFloorToCeil;
    }

    return true;
}

static void cpr_smem_read_margin_data(cpr_margin_cfg *margins)
{
    int i = 0;

    CPR_ASSERT(margins->count);
    margins->data = cpr_image_malloc(sizeof(cpr_margin_data) *
            margins->count);

    for (i = 0; i < margins->count; i++) {
        cpr_smem_margin_data *s_data = (cpr_smem_margin_data *)GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_margin_data));

        cpr_margin_data *data = &(margins->data[i]);

        data->cprRevMin       = s_data->cprRevMin;
        data->cprRevMax       = s_data->cprRevMax;
        data->openLoop        = s_data->openLoop;
        data->closedLoop      = s_data->closedLoop;
        data->maxFloorToCeil  = s_data->maxFloorToCeil;
    }
}

static boolean cpr_smem_write_margin_cfg(cpr_rail_state* state,
        struct cpr_voltage_data* modes, int modeIdx)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s Mode %d Margin Cfg @ (offset: 0x%x)",
            rail->name, modeIdx, *currOffset);

    if (!(modes->margins))
        return false;

    cpr_smem_margin_cfg* s_margins = (cpr_smem_margin_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_margin_cfg));

    cpr_margin_cfg* margins = modes->margins;

    s_margins->count = margins->count;
    s_margins->data = cpr_smem_write_margin_data(state, margins); 

    return true;
}

static void cpr_smem_read_margin_cfg(struct cpr_voltage_data* modes)
{
    cpr_smem_margin_cfg* s_margins = (cpr_smem_margin_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_margin_cfg));

    modes->margins = cpr_image_malloc(sizeof(cpr_margin_cfg));
    cpr_margin_cfg* margins = modes->margins;

    margins->count = s_margins->count;
    cpr_smem_read_margin_data(margins); 
}


static boolean cpr_smem_write_raw_fuse_data(cpr_rail_state* state,
        cpr_fuse* fuse, const char *fuse_name)
{
    int i = 0;

    if (!(fuse->count))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s %s raw fuse data[%d] @ (offset: 0x%x)",
            rail->name, fuse_name, fuse->count, *currOffset);

    for (i = 0; i < fuse->count; i++) {
        cpr_smem_raw_fuse_data* s_data = (cpr_smem_raw_fuse_data*) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_raw_fuse_data));

        struct raw_fuse_data* data = &(fuse->data[i]);

        s_data->address = (uint32)data->address;
        s_data->offset = data->offset;
        s_data->mask = data->mask;
    }

    return true;
}

static void cpr_smem_read_raw_fuse_data(cpr_fuse* fuse)
{
    int i = 0;
    CPR_ASSERT(fuse->count);

    fuse->data = cpr_image_malloc(sizeof(struct raw_fuse_data) * fuse->count);
    CPR_ASSERT(fuse->data);

    for (i = 0; i < fuse->count; i++) {
        cpr_smem_raw_fuse_data* s_data = (cpr_smem_raw_fuse_data*) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_raw_fuse_data));

        struct raw_fuse_data* data = &(fuse->data[i]);

        data->address = (uintptr_t)s_data->address;
        data->offset  = s_data->offset;
        data->mask    = s_data->mask;
    }
}

static boolean cpr_smem_write_fuse(cpr_rail_state* state,
        cpr_fuse* fuse, const char *fuse_name)
{
    if (!fuse)
        return false;
    
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s %s fuse @ (offset: 0x%x)",
            rail->name, fuse_name, *currOffset);

    cpr_smem_fuse* s_fuse = (cpr_smem_fuse *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_fuse));

    s_fuse->count = fuse->count;
    s_fuse->data = cpr_smem_write_raw_fuse_data(state, fuse, fuse_name);

    return true;
}

static void cpr_smem_read_fuse(cpr_fuse* fuse)
{
    CPR_ASSERT(fuse);

    cpr_smem_fuse* s_fuse = (cpr_smem_fuse *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_fuse));

    fuse->count = s_fuse->count;
    cpr_smem_read_raw_fuse_data(fuse);
}

static boolean cpr_smem_write_fuse_data( cpr_rail_state* state,
        struct cpr_voltage_data *modes, int modeIdx)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s Mode %d fuse data @ (offset: 0x%x)",
            rail->name, modeIdx, *currOffset);

    cpr_smem_fuse_data *s_fuses = (cpr_smem_fuse_data *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_fuse_data));

    cpr_fuse_data *fuses = modes->fuses;

    s_fuses->volt = cpr_smem_write_fuse(state, fuses->volt, "volt");
    s_fuses->quot = cpr_smem_write_fuse(state, fuses->quot, "quot");
    s_fuses->rosel = cpr_smem_write_fuse(state, fuses->rosel, "rosel");
    s_fuses->quotOffset = cpr_smem_write_fuse(state, fuses->quotOffset,
            "quotOffset");
    s_fuses->voltOffset = cpr_smem_write_fuse(state, fuses->voltOffset,
            "voltOffset");

    return true;
}

static void cpr_smem_read_fuse_data(struct cpr_voltage_data *modes)
{
    modes->fuses = cpr_image_malloc(sizeof(cpr_fuse_data));
    CPR_ASSERT(modes->fuses);

    cpr_smem_fuse_data *s_fuses = (cpr_smem_fuse_data *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_fuse_data));

    cpr_fuse_data *fuses = modes->fuses;

    if (s_fuses->volt) {
        fuses->volt = cpr_image_malloc(sizeof(cpr_fuse));
        cpr_smem_read_fuse(fuses->volt);
    }

    if (s_fuses->quot) {
        fuses->quot = cpr_image_malloc(sizeof(cpr_fuse));
        cpr_smem_read_fuse(fuses->quot);
    }

    if(s_fuses->rosel) {
        fuses->rosel = cpr_image_malloc(sizeof(cpr_fuse));
        cpr_smem_read_fuse(fuses->rosel);
    }

    if(s_fuses->quotOffset) {
        fuses->quotOffset = cpr_image_malloc(sizeof(cpr_fuse));
        cpr_smem_read_fuse(fuses->quotOffset);
    }

    if(s_fuses->voltOffset) {
        fuses->voltOffset = cpr_image_malloc(sizeof(cpr_fuse));
        cpr_smem_read_fuse(fuses->voltOffset);
    }
}

static boolean cpr_smem_write_quots(cpr_rail_state* state,
        cpr_quotient_cfg *quotients, int modeIdx)
{
    int i = 0;

    if (!(quotients->count))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s Mode %d cpr quotients[%d] @ (offset: 0x%x)",
            rail->name, modeIdx, quotients->count, *currOffset);

    for (i = 0; i < quotients->count; i++) {
        cpr_smem_quotient* s_quots = (cpr_smem_quotient *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_quotient));

        cpr_quotient* quots = &(quotients->quots[i]);

        s_quots->ro = quots->ro;
        s_quots->quotient = quots->quotient;
    }
    return true;
}

static void cpr_smem_read_quots(cpr_quotient_cfg *quotients)
{
    int i = 0;
    quotients->quots = cpr_image_malloc(sizeof(cpr_quotient) *
            quotients->count);

    for (i = 0; i < quotients->count; i++) {
        cpr_smem_quotient* s_quots = (cpr_smem_quotient *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_quotient));

        cpr_quotient* quots = &(quotients->quots[i]);

        quots->ro       = s_quots->ro;
        quots->quotient = s_quots->quotient;
    }
}

static boolean cpr_smem_write_quotient_cfg(cpr_rail_state* state,
        struct cpr_voltage_data *modes, int modeIdx)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s Mode %d quotient cfg @ (offset: 0x%x)",
            rail->name, modeIdx, *currOffset);

    if (!(modes->quotients))
        return false;

    cpr_smem_quotient_cfg* s_quotients =
        (cpr_smem_quotient_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_quotient_cfg));

    cpr_quotient_cfg* quotients = modes->quotients;

    s_quotients->count = quotients->count;
    s_quotients->quots =
        cpr_smem_write_quots(state, quotients, modeIdx);

    return true;
}

static void cpr_smem_read_quotient_cfg(struct cpr_voltage_data *modes)
{
    cpr_smem_quotient_cfg* s_quotients =
        (cpr_smem_quotient_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_quotient_cfg));

    modes->quotients = cpr_image_malloc(sizeof(cpr_quotient_cfg));
    CPR_ASSERT(modes->quotients);

    cpr_quotient_cfg* quotients = modes->quotients;

    quotients->count = s_quotients->count;
    cpr_smem_read_quots(quotients);
}

static boolean cpr_smem_write_freq_data(cpr_rail_state* state,
        struct cpr_voltage_data *modes, int modeIdx)
{
    int i = 0;

    if (!(modes->subModesCount))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s Mode %d freq data @ (offset: 0x%x)",
            rail->name, modeIdx, *currOffset);

    for (i = 0; i < modes->subModesCount; i++) {
        cpr_smem_freq_data* s_subModes =
            (cpr_smem_freq_data*) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_freq_data));

        struct cpr_freq_data* subModes = &(modes->subModes[i]);

        s_subModes->interplEnabled = subModes->interplEnabled;
        s_subModes->freq = subModes->freq;
        s_subModes->ceiling = subModes->ceiling;
        s_subModes->floor = subModes->floor;
    }
    return true;
}

static void cpr_smem_read_freq_data(struct cpr_voltage_data *modes)
{
    int i = 0;
    CPR_ASSERT(modes->subModesCount);

    modes->subModes = cpr_image_malloc(sizeof(struct cpr_freq_data) *
            modes->subModesCount);
    CPR_ASSERT(modes->subModes);

    for (i = 0; i < modes->subModesCount; i++) {
        cpr_smem_freq_data* s_subModes = (cpr_smem_freq_data*) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_freq_data));

        struct cpr_freq_data* subModes = &(modes->subModes[i]);

        subModes->interplEnabled  = s_subModes->interplEnabled;
        subModes->freq            = s_subModes->freq;
        subModes->ceiling         = s_subModes->ceiling;
        subModes->floor           = s_subModes->floor;
    }
}


static boolean cpr_smem_write_voltage_data(cpr_rail_state* state,
        cpr_voltage_plan *vp)
{
    int i = 0;

    if (!(vp->modesCount))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s modes[%d] @ (offset: 0x%x)",
            rail->name, vp->modesCount, *currOffset);

    for (i = 0; i < vp->modesCount; i++) {
        int j = 0;

        cpr_smem_voltage_data* s_modes =
            (cpr_smem_voltage_data *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_voltage_data));

        struct cpr_voltage_data *modes = &(vp->modes[i]);

        s_modes->fref = modes->fref;
        s_modes->margins = cpr_smem_write_margin_cfg(state, modes, i);
        s_modes->fuses = cpr_smem_write_fuse_data(state, modes, i);
        for (j = 0; j < 4; j++)
            s_modes->marginTempBands[j] = modes->marginTempBands[j];
        s_modes->freqDelta = modes->freqDelta;
        s_modes->quotients = cpr_smem_write_quotient_cfg(state, modes, i);
        s_modes->subModesCount = modes->subModesCount;
        s_modes->subModes = cpr_smem_write_freq_data(state, modes, i);
    }

    return true;
}

static void cpr_smem_read_voltage_data(cpr_voltage_plan *vp)
{
    int i = 0;
    int j = 0;

    CPR_ASSERT(vp->modesCount);

    vp->modes = cpr_image_malloc(sizeof(struct cpr_voltage_data) *
            vp->modesCount);

    for (i = 0; i < vp->modesCount; i++) {
        cpr_smem_voltage_data* s_modes =
            (cpr_smem_voltage_data *)GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_voltage_data));

        struct cpr_voltage_data *modes = &(vp->modes[i]);

        modes->fref = s_modes->fref;
        if (s_modes->margins)
            cpr_smem_read_margin_cfg(modes);
        if (s_modes->fuses)
            cpr_smem_read_fuse_data(modes);
        for (j = 0; j < 4; j++)
            modes->marginTempBands[j] = s_modes->marginTempBands[j];
        modes->freqDelta = s_modes->freqDelta;
        if (s_modes->quotients)
            cpr_smem_read_quotient_cfg(modes);
        modes->subModesCount = s_modes->subModesCount;
        if (s_modes->subModes)
            cpr_smem_read_freq_data(modes);
    }
}

static boolean cpr_smem_write_voltage_plan(cpr_rail_state* state,
        struct voltage_plan_list *vpl)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s voltage plan list @ (offset: 0x%x)",
            rail->name, *currOffset);

    if (!(vpl->cfg))
        return false;

    cpr_smem_voltage_plan *s_vp = (cpr_smem_voltage_plan *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_voltage_plan));

    cpr_voltage_plan *vp = vpl->cfg;

    s_vp->modesCount = vp->modesCount;
    s_vp->supportedModes = cpr_smem_write_supported_voltage_mode(state, vp);
    s_vp->tempAdjCfg = cpr_smem_write_temp_adj_cfg(state, vp);
    s_vp->kvCfg = cpr_smem_write_kv_cfg(state, vp);
    s_vp->modes = cpr_smem_write_voltage_data(state, vp);

    return true;
}

static void cpr_smem_read_voltage_plan(struct voltage_plan_list *vpl)
{
    vpl->cfg = cpr_image_malloc(sizeof(cpr_voltage_plan));
    CPR_ASSERT(vpl->cfg);

    cpr_voltage_plan *vp = vpl->cfg;

    cpr_smem_voltage_plan *s_vp = (cpr_smem_voltage_plan *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_voltage_plan));

    vp->modesCount = s_vp->modesCount;

    cpr_smem_read_supported_voltage_mode(vp);

    if (s_vp->tempAdjCfg)
        cpr_smem_read_temp_adj_cfg(vp);

    if (s_vp->kvCfg)
        cpr_smem_read_kv_cfg(vp);

    if (s_vp->modes)
        cpr_smem_read_voltage_data(vp);
}

static boolean cpr_smem_write_voltage_plan_list(cpr_rail_state* state,
        const cpr_versioned_voltage_plan* vvp)
{
    int i = 0;

    if (!(vvp->count))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s voltage plan list @ (offset: 0x%x)",
            rail->name, *currOffset);

    for (i = 0; i < vvp->count; i++) {
        cpr_smem_voltage_plan_list * s_vpl =
            (cpr_smem_voltage_plan_list *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_voltage_plan_list));

        struct voltage_plan_list* vpl = &(vvp->list[i]);
    
        s_vpl->version = cpr_smem_write_version(state, vpl->version);
        s_vpl->cfg = cpr_smem_write_voltage_plan(state, vpl);
    }
    return true;
}

static void cpr_smem_read_voltage_plan_list(cpr_versioned_voltage_plan *vvp)
{
    int i = 0;

    CPR_ASSERT(vvp->count);
    vvp->list =
        cpr_image_malloc(sizeof(struct voltage_plan_list) * vvp->count);
    CPR_ASSERT(vvp->list);

    for (i = 0; i < vvp->count; i++) {
        struct voltage_plan_list * vpl = &(vvp->list[i]);

        INCREMENT_OFFSET(sizeof(cpr_smem_voltage_plan_list));

        vpl->version = cpr_image_malloc(sizeof(cpr_version));
        CPR_ASSERT(vpl->version);

        cpr_smem_read_version(vpl->version);
        cpr_smem_read_voltage_plan(vpl);
    }
}

static boolean cpr_smem_write_sensors(cpr_rail_state* state, uint16 count,
        uint8 *sensors, char *name)
{
    int i = 0;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s %s Sensors[%d] @ (offset: 0x%x)",
            rail->name, name, count, *currOffset);

    if (!count)
        return false;

    for (i = 0; i < count; i++) {
        uint8 *s_sensors = (uint8 *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(uint8));

        *s_sensors = sensors[i];
    }
    return true;
}

static void cpr_smem_read_sensors(uint16 count, uint8 *sensors)
{
    int i = 0;
    CPR_ASSERT(sensors);

    uint8 *s_sensors = (uint8 *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(uint8) * count);

    for (i = 0; i < count; i++)
         sensors[i] = s_sensors[i];
}

static boolean cpr_smem_write_aging_cfg(cpr_rail_state* state,
        const cpr_versioned_voltage_plan* vvp)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s aging config @ (offset: 0x%x)",
            rail->name, *currOffset);

    if (!(vvp->agingCfg))
        return false;

    cpr_smem_aging_cfg* s_agingCfg = (cpr_smem_aging_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_aging_cfg));

    cpr_aging_cfg* agingCfg = vvp->agingCfg;

    s_agingCfg->cprRevMinToRun = agingCfg->cprRevMinToRun;
    s_agingCfg->modeToRun = (uint32)agingCfg->modeToRun; 
    s_agingCfg->sensorID = agingCfg->sensorID;
    s_agingCfg->bypassSensorIDsCount = agingCfg->bypassSensorIDsCount;
    s_agingCfg->fusePackingFactor = agingCfg->fusePackingFactor;
    s_agingCfg->kv = agingCfg->kv;
    s_agingCfg->scalingFactor = agingCfg->scalingFactor;
    s_agingCfg->marginLimit = agingCfg->marginLimit;
    s_agingCfg->bypassSensorIDs = cpr_smem_write_sensors(state,
            agingCfg->bypassSensorIDsCount, agingCfg->bypassSensorIDs,
            "aging");
    s_agingCfg->fuse.count = agingCfg->fuse.count;
    s_agingCfg->fuse.data = cpr_smem_write_raw_fuse_data(state,
            &(agingCfg->fuse), "aging");

    return true;
}

static void cpr_smem_read_aging_cfg(cpr_versioned_voltage_plan* vvp)
{
    cpr_smem_aging_cfg* s_agingCfg = (cpr_smem_aging_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_aging_cfg));

    vvp->agingCfg = cpr_image_malloc(sizeof(cpr_aging_cfg));
    CPR_ASSERT(vvp->agingCfg);

    cpr_aging_cfg* agingCfg = vvp->agingCfg;

    agingCfg->cprRevMinToRun = s_agingCfg->cprRevMinToRun;
    agingCfg->modeToRun = (cpr_voltage_mode)s_agingCfg->modeToRun;
    agingCfg->sensorID = s_agingCfg->sensorID;
    agingCfg->bypassSensorIDsCount = s_agingCfg->bypassSensorIDsCount;
    agingCfg->fusePackingFactor = s_agingCfg->fusePackingFactor;
    agingCfg->kv = s_agingCfg->kv;
    agingCfg->scalingFactor = s_agingCfg->scalingFactor;
    agingCfg->marginLimit = s_agingCfg->marginLimit;
    if (agingCfg->bypassSensorIDsCount) {
        agingCfg->bypassSensorIDs = cpr_image_malloc(sizeof(uint8) *
                agingCfg->bypassSensorIDsCount);
        cpr_smem_read_sensors(agingCfg->bypassSensorIDsCount,
                agingCfg->bypassSensorIDs);
    }
    agingCfg->fuse.count = s_agingCfg->fuse.count;
    if (agingCfg->fuse.count)
        cpr_smem_read_raw_fuse_data(&(agingCfg->fuse));
}

static boolean cpr_smem_write_vvps(cpr_rail_state* state,
        uint8 railsCount)
{
    if (!railsCount)
        return false;

    for (int i = 0; i < railsCount; i++) {
        cpr_rail *rail = cpr_utils_get_rail(state->id);
        CPR_ASSERT(rail);
        CPR_LOG_INFO(" Exporting %s versioned voltage plan @ (offset: 0x%x)",
                rail->name, *currOffset);

        cpr_smem_versioned_voltage_plan* s_vvp =
            (cpr_smem_versioned_voltage_plan*) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_versioned_voltage_plan));

        const cpr_versioned_voltage_plan* vvp =
            cpr_utils_get_versioned_voltage_plan(state->id);

        s_vvp->rail = (uint32)vvp->rail;
        s_vvp->count = vvp->count;
        s_vvp->list = cpr_smem_write_voltage_plan_list(state, vvp);
        s_vvp->agingCfg = cpr_smem_write_aging_cfg(state, vvp);
    }
    return true;
}

static void cpr_smem_read_vvps(cpr_settings *settings)
{
    int i = 0;

    CPR_ASSERT(settings->railsCount);
    settings->railVVPs = (cpr_versioned_voltage_plan **) \
        cpr_image_malloc(sizeof(cpr_versioned_voltage_plan *) * \
                settings->railsCount);

    CPR_ASSERT(settings->railVVPs);

    for (i = 0; i < settings->railsCount; i++) {
        settings->railVVPs[i] = (cpr_versioned_voltage_plan *) \
            cpr_image_malloc(sizeof(cpr_versioned_voltage_plan));

        cpr_versioned_voltage_plan* vvp = settings->railVVPs[i];

        cpr_smem_versioned_voltage_plan* s_vvp =
            (cpr_smem_versioned_voltage_plan*) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_versioned_voltage_plan));

        vvp->rail = (cpr_domain_id)s_vvp->rail;
        vvp->count = s_vvp->count;

        CPR_ASSERT(s_vvp->list);
        cpr_smem_read_voltage_plan_list(vvp);

        if (s_vvp->agingCfg)
            cpr_smem_read_aging_cfg(vvp);
    }
}

static boolean cpr_smem_write_string(cpr_rail_state* state, char *src)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s  string:%s info @ (offset: 0x%x)",
            rail->name, src, *currOffset);

    if (!src)
        return false;

    char *dest = (char *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(char) * 50);

    cpr_image_memscpy(dest, src, strlen(src));

    return true;
}

static void cpr_smem_read_string(char **dest)
{
    *dest = cpr_image_malloc(sizeof(char) * 50);
    CPR_ASSERT(*dest);

    char *src = (char *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(char) * 50);

    cpr_image_memscpy(*dest, src, (sizeof(char) * 50));
}

static boolean cpr_smem_write_hal_controller_cfg(cpr_rail_state* state,
        cpr_hal_controller *controllers)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s CPR Settings @ (offset: 0x%x)",
            rail->name, *currOffset);

    cpr_smem_hal_controller_cfg *s_cfg = (cpr_smem_hal_controller_cfg *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_hal_controller_cfg));

    cpr_hal_controller_cfg *cfg = controllers->cfg;

    s_cfg->sensorsTotal = cfg->sensorsTotal;
    s_cfg->bypassSensorsCount = cfg->bypassSensorsCount;
    s_cfg->disableSensorsCount = cfg->disableSensorsCount;
    s_cfg->stepQuotMax = cfg->stepQuotMax;
    s_cfg->stepQuotMin = cfg->stepQuotMin;
    s_cfg->bypassSensors = cpr_smem_write_sensors(state,
            cfg->bypassSensorsCount, cfg->bypassSensors, "bypass");
    s_cfg->disableSensors = cpr_smem_write_sensors(state,
            cfg->disableSensorsCount, cfg->disableSensors, "disable");
    s_cfg->refClk = cpr_smem_write_string(state, cfg->refClk);
    s_cfg->ahbClk = cpr_smem_write_string(state, cfg->ahbClk);

    return true;
}

static void cpr_smem_read_hal_controller_cfg(cpr_hal_controller *controllers)
{
    cpr_smem_hal_controller_cfg *s_cfg = (cpr_smem_hal_controller_cfg *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_hal_controller_cfg));

    controllers->cfg = cpr_image_malloc(sizeof(cpr_hal_controller_cfg));
    CPR_ASSERT(controllers->cfg);

    cpr_hal_controller_cfg *cfg = controllers->cfg;

    cfg->sensorsTotal = s_cfg->sensorsTotal;
    cfg->bypassSensorsCount = s_cfg->bypassSensorsCount;
    cfg->disableSensorsCount = s_cfg->disableSensorsCount;
    cfg->stepQuotMax = s_cfg->stepQuotMax;
    cfg->stepQuotMin = s_cfg->stepQuotMin;
    if (cfg->bypassSensorsCount) {
        cfg->bypassSensors = cpr_image_malloc(sizeof(uint8) *
                cfg->bypassSensorsCount);
        cpr_smem_read_sensors(cfg->bypassSensorsCount, cfg->bypassSensors);
    }
    if (cfg->disableSensorsCount) {
        cfg->disableSensors = cpr_image_malloc(sizeof(uint8) *
                cfg->disableSensorsCount);
        cpr_smem_read_sensors(cfg->disableSensorsCount,
                cfg->disableSensors);
    }
    if (s_cfg->refClk)
        cpr_smem_read_string(&(cfg->refClk));
    if (s_cfg->ahbClk)
        cpr_smem_read_string(&(cfg->ahbClk));
}

static boolean cpr_smem_write_hal_thread_cfg(cpr_rail_state* state,
        cpr_hal_thread *thread)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s hal thread cfg @ (offset: 0x%x)",
            rail->name, *currOffset);

    cpr_smem_hal_thread_cfg *s_cfg = (cpr_smem_hal_thread_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_hal_thread_cfg));

    cpr_hal_thread_cfg *cfg = thread->cfg;

    s_cfg->upThresh = cfg->upThresh;
    s_cfg->dnThresh = cfg->dnThresh;
    s_cfg->consecUp = cfg->consecUp;
    s_cfg->consecDn = cfg->consecDn;
    s_cfg->sensorsCount = cfg->sensorsCount;
    s_cfg->sensors = cpr_smem_write_sensors(state,
                cfg->sensorsCount, cfg->sensors, "thread sensors");

    return true;
}

static void cpr_smem_read_hal_thread_cfg(cpr_hal_thread *thread)
{
    cpr_smem_hal_thread_cfg *s_cfg = (cpr_smem_hal_thread_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_hal_thread_cfg));

    thread->cfg = cpr_image_malloc(sizeof(cpr_hal_thread_cfg));
    CPR_ASSERT(thread->cfg);

    cpr_hal_thread_cfg *cfg = thread->cfg;

    cfg->upThresh = s_cfg->upThresh;
    cfg->dnThresh = s_cfg->dnThresh;
    cfg->consecUp = s_cfg->consecUp;
    cfg->consecDn = s_cfg->consecDn;
    cfg->sensorsCount = s_cfg->sensorsCount;
    if (cfg->sensorsCount) {
        cfg->sensors = cpr_image_malloc(sizeof(uint8) * cfg->sensorsCount);
        cpr_smem_read_sensors(cfg->sensorsCount, cfg->sensors);
    }
}

static boolean cpr_smem_write_hal_thread(cpr_rail_state* state,
        cpr_hal_controller *controllers)
{
    int i = 0;

    if (!(controllers->threadsCount))
        return false;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s hal thread @ (offset: 0x%x)",
            rail->name, *currOffset);

    for (i = 0; i < controllers->threadsCount; i++) {
        cpr_smem_hal_thread *s_threads = (cpr_smem_hal_thread *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_hal_thread));

        cpr_hal_thread *threads = controllers->threads[i];

        s_threads->id = (uint32)threads->id;
        s_threads->init = (uint32)threads->init;
        s_threads->enabled = threads->enabled;
        s_threads->roMask4Thread = threads->roMask4Thread;
        s_threads->cfg = cpr_smem_write_hal_thread_cfg(state, threads);
    }
    return true;
}

static void cpr_smem_read_hal_thread(cpr_hal_controller *controllers)
{
    int i = 0;
    CPR_ASSERT(controllers->threadsCount);
    controllers->threads = (cpr_hal_thread **) \
        cpr_image_malloc(sizeof(cpr_hal_thread *) * \
            controllers->threadsCount);
    CPR_ASSERT(controllers->threads);

    for (i = 0; i < controllers->threadsCount; i++) {
        cpr_smem_hal_thread *s_threads = (cpr_smem_hal_thread *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_hal_thread));

        controllers->threads[i] = (cpr_hal_thread *) \
            cpr_image_malloc(sizeof(cpr_hal_thread)); 
        cpr_hal_thread *threads = controllers->threads[i];

        threads->id = s_threads->id;
        threads->init = s_threads->init;
        threads->enabled = s_threads->enabled;
        threads->roMask4Thread = s_threads->roMask4Thread;
        cpr_smem_read_hal_thread_cfg(threads);
    }
}

static boolean cpr_smem_write_controllers(cpr_rail_state* state,
        uint8 controllersCount)
{
    int i = 0;

    if (!controllersCount)
        return false;

    for (i=0; i < controllersCount; i++) {
        cpr_rail *rail = cpr_utils_get_rail(state->id);
        CPR_ASSERT(rail);
        CPR_LOG_INFO(" Exporting %s CPR Controllers @ (offset: 0x%x)",
                rail->name, *currOffset);

        cpr_smem_hal_controller *s_controllers = (cpr_smem_hal_controller *)GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_hal_controller));

        cpr_hal_controller *controllers = cpr_utils_get_hal_controller(state->id);

        s_controllers->base = (uint32)controllers->base;
        s_controllers->type = (uint32)controllers->type;
        s_controllers->threadsCount = controllers->threadsCount;
        s_controllers->tempSensorData.hotTempCx10 =
            controllers->tempSensorData.hotTempCx10;
        s_controllers->tempSensorData.hotSensor =
            controllers->tempSensorData.hotSensor;
        s_controllers->tempSensorData.coldTempCx10 =
            controllers->tempSensorData.coldTempCx10;
        s_controllers->tempSensorData.coldSensor =
            controllers->tempSensorData.coldSensor;
        s_controllers->cfg = cpr_smem_write_hal_controller_cfg(state,
                controllers);
        s_controllers->threads = cpr_smem_write_hal_thread(state,
                controllers);
    }
    return true;
}

static void cpr_smem_read_controllers(cpr_settings *settings)
{
    int i = 0;

    CPR_ASSERT(settings->controllersCount);
    settings->controllers = (cpr_hal_controller **) \
        cpr_image_malloc(sizeof(cpr_hal_controller *) * \
                settings->controllersCount);
    CPR_ASSERT(settings->controllers);

    for (i = 0; i < settings->controllersCount; i++) {
        settings->controllers[i] = (cpr_hal_controller *) \
            cpr_image_malloc(sizeof(cpr_hal_controller));
        cpr_hal_controller *controllers = settings->controllers[i];

        cpr_smem_hal_controller *s_controllers =
            (cpr_smem_hal_controller *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_hal_controller));

        controllers->base = (uintptr_t)s_controllers->base;
        controllers->type = (cpr_controller_type)s_controllers->type;
        controllers->threadsCount = s_controllers->threadsCount;
        controllers->tempSensorData.hotTempCx10 =
            s_controllers->tempSensorData.hotTempCx10;
        controllers->tempSensorData.hotSensor =
            s_controllers->tempSensorData.hotSensor;
        controllers->tempSensorData.coldTempCx10 =
            s_controllers->tempSensorData.coldTempCx10;
        controllers->tempSensorData.coldSensor =
            s_controllers->tempSensorData.coldSensor;
        if (s_controllers->cfg)
            cpr_smem_read_hal_controller_cfg(controllers);
        if (s_controllers->threads)
            cpr_smem_read_hal_thread(controllers);
    }
}

static boolean cpr_smem_write_settled_voltage_mode(cpr_rail_state* state,
        cpr_rail *rail)
{
    CPR_LOG_INFO(" Exporting %s Settled Modes[%d] starting @ offset: 0x%x)",
            rail->name, rail->settleModesCount, *currOffset);

    return __cpr_smem_write_voltage_mode(state, rail->settleModes,
            rail->settleModesCount);
}

static void cpr_smem_read_settled_voltage_mode(cpr_rail *rail)
{
    CPR_ASSERT(!(rail->settleModesCount));
    
    rail->settleModes = cpr_image_malloc(sizeof(cpr_voltage_mode) *
            rail->settleModesCount);
    CPR_ASSERT(rail->settleModes);

    __cpr_smem_read_voltage_mode(rail->settleModes, rail->settleModesCount);
}

static boolean cpr_smem_write_enablements(cpr_rail_state* state,
        uint8 enablementsCount)
{
    int i = 0;
    int j = 0;

    if (!enablementsCount)
        return false;

    for (i = 0; i < enablementsCount; i++) {
        cpr_rail *rail = cpr_utils_get_rail(state->id);
        CPR_LOG_INFO(" Exporting %s enablement @ offset: 0x%x)",
                rail->name, rail->settleModesCount, *currOffset);

        cpr_smem_enablement* s_enablement = (cpr_smem_enablement *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_enablement));

        cpr_enablement* enablement = rail->enablement;

        s_enablement->id = (uint32)enablement->id;
        s_enablement->funcId = (uint32)enablement->funcId;
        s_enablement->version = cpr_smem_write_version(state,
                enablement->version);

        s_enablement->railCPRRevFuse.count = enablement->railCPRRevFuse.count;
        s_enablement->railCPRRevFuse.data = cpr_smem_write_raw_fuse_data(state,
                &enablement->railCPRRevFuse, "railCPRRevFuse");
        s_enablement->stepSize = enablement->stepSize;
        s_enablement->fuseMultiplier = enablement->fuseMultiplier;
        for (j = 0; j < CPR_THERMAL_REGION_MAX; j++)
            s_enablement->thermalAdjustment[j] = enablement->thermalAdjustment[j];
    }
    return true;
}

static void cpr_smem_read_enablements(cpr_settings *settings)
{
    int i = 0;
    settings->enablements = (cpr_enablement **) \
        cpr_image_malloc(sizeof(cpr_enablement *) * \
                settings->enablementsCount);

    for (i = 0; i < settings->enablementsCount; i++) {
        cpr_smem_enablement* s_enablement =
            (cpr_smem_enablement *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_enablement));

        settings->enablements[i] =
            (cpr_enablement *) cpr_image_malloc(sizeof(cpr_enablement));
        cpr_enablement* enablement = settings->enablements[i]; 

        enablement->id = (cpr_domain_id)s_enablement->id;
        enablement->funcId = (cpr_init_funcs_id)s_enablement->funcId;
        
        enablement->version = cpr_image_malloc(sizeof(cpr_version));
        CPR_ASSERT(enablement->version);
        cpr_smem_read_version(enablement->version);

        enablement->railCPRRevFuse.count = s_enablement->railCPRRevFuse.count;
        cpr_smem_read_raw_fuse_data(&(enablement->railCPRRevFuse));

        enablement->stepSize = s_enablement->stepSize;
        enablement->fuseMultiplier = s_enablement->fuseMultiplier;

        for (i = 0; i < CPR_THERMAL_REGION_MAX; i++)
            enablement->thermalAdjustment[i] = s_enablement->thermalAdjustment[i];
    }
}

static boolean cpr_smem_write_rails(cpr_rail_state* state, uint8 railsCount)
{
    int i = 0;

    if (!railsCount)
        return false;

    for(i = 0; i < railsCount; i++) {
        cpr_rail *rail = cpr_utils_get_rail(state->id);
        CPR_LOG_INFO(" Exporting %s Rail Settings @ (offset: 0x%x)",
                rail->name, *currOffset);

        cpr_smem_rail* s_rail = (cpr_smem_rail *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_rail));

        s_rail->id = (uint32)rail->id;
        s_rail->railIdx = rail->railIdx;
        s_rail->settleModesCount = rail->settleModesCount;
        s_rail->name = cpr_smem_write_string(state, rail->name);
        s_rail->interruptId = rail->interruptId;
        s_rail->settleModes = cpr_smem_write_settled_voltage_mode(state, rail); 
    }
    return true;
}

static void cpr_smem_read_rails(cpr_settings *settings)
{
    int i = 0;
    settings->rails = (cpr_rail **) cpr_image_malloc(
            sizeof(cpr_rail *) * settings->railsCount);

    for (i = 0; i < settings->railsCount; i++) {
        settings->rails[i] = (cpr_rail *) cpr_image_malloc(sizeof(cpr_rail));
        cpr_rail *rail = settings->rails[i];

        cpr_smem_rail* s_rail = (cpr_smem_rail *) GET_BUFFER();
        INCREMENT_OFFSET(sizeof(cpr_smem_rail));

        rail->id = s_rail->id;
        rail->railIdx = s_rail->railIdx;
        rail->settleModesCount = s_rail->settleModesCount;
        if (s_rail->name)
            cpr_smem_read_string(&(rail->name));
        rail->hal.controller = settings->controllers[0];
        rail->hal.thread = settings->controllers[0]->threads[0];
        rail->interruptId = s_rail->interruptId;
        if (s_rail->settleModes)
            cpr_smem_read_settled_voltage_mode(rail); 
    }
}

static boolean cpr_smem_write_miscCfg(cpr_rail_state* state)
{
    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s misc cfg @ (offset: 0x%x)",
            rail->name, *currOffset);

    cpr_smem_misc_cfg* s_miscCfg = (cpr_smem_misc_cfg *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_misc_cfg));

    cpr_misc_cfg* miscCfg = cpr_utils_get_misc_cfg();

    s_miscCfg->cprRev.count = miscCfg->cprRev.count;
    s_miscCfg->cprRev.data = cpr_smem_write_raw_fuse_data(state,
            &(miscCfg->cprRev), "CPR Rev");

    return true;
}

static void cpr_smem_read_miscCfg(cpr_settings *settings)
{
    cpr_smem_misc_cfg* s_miscCfg = (cpr_smem_misc_cfg *) GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_misc_cfg));

    settings->miscCfg = (cpr_misc_cfg *)
        cpr_image_malloc(sizeof(cpr_misc_cfg));
    cpr_misc_cfg* miscCfg = settings->miscCfg;

    miscCfg->cprRev.count = s_miscCfg->cprRev.count;
    cpr_smem_read_raw_fuse_data(&(miscCfg->cprRev));
}

void cpr_smem_write_rail_settings(cpr_smem_hdr *header,
        cpr_smem_rail_hdr* railHdr, uint32 *offset, cpr_rail_state *state)
{
    //ToDo: Have a seperate func to determine the number of
    //      rails, controllers, enablements.
    //      Will need this for MSS which will have 2 VVP's,
    //      enablements etc. One for MSS and one for MSS_ELDO.
    uint8 cpr_num_rails = 1;
    uint8 cpr_num_controllers = 1;
    uint8 cpr_num_enablements = 1;

    currOffset = offset;
    hdr = header;

    cpr_rail *rail = cpr_utils_get_rail(state->id);
    CPR_ASSERT(rail);
    CPR_LOG_INFO(" Exporting %s CPR Settings @ (offset: 0x%x)",
            rail->name, *currOffset);

    cpr_smem_settings *s_settings = (cpr_smem_settings *)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_settings));

    s_settings->railsCount = cpr_num_rails;;
    s_settings->controllersCount = cpr_num_controllers;
    s_settings->enablementsCount = cpr_num_enablements;

    s_settings->railVVPs = cpr_smem_write_vvps(state,
            s_settings->railsCount);
    s_settings->controllers = cpr_smem_write_controllers(state,
            s_settings->controllersCount);
    s_settings->rails = cpr_smem_write_rails(state,
            s_settings->railsCount);
    s_settings->enablements = cpr_smem_write_enablements(state,
            s_settings->enablementsCount);
    s_settings->miscCfg = cpr_smem_write_miscCfg(state);

    railHdr->size = (uint16)((uint8*)hdr + *currOffset - (uint8*)railHdr);
    hdr->dataSize = *currOffset;
}

void cpr_smem_read_rail_settings(cpr_smem_hdr *header,
        cpr_smem_rail_hdr* railHdr, uint32 *offset, cpr_settings *settings)
{
    currOffset = offset;
    hdr = header;
    
    CPR_LOG_INFO("Deserialize rail: %d @offset 0x%x", railHdr->id, currOffset);

    cpr_smem_settings* s_settings = (cpr_smem_settings*)GET_BUFFER();
    INCREMENT_OFFSET(sizeof(cpr_smem_settings));

    settings->railsCount = s_settings->railsCount;
    settings->controllersCount = s_settings->controllersCount;
    settings->enablementsCount = s_settings->enablementsCount;

    if (s_settings->railVVPs)
        cpr_smem_read_vvps(settings);
    if (s_settings->controllers)
        cpr_smem_read_controllers(settings);
    if (s_settings->rails)
        cpr_smem_read_rails(settings);
    if (s_settings->enablements)
        cpr_smem_read_enablements(settings);
    if (s_settings->miscCfg)
        cpr_smem_read_miscCfg(settings);
}
