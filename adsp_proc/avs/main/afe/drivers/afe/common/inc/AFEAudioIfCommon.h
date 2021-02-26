#ifndef __AFE_AUDIOIF_COMMON_H__
#define __AFE_AUDIOIF_COMMON_H__

#if defined(__cplusplus)
extern "C" {
#endif


/** 
 * Define a new type for the interfaces supported by audio hardware
 */
typedef enum
{
   AUDIOIF_I2S = 0,
   AUDIOIF_PCM, 
   AUDIOIF_TDM,
   AUDIOIF_COMMON_MAX,
}audioif_hw_t;

ADSPResult afe_audioif_mux_init(void);
ADSPResult afe_audioif_mux_deinit(void);
ADSPResult afe_audioif_mux_alloc(audioif_hw_t intf_type, uint32_t mux_index);
ADSPResult afe_audioif_mux_free(audioif_hw_t intf_type, uint32_t mux_index);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif
