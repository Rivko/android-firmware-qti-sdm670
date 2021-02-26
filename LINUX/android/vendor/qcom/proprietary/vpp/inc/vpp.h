/*!
 * @file vpp.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services Defines the external interface for VPP library.
 */

#ifndef _VPP_LIBRARY_H_
#define _VPP_LIBRARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HQV_CUSTOM_PAYLOAD_SZ   256

#define CADE_LEVEL_MIN          0
#define CADE_LEVEL_MAX          100
#define CADE_CONTRAST_MIN       (-50)
#define CADE_CONTRAST_MAX       50
#define CADE_SATURATION_MIN     (-50)
#define CADE_SATURATION_MAX     50
#define TNR_LEVEL_MIN           0
#define TNR_LEVEL_MAX           100
#define CNR_LEVEL_MIN           0
#define CNR_LEVEL_MAX           100
#define AIE_CADE_LEVEL_MIN      0
#define AIE_CADE_LEVEL_MAX      100
#define AIE_LTM_LEVEL_MIN       0
#define AIE_LTM_LEVEL_MAX       100
#define AIE_LTM_SAT_GAIN_MIN    0
#define AIE_LTM_SAT_GAIN_MAX    100
#define AIE_LTM_SAT_OFFSET_MIN  0
#define AIE_LTM_SAT_OFFSET_MAX  100
#define AIE_LTM_ACE_STR_MIN     0
#define AIE_LTM_ACE_STR_MAX     100
#define AIE_LTM_ACE_BRI_L_MIN   0
#define AIE_LTM_ACE_BRI_L_MAX   100
#define AIE_LTM_ACE_BRI_H_MIN   0
#define AIE_LTM_ACE_BRI_H_MAX   100
#define SPLIT_PERCENT_MIN       0
#define SPLIT_PERCENT_MAX       100
#define SPLIT_PERCENT_DEFAULT   100

#define RES_SD_MAX_W            800
#define RES_SD_MAX_H            600
#define RES_HD_MAX_W            1366
#define RES_HD_MAX_H            768
#define RES_FHD_MAX_W           1920
#define RES_FHD_MAX_H           1088
#define RES_UHD_MAX_W           4096
#define RES_UHD_MAX_H           2176

#define META_MAX_CNT            64

enum vpp_error {
    VPP_OK = 0,
    VPP_ERR,
    VPP_PENDING,
    VPP_ERR_STATE,
    VPP_ERR_INVALID_CFG,
    VPP_ERR_PARAM,
    VPP_ERR_NO_MEM,
    VPP_ERR_RESOURCES,
    VPP_ERR_HW,
};

enum vpp_resolution {
    VPP_RESOLUTION_SD,
    VPP_RESOLUTION_HD,
    VPP_RESOLUTION_FHD,
    VPP_RESOLUTION_UHD,
    VPP_RESOLUTION_MAX,
};

enum hqv_mode {
    HQV_MODE_OFF,
    HQV_MODE_AUTO,
    HQV_MODE_MANUAL,
    HQV_MODE_MAX,
};


#define HQV_CONTROL_GLOBAL_START (HQV_CONTROL_CUST + 1)
enum hqv_control_type {
    // All standard IP algorithm controls should be added above HQV_CONTROL_CUST
    HQV_CONTROL_NONE, // Control type is still checked for HQV mode Auto, so have NONE at index 0
    HQV_CONTROL_CADE,
    HQV_CONTROL_DI,
    HQV_CONTROL_TNR,
    HQV_CONTROL_CNR,
    HQV_CONTROL_AIE,
    HQV_CONTROL_FRC,
    HQV_CONTROL_EAR,
    HQV_CONTROL_QBR,
    HQV_CONTROL_MEAS,
    HQV_CONTROL_CUST, // Custom/advanced controls
    // Global controls independent of use cases start after custom
    HQV_CONTROL_GLOBAL_DEMO = HQV_CONTROL_GLOBAL_START,
    HQV_CONTROL_MAX,
};

enum hqv_di_mode {
    HQV_DI_MODE_OFF,
    HQV_DI_MODE_VIDEO_1F,
    HQV_DI_MODE_VIDEO_3F,
    HQV_DI_MODE_AUTO,
    HQV_DI_MODE_MAX,
};

enum hqv_hue_mode {
    HQV_HUE_MODE_OFF,
    HQV_HUE_MODE_ON,
    HQV_HUE_MODE_MAX,
};

enum hqv_split_direction {
    HQV_SPLIT_LEFT_TO_RIGHT,
    HQV_SPLIT_RIGHT_TO_LEFT,
    HQV_SPLIT_TOP_TO_BOTTOM,
    HQV_SPLIT_BOTTOM_TO_TOP,
    HQV_SPLIT_MAX,
};

enum hqv_frc_mode {
    HQV_FRC_MODE_OFF,
    HQV_FRC_MODE_SMOOTH_MOTION,
    HQV_FRC_MODE_SLOMO,
    HQV_FRC_MODE_MAX,
};

enum hqv_frc_level {
    HQV_FRC_LEVEL_OFF,
    HQV_FRC_LEVEL_LOW,
    HQV_FRC_LEVEL_MED,
    HQV_FRC_LEVEL_HIGH,
    HQV_FRC_LEVEL_MAX,
};

enum hqv_frc_interp {
    HQV_FRC_INTERP_1X,
    HQV_FRC_INTERP_2X,
    HQV_FRC_INTERP_3X,
    HQV_FRC_INTERP_4X,
    HQV_FRC_INTERP_MAX,
};

enum hqv_qbr_mode {
    HQV_QBR_MODE_OFF,
    HQV_QBR_MODE_ON,
    HQV_QBR_MODE_MAX,
};

enum hqv_ear_mode {
    HQV_EAR_MODE_OFF,
    HQV_EAR_MODE_BYPASS,
    HQV_EAR_MODE_LOW,
    HQV_EAR_MODE_MEDIUM,
    HQV_EAR_MODE_HIGH,
    HQV_EAR_MODE_STREAM_ADAPTIVE,
    HQV_EAR_MODE_FRAME_ADAPTIVE,
    HQV_EAR_MODE_MAX,
};

struct hqv_ctrl_cade {
    /*! Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
    enum hqv_mode mode;
    /*! Range: 0 - 100 */
    uint32_t cade_level;
    /*! Range: (-50) - 50 */
    int32_t contrast;
    /*! Range: (-50) - 50 */
    int32_t saturation;
};

struct hqv_ctrl_tnr {
    /*! Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
    enum hqv_mode mode;
    /*! Range: 0 - 100 */
    uint32_t level;
};

struct hqv_ctrl_cnr {
    /*! Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
    enum hqv_mode mode;
    /*! Range: 0 - 100 */
    uint32_t level;
};

struct hqv_ctrl_aie {
    /*! Valid values: HQV_MODE_OFF, HQV_MODE_AUTO, HQV_MODE_MANUAL */
    enum hqv_mode mode;
    /*! Valid values: HQV_HUE_MODE_ON, HQV_HUE_MODE_OFF */
    enum hqv_hue_mode hue_mode;
    /*! Range: 0 - 100 */
    uint32_t cade_level;
    /*! Range: 0 - 100 */
    uint32_t ltm_level;
    /*! Range: 0 - 100 */
    uint32_t ltm_sat_gain;
    /*! Range: 0 - 100 */
    uint32_t ltm_sat_offset;
    /*! Range: 0 - 100 */
    uint32_t ltm_ace_str;
    /*! Range: 0 - 100 */
    uint32_t ltm_ace_bright_l;
    /*! Range: 0 - 100 */
    uint32_t ltm_ace_bright_h;
};

struct hqv_ctrl_di {
    /*! Valid values: HQV_DI_MODE_OFF, HQV_DI_MODE_1F, HQV_DI_MODE_3F */
    enum hqv_di_mode mode;
};

struct hqv_ctrl_frc {
    /*! Valid values: HQV_FRC_MODE_OFF, HQV_FRC_MODE_SMOOTH_MOTION, HQV_FRC_MODE_SLOMO */
    enum hqv_frc_mode mode;
    /*! Valid values: HQV_FRC_LEVEL_OFF, HQV_FRC_LEVEL_LOW, HQV_FRC_LEVEL_MED, HQV_FRC_LEVEL_HIGH */
    enum hqv_frc_level level;
    /*! Valid values: HQV_FRC_INTERP_1X, HQV_FRC_INTERP_2X, HQV_FRC_SLOMO_3X, HQV_FRC_INTERP_4X */
    /*! This parameter is only used if mode=HQV_FRC_MODE_SLOMO */
    enum hqv_frc_interp interp;
};

struct hqv_ctrl_qbr {
    enum hqv_qbr_mode mode;
};

struct hqv_ctrl_ear {
    enum hqv_ear_mode mode;
};

struct hqv_ctrl_meas {
    uint32_t enable;
};

struct hqv_ctrl_global_demo {
    /*! Range: 0 - 100 */
    uint32_t process_percent;
    enum hqv_split_direction process_direction;
};

struct hqv_ctrl_custom {
    uint32_t id;
    uint32_t len;
    char data[HQV_CUSTOM_PAYLOAD_SZ];
};

struct hqv_control {
    /*!
     * HQV_MODE_OFF: HQV is turned off and hardware will transfer buffers from
     * input to output.
     *
     * HQV_MODE_AUTO: HQV will automatically determine the best settings for
     * the applicable parameters. HQV controls will be ignored, with the
     * exception of the custom control.
     *
     * HQV_MODE_MANUAL: HQV will apply the settings in the corresponding HQV
     * controls.
     */
    enum hqv_mode mode;

    /*!
     * This field is only valid if mode is set to HQV_MODE_MANUAL or
     * HQV_MODE_AUTO (custom and global controls are allowed in AUTO). Specifies the
     * HQV control that is being set. Corresponds to one of the fields in the union.
     */
    enum hqv_control_type ctrl_type;

    union {
        struct hqv_ctrl_cade cade;
        struct hqv_ctrl_tnr tnr;
        struct hqv_ctrl_cnr cnr;
        struct hqv_ctrl_aie aie;
        struct hqv_ctrl_di di;
        struct hqv_ctrl_frc frc;
        struct hqv_ctrl_ear ear;
        struct hqv_ctrl_qbr qbr;
        struct hqv_ctrl_meas meas;
        struct hqv_ctrl_custom custom;
        struct hqv_ctrl_global_demo demo;
    };
};

enum vpp_codec_type {
    VPP_CODEC_TYPE_UNKNOWN,
    VPP_CODEC_TYPE_AVC,
    VPP_CODEC_TYPE_DIVX4,
    VPP_CODEC_TYPE_DIVX,
    VPP_CODEC_TYPE_DIVX311,
    VPP_CODEC_TYPE_MPEG4,
    VPP_CODEC_TYPE_MPEG2,
    VPP_CODEC_TYPE_VC1,
    VPP_CODEC_TYPE_WMV,
    VPP_CODEC_TYPE_H263,
    VPP_CODEC_TYPE_HEVC,
    VPP_CODEC_TYPE_VP8,
    VPP_CODEC_TYPE_VP9,
    VPP_CODEC_TYPE_MAX,
};

enum vid_prop_type {
    VID_PROP_CODEC,
    VID_PROP_NON_REALTIME,
    VID_PROP_OPERATING_RATE,
    VID_PROP_MAX,
};

struct vid_prop_codec {
    /*! Codec type.  Can be used by an IP block that needs this info */
    enum vpp_codec_type eCodec;
};

struct vid_prop_non_realtime {
    /*! Setting to 1 will indicate this session has non-realtime processing requirements */
    uint32_t bNonRealtime;
};

struct vid_prop_operating_rate {
    /*!
     * If zero (default), use timestamp calculated framerate;
     * Otherwise use this value. The rate should be an integer representing frames per second
     */
    uint32_t u32OperatingRate;
};


struct video_property {
    /*!
     * Specifies the HQV property that is being set. Corresponds to one of the fields
     * in the union.
     */
    enum vid_prop_type property_type;

    union {
        struct vid_prop_codec codec;
        struct vid_prop_non_realtime non_realtime;
        struct vid_prop_operating_rate operating_rate;
    };
};

enum vpp_session_flags {
    VPP_SESSION_SECURE          = (1 << 0),
    VPP_SESSION_NON_REALTIME    = (1 << 1),
};

enum vpp_color_format {
    // Linear formats
    VPP_COLOR_FORMAT_NV12_VENUS,
    VPP_COLOR_FORMAT_NV21_VENUS,
    VPP_COLOR_FORMAT_P010,

    // Compressed formats
    VPP_COLOR_FORMAT_UBWC_NV12,
    VPP_COLOR_FORMAT_UBWC_NV21,
    VPP_COLOR_FORMAT_UBWC_TP10,

    // Max should be last
    VPP_COLOR_FORMAT_MAX,
};

enum vpp_port {
    VPP_PORT_INPUT,
    VPP_PORT_OUTPUT,
    VPP_PORT_MAX,
};

struct vpp_port_factor {
    uint32_t add;
    uint32_t mul;
};

struct vpp_port_param {
    uint32_t height;
    uint32_t width;
    uint32_t stride;
    uint32_t scanlines;
    enum vpp_color_format fmt;
};

struct vpp_requirements {
    struct vpp_port_factor in_factor[VPP_RESOLUTION_MAX];
    struct vpp_port_factor out_factor[VPP_RESOLUTION_MAX];
    /*!
     * Bitmask of the supported color format.
     * Based on vpp_color_format, if the color format is supported,
     * the corresponding bit is 1.
     * eg: if color_fmt_mask & (0x1 << VPP_COLOR_FORMAT_NV12_VENUS) is non-zero
     * it means VPP_COLOR_FORMAT_NV12_VENUS is supported
     * Only "in" is defined now since color conversion is not supported.
     */
    uint64_t in_color_fmt_mask;
    struct {
        /*!
         * Number of metadata required by VPP. Denotes the size of meta.
         */
        uint32_t cnt;

        /*!
         * Array of size cnt, each entry containing a metadata required by
         * the VPP.
         */
        uint32_t meta[META_MAX_CNT];
    } metadata;
};

enum vpp_event_type {
    VPP_EVENT_FLUSH_DONE        = (1 << 0),
    VPP_EVENT_RECONFIG_DONE     = (1 << 1),
    VPP_EVENT_ERROR             = (1 << 2),
};

struct vpp_event {
    /*! Specifies the event. */
    enum vpp_event_type type;

    union {
        /*! Valid when the event type is VPP_EVENT_FLUSH_DONE. */
        struct {
            /*! The port that completed flushing */
            enum vpp_port port;
        } flush_done;

        /*! Valid when the event type is VPP_EVENT_RECONFIG_DONE. */
        struct {
            /*!
             * Status of the reconfigure. If this is set to error, then the
             * client should bypass the VPP.
             */
            uint32_t reconfig_status;
            /*! The new requirements of VPP. */
            struct vpp_requirements req;
        } port_reconfig_done;
    };
};

enum vpp_buffer_flag {
    VPP_BUFFER_FLAG_EOS         = (1 << 0),
    VPP_BUFFER_FLAG_DATACORRUPT = (1 << 1),
    VPP_BUFFER_FLAG_SYNCFRAME   = (1 << 2),
    VPP_BUFFER_FLAG_READONLY    = (1 << 3),
    VPP_BUFFER_FLAG_EXTRADATA   = (1 << 4),
};

struct vpp_mem_buffer {
    /*! File descriptor of the ion buffer. */
    int fd;

    /*! Offset of the buffer from the base address, in bytes */
    int offset;

    /*! Physically allocated buffer length, in bytes */
    uint32_t alloc_len;

    /*! Filled buffer length, in bytes */
    uint32_t filled_len;

    /*!
     * The number of bytes that *may* contain data.
     *
     * In the typical scenario where there is both pixel data and extra data in
     * a the buffer, this field shall describe the valid data length of each
     * section.  That is, for the pixel region, valid_data_len will describe
     * the maximum number of bytes that can be occupied by pixel data, and for
     * the extra data region, valid_data_len will describe the maximum number
     * of bytes that can be occupied by the extra data. In such a case,
     * alloc_len shall be equal for both the pixel data and the extra data
     * regions.
     *
     * In the case where a single physical buffer contains multiple buffers,
     * this field is used to describe just the memory region that is being
     * described by this structure. As an example, if there are multiple extra
     * data buffers in one physical buffer, the valid_data_len for each extra
     * data buffer shall equal the maximum number of bytes that a specific
     * extra data region data may occupy in that buffer.  In this case, the
     * alloc_len for each buffer segment in the extradata buffer will be equal.
     */
    uint32_t valid_data_len;

    /*!
     * If the buffer has been mmap'd into the calling process's virtual address
     * space at fd, with offset = 0, pvMapped can be used to pass this base
     * address to vpp. In this case, vpp will not try to map the buffer again,
     * but will assume that pvMapped + (char *)offset is the start of valid
     * memory for this buffer. If the buffer has not been mapped, pass NULL
     * to pvMapped.
     */
    void *pvMapped;
};

struct vpp_buffer {
    struct vpp_mem_buffer pixel;
    struct vpp_mem_buffer extradata;

    /*! Buffer flags, as defined by enum vpp_buffer_flag */
    uint32_t flags;

    /*! Timestamp in microseconds */
    uint64_t timestamp;

    /*! Cookie to be copied from the input buffer to the output buffer. */
    void *cookie_in_to_out;

    /*! Cookie owned by the client. */
    void *cookie;

    /*! Pointer to a gralloc handle */
    void *pvGralloc;
};

struct vpp_callbacks {
    /*!
     * Client owned void pointer, returned with each callback. Not modified
     * by VPP.
     */
    void *pv;
    /*!
     * @brief       Return an input buffer to the client.
     *
     * @description Called when an input buffer has been processed and can be
     *              released from VPP library. Returns the pointer to the
     *              buffer that was enqueued on the input port.
     *
     * @input       buf     Returned buffer
     */
    void (*input_buffer_done)(void *pv, struct vpp_buffer *buf);

    /*!
     * @brief       Return a buffer to be displayed to the client.
     *
     * @description Called when a buffer has been processed and is destined for
     *              the display.  Buffers that are returned in this way may
     *              have been originally queued to the VPP library on the input
     *              or output port.
     *
     * @input       buf     Returned buffer
     */
    void (*output_buffer_done)(void *pv, struct vpp_buffer *buf);

    /*!
     * @brief       Indicate to the client an event
     *
     * @description Called when there is an event of significance within the
     *              VPP that the client should know about.
     *
     * @input       e       The event
     */
    void (*vpp_event)(void *pv, struct vpp_event e);
};

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

/*!
 * @brief       Synchronous. Optional. Boots the VPP Library
 *
 * @description This is an optional function that can be used to perform any
 *              boot tasks that would help save time when init is called later.
 *              In the case of VPP running as a service, boot can be called when
 *              the service starts, performing tasks that can later be skipped
 *              when a playback session is started with init.
 *
 * @return      OK          if no error while booting
 *              ERROR       if there was an error during the boot process
 */
uint32_t vpp_boot();

/*!
 * @brief       Synchronous. Optional. Shuts down the VPP library.
 *
 * @description This is an optional function that is used to perform shutdown of
 *              the VPP Library. It cleans up operations performed during boot.
 *
 * @return      OK          if no error while shutting down
 *              ERROR       if there was an error during the shutdown process
 */
uint32_t vpp_shutdown();

/*!
 * @brief       Synchronous. Initialize the VPP library.
 *
 * @description Initializes the VPP library. Session level parameters shall be
 *              passed during this time. Such information can refer to content
 *              protection state of the session.
 *
 * @input       flags       Indicates session level parameters
 * @input       cb          The set of callbacks for events of interest.
 *
 * @return      NULL if failure, otherwise a context pointer which shall not be
 *              modified by the client of VPP library, and shall be passed in
 *              to the VPP library with every subsequent call.
 */
void *vpp_init(uint32_t flags, struct vpp_callbacks cb);

/*!
 * @brief       Synchronous. Terminates the VPP library.
 *
 * @description Indicate to the VPP library that its services are no longer
 *              needed. As a result, this will invalidate the context and no
 *              subsequent calls may be made to the VPP library without first
 *              calling vpp_init.
 *
 *              It is strongly suggested that vpp_flush is called first, and
 *              all buffers are returned to the client, prior to calling this
 *              function.
 *
 * @input       ctx         Context returned from vpp_init.
 *
 * @return      void
 */
void vpp_term(void *ctx);

/*!
 * @brief       Synchronous. Configures VPP control parameters.
 *
 * @description Indicate to the VPP library that a control, specified in ctrl
 *              should be set. Upon returning from this function, the library
 *              will indicate the buffering requirements and necessary metadata
 *              for the session.
 *
 *              This function should be called multiple times, foreach control
 *              that should be turned on.
 *
 * @input       ctx         Context returned from vpp_init
 * @input       ctrl        The control parameters
 * @output      req         Requirements from VPP for this session
 *
 * @return      TODO: should we use this return to indicate if the set of
 *              controls is not valid and we can not configure for this session?
 */
uint32_t vpp_set_ctrl(void *ctx, struct hqv_control ctrl, struct vpp_requirements *req);

/*!
 * @brief       Synchronous. Set the parameters for a given port.
 *
 * @description This function should be called once for each port. When the VPP
 *              library has received a call for both ports, it shall transition
 *              into active video state, where buffers can then be exchanged.
 *
 * @input       ctx         Context returned from vpp_init
 * @input       port        The port to set the parameters on
 * @input       param       The parameters to set on the port
 *
 * @return      OK          if no error and can exchange buffers
 *              PENDING     if waiting for another parameter on another port
 *              ERROR       if there was an error with the set_parameter call
 */
uint32_t vpp_set_parameter(void *ctx, enum vpp_port port, struct vpp_port_param param);

/*!
 * @brief       Asynchronous. Enqueue a buffer to the specified port.
 *
 * @description Gives a buffer to the VPP library. If the port is on the input
 *              port, this will indicate that this buffer should be processed
 *              by the VPP library. If the port is on the output port, this
 *              will indicate that this buffer should be written into.
 *
 *              It is possible that a buffer that is queued on the input port
 *              is returned on the output port.
 *
 * @input       ctx         Context returned from vpp_init
 * @input       port        The port to set the parameters on
 * @input       buf         The buffer descriptor
 *
 * @return      OK          if there is no error with accepting the buffer
 *              ERROR       if there is an error
 */
uint32_t vpp_queue_buf(void *ctx, enum vpp_port port, struct vpp_buffer *buf);

/*!
 * @brief       Asynchronous. Request for a port reconfiguration.
 *
 * @description Requests for a port reconfiguration. The new input and output
 *              port sizes shall be given to the VPP library with this request.
 *
 *              Upon receiving a reconfigure request, it is expected that no
 *              more input buffers shall be queued on the input port. Output
 *              buffers are expected to continue to be queued, so that there
 *              are enough output buffers for the VPP to process all of the
 *              pending input buffers.
 *
 *              When all of the input buffers have been processed, VPP library
 *              shall send an event indicating that the reconfigure has been
 *              completed. With this event, VPP will convey a new set of
 *              requirements.
 *
 *              It is recommended that after the PORT_RECONFIGURE_EVENT has
 *              been received, the client flushes both the input and output
 *              ports of the VPP, prior to queuing any new buffers.
 *
 * @input       ctx         Context returned from vpp_init
 * @input       input_param The new input port parameters
 * @input       output_param The new output port parameters
 *
 * @return      OK          if the request was accepted
 *              ERROR       if the request can not be accepted
 */
uint32_t vpp_reconfigure(void *ctx,
                         struct vpp_port_param input_param,
                         struct vpp_port_param output_param);

/*!
 * @brief       Asynchronous. Request that a port is flushed.
 *
 * @description Requests that a given port is flushed. This call is
 *              asynchronous. Upon receiving this call, VPP will queue the
 *              request. All of the buffers that are on the specified port
 *              shall be returned. If there are any in flight buffers, those
 *              buffers will finish processing first and be returned via
 *              input_buffer_done or output_buffer done callbacks. Once all in
 *              flight buffers have been processed, then any buffers that are
 *              in the queues on the specified port shall be returned, with
 *              their filled_len set to 0.
 *
 *              Once the flush for the specified port has been returned, the
 *              VPP will raise the VPP_EVENT_FLUSH_DONE event, indicating the
 *              port that has been flushed.
 *
 * @input       ctx         Context returned from vpp_init
 * @input       port        The port to flush
 *
 * @return      OK          if the request was accepted
 *              ERROR       if the request can not be accepted
 */
uint32_t vpp_flush(void *ctx, enum vpp_port port);

/*!
 * @brief       Synchronous. Sets VPP video properties.
 *
 * @description Indicate to the VPP library that a video property, specified in prop,
 *              should be set. The property will be stored in the context, available
 *              for any blocks that may need it.
 *
 *              This function should be called multiple times, for each property
 *              that needs to be set.
 *
 * @input       ctx         Context returned from vpp_init
 * @input       prop        The video property parameters
 *
 * @return      OK           if the property was accepted and stored
 *              ERROR        if the property was not accepted
 */
uint32_t vpp_set_vid_prop(void *ctx, struct video_property prop);

#ifdef __cplusplus
 }
#endif

#endif /* _VPP_LIBRARY_H_ */
