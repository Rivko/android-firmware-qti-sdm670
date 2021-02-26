# pakman tree build file
_@ ?= @

.PHONY:  tree tree_clean MAKE_APP_OEM_INCDIR_clean

tree: 
	$(call job,,$(MAKE) V=hexagon_Release_dynamic_toolv81_v65,making .)

tree_clean: MAKE_APP_OEM_INCDIR_clean 
	$(call job,,$(MAKE) V=hexagon_Release_dynamic_toolv81_v65 clean,cleaning .)

MAKE_APP_OEM_INCDIR_clean: 
	$(call job,../chre_app_oem,$(MAKE) V=hexagon_Release_dynamic_toolv81_v65 clean,cleaning ../chre_app_oem)
    
W := $(findstring ECHO,$(shell echo))# W => Windows environment
@LOG = $(if $W,$(TEMP)\\)$@-build.log

C = $(if $1,cd $1 && )$2
job = $(_@)echo $3 && ( $C )> $(@LOG) && $(if $W,del,rm) $(@LOG) || ( echo ERROR $3 && $(if $W,type,cat) $(@LOG) && $(if $W,del,rm) $(@LOG) && exit 1)
ifdef VERBOSE
  job = $(_@)echo $3 && $C
endif
