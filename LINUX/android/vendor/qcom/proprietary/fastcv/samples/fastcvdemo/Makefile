#
# Determine host system and architecture from the environment
# (Borrowed from NDK makefile "init.mk"
#

HOST_OS := $(strip $(HOST_OS))
ifndef HOST_OS
    # On all modern variants of Windows (including Cygwin and Wine)
    # the OS environment variable is defined to 'Windows_NT'
    #
    # The value of PROCESSOR_ARCHITECTURE will be x86 or AMD64
    #
    ifeq ($(OS),Windows_NT)
        HOST_OS := windows
    else
        # For other systems, use the `uname` output
        UNAME := $(shell uname -s)
        ifneq (,$(findstring Linux,$(UNAME)))
            HOST_OS := linux
        endif
        ifneq (,$(findstring Darwin,$(UNAME)))
            HOST_OS := darwin
        endif
        # We should not be there, but just in case !
        ifneq (,$(findstring CYGWIN,$(UNAME)))
            HOST_OS := windows
        endif
        ifeq ($(HOST_OS),)
            $(error Unable to determine HOST_OS from uname -s: $(UNAME))
            $(error Please define HOST_OS in your environment.)
        endif
    endif
    $(info Host OS was auto-detected: $(HOST_OS))
else
    $(info Host OS from environment: $(HOST_OS))
endif

#
# Define function to fix path names to work with Make
#
ifeq ($(HOST_OS),windows)
    fixpath = $(shell cygpath $(subst \,/,$(1)))
else
    fixpath = $(1)
endif

# Fix working directory path for NDK tools
PWD := $(call fixpath,$(PWD))

# Fix Android NDK root path
ANDROID_NDK_ROOT := $(call fixpath,$(ANDROID_NDK_ROOT))

all:
    @$(ANDROID_NDK_ROOT)/ndk-build

clean:
    @$(ANDROID_NDK_ROOT)/ndk-build clean
