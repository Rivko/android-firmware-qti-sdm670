# pakman tree build file
_@ ?= @

.PHONY: MAKE_D_1_LIBDIR MAKE_D_2_LIBDIR tree MAKE_D_1_LIBDIR_clean MAKE_D_2_LIBDIR_clean tree_clean

tree: MAKE_D_1_LIBDIR 
	$(call job,,$(MAKE) V=android_Debug,making .)

tree_clean: MAKE_D_1_LIBDIR_clean MAKE_D_1_LIBDIR_clean
	$(call job,,$(MAKE) V=android_Debug clean,cleaning .)

MAKE_D_2_LIBDIR: 
	$(call job,../atomic,$(MAKE) V=android_Debug,making ../atomic)

MAKE_D_2_LIBDIR_clean: 
	$(call job,../atomic,$(MAKE) V=android_Debug clean,cleaning ../atomic)

MAKE_D_1_LIBDIR:  
	$(call job,../cellinfo_tools,$(MAKE) V=android_Debug,making ../cellinfo_tools)

MAKE_D_1_LIBDIR_clean: 
	$(call job,../cellinfo_tools,$(MAKE) V=android_Debug clean,cleaning ../cellinfo_tools)

W := $(findstring ECHO,$(shell echo))# W => Windows environment
@LOG = $(if $W,$(TEMP)\\)$@-build.log

C = $(if $1,cd $1 && )$2
job = $(_@)echo $3 && ( $C )> $(@LOG) && $(if $W,del,rm) $(@LOG) || ( echo ERROR $3 && $(if $W,type,cat) $(@LOG) && $(if $W,del,rm) $(@LOG) && exit 1)
ifdef VERBOSE
  job = $(_@)echo $3 && $C
endif
