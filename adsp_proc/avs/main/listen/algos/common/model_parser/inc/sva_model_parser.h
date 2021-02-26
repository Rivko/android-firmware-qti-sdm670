/*============================================================================
  Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
============================================================================*/

#ifndef __SVA_MODEL_PARSER_H__
#define __SVA_MODEL_PARSER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define ENSEMBLE_MARKER		"#ESB#"
#define ENSEMBLE_MARKER_LEN (5)

#ifdef NON_VOICEWAKEUP_CSIM
#include "sva_model_attribute_ids.h"
#endif

#include "AEEStdDef.h"

#ifndef LISTEN_UNITTEST
#include "stringl.h"
#endif

enum {
	SVA_PARSER_SUCCESS = 0,
	SVA_PARSER_ERROR = 1,
	SVA_PARSER_NOT_SUPPORTED_VERSION,
	SVA_PARSER_ID_NOT_EXIST,
	SVA_PARSER_NOT_ALIGNED_SIZE,
	SVA_PARSER_SIZE_MISSMATCH,
	SVA_PARSER_VALUE_MISSMATCH,
	SVA_PARSER_REF_UNINIT_VALUE,
	SVA_PARSER_OUT_OF_RANGE,
	SVA_PARSER_WRONG_MODEL,
	SVA_PARSER_WRONG_VALUE,
};

enum {
	SVA_FEATUREKIND_V4_START = 0,
	SVA_FEATUREKIND_V4_END,
};

enum {
	SVA_GLOBAL_HEADER_MAGIC_NUMBER = 0x54783601,				// LISTEN01
	SVA_KEYWORD_PHYSICAL_MARKER_V4 = 0x64159386,
	SVA_USER_PHYSICAL_MARKER_V4 = 0x64159387,
	SVA_KEYWORD_MODEL_MARKER_V4 = 0x64159388,
	SVA_USER_MODEL_MARKER_V4 = 0x64159389,
};

enum {
	SVA_FEAT_TYPE_MVA = 0,
	SVA_FEAT_TYPE_MRA = 1,
};

enum {
	SVA_MAX_PAYLOAD = 1000000,
	SVA_MIN_KEYWORDS = 1,
	SVA_NUM_KEYWORD_SENSITIVITY_LEVEL = 100,
	SVA_NUM_USER_SENSITIVITY_LEVEL = 100,
	SVA_MIN_GMAP_RATIO = 0,
	SVA_MIN_GMAP_SCALE = 0,
	SVA_MIN_DURATION_SCALE = 0,
	SVA_MAX_MAX_PROCESSING_FRAME = 3,
	SVA_MIN_PHYSICAL_STATE = 0,
	SVA_MIN_NUM_END_STATE = 1,
	SVA_MAX_TRANS_PROB = 0,
	SVA_NUM_FILLER_STATES = 1,
	SVA_MAX_NUM_USERS_STATES = 1,
};

typedef struct _SVA_GlobalHeaderType {
	uint32 magicNumber;								// Magic number
	uint32 payloadBytes;							// Payload bytes size
	uint32 modelVersion;							// Model version
} SVA_GlobalHeaderType;

////////////////////////////////////////////////////////////////////////////////
// V5
////////////////////////////////////////////////////////////////////////////////
typedef struct _SVA_HeaderTypeV5 {
	uint32 featureKind;								// kind of feature
													// SVA_FEAT_TYPE_MVA: (MVA)
													// SVA_FEAT_TYPE_MRA: (MRA)
	uint32 configurationOffset;						// offset of configuration
	uint32 keywordPhysicalStateOffset;				// offset of keyword physical state
	uint32 userPhysicalStateOffset;					// offset of user physical state
	uint32 keywordModelOffset;						// offset of keyword model
	uint32 userModelOffset;							// offset of user model
	uint32 reserved;								// reserved
} SVA_HeaderTypeV5;

typedef struct _SVA_ConfigurationTypeV5 {
	uint16 numKeywords;								// number of keywords
	uint16 numUsers;								// number of users

	uint16 numKeywordSensitivityLevels;				// number of keyword sensitivity levels
	uint16 numUserSensitivityLevels;				// number of user sensitivity levels

	int32 keywordGmapScale;							// keyword gmap scale
	int32 keywordGmapRatio;							// keyword gmap ratio

	int32 userGmapScale;							// user gmap scale
	int32 userGmapRatio;							// user gmap ratio

	int16 durationScoreScale;						// duration scale [kappa_d] for the state score
	uint16 frameSkipRate;							// rate to skip the GMM computation when there remains frames t
	uint16 numMaxProcessingFrame;					// maximum number of frames to be processed at the same time
	uint16 maxSmmBufferSize;						// maximum number of frames to keep the SMM scores

	int32 forgettingFactor;							// forgetting factor of feature normalization

	uint16 numActiveUserModels;						// number of active users

	uint8 is_ensemble;								// check for ensemble model
	uint8 reserved1;								// reserved1
} SVA_ConfigurationTypeV5;

typedef struct _SVA_InnerProductPhysicalStatesTypeV5 {
	uint32 physicalStateMarker;						// marker about physical state
	uint16 numStates;								// number of states
	uint16 numTotalGauss;							// number of all Gaussian kernels inside the model
	uint16 numDimension;							// dimension of Gaussian kernels
	uint16 reserved1;								// reserved
	uint16 reserved2;								// reserved
	uint16 reserved3;								// reserved
	uint32 *gaussLength;							// offsets of the first Gaussian kernel of each state
	int16 *innerProductParams;						// Inner product params
} SVA_InnerProductPhysicalStatesTypeV5;

typedef struct _SVA_PhysicalStatesTypeV5 {
	uint32 physicalStateMarker;						// marker about physical state
	uint16 numStates;								// number of states
	uint16 numTotalGauss;							// number of all Gaussian kernels inside the model
	uint16 numDimension;							// dimension of Gaussian kernels
	uint16 reserved1;								// reserved
	uint16 reserved2;								// reserved
	uint16 reserved3;								// reserved
	uint32 *gaussLength;							// offsets of the first Gaussian kernel of each state
	int32 *weightGconst;							// log weight and the GCONST of each Gaussian kernel
	int16 *means;									// mean vector of each Gaussian kernel
	int16 *invar;									// inverse variance vector of each Gaussian kernel
} SVA_PhysicalStatesTypeV5;

typedef struct _StateTypeV5 {
	int16 prevState;								// first prev arc index. Negative means offset of prevStates
	int8 minDuration;								// negative means initial State and (-minDuration) is actual minDuration
	uint8 maxDuration;								// max duration of state.
	uint16 durationOffset;							// starting index of current duration model in duration prob array
	uint16 physicalStateIdx;						// physical state offset
} StateTypeV5;

typedef struct _ArcTypeV5 {
	uint16 prevState;								// prev state index
	int16 transProb;								// transition prob
} ArcTypeV5;

typedef struct _DurationRangeTypeV5 {
	uint16 minDuration;								// minimum duration
	uint16 maxDuration;								// maximum duration
	uint16 durationOffset;							// starting index of current duration model in duration prob array
	uint16 reserved;								// reserved
} DurationRangeTypeV5;

typedef struct _CheckpointTypeV5 {
	uint16 state;									// State
	int16 frameLengthRatio;							// frameLength from end frame
	int16 keywordStartThreshold;					// threshold for checkpoint keyword start
	int16 minThreshold;								// min threshold for checkpoint confidence
	int16 maxThreshold;								// max threshold for checkpoint confidence
	int16 diffThreshold;							// diff threshold for checkpoint confidence between checkpoints
	int32 reserved;									// reserved
} CheckpointTypeV5;

typedef struct _KeywordEndTypeV5 {
	uint16 sizePeakWindow;							// size of peak window
	uint8 thresholdPeak;							// peak threshold
	uint8 posPeakCandidate;							// candidate position of peak
	int16 endPosAdjustment;							// adjustment of end position
	uint16 maxDecisionLen;							// Max Decision Length of Keyword ending detection
} KeywordEndTypeV5;

typedef struct _UserThresholdSeedTypeV5 {
	int32 minNegativeScore;							// minimum score of negative (user verification) trials
	int32 maxNegativeScore;							// maximum score of negative (user verification) trials
	int32 meanNegativeScore;						// mean of negative (user verification) scores
	int32 stdNegativeScore;							// standard deviation of negative (user verification) scores
	int32 minPositiveScore;							// minimum score of positive (user verification) trials
	int32 maxPositiveScore;							// maximum score of positive (user verification) trials
	int32 meanPositiveScore;						// mean of positive (user verification) scores
	int32 stdPositiveScore;							// standard deviation of positive (user verification) scores
} UserThresholdSeedTypeV5;


typedef struct _SVA_KeywordModelTypeV5 {
	uint32 keywordModelMarker;						// marker about keyword model
	int16 numSpellCharacters;						// number of spell characters in the model
	int16 checkpointUnitFrameLength;				// unit frame length for the check point
	uint64 userDefinedKeywordBitFlag;				// set 1 if corresponding bit is user defined keyword

	uint16 *minDecisionLength;						// minimum number of frames for each target keyword
	uint16 *maxDecisionLength;						// maximum number of frames for each target keyword
	uint16 *keywordSpellLength;						// keyword spell length of each keyword
	char *spells;									// spells of all keywords are concatenated, and the concatenated spell
	uint16 *localeIdPerKeywords;					// TBD
	int16 *keywordThresholds;						// threshold values for keyword detection
	int16 *rejectionConfidenceThreshold;			// rejection confidence threshold for keyword
	uint16 *numStates;								// number of states associated with each target keyword
	uint16 *numPrevStates;							// number of previous states associated with each target keyword
	uint16 *numDurationScores;						// number of duration score associated with each target keyword
	uint16 *numEndStates;							// number of end states associated with each target keyword
	uint16 *numCheckpoints;							// number of check points per each keyword
	StateTypeV5 *states;							// states array
	int16 *prevStates;								// previous states array
	int16 *durationScores;							// duration scores array
	uint16 *endStates;								// end states array
	CheckpointTypeV5 *checkpoints;					// check points array
	int16 *rejectionFillerThresholds;				// rejection filler threshold for filler
	uint16 *numFillerStates;						// number of physical states per filler
	uint16 *physicialStateIdxForFiller;				// physical state index for each filler
	KeywordEndTypeV5 *keywordEnd;					// information that is required for keyword end discovery of each target keyword
	UserThresholdSeedTypeV5 *userThresholdSeeds;	// user threshold parameters of each keyword
	uint16 *rejectionWindowSize;						// rejection windows size for each keyword
} SVA_KeywordModelTypeV5;

typedef struct _SVA_UserModelTypeV5 {
	uint32 userModelMarker;							// marker about user model
	uint16 numSpellCharacters;						// number of spell characters in the model
	uint16 *userModelUpdateCounter;					// number of updates in user model per each target user
	int16 *userThresholds;							// threshold values for user verification
	uint16 *userSepllLength;						// user spell length of each user
	char *userSpells;								// spells of all users are concatenated, and the concatenated spell
	uint16 *numUserStatesPerKeywords;				// number of states per user
	uint16 *userPhysicalStateIdxForUser;			// physical state index associated with each keyword
	uint16 *numImpostorStatesPerKeywords;			// number of states per user plus default impostor model
	uint16 *userPhysicalStateIdxForImpostor;		// physical state index associated with each keyword
} SVA_UserModelTypeV5;

typedef struct _SVA_ModelTypeV5 {
	SVA_HeaderTypeV5 header;						// header structure
	SVA_ConfigurationTypeV5 configuration;			// congfiguration structre
	SVA_InnerProductPhysicalStatesTypeV5 keywordPhysicalState;	// keyword physical state structure
	SVA_PhysicalStatesTypeV5 userPhysicalState;		// user physical state structure
	SVA_KeywordModelTypeV5 keywordModel;			// keyword model
	SVA_UserModelTypeV5 userModel;					// user model
} SVA_ModelTypeV5;

////////////////////////////////////////////////////////////////////////////////
// V4
////////////////////////////////////////////////////////////////////////////////

typedef struct _SVA_HeaderTypeV4 {
	uint32 modelKind;								// Model kind (bit flag user - bit_1, keyword - bit_0)
	uint32 keywordConfigurationOffset;				// Offset about keyword configuration
	uint32 userConfigurationOffset;					// Offset about user configuration
	uint32 keywordModelOffset;						// Offset about keyword model
	uint32 fillerModelOffset;						// Offset about filler model
	uint32 userModelOffset;							// Offset about user model
	uint32 impostor_model_offset;					// Offset about impostor model
} SVA_HeaderTypeV4;

typedef struct _CheckpointTypeV4 {
	int16 state;									// State
	int16 frameLengthRatio;							// frameLength from end frame
	int16 keywordStartThreshold;					// threshold for checkpoint keyword start
	int16 minThreshold;								// min threshold for checkpoint confidence
	int16 maxThreshold;								// max threshold for checkpoint confidence
	int16 diffThreshold;							// diff threshold for checkpoint confidence between checkpoints
													// this threshold represent diff between this checkpoint and next checkpoint
	int16 confidence;								// confidence of this checkpoint ( Please note that this is read/write
	int16 padding;									// padding
} CheckpointTypeV4;

typedef struct _SVA_KeywordConfigurationTypeV4 {
	// configurations
	uint16 numKeywords;								// number of keywords, maybe one
	uint16 minDecisionLength;						// do not make any decision before this (in # of frames)
	uint16 maxDecisionLength;						// keyword can be no longer than this (in # of frames)
	uint16 numSensitivityLevels;					// number of keyword sensitivity levels
	int16 rejectionConfidenceThreshold;				// rejection confidence threshold for keyword
	int16 rejectionFillerThreshold;					// rejection filler threshold for filler
	uint32 padding1;								// padding
	int16 *keywordThresholds;						// keyword threshold array ([keyword0_sensitivity0, keyword0_sensitivity1, .....])
	int16 durationScale;							// duration score scale factor
	int16 beamThreshold;							// search pruning threshold
	uint32 padding2;								// padding
	int32 gmapScale;								// keyword gmap scale
	int32 gmapRatio;								// keyword gmap ratio
	uint32 numCheckpoints;							// number of checkpoints
	uint32 checkpointFrameLength;					// unit frame length of checkpoints
	CheckpointTypeV4 *checkpoints;					// checkpoint array
} SVA_KeywordConfigurationTypeV4;

typedef struct _SVA_UserConfigurationTypeV4 {
	uint16 number_users;							// number of users
	uint16 maximum_decision_length;					// determines the feature accumulation buffer size
	uint16 number_sensitivity_levels;				// number of sensitivity levels
	uint16 padding1;								// padding
	int16 *user_thresholds;							// user threshold array ([user0_sensitivity0, user0_sensitivity1, .....])
	int32 gmap_scale;								// user gmap scale
	int32 gmap_ratio;								// user gmap ratio
} SVA_UserConfigurationTypeV4;

typedef struct _StateTypeV4 {
	int16 prevArcIdx;								// first prev arc index
	int16 numPrevArcs;								// number of prev arcs (consecutively stored)
	int16 gmmIdx;									// corresponding gmm index
	int16 padding;									// padding
} StateTypeV4;

typedef struct _ArcTypeV4 {
	int16 prevState;								// prev state index
	int16 transProbs;								// transition prob
} ArcTypeV4;

typedef struct _DurationRangeTypeV4{
	int16 minDuration;								// minimum duration
	int16 maxDuration;								// maximum duration
	int16 durationOffset;							// starting index of current duration model in duration prob array
	int16 padding;									// padding
} DurationRangeTypeV4;

typedef struct _QuantizedGmmTypeV4 {
	int16 numGmms;									// number of GMMs
	int16 numGausses;								// number of all Gaussians
	int16 numMixtures;								// number of mixtures per GMMs
	int16 numSubVectors;							// number of subvector
	int16 quantizationTableSize;					// quantization Table Size
	uint8 padding1[6];								// padding


	int16 *dimReorderTable;							// dimension reorder table
	int16 *subVectorDimTable;						// subvector dimension table (for future use)
	int16 *subVectorStartTable;						// subvector starting point in original dimension (for future use)

	uint8 *gaussianIdxMapEntry;						// starting point of GMM pool
	int16 *quantizedGaussianMapEntry;				// quantized GMM
} QuantizedGmmTypeV4;

typedef struct _GmmTypeV4 {
	uint32 numGaussians;							// number of all Gaussians
	uint32 padding;									// padding

	uint8 *gaussians;								// #Gaussian[1] [weight(2)][gconst(2)][padding(4)][mean(2) & inv(2)] * MULTIPLE4(DIM)
													// #Gaussian[2] [weight(2)][gconst(2)][padding(4)][mean(2) & inv(2)] * MULTIPLE4(DIM)
													// ...
													// #Gaussian[n] n means number of all Gaussians
} GmmTypeV4;

typedef struct _SVA_KeywordModelTypeV4 {
	uint32 keywordModelMarker;						// marker about keyword model
	uint32 quantizedGmmOffset;						// unused

	// search network
	uint16 numStates;								// number of states
	uint16 maxDuration;								// longest duration (needed for both buffer setup and smm frame advance)
	uint16 numArcs;									// number of arcs
	uint16 numDurationProbs;						// number of duration likelihood
	uint16 numEndstates;							// number of end states (keyword end states)
	uint8 padding1[6];								// padding
	uint16 numSpells;								// number of keyword spells
	uint8 padding2[6];								// padding
	int16 *keywordSpellOffset;						// spell offset array
	uint8 *keywordSpell;							// keyword spells
	StateTypeV4 *states;							// states array
	ArcTypeV4 *arcs;								// arcs array
	int16 *durationProbs;							// [state_0_duration_1, state_0_duration_2, ...]
	DurationRangeTypeV4 *duration_range;			// duration model

	int16 *initialProbs;							// initial state probability
	int16 *endStates;								// keyword end states
	int16 *keywordIds;								// mapping from the keyword end state to the keyword id

	QuantizedGmmTypeV4 keywordGmm;					// keyword quantized GMM data structure
} SVA_KeywordModelTypeV4;

typedef struct _SVA_FillerModelTypeV4 {
	QuantizedGmmTypeV4 fillerGmm;					// filler quantized GMM data structure
} SVA_FillerModelTypeV4;

typedef struct _SVA_UserModelTypeV4 {
	uint32 userModelMarker;							// marker about user model
	uint32 userModelUpdateCounter;					// model update count
	GmmTypeV4 userGmm;								// user GMM data structure
} SVA_UserModelTypeV4;

typedef struct _SVA_ImpostorModelTypeV4 {
	uint32 impostorModelMarker;						// marker about impostor model
	uint32 impostorModelUpdateCounter;				// model update count
	GmmTypeV4 impostorGmm;							// user GMM data structure
} SVA_ImpostorModelTypeV4;

typedef struct _SVA_ModelTypeV4 {
	SVA_HeaderTypeV4 header;								// header structure
	SVA_KeywordConfigurationTypeV4 keywordConfiguration;	// keyword configuration structure
	SVA_UserConfigurationTypeV4 userConfiguration;			// user configuration structure
	SVA_KeywordModelTypeV4 keywordModel;					// keyword model structure
	SVA_FillerModelTypeV4 fillerModel;						// filler model structure
	SVA_UserModelTypeV4 userModel;							// user model structure
	SVA_ImpostorModelTypeV4 impostorModel;					// imposter model structure
} SVA_ModelTypeV4;

////////////////////////////////////////////////////////////////////////////////
// COMMON
////////////////////////////////////////////////////////////////////////////////

// all of model versions about SVA
typedef enum _SVA_ModelVersion {
	SVA_MODEL_V4 = 4,
	SVA_MODEL_V5 = 5,
} SVA_ModelVersion;

// universial SVA model structure
typedef struct _SVA_ModelType {

	SVA_GlobalHeaderType header;					// global header

	union _sva_model {
		SVA_ModelTypeV4 SVAModelV4;					// SVA model V4 structure
		SVA_ModelTypeV5 SVAModelV5;					// SVA model V5 structure
													// ...
	} SVAModel;
} SVA_ModelType;

/**
@brief decode to the SVA model structure from serialized memory
*/
int sva_model_decoder(uint8 *buffer, SVA_ModelType *model);


/*
@brief find ensemble model index from keyword index
*/
void sva_model_find_ensemble_idx(SVA_ConfigurationTypeV5 *config,
								 SVA_KeywordModelTypeV5 *keyword_model,
								 int keyword_idx,
								 int16 *slave_indices);

#ifdef NON_VOICEWAKEUP_CSIM
/**
@brief create SVA model through heap allocation
*/
SVA_ModelType *sva_model_create(SVA_ModelVersion svaModelVersion);

/**
@brief destroy SVA model from heap allocation
*/
void sva_model_destroy(SVA_ModelType *pModel);

/**
@brief used by encoder, and migration to set the non-array type value into the SVA model
*/
int sva_model_set_value(SVA_ModelType *model, int attribute_id, void *value);

/**
@brief used by encoder, and migration to set the array type value into the SVA model
*/
int sva_model_set_array(SVA_ModelType *model, int attribute_id, void *values, int size);

/**
@brief serialize from encoded SVA model structure
*/
int sva_model_serializer(SVA_ModelType *model, uint8 *buffer);

/**
@brief used by encoder, merger, serializer, and migration to estimate serializing SVA model size
*/
int sva_model_get_size(SVA_ModelType *model);

/**
@brief merge the two SVA models
*/
int sva_model_merger(SVA_ModelType *model1, SVA_ModelType *model2, SVA_ModelType *merged_model);


/**
@brief migrate the SVA model
*/
int sva_model_migration(SVA_ModelType *to, SVA_ModelType *from);

/*
@brief make the SVA ensemble model
*/
int sva_model_make_ensemble(SVA_ModelType **models, uint32 num_models, SVA_ModelType *ensemble);

#endif //#ifdef NON_VOICEWAKEUP_CSIM

#if defined(__cplusplus)
};
#endif

#endif

