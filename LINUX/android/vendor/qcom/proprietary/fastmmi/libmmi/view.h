/*
 * Copyright (c) 2014,2017, Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __VIEW__
#define __VIEW__
#include "common.h"

/**For surface*/
#if defined(ANDROID)
#include <androidfw/AssetManager.h>
#include <binder/IPCThreadState.h>
#include <ui/PixelFormat.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <ui/DisplayInfo.h>

#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>

#include <SkBitmap.h>
#include <SkCanvas.h>
#include <SkColor.h>
#include <SkPaint.h>
#include <SkTypeface.h>

#include <SkStream.h>
#include <android/native_window.h>
#elif defined(NO_UI)
/*TBD*/
#elif defined(USE_LIBMINIUI)
/*TBD*/
#else
#endif

#include <utils/Atomic.h>
#include <utils/Errors.h>
#include <utils/Log.h>

typedef struct {
    uint32_t hidden;
    uint32_t index;
    rect_t rect;

    module_info *mod;
} item_t;

bool is_point_in_rect(int x, int y, rect_t * rect);
bool is_point_in_rect(point_t point, rect_t * rect);
#if defined(ANDROID)
using namespace android;

typedef struct draw_control {
    char name[32];
    int layer;
    sp < SurfaceComposerClient > client;
    sp < SurfaceControl > control;
    sp < Surface > surface;
    int surface_w;
    int surface_h;
    sem_t sem;
    SkPaint paint;
    DisplayInfo dinfo;
    float pixels_percent_x;
    float pixels_percent_y;
} draw_control_t;

bool init_surface(draw_control_t * draw);
void remove_surface(draw_control_t * draw);
bool create_surface(draw_control_t * draw);
void draw_bitmap(SkBitmap *bitmap,draw_control_t * pdraw);
#endif

#endif
