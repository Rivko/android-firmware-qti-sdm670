#ifndef __GSM_FR_DEC__
#define __GSM_FR_DEC__
/*_________________________________________________________________________
 |                                                                                                                                                   |
 |                           Function Prototypes                                                                                          |
 |_________________________________________________________________________ |
*/
extern size_t get_fr_dec_struct_size (void);
extern void   gsm_fr_decoderInit(void *decState);
extern void   gsm_fr_decoder(void *decState, int16 *packet, int16 *output,int16 lsb_to_msb_unpack_flag);
extern void   gsm_fr_repacking_mvs_to_dsp(int8 *iPacket, int16 *oPacket);
#endif
