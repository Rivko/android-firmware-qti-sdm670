/*
  Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 */

#ifndef __ACD_MODEL_PARSER_QFACTORS_H__
#define __ACD_MODEL_PARSER_QFACTORS_H__

enum {
	ACD_Q_FEATURE = 11,				// feature q factor
	ACD_Q_FORGET_FACTOR = 31,		// Forget factor q factor
	ACD_Q_RA_FACTOR = 15,
	ACD_Q_GAIN = 8,					// audio gain q factor
	ACD_Q_VOTING = 15,				// voting q factor
	ACD_Q_TRACKING = 6,				// tracking threshold q factor
	ACD_Q_REJECTION = 6,			// no decision yet
	ACD_Q_ADAPTATION_TAU = 16,		// adaptation TAU
	ACD_Q_ADAPTATION_RATIO = 31,	// adaptation ratio q factor
	ACD_Q_WGCONST = 4,
	ACD_Q_MEAN = 11,
	ACD_Q_INVAR = 7,
	ACD_Q_INNER_PRODUCT_WEIGHTS = 4,
	ACD_Q_DNN_WEIGHTS = 9,
	ACD_Q_REJECTION_THRESHOLD = 6,
	ACD_Q_FRAME_SKIP_RATE = 0,
	ACD_Q_FORGETTING_FACTOR = 31,
	ACD_Q_DETECTION_THRESHOLD = 15,
	ACD_Q_TRACKING_THRESHOLD = 6,
	ACD_Q_DETECTION_CONFIDENCE = 6,
};

#endif /* __ACD_MODEL_PARSER_QFACTORS_H__ */
