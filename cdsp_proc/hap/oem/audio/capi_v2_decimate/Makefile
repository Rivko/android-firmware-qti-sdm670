# always first
include glue/defines.min

INCDIRS += \
   inc \
   src \

# only build the shared object if dynamic option specified in V
ifeq (1,$(V_dynamic))
BUILD_DLLS = capi_v2_decimate
endif

BUILD_LIBS = capi_v2_decimate

capi_v2_decimate_C_SRCS += \
   src/capi_v2_decimate \
   src/decimate_block \

capi_v2_decimate_CPP_SRCS += \
   src/capi_v2_decimate_utils
   
# quality test
BUILD_QEXES += capi_v2_decimate_q

capi_v2_decimate_q_CPP_SRCS = src/capi_v2_decimate_test
capi_v2_decimate_q_LIBS = rtld a1std test_main test_util test_capi_v2 capi_v2_utils_props capi_v2_decimate test_profile atomic

# if building shared object must also list it as a dependency to the qtest
ifeq (1,$(V_dynamic))
$V/capi_v2_decimate_q.ok: $V/capi_v2_decimate_q $V/capi_v2_decimate$(DLL_EXT)
endif

# specify final build products, in this case the module's header file and
# its associated libraries (and shared objects)
BUILD_COPIES = \
   inc/capi_v2_decimate.h \
   $(DLLS) \
   $(LIBS) \
   $(SHIP_DIR)/ ;

# always last
include $(RULES_MIN)

