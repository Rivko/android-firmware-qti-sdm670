/*============================================================================
  Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
============================================================================*/

#ifndef __ACD_MODEL_PARSER_H__
#define __ACD_MODEL_PARSER_H__

#include <stdint.h>
#include <acd_model_attribute_ids.h>
#include <acd_model_parser_utils.h>
#include <acd_model_parser_qfactors.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
	ACD_PARSER_SUCCESS = 0,
	ACD_PARSER_ERROR = 1,
	ACD_PARSER_NOT_SUPPORTED_VERSION,
	ACD_PARSER_ID_NOT_EXIST,
	ACD_PARSER_NOT_ALIGNED_SIZE,
	ACD_PARSER_SIZE_MISSMATCH,
	ACD_PARSER_VALUE_MISSMATCH,
	ACD_PARSER_REF_UNINIT_VALUE,
	ACD_PARSER_OUT_OF_RANGE,
	ACD_PARSER_WRONG_MODEL,
	ACD_PARSER_WRONG_VALUE,
};

enum {
	ACD_MODEL_VERSION = 2,
	ACD_GLOBAL_HEADER_MARKER = 0xB0A4CCD0,
	ACD_CONFIGURATION_MARKER = 0xB0ACCCD0,
	ACD_DETECTION_PHYSICAL_STATE_MARKER = 0xB0A4CCDF,
	ACD_CHANGE_PHYSICAL_STATE_MARKER = 0xB0A4CED0,
	ACD_SOUND_MODEL_MARKER = 0xFBA4CCD0,
	ACD_MODEL_METAINFO_MARKER = 0xFCACCCDF,
};

enum {
	ACD_FEATURE_KIND_MFCC = 0x0001,
	ACD_FEATURE_KIND_MFCC_MVA = 0x0010,
	ACD_FEATURE_KIND_MFCC_MRA = 0x0100,
};

enum {
	ACD_ACTIVE_FUNC_NONE = 0,
	ACD_ACTIVE_FUNC_MAX = 1,
	ACD_ACTIVE_FUNC_RELU = 2,
};

typedef struct _AudioContextGlobalHeaderType {
	uint32_t magicNumber;	//Magic number
	uint32_t payloadBytes;	//Payload Size
	uint32_t modelVersion;
} AudioContextGlobalHeaderType;

typedef struct _AudioContextHeaderType {
	uint32_t configurationOffset;
	uint32_t detectionPhysicalStateOffset;
	uint32_t changePhysicalStateOffset;
	uint32_t soundModelOffset;
	uint32_t metaInfoOffset;
} AudioContextHeaderType;

typedef struct _AudioContextConfigurationType {
	uint32_t configurationMarker;
	uint32_t featureKindBitSet;
	uint16_t numSoundModels;
	uint16_t numDetectionSensitivities;
	uint16_t numTrackingSensitivities;
	uint16_t frameSkipRate;
	uint32_t trackingEnableBitSet;
	int32_t forgettingFactor;
	uint16_t numMaxProcessingFrame;
	uint16_t numMaxCrosslikelihoodProcessingFrames;
	uint16_t rejectionWindowSize;
	uint16_t reserved1;
} AudioContextConfigurationType;

typedef struct _MaxActiveFunctionType {
	uint16_t paramOffset;
	uint16_t paramLength;
	uint16_t numOutputNodes;
	uint16_t reserved1;
} MaxActiveFunctionType;

typedef struct _RELUActiveFunctionType {
	uint64_t reserved;
} RELUActiveFunctionType;

typedef union _ActiveFunctionType {
	MaxActiveFunctionType maxActiveFunction;
	RELUActiveFunctionType RELUActiveFunction;
} ActiveFunctionType;

typedef struct _NeuralNetworkLayerType {
	uint16_t nrow;
	uint16_t ncol;
	uint16_t weightOffset;
	uint16_t activeFunctionType;
	ActiveFunctionType activeFunctions;
} NeuralNetworkLayerType;

typedef struct _DetectionPhysicalStatesType {
	uint32_t physicalStateMarker;					// marker about physical state
	uint16_t numGraphs;
	uint16_t numTotalLayers;
	uint32_t numTotalWeights;
	uint16_t numTotalActiveFuncParams;
	uint16_t weightQFactor;
	uint16_t *numLayers;
	uint16_t *numOutputNodes;
	NeuralNetworkLayerType *layers;
	int16_t *weights;
	uint16_t *activeFuncParams;
} DetectionPhysicalStatesType;

typedef struct _ChangePhysicalStatesType {
	uint32_t physicalStateMarker;					// marker about physical state
	uint16_t numStates;								// number of states
	uint16_t numTotalGauss;							// number of all Gaussian kernels inside the model
	uint16_t numDimension;							// dimension of Gaussian kernels
	uint16_t reserved1;								// reserved
	uint16_t reserved2;								// reserved
	uint16_t reserved3;								// reserved
	uint32_t *kernelLength;							// lengths of the each Gaussian kernel of each state
	int32_t *wgconst;								// weight + gconst
	int16_t *means;									// means
	int16_t	*invars;								// inverse variance
} ChangePhysicalStatesType;

typedef struct _NextTurnOnTimeType {
	uint16_t detected;
	uint16_t changed;
	uint16_t rejected;
	uint16_t timeout;
	uint16_t quickRejected;
	uint16_t reserved1;
	uint16_t reserved2;
	uint16_t reserved3;
} NextTurnOnTimeType;

typedef struct _RejectionType {
	int16_t lowThreshold;
	int16_t highThreshold;
	uint16_t decisionLength;
	uint16_t gap;
} RejectionType;

typedef struct _SoundModelType {
	uint32_t soundModelMarker;
	uint16_t numSpellCharacters;
	uint16_t reserved1;
	NextTurnOnTimeType *nextTurnOnTimes;
	RejectionType *rejections;
	RejectionType *trackingRejections;
	uint32_t *featureKind;
	int32_t *modelAdaptRatio;
	int32_t *adaptSamplingRate;
	uint16_t *detectionDecisionLength;
	uint16_t *trackingDecisionLength;
	uint16_t *detectionMaxDecisionLength;
	uint16_t *trackingMaxDecisionLength;
	uint16_t *soundSpellLength;
	char *spells;
	int16_t *detectionThresholds;
	int16_t *trackingThresholds;
	uint16_t *numTargetStatesPerSound;
	uint16_t *physicalStateIndexForTarget;
	uint16_t *numFillerStatesPerSound;
	uint16_t *physicalStateIndexForFiller;
	uint16_t *numTrackingStatesPerSound;
	uint16_t *physicalStateIndexForTracking;
	uint16_t *trackingAdaptationLength;
	int16_t *quickRejectionThreshold;
} SoundModelType;

typedef struct _ModelMetaInfoType {
	uint32_t modelMetaInfoMarker;
	uint16_t descriptionLength;
	char* description;
} ModelMetaInfoType;

typedef struct _AudioContextModelType {
	AudioContextGlobalHeaderType globalHeader;
	AudioContextHeaderType header;
	AudioContextConfigurationType configuration;
	DetectionPhysicalStatesType detectionPhysicalStates;
	ChangePhysicalStatesType changePhysicalStates;
	SoundModelType soundModel;
	ModelMetaInfoType metaInfo;
 } AudioContextModelType;


/**
@brief create ACD model
*/
AudioContextModelType *acd_model_create(void);

/**
@brief release ACD model
*/
void acd_model_destroy(AudioContextModelType *model);

/**
@brief decode to the ACD model structure from serialized memory
*/
int acd_model_decoder(uint8_t *buffer, AudioContextModelType *model);

/**
@brief used by encoder to set the non-array type value into the ACD model
*/
int acd_model_set_value(AudioContextModelType *model, int attribute_id, void *value);

/**
@brief used by encoder to set the array type value into the ACD model
*/
int acd_model_set_array(AudioContextModelType *model, int attribute_id, void *values, int size);

/**
@brief serialize from encoded ACD model structure
*/
int acd_model_serializer(AudioContextModelType *model, uint8_t *buffer);

/**
@brief used by encoder and serializer to estimate serializing SVA model size
*/
int acd_model_get_size(AudioContextModelType *model);

// redefine
typedef AudioContextModelType			ACD_ModelType;
typedef SoundModelType					ACD_SoundModelType;
typedef AudioContextGlobalHeaderType	ACD_GlobalHeaderType;
typedef AudioContextHeaderType			ACD_HeaderType;
typedef AudioContextConfigurationType	ACD_ConfigureType;
typedef DetectionPhysicalStatesType		ACD_DetectionPhysicalStateType;
typedef ChangePhysicalStatesType		ACD_ChangePhysicalStateType;

#ifdef __cplusplus
};
#endif

#endif //__ACD_MODEL_PARSER_H__