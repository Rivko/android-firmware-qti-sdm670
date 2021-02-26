/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "utils.h"
#include "view.h"
#ifdef ANDROID
bool init_surface(draw_control_t * draw) {

    MMI_ALOGI("start init surface");
    char value[PROPERTY_VALUE_MAX] = { 0 };
    if(draw == NULL) {
        MMI_ALOGE("Invalid param!");
        return false;
    }

    draw->client = new SurfaceComposerClient();
    if(draw->client == NULL) {
        MMI_ALOGE("get surface composer client fail");
        return false;
    }
    sp < IBinder > dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    status_t status = SurfaceComposerClient::getDisplayInfo(dtoken, &draw->dinfo);

    if(status) {
        MMI_ALOGE("get display info fail");
        return false;
    }

    property_get("persist.panel.orientation", value, "0");
    int orient = atoi(value) / 90;

    if(orient == eOrientation90 || orient == eOrientation270) {
        int temp = draw->dinfo.h;

        draw->dinfo.h = draw->dinfo.w;
        draw->dinfo.w = temp;
    }

    draw->pixels_percent_x = draw->dinfo.w / PERCENT_MAX;
    draw->pixels_percent_y = draw->dinfo.h / PERCENT_MAX;

    Rect destRect(draw->dinfo.w, draw->dinfo.h);

    draw->client->setDisplayProjection(dtoken, orient, destRect, destRect);

    // create the native surface
    draw->control =
        draw->client->createSurface(String8(draw->name), draw->dinfo.w, draw->dinfo.h, PIXEL_FORMAT_RGBA_8888);

    if(draw->control == NULL) {
        MMI_ALOGE("get native surface fail");
        return false;
    }

    draw->surface = draw->control->getSurface();

    SurfaceComposerClient::openGlobalTransaction();
    draw->control->setLayer(draw->layer);
    draw->control->show();
    SurfaceComposerClient::closeGlobalTransaction();

    MMI_ALOGD("surface init success: draw name=%s, draw layer=%d, draw control=%p, "
              "surface width:%d, surface heigth=%d",
              draw->name, draw->layer, draw->control.get(), draw->dinfo.w, draw->dinfo.h);
    return true;
}

bool create_surface(draw_control_t * draw) {
    MMI_ALOGI("start create surface");

    if(draw == NULL || draw->client == NULL) {
        MMI_ALOGE("Invalid param!");
        return false;
    }

    draw->control =
        draw->client->createSurface(String8(draw->name), draw->surface_w, draw->surface_h, PIXEL_FORMAT_RGBA_8888);

    if((draw->control == NULL) || (!draw->control->isValid())) {
        MMI_ALOGE("create Surface failed\n");
        return false;
    }
    MMI_ALOGD("create surface success: draw name=%s, draw layer=%d, draw control=%p, "
              "surface width=%d, surface high=%d\n",
              draw->name, draw->layer, draw->control.get(), draw->surface_w, draw->surface_h);

    draw->surface = draw->control->getSurface();

    SurfaceComposerClient::openGlobalTransaction();
    draw->control->setLayer(draw->layer);
    SurfaceComposerClient::closeGlobalTransaction();

    return true;
}

void remove_surface(draw_control_t * draw) {
    SurfaceComposerClient::openGlobalTransaction();
    draw->control->clear();
    SurfaceComposerClient::closeGlobalTransaction();
}

void draw_bitmap(SkBitmap * bitmap, draw_control_t * pdraw) {
    if(bitmap == NULL || pdraw == NULL || pdraw->surface == NULL) {
        MMI_ALOGE("Invalid parameter\n");
        return;
    }

    ANativeWindow_Buffer outBuffer;

    pdraw->surface->lock(&outBuffer, NULL);
    SkBitmap surfaceBitmap;
    ssize_t bpr = outBuffer.stride * bytesPerPixel(outBuffer.format);

    surfaceBitmap.installPixels(SkImageInfo::MakeN32Premul(outBuffer.width, outBuffer.height), outBuffer.bits, bpr);
    SkCanvas surfaceCanvas(surfaceBitmap);

        /**Initial background to Black*/
    surfaceCanvas.clear(SK_ColorBLACK);

    surfaceCanvas.drawBitmap(*bitmap, 0, 0, &pdraw->paint);
    pdraw->surface->unlockAndPost();

}
#endif

bool is_point_in_rect(int x, int y, rect_t * rect) {
    if(rect == NULL)
        return false;

    return (x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h);
}

bool is_point_in_rect(point_t point, rect_t * rect) {
    if(rect == NULL)
        return false;

    return (point.x >= rect->x && point.x <= rect->x + rect->w && point.y >= rect->y && point.y <= rect->y + rect->h);
}
