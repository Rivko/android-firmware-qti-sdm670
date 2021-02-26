/********************************************************************************
 *
 * File             : efr_enc_export.h
 * Purpose          : API functions for EFR Encoder 
 *
 ********************************************************************************
 */
#ifndef _efr_enc_export_H_
#define _efr_enc_export_H_

/* These constants define the number of consecutive parameters
   that function decoder_homing_frame_test() checks */

extern void efr_encoder(
      void *EfrEncStructPtr,				// i/o: encoder struct
      int16 *inputPtr,						// i: input buffer
      int16 *efrPacket,
      int16 *pVad, 
      int16* pSp, 
      int16 dtx	                     // i/o: parameters
      );

extern void efr_reset_enc (
      void *EfrEncStructPtr
      );

extern int get_efr_enc_struct_size(void);
extern void gsm_efr_repacking_dsp_to_mvs(int8 *iPacket);

#endif
