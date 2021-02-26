/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>
// freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <string>

#include "vaapi.h"

class FreeTypeChar {
 public:
    FreeTypeChar();
    ~FreeTypeChar();

    void printGlyphInText(FT_GlyphSlot slot);

    inline FT_GlyphSlot getGlyph(char c) {
        if (c > ' ' /*32*/ && c < '~' /*126*/) {
            return face[c-33]->glyph;
        }

        return nullptr;
    }

    static const int FontSize = 24;

 private:
    // int loadChar(char c, );

    bool isInited;
    FT_Library  library;

    FT_Face face[94];      /* handle to face object */
};

struct VASimColor {
    VASimColor();
    VASimColor(uint8_t R, uint8_t G, uint8_t B);

    uint8_t Y, U, V;
};

int draw_bounding_box(uint8_t **pixels,
                      unsigned int stride, unsigned int img_height,
                      vaapi_object *pObj, VASimColor color);

int draw_text(uint8_t **pixels,
              unsigned int stride, unsigned int img_height,
              vaapi_position *pos, const char displayName[VAAPI_NAME_LEN],
              VASimColor color);

int draw_line(uint8_t **pixels, int widthStride, int img_height,
        int startX, int startY, int stopX, int stopY, VASimColor color);

vaapi_img_format convert_AVPixelFormat_to_vaapi_img_format(int img_format);
int convert_vaapi_img_format_to_AVPixelFormat(vaapi_img_format img_format);
std::string convert_AVPixelFormat_to_string(int img_format);

int generateSDLQuitEvent(std::string errMsg);
