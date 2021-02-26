/*===========================================================================* 
Copyright (c) 2009-2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 * reproducedin whole or in part, nor its contents revealed in any manner to * 
 * others without the expressed written premission of Qualcomm Technologies INC.          * 
 *===========================================================================*/
/* FILE NAME: EasyHearing.h                                                  *
 * DESCRIPTION:                                                              *
 *   Header file for easyHearing.cpp file.                                   *
 *===========================================================================*/
/* REVISION HISTORY:                                                         *
 *                                                                           *
 *   when        who      what, where, why                                   *
 *   --------    ------   ---------------------------------------------------*
 *   11/11/08    hesuh    initial release of the file.                       *
 *                                                                           *
 *===========================================================================*/

#ifndef ST_H_INCLUDED_
#define ST_H_INCLUDED_

#define state_type st_state_type
#define vocoder_type st_vocoder_type

/* The enum type used for describing slowtalk state */
typedef enum
{
	/* ENABLED:    Slowtalk is enabled
	 * DISABLED:   SlowTalk is disabled, circular buffer is not cleared yet
	 * NO_SLOWTALK:SlowTalk completely ends, circular buffer got cleared too*/
	ENABLED = 1,
	DISABLED = 2,
	NO_SLOWTALK = 0								 
}state_type;

/* The enum type used for describing narrow or wide band working mode */
typedef enum 
{
	NARROW_BAND = 1,
	WIDE_BAND	= 2
}vocoder_type;

/* Function prototypes, the functions that'll show on API */
void	easyhearing_default_init(long  int *stParam, long  int *vadParam);
void	easyhearing_init(long  int *stParam, long  int *vadParam, void *s, void *expStruct);
void  easyhearing_close(void *s);
short int	easyhearing_enabled(void *s, short  int *inputFrameBuf, short  int *outputFrameBuf,void *expStruct, short int outputFrameBufSize);
short int   easyhearing_disabled(void *s, short  int *inputFrameBuf, short  int *outputFrameBuf, short int outputFrameBufSize);
unsigned int easyhearing_get_struct_size(void);
unsigned int easyhearing_get_exapansion_struct_size(void);

#endif // !defined(ST_H_INCLUDED_)
