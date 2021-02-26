/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2012-2016,2017 QUALCOMM Technologies, Incorporated.         *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   epd.h           TYPE: C-header file                          *]
[* DESCRIPTION: Public header file for the End-Point-Detection algorithm.    *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   2012-06-16 kyuwoong Initial revision.                                   *]
[*   2012-08-30 acho Modified to use fixed point                             *]
[*   2012-10-11 kyou   Interface revision 2                                  *]
[*****************************************************************************/

#ifndef EPD_H_
#define EPD_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AEEStdDef.h"

/*----------------------------------------------------------------------------
* Type Declarations for overall configuration and state data structures
* -------------------------------------------------------------------------*/

enum {
	GAP = 50,
	EDGE_N = 25,
	HALF_EDGE_N = 12,
	EPD_NOT_READY = -1,
	MAX_DURATION = 200,

};
enum {
	kEpdSilence = 1, 
	kEpdInSpeech = 2, 
	kEpdEndSpeech = 3, 
	kEpdLeavingSpeech = 4, 
	kEpdGap = 5 
};

enum {
	EPD_SUCCESS = 0,
	// ERROR Start
	EPD_NEEDMORE = 2,
};

/*
@brief data structure of Epd
*/
typedef struct
{

//	for QSIM
	int16 nBeginThresholdL16Q5;		//Speech begin threshold
	int16 nEndThresholdL16Q5;		//Speech end threshold

	int16 epdState;					//current state of EPD
	int8 bIsReady;					//EPD is initialized?
	int16 nEnergyIdx;				//current index of energy ring buffer
	int16 nGap;
	int16 aEnergyBuffer[EDGE_N];	//energy ring buffer to compute Epd

	
} EpdDataType;

/*
@breif Initialize EDP algorithm 

Performs initialization of data structure for the EPD algorithm.

@param pData : [in] pointer to Epd data structure
@param nBeginThresholL16Q5 : [in] speech begin threshold
@param nEndThresholdL16Q5 : [in] speech end threshold
*/

int Epd_init(EpdDataType *pData,
			 int16 nBeginThresholdL16Q5,
			 int16 nEndThresholdL16Q5);


int Epd_reinit(EpdDataType *pData);

/*
@breif Process EPD computation and figure out current state of Epd

Perform EPD algorithm and find out what state it is.

@param pData : [in] pointer to Epd data structure
@param energyL16Q8 : [in] current energy value L16 in Q8
@param pEpdResult : [out] result code of Epd
*/

int Epd_process(EpdDataType *pData,
				int16 energyL16Q8,
				int16 *pEpdResult);


#endif /* EPD_H_ */
