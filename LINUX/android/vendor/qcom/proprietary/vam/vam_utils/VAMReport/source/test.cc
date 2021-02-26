/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <string.h>
#include <stdio.h>
#include "metadata_comparisor.h"
#include <json/json.h>
extern "C"{
#include "metadata_parser_apis.h"
#include "metadata_generator_apis.h"
}
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <ctime>
using namespace std;
#ifdef WIN32
#include <vld.h>
#endif
//#define SHOW_IMAGE
#ifdef SHOW_IMAGE
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
#endif
struct comparison{
    string ground_truth_prefix;
    string detected_prefix;
    uint64_t start_time_stamp;
    uint32_t start_frame_cnt;
};
struct config{
    string data_path;
    string data_source;
    vector<struct comparison> pairs;
};
struct object_history{
    struct metadata_rect_t loc;
    uint32_t object_id;
    int cnt_still;
};

void find_still_objects_at_beginning(char *json_file, uint32_t **still_object_ids, uint32_t *cnt_still_objects)
{
    struct object_history * object_history = new struct object_history[200];
    int obj_cnt = 0;
    void *handle;
    memset(object_history, 0, sizeof(struct object_history) * 200);
    metadata_parser_init(json_file, VAM_FILE, 0, 0, &handle);
    struct metadata_content_t *metadata;
    for (int i = 0; i < 15; i++){
        MetadataParserStatus res = metadata_parser_parse_next_frame(handle, &metadata);
        if (res != MP_OK){
            *cnt_still_objects = 0;
            delete[]object_history;
            metadata_parser_deinit(handle);
            return;
        }

        for (uint32_t k = 0; k < metadata->cnt_objects; k++){
            int found = 0;
            for (int m = 0; m < obj_cnt; m++){
                if (metadata->objects[k].object_id == object_history[m].object_id){
                    found = 1;
                    if ((metadata->objects[k].appearance_descriptor.location.x == object_history[m].loc.x) &&
                        (metadata->objects[k].appearance_descriptor.location.y == object_history[m].loc.y) &&
                        (metadata->objects[k].appearance_descriptor.location.width == object_history[m].loc.width) &&
                        (metadata->objects[k].appearance_descriptor.location.height == object_history[m].loc.height)){
                        object_history[m].cnt_still++;
                    }
                    else
                        object_history[m].cnt_still = 0;
                    break;
                }
            }
            if (found == 0){
                object_history[obj_cnt].object_id = metadata->objects[k].object_id;
                object_history[obj_cnt].loc = metadata->objects[k].appearance_descriptor.location;
                object_history[obj_cnt].cnt_still = 1;
                obj_cnt++;
            }
        }

    }
    uint32_t still_cnt = 0;
    for (int i = 0; i < obj_cnt; i++){
        if (object_history[i].cnt_still >10){
            still_cnt++;
        }
    }
    *still_object_ids = new uint32_t[still_cnt];
    int j = 0;
    for (int i = 0; i < obj_cnt; i++){
        if (object_history[i].cnt_still >10){
            (*still_object_ids)[j] = object_history[i].object_id;
            j++;
        }
    }
    *cnt_still_objects = still_cnt;
    metadata_parser_deinit(handle);
    delete[]object_history;
}
void remove_still_objects(struct metadata_content_t * metadata_frame, uint32_t * still_objs, uint32_t still_cnt)
{

    uint32_t still_cur = 0;

    for (uint32_t i = 0; i < metadata_frame->cnt_objects; i++){
        for (uint32_t j = 0; j < still_cnt; j++){
            if (metadata_frame->objects[i].object_id == still_objs[j]){
                still_cur++;
                break;
            }
        }
    }
    uint32_t new_obj_cnt = metadata_frame->cnt_objects - still_cur;
    //struct metadata_object_t *objs = new struct metadata_object_t[new_obj_cnt];
    uint32_t new_pos = 0;
    for (uint32_t i = 0; i < metadata_frame->cnt_objects; i++){
        int found_still = 0;
        for (uint32_t j = 0; j < still_cnt; j++){
            if (metadata_frame->objects[i].object_id == still_objs[j]){
                found_still++;
                break;
            }
        }
        if (found_still == 0){
            if (new_pos > new_obj_cnt)
                break;
            if (new_pos != i){
                memcpy(&metadata_frame->objects[new_pos], &metadata_frame->objects[i], sizeof(struct metadata_object_t));
            }
            new_pos++;
        }
    }
    //	memcpy(metadata_frame->objects, objs, sizeof(struct metadata_object_t)*new_obj_cnt);
    metadata_frame->cnt_objects = new_obj_cnt;
    //	delete []objs;
}

void filter_out_still_objects_from_ground_truth(char *prefix_src, char *prefix_dst, uint64_t start_time_stamp, int start_frame_cnt)
{

        void * handle1;
        void * handle2;

        uint32_t *still_object_ids = 0;
        uint32_t cnt_still_objects = 0;
        find_still_objects_at_beginning(prefix_src, &still_object_ids, &cnt_still_objects);

        metadata_parser_init(prefix_src,
            VAM_FILE,start_time_stamp,start_frame_cnt,&handle1);
        metadata_generator_init(prefix_dst,
            VAM_FILE, 18000, 1, &handle2);
        int cnt = 0;
        while (1){
            char *buf1;
//            char *buf2;
            cnt++;
            //if (cnt == 1000)
            //	break;
            MetadataParserStatus res1 = metadata_parser_get_next_frame(handle1, &buf1);
            metadata_content_t *metadata1;
            if (res1 == MP_INVALID)
                break;
            else if (res1 == MP_END_OF_SEQUENCE)
                break;
            metadata_parser_parse_metadata_frame(handle1, buf1, &metadata1);
            remove_still_objects(metadata1, still_object_ids, cnt_still_objects);
            metadata_generator_output_json(handle2, metadata1);
        }

        metadata_parser_deinit(handle1);
        metadata_generator_deinit(handle2);
    return;
}

void get_simulated_metadata_from_ground_truth(struct config config, string simulated_prefix, int random_scale)
{
    for (uint32_t i = 0; i < config.pairs.size(); i++)
    {
        void * handle1;
        void * handle2;
        string prefix1 = config.data_path + config.pairs[i].ground_truth_prefix;
        string prefix2 = config.data_path + simulated_prefix + config.pairs[i].ground_truth_prefix;
        metadata_parser_init((const char *)prefix1.c_str(),
            VAM_FILE,
            config.pairs[i].start_time_stamp,
            config.pairs[i].start_frame_cnt,
            &handle1);
        metadata_generator_init((const char *)prefix2.c_str(),
            VAM_FILE, 18000, 1, &handle2);
        int cnt = 0;
        while (1){
            char *buf1;
//            char *buf2;
            cnt++;
            //if (cnt == 1000)
            //	break;
            MetadataParserStatus res1 = metadata_parser_get_next_frame(handle1, &buf1);
            metadata_content_t *metadata1;
            if (res1 == MP_INVALID)
                break;
            else if (res1 == MP_END_OF_SEQUENCE)
                break;
            metadata_parser_parse_metadata_frame(handle1, buf1, &metadata1);
            time_t t;
            srand((unsigned)time(&t));
            for (uint32_t i = 0; i < metadata1->cnt_objects; i++)
            {
                metadata1->objects[i].appearance_descriptor.location.x += (rand() % 2 == 0 ? 1 : -1)* rand() % (metadata1->objects[i].appearance_descriptor.location.width * random_scale / 4);
                metadata1->objects[i].appearance_descriptor.location.y += (rand() % 2 == 0 ? 1 : -1)* rand() % (metadata1->objects[i].appearance_descriptor.location.height * random_scale / 4);
                if (metadata1->objects[i].appearance_descriptor.location.x < 0)
                    metadata1->objects[i].appearance_descriptor.location.x = 0;
                if (metadata1->objects[i].appearance_descriptor.location.x + metadata1->objects[i].appearance_descriptor.location.width>1920)
                    metadata1->objects[i].appearance_descriptor.location.x = 1920 - metadata1->objects[i].appearance_descriptor.location.width - 1;
                if (metadata1->objects[i].appearance_descriptor.location.y < 0)
                    metadata1->objects[i].appearance_descriptor.location.y = 0;
                if (metadata1->objects[i].appearance_descriptor.location.y + metadata1->objects[i].appearance_descriptor.location.height>1080)
                    metadata1->objects[i].appearance_descriptor.location.y = 1080 - metadata1->objects[i].appearance_descriptor.location.height - 1;
            }
            if (rand() % 10 == 5){
                metadata1->cnt_objects--;
            }
            if (rand() % 10 == 6){
                uint32_t k = metadata1->cnt_objects;
                metadata1->cnt_objects++;
                metadata1->objects[k].object_id += 1000;
                metadata1->objects[k] = metadata1->objects[k-1];
                metadata1->objects[k].appearance_descriptor.location.x += (rand() % 2 == 0 ? 1 : -1)* rand() % (metadata1->objects[k].appearance_descriptor.location.width * random_scale / 4);
                metadata1->objects[k].appearance_descriptor.location.y += (rand() % 2 == 0 ? 1 : -1)* rand() % (metadata1->objects[k].appearance_descriptor.location.height * random_scale / 4);
                if (metadata1->objects[k].appearance_descriptor.location.x < 0)
                    metadata1->objects[k].appearance_descriptor.location.x = 0;
                if (metadata1->objects[k].appearance_descriptor.location.x + metadata1->objects[k].appearance_descriptor.location.width>1920)
                    metadata1->objects[k].appearance_descriptor.location.x = 1920 - metadata1->objects[k].appearance_descriptor.location.width - 1;
                if (metadata1->objects[k].appearance_descriptor.location.y < 0)
                    metadata1->objects[k].appearance_descriptor.location.y = 0;
                if (metadata1->objects[k].appearance_descriptor.location.y + metadata1->objects[k].appearance_descriptor.location.height>1080)
                    metadata1->objects[k].appearance_descriptor.location.y = 1080 - metadata1->objects[k].appearance_descriptor.location.height - 1;

            }
                metadata_generator_output_json(handle2, metadata1);
        }

        metadata_parser_deinit(handle1);
        metadata_generator_deinit(handle2);
    }
    return ;
}
void print_matching_matrix(struct TrackingMetrics metrics)
{
    printf("\n************print_matching_matrix**********\n");
    printf("G(%d)--D(%d)\n", metrics.cnt_ground_truth_objects,
        metrics.cnt_detected_objects);
    printf("Matched  missed  falsePositive idswitched  merged  split\n");
    printf("%7d  %6d  %13d  %9d  %6d  %5d\n", metrics.id_matched,
        metrics.missed, metrics.false_positive, metrics.id_switched,
        metrics.merged, metrics.split);
}

int load_file(const char * fname, string & doc)
{

    FILE *fp = fopen(fname, "r+t");
    if (fp == NULL)
        return -1;
    int pos1 = ftell(fp);
    fseek(fp, 0, SEEK_END);
    int pos2 = ftell(fp);
    int length =pos2-pos1;
    char * buf =new char[length + 1];
    fseek(fp, 0, SEEK_SET);
    int size2 = fread(buf, 1, length, fp);
    buf[size2] = 0;
    doc = string(buf);
    fclose(fp);
    delete []buf;
    //if (size2 != length){
    //	printf("read error in loading %s\n", fname);
    //	return -1;
    //}
    return 0;
}

int parse_config(string doc, struct config * config)
{
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    config->pairs.clear();
    bool parsingSuccessful = reader.parse(doc, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse config file\n"
            << reader.getFormattedErrorMessages();
        return -1;
    }
    if (root.isMember("data_source")){
        config->data_source = root["data_source"].asString();
    }
    if (root.isMember("data_path")){
        config->data_path = root["data_path"].asString();
    }
    if (root.isMember("comparison")){

        Json::Value comparison = root["comparison"];
        Json::Value::iterator it;
        for (it = comparison.begin(); it != comparison.end(); ++it){
            struct comparison pair;
            pair.ground_truth_prefix = (*it)["ground_truth_prefix"].asString();
            pair.detected_prefix = (*it)["detected_prefix"].asString();
            pair.start_frame_cnt = (*it)["start_frame_cnt"].asInt();
            pair.start_time_stamp = (uint64_t)(*it)["start_time_stamp"].asInt64();
            config->pairs.push_back(pair);
        }
    }
    return 0;
}
int load_config(const char * fname, struct config * config)
{
    string buf;
    load_file(fname, buf);
    parse_config(buf, config);
    return 0;
}

void print_metrics(struct TrackingMetrics metrics)
{
    printf("cnt_ground_truth_objects:%8d\n", metrics.cnt_ground_truth_objects);
    printf("cnt_detected_objects:%8d\n", metrics.cnt_detected_objects);
    printf("cnt_mapped_objects:%8d\n", metrics.cnt_mapped_objects);
    printf("id_matched:%8d\n", metrics.id_matched);
    printf("missed:%8d\n", metrics.missed);
    printf("false_positive:%8d\n", metrics.false_positive);
    printf("id_switched:%8d\n", metrics.id_switched);
    printf("merged:%8d\n", metrics.merged);
    printf("split:%8d\n", metrics.split);
    printf("frame_detection_accuracy_t:%8f\n", metrics.frame_detection_accuracy_t);
}
//int compare_pets(int argc, char* argv[])
//{
//    struct config config;
//    if (argc == 3)
//    {
//        if (load_config(argv[1], &config)){
//            return -1;
//        }
//    }
//    else{
//        printf("Usage: MetadataComparison config_filename output_filename\n");
//        return -1;
//    }
//
//    //get_simulated_metadata_from_ground_truth(config,"Simulated3_",3);
//
//    char metric_name[512];
//    sprintf(metric_name, "%s.csv", argv[2]);
//    FILE *fp = fopen(metric_name, "w+t");
//    if (fp == NULL){
//        printf("Cannot open %s to write\n", metric_name);
//        return -1;
//    }
//         fprintf(fp, "ground_truth, system_output, cnt_ground_truth_objects,");
//        fprintf(fp, "cnt_detected_objects, cnt_mapped_objects, id_matched, missed, ");
//        fprintf(fp, "false_positive, id_switched, merged, split, ");
//        fprintf(fp, "sequence_frame_detection_accuracy, multi_object_tracking_accuracy, multi_object_tracking_precision, true_positive_object_rate,false_positive_object_rate\n");
//
//    for (int k = 0; k < config.pairs.size(); k++){
//        MetadataComparisor * comparisor = new MetadataComparisor(
//            (config.data_path+config.pairs[k].ground_truth_prefix).c_str(),
//            (config.data_path + config.pairs[k].detected_prefix).c_str(),
//            config.pairs[k].start_time_stamp,
//            config.pairs[k].start_frame_cnt,0);
//
//        struct metadata_content_t *metadata_content1;
//        struct metadata_content_t *metadata_content2;
//        struct metadata_content_t * metadata_frame1;
//        struct metadata_content_t * metadata_frame2;
//#ifdef SHOW_IMAGE
//        Scalar colors[] = {
//            Scalar(0, 0, 255),
//            Scalar(0, 255, 0),
//            Scalar(255, 0, 0),
//            Scalar(0, 255, 255),
//            Scalar(255, 0, 255),
//            Scalar(255, 255, 0),
//            Scalar(255, 255, 255),
//            Scalar(128, 128, 128)
//        };
//        Scalar colors2[] = {
//            Scalar(0, 0, 180),
//            Scalar(0, 180, 0),
//            Scalar(180, 0, 0),
//            Scalar(0, 180, 180),
//            Scalar(180, 0, 180),
//            Scalar(180, 180, 0),
//            Scalar(180, 180, 180),
//            Scalar(64, 64, 64)
//        };
//        namedWindow("Matching", 0);
//        Mat img;//C:\data\benchmark\PETS\PETS2014\06_04\TRK_RGB_3
//#endif
//        int cnt_content1;
//        int cnt_content2;
//        struct TrackingMetrics metrics;
//        memset(&metrics, 0, sizeof(struct TrackingMetrics));
//        int cnt = 0;
//        //Mat img(1080, 1920, CV_8UC3);
//        //VideoCapture cap("C:\\data\\benchmark\\VIRAT2.0\\Dataset\\videos_original\\VIRAT_S_000002.mp4");
//        //string path = string("C:/data/benchmark/PETS/PETS2014/06_04/TRK_RGB_3/");
//        while (1){
//            cnt++;
//            if (cnt == 100)
//                break;
//            struct metadata_content_t * metadata_frame1;
//            struct metadata_content_t * metadata_frame2;
//            int res = comparisor->LocateNextMetadataForMatching(&metadata_frame1,
//                &metadata_frame2, &metrics);
//            if (res){
//                //printf("failed to match\n");
//                break;
//            };
//            comparisor->MeasureTrackingMetrics(*metadata_frame1, *metadata_frame2, &metrics);
//            //printf("\n\n==============frame %d===============\n\n", comparisor->mapping_.frame_cnt);
//            //print_metrics(metrics);
//            //printf("---------total---------\n");
//            print_metrics(comparisor->mapping_.measure_total);
//            if (comparisor->mapping_.frame_cnt % 100 == 0)
//            {
//                //printf("frame %d\n", comparisor->mapping_.frame_cnt);
//                //print_matching_matrix(comparisor->mapping_.measure_total);
//            }
//
//            char imgpath[512];
//            char tmp_buf[512];
//            char *mm,*dd,*subfolder;
//            sprintf(imgpath, "%s../", config.data_path.c_str());
//            strcpy(tmp_buf,config.pairs[k].ground_truth_prefix.c_str());
//            mm=strtok(tmp_buf,"_");
//            dd=strtok(NULL,"_");
//            subfolder=strtok(NULL,"_");
//            subfolder=strtok(NULL,"_");
//            subfolder=strtok(NULL,"_");
//            sprintf(imgpath,"%s%s_%s/TRK_RGB_%s/",imgpath,mm,dd,subfolder);
//#ifdef SHOW_IMAGE
//            char time[20]; //1377180143859//1377180149159//1377102153296//1377180151452
//            sprintf(time, "%llu", metadata_frame1->time_stamp);
//            string imgname = string(imgpath) + string(time) + ".jpg";
//            img = imread(imgname.c_str());
//            if (img.data == 0)
//                break;
//            //	memset(img.data, 0, 1920 * 1080 * 3);
//            for (int i = 0; i < metadata_frame1->cnt_objects; i++){
//                if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
//                    continue;
//                metadata_rect_t loc = metadata_frame1->objects[i].appearance_descriptor.location;
//                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors[metadata_frame1->objects[i].object_id % 8], 3);
//                char val[5];
//                sprintf(val, "%d", metadata_frame1->objects[i].object_id);
//                putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
//                    FONT_HERSHEY_PLAIN, 2, colors[metadata_frame1->objects[i].object_id % 8], 3);
//            }
//            for (int i = 0; i < metadata_frame1->cnt_objects; i++){
//                //if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
//                //	continue;
//                metadata_rect_t loc = metadata_frame2->objects[i].appearance_descriptor.location;
//                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors2[metadata_frame1->objects[i].object_id % 8], 3);
//                char val[5];
//                sprintf(val, "%d", metadata_frame2->objects[i].object_id);
//                putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
//                    FONT_HERSHEY_PLAIN, 2, colors2[metadata_frame2->objects[i].object_id % 8], 3);
//            }
//            char val[32];
//            sprintf(val, "Frame %d", comparisor->mapping_.frame_cnt);
//            putText(img, string(val), Point(500, 50),
//                FONT_HERSHEY_PLAIN, 2, colors[2], 3);
//            cv::imshow("Matching", img);
//            int ch = waitKey(1);
//            if (ch == 'q')
//                break;
//#endif
//        }
//        MeasurementMetrics final_metrics;
//        comparisor->GetFinalMetrics(&final_metrics);
//
//    fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f\n",
//        ground_truth_prefix,
//        system_output_prefix,
//        final_metrics.cnt_ground_truth_objects,
//        final_metrics.cnt_detected_objects,
//        final_metrics.cnt_mapped_objects,
//        final_metrics.id_matched,
//        final_metrics.missed,
//        final_metrics.false_positive,
//        final_metrics.id_switched,
//        final_metrics.merged,
//        final_metrics.split,
//        final_metrics.sequence_frame_detection_accuracy,
//        final_metrics.multiple_object_tracking_accuracy,
//        final_metrics.multiple_object_tracking_precision,
//        (final_metrics.ground_truth_tracks_cnt - final_metrics.missed_ground_truth_tracks_cnt) *1.0 /
//        final_metrics.ground_truth_tracks_cnt,
//        final_metrics.false_positive_tracks_cnt *1.0 /
//        final_metrics.detected_tracks_cnt);
//        delete comparisor;
//    }
//    fclose(fp);
//    return 0;
//}
//
//int compare_virat(int argc, char* argv[])
//{
//    struct config config;
//    if (argc == 3)
//    {
//        if (load_config(argv[1], &config)){
//            return -1;
//        }
//    }
//    else{
//        printf("Usage: MetadataComparison config_filename output_filename\n");
//        return -1;
//    }
//    //get_simulated_metadata_from_ground_truth(config,"Simulated1_",3);
//
//    char metric_name[512];
//    sprintf(metric_name, "%s.csv", argv[2]);
//    FILE *fp = fopen(metric_name, "w+t");
//    if (fp == NULL){
//        printf("Cannot open %s to write\n", metric_name);
//        return -1;
//    }
//
//        fprintf(fp, "ground_truth, system_output, cnt_ground_truth_objects,");
//        fprintf(fp, "cnt_detected_objects, cnt_mapped_objects, id_matched, missed, ");
//        fprintf(fp, "false_positive, id_switched, merged, split, ");
//        fprintf(fp, "sequence_frame_detection_accuracy, multi_object_tracking_accuracy, multi_object_tracking_precision, true_positive_object_rate,false_positive_object_rate\n");
//
//    for (int k = 0; k < config.pairs.size(); k++){
//
//        int *still_object_ids = 0;
//        int cnt_still_objects;
//        char json_file[512];
//        sprintf(json_file, "%s", (config.data_path + config.pairs[k].ground_truth_prefix).c_str());
//        find_still_objects_at_beginning(json_file, &still_object_ids, &cnt_still_objects);
//        MetadataComparisor * comparisor = new MetadataComparisor(
//            (config.data_path + config.pairs[k].ground_truth_prefix).c_str(),
//            (config.data_path + config.pairs[k].detected_prefix).c_str(),
//            config.pairs[k].start_time_stamp,
//            config.pairs[k].start_frame_cnt,1,
//            still_object_ids, cnt_still_objects);
//
//        struct metadata_content_t *metadata_content1;
//        struct metadata_content_t *metadata_content2;
//        struct metadata_content_t * metadata_frame1;
//        struct metadata_content_t * metadata_frame2;
//#ifdef SHOW_IMAGE
//        Scalar colors[] = {
//            Scalar(0, 0, 255),
//            Scalar(0, 255, 0),
//            Scalar(255, 0, 0),
//            Scalar(0, 255, 255),
//            Scalar(255, 0, 255),
//            Scalar(255, 255, 0),
//            Scalar(255, 255, 255),
//            Scalar(128, 128, 128)
//        };
//        Scalar colors2[] = {
//            Scalar(0, 0, 180),
//            Scalar(0, 180, 0),
//            Scalar(180, 0, 0),
//            Scalar(0, 180, 180),
//            Scalar(180, 0, 180),
//            Scalar(180, 180, 0),
//            Scalar(180, 180, 180),
//            Scalar(64, 64, 64)
//        };
//        namedWindow("Matching", 0);
//        Mat img;//C:\data\benchmark\PETS\PETS2014\06_04\TRK_RGB_3
//#endif
//        int cnt_content1;
//        int cnt_content2;
//        struct TrackingMetrics metrics;
//        memset(&metrics, 0, sizeof(struct TrackingMetrics));
//        int cnt = 0;
//        //Mat img(1080, 1920, CV_8UC3);
//#ifdef 	SHOW_IMAGE
//        char videopath[512];
//        sprintf(videopath, "%s../videos_original/%s.mp4", config.data_path.c_str(), config.pairs[k].ground_truth_prefix.c_str());
//        VideoCapture cap(videopath);
//#endif		//string path = string("C:/data/benchmark/PETS/PETS2014/06_04/TRK_RGB_3/");
//        while (1){
//            cnt++;
//            if (cnt % 10)
//                printf(".");
//            struct metadata_content_t * metadata_frame1;
//            struct metadata_content_t * metadata_frame2;
//            int res = comparisor->LocateNextMetadataForMatching(&metadata_frame1,
//                &metadata_frame2, &metrics);
//            if (res){
//                //printf("failed to match\n");
//                break;
//            };
//            {
//                remove_still_objects(metadata_frame1, still_object_ids, cnt_still_objects);
//                //remove_still_objects(metadata_frame2, still_object_ids, cnt_still_objects);
//                metrics.cnt_ground_truth_objects = metadata_frame1->cnt_objects;
//                metrics.cnt_detected_objects = metadata_frame1->cnt_objects;
//                comparisor->mapping_.cnt_ground_truth_objects = metadata_frame1->cnt_objects;
//                comparisor->mapping_.cnt_detected_objects = metadata_frame2->cnt_objects;
//            }
//            comparisor->MeasureTrackingMetrics(*metadata_frame1, *metadata_frame2, &metrics);
//            //printf("\n\n==============frame %d===============\n\n", comparisor->mapping_.frame_cnt);
//            //print_metrics(metrics);
//        //	printf("---------total---------\n");
//            //print_metrics(comparisor->mapping_.measure_total);
//            if (comparisor->mapping_.frame_cnt % 100 == 0)
//            {
//                //printf("frame %d\n", comparisor->mapping_.frame_cnt);
//                //print_matching_matrix(comparisor->mapping_.measure_total);
//            }
//
//#ifdef SHOW_IMAGE
//            cap >> img;
//            if (img.data == 0)
//                break;
//            //while (cnt * 33 < comparisor->time_stamp_){
//            //	cap >> img;
//            //	if (img.data == 0)
//            //		break;
//            //	char val[32];
//            //	sprintf(val, "Frame %d", cnt);
//            //	putText(img, string(val), Point(500, 50),
//            //		FONT_HERSHEY_PLAIN, 2, colors[2], 3);
//            //	cv::imshow("Matching", img);
//            //	int ch = waitKey(1);
//            //	if (ch == 'q')
//            //		break;
//            //	cnt++;
//            //}
//
//            //	memset(img.data, 0, 1920 * 1080 * 3);
//            for (int i = 0; i < metadata_frame1->cnt_objects; i++){
//                if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
//                    continue;
//                metadata_rect_t loc = metadata_frame1->objects[i].appearance_descriptor.location;
//                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors[1], 3);
//                //rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors[metadata_frame1->objects[i].object_id % 8], 3);
//                char val[5];
//                sprintf(val, "%d", metadata_frame1->objects[i].object_id+1000);
//                putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
//                    FONT_HERSHEY_PLAIN, 2, colors[1], 3);
//                    //FONT_HERSHEY_PLAIN, 2, colors[metadata_frame1->objects[i].object_id % 8], 3);
//            }
//            for (int i = 0; i < metadata_frame2->cnt_objects; i++){
//                //if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
//                //	continue;
//                metadata_rect_t loc = metadata_frame2->objects[i].appearance_descriptor.location;
//                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors2[2], 3);
//                //rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors2[metadata_frame1->objects[i].object_id % 8], 3);
//                char val[5];
//                sprintf(val, "%d", metadata_frame2->objects[i].object_id);
//                putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
//                    FONT_HERSHEY_PLAIN, 2, colors2[2], 3);
//                    //FONT_HERSHEY_PLAIN, 2, colors2[metadata_frame2->objects[i].object_id % 8], 3);
//            }
//            char val[32];
//            sprintf(val, "Frame %d", comparisor->mapping_.frame_cnt);
//            putText(img, string(val), Point(500, 50),
//                FONT_HERSHEY_PLAIN, 2, colors[2], 3);
//            cv::imshow("Matching", img);
//            int ch = waitKey(1);
//            if (ch == 'q')
//                break;
//#endif
//        }
//        MeasurementMetrics final_metrics;
//        comparisor->GetFinalMetrics(&final_metrics);
//    fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f\n",
//        ground_truth_prefix,
//        system_output_prefix,
//        final_metrics.cnt_ground_truth_objects,
//        final_metrics.cnt_detected_objects,
//        final_metrics.cnt_mapped_objects,
//        final_metrics.id_matched,
//        final_metrics.missed,
//        final_metrics.false_positive,
//        final_metrics.id_switched,
//        final_metrics.merged,
//        final_metrics.split,
//        final_metrics.sequence_frame_detection_accuracy,
//        final_metrics.multiple_object_tracking_accuracy,
//        final_metrics.multiple_object_tracking_precision,
//        (final_metrics.ground_truth_tracks_cnt - final_metrics.missed_ground_truth_tracks_cnt) *1.0 /
//        final_metrics.ground_truth_tracks_cnt,
//        final_metrics.false_positive_tracks_cnt *1.0 /
//        final_metrics.detected_tracks_cnt);
//        delete comparisor;
//        if (still_object_ids)
//            delete[]still_object_ids;
//    }
//    fclose(fp);
//    return 0;
//}
//
//int compare_qcom(int argc, char* argv[])
//{
//    struct config config;
//    if (argc == 3)
//    {
//        if (load_config(argv[1], &config)){
//            return -1;
//        }
//    }
//    else{
//        printf("Usage: MetadataComparison config_filename output_filename\n");
//        return -1;
//    }
//
//    //get_simulated_metadata_from_ground_truth(config, "Simulated_", 3);
//
//    char metric_name[512];
//    sprintf(metric_name, "%s.csv", argv[2]);
//    FILE *fp = fopen(metric_name, "w+t");
//    if (fp == NULL){
//        printf("Cannot open %s to write\n", metric_name);
//        return -1;
//    }
//       fprintf(fp, "ground_truth, system_output, cnt_ground_truth_objects,");
//        fprintf(fp, "cnt_detected_objects, cnt_mapped_objects, id_matched, missed, ");
//        fprintf(fp, "false_positive, id_switched, merged, split, ");
//        fprintf(fp, "sequence_frame_detection_accuracy, multi_object_tracking_accuracy, multi_object_tracking_precision, true_positive_object_rate,false_positive_object_rate\n");
//
//    for (int k = 0; k < config.pairs.size(); k++){
//        MetadataComparisor * comparisor = new MetadataComparisor(
//            (config.data_path + config.pairs[k].ground_truth_prefix).c_str(),
//            (config.data_path + config.pairs[k].detected_prefix).c_str(),
//            config.pairs[k].start_time_stamp,
//            config.pairs[k].start_frame_cnt,1);
//
//        struct metadata_content_t *metadata_content1;
//        struct metadata_content_t *metadata_content2;
//        struct metadata_content_t * metadata_frame1;
//        struct metadata_content_t * metadata_frame2;
//#ifdef SHOW_IMAGE
//        Scalar colors[] = {
//            Scalar(0, 0, 255),
//            Scalar(0, 255, 0),
//            Scalar(255, 0, 0),
//            Scalar(0, 255, 255),
//            Scalar(255, 0, 255),
//            Scalar(255, 255, 0),
//            Scalar(255, 255, 255),
//            Scalar(128, 128, 128)
//        };
//        Scalar colors2[] = {
//            Scalar(0, 0, 180),
//            Scalar(0, 180, 0),
//            Scalar(180, 0, 0),
//            Scalar(0, 180, 180),
//            Scalar(180, 0, 180),
//            Scalar(180, 180, 0),
//            Scalar(180, 180, 180),
//            Scalar(64, 64, 64)
//        };
//        namedWindow("Matching", 0);
//        Mat img;//C:\data\benchmark\PETS\PETS2014\06_04\TRK_RGB_3
//#endif
//        int cnt_content1;
//        int cnt_content2;
//        struct TrackingMetrics metrics;
//        memset(&metrics, 0, sizeof(struct TrackingMetrics));
//        int cnt = 0;
//        //Mat img(1080, 1920, CV_8UC3);
//#ifdef 	SHOW_IMAGE
//        char videopath[512];
//        sprintf(videopath, "%s../videos_original/%s.mp4", config.data_path.c_str(), config.pairs[k].ground_truth_prefix.c_str());
//        VideoCapture cap(videopath);
//#endif		//string path = string("C:/data/benchmark/PETS/PETS2014/06_04/TRK_RGB_3/");
//        while (1){
//            if (cnt == 100)
//                break;
//            struct metadata_content_t * metadata_frame1;
//            struct metadata_content_t * metadata_frame2;
//            int res = comparisor->LocateNextMetadataForMatching(&metadata_frame1,
//                &metadata_frame2, &metrics);
//            if (res){
//                //printf("failed to match\n");
//                break;
//            };
//            comparisor->MeasureTrackingMetrics(*metadata_frame1, *metadata_frame2, &metrics);
//#ifdef DEBUG_MAPPING
//            printf("\n\n==============frame %d===============\n\n", comparisor->mapping_.frame_cnt);
//            print_metrics(metrics);
//                printf("---------total---------\n");
//            print_metrics(comparisor->mapping_.measure_total);
//#endif
//            if (comparisor->mapping_.frame_cnt % 100 == 0)
//            {
//                //printf("frame %d\n", comparisor->mapping_.frame_cnt);
//                //print_matching_matrix(comparisor->mapping_.measure_total);
//            }
//
//#ifdef SHOW_IMAGE
//            cap >> img;
//            while (cnt*33 < comparisor->time_stamp_){
//                cap >> img;
//                if (img.data == 0)
//                    break;
//                char val[32];
//                sprintf(val, "Frame %d", cnt);
//                putText(img, string(val), Point(500, 50),
//                    FONT_HERSHEY_PLAIN, 2, colors[2], 3);
//                cv::imshow("Matching", img);
//                int ch = waitKey(1);
//                if (ch == 'q')
//                    break;
//                cnt++;
//            }
//            //	memset(img.data, 0, 1920 * 1080 * 3);
//            for (int i = 0; i < metadata_frame1->cnt_objects; i++){
//                if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
//                    continue;
//                metadata_rect_t loc = metadata_frame1->objects[i].appearance_descriptor.location;
//                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors[metadata_frame1->objects[i].object_id % 8], 3);
//                char val[5];
//                sprintf(val, "%d", metadata_frame1->objects[i].object_id);
//                putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
//                    FONT_HERSHEY_PLAIN, 2, colors[metadata_frame1->objects[i].object_id % 8], 3);
//            }
//            //for (int i = 0; i < metadata_frame2->cnt_objects; i++){
//            //	//if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
//            //	//	continue;
//            //	metadata_rect_t loc = metadata_frame2->objects[i].appearance_descriptor.location;
//            //	rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors2[metadata_frame1->objects[i].object_id % 8], 3);
//            //	char val[5];
//            //	sprintf(val, "%d", metadata_frame2->objects[i].object_id);
//            //	putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
//            //		FONT_HERSHEY_PLAIN, 2, colors2[metadata_frame2->objects[i].object_id % 8], 3);
//            //}
//            char val[32];
//            sprintf(val, "Frame %d", comparisor->mapping_.frame_cnt);
//            putText(img, string(val), Point(500, 50),
//                FONT_HERSHEY_PLAIN, 2, colors[2], 3);
//            cv::imshow("Matching", img);
//            int ch = waitKey(1);
//            if (ch == 'q')
//                break;
//#endif
//            cnt++;
//
//        }
//        MeasurementMetrics final_metrics;
//        comparisor->GetFinalMetrics(&final_metrics);
//        fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f\n",
//        ground_truth_prefix,
//        system_output_prefix,
//        final_metrics.cnt_ground_truth_objects,
//        final_metrics.cnt_detected_objects,
//        final_metrics.cnt_mapped_objects,
//        final_metrics.id_matched,
//        final_metrics.missed,
//        final_metrics.false_positive,
//        final_metrics.id_switched,
//        final_metrics.merged,
//        final_metrics.split,
//        final_metrics.sequence_frame_detection_accuracy,
//        final_metrics.multiple_object_tracking_accuracy,
//        final_metrics.multiple_object_tracking_precision,
//        (final_metrics.ground_truth_tracks_cnt - final_metrics.missed_ground_truth_tracks_cnt) *1.0 /
//        final_metrics.ground_truth_tracks_cnt,
//        final_metrics.false_positive_tracks_cnt *1.0 /
//        final_metrics.detected_tracks_cnt);
//        delete comparisor;
//    }
//    fclose(fp);
//    return 0;
//}

void print_classificaiton_info(TrackingMetrics measure)
{
    printf("\nframe %d\nPerson: gt:%5d  dt:%5d  matched:%5d  missed: %5d  fp:%5d\n",
        measure.frame_id, measure.cnt_ground_truth_person, measure.cnt_detected_person,
        measure.cnt_mapped_person, measure.cnt_missed_person, measure.cnt_false_positive_person);
    printf("\nVehicle: gt:%5d  dt:%5d  matched:%5d  missed: %5d  fp:%5d\n",
        measure.cnt_ground_truth_vehicle, measure.cnt_detected_vehicle,
        measure.cnt_mapped_vehicle, measure.cnt_missed_vehicle, measure.cnt_false_positive_vehicle);
    //printf("\Face: gt:%5d  dt:%5d  matched:%5d  missed: %5d  fp:%5d\n",
    //    measure.cnt_ground_truth_face, measure.cnt_detected_face,
    //    measure.cnt_mapped_face, measure.cnt_missed_face, measure.cnt_false_positive_face);
}
int compare_metadata(int argc, char* argv[])
{
#ifdef SHOW_IMAGE
    if (argc <9)
    {
        printf("Usage: VAMReport groundtruth_prefix\
                system_output_prefix start_time start_frame mode output_name [cnt_skip] \
                [smode] [video_file] [output_video_file]\n");
        return -1;
    }
#else
    if (argc <9)
    {
        printf("Usage: VAMReport groundtruth_prefix\
 system_output_prefix start_time start_frame mode output_name [cnt_skip] \
[smode] [video_file] \n");
        return -1;
    }
#endif
    //get_simulated_metadata_from_ground_truth(config,"Simulated1_",3);
    int hide_id2 = 0;
    char output_video_name[256];
    char metrics_name[512];
    char classification_metrics_name[512];
    int  flag_classification_enabled=0;
    char ground_truth_prefix[512];
    char system_output_prefix[512];
    char video_id[512];
    video_id[0] = 0;
    uint64_t start_time_stamp;
    uint32_t start_frame_id;
    struct metadata_content_t * metadata_frame1=0;
    struct metadata_content_t * metadata_frame2=0;
    int video_enabled = 0;
    strcpy(ground_truth_prefix, argv[1]);
    strcpy(system_output_prefix, argv[2]);
    start_time_stamp = atoll(argv[3]);
    start_frame_id = atoi(argv[4]);
    int time_stamp_mode = atoi(argv[5]);
    int mode = 0;
    int cnt_skip = 0;
    if (argc >=8)
        cnt_skip = atoi(argv[7]);
    if (argc >=9)
        mode = atoi(argv[8]);
    if (argc >= 10){
        strcpy(video_id, argv[9]);//C:\data\benchmark\VIRAT2.0\Dataset\videos_original
        video_enabled = 1;
    }
    if (argc >= 11){
        strcpy(output_video_name, argv[10]);
    }
    sprintf(classification_metrics_name, "%s_classification.csv", argv[6]);
    sprintf(metrics_name, "%s.csv", argv[6]);
    if (strcmp(argv[1], argv[2]) == 0)
        hide_id2 = 1;
    FILE *fp = fopen(metrics_name, "r+t");
    if (fp == NULL){
        fp = fopen(metrics_name, "a+t");
        if (fp == 0)
        {
            printf("failed to write to %s\n", metrics_name);
            return -1;
        }
        fprintf(fp, "ground_truth, system_output, cnt_ground_truth_objects,");
        fprintf(fp, "cnt_detected_objects, cnt_mapped_objects, id_matched, missed, ");
        fprintf(fp, "false_positive, id_switched, merged, split, ground_truth_tracks_cnt, ");
        fprintf(fp, "detected_tracks_cnt, false_positive_tracks_cnt,");
        fprintf(fp, "missed_ground_truth_tracks_cnt, ");
        fprintf(fp, "sequence_frame_detection_accuracy, multi_object_tracking_accuracy,");
        fprintf(fp, " multi_object_tracking_precision, true_positive_object_rate, ");
        fprintf(fp, "false_positive_object_rate, cnt_matched_type, classification_accuracy, object_displacement, trail_smoothness, report_time, video_name\n");
        fclose(fp);
    }
    else {
        fclose(fp);
    }
    uint32_t *still_object_ids = 0;
    uint32_t cnt_still_objects=0;
    if (mode == 1){
        find_still_objects_at_beginning(ground_truth_prefix, &still_object_ids, &cnt_still_objects);
    }
    MetadataComparisor * comparisor = new MetadataComparisor(
        ground_truth_prefix, system_output_prefix,
        start_time_stamp,
        start_frame_id, time_stamp_mode,
        still_object_ids, cnt_still_objects);

#ifdef SHOW_IMAGE
    Scalar colors[] = {
        Scalar(0, 0, 255),
        Scalar(0, 255, 0),
        Scalar(255, 0, 0),
        Scalar(0, 255, 255),
        Scalar(255, 0, 255),
        Scalar(255, 255, 0),
        Scalar(255, 255, 255),
        Scalar(128, 128, 128)
    };
    Scalar colors2[] = {
        Scalar(0, 0, 180),
        Scalar(0, 180, 0),
        Scalar(180, 0, 0),
        Scalar(0, 180, 180),
        Scalar(180, 0, 180),
        Scalar(180, 180, 0),
        Scalar(180, 180, 180),
        Scalar(64, 64, 64)
    };
    Mat img;//C:\data\benchmark\PETS\PETS2014\06_04\TRK_RGB_3
#endif
    struct TrackingMetrics metrics_total;
    memset(&metrics_total, 0, sizeof(struct TrackingMetrics));
    int cnt = 0;
#ifdef 	SHOW_IMAGE
    VideoCapture cap;
    VideoWriter writer;
    if (video_enabled){
        sprintf(video_id, argv[9]);//C:\data\benchmark\VIRAT2.0\Dataset\videos_original
        cap = VideoCapture(video_id);
        if (argc >= 11){
           // namedWindow("Matching", 0);
            Size S = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
            int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));
            writer.open(output_video_name, CV_FOURCC('D', 'I', 'V', 'X'), 30, S, true);
        }

    }
#endif		//string path = string("C:/data/benchmark/PETS/PETS2014/06_04/TRK_RGB_3/");
    int res=0;
    for (int i = 0; i < cnt_skip; i++){
        //int res = comparisor->LocateNextMetadataForMatching(&metadata_frame1,
        //	&metadata_frame2, &metrics);

        if (res){
            //printf("failed to match\n");
            break;
        };
#ifdef 	SHOW_IMAGE
        if (video_enabled){
            cap >> img;
            char val[32];
            sprintf(val, "Frame %d", cnt);
            putText(img, string(val), Point(700, 50),
                FONT_HERSHEY_PLAIN, 2, colors[2], 3);
            if (argc >= 11){
                writer.write(img);
            }
            //cv::imshow("Matching", img);
            //int ch = waitKey(1);
            //if (ch == 'q')
            //    break;
            cnt++;
        }
#endif

    }
    if (res){
        //printf("failed to match\n");
        delete comparisor;
        if (still_object_ids)
            delete[]still_object_ids;
        fclose(fp);
        return -1;
    }

    while (1){
        //if (cnt == 100)
        //	break;
        if (cnt % 10 == 0)
            printf(".");
        int res = comparisor->LocateNextMetadataForMatching(&metadata_frame1,
            &metadata_frame2, &metrics_total);
        if (res){
            //printf("failed to match\n");
            break;
        };
         cnt++;
       if (metadata_frame1->time_stamp < (uint64_t)cnt_skip*33)
            continue;
        if (metadata_frame2->time_stamp < (uint64_t)cnt_skip * 33)
            continue;
        if (metadata_frame1->time_stamp > (uint64_t)cnt_skip * 33 &&
              metadata_frame2->time_stamp > (uint64_t)cnt_skip * 33)
            break;
    }
    cnt = cnt_skip;
    while (1){
        //if (cnt == 100)
        //	break;
        if (cnt % 10==0)
            printf(".");
        int res = comparisor->LocateNextMetadataForMatching(&metadata_frame1,
            &metadata_frame2, &metrics_total);
        if (res){
            //printf("failed to match\n");
            break;
        };
        if (metadata_frame1->time_stamp < (uint64_t)cnt_skip * 33)
            continue;
        if (metadata_frame2->time_stamp < (uint64_t)cnt_skip * 33)
            continue;
        if(mode==1){
            remove_still_objects(metadata_frame1, still_object_ids, cnt_still_objects);
            //remove_still_objects(metadata_frame2, still_object_ids, cnt_still_objects);
            metrics_total.cnt_ground_truth_objects += metadata_frame1->cnt_objects;
            metrics_total.cnt_detected_objects += metadata_frame2->cnt_objects;
            comparisor->mapping_.cnt_ground_truth_objects = metadata_frame1->cnt_objects;
            comparisor->mapping_.cnt_detected_objects = metadata_frame2->cnt_objects;
        }
        comparisor->MeasureTrackingMetrics(*metadata_frame1, *metadata_frame2, &metrics_total);
        comparisor->mapping_.measure_total.frame_id = metadata_frame1->time_stamp / 33;
#ifdef DEBUG_MAPPING
        if (metadata_frame1->cnt_objects>0 || metadata_frame2->cnt_objects>0)
            print_classificaiton_info(comparisor->mapping_.measure_total);
        printf("\n\n==============frame %d===============\n\n", comparisor->mapping_.frame_cnt);
        print_metrics(metrics_total);
            printf("---------total---------\n");
        print_metrics(comparisor->mapping_.measure_total);
#endif
        if (comparisor->mapping_.frame_cnt % 100 == 0)
        {
            //printf("frame %d\n", comparisor->mapping_.frame_cnt);
            //print_matching_matrix(comparisor->mapping_.measure_total);
        }

#ifdef SHOW_IMAGE
        if (video_enabled){
            cap >> img;
            if (img.data == 0)
                break;
            while (cnt * 33 < comparisor->time_stamp_){
                cap >> img;
                if (img.data == 0)
                    break;
                char val[32];
                sprintf(val, "Frame %d", cnt);
                putText(img, string(val), Point(700, 50),
                    FONT_HERSHEY_PLAIN, 2, colors[2], 3);
                if (argc >= 11){
                    writer.write(img);
                }
                //cv::imshow("Matching", img);
                //int ch = waitKey(1);
                //if (ch == 'q')
                //    break;
                cnt++;
            }

            //	memset(img.data, 0, 1920 * 1080 * 3);
            for (int i = 0; i < metadata_frame1->cnt_objects; i++){
                //if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
                //	continue;
                metadata_rect_t loc = metadata_frame1->objects[i].appearance_descriptor.location;
                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors[1], 3);
                //rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors[metadata_frame1->objects[i].object_id % 8], 3);
                char val[5];
                sprintf(val, "%d", metadata_frame1->objects[i].object_id + 1000);
                putText(img, string(val), Point(loc.x + loc.width / 3, loc.y + loc.height / 3),
                    FONT_HERSHEY_PLAIN, 2, colors[1], 3);
                //FONT_HERSHEY_PLAIN, 2, colors[metadata_frame1->objects[i].object_id % 8], 3);
            }
            for (int i = 0; i < metadata_frame2->cnt_objects; i++){
                //if (metadata_frame1->objects[i].appearance_descriptor.object_type == 0)
                //	continue;
                metadata_rect_t loc = metadata_frame2->objects[i].appearance_descriptor.location;
                rectangle(img, Rect(Point(loc.x, loc.y), Size(loc.width, loc.height)), colors2[2], 3);

                if (hide_id2 == 0){
                    char val[5];
                    sprintf(val, "%d", metadata_frame2->objects[i].object_id);
                    putText(img, string(val), Point(loc.x + loc.width / 3, loc.y - 25 /*loc.height / 3*/),
                        FONT_HERSHEY_PLAIN, 2, colors2[2], 3);
                }
                //FONT_HERSHEY_PLAIN, 2, colors2[metadata_frame2->objects[i].object_id % 8], 3);
            }
            char val[32];
            sprintf(val, "Frame %d", cnt);
            putText(img, string(val), Point(500, 50),
                FONT_HERSHEY_PLAIN, 2, colors[2], 3);
            if (argc >= 11){
                writer.write(img);
            }
            //cv::imshow("Matching", img);
            //int ch = waitKey(1);
            //if (ch == 'q')
            //    break;
        }
#endif
       // printf("frame %d: gt=%d  dt=%d\n", comparisor->mapping_.frame_cnt, comparisor->mapping_.measure_total.cnt_ground_truth_objects, comparisor->mapping_.measure_total.cnt_detected_objects);
        cnt++;
    }
    for (uint32_t i = 0; i < comparisor->mapping_.ground_truth.size(); i++){
        if (comparisor->mapping_.ground_truth[i].mapped_cnt == 0)
            comparisor->missed_ground_truth_objects_cnt_++;
    }
    MeasurementMetrics final_metrics;
    comparisor->GetFinalMetrics(&final_metrics);
    char time_str[100];
    struct config config;

    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(time_str, "%4d-%02d-%02dT%02d:%02d:%02d", timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
        timeinfo->tm_min, timeinfo->tm_sec);
    float type_accuracy = final_metrics.id_matched + final_metrics.merged>0 ? final_metrics.cnt_matched_type*1.0 / (final_metrics.id_matched + final_metrics.merged) : 0;
    type_accuracy = type_accuracy > 1 ? 1 : type_accuracy;

    fp = fopen(metrics_name, "a+t");
    if (fp == NULL){
        printf("Cannot open %s to write\n", metrics_name);
        return -1;
    }

    if (argc >= 10)
    fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6d,%6.4f,%6.4f,%6.4f,%s,%s\n",
        ground_truth_prefix,
        system_output_prefix,
        final_metrics.cnt_ground_truth_objects,
        final_metrics.cnt_detected_objects,
        final_metrics.cnt_mapped_objects,
        final_metrics.id_matched,
        final_metrics.missed,
        final_metrics.false_positive,
        final_metrics.id_switched,
        final_metrics.merged,
        final_metrics.split,
        comparisor->new_ground_truth_objects_cnt_, comparisor->new_detected_objects_cnt_,
        comparisor->new_false_positive_objects_cnt_, comparisor->missed_ground_truth_objects_cnt_,
        final_metrics.sequence_frame_detection_accuracy,
        final_metrics.multiple_object_tracking_accuracy,
        final_metrics.multiple_object_tracking_precision,
        (final_metrics.ground_truth_tracks_cnt - final_metrics.missed_ground_truth_tracks_cnt) *1.0 /
        final_metrics.ground_truth_tracks_cnt,
        final_metrics.false_positive_tracks_cnt *1.0 /
        final_metrics.detected_tracks_cnt,
        final_metrics.cnt_matched_type,
        type_accuracy,
        final_metrics.displacement,
        final_metrics.smoothness,
        time_str,
        video_id
    );
    else
        fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,\
                    %6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6d,%6.4f,%6.4f,%6.4f,%s\n",
        ground_truth_prefix,
        system_output_prefix,
        final_metrics.cnt_ground_truth_objects,
        final_metrics.cnt_detected_objects,
        final_metrics.cnt_mapped_objects,
        final_metrics.id_matched,
        final_metrics.missed,
        final_metrics.false_positive,
        final_metrics.id_switched,
        final_metrics.merged,
        final_metrics.split,
        comparisor->new_ground_truth_objects_cnt_, comparisor->new_detected_objects_cnt_,
        comparisor->new_false_positive_objects_cnt_, comparisor->missed_ground_truth_objects_cnt_,
        final_metrics.sequence_frame_detection_accuracy,
        final_metrics.multiple_object_tracking_accuracy,
        final_metrics.multiple_object_tracking_precision,
        (final_metrics.ground_truth_tracks_cnt - final_metrics.missed_ground_truth_tracks_cnt) *1.0 /
        final_metrics.ground_truth_tracks_cnt,
        final_metrics.false_positive_tracks_cnt *1.0 /
        final_metrics.detected_tracks_cnt,
        final_metrics.cnt_matched_type,
        final_metrics.id_matched>0 ? final_metrics.cnt_matched_type*1.0 / final_metrics.id_matched : 0,
        final_metrics.displacement,
        final_metrics.smoothness,
        time_str
        );
    fclose(fp);
    if (final_metrics.cnt_detected_person > 0 || final_metrics.cnt_detected_vehicle > 0 || final_metrics.cnt_detected_face > 0){
        flag_classification_enabled = 1;
    }
    else
        flag_classification_enabled = 0;
    if (flag_classification_enabled == 1){
        float person_detection_rate = 0;
        float vehicle_detection_rate = 0;
        float face_detection_rate = 0;
        float person_detection_accuracy = 0;
        float vehicle_detection_accuracy = 0;
        float face_detection_accuracy = 0;
        float person_false_positive_rate = 0;
        float vehicle_false_positive_rate = 0;
        float face_false_positive_rate = 0;
        if (final_metrics.cnt_ground_truth_person>0)
            person_detection_rate = (float)final_metrics.cnt_mapped_person / (float)final_metrics.cnt_ground_truth_person;
        if (final_metrics.cnt_ground_truth_vehicle>0)
            vehicle_detection_rate = (float)final_metrics.cnt_mapped_vehicle / (float)final_metrics.cnt_ground_truth_vehicle;
        if (final_metrics.cnt_ground_truth_face>0)
            face_detection_rate = (float)final_metrics.cnt_mapped_face / (float)final_metrics.cnt_ground_truth_face;
        if (final_metrics.cnt_detected_person>0)
            person_detection_accuracy = (float)final_metrics.cnt_mapped_person / (float)final_metrics.cnt_detected_person;
        if (final_metrics.cnt_detected_vehicle>0)
            vehicle_detection_accuracy = (float)final_metrics.cnt_mapped_vehicle / (float)final_metrics.cnt_detected_vehicle;
        if (final_metrics.cnt_detected_face>0)
            face_detection_accuracy = (float)final_metrics.cnt_mapped_face / (float)final_metrics.cnt_detected_face;
        if (final_metrics.cnt_detected_person>0)
            person_false_positive_rate = (float)final_metrics.cnt_false_positive_person / (float)final_metrics.cnt_detected_person;
        if (final_metrics.cnt_detected_vehicle>0)
            vehicle_false_positive_rate = (float)final_metrics.cnt_false_positive_vehicle / (float)final_metrics.cnt_detected_vehicle;
        if (final_metrics.cnt_detected_face>0)
            face_false_positive_rate = (float)final_metrics.cnt_false_positive_face / (float)final_metrics.cnt_detected_face;

        FILE *fp = fopen(classification_metrics_name, "r+t");
        if (fp == NULL){
            fp = fopen(classification_metrics_name, "a+t");
            if (fp == 0)
            {
                printf("failed to write to %s\n", metrics_name);
                return -1;
            }
            fprintf(fp, "ground_truth, system_output, cnt_ground_truth_person,");
            fprintf(fp, "cnt_detected_person, cnt_mapped_person, cnt_missed_person, ");
            fprintf(fp, "cnt_false_positive_person, cnt_ground_truth_vehicle,");
            fprintf(fp, "cnt_detected_vehicle, cnt_mapped_vehicle, cnt_missed_vehicle, ");
            fprintf(fp, "cnt_false_positive_vehicle, cnt_ground_truth_face,");
            fprintf(fp, "cnt_detected_face, cnt_mapped_face, cnt_missed_face, ");
            fprintf(fp, "cnt_false_positive_face, person_detection_rate, person_detection_accuracy, person_false_positive_rate, vehicle_detection_rate, vehicle_detection_accuracy, vehicle_false_positive_rate,  face_detection_rate, face_detection_accuracy, face_false_positive_rate, report_time, video_name\n");
            fclose(fp);
        }
        else {
            fclose(fp);
        }

        fp = fopen(classification_metrics_name, "a+t");
        if (fp == NULL){
            printf("Cannot open %s to write\n", classification_metrics_name);
            return -1;
        }
        if (argc >= 10){
            fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%s,%s\n",
                ground_truth_prefix,
                system_output_prefix,
                final_metrics.cnt_ground_truth_person,
                final_metrics.cnt_detected_person,
                final_metrics.cnt_mapped_person,
                final_metrics.cnt_missed_person,
                final_metrics.cnt_false_positive_person,
                final_metrics.cnt_ground_truth_vehicle,
                final_metrics.cnt_detected_vehicle,
                final_metrics.cnt_mapped_vehicle,
                final_metrics.cnt_missed_vehicle,
                final_metrics.cnt_false_positive_vehicle,
                final_metrics.cnt_ground_truth_face,
                final_metrics.cnt_detected_face,
                final_metrics.cnt_mapped_face,
                final_metrics.cnt_missed_face,
                final_metrics.cnt_false_positive_face,
                person_detection_rate, person_detection_accuracy,  person_false_positive_rate,
                vehicle_detection_rate, vehicle_detection_accuracy,
                vehicle_false_positive_rate, face_detection_rate,
                face_detection_accuracy, face_false_positive_rate,
                time_str, video_id
                );
            fclose(fp);
        }
        else{
            fprintf(fp, "%s,%s,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%s\n",
                ground_truth_prefix,
                system_output_prefix,
                final_metrics.cnt_ground_truth_person,
                final_metrics.cnt_detected_person,
                final_metrics.cnt_mapped_person,
                final_metrics.cnt_missed_person,
                final_metrics.cnt_false_positive_person,
                final_metrics.cnt_ground_truth_vehicle,
                final_metrics.cnt_detected_vehicle,
                final_metrics.cnt_mapped_vehicle,
                final_metrics.cnt_missed_vehicle,
                final_metrics.cnt_false_positive_vehicle,
                final_metrics.cnt_ground_truth_face,
                final_metrics.cnt_detected_face,
                final_metrics.cnt_mapped_face,
                final_metrics.cnt_missed_face,
                final_metrics.cnt_false_positive_face,
                person_detection_rate, person_detection_accuracy, person_false_positive_rate,
                vehicle_detection_rate, vehicle_detection_accuracy,
                vehicle_false_positive_rate, face_detection_rate,
                face_detection_accuracy, face_false_positive_rate, time_str
                );
            fclose(fp);
        }
    }
    printf("\n");
    delete comparisor;
    if (still_object_ids)
        delete[]still_object_ids;
    return 0;
}
//void doubleframerate(char * fname_in, char * fname_out)
//{
//    VideoCapture cap(fname_in);
//    VideoWriter writer;
//    Size S = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
//        (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
//    writer.open(fname_out, CV_FOURCC('D', 'I', 'V', 'X'), 30, S, true);
//    while (1){
//        Mat image;
//        cap >> image;
//        if (image.data == 0)
//            break;
//        writer.write(image);
//        writer.write(image);
//    }
//}
int main(int argc, char* argv[])
{
    //doubleframerate("C:/benchmark/videos_original/pedestrian_detection.avi", "C:/benchmark/videos_original/pedestrian_detection30.avi");
    //compare_pets(argc, argv);
    compare_metadata(argc, argv);
    //compare_virat(argc, argv);
    //compare_qcom(argc, argv);
    return 0;
}
