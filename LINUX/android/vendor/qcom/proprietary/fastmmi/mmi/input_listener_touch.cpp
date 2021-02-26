/*
 * Copyright (c) 2014,2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "input_listener.h"
#include "mmi.h"
bool input_listener_touch::dispatch_event(input_event ev) {

    static int x_last = -1;
    static int y_last = -1;
    static point_t last_point(-1, -1);
    char str[32] = { 0 };

    int x = x_last;
    int y = y_last;

    point_t cur_point(-1, -1);
    module_info *m = this->get_module();
    layout *lay = this->get_lay();
    cb_t cb = this->get_cb();

    __u16 type = ev.type;
    __u16 code = ev.code;
    __u32 value = ev.value;

    if(type == EV_ABS) {
        if(code == ABS_MT_POSITION_X) {
            x_last = x = value;
        } else if(code == ABS_MT_POSITION_Y) {
            y_last = y = value;
        }
    } else if(type == EV_SYN) {

        point_t cur_point(x, y);

        if(!(last_point == cur_point)) {
            last_point = cur_point;

            /**Delete the touched button*/
            acquire_cur_layout();
            lay->trace.push_back(cur_point);
            lay->delete_btn_by_point(cur_point, KEY_BTN);
            release_cur_layout();
            button *btn = lay->find_button_by_name(KEY_DISPLAY);

            if(btn != NULL) {
                snprintf(str, sizeof(str), "(%d,%d)", cur_point.x, cur_point.y);
                btn->set_text(str);
            }

            invalidate();

            /**Check if it is the last point*/
            if(lay->find_button_by_name(KEY_BTN) == NULL) {
                acquire_cur_layout();
                lay->trace.clear();
                release_cur_layout();
                cb(m);
            }
        }
    }

    return false;
}
