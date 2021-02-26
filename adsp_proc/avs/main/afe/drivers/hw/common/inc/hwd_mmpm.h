
#ifndef HWDMMPM_H
#define HWDMMPM_H


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


#include "mmpm.h"

typedef struct
{
    uint32 masterPort;
    uint32 slavePort;
    uint64 bwVal;
    uint32 usagePercent;
    uint32 usageType;
} HwdBwReqType;


typedef struct
{
    uint32  clkId;
    uint32  clkFreqHz;
    uint32  clkDomainSrc;
} HwdClkDomainReqType;


typedef struct
{
    uint32 clkId;
    uint32 freq;
    uint32 freqMatch;
} HwdCoreClkInfoType;


uint32 HwdMmpm_Register(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId);
ADSPResult HwdMmpm_Deregister(uint32 clientId);
ADSPResult HwdMmpm_RequestPwr(uint32 clientId);
ADSPResult HwdMmpm_ReleasePwr(uint32 clientId);
ADSPResult HwdMmpm_RequestClk(uint32 clientId, MmpmClkIdLpassType clkId[], uint32 clkFreq[], uint32 numClk);
ADSPResult HwdMmpm_ReleaseClk(uint32 clientId,  MmpmClkIdLpassType clkId[], uint32 numClk);
ADSPResult HwdMmpm_RequestRegProg(uint32 clientId, MmpmRegProgMatchType match);
ADSPResult HwdMmpm_ReleaseRegProg(uint32 clientId);
ADSPResult HwdMmpm_RequestSleepLatency(uint32 clientId, uint32 microSec);
ADSPResult HwdMmpm_ReleaseSleepLatency(uint32 clientId);
ADSPResult HwdMmpm_RequestBw(uint32 clientId, HwdBwReqType *bw, uint32 numBw);
ADSPResult HwdMmpm_ReleaseBw(uint32 clientId);
ADSPResult HwdMmpm_RequestMips(uint32 clientId, uint32 mips, uint32 mipsPerTherad);
ADSPResult HwdMmpm_ReleaseMips(uint32 clientId);

ADSPResult HwdMmpm_RequestClkDomain(uint32 clientId, HwdClkDomainReqType clk[], uint32 numClk);
ADSPResult HwdMmpm_ReleaseClkDomain(uint32 clientId, uint32 relClkId[], uint32 numClk);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef HWDMMPM_H

