/*========================================================================

Copyright (c) 2008-2009, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/*======================= COPYRIGHT NOTICE ======================*/
/* Copyright (c) 2008-2009, 2012-2014 by Qualcomm Technologies, Inc.          */
/* All rights reserved.                                          */
/* All data and information contained in or disclosed by this    */
/* document is confidential and proprietary information of       */
/* Qualcomm Technologies, Inc and all rights therein are expressly reserved.  */
/* By accepting this material the recipient agrees that this     */
/* material and the information contained therein is held in     */
/* confidence and in trust and will not be used, copied,         */
/* reproduced in whole or in part, nor its contents revealed in  */
/* any manner to others without the express written permission   */
/* of Qualcomm Technologies, Inc.                                             */
/*===============================================================*/

#ifndef __V13K_DEC_API_H
#define __V13K_DEC_API_H


extern int16 v13k_get_dec_struct_size(void);

extern void v13k_init_dec13(void *decStructPtr);

void v13k_init_tty_dec(void *decStructPtr, int16 ttyOption);

extern int16 v13k_decoder13(void *decStructPtr,
                    int16 *rxPacketRealTime,
                    int16 *decOutBuf,
                    void *decTtyStructPtr);

extern void v13k_repacking_mvs_to_dsp(int8 *iPacket);

#endif //__V13K_DEC_API_H
