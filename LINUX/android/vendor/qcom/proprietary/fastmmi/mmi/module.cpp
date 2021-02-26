/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "config.h"
#include "module.h"
#include "utils.h"
#include "mmi.h"
#include "input.h"
#include "lang.h"
#include "events.h"

static pthread_t g_module_tid;

static const char *module_stem_32[] = {
    MODULE_CAMERA,
};

/**If it only support 32bit*/
static bool is_32bit_module(module_info * mod) {
    bool ret = false;
    uint32_t i = 0;

    if(mod == NULL)
        return false;

    for(i = 0; i < sizeof(module_stem_32) / sizeof(char *); i++) {
        if(!mod->config_list[KEY_LIB_NAME].empty()
           && !strcmp(module_stem_32[i], mod->config_list[KEY_LIB_NAME].c_str())) {
            ret = true;
            break;
        }
    }

    return ret;
}

/**If it is audio module*/
bool is_audio_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_AUDIO, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is audio headset module*/
bool is_audio_headset(module_info * mod) {
    return is_audio_module(mod) && (strstr(mod->module, "HEADSET") != NULL);
}

/**If it is battery module*/
bool is_battery_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_BATTERY, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is bluetooth module*/
bool is_bluetooth_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_BLUETOOTH, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is camera module*/
bool is_camera_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_CAMERA, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is cpu module*/
bool is_cpu_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_CPU, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is fm module*/
bool is_fm_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_FM, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is gps module*/
bool is_gps_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_GPS, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is headset module*/
bool is_headset_key_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_HEADSET_KEY, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is key module*/
bool is_key_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_KEY, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is keypad module*/
bool is_keypad_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_KEY, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is light module*/
bool is_light_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_LIGHT, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is lcd module*/
bool is_lcd_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_LCD, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is memory module*/
bool is_memory_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_MEMORY, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is nfc module*/
bool is_nfc_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_NFC, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is sensor module*/
bool is_sensor_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_SENSOR, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is sim module*/
bool is_sim_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_SIM, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is storage module*/
bool is_storage_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_STORAGE, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is touch module*/
bool is_touch_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_TOUCH, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is vibrator module*/
bool is_vibrator_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_VIBRATOR, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is wifi module*/
bool is_wifi_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_WIFI, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is telephone module*/
bool is_telephone_module(module_info * mod) {
    return mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
        && !strcmp(MODULE_TELEPHONE, mod->config_list[KEY_LIB_NAME].c_str());
}

/**If it is autoswitch module*/
bool is_autoswitch_module(module_info * mod) {
    return is_bluetooth_module(mod) || is_gps_module(mod) || is_sim_module(mod) || is_storage_module(mod)
        || is_wifi_module(mod) || is_battery_module(mod) || is_nfc_module(mod) || is_telephone_module(mod);
}

/**Check if test locally */
bool is_local_module(module_info * mod) {
    return is_lcd_module(mod) || is_touch_module(mod) || is_key_module(mod) ||  is_keypad_module(mod) ||is_headset_key_module(mod);
}

/**Check touch module*/
bool is_touch_show_rgb(module_info * mod) {
    unordered_map < string, string > params;

    if(mod == NULL || mod->config_list[KEY_LIB_NAME].empty())
        return false;

    if(!strcmp(MODULE_TOUCH, mod->config_list[KEY_LIB_NAME].c_str())) {

        parse_parameter(mod->config_list["parameter"], params);
        if(!strcmp(params[METHOD].c_str(), TOUCH_METHOD_SHOW_RGB)) {
            return true;
        }
    }

    return false;
}

/**Check lcd module*/
bool is_lcd_show_image(module_info * mod) {
    unordered_map < string, string > params;

    if(mod == NULL || mod->config_list[KEY_LIB_NAME].empty())
        return false;

    if(!strcmp(MODULE_LCD, mod->config_list[KEY_LIB_NAME].c_str())) {

        parse_parameter(mod->config_list["parameter"], params);
        if(!strcmp(params[METHOD].c_str(), LCD_METHOD_SHOW_IMAGE)) {
            return true;
        }
    }

    return false;
}

/**Check lcd module*/
bool is_lcd_show_rgb(module_info * mod) {
    unordered_map < string, string > params;

    if(mod == NULL || mod->config_list[KEY_LIB_NAME].empty())
        return false;

    if(!strcmp(MODULE_LCD, mod->config_list[KEY_LIB_NAME].c_str())) {

        parse_parameter(mod->config_list["parameter"], params);
        if(!strcmp(params[METHOD].c_str(), LCD_METHOD_SHOW_RGB)) {
            return true;
        }
    }

    return false;
}



/**Function called by others */
void launch_main() {
       /**Unregister all listener when return to main*/
#ifdef ANDROID
    unregister_input_listener();
#endif
    switch_module(get_main_module());
}

void initial_screen(module_info * mod) {
    button *obj = NULL;
    textview *tv = NULL;
    char tmp[64] = { 0 };
    char module_name[32] = { 0 };
    if(mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return;
    }

    layout *curlay = acquire_cur_layout();

    if(curlay != NULL) {
        MMI_ALOGI("acquire cur layout: '%s'", curlay->get_layout_path());
        obj = curlay->find_button_by_name(KEY_STR_TITLE);
        if(obj != NULL)
            obj->set_text(get_string(mod->config_list[KEY_DISPALY_NAME]));
        else {
            MMI_ALOGW("no find button(%s) in layout:'%s'", KEY_STR_TITLE, curlay->get_layout_path());
        }

        tv = curlay->find_textview_by_name(KEY_STR_INSTRUCTION);
        if(tv != NULL) {
            strlcpy(module_name, mod->module, sizeof(module_name));
            string_to_lower(module_name, sizeof(module_name));
            MMI_ALOGI("Trans: %s to %s", mod->module, module_name);
            snprintf(tmp, sizeof(tmp), "%s_instruction", module_name);
            tv->set_text(get_string(tmp));
        } else {
            MMI_ALOGW("no find textview(%s) in layout:'%s'", KEY_STR_INSTRUCTION, curlay->get_layout_path());
        }

        tv = curlay->find_textview_by_name(KEY_DISPLAY);
        if(tv != NULL) {
            tv->set_text(get_string("notice"));
        } else {
            MMI_ALOGW("no find textview(%s) in layout:'%s'", KEY_DISPLAY, curlay->get_layout_path());
        }
    } else {
        MMI_ALOGE("acquire cur layout fail for module:[%s]", mod->module);
    }

    release_cur_layout();
}

/**Wrapped basical function for running specified cmd*/
static void send_cmd(module_info * mod, int cmd, const char *subcmd) {

    if(mod == NULL) {
        MMI_ALOGE("Invalid command, mod is NULL");
        return;
    }

    if(mod->socket_fd < 0) {
        MMI_ALOGE("Invalid  socket fd for module:[%s]", mod->module);
        return;
    }

    string parameter = mod->config_list["parameter"];
    string limit = mod->config_list["limit"];

    if(!parameter.empty() && parameter.at(parameter.length() - 1) != ';') {
        parameter.append(";");
    }
    parameter.append(limit);

    send_cmd(mod->socket_fd, mod->module, cmd, subcmd, parameter.c_str(), parameter.length());
}

/** RUN MMI */
static void send_run_mmi(module_info * mod) {
    send_cmd(mod, CMD_RUN, SUBCMD_MMI);
}

/**RUN PCBA*/
static void send_run_pcba(module_info * mod) {
    send_cmd(mod, CMD_RUN, SUBCMD_PCBA);
}

/**RUN extra cmd*/
void send_run_extra(module_info * mod, const char *subcmd) {
    send_cmd(mod, CMD_RUN, subcmd);
}

/**Query*/
static void send_query(module_info * mod) {
    send_cmd(mod, CMD_QUERY, NULL);
}

/**INIT*/
static void send_init(module_info * mod) {
    send_cmd(mod, CMD_INIT, NULL);
}

/**DEINIT*/
static void send_deinit(module_info * mod) {
    send_cmd(mod, CMD_DEINIT, NULL);
}

/**STOP*/
static void send_stop(module_info * mod) {
    send_cmd(mod, CMD_STOP, NULL);
}

/**Fork module process*/
int fork_launch_module(module_info * mod) {
    char para[1024] = { 0 };
    char agent[64] = { 0 };
    char lib_path[64] = { 0 };
    if(mod == NULL)
        return -1;

#ifdef __LP64__
    if(is_32bit_module(mod)) {
        strlcpy(agent, get_value(KEY_MMI_AGENT32), sizeof(agent));
        strlcpy(lib_path, get_value(KEY_MMI_LIB_BASE), sizeof(lib_path));
    } else {
        strlcpy(agent, get_value(KEY_MMI_AGENT64), sizeof(agent));
        strlcpy(lib_path, get_value(KEY_MMI_LIB64_BASE), sizeof(lib_path));
    }
#else
    if(check_file_exist(get_value(KEY_MMI_AGENT32))) {
        strlcpy(agent, get_value(KEY_MMI_AGENT32), sizeof(agent));
    } else {
        strlcpy(agent, get_value(KEY_MMI_AGENT), sizeof(agent));
    }
    strlcpy(lib_path, get_value(KEY_MMI_LIB_BASE), sizeof(lib_path));
#endif

    MMI_ALOGI("fork '%s' for module:[%s]", agent, mod->module);
    snprintf(para, sizeof(para), "lib_path:%s%s;%s", lib_path,
             mod->config_list[KEY_LIB_NAME].c_str(), mod->config_list["parameter"].c_str());

    int pid = fork();

    if(pid == 0) {
        char *args[6] = { agent, "-m", mod->module, "-p", para, NULL };

        int res = execv(agent, args);

        if(res < 0) {
            mod->pid = -1;
            MMI_ALOGE("%s exec failed and exit, error=%s\n", agent, strerror(errno));
            exit(1);
        }
    } else if(pid < 0) {
        MMI_ALOGE("fork failed, error=%s\n", strerror(errno));
        return -1;
    } else if(pid > 0) {
        mod->pid = pid;
        MMI_ALOGD("mmi_agent process pid=%d for module:[%s]\n", pid, mod->module);
    }
    return 0;
}

void module_cleanup(module_info * mod) {
    layout *curlay = NULL;
    char temp[8] = { 0 };
    int i = 0;
    button *btn = NULL;
    bool ret = false;

    MMI_ALOGI("cleanup start for module:[%s]", mod->module);
    /*Send command to stop test */
    MMI_ALOGI("send stop cmd to stop module:[%s] test", mod->module);
    send_stop(mod);

    if(is_camera_module(mod)) {
        usleep(500 * 1000);
    }
#ifdef ANDROID
    if(is_lcd_module(mod)) {
        if(is_lcd_show_image(mod)) {
            stop_lcd_show_image();
        }
    }
#endif

    kill_thread(g_module_tid);

    if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "magnetic") != NULL) {
        curlay = acquire_cur_layout();
        for(i = 0; i < ROUND_ANGLE; i++) {
            snprintf(temp, sizeof(temp), "%d", i);
            if(curlay != NULL) {
                ret = curlay->delete_btn_by_name(temp);
                if(!ret) {
                    MMI_ALOGE("fail to delete button: %s", temp);
                } else {
                    MMI_ALOGI("button:%s be deleted", temp);
                }
            }
        }
        release_cur_layout();
    }

    MMI_ALOGI("cleanup finished for module:[%s]", mod->module);
}

void update_main_status() {

    uint32_t fail_num = 0;
    uint32_t pass_num = 0;
    uint32_t total_num = 0;
    uint32_t remain_num = 0;
    uint32_t batt_capacity = 0;
    char status[128] = { 0 };
    char buf[32] = { 0 };

    /*Get the main layout */
    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL) {
        MMI_ALOGE("No Main layout\n");
        return;
    }

    button *btn = lay->find_button_by_name(KEY_MAIN_STATUS);

    if(btn == NULL) {
        MMI_ALOGE("Main layout no status button\n");
        return;
    }

    list < item_t * >*item_list = lay->m_listview->get_items();

    total_num = item_list->size();
    pass_num = lay->m_listview->get_pass_count();
    fail_num = lay->m_listview->get_fail_count();

    remain_num = total_num - pass_num - fail_num;

    read_file("/sys/class/power_supply/battery/capacity", buf, sizeof(buf));
    snprintf(status, sizeof(status), "%d P | %d F  | %d L  | %d R  | %s %% B", pass_num, fail_num, remain_num,
             total_num, buf);
    btn->set_text(status);

    MMI_ALOGD("pass item: %d, fail item: %d, remain item: %d\n", pass_num, fail_num, remain_num);
}

#ifdef ANDROID
static void sensor_gyroscope_indication(module_info * mod, layout * layout) {

}

static void parse_sensors_xyz_value(const char *line, char *x, int x_len, char *y, int y_len, char *z, int z_len) {
    if(line == NULL || x == NULL || y == NULL || z == NULL)
        return;
    string input(line);
    int split_index = 0;

    split_index = input.find_first_of('x');
    if(split_index > 0) {
        if(strlen(line) - split_index - 3 < x_len) {
            strlcpy(x, line + split_index + 3, x_len);
            x[strlen(line) - split_index - 1] = '\0';
        }
    }
    split_index = input.find_first_of('y');
    if(split_index > 0) {
        if(strlen(line) - split_index - 3 < y_len) {
            strlcpy(y, line + split_index + 3, y_len);
            y[strlen(line) - split_index - 1] = '\0';
        }
    }
    split_index = input.find_first_of('z');
    if(split_index > 0) {
        if(strlen(line) - split_index - 3 < z_len) {
            strlcpy(z, line + split_index + 3, z_len);
            z[strlen(line) - split_index - 1] = '\0';
        }
    }
}

static void sensor_accelerometer_indication(module_info * mod, layout * layout) {
    button *front_btn = NULL;
    button *back_btn = NULL;
    button *left_btn = NULL;
    button *right_btn = NULL;
    button *centre_btn = NULL;
    char x[SIZE_1K] = { 0 };
    char y[SIZE_1K] = { 0 };
    char z[SIZE_1K] = { 0 };
    double x_temp = 0;
    double y_temp = 0;
    double z_temp = 0;

    front_btn = layout->find_button_by_name(KEY_STR_FRONT);
    if(front_btn != NULL)
        front_btn->set_color(COLOR_RED);
    else {
        MMI_ALOGE("Fail to get front button point for accelerometer sensor");
        return;
    }

    back_btn = layout->find_button_by_name(KEY_STR_BACK);
    if(back_btn != NULL)
        back_btn->set_color(COLOR_RED);
    else {
        MMI_ALOGE("Fail to get back button point for accelerometer sensor");
        return;
    }

    left_btn = layout->find_button_by_name(KEY_STR_LEFT);
    if(left_btn != NULL)
        left_btn->set_color(COLOR_RED);
    else {
        MMI_ALOGE("Fail to get lef button point for accelerometer sensor");
        return;
    }

    right_btn = layout->find_button_by_name(KEY_STR_RIGHT);
    if(right_btn != NULL)
        right_btn->set_color(COLOR_RED);
    else {
        MMI_ALOGE("Fail to get right button point for accelerometer sensor");
        return;
    }

    centre_btn = layout->find_button_by_name(KEY_STR_CENTER);
    if(centre_btn != NULL)
        centre_btn->set_color(COLOR_RED);
    else {
        MMI_ALOGE("Fail to get center button point for accelerometer sensor");
        return;
    }

    do {
        sem_wait(&g_data_print_sem);
        parse_sensors_xyz_value(mod->data, x, sizeof(x), y, sizeof(y), z, sizeof(z));
        x_temp = atof(x);
        y_temp = atof(y);
        z_temp = atof(z);
        if(x_temp > 4)
            left_btn->set_color(COLOR_GREEN);
        else if(x_temp < -4)
            right_btn->set_color(COLOR_GREEN);
        else if(y_temp < -3)
            front_btn->set_color(COLOR_GREEN);
        else if(y_temp > 4)
            back_btn->set_color(COLOR_GREEN);
        else if(z_temp < 0)
            centre_btn->set_color(COLOR_GREEN);
    } while(1);
}

static void sensor_light_indication(module_info * mod, layout * layout) {
    button *fb_btn = NULL;
    double temp = 0;
    double current = 0;
    char indicator = '=';
    char name[SIZE_1K] = { 0 };
    char value[SIZE_1K] = { 0 };

    fb_btn = layout->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);
    } else {
        MMI_ALOGE("Fail to get feedback button for light sensor");
        return;
    }

    do {
        temp = current;
        sem_wait(&g_data_print_sem);
        parse_value(mod->data, indicator, name, sizeof(name), value, sizeof(value));
        current = atof(value);
    } while((temp - current < 50) || (temp == 0));

    fb_btn->set_text("Test pass");
    fb_btn->set_color(COLOR_GREEN);
}

static void sensor_hall_indication(module_info * mod, layout * layout) {
    button *fb_btn = NULL;
    double temp = 0;
    double current = 0;
    char indicator = '=';
    char name[SIZE_1K] = { 0 };
    char value[SIZE_1K] = { 0 };

    fb_btn = layout->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);
    } else {
        MMI_ALOGE("Fail to get feedback button for light sensor");
        return;
    }

    do {
        temp = current;
        sem_wait(&g_data_print_sem);
        parse_value(mod->data, indicator, name, sizeof(name), value, sizeof(value));
        current = atof(value);
        MMI_ALOGD("sensor value: %lf", current);
    } while(current - temp == 0);

    fb_btn->set_text("Test pass");
    fb_btn->set_color(COLOR_GREEN);
}

static int convert_orientation(float mag_x, float mag_y) {
    float azimuth;
    const float rad2deg = 180 / M_PI;
    int angle = 0;

    azimuth = atan2(-(mag_x), mag_y);
    angle = (int) (azimuth * rad2deg);

    return angle;
}

static void sensor_magnetometer_indication(module_info * mod, layout * layout) {
    int w = get_fb_width();
    int h = get_fb_height();

    int centre_x = w / 2;
    int centre_y = 2 * h / 3;
    int radius = w / 4;
    int origin_x = 0;
    int origin_y = 0;
    int i = 0;
    int temp = 0;
    char x[SIZE_1K] = { 0 };
    char y[SIZE_1K] = { 0 };
    char z[SIZE_1K] = { 0 };
    char angle[8] = { 0 };

    double x_temp = 0;
    double y_temp = 0;
    double z_temp = 0;

    rect_t rect;
    button *btn[ROUND_ANGLE];

    for(i = 0; i < ROUND_ANGLE; i++) {
        origin_x = radius * sin((i * M_PI / 180) + (M_PI / 2));
        origin_y = radius * cos((i * M_PI / 180) + (M_PI / 2));
        rect.x = centre_x + origin_x - ROUND_POINT_HALF_SIDE;
        rect.y = centre_y - origin_y - ROUND_POINT_HALF_SIDE;
        rect.w = ROUND_POINT_SIDE;
        rect.h = ROUND_POINT_SIDE;

        snprintf(angle, sizeof(angle), "%d", i);
        btn[i] = new button(angle, rect, "", NULL);
        btn[i]->set_color(COLOR_BLUE);
        layout->add_button_locked(btn[i]);
    }

    do {
        sem_wait(&g_data_print_sem);
        parse_sensors_xyz_value(mod->data, x, sizeof(x), y, sizeof(y), z, sizeof(z));
        x_temp = atof(x);
        y_temp = atof(y);
        i = convert_orientation(x_temp, y_temp);
        if((i >= 0) && (i <= 180) && (i != temp)) {
            origin_x = radius * sin((i * M_PI / 180) + (M_PI / 2));
            origin_y = radius * cos((i * M_PI / 180) + (M_PI / 2));
            rect.x = centre_x + origin_x - POINT_HALF_SIDE;
            rect.y = centre_y - origin_y - POINT_HALF_SIDE;
            rect.w = POINT_SIDE;
            rect.h = POINT_SIDE;
            btn[i]->set_rect(&rect);
            btn[i]->set_color(COLOR_RED);

            origin_x = radius * sin((temp * M_PI / 180) + (M_PI / 2));
            origin_y = radius * cos((temp * M_PI / 180) + (M_PI / 2));
            rect.x = centre_x + origin_x - ROUND_POINT_HALF_SIDE;
            rect.y = centre_y - origin_y - ROUND_POINT_HALF_SIDE;
            rect.w = ROUND_POINT_SIDE;
            rect.h = ROUND_POINT_SIDE;
            btn[temp]->set_rect(&rect);
            btn[temp]->set_color(COLOR_GREEN);
        } else if((i < 0) && (i >= -180) && (i != temp)) {
            i = i + ROUND_ANGLE;
            origin_x = radius * sin((i * M_PI / 180) + (M_PI / 2));
            origin_y = radius * cos((i * M_PI / 180) + (M_PI / 2));
            rect.x = centre_x + origin_x - POINT_HALF_SIDE;
            rect.y = centre_y - origin_y - POINT_HALF_SIDE;
            rect.w = POINT_SIDE;
            rect.h = POINT_SIDE;
            btn[i]->set_rect(&rect);
            btn[i]->set_color(COLOR_RED);
            origin_x = radius * sin((i * M_PI / 180) + (M_PI / 2));
            origin_y = radius * cos((i * M_PI / 180) + (M_PI / 2));
            rect.x = centre_x + origin_x - ROUND_POINT_HALF_SIDE;
            rect.y = centre_y - origin_y - ROUND_POINT_HALF_SIDE;
            rect.w = ROUND_POINT_SIDE;
            rect.h = ROUND_POINT_SIDE;
            btn[temp]->set_rect(&rect);
            btn[temp]->set_color(COLOR_GREEN);
        }
        temp = i;
    } while(1);
}

static void sensor_proximity_indication(module_info * mod, layout * layout) {
    button *fb_btn = NULL;
    double temp;
    char indicator = '=';
    bool max_pass = false;
    bool min_pass = false;
    char name[1024] = { 0 };
    char value[1024] = { 0 };

    fb_btn = layout->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);
    } else {
        MMI_ALOGE("Fail to get feedback button for proximity sensor");
        return;
    }

    do {
        sem_wait(&g_data_print_sem);
        parse_value(mod->data, indicator, name, sizeof(name), value, sizeof(value));
        temp = atof(value);
        if(temp >= 5)
            max_pass = true;
        else if(temp < 2)
            min_pass = true;
    } while(!max_pass || !min_pass);

    fb_btn->set_text("Test pass");
    fb_btn->set_color(COLOR_GREEN);
}

static void launch_module_sensor(module_info * mod) {
    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "gyroscope") != NULL)
        sensor_gyroscope_indication(mod, lay);
    else if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "accelermeter") != NULL)
        sensor_accelerometer_indication(mod, lay);
    else if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "light") != NULL)
        sensor_light_indication(mod, lay);
    else if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "hall") != NULL)
        sensor_hall_indication(mod, lay);
    else if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "magnetic") != NULL)
        sensor_magnetometer_indication(mod, lay);
    else if(strstr(mod->config_list[KEY_PARAMETER].c_str(), "proximity") != NULL)
        sensor_proximity_indication(mod, lay);
}

static void launch_module_bluetooth(module_info * mod) {
    button *fb_btn = NULL;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    fb_btn = lay->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);

        sem_wait(&g_result_sem);
        if(mod->result == SUCCESS) {
            fb_btn->set_text("Test pass");
            fb_btn->set_color(COLOR_GREEN);
            MMI_ALOGI("module:[%s] test pass\n", mod->module);
        } else {
            fb_btn->set_text("Test fail");
            fb_btn->set_color(COLOR_RED);
            MMI_ALOGE("module:[%s] test fail\n", mod->module);
        }
    } else {
        MMI_ALOGW("Not find button(%s) in the layout(%s)\n", KEY_STR_INDICATION, lay->get_layout_path());
    }
}

static void launch_module_wifi(module_info * mod) {
    button *fb_btn = NULL;

        /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    fb_btn = lay->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);

        sem_wait(&g_result_sem);
        if(mod->result == SUCCESS) {
            fb_btn->set_text("Test pass");
            fb_btn->set_color(COLOR_GREEN);
            MMI_ALOGI("module:[%s] test pass\n", mod->module);
        } else {
            fb_btn->set_text("Test fail");
            fb_btn->set_color(COLOR_RED);
            MMI_ALOGE("module:[%s] test fail\n", mod->module);
        }
    } else {
        MMI_ALOGW("Not find button(%s) in the layout(%s)\n", KEY_STR_INDICATION, lay->get_layout_path());
    }
}

static void launch_module_gps(module_info * mod) {
    button *fb_btn = NULL;

        /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    fb_btn = lay->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);

        sem_wait(&g_result_sem);
        if(mod->result == SUCCESS) {
            fb_btn->set_text("Test pass");
            fb_btn->set_color(COLOR_GREEN);
            MMI_ALOGI("module:[%s] test pass\n", mod->module);
        } else {
            fb_btn->set_text("Test fail");
            fb_btn->set_color(COLOR_RED);
            MMI_ALOGE("module:[%s] test fail\n", mod->module);
        }
    } else {
        MMI_ALOGW("Not find button(%s) in the layout(%s)\n", KEY_STR_INDICATION, lay->get_layout_path());
    }
}

static void launch_module_nfc(module_info * mod) {
    button *fb_btn = NULL;

        /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        ALOGE("[%s] No layout\n", mod->module);
        return;
    }

    fb_btn = lay->find_button_by_name(KEY_STR_INDICATION);

    if(fb_btn != NULL) {
        fb_btn->set_text("Testing...");
        fb_btn->set_color(COLOR_BLUE);

        sem_wait(&g_result_sem);
        if(mod->result == SUCCESS) {
            fb_btn->set_text("Test pass");
            fb_btn->set_color(COLOR_GREEN);
        } else {
            fb_btn->set_text("Test fail");
            fb_btn->set_color(COLOR_RED);
        }
    }
}

static bool wait_for_headset(module_info * mod) {
    bool inserted = false;

    while(!inserted) {
        MMI_ALOGI("waiting for headset insert ...");
        inserted = get_sw_state(SW_HEADPHONE_INSERT);
        usleep(500 * 1000);
    }

    return inserted;
}

static bool pre_check(module_info * mod) {
    bool ret = true;

    if(is_audio_headset(mod)) {
        //ret = wait_for_headset(mod);
    }
    return ret;
}

static void *launch_module(void *module) {
    signal(SIGUSR1, signal_handler);

    if(module == NULL) {
        MMI_ALOGE("Invalid parameter");
        return NULL;
    }

    module_info *mod = (module_info *) module;

     /**Prepare run, Record the start time*/
    time(&(mod->start_time));
    mod->running_state = MODULE_RUNNING;

    MMI_ALOGI("launch module:[%s] testing\n", mod->module);
    if(!pre_check(mod)) {
        MMI_ALOGE("pre check failed for module:[%s]\n", mod->module);
        return NULL;
    }

    /**start test*/
    if(!is_local_module(mod)) {
        /**Query extra command*/
        if(mod->extracmd.is_valid == false)
            send_query(mod);

        send_run_mmi(mod);
    }

    /**start local test and deal with the test result*/
    if(is_local_module(mod)) {
        launch_local_module(mod);
    } else if(is_sensor_module(mod)) {
        launch_module_sensor(mod);
    } else if(is_bluetooth_module(mod)) {
        launch_module_bluetooth(mod);
    } else if(is_wifi_module(mod)) {
        launch_module_wifi(mod);
    } else if(is_gps_module(mod)) {
        launch_module_gps(mod);
    } else if(is_nfc_module(mod)) {
        launch_module_nfc(mod);
    }
    return NULL;
}

int module_exec_ui(module_info * mod) {

    int ret = -1;

    ret = pthread_create(&g_module_tid, NULL, launch_module, (void *) mod);
    if(ret < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create thread(thread id=%lu) for module:[%s] test", g_module_tid, mod->module);

    return 0;
}
#endif

void module_exec_pcba(module_info * mod) {

    if(mod == NULL)
        return;

    /**Record the start time*/
    time(&(mod->start_time));
    mod->running_state = MODULE_RUNNING;
    MMI_ALOGI("start to exec pcba for module:[%s]", mod->module);
    if(is_lcd_module(mod)) {
#ifdef ANDROID
        if(is_lcd_show_image(mod)) {
            launch_lcd_show_image(mod);
        } else if(is_lcd_show_rgb(mod)) {
            launch_lcd_show_rgb(mod);
        } else {
            send_run_pcba(mod);
        }
#endif
    } else if(is_touch_module(mod)) {
#ifdef ANDROID
        if(is_touch_show_rgb(mod))
            launch_touch_show_rgb(mod);
        else
            send_run_pcba(mod);
#endif
    } else {
        send_run_pcba(mod);
    }
}
