/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VASIMERROR_H_
#define _VASIMERROR_H_

enum VAMSimErrCode {
    VS_ERROR = -1,
    VS_OK = 0,

    VS_NULLPTR,

    VS_DECODER_INIT_FAIL,
    VS_DECODER_RESIZE_FAIL,

    VS_RENDERER_INIT_FAIL,

    VS_FRAMEQUEUE_INIT_FAIL
};

#define FF_ALLOC_EVENT   (SDL_USEREVENT + 1)
#define FF_QUIT_EVENT    (SDL_USEREVENT + 2)
#define VAM_QUIT         (SDL_USEREVENT + 3)

#endif  // #define _VASIMERROR_H_
