/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "PowerOffHandler"

#include <stdint.h>
#include <sys/types.h>
#include <math.h>
#include <fcntl.h>
#include <utils/misc.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>

#include <cutils/properties.h>

#include <androidfw/AssetManager.h>
#include <binder/IPCThreadState.h>
#include <utils/Atomic.h>
#include <utils/Errors.h>
#include <utils/Log.h>

#include <ui/PixelFormat.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <ui/DisplayInfo.h>

#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>

// TODO: Fix Skia.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImageDecoder.h>
#pragma GCC diagnostic pop

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/eglext.h>

#include <media/AudioSystem.h>
#include <media/mediaplayer.h>
#include <media/IMediaHTTPService.h>

#include <private/regionalization/Environment.h>

#include "PowerOffHandler.h"

#define OEM_SHUTDOWN_ANIMATION_FILE "/oem/media/shutdownanimation.zip"
#define SYSTEM_SHUTDOWN_ANIMATION_FILE "/system/media/shutdownanimation.zip"
#define SYSTEM_ENCRYPTED_SHUTDOWN_ANIMATION_FILE "/system/media/shutdownanimation-encrypted.zip"

#define OEM_SHUTDOWN_MUSIC_FILE "/oem/media/shutdown.wav"
#define SYSTEM_SHUTDOWN_MUSIC_FILE "/system/media/shutdown.wav"

#define EXIT_PROP_NAME "service.poweroffhandler.exit"

namespace android {

static const int ANIM_ENTRY_NAME_MAX = 256;

// ---------------------------------------------------------------------------

static pthread_mutex_t mp_lock;
static pthread_cond_t mp_cond;
static bool isMPlayerPrepared = false;
static bool isMPlayerCompleted = false;

static void* playMusic(void* arg);

class MPlayerListener: public MediaPlayerListener {
    void notify(int msg, int ext1, int ext2, const Parcel *obj) {
        ALOGV("message received msg=%d, ext1=%d, ext2=%d, obj=%x", msg, ext1,
                ext2, obj);
        switch (msg) {
        case MEDIA_NOP: // interface test message
            break;
        case MEDIA_PREPARED:
            pthread_mutex_lock(&mp_lock);
            isMPlayerPrepared = true;
            pthread_cond_signal(&mp_cond);
            pthread_mutex_unlock(&mp_lock);
            break;
        case MEDIA_PLAYBACK_COMPLETE:
            pthread_mutex_lock(&mp_lock);
            isMPlayerCompleted = true;
            pthread_cond_signal(&mp_cond);
            pthread_mutex_unlock(&mp_lock);
            break;
        default:
            break;
        }
    }
};

PowerOffHandler::PowerOffHandler() :
        Thread(false), mZip(NULL) {
    mSession = new SurfaceComposerClient();
}

PowerOffHandler::~PowerOffHandler() {
    if (mZip != NULL) {
        delete mZip;
    }
}

void PowerOffHandler::onFirstRef() {
    status_t err = mSession->linkToComposerDeath(this);
    ALOGE_IF(err, "linkToComposerDeath failed (%s) ", strerror(-err));
    if (err == NO_ERROR) {
        //run("BootAnimation", PRIORITY_DISPLAY);
        run("BootAnimation", ANDROID_PRIORITY_URGENT_DISPLAY);
    }
}

sp<SurfaceComposerClient> PowerOffHandler::session() const {
    return mSession;
}

void PowerOffHandler::binderDied(const wp<IBinder>&) {
    // woah, surfaceflinger died!
    ALOGD("SurfaceFlinger died, exiting...");

    // calling requestExit() is not enough here because the Surface code
    // might be blocked on a condition variable that will never be updated.
    kill(getpid(), SIGKILL);
    requestExit();
}

status_t PowerOffHandler::initTexture(Texture* texture, AssetManager& assets,
        const char* name) {
    Asset* asset = assets.open(name, Asset::ACCESS_BUFFER);
    if (asset == NULL)
        return NO_INIT;
    SkBitmap bitmap;
    SkImageDecoder::DecodeMemory(asset->getBuffer(false), asset->getLength(),
            &bitmap, kUnknown_SkColorType, SkImageDecoder::kDecodePixels_Mode);
    asset->close();
    delete asset;

    // ensure we can call getPixels(). No need to call unlock, since the
    // bitmap will go out of scope when we return from this method.
    bitmap.lockPixels();

    const int w = bitmap.width();
    const int h = bitmap.height();
    const void* p = bitmap.getPixels();

    GLint crop[4] = { 0, h, w, -h };
    texture->w = w;
    texture->h = h;

    glGenTextures(1, &texture->name);
    glBindTexture(GL_TEXTURE_2D, texture->name);

    switch (bitmap.colorType()) {
    case kAlpha_8_SkColorType:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA,
                GL_UNSIGNED_BYTE, p);
        break;
    case kARGB_4444_SkColorType:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                GL_UNSIGNED_SHORT_4_4_4_4, p);
        break;
    case kN32_SkColorType:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, p);
        break;
    case kRGB_565_SkColorType:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                GL_UNSIGNED_SHORT_5_6_5, p);
        break;
    default:
        break;
    }

    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return NO_ERROR;
}

status_t PowerOffHandler::initTexture(const Animation::Frame& frame) {
    SkBitmap bitmap;
    SkMemoryStream stream(frame.map->getDataPtr(), frame.map->getDataLength());
    SkImageDecoder* codec = SkImageDecoder::Factory(&stream);
    if (codec != NULL) {
        codec->setDitherImage(false);
        codec->decode(&stream, &bitmap, kN32_SkColorType,
                SkImageDecoder::kDecodePixels_Mode);
        delete codec;
    }

    // FileMap memory is never released until application exit.
    // Release it now as the texture is already loaded and the memory used for
    // the packed resource can be released.
    delete frame.map;

    // ensure we can call getPixels(). No need to call unlock, since the
    // bitmap will go out of scope when we return from this method.
    bitmap.lockPixels();

    const int w = bitmap.width();
    const int h = bitmap.height();
    const void* p = bitmap.getPixels();

    GLint crop[4] = { 0, h, w, -h };
    int tw = 1 << (31 - __builtin_clz(w));
    int th = 1 << (31 - __builtin_clz(h));
    if (tw < w)
        tw <<= 1;
    if (th < h)
        th <<= 1;

    switch (bitmap.colorType()) {
    case kN32_SkColorType:
        if (tw != w || th != h) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, 0);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA,
                    GL_UNSIGNED_BYTE, p);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, p);
        }
        break;

    case kRGB_565_SkColorType:
        if (tw != w || th != h) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB,
                    GL_UNSIGNED_SHORT_5_6_5, 0);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB,
                    GL_UNSIGNED_SHORT_5_6_5, p);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB,
                    GL_UNSIGNED_SHORT_5_6_5, p);
        }
        break;
    default:
        break;
    }

    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);

    return NO_ERROR;
}

status_t PowerOffHandler::readyToRun() {
    mAssets.addDefaultAssets();

    sp < IBinder
            > dtoken(
                    SurfaceComposerClient::getBuiltInDisplay(
                            ISurfaceComposer::eDisplayIdMain));
    DisplayInfo dinfo;
    status_t status = SurfaceComposerClient::getDisplayInfo(dtoken, &dinfo);
    if (status)
        return -1;

    char value[PROPERTY_VALUE_MAX];
    property_get("persist.panel.orientation", value, "0");
    int orient = atoi(value) / 90;
    if (orient == eOrientation90 || orient == eOrientation270) {
        int temp = dinfo.h;
        dinfo.h = dinfo.w;
        dinfo.w = temp;
    }
    Rect destRect(dinfo.w, dinfo.h);
    mSession->setDisplayProjection(dtoken, orient, destRect, destRect);
    // create the native surface
    sp < SurfaceControl > control = session()->createSurface(
            String8("BootAnimation"), dinfo.w, dinfo.h, PIXEL_FORMAT_RGB_565);

    SurfaceComposerClient::openGlobalTransaction();
    control->setLayer(0x40000000);
    SurfaceComposerClient::closeGlobalTransaction();

    sp < Surface > s = control->getSurface();

    // initialize opengl and egl
    const EGLint attribs[] = { EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8, EGL_DEPTH_SIZE, 0, EGL_NONE };
    EGLint w, h;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    surface = eglCreateWindowSurface(display, config, s.get(), NULL);
    context = eglCreateContext(display, config, NULL, NULL);
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
        return NO_INIT;

    mDisplay = display;
    mContext = context;
    mSurface = surface;
    mWidth = w;
    mHeight = h;
    mFlingerSurfaceControl = control;
    mFlingerSurface = s;

    // If the device has encryption turned on or is in process
    // of being encrypted we show the encrypted boot animation.
    char decrypt[PROPERTY_VALUE_MAX];
    property_get("vold.decrypt", decrypt, "");

    // Use customized resources for boot and showdown animation
    // instead of system predefined boot animation files.
    bool encryptedAnimation = atoi(decrypt) != 0
            || !strcmp("trigger_restart_min_framework", decrypt);

    ZipFileRO* zipFile = NULL;
    if ((encryptedAnimation
            && (access(getShutDownAnimationFileName(IMG_ENC), R_OK) == 0)
            && ((zipFile = ZipFileRO::open(
                    getShutDownAnimationFileName(IMG_ENC))) != NULL)) ||

    ((access(getShutDownAnimationFileName(IMG_DATA), R_OK) == 0) && ((zipFile =
            ZipFileRO::open(getShutDownAnimationFileName(IMG_DATA))) != NULL))
            ||

            ((access(getShutDownAnimationFileName(IMG_SYS), R_OK) == 0)
                    && ((zipFile = ZipFileRO::open(
                            getShutDownAnimationFileName(IMG_SYS))) != NULL))) {
        mZip = zipFile;
    }

    return NO_ERROR;
}

bool PowerOffHandler::threadLoop() {
    bool r = false;
    // animation.
    if (mZip != NULL) {
        r = movie();
    }

    eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(mDisplay, mContext);
    eglDestroySurface(mDisplay, mSurface);
    mFlingerSurface.clear();
    mFlingerSurfaceControl.clear();
    eglTerminate (mDisplay);
    IPCThreadState::self()->stopProcess();
    return r;
}

void PowerOffHandler::checkExit() {
    // Allow surface flinger to gracefully request shutdown
    char value[PROPERTY_VALUE_MAX];
    property_get(EXIT_PROP_NAME, value, "0");
    int exitnow = atoi(value);
    if (exitnow) {
        requestExit();
    }
}

// Parse a color represented as an HTML-style 'RRGGBB' string: each pair of
// characters in str is a hex number in [0, 255], which are converted to
// floating point values in the range [0.0, 1.0] and placed in the
// corresponding elements of color.
//
// If the input string isn't valid, parseColor returns false and color is
// left unchanged.
static bool parseColor(const char str[7], float color[3]) {
    float tmpColor[3];
    for (int i = 0; i < 3; i++) {
        int val = 0;
        for (int j = 0; j < 2; j++) {
            val *= 16;
            char c = str[2 * i + j];
            if (c >= '0' && c <= '9')
                val += c - '0';
            else if (c >= 'A' && c <= 'F')
                val += (c - 'A') + 10;
            else if (c >= 'a' && c <= 'f')
                val += (c - 'a') + 10;
            else
                return false;
        }
        tmpColor[i] = static_cast<float>(val) / 255.0f;
    }
    memcpy(color, tmpColor, sizeof(tmpColor));
    return true;
}

bool PowerOffHandler::readFile(const char* name, String8& outString) {
    ZipEntryRO entry = mZip->findEntryByName(name);
    ALOGE_IF(!entry, "couldn't find %s", name);
    if (!entry) {
        return false;
    }

    FileMap* entryMap = mZip->createEntryFileMap(entry);
    mZip->releaseEntry(entry);
    ALOGE_IF(!entryMap, "entryMap is null");
    if (!entryMap) {
        return false;
    }

    outString.setTo((char const*) entryMap->getDataPtr(),
            entryMap->getDataLength());
    delete entryMap;
    return true;
}

bool PowerOffHandler::movie() {
    char value[PROPERTY_VALUE_MAX];
    String8 desString;

    if (!readFile("desc.txt", desString)) {
        return false;
    }
    char const* s = desString.string();

    Animation animation;

    // Parse the description file
    for (;;) {
        const char* endl = strstr(s, "\n");
        if (endl == NULL)
            break;
        String8 line(s, endl - s);
        const char* l = line.string();
        int fps, width, height, count, pause;
        char path[ANIM_ENTRY_NAME_MAX];
        char color[7] = "000000"; // default to black if unspecified

        char pathType;
        if (sscanf(l, "%d %d %d", &width, &height, &fps) == 3) {
            // ALOGD("> w=%d, h=%d, fps=%d", width, height, fps);
            animation.width = width;
            animation.height = height;
            animation.fps = fps;
        } else if (sscanf(l, " %c %d %d %s #%6s", &pathType, &count, &pause,
                path, color) >= 4) {
            // ALOGD("> type=%c, count=%d, pause=%d, path=%s, color=%s", pathType, count, pause, path, color);
            Animation::Part part;
            part.playUntilComplete = pathType == 'c';
            part.count = count;
            part.pause = pause;
            part.path = path;
            part.audioFile = NULL;
            if (!parseColor(color, part.backgroundColor)) {
                ALOGE("> invalid color '#%s'", color);
                part.backgroundColor[0] = 0.0f;
                part.backgroundColor[1] = 0.0f;
                part.backgroundColor[2] = 0.0f;
            }
            animation.parts.add(part);
        }

        s = ++endl;
    }

    // read all the data structures
    const size_t pcount = animation.parts.size();
    void *cookie = NULL;
    if (!mZip->startIteration(&cookie)) {
        return false;
    }

    ZipEntryRO entry;
    char name[ANIM_ENTRY_NAME_MAX];
    while ((entry = mZip->nextEntry(cookie)) != NULL) {
        const int foundEntryName = mZip->getEntryFileName(entry, name,
                ANIM_ENTRY_NAME_MAX);
        if (foundEntryName > ANIM_ENTRY_NAME_MAX || foundEntryName == -1) {
            ALOGE("Error fetching entry file name");
            continue;
        }

        const String8 entryName(name);
        const String8 path(entryName.getPathDir());
        const String8 leaf(entryName.getPathLeaf());
        if (leaf.size() > 0) {
            for (size_t j = 0; j < pcount; j++) {
                if (path == animation.parts[j].path) {
                    uint16_t method;
                    // supports only stored png files
                    if (mZip->getEntryInfo(entry, &method, NULL, NULL, NULL,
                            NULL, NULL)) {
                        if (method == ZipFileRO::kCompressStored) {
                            FileMap* map = mZip->createEntryFileMap(entry);
                            if (map) {
                                Animation::Part& part(
                                        animation.parts.editItemAt(j));
                                if (leaf == "audio.wav") {
                                    // a part may have at most one audio file
                                    part.audioFile = map;
                                } else {
                                    Animation::Frame frame;
                                    frame.name = leaf;
                                    frame.map = map;
                                    part.frames.add(frame);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    mZip->endIteration(cookie);

    glShadeModel (GL_FLAT);
    glDisable (GL_DITHER);
    glDisable (GL_SCISSOR_TEST);
    glDisable (GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable (GL_TEXTURE_2D);
    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const int xc = (mWidth - animation.width) / 2;
    const int yc = ((mHeight - animation.height) / 2);
    nsecs_t frameDuration = s2ns(1) / animation.fps;

    Region clearReg(Rect(mWidth, mHeight));
    clearReg.subtractSelf(
            Rect(xc, yc, xc + animation.width, yc + animation.height));

    pthread_mutex_init(&mp_lock, NULL);
    pthread_cond_init(&mp_cond, NULL);

    property_get("persist.sys.silent", value, "null");
    if (strncmp(value, "1", 1) != 0) {
        playBackgroundMusic();
    }
    for (size_t i = 0; i < pcount; i++) {
        const Animation::Part& part(animation.parts[i]);
        const size_t fcount = part.frames.size();
        glBindTexture(GL_TEXTURE_2D, 0);

        for (int r = 0; !part.count || r < part.count; r++) {
            // Exit any non playuntil complete parts immediately
            if (exitPending() && !part.playUntilComplete)
                break;

            glClearColor(part.backgroundColor[0], part.backgroundColor[1],
                    part.backgroundColor[2], 1.0f);

            for (size_t j = 0;
                    j < fcount && (!exitPending() || part.playUntilComplete);
                    j++) {
                const Animation::Frame& frame(part.frames[j]);
                nsecs_t lastFrame = systemTime();

                if (r > 0) {
                    glBindTexture(GL_TEXTURE_2D, frame.tid);
                } else {
                    if (part.count != 1) {
                        glGenTextures(1, &frame.tid);
                        glBindTexture(GL_TEXTURE_2D, frame.tid);
                        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR);
                        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_LINEAR);
                    }
                    initTexture(frame);
                }

                if (!clearReg.isEmpty()) {
                    Region::const_iterator head(clearReg.begin());
                    Region::const_iterator tail(clearReg.end());
                    glEnable(GL_SCISSOR_TEST);
                    while (head != tail) {
                        const Rect& r2(*head++);
                        glScissor(r2.left, mHeight - r2.bottom, r2.width(),
                                r2.height());
                        glClear (GL_COLOR_BUFFER_BIT);
                    }
                    glDisable(GL_SCISSOR_TEST);
                }
                // specify the y center as ceiling((mHeight - animation.height) / 2)
                // which is equivalent to mHeight - (yc + animation.height)
                glDrawTexiOES(xc, mHeight - (yc + animation.height), 0,
                        animation.width, animation.height);
                eglSwapBuffers(mDisplay, mSurface);

                nsecs_t now = systemTime();
                nsecs_t delay = frameDuration - (now - lastFrame);
                //ALOGD("%lld, %lld", ns2ms(now - lastFrame), ns2ms(delay));
                lastFrame = now;

                if (delay > 0) {
                    struct timespec spec;
                    spec.tv_sec = (now + delay) / 1000000000;
                    spec.tv_nsec = (now + delay) % 1000000000;
                    int err;
                    do {
                        err = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
                                &spec, NULL);
                    } while (err < 0 && errno == EINTR);
                }

                checkExit();
            }

            usleep(part.pause * ns2us(frameDuration));

            // For infinite parts, we've now played them at least once, so perhaps exit
            if (exitPending() && !part.count)
                break;
        }

        // free the textures for this part
        if (part.count != 1) {
            for (size_t j = 0; j < fcount; j++) {
                const Animation::Frame& frame(part.frames[j]);
                glDeleteTextures(1, &frame.tid);
            }
        }
    }

    ALOGD("waiting for media player to complete.");
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 5; //timeout after 5s.

    pthread_mutex_lock (&mp_lock);
    while (!isMPlayerCompleted) {
        int err = pthread_cond_timedwait(&mp_cond, &mp_lock, &timeout);
        if (err == ETIMEDOUT) {
            break;
        }
    }
    pthread_mutex_unlock(&mp_lock);
    ALOGD("media player is completed.");

    pthread_cond_destroy (&mp_cond);
    pthread_mutex_destroy(&mp_lock);

    return false;
}

const char *PowerOffHandler::getShutDownAnimationFileName(ImageID image) {
    const char *fileName[3] = { OEM_SHUTDOWN_ANIMATION_FILE,
            SYSTEM_SHUTDOWN_ANIMATION_FILE,
            SYSTEM_ENCRYPTED_SHUTDOWN_ANIMATION_FILE };

    // Load animations of Carrier through regionalization environment
    if (Environment::isSupported()) {
        Environment* environment = new Environment();
        const char* animFile = environment->getMediaFile(
                Environment::ANIMATION_TYPE, Environment::SHUTDOWN_STATUS);
        if (animFile != NULL && strcmp(animFile, "") != 0) {
           ALOGD("Get Carrier shutdown Animation file: %s", animFile);
           return animFile;
        }
        delete environment;
    }

    return fileName[image];
}

const char *PowerOffHandler::getShutDownRingtoneFileName(ImageID image) {
    if (image == IMG_ENC) {
        return NULL;
    }

    const char *fileName[2] = { OEM_SHUTDOWN_MUSIC_FILE,
            SYSTEM_SHUTDOWN_MUSIC_FILE };

    // Load ringtones of Carrier through regionalization environment
    if (Environment::isSupported()) {
        Environment* environment = new Environment();
        const char* toneFile = environment->getMediaFile(
                Environment::MUSIC_TYPE, Environment::SHUTDOWN_STATUS);
        if (toneFile != NULL && strcmp(toneFile, "") != 0) {
           ALOGD("Get Carrier shutdown ringtone file: %s", toneFile);
           return toneFile;
        }
        delete environment;
    }

    return fileName[image];
}

void PowerOffHandler::playBackgroundMusic(void) {
    /* Make sure sound cards are populated */
    FILE* fp = NULL;
    if ((fp = fopen("/proc/asound/cards", "r")) == NULL) {
        ALOGW("Cannot open /proc/asound/cards file to get sound card info.");
    } else {
        fclose(fp);
    }

    char value[PROPERTY_VALUE_MAX];
    property_get("qti.audio.init", value, "null");
    if (strncmp(value, "complete", 8) != 0) {
        ALOGW("Audio service is not initiated.");
    }

    const char *fileName;
    if (((fileName = getShutDownRingtoneFileName(IMG_DATA)) != NULL
            && access(fileName, R_OK) == 0)
            || ((fileName = getShutDownRingtoneFileName(IMG_SYS)) != NULL
                    && access(fileName, R_OK) == 0)) {
        pthread_t tid;
        pthread_create(&tid, NULL, playMusic, (void *) fileName);
        pthread_join(tid, NULL);
    }
}

void* playMusic(void* arg) {
    int index = 0;
    char *fileName = (char *) arg;
    sp < MediaPlayer > mp = new MediaPlayer();
    sp<MPlayerListener> mListener = new MPlayerListener();
    if (mp != NULL) {
        ALOGD("starting to play %s", fileName);
        mp->setListener(mListener);

        if (mp->setDataSource(NULL, fileName, NULL) == NO_ERROR) {
            mp->setAudioStreamType(AUDIO_STREAM_ENFORCED_AUDIBLE);
            mp->prepare();
        } else {
            ALOGE("failed to setDataSource for %s", fileName);
            return NULL;
        }

        //waiting for media player is prepared.
        pthread_mutex_lock(&mp_lock);
        while (!isMPlayerPrepared) {
            pthread_cond_wait(&mp_cond, &mp_lock);
        }
        pthread_mutex_unlock(&mp_lock);

        audio_devices_t device = AudioSystem::getDevicesForStream(
                AUDIO_STREAM_ENFORCED_AUDIBLE);
        AudioSystem::initStreamVolume(AUDIO_STREAM_ENFORCED_AUDIBLE, 0, 7);
        AudioSystem::setStreamVolumeIndex(AUDIO_STREAM_ENFORCED_AUDIBLE, 7,
                device);

        AudioSystem::getStreamVolumeIndex(AUDIO_STREAM_ENFORCED_AUDIBLE, &index,
                device);
        if (index != 0) {
            ALOGD("playing %s", fileName);
            mp->seekTo(0);
            mp->start();
        } else {
            ALOGW("current volume is zero.");
        }
    }
    return NULL;
}
// ---------------------------------------------------------------------------

}
;
// namespace android
