#ifndef __ECHOCANCELLERRX_EXPORT_H__
#define __ECHOCANCELLERRX_EXPORT_H__
#define LVHF_NUM_RX_SAMPLES 8

    void ec_rx_set_param( char *par_name, short par_val, void *parametersStruct, char *inst_name );
    void ec_rx( short *aubuf, void *staticPtr);
    void ec_rx_init( void *s, void *p );
    void ec_rx_close( void *voidStaticPtr );
	 void ec_rx_set_sample_slip(void *staticPtr, int rxSampleSlip);
    int ec_rx_get_static_size(void);
    int ec_rx_get_parameters_size(void);


    typedef struct {
        short HPFCoeffs[6];
        short nb_samples;
        short mode;
        short NLPP_gain;
        short NLPP_limit;  
    } EC_RX_PAR;

    typedef struct
    {  
      short    Xdelay[2];
      short		Ydelay[2];
      short    *A_coef; 
      short    *B_coef;
      short		nrsos;   // number second order section
      short	   order;
    } BIQUAD_STRUCT;
    
    typedef struct {
        short          mode; 
        BIQUAD_STRUCT  BIQUAD_FE; // contains filter struct for FE
        short          nb_samples;
        short          NLPP_fact;
        short          NLPP_limit;
        short          rx_sample_slip;
    } EC_RX_STATIC;

#endif

