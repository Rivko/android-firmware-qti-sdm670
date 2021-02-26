# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import defines as df
from sectools.common.parsegen.dbgp import ParseGenDbgp
from sectools.common.parsegen.elf.format import ParseGenElf
from sectools.common.parsegen.elf.segment.type import PT_LOAD


class ParseGenDbgpElf(ParseGenDbgp):

    def __init__(self, data=None,
                 version=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 elf_class=None):

        self.elfparser = ParseGenElf(data, _class = elf_class)

        if data is None:
            phdr = self.elfparser.get_new_phdr_entry()()
            self.dbgpparser = ParseGenDbgp(data, version, debug_dir, debug_prefix, debug_suffix)
            self.elfparser.add_segment(phdr, self.dbgpparser.get_data())
        else:
            phdr = self.elfparser.phdrs[-1]
            data = self.elfparser.get_segment_data(phdr)
            self.dbgpparser = ParseGenDbgp(data, version, debug_dir, debug_prefix, debug_suffix)

    def get_data(self):
        data = self.dbgpparser.get_data()
        self.elfparser.phdrs[-1].p_offset = df.OFFSET
        self.elfparser.phdrs[-1].p_type = PT_LOAD
        self.elfparser.update_segment(self.elfparser.phdrs[-1], data)
        data = self.elfparser.get_data()
        return data

    @property
    def phys_addr(self):
        return self.elfparser.phdrs[-1].p_paddr

    @phys_addr.setter
    def phys_addr(self, address):
        self.elfparser.phdrs[-1].p_paddr = address
        self.elfparser.phdrs[-1].p_vaddr = address

        self.elfparser.ehdr.e_entry = address

    def __repr__(self):
        return (repr(self.elfparser) + '\n' + repr(self.dbgpparser))

