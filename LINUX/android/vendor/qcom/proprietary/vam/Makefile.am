AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0  --std=c++11 \
        -fno-inline \
        -fno-short-enums \
        -fpic

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = vam_utils/JSON vam_utils/VAMReport test_engines/TestEngine1 \
test_engines/TestEngine2 test_engines/GroundTruthEngine engine_generator/

if SNPE_ENABLE
SUBDIRS += deeplearning_engine
endif

