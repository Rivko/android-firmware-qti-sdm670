/*============================================================================
  FILE:         TsensCore.c

  OVERVIEW:     Implementation of TSENS supporting XBL core.

  DEPENDENCIES: None

                Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-08  jjo  Memory enhancements.
  2014-07-21  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensLib.h"
#include "HALtsens.h"
#include "TsensBsp.h"
#include "DALSys.h"
#include "DDIChipInfo.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   uint8 *pucTsensSROTAddr;
   uint8 *pucTsensTMAddr;
   uint32 uChannelEnableMask;
} TsensControllerType;

typedef struct
{
   const TsensBspType *pTsensBsp;
   TsensControllerType *paControllers;
} TsensDevCtxtType;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
TsensDevCtxtType gTsensDevCtxt = {0};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint8* Tsens_MapHWIORegion(uint8 *puPhysical, uint32 uSize)
{
   return puPhysical;
}

static void Tsens_GetTempInternal(uint32 uController, uint32 uChannel, int32 *pnDeciDegC)
{
   int32 nDeciDegC = 0;
   int32 nDeciDegCTry1;
   int32 nDeciDegCTry2;
   int32 nDeciDegCTry3;
   uint8 *pucTsensTMAddr;
   boolean bValid;

   pucTsensTMAddr = gTsensDevCtxt.paControllers[uController].pucTsensTMAddr;

   bValid = HAL_tsens_tm_GetChannelPrevTemp(pucTsensTMAddr, uChannel, &nDeciDegCTry1);
   if (bValid)
   {
      nDeciDegC = nDeciDegCTry1;
   }
   else
   {
      bValid = HAL_tsens_tm_GetChannelPrevTemp(pucTsensTMAddr, uChannel, &nDeciDegCTry2);
      if (bValid)
      {
         nDeciDegC = nDeciDegCTry2;
      }
      else
      {
         bValid = HAL_tsens_tm_GetChannelPrevTemp(pucTsensTMAddr, uChannel, &nDeciDegCTry3);
         if (bValid)
         {
            nDeciDegC = nDeciDegCTry3;
         }
         else if (nDeciDegCTry1 == nDeciDegCTry2)
         {
            nDeciDegC = nDeciDegCTry1;
         }
         else if (nDeciDegCTry2 == nDeciDegCTry3)
         {
            nDeciDegC = nDeciDegCTry2;
         }
         else
         {
            nDeciDegC = nDeciDegCTry1;
         }
      }
   }

   *pnDeciDegC = (int32)nDeciDegC;

   return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
TsensResultType Tsens_Init(void)
{
   const char *pszTsensBspPropName = "TSENS_BSP";
   const TsensControllerCfgType *pControllerCfg;
   const TsensBspType *pBsp;
   DALSYS_PROPERTY_HANDLE_DECLARE(hTsensBootProperties);
   DALSYSPropertyVar propertyVar;
   TsensControllerType *pController;
   uint32 uController;
   DALResult status;
   DalChipInfoFamilyType eChipFamily;

   DALSYS_InitMod(NULL);

   /* Get the BSP */
   eChipFamily = DalChipInfo_ChipFamily();

   if (eChipFamily == DALCHIPINFO_FAMILY_MSM8997)
   {
      pszTsensBspPropName = "TSENS_BSP_8997";
   }

   status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/tsens", hTsensBootProperties);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   status = DALSYS_GetPropertyValue(hTsensBootProperties,
                                    pszTsensBspPropName,
                                    0,
                                    &propertyVar);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   pBsp = (TsensBspType *)propertyVar.Val.pStruct;
   gTsensDevCtxt.pTsensBsp = pBsp;

   /* Allocate memory for controller context */
   status = DALSYS_Malloc((pBsp->uNumControllers * sizeof(TsensControllerType)),
                          (void **)&gTsensDevCtxt.paControllers);
   if (status != DAL_SUCCESS || gTsensDevCtxt.paControllers == NULL)
   {
      return TSENS_ERROR;
   }

   /* Map physical to virtual addresses */
   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pControllerCfg = &pBsp->paControllerCfgs[uController];
      pController = &gTsensDevCtxt.paControllers[uController];

      pController->pucTsensSROTAddr = Tsens_MapHWIORegion(pControllerCfg->pucTsensSROTPhys,
                                                          pControllerCfg->uTsensSROTSize);
      if (pController->pucTsensSROTAddr == NULL)
      {
         return TSENS_ERROR;
      }

      pController->pucTsensTMAddr = Tsens_MapHWIORegion(pControllerCfg->pucTsensTMPhys,
                                                        pControllerCfg->uTsensTMSize);
      if (pController->pucTsensSROTAddr == NULL)
      {
         return TSENS_ERROR;
      }
   }

   /* Check which sensors are actually enabled */
   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pController = &gTsensDevCtxt.paControllers[uController];

      pController->uChannelEnableMask = HAL_tsens_srot_GetEnabledChannels(pController->pucTsensSROTAddr);
   }

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;

   if (puNumSensors == NULL)
   {
      return TSENS_ERROR;
   }

   *puNumSensors = pBsp->uNumSensors;

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnDeciDegC)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   uint32 uController;
   uint32 uChannel;
   int32 nDeciDegC;

   if (uSensor >= pBsp->uNumSensors)
   {
      return TSENS_ERROR;
   }

   if (pnDeciDegC == NULL)
   {
      return TSENS_ERROR;
   }

   uController = pBsp->paSensorCfgs[uSensor].ucController;
   uChannel = pBsp->paSensorCfgs[uSensor].ucChannel;

   if (((1 << uChannel) & gTsensDevCtxt.paControllers[uController].uChannelEnableMask) == 0)
   {
      return TSENS_ERROR_SENSOR_NOT_AVAILABLE;
   }

   Tsens_GetTempInternal(uController, uChannel, &nDeciDegC);

   *pnDeciDegC = nDeciDegC;

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetMaxTemp(int32 *pnMaxDeciDegC)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   int32 nMaxDeciDegC = 0;
   DALBOOL bFirst = TRUE;
   uint8 *pucTsensTMAddr;
   uint32 uController;
   int32 nDeciDegC;
   uint32 uSensorID;

   if (pnMaxDeciDegC == NULL)
   {
      return TSENS_ERROR;
   }

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pucTsensTMAddr = gTsensDevCtxt.paControllers[uController].pucTsensTMAddr;

      HAL_tsens_tm_GetMaxTemp(pucTsensTMAddr, &uSensorID, &nDeciDegC);

      if (bFirst)
      {
         nMaxDeciDegC = nDeciDegC;
         bFirst = FALSE;
      }
      else
      {
         if (nDeciDegC > nMaxDeciDegC)
         {
            nMaxDeciDegC = nDeciDegC;
         }
      }
   }

   *pnMaxDeciDegC = nMaxDeciDegC;

   return TSENS_SUCCESS;
}

