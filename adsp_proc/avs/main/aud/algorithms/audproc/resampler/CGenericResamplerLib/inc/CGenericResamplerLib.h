#ifndef __GENERICRESAMPLER_H_
#define __GENERICRESAMPLER_H_
/*========================================================================

C G e n e r i c  R e s a m p l e r     L i b r a r y
C o m m o n   D e f i n i t i o n s

*//** @file CGenericResamplerLib.h
CGenericResamplerLib is a module that can change the playback rate of an
Audio File. The Module can Upsample or Downsample.

Copyright (c) 2008, 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/18/08   sg      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#ifdef RESAMPLER_UNIT_TEST
#include "qurt_elite_types.h"
#endif


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
//////////////////////////////////////////////////////////////////////////////
// ENUMERATIONS
//////////////////////////////////////////////////////////////////////////////
/**
* This type is used to identify the Generic Resampler Type.
* Fixed Input type or Fixed Output type
*/
typedef enum
{
    GEN_RESAMP_FIXED_INPUT,   ///< Resampler Type: Fixed Input
    GEN_RESAMP_FIXED_OUTPUT   ///< Resampler Type: Fixed Output
} ResampType;

/**
* This type is used to identify the Generic Resampler operating quality
* High Quality, Super Low Power and Low Power operating modes.
* Before changing the type of Resampler, the module has to be re initialized.
*/
typedef enum
{
    GEN_RESAMP_HQ,     ///< Resampler Quality: High Quality
    GEN_RESAMP_LP      ///< Resampler Quality: Low Power
} ResampQuality;

/**
* This enumeration contains return status values.
*/
enum ResampReturnType
{
    GEN_RESAMP_SUCCESS=0,
    GEN_RESAMP_FAILURE,
    GEN_RESAMP_NOMEMORY,
	GEN_RESAMP_INVALIDFS
};


/**
* This is the implementation of CGenericResamplerLib, a module which
* represents a Generic Resampler. This class performs Upsampling and Downsampling.
*/
class CGenericResamplerLib
{
    /*===========================================================================*
    *           MACROS, CONSTANTS, STRUCTURES, AND FUNCTION DECLARATIONS        *
    *           FOR UP-SAMPLER AND DOWNSAMPLER                                  *
    *===========================================================================*/
private:
    // fixed point math constants.
    static const uint16 Q15_SHIFT       = 15;    ///< Shift factor for converting to Q15
    static const uint16 Q18_SHIFT       = 18;    ///< Shift factor for converting to Q18
    static const int16  MAX_Q15         = 32767; ///< Maximum value of  signed Q15 number
    static const int16  MIN_Q15         = -32768; ///< Minimum value of  signed Q15 number
    static const uint32 MAX_FREQ        = 48000; ///< Maximum sample rate supported

    /// constant denoting 0.5 in Q18
    static const uint32   halfQ18       = (uint32)(1<<(Q18_SHIFT-1));
    /// constant denoting 1 in Q15
    static const uint16   oneQ15        = (uint16)(1<<Q15_SHIFT);
    /// constant denoting 0.5 in Q15
    static const uint16   halfQ15       = (uint16)(1<<(Q15_SHIFT-1));

    /*===========================================================================*
    *           MACROS, CONSTANTS, STRUCTURES FOR UP-SAMPLER                    *
    *===========================================================================*/
private:
    /// up-sampler rate
    static const uint16 UP_SAMP_RATE =  24;

    /// Number of filter coefficients for a HQ Upsampler
    static const uint16 HQ_m_usUpSampFiltLen = 720;

    /// High Quality Up-sampler poly phase filter length
    static const uint16 HQ_POLY_PHASE_LEN    =  (HQ_m_usUpSampFiltLen/UP_SAMP_RATE);

    /// Number of filter coefficients for a LP Upsampler
    static const uint16 LP_m_usUpSampFiltLen = 384;

    /// Low Power Up-sampler poly phase filter length
    static const uint16 LP_POLY_PHASE_LEN    =  (LP_m_usUpSampFiltLen/UP_SAMP_RATE);

    /// additional constants for number of filter coefficients for HQ upsampler
    static const uint16 HQ_m_usUpSampFiltLen_2 = (HQ_m_usUpSampFiltLen * 2);


    /// additional constants for number of filter coefficients for LP upsampler
    static const uint16 LP_m_usUpSampFiltLen_2 = (LP_m_usUpSampFiltLen * 2);


    /// Filter coefficients for High Quality Upsampler
#if ((defined __hexagon__) || (defined __qdsp6__))
        /// section label resampler_filter_tables is defined in elite.linker file. Will consider the possibility of keeping in a compile time macro to
    /// keep these changes generic.
    static const int16 HighQualitym_psUpSampCoef1Q15[HQ_m_usUpSampFiltLen_2] __attribute__((aligned(8)));
    static const int16 HighQualitym_psUpSampCoef2Q15[HQ_m_usUpSampFiltLen_2] __attribute__((aligned(8)));


    /// Filter coefficients for Low Power Upsampler
    static const int16 LowPowerm_psUpSampCoef1Q15[LP_m_usUpSampFiltLen_2] __attribute__((aligned(8))); /* coefficients for low cost */
    static const int16 LowPowerm_psUpSampCoef2Q15[LP_m_usUpSampFiltLen_2] __attribute__((aligned(8)));

#else

    static const int16 HighQualitym_psUpSampCoef1Q15[HQ_m_usUpSampFiltLen_2];
    static const int16 HighQualitym_psUpSampCoef2Q15[HQ_m_usUpSampFiltLen_2];

    /// Filter coefficients for Low Power Upsampler
    static const int16 LowPowerm_psUpSampCoef1Q15[LP_m_usUpSampFiltLen_2];/* coefficients for low cost */
    static const int16 LowPowerm_psUpSampCoef2Q15[LP_m_usUpSampFiltLen_2];

#endif
    //static int16 LowPowerm_psUpSampCoefmonoQ15[LP_m_usUpSampFiltLen_mono_total];/* coefficients for low cost */

    /// Length of up-sample filter memory for HQ filter
    //static const uint16 HQ_m_usUpSampFmemLen  =  HQ_POLY_PHASE_LEN;
    /// Number of bytes in circular buffer. Must be 2^n such that 2^(n-2) < HQ_m_usUpSampFmemLen <= 2^(n-1)
    static const uint16 HQ_UP_SAMP_FMEM_CIRC_LEN = ((HQ_POLY_PHASE_LEN + 3) / 4) * 4 * 2;

    /// Length of up-sample filter memory for LP filter
    //static const uint16 LP_m_usUpSampFmemLen  =  LP_POLY_PHASE_LEN;
    /// number of bytes in circular buffer. Must be power of (=2^n) such that 2^(n-2) < LP_m_usUpSampFmemLen <= 2^(n-1)
    static const uint16 LP_UP_SAMP_FMEM_CIRC_LEN = HQ_UP_SAMP_FMEM_CIRC_LEN;

    /// maximum possible up sample filter memory length, equal to high quality.
    static const uint16 MAX_m_usUpSampFmemLen = HQ_POLY_PHASE_LEN;
    /// Maximum possible memory allocated for HQ filter circular buffer
    static const uint16 MAX_UP_SAMP_CIRC_LEN = HQ_UP_SAMP_FMEM_CIRC_LEN;
    static const uint16 MAX_UP_SAMP_CIRC_LEN_4 = (HQ_UP_SAMP_FMEM_CIRC_LEN * 4);

    /// size of intermediate scratch memory
    static const uint16 SCRA_MEM_LEN =  512;

    /* structure for up-sampler */
    typedef struct {
        uint16  sampInc;
        uint16  samp;
        uint16  subSampInc;
        uint16  subSamp;
        int16   filtMemIndex;
        uint16  outFreq;     
        uint16  fracConst;
        uint16  shiftConst;
        int16   *filtCircMemLR;
        int16   *filtCircMem;
        int16   filtMemLR[MAX_UP_SAMP_CIRC_LEN_4];  /* filter memory for L and R channels with interleaved data */
        int16   filtMem[MAX_UP_SAMP_CIRC_LEN_4];

    } typeUpSampStruct;

    /*===========================================================================*
    *           MACROS, CONSTANTS, STRUCTURES FOR DOWN_SAMPLER                   *
    *===========================================================================*/
private:
    static const uint16 DOWN_SAMPLER_FILTER_SIZE = 698; // Byte size of filter

    static const uint16 DN_SAMP_FILT_LEN  = (DOWN_SAMPLER_FILTER_SIZE/sizeof(int16));
#if ((defined __hexagon__) || (defined __qdsp6__))
    static const uint16 DN_SAMP_FMEM_LEN  = ((((DN_SAMP_FILT_LEN+1)>>1)+3)&-4);
#else
    static const uint16 DN_SAMP_FMEM_LEN  = ((DN_SAMP_FILT_LEN+1)>>1);
#endif
    static const int16 dnSampCoefVec[DN_SAMP_FILT_LEN];
    static const int16 dnSampCoef48to8[DN_SAMP_FILT_LEN];
    static const int16 dnSampCoef48to16[DN_SAMP_FILT_LEN];
    static const int16 dnSampCoef16to8[DN_SAMP_FILT_LEN];

    typedef struct {
        typeUpSampStruct  upSampStruct;
        int16    maxInBufLen;
        int16    dsRatio;
        int16    dsFiltInc;
        int16    dnSampFiltIterations;
        int16    filtMemConvIndex;
        int16    filtMemLoadIndex;
#if ((defined __hexagon__) || (defined __qdsp6__))
        int16    filtMem[DN_SAMP_FMEM_LEN]  __attribute__((aligned(8))); /* filter memory for L/Mono channel */
#else
        int16    filtMem[DN_SAMP_FMEM_LEN]; /* filter memory for L/Mono channel */
#endif
        int16    filtMemR[DN_SAMP_FMEM_LEN]; /* filter memory for R channel */
        /* filter memory used for down-sample filter
        * in circular buffer form */
    } typeDnSampStruct;

    /**************************************************************************
    ** Constructors/destructor
    *************************************************************************/
public:
    CGenericResamplerLib();


    /**************************************************************************
    ** Public Methods
    *************************************************************************/
public:
    /**
    * Resmaples the input buffer to the desired output frequency. The
    * resampler stores the input and output freqeuncy internally this method
    * takes the input buffer and the input buffer size as input and returns
    * the resampled output buffer and the output buffer size.
    *
    * @param[in] unFixedInSamples  Input buffer number of samples;
    *            Ignored if the rsType=FIXED_OUTPUT
    * @param[in] unFixedOutSamples  Number of samples to generate;
    *            Ignored if the rsType=FIXED_INPUT
    * @param[out] punActualSamples   If rsType=FIXED_OUTPUT, the number of input samples
    *            actually consumed. If rsType=FIXED_INPUT, the number of output samples
    *            actually generated.
    * @param[in] psLeftIn   Pointer to L/Mono channel input samples.
    * @param[out] psLeftOut  Pointer to L/Mono channel output samples
    * @param[in] psRightIn  Pointer to R channel input samples (ignored if signal is mono)
    * @param[out] psRightOut  Pointer to R channel output samples (ignored if signal is mono)
    * @param[in] rsType  Fixed Input or Fixed Output type?
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType Resample(uint32 unFixedInSamples,
        uint32 unFixedOutSamples,
        uint32* punActualNumSamples,
        int16*  psLeftIn,
        int16*  psRightIn,
        int16*  psLeftOut,
        int16*  psRightOut,
        ResampType rsType);

    /**
    * Initializes the resampler by taking input and output frequency
    *
    * @param[in] InFreq- Input Frequency
    * @param[in] OutFreq- Output Frequency
    * @param[in] nChannels- number of channels
    * @param[in] rsMode- Resampler Mode High Quality, Super Low Power and
    *                    Low Power
    *
    * @param[out] None
    *
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType Initialize(uint32 InFreq,
        uint32 OutFreq,
        int16 nChannels,
        ResampQuality rsMode
        );

    /**
    * This function notifies if the internal data structure is initialized
    *
    * @param[in] None.
    *
    * @param[out] bInitialized- Flag to indicate if the resampler is
    *            successfully initialized.
    *
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType GetInitStatus( int32* bInitialized);

    /**
    * Get Maximum output size possible for the given number of inputs.
    *
    * @param[in] uiNrInpSamps- Given input number of samples
    * @param[in] uiInFreq- input frequency
    * @param[in] uiOutFreq- output frequency
    *
    * @param[out] None
    *
    * @return
    * - Number of predicted Maximum number of output samples
    */
    uint32 GetMaxOutputSize(const uint32 uiNrInpSamps,
        const uint32 uiInFreq,
        const uint32 uiOutFreq);

    /**
    * Get Maximum output size possible for the given number of inputs.
    *
    * @param[in] uiNrOutSamps- Desired output number of samples
    * @param[in] uiInFreq- input frequency
    * @param[in] uiOutFreq- output frequency
    *
    * @param[out] None
    *
    * @return
    * - Minimum number of input samples guaranteed to produce at least the
    * specified number of output samples
    */
    uint32 GetMinInputSize(const uint32 uiNrInpSamps,
        const uint32 uiInFreq,
        const uint32 uiOutFreq);

    /**
    * Get number of output samples that will be generated with the
    * given number of input samples.
    *
    * @param[in] uiNrInpSamps- Given input samples
    *
    * @param[out] retOutSize- number of output samples predicted
    *
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType GetInstOutputSize(const uint32 uiNrInpSamps,
        uint32* retOutSize);


    /**
    * Get number of input samples required to generate the given
    * number of output samples.
    *
    * @param[in] uiNrOutSamps- Given output samples
    *
    * @param[out] retInSize- number of output samples predicted
    *
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType GetInstInputSize (uint32 uiNrOutSamps,
        uint32* retInSize);

    /**
    * Get memory required for cascaded resampler(downsamplers)
    * size will be zero if cascaded resampler is not required or
    * memory for cascaded resampler is already allocated.	
    *		
    * @param[in] uiInFreq- input frequency
    *	
    * @param[in] uiOutFreq- output frequency
    *	
    * @param[out] retMemSize- memory size required
    *
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType GetMemoryRequired(uint32 uiInFreq,        
        uint32 uiOutFreq,		
        uint32* retMemSize);
	
    /**
    * Set memory required for cascaded resampler(downsamplers)
    *
    * @param[in] ptrMemory- pointer to memory
	*
    * @param[in] uiMemSize- size of the memory
    *
    * @return
    * - SUCCESS - The initialization was successful.
    * - error code - There was an error which needs to propagate.
    */
    ResampReturnType SetMemoryRequired (uint8* ptrMemory, uint32 uiMemSize);

    /**************************************************************************
    ** Private methods
    *************************************************************************/
private:
    // Common math functions
    int16  sat_Q15(int64 in, int16 satMax, int16 satMin);
    uint16 int_div(uint32 num, uint32 den, uint16* rem);
    int32  rs_mixing(uint16 subSamp, uint16 locFracConstQ15, uint16 shiftConst,
        int32 samp1LR, int32 samp2LR);
    uint32 frac_div(uint32 num, uint32 den, uint16 Qfac);

    /* Downsampler Functions */
    uint16 down_by_n(uint16  numInSamps, int16*  psLeftIn, int16*  psLeftOut,
        int16* psRightIn, int16* psRightOut, typeDnSampStruct *ptrDnSampStruct);

    /* uint16 down_by_n_mono(uint16  numInSamps, int16*  psLeftIn, int16*  psLeftOut,
    typeDnSampStruct *ptrDnSampStruct); */

    uint16   init_down_sampler(
        uint16  inFreq,
        uint16  outFreq,
        typeDnSampStruct *ptrDnSampStruct);

    uint16  down_sampler(
        uint16  numInSamps,
        int16*  psLeftIn,
        int16*  psLeftOut,
        int16*  psRightIn,
        int16*  psRightOut,
        typeDnSampStruct *ptrDnSampStruct,
        ResampType rsType);

    /*uint16  down_sampler_mono(
    uint16  numInSamps,
    int16*  psLeftIn,
    int16*  psLeftOut,
    typeDnSampStruct *ptrDnSampStruct,
    ResampType rsType); */

    /* Upsampler Functions */
    uint16  init_up_sampler(
        uint16  inFreq,
        uint16  outFreq,
        typeUpSampStruct *ptrUpSampStruct);

    uint16  up_sampler(
        uint16  numInSamps,
        int16*  psLeftIn,
        int16*  psLeftOut,
        int16*  psRightIn,
        int16*  psRightOut,
        typeUpSampStruct *ptrUpSampStruct,
        ResampType rsType);

#if ((defined __hexagon__) || (defined __qdsp6__))
    uint16  up_sampler_fixed_io(
        uint16  numInSamps,
        int16*  psLeftIn,
        int16*  psLeftOut,
        int16*  psRightIn,
        int16*  psRightOut,
        typeUpSampStruct *ptrUpSampStruct,
        int mode);
#endif


    /* uint16  up_sampler_mono(
    uint16  numInSamps,
    int16*  psLeftIn,
    int16*  psLeftOut,
    typeUpSampStruct *ptrUpSampStruct,
    ResampType rsType); */

    uint16 GetDownByNInputSize(const typeDnSampStruct *ptrDnSampStruct, const uint16 numOutSamps);

    void align_circular_buffer_pointers(
         typeDnSampStruct* ptrDSStruct);
	
    void convert_freq(uint32 inFreq32, uint32 outFreq32, uint16* pInFreq16, uint16* pOutFreq16);
    
    /**************************************************************************
    ** Data members
    *************************************************************************/
private:
    typeDnSampStruct m_DSStruct;     ///< Resampler state
    bool m_bUpSample;                ///< flags if upsampling is needed.
    bool m_bInitialized;             ///< flags whether resampler has been initialized.
    uint16 m_usChannels;             ///< number of channels in signal being resampled.

    bool m_usVoicepath;               ///<Optimization for voice specific code>
    ResampQuality m_resampQuality;   ///< high-quality or low-power mode.

    uint16 m_usUpSampFmemLen;        ///< Length of upsampler filter memory
    //uint16 m_usUpSampFmemLenmono;    ///< Length of upsampler filter memory for voice
    uint16 m_usUpSampFmemLen_2;      ///< Twice length of upsampler filter memory
    uint16 m_usUpSampFiltLen;        ///< Length of upsampler filter
    const int16* m_psUpSampCoef1Q15; ///< upsampler coefficients table1
    const int16* m_psUpSampCoef2Q15; ///< upsampler coefficients table2

    //int16 *m_psUpSampCoefmonoQ15;
    //int16 m_usUpSampFiltLenmono;

    int16  m_psScratchMem[SCRA_MEM_LEN]; ///< used to store intermediate output
    int16  m_psScratchMemR[SCRA_MEM_LEN]; ///< used to store intermediate output
    uint16 m_uiDownSampInputCarryOver;
    const int16 *m_piDownSampCoefPtr;
    int16 *m_piDownFilteDelayMem;

    //added for cascading of resamplers
    bool   m_bCascade;                ///< flags if cascading of resamplers is needed.
    int16* m_psScratchMemSecStage;    ///< used to store intermediate output of cascade resamplers
    int16* m_psScratchMemSecStageR;   ///< used to store intermediate output of cascade resamplers
    typeDnSampStruct* m_pDSStructSecStage;     ///< Resampler state for second stage in cascade resamplers.
};

#endif //__GENERICRESAMPLER_H_
