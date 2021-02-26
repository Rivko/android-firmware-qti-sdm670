//This file will be moved once ACP team fully removed HW Resampler driver V1 APIs

#include "rs_driver.h"
#include "adsp_error_codes.h"

ADSPResult rs_init(void)
{
   return ADSP_EOK;
}

ADSPResult rs_deinit(void)
{
   return ADSP_EOK;
}

int16 rs_init_job(uint16 px, struct_rs_job_config* job_struct, rs_job_event_callback_func cb_func, void* user_ctx)
{
   return -1;
}

ADSPResult rs_send_job(uint16 px, int16 job_id, uint16 in_samples, uint16 out_samples, int32** in_buf_ptr, int32** out_buf_ptr)
{
   return ADSP_EOK;     
}

rs_job_status_type rs_confirm_job(uint16 px, int16 job_id, uint32* samples_processed)
{
   return ADSP_EOK;
}

ADSPResult rs_reset_job(uint16 px, int16 job_id, struct_rs_job_config* job_struct)
{
   return ADSP_EOK;     
}

void rs_uninit_job(uint16 px, int16 job_id)
{
}


ADSPResult rs_get_stream_config(int16 job_id, struct_rs_dnsampler *ptr_rs_dnsamp)
{
   return ADSP_EOK;
}

ADSPResult rs_set_stream_data(int16 job_id, struct_rs_stream_data *ptr_rs_str_data, uint16 flag)
{
   return ADSP_EOK;
}

ADSPResult rs_estimate_core_clks(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info)    
{
   return ADSP_EOK;
}






