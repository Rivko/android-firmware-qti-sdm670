/*========================================================================

Copyright (c) 2009-2010, 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

====================================================================== */

/*======================= COPYRIGHT NOTICE ======================*
 * Copyright (c) 2009-2010, 2012-2015 by Qualcomm Technologies, Inc.  All     *
 * rights reserved.                                              *
 * All data and information contained in or disclosed by this    *
 * document is confidental and proprietary information of        *
 * Qualcomm Technologies, Inc and all rights therein are expressly reserved.  *
 * By accepting this material the recipient agrees that this     *
 * material and the information contained therein is held in     *
 * confidence and in trust and will not be used, copied,         *
 * reproduced in whole or in part, nor its contents revealed in  *
 * any manner to others without the express written permission   *
 * of Qualcomm Technologies, Inc.                                             *
 *===============================================================*/
        
/*****************************************************************
 * FILE NAME: g711a2.h									         *
 *                                                               *
 * DESCRIPTION: Function prototypes and constants use in G.711A2 *
 *                                                               *
 *                                                               *
 *****************************************************************/

/*___________________________________________________________________________
 |                                                                           |
 |   Include-Files                                                           |
 |___________________________________________________________________________|
*/

#ifndef __G711API_H__
#define __G711API_H__


/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |___________________________________________________________________________|
*/

#define G711_L_FRAME_20MS    160     /* Frame size for 20ms window               */
#define G711_L_FRAME_10MS    80      /* Frame size for 10ms window               */
#define G711_L_TOTALFRAME    160     /* Frame length for 20ms                    */
#define G711_PCM_FRAMED_SIZE 321     /* max size of 20 ms DSP framed data is 321 bytes => PCM speech frame (20 ms speech + 1 byte header) */
#define ACTIVE_FRAME    0      /* Full rate  (8000 bit/s)       */
#define SID             1      /* SID                           */
#define NTX             2      /* 0 bit/s rate;no transmission  */
#define ERASURE         3      /* Erasure flag; applicable for downlink only */

/*___________________________________________________________________________
 |                                                                           |
 |   Operators prototypes                                                    |
 |___________________________________________________________________________|
*/

/*--------------------------------------------------------------------------*
 * Main Encoder and Decoder functions										*
 *--------------------------------------------------------------------------*/

int16 G711EncoderGetSize( void);

void G711A2EncoderInit(
   void   *G711Struct,         /*     : Structure pointer						*/
   int16  framesize	           /*     : framesize in number of samples          */
);

void G711A2Encoder(
   void           *G711Struct, 
	int16          speech[],    /* (i)  : current frame samples             */
	int16			   serial[],	/* (o)  : speech vector						*/
	int16			   dtx_enable,  /* (i)  : VAD flag							*/
	int16			   user_mode,   /* (i)  : A-law/Mu-law flaf                 */ 
	int16	   	       *frame,		/* (i)  : frame count						*/
	int16             serial_bufsize /* (i) : speech vector buffer size         */
);

void G711FSEncoder(
   void            *G711Struct, 
   int16           nSamples,    /* (i)  : number of input samples           */
	int16           speech[],    /* (i)  : current frame samples             */
	int16			    serial[],	/* (o)  : speech vector						*/
	int16			    user_mode,   /* (i)  : A-law/Mu-law/PCM flag             */ 
	int16			    *frame,		/* (i)  : frame count						*/
	int16             serialbufsize /* (i) : speech vector buffer size         */
);

void G711Framing(
	uint8   *codedBuf,		/* (i) : Coded data from core library							*/
	uint8   *framedBuf,	    /* (o) : Input speech vector(linear samples)			        */
	uint16   len,           /* (i) : Number of total bytes in codedBuf                      */
	uint16   framedbuflen   /* (i)  : buffer lenght in bytes for input speech vector        */
);


int16 G711DecoderGetSize( void);

void G711FSDecoder(
	int16   parm[],			  /* (i)  : input signal       */
	int16   output[], 		  /* (o)  : output signal      */
	int16   inputSamples,
	int16   mode,
	int16   outputbufsize     /* (o)  : output buffer size */
);

void G711A2DecoderInit(
	void  *dec,         /*     : Structure pointer						*/
	int16 framesize	/*     : framesize in number of samples         */
);

void G711A2Decoder(
   void   *dec,
	int16 *parm,			/* (i) : input signal							*/
	int16 *output,		    /* (o) : output signal							*/
	int16 outputbufsize     /* (i) : outputbufsize                          */
);

void G711A2DecoderPlcAddToHistory( 
   void  *dec,
   int16 *speech
);   

void G711A2DecoderPlcConceal( 
   void    *dec,
   int16   *speech
);  

void G711Deframing(
	uint8   *framedBuf,		/* (i) : framed coded data from core src						*/
	uint8   *deframedBuf,    /* (o) : deframed coded data suitable for core decoder lib)     */
	uint16  len,             /* (i) : Number of total bytes in framedBuf                     */
	int16   deframedBuflen  /* (i) : deframed buffer length                                  */
);

int16 G711_check_erasure(
    int16 frameisgood,       /* (i)     :good frame flag                */
    void *g711_dec  /* Decoder : structure pointer            */
);

#endif /* __G711API_H__ */

