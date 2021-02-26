/*===========================================================================
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
10/08/16    dr     Add custom dialog sample code
07/17/16    dr     Move GP dialogs sample code into a separate source file
===========================================================================*/

#include "sselog.h"
#include <comdef.h>
#include "qsee_fs.h"
#include "qsee_sfs.h"
#include <string.h>
#include "qsee_log.h"
#include "qsee_services.h"
#include "qsee_core.h"
#include "SecureUI.h" //needed to get the resolution
#include <stdio.h>
#include <stringl.h>
#include "qsee_heap.h"
#include "AccessControlHyp.h"
#include "qsee_tui_dialogs.h"
#include "qsee_tui_layout.h"

/* GP screens demo commands */
#define SEC_UI_SAMPLE_CMD10_GET_PIN           10
#define SEC_UI_SAMPLE_CMD11_LOGIN             11
#define SEC_UI_SAMPLE_CMD12_MSG_PIN           12
#define SEC_UI_SAMPLE_CMD13_MSG_LOGIN         13
#define SEC_UI_SAMPLE_CMD14_MSG_CLEAN_UP      14
#define SEC_UI_SAMPLE_CMD15_CUSTOM_DIALOG     15
#define SEC_UI_SAMPLE_CMD16_MSG_CUSTOM        16

#define MAX_FILENAME_LEN                   (256)
#define NO_TIMEOUT                          (-1)
#define DEFAULT_TIMEOUT               NO_TIMEOUT

#define MAX_INPUT_LEN_BYTES 500
#define MAX_INPUT_LEN       200
#define MIN_INPUT_LEN         0
#define MAX_MSG_LEN         300
#define MAX_UPDATE_EVENTS    40

#define PIN_TITLE "ENTER YOUR PIN"
#define LOGIN_TITLE "SECURE LOGIN"
#define MSG_TITLE "SECURE MESSAGE"
#define DESC_TITLE "Test Secure UI®"
#define MSG_RIGHT_BTN_LABEL "ok"
#define MSG_LEFT_BTN_LABEL "cancel"
#define MSG_MIDDLE_BTN_LABEL ""
#define PASSWORD_LABEL "PASSWORD"
#define USERNAME_LABEL "USERNAME"
#define DEFAULT_USER "sample user"
#define PIN_LABEL ""

//#define SFS 1 // set this flag to read the secure indicator image from SFS
//#define PROVISION 1 // set this flag to provision the secure indicator image into SFS

struct send_cmd{
  uint32_t cmd_id;
};

struct send_cmd_rsp{
  int32_t status;
};

static char sfs_sec_ind_path[] = "/data/misc/secure_ui/secure_images/secure_logo.png";
static char fs_sec_ind_path[] = "/data/vendor/tui/sec_ind.png";
static char fs_logo_path[] = "/data/vendor/tui/logo.png";
static uint32_t sec_ind_size = 0;
static uint32_t logo_size = 0;

static qsee_tui_dialog_ret_val_t status = TUI_SUCCESS;
static uint8_t* sec_ind_buffer           = NULL;
static uint8_t* logo_buffer           = NULL;
static uint8_t received_pin[MAX_INPUT_LEN_BYTES];
static uint32_t received_pin_len      = 0;
static uint8_t received_username[MAX_INPUT_LEN_BYTES];
static uint32_t received_username_len = 0;
static uint8_t received_password[MAX_INPUT_LEN_BYTES];
static uint32_t received_password_len = 0;

static void provision(const char * fs_path, const char* sfs_path){
	int l_fd, sfs_fd;
	uint32_t size_read, i = 0;
	char temp_copy_buf[1024];

	qsee_log(QSEE_LOG_MSG_DEBUG, "trying to open the image = %s", fs_path);
	l_fd = open(fs_path, O_RDWR);
	if(l_fd < 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed to open the image = %s", fs_path);
		return;
	}

	qsee_sfs_rm(sfs_path);
	sfs_fd = qsee_sfs_open( sfs_path, O_RDWR | O_CREAT | O_TRUNC );
	if(sfs_fd <= 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "sfs open failed for %s, returned = %d", sfs_path, sfs_fd);
		close(l_fd);
		return;
	}

	/*copy the image from fs to sfs*/
	do {
		size_read = read(l_fd, temp_copy_buf, sizeof(temp_copy_buf));
		qsee_sfs_write( sfs_fd, (const char*)temp_copy_buf, size_read );
		i+=size_read;
		qsee_log(QSEE_LOG_MSG_DEBUG, "copying... %d", i);
	} while (size_read > 0);
	qsee_log(QSEE_LOG_MSG_DEBUG, "done copying");
	close(l_fd);
	qsee_sfs_close(sfs_fd);

	qsee_log(QSEE_LOG_MSG_DEBUG, "Done provisioning image = %s", fs_path);
}

void tui_dialogs_app_init(void) {
	int l_fd, bytes, res;
	fs_stat logo_stat;

	/* logo */
	res = lstat(fs_logo_path,&logo_stat);
	if(res < 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed lstat for the logo");
		return;
	}

	logo_size = logo_stat.st_size;

	logo_buffer = (uint8_t*)qsee_malloc(logo_size);
	if(NULL == logo_buffer){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed qsee_malloc the logo buffer");
		return;
	}

	l_fd = open(fs_logo_path, O_RDONLY);
	if(l_fd < 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed reading logo image path");
		return;
	}

	bytes = read(l_fd,logo_buffer,logo_size);
	qsee_log(QSEE_LOG_MSG_DEBUG, "read logo image, bytes =%d", bytes);
	close(l_fd);


#ifdef PROVISION

	provision(fs_sec_ind_path, sfs_sec_ind_path);

#endif

#ifdef SFS

	l_fd = qsee_sfs_open(sfs_sec_ind_path, O_RDWR);
	if(l_fd < 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed reading image path");
		return;
	}

	res = qsee_sfs_getSize(l_fd, &sec_ind_size);
	if(0 != res){
		qsee_log(QSEE_LOG_MSG_DEBUG, "qsee_sfs_getSize failed");
		return;
	}

	sec_ind_buffer = (uint8_t*)qsee_malloc(sec_ind_size);
	if(NULL == sec_ind_buffer){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed qsee_malloc the secure indicator buffer");
		return;
	}

	bytes = qsee_sfs_read(l_fd,(char*)sec_ind_buffer,sec_ind_size);
	qsee_log(QSEE_LOG_MSG_DEBUG, "read image, bytes =%d", bytes);
	qsee_sfs_close(l_fd);

#else

	/* secure indicator */
	fs_stat sec_ind_stat;
	res = lstat(fs_sec_ind_path,&sec_ind_stat);
	if(res < 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed lstat for the indicator. Trying to display the secure indicator instead");
		return;
	}

	sec_ind_size = sec_ind_stat.st_size;

	sec_ind_buffer = (uint8_t*)qsee_malloc(sec_ind_size);
	if(NULL == sec_ind_buffer){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed qsee_malloc the secure indicator buffer");
		return;
	}

	l_fd = open(fs_sec_ind_path, O_RDONLY);
	if(l_fd < 0){
		qsee_log(QSEE_LOG_MSG_DEBUG, "failed reading the secure indicator image path");
		return;
	}

	bytes = read(l_fd,sec_ind_buffer,sec_ind_size);
	qsee_log(QSEE_LOG_MSG_DEBUG, "read secure indicator image, bytes =%d", bytes);
	close(l_fd);

#endif

	qsee_tui_init();
}

//This function demonstrates how we can achieve dialog selection at runtime based on
// screen resolution or other parameters. In this example we assume that the provided
// layout has three levels of resolution support - large, regular and small.
// The special dialog name suffix is used for "large" and "small" dialogs, and no suffix for regular.
// e.g: for message_double GP dialog with high resolution - "message_double_large" is expected.
// Note, that this model can be extended for any number of resolutions or suffixes as memory space permits
static LayoutPage_t* get_layout_name_for_current_resolution(const char* dialog_base_name)
{
	const char* dialog_suffixes[] = {"_large", "", "_small"};
	const uint32_t MAX_DIALOG_NAME_LENGTH = 64;
	uint32_t disp_width = 0, disp_height = 0;
	uint32_t layout_width = 0, layout_height = 0;
	char str_buffer[MAX_DIALOG_NAME_LENGTH] = {0};
	sec_ui_err_t secui_err = SEC_UI_SUCCESS;
	layout_mgr_err_t layout_err = LAYOUT_MGR_SUCCESS;
	LayoutPage_t* layoutPage = NULL;

	secui_err = sec_ui_get_disp_properties(&disp_height, &disp_width);

	if (secui_err < 0) {
		 qsee_log(QSEE_LOG_MSG_ERROR, "cannot get current screen resolution");
		 return NULL;
	}

	if (NULL == dialog_base_name) {
		 qsee_log(QSEE_LOG_MSG_ERROR, "invalid dialog base name received");
		 return NULL;
	}

	//we assume that the iteration is from large to small layout sizes
	for (uint32_t i=0; i < (sizeof (dialog_suffixes) / sizeof (*dialog_suffixes)); i++) {
		size_t str_res_len1 = strlcpy(str_buffer, dialog_base_name, sizeof(str_buffer));
		size_t str_res_len2 = strlcat(str_buffer, dialog_suffixes[i], sizeof(str_buffer));
		if (str_res_len1 >= sizeof(str_buffer) || str_res_len2 >= sizeof(str_buffer)) {
			 qsee_log(QSEE_LOG_MSG_DEBUG,
					 "String truncation error while processing dialog name: %s", str_buffer);
		}
		layoutPage = get_layout_by_name(str_buffer);
		if (NULL == layoutPage) {
			continue;
		}

		layout_err = layout_mgr_load_layout(layoutPage);
		if (layout_err < 0) {
			continue;
		}

		layout_err = layout_mgr_get_layout_size(&layout_height, &layout_width);
		if (layout_err < 0) {
			continue;
		}

		if ( (disp_height >= layout_height && disp_width>=layout_width) ) {
			 qsee_log(QSEE_LOG_MSG_DEBUG,
					 "Suitable layout found (%s) for current display size", str_buffer);
			 qsee_log(QSEE_LOG_MSG_DEBUG,
					 "base_name: %s, disp_width: %u, disp_height: %u, layout_width: %u, layout_height: %u",
					 dialog_base_name, disp_width, disp_height, layout_width, layout_height);
			 return layoutPage;
		}

	}

	return NULL;
}

static int32_t start_pin_dialog(){
	  qsee_tui_dialog_params_t pin_params = {0};

	  //load dialog based on display resolution. get_layout_by_name() can be
	  //used directly if the layout name is known at compile time
	  LayoutPage_t* pinPage = get_layout_name_for_current_resolution("pin");
	  if (NULL == pinPage) {
		  qsee_log(QSEE_LOG_MSG_ERROR, "error get_layout_name_for_current_resolution('pin') for pin dialog layout");
		  return TUI_ILLEGAL_INPUT;
	  }

	  pin_params.layout_page = pinPage;

	  pin_params.dialogType = TUI_DIALOG_PIN;

	  pin_params.dialogCommonParams.title.pu8Buffer = (uint8_t*)PIN_TITLE;
	  pin_params.dialogCommonParams.title.u32Len = strlen(PIN_TITLE);

	  pin_params.dialogCommonParams.description.pu8Buffer = (uint8_t*)DESC_TITLE;
	  pin_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

	  if (sec_ind_buffer != NULL) {
	      // display local indicator
	      pin_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = FALSE;
	      pin_params.dialogCommonParams.secureIndicatorParams.secureIndicator.pu8Buffer = sec_ind_buffer;
	      pin_params.dialogCommonParams.secureIndicatorParams.secureIndicator.u32Len = sec_ind_size;
	  } else {
	      // display global secure indicator
	      pin_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = TRUE;
	  }

	  pin_params.dialogCommonParams.logo.pu8Buffer = logo_buffer;
	  pin_params.dialogCommonParams.logo.u32Len = logo_size;

	  pin_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;

	  pin_params.pinDialogParams.inpMode = DISP_MODE_VIS_THEN_HID;

	  pin_params.pinDialogParams.pin.u32MaxLen = MAX_INPUT_LEN;
	  pin_params.pinDialogParams.pin.u32MinLen = MIN_INPUT_LEN;
	  pin_params.pinDialogParams.pin.inpValue.pu8Buffer = received_pin;
	  pin_params.pinDialogParams.pin.inpValue.u32Len = MAX_INPUT_LEN_BYTES;
	  pin_params.pinDialogParams.pin.pu32InpValueLen = &received_pin_len;
	  pin_params.pinDialogParams.pin.defaultValue.pu8Buffer = received_pin;
	  pin_params.pinDialogParams.pin.defaultValue.u32Len = received_pin_len;

	  pin_params.pinDialogParams.pin.label.pu8Buffer = (uint8_t*)PIN_LABEL;
	  pin_params.pinDialogParams.pin.label.u32Len = strlen(PIN_LABEL);

	  pin_params.pinDialogParams.keyPadMode = TUI_KEYPAD_RANDOMIZED;
	  pin_params.pinDialogParams.bHideInputBox = 0;
	  return qsee_tui_dialog(&pin_params,&status);
}

static int32_t start_login_dialog(){
	  qsee_tui_dialog_params_t login_params = {0};

	  //load dialog based on display resolution. get_layout_by_name() can be
	  //used directly if the layout name is known at compile time
	  LayoutPage_t* loginPage = get_layout_name_for_current_resolution("login_double");
	  if (NULL == loginPage) {
		  qsee_log(QSEE_LOG_MSG_ERROR, "error get_layout_name_for_current_resolution('login_double') for login dialog layout");
		  return TUI_ILLEGAL_INPUT;
	  }

	  login_params.layout_page = loginPage;

	  login_params.dialogType = TUI_DIALOG_PASSWORD;

	  login_params.dialogCommonParams.title.pu8Buffer = (uint8_t*)LOGIN_TITLE;
	  login_params.dialogCommonParams.title.u32Len = strlen(LOGIN_TITLE);

	  login_params.dialogCommonParams.description.pu8Buffer = (uint8_t*)DESC_TITLE;
	  login_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

	  if (sec_ind_buffer != NULL) {
	      // display local indicator
	      login_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = FALSE;
	      login_params.dialogCommonParams.secureIndicatorParams.secureIndicator.pu8Buffer = sec_ind_buffer;
	      login_params.dialogCommonParams.secureIndicatorParams.secureIndicator.u32Len = sec_ind_size;
	  } else {
	      // display global secure indicator
	      login_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = TRUE;
	  }

	  login_params.dialogCommonParams.logo.pu8Buffer = logo_buffer;
	  login_params.dialogCommonParams.logo.u32Len = logo_size;

	  login_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;

	  login_params.pswdDialogParams.inpMode = DISP_MODE_VIS_THEN_HID;
	  login_params.pswdDialogParams.bUserNameInpShow = 1;
	  login_params.pswdDialogParams.bPasswordInpShow = 1;

	  login_params.pswdDialogParams.username.u32MaxLen = MAX_INPUT_LEN;
	  login_params.pswdDialogParams.username.u32MinLen = MIN_INPUT_LEN;
	  login_params.pswdDialogParams.username.inpValue.pu8Buffer = received_username;
	  login_params.pswdDialogParams.username.inpValue.u32Len = MAX_INPUT_LEN_BYTES;
	  login_params.pswdDialogParams.username.pu32InpValueLen = &received_username_len;
	  login_params.pswdDialogParams.username.defaultValue.pu8Buffer = (uint8_t*)DEFAULT_USER;
	  login_params.pswdDialogParams.username.defaultValue.u32Len = strlen(DEFAULT_USER);
	  login_params.pswdDialogParams.username.label.pu8Buffer = (uint8_t*)USERNAME_LABEL;
	  login_params.pswdDialogParams.username.label.u32Len = strlen(USERNAME_LABEL);

	  login_params.pswdDialogParams.password.u32MaxLen = MAX_INPUT_LEN;
	  login_params.pswdDialogParams.password.u32MinLen = MIN_INPUT_LEN;
	  login_params.pswdDialogParams.password.inpValue.pu8Buffer = received_password;
	  login_params.pswdDialogParams.password.inpValue.u32Len = MAX_INPUT_LEN_BYTES;
	  login_params.pswdDialogParams.password.pu32InpValueLen = &received_password_len;
	  login_params.pswdDialogParams.password.defaultValue.pu8Buffer = received_password;
	  login_params.pswdDialogParams.password.defaultValue.u32Len = received_password_len;
	  login_params.pswdDialogParams.password.label.pu8Buffer = (uint8_t*)PASSWORD_LABEL;
	  login_params.pswdDialogParams.password.label.u32Len = strlen(PASSWORD_LABEL);

	  return qsee_tui_dialog(&login_params,&status);
}

static int32_t start_msg_dialog(uint8* msg, uint32_t msg_len){
	  qsee_tui_dialog_params_t msg_params = {0};

	  if(NULL == msg){
		  return TUI_ILLEGAL_INPUT;
	  }

	  //load dialog based on display resolution. get_layout_by_name() can be
	  //used directly if the layout name is known at compile time
	  LayoutPage_t* loginPage = get_layout_name_for_current_resolution("message_double");
	  if (NULL == loginPage) {
		  qsee_log(QSEE_LOG_MSG_ERROR, "error get_layout_name_for_current_resolution('message_double') for login dialog layout");
		  return TUI_ILLEGAL_INPUT;
	  }

	  msg_params.layout_page = loginPage;

	  msg_params.dialogType = TUI_DIALOG_MESSAGE;

	  msg_params.dialogCommonParams.title.pu8Buffer = (uint8_t*)MSG_TITLE;
	  msg_params.dialogCommonParams.title.u32Len = strlen(MSG_TITLE);

	  msg_params.dialogCommonParams.description.pu8Buffer = (uint8_t*)DESC_TITLE;
	  msg_params.dialogCommonParams.description.u32Len = strlen(DESC_TITLE);

	  if (sec_ind_buffer != NULL) {
	      // display local indicator
	      msg_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = FALSE;
	      msg_params.dialogCommonParams.secureIndicatorParams.secureIndicator.pu8Buffer = sec_ind_buffer;
	      msg_params.dialogCommonParams.secureIndicatorParams.secureIndicator.u32Len = sec_ind_size;
	  } else {
	      // display global secure indicator
	      msg_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = TRUE;
	  }

	  msg_params.dialogCommonParams.logo.pu8Buffer = logo_buffer;
	  msg_params.dialogCommonParams.logo.u32Len = logo_size;

	  msg_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;

	  msg_params.msgDialogParams.rightBtnParams.btnLabel.pu8Buffer = (uint8_t*)MSG_RIGHT_BTN_LABEL;
	  msg_params.msgDialogParams.rightBtnParams.btnLabel.u32Len = strlen(MSG_RIGHT_BTN_LABEL);
	  msg_params.msgDialogParams.rightBtnParams.bShowBtn = 1;
	  msg_params.msgDialogParams.rightBtnParams.bDisableBtn = 0;

	  msg_params.msgDialogParams.leftBtnParams.btnLabel.pu8Buffer = (uint8_t*)MSG_LEFT_BTN_LABEL;
	  msg_params.msgDialogParams.leftBtnParams.btnLabel.u32Len = strlen(MSG_LEFT_BTN_LABEL);
	  msg_params.msgDialogParams.leftBtnParams.bShowBtn = 1;
	  msg_params.msgDialogParams.leftBtnParams.bDisableBtn = 0;

	  msg_params.msgDialogParams.middleBtnParams.bShowBtn = 0;
	  msg_params.msgDialogParams.middleBtnParams.btnLabel.pu8Buffer = (uint8_t*)MSG_MIDDLE_BTN_LABEL;
	  msg_params.msgDialogParams.middleBtnParams.btnLabel.u32Len = strlen(MSG_MIDDLE_BTN_LABEL);
	  msg_params.msgDialogParams.middleBtnParams.bDisableBtn = 0;

	  msg_params.msgDialogParams.msg.pu8Buffer = msg;
	  msg_params.msgDialogParams.msg.u32Len = msg_len;

	  return qsee_tui_dialog(&msg_params,&status);
}

/***** The following definition are used in a custom secure UI dialog sample for entering credit card info *******/

const Color_t COLOR_RED = {
		.format = COLOR_FORMAT_RGBA,
		.rgba_color = {
			.red   = 0xFF,
			.green = 0x00,
			.blue  = 0x00,
			.alpha = 0xFF,
		}
};

const Color_t COLOR_BLACK = {
		.format = COLOR_FORMAT_RGBA,
		.rgba_color = {
			.red   = 0x00,
			.green = 0x00,
			.blue  = 0x00,
			.alpha = 0xFF,
		}
};

const uint32_t CUSTOM_INPUT_BUFFER_SIZE = 10;

//names of non-input controls use explicitly via layout manager API
const char* BUTTON_PAY = "button_pay";
const char* LABEL_CARD_STATUS = "label_card_status";

//the order of items assumes order of focus movement
typedef enum {
	CARD_INPUT_NUM1 = 0,
	CARD_INPUT_NUM2,
	CARD_INPUT_NUM3,
	CARD_INPUT_NUM4,
	CARD_INPUT_MM,
	CARD_INPUT_YY,
	CARD_INPUT_CVV,
	CARD_INPUT_LAST
} credit_card_input_id_t;

typedef struct credit_card_input {
	credit_card_input_id_t id;
	char* name;
	uint32_t expected_num_digits;
	credit_card_input_id_t next_input_id;
	uint8_t buffer[CUSTOM_INPUT_BUFFER_SIZE];
} credit_card_input_t;

static credit_card_input_t credit_card_data[] = {
		{CARD_INPUT_NUM1, "input_num1", 4, CARD_INPUT_NUM2 },
		{CARD_INPUT_NUM2, "input_num2", 4, CARD_INPUT_NUM3 },
		{CARD_INPUT_NUM3, "input_num3", 4, CARD_INPUT_NUM4 },
		{CARD_INPUT_NUM4, "input_num4", 4, CARD_INPUT_MM },
		{CARD_INPUT_MM,   "input_mm",   2, CARD_INPUT_YY },
		{CARD_INPUT_YY,   "input_yy",   2, CARD_INPUT_CVV },
		{CARD_INPUT_CVV,  "input_cvv",  3, CARD_INPUT_LAST /* no next input */},
};
const size_t credit_card_data_size = sizeof(credit_card_data)/sizeof(credit_card_input_t);


/************ custom secure UI dialog for entering credit card info ****************/

static const credit_card_input_t* credit_card_input_get_by_id(credit_card_input_id_t id) {
	if (id >= CARD_INPUT_LAST) {
		return NULL;
	}

	for (uint32_t i = 0; i < credit_card_data_size; i ++) {
		if (credit_card_data[i].id == id) {
			return &credit_card_data[i];
		}
	}
	//not found
	return NULL;
}

static const credit_card_input_t* credit_card_input_get_by_name(const char* name) {
	if (NULL == name) {
		return NULL;
	}

	for (uint32_t i = 0; i < credit_card_data_size; i ++) {
		if (0 == timesafe_strncmp(credit_card_data[i].name, name, (strlen(credit_card_data[i].name)+1))) {
			return &credit_card_data[i];
		}
	}
	//not found
	return NULL;
}

static inline uint32_t is_valid_month(char month_msd, char month_lsd) {
	return ( (month_msd == '0' && month_lsd >= '0' && month_lsd <= '9') ||
			 (month_msd == '1' && month_lsd >= '0' && month_lsd <= '2') );
}

static void credit_card_clear_input_buffers() {
	for (uint32_t i = 0; i < credit_card_data_size; i ++) {
		secure_memset(credit_card_data[i].buffer, 0, sizeof(credit_card_data[i].buffer));
	}
}

static uint32_t credit_card_is_card_number_checksum_valid(const uint8_t* card_number, size_t size ) {
	if (NULL == card_number) {
		return FALSE;
	}

	//For demo code we just check the card number is not all zeros (0000-0000-0000-0000)
	//Should be replace by implementation of a real number card validation method
	for (uint32_t i = 0; i < size; i++)
	if ('0' != card_number[i] ) {
		return TRUE;
	}

	return FALSE;
}

//Fill the provided buffer with the full (16 digits) credit card number
static uint32_t credit_card_fill_card_number_from_input(uint8_t* card_number_buffer, size_t buffer_size) {
	const uint8_t* text = NULL;
	uint32_t textByteSize = 0;
	uint32_t textCharSize = 0;
	uint32_t current_buffer_index = 0;

	if (NULL == card_number_buffer) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: card_number_buffer is NULL",__func__);
		return FALSE;
	}

	for (uint32_t id = CARD_INPUT_NUM1; id<= CARD_INPUT_NUM4; id++) {
		const credit_card_input_t* input = credit_card_input_get_by_id(id);
		if(input == NULL)
 	 	{
			qsee_log(QSEE_LOG_MSG_ERROR, "credit_card_input_get_by_id() returned null value");
			return FALSE;
 	 	}
		layout_mgr_err_t layout_err_val = layout_mgr_get_text(input->name, &text, &textByteSize, &textCharSize);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_get_text API error - %d", __func__, layout_err_val);
			return FALSE;
		}
		for (uint32_t i = 0; i < input->expected_num_digits && i < textByteSize; i++) {
			if (current_buffer_index >= buffer_size) {
				qsee_log(QSEE_LOG_MSG_ERROR, "Credit card number buffer is too small");
				return FALSE;
			}
			card_number_buffer[current_buffer_index] = text[i];
			card_number_buffer++;
		}
	}
	return TRUE;
}

//Validates if the user can complete the dialog, highlight inputs with errors if any
//Returns TUI_SUCCESS if dialog passes the validation
static qsee_tui_dialog_ret_val_t credit_card_dialog_validate() {
	const uint8_t* text = NULL;
	uint32_t textByteSize = 0;
	uint32_t textCharSize = 0;
	uint32_t hasEmptyInput = TRUE;
	qsee_tui_dialog_ret_val_t retVal = TUI_SUCCESS;
	for (uint32_t i = 0; i < credit_card_data_size; i ++) {
		const credit_card_input_t* input = &credit_card_data[i];
		layout_mgr_err_t layout_err_val = layout_mgr_get_text(input->name, &text, &textByteSize, &textCharSize);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_get_text API error - %d", __func__, layout_err_val);
			retVal = TUI_LAYOUT_ERROR;
		}
		if (0 == textCharSize) {
			hasEmptyInput = TRUE;
			retVal = TUI_ILLEGAL_INPUT;
		}
		else if (textCharSize != input->expected_num_digits) {
			layout_err_val = layout_mgr_set_text_color(input->name, COLOR_RED);
			if (layout_err_val != LAYOUT_MGR_SUCCESS) {
				qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_text_color API error - %d", __func__, layout_err_val);
				retVal = TUI_LAYOUT_ERROR;
			} else {
				retVal = TUI_ILLEGAL_INPUT;
			}
		}
		else if (input->id == CARD_INPUT_MM && !is_valid_month(text[0],text[1])) {
			layout_err_val = layout_mgr_set_text_color(input->name, COLOR_RED);
			if (layout_err_val != LAYOUT_MGR_SUCCESS) {
				qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_text_color API error - %d", __func__, layout_err_val);
				retVal = TUI_LAYOUT_ERROR;
			} else {
				retVal = TUI_ILLEGAL_INPUT;
			}
		}
	}

	//if all the input fields are valid we need to check for card number checksum
	if (retVal == TUI_SUCCESS) {
		uint8_t card_number[16] = {0};
		credit_card_fill_card_number_from_input(card_number, sizeof(card_number));
		if (! credit_card_is_card_number_checksum_valid(card_number, sizeof(card_number))) {
			//number is invalid - let's show the warning string
			layout_mgr_err_t layout_err_val = layout_mgr_set_show_flag(LABEL_CARD_STATUS, TRUE);
			if (layout_err_val != LAYOUT_MGR_SUCCESS) {
				qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_show_flag API error - %d", __func__, layout_err_val);
				retVal = TUI_LAYOUT_ERROR;
			} else {
				retVal = TUI_ILLEGAL_INPUT;
			}
		}
	}

	return retVal;
}

static uint32_t credit_card_has_empty_input()
{
	const uint8_t* text = NULL;
	uint32_t textByteSize = 0;
	uint32_t textCharSize = 0;
	uint32_t hasEmptyInput = FALSE;
	qsee_tui_dialog_ret_val_t retVal = TUI_SUCCESS;
	for (uint32_t i = 0; i < credit_card_data_size; i ++) {
		const credit_card_input_t* input = &credit_card_data[i];
		layout_mgr_err_t layout_err_val = layout_mgr_get_text(input->name, &text, &textByteSize, &textCharSize);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_get_text API error - %d", __func__, layout_err_val);
		}
		if (0 == textCharSize) {
			hasEmptyInput = TRUE;
		}
	}
	return hasEmptyInput;
}

static uint32_t credit_card_move_focus_to_next_control(const char* control_in_focus) {
	if (NULL == control_in_focus) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s got NULL input", __func__);
		return FALSE;
	}

	const credit_card_input_t* input_in_focus = credit_card_input_get_by_name(control_in_focus);
	if (NULL == input_in_focus) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: can't find input control structure for %s", __func__, control_in_focus);
		return FALSE;
	}

	const credit_card_input_t* next_input_to_focus = credit_card_input_get_by_id(input_in_focus->next_input_id);
	if (NULL == next_input_to_focus) {
		//no next control to move focus onto
		return FALSE;
	}

	layout_mgr_err_t layout_err_val = layout_mgr_set_focus_input(next_input_to_focus->name);
	if (layout_err_val != LAYOUT_MGR_SUCCESS) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_focus_input API error - %d", __func__, layout_err_val);
		return FALSE;
	}

	return TRUE;
}

static qsee_tui_dialog_ret_val_t credit_card_update_controls(const layout_mgr_touched_obj_t* curr_touched_object)
{

	layout_mgr_err_t layout_err_val = LAYOUT_MGR_SUCCESS;
	//update all controls which doesn't depend on the currently touched object

	if (credit_card_has_empty_input()) {
		//if we have even one empty input disable the PAY button
		layout_err_val = layout_mgr_set_button_state(BUTTON_PAY, LAYOUT_BUTTON_STATUS_DISABLED);
	}

	if (NULL == curr_touched_object || curr_touched_object->eventType != LAYOUT_MGR_EVENT_UP) {
		//we don't have anything else to update as this function not called in touch context
		return TUI_SUCCESS;
	}

	//set the pay button state to release as all the fields are filled
	//this should be done only as a response to touch event on the dialog
	if (!credit_card_has_empty_input()) {
			layout_err_val = layout_mgr_set_button_state(BUTTON_PAY, LAYOUT_BUTTON_STATUS_RELEASED);
	}

	if (layout_err_val != LAYOUT_MGR_SUCCESS) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_button_state API error - %d", __func__, layout_err_val);
		return TUI_LAYOUT_ERROR;
	}

	//now update controls based on the touched object

	//we need to move focus to then next input if the following happens:
	// - the focused input control has exactly the number of characters it expects
	// - the last character was added to it during this event (any of the data keys pressed)
	utf8_char_t button_data = {0};
	if (LAYOUT_MGR_SUCCESS == layout_mgr_get_button_data(curr_touched_object->objectName, &button_data)) {
		const char* focused_input_name = NULL;
		const uint8_t* text = NULL;
		uint32_t textByteSize = 0;
		uint32_t textCharSize = 0;

		//set the invalid card message label to hidden
		layout_err_val = layout_mgr_set_show_flag(LABEL_CARD_STATUS, FALSE);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_show_flag API error - %d", __func__, layout_err_val);
			return TUI_LAYOUT_ERROR;
		}

		layout_err_val = layout_mgr_get_focus_input(&focused_input_name);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_get_focus_input API error - %d", __func__, layout_err_val);
			return TUI_LAYOUT_ERROR;
		}
		//set the current color to regular just in case it was changed before
		layout_err_val = layout_mgr_set_text_color(focused_input_name, COLOR_BLACK);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_text_color API error - %d", __func__, layout_err_val);
			return TUI_LAYOUT_ERROR;
		}
		//check if the focused input field has exactly the number of chars
		layout_err_val = layout_mgr_get_text(focused_input_name, &text, &textByteSize, &textCharSize);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_get_text API error - %d", __func__, layout_err_val);
			return TUI_LAYOUT_ERROR;
		}
		const credit_card_input_t* focused_input = credit_card_input_get_by_name(focused_input_name);
		if (NULL == focused_input) {
		     qsee_log(QSEE_LOG_MSG_DEBUG, "%s: credit_card_input_get_by_name() returned NULL value", __func__);
		     return TUI_LAYOUT_ERROR;
	    }
		if (textCharSize  == focused_input->expected_num_digits) {
			//try to move focus to the next control
			credit_card_move_focus_to_next_control(focused_input_name);
		}
	}

	return TUI_SUCCESS;
}

static qsee_tui_dialog_ret_val_t credit_card_dialog_init(LayoutPage_t* page) {

	layout_mgr_err_t layout_err_val = LAYOUT_MGR_SUCCESS;

	if (NULL == page) {
		  qsee_log(QSEE_LOG_MSG_ERROR, "%s: layout is NULL", __func__);
		  return TUI_ILLEGAL_INPUT;
	}

	layout_mgr_err_t layout_res = layout_mgr_load_layout(page);
	if (layout_res < 0) {
		  qsee_log(QSEE_LOG_MSG_ERROR, "%s: layout_mgr_load_layout failed with %d", __func__, layout_res);
		  return TUI_LAYOUT_ERROR;
	}

	for (uint32_t i = 0; i < credit_card_data_size; i ++) {
		credit_card_input_t* input = &credit_card_data[i];
		layout_err_val = layout_mgr_set_buffer_for_input(input->name, input->buffer, CUSTOM_INPUT_BUFFER_SIZE);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_buffer_for_input API error - %d", __func__, layout_err_val);
			return TUI_LAYOUT_ERROR;
		}
	}

	static const char* hint_mm = "MM";
	static const char* hint_yy = "YY";
	const credit_card_input_t* input_mm = credit_card_input_get_by_id(CARD_INPUT_MM);
	const credit_card_input_t* input_yy = credit_card_input_get_by_id(CARD_INPUT_YY);
	if (NULL == input_mm || NULL == input_yy) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: error getting input fields", __func__);
		return TUI_LAYOUT_ERROR;
	}
	layout_err_val = layout_mgr_set_text_hint(input_mm->name,(const uint8_t*) hint_mm, strlen(hint_mm));
	if (layout_err_val != LAYOUT_MGR_SUCCESS) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_text_hint API error - %d", __func__, layout_err_val);
		return TUI_LAYOUT_ERROR;
	}
	layout_err_val = layout_mgr_set_text_hint(input_yy->name,(const uint8_t*) hint_yy, strlen(hint_yy));
	if (layout_err_val != LAYOUT_MGR_SUCCESS) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_text_hint API error - %d", __func__, layout_err_val);
		return TUI_LAYOUT_ERROR;
	}

	//set the invalid card message label to hidden
	layout_err_val = layout_mgr_set_show_flag(LABEL_CARD_STATUS, FALSE);
	if (layout_err_val != LAYOUT_MGR_SUCCESS) {
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: layout_mgr_set_show_flag API error - %d", __func__, layout_err_val);
		return TUI_LAYOUT_ERROR;
	}

	//set own secure indicator image if provided
	if (sec_ind_buffer != NULL) {
		layout_err_val = layout_mgr_set_image("image_0", sec_ind_buffer, LAYOUT_ALIGN_MID, LAYOUT_ALIGN_CENTER);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_ERROR, "Error setting local secure indicator - %d", layout_err_val);
			return TUI_LAYOUT_ERROR;
		}
	}

	//set dynamic logo image of provided
	if (logo_buffer != NULL) {
		layout_err_val = layout_mgr_set_image("image_1", logo_buffer, LAYOUT_ALIGN_MID, LAYOUT_ALIGN_CENTER);
		if (layout_err_val != LAYOUT_MGR_SUCCESS) {
			qsee_log(QSEE_LOG_MSG_ERROR, "Error setting dynamic logo - %d", layout_err_val);
			return TUI_LAYOUT_ERROR;
		}
	}

	return credit_card_update_controls(NULL);
}

static sec_touch_cmd_t credit_card_dialog_event(layout_mgr_err_t layout_mgr_status, layout_mgr_touched_obj_t curr_touched_object,
		qsee_tui_dialog_ret_val_t* final_status)
{
	*final_status = TUI_SUCCESS;

	//if the pay button (ENTER) was pressed we validate the input and only then allow the dialog to complete
	if (LAYOUT_MGR_ENTER_PRESSED == layout_mgr_status && (TUI_SUCCESS == credit_card_dialog_validate())) {
		*final_status = TUI_OK_PRESSED;
		return SEC_TOUCH_CMD_COMPLETED;
	}

	//check if the 'SKIP' button was pressed
	if (LAYOUT_MGR_F1_PRESSED == layout_mgr_status) {
		// clear all buffers as we skip validation
		credit_card_clear_input_buffers();
		*final_status = TUI_OK_PRESSED;
		return SEC_TOUCH_CMD_COMPLETED;
	}

	if (LAYOUT_MGR_CANCEL_PRESSED == layout_mgr_status) {
		*final_status = TUI_CANCEL_PRESSED;
		return SEC_TOUCH_CMD_CANCELLED;
	}

	//update all the dialog controls state
	*final_status = credit_card_update_controls(&curr_touched_object);
	if (*final_status != TUI_SUCCESS) {
		return SEC_TOUCH_CMD_CANCELLED;
	}

	//is it expected to set the final status according to layout manager status
	return SEC_TOUCH_CMD_CONTINUE;
}


static int32_t start_custom_credit_card_dialog() {

	  qsee_tui_dialog_params_t credit_card_params = {0};

	  //load dialog based on display resolution. get_layout_by_name() can be
	  //used directly if the layout name is known at compile time
	  LayoutPage_t* page = get_layout_name_for_current_resolution("credit_card");//get_layout_by_name("credit_card");

	  if (NULL == page) {
		  qsee_log(QSEE_LOG_MSG_ERROR, "error get_layout_name_for_current_resolution('credit_card') for login dialog layout");
		  return TUI_ILLEGAL_INPUT;
	  }

	  qsee_tui_dialog_ret_val_t res = credit_card_dialog_init(page);
	  if (res < 0) {
		  return res;
	  }

	  credit_card_params.layout_page = page;

	  credit_card_params.dialogType = TUI_DIALOG_CUSTOM;

	  if (sec_ind_buffer != NULL) {
	      // we will use own secure indicator
	      credit_card_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = FALSE;
	  } else {
	      // display global secure indicator
	      credit_card_params.dialogCommonParams.secureIndicatorParams.bDisplayGlobalSecureIndicator = TRUE;
	  }

	  credit_card_params.dialogCommonParams.n32TimeOut = DEFAULT_TIMEOUT;
	  credit_card_params.customDialogParams.manage_layout_event_custom = credit_card_dialog_event;
	return qsee_tui_dialog(&credit_card_params,&status);
}

/******** end of implementation of a custom secure UI dialog sample for entering credit card info *******/


void tui_dialogs_app_cmd_handler(void* cmd, uint32_t cmdlen,
                        void* rsp, uint32_t rsplen)
{
  struct send_cmd *cmd_ptr = (struct send_cmd *)cmd;
  struct send_cmd_rsp *rsp_ptr = (struct send_cmd_rsp *)rsp;
  uint8_t msg[MAX_MSG_LEN];
  const char* login_user = "Your username is: ";
  const char* login_pass = " Your password is: ";
  const char* pin = "Your pin is: ";
  uint32_t len;
  int32_t str_len_ret=0;

  if ( NULL == cmd || NULL == rsp) {
	qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received null cmd/rsp buffers");
    return;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "status = %d", status);

  if(status < 0 || TUI_CANCEL_PRESSED == status || TUI_MIDDLE_PRESSED == status|| TUI_LEFT_PRESSED == status){
	  if (cmd_ptr->cmd_id != SEC_UI_SAMPLE_CMD14_MSG_CLEAN_UP) {
		  qsee_log(QSEE_LOG_MSG_DEBUG, "point 0, status %d", status);
		  rsp_ptr->status = -1;
		  return;
	  }
  }

  rsp_ptr->status = 0;
  switch (cmd_ptr->cmd_id) {
    case SEC_UI_SAMPLE_CMD10_GET_PIN:
    	rsp_ptr->status = start_pin_dialog();
    	break;

    case SEC_UI_SAMPLE_CMD11_LOGIN:
    	rsp_ptr->status = start_login_dialog();
    	break;

    case SEC_UI_SAMPLE_CMD12_MSG_PIN:

    	/* set the null terminator to treat the user input as a string */
    	if(MAX_INPUT_LEN_BYTES - 1 < received_pin_len){
    		qsee_log(QSEE_LOG_MSG_DEBUG, "cannot set the null terminator, received pin is too long");
    		rsp_ptr->status = -1;
    		break;
    	}
    	received_pin[received_pin_len] = '\0';

    	strlcpy((char*)msg, pin, MAX_MSG_LEN);
    	len = strlcat((char*)msg, (char*)received_pin, MAX_MSG_LEN);
    	if (len > MAX_MSG_LEN){
    		qsee_log(QSEE_LOG_MSG_DEBUG, "truncated response");
    		rsp_ptr->status = -1;
    	} else{
    		rsp_ptr->status = start_msg_dialog(msg, len);
    	}
    	break;

    case SEC_UI_SAMPLE_CMD13_MSG_LOGIN:
    	/* set the null terminator to treat the user input as a string */
    	if(MAX_INPUT_LEN_BYTES - 1 < received_username_len){
    		qsee_log(QSEE_LOG_MSG_DEBUG, "cannot set the null terminator, received username is too long");
    		break;
    	}
    	received_username[received_username_len] = '\0';

    	/* set the null terminator to treat the user input as a string */
    	if(MAX_INPUT_LEN_BYTES - 1 < received_password_len){
    		qsee_log(QSEE_LOG_MSG_DEBUG, "cannot set the null terminator, received password is too long");
    		break;
    	}
    	received_password[received_password_len] = '\0';

    	strlcpy((char*)msg, login_user, MAX_MSG_LEN);
    	strlcat((char*)msg, (char*)received_username, MAX_MSG_LEN);
    	strlcat((char*)msg, login_pass, MAX_MSG_LEN);
    	len = strlcat((char*)msg, (char*)received_password, MAX_MSG_LEN);
    	if(len > MAX_MSG_LEN){
    		qsee_log(QSEE_LOG_MSG_DEBUG, "cannot call start_msg_dialog, msg is too long");
    		rsp_ptr->status = -1;
    		break;
    	}
    	qsee_log(QSEE_LOG_MSG_DEBUG, "calling start_msg_dialog with msg = %s, len= %d", msg, len);
    	rsp_ptr->status = start_msg_dialog(msg,len);
    	break;
    case SEC_UI_SAMPLE_CMD15_CUSTOM_DIALOG:
    	rsp_ptr->status = start_custom_credit_card_dialog();
    	break;
    case SEC_UI_SAMPLE_CMD16_MSG_CUSTOM:
    	str_len_ret = snprintf((char*)msg, MAX_MSG_LEN, "card: %s-%s-%s-%s %s/%s cvv:%s",
    			credit_card_input_get_by_id(CARD_INPUT_NUM1)->buffer,
				credit_card_input_get_by_id(CARD_INPUT_NUM2)->buffer,
				credit_card_input_get_by_id(CARD_INPUT_NUM3)->buffer,
				credit_card_input_get_by_id(CARD_INPUT_NUM4)->buffer,
				credit_card_input_get_by_id(CARD_INPUT_MM)->buffer,
				credit_card_input_get_by_id(CARD_INPUT_YY)->buffer,
				credit_card_input_get_by_id(CARD_INPUT_CVV)->buffer);
    	len = strlen((char*)msg);
    	if (str_len_ret < 0 || str_len_ret >= MAX_MSG_LEN) {
    		qsee_log(QSEE_LOG_MSG_DEBUG, "custom dialog message string invalid or too long %s, len= %d", msg, len);
    		break;
    	}
    	qsee_log(QSEE_LOG_MSG_DEBUG, "calling start_msg_dialog with msg = %s, len= %d", msg, len);
    	rsp_ptr->status = start_msg_dialog(msg,len);
    	break;
    case SEC_UI_SAMPLE_CMD14_MSG_CLEAN_UP:
    	qsee_tui_tear_down();
    	break;

    default:
      rsp_ptr->status = SEC_UI_NOT_SUPPORTED;
      qsee_log(QSEE_LOG_MSG_DEBUG, "Unsupported command\n\n");
      break;
  }
}

void tui_dialogs_app_shutdown(void)
{
  qsee_tui_release_secure_indicator();
  qsee_tui_tear_down();
  qsee_free(sec_ind_buffer);
  qsee_free(logo_buffer);
}
