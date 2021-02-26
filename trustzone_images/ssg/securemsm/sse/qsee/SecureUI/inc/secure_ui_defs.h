/** @file secure_ui_defs.h
*/

/*===========================================================================
 * Copyright(c) 2013-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
					EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/07/16   dr      Increase max image width to support landscape orientation
04/05/16   dr      Add Android service error code
12/10/15   kl      (Tech Pubs) Updates for 80-NJ546-1 Rev. J.
===========================================================================*/

#ifndef _SEC_UI_DEFS_H
#define _SEC_UI_DEFS_H

#include <stdint.h>

/* We assume the following max values to avoid recurrent dynamic allocations of small buffers */

/** @addtogroup qsee_core_layer
@{ */
/** @name Maximum Values
Qualcomm's Secure UI layout is subject to the limitations in the following constants:
@{ */
/** Maximum image line width in pixels. */
#define MAX_IMAGE_LINE_WIDTH_IN_PIXELS 4096
/** Maximum image size in pixels (width * height). */
#define MAX_IMAGE_SIZE_IN_PIXELS       90000
/** Maximum number of lines in the text label. */
#define MAX_LINES_IN_TEXT_LABEL        100
/** Maximum hidden field length in characters. */
#define MAX_HIDDEN_FIELD_LEN_IN_CHARS  100
/** @} */ /* end_namegroup */

/** Available status and error codes.*/
 typedef enum {
    SEC_UI_SUCCESS                   = 0,    /**< Success. */
    SEC_UI_GENERAL_ERROR             = -1,   /**< General error. */
    SEC_UI_LISTENER_ERROR            = -2,   /**< Error reported by the HLOS listener service. */
    SEC_UI_ALREADY_RUNNING           = -3,   /**< Secure UI is already running and should not be restarted. */
    SEC_UI_IS_NOT_RUNNING            = -4,   /**< Secure UI is not running; call the start function(s) first. */
    SEC_UI_NON_CONTIGUOUS_BUFFER     = -5,   /**< Invalid memory format received from HLOS. */
    SEC_UI_COPY_TO_BUFFER_FAILED     = -6,   /**< Error reading from a file input to the display buffer. */
    SEC_UI_NON_SECURE_BUFFER         = -7,   /**< Display buffer is unprotected. */
    SEC_UI_QSEE_SECURE_BUFFER_ERROR  = -8,   /**< Error mapping the display buffer in the TrustZone software. */
    SEC_UI_NOT_SUPPORTED             = -9,   /**< Not supported. */
    SEC_UI_INVALID_INPUT             = -10,  /**< Invalid input given to an API function, for example, NULL pointer. */
    SEC_UI_TOUCH_DRIVER_ERROR        = -11,  /**< Not supported; may be removed in future releases. */
    SEC_UI_TOUCH_TIMEOUT             = -12,  /**< Not supported; may be removed in future releases. */
    SEC_UI_ABORTED                   = -13,  /**< Secure UI was aborted by the HLOS. */
    SEC_UI_TOUCH_ABORTED             = -14,  /**< Not supported; may be removed in future releases. */
    SEC_UI_SD_STATE_CHANGED          = -15,  /**< Secure display session has changed while the system was in
                                                  the HLOS; the application should abort after this if the change
                                                  was due to a hacking attempt. */
    SEC_UI_SD_NOT_ACTIVE             = -16,  /**< Not currently used. */
    SEC_UI_NO_MEMORY                 = -17,  /**< No buffer memory has been relinquished by the HLOS; possible
                                                  indicator of out-of-memory issues in the multimedia memory
                                                  allocator. */
    SEC_UI_NOT_ENOUGH_BUFFERS        = -18,  /**< HLOS has not relinquished enough buffers to store display
                                                  content; the minimum number of buffers is now 2 (double buffer);
                                                  this number is implementation-dependent. */
    SEC_UI_MORE_BUFFERS_AVAILABLE    = -19,  /**< More buffers than this QSEE application owns are tagged for
                                                  secure display usage; this might indicate a hacking attempt. */
    SEC_UI_TAG_ERROR                 = -20,  /**< Secure display memory is not properly tagged; this might
                                                  indicate a hacking attempt. */
    SEC_UI_OUT_OF_ORDER              = -22,  /**< Wrong buffer was passed by the HLOS during initialization; this
                                                  might indicate a hacking attempt. */
    SEC_UI_WRONG_BUFFER              = -23,  /**< Metadata associated with the passed buffer is inconsistent; this
                                                  might indicate a hacking attempt. */
    SEC_UI_TOO_MANY_BUFFERS          = -24,  /**< HLOS is relinquishing more buffers than the QSEE service can
                                                  handle; this might indicate that different incompatible software
                                                  versions of the two systems are active in the HLOS and QSEE. */
    SEC_UI_INVALID_BUFFER            = -25,  /**< HLOS is passing a buffer that has not been previously
                                                  relinquished; this might indicate a hacking attempt. */
    SEC_UI_CALL_ACTIVE               = -26,  /**< Phone is off the hook for either an incoming or outgoing call. */
    SEC_UI_SCREEN_OFF                = -27,  /**< Device screen is not available for Secure UI use; the screen
                                                  may be off or locked. */
	SEC_UI_TIMEOUT                   = -28,  /**< Waited too long for an action to complete. */
	SEC_UI_FONT_FILE_ERROR           = -29,  /**< Error occurred related to the dynamic font file. */
	SEC_UI_MINK_API_ERROR            = -30,  /**< Error occurred during a MINK API call. */
    SEC_UI_SERVICE_NOT_READY         = -31,  /**< Error occurred while communicating with the Android service
                                                  this might indicate the service did not start yet. */
    SEC_UI_FRAME_AUTHENTICATION_FAILED = -32,  /**< Error occurred while authemticating the secure ui frame */
    SEC_UI_ERR_SIZE                  = 0x7FFFFFFF, /* Internal - not in customer documentation */
    SEC_UI_ROTATION                  = 1,    /* Internal - not in customer documentation */
    SEC_UI_HEAP                      = 2,    /* Internal - not in customer documentation */
 } sec_ui_err_t;

/* SecureUI Lib status */
typedef enum {
  SEC_UI_STATE_NOT_ACTIVE = 0,    /* SecureUI Session not yet started*/
  SEC_UI_STATE_ACTIVE,                /* SecureUI Session  started*/
  SEC_UI_STATE_ABORTED,           /* SecureUI Session aborted*/
  SEC_UI_STATE_MAX        = 0x7FFFFFFF
} sec_ui_state_type;

 /** @} */ /* end_addtogroup qsee_core_layer */

 /* Touch status. (not used) */
  typedef enum {
	SEC_TOUCH_GOT_EVENT,	/* Normal touch event. */
	SEC_TOUCH_TIMEOUT,		/* Wait() timed out. */
	SEC_TOUCH_EXT_ABORT,	/* Aborted externally from the HLOS side. */
	SEC_TOUCH_STATUS_SIZE   = 0x7FFFFFFF
 } touch_status_t;

#endif //_SEC_UI_DEFS_H

