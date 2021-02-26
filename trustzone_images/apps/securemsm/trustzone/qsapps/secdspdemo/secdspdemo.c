/*
@file secdspdemo.c
*/
/*===========================================================================
  Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.0.2/securemsm/trustzone/qsapps/secdspdemo/secdspdemo.c#1 $
  $DateTime: 2018/08/09 06:10:01 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "dsc_api.h"
//#include "ipprotector_api.h"
#include "secdspdemo.h"
#include "utils.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

static uint8_t qsee_log_mask;
static dsc_feat_handle_t *g_dsc_handle = NULL;

static buffer_vm_list_t g_buf_vm_list [] = {
    { MODEL, { AC_VM_CP_CDSP }, 1 },
    { AUTH, { AC_VM_CP_CDSP }, 1 },
    { CAMERA, { AC_VM_CP_CDSP, AC_VM_CP_CAMERA }, 2 },
};

static uint32_t nr_buf_vm_list = NUM_ELEMS(g_buf_vm_list);

static int32_t __dsc_mem_ops(uint32_t cmd_id,
        uint64_t addr, size_t size, uint32_t buf_type)
{
    int32_t ret = SUCCESS;
    int32_t i = 0;
    ACVirtualMachineId *vm_list = NULL;
    uint32_t nr_vmids = 0;

    if (cmd_id == REGISTER_BUFFER) {

        for (i = 0; i < nr_buf_vm_list; i++) {
            if (buf_type == g_buf_vm_list[i].buf_type) {
                vm_list = g_buf_vm_list[i].vm_list;
                nr_vmids =  g_buf_vm_list[i].nr_vmids;
                break;
            }
        }

        CHECK_COND(vm_list == NULL,
                ret, FAILED, "Cannot find VM List for buf_type : %d", buf_type);

        CHECK_N_CALL(ret, dsc_register_buffer(
                    g_dsc_handle, buf_type, addr, size, vm_list, nr_vmids));

    } else if(cmd_id == DEREGISTER_BUFFER) {
        CHECK_N_CALL(ret, dsc_deregister_buffer(g_dsc_handle, addr, size));
    } else {
        ret = FAILED;
    }

end:
    return ret;
}

//******************************************
// TZ APP INITIALIZATION CODE
//******************************************
void tz_app_init(void)
{
    /*  App specific initialization code*/
    qsee_log_mask = qsee_log_get_mask();
    qsee_log_set_mask(qsee_log_mask | QSEE_LOG_MSG_DEBUG);
    LOG_MSG("Init");
    LOG_MSG("Version: %x.%x", VERSION_MAJOR, VERSION_MINOR);

    // Feature specific code
    (void)dsc_get_feat_handle(DSP_FACE_3D, &g_dsc_handle);
}

//******************************************
// TZ APP MAIN ENTRY POINT
//******************************************
void tz_app_cmd_handler(void* cmd, uint32_t cmdlen,
        void* rsp, uint32_t rsplen)
{
    /* Request-response buffers are allocated by non-secure side*/
    send_cmd_t *cmd_in = NULL;
    send_cmd_rsp_t *rsp_ptr = NULL;
    int32_t ret = SUCCESS;
    uint32_t state;

    CHECK_COND(!cmd || !rsp,
            ret, FAILED, "Invalid cmd/rsp parameters");
    CHECK_COND(rsplen < sizeof(send_cmd_rsp_t),
            ret, FAILED, "Response buffer len insufficient");
    CHECK_COND(cmdlen < sizeof(send_cmd_t),
            ret, FAILED, "Command buffer len insufficient");

    rsp_ptr = (send_cmd_rsp_t *)rsp;
    cmd_in = (send_cmd_t *)cmd;

    CHECK_COND(g_dsc_handle == NULL,
            ret, FAILED, "DSC Handle initialization failed");

    LOG_MSG("Input CommandID = %d", cmd_in->cmd_id);
    CHECK_COND(cmd_in->cmd_id > LAST_CMD, ret, FAILED, "Unsupported cmd");

    LOG_MSG("in = 0x%x, out = 0x%x, in_size = %d, out_size = %d",
            cmd_in->addr, cmd_in->out_addr, cmd_in->size, cmd_in->out_size);

    if (cmd_in->cmd_id > DECRYPT_IP) {
        CHECK_N_CALL(ret, __dsc_mem_ops(cmd_in->cmd_id,
                    cmd_in->addr, cmd_in->size, cmd_in->buf_type));
        goto end;
    }

    // DECRYPT_IP cmd
    CHECK_N_CALL(ret, dsc_get_state(g_dsc_handle, &state));
    CHECK_COND(state != RESET,
                ret, FAILED, "Feature in wrong state : %d", state);

    CHECK_N_CALL(ret, dsc_set_state(g_dsc_handle, INIT));

    // Verify out buffer
    CHECK_N_CALL(ret, dsc_verify_buffer(g_dsc_handle,
                cmd_in->out_addr, cmd_in->out_size));

  //  CHECK_N_CALL(ret, ipprotector_decrypt(
  //              cmd_in->addr, cmd_in->size, cmd_in->out_addr, cmd_in->out_size));

    state = LOADED;
    CHECK_N_CALL(ret, dsc_set_state(g_dsc_handle, state));

    // Other operations as required


// Cleanup:
end:
    LOG_MSG("Exiting Secure DSP Demo App!");

    if (rsp_ptr) {
        rsp_ptr->status = ret;
    }
}

//******************************************
// TZ APP SHUTDOWN CODE
//******************************************
void tz_app_shutdown(void)
{
    dsc_release_feat_handle(g_dsc_handle);
    g_dsc_handle = NULL;

    LOG_MSG("shutdown");
    qsee_log_set_mask(qsee_log_mask);
    return;
}
