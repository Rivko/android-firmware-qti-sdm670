#ifndef __ECHOCANCELLER_API_H__
#define __ECHOCANCELLER_API_H__


#define VOICE_MEM_ALIGN(t, a)    t __attribute__((aligned(a)))
#define MAX_TAPS 400
#define SMECNS_MAX_TAPS2 200
#define TAPS_BG  32
#define TH_PATHDET 20480
#define ERL_STEP 16800
#define WB_GAMMA_NN      256         // (1) Modified to Q7.8 format  - Changes May 2010
#define WB_GAMMA_EE      512         // (2) Modified to Q7.8 format

#define WB_TAIL_ALPHA    6000       // (1) Q15 high-band echo tail alpha
#define WB_TAIL_PORTION  4000       // (1) Q15 high-band echo tail portion

#define CSIM_VERSION     104003

typedef struct {
    /* Global */
    //alignment to make the wrapper structure similar to the lib structure EC_PAR
    VOICE_MEM_ALIGN(short nearEndHPFCoeffs[6],8); //used in tx path
    short nb_samples;
    short mode;
    short tuning_mode;
    short echo_path_delay;
    /* GAINS */
    short OutputGain;
    short InputGain;
    /* NLMS */
    short AF_limit;
    short AF_taps;
    short AF_preset_coefs;
    short AF_twoalpha;
    short AF_erl;
    short AF_offset;
    /* PCD */
    short AF_taps_bg;
    short AF_erl_bg;
    short PCD_threshold;
    short DENS_gamma_e_rescue;
    short MinimumErl;
    short ErlStep;
    /* Detectors */
    short SPDET_far;
    short SPDET_mic;
    short SPDET_x_clip;
    /* DENS */
    short DENS_tail_alpha;
    short DENS_tail_portion;
    short DENS_gamma_e_alpha;
    short DENS_spdet_near;
//WTI
    short DENS_spdet_act;
    short DENS_gamma_e_high;
    short DENS_gamma_e_dt;
    short DENS_gamma_e_low;
    short DENS_gamma_n;
    short DENS_NFE_blocksize;
    short DENS_limit_NS;
    short DENS_NL_atten;
    short DENS_CNI_level;
    /* WB extension */
    short WB_echo_ratio;
    short WB_gamma_n;
    short WB_gamma_e;
    short max_noise_floor;
    short det_threshold;
    short WB_tail_alpha;
    short WB_tail_portion;
    /* High Band */
    short NLMS_PostGain;
    short NLMS_High_limit;
    short NLMS_High_taps;
    short NLMS_High_twoalpha;
    short NLMS_High_erl;
    short NLMS_High_offset;
    short WB_Echo_Scale;
    short Rx_Ref_Gain;
} EC_PAR;

int ec_get_lib_static_size(void);
int ec_get_lib_parameters_size(void);
int ec_get_nb_samples(void *par);
void ec_set_tx_sample_slip(void *staticPtr, int txSampleSlip);
int ec_get_tx_sample_slip(void *staticPtr);
void ec_set_rx_advance_factor(void *staticPtr, int rxAdvanceFactor);
int ec_get_rx_advance_factor(void *staticPtr);
void ec_tx( short *aubuf, short *farBuffer, short *nbuf, void *staticPtr, void *parametersPtr, short aubufsize );
short ec_init( void *s, void *p ,int16 *presetcoeffs, int presetcoeffsLen);
void ec_close(void *StaticPtr);
short *ec_get_taps_ptr_len(void *staticPtr, short *nlmsCoefLen);

#endif

