# pakman tree build file
_@ ?= @

.PHONY: MAKE_D_5_INCDIR MAKE_D_4_INCDIR MAKE_D_7_INCDIR tree MAKE_D_5_INCDIR_clean MAKE_D_4_INCDIR_clean MAKE_D_7_INCDIR_clean tree_clean

tree:  
	$(call job,,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65,making .)

tree_clean:
	$(call job,,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65 clean,cleaning .)

MAKE_D_7_INCDIR: MAKE_D_4_INCDIR MAKE_D_5_INCDIR MAKE_D_5_INCDIR MAKE_D_4_INCDIR
	$(call job,../test_rsp,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65,making ../test_rsp)

MAKE_D_7_INCDIR_clean: MAKE_D_4_INCDIR_clean MAKE_D_5_INCDIR_clean MAKE_D_5_INCDIR_clean MAKE_D_4_INCDIR_clean
	$(call job,../test_rsp,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65 clean,cleaning ../test_rsp)

MAKE_D_4_INCDIR: 
	$(call job,../test_util,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65,making ../test_util)

MAKE_D_4_INCDIR_clean: 
	$(call job,../test_util,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65 clean,cleaning ../test_util)

MAKE_D_5_INCDIR: 
	$(call job,../atomic,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65,making ../atomic)

MAKE_D_5_INCDIR_clean: 
	$(call job,../atomic,$(MAKE) V=hexagon_Debug_dynamic_toolv81_v65 clean,cleaning ../atomic)

W := $(findstring ECHO,$(shell echo))# W => Windows environment
@LOG = $(if $W,$(TEMP)\\)$@-build.log

C = $(if $1,cd $1 && )$2
job = $(_@)echo $3 && ( $C )> $(@LOG) && $(if $W,del,rm) $(@LOG) || ( echo ERROR $3 && $(if $W,type,cat) $(@LOG) && $(if $W,del,rm) $(@LOG) && exit 1)
ifdef VERBOSE
  job = $(_@)echo $3 && $C
endif
