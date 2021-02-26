/*
 * Copyright (c) 2013-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <hardware/hwcomposer_defs.h>
#include "input.h"
#include "utils.h"
#include "layout.h"
#include "input_listener.h"
#include "module.h"
#include "mmi.h"
#include "interface.h"

static runnable_queue_t g_runnable_queue;
static pthread_mutex_t runnable_mutex;

static pthread_t g_input_waiting_tid;
static pthread_t g_input_handle_tid;

/**Only support one listener at one time*/
static input_listener *g_input_listener = NULL;
static pthread_mutex_t g_listener_mutex;
static union vkey_map g_key_map[MAX_KEYMAP_LINES];
static input_adjust_t g_input_adjust;
static sem_t g_sem_runable;
static unsigned long swbit[BITS_TO_LONGS(SW_CNT)] = { NULL };

static list < point_t > scroll_point_list;
static bool scroll_force_stop_flag = false;

#define FLASH_FREQ_MS 16
#define SHAKING_PIX_X 60

#define SCROLL_ACC (-0.02)

bool get_sw_state(unsigned int key) {

    if(key > SW_MAX)
        return false;
    return ! !test_bit(key, swbit);
}

void register_input_listener(input_listener * listener) {

    if(listener == NULL)
        return;

    pthread_mutex_lock(&g_listener_mutex);
    /**Clean before register */
    if(g_input_listener != NULL)
        delete g_input_listener;

    g_input_listener = listener;
    pthread_mutex_unlock(&g_listener_mutex);
}

void unregister_input_listener() {
    pthread_mutex_lock(&g_listener_mutex);
    /**Clean */
    if(g_input_listener != NULL) {
        delete g_input_listener;

        g_input_listener = NULL;
    }
    pthread_mutex_unlock(&g_listener_mutex);
}

static bool invoke_listener(input_event ev) {
    bool ret = true;

    pthread_mutex_lock(&g_listener_mutex);
    if(g_input_listener != NULL)
        ret = g_input_listener->dispatch_event(ev);
    pthread_mutex_unlock(&g_listener_mutex);

    return ret;
}

static void enqueue_runnable_locked(runnable_queue_t * queue, runnable_t * r) {

    pthread_mutex_lock(&queue->lock);
    queue->queue.push_back(r);
    pthread_mutex_unlock(&queue->lock);
}

static void dequeue_runnable_locked(runnable_queue_t * queue, runnable_t ** r) {
    pthread_mutex_lock(&queue->lock);
    if(!queue->queue.empty()) {
        *r = queue->queue.front();
        queue->queue.pop_front();
    }
    pthread_mutex_unlock(&queue->lock);
}

/**Scroll screen, and delete the points which already used, leave the last one point*/
static touch_xy_min_max_t find_min_max_in_trace(list < point_t > *trace) {
    list < point_t >::iterator iter;
    point_t point;
    touch_xy_min_max_t ret;

    iter = trace->begin();
    point = *iter;

    ret.x_max = ret.x_min = point.x;
    ret.y_max = ret.y_min = point.y;

    for(iter = trace->begin(); iter != trace->end(); iter++) {
        point = *iter;

        if(point.x > ret.x_max) {
            ret.x_max = point.x;
        } else if(point.x < ret.x_min) {
            ret.x_min = point.x;
        }
        if(point.y > ret.y_max) {
            ret.y_max = point.y;
        } else if(point.y < ret.y_min) {
            ret.y_min = point.y;
        }
    }
    return ret;
}

static bool is_screen_scroll(list < point_t > *points) {

    if(points == NULL)
        return false;

    touch_xy_min_max_t tmp = find_min_max_in_trace(points);

    if(tmp.x_max - tmp.x_min <= PRESS_SHAKE_X && tmp.y_max - tmp.y_min <= PRESS_SHAKE_Y) {
        MMI_ALOGW("it is not screen scroll event, just click");
        return false;
    }

    return true;
}
static bool is_redraw(list < point_t > *points) {

    point_t first = points->front();
    point_t back = points->back();
    int time_ms =
        ((back.tv.tv_sec * 1000 * 1000 + back.tv.tv_usec) - (first.tv.tv_sec * 1000 * 1000 + first.tv.tv_usec)) / 1000;

    return time_ms > FLASH_FREQ_MS;
}

static bool is_move_x(list < point_t > *points) {
    point_t first = points->front();
    point_t back = points->back();
    int diff_y = 0, diff_x = 0;

    diff_x = abs(back.x - first.x);
    diff_y = abs(back.y - first.y);

    if(diff_x > (diff_y + 10) && diff_x > SHAKING_PIX_X) {
        MMI_ALOGW("move x in home screen: first.x=%d, back.x=%d, first.y=%d, back.y=%d, points_size=%d\n",
                  first.x, back.x, first.y, back.y, points->size());
        return true;
    }

    return false;
}

static bool is_right(list < point_t > *points) {
    point_t first = points->front();
    point_t back = points->back();

    return back.x - first.x > 0;
}

static void screen_scroll_acc() {

    point_t back, first;
    int step_len = 0, i = 0, nsteps = 0;
    double v0 = 0, speed = 0;
    long time_diff = 0;
    bool ret;

    /*Get the Main layout */
    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL || lay->touch_list.size() < 2) {
        if(NULL == lay)
            MMI_ALOGE("No main layout\n");
        else if(NULL == lay->m_listview)
            MMI_ALOGW("no listview, no need to scroll screen ");
        else
            MMI_ALOGW("Less then two touch list, no need to scroll screen ");

        return;
    }

    /**Get the last two points and calcurate the speed base on that*/
    list < point_t >::reverse_iterator iter;
    iter = lay->touch_list.rbegin();
    back = *iter;
    first = *(++iter);

    time_diff =
        ((back.tv.tv_sec * 1000 * 1000 + back.tv.tv_usec) - (first.tv.tv_sec * 1000 * 1000 + first.tv.tv_usec)) / 1000;
    speed = (double) (back.y - first.y) / time_diff;
    v0 = speed;
    nsteps = abs(speed / (SCROLL_ACC * FLASH_FREQ_MS));
    double scroll_acc = speed > 0 ? SCROLL_ACC : -SCROLL_ACC;

    MMI_ALOGD
        ("Scroll screen ACC info: time diff=%llu us, nsteps=%d, speed=%d, first.x=%d, back.x=%d, first.y=%d, back.y=%d",
         time_diff, nsteps, speed, first.x, back.x, first.y, back.y);
    for(i = 0; i < nsteps; i++) {
        step_len = v0 * FLASH_FREQ_MS + (scroll_acc * FLASH_FREQ_MS * FLASH_FREQ_MS) / 2;
        v0 = v0 + scroll_acc * FLASH_FREQ_MS;
        if(scroll_force_stop_flag)
            break;

        ret = lay->m_listview->scroll(step_len);
        /**If it is the last point*/
        if(!ret)
            break;

        usleep(FLASH_FREQ_MS * 1000);
        invalidate();
    }
}

static void screen_scroll(list < point_t > *points) {

    point_t back, first;
    int nsteps = 0;
    int step_len = 0, i = 0;
    long time_diff = 0;

    if(points == NULL || points->size() < 1) {
        MMI_ALOGW("Less then one point, no need to scroll screen ");
        return;
    }

    /*Check if it is MAIN screen */
    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL) {
        if(NULL == lay)
            MMI_ALOGE("No main layout\n");
        else
            MMI_ALOGW("No listview, no need to scroll screen ");
        return;
    }

    first = points->front();
    back = points->back();
    time_diff =
        ((back.tv.tv_sec * 1000 * 1000 + back.tv.tv_usec) - (first.tv.tv_sec * 1000 * 1000 + first.tv.tv_usec)) / 1000;
    nsteps = time_diff / FLASH_FREQ_MS;
    step_len = (back.y - first.y) / nsteps;

    MMI_ALOGD
        ("Scroll screen info: time diff=%llu us, nsteps=%d, step_len=%d, fisrt.x=%d, back.x=%d, fisrt.y=%d, back.y=%d",
         time_diff, nsteps, step_len, first.x, back.x, first.y, back.y);
    for(i = 0; i < nsteps; i++) {
        lay->m_listview->scroll(step_len);
        invalidate();
    }

    /**store the last point*/
    points->clear();
    points->push_back(back);
}

static void process_touch_up() {

    layout *curlay = acquire_cur_layout();

    if(curlay->touch_list.size() > 0 && !is_screen_scroll(&curlay->touch_list)) {
        point_t tail_point = curlay->touch_list.back();

        MMI_ALOGD("touch list tail point: tail_point.x=%d, tail_point.y=%d, btn_list_size=%d\n",
                  tail_point.x, tail_point.y, curlay->button_list.size());

        /**Button clicked*/
        list < button * >::iterator iter;
        for(iter = curlay->button_list.begin(); iter != curlay->button_list.end(); iter++) {
            button *btn = (button *) (*iter);
            rect_t *rect = btn->get_rect();

            if(btn->get_visibility() && !btn->get_disabled() && is_point_in_rect(tail_point, btn->get_rect())) {
                MMI_ALOGD("button(%s) x=%d, y=%d, width=%d, high=%d\n",
                          btn->get_name(), rect->x, rect->y, rect->w, rect->h);
                if(btn->get_cb() != NULL) {
                    runnable_t *r = new runnable_t;

                    r->cb = btn->get_cb();
                    r->module = curlay->module;
                    enqueue_runnable_locked(&g_runnable_queue, r);
                    sem_post(&g_sem_runable);
                }
                break;
            }
        }

        /**List view item clicked*/
        if(curlay->m_listview != NULL && curlay->m_listview->get_cb() != NULL
           && curlay->m_listview->get_items() != NULL && is_point_in_rect(tail_point, curlay->m_listview->get_rect())) {

            list < item_t * >::iterator iter;
            for(iter = curlay->m_listview->get_items()->begin(); iter != curlay->m_listview->get_items()->end(); iter++) {
                item_t *obj = (item_t *) (*iter);

                if(is_point_in_rect(tail_point, &obj->rect)) {

                    runnable_t *r = new runnable_t;

                    r->cb = curlay->m_listview->get_cb();
                    r->module = obj->mod;
                    enqueue_runnable_locked(&g_runnable_queue, r);
                    MMI_ALOGD("g_sem_runable: addr=%p, val=%d", &g_sem_runable, g_sem_runable);
                    sem_post(&g_sem_runable);
                }
            }
        }
    }

    curlay->touch_list.clear();
    release_cur_layout();
}

static int key_callback(int type, int code, int value) {
    int down = ! !value;

    if(type == EV_KEY) {
        /**Key Down*/
        if(code == KEY_BACK && down)
            MMI_ALOGI("KEYBACK press down, return to main");

        if(code == KEY_POWER && !down) {
            /**Turn on LCD*/
            set_display_power_mode(HWC_POWER_MODE_NORMAL);
        }

        /**Other keys*/
        if(down) {
            MMI_ALOGI("key:%d press down", code);
        } else {
            MMI_ALOGI("key:%d release", code);
        }
    }
    return 0;
}

static int sw_callback(int type, int code, int value) {
    int down = ! !value;

    if(type == EV_SW) {

        if(code > SW_MAX || code < 0) {
            MMI_ALOGE("Code of EV_SW error, code=%d\n", code);
            return -1;
        }

        MMI_ALOGD("EV_SW info: keycode=%d, value=%d\n", code, down);
        if(down)
            set_bit(code, swbit);
        else
            clear_bit(code, swbit);
    }

    return 0;
}

static int touch_callback(input_event * ev) {

    static point_t last_point(-1, -1);
    static int x_last = -1;
    static int y_last = -1;

    int x = x_last;
    int y = y_last;

    if(ev->type == EV_ABS) {
        if(ev->code == ABS_X || ev->code == ABS_MT_POSITION_X) {
            x_last = x = ev->value;
        } else if(ev->code == ABS_Y || ev->code == ABS_MT_POSITION_Y) {
            y_last = y = ev->value;
        } else if(ev->code == ABS_MT_TRACKING_ID && ev->value == 0xffffffff) {

         /**left-right slip*/
            if(is_cur_home_screen() && is_move_x(&scroll_point_list)) {
                MMI_ALOGI("switch home screen\n");
                switch_home_screen(is_right(&scroll_point_list));
            }

            /**clear scroll point list*/
            scroll_point_list.clear();
            scroll_force_stop_flag = false;

            /**Touch up*/
            if(is_main_screen())
                screen_scroll_acc();

            process_touch_up();

        } else if(ev->code == ABS_MT_TRACKING_ID && ev->value != 0xffffffff) {
            scroll_force_stop_flag = true;
        }

    } else if(ev->type == EV_SYN && ev->code == SYN_REPORT) {

        point_t cur_point(x, y);

        if(!(last_point == cur_point)) {
            last_point = cur_point;
            last_point.tv = ev->time;

            layout *curlay = acquire_cur_layout();

            curlay->touch_list.push_back(last_point);
            release_cur_layout();

            /**trace */
            scroll_point_list.push_back(last_point);
            if(curlay->m_listview != NULL && is_redraw(&scroll_point_list) && !is_move_x(&scroll_point_list))
                screen_scroll(&scroll_point_list);
        }
    }

    return 0;
}

/**Handle virtual keys*/
static int vkey_to_keycode(union vkey_map *key_map, int x, int y) {
    int i;

    for(i = 0; i < MAX_KEYMAP_LINES; i++) {
        if((abs((int) (x - key_map[i].map.center_x)) <= (key_map[i].map.width >> 1)) &&
           (abs((int) (y - key_map[i].map.center_y)) <= (key_map[i].map.height >> 1))) {
            return key_map[i].map.key_code;
        }
    }

    return -1;
}

static int hook_vkey(input_event * ev, void *data) {

    static int x_last = -1;
    static int y_last = -1;
    static point_t last_point(-1, -1);
    int key_code = -1;

    int x = x_last;
    int y = y_last;
    point_t cur_point(-1, -1);

    if(ev->type == EV_ABS) {
        if(ev->code == ABS_X || ev->code == ABS_MT_POSITION_X) {
            x_last = x = ev->value;
        } else if(ev->code == ABS_Y || ev->code == ABS_MT_POSITION_Y) {
            y_last = y = ev->value;
        }
    } else if(ev->type == EV_SYN && ev->code == SYN_REPORT) {
        point_t cur_point(x, y);

        if(!(last_point == cur_point)) {
            last_point = cur_point;
            key_code = vkey_to_keycode((union vkey_map *) data, cur_point.x, cur_point.y);
            if(key_code > 0) {
                MMI_ALOGI("Got virtual keycode:%d\n", key_code);
                ev->type = EV_KEY;
                ev->code = key_code;
                ev->value = 1;
            }
        }
    }

    return 0;
}

static void adjust_ev(input_event * ev) {
    if(ev != NULL && ev->type == EV_ABS) {

        if(ev->code == ABS_X || ev->code == ABS_MT_POSITION_X) {
            ev->value = ev->value * g_input_adjust.ratio_x;
        } else if(ev->code == ABS_Y || ev->code == ABS_MT_POSITION_Y) {
            ev->value = ev->value * g_input_adjust.ratio_y;
        }
    }
}

int input_callback(int fd, uint32_t revents, void *data) {
    struct input_event ev;
    int retval;

    retval = ev_get_input(fd, revents, &ev);
    if(retval < 0) {
        MMI_ALOGE("input event get fail\n");
        return -1;
    }

    /**Adjust the value to match LCD resolution*/
    adjust_ev(&ev);

   /**Convert virtual key to KEY code*/
    hook_vkey(&ev, &g_key_map);

    /**Call listener, if return False mean stop here,
  * if return true mean continue process event.
  */
    if(!invoke_listener(ev))
        return 0;

    if(ev.type == EV_KEY) {
        key_callback(ev.type, ev.code, ev.value);
    } else if(ev.type == EV_SW) {
        sw_callback(ev.type, ev.code, ev.value);
    } else if(ev.type == EV_ABS || ev.type == EV_SYN) {
        touch_callback(&ev);
    }

    return 0;
}


static int parse_vkey_map(char *tp_name, union vkey_map *key_map) {
    char path[PATH_MAX];
    char *buffer;
    off_t length;
    int fd_map;
    char *str;
    char *save_ptr1, *save_ptr2;
    char *main_token;
    char *sub_token;
    char *substr;
    int val;
    int i, j;

    strlcpy(path, "/sys/board_properties/virtualkeys.", sizeof(path));
    strlcat(path, tp_name, sizeof(path));
    MMI_ALOGI("virtual keys file: '%s'\n", path);
    fd_map = open(path, O_RDONLY | O_CLOEXEC);
    if(fd_map < 0) {
        MMI_ALOGE("could not open virtual key map file:%s, error=%s\n", path, strerror(errno));
        return -1;
    }

    length = lseek(fd_map, 0, SEEK_END);
    if(length < 0) {
        MMI_ALOGE("could not seek to the end of file:%s, error=%s\n", path, strerror(errno));
        close(fd_map);
        return -1;
    }
    lseek(fd_map, 0, SEEK_SET);
    buffer = (char *) malloc((size_t) length);
    if(buffer == NULL) {
        MMI_ALOGE("malloc %ld bytes failed, error=%s\n", length, strerror(errno));
        close(fd_map);
        return -1;
    }

    if(read(fd_map, buffer, length) <= 0) {
        MMI_ALOGE("read from virtual key map file failed, strerror=%s\n", strerror(errno));
        close(fd_map);
        free(buffer);
        return -1;
    }
    buffer[length - 1] = '\0';
    /* Parse the virtual key map finally */
    for(str = buffer, i = 0;; str = NULL) {
        main_token = strtok_r(str, "\n", &save_ptr1);
        if(main_token == NULL)
            break;
        if(i >= MAX_KEYMAP_LINES) {
            MMI_ALOGW("lines(%d) exceeds max supported soft keys\n", i);
            break;
        }
        /* The comment line starts with '#' */
        if(main_token[0] == '#')
            continue;
        for(j = 0, substr = main_token;; substr = NULL, j++) {
            sub_token = strtok_r(substr, ":", &save_ptr2);
            if(sub_token == NULL)
                break;
            val = strtol(sub_token, NULL, 0);
            key_map[i].vkey_map_value[j] = val;
        }
        i++;
    }

    free(buffer);
    close(fd_map);
    return 0;
}

static int update_ts_info_callback(input_absinfo * abs_x, input_absinfo * abs_y, void *data) {
    if(abs_x == NULL || abs_y == NULL) {
        MMI_ALOGE("Null params \n");
        return -1;
    }
    g_input_adjust.ts_x_max = abs_x->maximum;
    g_input_adjust.ts_x_min = abs_x->minimum;
    g_input_adjust.ts_y_max = abs_y->maximum;
    g_input_adjust.ts_y_min = abs_y->minimum;
    g_input_adjust.lcd_x = get_fb_width();
    g_input_adjust.lcd_y = get_fb_height();
    g_input_adjust.valid = 1;

    if(g_input_adjust.ts_x_max - g_input_adjust.ts_x_min > 0)
        g_input_adjust.ratio_x = g_input_adjust.lcd_x / (g_input_adjust.ts_x_max - g_input_adjust.ts_x_min);
    else
        g_input_adjust.ratio_x = 0;

    if(g_input_adjust.ts_y_max - g_input_adjust.ts_y_min > 0)
        g_input_adjust.ratio_y = g_input_adjust.lcd_y / (g_input_adjust.ts_y_max - g_input_adjust.ts_y_min);
    else
        g_input_adjust.ratio_y = 0;

    MMI_ALOGD("touchscreen resolution:(abs_x=%d,abs_y=%d),lcd resolution(lcd_x=%d,lcd_y=%d), ratio(%f,%f)\n",
              g_input_adjust.ts_x_max, g_input_adjust.ts_y_max, g_input_adjust.lcd_x, g_input_adjust.lcd_y,
              g_input_adjust.ratio_x, g_input_adjust.ratio_y);

    return 0;
}

static int get_ts_resolution() {

    DIR *dir;
    struct dirent *de;
    int fd;
    int i = 0, j = 0;
    bool found = false;
    int max_event_index = 0;
    int index = 0;
    char buffer[80];
    unsigned long keyBitmask[BITS_TO_LONGS(KEY_MAX)];
    unsigned long absBitmask[BITS_TO_LONGS(ABS_MAX)];
    input_absinfo abs_x = { 0, 0, 0 };
    input_absinfo abs_y = { 0, 0, 0 };

    char filepath[256] = {
        0
    };
    dir = opendir("/dev/input");
    if(dir == 0) {
        MMI_ALOGE("dir(%s) open fail, error=%s\n", "/dev/input", strerror(errno));
        return -1;
    }
    while((de = readdir(dir))) {
        if(strncmp(de->d_name, "event", 5))
            continue;
        get_device_index(de->d_name, "event", &index);
        if(index > max_event_index)
            max_event_index = index;
        MMI_ALOGI("input event file: %s\n", de->d_name);
    }

    for(i = 0; i < max_event_index + 1; i++) {
        unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];

        snprintf(filepath, sizeof(filepath), "/dev/input/event%d", i);
        fd = open(filepath, O_RDONLY);
        if(fd < 0)
            continue;
        /* read the evbits of the input device */
        if(ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), ev_bits) < 0) {
            MMI_ALOGE("could not get ev_bits for %s, error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }

        /* TODO: add ability to specify event masks. For now, just assume
         * that only EV_KEY and EV_REL event types are ever needed. */
        if(!test_bit(EV_KEY, ev_bits) && !test_bit(EV_REL, ev_bits)) {
            MMI_ALOGE("could not get EV_KEY for %s, error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }

        /* read the evbits of the input device */
        if(ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBitmask)), keyBitmask) < 0) {
            MMI_ALOGE("could not get keyBitmask for %s, error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }

        if(ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBitmask)), absBitmask) < 0) {
            MMI_ALOGE("could not get absBitmask for %s, error=%s\n", filepath, strerror(errno));
            close(fd);
            continue;
        }
        /*See if this is a touch pad. Is this a new modern multi-touch driver */
        if(test_bit(ABS_MT_POSITION_X, absBitmask)
           && test_bit(ABS_MT_POSITION_Y, absBitmask)) {

            if(ioctl(fd, EVIOCGNAME(sizeof(buffer) - 1), &buffer) < 1) {
                MMI_ALOGE("could not get device name for %s, %s\n", filepath, strerror(errno));
                close(fd);
                continue;
            } else {
                buffer[sizeof(buffer) - 1] = '\0';
                memset(g_key_map, 0, sizeof(g_key_map));
                parse_vkey_map(buffer, g_key_map);
            }
            MMI_ALOGI("input device name: %s\n", buffer);

            if(ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &abs_x) < 0
               || ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &abs_y) < 0) {
                MMI_ALOGE("could not get ABS_MT_POSITION_X for %s, error=%s\n", filepath, strerror(errno));
                close(fd);
                continue;
            }
            found = true;
        }
        if(found) {
            update_ts_info_callback(&abs_x, &abs_y, NULL);
            break;
        }
    }
    closedir(dir);
    if(!found)
        return -1;
    return 0;
}

void init_input() {

    pthread_mutex_init(&runnable_mutex, NULL);
    pthread_mutex_init(&g_listener_mutex, NULL);
    int ret = sem_init(&g_sem_runable, 0, 0);

    if(ret < 0)
        MMI_ALOGW("init input fail, error=%s\n", strerror(errno));

    usleep(1000 * 1000);
    memset(swbit, 0, sizeof(swbit));
    ev_init(input_callback, NULL);
    get_ts_resolution();
    //add for waiting uevent
    monitor_uevent(update_ts_info_callback);
}

void *input_waiting_thread(void *) {
    signal(SIGUSR1, signal_handler);

    MMI_ALOGI("thread(input_waiting_thread) started\n");
    while(1) {
        if(!ev_wait(-1))
            ev_dispatch();
    }
    return NULL;
}

void *input_handle_thread(void *) {
    signal(SIGUSR1, signal_handler);
    runnable_t *r;
    struct timeval current;
    struct timeval last = { 0, 0 };
    double time_diff = 0;

    MMI_ALOGI("input_handle_thread started\n");

    gettimeofday(&last, NULL);
    while(1) {
        sem_wait(&g_sem_runable);
        dequeue_runnable_locked(&g_runnable_queue, &r);
        if((r != NULL) && (r->cb != NULL) && (r->module != NULL)) {
            gettimeofday(&current, NULL);
            if((last.tv_sec != 0) && (last.tv_usec != 0))
                time_diff = ((current.tv_sec - last.tv_sec) * 1000 * 1000 + (current.tv_usec - last.tv_usec));
            last.tv_sec = current.tv_sec;
            last.tv_usec = current.tv_usec;
            if((time_diff < 100 * 1000) && (time_diff > 0)) {
                MMI_ALOGW("Input interval time=%llu, input invalid.\n", time_diff);
                continue;
            } else
                r->cb(r->module);

            MMI_ALOGI("handle input event for module:[%s]\n", r->module);
        }
    }
    return NULL;
}

int create_input_threads() {
    int retval = -1;

    /**Intial input system*/
    init_input();

    retval = pthread_create(&g_input_waiting_tid, NULL, input_waiting_thread, NULL);
    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create input waiting thread(thread id=%lu) for waiting input\n", g_input_waiting_tid);

    retval = pthread_create(&g_input_handle_tid, NULL, input_handle_thread, NULL);
    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create input handle thread(thread id=%lu) for handle input\n", g_input_handle_tid);

    return 0;
}
