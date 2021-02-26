

#include "rs_driver_ext.h"
#include "adsp_error_codes.h"

ADSPResult rs_drv_init(void)
{
   return ADSP_EOK;
}


ADSPResult rs_drv_deinit(void)
{
   return ADSP_EOK;
}


ADSPResult rs_drv_job_open(rs_drv_handle_t *rs_drv_h, rs_drv_job_cfg_t* job_cfg_ptr)
{
   return ADSP_EOK;
}


ADSPResult rs_drv_job_process(rs_drv_handle_t rs_drv_h, rs_drv_buflist_t* in_buflist_ptr, rs_drv_buflist_t* out_buflist_ptr)
{
   return ADSP_EOK;
}


ADSPResult rs_drv_job_process_done(rs_drv_handle_t rs_drv_h, rs_drv_buflist_t* in_buflist_ptr, rs_drv_buflist_t* out_buflist_ptr)
{
   return ADSP_EOK;
}


ADSPResult rs_drv_job_reset(rs_drv_handle_t rs_drv_h)
{
   return ADSP_EOK;
}


ADSPResult rs_drv_job_close(rs_drv_handle_t rs_drv_h)
{
   return ADSP_EOK;
}

ADSPResult rs_drv_ioctl(rs_drv_handle_t rs_drv_h, rs_drv_ioctl_prop_info_t *prop_info_ptr)
{
   return ADSP_EOK;
}





