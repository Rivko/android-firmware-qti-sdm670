/*
 * Copyright (c) 2013-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi.h"
#include "utils.h"

#ifdef NO_UI
bool init_draw() {
}
void invalidate() {
}
int get_fb_height() {
    return 320;
}
int get_fb_width() {
    return 480;
}
void set_font_size(const char *font_size) {
}
#else
#include "lang.h"
#include "layout.h"
#include "draw.h"

static bool is_multi_enable = false;
static unordered_map < string, SkBitmap * >image_map;
static draw_control_t g_draw;

draw_control_t *get_draw() {
    return &g_draw;
}

int get_fb_width() {
    return g_draw.dinfo.w;
}
int get_fb_height() {
    return g_draw.dinfo.h;
}

float get_pixels_percent_x() {
    return g_draw.pixels_percent_x;
}

float get_pixels_percent_y() {
    return g_draw.pixels_percent_y;
}

void set_font_size(const char *font_size) {

    draw_control_t *draw = get_draw();

    if(font_size != NULL && draw != NULL)
        draw->paint.setTextSize(SkIntToScalar(atoi(font_size)));
}

#if 0
SkBitmap *load_image(string image_name, bool is_system) {
    string path;

    if(image_map[image_name] != NULL) {
        return image_map[image_name];
    } else {
        if(is_system)
            path = MMI_RES_BASE_DIR + image_name;
        else
            path = string(get_value(KEY_FTM_AP_DIR)) + image_name;

        SkBitmap *orig = new SkBitmap();

        if(orig == NULL)
            return NULL;

        bool skdecode = true;
        sk_sp<SkData> data(SkData::MakeFromFileName(path.c_str()));
        std::unique_ptr<SkCodec> codec(SkCodec::NewFromData(data));
        if (!codec) {
            skdecode = false;
        } else {
            SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
            if (!orig->tryAllocPixels(info)) {
                skdecode = false;
            }
        }

        if (!skdecode) {
            delete orig;
            return NULL;
        }
        image_map[image_name] = orig;
        return orig;
    }
}

SkBitmap *load_image(char *image_name, bool is_system) {

    if(image_name == NULL)
        return NULL;
    else
        return load_image((string) image_name, is_system);
}
#endif

/*
 * To refresh screen
 */
void invalidate() {
    sem_post(&g_draw.sem);
}

bool init_draw() {

    MMI_ALOGI("start draw init!");
    char value[PROPERTY_VALUE_MAX] = { 0 };
    /**Init */
    strlcpy(g_draw.name, "MMI", sizeof(g_draw.name));
    g_draw.layer = 0x70000000;
    init_surface(&g_draw);

#if 0
    g_draw.paint.setTypeface(SkTypeface::CreateFromFile(MMI_RES_FONTS));
#else
    g_draw.paint.setTypeface(SkTypeface::MakeFromFile(MMI_RES_FONTS));
#endif
    g_draw.paint.setTextSize(SkIntToScalar(48));

    MMI_ALOGD("g_draw.layer=0x%x, g_draw.name=%s", g_draw.layer, g_draw.name);
    sem_init(&g_draw.sem, 0, 0);
    return true;
}

static void align_center(rect_t * rect, int text_w, int text_h, point_t * point) {
    if(rect == NULL || point == NULL)
        return;

    if(text_h > rect->h && text_w > rect->w) {
        point->x = rect->x;
        point->y = rect->y + text_h;
    } else if(text_h > rect->h && text_w < rect->w) {
        point->x = rect->x + rect->w / 2 - text_w / 2;
        point->y = rect->y + text_h;
    } else if(text_h < rect->h && text_w > rect->w) {
        point->x = rect->x;
        point->y = rect->y + rect->h / 2 + text_h / 2;
    } else {
        point->x = rect->x + rect->w / 2 - text_w / 2;
        point->y = rect->y + rect->h / 2 + text_h / 2;
    }
}

static int get_text_width(SkPaint * paint, char *str) {
    int iRet = 0;

    if(str != NULL && strlen(str) > 0) {
        int len = strlen(str);
        float widths[len];
        for (int j = 0; j < len; j++) {
            widths[j] = 0;
        }
        paint->getTextWidths(str, len, widths, NULL);
        for(int j = 0; j < len; j++) {
            iRet += (int) ceil(widths[j]);
        }
    }
    return iRet;
}

static int get_text_high(SkPaint * paint) {
    int iRet = 0;

    SkPaint::FontMetrics metrics;

    if(paint != NULL) {
        iRet = paint->getFontMetrics(&metrics) - 10;
    }

    return iRet;
}

/**Draw point*/
static void draw_points(layout * lay, SkCanvas * skcan, draw_control_t * pdraw) {

    if(lay == NULL)
        return;

    list < point_t >::iterator iter;
    for(iter = lay->trace.begin(); iter != lay->trace.end(); iter++) {
        point_t point = (point_t) (*iter);

        pdraw->paint.setColor(0xFFFF0000);
        skcan->drawPoint(point.x, point.y, pdraw->paint);
    }
}

/**draw button*/
static void draw_buttons(layout * lay, SkCanvas * skcan, draw_control_t * pdraw) {

    int text_h = 0, text_w = 0, bitmap_h = 0, bitmap_w = 0;
    SkBitmap *bitmap;
    SkRect srect;

    point_t point;

    list < button * >::iterator iter;
    rect_t half_rect;

    for(iter = lay->button_list.begin(); iter != lay->button_list.end(); iter++) {
        button *btn = (button *) (*iter);

        if(btn->get_visibility()) {
            rect_t *rect = btn->get_rect();
            color_t *btn_color = btn->get_color();

            /**Draw button rect*/
            srect.set(rect->x, rect->y, rect->x + rect->w, rect->y + rect->h);
            pdraw->paint.setARGB(btn_color->a, btn_color->r, btn_color->g, btn_color->b);
            skcan->drawRect(srect, pdraw->paint);

            text_w = get_text_width(&pdraw->paint, btn->get_text());
            text_h = get_text_high(&pdraw->paint);

            /**Draw button icon if have*/
            if(strlen(btn->get_image()) != 0) {
                //bitmap = load_image(btn->get_image(), true);
                //bitmap_w = bitmap->width();
                //bitmap_h = bitmap->height();
                /**Up half rect*/
                half_rect.x = rect->x;
                half_rect.y = rect->y;
                half_rect.w = rect->w;
                half_rect.h = rect->h / 2;

                //align_center(&half_rect, bitmap_w, bitmap_h, &point);
                //skcan->drawBitmap(*bitmap, point.x, point.y - bitmap_h, &pdraw->paint);

                /**Down half rect*/
                half_rect.y = rect->y + rect->h / 2;
                align_center(&half_rect, text_w, text_h, &point);
            } else {
                align_center(rect, text_w, text_h, &point);
            }

            /**Draw button text*/
            pdraw->paint.setColor(SK_ColorWHITE);
            skcan->drawText(btn->get_text(), strlen(btn->get_text()), point.x, point.y, pdraw->paint);

        }
    }
}

void draw_textviews(layout * lay, SkCanvas * skcan, draw_control_t * pdraw) {
    char tmp[1024] = { 0 };
    int32_t last_text_x = 0;
    int32_t last_text_y = 0;
    int text_h = 0, text_w = 0;

    if(lay == NULL)
        return;

    text_h = get_text_high(&pdraw->paint);
    list < textview * >::iterator iter;
    for(iter = lay->textview_list.begin(); iter != lay->textview_list.end(); iter++) {
        textview *tv = (textview *) (*iter);
        rect_t *rect = tv->get_rect();

        last_text_x = rect->x;
        last_text_y = rect->y;

        char *p = tv->get_text();
        int line_num = count_char(p, '\n');
        int max_line = rect->h / (2 * text_h);
        int pos = 0;

        /**set point to the start line*/
        if(line_num > max_line) {
            pos = get_pos(p, line_num - max_line);
            p += pos;
        }

        char *ptr;

        while(*p != '\0') {
            ptr = tmp;

            while(*p != '\n' && *p != '\0') {
                *ptr++ = *p++;
            }

            if(*p == '\n')
                p++;

            *ptr = '\0';
             /**Only check Y not outof rect, X will cut off String*/
            if((last_text_y + text_h) < (rect->y + rect->h)) {
                skcan->drawText(tmp, strlen(tmp), last_text_x, last_text_y, pdraw->paint);
            }

            /*move to next line */
            last_text_y = last_text_y + text_h + text_h / 2;
        }
    }
}

void draw_listviews(layout * lay, SkCanvas * skcan, draw_control_t * pdraw) {

    int ret = -1;
    int text_h = -1, text_w = -1;
    point_t point;
    SkBitmap *bitmap = NULL, *bitmap_fail = NULL, *bitmap_pass = NULL;

    int bitmap_h = 0, bitmap_w = 0;
    SkRect srect;
    char text[128];

    if(lay == NULL || lay->m_listview == NULL)
        return;

    /**Load images*/
    //bitmap_pass = load_image("pass.png", true);
    //bitmap_fail = load_image("fail.png", true);
    //bitmap_w = bitmap_pass->width();
    //bitmap_h = bitmap_pass->height();

    listview *container = lay->m_listview;
    rect_t *rect = container->get_rect();
    int h = rect->h / container->get_item_per_page();

    if(container->get_items() == NULL)
        return;

    /**Indicator*/
    double radio = container->get_item_per_page() / (double) container->get_item_num();
    int indicator_h = radio * rect->h;
    int indicator_w = 8;
    int indicator_y = rect->y - radio * (container->get_items()->front()->rect.y - rect->y);
    int indicator_x = rect->x + rect->w - indicator_w;

    list < item_t * >::iterator iter;
    for(iter = container->get_items()->begin(); iter != container->get_items()->end(); iter++) {

        item_t *obj = (item_t *) (*iter);

        if(obj->hidden == 1 || !is_point_in_rect(obj->rect.x, obj->rect.y, rect)
           || !is_point_in_rect(obj->rect.x, obj->rect.y + obj->rect.h, rect)) {

            continue;
        }

#if 0
        if(obj->mod->result == FAILED) {
            bitmap = bitmap_fail;
        } else if(obj->mod->result == SUCCESS) {
            bitmap = bitmap_pass;
        }
#endif

        /**Draw rect*/
        rect_t *rect = &obj->rect;

        srect.set(rect->x, rect->y, rect->x + rect->w, rect->y + rect->h);
        pdraw->paint.setColor(0xFF858585);
        skcan->drawLine(rect->x, rect->y + rect->h, rect->x + rect->w, rect->y + rect->h, pdraw->paint);

        if(obj->mod->running_state == MODULE_RUNNING) {
            pdraw->paint.setColor(0xFFFF0000);
            skcan->drawRect(srect, pdraw->paint);
        }

#if 0
        /**Draw bitmap*/
        if(obj->mod->result == FAILED || obj->mod->result == SUCCESS) {
            align_center(rect, rect->w - 10, bitmap_h, &point);
            skcan->drawBitmap(*bitmap, point.x, point.y - bitmap_h, &pdraw->paint);
        }
#endif

        /**Draw text*/
        snprintf(text, sizeof(text), "%d %s", obj->index, get_string(obj->mod->config_list[KEY_DISPALY_NAME]));
        pdraw->paint.setColor(SK_ColorWHITE);
        text_h = get_text_high(&pdraw->paint);
#if 0
        align_center(rect, rect->w - 2 * bitmap_w, text_h, &point);
#else
        align_center(rect, rect->w, text_h, &point);
#endif
        skcan->drawText(text, strlen(text), point.x, point.y, pdraw->paint);
    }

    /**Draw indicator*/
    srect.set(indicator_x, indicator_y, indicator_x + indicator_w, indicator_y + indicator_h);
    pdraw->paint.setColor(SK_ColorWHITE);
    skcan->drawRect(srect, pdraw->paint);
}

void *draw_thread(void *) {

    signal(SIGUSR1, signal_handler);
    draw_control_t *draw = get_draw();
    ANativeWindow_Buffer outBuffer;

    MMI_ALOGI("thread(draw_thread) started\n");
    while(1) {

        sem_wait(&g_draw.sem);
        layout *lay = acquire_cur_layout();

        draw->surface->lock(&outBuffer, NULL);
        SkBitmap surfaceBitmap;
        ssize_t bpr = outBuffer.stride * bytesPerPixel(outBuffer.format);

        surfaceBitmap.installPixels(SkImageInfo::MakeN32Premul(outBuffer.width, outBuffer.height), outBuffer.bits, bpr);
        SkCanvas surfaceCanvas(surfaceBitmap);

        /**Initial background to Black*/
        surfaceCanvas.clear(SK_ColorBLACK);

        draw_buttons(lay, &surfaceCanvas, draw);
        draw_textviews(lay, &surfaceCanvas, draw);
        draw_listviews(lay, &surfaceCanvas, draw);
        draw_points(lay, &surfaceCanvas, draw);
        draw->surface->unlockAndPost();

        release_cur_layout();

    }
    return NULL;
}
#endif
