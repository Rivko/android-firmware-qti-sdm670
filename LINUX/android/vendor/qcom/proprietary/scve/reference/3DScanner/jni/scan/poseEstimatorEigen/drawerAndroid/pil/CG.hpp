/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef PIL_CG_HPP
#define PIL_CG_HPP

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#define GL_ALL_SHADER_BITS GL_ALL_SHADER_BITS_EXT
#ifndef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT 0x8DA8
#endif
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_TESS_CONTROL_SHADER 0x8E88
#define GL_TESS_EVALUATION_SHADER 0x8E87
#define GL_TIMESTAMP 0x8E28

#define glDeleteProgramPipelines glDeleteProgramPipelinesEXT
#define glGenProgramPipelines glGenProgramPipelinesEXT
#define glUseProgramStages glUseProgramStagesEXT

#endif
