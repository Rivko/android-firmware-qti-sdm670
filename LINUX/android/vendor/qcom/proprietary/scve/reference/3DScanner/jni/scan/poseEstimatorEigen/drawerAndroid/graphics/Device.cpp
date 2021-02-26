/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Device.hpp"

#include <cassert>
#include <map>

#include <pil/CG.hpp>

#include "graphics/Blending.hpp"
#include "graphics/Culling.hpp"
#include "graphics/Frame.hpp"
#include "graphics/Sampler.hpp"
#include "graphics/Program.hpp"
#include "graphics/Texture.hpp"
#include "graphics/UniformBuffer.hpp"
#include "graphics/Viewport.hpp"

static uint32_t gWindowFrameHeight = 0, gWindowFrameWidth = 0;

namespace graphics {

int32_t Device::initialize(uint32_t width, uint32_t height) {
    gWindowFrameWidth = width;
    gWindowFrameHeight = height;

    return 0;
}

int32_t Device::terminate() {
    return 0;
}

Device::Device() {
}

Device::~Device() {
}

uint32_t Device::getDefaultWindowFrameHeight() {
    return gWindowFrameHeight;
}

uint32_t Device::getDefaultWindowFrameWidth() {
    return gWindowFrameWidth;
}

int32_t Device::dispatch(Primitive type, uint32_t count) {
    //attribute-less rendering
    if(Primitive::Point == type)
        glDrawArrays(GL_POINTS, 0, static_cast<int32_t>(count));
    else if(Primitive::Line == type) {
        glLineWidth(5.0f);
        glDrawArrays(GL_LINES, 0, static_cast<int32_t>(count * 2));
    }
    else
        glDrawArrays(GL_TRIANGLES, 0, static_cast<int32_t>(count * 3));

    return 0;
}

int32_t Device::set(const Blending &state) {
    auto convert_eq = [](Blending::Equation eq) -> uint32_t {
        uint32_t result = 0;
        switch(eq) {
            case Blending::Equation::Add: {
                result = GL_FUNC_ADD;
            } break;
            case Blending::Equation::Max: {
                result = GL_MAX;
            } break;
            case Blending::Equation::Min: {
                result = GL_MIN;
            } break;
            case Blending::Equation::ReverseSubtract: {
                result = GL_FUNC_REVERSE_SUBTRACT;
            } break;
            case Blending::Equation::Subtract: {
                result = GL_FUNC_SUBTRACT;
            } break;
        }
        return result;
    };

    auto alpha_eq = convert_eq(state.alpha);
    auto rgb_eq = convert_eq(state.rgb);
    assert(0 != alpha_eq && 0 != rgb_eq);

    auto convert_bf = [](Blending::Factor bf) -> uint32_t {
        uint32_t result = 0;
        switch(bf) {
            case Blending::Factor::One: {
                result = GL_ONE;
            } break;
            case Blending::Factor::OneMinusSinkAlpha: {
                result = GL_ONE_MINUS_DST_ALPHA;
            } break;
            case Blending::Factor::OneMinusSinkRGB: {
                result = GL_ONE_MINUS_DST_COLOR;
            } break;
            case Blending::Factor::OneMinusSourceAlpha: {
                result = GL_ONE_MINUS_SRC_ALPHA;
            } break;
            case Blending::Factor::OneMinusSourceRGB: {
                result = GL_ONE_MINUS_SRC_COLOR;
            } break;
            case Blending::Factor::SinkAlpha: {
                result = GL_DST_ALPHA;
            } break;
            case Blending::Factor::SinkRGB: {
                result = GL_DST_COLOR;
            } break;
            case Blending::Factor::SourceAlpha: {
                result = GL_SRC_ALPHA;
            } break;
            case Blending::Factor::SourceRGB: {
                result = GL_SRC_COLOR;
            } break;
            case Blending::Factor::Zero: {
                result = GL_ZERO;
            } break;
        }
        return result;
    };
    auto src_alpha = convert_bf(state.sourceAlpha);
    auto dst_alpha = convert_bf(state.sinkAlpha);
    auto src_rgb = convert_bf(state.sourceRGB);
    auto dst_rgb = convert_bf(state.sinkRGB);
    assert(0 != src_alpha && 0 != src_rgb && 0 != dst_alpha && 0 != dst_rgb);

    glBlendEquationSeparate(rgb_eq, alpha_eq);
    glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);

    return 0;
}

int32_t Device::set(const Culling &mode) {
    switch(mode) {
        case Culling::Back: {
            glCullFace(GL_BACK);
        } break;
        case Culling::Front: {
            glCullFace(GL_FRONT);
        } break;
    }

    return 0;
}

int32_t Device::set(const Frame &fb) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.getID());

    return 0;
}

int32_t Device::set(const Program &p) {
    glUseProgram(p.getID());

    return 0;
}

int32_t Device::set(const std::vector<
        std::tuple<const Sampler &, const Texture &>> &units) {
    uint32_t idx = 0;
    for(auto &unit : units) {
        glActiveTexture(GL_TEXTURE0 + idx);

        auto &tex = std::get<1>(unit);
        if(!(Texture::Format::External == tex.getFormat()))
            glBindSampler(idx, std::get<0>(unit).getID());

        std::get<1>(unit).bind();

        ++idx;
    }

    return 0;
}

int32_t Device::set(const std::vector<
        std::tuple<const char *, const Texture &>> &units) {
    uint32_t id;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<int32_t *>(&id));
    uint32_t idx = 0;
    for(auto &unit : units) {
        glActiveTexture(GL_TEXTURE0 + idx);
        int32_t loc = glGetUniformLocation(id, std::get<0>(unit));
        glUniform1i(loc, static_cast<int32_t>(idx));

        std::get<1>(unit).bind();

        ++idx;
    }

    return 0;
}

int32_t Device::set(uint32_t count, const UniformBuffer *ubos[]) {
    assert(0 != count && nullptr != ubos);

    //order all binding points
    std::map<uint32_t, std::tuple<uint32_t, uint32_t>> sorted_ubos;
    uint32_t first = 0xFFFFFFFF;
    for(uint32_t i = 0; i < count; ++i) {
        auto &ubo = *ubos[i];
        for(uint32_t j = 0; j < ubo.getNumberBlocks(); ++j) {
            auto binding_point = ubo.getBlockBinding(j);
            assert(sorted_ubos.end() ==
                sorted_ubos.find(binding_point));

            sorted_ubos[binding_point] = std::make_tuple(i, j);
            first = (binding_point < first) ? binding_point : first;
        }
    }

    //set up ranges
    auto max_count = sorted_ubos.size();
    std::unique_ptr<uint32_t[]> buffers(new uint32_t[max_count]);
    std::unique_ptr<intptr_t[]> offsets(new intptr_t[max_count]);
    std::unique_ptr<intptr_t[]> sizes(new intptr_t[max_count]);
    uint32_t n = 0;
    uint32_t bp_previous = first;
    for(auto kv : sorted_ubos) {
        if(kv.first - bp_previous > 1) {
            //consecutive binding points broken
            for(uint32_t i = 0; i < n; ++i)
                glBindBufferRange(GL_UNIFORM_BUFFER, first + i,
                    buffers[i], offsets[i], sizes[i]);
            n = 0;
            first = kv.first;
        }

        auto &ubo = *ubos[std::get<0>(kv.second)];
        auto j = std::get<1>(kv.second);
        buffers[static_cast<uint32_t>(n)] = ubo.getID();
        offsets[static_cast<uint32_t>(n)] = ubo.getBlockOffset(j);
        sizes[static_cast<uint32_t>(n)] =
            static_cast<intptr_t>(ubo.getBlockSize(j));
        ++n;
        bp_previous = kv.first;
    }

    if(0 != n)
        for(uint32_t i = 0; i < n; ++i)
            glBindBufferRange(GL_UNIFORM_BUFFER, first + i,
                buffers[i], offsets[i], sizes[i]);

    return 0;
}

int32_t Device::set(uint32_t count, const Viewport *vps) {
    return set(count, vps, 0);
}

int32_t Device::set(uint32_t count, const Viewport *vps, uint32_t first) {
    assert(0 != count && nullptr != vps && 0 == first);

    glViewport(static_cast<int32_t>(vps[0].originX),
        static_cast<int32_t>(vps[0].originY),
        static_cast<int32_t>(vps[0].width),
        static_cast<int32_t>(vps[0].height));
    glDepthRangef(vps[0].minDepth, vps[0].maxDepth);

    return 0;
}

int32_t Device::setDefaultWindowFrame() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return 0;
}

int32_t Device::toggleBlending(bool turn_on) {
    if(turn_on)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    return 0;
}

int32_t Device::toggleColorWrite(bool turn_on) {
    glColorMask(turn_on, turn_on, turn_on, turn_on);

    return 0;
}

int32_t Device::toggleColorWrite(bool turn_on_red, bool turn_on_green,
        bool turn_on_blue, bool turn_on_alpha) {
    glColorMask(turn_on_red, turn_on_green, turn_on_blue, turn_on_alpha);

    return 0;
}

int32_t Device::toggleCulling(bool turn_on) {
    if(turn_on)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    return 0;
}

int32_t Device::toggleDepthTest(bool turn_on) {
    if(turn_on)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    return 0;
}

int32_t Device::toggleDepthWrite(bool turn_on) {
    glDepthMask(turn_on);

    return 0;
}

}
