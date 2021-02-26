# Custom Engine Template - Engine Generator
=======================================================================
Author:
  Chester Yen

=======================================================================
Files:
    includes a wrapper template file.

=======================================================================
VERSION:
  vam engine api version: 201605121415

=======================================================================
QUICK START:
  Please modify functions in src/vam_engine_wrapper_template.cpp accordingly and then run

  $ bitbake -v -f vam-test

  A dynamic VA engine library will be created in usr/lib64 folder

=======================================================================
CALL FLOW:
    Following is the pseudo-code from VAM to per engine:
    ====start===========================================
        init();

        //query for desired FPS
        get_target_FPS();

        for-each(event type)
        {
            //query for supported events
            is_event_supported();

            //query for event dependency
            is_depends_on_other_event();
        }

        if(user enabled supported event)
        {
            //add event-associated rule configuration
            add_rule(event type);

            for(per video frame)
            {
                //feed in frame data
                process(frame data);

                get_event_count()
                if(event_count > 0)
                    get_events()

                get_object_count()
                if(object_count > 0)
                    get_objects()
            }
        }

        deinit();
    ====end=============================================

=======================================================================
Engine APIs:

/**
 * Creates an Engine instance.
 * This function will be the first call in all APIs and will be call only
 * once per session.
 * handle is an optional parameter which can be used to customize engine
 * instance creation. The handle will then be passed to all APIs.
 * source_info contains video paramters such as width, height and format
 * for both high resolution frame and preview (low resolution) frame.
 * All params for low resolution frame will be set to 0 if not available.
 * data_folder is a readable/writable file path assigned to this engine.
 * Engine could save individual pre-trained or required data in that
 * folder when plug-in the engine to VAM.
 */
int32_t (*init)(const struct vaapi_source_info *source_info, void **handle);

/**
 * Free up engine resources.
 * This function will be the last call in all APIs and will be call only
 * once per session.
 */
int32_t (*deinit)( void *handle);

/**
 * Process input frame. Engine does the frame processing and extracts
 * relevant information.
 * This function will be called everytime when a frame captured from the
 * camera or the video. The attached hi_res and low_res frames are always
 * in sync with same timestamp.
 * The function should always be a blocking call. The frame raw data pointers
 * will be no longer valid after function returns.
 * The engine is highly recommented not to copy but directly use the raw data
 * unless the estimated process time for current frame is over 100 ms.
 * In the case this function takes longer process time, VAM could start to
 * drop frame when frame queue for the engine is full.
 * The objects will always be NULL, unless the engine claims to depend on
 * other events (see is_depends_on_other_event). In that case, the objects
 * will be filled with objects information detected from other engines.
 */
int32_t (*process)(void *handle, struct vaapi_frame_info *frame_info);

/**
 * add or update the rule configuration related to this engine.
 * This function could be called multiple times per session.
 * The engine should preserve the rule content inside the engine. The pointer
 * will be no longer valid after function returns.
 * Multiple rules could be assigned to the engine. Rules could be
 * distinguished by the unique rule ID.
 * The rule contains both common fields and reserved fields. Please refer to
 * configuration document for further details.
 */
int32_t (*add_rule)(void *handle, vaapi_rule *rule);

/**
 * delete a rule based on its unique rule ID.
 * This function could be called multiple times per session.
 */
int32_t (*del_rule)(void *handle, const char *rule_id);

/**
 * Enroll an image or object for recognition
 * This function could be called once per image for the enorll target.
 * Example: In face reocgnition, assuming 3 images with unique image ID
 * are enrolled to same person, this function will get called 3 times.
 * An image should be enroll only once per session.
 */
int32_t (*enroll_obj)(void *handle, struct vaapi_enrollment_info *enroll_info);

/**
 * Dis-Enroll an image or object from recognition
 * this function could be called multiple times per session.
 * In face recognition case, the obj_id could be either a person ID or
 * an image ID.
 * If the obj_id is an image ID, a single image should be dis-enrolled.
 * If the obj_id is a person ID, engine should dis-enroll all associated
 * images from that person
 */
int32_t (*disenroll_obj)(void *handle, const char *obj_id);

/**
 * Retrieves size of detected events in current frame.
 * This function will be called once after process() is called.
 * Engine returns the event size detected in current frame.
 */
int32_t (*get_event_count)(void *handle, uint32_t *event_count);

/**
 * Retrieves event information associated with current frame.
 * This function will be called once everytime when get_event_count()
 * returns a positive value.
 * The pointer *events will be allocated properly in VAM based on
 * the returned event size. Engine should fill the event strcutre
 * with all event details.
 * Please read event schema document for further information about
 * events.
 */
int32_t (*get_events)(void *handle, struct vaapi_event *events, uint32_t num_events);

/**
 * Retrieves size of detected objects in current frame.
 * This function will be called once after process() is called.
 * Engine returns the object size detected in current frame.
 */
int32_t (*get_object_count)(void *handle, uint32_t *obj_count);

/**
 * Retrieves object information associated with current frame.
 * This function will be called once everytime when get_object_count()
 * returns a positive value.
 * The pointer *objects will be allocated properly in VAM based on
 * the returned object size. Engine should fill the object strcutre
 * with all object details. Please read metadata schema document for
 * further information about objects.
 */
int32_t (*get_objects)(void *handle, vaapi_object *objects, uint32_t obj_count);

/**
 * Retrieves engine's desired frame rate
 * This function will be called once per session.
 * VAM will feed in raw video frame through process() based on the
 * desired frame rate that engine specified.
 * For example:
 *   If the video is 30 fps and engine required 10 fps. VAM will feed in
 * every 3rd frame into the engine.
 */
int32_t (*get_target_FPS)(void *handle, uint8_t *desired_fps);

/**
 * Retrieves engine's support on events.
 * This function will be called multiple times per session.
 * VAM will assign associated rules to engine based on its supported event
 * type.
 */
int32_t (*is_event_supported)(void *handle, vaapi_event_type type, uint8_t *is_supported);

/**
 * Retrieves engine's dependency on other events.
 * This function will be called once per event type in a session.
 * If the engine requests a dependency on other events, in the process()
 * VAM will pass not only raw video data but also the output objects from
 * engines which support the dependant event type to this engine.
 */
int32_t (*is_depends_on_other_event)(void *handle, vaapi_event_type type, uint8_t *is_depended);


=======================================================================

For questions or bugs, please report to chenlany@qti.qualcomm.com
Thank you.
Chester

