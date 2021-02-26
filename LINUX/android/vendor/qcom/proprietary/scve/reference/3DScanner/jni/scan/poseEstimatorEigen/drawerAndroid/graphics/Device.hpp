/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_DEVICE_HPP
#define GRAPHICS_DEVICE_HPP

#include <cstdint>
#include <functional>
#include <tuple>
#include <vector>

namespace graphics {

enum class Culling : uint8_t;

struct Blending;
class Frame;
class Program;
class Sampler;
class Texture;
class UniformBuffer;
struct Viewport;

class Device {
public:
    enum class Primitive : uint8_t {
        Line,
        Point,
        Triangle
    };

    static int32_t initialize(uint32_t width, uint32_t height);
    static int32_t terminate();

    Device();
    ~Device();

    uint32_t getDefaultWindowFrameHeight();
    uint32_t getDefaultWindowFrameWidth();

    int32_t dispatch(Primitive type, uint32_t count);
    int32_t set(const Blending &state);
    int32_t set(const Culling &mode);
    int32_t set(const Frame &f);
    int32_t set(const Program &p);
    int32_t set(const std::vector<
        std::tuple<const Sampler &, const Texture &>> &units);
    int32_t set(const std::vector<
        std::tuple<const char *, const Texture &>> &units);
    int32_t set(uint32_t count, const UniformBuffer *ubos[]);
    int32_t set(uint32_t count, const Viewport *vps);
    int32_t set(uint32_t count, const Viewport *vps, uint32_t first);
    int32_t setDefaultWindowFrame();
    int32_t toggleBlending(bool turn_on);
    int32_t toggleColorWrite(bool turn_on);
    int32_t toggleColorWrite(bool turn_on_red, bool turn_on_green,
        bool turn_on_blue, bool turn_on_alpha);
    int32_t toggleCulling(bool turn_on);
    int32_t toggleDepthTest(bool turn_on);
    int32_t toggleDepthWrite(bool turn_on);

private:
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
};

}
#endif
