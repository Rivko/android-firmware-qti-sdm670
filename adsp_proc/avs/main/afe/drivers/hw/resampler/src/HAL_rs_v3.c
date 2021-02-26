/*
============================================================================

FILE:          HAL_rs_v3.c

DESCRIPTION:   Resampler HW driver HAL file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================

============================================================================
  EDIT HISTORY FOR MODULE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------

============================================================================
*/

#include "HAL_rs_v3.h"

#include "LPASS_ADDRESS_FILE_RS_v3.h"   

#ifndef HAL_DEBUG
//#define HAL_DEBUG 
#endif


// The following is the organization of the filter memory in the model:
// 1. The upsampler filter memory is 80 words in length.
// 2. The intermediate buffer is 15 samples in length and follows the upsampler filter memory. 
// 3. The downsampler filter memory size is programmable and it follows the downsampler filter memory.
// Why is the downsampler memory size programmable and not the upsampler filter memory?
// The upsampler filter memory can only take two values  - 31(HQ) and 51 (SHQ). 
// There is no significant advantage in keeping this programmable in terms of bandwidth. 
// The downsampler filter memory can vary a lot - minimum is 59 words (for HQ resampling from 16000->8000).
// The maximum is 1105 words ( for SHQ resampling from 192000->8000). 
// It has been kept this way because this can help in reducing the bandwidth for reading filter memory.
#define UPSAMPLING_FILTER_MEM_LENGTH_ALL                  128             //DWs 
#define UPSAMPLING_FILTER_COEFF_LENGTH_HQ                 1872           //For RAM case only
#define UPSAMPLING_FILTER_COEFF_LENGTH_SHQ                12480          //For RAM case only
#define DOWNSAMPLING_FILTER_COEFF_LENGTH_HQ               1872           //For RAM case only
#define DOWNSAMPLING_FILTER_COEFF_LENGTH_SHQ              1872          //For RAM case only

#define STREAM_STRUCTURE_STATIC_SIZE_IN_DWS_V3            21 

#define BURST_LENGTH_FOR_LOADING_V3                       16     //The burst length for loading and saving filter states, input/output samples, the unit is DW


#define POWER_MODE_SUPPORTED_HIGHEST                             POWER_MODE_NORMAL     //TBD: will move to POWER_MODE_TURBO once VI verified
#define POWER_MODE_SUPPORTED_LOWEST                              POWER_MODE_SVS 


static const struct_clock_table rs_clock_table_v3[] =
{
   //power mode        core_clock     core_clock_period (ns * 1000)     AHBI_clock      AHBI_clok_period (ns * 1000)   latency (ns * 1000)
   {POWER_MODE_TURBO,  351090000,     2848,                             153600000,      6510,                          300000},       //do not support
   {POWER_MODE_NORMAL, 307200000,     3225,                             153600000,      6510,                          300000},
   {POWER_MODE_SVS,    153600000,     6510,                              76800000,      13021,                         300000},
   {POWER_MODE_SVS2,   76800000,      13021,                             38400000,      26042,                         300000},
};

static int32 rs_base_virt_addr = 0;
/************************************************
 *            HAL    Functions                  *
 ************************************************/

/**
 * Set the virtual address for HW resampler base register address
 */
//void HAL_rs_init_v3(uint32 base_virt_addr)
//{
//   rs_base_virt_addr = base_virt_addr - LPASS_RESAMPLER_BASE;   
   //rs_base_virt_addr = base_virt_addr;
//}
 
/**
 * Set the resampler to start
 */
void HAL_rs_start_v3(uint16 px, uint16 mask)
{
#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_start Begin\n");
#endif        

   reg_wr(rs_base_virt_addr + LPASS_RESAMPLER_CONTROL, (mask << LPASS_RESAMPLER_CONTROL__INT0_MASK___S) & LPASS_RESAMPLER_CONTROL__INT0_MASK___S ); 

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_start End\n");
#endif 
}

/**
 * Get the channel number of current job
 */
uint16 HAL_rs_get_cur_channel_v3(uint16 px)
{
   uint32 tmp; 

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_cur_channel Begin\n");
#endif

   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_STATUS, &tmp);

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_cur_channel End\n");
#endif

   return ((tmp & LPASS_RESAMPLER_STATUS__CURRENT_CHANNEL___M) >> LPASS_RESAMPLER_STATUS__CURRENT_CHANNEL___S);
}


/**
 * Get current job ID = {Processor ID, Job ID}
 */
uint16 HAL_rs_get_cur_job_id_v3(uint16 px)
{
   uint32 tmp;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_cur_job_id Begin\n");
#endif

   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_STATUS, &tmp);


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_cur_job_id End\n");
#endif

   return ((tmp & LPASS_RESAMPLER_STATUS__CURRENT_JOB_ID___M) >> LPASS_RESAMPLER_STATUS__CURRENT_JOB_ID___S);   //current_job_id = {Processor ID, Job ID}
}


/**
 * Get current operation status, 0:sleep, 1:active
 */
uint16 HAL_rs_get_cur_op_status_v3(uint16 px)
{
   uint32 tmp;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_cur_op_status Begin\n");
#endif

   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_STATUS, &tmp);

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_cur_op_status End\n");
#endif

   return (tmp & LPASS_RESAMPLER_STATUS__OP_STATUS___M) >> LPASS_RESAMPLER_STATUS__OP_STATUS___S;
}

//num_channels starts from 1 to 32
uint16 HAL_get_stream_struct_size_v3(uint16 num_channels)   //in bytes
{
   uint32 size = STREAM_STRUCTURE_STATIC_SIZE_IN_DWS_V3 * sizeof(uint32);

   //add INPUT and OUTPUT buffer lsb pointer according to num of channel of this job
   size = size + num_channels * 2 * sizeof(uint32);
   //size need to be multiply of 32
   if(size % 32 >0)
   {
      size = ((size / 32) + 1) * 32;
   }

   return size;
}

/**
 * wrap up stream config info and write to RESAMPLER_Px_STREAM_CFG
 */
void HAL_rs_set_stream_config_v3(uint16 px, struct_rs_config* ptr_config_struct)
{
   uint32 config_reg = 0;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_stream_config Begin\n");
#endif    
   //fill in STREAM_CFG register
   config_reg += (ptr_config_struct->dynamic_mode << LPASS_RESAMPLER_STREAM_CFG__DYNAMIC_MODE___S) & LPASS_RESAMPLER_STREAM_CFG__DYNAMIC_MODE___M;
   config_reg += (ptr_config_struct->force_interpol << LPASS_RESAMPLER_STREAM_CFG__FORCE_INTERPOL___S) & LPASS_RESAMPLER_STREAM_CFG__FORCE_INTERPOL___M;
   config_reg += (ptr_config_struct->us_interp_mode << LPASS_RESAMPLER_STREAM_CFG__US_INTERP_MODE___S) & LPASS_RESAMPLER_STREAM_CFG__US_INTERP_MODE___M;
   config_reg += (ptr_config_struct->first_frame << LPASS_RESAMPLER_STREAM_CFG__FIRST_FRAME___S) & LPASS_RESAMPLER_STREAM_CFG__FIRST_FRAME___M;
   config_reg += (ptr_config_struct->data_width << LPASS_RESAMPLER_STREAM_CFG__DATA_WIDTH___S) & LPASS_RESAMPLER_STREAM_CFG__DATA_WIDTH___M;
   config_reg += (ptr_config_struct->ds_coeff_id << LPASS_RESAMPLER_STREAM_CFG__DS_COEFF_ID___S) & LPASS_RESAMPLER_STREAM_CFG__DS_COEFF_ID___M;
   config_reg += (ptr_config_struct->us_coeff_id << LPASS_RESAMPLER_STREAM_CFG__US_COEFF_ID___S) & LPASS_RESAMPLER_STREAM_CFG__US_COEFF_ID___M;
   config_reg += (ptr_config_struct->operation << LPASS_RESAMPLER_STREAM_CFG__OPERATION___S) & LPASS_RESAMPLER_STREAM_CFG__OPERATION___M;
   config_reg += (ptr_config_struct->job_id << LPASS_RESAMPLER_STREAM_CFG__JOB_ID___S) & LPASS_RESAMPLER_STREAM_CFG__JOB_ID___M;
   config_reg += ((ptr_config_struct->num_channels-1) << LPASS_RESAMPLER_STREAM_CFG__NUM_OF_CHANNELS___S) & LPASS_RESAMPLER_STREAM_CFG__NUM_OF_CHANNELS___M;
   config_reg += (ptr_config_struct->submit_job << LPASS_RESAMPLER_STREAM_CFG__SUBMIT_JOB___S) & LPASS_RESAMPLER_STREAM_CFG__SUBMIT_JOB___M;

#ifdef HAL_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"======@HAL config reg value: 0x%x\n",config_reg);
#endif	
   //Write to Resampler HW

   reg_wr(rs_base_virt_addr + LPASS_RESAMPLER_STREAM_CFG, config_reg);

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_stream_config End\n");
#endif  
} 

/**
 * wrap up stream config info and write to RESAMPLER_Px_STREAM_CFG
 */
void HAL_rs_get_stream_config_v3(uint16 px, struct_rs_config* ptr_config_struct)
{
   uint32 config_reg=0;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_stream_config Begin\n");
#endif

  //Read Resampler Stream CFG reg
   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_STREAM_CFG, &config_reg);

   //fill in STREAM_CFG register

   ptr_config_struct->force_interpol = (config_reg & LPASS_RESAMPLER_STREAM_CFG__FORCE_INTERPOL___M) >> LPASS_RESAMPLER_STREAM_CFG__FORCE_INTERPOL___S;
   ptr_config_struct->us_interp_mode = (config_reg & LPASS_RESAMPLER_STREAM_CFG__US_INTERP_MODE___M)>> LPASS_RESAMPLER_STREAM_CFG__US_INTERP_MODE___S;
   ptr_config_struct->first_frame = (config_reg & LPASS_RESAMPLER_STREAM_CFG__FIRST_FRAME___M) >> LPASS_RESAMPLER_STREAM_CFG__FIRST_FRAME___S;
   ptr_config_struct->data_width = (config_reg & LPASS_RESAMPLER_STREAM_CFG__DATA_WIDTH___M)>> LPASS_RESAMPLER_STREAM_CFG__DATA_WIDTH___S;
   ptr_config_struct->ds_coeff_id = (config_reg & LPASS_RESAMPLER_STREAM_CFG__DS_COEFF_ID___M) >> LPASS_RESAMPLER_STREAM_CFG__DS_COEFF_ID___S;
   ptr_config_struct->us_coeff_id = (config_reg & LPASS_RESAMPLER_STREAM_CFG__US_COEFF_ID___M)>> LPASS_RESAMPLER_STREAM_CFG__US_COEFF_ID___S;
   ptr_config_struct->operation = (config_reg & LPASS_RESAMPLER_STREAM_CFG__OPERATION___M) >> LPASS_RESAMPLER_STREAM_CFG__OPERATION___S;
   ptr_config_struct->job_id = (config_reg & LPASS_RESAMPLER_STREAM_CFG__JOB_ID___M) >> LPASS_RESAMPLER_STREAM_CFG__JOB_ID___S;
   ptr_config_struct->num_channels = ((config_reg & LPASS_RESAMPLER_STREAM_CFG__NUM_OF_CHANNELS___M) >> LPASS_RESAMPLER_STREAM_CFG__NUM_OF_CHANNELS___S) + 1;
   ptr_config_struct->submit_job = (config_reg & LPASS_RESAMPLER_STREAM_CFG__SUBMIT_JOB___M) >> LPASS_RESAMPLER_STREAM_CFG__SUBMIT_JOB___S;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_stream_config End\n");
#endif
} 


/**
 * Set the stream structure pointer
 */
void HAL_rs_set_stream_struct_v3(uint16 px, struct_rs_stream* stream_struct, uint32* ptr_stream_mem, uint32* ptr_stream_mem_phys, uint16 num_channels)
{
   // map stream structure to external memory chunck
   uint32 *temp = ptr_stream_mem;
   uint16 i;


#ifdef HAL_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_stream_struct Begin\n");
#endif       

   // -- For RESAMPLER_FRAME_SIZE
   *temp = ((stream_struct->in_frame_size << LPASS_RESAMPLER_FRAME_SIZE__INPUT_FRAME_SIZE___S) & LPASS_RESAMPLER_FRAME_SIZE__INPUT_FRAME_SIZE___M) | 
           ((stream_struct->out_frame_size << LPASS_RESAMPLER_FRAME_SIZE__OUTPUT_FRAME_SIZE___S) & LPASS_RESAMPLER_FRAME_SIZE__OUTPUT_FRAME_SIZE___M);

   temp++;

   // -- For RESAMPLER_FILT_INDEX_PTRS_I
   *temp = ((stream_struct->up_sampler_flt_fptr << LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_FPTR___S) & LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_FPTR___M) |
           ((stream_struct->up_sampler_flt_optr << LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_OPTR___S) & LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_OPTR___M);
   temp++;

   // -- For RESAMPLER_FILT_INDEX_PTRS_II
   *temp = ((stream_struct->dn_sampler_flt_fptr << LPASS_RESAMPLER_FILT_INDEX_PTRS_II__DNSAMPLER_FLT_FPTR___S) & LPASS_RESAMPLER_FILT_INDEX_PTRS_II__DNSAMPLER_FLT_FPTR___M) |
           ((stream_struct->resampler_flt_iptr << LPASS_RESAMPLER_FILT_INDEX_PTRS_II__RESAMPLER_IPTR___S) & LPASS_RESAMPLER_FILT_INDEX_PTRS_II__RESAMPLER_IPTR___M);
   temp++;              

   // -- For RESAMPLER_US_CUR_PHASE
   *temp = ((stream_struct->cur_int_phase << LPASS_RESAMPLER_US_CUR_PHASE__INTPHASE___S) & LPASS_RESAMPLER_US_CUR_PHASE__INTPHASE___M) |
           ((stream_struct->cur_frac_phase << LPASS_RESAMPLER_US_CUR_PHASE__FRACPHASE___S) & LPASS_RESAMPLER_US_CUR_PHASE__FRACPHASE___M);
   temp++;

   // -- For RESAMPLER_US_FILTER_CONSTS
   *temp = ((stream_struct->frac_const << LPASS_RESAMPLER_US_FILTER_CONSTS__FRACCONST___S) & LPASS_RESAMPLER_US_FILTER_CONSTS__FRACCONST___M) |
           ((stream_struct->shift_const << LPASS_RESAMPLER_US_FILTER_CONSTS__SHIFTCONST___S) & LPASS_RESAMPLER_US_FILTER_CONSTS__SHIFTCONST___M);
   temp++;

   // -- For RESAMPLER_US_PHASE_STEP_SIZE
   *temp = ((stream_struct->int_phase_step_size << LPASS_RESAMPLER_US_PHASE_STEP_SIZE__INTPHASESTEPSIZE___S) & LPASS_RESAMPLER_US_PHASE_STEP_SIZE__INTPHASESTEPSIZE___M) |
           ((stream_struct->frac_phase_step_size<< LPASS_RESAMPLER_US_PHASE_STEP_SIZE__FRACPHASESTEPSIZE___S)& LPASS_RESAMPLER_US_PHASE_STEP_SIZE__FRACPHASESTEPSIZE___M);
   temp++;

   // -- For RESAMPLER_US_OUTPUT_FREQ
   *temp = (stream_struct->out_freq << LPASS_RESAMPLER_US_OUTPUT_FREQ__OUTFREQ___S) & LPASS_RESAMPLER_US_OUTPUT_FREQ__OUTFREQ___M;
   temp++;

   // -- For RESAMPLER_DS_CFG
   *temp = ((stream_struct->dn_step << LPASS_RESAMPLER_DS_CFG__DS_STEP___S) & LPASS_RESAMPLER_DS_CFG__DS_STEP___M)       |
           ((stream_struct->dn_factor << LPASS_RESAMPLER_DS_CFG__DS_FACTOR___S) & LPASS_RESAMPLER_DS_CFG__DS_FACTOR___M) |
           ((stream_struct->dn_filt_mem_size << LPASS_RESAMPLER_DS_CFG__DS_FILT_MEM_SIZE___S) & LPASS_RESAMPLER_DS_CFG__DS_FILT_MEM_SIZE___M);
   temp++;

   // -- For RESAMPLER_FILT_MEM_BASE_ADDR
   *temp = (stream_struct->filt_mem_base_addr_phys << LPASS_RESAMPLER_FILT_MEM_BASE_ADDR___S) & LPASS_RESAMPLER_FILT_MEM_BASE_ADDR___M;
   temp++;

   // ---For RESAMPLER_FILT_MEM_BASE_ADDR_MSB, V3 only
   *temp = (stream_struct->filt_mem_base_addr_phys_msb << LPASS_RESAMPLER_FILT_MEM_BASE_ADDR_MSB__ADDR___S) & LPASS_RESAMPLER_FILT_MEM_BASE_ADDR_MSB__ADDR___M;
   temp++;

   // -- For RESAMPLER_STREAM_STATUS_I
   *temp = (stream_struct->status_i_value << LPASS_RESAMPLER_STREAM_PROCESS_COUNTER__CLKS___S) & LPASS_RESAMPLER_STREAM_PROCESS_COUNTER__CLKS___M;
   temp++;

   // -- For RESAMPLER_STREAM_STATUS_II
   *temp = (stream_struct->status_ii_value << LPASS_RESAMPLER_STREAM_XFER_COUNTER__CLKS___S) & LPASS_RESAMPLER_STREAM_XFER_COUNTER__CLKS___M;
   temp++;

   // -- For RESAMPLER_STREAM_SPARE
   *temp = (stream_struct->spare_value << LPASS_RESAMPLER_STREAM_HALT_COUNTER__CLKS___S) & LPASS_RESAMPLER_STREAM_HALT_COUNTER__CLKS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH7_0_IBUF_OFFSET, V3 only
   *temp = (stream_struct->inp_buf_addr_phys_ch7_0_offset << LPASS_RESAMPLER_CH7_0_IBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH7_0_IBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH7_0_OBUF_OFFSET, V3 only
   *temp = (stream_struct->out_buf_addr_phys_ch7_0_offset << LPASS_RESAMPLER_CH7_0_OBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH7_0_OBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH15_8_IBUF_OFFSET, V3 only
   *temp = (stream_struct->inp_buf_addr_phys_ch15_8_offset << LPASS_RESAMPLER_CH15_8_IBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH15_8_IBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH15_8_OBUF_OFFSET, V3 only
   *temp = (stream_struct->out_buf_addr_phys_ch15_8_offset << LPASS_RESAMPLER_CH15_8_OBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH15_8_OBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH23_16_IBUF_OFFSET, V3 only
   *temp = (stream_struct->inp_buf_addr_phys_ch23_16_offset << LPASS_RESAMPLER_CH23_16_IBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH23_16_IBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH23_16_OBUF_OFFSET, V3 only
   *temp = (stream_struct->out_buf_addr_phys_ch23_16_offset << LPASS_RESAMPLER_CH23_16_OBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH23_16_OBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH31_23_IBUF_OFFSET, V3 only
   *temp = (stream_struct->inp_buf_addr_phys_ch31_24_offset << LPASS_RESAMPLER_CH31_23_IBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH31_23_IBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For LPASS_RESAMPLER_CH31_23_OBUF_OFFSET, V3 only
   *temp = (stream_struct->out_buf_addr_phys_ch31_24_offset << LPASS_RESAMPLER_CH31_23_OBUF_OFFSET__ADDRESS___S) & LPASS_RESAMPLER_CH31_23_OBUF_OFFSET__ADDRESS___M;
   temp++;

   // -- For RESAMPLER_INP/OUTP_BUFF_CH0-31
   for(i = 0; i < num_channels; i++)
   {
      *temp = stream_struct->inp_buf_addr_phys[i];
      temp++;
      *temp = stream_struct->out_buf_addr_phys[i];
      temp++;
   }

   qurt_elite_memorymap_cache_flush( (uint32)ptr_stream_mem, HAL_get_stream_struct_size(num_channels));
   
   //will be replaced by qurt wrap API
   //Do syncht to guraneete the right memory order before HW read it.
      __asm__  __volatile__ (
    "    SYNCHT \n"
    :::);    

   // write the memory pointer to HW register
   reg_wr(rs_base_virt_addr + LPASS_RESAMPLER_STRUCTURE_PTR, (uint32)ptr_stream_mem_phys);
}

/**
 * Set the stream structure pointer
 */
void HAL_rs_get_stream_struct_v3(uint16 px, struct_rs_stream* stream_struct, uint32* ptr_stream_mem, uint32* ptr_stream_mem_phys)
{
   volatile uint32 *temp;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_stream_struct Begin\n");
#endif

   qurt_elite_memorymap_cache_invalidate((uint32)ptr_stream_mem, HAL_get_stream_struct_size(0));   //this is OS/firmware dependent. How to avoid this?

   temp = ptr_stream_mem; 

   // -- For RESAMPLER_FRAME_SIZE
#ifdef HAL_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"RESAMPLER_FRAME_SIZE: 0x%x\n",*temp);
#endif

   stream_struct->in_frame_size = ((*temp) & LPASS_RESAMPLER_FRAME_SIZE__INPUT_FRAME_SIZE___M) >> LPASS_RESAMPLER_FRAME_SIZE__INPUT_FRAME_SIZE___S;
   stream_struct->out_frame_size = ((*temp) & LPASS_RESAMPLER_FRAME_SIZE__OUTPUT_FRAME_SIZE___M) >> LPASS_RESAMPLER_FRAME_SIZE__OUTPUT_FRAME_SIZE___S;
   temp++;

   // -- For RESAMPLER_FILT_INDEX_PTRS_I
   stream_struct->up_sampler_flt_fptr = ((*temp) & LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_FPTR___M) >> LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_FPTR___S;
   stream_struct->up_sampler_flt_optr = ((*temp) & LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_OPTR___M) >> LPASS_RESAMPLER_FILT_INDEX_PTRS_I__UPSAMPLER_FLT_OPTR___S;
   temp++;

   // -- For RESAMPLER_FILT_INDEX_PTRS_II
   stream_struct->dn_sampler_flt_fptr = ((*temp) & LPASS_RESAMPLER_FILT_INDEX_PTRS_II__DNSAMPLER_FLT_FPTR___M) >> LPASS_RESAMPLER_FILT_INDEX_PTRS_II__DNSAMPLER_FLT_FPTR___S;
   stream_struct->resampler_flt_iptr = ((*temp) & LPASS_RESAMPLER_FILT_INDEX_PTRS_II__RESAMPLER_IPTR___M) >> LPASS_RESAMPLER_FILT_INDEX_PTRS_II__RESAMPLER_IPTR___S;
   temp++;

   // -- For RESAMPLER_US_CUR_PHASE
#ifdef HAL_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"RESAMPLER_US_CUR_PHASE: 0x%x\n",*temp);    
#endif

   stream_struct->cur_int_phase = ((*temp) & LPASS_RESAMPLER_US_CUR_PHASE__INTPHASE___M) >> LPASS_RESAMPLER_US_CUR_PHASE__INTPHASE___S;
   stream_struct->cur_frac_phase= ((*temp) & LPASS_RESAMPLER_US_CUR_PHASE__FRACPHASE___M) >> LPASS_RESAMPLER_US_CUR_PHASE__FRACPHASE___S;
   temp++;

   // -- For RESAMPLER_US_FILTER_CONSTS
#ifdef HAL_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"RESAMPLER_US_FILTER_CONSTS: 0x%x\n",*temp);    
#endif

   stream_struct->frac_const = ((*temp) & LPASS_RESAMPLER_US_FILTER_CONSTS__FRACCONST___M) >> LPASS_RESAMPLER_US_FILTER_CONSTS__FRACCONST___S; 
   stream_struct->shift_const = ((*temp) & LPASS_RESAMPLER_US_FILTER_CONSTS__SHIFTCONST___M) >> LPASS_RESAMPLER_US_FILTER_CONSTS__SHIFTCONST___S;
   temp++;

   // -- For RESAMPLER_US_PHASE_STEP_SIZE
#ifdef HAL_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"RESAMPLER_US_PHASE_STEP_SIZE: 0x%x\n",*temp);    
#endif

   stream_struct->int_phase_step_size = ((*temp) & LPASS_RESAMPLER_US_PHASE_STEP_SIZE__INTPHASESTEPSIZE___M) >> LPASS_RESAMPLER_US_PHASE_STEP_SIZE__INTPHASESTEPSIZE___S;
   stream_struct->frac_phase_step_size= ((*temp) & LPASS_RESAMPLER_US_PHASE_STEP_SIZE__FRACPHASESTEPSIZE___M) >> LPASS_RESAMPLER_US_PHASE_STEP_SIZE__FRACPHASESTEPSIZE___S;
   temp++;   

   // -- For RESAMPLER_US_OUTPUT_FREQ
#ifdef HAL_DEBUG
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"RESAMPLER_US_OUTPUT_FREQ: 0x%x\n",*temp);    
#endif    
   stream_struct->out_freq = ((*temp) & LPASS_RESAMPLER_US_OUTPUT_FREQ__OUTFREQ___M) >> LPASS_RESAMPLER_US_OUTPUT_FREQ__OUTFREQ___S;
   temp++;   

   // -- For RESAMPLER_DS_CFG
#ifdef HAL_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"RESAMPLER_DS_CFG: 0x%x\n",*temp);    
#endif    
   stream_struct->dn_step = ((*temp) & LPASS_RESAMPLER_DS_CFG__DS_STEP___M) >> LPASS_RESAMPLER_DS_CFG__DS_STEP___S;
   stream_struct->dn_factor = ((*temp) & LPASS_RESAMPLER_DS_CFG__DS_FACTOR___M) >> LPASS_RESAMPLER_DS_CFG__DS_FACTOR___S;
   temp++;
       
   // -- For RESAMPLER_FILT_MEM_BASE_ADDR
   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO," RESAMPLER_FILT_MEM_BASE_ADDR: 0x%x\n",*temp);    
   stream_struct->filt_mem_base_addr_phys = ((*temp) & LPASS_RESAMPLER_FILT_MEM_BASE_ADDR___M) >> LPASS_RESAMPLER_FILT_MEM_BASE_ADDR___S;
   temp++;   

   // For LPASS_RESAMPLER_FILT_MEM_BASE_ADDR_MSB
   stream_struct->filt_mem_base_addr_phys_msb = ((*temp) & LPASS_RESAMPLER_FILT_MEM_BASE_ADDR_MSB__ADDR___M) >> LPASS_RESAMPLER_FILT_MEM_BASE_ADDR_MSB__ADDR___S;
   temp++;   
       
   //RESAMPLER_STREAM_STATUS_I
   stream_struct->status_i_value = ((*temp) & LPASS_RESAMPLER_STREAM_PROCESS_COUNTER__CLKS___M) >> LPASS_RESAMPLER_STREAM_PROCESS_COUNTER__CLKS___S;
   temp++; 

   //RESAMPLER_STREAM_STATUS_II
   stream_struct->status_ii_value = ((*temp) & LPASS_RESAMPLER_STREAM_XFER_COUNTER__CLKS___M) >> LPASS_RESAMPLER_STREAM_XFER_COUNTER__CLKS___S;
   temp++; 

   //RESAMPLER_STREAM_SPARE
   stream_struct->spare_value = ((*temp) & LPASS_RESAMPLER_STREAM_HALT_COUNTER__CLKS___M) >> LPASS_RESAMPLER_STREAM_HALT_COUNTER__CLKS___S;
   temp++; 

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_stream_struct End\n");
#endif
}


/**
 * Check the job submit ack status, return 1 if valid, 0 invalid
 */
boolean HAL_rs_is_ack_valid_v3(uint16 px)
{
   //return valid bit
   uint32 rtn_ack;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_is_ack_valid Begin\n");
#endif

   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_ACK, &rtn_ack);

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_is_ack_valid End\n");
#endif  
   return (rtn_ack & LPASS_RESAMPLER_ACK__VALID___M) >> LPASS_RESAMPLER_ACK__VALID___S;
}

/**
 * check the job submit status, return 1: accepted, 0 rejected
 * Job ID will be updated in the *job_id pointer location
 */
boolean HAL_rs_get_job_accepted_status_v3(uint16 px, uint16 *job_id)
{
   //read job accepted status and job id from register
   uint32 ack_reg;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_job_accepted_status Begin\n");
#endif    
   //write job ID to input pointer
   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_ACK, &ack_reg);

   //return job accepted status
    *job_id = (ack_reg & LPASS_RESAMPLER_ACK__JOB_ID___M) >> LPASS_RESAMPLER_ACK__JOB_ID___S;  

#ifdef HAL_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_job_accepted_status End\n");
#endif    

   return ack_reg & LPASS_RESAMPLER_ACK__ACCEPT___M;
}

/*
 * Check if the job is done, job finish status will be update in 
 * job array using the input pointer
 */
void HAL_rs_get_job_done_status_v3(uint16 px, uint16 *ptr_job_array)
{
   uint32 job_status=0;
   uint32 i;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_job_done_status Begin\n");
#endif

   //read from job done status register
   reg_rd(rs_base_virt_addr + LPASS_RESAMPLER_JOB_STATUS, &job_status);

   //translate 32bit status info to job_array[16]
   for(i = 0; i < MAX_JOB_NUM; i++)
   {
      *ptr_job_array = (job_status >> (i*2)) & 0x3;
      ptr_job_array++;
   }

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_job_done_status End\n");
#endif
}

/*
 * clear finished jobs
 *
 * @param[in] job_id ID of finished job
 */
void HAL_rs_set_job_clear_v3(uint16 px, uint16 job_id, uint16 this_job_status)
{
   uint32 reg = this_job_status;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_job_clear Begin\n");
#endif  
     
   reg = reg << (job_id * 2);

   reg_wr(rs_base_virt_addr + LPASS_RESAMPLER_JOB_CLEAR, reg);

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_job_clear End\n");
#endif  
}


/**
 * wrap up for getting sum of the clocks estimated in HI queue
 */
uint32 HAL_rs_get_estimated_total_clks_hiq_v3(uint16 px)
{
   uint32 hiq_perf_reg=0;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_estimated_total_clks_hiq Begin\n");
#endif

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_estimated_total_clks_hiq End\n");
#endif

   return hiq_perf_reg;
} 

/**
 * wrap up for getting sum of the clocks estimated in LOW queue
 */
uint32 HAL_rs_get_estimated_total_clks_loq_v3(uint16 px)
{
   uint32 loq_perf_reg=0;

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_estimated_total_clks_loq Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_estimated_total_clks_loq End\n");
#endif

   return loq_perf_reg;
} 

/**
 * Setting up estimated clks for this job
 */
void HAL_rs_set_estimated_clks_v3(uint16 px, uint32 clks)
{
#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_estimated_clks Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_set_estimated_clks End\n");
#endif
}


/**
 * get filter length
 */
uint32 HAL_rs_get_upsampler_flt_mem_length_v3(void)
{

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_upsampler_flt_mem_length_v3 Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_upsampler_flt_mem_length_v3 End\n");
#endif

   return UPSAMPLING_FILTER_MEM_LENGTH_ALL;
} 

/**
 * get filter length
 */
uint32 HAL_rs_get_upsampler_flt_coeff_length_hq_v3(void)
{

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_upsampler_flt_coeff_length_hq_v3 Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_upsampler_flt_coeff_length_hq_v3 End\n");
#endif

   return UPSAMPLING_FILTER_COEFF_LENGTH_HQ;
} 

/**
 * get filter length
 */
uint32 HAL_rs_get_upsampler_flt_coeff_length_shq_v3(void)
{

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_upsampler_flt_coeff_length_shq_v3 Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_upsampler_flt_coeff_length_shq_v3 End\n");
#endif

   return UPSAMPLING_FILTER_COEFF_LENGTH_SHQ;
} 

/**
 * get filter length
 */
uint32 HAL_rs_get_downsampler_flt_coeff_length_hq_v3(void)
{

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_downsampler_flt_coeff_length_hq_v3 Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_downsampler_flt_coeff_length_hq_v3 End\n");
#endif

   return DOWNSAMPLING_FILTER_COEFF_LENGTH_HQ;
} 

/**
 * get filter length
 */
uint32 HAL_rs_get_downsampler_flt_coeff_length_shq_v3(void)
{

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_downsampler_flt_coeff_length_shq_v3 Begin\n");
#endif


#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_downsampler_flt_coeff_length_shq_v3 End\n");
#endif

   return DOWNSAMPLING_FILTER_COEFF_LENGTH_SHQ;
}


/**
 * get down sampler output buffer size 
 * It is 16 in most cases but it is 8 when div_factor is 24
 */
uint32 HAL_rs_get_downsampler_flt_output_buf_size_v3(uint16 div_factor)
{
#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_downsampler_flt_output_buf_size_v3 Begin\n");
#endif

   uint32 buf_size = 16;

   if(24 == div_factor)
   {
      buf_size = 8;
   }

#ifdef HAL_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@HAL_rs_get_downsampler_flt_output_buf_size_v3 End\n");
#endif

   return buf_size;
} 

/**
 * get hw resampler burst load size 

   The burst length for loading and saving filter states, input/oupt samples
 */
uint32 HAL_rs_get_memory_load_burst_size_v3(void)
{
   return BURST_LENGTH_FOR_LOADING_V3;
}

/**
 * get num of resampler core parallel process pipeline
 
   @return  num of pipeline
 */
uint32 HAL_rs_get_parallel_pipeline_num_v3(void)
{
   return 2;
}

/**
Query data from clock table
 
@return  
*/
uint32 HAL_rs_get_data_from_clk_tbl_v3(rs_hw_power_mode power_mode, rs_clock_idx clk_idx)
{
   struct_clock_table cur_tbl;
   uint32 cur_data = 0;
   int16 i;
   int16 max_num_tbls = sizeof(rs_clock_table_v3)/sizeof(struct_clock_table);

   for(i = 0; i < max_num_tbls; i++)
   {
      cur_tbl = rs_clock_table_v3[i];

      if(power_mode == cur_tbl.power_mode)
      {
         switch(clk_idx)
         {
            case CORE_CLOCK:
               cur_data = cur_tbl.core_clock;
               break;
            case CORE_CLOCK_PERIOD:
               cur_data = cur_tbl.core_clock_period;
               break;
            case AHB_CLOCK:
               cur_data = cur_tbl.ahb_clock;
               break;
            case AHB_CLOCK_PERIOD:
               cur_data = cur_tbl.ahb_clock_period;
               break;
            case LATENCY:
               cur_data = cur_tbl.latency;
               break;
            default:
               break;
         }

         break;
      }
      else
      {
         continue;
      }
   }

   return cur_data;
}

/**
get the lowest supported power mode
 
@return  power mode
*/
rs_hw_power_mode HAL_rs_get_power_mode_lowest_v3(void)
{
   return POWER_MODE_SUPPORTED_LOWEST;
}

/**
get the highest supported power mode
 
@return  power mode
*/
rs_hw_power_mode HAL_rs_get_power_mode_highest_v3(void)
{
   return POWER_MODE_SUPPORTED_HIGHEST;
}

void HAL_rs_init_v3(uint32 base_virt_addr, hal_rs_func_def_t* hal_func_def_ptr)
{
   hal_func_def_ptr->HAL_rs_init_f = NULL;
   hal_func_def_ptr->HAL_rs_start_f = HAL_rs_start_v3;
   hal_func_def_ptr->HAL_rs_get_cur_channel_f = HAL_rs_get_cur_channel_v3;
   hal_func_def_ptr->HAL_rs_get_cur_job_id_f = HAL_rs_get_cur_job_id_v3;
   hal_func_def_ptr->HAL_rs_get_cur_op_status_f = HAL_rs_get_cur_op_status_v3;
   hal_func_def_ptr->HAL_rs_set_stream_config_f = HAL_rs_set_stream_config_v3;
   hal_func_def_ptr->HAL_rs_get_stream_config_f = HAL_rs_get_stream_config_v3;
   hal_func_def_ptr->HAL_get_stream_struct_size_f = HAL_get_stream_struct_size_v3;
   hal_func_def_ptr->HAL_rs_set_stream_struct_f = HAL_rs_set_stream_struct_v3;
   hal_func_def_ptr->HAL_rs_get_stream_struct_f = HAL_rs_get_stream_struct_v3;
   hal_func_def_ptr->HAL_rs_is_ack_valid_f = HAL_rs_is_ack_valid_v3;
   hal_func_def_ptr->HAL_rs_get_job_accepted_status_f = HAL_rs_get_job_accepted_status_v3;
   hal_func_def_ptr->HAL_rs_get_job_done_status_f = HAL_rs_get_job_done_status_v3;
   hal_func_def_ptr->HAL_rs_set_job_clear_f = HAL_rs_set_job_clear_v3;
   hal_func_def_ptr->HAL_rs_get_estimated_total_clks_hiq_f = HAL_rs_get_estimated_total_clks_hiq_v3;
   hal_func_def_ptr->HAL_rs_get_estimated_total_clks_loq_f = HAL_rs_get_estimated_total_clks_loq_v3;
   hal_func_def_ptr->HAL_rs_set_estimated_clks_f = HAL_rs_set_estimated_clks_v3;

   hal_func_def_ptr->HAL_rs_get_upsampler_flt_mem_length_f = HAL_rs_get_upsampler_flt_mem_length_v3;
   hal_func_def_ptr->HAL_rs_get_upsampler_flt_coeff_length_hq_f = HAL_rs_get_upsampler_flt_coeff_length_hq_v3;
   hal_func_def_ptr->HAL_rs_get_upsampler_flt_coeff_length_shq_f = HAL_rs_get_upsampler_flt_coeff_length_shq_v3;
   hal_func_def_ptr->HAL_rs_get_downsampler_flt_coeff_length_hq_f = HAL_rs_get_downsampler_flt_coeff_length_hq_v3;
   hal_func_def_ptr->HAL_rs_get_downsampler_flt_coeff_length_shq_f = HAL_rs_get_downsampler_flt_coeff_length_shq_v3;

   hal_func_def_ptr->HAL_rs_get_downsampler_flt_output_buf_size_f = HAL_rs_get_downsampler_flt_output_buf_size_v3;
   hal_func_def_ptr->HAL_rs_get_memory_load_burst_size_f = HAL_rs_get_memory_load_burst_size_v3;

   hal_func_def_ptr->HAL_rs_get_parallel_pipeline_num_f = HAL_rs_get_parallel_pipeline_num_v3;
   hal_func_def_ptr->HAL_rs_get_data_from_clk_tbl_f = HAL_rs_get_data_from_clk_tbl_v3;

   hal_func_def_ptr->HAL_rs_get_power_mode_lowest_f = HAL_rs_get_power_mode_lowest_v3;
   hal_func_def_ptr->HAL_rs_get_power_mode_highest_f = HAL_rs_get_power_mode_highest_v3;

   rs_base_virt_addr = base_virt_addr - LPASS_RESAMPLER_BASE;   
}

