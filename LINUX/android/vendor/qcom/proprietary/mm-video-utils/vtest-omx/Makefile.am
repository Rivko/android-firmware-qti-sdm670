# sources and intermediate files are separated

AM_CFLAGS := -g -O0 -Werror
AM_CFLAGS += -D_POSIX_SOURCE -DPOSIX_C_SOURCE=199506L
AM_CFLAGS += -D_XOPEN_SOURCE=600
AM_CFLAGS += -D_ANDROID_
AM_CFLAGS += -D__ANDROID__
AM_CFLAGS += -Wno-deprecated-declarations
AM_CFLAGS += -Wno-multichar
#AM_CFLAGS += -Wno-mismatched-tags
AM_CFLAGS += -fno-stack-protector
AM_CFLAGS += -DNON_BIONIC
AM_CFLAGS += "-Dstrlcpy=g_strlcpy"
AM_CFLAGS += "-Dstrlcat=g_strlcat"
#AM_CFLAGS += "-Dchar32_t=uint32_t"
#AM_CFLAGS += "-Dchar16_t=uint16_t"
AM_CFLAGS += -DNATIVE_WINDOW_SINK_DISABLE
AM_CFLAGS += -DMDP_OVERLAY_SINK_DISABLE
AM_CFLAGS += -DMDP_SOURCE_DISABLE
AM_CFLAGS += -DDISABLE_MBICONVERTOR
AM_CFLAGS += -DLIBGBM_ENABLED
AM_CFLAGS += -DLINUX_OMX_TEST_ONLY_ENCODE

AM_CFLAGS += -Istdint.h
AM_CFLAGS += -Iassert.h
AM_CFLAGS += -Iglibconfig.h
AM_CFLAGS += -Iglib.h
AM_CFLAGS += -Istdlib.h

h_sources  =  -I$(top_srcdir)/vtest-omx/common/inc/
h_sources  += -I$(top_srcdir)/vtest-omx/utils/inc/
h_sources  += -I$(top_srcdir)/vtest-omx/xmlparser/inc/
h_sources  += -I$(WORKSPACE)/hardware/qcom/media/mm-video-v4l2/vidc/

AM_CPPFLAGS = $(AM_CFLAGS)
#AM_CPPFLAGS += "-std=c++11"
AM_CPPFLAGS += "-Dstrlcpy=g_strlcpy"
AM_CPPFLAGS += "-Dstrlcat=g_strlcat"

AM_CFLAGS += -Itime.h
AM_CFLAGS += -D__need_timespec
AM_CFLAGS += -D_POSIX_C_SOURCE=199309
AM_CFLAGS += -DEINVAL=22

cpp_sources :=  utils/src/vt_queue.c
cpp_sources +=  utils/src/vt_semaphore.c
cpp_sources +=  utils/src/vt_signal.c
cpp_sources +=  utils/src/vt_file.c
cpp_sources +=  utils/src/vt_ion_allocator.c

cpp_sources +=  xmlparser/src/vtest_XmlParser.cpp
cpp_sources +=  xmlparser/src/vtest_XmlParserHelper.cpp
cpp_sources +=  common/src/vtest_Config.cpp
cpp_sources +=  common/src/vtest_File.cpp
cpp_sources +=  common/src/vtest_Mutex.cpp
cpp_sources +=  common/src/vtest_Parser.cpp
cpp_sources +=  common/src/vtest_Queue.cpp
cpp_sources +=  common/src/vtest_Script.cpp
cpp_sources +=  common/src/vtest_Signal.cpp
cpp_sources +=  common/src/vtest_SignalQueue.cpp
cpp_sources +=  common/src/vtest_Sleeper.cpp
cpp_sources +=  common/src/vtest_Thread.cpp
cpp_sources +=  common/src/vtest_Time.cpp
cpp_sources +=  common/src/vtest_Crypto.cpp
cpp_sources +=  common/src/vtest_BufferManager.cpp
cpp_sources +=  common/src/vtest_ISource.cpp
cpp_sources +=  common/src/vtest_ITestCase.cpp
#cpp_sources += common/src/vtest_IPostProc.cpp
#cpp_sources += common/src/vtest_PostProcFactory.cpp
cpp_sources +=  common/src/vtest_TestCaseFactory.cpp
cpp_sources +=  common/src/vtest_Encoder.cpp
cpp_sources +=  common/src/vtest_EncoderFileSink.cpp
cpp_sources +=  common/src/vtest_EncoderFileSource.cpp
#cpp_sources += common/src/vtest_PostProcSource.cpp
cpp_sources +=  common/src/vtest_TestEncode.cpp
cpp_sources +=  app/src/vtest_App.cpp
#cpp_sources +=  common/src/vtest_Decoder.cpp
#cpp_sources +=  common/src/vtest_DecoderFileSource.cpp
#cpp_sources +=  common/src/vtest_DecoderFileSink.cpp
#cpp_sources +=  common/src/vtest_TestDecode.cpp

bin_PROGRAMS = mm-vidc-omx-test

mm_vidc_omx_test_SOURCES = $(cpp_sources)
mm_vidc_omx_test_CFLAGS = $(h_sources)
mm_vidc_omx_test_CFLAGS  += $(AM_CFLAGS) -include sys/time.h
mm_vidc_omx_test_CPPFLAGS = $(h_sources)
mm_vidc_omx_test_CPPFLAGS += $(AM_CPPFLAGS) -include stdint.h
mm_vidc_omx_test_CXXFLAGS = $(h_sources) -include stdint.h "-std=c++11"
mm_vidc_omx_test_CXXFLAGS += $(AM_CPPFLAGS) -include stdint.h -include glib.h -include assert.h -include sys/time.h
mm_vidc_omx_test_LDFLAGS = -lOmxCore -lOmxVenc -lglib-2.0 -lcutils -ldl -lutils -lxml2 -lpthread
mm_vidc_omx_test_LDFLAGS += -lfastcrc -lstreamparser -lgbm -llog