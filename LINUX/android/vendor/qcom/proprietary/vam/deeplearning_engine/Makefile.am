if SNPE_ENABLE
#SNPE_ROOT= Please specify SNPE_ROOT

common_sources := source/snpe_base.cc
common_sources += source/snpe_complex.cc
common_sources += source/snpe_single_ssd.cc
common_sources += source/tflite_base.cc
common_sources += source/vam_engine_wrapper_ml.cc

lib_LTLIBRARIES = libEngine_ML.la
libEngine_ML_la_SOURCES := $(common_sources)
libEngine_ML_la_CPPFLAGS := -I$(WORKSPACE)/vendor/qcom/proprietary/vam/deeplearning_engine/include
libEngine_ML_la_CPPFLAGS += -I$(SNPE_ROOT)/include/zdl
libEngine_ML_la_CPPFLAGS += -std=c++11 -fpermissive
libEngine_ML_la_CPPFLAGS += -DANDROID
libEngine_ML_la_CPPFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter
libEngine_ML_la_LDFLAGS  := -Wl,--no-undefined
libEngine_ML_la_LDFLAGS  += -llog -ljsoncpp -ldl -lcutils
libEngine_ML_la_LDFLAGS  += -ltensorflow-lite -lfastcvopt
libEngine_ML_la_LDFLAGS  += -L$(SNPE_ROOT)/lib/arm-linux-gcc4.8hf -lSNPE

root_etcdir   := "/data/misc/camera"
root_etc_DATA := va-ml-engine-library_config.json

endif

