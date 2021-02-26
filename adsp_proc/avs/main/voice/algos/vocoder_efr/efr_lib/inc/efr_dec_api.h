/********************************************************************************
 *
 * File             : efr_dec_export.h
 * Purpose          : API functions for EFR Decoder 
 *
 ********************************************************************************
 */
#ifndef _efr_dec_export_H_
#define _efr_dec_export_H_

/* These constants define the number of consecutive parameters
   that function decoder_homing_frame_test() checks */

extern void efr_decoder(
      void *pEDS,
      int16 *serial,
      int16 **synth
      );

extern void efr_reset_dec (
      void *EfrDecStructPtr
      );

extern int get_efr_dec_struct_size(void);
extern void gsm_efr_repacking_mvs_to_dsp(int8 *iPacket);

#endif
