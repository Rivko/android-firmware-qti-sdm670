/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __COMMON_H
#define __COMMON_H
#include <cutils/log.h>
#include <cutils/properties.h>
#include <ctype.h>
#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unordered_map>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <list>
#include <private/android_filesystem_config.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/prctl.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/input.h>

#include "events.h"
#include "mmi_log.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif


#ifndef ANDROID
#include <stdio.h>
#define ALOGE(fmt, args...) do { \
          fprintf(stderr, fmt"\n", ##args); \
        } while (0)
#define ALOGI(fmt, args...) do { \
          fprintf(stderr, fmt"\n", ##args); \
        } while (0)
#define ALOGD(fmt, args...) do { \
          fprintf(stderr, fmt"\n", ##args); \
        } while (0)
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG   "mmi"
#endif

using namespace std;

/**Function*/
#define BITS_PER_LONG  (sizeof(unsigned long) * 8)
#define BITS_TO_LONGS(x)  (((x) + BITS_PER_LONG - 1) / BITS_PER_LONG)
#define set_bit(bit, array)   (array)[(bit)/BITS_PER_LONG] |=(1L << ((bit) % BITS_PER_LONG))
#define clear_bit(bit, array)	(array)[(bit)/BITS_PER_LONG] &=(~ (1L << ((bit) % BITS_PER_LONG)))
#define test_bit(bit, array)   ((array)[(bit)/BITS_PER_LONG] & (1L << ((bit) % BITS_PER_LONG)))

typedef void (*cb_t) (void *);
typedef void *(*thread_func_t) (void *);

/**MMI*/
#define MMI_VERSION "Version 2.1"
#define MMI_DIAG_READ_VERSION "210"

#define MMI_SOCKET MMI_SOCKET_DIR"mmi"
#define MMI_SOCKET_DIR "/dev/socket/"
#define MAIN_MODULE  "MMI"
#define KEY_CAMERA_SNAPSHOT	0x2fe
#define MMI_PCBA_SYS_CONFIG "mmi-pcba-sys.cfg"
#define AUTOSTART_CONFIG ".autostart"
#define MMI_LOG_FILENAME "mmi.log"

#define MMI_RES_BASE_DIR "/etc/mmi/"
#define MMI_RES_FONTS MMI_RES_BASE_DIR"fonts.ttf"
#define MMI_RES_LANG_EN MMI_RES_BASE_DIR"strings.xml"
#define MMI_RES_LANG_CN MMI_RES_BASE_DIR"strings-zh-rCN.xml"

#define MMI_PCBA_CONFIG get_mmi_cfg_file(CFG_TYPE_MMI_PCBA_CFG)
#define MMI_PATH_CONFIG MMI_RES_BASE_DIR"path_config.xml"

#define MMI_LAYOUT_BASE_DIR MMI_RES_BASE_DIR"layout/"
#define MMI_CONFIG get_mmi_cfg_file(CFG_TYPE_MMI_CFG)

#define HOME_SCREENS "home_screens"
#define LAYOUT_CONFIRM  "layout_confirm.xml"
#define LAYOUT_REBOOT  "layout_reboot.xml"
#define LAYOUT_PCBA  "layout_pcba.xml"
#define LAYOUT_REPORT "layout_report.xml"
#define LAYOUT_INDICATOR  "layout_indicator.xml"

#define PERCENT_MAX 100
#define SIZE_512 512
#define BLOCK_SIZE 512
#define SIZE_1K 1024
#define SIZE_2K (SIZE_1K*2)
#define SIZE_8K (SIZE_1K*8)
#define SIZE_1M (SIZE_1K*SIZE_1K)
#define SIZE_1G (SIZE_1K*SIZE_1K*SIZE_1K)

/**Font size config, percent of LCD density*/
#define FONT_SIZE_SMALL 3
#define FONT_SIZE_NORMAL 4
#define FONT_SIZE_LARGE 6
#define FONT_SIZE_HUGE 8

#define SUBCMD_MMI "mmi"
#define SUBCMD_PCBA "pcba"

#define METHOD "method"
/**PCBA test method for LCD module*/
#define LCD_METHOD_READ_ID "read_id"
#define LCD_METHOD_SHOW_RGB "show_rgb"
#define LCD_METHOD_SHOW_IMAGE "show_image"

/**PCBA test method for touch module*/
#define TOUCH_METHOD_READ_EVENT_ID "read_event_id"
#define TOUCH_METHOD_READ_TOUCH_ID "read_touch_id"
#define TOUCH_METHOD_TOUCH_TRACK "touch_track"
#define TOUCH_METHOD_SHOW_RGB "show_rgb"

/**Power test method for power module*/
#define POWER_METHOD_SUSPEND "suspend"

/**Config file Key words*/
#define KEY_AUTOMATION "automation"
#define KEY_TESTMODE "test_mode"
#define KEY_STR_LANGUAGE "language"
#define KEY_FONTSIZE "font_size"
#define KEY_LIB_NAME "lib_name"
#define KEY_DISPALY_NAME "display_name"
#define KEY_ENABLE "enable"
#define KEY_LAYOUT "layout"
#define KEY_PARAMETER "parameter"

/**Result file keywords*/
#define KEY_TIMESTAMP_WORDS "Timestamp"
#define KEY_RESULT_WORDS "Result"
#define KEY_TESTTIME_WORDS "TestTime_Sec"

/**Button name string list*/
#define KEY_MAIN_STATUS "main_status"
#define KEY_MAIN_ALL "main_all"
#define KEY_MAIN_FAILED "main_failed"
#define KEY_MAIN_RUNALL "main_runall"
#define KEY_DISPLAY "display"
#define KEY_INDICATOR "indicator"

#define KEY_DISPLAY_PCBA "display_pcba"
#define KEY_REBOOT_ANDROID "reboot_android"
#define KEY_REBOOT_POWEROFF "reboot_poweroff"
#define KEY_REBOOT_POWEROFF_SHIPMODE "reboot_poweroff_shipmode"
#define KEY_REBOOT_FFBM "reboot_ffbm"

#define KEY_STR_EXTRACMD "extracmd"
#define KEY_STR_EXTRACMDSIZE "extracmd_size"
#define KEY_BTN "btn"
#define KEY_PASS "pass"
#define KEY_FAIL "fail"
#define KEY_STR_TITLE "title"
#define KEY_STR_INSTRUCTION "instruction"
#define KEY_STR_INDICATION "indication"
#define KEY_STR_FRONT "front"
#define KEY_STR_BACK "back"
#define KEY_STR_LEFT "left"
#define KEY_STR_RIGHT "right"
#define KEY_STR_CENTER "center"

/**KEYPAD name define*/
#define KEYPAD_STR_HOME "home"
#define KEYPAD_STR_MENU "menu"
#define KEYPAD_STR_BACK "back"
#define KEYPAD_STR_VOLUMEUP "volumeup"
#define KEYPAD_STR_VOLUMEDOWN "volumedown"
#define KEYPAD_STR_SNAPSHOT "snapshot"
#define KEYPAD_STR_POWER "power"
#define KEYPAD_STR_HEADPHONE "headphone"
#define KEYPAD_STR_MICROPHONE "microphone"
#define KEYPAD_STR_HANGUP "hangup"
#define KEYPAD_STR_0 "0"
#define KEYPAD_STR_1 "1"
#define KEYPAD_STR_2 "2"
#define KEYPAD_STR_3 "3"
#define KEYPAD_STR_4 "4"
#define KEYPAD_STR_5 "5"
#define KEYPAD_STR_6 "6"
#define KEYPAD_STR_7 "7"
#define KEYPAD_STR_8 "8"
#define KEYPAD_STR_9 "9"
#define KEYPAD_STR_NUMERIC_POUND "#"
#define KEYPAD_STR_STAR "*"
#define KEYPAD_STR_LEFTSHIFT "left"
#define KEYPAD_STR_RIGHTSHIFT "right"
#define KEYPAD_STR_LEFT "leftsoft"
#define KEYPAD_STR_RIGHT "rightsoft"
#define KEYPAD_STR_OK "ok"
#define KEYPAD_STR_CALL "call"

/**Parameter keys*/
#define KEY_DIMENSION "dimension"
#define KEY_SMALL "small"
#define KEY_NORMAL "normal"
#define KEY_LARGE "large"
#define KEY_MIN_LIMINT "min_limit"
#define KEY_MAX_LIMINT "max_limit"

/**LANGUAGE KEYWORDS*/
#define KEY_TITLE_PCBA "pcba_title"
#define KEY_POWER_OFF_NOTICE "power_off_notice"
#define KEY_RESET_NOTICE "reset_notice"
#define KEY_REBOOT_NOTICE "reboot_notice"
#define KEY_RUN_ALL_NOTICE "run_all_notice"
#define KEY_UNPLUG_USB_NOTICE "unplug_usb_notice"

#define MAX_CAM_PREVIEW_BUFFERS 5

/**Diag control mmi commands list*/
#define CLIENT_DIAG_NAME "DIAG"
#define CLIENT_DEBUG_NAME "DEBUG"

#define SUBCMD_STAT "stat"                          /**Query the MMI status,include fail case number*/
#define SUBCMD_RECONFIG "reconfig"        /**Reconfig MMI using one specified CFG file*/
#define SUBCMD_CLEAR "clear"                     /**Clear the current result file*/
#define SUBCMD_RUNCASE "runcase"           /**Run specified case*/
#define SUBCMD_RUNALL "runall"                 /** Start Run all test cases*/
#define SUBCMD_LISTCASE "listcase"            /** List all the test cases into specified file*/
#define SUBCMD_EXITCASE "exitcase"          /** Exit the current test case*/
#define SUBCMD_APPEND_TO_RES "append"       /** Append to result test case*/
#define SUBCMD_UTILITY "utility"           /**Run utility for enter ship mode*/


/**Keyword used in parameters*/
#define KEY_MMI_STAT "state"
#define KEY_FAIL_COUNT "fail_count"
#define KEY_CFG_PATH "cfg_path"
#define KEY_TESTLIST_PATH "testlist_path"
#define KEY_CASE_NUM "case_num"
#define KEY_CASE_NAME "case_name"
#define KEY_EXIT_RESULT "exit_result"
#define KEY_CASE_DATA "case_data"
#define KEY_UTILITY_ID "utility_id"

/**Common keywords***/
#define KEY_ASCII_TRUE "54525535"   //ASCII for "TRUE"

/**Color value*/
#define COLOR_RED 0xff0000ff
#define COLOR_GREEN 0x00ff00ff
#define COLOR_BLUE 0x0000ffff
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x000000ff
#define COLOR_KEY_DEFAULT 0x007D7D7D

/**Magnetometer compass value*/
#define ROUND_ANGLE 360
#define ROUND_POINT_HALF_SIDE 4
#define ROUND_POINT_SIDE 8
#define POINT_HALF_SIDE 16
#define POINT_SIDE 32

#define IPC_SYNC_USE_SEM_T


enum {
    eOrientationDefault = 0,
    eOrientation90 = 1,
    eOrientation180 = 2,
    eOrientation270 = 3,
};


enum {
    CONFIG_SUCCESS = 0,
    CONFIG_NOT_FOUND_ERR = 1,
    CONFIG_FORTMAT_ERR = 2,
    CONFIG_TEST_CASE_ERR = 3,
    CONFIG_NO_DEFAULT_CFG_ERR = 4,
};

typedef enum {
    BOOT_MODE_NORMAL = 0,
    BOOT_MODE_PCBA = 1,
    BOOT_MODE_UI = 2,
    MAX_BOOT_MODE,
} boot_mode_type;

typedef enum {
    REBOOT_NONE = 0,
    REBOOT_POWEROFF = 1,
    REBOOT_FFBM = 2,
    REBOOT_ANDROID = 3,
    REBOOT_POWEROFF_SHIPMODE = 4,
    MAX_REBOOT,
} reboot_opt_t;

typedef enum {
    LEVEL_NONE = 0,
    LEVEL_STANDARD = 1,
    LEVEL_FUNCTION = 2,
} level_t;

enum {
    FAILED = -1,
    SUCCESS = 0,
    ERR_UNKNOW = INT_MAX
};

enum {
    FTM_SUCCESS = 0,
    FTM_FAIL = 1,
};

enum {
    MMI_IDLE = 0x0,
    MMI_BUSY = 0x1,
};

enum {
    MODULE_IDLE = 0x0,
    MODULE_RUNNING = 0x1,
};

enum {
    // server to client
    CMD_INIT = 0x1,
    CMD_DEINIT = 0x02,
    CMD_QUERY = 0x03,
    CMD_RUN = 0x04,
    CMD_STOP = 0x05,
    CMD_NV_READ = 0X06,
    CMD_NV_WRITE = 0X07,
    CMD_RESULT = 0x08,

    // client to server
    CMD_PRINT = 0x101,          // for display/log message
    CMD_HELLO = 0x103,          // for display/log message
    CMD_CTRL = 0x104,           //Diag control mmi behavior
};

typedef enum {
    PRINT = 0,                  //only print in screen
    DATA,                       //only save data in file
    PRINT_DATA,                 //print in screen and save data
} print_type_t;

/**MMI test mode*/
typedef enum {
    TEST_MODE_NONE = 0,
    TEST_MODE_PCBA = 1,
    TEST_MODE_UI = 2,
    MAX_TEST_MODE
} module_mode_t;

/*
 * communication header between process
 * */
typedef enum {
    BUF_STATE_EMPTY,
    BUF_STATE_DONE
} buffer_state_t;

enum {
    ID_AUTOSTART = 0,
    ID_WLAN = 1,
    ID_BT = 2,
    ID_CHARGER = 3,
    ID_NFC = 4,
    ID_SHIPMODE = 5
};

/*
 * communication message between mmi core and client
 * */
typedef struct {
    char module[64];
    int msg_id;
    int cmd;
    char subcmd[32];
    int length;
    int result;
    char msg[SIZE_1K];          //para:xxx;  capture:normal:para
} __attribute__ ((__packed__)) msg_t;

/**Queue List*/
typedef struct {
    list < msg_t * >queue;
    pthread_mutex_t lock;
} msg_queue_t;

typedef struct {
    bool is_valid;
    uint32_t size;
      list < string > cmd_list;
} extra_cmd_t;

class module_info {
  public:
    char module[64];
    int socket_fd;
    int result;
    pid_t pid;
    int mode;
    int running_state;
    bool is_run_all;
    extra_cmd_t extracmd;
    time_t start_time;          //start test time
    double duration;            //test duration
    time_t last_time;           //last time to modify test result data
    char data[SIZE_512];        //module test data

      unordered_map < string, string > config_list;
      module_info(char *mod) {
        if(mod != NULL)
            strlcpy(module, mod, sizeof(module));

        memset(data, 0, sizeof(data));
        result = INT_MAX;
        pid = -1;
        socket_fd = -1;
        extracmd.is_valid = false;
        running_state = MODULE_IDLE;
        is_run_all = false;
    }
};

typedef struct {
    cb_t cb;
    void *module;
} runnable_t;

typedef struct {
    list < runnable_t * >queue;
    pthread_mutex_t lock;
} runnable_queue_t;

typedef struct {
    union {
        struct {
            char r;
            char g;
            char b;
            char a;
        };
        int32_t color;
    };
} color_t;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} rect_t;

typedef enum {
    RECT_X = 0,
    RECT_Y = 1,
    RECT_W = 2,
    RECT_H = 3
} rect_value_t;

typedef struct {
    char name[32];
    cb_t cb;
} func_map_t;

#define PRESS_SHAKE_X 10
#define PRESS_SHAKE_Y 10

typedef struct {
    int x_max;
    int x_min;
    int y_max;
    int y_min;
} touch_xy_min_max_t;

class point_t {
  public:
    int x;
    int y;
    timeval tv;                 //receive time

      point_t() {
    }
    point_t(int x, int y) {
        this->x = x;
        this->y = y;
    }
    void operator=(const point_t & point) {
        this->x = point.x;
        this->y = point.y;
        this->tv = point.tv;
    }

    bool operator==(const point_t & point) {
        if(this->x == point.x && this->y == point.y)
            return true;
        return false;
    }
};


/**************************/
class mutex_locker {
    pthread_mutex_t m_mutex;
  public:
      class autolock {
        mutex_locker & locker;
      public:
        inline autolock(mutex_locker & locker):locker(locker) {
            locker.lock();
        }
        inline ~ autolock() {
            locker.unlock();
        }
    };
    inline mutex_locker() {
        pthread_mutex_init(&m_mutex, 0);
    }
    inline ~ mutex_locker() {
        pthread_mutex_destroy(&m_mutex);
    }
    inline void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    inline void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
};

typedef struct exec_cmd_t {
    const char *cmd;
    char **params;
    char *result;
    char *exit_str;
    int *pid;
    int size;
} exec_cmd_t;

typedef void (*callback) (char *, int);

typedef struct {
    const char *key_name;
    class button *key_btn;
    int key_code;
    bool exist;
    bool tested;
} key_map_t;

typedef struct {
    class module_info *mod;
    struct input_event *ev;
} mod_ev_t;

#endif
