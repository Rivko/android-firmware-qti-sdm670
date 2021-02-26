/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "common.h"
#include "module.h"
#include "utils.h"
#include "draw.h"
#include "input.h"
#include "mmi.h"

static sem_t g_local_module_sem;
static pthread_t g_module_tid;
static bool touch_completed = false;
static draw_control_t lcd_draw;

static key_map_t key_map[] = {
    {KEYPAD_STR_HOME, NULL, KEY_HOME, false, false},
    {KEYPAD_STR_MENU, NULL, KEY_MENU, false, false},
    {KEYPAD_STR_BACK, NULL, KEY_BACK, false, false},
    {KEYPAD_STR_VOLUMEDOWN, NULL, KEY_VOLUMEDOWN, false, false},
    {KEYPAD_STR_VOLUMEUP, NULL, KEY_VOLUMEUP, false, false},
    {KEYPAD_STR_POWER, NULL, KEY_POWER, false, false},
    {KEYPAD_STR_SNAPSHOT, NULL, KEY_CAMERA_SNAPSHOT, false, false},
    {KEYPAD_STR_HEADPHONE, NULL, SW_HEADPHONE_INSERT, false, false},
    {KEYPAD_STR_MICROPHONE, NULL, SW_MICROPHONE_INSERT, false, false},
    {KEYPAD_STR_HANGUP, NULL, KEY_MEDIA, false, false},
    {KEYPAD_STR_0, NULL, KEY_0, false, false},
    {KEYPAD_STR_1, NULL, KEY_1, false, false},
    {KEYPAD_STR_2, NULL, KEY_2, false, false},
    {KEYPAD_STR_3, NULL, KEY_3, false, false},
    {KEYPAD_STR_4, NULL, KEY_4, false, false},
    {KEYPAD_STR_5, NULL, KEY_5, false, false},
    {KEYPAD_STR_6, NULL, KEY_6, false, false},
    {KEYPAD_STR_7, NULL, KEY_7, false, false},
    {KEYPAD_STR_8, NULL, KEY_8, false, false},
    {KEYPAD_STR_9, NULL, KEY_9, false, false},
    {KEYPAD_STR_NUMERIC_POUND, NULL, KEY_NUMERIC_POUND, false, false},
    {KEYPAD_STR_STAR, NULL, KEY_KPASTERISK, false, false},
    {KEYPAD_STR_STAR, NULL, KEY_NUMERIC_STAR, false, false},
    {KEYPAD_STR_LEFTSHIFT, NULL, KEY_LEFTSHIFT, false, false},
    {KEYPAD_STR_RIGHTSHIFT, NULL, KEY_RIGHTSHIFT, false, false},
    {KEYPAD_STR_LEFT, NULL, KEY_LEFT, false, false},
    {KEYPAD_STR_RIGHT, NULL, KEY_RIGHT, false, false},
    {KEYPAD_STR_OK, NULL, KEY_OK, false, false},

};

/**Change the operation permission for PASS,FAIL button:
*  enable = 1: enable operation for the buttons(both pass and fail button).
*  enable = 0: disable operation for the buttons(both pass and fail button).
**/
static void btn_enable(layout * lay, bool enable) {

    if(lay == NULL) {
        MMI_ALOGE("No layout\n");
        return;
    }

    button *pass_btn = lay->find_button_by_name(KEY_PASS);
    button *fail_btn = lay->find_button_by_name(KEY_FAIL);

    if(pass_btn != NULL) {
        if(enable)
            pass_btn->set_disabled(false);
        else
            pass_btn->set_disabled(true);
    } else {
        MMI_ALOGW("button(%s) not find\n", KEY_PASS);
    }

    if(fail_btn != NULL) {
        if(enable)
            fail_btn->set_disabled(false);
        else
            fail_btn->set_disabled(true);
    } else {
        MMI_ALOGW("button(%s) not find\n", KEY_FAIL);
    }
}

/**Change the visibility for PASS,FAIL,DISPLAY button:
*  visible = 1: dispaly button visible,pass and fail button invisible.
*  visible = 0: pass and fail button visible, display button invisible.
**/
static void change_visibility(layout * lay, bool visible) {

    if(lay == NULL) {
        MMI_ALOGE("No layout\n");
        return;
    }

    button *pass_btn = lay->find_button_by_name(KEY_PASS);
    button *fail_btn = lay->find_button_by_name(KEY_FAIL);
    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(pass_btn != NULL) {
        pass_btn->set_visibility(!visible);
    } else {
        MMI_ALOGW("button(%s) not find\n", KEY_PASS);
    }

    if(fail_btn != NULL) {
        fail_btn->set_visibility(!visible);
    } else {
        MMI_ALOGW("button(%s) not find\n", KEY_FAIL);
    }

    if(display_btn != NULL) {
        display_btn->set_visibility(visible);
    } else {
        MMI_ALOGW("button(%s) not find\n", KEY_DISPLAY);
    }
}
static void *process_auto_exit(void *m) {
    process_exit(m, SUCCESS);
    return NULL;
}

void do_auto_exit(void *m) {
    MMI_ALOGI("module:[%s] test auto exit", !m ? "null" : ((module_info *) m)->module);
    int ret = -1;
    pthread_t pid;

    ret = pthread_create(&pid, NULL, process_auto_exit, m);
    if(ret < 0) {
        MMI_ALOGE("create thread fail, error=%s\n", strerror(errno));
    }
}

/**========================================*/
/**LCD test*/
void do_next(void *m) {
    sem_post(&g_local_module_sem);
}


void launch_lcd_show_image(module_info * mod) {

    unordered_map < string, string > params;

    if(mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return;
    }

    parse_parameter(mod->config_list["parameter"], params);
#if 0
    SkBitmap *bitmap = load_image(params["filename"], false);

    if(bitmap == NULL) {
        MMI_ALOGE("No image file specified");
        return;
    }
#endif
    MMI_ALOGI("image file(%s) loading success", params["filename"].c_str());

    strlcpy(lcd_draw.name, "LCD", sizeof(lcd_draw.name));
    lcd_draw.layer = 0x71000000;
    init_surface(&lcd_draw);

    MMI_ALOGI("draw image file(%s)", params["filename"].c_str());
    //draw_bitmap(bitmap, &lcd_draw);
    MMI_ALOGI("module:[%s] test finished", mod->module);
}

void stop_lcd_show_image() {
    MMI_ALOGI("stop show image");
    remove_surface(&lcd_draw);
}

void launch_lcd_show_rgb(module_info * mod) {

    unordered_map < string, string > params;
    int color = 0x00000000;

    /*Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }
    /**switch layout to pcba and initial the module*/
    switch_cur_layout_locked(lay, mod);

    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(display_btn == NULL) {
        MMI_ALOGE("Not find button(%s) in the layout(%s)\n", KEY_DISPLAY, lay->get_layout_path());
        return;
    }

    parse_parameter(mod->config_list["parameter"], params);
    if(!strcmp(params["color"].c_str(), "red")) {
        color = COLOR_RED;
        MMI_ALOGI("button(%s) display red\n", KEY_DISPLAY);
    } else if(!strcmp(params["color"].c_str(), "blue")) {
        color = COLOR_BLUE;
        MMI_ALOGI("button(%s) display blue\n", KEY_DISPLAY);
    } else if(!strcmp(params["color"].c_str(), "green")) {
        color = COLOR_GREEN;
        MMI_ALOGI("button(%s) display green\n", KEY_DISPLAY);
    } else if(!strcmp(params["color"].c_str(), "black")) {
        color = COLOR_BLACK;
        MMI_ALOGI("button(%s) display black\n", KEY_DISPLAY);
    } else {
        MMI_ALOGE("Invalid parames <color>, color=%s", MMI_STR(params["color"].c_str()));
    }

    change_visibility(lay, true);

    display_btn->set_color(color);
    invalidate();

    MMI_ALOGI("module:[%s] test finished", mod->module);
}

static void launch_local_lcd(module_info * mod) {
    /*Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(display_btn == NULL) {
        MMI_ALOGE("Not find button(%s) in the layout(%s)\n", KEY_DISPLAY, lay->get_layout_path());
        return;
    }

    change_visibility(lay, true);

    display_btn->set_color(COLOR_RED);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("RED test finished for module:[%s]\n", mod->module);

    display_btn->set_color(COLOR_GREEN);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("GREEN test finished for module:[%s]\n", mod->module);

    display_btn->set_color(COLOR_BLUE);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("BLUE test finished for module:[%s]\n", mod->module);

    display_btn->set_color(COLOR_WHITE);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("WHITE test finished for module:[%s]\n", mod->module);

    display_btn->set_color(COLOR_BLACK);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("BLACK test finished for module:[%s]\n", mod->module);

    change_visibility(lay, false);

    invalidate();
    MMI_ALOGI("module:[%s] test finished\n", mod->module);
}

/**KEY test*/
void key_down_cb(void *modev) {
    unsigned int i = 0;
    bool ret = true;

    if(modev == NULL)
        return;

    mod_ev_t *pmodev = (mod_ev_t *) modev;

    MMI_ALOGI("input key code=%d", pmodev->ev->code);

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(pmodev->ev->code == key_map[i].key_code) {
            key_map[i].tested = true;
            MMI_ALOGI("key(key_name=%s) test pass", key_map[i].key_name);
        }
    }

   /**Check if completed*/
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist)
            ret &= key_map[i].tested;
    }

    if(ret) {
        MMI_ALOGI("module:[%s] test pass", pmodev->mod->module);
        do_auto_exit(pmodev->mod);
    }

}

static void launch_local_key(module_info * mod) {
    unsigned int i = 0;

    unordered_map < string, string > params;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    parse_parameter(mod->config_list["parameter"], params);
    /**Initial status*/
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(strstr(params["keys"].c_str(), key_map[i].key_name) != NULL) {
            key_map[i].exist = true;
            key_map[i].tested = false;
            MMI_ALOGI("key(key_name=%s) need to test", key_map[i].key_name);
        } else {
            key_map[i].exist = false;
            key_map[i].tested = false;
        }
    }

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist) {
            lay->set_button_color_by_name(key_map[i].key_name, COLOR_KEY_DEFAULT);
        } else {
            key_map[i].key_btn = lay->find_button_by_name(key_map[i].key_name);
            if(key_map[i].key_btn != NULL)
                key_map[i].key_btn->set_visibility(false);
        }
    }

    invalidate();

    /**Register call back */
    input_listener *lis = new input_listener_key(mod, lay, key_down_cb);

    if(lis != NULL)
        register_input_listener(lis);
}

/**TOUCH test*/
void launch_touch_show_rgb(module_info * mod) {
    /*Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    /**switch layout to pcba and initial the module*/
    switch_cur_layout_locked(lay, mod);

    button *display_btn = lay->find_button_by_name(KEY_DISPLAY);

    if(display_btn == NULL) {
        MMI_ALOGE("Not find button(%s) in the layout(%s)\n", KEY_DISPLAY, lay->get_layout_path());
        return;
    }

    change_visibility(lay, true);

    display_btn->set_color(COLOR_RED);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("RED test finished for module:[%s]\n", mod->module);

    display_btn->set_color(COLOR_GREEN);
    invalidate();
    sem_wait(&g_local_module_sem);
    MMI_ALOGI("GREEN test finished for module:[%s]\n", mod->module);

    display_btn->set_color(COLOR_BLUE);
    invalidate();
    MMI_ALOGI("BLUE test finished for module:[%s]\n", mod->module);

    MMI_ALOGI("module:[%s] test finished\n", mod->module);
}


void touch_complete_cb(void *m) {

    if(!touch_completed) {
        MMI_ALOGI("Touch callback invoke");
        sleep(1);
        do_auto_exit(m);
        touch_completed = true;
    }
}

static void touch_mode_edges(layout * lay) {
    int w = get_fb_width();
    int h = get_fb_height();
    int row = 20 - 1;
    int col = 10 - 1;
    rect_t rect;
    button *btn;

    for(int i = 0; i < row; i += 2) {
        rect.x = 0;
        rect.y = i * h / row;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
    for(int i = 0; i < row; i += 2) {
        rect.x = 8 * w / col;
        rect.y = i * h / row;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
    for(int i = 0; i < col; i += 2) {
        rect.x = i * w / col;
        rect.y = 0;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
    for(int i = 0; i < col; i += 2) {
        rect.x = i * w / col;
        rect.y = 18 * h / row;
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
}

static void set_btn(layout * lay, int w, int h, int x, int y) {
    int row = 20 - 1;
    int col = 10 - 1;

    rect_t rect;
    button *btn;

    if((x == 0) || (y == 0) || (x == w) || (y == h)) {
        if((x == 0) && (y == 0)) {
            rect.x = 0;
            rect.y = 0;
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
        if((x == 0) && (y == h)) {
            rect.x = 0;
            rect.y = h - (h / row);
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
        if((x == w) && (y == 0)) {
            rect.x = w - (w / col);
            rect.y = 0;
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);

        }
        if((x == w) && (y == h)) {
            rect.x = w - (w / col);
            rect.y = h - (h / row);
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);

            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
    } else {
        rect.x = x - (w / (2 * col));
        rect.y = y - (h / (2 * row));
        rect.w = w / col;
        rect.h = h / row;

        btn = new button(KEY_BTN, rect, "", NULL);

        btn->set_color(0, 125, 125, 255);
        lay->add_button_locked(btn);
    }
}

static void touch_mode_diagonal(layout * lay) {
    int w = get_fb_width();
    int h = get_fb_height();

    for(int i = 0; i < 9; i++) {
        set_btn(lay, w, h, w * i / 8, h * i / 8);
        set_btn(lay, w, h, w * i / 8, h * (8 - i) / 8);
    }
}


static void touch_mode_full(layout * lay) {
    int w = get_fb_width();
    int h = get_fb_width();
    int row = 20 - 1;
    int col = 10 - 1;
    rect_t rect;
    button *btn;

    for(int i = 0; i < row; i += 2) {
        for(int j = 0; j < col; j += 2) {
            rect.x = j * w / col;
            rect.y = i * h / row;
            rect.w = w / col;
            rect.h = h / row;

            btn = new button(KEY_BTN, rect, "", NULL);
            btn->set_color(0, 125, 125, 255);
            lay->add_button_locked(btn);
        }
    }
}

static void launch_local_touch(module_info * mod) {

    if(mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return;
    }
    unordered_map < string, string > params;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    change_visibility(lay, true);

    /**Register call back */
    touch_completed = false;
    input_listener *lis = new input_listener_touch(mod, lay, touch_complete_cb);

    if(lis != NULL)
        register_input_listener(lis);

   /**build touch mode*/
    MMI_ALOGE("touch mode: %s for module:[%s]\n", mod->config_list["parameter"].c_str(), mod->module);

    parse_parameter(mod->config_list["parameter"], params);
    if(!strcmp(params["mode"].c_str(), "edges")) {
        touch_mode_edges(lay);
    } else if(!strcmp(params["mode"].c_str(), "full")) {
        touch_mode_full(lay);
    } else if(!strcmp(params["mode"].c_str(), "diagonal")) {
        touch_mode_edges(lay);
        touch_mode_diagonal(lay);
    } else {
        MMI_ALOGE("Invalid params <mode>, mode=%s\n", params["mode"].c_str());
    }
    invalidate();
}

static void launch_local_headset(module_info * mod) {
    unsigned int i = 0;

    unordered_map < string, string > params;

    /**Get the layout */
    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout for module:[%s]\n", mod->module);
        return;
    }

    parse_parameter(mod->config_list["parameter"], params);
    /**Initial status*/
    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(strstr(params["keys"].c_str(), key_map[i].key_name) != NULL) {
            key_map[i].exist = true;
            key_map[i].tested = false;
            MMI_ALOGI("headset key(key_name=%s) need to test", key_map[i].key_name);

        } else {
            key_map[i].exist = false;
            key_map[i].tested = false;
        }
    }

    for(i = 0; i < (sizeof(key_map) / sizeof(key_map_t)); i++) {
        if(key_map[i].exist) {
            lay->set_button_color_by_name(key_map[i].key_name, COLOR_KEY_DEFAULT);
        } else {
            key_map[i].key_btn = lay->find_button_by_name(key_map[i].key_name);
            if(key_map[i].key_btn != NULL)
                key_map[i].key_btn->set_visibility(false);
        }
    }

    invalidate();

      /**Register call back */
    input_listener *lis = new input_listener_key(mod, lay, key_down_cb);

    if(lis != NULL)
        register_input_listener(lis);
}

/**Handle local modules*/
void launch_local_module(module_info * mod) {
    /**Init sem*/
    sem_init(&g_local_module_sem, 0, 0);

    if(is_lcd_module(mod)) {
        launch_local_lcd(mod);
    } else if(is_key_module(mod)) {
        launch_local_key(mod);
    } else if(is_keypad_module(mod)) {
        launch_local_key(mod);
    } else if(is_touch_module(mod)) {
        launch_local_touch(mod);
    } else if(is_headset_key_module(mod)) {
        launch_local_headset(mod);
    }
    sem_close(&g_local_module_sem);
}
