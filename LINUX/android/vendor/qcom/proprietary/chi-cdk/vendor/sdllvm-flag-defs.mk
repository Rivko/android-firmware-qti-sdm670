# sdllvm-flag-defs.mk - Makefile to include SDLLVM optimization
#
# Include the optimization flags for SDLLVM compiler
#

# SDLLVM C flags
SDCLANG_FLAGS :=                                    \
    -ffp-contract=fast                              \
    -mcpu=cortex-a53                                \
    -mfpu=crypto-neon-fp-armv8                      \
    -fno-fast-math                                  \
    -Wno-logical-not-parentheses                    \
    -Wl,--no-fatal-warnings                         \
    -Wno-address-of-packed-member                   \
    -Wno-tautological-constant-out-of-range-compare

# Note: The Wno-x flags would need to be removed eventually once
# those warnings in the source are addressed.

ifeq ($(LOCAL_SDCLANG_OFAST), true)
  SDCLANG_FLAGS += -Ofast
endif

# Use SDLLVM linker
SDCLANG_LINK := -fuse-ld=qcld

# Turn on LTO for libs which set LOCAL_SDCLANG_LTO.
SDCLANG_LTO  :=
ifeq ($(LOCAL_SDCLANG_LTO), true)
  SDCLANG_LTO := -flto
endif

# Note: LOCAL_SDCLANG_EXTRA_FLAGS can be set in the individual module's .mk
# file in order to override the default SDCLANG_FLAGS.

SDCLANG_CFLAGS   := $(SDCLANG_FLAGS) $(SDCLANG_LTO) $(SDCLANG_LINK)
SDCLANG_LDFLAGS  := $(SDCLANG_FLAGS) $(SDCLANG_LTO)

LOCAL_CFLAGS     += $(SDCLANG_CFLAGS)
LOCAL_CXX_FLAGS  += $(SDCLANG_CFLAGS)
LOCAL_LDFLAGS    += $(SDCLANG_LDFLAGS)
LOCAL_CFLAGS_32  += $(LOCAL_SDCLANG_EXTRA_FLAGS_32)
LOCAL_LDFLAGS_32 += $(LOCAL_SDCLANG_EXTRA_FLAGS_32)

