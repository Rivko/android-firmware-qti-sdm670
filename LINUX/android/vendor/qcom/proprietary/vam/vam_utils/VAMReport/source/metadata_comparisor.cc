/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

// metadata_comparison.cpp : Defines the entry point for the console application.
//
//#define DEBUG_MAPPING

#include "metadata_comparisor.h"

extern "C"{
#include "metadata_parser_apis.h"
}
#include <vector>
#include <string.h>
#include <stdio.h>
#include <math.h>
using namespace std;
MetadataComparisor::MetadataComparisor() :
    frame_cnt_(0),
    new_ground_truth_objects_cnt_(0),
    new_detected_objects_cnt_(0),
    new_false_positive_objects_cnt_(0),
    missed_ground_truth_objects_cnt_(0),
    handle1_(0),
    handle2_(0)
{
    //memset(&mapping_, 0, sizeof(GroundtruthDetectedMapping));
    mapping_.frame_cnt = 0;
    mapping_.overlap_ratio_table = 0;
    mapping_.overlap_ratio_table_hungarian = 0;
    mapping_.detected_mapping = 0;
    mapping_.ground_truth_mapping = 0;
    mapping_.cnt_matched_type = 0;
    mapping_.cnt_unknown_type = 0;
    mapping_.measure_total.frame_detection_accuracy = 0;
    mapping_.measure_total.frame_detection_accuracy_t = 0;
    mapping_.measure_total.multiple_object_tracking_accuracy = 0;
    mapping_.measure_total.multiple_object_tracking_accuracy2 = 0;
    mapping_.measure_total.multiple_object_tracking_precision = 0;
    mapping_.measure_total.sequence_frame_detection_accuracy = 0;

    mapping_.measure_total.cnt_mapped_objects = 0;
    mapping_.measure_total.cnt_ground_truth_objects = 0;
    mapping_.measure_total.cnt_detected_objects = 0;
    mapping_.measure_total.cnt_ground_truth_person = 0;
    mapping_.measure_total.cnt_ground_truth_vehicle = 0;
    mapping_.measure_total.cnt_ground_truth_face = 0;
    mapping_.measure_total.cnt_detected_person = 0;
    mapping_.measure_total.cnt_detected_vehicle = 0;
    mapping_.measure_total.cnt_detected_face = 0;
    mapping_.measure_total.cnt_mapped_person = 0;
    mapping_.measure_total.cnt_mapped_vehicle = 0;
    mapping_.measure_total.cnt_mapped_face = 0;
    mapping_.measure_total.cnt_false_positive_person = 0;
    mapping_.measure_total.cnt_false_positive_vehicle = 0;
    mapping_.measure_total.cnt_false_positive_face = 0;
    mapping_.measure_total.cnt_missed_person = 0;
    mapping_.measure_total.cnt_missed_vehicle = 0;
    mapping_.measure_total.cnt_missed_face = 0;
    mapping_.measure_total.false_positive = 0;
    mapping_.measure_total.id_matched = 0;
    mapping_.measure_total.type_matched = 0;
    mapping_.measure_total.id_switched = 0;
    mapping_.measure_total.merged = 0;
    mapping_.measure_total.missed = 0;
    mapping_.measure_total.split = 0;
}
MetadataComparisor::MetadataComparisor(const char * ground_truth_filename,
                                       const char * detected_filename,
                                       uint64_t start_time_stamp,
                                       int start_frame_id,
                                       int matching_mode,
                                       uint32_t * still_objs,
                                       uint32_t still_cnt)
    : frame_cnt_(0),
      new_ground_truth_objects_cnt_(0),
      new_detected_objects_cnt_(0),
      new_false_positive_objects_cnt_(0),
      missed_ground_truth_objects_cnt_(0),
      handle1_(0),
      handle2_(0),
      matching_mode_(matching_mode)
{
    Init(ground_truth_filename, detected_filename, start_time_stamp, start_frame_id);
    still_objs_ = still_objs;
    still_cnt_ = still_cnt;
#if DEBUG_OUTPUT
    fp_debug = fopen("debug.txt", "w+t");
#endif
}
MetadataComparisor::~MetadataComparisor(){
    if (mapping_.detected_mapping)
        delete []mapping_.detected_mapping;
    if (mapping_.ground_truth_mapping)
        delete []mapping_.ground_truth_mapping;
    if (mapping_.overlap_ratio_table)
        delete[]mapping_.overlap_ratio_table;
    if (mapping_.overlap_ratio_table_hungarian)
        delete[]mapping_.overlap_ratio_table_hungarian;
    mapping_.detected.clear();
    mapping_.ground_truth.clear();
    if (handle1_)
        metadata_parser_deinit(handle1_);
    if (handle2_)
        metadata_parser_deinit(handle2_);
    assignment.clear();
#if DEBUG_OUTPUT
    fclose(fp_debug);
#endif
}
int MetadataComparisor::Init(
    const char * ground_truth_filename,
    const char * detected_filename,
    uint64_t start_time_stamp,
    int start_frame_id){
    //memset(&mapping_, 0, sizeof(GroundtruthDetectedMapping));
    mapping_.frame_cnt = 0;
    mapping_.overlap_ratio_table = 0;
    mapping_.overlap_ratio_table_hungarian = 0;
    mapping_.detected_mapping = 0;
    mapping_.ground_truth_mapping = 0;
    mapping_.cnt_matched_type = 0;
    mapping_.cnt_unknown_type = 0;
    mapping_.measure_total.frame_detection_accuracy = 0;
    mapping_.measure_total.frame_detection_accuracy_t = 0;
    mapping_.measure_total.multiple_object_tracking_accuracy = 0;
    mapping_.measure_total.multiple_object_tracking_precision = 0;
    mapping_.measure_total.sequence_frame_detection_accuracy = 0;
    mapping_.measure_total.cnt_mapped_objects = 0;
    mapping_.measure_total.cnt_ground_truth_objects = 0;
    mapping_.measure_total.cnt_detected_objects = 0;
    mapping_.measure_total.cnt_ground_truth_person = 0;
    mapping_.measure_total.cnt_ground_truth_vehicle = 0;
    mapping_.measure_total.cnt_ground_truth_face = 0;
    mapping_.measure_total.cnt_detected_person = 0;
    mapping_.measure_total.cnt_detected_vehicle = 0;
    mapping_.measure_total.cnt_detected_face = 0;
    mapping_.measure_total.cnt_mapped_person = 0;
    mapping_.measure_total.cnt_mapped_vehicle = 0;
    mapping_.measure_total.cnt_mapped_face = 0;
    mapping_.measure_total.cnt_false_positive_person = 0;
    mapping_.measure_total.cnt_false_positive_vehicle = 0;
    mapping_.measure_total.cnt_false_positive_face = 0;
    mapping_.measure_total.cnt_missed_person = 0;
    mapping_.measure_total.cnt_missed_vehicle = 0;
    mapping_.measure_total.cnt_missed_face = 0;
    mapping_.measure_total.cnt_detected_objects = 0;
    mapping_.measure_total.false_positive = 0;
    mapping_.measure_total.id_matched = 0;
    mapping_.measure_total.type_matched = 0;
    mapping_.measure_total.id_switched = 0;
    mapping_.measure_total.merged = 0;
    mapping_.measure_total.missed = 0;
    mapping_.measure_total.split = 0;
    metadata_parser_init(ground_truth_filename, VAM_FILE, start_time_stamp, start_frame_id, &handle1_);
    metadata_parser_init(detected_filename, VAM_FILE, start_time_stamp, start_frame_id, &handle2_);
    return 0;
}

int MetadataComparisor::LocateNextMetadataForMatching(
    struct metadata_content_t ** metadata_frame1,//ground_truth
    struct metadata_content_t ** metadata_frame2,//detected
    struct TrackingMetrics * metrics)
{
    MetadataParserStatus res1 /*= metadata_parser_get_next_frame(handle1, &buf1)*/;
    MetadataParserStatus res2 /*= metadata_parser_get_next_frame(handle2, &buf2)*/;
    metadata_content_t *metadata1;
    metadata_content_t *metadata2;
    int frame_cnt = frame_cnt_;
    res1 = metadata_parser_parse_next_frame(handle1_, &metadata1);
    res2=metadata_parser_parse_next_frame(handle2_, &metadata2);
/////just for test. need to be removed when time stamp mismatch issue is fixed  --Lei
////

    if (res1 != MP_OK|| res2 != MP_OK)
        return -1;
    if (matching_mode_ == 1)
    {
        metadata1->time_stamp = frame_cnt_ * 33;
        metadata2->time_stamp = frame_cnt_ * 33;
    }
    memset(metrics, 0, sizeof(TrackingMetrics));
    while (metadata1->time_stamp<metadata2->time_stamp ){
        //all ground truth objects in this frame missed from detected metadata frames
        metrics->missed += metadata1->cnt_objects;
        metrics->cnt_ground_truth_objects += metadata1->cnt_objects;
        AddTrackingMetrics(metrics);
        for (uint32_t i = 0; i < metadata1->cnt_objects; i++){
            if (metadata1->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_ground_truth_person++;
            if (metadata1->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_ground_truth_vehicle++;
            if (metadata1->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_ground_truth_face++;
            if (metadata1->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_missed_person++;
            if (metadata1->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_missed_vehicle++;
            if (metadata1->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_missed_face++;
        }
        res1 = metadata_parser_parse_next_frame(handle1_, &metadata1);
        //metadata_parser_parse_metadata_frame(handle1, buf1, &metadata1);
        if (res1 != MP_OK)
            return -1;
        frame_cnt_++;
    }
    while (metadata1->time_stamp > metadata2->time_stamp ){
        //all the objects detected in this frame are false positives
        //memset(metrics, 0, sizeof(TrackingMetrics));
        metrics->false_positive += metadata2->cnt_objects;
        metrics->cnt_detected_objects += metadata2->cnt_objects;
        for (uint32_t i = 0; i < metadata2->cnt_objects; i++){
            if (metadata2->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_detected_person++;
            if (metadata2->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_detected_vehicle++;
            if (metadata2->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_detected_face++;
            if (metadata2->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_false_positive_person++;
            if (metadata2->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_false_positive_vehicle++;
            if (metadata2->objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_false_positive_face++;
        }
        AddTrackingMetrics(metrics);
        res2=metadata_parser_parse_next_frame(handle2_, &metadata2);
        if (res2 != MP_OK)
            return -1;
        frame_cnt_++;
    }
    time_stamp_ = metadata1->time_stamp;
    //memset(metrics, 0, sizeof(TrackingMetrics));
    metrics->cnt_ground_truth_objects += metadata1->cnt_objects;
    metrics->cnt_detected_objects += metadata2->cnt_objects;
    mapping_.cnt_ground_truth_objects = metadata1->cnt_objects;
    mapping_.cnt_detected_objects = metadata2->cnt_objects;
    mapping_.frame_cnt = frame_cnt;
    *metadata_frame1 = metadata1;
    *metadata_frame2 = metadata2;
    mapping_.frame_cnt=frame_cnt;
#if DEBUG_OUTPUT
    static int total_gt = 0;
    static int total_so = 0;
    total_gt += metrics->cnt_ground_truth_objects;
    total_so += metrics->cnt_detected_objects;
    fprintf(fp_debug, "frame %d  %d--%d  (%d,%d)\n", frame_cnt, metrics->cnt_ground_truth_objects, metrics->cnt_detected_objects, total_gt, total_so);
#endif
    frame_cnt_++;
    return 0;

}

void MetadataComparisor::UpdateTracks(
    struct metadata_content_t metadata,
    enum ComparisorMetadataType type)
{
    std::vector<struct Track> tracks;
    if (type == METADATA_GROUND_TRUTH)
        tracks = mapping_.ground_truth;
    else
        tracks = mapping_.detected;
    std::vector<struct Track> new_tracks;
    if ((tracks).size() > 0){
        std::vector<struct Track>::iterator it;
        for (it = (tracks).begin(); it != (tracks).end(); it++){
            (*it).updated = 0;
        }
    }
    for (uint32_t i = 0; i < metadata.cnt_objects; i++){
        int found = 0;
        if (tracks.size() > 0){
            std::vector<struct Track>::iterator it;
            for (it = tracks.begin(); it != tracks.end(); it++){
                if (metadata.objects[i].object_id == (uint32_t)it->object_id){
                    it->frame_id_last_tracked = mapping_.frame_cnt;
                    it->location =
                        metadata.objects[i].appearance_descriptor.location;
                    if (it->status != TRACK_TRACKED){
                        it->status = TRACK_TRACKED;
                        it->status_history = 1;
                        it->location_pre = it->location;
                        it->location_est = it->location;
                    }
                    else{
                        it->status_history++;
                        if (type == METADATA_DETECTED){
                            metadata_rect_t rect1, rect2, rect3;
                            rect1 = it->location;
                            rect2 = it->location_est;
                            rect3 = it->location_pre;

                            int xc1 = rect1.x + rect1.width / 2;
                            int yc1 = rect1.y + rect1.height / 2;
                            int xc2 = rect2.x + rect2.width / 2;
                            int yc2 = rect2.y + rect2.height / 2;
                            int xc3 = rect3.x + rect3.width / 2;
                            int yc3 = rect3.y + rect3.height / 2;
                            int dx = xc1 - xc3;
                            int dy = yc1 - yc3;
                            it->location_est.x += dx;
                            it->location_est.y += dy;
                            it->location_est.width = (it->location_pre.width + it->location.width) / 2;
                            it->location_est.height = (it->location_pre.height + it->location.height) / 2;
                            double dist = sqrt((xc1 - xc2)*(xc1 - xc2) + (yc1 - yc2)*(yc1 - yc2));
                            running_statistics.Push(dist);
                            it->location_pre = it->location;
                        }
                    }
                    it->updated = 1;
                    found = 1;
                    break;
                }
            }
        }
        if (found == 0)
        {
            struct Track new_track;
            memset(&new_track, 0, sizeof(struct Track));
            new_track.mapped_cnt = 0;
            new_track.frame_id_last_tracked = mapping_.frame_cnt;
            new_track.location =
                metadata.objects[i].appearance_descriptor.location;
            new_track.object_id = metadata.objects[i].object_id;
            new_track.status = TRACK_TRACKED;
            new_track.status_history = 0;
            new_track.updated = 1;
            new_track.mapped_id = -1;
            new_track.location_pre = new_track.location;
            new_track.location_est = new_track.location;
            new_tracks.push_back(new_track);

            if (type == METADATA_GROUND_TRUTH){
                new_ground_truth_objects_cnt_++;
            }
            else
                new_detected_objects_cnt_++;
        }
    }
    std::vector<struct Track>::iterator it = tracks.begin();
    for (it = tracks.begin(); it != tracks.end();){
        int flag_erase = 0;
        if (it->updated == 0){
            if (it->status != TRACK_LOST){
                it->status_history = 1;
                it->status = TRACK_LOST;
            }
            else{
                it->status_history++;
                if (it->status_history > TRACK_LOST_MAX){
                    if (it->mapped_cnt == 0 && type == METADATA_GROUND_TRUTH)
                        missed_ground_truth_objects_cnt_++;
                    it = tracks.erase(it);
                    flag_erase = 1;
                }
            }
        }
        if (flag_erase == 0)
            it++;
    }
    for (uint32_t i = 0; i < new_tracks.size(); i++)
        tracks.push_back(new_tracks[i]);
    new_tracks.clear();
    if (type == METADATA_GROUND_TRUTH)
        mapping_.ground_truth = tracks;
    else
        mapping_.detected = tracks;
}
void MetadataComparisor::CalculateOverlapRatio(
    metadata_object_t obj1,
    metadata_object_t obj2,
    float &overlap_ratio)
{
    int x1, x2, y1, y2;
    struct metadata_rect_t location1, location2;
    location1 = obj1.appearance_descriptor.location;
    location2 = obj2.appearance_descriptor.location;
    x1 = MAX(location1.x, location2.x);
    x2 = MIN(location1.x + location1.width-1, location2.x + location2.width-1);
    y1 = MAX(location1.y, location2.y);
    y2 = MIN(location1.y + location1.height-1, location2.y + location2.height-1);
    if (x2 < x1 || y2 < y1)
        overlap_ratio = 0;
    else{
        float common_area = (float)(x2 - x1 + 1)*(y2 - y1 + 1);
        float total_area = location1.width*location1.height +
                           location2.width*location2.height - common_area;
        overlap_ratio = common_area / total_area;
        if (overlap_ratio > 1)
            printf("!!!!overlap_ratio>1");
    }
}
void MetadataComparisor::MeasureTrackingMetrics(
    struct metadata_content_t metadata1,
    struct metadata_content_t metadata2,
    struct TrackingMetrics * metrics)
{
    int match_case = 0;
        if (metadata1.cnt_objects == 0){
            metrics->false_positive += metadata2.cnt_objects;
            metrics->cnt_detected_objects += metadata2.cnt_objects;
            match_case = 1;
            //return;
        }
        else if (metadata2.cnt_objects == 0){
            metrics->missed += metadata1.cnt_objects;
            metrics->cnt_ground_truth_objects += metadata1.cnt_objects;
            match_case = 2;
            //return;
        }
        else{

            int table_size = metadata1.cnt_objects >= metadata2.cnt_objects ?
                metadata1.cnt_objects : metadata2.cnt_objects;
            if (mapping_.overlap_ratio_table)
                delete[]mapping_.overlap_ratio_table;
            mapping_.overlap_ratio_table =
                new float[metadata1.cnt_objects*metadata2.cnt_objects];
            if (mapping_.overlap_ratio_table_hungarian)
                delete[]mapping_.overlap_ratio_table_hungarian;
            mapping_.overlap_ratio_table_hungarian =
                new float[table_size*table_size];
            for (uint32_t i = 0; i < metadata1.cnt_objects*metadata2.cnt_objects; i++)
                mapping_.overlap_ratio_table[i] = 0.;
            for (int i = 0; i < table_size*table_size; i++)
                mapping_.overlap_ratio_table_hungarian[i] = 2.;
            if (mapping_.ground_truth_mapping)
                delete[]mapping_.ground_truth_mapping;
            if (metadata1.cnt_objects > 0){
                mapping_.ground_truth_mapping =
                    new struct ObjectMapping[metadata1.cnt_objects];
                memset(mapping_.ground_truth_mapping, 0, sizeof(ObjectMapping)*metadata1.cnt_objects);
            }
            else
                mapping_.ground_truth_mapping = 0;
            if (mapping_.detected_mapping)
                delete[]mapping_.detected_mapping;
            if (metadata2.cnt_objects > 0){
                mapping_.detected_mapping =
                    new struct ObjectMapping[metadata2.cnt_objects];
                memset(mapping_.detected_mapping, 0, sizeof(ObjectMapping)*metadata2.cnt_objects);
            }
            else
                mapping_.detected_mapping = 0;
            for (uint32_t i = 0; i < metadata1.cnt_objects; i++){
                for (uint32_t j = 0; j < metadata2.cnt_objects; j++){
                    float score;
                    CalculateOverlapRatio(
                        metadata1.objects[i], metadata2.objects[j], score);
                    mapping_.overlap_ratio_table[i*metadata2.cnt_objects + j] = score;
                    mapping_.overlap_ratio_table_hungarian[i*table_size + j] = 1 - score;
                }
            }

            //greedy method

            for (uint32_t i = 0; i < metadata1.cnt_objects; i++){
                int max_id = 0;
                float max_score = 0;
                mapping_.ground_truth_mapping[i].id = metadata1.objects[i].object_id;
                mapping_.ground_truth_mapping[i].mapped_id = -1;
                mapping_.ground_truth_mapping[i].cnt_id_mapped = 0;
                for (uint32_t j = 0; j < metadata2.cnt_objects; j++){
                    if (mapping_.overlap_ratio_table[i*metadata2.cnt_objects + j] >
                        max_score){
                        max_id = j;
                        max_score =
                            mapping_.overlap_ratio_table[i*metadata2.cnt_objects + j];
                    }
                }
                if (max_score > MATCHING_TH){
                    mapping_.ground_truth_mapping[i].id =
                    metadata1.objects[i].object_id;
                    mapping_.ground_truth_mapping[i].mapped_id =
                    metadata2.objects[max_id].object_id;
                    mapping_.ground_truth_mapping[i].idx_mapped = max_id;
                    mapping_.ground_truth_mapping[i].score = max_score;
                    mapping_.ground_truth_mapping[i].cnt_id_mapped = 1;
                }
            }
            for (uint32_t i = 0; i < metadata2.cnt_objects; i++){
                int max_id = 0;
                float max_score = 0;
                mapping_.detected_mapping[i].id = metadata2.objects[i].object_id;
                mapping_.detected_mapping[i].mapped_id = -1;
                mapping_.detected_mapping[i].cnt_id_mapped = 0;
                for (uint32_t j = 0; j < metadata1.cnt_objects; j++){
                    if (mapping_.overlap_ratio_table[j*metadata2.cnt_objects + i] >
                        max_score){
                        max_id = j;
                        max_score =
                        mapping_.overlap_ratio_table[j*metadata2.cnt_objects + i];
                    }
                }
                if (max_score > MATCHING_TH){
                    mapping_.detected_mapping[i].mapped_id =
                    metadata1.objects[max_id].object_id;
                    mapping_.detected_mapping[i].id = metadata2.objects[i].object_id;
                    mapping_.detected_mapping[i].idx_mapped = max_id;
                    mapping_.detected_mapping[i].score = max_score;
                    mapping_.detected_mapping[i].cnt_id_mapped = 1;
                }
            }
        }
    UpdateTracks(metadata1, METADATA_GROUND_TRUTH);
    UpdateTracks(metadata2, METADATA_DETECTED);
    mapping_.measure_current = *metrics;
    UpdateTrackObjectMapping();
    if (match_case == 0)
        SmoothnessCheck(metadata1, metadata2, metrics);
    UpdateCurrentMetrics();
    UpdateObjectTypeMatching(metadata1, metadata2);
    *metrics = mapping_.measure_current;
    AddTrackingMetrics(metrics);
}

std::vector<struct Track> & MetadataComparisor::GetTracks(
    ComparisorMetadataType type)
{
    std::vector<struct Track> & tracks = type ==
        METADATA_GROUND_TRUTH ? mapping_.ground_truth : mapping_.detected;
    return tracks;
}

void MetadataComparisor::UpdateTrackObjectMapping()
{
    if (mapping_.cnt_detected_objects == 0 || mapping_.cnt_ground_truth_objects == 0)
        return;
    for (int i = 0; i < mapping_.cnt_ground_truth_objects; i++){
        mapping_.ground_truth_mapping[i].marked = 0;
    }
    for (uint32_t i = 0; i < mapping_.ground_truth.size(); i++){
        mapping_.ground_truth[i].mapped_cnt = 0;
    }
    for (uint32_t i = 0; i < mapping_.ground_truth.size(); i++){
        int found = 0;
        for (int j = 0; j < mapping_.cnt_ground_truth_objects; j++){
            //find matched object
            if (mapping_.ground_truth[i].object_id ==
                    mapping_.ground_truth_mapping[j].id){
                found = 1;
                if (mapping_.ground_truth_mapping[j].cnt_id_mapped > 0){
                    mapping_.ground_truth[i].mapped_cnt++;
                    if (mapping_.ground_truth[i].status == TRACK_TRACKED &&
                        (mapping_.ground_truth[i].status_history == 1 ||
                        mapping_.ground_truth[i].mapped_id==-1)){
                        mapping_.ground_truth[i].mapped_id =
                            mapping_.ground_truth_mapping[j].mapped_id;
                    }

                    if (mapping_.ground_truth[i].mapped_id !=
                            mapping_.ground_truth_mapping[j].mapped_id){
                        mapping_.measure_current.id_switched++;
                        mapping_.ground_truth_mapping[j].status = MAPPING_ID_SWITCHED;
                        mapping_.ground_truth_mapping[j].marked++;
#ifdef DEBUG_MAPPING
                        printf("MAPPING_ID_SWITCHED: %d--(%d-->%d)\n", mapping_.ground_truth[i].object_id, mapping_.ground_truth[i].mapped_id, mapping_.ground_truth_mapping[j].mapped_id);
#endif
                        mapping_.ground_truth[i].mapped_id =
                            mapping_.ground_truth_mapping[j].mapped_id;
                    }
                    else if (mapping_.ground_truth_mapping[j].marked==0){
                        mapping_.measure_current.id_matched++;
                        mapping_.ground_truth_mapping[j].marked++;
                        mapping_.ground_truth_mapping[j].status = MAPPING_MATCHED;
#ifdef DEBUG_MAPPING
                        printf("MAPPING_MATCHED: %d--%d\n", mapping_.ground_truth[i].object_id, mapping_.ground_truth[i].mapped_id);
#endif
                    }
                }
                else{
                    mapping_.measure_current.missed++;
                    mapping_.ground_truth_mapping[j].marked++;
                    mapping_.ground_truth_mapping[j].status = MAPPING_MISSED;
#ifdef DEBUG_MAPPING
                    printf("MAPPING_MISSED: %d\n", mapping_.ground_truth[i].object_id);
#endif
                }
                break;
            }
        }
    }

    for (int i = 0; i < mapping_.cnt_detected_objects; i++){
        if (mapping_.detected_mapping[i].cnt_id_mapped == 0){
            mapping_.measure_current.false_positive++;
            mapping_.detected_mapping[i].status = MAPPING_FALSE_POSITIVE;
            uint32_t k = 0;
            for (k = 0; k < mapping_.detected.size(); k++){
                if (mapping_.detected[k].object_id == mapping_.detected_mapping[i].id){
                    break;
                }
            }
            if (k< mapping_.detected.size() && mapping_.detected[k].status_history == 0){
                new_false_positive_objects_cnt_++;
                mapping_.detected[k].status_history++;
            }
#ifdef DEBUG_MAPPING
            printf("MAPPING_FALSE_POSITIVE: --%d\n", mapping_.detected_mapping[i].id);
#endif
        }
    }
    for (int i = 0; i < mapping_.cnt_ground_truth_objects; i++){
        mapping_.ground_truth_mapping[i].marked = 0;
    }
    for (int i = 0; i < mapping_.cnt_ground_truth_objects-1; i++){
        for (int j = i + 1; j < mapping_.cnt_ground_truth_objects; j++){
            if (mapping_.ground_truth_mapping[i].cnt_id_mapped >0 &&
                mapping_.ground_truth_mapping[j].cnt_id_mapped >0 &&
                (mapping_.ground_truth_mapping[i].mapped_id ==
                mapping_.ground_truth_mapping[j].mapped_id)){
                if (mapping_.ground_truth_mapping[j].marked == 0){
                    mapping_.ground_truth_mapping[j].marked = 1;
                    mapping_.measure_current.merged++;
                    mapping_.ground_truth_mapping[i].status = MAPPING_MERGED;
                    mapping_.ground_truth_mapping[j].status = MAPPING_MERGED;
#ifdef DEBUG_MAPPING
                    printf("MAPPING_MERGED: (%d,%d)--%d\n",
                        mapping_.ground_truth_mapping[i].id,
                        mapping_.ground_truth_mapping[j].id,
                        mapping_.ground_truth_mapping[j].mapped_id);
#endif
                }
            }
        }
    }
    for (int i = 0; i < mapping_.cnt_detected_objects; i++){
        mapping_.detected_mapping[i].marked = 0;
    }
    for (int i = 0; i < mapping_.cnt_detected_objects - 1; i++){
        for (int j = i + 1; j < mapping_.cnt_detected_objects; j++){
            if (mapping_.detected_mapping[i].cnt_id_mapped>0 &&
                mapping_.detected_mapping[j].cnt_id_mapped>0 &&
                mapping_.detected_mapping[i].mapped_id ==
                mapping_.detected_mapping[j].mapped_id){
                if (mapping_.detected_mapping[j].marked == 0){
                    mapping_.detected_mapping[j].marked = 1;
                        mapping_.measure_current.split++;
                    mapping_.detected_mapping[i].status = MAPPING_SPLIT;
                    mapping_.detected_mapping[j].status = MAPPING_SPLIT;
#ifdef DEBUG_MAPPING
                    printf("MAPPING_SPLIT: %d--(%d,%d)\n", mapping_.detected_mapping[i].mapped_id, mapping_.detected_mapping[i].id, mapping_.detected_mapping[j].id);
#endif
                }
            }
        }
    }
}

void MetadataComparisor::AddTrackingMetrics(struct TrackingMetrics * metrics)
{
    //mapping_.measure_history.push_back(*metrics);
    mapping_.measure_total.cnt_detected_objects += metrics->cnt_detected_objects;
    mapping_.measure_total.cnt_ground_truth_objects += metrics->cnt_ground_truth_objects;
    mapping_.measure_total.cnt_mapped_objects += metrics->cnt_mapped_objects;
    mapping_.measure_total.false_positive += metrics->false_positive;
    mapping_.measure_total.id_matched += metrics->id_matched;
    mapping_.measure_total.id_switched += metrics->id_switched;
    mapping_.measure_total.merged += metrics->merged;
    mapping_.measure_total.missed += metrics->missed;
    mapping_.measure_total.split += metrics->split;
    mapping_.measure_total.type_matched += metrics->type_matched;
}
void MetadataComparisor::UpdateCurrentMetrics()
{
    mapping_.measure_current.cnt_mapped_objects = 0;
    mapping_.measure_current.frame_detection_accuracy = 0;
    mapping_.measure_current.frame_detection_accuracy_t = 0;
    mapping_.measure_current.multiple_object_tracking_accuracy = 0;
    mapping_.measure_current.multiple_object_tracking_precision = 0;
    mapping_.measure_current.sequence_frame_detection_accuracy = 0;
    mapping_.measure_current.cnt_ground_truth_objects = mapping_.cnt_ground_truth_objects;
    mapping_.measure_current.cnt_detected_objects = mapping_.cnt_detected_objects;
    if (mapping_.cnt_ground_truth_objects > 0 && mapping_.cnt_detected_objects>0){
        for (int i = 0; i < mapping_.cnt_ground_truth_objects; i++){
            if (mapping_.ground_truth_mapping[i].score > 0){
                mapping_.measure_current.frame_detection_accuracy +=
                    mapping_.ground_truth_mapping[i].score;
                mapping_.measure_current.frame_detection_accuracy_t +=
                    mapping_.ground_truth_mapping[i].score > MATCHING_TH ?
                    1 : mapping_.ground_truth_mapping[i].score;
                mapping_.measure_current.cnt_mapped_objects++;
            }
        }
            //if ((mapping_.cnt_ground_truth_objects +
            //	mapping_.cnt_detected_objects) / 2. > 0){
            //	mapping_.measure_current.frame_detection_accuracy /=
            //		(mapping_.cnt_ground_truth_objects +
            //		mapping_.cnt_detected_objects) / 2.;
            //	mapping_.measure_current.frame_detection_accuracy_t /=
            //		(mapping_.cnt_ground_truth_objects +
            //		mapping_.cnt_detected_objects) / 2.;
            //}
            //else{
            //	mapping_.measure_current.frame_detection_accuracy = 0;
            //	mapping_.measure_current.frame_detection_accuracy_t = 0;
            //}

    }
    //FDA
    mapping_.measure_total.frame_detection_accuracy_t +=
        mapping_.measure_current.frame_detection_accuracy;
    //MOTP
    mapping_.measure_total.multiple_object_tracking_precision +=
        mapping_.measure_current.frame_detection_accuracy_t;
    //MOTA
    int error_id_switch = mapping_.measure_current.id_switched +
        mapping_.measure_current.merged + mapping_.measure_current.split;
    mapping_.measure_current.multiple_object_tracking_accuracy =
        (error_id_switch>0 ? log10(error_id_switch):0 )+
        mapping_.measure_current.missed + mapping_.measure_current.false_positive;
    mapping_.measure_total.multiple_object_tracking_accuracy +=
        mapping_.measure_current.multiple_object_tracking_accuracy;

}

void MetadataComparisor::GetFinalMetrics(struct MeasurementMetrics * metrics)
{
    metrics->sequence_frame_detection_accuracy =
        mapping_.measure_total.frame_detection_accuracy_t /
            mapping_.measure_total.cnt_ground_truth_objects;
    metrics->multiple_object_tracking_accuracy =
        1 - mapping_.measure_total.multiple_object_tracking_accuracy /
        mapping_.measure_total.cnt_ground_truth_objects;
    metrics->multiple_object_tracking_precision =
        mapping_.measure_total.multiple_object_tracking_precision /
            mapping_.measure_total.cnt_mapped_objects;
    metrics->cnt_detected_objects = mapping_.measure_total.cnt_detected_objects;
    metrics->cnt_ground_truth_objects = mapping_.measure_total.cnt_ground_truth_objects;
    metrics->cnt_mapped_objects = mapping_.measure_total.cnt_mapped_objects;
    metrics->false_positive = mapping_.measure_total.false_positive;
    metrics->id_matched = mapping_.measure_total.id_matched;
    metrics->id_switched = mapping_.measure_total.id_switched;
    metrics->merged = mapping_.measure_total.merged;
    metrics->missed = mapping_.measure_total.missed;
    metrics->split = mapping_.measure_total.split;
    metrics->ground_truth_tracks_cnt = new_ground_truth_objects_cnt_;
    metrics->detected_tracks_cnt = new_detected_objects_cnt_;
    metrics->false_positive_tracks_cnt = new_false_positive_objects_cnt_;
    metrics->missed_ground_truth_tracks_cnt = missed_ground_truth_objects_cnt_;
    metrics->smoothness = running_statistics.StandardDeviation() / 20;
    metrics->displacement = running_statistics.Mean()/20;
    metrics->smoothness = metrics->smoothness > 1 ? 1 : metrics->smoothness;
    metrics->displacement =  metrics->displacement>1?1: metrics->displacement;
    metrics->cnt_matched_type = mapping_.cnt_matched_type;
    metrics->cnt_unknown_type = mapping_.cnt_unknown_type;
    metrics->cnt_false_positive_face = mapping_.measure_total.cnt_false_positive_face;
    metrics->cnt_false_positive_person = mapping_.measure_total.cnt_false_positive_person;
    metrics->cnt_false_positive_vehicle = mapping_.measure_total.cnt_false_positive_vehicle;
    metrics->cnt_ground_truth_face = mapping_.measure_total.cnt_ground_truth_face;
    metrics->cnt_ground_truth_person = mapping_.measure_total.cnt_ground_truth_person;
    metrics->cnt_ground_truth_vehicle = mapping_.measure_total.cnt_ground_truth_vehicle;
    metrics->cnt_mapped_face = mapping_.measure_total.cnt_mapped_face;
    metrics->cnt_mapped_person = mapping_.measure_total.cnt_mapped_person;
    metrics->cnt_mapped_vehicle = mapping_.measure_total.cnt_mapped_vehicle;
    metrics->cnt_missed_face = mapping_.measure_total.cnt_missed_face;
    metrics->cnt_missed_person = mapping_.measure_total.cnt_missed_person;
    metrics->cnt_missed_vehicle = mapping_.measure_total.cnt_missed_vehicle;
    metrics->cnt_detected_face = mapping_.measure_total.cnt_detected_face;
    metrics->cnt_detected_vehicle = mapping_.measure_total.cnt_detected_vehicle;
    metrics->cnt_detected_person = mapping_.measure_total.cnt_detected_person;
#if DEBUG_OUTPUT
    fclose(fp_debug);
#endif
}
void MetadataComparisor::SmoothnessCheck(
    struct metadata_content_t metadata1,
    struct metadata_content_t metadata2,
    struct TrackingMetrics * metrics)
{
    for (int i = 0; i < mapping_.cnt_detected_objects; i++)
    {
        if (mapping_.detected_mapping[i].cnt_id_mapped > 0){
            int gt_id = mapping_.detected_mapping[i].mapped_id;
            uint32_t id = metadata1.cnt_objects;
            for (uint32_t k = 0; k < metadata1.cnt_objects; k++){
                if (metadata1.objects[k].object_id == (uint32_t)gt_id){
                    id = k;
                    break;
                }
            }
            if (id == metadata1.cnt_objects)
                continue;
//            metadata_rect_t rect1 = metadata1.objects[id].appearance_descriptor.location;
//            metadata_rect_t rect2 = metadata2.objects[i].appearance_descriptor.location;
//            int xc1 = rect1.x + rect1.width / 2;
//            int yc1 = rect1.y + rect1.height / 2;
//            int xc2 = rect2.x + rect2.width / 2;
//            int yc2 = rect2.y + rect2.height / 2;
//            double dist = sqrt((xc1 - xc2)*(xc1 - xc2) + (yc1 - yc2)*(yc1 - yc2));
      //      running_statistics.Push(dist);
        }
    }
}


void MetadataComparisor::UpdateObjectTypeMatching(
    struct metadata_content_t metadata1,
    struct metadata_content_t metadata2)
{
    for (uint32_t i = 0; i < metadata1.cnt_objects; i++){
        if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
            mapping_.measure_total.cnt_ground_truth_person++;
        else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
            mapping_.measure_total.cnt_ground_truth_vehicle++;
        else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
            mapping_.measure_total.cnt_ground_truth_face++;
    }
    for (uint32_t i = 0; i < metadata2.cnt_objects; i++){
        if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
            mapping_.measure_total.cnt_detected_person++;
        else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
            mapping_.measure_total.cnt_detected_vehicle++;
        else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
            mapping_.measure_total.cnt_detected_face++;
    }
    if (metadata1.cnt_objects == 0){
        for (uint32_t i = 0; i < metadata2.cnt_objects; i++){
            if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_false_positive_person++;
            else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_false_positive_vehicle++;
            else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_false_positive_face++;
        }
        return;
    }
    if (metadata2.cnt_objects == 0){
        for (uint32_t i = 0; i < metadata1.cnt_objects; i++){
            if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_missed_person++;
            else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_missed_vehicle++;
            else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_missed_face++;
        }
        return;
    }

    for (uint32_t i = 0; i < metadata1.cnt_objects; i++){
        if (mapping_.ground_truth_mapping[i].mapped_id != -1){
            int idx = -1;
            idx = mapping_.ground_truth_mapping[i].idx_mapped;
            if (idx == -1){

                if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                    mapping_.measure_total.cnt_missed_person++;
                else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                    mapping_.measure_total.cnt_missed_vehicle++;
                else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                    mapping_.measure_total.cnt_missed_face++;
            }
            else
                if (i != (uint32_t)mapping_.detected_mapping[idx].idx_mapped){
                    if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                        mapping_.measure_total.cnt_missed_person++;
                    else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                        mapping_.measure_total.cnt_missed_vehicle++;
                    else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                        mapping_.measure_total.cnt_missed_face++;
                }
                else if (metadata1.objects[i].appearance_descriptor.object_type == metadata2.objects[idx].appearance_descriptor.object_type)
            {
                mapping_.cnt_matched_type++;
                if ((uint32_t)mapping_.ground_truth_mapping[i].mapped_id == metadata2.objects[idx].object_id){
                    if ((uint32_t)mapping_.detected_mapping[idx].mapped_id == metadata1.objects[i].object_id){
                        if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                            mapping_.measure_total.cnt_mapped_person++;
                        else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                            mapping_.measure_total.cnt_mapped_vehicle++;
                        else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                            mapping_.measure_total.cnt_mapped_face++;
                    }
                    else{

                        if (metadata2.objects[idx].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                            mapping_.measure_total.cnt_missed_person++;
                        else if (metadata2.objects[idx].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                            mapping_.measure_total.cnt_missed_vehicle++;
                        else if (metadata2.objects[idx].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                            mapping_.measure_total.cnt_missed_face++;
                    }
                }
            }
            else
            {
                if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                    mapping_.measure_total.cnt_missed_person++;
                else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                    mapping_.measure_total.cnt_missed_vehicle++;
                else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                    mapping_.measure_total.cnt_missed_face++;
                if (metadata2.objects[idx].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                    mapping_.measure_total.cnt_false_positive_person++;
                else if (metadata2.objects[idx].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                    mapping_.measure_total.cnt_false_positive_vehicle++;
                else if (metadata2.objects[idx].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                    mapping_.measure_total.cnt_false_positive_face++;
            }
            if (idx != -1 && metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_UNKNOWN){
                mapping_.cnt_unknown_type++;
            }
        }
        else{
            if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_missed_person++;
            else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_missed_vehicle++;
            else if (metadata1.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_missed_face++;
        }
    }
    for (uint32_t i = 0; i < metadata2.cnt_objects; i++){
        if (mapping_.detected_mapping[i].mapped_id == -1){
            if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                mapping_.measure_total.cnt_false_positive_person++;
            else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                mapping_.measure_total.cnt_false_positive_vehicle++;
            else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                mapping_.measure_total.cnt_false_positive_face++;
        }
        else{
            if (i != (uint32_t)(mapping_.ground_truth_mapping[mapping_.detected_mapping[i].idx_mapped].idx_mapped)){

                if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_PEOPLE)
                    mapping_.measure_total.cnt_false_positive_person++;
                else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_VEHICLE)
                    mapping_.measure_total.cnt_false_positive_vehicle++;
                else if (metadata2.objects[i].appearance_descriptor.object_type == METADATA_CATEGORY_FACE)
                    mapping_.measure_total.cnt_false_positive_face++;
            }
        }
    }
    if (mapping_.measure_total.cnt_detected_person !=
        (mapping_.measure_total.cnt_mapped_person + mapping_.measure_total.cnt_false_positive_person))
        printf("???\n");
    if (mapping_.measure_total.cnt_detected_vehicle !=
        (mapping_.measure_total.cnt_mapped_vehicle + mapping_.measure_total.cnt_false_positive_vehicle))
        printf("??????\n");
}

void RunningStatistics::Clear()
{
    m_n = 0;
}

void RunningStatistics::Push(double x)
{
    m_n++;

    // See Knuth TAOCP vol 2, 3rd edition, page 232
    if (m_n == 1)
    {
        m_oldM = m_newM = x;
        m_oldS = 0.0;
    }
    else
    {
        m_newM = m_oldM + (x - m_oldM) / m_n;
        m_newS = m_oldS + (x - m_oldM)*(x - m_newM);

        // set up for next iteration
        m_oldM = m_newM;
        m_oldS = m_newS;
    }
}

int RunningStatistics::NumDataValues()
{
    return m_n;
}

double RunningStatistics::Mean()
{
    return (m_n > 0) ? m_newM : 0.0;
}

double RunningStatistics::Variance()
{
    return ((m_n > 1) ? m_newS / (m_n - 1) : 0.0);
}

double RunningStatistics::StandardDeviation()
{
    return sqrt(Variance());
}
