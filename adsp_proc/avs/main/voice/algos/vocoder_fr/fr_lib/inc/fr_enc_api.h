#ifndef __GSM_FR_ENC__
#define __GSM_FR_ENC__
/*_________________________________________________________________________
 |                                                                                                                                                   |
 |                           Function Prototypes                                                                                          |
 |_________________________________________________________________________ |
*/
extern size_t get_fr_enc_struct_size (void);
extern void   gsm_fr_encoderInit(void *EncState);
extern void   gsm_fr_vad_reset(void *EncState);
extern void   gsm_fr_encoder(void *EncState,int16 *input,int16 *packet, int16 dtx_mode,int16 lsb_to_msb_pack_flag);
extern void   gsm_fr_repacking_dsp_to_mvs(int16 *iPacket, int8 *oPacket);

#endif
