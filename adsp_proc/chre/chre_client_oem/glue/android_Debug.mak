# pakman tree build file
_@ ?= @

.PHONY: MAKE_D_2_LIBDIR MAKE_D_1_LIBDIR_MAKE_D_1_LIBDIR MAKE_D_1_LIBDIR tree tree_MAKE_D_3_INCDIR MAKE_D_2_LIBDIR_clean MAKE_D_1_LIBDIR_MAKE_D_1_LIBDIR_clean MAKE_D_1_LIBDIR_clean tree_MAKE_D_3_INCDIR_clean tree_clean

tree: tree_MAKE_D_3_INCDIR
	$(call job,,$(MAKE) V=android_Debug,making .)

tree_clean: tree_MAKE_D_3_INCDIR_clean
	$(call job,,$(MAKE) V=android_Debug clean,cleaning .)

tree_MAKE_D_3_INCDIR:
	$(call job,../chre_app_oem,$(MAKE) V=android_Debug,making ../chre_app_oem)

tree_MAKE_D_3_INCDIR_clean:
	$(call job,../chre_app_oem,$(MAKE) V=android_Debug clean,cleaning ../chre_app_oem)

MAKE_D_1_LIBDIR: MAKE_D_2_LIBDIR MAKE_D_1_LIBDIR_MAKE_D_1_LIBDIR
	$(call job,../../../libs/common/rpcmem,$(MAKE) V=android_Debug,making ../../../libs/common/rpcmem)

MAKE_D_1_LIBDIR_clean: MAKE_D_2_LIBDIR_clean MAKE_D_1_LIBDIR_MAKE_D_1_LIBDIR_clean
	$(call job,../../../libs/common/rpcmem,$(MAKE) V=android_Debug clean,cleaning ../../../libs/common/rpcmem)

MAKE_D_1_LIBDIR_MAKE_D_1_LIBDIR: 
	$(call job,../../../test/common/test_util,$(MAKE) V=android_Debug,making ../../../test/common/test_util)

MAKE_D_1_LIBDIR_MAKE_D_1_LIBDIR_clean: 
	$(call job,../../../test/common/test_util,$(MAKE) V=android_Debug clean,cleaning ../../../test/common/test_util)

MAKE_D_2_LIBDIR: 
	$(call job,../../../libs/common/atomic,$(MAKE) V=android_Debug,making ../../../libs/common/atomic)

MAKE_D_2_LIBDIR_clean: 
	$(call job,../../../libs/common/atomic,$(MAKE) V=android_Debug clean,cleaning ../../../libs/common/atomic)

W := $(findstring ECHO,$(shell echo))# W => Windows environment
@LOG = $(if $W,$(TEMP)\\)$@-build.log

C = $(if $1,cd $1 && )$2
job = $(_@)echo $3 && ( $C )> $(@LOG) && $(if $W,del,rm) $(@LOG) || ( echo ERROR $3 && $(if $W,type,cat) $(@LOG) && $(if $W,del,rm) $(@LOG) && exit 1)
ifdef VERBOSE
  job = $(_@)echo $3 && $C
endif
