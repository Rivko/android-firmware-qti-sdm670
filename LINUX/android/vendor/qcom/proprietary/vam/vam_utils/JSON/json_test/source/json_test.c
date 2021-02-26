/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifdef WIN32
#include <vld.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json_metadata.h"
#include "metadata_parser_apis.h"
#include "metadata_generator_apis.h"
#include "event_generator_apis.h"
#include "event_parser_apis.h"
#include "configuration_generator_apis.h"
#include "json_common_types.h"
#include "configuration_parser_apis.h"

#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif

void newUUID(char *uuid_str)
{
#ifdef WIN32
    UUID uuid0;
    UuidCreate(&uuid0);
    unsigned char * str;
    UuidToStringA(&uuid0, &str);

    //std::string s((char*)str);
    strcpy(uuid_str, (char*)str);
//	RpcStringFreeA(str);
#else
    char s[37];
    uuid_t uuid;
    uuid_generate_random(uuid);


    uuid_unparse(uuid, s);
    strcpy(uuid_str, (char*)s);
#endif
    return;
}

//metadata generation
void JsonMetadataGeneratorTest()
{
    void *metadata_generator_handle;
    metadata_generator_init("vamtest_", VAM_FILE, 150, 120, &metadata_generator_handle);
    struct metadata_uuid_t camera_id;
    strcpy(camera_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
    uint64_t time_stamp = 1444799739000;
    struct metadata_content_t metadata_content;
    metadata_generator_initialize_metadata_content(metadata_generator_handle, &metadata_content);
    strcpy(metadata_content.camera_id.uuid, camera_id.uuid);
    metadata_content.time_stamp = time_stamp;
    {
        strcpy(metadata_content.objects[0].engine_type, "object_tracker");
        metadata_content.objects[0].object_id = 101;
        metadata_content.objects[0].appearance_descriptor.location.x = 1000;
        metadata_content.objects[0].appearance_descriptor.location.y = 2000;
        metadata_content.objects[0].appearance_descriptor.location.width = 500;
        metadata_content.objects[0].appearance_descriptor.location.height = 1200;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.physical_width =
            750;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.physical_height =
            1755;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.moving_dir =
            METADATA_DIR_UP;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.moving_speed =
            3.1F;
        metadata_content.objects[0].appearance_descriptor.object_type = METADATA_CATEGORY_PEOPLE;
        metadata_content.objects[0].appearance_descriptor.object_type_confidence = 80;
        metadata_content.objects[0].on_event.status = 1;
        metadata_content.objects[0].on_event.num_events = 2;
        strcpy(metadata_content.objects[0].on_event.event_ids[0].uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
        strcpy(metadata_content.objects[0].on_event.event_ids[1].uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");
    }
    {
        strcpy(metadata_content.objects[1].engine_type, "object_tracker");
        metadata_content.objects[1].object_id = 101;
        metadata_content.objects[1].appearance_descriptor.location.x = 7000;
        metadata_content.objects[1].appearance_descriptor.location.y = 8000;
        metadata_content.objects[1].appearance_descriptor.location.width = 1500;
        metadata_content.objects[1].appearance_descriptor.location.height = 1600;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.physical_width = 2750;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.physical_height = 2755;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.moving_dir = METADATA_DIR_RIGHT;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.moving_speed = 3.1F;
        metadata_content.objects[1].appearance_descriptor.object_type = METADATA_CATEGORY_VEHICLE;
        metadata_content.objects[1].appearance_descriptor.object_type_confidence = 70;
        metadata_content.objects[1].on_event.status = 1;
        metadata_content.objects[1].on_event.num_events = 1;
        strcpy(metadata_content.objects[1].on_event.event_ids[0].uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    }
    {
        strcpy(metadata_content.objects[2].engine_type, "face_recognizer");
        metadata_content.objects[2].object_id = 301;
        metadata_content.objects[2].appearance_descriptor.location.x = 1000;
        metadata_content.objects[2].appearance_descriptor.location.y = 2000;
        metadata_content.objects[2].appearance_descriptor.location.width = 500;
        metadata_content.objects[2].appearance_descriptor.location.height = 1200;
        metadata_content.objects[2].appearance_descriptor.object_type = METADATA_CATEGORY_FACE;
        metadata_content.objects[2].on_event.status = 0;
        metadata_content.objects[2].on_event.num_events = 0;
        metadata_content.objects[2].reserve[0] = 90;
        strcpy(metadata_content.objects[2].reserve_str[0], "Person ID of this person");
        metadata_content.objects[2].appearance_descriptor.object_type_confidence = (uint8_t)metadata_content.objects[2].reserve[0];
    }
    {
        metadata_content.object_trees[0].action = METADATA_OBJECT_TREE_MERGED;
        metadata_content.object_trees[0].num_from = 3;
        metadata_content.object_trees[0].num_to = 1;
        metadata_content.object_trees[0].from[0] = 100;
        metadata_content.object_trees[0].from[1] = 101;
        metadata_content.object_trees[0].from[2] = 102;
        metadata_content.object_trees[0].to[0] = 100;
    }
    {
        metadata_content.object_trees[1].action = METADATA_OBJECT_TREE_SPLIT;
        metadata_content.object_trees[1].num_from = 1;
        metadata_content.object_trees[1].num_to = 2;
        metadata_content.object_trees[1].from[0] = 100;
        metadata_content.object_trees[1].to[0] = 100;
        metadata_content.object_trees[1].to[1] = 105;
    }
    {
        metadata_content.object_trees[2].action = METADATA_OBJECT_TREE_CREATED;
        metadata_content.object_trees[2].num_from = 0;
        metadata_content.object_trees[2].num_to = 1;
        metadata_content.object_trees[2].to[0] = 100;
    }
    {
        metadata_content.object_trees[3].action = METADATA_OBJECT_TREE_DELETED;
        metadata_content.object_trees[3].num_from = 1;
        metadata_content.object_trees[3].num_to = 0;
        metadata_content.object_trees[3].from[0] = 100;
    }
    {
        metadata_content.object_trees[4].action = METADATA_OBJECT_TREE_RENAMED;
        metadata_content.object_trees[4].num_from = 1;
        metadata_content.object_trees[4].num_to = 1;
        metadata_content.object_trees[4].from[0] = 100;
        metadata_content.object_trees[4].to[0] = 108;
    }
    {
        strcpy(metadata_content.atomic_events[0].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
        strcpy(metadata_content.atomic_events[0].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
        strcpy(metadata_content.atomic_events[0].rule_name, "camera tamper detected");
        metadata_content.atomic_events[0].event_type = METADATA_EVENT_CAMERA_TAMPER_DETECTED;
        metadata_content.atomic_events[0].start_time = 1444799739000 - 10000;
        metadata_content.atomic_events[0].event_details.details_camera_tamper_detected.tamper_type =
            METADATA_TAMPER_TYPE_MOVED;
    }
    {
        strcpy(metadata_content.atomic_events[1].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");
        strcpy(metadata_content.atomic_events[1].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");
        strcpy(metadata_content.atomic_events[1].rule_name, "motion detected in zone 1");
        metadata_content.atomic_events[1].event_type = METADATA_EVENT_MOTION_DETECTED;
        metadata_content.atomic_events[1].start_time = 1444799739000;
        metadata_content.atomic_events[1].event_details.details_motion_detected.motion_activity = 3;
        metadata_content.atomic_events[1].event_details.details_motion_detected.object_id = 101;
    }
    {
        strcpy(metadata_content.atomic_events[2].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
        strcpy(metadata_content.atomic_events[2].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
        strcpy(metadata_content.atomic_events[2].rule_name, "Intrusion detected");
        metadata_content.atomic_events[2].event_type = METADATA_EVENT_INTRUSION_DETECTED;
        metadata_content.atomic_events[2].start_time = 1444799739000;
        metadata_content.atomic_events[2].event_details.details_object_trackor.object_id = 101;
    }
    {
        strcpy(metadata_content.atomic_events[3].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
        strcpy(metadata_content.atomic_events[3].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
        strcpy(metadata_content.atomic_events[3].rule_name, "line crossed at line 1");
        metadata_content.atomic_events[3].event_type = METADATA_EVENT_LINECROSSED;
        metadata_content.atomic_events[3].start_time = 1444799739000;
        metadata_content.atomic_events[3].event_details.details_object_trackor.object_id = 101;
    }
    {
        strcpy(metadata_content.atomic_events[4].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A4");
        strcpy(metadata_content.atomic_events[4].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A4");
        strcpy(metadata_content.atomic_events[4].rule_name, "people counted");
        metadata_content.atomic_events[4].event_type = METADATA_EVENT_OBJECT_COUNTED;
        metadata_content.atomic_events[4].start_time = 1444799739000;
        metadata_content.atomic_events[4].event_details.details_object_counted.count = 1;
    }
    {
        strcpy(metadata_content.atomic_events[5].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A5");
        strcpy(metadata_content.atomic_events[5].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A5");
        strcpy(metadata_content.atomic_events[5].rule_name, "loitering detected");
        metadata_content.atomic_events[5].event_type = METADATA_EVENT_LOITERING_DETECTED;
        metadata_content.atomic_events[5].start_time = 1444799739000 - 30000;
        metadata_content.atomic_events[5].event_details.details_object_trackor.object_id = 101;
    }
    {
        strcpy(metadata_content.atomic_events[6].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A6");
        strcpy(metadata_content.atomic_events[6].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A6");
        strcpy(metadata_content.atomic_events[6].rule_name, "face detected");
        metadata_content.atomic_events[6].event_type = METADATA_EVENT_FACE_DETECTED;
        metadata_content.atomic_events[6].start_time = 1444799739000;
        metadata_content.atomic_events[6].event_details.details_face_detected.object_id = 301;
    }
    {
        strcpy(metadata_content.atomic_events[7].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A7");
        strcpy(metadata_content.atomic_events[7].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A7");
        strcpy(metadata_content.atomic_events[7].rule_name, "face recognized");
        metadata_content.atomic_events[7].event_type = METADATA_EVENT_FACE_RECOGNIZED;
        metadata_content.atomic_events[7].start_time = 1444799739000;
        strcpy(metadata_content.atomic_events[7].event_details.details_face_recognized.display_name,
            "George");
        strcpy(metadata_content.atomic_events[7].event_details.details_face_recognized.group_id.uuid,
            "D6D8D887-8F4D-4647-ADE7-BDF5A88D50A7");
        strcpy(metadata_content.atomic_events[7].event_details.details_face_recognized.group_name,
            "Engineering");
        metadata_content.atomic_events[7].event_details.details_face_recognized.object_id = 301;
        strcpy(metadata_content.atomic_events[7].event_details.details_face_recognized.person_id.uuid,
            "00000000-8F4D-4647-ADE7-BDF5A88D58A7");
    }
    {
        strcpy(metadata_content.atomic_events[8].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        strcpy(metadata_content.atomic_events[8].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        strcpy(metadata_content.atomic_events[8].rule_name, "object classified");
        metadata_content.atomic_events[8].event_type = METADATA_EVENT_OBJECT_CLASSIFIED;
        metadata_content.atomic_events[8].start_time = 1444799739000;
        metadata_content.atomic_events[8].event_details.details_object_trackor.object_id = 101;
        metadata_content.cnt_atomic_events = 9;
    }
   {
        strcpy(metadata_content.events[0].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A9");
        strcpy(metadata_content.events[0].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A9");
        strcpy(metadata_content.events[0].rule_name, "people intrude the zone 1");
        metadata_content.events[0].start_time = 1444799739000;
        strcpy(metadata_content.events[0].composite_event.sub_event_id1.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
        strcpy(metadata_content.events[0].composite_event.sub_event_id2.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        metadata_content.events[0].composite_event.num_sub_events = 2;
    }
    {
        strcpy(metadata_content.events[1].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58B0");
        strcpy(metadata_content.events[1].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58B0");
        strcpy(metadata_content.events[1].rule_name, "people cross the line 1");
        metadata_content.events[1].start_time = 1444799739000;
        strcpy(metadata_content.events[1].composite_event.sub_event_id1.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
        strcpy(metadata_content.events[1].composite_event.sub_event_id2.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        metadata_content.events[1].composite_event.num_sub_events = 2;
        metadata_content.cnt_events = 2;
    }

    {
        metadata_content.heat_maps[0].cells_per_row = 8;
        metadata_content.heat_maps[0].rows_per_column = 8;
        metadata_content.heat_maps[0].start_time = 1444799739000 - 3600000;
        metadata_content.heat_maps[0].end_time = 1444799739000;

        metadata_content.heat_maps[0].data = (uint16_t*)malloc(metadata_content.heat_maps[0].cells_per_row*metadata_content.heat_maps[0].
            rows_per_column*sizeof(uint16_t));
        for (int k = 0; k < (int)metadata_content.heat_maps[0].cells_per_row*(int)metadata_content.heat_maps[0].
            rows_per_column; k++)
        {
            metadata_content.heat_maps[0].data[k] = k % 65536;
        }
        metadata_content.cnt_heat_maps = 1;
    }
    {
        strcpy(metadata_content.extensions[0].engine_id, "QCOM_SCVE");
        metadata_content.extensions[0].data = (char*)malloc(strlen("abcdefg") + 1);
        strcpy(metadata_content.extensions[0].data, "abcdefg");

        metadata_content.extensions[0].size_data = strlen(metadata_content.extensions[0].data) + 1;
    }
    {
        strcpy(metadata_content.extensions[1].engine_id, "QCOM_FACE_RECOGNIZER");
        metadata_content.extensions[1].data = (char*)malloc(strlen("hijklmn") + 1);
        strcpy(metadata_content.extensions[1].data, "hijklmn");
        metadata_content.extensions[1].size_data = strlen(metadata_content.extensions[1].data) + 1;
        metadata_content.cnt_extensions = 2;
    }
    char * output;
    metadata_generator_serialize(metadata_generator_handle, &metadata_content, &output);
    FILE *fp = fopen("metadatatest3.json", "w+t");
    fprintf(fp, "%s", output);
    fclose(fp);
    metadata_generator_release_metadata_content(metadata_generator_handle, &metadata_content);
    metadata_generator_deinit(metadata_generator_handle);
}
//metadata generation example
void JsonMetadataGenerator_example()
{
    void *metadata_generator_handle; //the handle to the metadata generator
    //metadata generator should be initialized before any other metadata generator apis are called.
    metadata_generator_init("vamtest01_", VAM_FILE, 150, 120, &metadata_generator_handle);
    struct metadata_uuid_t camera_id;
    strcpy(camera_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
    uint64_t time_stamp = 1444799739000;
    struct metadata_content_t metadata_content; //this structure is the data holder of metadata frame to be convereted
    //metadata_content should be initialized before it can be used.
    metadata_generator_initialize_metadata_content(metadata_generator_handle, &metadata_content);
    strcpy(metadata_content.camera_id.uuid, camera_id.uuid);
    metadata_content.time_stamp = time_stamp;
    //the first thing is to add metadata to the metadata_content
    //configure an object
    {
        strcpy(metadata_content.objects[0].engine_type, "object_tracker");
        metadata_content.objects[0].object_id = 101;
        metadata_content.objects[0].appearance_descriptor.location.x = 1000;
        metadata_content.objects[0].appearance_descriptor.location.y = 2000;
        metadata_content.objects[0].appearance_descriptor.location.width = 500;
        metadata_content.objects[0].appearance_descriptor.location.height = 1200;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.physical_width =
            750;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.physical_height =
            1755;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.moving_dir =
            METADATA_DIR_UP;
        metadata_content.objects[0].appearance_descriptor.object_tracker_appearance.moving_speed =
            3.1F;
        metadata_content.objects[0].appearance_descriptor.object_type = METADATA_CATEGORY_PEOPLE;
        metadata_content.objects[0].appearance_descriptor.object_type_confidence = 80;
        metadata_content.objects[0].on_event.status = 1;
        metadata_content.objects[0].on_event.num_events = 1;
        strcpy(metadata_content.objects[0].on_event.event_ids[0].uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
        metadata_content.cnt_objects = 1;
    }
    //configure a face object
    {
        strcpy(metadata_content.objects[1].engine_type, "face recognizer");
        metadata_content.objects[1].object_id = 101;
        metadata_content.objects[1].appearance_descriptor.location.x = 1000;
        metadata_content.objects[1].appearance_descriptor.location.y = 2000;
        metadata_content.objects[1].appearance_descriptor.location.width = 500;
        metadata_content.objects[1].appearance_descriptor.location.height = 1200;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.physical_width =
            750;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.physical_height =
            1755;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.moving_dir =
            METADATA_DIR_UP;
        metadata_content.objects[1].appearance_descriptor.object_tracker_appearance.moving_speed =
            3.1F;
        metadata_content.objects[1].appearance_descriptor.object_type = METADATA_CATEGORY_FACE;
        metadata_content.objects[1].on_event.status = 1;
        metadata_content.objects[1].on_event.num_events = 1;
        strcpy(metadata_content.objects[1].on_event.event_ids[0].uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
        strcpy(metadata_content.objects[1].reserve_str[0], "person id of the face");
        metadata_content.objects[1].reserve[0] = 80;
        metadata_content.objects[1].appearance_descriptor.object_type_confidence = metadata_content.objects[1].reserve[0];
        metadata_content.cnt_objects = 2;
    }
    //configure an atomic event
    {
        strcpy(metadata_content.atomic_events[0].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
        strcpy(metadata_content.atomic_events[0].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
        strcpy(metadata_content.atomic_events[0].rule_name, "line crossed at line 1");
        metadata_content.atomic_events[0].event_type = METADATA_EVENT_LINECROSSED;
        metadata_content.atomic_events[0].start_time = 1444799739000;
        metadata_content.atomic_events[0].event_details.details_object_trackor.object_id = 101;
    }
    //configure another atomic event
    {
        strcpy(metadata_content.atomic_events[1].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        strcpy(metadata_content.atomic_events[1].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        strcpy(metadata_content.atomic_events[1].rule_name, "object classified");
        metadata_content.atomic_events[1].event_type = METADATA_EVENT_OBJECT_CLASSIFIED;
        metadata_content.atomic_events[1].start_time = 1444799739000;
        metadata_content.atomic_events[1].event_details.details_object_trackor.object_id = 101;
    }
    metadata_content.cnt_atomic_events = 2; //set the number of atomic events to be converted
    //configure a compound event
    {
        strcpy(metadata_content.events[0].event_id.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58B0");
        strcpy(metadata_content.events[0].rule_id.uuid, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58B0");
        strcpy(metadata_content.events[0].rule_name, "people cross the line 1");
        metadata_content.events[1].start_time = 1444799739000;
        strcpy(metadata_content.events[0].composite_event.sub_event_id1.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
        strcpy(metadata_content.events[0].composite_event.sub_event_id2.uuid, "E6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        metadata_content.events[0].composite_event.num_sub_events = 2;
    }
    metadata_content.cnt_events = 1;

    char * output;
    //convert the metadata from c structure to json string. The output buffer is provided by the metadata generator. The user doesn't need to allocate memory for it.
    metadata_generator_serialize(metadata_generator_handle, &metadata_content, &output);
    metadata_generator_output_json(metadata_generator_handle, &metadata_content);
    FILE *fp = fopen("metadata_generator_example.json", "w+t");
    fprintf(fp, "%s", output);
    fclose(fp);
    //release memeory allocated in the metadata_content
    metadata_generator_release_metadata_content(metadata_generator_handle, &metadata_content);
    //release memeory allocated in the metadata generator before exiting
    metadata_generator_deinit(metadata_generator_handle);
}

//example of metadata parsing
void JsonMetadataParseTest()
{
    void *metadata_parser_handle; //handle to the metadata parser
    void *metadata_generator_handle; //handle to the metadata parser
    //metadata parser should be initialized with metadata_parser_init() before
    //any other metadata generator apis are called.
    enum MetadataParserStatus res;
    res = metadata_parser_init("vamtest01_", VAM_FILE, 0, 0, &metadata_parser_handle);
    metadata_generator_init("vamtest02_", VAM_FILE, 150, 120, &metadata_generator_handle);
    struct metadata_content_t *metadata_content; //data holder of parsed metaddata. It will be allocated by metadata parser interally
    while (1){
        //parse next metadata frame in the stream
        res = metadata_parser_parse_next_frame(metadata_parser_handle, &metadata_content);
        if (res != MP_OK)
            break;
        metadata_generator_output_json(metadata_generator_handle, metadata_content);
        //do whatever with metadata_content
    }
    //release handle before exiting
    metadata_parser_deinit(metadata_parser_handle);
    //release memeory allocated in the metadata_content
//    metadata_generator_release_metadata_content(metadata_generator_handle, metadata_content);
    //release memeory allocated in the metadata generator before exiting
    metadata_generator_deinit(metadata_generator_handle);
}

void JsonConfigurationGeneratorTest()
{
#if (VA_CONFIG_VERSION==0)
    void * configuration_generator_handle;
    configuration_generator_init(&configuration_generator_handle);
    struct JSONConfiguration configuration_1;
    memset(&configuration_1, 0, sizeof(configuration_1));
    {
      //  GeneralConfiguration general_configuration;
        strcpy(configuration_1.general_configuration.configuration_id.uuid,
            "E8D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
        configuration_1.general_configuration.scene_type = SCENE_TYPE_OUTDOOR;
        configuration_1.general_configuration.time_stamp = 1444609365000;
        strcpy(configuration_1.general_configuration.version, "0.1");
    }
//calibration
    {
        //  VASceneCalibration  scene_calibration;
        configuration_1.scene_calibration.length_unit = VA_UNIT_MM;
        configuration_1.scene_calibration.cnt_objects = 0;
        struct  CalibObject obj;
        for (int i = 1; i < 3; i++)
            for (int j = 1; j < 3; j++){
            obj.height = j * 100;
            obj.physical_height = 1700;
            obj.width = i * 100;
            obj.x = i * 10;
            obj.y = j * 10;
            configuration_1.scene_calibration.objects[configuration_1.scene_calibration.cnt_objects] = obj;
            configuration_1.scene_calibration.cnt_objects++;
        }
    }
//rule configuraton
    {
        //zones
        {
            struct VAPoint points1[4];
            points1[0].x = 100;
            points1[0].y = 100;
            points1[1].x = 300;
            points1[1].y = 100;
            points1[2].x = 300;
            points1[2].y = 300;
            points1[3].x = 100;
            points1[3].y = 300;
            strcpy(configuration_1.zones[0].zone_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
            configuration_1.zones[0].cnt_points = 4;
            configuration_1.zones[0].zone_type = ZONE_TYPE_INCLUDE;
            for(int i=0;i<4;i++)
                configuration_1.zones[0].points[i] = points1[i];
            struct VAPoint points2[4];
            points2[0].x = 1000;
            points2[0].y = 1000;
            points2[1].x = 3000;
            points2[1].y = 1000;
            points2[2].x = 3000;
            points2[2].y = 3000;
            points2[3].x = 1000;
            points2[3].y = 3000;
            strcpy(configuration_1.zones[1].zone_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A4");
            configuration_1.zones[1].zone_type = ZONE_TYPE_INCLUDE;
            configuration_1.zones[1].cnt_points = 4;
            for (int i = 0; i<4; i++)
                configuration_1.zones[1].points[i] = points2[i];
            struct VAPoint points3[4];
            points3[0].x = 5000;
            points3[0].y = 5000;
            points3[1].x = 7000;
            points3[1].y = 5000;
            points3[2].x = 7000;
            points3[2].y = 7000;
            points3[3].x = 5000;
            points3[3].y = 7000;
            strcpy(configuration_1.zones[2].zone_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A5");
            configuration_1.zones[2].zone_type = ZONE_TYPE_EXCLUDE;
            configuration_1.zones[2].cnt_points = 4;
            for (int i = 0; i<4; i++)
                configuration_1.zones[2].points[i] = points3[i];
            configuration_1.cnt_zones = 3;
        }
        //lines
        {
            struct VAPoint points1[3];
            points1[0].x = 1500;
            points1[0].y = 1500;
            points1[1].x = 3500;
            points1[1].y = 1500;
            points1[2].x = 3500;
            points1[2].y = 3500;
            strcpy(configuration_1.lines[0].line_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
            configuration_1.lines[0].cnt_points = 3;
            for (int i = 0; i<3; i++)
                configuration_1.lines[0].points[i] = points1[i];

            struct VAPoint points2[3];
            points2[0].x = 2000;
            points2[0].y = 2000;
            points2[1].x = 4000;
            points2[1].y = 2000;
            points2[2].x = 4000;
            points2[2].y = 4000;
            strcpy(configuration_1.lines[1].line_id.uuid, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
            configuration_1.lines[1].cnt_points = 3;
            for (int i = 0; i<3; i++)
                configuration_1.lines[1].points[i] = points2[i];
            configuration_1.cnt_lines = 2;
         }
        //atomic_rules
        {
            newUUID(configuration_1.atomic_rules[0].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[0].rule_name, "Camera Tamper Detected");
            configuration_1.atomic_rules[0].event_type = METADATA_EVENT_CAMERA_TAMPER_DETECTED;
            configuration_1.atomic_rules[0].status = VA_ENABLED;
            configuration_1.atomic_rules[0].attributes.camera_tamper_detected.sensitivity = 30;

            newUUID(configuration_1.atomic_rules[1].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[1].rule_name, "Motion Detected");
            configuration_1.atomic_rules[1].event_type = METADATA_EVENT_MOTION_DETECTED;
            configuration_1.atomic_rules[1].status = VA_ENABLED;
            configuration_1.atomic_rules[1].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[1].attributes.motion_detected.sensitivity = 30;
            configuration_1.atomic_rules[1].attributes.motion_detected.minimum_size = 30;
            configuration_1.atomic_rules[1].attributes.motion_detected.event_on_delay = 2000;
            configuration_1.atomic_rules[1].attributes.motion_detected.event_off_delay = 1000;

            newUUID(configuration_1.atomic_rules[2].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[2].rule_name, "Intrution Detected");
            configuration_1.atomic_rules[2].event_type = METADATA_EVENT_INTRUSION_DETECTED;
            configuration_1.atomic_rules[2].status = VA_ENABLED;
            configuration_1.atomic_rules[2].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[2].attributes.intrusion_detected.sensitivity = 30;
            configuration_1.atomic_rules[2].attributes.intrusion_detected.minimum_size = 10;

            newUUID(configuration_1.atomic_rules[3].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[3].rule_name, "Line Crossing Detected");
            configuration_1.atomic_rules[3].event_type = METADATA_EVENT_LINECROSSED;
            configuration_1.atomic_rules[3].status = VA_ENABLED;
            configuration_1.atomic_rules[3].line_id = configuration_1.lines[0].line_id;
            configuration_1.atomic_rules[3].attributes.line_crossed.sensitivity = 30;
            configuration_1.atomic_rules[3].attributes.line_crossed.direction = VA_DIR_LINE_IN;
            configuration_1.atomic_rules[3].attributes.line_crossed.minimum_size = 10;

            newUUID(configuration_1.atomic_rules[4].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[4].rule_name, "Object Counted at line");
            configuration_1.atomic_rules[4].event_type = METADATA_EVENT_OBJECT_COUNTED;
            configuration_1.atomic_rules[4].status = VA_ENABLED;
            configuration_1.atomic_rules[4].line_id = configuration_1.lines[0].line_id;
            configuration_1.atomic_rules[4].attributes.object_counted.detector_type = 1;
            configuration_1.atomic_rules[4].attributes.object_counted.direction = VA_DIR_LINE_IN;
            configuration_1.atomic_rules[4].attributes.object_counted.sensitivity = 30;
            configuration_1.atomic_rules[4].attributes.object_counted.minimum_size = 30;
            configuration_1.atomic_rules[4].attributes.object_counted.report_time_interval = 5;
            configuration_1.atomic_rules[4].attributes.object_counted.reset_time_interval = 60;


            newUUID(configuration_1.atomic_rules[5].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[5].rule_name, "Object Counted in zone");
            configuration_1.atomic_rules[5].event_type = METADATA_EVENT_OBJECT_COUNTED;
            configuration_1.atomic_rules[5].status = VA_ENABLED;
            configuration_1.atomic_rules[5].zone_id = configuration_1.zones[0].zone_id;
            configuration_1.atomic_rules[5].attributes.object_counted.detector_type = 0;
            configuration_1.atomic_rules[5].attributes.object_counted.sensitivity = 30;
            configuration_1.atomic_rules[5].attributes.object_counted.minimum_size = 30;
            configuration_1.atomic_rules[5].attributes.object_counted.report_time_interval = 5;
            configuration_1.atomic_rules[5].attributes.object_counted.reset_time_interval = 60;

            newUUID(configuration_1.atomic_rules[6].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[6].rule_name, "Loitering Detected");
            configuration_1.atomic_rules[6].event_type = METADATA_EVENT_LOITERING_DETECTED;
            configuration_1.atomic_rules[6].status = VA_ENABLED;
            configuration_1.atomic_rules[6].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[6].attributes.loitering_detected.sensitivity = 30;
            configuration_1.atomic_rules[6].attributes.loitering_detected.minimum_size = 30;
            configuration_1.atomic_rules[6].attributes.loitering_detected.dwell_time = 30;

            newUUID(configuration_1.atomic_rules[7].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[7].rule_name, "Face Detected");
            configuration_1.atomic_rules[7].event_type = METADATA_EVENT_FACE_RECOGNIZED;
            configuration_1.atomic_rules[7].status = VA_ENABLED;
            configuration_1.atomic_rules[7].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[7].attributes.face_recognized.sensitivity = 30;
            configuration_1.atomic_rules[7].attributes.face_recognized.minimum_size = 1;
            configuration_1.atomic_rules[7].attributes.face_recognized.confidence = 80;
            configuration_1.atomic_rules[7].attributes.face_recognized.cnt_groups = 0;

            newUUID(configuration_1.atomic_rules[8].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[8].rule_name, "Face Recognized");
            configuration_1.atomic_rules[8].event_type = METADATA_EVENT_FACE_RECOGNIZED;
            configuration_1.atomic_rules[8].status = VA_ENABLED;
            configuration_1.atomic_rules[8].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[8].attributes.face_recognized.sensitivity = 30;
            configuration_1.atomic_rules[8].attributes.face_recognized.minimum_size = 1;
            configuration_1.atomic_rules[8].attributes.face_recognized.confidence = 80;
            configuration_1.atomic_rules[8].attributes.face_recognized.cnt_groups = 0;
            strcpy(configuration_1.atomic_rules[8].attributes.face_recognized.groups[configuration_1.atomic_rules[8].
                attributes.face_recognized.cnt_groups].uuid, "E8D8D887-8F4D-4647-ADE7-BDF5A88D58D0");
            configuration_1.atomic_rules[8].attributes.face_recognized.cnt_groups++;
            strcpy(configuration_1.atomic_rules[8].attributes.face_recognized.groups[configuration_1.atomic_rules[8].
                attributes.face_recognized.cnt_groups].uuid, "E8D8D887-8F4D-4647-ADE7-BDF5A88D58D1");
            configuration_1.atomic_rules[8].attributes.face_recognized.cnt_groups++;

            newUUID(configuration_1.atomic_rules[9].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[9].rule_name, "People or Vehicle Classified");
            configuration_1.atomic_rules[9].event_type = METADATA_EVENT_OBJECT_CLASSIFIED;
            configuration_1.atomic_rules[9].status = VA_ENABLED;
            configuration_1.atomic_rules[9].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[9].attributes.object_classified.sensitivity = 30;
            configuration_1.atomic_rules[9].attributes.object_classified.object_type[0] = VA_CATEGORY_PEOPLE;
            configuration_1.atomic_rules[9].attributes.object_classified.object_type[0] = VA_CATEGORY_VEHICLE;
            configuration_1.atomic_rules[9].attributes.object_classified.cnt_object_types = 2;

            newUUID(configuration_1.atomic_rules[10].rule_id.uuid);
            strcpy(configuration_1.atomic_rules[10].rule_name, "People  Classified");
            configuration_1.atomic_rules[10].event_type = METADATA_EVENT_OBJECT_CLASSIFIED;
            configuration_1.atomic_rules[10].status = VA_ENABLED;
            configuration_1.atomic_rules[10].zone_id = configuration_1.zones[1].zone_id;
            configuration_1.atomic_rules[10].attributes.object_classified.sensitivity = 30;
            configuration_1.atomic_rules[10].attributes.object_classified.cnt_object_types = 1;
            configuration_1.atomic_rules[10].attributes.object_classified.object_type[0] =
                VA_CATEGORY_PEOPLE;
            configuration_1.cnt_atomic_rules = 11;
         }
        //rules
        {
            newUUID(configuration_1.rules[0].rule_id.uuid);
            strcpy(configuration_1.rules[0].rule_name, "People intrude forbidden zone1");
            strcpy(configuration_1.rules[0].operation, "AND");
            configuration_1.rules[0].status = VA_ENABLED;
            configuration_1.rules[0].sub_rule_id1 = configuration_1.atomic_rules[2].rule_id;
            configuration_1.rules[0].sub_rule_id2 = configuration_1.atomic_rules[10].rule_id;
            configuration_1.rules[0].relationship.relative_end_time = 0;
            configuration_1.rules[0].relationship.relative_start_time = 0;

            newUUID(configuration_1.rules[1].rule_id.uuid);
            strcpy(configuration_1.rules[1].rule_name,
                "object intrude or motion detected from forbidden zone1");
            strcpy(configuration_1.rules[1].operation, "OR");
            configuration_1.rules[1].status = VA_ENABLED;
            configuration_1.rules[1].sub_rule_id1 = configuration_1.atomic_rules[1].rule_id;
            configuration_1.rules[1].sub_rule_id2 = configuration_1.atomic_rules[2].rule_id;
            configuration_1.rules[1].relationship.relative_end_time = 0;
            configuration_1.rules[1].relationship.relative_start_time = 0;
            configuration_1.cnt_rules = 2;
        }
        {
           // VAConfigExtension extension;
            newUUID(configuration_1.extension[0].engine_id);
            char data[] = "extension data for test";
            configuration_1.extension[0].data =  (char*)malloc(strlen(data) + 1);
            strcpy(configuration_1.extension[0].data, data);
            configuration_1.extension[0].size_data = strlen(configuration_1.extension[0].data);
            configuration_1.extension[0].data[configuration_1.extension[0].size_data] = 0;
            configuration_1.cnt_extension = 1;
        }
    }
    char * doc;
    configuration_generator_serialize(configuration_generator_handle, &configuration_1, &doc);
    FILE *fp = fopen("configuration_test01.json", "w+t");
    fprintf(fp, "%s", doc);
    fclose(fp);
	for (unsigned int i = 0; i < configuration_1.cnt_extension; i++){
		free(configuration_1.extension[i].data);
	}
    configuration_generator_deinit(configuration_generator_handle);

#endif
#if (VA_CONFIG_VERSION==1)
    void * configuration_generator_handle;
    configuration_generator_init(&configuration_generator_handle);
    struct JSONConfiguration configuration_1;
    memset(&configuration_1, 0, sizeof(configuration_1));
    {
        configuration_1.time_stamp = 1444609365000;
        strcpy(configuration_1.version, "1.0");
        strcpy(configuration_1.id, "0000-1111-12345678");
    }
//calibration
    {
        //  VASceneCalibration  scene_calibration;
        configuration_1.scene_calibration.length_unit = VA_UNIT_MM;
        configuration_1.scene_calibration.cnt_objects = 0;
        struct  CalibObject obj;
        for (int i = 1; i < 3; i++)
            for (int j = 1; j < 3; j++){
            obj.height = j * 100;
            obj.physical_height = 1700;
            obj.width = i * 100;
            obj.x = i * 10;
            obj.y = j * 10;
            configuration_1.scene_calibration.objects[configuration_1.scene_calibration.cnt_objects] = obj;
            configuration_1.scene_calibration.cnt_objects++;
        }
    }
//rule configuraton
    {
        //zones
        {
            struct VAPoint points1[4];
            points1[0].x = 100;
            points1[0].y = 100;
            points1[1].x = 300;
            points1[1].y = 100;
            points1[2].x = 300;
            points1[2].y = 300;
            points1[3].x = 100;
            points1[3].y = 300;
            strcpy(configuration_1.zones[0].id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
            configuration_1.zones[0].cnt_points = 4;
            configuration_1.zones[0].type = ZONE_TYPE_INCLUDE;
            for(int i=0;i<4;i++)
                configuration_1.zones[0].points[i] = points1[i];
            struct VAPoint points2[4];
            points2[0].x = 1000;
            points2[0].y = 1000;
            points2[1].x = 3000;
            points2[1].y = 1000;
            points2[2].x = 3000;
            points2[2].y = 3000;
            points2[3].x = 1000;
            points2[3].y = 3000;
            strcpy(configuration_1.zones[1].id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A4");
            configuration_1.zones[1].type = ZONE_TYPE_INCLUDE;
            configuration_1.zones[1].cnt_points = 4;
            for (int i = 0; i<4; i++)
                configuration_1.zones[1].points[i] = points2[i];
            struct VAPoint points3[4];
            points3[0].x = 5000;
            points3[0].y = 5000;
            points3[1].x = 7000;
            points3[1].y = 5000;
            points3[2].x = 7000;
            points3[2].y = 7000;
            points3[3].x = 5000;
            points3[3].y = 7000;
            strcpy(configuration_1.zones[2].id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A5");
            configuration_1.zones[2].type = ZONE_TYPE_EXCLUDE;
            configuration_1.zones[2].cnt_points = 4;
            for (int i = 0; i<4; i++)
                configuration_1.zones[2].points[i] = points3[i];
        }
        //lines
        {
            struct VAPoint points1[3];
            points1[0].x = 1500;
            points1[0].y = 1500;
            points1[1].x = 3500;
            points1[1].y = 1500;
            points1[2].x = 3500;
            points1[2].y = 3500;
            strcpy(configuration_1.zones[3].id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A6");
            configuration_1.zones[3].type = LINE_TYPE_IN;
            configuration_1.zones[3].cnt_points = 3;
            for (int i = 0; i<3; i++)
                configuration_1.zones[3].points[i] = points1[i];

            struct VAPoint points2[3];
            points2[0].x = 2000;
            points2[0].y = 2000;
            points2[1].x = 4000;
            points2[1].y = 2000;
            points2[2].x = 4000;
            points2[2].y = 4000;
            strcpy(configuration_1.zones[4].id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A7");
            configuration_1.zones[4].type = LINE_TYPE_OUT;
            configuration_1.zones[4].cnt_points = 3;
            for (int i = 0; i<3; i++)
                configuration_1.zones[4].points[i] = points2[i];
            configuration_1.cnt_zones = 5;
         }
        //atomic_rules
        {
            for (int i = 0; i < ATOMIC_RULE_MAX; i++){
                memset(configuration_1.atomic_rules[i].reserve, 0, sizeof(int64_t) * 5);
                for (int k = 0; k < 5; k++)
                    configuration_1.atomic_rules[i].reserve_str[k][0] = 0;
            }
            newUUID(configuration_1.atomic_rules[0].id);
            strcpy(configuration_1.atomic_rules[0].name, "Camera Tamper Detected");
            configuration_1.atomic_rules[0].event_type = METADATA_EVENT_CAMERA_TAMPER_DETECTED;
            configuration_1.atomic_rules[0].status = VA_ENABLED;
            configuration_1.atomic_rules[0].sensitivity = 30;
            configuration_1.atomic_rules[0].min_size = 10;
            configuration_1.atomic_rules[0].cnt_zones = 0;

            newUUID(configuration_1.atomic_rules[1].id);
            strcpy(configuration_1.atomic_rules[1].name, "Object Detected");
            configuration_1.atomic_rules[1].event_type = METADATA_EVENT_OBJECT_DETECTED;
            configuration_1.atomic_rules[1].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[1].zone_ids[0], configuration_1.zones[1].id);
            configuration_1.atomic_rules[1].sensitivity = 30;
            configuration_1.atomic_rules[1].min_size = 10;
            configuration_1.atomic_rules[1].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[2].id);
            strcpy(configuration_1.atomic_rules[2].name, "Intrution Detected");
            configuration_1.atomic_rules[2].event_type = METADATA_EVENT_INTRUSION_DETECTED;
            configuration_1.atomic_rules[2].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[2].zone_ids[0], configuration_1.zones[1].id);
            configuration_1.atomic_rules[2].sensitivity = 30;
            configuration_1.atomic_rules[2].min_size = 10;
            configuration_1.atomic_rules[2].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[3].id);
            strcpy(configuration_1.atomic_rules[3].name, "Line Crossing Detected");
            configuration_1.atomic_rules[3].event_type = METADATA_EVENT_LINECROSSED;
            configuration_1.atomic_rules[3].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[3].zone_ids[0], configuration_1.zones[3].id);
            configuration_1.atomic_rules[3].sensitivity = 30;
            configuration_1.atomic_rules[3].min_size = 10;
            configuration_1.atomic_rules[3].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[4].id);
            strcpy(configuration_1.atomic_rules[4].name, "Object Counted at line");
            configuration_1.atomic_rules[4].event_type = METADATA_EVENT_OBJECT_COUNTED;
            configuration_1.atomic_rules[4].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[4].zone_ids[0], configuration_1.zones[4].id);
            configuration_1.atomic_rules[4].sensitivity = 30;
            configuration_1.atomic_rules[4].min_size = 10;
            configuration_1.atomic_rules[4].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[5].id);
            strcpy(configuration_1.atomic_rules[5].name, "People  Classified");
            configuration_1.atomic_rules[5].event_type = METADATA_EVENT_OBJECT_DETECTED;
            configuration_1.atomic_rules[5].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[5].zone_ids[0], configuration_1.zones[1].id);
            configuration_1.atomic_rules[5].sensitivity = 30;
            configuration_1.atomic_rules[5].min_size = 10;
            configuration_1.atomic_rules[5].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[6].id);
            strcpy(configuration_1.atomic_rules[6].name, "Loitering Detected");
            configuration_1.atomic_rules[6].event_type = METADATA_EVENT_LOITERING_DETECTED;
            configuration_1.atomic_rules[6].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[6].zone_ids[0],  configuration_1.zones[1].id);
            configuration_1.atomic_rules[6].reserve[0] = 30;
            configuration_1.atomic_rules[6].sensitivity = 30;
            configuration_1.atomic_rules[6].min_size = 10;
            configuration_1.atomic_rules[6].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[7].id);
            strcpy(configuration_1.atomic_rules[7].name, "Face Detected");
            configuration_1.atomic_rules[7].event_type = METADATA_EVENT_FACE_DETECTED;
            configuration_1.atomic_rules[7].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[7].zone_ids[0], configuration_1.zones[1].id);
            configuration_1.atomic_rules[7].sensitivity = 30;
            configuration_1.atomic_rules[7].min_size = 10;
            configuration_1.atomic_rules[7].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[8].id);
            strcpy(configuration_1.atomic_rules[8].name, "Face Recognized");
            configuration_1.atomic_rules[8].event_type = METADATA_EVENT_FACE_RECOGNIZED;
            configuration_1.atomic_rules[8].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[8].zone_ids[0], configuration_1.zones[1].id);
            configuration_1.atomic_rules[8].sensitivity = 30;
            configuration_1.atomic_rules[8].min_size = 10;
            strcpy(configuration_1.atomic_rules[8].reserve_str[0], "E8D8D887-8F4D-4647-ADE7-BDF5A88D58D0");
            strcpy(configuration_1.atomic_rules[8].reserve_str[1], "E8D8D887-8F4D-4647-ADE7-BDF5A88D58D1");
            configuration_1.atomic_rules[8].cnt_zones = 1;

            newUUID(configuration_1.atomic_rules[9].id);
            strcpy(configuration_1.atomic_rules[9].name, "People or Vehicle Classified");
            configuration_1.atomic_rules[9].event_type = METADATA_EVENT_OBJECT_CLASSIFIED;
            configuration_1.atomic_rules[9].status = VA_ENABLED;
            strcpy(configuration_1.atomic_rules[9].zone_ids[0], configuration_1.zones[1].id);
            configuration_1.atomic_rules[9].sensitivity = 30;
            configuration_1.atomic_rules[9].min_size = 30;
            configuration_1.atomic_rules[9].reserve[0] = VA_CATEGORY_PEOPLE;
            configuration_1.atomic_rules[9].reserve[1] = VA_CATEGORY_VEHICLE;
            configuration_1.atomic_rules[9].cnt_zones = 1;

            configuration_1.cnt_atomic_rules = 10;
         }
        //rules
        {
            for (int i = 0; i < ATOMIC_RULE_MAX; i++){
                memset(configuration_1.rules[i].reserve, 0, sizeof(int64_t) * 5);
                for (int k = 0; k < 5; k++)
                    configuration_1.rules[i].reserve_str[k][0] = 0;
            }
            newUUID(configuration_1.rules[0].id);
            strcpy(configuration_1.rules[0].name, "People intrude forbidden zone1");
            configuration_1.rules[0].operation=0;
            configuration_1.rules[0].status = VA_ENABLED;
            strcpy(configuration_1.rules[0].sub_rule_id1, configuration_1.atomic_rules[2].id);
            strcpy(configuration_1.rules[0].sub_rule_id2, configuration_1.atomic_rules[9].id);
            configuration_1.rules[0].reserve[0] = 0;
            configuration_1.rules[0].reserve[1] = 0;

            newUUID(configuration_1.rules[1].id);
            strcpy(configuration_1.rules[1].name,
                "object intrude or motion detected from forbidden zone1");
            configuration_1.rules[1].operation=1;
            configuration_1.rules[1].status = VA_ENABLED;
            strcpy(configuration_1.rules[1].sub_rule_id1, configuration_1.atomic_rules[1].id);
            strcpy(configuration_1.rules[1].sub_rule_id2, configuration_1.atomic_rules[2].id);
            configuration_1.rules[1].reserve[0] = 0;
            configuration_1.rules[1].reserve[1] = 0;
            configuration_1.cnt_rules = 2;
        }
    }
    char * doc;
    configuration_generator_serialize(configuration_generator_handle, &configuration_1, &doc);
    FILE *fp = fopen("configuration_test11.json", "w+t");
    fprintf(fp, "%s", doc);
    fclose(fp);
    configuration_generator_deinit(configuration_generator_handle);
#endif
}

//metadata parsing
void JsonConfigurationParserTest()
{
    void * configuration_parser_handle;
    void * configuration_generator_handle;
    configuration_generator_init(&configuration_generator_handle);
    configuration_parser_init(&configuration_parser_handle);
    struct JSONConfiguration *configuration=0;
   // memset(&configuration, 0, sizeof(configuration));
#if (VA_CONFIG_VERSION==0)
    configuration_parser_parse_file(configuration_parser_handle,
        "configuration_test01.json", &configuration);
    char *str;
    configuration_generator_serialize(configuration_generator_handle,
        configuration, &str);
    FILE *fp = fopen("configuration_test02.json", "w+t");
    fprintf(fp, "%s", str);
    fclose(fp);
#endif
#if (VA_CONFIG_VERSION==1)
    configuration_parser_parse_file(configuration_parser_handle,
        "configuration_test11.json", &configuration);
    char *str;
     configuration_generator_serialize(configuration_generator_handle,
        configuration, &str);
    FILE *fp = fopen("configuration_test12.json", "w+t");
    fprintf(fp, "%s", str);
    fclose(fp);
#endif
    /* configuration_parser_load_file(configuration_parser_handle,
        "configuration_test11.json", &doc_str);
    configuration_parser_parse_doc(configuration_parser_handle, doc_str,
        &configuration);
    configuration_generator_serialize(configuration_generator_handle,
       &configuration, &str);
    fp = fopen("configuration_test14.json", "w+t");
    fprintf(fp, "%s", str);
    fclose(fp);  */
    configuration_generator_deinit(configuration_generator_handle);
    configuration_parser_deinit(configuration_parser_handle);
}

void json_event_generator_test()
{
    void * event_generator_handle;
    char *buf;
    //Initialize event generator
    json_event_generator_init("event_generation_test", JSON_EVENT_FILE, 1459623438000, 3600, &event_generator_handle);
    struct json_event events[10];

    memset(&events[0], 0, sizeof(struct json_event));
    //events[0] is tamper
    strcpy(events[0].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
    strcpy(events[0].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
    strcpy(events[0].base.rule_name, "tamper detected");
    events[0].base.event_time = 1459623438000;
    events[0].base.event_type = JSON_EVENT_CAMERA_TAMPER_DETECTED;
    events[0].base.priority = 0;
    events[0].body.tamper.confidence = 100;
    events[0].body.tamper.duration = 10;
    events[0].body.tamper.tamper_type = 1; //In motion / redirected
    //one event will be output
    json_event_generator_output_json(event_generator_handle, 1459623438000, 0, events, 1);
    json_event_generator_serialize(event_generator_handle, 1459623438000, 0, events, 1, &buf);
    //events[0] is line crossed
    memset(&events[0], 0, sizeof(struct json_event));
    memset(&events[1], 0, sizeof(struct json_event));
    strcpy(events[0].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");
    strcpy(events[0].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");
    strcpy(events[0].base.rule_name, "line crossed");
    events[0].base.event_time = 1459623439000;
    events[0].base.event_type = JSON_EVENT_LINECROSSED;
    events[0].base.priority = 0;
    events[0].body.line_crossed.object_id = 101;
    events[0].body.line_crossed.dir = 0;
    events[0].body.line_crossed.x = 2000;
    events[0].body.line_crossed.y = 7000;
    events[0].body.line_crossed.height = 200;
    events[0].body.line_crossed.width = 100;
    events[0].body.line_crossed.cnt_zone_vertices = 4;
    events[0].body.line_crossed.points[0].x = 1000;
    events[0].body.line_crossed.points[0].y = 1000;
    events[0].body.line_crossed.points[1].x = 1000;
    events[0].body.line_crossed.points[1].y = 9000;
    events[0].body.line_crossed.points[2].x = 9000;
    events[0].body.line_crossed.points[2].y = 9000;
    events[0].body.line_crossed.points[3].x = 9000;
    events[0].body.line_crossed.points[3].y = 1000;
    strcpy(events[0].body.line_crossed.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");

    //events[1] is intrusion detected
    strcpy(events[1].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    strcpy(events[1].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    strcpy(events[1].base.rule_name, "intrusion detected");
    events[1].base.event_time = 1459623439000;
    events[1].base.event_type = JSON_EVENT_INTRUSION_DETECTED;
    events[1].base.priority = 0;
    events[1].body.intrusion_detected.object_id = 101;
    events[1].body.intrusion_detected.x = 3000;
    events[1].body.intrusion_detected.y = 7000;
    events[1].body.intrusion_detected.height = 200;
    events[1].body.intrusion_detected.width = 100;
    events[1].body.intrusion_detected.cnt_zone_vertices = 4;
    events[1].body.intrusion_detected.points[0].x = 1000;
    events[1].body.intrusion_detected.points[0].y = 1000;
    events[1].body.intrusion_detected.points[1].x = 1000;
    events[1].body.intrusion_detected.points[1].y = 9000;
    events[1].body.intrusion_detected.points[2].x = 9000;
    events[1].body.intrusion_detected.points[2].y = 9000;
    events[1].body.intrusion_detected.points[3].x = 9000;
    events[1].body.intrusion_detected.points[3].y = 1000;
    strcpy(events[1].body.intrusion_detected.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    //two events will be output
    json_event_generator_output_json(event_generator_handle, 1459623439000, 30, events, 2);

    //testing another time slot
    //events[0] is object counted
    memset(&events[0], 0, sizeof(struct json_event));
    memset(&events[1], 0, sizeof(struct json_event));
    strcpy(events[0].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
    strcpy(events[0].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A3");
    strcpy(events[0].base.rule_name, "object counted");
    events[0].base.event_time = 1459623539000;
    events[0].base.event_type = JSON_EVENT_OBJECT_COUNTED;
    events[0].base.priority = 0;
    events[0].body.object_counted.count = 1;
    events[0].body.object_counted.object_id = 101;
    events[0].body.object_counted.dir = 0;
    events[0].body.object_counted.x = 2000;
    events[0].body.object_counted.y = 7000;
    events[0].body.object_counted.height = 200;
    events[0].body.object_counted.width = 100;
    events[0].body.object_counted.cnt_zone_vertices = 4;
    events[0].body.object_counted.points[0].x = 1000;
    events[0].body.object_counted.points[0].y = 1000;
    events[0].body.object_counted.points[1].x = 1000;
    events[0].body.object_counted.points[1].y = 9000;
    events[0].body.object_counted.points[2].x = 9000;
    events[0].body.object_counted.points[2].y = 9000;
    events[0].body.object_counted.points[3].x = 9000;
    events[0].body.object_counted.points[3].y = 1000;
    strcpy(events[0].body.object_counted.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A1");

    //events[1] is loitering detected
    strcpy(events[1].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A4");
    strcpy(events[1].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A4");
    strcpy(events[1].base.rule_name, "intrusion detected");
    events[1].base.event_time = 1459623539000;
    events[1].base.event_type = JSON_EVENT_LOITERING_DETECTED;
    events[1].base.priority = 0;
    events[1].body.loitering_detected.object_id = 101;
    events[1].body.loitering_detected.duration = 101;
    events[1].body.loitering_detected.x = 3000;
    events[1].body.loitering_detected.y = 7000;
    events[1].body.loitering_detected.height = 200;
    events[1].body.loitering_detected.width = 100;
    events[1].body.loitering_detected.cnt_zone_vertices = 4;
    events[1].body.loitering_detected.points[0].x = 1000;
    events[1].body.loitering_detected.points[0].y = 1000;
    events[1].body.loitering_detected.points[1].x = 1000;
    events[1].body.loitering_detected.points[1].y = 9000;
    events[1].body.loitering_detected.points[2].x = 9000;
    events[1].body.loitering_detected.points[2].y = 9000;
    events[1].body.loitering_detected.points[3].x = 9000;
    events[1].body.loitering_detected.points[3].y = 1000;
    strcpy(events[1].body.loitering_detected.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    //another two events will be output after 100 seconds to test new event file generation
    json_event_generator_output_json(event_generator_handle, 1459623539000, 3000, events, 2);

    for(int i=0;i<10;i++)
        memset(&events[i], 0, sizeof(struct json_event));
    //events[0] is face detected
    strcpy(events[0].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A5");
    strcpy(events[0].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A5");
    strcpy(events[0].base.rule_name, "face detected");
    events[0].base.event_time = 1459623540000;
    events[0].base.event_type = JSON_EVENT_FACE_DETECTED;
    events[0].base.priority = 0;
    events[0].body.face_detected.object_id = 201;
    events[0].body.face_detected.confidence = 100;
    events[0].body.face_detected.x = 5000;
    events[0].body.face_detected.y = 7000;
    events[0].body.face_detected.height = 400;
    events[0].body.face_detected.width = 400;
    events[0].body.face_detected.cnt_zone_vertices = 4;
    events[0].body.face_detected.points[0].x = 1000;
    events[0].body.face_detected.points[0].y = 1000;
    events[0].body.face_detected.points[1].x = 1000;
    events[0].body.face_detected.points[1].y = 9000;
    events[0].body.face_detected.points[2].x = 9000;
    events[0].body.face_detected.points[2].y = 9000;
    events[0].body.face_detected.points[3].x = 9000;
    events[0].body.face_detected.points[3].y = 1000;
    strcpy(events[0].body.face_detected.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");

    //events[1] is face recognized
    strcpy(events[1].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A6");
    strcpy(events[1].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A6");
    strcpy(events[1].base.rule_name, "face recognized");
    events[1].base.event_time = 1459623540000;
    events[1].base.event_type = JSON_EVENT_FACE_RECOGNIZED;
    events[1].base.priority = 0;
    events[1].body.face_recognized.object_id = 201;
    events[1].body.face_recognized.confidence = 100;
    events[1].body.face_recognized.x = 5000;
    events[1].body.face_recognized.y = 7000;
    events[1].body.face_recognized.height = 400;
    events[1].body.face_recognized.width = 400;
    events[1].body.face_recognized.cnt_zone_vertices = 4;
    events[1].body.face_recognized.points[0].x = 1000;
    events[1].body.face_recognized.points[0].y = 1000;
    events[1].body.face_recognized.points[1].x = 1000;
    events[1].body.face_recognized.points[1].y = 9000;
    events[1].body.face_recognized.points[2].x = 9000;
    events[1].body.face_recognized.points[2].y = 9000;
    events[1].body.face_recognized.points[3].x = 9000;
    events[1].body.face_recognized.points[3].y = 1000;
    strcpy(events[1].body.face_recognized.person_name, "Lei Wang");
    strcpy(events[1].body.face_recognized.group_id, "D6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
    strcpy(events[1].body.face_recognized.person_id, "F6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");
    strcpy(events[1].body.face_recognized.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    strcpy(events[1].reserve_str[0], "F6D8D887-8F4D-4647-ADE7-BDF5A88D58A0");


    //events[2] is object detected
    strcpy(events[2].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A7");
    strcpy(events[2].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A7");
    strcpy(events[2].base.rule_name, "object detected");
    events[2].base.event_time = 1459623540000;
    events[2].base.event_type = JSON_EVENT_OBJECT_DETECTED;
    events[2].base.priority = 0;
    events[2].body.object_detected.object_id = 101;
    events[2].body.object_detected.x = 4000;
    events[2].body.object_detected.y = 7000;
    events[2].body.object_detected.height = 100;
    events[2].body.object_detected.width = 200;
    events[2].body.object_detected.cnt_zone_vertices = 4;
    events[2].body.object_detected.points[0].x = 1000;
    events[2].body.object_detected.points[0].y = 1000;
    events[2].body.object_detected.points[1].x = 1000;
    events[2].body.object_detected.points[1].y = 9000;
    events[2].body.object_detected.points[2].x = 9000;
    events[2].body.object_detected.points[2].y = 9000;
    events[2].body.object_detected.points[3].x = 9000;
    events[2].body.object_detected.points[3].y = 1000;
    strcpy(events[2].body.object_detected.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");

    //events[3] is object classified
    strcpy(events[3].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
    strcpy(events[3].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");
    strcpy(events[3].base.rule_name, "object classified");
    events[3].base.event_time = 1459623540000;
    events[3].base.event_type = JSON_EVENT_OBJECT_CLASSIFIED;
    events[3].base.priority = 0;
    events[3].body.object_classified.object_id = 101;
    events[3].body.object_classified.confidence = 100;
    events[3].body.object_classified.type = 1; //person
    events[3].body.object_classified.x = 4000;
    events[3].body.object_classified.y = 7000;
    events[3].body.object_classified.height = 100;
    events[3].body.object_classified.width = 200;
    events[3].body.object_classified.cnt_zone_vertices = 4;
    events[3].body.object_classified.points[0].x = 1000;
    events[3].body.object_classified.points[0].y = 1000;
    events[3].body.object_classified.points[1].x = 1000;
    events[3].body.object_classified.points[1].y = 9000;
    events[3].body.object_classified.points[2].x = 9000;
    events[3].body.object_classified.points[2].y = 9000;
    events[3].body.object_classified.points[3].x = 9000;
    events[3].body.object_classified.points[3].y = 1000;
    strcpy(events[3].body.object_classified.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");
    //events[4] is a compound event
    strcpy(events[4].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A9");
    strcpy(events[4].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58A9");
    strcpy(events[4].base.rule_name, "a person detected in a zone");
    events[4].base.event_time = 1459623540000;
    events[4].base.event_type = JSON_EVENT_COMPOUND_EVENT;
    events[4].base.priority = 0;
    events[4].body.compound_event.operation = 0;
    strcpy(events[4].body.compound_event.sub_event_id1, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A8");//person is classified
    strcpy(events[4].body.compound_event.sub_event_id2, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58A7");//object is detected
    json_event_generator_output_json(event_generator_handle, 1459623540000, 3060, events, 5);
    char * buf2=0;
    json_event_generator_serialize(event_generator_handle, 1459623540000, 3060, events, 5, &buf2);
//    printf("event string:\n%s\n", buf2);

    for (int i = 0; i<10; i++)
        memset(&events[i], 0, sizeof(struct json_event));
    ////events[0] is abandoned object detected
    //strcpy(events[0].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58B0");
    //strcpy(events[0].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58B0");
    //strcpy(events[0].base.rule_name, "abandoned object detected");
    //events[0].base.event_time = 1459623600000;
    //events[0].base.event_type = JSON_EVENT_ABANDONED_OBJECT_DETECTED;
    //events[0].base.priority = 0;
    //events[0].body.abandoned_object_detected.object_id = 101;
    //events[0].body.abandoned_object_detected.duration = 100;
    //events[0].body.abandoned_object_detected.x = 5000;
    //events[0].body.abandoned_object_detected.y = 7000;
    //events[0].body.abandoned_object_detected.height = 400;
    //events[0].body.abandoned_object_detected.width = 400;
    //events[0].body.abandoned_object_detected.cnt_zone_vertices = 4;
    //events[0].body.abandoned_object_detected.points[0].x = 1000;
    //events[0].body.abandoned_object_detected.points[0].y = 1000;
    //events[0].body.abandoned_object_detected.points[1].x = 1000;
    //events[0].body.abandoned_object_detected.points[1].y = 9000;
    //events[0].body.abandoned_object_detected.points[2].x = 9000;
    //events[0].body.abandoned_object_detected.points[2].y = 9000;
    //events[0].body.abandoned_object_detected.points[3].x = 9000;
    //events[0].body.abandoned_object_detected.points[3].y = 1000;
    //strcpy(events[0].body.abandoned_object_detected.zone_id, "C6D8D887-8F4D-4647-ADE7-BDF5A88D58A2");

    //events[1] is JSON_EVENT_RESERVED_1
    strcpy(events[0].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58B1");
    strcpy(events[0].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58B1");
    strcpy(events[0].base.rule_name, "reserved_1 generated");
    events[0].base.event_time = 1459623600000;
    events[0].base.event_type = JSON_EVENT_RESERVED_1;
    events[0].base.priority = 0;
    events[0].reserve[0] = 100;
    events[0].reserve[1] = 50;
    strcpy(events[0].reserve_str[0],"reserved_event message");
    //events[1].body.heat_map.start_time = 1459620000000;
    //events[1].body.heat_map.end_time = 1459623600000;
    //events[1].body.heat_map.rows_per_column = 4;
    //events[1].body.heat_map.cells_per_row = 4;
    //events[1].body.heat_map.data = malloc(16 * sizeof(uint16_t));
    //for (int i = 0; i < 16; i++)
    //    events[1].body.heat_map.data[i] = i * 4;

    //events[2] is extension
    strcpy(events[1].base.event_id, "A6D8D887-8F4D-4647-ADE7-BDF5A88D58B2");
    strcpy(events[1].base.rule_id, "B6D8D887-8F4D-4647-ADE7-BDF5A88D58B2");
    strcpy(events[1].base.rule_name, "engine A extension");
    events[1].base.event_time = 1459623600000;
    events[1].base.event_type = JSON_EVENT_RESERVED_1;
    events[1].base.priority = 0;
    //events[2].body.extension.size = 4;
    //events[2].body.extension.payload = malloc(events[0].body.extension.size);
    //strcpy(events[2].body.extension.payload, "ABC");
    json_event_generator_output_json(event_generator_handle, 1459623600000, 108000, events, 2);
   // free(events[1].body.heat_map.data);
    free(events[1].body.extension.payload);
    json_event_generator_deinit(event_generator_handle);
}

//test event parser
//write the pared events back to a json file via event generator
void json_event_parser_test()
{
    void * event_parser_handle;
    void * event_generator_handle;
    enum EventParserStatus res;
    //initialize event parser
    res = json_event_parser_init(
        "event_generation_test",//json event file generated from json_event_generator_test()
        JSON_EVENT_FILE,        //event source is file
        1459623438000,          //start time in millisecond
        1459623438000+2000*1000,//end time in millisecond
        3600,                   //period of event files in secnond
        &event_parser_handle    //returned handle
        );

    //Initialize event generator
    json_event_generator_init("event_parser_test", JSON_EVENT_FILE,
        1459623438000, 3600, &event_generator_handle);

    if (res != EP_OK)
        return;
    while (1){
        struct json_event *events;
        int cnt_events;
        //grab and parse an events packet
        res = json_event_parser_parse_next_packet(event_parser_handle,
            &events, &cnt_events);
        if (res != EP_OK)
        {
            printf("status in parsing:%d\n",res);
            break;
        }
        if (cnt_events > 0){
            json_event_generator_output_json(event_generator_handle,
                events[0].base.event_time, events[0].base.frame_id, events, cnt_events);
        }
    }
    json_event_parser_deinit(event_parser_handle);
    json_event_generator_deinit(event_generator_handle);
}
int main(int argc, char * argv[])
{
    printf("JsonMetadataGenerator_example\n");
    JsonMetadataGenerator_example();
    printf("JsonMetadataParseTest\n");
    JsonMetadataParseTest();
    printf("JsonConfigurationGeneratorTest\n");
    JsonConfigurationGeneratorTest();
    printf("JsonConfigurationParserTest\n");
    JsonConfigurationParserTest();
    printf("json_event_generator_test\n");
    json_event_generator_test();
    printf("json_event_parser_test\n");
    json_event_parser_test();

    return 0;
}

