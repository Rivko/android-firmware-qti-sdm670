#ifndef DOG_HAL_H
#define DOG_HAL_H
/** vi: tw=128 ts=3 sw=3 et
@file dog_hal.c
@brief This file contains the API details for the Dog Services, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014-2018 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/api/dog_hal.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

FUNCTIONS
  HAL_dogEnable
  HAL_dogDisable
  HAL_dogKick
  HAL_dogAutokick
  HAL_dogEnableBark
  HAL_dogDisableBark
  HAL_dogRegisterBark
  HAL_dogIsAutokicking
  HAL_dogForceBite
  HAL_dogForceNMI
  HAL_dogDisableBark_NMI
  HAL_dogInit

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/** =====================================================================
 * Function:
 *     HAL_dogEnable
 *
 * Description:
 *     This function enables the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnable(void);

/** =====================================================================
 * Function:
 *     HAL_dogDisable
 *
 * Description:
 *     This function disables the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogDisable(void);

/** =====================================================================
 * Function:
 *     HAL_dogKick
 *
 * Description:
 *     This function kicks the hardware dog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogKick(void);

/** =====================================================================
 * Function:
 *     HAL_dogAutokick
 *
 * Description:
 *     This function turns on dog autokick by pausing the HW, HB and SW timer
 *     and disabling the HW wdog
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogAutokick(boolean enable);

/** =====================================================================
 * Function:
 *     HAL_dogEnableBark
 *
 * Description:
 *     Enable the watchdog bark timer.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     none
 * =====================================================================  */
void HAL_dogEnableBark(void);

/** =====================================================================
* Function:
*     HAL_dogDisableBark
*
* Description:
*     Disable the watchdog bark timer.
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogDisableBark(void);


/** =====================================================================
* Function:
*     HAL_dogDisableBark_NMI
*
* Description:
*     Disable the watchdog bark timer and NMI
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogDisableBark_NMI(void);

/** =====================================================================
* Function:
*     HAL_dogRegisterBark
*
* Description:
*     Register an ISR for the watchdog bark interrupt.
*
* Parameters:
*     none
*
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogRegisterBark(void);

/** =====================================================================
* Function:
*     HAL_dogIsAutokicking
*
* Description:
*     Check if dog is autokicking
*
* Parameters:
*     uint32 status pointer
*
* Returns:
*     pointer set to 1 if autokicking, 0 otherwise
* =====================================================================  */
void HAL_dogIsAutokicking(unsigned int* status);

/** =====================================================================
* Function:
*     HAL_dogForceBite
*
* Description:
*     This function forces the watchdog to bite
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogForceBite(void);

/** =====================================================================
* Function:
*     HAL_dogForceNMI
*
* Description:
*     This function forces the watchdog to do an NMI
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogForceNMI(void);

/** =====================================================================
* Function:
*     HAL_dogInit
*
* Description:
*     Initialize hardware dog
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
void HAL_dogInit(void);

#endif
