/*======================= COPYRIGHT NOTICE ======================*/
/* Copyright (c) 2007-2008, 2012-2014 Qualcomm Technologies, Inc.*/
/* All Rights Reserved.                                          */
/*  Confidential and Proprietary - Qualcomm Technologies, Inc.   */
/* By accepting this material the recipient agrees that this     */
/* material and the information contained therein is held in     */
/* confidence and in trust and will not be used, copied,         */
/* reproduced in whole or in part, nor its contents revealed in  */
/* any manner to others without the express written permission   */
/* of Qualcomm Technologies, Inc.                                             */
/*===============================================================*/
/*---------------------------------------------------------------*/
/* FileName: VolumeControl_api.h            FileType: C Header   */
/*---------------------------------------------------------------*/
/* Description:                                                  */
/*   Publicizes the functions to systems code                    */
/*---------------------------------------------------------------*/
/* Revision History: None                                        */
/* Author            Date                      Comments          */
/* -------          ------                     ----------        */
/* Syed Arif      15,Nov,2007  Exports req func's to systems code*/
/*****************************************************************/

#ifndef VOLUMECONTROL_API_H
#define VOLUMECONTROL_API_H

/* Public function declarations  */

//Function returns output of volume control
void volume_control(int16 *input, int16 *output, int32 length, int32 volLevel);

#endif

