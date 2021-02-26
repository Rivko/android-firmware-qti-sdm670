#ifndef QSEE_TUI_H_
#define QSEE_TUI_H_

/** @file qsee_tui_dialogs.h
 * @brief
 * This file contains the interfaces to Secure UI GP dialog screens.
 */

/*===========================================================================
 Copyright (c) 2013-2016,2019 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/16   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/07/16   dr      Add dialog orientation support
05/22/16   sn      Modified the custom layout callback to receive also the event type of last touched object
01/28/16   gs      Added support for versioning
04/11/16   sn      Modified the custom layout callback to receive last touched object
01/04/16   dr      Added font manager configuration support
12/10/15   kl      (Tech Pubs) Comment updates for 80-NJ546-1 Rev. J.
07/30/15   sn      Added custom screen API.
12/03/15   rz      Added TUI_SECURE_INDICATOR_ERROR return value.
08/03/15   ls      Add one-time per device keypad randomization.
23/02/15   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
05/02/15   rz      Changed dialogParams for better support in secure indicator.
23/12/14   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
25/06/14   rz      Added secure indicator support.
07/07/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
31/10/13   sn      Initial Version.

===========================================================================*/

#include "SecureTouchLayout.h"
#include "SecureUILibDefs.h"
#include "layout_manager.h"

/** @addtogroup qsee_gp_screens
@{ */

/* SecureUI version information */
#define TUI_MAJOR_VERSION 3 /**< Major version. */
#define TUI_MINOR_VERSION 0	/**< Minor version. */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** Secure UI dialog return values, that is, status and error codes for the GP screens API. */
typedef enum {
	TUI_GENERAL_ERROR                  = -6,  /**< Secure UI General error. */
	TUI_SECURE_INDICATOR_ERROR         = -5,  /**< Error in setting the secure
                                                   indicator for the local application. */
	TUI_LAYOUT_ERROR                   = -4,  /**< Internal layout management error. */
	TUI_ILLEGAL_INPUT                  = -3,  /**< Illegal input was received. */
	TUI_SYS_EVENT_ABORTED              = -2,  /**< Secure UI session was aborted by some
                                                   external abort request, for example, an
                                                   incoming call or a power down. */
	TUI_TIMEOUT_ABORTED                = -1,  /**< Secure UI session was aborted due to a timeout. */
	TUI_SUCCESS                        = 0,   /**< Success. */
	TUI_OK_PRESSED                     = 1,   /**< OK button was pressed by the user. */
	TUI_CANCEL_PRESSED                 = 2,   /**< Cancel button was pressed by the user. */
	TUI_RIGHT_PRESSED                  = 3,   /**< Right button of the message screen was
                                                   pressed by the user. */
	TUI_LEFT_PRESSED                   = 4,   /**< Left button of the message screen was
                                                   pressed by the user. */
	TUI_MIDDLE_PRESSED                 = 5,   /**< Middle button of the message screen was
                                                   pressed by the end user. */
	TUI_RETVAL_MAX                     = 0x7FFFFFFF
} qsee_tui_dialog_ret_val_t;


/**
  Application callback that implements user customized screen logic.
  @xreflabel{sec:LayoutEventCallback}

  @param[in]   layout_mgr_status      Layout Manager event evaluation status.
  @param[in]   curr_touched_object    Name of the currently touched object in the 
                                      screen, and the event type: up/down/move.
  @param[out]  final_status           A status to be returned in qsee_tui_dialog() 
                                      in case of failure during the callback.

  @return
  Command for the next touch operation: completed/canceled/continue.

  @dependencies
  None.
  @newpage
*/
typedef sec_touch_cmd_t (*qsee_tui_manage_layout_event_callback_t)(layout_mgr_err_t layout_mgr_status, layout_mgr_touched_obj_t curr_touched_object, qsee_tui_dialog_ret_val_t* final_status);



/** TUI GP screen type. */
typedef enum {
	TUI_DIALOG_MESSAGE,   /**< Message screen. */
	TUI_DIALOG_PIN,       /**< PIN entry screen. */
	TUI_DIALOG_PASSWORD,  /**< Login and password entry screen. */
	TUI_DIALOG_CUSTOM,    /**< User-customized screen. */
	TUI_DIALOG_MAX = 0x7FFFFFFF
} qsee_tui_dialog_t;

/** Secure UI keypad mode. */
typedef enum {
	TUI_KEYPAD_NORMAL,      /**< Keypad mode is Normal. */
	TUI_KEYPAD_RANDOMIZED,  /**< Keypad mode is Randomized. */
	TUI_KEYPAD_RANDOMIZED_ONCE,  /**< Keypad mode is Randomized only one-time per device. @newpage */
	TUI_KEYPAD_MAX = 0x7FFFFFFF
} qsee_tui_keypad_mode_t;

/** Secure UI font heat map mode. */
typedef enum {
	TUI_FONT_HEATMAP_DISABLED, /**< Dynamic font cache loads characters on demand. */
	TUI_FONT_HEATMAP_ENABLED,  /**< Dynamic font cache is pre-populated with static font data
                                    (heat map) copied from the layout file for all dynamic fonts. */
	TUI_FONT_HEATMAP_MAX = 0x7FFFFFFF
} qsee_tui_font_heatmap_mode_t;

/** Secure UI dialog orientation relative to the device native screen orientation. 

    On the phone form factor, the native (0 degrees) is commonly a portrait
    orientation. Some dialog designs might require landscape orientation on
    phone devices as well. In such cases, the provided layout should have the
    dimensions matching the preferred orientation (e.g., 2560 width and
    1600 height) and a 90 degree (or 270 if preferred) rotation specified in
    the common dialog option (qsee_tui_dialog_orienation_t).
*/
typedef enum {
	TUI_ORIENTATION_0,   /**< Native orientation (0 degrees). */
	TUI_ORIENTATION_90,  /**< 90 degrees. */
	TUI_ORIENTATION_180, /**< 180 degrees. */
	TUI_ORIENTATION_270, /**< 270 degrees. */
	TUI_ORIENTATION_MAX = 0x7FFFFFFF
} qsee_tui_dialog_orienation_t;

/** Defines a general buffer parameter.

 @note1hang If the buffer is used to represent text, the text string must be
            formatted as UTF-8. For the string to be displayed properly on the
            screen, an appropriate font file must be present on the device.
            For instructions on how to create and reference a font file,
            refer to the <i>QSEE Secure UI Customization Tool User
            Guide</i> @xhyperref{80-P3696-1,(80-P3696-1)}.
 */
typedef struct{
	uint8_t* pu8Buffer;  /**< Start address of the buffer. */
	uint32_t u32Len;     /**< Length of the buffer in bytes. */
} qsee_tui_buffer_t;

/** Defines the parameters for user input in a dialog screen. */
typedef struct{
	uint32_t u32MinLen;              /**< Minimum length in characters (UTF-8
                                          encoding may take more than one byte
                                          to represent a character). */
	uint32_t u32MaxLen;              /**< Maximum length in characters. */
	qsee_tui_buffer_t inpValue;      /**< Buffer parameter to be filled with
                                          user input. */
	qsee_tui_buffer_t defaultValue;  /**< Buffer parameter that contains the
                                          default value to be shown in the
                                          dialog screen. */
	qsee_tui_buffer_t label;         /**< Buffer parameter that contains the
                                          label text to be shown above the
                                          input object if it is empty. */
	uint32_t* pu32InpValueLen;       /**< Output parameter; the actual length
                                          of the user input in characters. */
} qsee_tui_input_params_t;

/** Defines the parameters for the secure indicator. @xreflabel{hdr:secureIndicatorParams} */
typedef struct {
    uint32_t bDisplayGlobalSecureIndicator;                     /**< Boolean flag to display the global secure indicator. */
    qsee_tui_buffer_t secureIndicator;                          /**< Secure indicator image buffer encoded as PNG. */
} qsee_tui_secure_indicator_params_t;

/** Common parameters for all GP screens.

 @note1hang Some of the common parameters are not relevant to custom screens,
            such as the title, description, and logo. Any entered value
            for these parameters is ignored.
 */
typedef struct{
	int32_t n32TimeOut;                                         /**< Touch session timeout (in milliseconds),
                                                                     that is, the maximum time to wait for
                                                                     user input. 0 means return
                                                                     immediately, and -1 means no
                                                                     timeout (wait indefinitely). */
	qsee_tui_buffer_t title;                                    /**< Buffer parameter that contains the
                                                                     title text of the screen. */
	qsee_tui_buffer_t description;                              /**< Description label text. */
	qsee_tui_buffer_t logo;                                     /**< Logo image buffer encoded as PNG. */
	qsee_tui_secure_indicator_params_t secureIndicatorParams;   /**< Secure indicator parameters; contains
                                                                     the optional image buffer encoded as PNG. */
	qsee_tui_font_heatmap_mode_t fontHeatmapMode;               /**< Dynamic font heat map support mode. */
	qsee_tui_dialog_orienation_t orientation;					/**< Screen orientation. */
} qsee_tui_dialog_common_params_t;

/** Defines the parameters of a configurable button in a dialog screen.. */
typedef struct{
	qsee_tui_buffer_t btnLabel;  /**< Label to be shown on the button. */
	uint32_t bShowBtn;           /**< Boolean flag to show the button. */
	uint32_t bDisableBtn;        /**< Boolean flag to disable (gray out) the button. */
} qsee_tui_button_params_t;

/** Defines the parameters that are specific to the message screen. */
typedef struct{
	qsee_tui_buffer_t msg;                     /**< Buffer parameter that contains
                                                    the message to be shown on the screen. */
	qsee_tui_button_params_t rightBtnParams;   /**< Parameters of the right button
                                                    of the message screen. */
	qsee_tui_button_params_t leftBtnParams;    /**< Parameters of the left button
                                                    of the message screen. */
	qsee_tui_button_params_t middleBtnParams;  /**< Parameters of the middle button
                                                    of the message screen. */
} qsee_tui_dialog_msg_params_t;

/** Defines the parameters that are specific to the pin entry screen. */
typedef struct {
	qsee_tui_input_params_t pin;        /**< Buffer details for storing the user PIN. */
	uint32_t bHideInputBox;             /**< Boolean flag to hide the user input on the screen. */
	LayoutInputDisplayMode_t inpMode;   /**< User input (for the PIN) display mode. */
	qsee_tui_keypad_mode_t keyPadMode;  /**< Keypad mode; randomized/randomized
	                                         once/normal. */
} qsee_tui_dialog_pin_params_t;

/** Defines the parameters that are specific to the login and password entry screen.

    @note1hang At least one of the flags in bUserNameInpShow and bPasswordInpShow
    must be set to TRUE; otherwise, it is considered an illegal input.
*/
typedef struct {
	qsee_tui_input_params_t username;  /**< User input parameters for the username. */
	uint32_t bUserNameInpShow;         /**< Boolean flag to show the username
	                                        input object on the screen. */
	uint32_t bPasswordInpShow;         /**< Boolean flag to show the password
	                                        input object on the screen. */
	qsee_tui_input_params_t password;  /**< User input parameters for password. */
	LayoutInputDisplayMode_t inpMode;  /**< Password display mode. */
} qsee_tui_dialog_pswd_params_t;

/** Defines the parameters that are specific to user-customized screen. */
typedef struct {
  qsee_tui_manage_layout_event_callback_t manage_layout_event_custom;
  /**< Callback to be called upon touch event in custom screen, after Layout
        Manager evaluation . */
} qsee_tui_dialog_custom_params_t;

/** Defines the general parameter set for all screens.

   When passing a layout for a GP screen (PIN, login, message),
   ensure that the parameters match the specified layout.
   For example, if the parameters refer to three buttons in
   the message screen, the layout must contain three buttons.
   This restriction also applies to user name and password
   input fields on login screens.


   @note1hang For backward compatibility, you can pass NULL for the
               layout_page. In this case, the default layout for the
               required dialog type is used. The default layout of
               each dialog type is defined by the Customization Tool
               (refer to the <i>QSEE Secure UI Customization
               Tool User Guide</i> @xhyperref{80-P3696-1,(80-P3696-1)})
               as follows:

   @note1bullet If only one layout of this type exists, it is the default. \n
   @note1bullet Otherwise, the first layout of each type that was defined in the tool
                is the default.

   If you implicitly select a layout by passing NULL,
   ensure that a corresponding layout exists in the application.
 */
typedef struct qsee_tui_dialog_params_s {
	qsee_tui_dialog_common_params_t dialogCommonParams;  /**< Common parameters; for example: timeout,
                                                              title, description, logo. */
	qsee_tui_dialog_t dialogType;                        /**< Dialog screen type. */
    /** Union of the parameters of the different screen types. */
	union {
		qsee_tui_dialog_msg_params_t  msgDialogParams;   /**< Message dialog parameters. */
		qsee_tui_dialog_pin_params_t  pinDialogParams;   /**< PIN dialog parameters. */
		qsee_tui_dialog_pswd_params_t pswdDialogParams;  /**< Password dialog parameters. */
	    qsee_tui_dialog_custom_params_t customDialogParams;  /**< Custom dialog parameters.  */
	};
    LayoutPage_t* layout_page;   /**< Dialog screen layout, may be GP screen
                                      or custom screen. @newpagetable*/

} /** @cond */qsee_tui_dialog_params_t/** @endcond */;

/** @cond */
typedef struct {
	uint8_t* pu8SecureIndicator;
	uint32_t u32IndicatorSize;
	uint32_t bIsValid;
} qsee_tui_secure_indicator_t;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*****************************************************************************************************************/

/**
   External function used to initialize the Secure UI GP screen service.

   @note1hang This function must be called before any other GP screen functions
              are called. For a description of the GP screen functions flow,
              see Section \xref{sec:GPScreenCallFlow}.

   @return
   #TUI_SUCCESS      \n
   #TUI_LAYOUT_ERROR

   @dependencies
   None.
*/
qsee_tui_dialog_ret_val_t qsee_tui_init(void);

/*****************************************************************************************************************/

/**
\xreflabel{sec:qseeTuiDialog}

   External function for using the three GP screens: LOGIN, GET_PIN, and
   SHOW_MESSAGE. For a description of the GP screens, see Section @xref{sec:GpScreens}.

   This function does not start the screen operation. It only sets up the
   screen so that when the touch session starts, the required screen appears.
   The operation completes during the touch session.

   @note1hang This function starts a secure display session (if there is no
   active session of the previous screen). So, until the touch session starts,
   a black screen is displayed.

   @caution1hang The status parameter, as well as other output parameters that
   are contained in the dialogParams parameter, for example, the buffers that are to
   be filled with the user input, <i>must be read only after the touch session
   ends</i>. A notification about the touch session ending is received from
   the HLOS after the return of UseSecureTouch().

   @param[in,out]  dialogParams  General parameters of the dialog, including
                                 the dialog type and the relevant input/output
                                 parameters. \n
                                 @note1 See Section @xref{sec:GPScreenParamLim}
                                 for a description of parameter limitations for
                                 this parameter.
   @param[out]     status        Final status of the dialog execution, which is
                                 to be read only after the touch session is
                                 finished.

   @return
   #TUI_SUCCESS  \n
   #TUI_LAYOUT_ERROR  \n
   #TUI_ILLEGAL_INPUT   \n
   #TUI_SECURE_INDICATOR_ERROR

   See #sec_ui_err_t for descriptions of return codes.

   @note1hang The return value represents the status of setting up the
   screen, as opposed to the status parameter, which reflects the final status
   of the operation of the GP screen.

   @dependencies
   Must be called after qsee_tui_init.
*/
qsee_tui_dialog_ret_val_t qsee_tui_dialog(qsee_tui_dialog_params_t* dialogParams, qsee_tui_dialog_ret_val_t* status);

/*****************************************************************************************************************/

/**
   External function for releasing the secure indicator buffer and clearing
   the valid and size fields. This function must be called when the application
   no longer requires the secure indicator (usually on shutdown).

   @return
   #TUI_SUCCESS

   @dependencies
   None.

   @sideeffects
   Secure indicator is not available any longer (unless it is set again).
*/
qsee_tui_dialog_ret_val_t qsee_tui_release_secure_indicator(void);

/*****************************************************************************************************************/

/**
   External function for closing the Secure UI GP screen service.
   @xreflabel{sec:QSEETuiTearDown}

   @note1hang If several GP screens must be called one after
   another, call this function only after the final one.

   @return
   #TUI_SUCCESS \n
   #TUI_LAYOUT_ERROR

   @dependencies
   None.

   @sideeffects
   Stops the secure display session.
*/
qsee_tui_dialog_ret_val_t qsee_tui_tear_down(void);

/** @} */ /* end_addtogroup qsee_gp_screens */

#endif /* QSEE_TUI_H_ */
