#=============================================================================
#
#                                    memsection_xml.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
class MemsectionWrapper(object):
    def __init__(self,base=None):
        if base:
            self._base = base
            self.name = base.name
    def walkdom(self,el):
        if el.hasAttribute('gap_recovery'):
            return MemsectionWrapper().parse_gap(el,self.cfg)
        return self._base.walkdom(el)
    def parse_gap(self,el,cfg):
        self.tag = 'memsection_gap'
        el.tagName = self.tag
        if el.hasAttribute('vaddr_variable_name'):
            el.setAttribute('vaddr_variable_index', cfg.alloc_slot(el.getAttribute('vaddr_variable_name')))
        if el.hasAttribute('size_variable_name'):
            el.setAttribute('size_variable_index', cfg.alloc_slot(el.getAttribute('size_variable_name')))
        cfg.build_elements.append(el)
        return self
