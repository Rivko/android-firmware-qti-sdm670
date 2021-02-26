/*
============================================================================

FILE:          rs_driver_ext_utils.c

DESCRIPTION:   Resampler HW driver extension file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================

============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/rs_driver_ext_utils.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

05/24/12   FZ      Created file

========================================================================== */

/*===========================================================================*
 *           INCLUDE HEADER FILES                                            *
 *===========================================================================*/

#include "qurt_elite.h"
#include "qurt.h"
#include  "resampler_32b_int.h"
#include  "resampler_32b_ext.h"

#include  "rs_driver.h"
#include  "rs_driver_ext_internal.h"

static int32 rs_drv_upsamp_calc_fixedin(void *pUS, int32 numInSamps);
static int32 rs_drv_upsamp_calc_fixedout(void *pUS, int32 numOutSamps);
static int32 rs_drv_dnsamp_calc_fixedin(void* pDS, int32 numInSamps);
static int32 rs_drv_down_by_n_calc_fixout(DSPhase *pDsPhase, int16 memLen, int32 numOutSamps);
static int32 rs_drv_dnsamp_calc_fixedout(void* pDS, int32 numOutSamps);

static ADSPResult rs_drv_update_us_phase(uint16 job_id, int32 up_samp_osr, USPhase* us_phase, struct_rs_dnsampler* rs_dnsampler);

/*===========================================================================*
 * MACROS, CONSTANTS, VARIABLES, AND FUNCTION DECLARATION FOR DOWN-SAMPLER   *
 *===========================================================================*/

/*===========================================================================*
 *           INTERNAL FUNCTION DECLARATIONS                                  *
 *===========================================================================*/

/*===========================================================================*
 *           CONSTANTS AND VARIABLES                                         *
 *===========================================================================*/

/*===========================================================================* 
 * FUNCTION NAME  : init_upsamp()                                            * 
 * DESCRIPTION    : Initialize up-sampler data structure.                    * 
 * INPUT ARGUMENTS:                                                          * 
 *     ptrUS      : Pointer to up-sampler data structure.                    * 
 *     param      : Pointer to parameter list.                               * 
 *     filtCoeff  : Pointer to filter coefficients                           *
 *     filtMem    : Pointer to filter memory                                 *
 * RETURN VALUE   :                                                          *  
 *     0          : If input frequency is less than outFreq / UP_SAMP_RATE_32*  
 *                : or input frequency is greater than output frequency      * 
 *                : or output frequency is greater than 48kHz                * 
 *     1          : If input frequency == output frequency.                  * 
 *     outFreq    : Otherwise                                                * 
 * COMMENTS       : The up-sample data structure will carry all information  * 
 *                : needed to do upsample conversion.  Output frequency is   * 
 *                : kept to serve as fractional phase cap.  Shift constant   * 
 *                : fractional constant are generated here.  Step sizes for  * 
 *                : integer phase and fractional phase are also generated    * 
 *===========================================================================*/
int32 rs_drv_init_upsamp(void* ptrUS, int32* param)
{
    int16 shift, totalPh;
    int32  temp32, inFreq, outFreq, tmp32;
    typeUpSampStruct* pUpsamp = ptrUS;
    USPhase *pPhase = &(pUpsamp->usPhase);

    inFreq  = param[IN_FREQ];
    outFreq = param[OUT_FREQ];
    totalPh = s16_extract_s32_l(param[US_OSR]);
    pPhase->outFreq     = outFreq;
    pPhase->inFreq      = inFreq;
    pUpsamp->is32Bit    = s16_extract_s32_l((param[MODE] & 1) + param[MODE_COEF_BIT]);    
    pPhase->totalIntPh  = s16_extract_s32_l(param[US_OSR]);

    if (((inFreq * totalPh) < outFreq) || (outFreq > MAX_FREQ))
    {
        return 0;
    }
    else if (inFreq  == outFreq)
    {
        if (0 == param[MODE_ASYNC])
        {
            pPhase->intPhStep  = -8000;
            pPhase->fracPhStep = 0;
            return 1;
        }
    }
    else if (inFreq > outFreq)
    {
        if (0 == param[MODE_ASYNC])
        {
            return 0;
        }
    }
  
    /*=======================================================================* 
     * This codes tries to break the step size into integer forms            * 
     * n(k) = fin / fout * k                                                 * 
     * n(k) = m(k) + [s(k) + t(k) / outFreq] / UP_SAMP_RATE_32               * 
     * where m(k) can be any non-negative integer, it is used as sample ptr  * 
     * s(k) is an integer, and 0 <= s(k) < UP_SAMP_RATE_32.  It is used to   * 
     * select polyphase filter                                               * 
     * t(k) is an integer, and 0 <= t(k) < outFreq. It's used for linear     * 
     * interpolation                                                         * 
     *=======================================================================*/
  
    /* the product will not exceed 32 bit if max fs is 384 kHz */
    temp32 = s32_extract_s64_l(s64_mult_s32_s16(inFreq, pPhase->totalIntPh));
    temp32 = int_div_32(temp32, outFreq, &(pPhase->fracPhStep));
    pPhase->intPhStep = s16_extract_s32_l(temp32);

    /* compute fractional phase constant and corresponding shift constant */
    temp32 = s32_shl_s32_sat((int32)1, Q20_SHIFT);
    shift  = Q20_SHIFT;
    while (temp32 >= outFreq)
    {
        temp32 = s32_shl_s32_sat(temp32, -1);
        shift--;
    } /* while (temp > outFreq) */
    pPhase->shiftConst = shift;

    tmp32 = frac_div_32(temp32, outFreq, Q23_SHIFT);
    pPhase->fracConst32  = tmp32;

    tmp32 = frac_div_32(temp32, outFreq, Q15_SHIFT);
    pPhase->fracConst  = s16_extract_s32_l(tmp32);

    /* Matching phase */
    if (pPhase->fracPhStep != 0)
    {
        pPhase->curIntPh = s16_sub_s16_s16(pPhase->totalIntPh, 1);
    }
    else
    {
        pPhase->curIntPh = s16_sub_s16_s16(pPhase->totalIntPh, 2);
    }

    pPhase->curFracPh = 0;

    return outFreq;
} /* init_upsamp */




/*===========================================================================* 
 * FUNCTION NAME  : upsamp_calc_fixedin                                      * 
 * DESCRIPTION    : Compute up-sampler output samples in fixed-input mode    *
 * INPUT ARGUMENTS:                                                          * 
 *     pUS        : Data structure of up-sampler                             * 
 *     numInSamps : Number of input samples, 32 bit unsigned                 * 
 * RETURN VALUE   : Prediction of number of output samples.                  * 
 * SIDE EFFECT    : No side effect.                                          *
 * DEPENDENCIES   : The up-sampler data structure needs to be valid.         * 
 * COMMENTS       : To accurately compute number of output sample based on   * 
 *                : number of input samples, all phase information is needed.*
 *                : This function serves as example.  Any variant without    *
 *                : using existing phase information will not guarantee to   *
 *                : give accurate results.                                   *
 *===========================================================================*/
int32 rs_drv_upsamp_calc_fixedin(void *pUS, int32 numInSamps)
{
    int16 temp16;
    int32 quot, numOutPredict;
    int32 start, step, temp32;
    int64 total, temp64;
    typeUpSampStruct *pUpsamp  = pUS;
    USPhase          *pUsPhase = &(pUpsamp->usPhase);

    /* predict number of output samples */
    if (0 > pUsPhase->intPhStep)
    {
        numOutPredict = numInSamps;
    }
    else if (0 == numInSamps)
    {
        numOutPredict = 0;
    }
    else
    {
        temp32 = s32_extract_s64_l(s64_mult_s32_s16(pUsPhase->outFreq, 
                                                    pUsPhase->curIntPh));
        start  = s32_add_s32_s32(pUsPhase->curFracPh, temp32);

        temp32 = s32_extract_s64_l(s64_mult_s32_s16(pUsPhase->outFreq, 
                                                    pUsPhase->totalIntPh));
        temp64 = s64_mult_s32_s32(temp32, numInSamps);
        total  = s64_sub_s64_s64(temp64, start);   

        temp16 = pUsPhase->intPhStep;
        temp32 = s32_extract_s64_l
                 (
                    s64_mult_s32_s16(pUsPhase->outFreq, temp16)
                 );
        step   = s32_add_s32_s32(pUsPhase->fracPhStep, temp32);

        /* integer division, only integer quotient is needed, ignore residue */
        quot   = s32_extract_s64_l(int_div_64(total, (int64)step, &temp64));  
        temp64 = s64_mult_s32_s32(quot, step);
        if (0 == s64_sub_s64_s64(total, temp64))
        {
            numOutPredict = quot;
        }
        else if ((total - temp64 + pUsPhase->outFreq*pUsPhase->totalIntPh) <= step)
        {
            numOutPredict = quot;
        }
        else
        {
            numOutPredict = s32_add_s32_s32(quot, 1);
        }
    }
    return numOutPredict;
}

/*===========================================================================* 
 * FUNCTION NAME  : upsamp_calc_fixedout                                     * 
 * DESCRIPTION    : Compute up-sampler output samples for fixed output mode  * 
 * INPUT ARGUMENTS:                                                          * 
 *     pUS        : Data structure of up-sampler                             * 
 *     numOutSamps: Number of output samples, 32 bit unsigned                * 
 * RETURN VALUE   : Prediction of number of output samples.                  * 
 * SIDE EFFECT    : No side effect.                                          *
 * DEPENDENCIES   : The up-sampler data structure needs to be valid.         * 
 * COMMENTS       : To accurately compute number of output sample based on   * 
 *                : number of input samples, all phase information is needed.*
 *                : This function serves as example for sample computaton.   *
 *                : Any variant without using existing phase information will*
 *                : not guarantee to give accurate results.                  *
 *===========================================================================*/
int32 rs_drv_upsamp_calc_fixedout(void *pUS, int32 numOutSamps)
{
    int16 temp16;
    int32 numInPredict, start, step, temp32;
    int64 total, temp64;
    typeUpSampStruct *pUpsamp = pUS;
    USPhase *pUsPhase = &(pUpsamp->usPhase);

    /* predict number of output samples */
    if (0 > pUsPhase->intPhStep)
    {
        numInPredict = numOutSamps;
    }
    else if (0 == numOutSamps)
    {
        numInPredict = 0;
    }
    else
    {

        temp32 = s32_extract_s64_l
                 (
                    s64_mult_s32_s16(pUsPhase->outFreq, pUsPhase->curIntPh)
                 );
        start  = s32_add_s32_s32(pUsPhase->curFracPh, temp32);

        temp16 = pUsPhase->intPhStep;
        temp32 = s32_extract_s64_l
                 (
                    s64_mult_s32_s16(pUsPhase->outFreq, temp16)
                 );
        temp32 = s32_add_s32_s32(temp32, pUsPhase->fracPhStep);
        temp64 = s64_mult_s32_s32(temp32, numOutSamps);
        total  = s64_add_s64_s64(temp64, start);   

        step   = s32_extract_s64_l
                 (
                    s64_mult_s32_s16(pUsPhase->outFreq, pUsPhase->totalIntPh)
                 );

        /* integer division, only integer quotient is needed, ignore residue */
        numInPredict = s32_extract_s64_l(int_div_64(total, (int64)step, &temp64));
    }
    return numInPredict;
}

/*===========================================================================* 
 * FUNCTION NAME  : init_dnsamp()                                            * 
 * DESCRIPTION    : Initialize down-sampler data structure.                  * 
 * INPUT ARGUMENTS:                                                          * 
 *     ptrDS      : Pointer to down-sampler data structure                   *
 *     param      : parameter list                                           * 
 *     pUsCoef    : Pointer to up-sampler filter coefficients                *
 *     pUsMem     : Pointer to up-sampler filter memory                      *
 *     pDsCoef    : Pointer to up-sampler filter coefficients                *
 *     pDsMem     : Pointer to up-sampler filter memory                      *
 *     pScrMem    : Pointer to intermediate buffer                           *
 * RETURN VALUE   :                                                          * 
 *     -2         : Intermediate sampling rate is greater than MAX_FREQ.     *
 *     -1         : If inFreq > MAX_FREQ or inFreq < outFreq or the down-    *
 *                : sampling rate is not suppoted                            * 
 *     0          : Fail to initialize up-sampler.                           *
 *     1          : outFreq == inFreq;                                       * 
 *     medFreq    : Otherwise.                                               * 
 * SIEDE EFFECT   : Anything inside the output buffer will be overwitten.    * 
 *                : The phase information inside the up-sample structure will* 
 *                : be fully updated.                                        * 
 * DEPENDENCIES   : The caller needs to make sure the output buffer is big   * 
 *                : enough.                                                  * 
 * COMMENTS       : The down-sample data structure will carry all information* 
 *                : needed to do down-sample conversion.  Absolute I/O       * 
 *                : frequency will not be kept.  Only relative information   * 
 *                : are kept                                                 * 
 *===========================================================================*/
int32 rs_drv_init_dnsamp(void* pStruct, int32* param)
{
    int16 mode, osr, temp16;
    int32  medFreq, temp32, inFreq, outFreq;
    typeDnSampStruct *pDnsamp = (typeDnSampStruct*)pStruct;
    DSPhase *pDsPhase = &(pDnsamp->dnMem.dsPhase);

    mode    = s16_extract_s32_l(param[MODE]);
    inFreq  = param[IN_FREQ];
    outFreq = param[OUT_FREQ];
    pDnsamp->dnMem.is32Bit     = s16_extract_s32_l((mode & 1) + param[MODE_COEF_BIT]);

    osr = s16_extract_s32_l(param[DS_OSR]);

    if ((inFreq > MAX_FREQ) || 
        (inFreq < outFreq)  ||
        (inFreq > outFreq * 24))
    {
        return -1;/* not supported */
    }
    else if (inFreq == outFreq)
    {
        pDsPhase->dnSampFac = 0;
        return 1;/* no need to convert */
    } /* if ((inFreq > MAX_FREQ) || ... */

    /* determine actually up-sampling and down-sampling rate */
    if ((inFreq <= outFreq * 2) && (outFreq * 2 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 2;
    }
    else if ((inFreq <= outFreq * 3) && (outFreq * 3 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 3;
    }
    else if ((inFreq <= outFreq * 4) && (outFreq * 4 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 4;
    }
    else if ((inFreq <= outFreq * 6) && (outFreq * 6 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 6;
    }
    else if ((inFreq <= outFreq * 8) && (outFreq * 8 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 8;
    }
    else if ((inFreq <= outFreq * 12) && (outFreq * 12 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 12;
    }
    else if ((inFreq <= outFreq * 24) && (outFreq * 24 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 24;
    }
    else 
    {
        return -2;
    }/*if ((inFreq <= outFreq * 2) && ...*/

    pDsPhase->dnSampStep    = int_div_16(osr, pDsPhase->dnSampFac, &temp16);
    medFreq = s32_extract_s64_l(s64_mult_s32_s16(outFreq, pDsPhase->dnSampFac));

    param[OUT_FREQ] = medFreq;
    temp32 = rs_drv_init_upsamp(&(pDnsamp->upSamp), param);
    param[OUT_FREQ] = outFreq;

    if (temp32 == 0)
    {
        return 0; /* initialization failed. */
    }/* if (temp == 0) */

    pDsPhase->inFreq     = inFreq;
    pDsPhase->outFreq    = outFreq;
    pDsPhase->medFreq    = medFreq;

    return medFreq;
} /* init_dnsamp */

/*===========================================================================* 
 * FUNCTION NAME  : dnsamp_calc_fixedin                                      * 
 * DESCRIPTION    : Compute dn-sampler output samples for fixed input mode   * 
 * INPUT ARGUMENTS:                                                          * 
 *     pDS        : Pointer to down-sampler data structure.                  * 
 *     numInSamps : Number of input samples, 32 bit                          * 
 * RETURN VALUE   : Prediction of number of output samples.                  * 
 * SIDE EFFECT    : No side effect.                                          *
 * DEPENDENCIES   : The dn-sampler data structure needs to be valid.         * 
 * COMMENTS       : None                                                     * 
 *===========================================================================*/
int32 rs_drv_dnsamp_calc_fixedin(void* pDS, int32 numInSamps)   
{
    int16 memLen;
    int32 numOutPredict;
    int32 temp32, start, total, dummy;
    typeDnSampStruct *pDnsamp = pDS;
    DSPhase *pDsPh = &(pDnsamp->dnMem.dsPhase);
    USPhase *pUsPh = &(pDnsamp->upSamp.usPhase);

    if (0 == numInSamps)
    {
        return 0;
    }

    /* compute number output samples from up-sampler */
   temp32 = rs_drv_upsamp_calc_fixedin(pUsPh, numInSamps);
   if (0 == pDsPh->dnSampFac)
   {
      return temp32;
   }

    memLen = pDnsamp->dnMem.dsFilt.memLen;

    /* compute how many sample is needed to produce first output samples */
    start = s32_sub_s32_s32(pDsPh->convIndex, pDsPh->loadIndex);
    if (start < 0)
    {
        start = s32_add_s32_s32(start, memLen);
    }

    /* difference between avaiable samples and needed samples */
    total = s32_sub_s32_s32(temp32, start);

    /* integer division, only integer quotient is needed, residue is ignored */
	if ((total >= 0) && (0 == pDsPh->dnSampFac))
	{
		numOutPredict = total;
	}
    else if ((total >= 0) && (pDsPh->dnSampFac > 0))
    {
        temp32 = int_div_32(total, pDsPh->dnSampFac, &dummy);
        numOutPredict = s32_add_s32_s32(1, temp32); 
    }
    else
    {
        numOutPredict = 0; 
    }

    return numOutPredict;
}

/*===========================================================================* 
 * FUNCTION NAME  : down_by_n_calc_fixout                                    * 
 * DESCRIPTION    : Compute down by n output sample for fixed output mode    * 
 * INPUT ARGUMENTS:                                                          * 
 *     DSPhase    : Pointer to down-sampler phase data structure             * 
 *     memLen     : length of filter memory.                                 * 
 *     numOutSamps: Number of output samples, 32 bit                         * 
 * RETURN VALUE   : Prediction of number of input samples needed.            * 
 * SIDE EFFECT    : No side effect.                                          *
 * DEPENDENCIES   : The dn-sampler data structure needs to be valid.         * 
 * COMMENTS       : None                                                     * 
 *===========================================================================*/
int32 rs_drv_down_by_n_calc_fixout(DSPhase *pDsPhase, int16 memLen, int32 numOutSamps)
{
    int16 start;
    int32 temp32;

    if (0 == pDsPhase->dnSampFac)
       return numOutSamps; // just copy

    /* compute total number of intermediate sample needed */
    start = s16_sub_s16_s16(pDsPhase->convIndex, pDsPhase->loadIndex);
    if (start < 0)
    {
        start = s16_add_s16_s16(start, memLen);
    }

    if (0 == start)
    {
        temp32 = s32_extract_s64_l(s64_mult_s32_s32(numOutSamps, pDsPhase->dnSampFac));
    }
    else
    {
        temp32 = s32_sub_s32_s32(numOutSamps, 1);
        temp32 = s32_extract_s64_l(s64_mult_s32_s32(temp32, pDsPhase->dnSampFac));
    }
    return s32_add_s32_s32(temp32, start);
}

/*===========================================================================* 
 * FUNCTION NAME  : dnsamp_calc_fixedout                                     * 
 * DESCRIPTION    : Compute dn-sampler output samples for fixed output mode  * 
 * INPUT ARGUMENTS:                                                          * 
 *     pDS        : Pointer to down-sampler data structure.                  * 
 *     numInSamps : Number of input samples, 32 bit                          * 
 * RETURN VALUE   : Prediction of number of output samples.                  * 
 * SIDE EFFECT    : No side effect.                                          *
 * DEPENDENCIES   : The dn-sampler data structure needs to be valid.         * 
 * COMMENTS       : None                                                     * 
 *===========================================================================*/
int32 rs_drv_dnsamp_calc_fixedout(void* pDS, int32 numOutSamps)
{
    int16 memLen;
    int32 numInPredict, numMidPredict;
    typeDnSampStruct *pDnsamp = pDS;
    DSPhase *pDsPh = &(pDnsamp->dnMem.dsPhase);

    if ((0 == pDsPh->dnSampFac) && (0 == pDnsamp->dnMem.isDynamic))
    {
        return numOutSamps;
    }
    else if (0 == numOutSamps)
    {
        return 0;
    }

    memLen = pDnsamp->dnMem.dsFilt.memLen;
    numMidPredict = rs_drv_down_by_n_calc_fixout(pDsPh, memLen, numOutSamps);

    /* compute number of input sample needed for up-sampler */
    numInPredict  = rs_drv_upsamp_calc_fixedout(&(pDnsamp->upSamp), numMidPredict);

    return numInPredict;
}


//upsampler, fixed output
int32 rs_drv_estimate_upsampler_input_samples(int32 numOutSamps, uint16 job_id, int32 upSampOsr, rs_drv_dynamic_resampler_type dynamic_rs)
{    
   ADSPResult rc = ADSP_EOK;;
   int32 numInPredict = 0;
   typeUpSampStruct Upsamp;
   struct_rs_dnsampler rs_dnsampler;
   int32 unProcessedSamples = 0;

   memset((void*) &Upsamp, 0, sizeof(typeUpSampStruct));
   memset((void*) &rs_dnsampler, 0, sizeof(struct_rs_dnsampler));

   if(ADSP_EOK == (rc = rs_drv_update_us_phase(job_id, upSampOsr, &Upsamp.usPhase, &rs_dnsampler)))
   {
      numInPredict = rs_drv_upsamp_calc_fixedout(&Upsamp,numOutSamps);

      // update number of input samples, based on unused samples present in the upsampler filter
      unProcessedSamples = rs_dnsampler.resampler_flt_iptr - rs_dnsampler.up_sampler_flt_fptr - 1;

      if (unProcessedSamples < 0)
      {
        unProcessedSamples += rs_dnsampler.up_samp_struct.up_filt_mem_size;
      }
      // Number of new samples actually required
      numInPredict -= unProcessedSamples;
      if (numInPredict < 0) numInPredict = 0;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_estimate_upsampler_input_samples rs_drv_update_us_phase failed\n");
   }

   return numInPredict;
}

//down sampler, fixed output
int32 rs_drv_estimate_dnsampler_input_samples(int32 numOutSamps, uint16 job_id, int32 upSampOsr, int32 operation, rs_drv_dynamic_resampler_type dynamic_rs)
{
   int32 numInPredict = 0;
   int32 unProcessedSamples = 0;
   ADSPResult rc = ADSP_EOK;;
   struct_rs_dnsampler rs_dnsampler;
   typeDnSampStruct Dnsamp;
   memset((void*) &Dnsamp, 0, sizeof(typeUpSampStruct));
   memset((void*) &rs_dnsampler, 0, sizeof(struct_rs_dnsampler));

   if(ADSP_EOK == (rc = rs_drv_update_us_phase(job_id, upSampOsr, &Dnsamp.upSamp.usPhase, &rs_dnsampler)))
   {
      if (RS_OPERATION_RESAMPLING == operation)
      {
         Dnsamp.upSamp.isDynamic = (int16)dynamic_rs;
         Dnsamp.dnMem.isDynamic = (int16)dynamic_rs;

         if ( dynamic_rs && (upSampOsr == Dnsamp.upSamp.usPhase.intPhStep) )
         {
            Dnsamp.upSamp.usPhase.intPhStep = -8000;
         }

         Dnsamp.dnMem.dsPhase.dnSampFac = rs_dnsampler.ds_factor;       
         Dnsamp.dnMem.dsFilt.memLen = rs_dnsampler.dn_filt_mem_size;   
         Dnsamp.dnMem.dsPhase.loadIndex = rs_dnsampler.dn_sampler_flt_fptr;              
         Dnsamp.dnMem.dsPhase.convIndex = (Dnsamp.dnMem.dsPhase.loadIndex + Dnsamp.dnMem.dsPhase.dnSampFac) % Dnsamp.dnMem.dsFilt.memLen;

         if (dynamic_rs && (1 == Dnsamp.dnMem.dsPhase.dnSampFac))
         {
            // Algorithm uses 0 to indicate copy for downsampling.
            Dnsamp.dnMem.dsPhase.dnSampFac = 0;
         }
         else
         {

            if (rs_dnsampler.up_sampler_flt_optr != ((rs_dnsampler.dn_sampler_flt_fptr + 1) % rs_dnsampler.dn_filt_mem_size) )
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_estimate_dnsampler_input_samples Extra samples in dnsampler filter memory during resampling\n");
               return ADSP_EFAILED;
            }
         }
        
         numInPredict = rs_drv_dnsamp_calc_fixedout(&Dnsamp,numOutSamps);

         if ( dynamic_rs && (upSampOsr == rs_dnsampler.up_samp_struct.int_phase_step_size) )
         {
            unProcessedSamples = 0;
         }
         else
         {
            // update number of input samples, based on unused samples present in the upsampler filter.
            unProcessedSamples = rs_dnsampler.resampler_flt_iptr - rs_dnsampler.up_sampler_flt_fptr - 1;

            if (unProcessedSamples < 0)
            {
               unProcessedSamples += rs_dnsampler.up_samp_struct.up_filt_mem_size;
            }
         }
         // Number of new samples actually required
         numInPredict -= unProcessedSamples;

         if (numInPredict < 0) numInPredict = 0;
      }
      else
      {
         //down sampler only
         // Number of input samples already in filter
         unProcessedSamples = (int32)(rs_dnsampler.resampler_flt_iptr - rs_dnsampler.dn_sampler_flt_fptr -1);

         if (unProcessedSamples < 0)
         {
            unProcessedSamples = unProcessedSamples + rs_dnsampler.dn_filt_mem_size;
         }
         // Call Csim estimation function
         Dnsamp.dnMem.dsPhase.dnSampFac = rs_dnsampler.ds_factor;                 
         Dnsamp.dnMem.dsFilt.memLen = rs_dnsampler.dn_filt_mem_size;               
         Dnsamp.dnMem.dsPhase.loadIndex = rs_dnsampler.dn_sampler_flt_fptr;       

         Dnsamp.dnMem.dsPhase.convIndex = (Dnsamp.dnMem.dsPhase.loadIndex + Dnsamp.dnMem.dsPhase.dnSampFac) % Dnsamp.dnMem.dsFilt.memLen;

         Dnsamp.upSamp.usPhase.intPhStep = -8000;
         numInPredict = rs_drv_dnsamp_calc_fixedout(&Dnsamp,numOutSamps);
         // total number of new samples required.
         numInPredict -= unProcessedSamples;

         if (numInPredict < 0) numInPredict = 0;
       }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_estimate_dnsampler_input_samples rs_drv_update_us_phase failed\n");
   }

   return numInPredict;
}


//upsampler fixed input
int32 rs_drv_estimate_upsampler_output_samples(int32 numInSamps, uint16 job_id, int32 upSampOsr, rs_drv_dynamic_resampler_type dynamic_rs)
{
   int32 numOutPredict = 0;
   int32 unProcessedSamples = 0;

   ADSPResult rc = ADSP_EOK;;
   struct_rs_dnsampler rs_dnsampler;
   typeUpSampStruct Upsamp;
   memset((void*) &Upsamp, 0, sizeof(typeUpSampStruct));
   memset((void*) &rs_dnsampler, 0, sizeof(struct_rs_dnsampler));

   if(ADSP_EOK == (rc = rs_drv_update_us_phase(job_id, upSampOsr, &Upsamp.usPhase, &rs_dnsampler)))
   {
      // update number of input samples, based on unused samples present in the upsampler filter.
      unProcessedSamples = rs_dnsampler.resampler_flt_iptr - rs_dnsampler.up_sampler_flt_fptr - 1;

      if (unProcessedSamples < 0)
      {
         unProcessedSamples += rs_dnsampler.up_samp_struct.up_filt_mem_size;
      }

      numInSamps += unProcessedSamples;

      numOutPredict = rs_drv_upsamp_calc_fixedin(&Upsamp.usPhase,numInSamps);
   }

   return numOutPredict;
}

//downsampler fixed input
int32 rs_drv_estimate_dnsampler_output_samples(int32 numInSamps, uint16 job_id, int32 upSampOsr, int32 operation, rs_drv_dynamic_resampler_type dynamic_rs)
{
   int32 numOutPredict = 0;
   int32 unProcessedSamples = 0;
   ADSPResult rc = ADSP_EOK;;
   struct_rs_dnsampler rs_dnsampler;
   typeDnSampStruct Dnsamp;
   memset((void*) &Dnsamp, 0, sizeof(typeUpSampStruct));
   memset((void*) &rs_dnsampler, 0, sizeof(struct_rs_dnsampler));

   if(ADSP_EOK == (rc = rs_drv_update_us_phase(job_id, upSampOsr, &Dnsamp.upSamp.usPhase, &rs_dnsampler)))
   {
      if (RS_OPERATION_RESAMPLING == operation)
      {
         Dnsamp.upSamp.isDynamic = (int16)dynamic_rs;
         Dnsamp.dnMem.isDynamic = (int16)dynamic_rs;

         if ( dynamic_rs && (upSampOsr == Dnsamp.upSamp.usPhase.intPhStep) )
         {
            Dnsamp.upSamp.usPhase.intPhStep = -8000;
         }

         Dnsamp.dnMem.dsPhase.dnSampFac = rs_dnsampler.ds_factor;                                          
         Dnsamp.dnMem.dsFilt.memLen = rs_dnsampler.dn_filt_mem_size;                                       
         Dnsamp.dnMem.dsPhase.loadIndex = rs_dnsampler.dn_sampler_flt_fptr;                                

         Dnsamp.dnMem.dsPhase.convIndex = (Dnsamp.dnMem.dsPhase.loadIndex + Dnsamp.dnMem.dsPhase.dnSampFac) % Dnsamp.dnMem.dsFilt.memLen;

         if ( dynamic_rs && (upSampOsr == rs_dnsampler.up_samp_struct.int_phase_step_size))
         {
            unProcessedSamples = 0;
         }
         else
         {

            // update number of input samples, based on unused samples present in the upsampler filter.
            unProcessedSamples = rs_dnsampler.resampler_flt_iptr - rs_dnsampler.up_sampler_flt_fptr - 1;

            if (unProcessedSamples < 0)
            {
               unProcessedSamples += rs_dnsampler.up_samp_struct.up_filt_mem_size;                
            }
         }

         if (dynamic_rs && (1 == Dnsamp.dnMem.dsPhase.dnSampFac))
         {
            // Algorithm uses 0 to indicate copy for downsampling.
            Dnsamp.dnMem.dsPhase.dnSampFac = 0;
         }
         else
         {

            if (rs_dnsampler.up_sampler_flt_optr != ((rs_dnsampler.dn_sampler_flt_fptr + 1) % rs_dnsampler.dn_filt_mem_size) )
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_estimate_dnsampler_output_samples Extra samples in dnsampler filter memory during resampling\n");
               //return ERROR_MISC;
               return ADSP_EFAILED;   
            }
         }
           
         numInSamps += unProcessedSamples;
         numOutPredict = rs_drv_dnsamp_calc_fixedin(&Dnsamp,numInSamps);
      }
      else
      {
         // Number of input samples already in filter
         unProcessedSamples = (int32)(rs_dnsampler.resampler_flt_iptr - rs_dnsampler.dn_sampler_flt_fptr -1);
         if (unProcessedSamples < 0)
         {
            unProcessedSamples = unProcessedSamples + rs_dnsampler.dn_filt_mem_size;   
         }
         // Call Csim estimation function
         Dnsamp.dnMem.dsPhase.dnSampFac = rs_dnsampler.ds_factor;               
         Dnsamp.dnMem.dsFilt.memLen = rs_dnsampler.dn_filt_mem_size;            
         Dnsamp.dnMem.dsPhase.loadIndex = rs_dnsampler.dn_sampler_flt_fptr;      
         Dnsamp.dnMem.dsPhase.convIndex = (Dnsamp.dnMem.dsPhase.loadIndex + Dnsamp.dnMem.dsPhase.dnSampFac) % Dnsamp.dnMem.dsFilt.memLen;
         Dnsamp.upSamp.usPhase.intPhStep = -8000;
         // Total number of input samples
         numInSamps += unProcessedSamples;
         numOutPredict = rs_drv_dnsamp_calc_fixedin(&Dnsamp,numInSamps);
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_estimate_dnsampler_output_samples rs_drv_update_us_phase failed\n");
   }

   return numOutPredict;
}

ADSPResult rs_drv_config_job(rs_drv_job_cfg_t* job_cfg_ptr, 
                             struct_rs_job_config* job_config_struct_ptr, 
                             struct_rs_dnsampler* dn_samp_hw_ptr,
                             typeUpSampStruct* up_samp_ptr,
                             typeDnSampStruct* dn_samp_ptr)
{
   ADSPResult ret = ADSP_EFAILED;

   if((job_cfg_ptr) && (job_config_struct_ptr) && (dn_samp_hw_ptr))
   {
      int32 param[DYNAMIC_DS_DELAY + 1];
      int32  in_freq = job_cfg_ptr->in_freq;
      int32  out_freq = job_cfg_ptr->out_freq;
      uint16  num_channels = job_cfg_ptr->num_channels;                      
      uint16  samp_bit_width = job_cfg_ptr->samp_bit_width;
      rs_drv_interp_type interp_mode = job_cfg_ptr->interp_mode;
      rs_drv_force_interpol_type force_interpol = job_cfg_ptr->force_interpol;
      rs_drv_dynamic_resampler_type dynamic_rs = job_cfg_ptr->dynamic_resampler;
      uint16 internal_buf_needed = job_cfg_ptr->internal_buf_needed;                
      uint16 input_num_sample_max = job_cfg_ptr->input_num_sample_max;
      uint16 osr = 160;
      rs_operation_type op_mode;
      int32 ret_val;

      if((up_samp_ptr) && (dn_samp_ptr))
      {
         memset((void*) &param[0], 0, DYNAMIC_DS_DELAY * sizeof(int32));

         if(RS_DRV_UPSAMPLER_FILTER_ID_1344 == job_cfg_ptr->upsamp_filt_id)  
         {
            osr = 24;
         }

         param[MODE] = 0;  // do not care
         param[IN_FREQ] = in_freq;    // input frequency
         param[OUT_FREQ] = out_freq;  // output frequency
         param[US_OSR] = osr; // upsampler OSR
         param[US_FLEN] = 0;  //upsampler filter length in words
         param[US_MEM_LEN] = 0; // upsampler filter memory size in words
         param[US_QFAC] = 0; // upsampler filter coefficient q-factor
         param[DS_FLEN] = 0;  // Downsampler filter length in words
         param[DS_MEM_LEN] = 0;  // Downsampler memory size in words
         param[DS_QFAC] = 0;      // downsampler filter coefficient q-factor
         param[DS_SCR_MLEN] = 0;  // scratch buffer len (in words)
         param[DS_OSR] = 24;       // Downsampler OSR
         param[US_SYMMETRIC] = 0;  // symmetry of upsampler filter
         param[DS_SYMMETRIC] = 0;  // symmetry of downsampler filter
         param[MODE_COEF_BIT] = 2; //do not care
         param[MODE_ASYNC] = force_interpol;
         param[MODE_DYNAMIC] = dynamic_rs;

         if(RS_DRV_DYNAMIC_RESAMPLER_ON == dynamic_rs)
         {
            param[DYNAMIC_US_DELAY] = DYNAMIC_UP_EXTRA_DELAY;   
            param[DYNAMIC_DS_DELAY] = DYNAMIC_DN_EXTRA_DELAY;   

            //call init_dynamic_resamp()
            ret_val = rs_drv_init_dynamic_resamp((void*) dn_samp_ptr, &param[0]);

            dn_samp_hw_ptr->up_samp_struct.int_phase_step_size = dn_samp_ptr->upSamp.usPhase.intPhStep;
            dn_samp_hw_ptr->up_samp_struct.cur_int_phase = dn_samp_ptr->upSamp.usPhase.curIntPh;
            dn_samp_hw_ptr->up_samp_struct.frac_phase_step_size = dn_samp_ptr->upSamp.usPhase.fracPhStep;
            dn_samp_hw_ptr->up_samp_struct.cur_frac_phase = dn_samp_ptr->upSamp.usPhase.curFracPh;
            dn_samp_hw_ptr->up_samp_struct.out_freq = dn_samp_ptr->upSamp.usPhase.outFreq;
            dn_samp_hw_ptr->up_samp_struct.frac_const = (RS_DRV_INTERP_MODE_LINEAR == interp_mode) ? dn_samp_ptr->upSamp.usPhase.fracConst32 : dn_samp_ptr->upSamp.usPhase.fracConst;
            dn_samp_hw_ptr->up_samp_struct.shift_const = dn_samp_ptr->upSamp.usPhase.shiftConst;

            dn_samp_hw_ptr->ds_factor = dn_samp_ptr->dnMem.dsPhase.dnSampFac;
            dn_samp_hw_ptr->ds_step = dn_samp_ptr->dnMem.dsPhase.dnSampStep;

            op_mode = RS_OPERATION_RESAMPLING;
         }
         else
         {
            param[DYNAMIC_US_DELAY] = 0;
            param[DYNAMIC_DS_DELAY] = 0;

            if (in_freq <= out_freq)  //upsampler
            {
               if(0 != (ret_val = rs_drv_init_upsamp((void*) up_samp_ptr, &param[0])))
               {
                  op_mode = RS_OPERATION_UPSAMPLING;
                  dn_samp_hw_ptr->up_samp_struct.int_phase_step_size = up_samp_ptr->usPhase.intPhStep;
                  dn_samp_hw_ptr->up_samp_struct.cur_int_phase = up_samp_ptr->usPhase.curIntPh;
                  dn_samp_hw_ptr->up_samp_struct.frac_phase_step_size = up_samp_ptr->usPhase.fracPhStep;
                  dn_samp_hw_ptr->up_samp_struct.cur_frac_phase = up_samp_ptr->usPhase.curFracPh;
                  dn_samp_hw_ptr->up_samp_struct.out_freq = up_samp_ptr->usPhase.outFreq;
                  dn_samp_hw_ptr->up_samp_struct.frac_const = (RS_DRV_INTERP_MODE_LINEAR == interp_mode) ? up_samp_ptr->usPhase.fracConst32 : up_samp_ptr->usPhase.fracConst;
                  dn_samp_hw_ptr->up_samp_struct.shift_const = up_samp_ptr->usPhase.shiftConst;
               }
            }
            else //down sampler 
            {
               if(0 != (ret_val = rs_drv_init_dnsamp((void*) dn_samp_ptr, &param[0])))
               {
                  dn_samp_hw_ptr->up_samp_struct.int_phase_step_size = dn_samp_ptr->upSamp.usPhase.intPhStep;
                  dn_samp_hw_ptr->up_samp_struct.cur_int_phase = dn_samp_ptr->upSamp.usPhase.curIntPh;
                  dn_samp_hw_ptr->up_samp_struct.frac_phase_step_size = dn_samp_ptr->upSamp.usPhase.fracPhStep;
                  dn_samp_hw_ptr->up_samp_struct.cur_frac_phase = dn_samp_ptr->upSamp.usPhase.curFracPh;
                  dn_samp_hw_ptr->up_samp_struct.out_freq = dn_samp_ptr->upSamp.usPhase.outFreq;
                  dn_samp_hw_ptr->up_samp_struct.frac_const = (RS_DRV_INTERP_MODE_LINEAR == interp_mode) ? dn_samp_ptr->upSamp.usPhase.fracConst32 : dn_samp_ptr->upSamp.usPhase.fracConst;
                  dn_samp_hw_ptr->up_samp_struct.shift_const = dn_samp_ptr->upSamp.usPhase.shiftConst;

                  dn_samp_hw_ptr->ds_factor = dn_samp_ptr->dnMem.dsPhase.dnSampFac;
                  dn_samp_hw_ptr->ds_step = dn_samp_ptr->dnMem.dsPhase.dnSampStep;

                  if(in_freq == ret_val)
                  {
                     op_mode = RS_OPERATION_DNSAMPLING;
                  }
                  else
                  {
                     op_mode = RS_OPERATION_RESAMPLING;
                  }
               }
            }
         }

         if(0 != ret_val)
         {
            job_config_struct_ptr->ptr_dn_samp_struct = (struct_rs_dnsampler *)dn_samp_hw_ptr;   
            job_config_struct_ptr->ptr_cust_coeff = NULL;  
            job_config_struct_ptr->operation = (rs_operation_type) op_mode;  
            job_config_struct_ptr->prio = RS_PRIORITY_LOW;                           
            job_config_struct_ptr->num_channels = num_channels; 
            job_config_struct_ptr->internal_buf_needed = internal_buf_needed;
            job_config_struct_ptr->input_num_sample_max  = input_num_sample_max;
            job_config_struct_ptr->data_width = (samp_bit_width == 16) ? RS_SAMPLE_WIDTH_16 : RS_SAMPLE_WIDTH_32; 
            job_config_struct_ptr->coeff_width = RS_COEFF_QUALITY_SHQ;   
            job_config_struct_ptr->submit_job = RS_JOB_SUBMIT;                    
            job_config_struct_ptr->coeff_src = RS_COEFF_SRC_ROM;
            job_config_struct_ptr->upsamp_filt_id = (rs_upsamp_filter_type)job_cfg_ptr->upsamp_filt_id;
            job_config_struct_ptr->dnsamp_filt_id = RS_DNSAMPLER_FILTER_ID_1344;
            job_config_struct_ptr->interp_mode = (rs_interp_type)interp_mode;
            job_config_struct_ptr->force_interpol = (rs_force_interpol_type)force_interpol;
            job_config_struct_ptr->dynamic_rs = (rs_dynamic_resampler_type)dynamic_rs;

            ret = ADSP_EOK;
         }
      }
      else
      {
         //print msg
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_config_job ERROR NULL pointer\n");
      }
   }
   else
   {
      //print msg
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_config_job ERROR input NULL pointer\n");
   }

   return ret;
}

ADSPResult rs_drv_cal_num_samp_to_hw(rs_drv_job_info_t* job_info_ptr, rs_drv_num_samp_t* num_samp_max_ptr, rs_drv_num_samp_t* num_samp_to_hw_ptr)
{
   int32 num_samp_in_max = num_samp_max_ptr->num_samp_in;
   int32 num_samp_out_max = num_samp_max_ptr->num_samp_out;

   int32 num_samp_out_possible = 0;
   int32 num_samp_in_possible = 0;
   int32 num_samp_out_real = 0;
   int32 num_samp_in_real = 0;

   //1. fixed input mode to get num_samp_out_possible according to num_samp_in_max
   if(RS_OPERATION_UPSAMPLING == job_info_ptr->op_mode)
   {
      num_samp_out_possible = rs_drv_estimate_upsampler_output_samples(num_samp_in_max, job_info_ptr->job_id, (int32) job_info_ptr->osr, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
   }
   else
   {
      num_samp_out_possible = rs_drv_estimate_dnsampler_output_samples(num_samp_in_max, job_info_ptr->job_id, (int32)job_info_ptr->osr, (int32) job_info_ptr->op_mode, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
   }

   if(num_samp_out_possible <= num_samp_out_max)
   {
      num_samp_to_hw_ptr->num_samp_in = num_samp_in_max;
      num_samp_to_hw_ptr->num_samp_out = num_samp_out_possible;
   }
   else
   {
      //2. Check against the num_samp_out provided 
      num_samp_out_real = (num_samp_out_possible < num_samp_out_max) ? num_samp_out_possible : num_samp_out_max;

      //3. fixed output mode - calculate the num of input according to num_samp_out_real
      if(RS_OPERATION_UPSAMPLING == job_info_ptr->op_mode)
      {
         num_samp_in_possible = rs_drv_estimate_upsampler_input_samples(num_samp_out_real, job_info_ptr->job_id, (int32) job_info_ptr->osr, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
      }
      else
      {
         num_samp_in_possible = rs_drv_estimate_dnsampler_input_samples(num_samp_out_real, job_info_ptr->job_id, (int32) job_info_ptr->osr, (int32) job_info_ptr->op_mode, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
      }

      //4. Check against the num_samp_in provided 
      num_samp_in_real = (num_samp_in_possible < num_samp_in_max) ? num_samp_in_possible : num_samp_in_max;

      num_samp_to_hw_ptr->num_samp_in = num_samp_in_real;
      num_samp_to_hw_ptr->num_samp_out = num_samp_out_real;
   }

   return ADSP_EOK;
}


ADSPResult rs_drv_update_us_phase(uint16 job_id, int32 up_samp_osr, USPhase* us_phase, struct_rs_dnsampler* rs_dnsampler)
{
   ADSPResult rc = ADSP_EOK;

   if((us_phase) && (rs_dnsampler) && (ADSP_EOK == (rc = rs_get_stream_config(job_id, rs_dnsampler))))
   {
      us_phase->intPhStep = rs_dnsampler->up_samp_struct.int_phase_step_size;  
      us_phase->outFreq = rs_dnsampler->up_samp_struct.out_freq;               
      us_phase->curIntPh = rs_dnsampler->up_samp_struct.cur_int_phase;         
      us_phase->curFracPh = rs_dnsampler->up_samp_struct.cur_frac_phase;       
      us_phase->fracPhStep = rs_dnsampler->up_samp_struct.frac_phase_step_size;  
      us_phase->totalIntPh = up_samp_osr;
      us_phase->missSamp = 0; // can never be 1 in case of HW
   }
   else
   {
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_update_us_phase ERROR rs_get_stream_config failed\n");
   }

   return rc;
}


ADSPResult rs_drv_cal_stream_data(rs_drv_job_info_t* job_info_ptr, int32 in_freq, int32 out_freq, int16 osr, struct_rs_stream_data *ptr_rs_str_data, rs_drv_sample_rate_drift_mode mode)
{
   int16  int_phase_step_size;
   int32  frac_phase_step_size;  
   int32 temp32;
   ADSPResult rc = ADSP_EFAILED;

   if(ptr_rs_str_data && job_info_ptr)
   {
      temp32 = s32_extract_s64_l(s64_mult_s32_s16(in_freq, osr));
      temp32 = int_div_32(temp32, out_freq, &frac_phase_step_size);
      int_phase_step_size = s16_extract_s32_l(temp32);

      ptr_rs_str_data->int_phase_step_size = int_phase_step_size;
      ptr_rs_str_data->frac_phase_step_size = frac_phase_step_size;

      if(RS_DRV_SAMPLE_RATE_DRIFT_MODE_INPUT != mode)
      {
         struct_rs_dnsampler rs_dnsampler;
         int32 temp32;
         int32 tmp32;
         int16 shift;
         rs_drv_interp_type interp_mode = job_info_ptr->in_job_cfg_ptr->interp_mode;
         int32 frac_const32;
         int32 frac_const;

         if(ADSP_EOK == (rc = rs_get_stream_config(job_info_ptr->job_id, &rs_dnsampler)))
         {
            int32 cur_med_freq;
            if(0 != rs_dnsampler.ds_factor)
            {
               cur_med_freq = out_freq * rs_dnsampler.ds_factor;
            }
            else
            {
               cur_med_freq = out_freq;
            }

            if(cur_med_freq != job_info_ptr->prev_out_freq)
            {
               ptr_rs_str_data->cur_frac_phase = rs_dnsampler.up_samp_struct.cur_frac_phase * cur_med_freq / job_info_ptr->prev_out_freq;
               job_info_ptr->prev_out_freq = cur_med_freq;
            }
            else
            {
               ptr_rs_str_data->cur_frac_phase = rs_dnsampler.up_samp_struct.cur_frac_phase;
            }

            ptr_rs_str_data->med_freq = cur_med_freq;

            temp32 = s32_shl_s32_sat((int32)1, Q20_SHIFT);
            shift  = Q20_SHIFT;
            while (temp32 >= cur_med_freq)
            {
              temp32 = s32_shl_s32_sat(temp32, -1);
              shift--;
            } 

            tmp32 = frac_div_32(temp32, cur_med_freq, Q23_SHIFT);
            frac_const32 = tmp32;

            tmp32 = frac_div_32(temp32, cur_med_freq, Q15_SHIFT);
            frac_const  = s16_extract_s32_l(tmp32);

            ptr_rs_str_data->frac_const = (RS_DRV_INTERP_MODE_LINEAR == interp_mode) ? frac_const32 : frac_const;
            ptr_rs_str_data->shift_const = shift;
         }
      }

      rc = ADSP_EOK;
   }

   return rc;
}

ADSPResult rs_drv_ioctl_global_prop(rs_drv_ioctl_prop_info_t *prop_info_ptr)
{
   ADSPResult rc = ADSP_EOK;

   switch(prop_info_ptr->prop)
   {
      case RS_DRV_PROPERTY_HW_VERSION_QUERY:

      if (sizeof(uint32) == prop_info_ptr->data_size && NULL != prop_info_ptr->data_ptr )
      {
         *((uint32*)prop_info_ptr->data_ptr) = rs_query_hw_version();
      }
      break;

      default:
         break;

   }
            
   return rc;
}



//////////////////////////////////////////////////////////////////////////
/////dynamic resampler support
/////////////////////////////////////////////////////////////////////////

/*===========================================================================* 
 * FUNCTION NAME  : init_dynamic_resamp()                                    * 
 * DESCRIPTION    : Initialize dynamic resampling data structure.            * 
 * INPUT ARGUMENTS:                                                          * 
 *     ptrDS      : Pointer to down-sampler data structure                   *
 *     param      : parameter list                                           * 
 *     pUsCoef    : Pointer to up-sampler filter coefficients                *
 *     pUsMem     : Pointer to up-sampler filter memory                      *
 *     pDsCoef    : Pointer to up-sampler filter coefficients                *
 *     pDsMem     : Pointer to up-sampler filter memory                      *
 *     pScrMem    : Pointer to intermediate buffer                           *
 * RETURN VALUE   :                                                          * 
 * SIEDE EFFECT   : Anything inside the output buffer will be overwitten.    * 
 *                : The phase information inside the up-sample structure will* 
 *                : be fully updated.                                        * 
 * DEPENDENCIES   : The caller needs to make sure the output buffer is big   * 
 *                : enough.                                                  * 
 * COMMENTS       : The down-sample data structure will carry all information* 
 *                : needed to do down-sample conversion.  Absolute I/O       * 
 *                : frequency will not be kept.  Only relative information   * 
 *                : are kept                                                 * 
 *===========================================================================*/
int32 rs_drv_init_dynamic_resamp(void* pStruct, int32* param)
{
    int16  mode, osr, temp16;
    int32  medFreq, temp32, inFreq, outFreq;
    typeDnSampStruct *pDnsamp = (typeDnSampStruct*)pStruct;
    DSPhase *pDsPhase = &(pDnsamp->dnMem.dsPhase);

    mode    = s16_extract_s32_l(param[MODE]);
    inFreq  = param[IN_FREQ];
    outFreq = param[OUT_FREQ];
    pDnsamp->dnMem.is32Bit     = s16_extract_s32_l((mode & 1) + param[MODE_COEF_BIT]);

    osr = s16_extract_s32_l(param[DS_OSR]);

    if ((inFreq > MAX_FREQ) || 
        (inFreq > outFreq * 24))
    {
        return -1;/* not supported */
    } /* if ((inFreq > MAX_FREQ) || ... */

    /* determine actually up-sampling and down-sampling rate */
    if ((inFreq <= outFreq) && (inFreq <= MAX_FREQ))
    {
       pDsPhase->dnSampFac     = 0;
    }
    else if ((inFreq <= outFreq * 2) && (outFreq * 2 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 2;
    }
    else if ((inFreq <= outFreq * 3) && (outFreq * 3 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 3;
    }
    else if ((inFreq <= outFreq * 4) && (outFreq * 4 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 4;
    }
    else if ((inFreq <= outFreq * 6) && (outFreq * 6 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 6;
    }
    else if ((inFreq <= outFreq * 8) && (outFreq * 8 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 8;
    }
    else if ((inFreq <= outFreq * 12) && (outFreq * 12 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 12;
    }
    else if ((inFreq <= outFreq * 24) && (outFreq * 24 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 24;
    }
    else 
    {
        return -2;
    }/*if ((inFreq <= outFreq * 2) && ...*/

   if (inFreq <= outFreq)
   {
      pDsPhase->dnSampStep    = 0;
      medFreq = outFreq;
   }
   else
   {
      pDsPhase->dnSampStep    = int_div_16(osr, pDsPhase->dnSampFac, &temp16);
      medFreq = s32_extract_s64_l(s64_mult_s32_s16(outFreq, pDsPhase->dnSampFac));
   }

    param[OUT_FREQ] = medFreq;
    temp32 = rs_drv_init_upsamp(&(pDnsamp->upSamp), param);
    param[OUT_FREQ] = outFreq;

    if (temp32 == 0)
    {
        return 0; /* initialization failed. */
    }/* if (temp == 0) */
   
    pDsPhase->inFreq     = inFreq;
    pDsPhase->outFreq    = outFreq;
    pDsPhase->medFreq    = medFreq;

    return medFreq;
}


//=============
ADSPResult rs_drv_reinit_dynamic_resamp_wrapper(rs_drv_job_info_t* job_info_ptr, int32 in_freq, int32 out_freq)
{
   int32 isPrevUpCopy = FALSE, isPrevDnCopy = FALSE, isCurrUpCopy = FALSE, isCurrDnCopy = FALSE;
   // Get some information about current operation

   ADSPResult rc = ADSP_EOK;;
   struct_rs_dnsampler rs_dnsampler;
   typeDnSampStruct Dnsamp;
   memset((void*) &Dnsamp, 0, sizeof(typeUpSampStruct));
   memset((void*) &rs_dnsampler, 0, sizeof(struct_rs_dnsampler));

   rs_drv_dynamic_resampler_type is_dynamic_resampler = job_info_ptr->in_job_cfg_ptr->dynamic_resampler;
   rs_drv_interp_type interp_mode = job_info_ptr->in_job_cfg_ptr->interp_mode;

   if(ADSP_EOK == (rc = rs_drv_update_us_phase(job_info_ptr->job_id, job_info_ptr->osr, &Dnsamp.upSamp.usPhase, &rs_dnsampler)))
   {
       typeDnSampStruct* dn_samp_ptr = job_info_ptr->dynamic_rs_dn_samp_ptr;


      if(dn_samp_ptr)
      {

         if((dn_samp_ptr->upSamp.usPhase.intPhStep == -8000) && (RS_DRV_DYNAMIC_RESAMPLER_ON == is_dynamic_resampler))
         {
            isPrevUpCopy = TRUE;
         }
         
         if((dn_samp_ptr->dnMem.dsPhase.dnSampFac == 0) && (RS_DRV_DYNAMIC_RESAMPLER_ON == is_dynamic_resampler) )
         {
            isPrevDnCopy = TRUE;
         }
      
         dn_samp_ptr->upSamp.usPhase.totalIntPh = job_info_ptr->osr;
         rs_drv_reinit_dynamic_resamp((void*)dn_samp_ptr, in_freq, out_freq, DNSAMPLER_FILT_OSR);  //OSR is for down sampler only which is 24.
      
         // HW requires dnsampfactor to be 1 for copying
         if ((dn_samp_ptr->dnMem.dsPhase.dnSampFac == 0) && (RS_DRV_DYNAMIC_RESAMPLER_ON == is_dynamic_resampler))
         {
            rs_dnsampler.ds_factor = 1;
            isCurrDnCopy = TRUE;
         }
         else
         {
            rs_dnsampler.ds_factor = dn_samp_ptr->dnMem.dsPhase.dnSampFac;
         }

         rs_dnsampler.ds_step = dn_samp_ptr->dnMem.dsPhase.dnSampStep;

         rs_dnsampler.dn_filt_mem_size = DNSAMPLER_FILT_MEM_LENGTH_1632_DIV_FACTOR_2;

         if ( (dn_samp_ptr->upSamp.usPhase.intPhStep == -8000) && (RS_DRV_DYNAMIC_RESAMPLER_ON == is_dynamic_resampler))
         {
            rs_dnsampler.up_samp_struct.int_phase_step_size = dn_samp_ptr->upSamp.usPhase.totalIntPh;
            isCurrUpCopy = TRUE;
         }
         else
         {
            rs_dnsampler.up_samp_struct.int_phase_step_size = dn_samp_ptr->upSamp.usPhase.intPhStep;
         }

         rs_dnsampler.up_samp_struct.frac_phase_step_size = dn_samp_ptr->upSamp.usPhase.fracPhStep;

         rs_dnsampler.up_samp_struct.frac_const = (RS_DRV_INTERP_MODE_LINEAR == interp_mode) ? dn_samp_ptr->upSamp.usPhase.fracConst32 : dn_samp_ptr->upSamp.usPhase.fracConst;
         rs_dnsampler.up_samp_struct.shift_const = dn_samp_ptr->upSamp.usPhase.shiftConst;

         // Setup the upsampler output frequency
         rs_dnsampler.up_samp_struct.out_freq = dn_samp_ptr->upSamp.usPhase.outFreq;

         ///get current 
         if (isPrevUpCopy && !isCurrUpCopy)
         {
            uint32 temp;

            temp = rs_dnsampler.up_sampler_flt_fptr;

            rs_dnsampler.up_sampler_flt_fptr = (temp + DYNAMIC_UP_EXTRA_DELAY) % UPSAMPLER_FILT_MEM_LENGTH;   // Point to last loaded sample as required
         }
         else if (!isPrevUpCopy && isCurrUpCopy)
         {
            uint32 temp;
            // Reinitalize the filter memory in upsampler for copy.
            temp = rs_dnsampler.up_sampler_flt_fptr;

            rs_dnsampler.up_sampler_flt_fptr = temp - DYNAMIC_UP_EXTRA_DELAY ; // Point to last processed sample
            if (rs_dnsampler.up_sampler_flt_fptr < 0)
            {
               rs_dnsampler.up_sampler_flt_fptr += UPSAMPLER_FILT_MEM_LENGTH;
            }
         }

         // Change filter mem offset parameter only in case of transitions. Otherwise dont disturb it.
         if (isPrevDnCopy && !isCurrDnCopy)
         {
            uint32 temp;
            // Reinitialize the filter memory offset for copy.
            temp = rs_dnsampler.dn_sampler_flt_fptr;
            rs_dnsampler.dn_sampler_flt_fptr = (temp + DYNAMIC_DN_EXTRA_DELAY) % rs_dnsampler.dn_filt_mem_size ; // Point to last loaded sample as required
         }
         else if (!isPrevDnCopy && isCurrDnCopy)
         {
            uint32 temp;
            // Reinitialize the filter memory offset for copy.
            temp = rs_dnsampler.dn_sampler_flt_fptr;
            rs_dnsampler.dn_sampler_flt_fptr = temp - DYNAMIC_DN_EXTRA_DELAY; // Point to last processed sample
            if (rs_dnsampler.dn_sampler_flt_fptr < 0) 
            {
               rs_dnsampler.dn_sampler_flt_fptr += rs_dnsampler.dn_filt_mem_size;
            }
         }

         rs_set_stream_config(job_info_ptr->job_id, &rs_dnsampler);
      }
      else
      {
         rc = ADSP_EFAILED;
      }
   }

   return rc;
}


void rs_drv_reinit_dynamic_resamp(void* pStruct, int32 inFreq, int32 outFreq, int16 osr)
{
    int16  temp16;  
    int32  medFreq;
    int16  DiffconvIndex;
    
   typeDnSampStruct *pDnsamp = (typeDnSampStruct*)pStruct;
    DSPhase *pDsPhase = &(pDnsamp->dnMem.dsPhase);

    DiffconvIndex = pDsPhase->dnSampFac;

    /* determine actually up-sampling and down-sampling rate */
    if ((inFreq <= outFreq) && (outFreq <= MAX_FREQ))
    {
       pDsPhase->dnSampFac     = 0;
    }
    else if ((inFreq <= outFreq * 2) && (outFreq * 2 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 2;
    }
    else if ((inFreq <= outFreq * 3) && (outFreq * 3 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 3;
    }
    else if ((inFreq <= outFreq * 4) && (outFreq * 4 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 4;
    }
    else if ((inFreq <= outFreq * 6) && (outFreq * 6 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 6;
    }
    else if ((inFreq <= outFreq * 8) && (outFreq * 8 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 8;
    }
    else if ((inFreq <= outFreq * 12) && (outFreq * 12 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 12;
    }
    else if ((inFreq <= outFreq * 24) && (outFreq * 24 <= MAX_FREQ))
    {
        pDsPhase->dnSampFac     = 24;
    }
   else 
    {
        return; /* sampling rate not supported */
    }
    
   DiffconvIndex = pDsPhase->dnSampFac - DiffconvIndex;
   pDsPhase->convIndex = pDsPhase->convIndex + DiffconvIndex;

   if (inFreq <= outFreq)
   {
      pDsPhase->dnSampStep    = 0;
      medFreq = outFreq;
   }
   else
   {
      pDsPhase->dnSampStep    = int_div_16(osr, pDsPhase->dnSampFac, &temp16);
      medFreq = s32_extract_s64_l(s64_mult_s32_s16(outFreq, pDsPhase->dnSampFac));
   }

    // re-init upsampler
    rs_drv_reinit_upsamp(&(pDnsamp->upSamp), inFreq, medFreq);
        
    pDsPhase->inFreq     = inFreq;
    pDsPhase->outFreq    = outFreq;
    pDsPhase->medFreq    = medFreq;

    return;
}


void rs_drv_reinit_upsamp(void* ptrUS, int32 inFreq, int32 outFreq)
{
    int16 shift;
   int32 temp32, tmp32;

    typeUpSampStruct* pUpsamp = ptrUS;
    USPhase *pPhase = &(pUpsamp->usPhase);
    
    pPhase->inFreq      = inFreq;
   pPhase->outFreq     = outFreq;
     
    /* the product will not exceed 32 bit if max fs is 384 kHz */
   if (inFreq  == outFreq)
   {
      pPhase->intPhStep  = -8000;
      pPhase->fracPhStep = 0;
   }
   else
   {
      /* the product will not exceed 32 bit if max fs is 384 kHz */
      temp32 = s32_extract_s64_l(s64_mult_s32_s16(inFreq, pPhase->totalIntPh));
      temp32 = int_div_32(temp32, outFreq, &(pPhase->fracPhStep));
      pPhase->intPhStep = s16_extract_s32_l(temp32);

      /* compute fractional phase constant and corresponding shift constant */
      temp32 = s32_shl_s32_sat((int32)1, Q20_SHIFT);
      shift  = Q20_SHIFT;
      while (temp32 >= outFreq)
      {
         temp32 = s32_shl_s32_sat(temp32, -1);
         shift--;
      } /* while (temp > outFreq) */
      pPhase->shiftConst = shift;

      tmp32 = frac_div_32(temp32, outFreq, Q23_SHIFT);
      pPhase->fracConst32  = tmp32;

      tmp32 = frac_div_32(temp32, outFreq, Q15_SHIFT);
      pPhase->fracConst  = s16_extract_s32_l(tmp32);

   }
   
} /* reinit_upsamp */
