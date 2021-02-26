/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_COMPARISON_H_
#define METADATA_COMPARISON_H_
//#define DEBUG_OUTPUT 1
#include <stdint.h>
#include <vector>
#include "metadata_types.h"
extern "C"{
#include "json_metadata.h"
}
#define  MAX(a, b) ((a > b)? a : b)
#define  MIN(a, b) ((a < b)? a : b)
#define TRACK_LOST_MAX (450)
#define MATCHING_TH (0.1)


enum ComparisorMetadataType{
	METADATA_GROUND_TRUTH,
	METADATA_DETECTED
};
enum TrackingStatus{
	TRACK_EMPTY,
	TRACK_TRACKED,
	TRACK_LOST
};
enum MappingStatus{
	MAPPING_UNMAPPED = 0,
	MAPPING_MATCHED,
	MAPPING_MISSED,
	MAPPING_FALSE_POSITIVE,
	MAPPING_ID_SWITCHED,
	MAPPING_MERGED,
	MAPPING_SPLIT,
	MAPPING_STATUS_MAX
};

struct MeasurementMetrics{
	int32_t cnt_ground_truth_objects;
	int32_t cnt_detected_objects;
    int32_t cnt_mapped_objects;
    int32_t cnt_matched_type;
    int32_t cnt_ground_truth_person;
    int32_t cnt_detected_person;
    int32_t cnt_false_positive_person;
    int32_t cnt_missed_person;
    int32_t cnt_mapped_person;
    int32_t cnt_ground_truth_vehicle;
    int32_t cnt_detected_vehicle;
    int32_t cnt_mapped_vehicle;
    int32_t cnt_false_positive_vehicle;
    int32_t cnt_missed_vehicle;
    int32_t cnt_ground_truth_face;
    int32_t cnt_detected_face;
    int32_t cnt_mapped_face;
    int32_t cnt_false_positive_face;
    int32_t cnt_missed_face;
    int cnt_unknown_type;
	float sequence_frame_detection_accuracy; //SFDA
	float multiple_object_tracking_accuracy; // MOTA;
	float multiple_object_tracking_precision; // MOTP;
    float displacement;
    float smoothness;
	int32_t missed;
	int32_t false_positive;
	int32_t split;
	int32_t merged;
	int32_t id_switched;
	int32_t id_matched;
    int32_t ground_truth_tracks_cnt;
    int32_t true_positive_detected_tracks_cnt;
    int32_t detected_tracks_cnt;
    int32_t false_positive_tracks_cnt;
    int32_t missed_ground_truth_tracks_cnt;
};
struct TrackingMetrics{
	int32_t frame_id;
	int32_t missed;
	int32_t false_positive;
	int32_t split;
	int32_t merged;
	int32_t id_switched;
	int32_t id_matched;
    int32_t type_matched;
	int32_t cnt_ground_truth_objects;
    int32_t cnt_ground_truth_person;
    int32_t cnt_ground_truth_vehicle;
    int32_t cnt_ground_truth_face;
	int32_t cnt_detected_objects;
    int32_t cnt_detected_person;
    int32_t cnt_detected_vehicle;
    int32_t cnt_detected_face;
    int32_t cnt_mapped_objects;
    int32_t cnt_mapped_person;
    int32_t cnt_mapped_vehicle;
    int32_t cnt_mapped_face;
    int32_t cnt_false_positive_person;
    int32_t cnt_false_positive_vehicle;
    int32_t cnt_false_positive_face;
    int32_t cnt_missed_person;
    int32_t cnt_missed_vehicle;
    int32_t cnt_missed_face;
	float frame_detection_accuracy; //FDA
	float sequence_frame_detection_accuracy; //SFDA
	float frame_detection_accuracy_t; //FDA_nonbinary thresholding
	float multiple_object_tracking_accuracy; // MOTA;
    float multiple_object_tracking_accuracy2; // MOTA;
	float multiple_object_tracking_precision; // MOTP;
};

struct ObjectMapping{
	int id;
	int mapped_id;
	int idx_mapped;
	float score;
	int cnt_id_mapped;
	int marked;
	enum MappingStatus status;
};

struct Track{
    int object_type;
	int object_id;
	int mapped_id;
	int frame_id_last_tracked;
    int mapped_cnt;
    int live_cnt;
	//struct ObjectMapping mapping;
	enum TrackingStatus status;
	int status_history;
	int updated;
	struct metadata_rect_t location;
    struct metadata_rect_t location_est;
    struct metadata_rect_t location_pre;
};

struct GroundtruthDetectedMapping{
	int cnt_ground_truth_objects;
	int cnt_detected_objects;
    int cnt_matched_type;
    int cnt_unknown_type;
	std::vector<struct Track> ground_truth;
	std::vector<struct Track> detected;
	float *overlap_ratio_table;
	float *overlap_ratio_table_hungarian;
	struct ObjectMapping *ground_truth_mapping;
	struct ObjectMapping *detected_mapping;
    std::vector< struct TrackingMetrics> measure_history;
	struct TrackingMetrics measure_current;
	struct TrackingMetrics measure_total;
	int frame_cnt;
};

//Ling, Robert F. (1974). Comparison of Several Algorithms for Computing Sample
//Means and Variances. Journal of the American Statistical Association, Vol. 69,
//No. 348, 859-866

class RunningStatistics
{
public:
    RunningStatistics() : m_n(0) {}

    void Clear();

    void Push(double x);

    int NumDataValues() ;

    double Mean() ;

    double Variance() ;

    double StandardDeviation() ;

private:
    int m_n;
    double m_oldM, m_newM, m_oldS, m_newS;
};

class MetadataComparisor{
public:
	MetadataComparisor();
	MetadataComparisor(
		const char * ground_truth_filename,
		const char * detected_filename,
		uint64_t start_time_stamp,
		int start_frame_id,
		int matching_mode=1,
		uint32_t * still_objs = 0,
		uint32_t still_cnt = 0);
	~MetadataComparisor();
	int Init(
		const char * ground_truth_filename,
		const char * detected_filename,
		uint64_t start_time_stamp,
		int start_frame_id);

	void CalculateOverlapRatio(
			metadata_object_t obj1,
			metadata_object_t obj2,
			float &overlap_ratio);

	void MeasureTrackingMetrics(
		struct metadata_content_t metadata1,
		struct metadata_content_t metadata2,
		struct TrackingMetrics * metrics);

		int LocateNextMetadataForMatching(
		struct metadata_content_t ** metadata_frame1,
		struct metadata_content_t ** metadata_frame2,
		struct TrackingMetrics * metrics);

	void UpdateTracks(
	    struct metadata_content_t metadata,
		enum ComparisorMetadataType type);
	std::vector<struct Track> & GetTracks(ComparisorMetadataType type);
	void UpdateTrackObjectMapping();
	void AddTrackingMetrics(struct TrackingMetrics * metrics);
	void CalculateFinalMetrics();
    void SmoothnessCheck(
        struct metadata_content_t metadata1,
        struct metadata_content_t metadata2,
        struct TrackingMetrics * metrics);
	void UpdateCurrentMetrics();
    void UpdateObjectTypeMatching(
        struct metadata_content_t metadata1,
        struct metadata_content_t metadata2);
	void GetFinalMetrics(struct MeasurementMetrics * metrics);
	std::vector<int> assignment;
	struct GroundtruthDetectedMapping mapping_;
	uint64_t time_stamp_;
	int frame_cnt_;
    int new_ground_truth_objects_cnt_;
    int new_detected_objects_cnt_;
    int new_false_positive_objects_cnt_;
    int missed_ground_truth_objects_cnt_;
    RunningStatistics running_statistics;
private:
#if DEBUG_OUTPUT
    FILE *fp_debug;
#endif
	uint32_t *still_objs_;
	uint32_t still_cnt_;
	void * handle1_;
	void * handle2_;
	int matching_mode_; //0: using time_stamp; 1: using frame_cnt;

};

#endif
