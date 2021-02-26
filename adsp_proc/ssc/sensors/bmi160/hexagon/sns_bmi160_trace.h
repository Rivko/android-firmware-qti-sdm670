/*******************************************************************************
 * Copyright (c) 2014, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/


#ifndef __LK_DEBUG_H
#define __LK_DEBUG_H
#include <stdint.h>



#define CONFIG_ENABLE_TP 0

#if !CONFIG_ENABLE_TP

#define INSERT_TRACE_POINT(mod_tag, tnum)
#define INSERT_TRACE_POINT2(mod_tag, tnum1, tnum2)
#define INSERT_TRACE_POINT3(mod_tag, tnum1, tnum2, tnum3)


#define INSERT_TRACE_POINT_T(mod_tag, tnum)
#define INSERT_TRACE_POINT2_T(mod_tag, tnum1, tnum2)
#define INSERT_TRACE_POINT3_T(mod_tag, tnum1, tnum2, tnum3)
#define INSERT_TRACE_POINT4_T(mod_tag, tnum1, tnum2, tnum3, tnum4)

#define INSERT_TRACE_POINT_P(mod_tag, ptr)
#else
#include "lk_debug_ossc.h"
#endif


#endif
