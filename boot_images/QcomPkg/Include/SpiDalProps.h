/*==================================================================================================

FILE: SpiDalProps.h

DESCRIPTION:
   The file declares structures and functions to fetch device configuration properties 

   ====================================================================================================

  Edit History
   

When      Who  What, where, why
--------  ---  ------------------------------------------------------------
04/01/14  vmk  Initial version

====================================================================================================
                     Copyright (c) 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR
==================================================================================================*/
#include "DDIClock.h"
#include "bam.h"
#include "npa.h"

typedef struct SpiDevProp_BamDescFifoType
{
   void     *phMemHandle;
   uint32   physAddr;
   uint32   virtualAddr;
}SpiDevProp_BamDescFifoType;   

typedef struct SpiDevProp_BamPipeCtxt
{
   bam_handle             hBamPipeDev;  
   uint32                 devBamPipeNum;  
   bam_pipe_config_type   devBamPipeConfig;
   bam_callback_type      devBamPipeCallbackCfg;
   SpiDevProp_BamDescFifoType   devBamPipeDescFifo;
} SpiDevProp_BamPipeCtxt;

typedef struct SpiDevProp_BamCallbackType
{
   void *func;
   void *data;
} SpiDevProp_BamCallbackType;

typedef struct SpiDevProp_BamDevCfgType
{
   uint32                    uBamDevId;
   bam_handle                hBamDev;  
   bam_config_type           devBamConfig;
   SpiDevProp_BamCallbackType  devBamCallbackCfg;
   SpiDevProp_BamPipeCtxt      devBamRxPipeCtxt;  
   SpiDevProp_BamPipeCtxt      devBamTxPipeCtxt;  
} SpiDevProp_BamDevCfgType;

typedef struct SpiDevProp_DevTargetCfgType
{
   char *pQupHClkName;
   char *pQupAppClkName;
   void *uOsDevId;
   uint32 uQupCoreNum;
   ClockIdType QupHClkId;
   ClockIdType QupAppClkId;
   uint8  *qupVirtBlockAddr;
   uint8  *qupPhysBlockAddr;
   uint8  *tcsrVirtBlockAddr;
   boolean bInterruptBased;
   boolean bBamSupported;
   boolean bTcsrInterruptRequired;
   boolean bUseCmdDesc;
   uint32 uTcsrInterruptBitMask;
   uint32 uTcsrInterruptBitShift;
   SpiDevProp_BamDevCfgType *spiBamDevCfg;
   uint32 uActiveDevCount;
   boolean bPnocEnable;
   uint64 uPnocIBVal;
   uint64 uPnocABVal;
   uint32 uPnocMaster;
   uint32 uPnocSlave;
   npa_client_handle pNpaHandle;
}SpiDevProp_DevTargetCfgType;

#define SPIDEVICEPLAT_CHK_RESULT_BREAK(res) if (SPIDEVICE_RES_SUCCESS !=(res)) break;

uint32 SpiDevProp_GetConfig(void *QupHandle, void *conf);
boolean SpiDevProp_SearchDevCfg (void *devId, SpiDevProp_DevTargetCfgType **ppDevCfg);
int32 SpiDevProp_RemoveDevCfg (void *uOsDevId);
