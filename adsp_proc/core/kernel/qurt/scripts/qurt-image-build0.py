#! /usr/bin/python
#=============================================================================
#
#                                    qurt-image-build0.py
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
import re
from lib.qurt import *
from lib.qiblog import QIBLog
from lib.elf_info_patch import eip_q6cfg

EIP_BUILD_SYMTAB = 10000
EIP_BUILD_STRTAB = 10001

class UniqueStringClass:
    #
    #  Return a synthesized unique string.
    #  Mainly designed to make sure each call returns a value
    #   which is distinct from all other valid strings.
    #
    #  With this and the below assignment after the class
    #   definition, UniqueString() will return a new value
    #   each time it's called.
    #
    def __init__(self):
        self.counter = 0
    def __call__(self):
        self.counter += 1
        return '$$UNIQUE$$%u' % self.counter

UniqueString = UniqueStringClass()

class Elf_File:
    #
    #  Abstraction class for reading in ELF files.
    #
    #  Not quite general purpose -- it has a few dependencies on the particular ELF processing done by qurt-image-build.
    #
    #  Arguments:
    #   cfg      : Reference to the main image builder object
    #   appname  : The "spawn" name of the process -- the string used to refer to the process image at runtime
    #   filename : The local filename to be used to open the ELF file
    #   restart  : Boolean indicating whether the process should be marked as restartable in the stitched image
    #
    #  Note that for the root ELF, appname and restart are processed but essentially have no effect.
    #  The root ELF is started regardless of its name, and it's not restartable.
    #
    def __init__(self, cfg, appname, filename, restart):
        self.rawfile = open(filename, 'rb')
        self.cfg = cfg
        self.appname = appname
        self.restart = restart
        self.restart_copies = []
        self.read_elf_info()

    def seek(self, *args, **kw):
        return self.rawfile.seek(*args, **kw)

    def read(self, *args, **kw):
        return self.rawfile.read(*args, **kw)

    def read_elf_info(self):
        #
        #  Read top level ELF header
        #
        self.seek(0)
        self.ehdr = Elf32Header(self)

        #
        #  Read all program headers
        #
        self.seek(self.ehdr.e_phoff)
        self.rawphdr = [Elf32ProgramHeader(self, temp=False, f=self.rawfile) for _ in xrange(self.ehdr.e_phnum)]

        #
        #  Remove all program headers except for PT_LOAD type with non-empty size.
        #

        self.phdr = []
        for p in self.rawphdr:
            if (p.p_type == p.PT_LOAD and p.p_memsz > 0):
                self.phdr.append(p)
            elif (p.p_type == p.PT_NULL):
                p.temp=True
                self.phdr.append(p)

        #
        #  Read all section headers
        #
        self.seek(self.ehdr.e_shoff)
        self.shdr = [Elf32SectionHeader(self, temp=False, f=self.rawfile, shindex=j) for j in xrange(self.ehdr.e_shnum)]

        #
        #  For each segment which does not end on a page boundary, and which has
        #   unused address space right after it, increase its memsz to either the
        #   page boundary or the next used address, whichever comes first.
        #

        for p in self.phdr:
            end_addr = p.p_vaddr + p.p_memsz
            padding = (-end_addr) & 0xFFF
            for p2 in self.phdr:
                if p2.p_vaddr >= end_addr:
                    padding = min(padding, p2.p_vaddr - end_addr)
            if padding:
                print '%X -> from %X to %X' % (p.p_vaddr, p.p_memsz, p.p_memsz+padding)
            p.p_memsz += padding
            p.section_list = []
            p.first_alloc = QurtInfinity
            p.first_progbits = QurtInfinity
            p.furthest_alloc = -QurtInfinity
            p.furthest_progbits = -QurtInfinity

        #
        #  Read the string table
        #
        strtab = self.shdr[self.ehdr.e_shstrndx].contents().read()

        #
        #  Extract section names from the string table
        #
        for s in self.shdr:
            s.name = strtab[s.sh_name:].split('\0')[0]

        #
        #  For each section which does not end on a page boundary, and which has
        #   unused address space right after it, add a NOBITS/ALLOC section right
        #   after it to fill to the page boundary or next used address, whichever
        #   comes first.
        #

        salloc = [s for s in self.shdr if (s.sh_flags & s.SHF_ALLOC) != 0 and s.sh_size > 0]

        for s in salloc[:]:
            end_addr = s.sh_addr + s.sh_size
            padding = (-end_addr) & 0xFFF
            for s2 in salloc:
                if s2.sh_addr >= end_addr:
                    padding = min(padding, s2.sh_addr - end_addr)
            if padding:
                d = dict(s.__dict__)
                d['sh_addr'] = end_addr
                d['sh_size'] = padding
                d['sh_type'] = s.SHT_NOBITS
                d['sh_offset'] = 0x80000000
                d['s_internal'] = True
                self.shdr.append(Elf32SectionHeader(d))
                salloc.append(self.shdr[-1])

        #
        #  Verify consistency between section headers and program headers
        #

        for s in salloc:
            # Find all program headers which overlap this section header
            over = []
            for p in self.phdr:
                if p.p_vaddr >= s.sh_addr + s.sh_size:
                    # Program header falls after section
                    pass
                elif p.p_vaddr + p.p_memsz <= s.sh_addr:
                    # Program header falls before section
                    pass
                else:
                    over.append(p)
            if len(over) == 0:
                raise Exception('Corrupted ELF file -- section at %X, length %X not covered by PT_LOAD segment' % (s.sh_addr, s.sh_size))            
            if len(over) > 1:
                raise Exception('Corrupted ELF file -- section at %X, length %X covered by multiple PT_LOAD segments' % (s.sh_addr, s.sh_size))
            p = over[0]
            p.furthest_alloc = max(p.furthest_alloc, s.sh_addr+s.sh_size)
            p.first_alloc = min(p.first_alloc, s.sh_addr)
            if s.sh_type == s.SHT_NOBITS:
                p.section_list.append(s)
                continue
            p.furthest_progbits = max(p.furthest_progbits, s.sh_addr+s.sh_size)
            p.first_progbits = min(p.first_progbits, s.sh_addr)
            if s.sh_addr < p.p_vaddr:
                raise Exception('Corrupted ELF file -- section at %X, length %X only partly covered by PT_LOAD segment' % (s.sh_addr, s.sh_size))
            if s.sh_addr + s.sh_size > p.p_vaddr + p.p_memsz:
                raise Exception('Corrupted ELF file -- section at %X, length %X only partly covered by PT_LOAD segment' % (s.sh_addr, s.sh_size))
            if s.sh_addr + s.sh_size > p.p_vaddr + p.p_filesz:
                raise Exception('Corrupted ELF file -- section at %X, length %X not initialized by PT_LOAD segment' % (s.sh_addr, s.sh_size))
            if (s.sh_addr - s.sh_offset != p.p_vaddr - p.p_offset) and not p.temp:
                raise Exception('Corrupted ELF file -- section at %X, length %X has wrong offset compared to PT_LOAD segment' % (s.sh_addr, s.sh_size))
            p.section_list.append(s)


        #
        #  Associate a list of section headers with each program header
        #

        for p in self.phdr:
            extra_front = p.first_alloc - p.p_vaddr
            extra_back = (p.p_vaddr + p.p_memsz) - p.furthest_alloc
            if extra_front < 0 or extra_back < 0:
                raise Exception('Contents fall outside PHDR')
            if p.furthest_progbits < p.p_vaddr:
                p.furthest_progbits = p.p_vaddr
            p.p_filesz = p.furthest_progbits - p.p_vaddr
            p.p_memsz = p.furthest_alloc - p.p_vaddr

        #
        #  Allocate the virtual addresses with the main object.
        #   This should detect overlaps and fail the build if they happen.
        #
        for p in self.phdr:
            self.cfg.vaddr_space.alloc(size=p.p_memsz,
                                       addr=p.p_vaddr,
                                       phdr=p)

        #
        #  Get the debug sections into self.debugsections.
        #
        #  We'll only propagate them if requested through an XML directive,
        #   but setting them aside here doesn't cost us anything.
        #

        tmp = [s for s in self.shdr if (s.sh_flags & s.SHF_ALLOC) == 0]
        tmp = [s for s in tmp if s.name == '.strtab' or s.name == '.symtab' or s.name.startswith('.debug_')]
        self.debugsections = tmp

        #
        #  Remove SHT_NULL and SHT_STRTAB sections.  We'll add one of each back in
        #   at the end.
        #

        self.shdr = [s for s in self.shdr if s.sh_type not in (s.SHT_NULL,s.SHT_STRTAB) or (s.sh_flags & s.SHF_ALLOC) != 0]

        #
        #  For our purposes, we require every stitched ELF to have a section named
        #   .start.  Verify that here, and extract the address and size of the
        #   .start section.
        #
        starts = [s for s in self.shdr if (s.name == '.start' or s.name == '.qstart') and not hasattr(s,'s_internal')]
        if len(starts) > 1:
            raise Exception('Input ELF must have a single .start section')
        self.start_addr = starts[0].sh_addr
        self.start_size = starts[0].sh_size

def parse_build_elements(cfg, els):
    #
    #  parse_build_elements() must be a top level function in this file;
    #   the qurt_config.py script finds it and imports it.
    #
    #  This function is called with a list of XML Element nodes and is
    #   expected to parse the nodes appropriately and update the provided
    #   cfg structure with a parsed representation of the nodes.  It is
    #   expected that other code elsewhere will know what to do with the
    #   information attached to the cfg structure.
    #
    #  This function is called once by qurt_config.py and again by
    #   qurt-image-build.py.  It should perform the same parsing and
    #   updates in both cases, although not all of the information
    #   will be needed in both passes.
    #
    def BadBuildElem(s):
        raise Exception('Failure in <build> configuration: %s' % s)

    class generic_xml:
        def __init__(self, cfg):
            setattr(cfg, self.__class__.__name__, self)
            for k,v in self.defaults:
                setattr(self, k, v)
        def add_attr(self, k, v):
            setattr(self, k, v)
        def finalize(self):
            pass
        force_init = True
        defaults = []

    # For each "simple" XML element, define a class with the same name
    #  as the XML element, have it inherit from generic_xml, and set
    #  the class variable "defaults" to a list of default attributes.

    class max_elf_tlb(generic_xml):
        defaults = [('value', 0x100000)]
        def finalize(self):
            self.value = int(self.value,0)
    class clade_base_paddr(generic_xml):
        defaults = [('value', 0x0)]
        def finalize(self):
            self.value = int(self.value,0)
    class image_elf_padded(generic_xml):
        defaults = [('value', False)]
        def finalize(self):
            self.value = (self.value.lower() == 'true')
    class relocatable_image(generic_xml):
        defaults = [('value', True), ('align', 0x100)]                  # Default:  True, 1MB alignment
        def finalize(self):
            if self.value.lower() == 'false':
                (self.value, self.align) = (False, 0x1000)              # False, 16MB alignment
            elif self.value.lower() == 'true':
                (self.value, self.align) = (True, 0x100)                # True, 1MB alignment
            else:
                (self.value, tmp) = (True, convert_size(self.value))
                if tmp < 0x1000 or (tmp & (tmp-1)) != 0:
                    raise Exception('relocatable_image value must be a power of two >= 4K')
                # Convert self.align to a number of 4K pages which can be represented in a TLB entry
                self.align = 0x1000
                while (self.align << 12) > tmp:
                    self.align /= 4
    class tlb_limit(generic_xml):
        defaults = [('value', 10000)]
        def finalize(self):
            self.value = int(self.value)
    class altscript(generic_xml):
        defaults = [('value', '')]
    class strip_debug_info(generic_xml):
        defaults = [('value', 'true')]
    class use_phdrs(generic_xml):
        defaults = [('value', '0')]
        def finalize(self):
            self.value = int(self.value,0)
    class gap_reclaim(generic_xml):
        defaults = [('value', 'normal')]
    class tlbdump_usage(generic_xml):
        defaults = [('value', 'normal')]
    class mapping_method(generic_xml):
        defaults = [('value', 'normal')]
    class allow_map_wx(generic_xml):
        defaults = [('value', 'false')]
    class l2_cache_v61_v1(generic_xml):
        defaults = [('value', 'false')]

    class export_symbol(generic_xml):
        def __init__(self,cfg):
            #  Get the list that we'll add symbols to
            #  It will be accessible as cfg.export_symbol
            #  Create it if it doesn't exist yet
            self.symbol_list = cfg.__dict__.setdefault('export_symbol',list())
        def add_attr(self, k, v):
            if k == 'name':
                self.symbol_list.extend(v.split())
            else:
                BadBuildElem('export_symbol only supports the "name" attribute')
        def finalize(self):
            #
            #  Actually push a synthetic symbol list into the source code.
            #
            #  What we do here is construct the .symtab and .strtab sections
            #   in binary ready format, and push them into EIP build records.
            #
            #  The .symtab section is built as a record of type EIP_BUILD_SYMTAB,
            #   and the .strtab section is built as a record of type EIP_BUILD_STRTAB.
            #
            #  After linking, when we build the final boot image, we will use
            #   the EIP_BUILD_SYMTAB and EIP_BUILD_STRTAB records to synthesize
            #   the .symtab and .strtab sections in the boot image.
            #
            symtab_code = [
                '   .pushsection .rodata.eip_build_symtab,"a",@progbits',
                '   .align 4',
                '   .short 99f-.',
                '   .short %u' % EIP_BUILD_SYMTAB,
                '   .long 0,0,0,0'
                ]
            strtab_code = [
                '   .pushsection .rodata.eip_build_strtab,"a",@progbits',
                '   .align 4',
                '   .short 99f-.',
                '   .short %u' % EIP_BUILD_STRTAB,
                '   .byte 0'
                ]
            strloc = 1
            for NAME in sorted(set(self.symbol_list)):
                #
                #  Symbol table entry, from ELF Format:
                #    4 byte index into string table pointing at name
                #    4 byte address
                #    4 byte size (we encode zero, since we don't know sizes)
                #    1 byte info (we encode 0x10 = STB_GLOBAL and STT_NOTYPE)
                #    1 byte other (zero)
                #    2 byte section header index (we encode 0xFFF1 = SHN_ABS)
                #
                symtab_code.append('   .long %u' % strloc)
                symtab_code.append('   .weak %s' % NAME)
                symtab_code.append('   .long %s' % NAME)
                symtab_code.append('   .long 0')
                symtab_code.append('   .byte 0x10')
                symtab_code.append('   .byte 0')
                symtab_code.append('   .short 0xFFF1')
                strtab_code.append('   .ascii "%s"' % NAME)
                strtab_code.append('   .byte 0')
                strloc += len(NAME)+1
            symtab_code.append('   .align 4')
            symtab_code.append('99:')
            symtab_code.append('   .popsection')
            strtab_code.append('   .align 4')
            strtab_code.append('99:')
            strtab_code.append('   .popsection')
            cfg.symtab_code = asm_to_C(symtab_code)
            cfg.strtab_code = asm_to_C(strtab_code)

    class section:
        def __init__(self,cfg):
            self.match = lambda x, sname: (sname==None)
            self.attrs = {}
            cfg.section_info.append(self)
        def finalize(self):
            # Allow all section elements and attributes for now.
            # In the future, check here for conflicting attributes,
            #  illegal attributes, etc.
            pass
        def add_attr(self,k,v):
            from re import compile
            if k == 'tlb_lock' and v == '1':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',True)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v == '0':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',False)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v.lower() == 'root':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',s.root_elf)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v.lower() == 'boot':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',s.root_elf)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',s.root_elf)
            elif k == 'mapping' and v.lower() == 'none':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','none')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',s.sh_flags & ~(s.SHF_WRITE | s.SHF_EXECINSTR))
                self.attrs['CACHE'] = lambda s: setattr(s,'cache_policy','-1')
            elif k == 'mapping' and v.lower() == 'rx':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','rx')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',(s.sh_flags | s.SHF_EXECINSTR) & ~s.SHF_WRITE)
            elif k == 'mapping' and v.lower() == 'rw':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','rw')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',(s.sh_flags | s.SHF_WRITE) & ~s.SHF_EXECINSTR)
            elif k == 'mapping' and v.lower() == 'overlay':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','overlay')
            elif k == 'user_mode' and v.lower() == 'false':
                self.attrs['UMODE'] = lambda s: setattr(s,'user_mode',not s.root_elf)
            elif k == 'user_mode' and v.lower() == 'true':
                self.attrs['UMODE'] = lambda s: setattr(s,'user_mode',True)
            elif k == 'cache_policy':
                self.attrs['CACHE'] = lambda s,v=v: setattr(s,'cache_policy',v)
            elif k == 'mapping_group':
                self.attrs['MAPGROUP'] = lambda s,v=v: setattr(s,'mapping_group',v)
            elif k == 'physpool':
                idx = None
                try:
                    if v in cfg.region_names:
                        idx = int(cfg.region_names[v].index,0)
                        v = cfg.region_names[v].pool
                except AttributeError:
                    pass
                self.attrs['PHYSPOOL'] = lambda s,v=v: setattr(s,'physpool', v)
                self.attrs['RANGE_IDX'] = lambda s,idx=idx: setattr(s,'range_idx', idx)
            elif k == 'clade_register':
                self.attrs['CLADE_REG'] = lambda s,v=v: setattr(s,'clade_reg', v)
            elif k == 'copybytes':
                self.attrs['COPYBYTES'] = lambda s,v=v: setattr(s,'copybytes', v)
            elif k == 'name' and not '*' in v:
                self.name = v
                self.match = lambda x, sname: (sname== x.name)
            elif k == 'name' and '*' in v:
                tmp = v.replace('.','\\.')
                tmp = v.replace('*','.*')
                self.regex = compile(tmp + '$')
                self.match = lambda x, sname: (sname and x.regex.match(sname) != None)
            elif k == 'match':
                self.regex = compile(v + '$')
                self.match = lambda x, sname: (sname and x.regex.match(sname) != None)
            else:
                BadBuildElem('%s cannot set %s to %s' % (self,k,v))

    class MemsectionGap(object):
        def __init__(self, cfg,
                     vaddr_variable_index,              # Mandatory; filled in by qurt_config
                     size_variable_index,               # Mandatory; filled in by qurt_config
                     size,                              # Mandatory
                     vaddr_variable_name = None,        # Optional; ignored
                     size_variable_name = None,         # Optional; ignored
                     cache_policy = "6",                # Optional; uncached
                     min_size = None,                   # Optional; set to size later
                     attach = "R",                      # Optional; read-only
                     glob = "1",                        # Optional; global
                     gap_recovery = "!R",               # Optional; not readable
                     physpool = "DEFAULT_PHYSPOOL",     # Optional; default physpool
                     max_fragment = "0xFFFFF000",       # Optional; effectively infinite
                     **kw):
            if kw:
                raise Exception('Unknown attributes for gap memsection: %s' % kw.keys())
            self.vaddr_index = int(vaddr_variable_index)
            self.size_index = int(size_variable_index)
            if min_size == None:
                min_size = size
            self.max_size = convert_size(size)
            self.min_size = convert_size(min_size)
            if cache_policy in cfg.cachepolicydict:
                cache = cfg.cachepolicydict[cache_policy]
            else:
                cache = int(cache_policy.split(':')[0], 0)
            self.tlb_entry = 1<<63
            self.tlb_entry |= cache << 24
            pdict = {'R': 1<<29, 'W': 1<<30, 'X': 1<<31}
            for c in attach:
                self.tlb_entry |= pdict[c.upper()]
            if glob == '1':
                self.tlb_entry |= 1<<62
            if physpool != 'DEFAULT_PHYSPOOL':
                raise Exception('Gap recovery only supported from DEFAULT_PHYSPOOL')
            self.max_fragment = convert_size(max_fragment)
            self.gap_recovery = gap_recovery
            self.cfg = cfg
        def rangefits(self, range):
            if isinstance(range, AllocatedRange):
                return False
            for s in self.gap_recovery.split(','):
                if s[0:1] == '!':
                    if getattr(range,s[1:]):
                        return False
                else:
                    if not getattr(range,s):
                        return False
            # Must contain at least 4K of usable space
            usable_size = aligndown(range.right,0x1000) - alignup(range.left,0x1000)
            if usable_size <= 0:
                return False
            range.usable_size = usable_size
            return True
        def process(self):
            vaddr_loc = self.cfg.unpack_from('<L', self.cfg.qurtos_slots + 4*self.vaddr_index)[0]
            size_loc = self.cfg.unpack_from('<L', self.cfg.qurtos_slots + 4*self.size_index)[0]
            poss = [r for r in self.cfg.physpool_space.ranges if self.rangefits(r)]
            poss.sort(key=lambda r: (r.usable_size, r.left))
            need_bytes = aligndown(self.max_size,0x1000)
            ranges = []
            for r in poss:
                if need_bytes > 0:
                    if r.usable_size <= need_bytes:
                        ranges.append(self.cfg.physpool_space.alloc(addr=alignup(r.left,0x1000),
                                                                    size=r.usable_size,
                                                                    R=getattr(r,'R',False),
                                                                    W=getattr(r,'W',False),
                                                                    X=getattr(r,'X',False),
                                                                    U=getattr(r,'U',False),
                                                                    gap_recovery=self))
                        need_bytes -= r.usable_size
                    elif r.usable_size <= self.max_fragment:
                        ranges.append(self.cfg.physpool_space.alloc(addr=alignup(r.left,0x1000),
                                                                    size=need_bytes,
                                                                    R=getattr(r,'R',False),
                                                                    W=getattr(r,'W',False),
                                                                    X=getattr(r,'X',False),
                                                                    U=getattr(r,'U',False),
                                                                    gap_recovery=self))
                        need_bytes = 0
            for r in self.cfg.physpool_space.ranges:
                if 'usable_size' in r.__dict__:
                    del r.__dict__['usable_size']
            tlb_list = []
            tsize = 0
            for r in ranges:
                addr = r.addr
                size = r.size
                while size:
                    tlbsize = 0x100000                                  # 1MB
                    while tlbsize > size or (addr%tlbsize) != 0:
                        tlbsize /= 4
                    tlb_list.append((tlbsize,addr))
                    addr += tlbsize
                    size -= tlbsize
                    tsize += tlbsize
            if tsize < self.min_size:
                raise Exception('Gap recovery memsection cannot meet minimum size')
            if tsize == 0:
                return
            tlb_list.sort(key=lambda x:(-x[0],x[1]))
            vaddr = self.cfg.vaddr_space.find(tsize,tlb_list[0][0],gap_recovery=self).addr
            self.cfg[(vaddr_loc,4)] = ('<L', vaddr)
            self.cfg[(size_loc,4)] = ('<L', tsize)
            for t in tlb_list:
                e = HexagonMapping()
                e.set_from_TLB_entry(self.tlb_entry | 1, locked=False, reloc=self.cfg.relocatable_image.value)
                e.vpage = (vaddr >> 12)
                e.ppage = (t[1] >> 12)
                e.size = (t[0] >> 12)
                self.cfg.extra_mappings.append(e)
                vaddr += t[0]

    section_attributes = section

    cfg.section_info = []

    for k,v in locals().items():
        if hasattr(v,'force_init'):
            v(cfg)

    cfg.cachepolicydict = {}
    for oe in els:
        if oe.tagName == 'cache_policy':
            cfg.cachepolicydict[oe.getAttribute('name')] = int(oe.getAttribute('value'),0)

    cfg.program_maps = {}
    for oe in els:
        if oe.tagName == 'program_name_map':
            cfg.program_maps[oe.getAttribute('name')] = int(oe.getAttribute('number'),0)

    cfg.memsectiongaps = []
    for oe in els:
        if oe.tagName == 'memsection_gap':
            cfg.memsectiongaps.append(MemsectionGap(cfg, **dict(oe.attributes.items())))

    for oe in els:
        for ie in oe.childNodes:
            if ie.nodeType == ie.ELEMENT_NODE:
                this = locals()[ie.tagName](cfg)
                if ie.attributes:
                    for k,v in ie.attributes.items():
                        this.add_attr(k,v)
                this.finalize()

def seg_optimize(segs):
    #
    # Look through the list for segments which overlap or touch
    # Elements are sorted by the start address
    # If they overlap, combine them and combine permissions
    # If they touch without overlapping, only combine them if permissions are identical
    # Edit the list and return True if we were able to optimize
    # Leave the list and return False if we were not able to optimize
    #
    for n in range(len(segs)-1):
        if segs[n][1] > segs[n+1][0]:
            ''' n and n+1 overlap '''
            if segs[n][2] != segs[n+1][2]:
                raise Exception('Overlapping segments with different memory maps')
            ''' n and n+1 overlap with the same offset '''
            ''' Combine them, including permissions, and return '''
            combined = (segs[n][0],
                        max(segs[n][1], segs[n+1][1]),
                        segs[n][2],
                        segs[n][3] or segs[n+1][3],
                        segs[n][4] or segs[n+1][4],
                        segs[n][5] or segs[n+1][5])
            segs = segs[:n] + [combined] + segs[n+2:]
            return (False, segs)
        elif segs[n][1] == segs[n+1][0]:
            ''' n and n+1 touch '''
            if segs[n][2:] == segs[n+1][2:]:
                ''' n and n+1 touch and have the same offset and permissions '''
                ''' combine them and return '''
                segs = segs[:n] + [(segs[n][0], segs[n+1][1]) + segs[n][2:]] + segs[n+2:]
                return (False, segs)
        else:
            ''' n and n+1 have separation between them '''
            pass
    return (True, segs)

def splitname(s, restart):
    from os import path
    names = s.split('=')
    if len(names) == 1:
        return (path.basename(s).replace('elf','pbn'), s, restart)
    if len(names) == 2:
        return (names[0], names[1], restart)
    raise Exception("input file argument cannot contain multiple '=' characters")

class PhysPool:
    def __init__(self, tuple, pool_relocs=None):
        self.ranges = []
        self.pool_relocs = pool_relocs
        self.name = tuple[0].split('\0')[0]
        for base,size in zip(tuple[1::2],tuple[2::2]):
            if size == 0:
                break
            if (size & 0xFFF) != 0:
                raise Exception("Pool %s has size which is not 4K aligned" % self.name)
            #Store base,size,original_size
            #we will use the original size in phys_island_report
            self.ranges.append([base,size >> 12,size >> 12, 7])
    def set_range_policy(self, range_idx, cache_policy):
        self.ranges[range_idx][3] = cache_policy
    #Function to check if the range of a pool has relocatable addresses or physical addresses
    def is_reloc(self, range_idx):
        if self.pool_relocs and self.name in self.pool_relocs:
            #idx = int(self.pool_relocs[str(self.idx)].range_idx,0)
            range_bitmap = int(self.pool_relocs[self.name].range_bitmap, 0)
            if range_bitmap & (1 << range_idx):
                return True
        return False
    def alloc_pages(self, size, align, range_idx=None):
        # Allocate pages from this PhysPool
        if range_idx != None:
            if range_idx < len(self.ranges):
                # we need to allocate from a specific range
                r = self.ranges[range_idx]
                if r[1] >= size:
                    r[0] += size
                    r[1] -= size
                    return self.is_reloc(range_idx), r[0]-size, r[3]
            else:
                raise Exception("Unable to allocate %u pages from phys pool %s Invalid range %d " % (size, self.name, range_idx))
            raise Exception("Unable to allocate %u pages from phys pool %s range %d" % (size, self.name, range_idx))
        # For now, don't get fancy.  We only allocate in order from large->small,
        #  and so we should always be able to take the first fit.
        # Returns the page number
        for range_idx, r in enumerate(self.ranges):
            if r[1] >= size:
                r[0] += size
                r[1] -= size
                return self.is_reloc(range_idx),r[0]-size, r[3]
        raise Exception("Unable to allocate %u pages from phys pool %s" % (size, self.name))

class HexagonMapping:
    """Class for representing a Hexagon TLB entry."""
    def __init__(self):
        self.sname = ''
        self._tokens = []

    def add_token (self, obj):
        self._tokens.append( obj )

    def get_tokens (self, obj):
        return tuple( self._tokens )

    def del_token (self, obj):
        self._tokens.remove( obj )

    def get_token (self, cls):
        for token in self._tokens:
            if isinstance( token, cls ):
                return token
        return None

    def has_token (self, cls):
        for token in self._tokens:
            if isinstance( token, cls ):
                return True
        return False

    def get_TLB_entry(self):
        """Return the 64-bit encoded TLB entry."""
        # Get the size in place first
        ret = 1
        while ret*ret < self.size:
            ret *= 2
        if ret*ret != self.size:
            raise Exception('Internal error -- locked TLB entry of illegal size')
        ret |= (1L << 63)       # Set the valid bit
        ret |= self.G << 62
        if self.ppage & 0x800000:
            ret |= (1L << 61)
        ret |= self.A1A0 << 59
        ret |= self.ASID << 52
        ret |= self.vpage << 32
        ret |= self.X << 31
        ret |= self.W << 30
        ret |= self.R << 29
        ret |= self.U << 28
        ret |= self.C << 24
        ret |= (self.ppage & 0x7FFFFF) << 1
        if self.ASID == 0 and self.X == 1 and self.W == 0 and self.C != 4 and self.C != 6:
            ret |= (1 << 28)
        return ret
    def set_from_TLB_entry(self, v, locked=True, mapping_group=None, reloc=False):
        if mapping_group == None:
            mapping_group = UniqueString()
        """Set the fields of the mapping based on the 64-bit TLB entry (v)."""
        if (v & (1L << 63)) == 0:
            raise Exception('Tried to use a TLB entry not set to valid')
        tmp = (v & 0x7F)
        tmp = (tmp & -tmp)    # Sets to 0, 1, 2, 4, 8, 16, 32, or 64
        sz = tmp * tmp       # Sets to 0, 1, 4, 16, 64, 256, 1024, or 4096
        if sz == 0:
            raise Exception('Tried to use a TLB entry with invalid size')
        self.island = False
        self.reclaim = False
        self.mapping_group = mapping_group
        self.locked = locked
        self.reloc = reloc
        self.physpool = None
        self.range_idx = None
        self.clade_reg = None
        self.overlay = 0
        self.G    = ((v >> 62) & 0x01)
        self.A1A0 = ((v >> 59) & 0x03)
        self.ASID = ((v >> 52) & 0x7F)
        self.X    = ((v >> 31) & 0x01)
        self.W    = ((v >> 30) & 0x01)
        self.R    = ((v >> 29) & 0x01)
        self.U    = ((v >> 28) & 0x01)
        self.C    = ((v >> 24) & 0x0F)
        self.vpage = ((v >> 32) & 0xFFFFF) & (~(sz-1))
        self.ppage = ((v >>  1) & 0x7FFFFF) & (~(sz-1))
        if ((v >> 61) & 0x01):
            # EP bit is set, ppage gets high bit set
            self.ppage += 0x800000
        self.size = sz
        return self
    def contains(self, other):
        """Return True if self is a proper superset of other."""
        my_attr = (self.mapping_group, self.locked, self.G, self.A1A0, self.ASID,
                   self.X, self.W, self.R, self.U, self.C, self.reloc)
        his_attr = (other.mapping_group, other.locked, other.G, other.A1A0, other.ASID,
                    other.X, other.W, other.R, other.U, other.C, other.reloc)
        if my_attr != his_attr:
            return False
        if self.vpage - self.ppage != other.vpage - other.ppage:
            return False
        if other.vpage < self.vpage:
            return False
        if other.vpage + other.size > self.vpage + self.size:
            return False
        return True
    def overlaps(self, other):
        """Return True if self and other overlap in virtual space."""
        if self.vpage + self.size <= other.vpage:
            return False
        if other.vpage + other.size <= self.vpage:
            return False
        return True
    def fix_to_TLB_entries(self, maxsize=0x1000):
        """Takes a mapping of any size and returns an equivalent list of mappings of legal sizes."""
        from copy import copy
        ret = []
        off = 0            # Offset from beginning
        sz = self.size     # Number of pages left
        while sz > 0:
            this_sz = 0x1000
            while this_sz > 1:
                if this_sz <= sz and this_sz <= maxsize:
                    if ((self.vpage+off) & (this_sz-1)) == 0:
                        if ((self.ppage+off) & (this_sz-1)) == 0:
                            break
                this_sz = this_sz >> 2
            tmp = copy(self)
            tmp.vpage += off
            tmp.ppage += off
            tmp.size = this_sz
            tmp.mapping_group = self.mapping_group
            ret.append(tmp)
            sz -= this_sz
            off += this_sz
        return ret
    def __str__(self):
        return '<%05X->%06X [%04X] G:%u A:%u ASID:%u X:%u W:%u R:%u U:%u C:%X %s %s %s %s %s>' % (self.vpage,
                                                                                               self.ppage,
                                                                                               self.size,
                                                                                               self.G,
                                                                                               self.A1A0,
                                                                                               self.ASID,
                                                                                               self.X,
                                                                                               self.W,
                                                                                               self.R,
                                                                                               self.U,
                                                                                               self.C,
                                                                                               ('unlocked','locked')[self.locked],
                                                                                               self.mapping_group,
                                                                                               ('non-island','island')[self.island],
                                                                                               ('non-reloc','reloc')[self.reloc],
                                                                                               self.sname)

class AddressPatchToken (object):
    """Class representing token for patching (virtual) address
    """
    def __init__ (self, variable_vaddr, mmap_index):
        """Constructor
        """
        self._variable_vaddr = variable_vaddr
        self._mmap_index = mmap_index

    @property
    def variable_vaddr (self):
        return self._variable_vaddr

    @property
    def mmap_index (self):
        return self._mmap_index

class QurtElfPatch:
    """Class for doing ELF patch processing on the merged ELF file."""
    #
    # When we call start() on this class, the class becomes basically like
    #  a 4GB bytearray in the ELF's virtual address space.
    # Slices of the virtual address space can be accessed using standard
    #  slice notation.  For instance, to access the 4 bytes at address ADDR
    #  you can use self[ADDR:ADDR+4].
    # To read the same 4 bytes into X as an unsigned 32-bit quantity:
    #  X = struct.unpack('<L', self[ADDR:ADDR+4])
    # To write the same 4 bytes with X which is an unsigned 32-bit quantity:
    #  self[ADDR:ADDR+4] = struct.pack('<L', X)
    #
    # Note that accesses outside of the defined virtual image of the ELF cause
    #  an exception.  Portions of the virtual image which are not backed by
    #  file data (such as bss sections, etc.) can be read, but return zeroes;
    #  attempts to write non-zero data to those cause an exception.
    #
    def __init__(self):
        self.all_mappings = []
        self.opt_mappings = []
        self.pool_moves = []
        self.clade_regs = [None]*5
        self.extra_mappings = []
        self.ramfs_info = []
        self.vaddr_space = QurtAddressSpace()
        self.vaddr_space.alloc(left= -QurtInfinity,
                               right= 0x1000, reserve_only=True)
        self.vaddr_space.alloc(left=0xFFFFF000,
                               right= QurtInfinity, reserve_only=True)
        self.physpool_space = QurtAddressSpace()
        self.deferred_writes = []
    def __getitem__(self, key):
        from struct import unpack, calcsize
        cvt = False
        if isinstance(key,slice):
            sz = key.stop - key.start
            addr = key.start
        elif isinstance(key[1], basestring):
            sz = calcsize(key[1])
            addr = key[0]
            cvt = True
        else:
            sz = key[1]
            addr = key[0]
        ret = ''
        while sz > 0:
            this_sz = 0
            for p in self.infiles[0].phdr:
                if addr >= p.p_vaddr:
                    if addr < p.p_vaddr + p.p_filesz:
                        # The next byte we need to read is in this segment
                        this_sz = min(sz, (p.p_vaddr + p.p_filesz) - addr)
                        this_offset = p.p_offset + (addr - p.p_vaddr)
                        self.infiles[0].seek(this_offset)
                        tmp = self.infiles[0].read(this_sz)
                        this_sz = len(tmp)
                        addr += this_sz
                        sz -= this_sz
                        ret += tmp
                        if sz == 0:
                            break
                    if addr < p.p_vaddr + p.p_memsz:
                        # The next byte we need to read is in the ZERO padding
                        this_sz = min(sz, (p.p_vaddr + p.p_memsz) - addr)
                        addr += this_sz
                        sz -= this_sz
                        ret += '\0' * this_sz
                        break
            if this_sz == 0:
                raise Exception('Cannot read %u bytes at vaddr %X' % (sz, addr))
        if cvt:
            ret = unpack(key[1], ret)
        return ret
    def __setitem__(self, key, value):
        # If value is not a string, assume it's an iterable (most likely a tuple)
        #  appropriate for being passed as the argument list to struct.pack
        if not isinstance(value, basestring):
            from struct import pack
            value = pack(*value)
        sz = len(value)
        if isinstance(key,slice):
            addr = key.start
        else:
            addr = key[0]
        if not hasattr(self, 'file'):
            self.deferred_writes.append((addr,value))
            return
        while sz > 0:
            this_sz = 0
            for p in self.phdr:
                if addr >= p.p_vaddr:
                    if addr < p.p_vaddr + p.p_filesz:
                        # The next byte we need to read is in this segment
                        this_sz = min(sz, (p.p_vaddr + p.p_filesz) - addr)
                        this_offset = p.p_offset + (addr - p.p_vaddr)
                        self.file.seek(this_offset)
                        self.file.write(value[:this_sz])
                        self.file.flush()
                        addr += this_sz
                        sz -= this_sz
                        value = value[this_sz:]
                        break
            if this_sz == 0:
                raise Exception('Cannot read %u bytes at vaddr %X' % (sz, addr))
    def parse_args(self,argv):
        from optparse import OptionParser, OptionGroup
        parser = OptionParser(usage='%prog [parameters] [[flags...] input_file]...')

        def add_flag(parser, key):
            if len(parser.largs) == 0 or not isinstance(parser.largs[-1],list):
                parser.largs.append(list())
            parser.largs[-1].append(key)

        def opt_callback_align(option, opt_str, value, parser):
            try:
                value = convert_size(value.upper())
            except ValueError:
                parser.error('Invalid format for --align')
            add_flag(parser, 'align')
            add_flag(parser, value)

        def opt_callback_file(option, opt_str, value, parser):
            add_flag(parser, 'ramfs')

        def opt_callback_contig(option, opt_str, value, parser):
            add_flag(parser, 'contig')

        def opt_callback_restart(option, opt_str, value, parser):
            add_flag(parser, 'restartable')

        group = OptionGroup(parser, 'Required Parameters')
        group.add_option('-o', action='store', dest='outfilename',
                         help='write output to FILENAME', metavar='FILENAME')
        group.add_option('-p', action='store', dest='physaddr',
                         help='set base physical address of output file to PHYSADDR', metavar='PHYSADDR')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Optional Parameters')
        group.add_option('-f', action='store_true', dest='no_altscript',
                         help='skip check for an alternate builder script')
        group.add_option('-n', action='store_true', dest='no_adjust',
                         help='skip writing _reloc versions of input files')
        group.add_option('--assert_free_microimage', action='store', dest='free_uimg', default='0',
                         help='fail the build if the amount of microimage free space falls below SIZE', metavar='SIZE')
        group.add_option('--qiblog', action='store', dest='qiblog_filename', default=None,
                         help='name of output XML file describing the output ELF file', metavar='FNAME')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Flags affecting the following input file')
        group.add_option('-R', action='callback', callback=opt_callback_restart,
                         help='mark the next input file as a restartable ELF')
        group.add_option('--file', action='callback', callback=opt_callback_file,
                         help='mark the next input file as a RAMFS file')
        group.add_option('--align', action='callback', callback=opt_callback_align, type='string',
                         help='mark the next input file as being aligned to BOUNDARY in physical memory', metavar='BOUNDARY')
        group.add_option('--contiguous', action='callback', callback=opt_callback_contig,
                         help='mark the next input file as being contiguous in physical memory')
        parser.add_option_group(group)

        group = OptionGroup(parser, 'Deprecated Parameters',
                            'These are allowed but ignored')
        group.add_option('-e', action='store_true',
                         help='request old EIP format')
        group.add_option('-t', action='store',
                         help='use Hexagon tools at TOOLSDIR', metavar='TOOLSDIR')
        parser.add_option_group(group)

        (opts, args) = parser.parse_args()

        # Put all of the arguments where they belong:

        self.outfilename = opts.outfilename
        if self.outfilename == None:
            parser.error('Must provide an output file name using -o')

        if opts.qiblog_filename:
            self.qiblog_filename = opts.qiblog_filename
        else:
            self.qiblog_filename = self.outfilename + '.qiblog.xml'

        try:
            if opts.physaddr == None:
                raise ValueError()
            self.physaddr = convert_size(opts.physaddr.upper())
            if (self.physaddr & 0xFFFFF000) != self.physaddr:
                raise ValueError()
        except ValueError:
            parser.error('Must provide a valid 4K-aligned physical address using -p')

        try:
            self.free_uimg = convert_size(opts.free_uimg.upper())
        except ValueError:
            parser.error('Invalid format for --assert_free_microimage')

        self.no_altscript = opts.no_altscript
        self.write_reloc = not opts.no_adjust

        if len(args) == 0:
            parser.error('At least one input ELF must be provided')

        if isinstance(args[-1],list):
            parser.error('Flags found with no file name following')

        self.infiles = []
        self.ramfs = []
        flags = []
        for x in args:
            if isinstance(x,list):
                flags = x
            elif 'ramfs' in flags:
                self.ramfs.append((x,flags))
                flags = []
            else:
                self.infiles.append(Elf_File(self, *splitname(x, 'restartable' in flags)))
                flags = []

    def handle_debug_sections(self):
        #
        #  Insert any needed debug sections into self.output_sections[] list.
        #
        #  For now, look at the EIP build records for SYMTAB and STRTAB records.
        #
        from StringIO import StringIO
        if self.strip_debug_info.value == 'false':
            return
        symtab_records = list(self.eip_build_records(EIP_BUILD_SYMTAB))
        strtab_records = list(self.eip_build_records(EIP_BUILD_STRTAB))
        counts = (len(symtab_records), len(strtab_records))
        if counts == (0,0):
            return
        if counts != (1,1):
            raise Exception('Count of EIP build records is bogus: (symtab = %u, strtab = %u)' % counts)

        symtab_string = symtab_records[0][0]      # It's a list containing a tuple containing a string
        strtab_string = strtab_records[0][0]
        syms = []
        while symtab_string:
            syms.append(Elf32Sym(symtab_string[:16]))
            symtab_string = symtab_string[16:]
        #
        #  Strip out symbols which have zero value; those are weak symbols which didn't resolve.
        #  Start at index 1 because index 0 is reserved and all zeroes.
        #
        syms[1:] = [x for x in syms[1:] if x.st_value != 0]
        symtab_string = ''.join([x.output() for x in syms])

        #
        #  Okay, the symtab_string and strtab_string are ready to get inserted.
        #

        self.shdr.append(Elf32SectionHeader(None,
                                    name = '.symtab',
                                    f = StringIO(symtab_string),
                                    sh_name = 0,
                                    sh_type = Elf32SectionHeader.SHT_SYMTAB,
                                    sh_flags = 0,
                                    sh_addr = 0,
                                    sh_offset = 0,
                                    sh_size = len(symtab_string),
                                    sh_link = 123456,           # Index must match strtab's shindex
                                    sh_info = 1,                # Index of first non-local symbol
                                    sh_addralign = 4,           # Alignment requirement
                                    sh_entsize = 16))           # Each entry is 16 bytes
        self.shdr.append(Elf32SectionHeader(None,
                                    name = '.strtab',
                                    f = StringIO(strtab_string),
                                    shindex = 123456,           # Magic number that matches above
                                    sh_name = 0,
                                    sh_type = Elf32SectionHeader.SHT_STRTAB,
                                    sh_flags = 0,
                                    sh_addr = 0,
                                    sh_offset = 0,
                                    sh_size = len(strtab_string),
                                    sh_link = Elf32SectionHeader.SHN_UNDEF,
                                    sh_info = 0,
                                    sh_addralign = 1,
                                    sh_entsize = 0))

    def unpack_from(self, fmt, offset):
        return self[(offset,fmt)]
    def finish(self):
        #
        # Write the following in order:
        #  * ELF Header
        #  * Program Headers
        #  * Section Headers
        #  * String Table
        #  * Comment Section contents
        #  * Program Header contents
        #
        # After all have been written, rewrite the ELF Header,
        #  Program Headers, and Section Headers, as they've
        #  now been updated with their final file offsets.
        #
        import shutil

        nullsection = Elf32SectionHeader(None,
                                         name = '',
                                         sh_name = 0,
                                         sh_type = Elf32SectionHeader.SHT_NULL,
                                         sh_flags = 0,
                                         sh_addr = 0,
                                         sh_offset = 0,
                                         sh_size = 0,
                                         sh_link = Elf32SectionHeader.SHN_UNDEF,
                                         sh_info = 0,
                                         sh_addralign = 0,
                                         sh_entsize = 0)

        strtabsection = Elf32SectionHeader(None,
                                           name = '.shstrtab',
                                           sh_name = 0,
                                           sh_type = Elf32SectionHeader.SHT_STRTAB,
                                           sh_flags = 0,
                                           sh_addr = 0,
                                           sh_offset = 0,
                                           sh_size = 0,
                                           sh_link = Elf32SectionHeader.SHN_UNDEF,
                                           sh_info = 0,
                                           sh_addralign = 1,
                                           sh_entsize = 0)

        self.shdr = [s for s in self.shdr if not hasattr(s,'s_internal')]
        if self.strip_debug_info.value == 'false':
            #self.shdr = self.infiles[0].debugsections + self.shdr
            self.shdr.extend(self.infiles[0].debugsections)

        self.shdr.insert(0,nullsection)
        self.shdr.insert(1,strtabsection)
        self.handle_debug_sections()
        self.phdr = sorted(self.phdr, key=lambda p: p.p_paddr)
        self.file = open(self.outfilename, 'wb')
        f = self.file
        e = self.ehdr

        #
        #  Set last PHDR to fill memory out to end of DEFAULT_PHYSPOOL
        #

        if self.image_elf_padded.value:
            real_phdrs = [p for p in self.phdr if not p.temp]
            last_phdr = real_phdrs[-1]
            last_phdr.p_memsz = self.physaddr_end - last_phdr.p_paddr

        #
        #  Build the dictionary for mapping input section headers to output section headers
        #
        sidict = dict()
        j = 0
        for s in self.shdr:
            if s.output():
                oldindex = getattr(s,'shindex',0)
                if oldindex:
                    sidict[oldindex] = j
                j += 1

        #
        #  Fix up section link numbers
        #
        for s in self.shdr:
            s.sh_link = sidict.get(s.sh_link, s.sh_link)

        #
        #  Fix up symbol table link numbers
        #

        for s in self.shdr:
            if s.name == '.symtab':
                symreader = s.contents()
                syms = [Elf32Sym(symreader) for _ in range(s.sh_size/16)]
                for sym in syms:
                    sym.st_shndx = sidict.get(sym.st_shndx, sym.st_shndx)
                newsymtab = ''.join([sym.output() for sym in syms])
                import StringIO
                s.sh_offset = 0
                s.f = StringIO.StringIO(newsymtab)

        BookmarkWrite("EHDR", f, e.output())
        (e.e_phoff, e.e_phnum) = BookmarkWrite("PHDR", f, ''.join([p.output() for p in self.phdr]))
        (e.e_shoff, e.e_shnum) = BookmarkWrite("SHDR", f, ''.join([s.output() for s in self.shdr]))
        e.e_phnum /= e.e_phentsize
        e.e_shnum /= e.e_shentsize
        e.e_shstrndx = 1
        (strtabsection.sh_offset, strtabsection.sh_size) = BookmarkWrite("_", f, strtabify(self.shdr))
        for p in self.phdr:
            if not p.temp:
                if p.p_align > 1:
                    f.write('\0' * ((p.p_paddr - f.tell()) % p.p_align))
                saveloc = f.tell()
                shutil.copyfileobj(p.contents(), f)
                p.p_offset = saveloc
                for s in p.section_list:
                    s.sh_offset = p.p_offset + (s.sh_addr - p.p_vaddr)
        for s in self.shdr[2:]:
            if (s.sh_flags & s.SHF_ALLOC) == 0:
                if s.sh_addralign > 1:
                    while (f.tell() % s.sh_addralign) != 0:
                        f.write('\0')
                (s.sh_offset, _) = BookmarkWrite("_", f, s.contents().read())

        #
        # Temp hack to write adjusted PHDRs
        #
        for p in self.phdr:
            p.p_paddr += 0x0000000      # Add 16MB to every paddr in the program headers
        e.e_entry += 0x0000000          # Add 16MB to entry point

        BookmarkWrite("@EHDR", f, e.output())
        BookmarkWrite("@PHDR", f, ''.join([p.output() for p in self.phdr]))
        BookmarkWrite("@SHDR", f, ''.join([s.output() for s in self.shdr]))

        f.flush()
        for t in self.deferred_writes:
            self[(t[0],len(t[1]))] = t[1]
        f.flush()
        f.close()
    def add_segment(self, vaddr, paddr, size, contents, sectionname=None, executable=False, writable=False):
        #
        # Add a new segment (and section) to the output ELF file.
        # The virtual address, physical address, and size are as specified.
        # The "contents" argument must be a file-like object.
        #
        # Optional arguments include:  sectionname (if omitted, a default is used)
        #                              executable (False by default)
        #                              writable (False by default)
        #
        # TBD:  Do we ever need to specify alignment different than 0x1000?
        #
        if sectionname == None:
            self.qibsectioncount = getattr(self,'qibsectioncount',0) + 1
            sectionname = '.qib.%u' % self.qibsectioncount

        p = self.phdr[0].copy()
        p.p_type = p.PT_LOAD
        p.p_offset = 0                  # Start at beginning of contents
        p.p_vaddr = vaddr
        p.p_paddr = paddr
        p.p_filesz = size
        p.p_memsz = size
        p.p_flags = 4                   # Readable
        if writable:
            p.p_flags |= 2              # Writable
        if executable:
            p.p_flags |= 1              # Executable
        p.p_align = 0x1000

        s = self.shdr[0].copy()
        s.name = sectionname
        s.sh_type = s.SHT_PROGBITS
        s.sh_flags = s.SHF_ALLOC
        s.sh_addr = vaddr
        s.sh_offset = 0                 # Start at beginning of contents
        s.sh_size = size
        s.sh_link = s.SHN_UNDEF
        s.sh_info = 0
        s.sh_addralign = 1
        s.sh_entsize = 0
        p.section_list = [s]

        self.phdr.append(p)
        self.shdr.append(s)
        p.f = contents

    def read_boot_mappings(self):
        # Get the boot mappings from the ELF file
        addr = self.eip_addr_boot_maps
        boot_maps = []
        while True:
            tmp = self.unpack_from('<QLL', addr)
            if tmp[0] == 0:
                break
            boot_maps.append(tmp)
            addr += 16
        self.boot_mappings = boot_maps
    def read_tlb_dump(self):
        self.compare_tlbs = []
        if self.tlbdump_usage.value == 'ignore':
            return None
        addr = self.eip_addr_tlb_dump
        tlb_dump = []
        for i in range(self.eip_size_tlb_dump):
            tmp = self.unpack_from('<Q', addr)[0]
            if (tmp & (1L << 63)):
                if self.tlbdump_usage.value == 'compare':
                    self.compare_tlbs.append(tmp)
                else:
                    tlb_dump.append(HexagonMapping().set_from_TLB_entry(tmp))
            addr += 8
        self.tlb_dump = tlb_dump
        self.all_mappings.extend(tlb_dump)
    def do_tlb_compare(self, v, vn):
        if not self.compare_tlbs:
            return
        original = self.compare_tlbs
        updated = v+vn
        total = sorted(original+updated)
        for t in total:
            if not t in updated:
                print 'Original only: %s' % HexagonMapping().set_from_TLB_entry(t)
            if not t in original:
                print 'Updated  only: %s' % HexagonMapping().set_from_TLB_entry(t)
    def write_tlb_dump(self, v, vn):
        addr = self.eip_addr_tlb_dump
        if len(v)+len(vn) > self.eip_size_tlb_dump:
            raise Exception('Build generates %u static TLB entries -- only %u are allowed' %
                            (len(v)+len(vn), self.eip_size_tlb_dump))
        if len(v)+len(vn) > self.tlb_limit.value:
            raise Exception('Build generates %u total TLB entries, exceeds XML configured limit of %u' %
                            (len(v)+len(vn), self.tlb_limit.value))
        for tmp in v:
            self[(addr,8)] = ('<Q', tmp)
            addr += 8
        for tmp in vn:
            self[(addr,8)] = ('<Q', tmp)
            addr += 8
        self[(self.eip_addr_tlb_relocs,4)] = ('<L', len(v))
    def write_tlb_reclaim(self, mask):
        if self.qurtos_tlb_reclaim:
            self[(self.qurtos_tlb_reclaim,16)] = ('<QQ',
                                                  (mask >>  0) & ((1L << 64) - 1),
                                                  (mask >> 64) & ((1L << 64) - 1))
    def write_quick_mmu(self, i):
        addr = self.eip_addr_tlb_dump
        pg = (addr >> 12)
        for t in self.opt_mappings:
            if pg >= t.vpage and pg < t.vpage + t.size and t.locked:
                val = (addr - (t.vpage << 12)) + (t.ppage << 12)
                if t.reloc:
                    val += 1
                self[(self.eip_addr_quick_mmu,4)] = ('<L', val)
    def read_mmap_table(self):
        """Read QuRT memory map table (qurtos_mmap_table), splits it into
           two - self.mmap_table_locked (locked mapping) and
           self.mmap_table_uncloedk (unlocked mappings) and appends them
           to self.all_mappings. Also reserves virtual address ranges for
           these mappings (?)
        """
        lock_count = self.unpack_from('<L', self.eip_addr_tlblock)[0]
        addr = self.eip_addr_mmap_table
        mmap_table_locked = []
        mmap_table_unlocked = []
        while True:
            tmp = self.unpack_from('<Q', addr)[0]
            if tmp == 0:
                break
            if len(mmap_table_locked) < lock_count:
                if self.tlbdump_usage.value == 'compare':
                    self.compare_tlbs.append(tmp)
                mapping = HexagonMapping().set_from_TLB_entry(tmp, locked=True)
                mmap_table_locked.append( mapping )
            else:
                mapping = HexagonMapping().set_from_TLB_entry(tmp, locked=False, mapping_group='default')
                mmap_table_unlocked.append( mapping )

            index = ( addr - self.eip_addr_mmap_table ) / 8
            if index in self.patchable_mmaps.keys():
                mapping.add_token( AddressPatchToken( self.patchable_mmaps[ index ], index ) )

            addr += 8

        self.mmap_table_locked = mmap_table_locked
        self.mmap_table_unlocked = mmap_table_unlocked
        for x in mmap_table_locked+mmap_table_unlocked:
            if x.has_token( AddressPatchToken ):
                # Cant reserve/allocate virtual address range yet
                pass
            else:
                self.vaddr_space.alloc(size=x.size << 12,
                                       addr=x.vpage << 12,
                                       mmap=x)

        self.all_mappings.extend(mmap_table_locked)
        self.all_mappings.extend(mmap_table_unlocked)

    def patch_mmap_table (self):
        """Patch memory map table. For locked entry, VA is patched in
           self.all_mappings[]. For unlocked entry, VA is patched
           in qurtos_mmap_table[] via self.eip_addr_mmap_table
        """
        for mapping in self.all_mappings:
            if mapping.has_token( AddressPatchToken ):
                token = mapping.get_token( AddressPatchToken )
                if not token:
                    raise ValueError( "Unable to find instance of AddressPatchToken in mapping" )
                old_vpage = mapping.vpage
                size = mapping.size << 12
                vaddr = self.vaddr_space.find_optimal( size ).addr
                self[(token.variable_vaddr,4)] = ( '<L', vaddr )
                if mapping.locked:
                    # Patch VA in the mapping. It will eventually make it into
                    # TLB dump table
                    mapping.vpage = vaddr >> 12
                else:
                    # Patch VA in qurtos_mmap_table[]
                    index = token.mmap_index
                    addr  = self.eip_addr_mmap_table + ( index * 8 )
                    raw_tlb_entry = self.unpack_from('<Q', addr )[ 0 ]
                    tmp_map = HexagonMapping().set_from_TLB_entry( raw_tlb_entry )
                    tmp_map.vpage = vaddr >> 12
                    raw_tlb_entry = tmp_map.get_TLB_entry()
                    self[(addr,8)] = ( '<Q', raw_tlb_entry )
                    self.eip.append( eip_q6cfg( index, eip_q6cfg.UNLOCKED ) )
                    mapping.del_token( token )

                print "Patched mapping. vpage=%05X (old) vpage=%05X ppage=%05X size=%u locked=%u" % ( old_vpage, mapping.vpage, mapping.ppage, size, mapping.locked )

    def read_pool_configs(self):
        self.pools = []
        addr = self.pool_configs
        if addr == 0:
            return
        # Count how many pages to take off the end of the first pool
        remove_pages = int(self.total_memsize,0) - int(self.effective_memsize,0)
        while True:
            tmp = self.unpack_from('<32s32L', addr)
            if tmp[0].startswith('\0'):
                break
            if remove_pages:
                # Remove from the end of the first region
                tmp = list(tmp)
                tmp[2] -= remove_pages << 12
                remove_pages = 0
            addr += 160
            try:
				# pool_relocs is populated by qurt_config
				# It contains a bit mask of all the ranges in a pool 
				# that has relocatable addresses
                self.pools.append(PhysPool(tmp, self.pool_relocs))
            except AttributeError:
                self.pools.append(PhysPool(tmp))
        s = map(int,self.region_cache_policies.split())
        for pool_num,region_num,cache_policy in zip(*[iter(s)]*3):
            self.pools[pool_num].set_range_policy(region_num, cache_policy)

    def do_fixups(self):
        for i in self.infiles:
            # For all sections with names of the form '.ukernel.*' or '.start'
            #  turn off the writeable flag.
            for s in i.shdr:
                sn = s.name
                if sn.startswith('.ukernel.') or sn == '.start':
                    s.sh_flags &= ~s.SHF_WRITE

            max_phys = 0
            for p in i.phdr:
                if p.p_type == p.PT_LOAD:
                    max_phys = max(max_phys, p.p_paddr + p.p_memsz)

            # Look for a PT_LOAD program header which spans .start address;
            #  if we find one, split it into two pieces.
            for k in range(len(i.phdr)):
                p = i.phdr[k]
                if p.p_paddr < i.start_addr:
                    if p.p_paddr + p.p_memsz > i.start_addr:

                        p0 = p.copy()
                        p0.p_memsz = i.start_addr - p.p_paddr
                        p0.p_filesz = min(p0.p_filesz, p0.p_memsz)

                        p1 = p.copy()
                        p1.p_memsz -= p0.p_memsz
                        p1.p_filesz -= p0.p_filesz
                        #p1.p_offset += p0.p_memsz
                        p1.p_paddr += p0.p_memsz
                        p1.p_vaddr += p0.p_memsz

                        i.phdr.pop(k)
                        i.phdr.insert(k, p1)
                        i.phdr.insert(k, p0)

            # For any PHDRs which fall before the start address, roll their
            #  physical address to the next aligned physical address past
            #  max_phys

            for p in i.phdr:
                if p.p_paddr < i.start_addr:
                    pad = (p.p_paddr - max_phys) & 0xFFF
                    max_phys += pad
                    p.p_paddr = max_phys
                    max_phys += p.p_memsz

    def process_input_files(self):
        ''' Figure out where we put it in memory '''
        curloc = self.physaddr
        for f in self.infiles:
            mbot = 0
            mtop = 0
            malign = 0
            for p in f.phdr:
                if (p.p_type == Elf32ProgramHeader.PT_NULL):
                    continue
                if mtop == 0:
                    mbot = p.p_paddr
                    mtop = p.p_paddr+p.p_memsz
                    malign = p.p_align
                else:
                    mbot = min(mbot, p.p_paddr)
                    mtop = max(mtop, p.p_paddr+p.p_memsz)
                    malign = max(malign,p.p_align)
            if malign == 0:
                malign = 1
            ''' Need to increase curloc until it is congruent to mbot modulo malign '''
            curloc += ((mbot%malign)+malign-(curloc%malign))%malign
            f.output_physaddr = curloc
            f.vmem_bot = mbot
            f.vmem_top = mtop
            f.memalign = malign
            curloc += (mtop-mbot)
            ''' Round up to next 1MB boundary '''
            curloc = (curloc+0xFFFFF) & ~0xFFFFF

    def do_section_overrides(self):
        infiles = self.infiles

        for i in infiles:
            for s in i.shdr:
                s.root_elf = (i == infiles[0])

        for i in infiles:
            for s in i.shdr:
                for o in self.section_info:
                    if o.match(o,None):
                        for k,v in o.attrs.items():
                            v(s)
        for i in infiles:
            for s in i.shdr:
                for o in self.section_info:
                    if o.match(o,s.name):
                        for k,v in o.attrs.items():
                            v(s)

        for i in infiles:
            for s in i.shdr:
                s.need_map = (s.sh_flags & s.SHF_ALLOC) != 0 and s.sh_size > 0
                mapattr = getattr(s,'mapping','normal')
                s.overlay = (0,1)[mapattr == 'overlay']
                if not hasattr(s, 'mapping_group'):
                    s.mapping_group = 'default'

        if True:
            #
            #  We've been asked to do unrestricted gap reclaiming.
            #  For each of our ELF files, fill in the holes in
            #   the virtual address space with read-only gaps.
            #  Do this for both the program headers and the
            #   section headers.
            #

            for i in infiles:
                # Get the sorted list of phdrs which are of type 1 (LOAD)
                #  and of non-zero memory size
                zp = sorted(i.phdr, key=lambda p: p.p_vaddr)
                # Add a bogus entry that starts at the next 1MB boundary past the end; this
                #  will cause a gap of up to 1MB to be added at the end.
                p = zp[-1].copy()
                p.p_vaddr = alignup(p.p_vaddr + p.p_memsz, 0x100000)
                p.p_paddr = alignup(p.p_paddr + p.p_memsz, 0x100000)
                if i is infiles[0]:
                    zp.append(p)
                for p0,p1 in zip(zp[:-1],zp[1:]):
                    gap = p1.p_vaddr - (p0.p_vaddr + p0.p_memsz)
                    if gap < 0:
                        raise Exception("Overlapping LOAD program headers?")
                    if gap > 0:
                        # There is a gap between two LOAD program headers
                        #  in virtual memory.
                        # Add a new phdr to fill the gap, with zero file size
                        #  and with read-only permissions.  This is just our
                        #  internal representation -- it shouldn't survive
                        #  into the output ELF file.
                        # To make sure we can avoid propagating it to the
                        #  output ELF, set its 'temp' attribute to True.
                        p = p1.copy()
                        p.p_vaddr -= gap
                        p.p_paddr -= gap
                        p.p_memsz = gap
                        p.p_filesz = 0
                        p.p_flags = 0
                        p.temp = True
                        p.section_list = []
                        i.phdr.append(p)
                        if True:
                            self.vaddr_space.alloc(size=p.p_memsz,
                                                   addr=p.p_vaddr,
                                                   phdr=p)

                # Get the sorted list of shdrs which have the ALLOC flag set
                #  and which are of non-zero size
                zs = sorted([s for s in i.shdr if s.need_map],
                            key=lambda s: s.sh_addr)
                # Add a bogus entry that starts at the next 1MB boundary past the end; this
                #  will cause a gap of up to 1MB to be added at the end.
                s = zs[-1].copy()
                s.sh_addr = alignup(s.sh_addr + s.sh_size, 0x100000)
                if i is infiles[0]:
                    zs.append(s)
                for s0,s1 in zip(zs[:-1],zs[1:]):
                    gap = s1.sh_addr - (s0.sh_addr + s0.sh_size)
                    if gap < 0:
                        raise Exception("Overlapping ALLOC section headers?")
                    if gap > 0:
                        # There is a gap between two ALLOC section headers
                        #  in virtual memory.
                        # Add a new shdr to fill the gap, with NOBITS/ALLOC
                        #  and with read-only permissions.  This is just our
                        #  internal representation -- it shouldn't survive
                        #  into the output ELF file.
                        # To make sure we can avoid propagating it to the
                        #  output ELF, set its 'temp' attribute to True.
                        s = s1.copy()
                        s.sh_addr -= gap
                        s.sh_size = gap
                        s.sh_type = s.SHT_NOBITS
                        s.sh_flags &= ~(s.SHF_WRITE | s.SHF_EXECINSTR)
                        s.temp = True
                        s.name = '$GAP%08X' % s.sh_addr
                        s.cache_policy = -1
                        s.overlay = -1
                        s.need_map = True
                        s.physpool = '*'
                        i.shdr.append(s)

    def synthesize_elf_mappings(self, infiles):
        asid = 0
        tb_virtual_start, tb_virtual_end = (0,0)
        tb_physical_start, tb_physical_end = (0,0)
        for addr, size, xmlstr in self.eip_build_records(3, 'LL'):
            tb_virtual_start, tb_virtual_end = (addr, addr+size)
            break
        for i in infiles:
            for s in i.shdr:
                if (s.sh_flags & s.SHF_ALLOC) != 0 and s.sh_size > 0:
                    sname = s.name
                    match = []
                    for p in self.phdr:
                        if s.sh_addr >= p.p_vaddr:
                            if s.sh_addr + (s.need_map * s.sh_size) <= p.p_vaddr + p.p_memsz:
                                match.append(p)
                    if len(match) != 1:
                        print 'PHDRS:'
                        for p in sorted(self.phdr, key=lambda p: p.p_vaddr):
                            print '    %s' % p.oneline()
                        print 'SHDRS:'
                        for s in sorted(i.shdr, key=lambda s: s.sh_addr):
                            if (s.sh_flags & s.SHF_ALLOC) != 0 and s.sh_size > 0:
                                print '    %s' % s.oneline()
                        raise Exception('Cannot find unique PHDR for section %s' % sname)
                    p = match[0]
                    paddr = s.sh_addr + p.p_paddr - p.p_vaddr
                    perm_write = ((s.sh_flags & s.SHF_WRITE) != 0)
                    perm_exec = ((s.sh_flags & s.SHF_EXECINSTR) != 0)
                    skip = False
                    if tb_virtual_start < tb_virtual_end:
                        if s.sh_addr <= tb_virtual_start:
                            if tb_virtual_end <= (s.sh_addr+s.sh_size):
                                # Trace buffer lies within this section
                                # Remember its physical address range
                                tb_physical_start = tb_virtual_start + p.p_paddr - p.p_vaddr
                                tb_physical_end = tb_virtual_end + p.p_paddr - p.p_vaddr
                    for z in self.boot_mappings:
                        if s.sh_addr >= z[1] and s.sh_addr < z[2]:
                            skip = True
                    if not skip:
                        self.add_elf_mapping(s, s.sh_addr, s.sh_size, paddr, perm_write, perm_exec, asid, sname.endswith('.island'), i)
            asid += 1
        self.tb_physical_start = tb_physical_start
        self.tb_physical_end = tb_physical_end
        print 'Trace buffer physical bounds: %X %X' % (self.tb_physical_start, self.tb_physical_end)
    def map_trace_buffer(self):
        if self.tb_physical_start == 0 and self.tb_physical_end == 0:
            return
        start = aligndown(self.tb_physical_start, 0x100000)
        end = alignup(self.tb_physical_end, 0x100000)
        vrange = self.vaddr_space.find_optimal(end-start)
        map = HexagonMapping()
        map.size = vrange.size >> 12
        map.vpage = vrange.addr >> 12
        map.ppage = start >> 12
        map.locked = True
        map.reclaim = False
        map.reloc = self.relocatable_image.value
        map.mapping_group = UniqueString()
        map.G = 1
        map.A1A0 = 0
        map.ASID = 0
        map.X = 0
        map.W = 0
        map.R = 0
        map.U = 0
        map.C = 6        # uncached
        map.overlay = 0
        map = map.fix_to_TLB_entries(maxsize = self.max_elf_tlb.value/0x1000)
        self.all_mappings.extend(map)
        self.opt_mappings.extend(map)
        tb_virtual_start = 0
        for addr, size, xmlstr in self.eip_build_records(3, 'LL'):
            adjust = (vrange.addr - start) + (self.tb_physical_start - addr)
            break
        else:
            adjust = 0
        for addr, size, data in self.eip_build_records(4, 'LL'):
            for a in range(addr, addr+size, 4):
                original_vaddr = self.unpack_from('<L', a)[0]
                if original_vaddr == 0:
                    #
                    #  Don't adjust NULL pointers.
                    #
                    continue
                new_vaddr = original_vaddr + adjust
                print 'Adjusting uncached mapping, was %X, is now %X' % (original_vaddr, new_vaddr)
                self[(a,4)] = ('<L', new_vaddr)
    def add_elf_mapping(self, section, vaddr, size, paddr, perm_write, perm_exec, asid, island, infile):
        # Round beginning down to 4K boundary
        size += (vaddr & 0xFFF)
        paddr -= (vaddr & 0xFFF)
        vaddr -= (vaddr & 0xFFF)
        # Convert size to page count
        pgcnt = (size + 0xFFF) >> 12
        if (paddr & 0xFFF) != 0:
            raise Exception('Virtual/physical address mis-alignment')
        vpage = vaddr >> 12
        ppage = paddr >> 12
        tmp = HexagonMapping()
        tmp.G = int(asid == 0)
        tmp.A1A0 = 0
        tmp.ASID = asid
        tmp.X = int(perm_exec)
        tmp.W = int(perm_write)
        tmp.R = 1
        tmp.U = int(getattr(section,'user_mode',asid != 0))
        tmp.C = int(getattr(section,'cache_policy','7'))    # Default L2 cacheable
        tmp.vpage = vpage
        tmp.ppage = ppage
        tmp.size = pgcnt
        tmp.locked = getattr(section,'locked',(asid==0))
        tmp.reclaim = getattr(section,'reclaim',False)
        tmp.mapping_group = section.mapping_group
        tmp.island = island
        tmp.reloc = self.relocatable_image.value
        tmp.infile = infile
        tmp.overlay = section.overlay
        tmp.physpool = getattr(section,'physpool',None)
        tmp.range_idx = getattr(section,'range_idx',None)
        tmp.clade_reg = getattr(section,'clade_reg',None)
        tmp.copybytes = getattr(section,'copybytes',8)
        tmp.sname = section.name
        tmp = tmp.fix_to_TLB_entries(maxsize = self.max_elf_tlb.value/0x1000)
        self.all_mappings.extend(tmp)
    def sort_mappings(self):
        """Sort the all_mappings array."""
        for t in self.all_mappings:
            t.sortkey = (t.vpage << 32) + (t.size)
        self.all_mappings.sort(key=lambda x: x.sortkey)
    def get_ranges(self):
        """Compute the list of virtual address ranges to be mapped."""
        current_range = None
        current_group = None
        all_ranges = []
        for t in self.all_mappings:
            if t.mapping_group != current_group:
                if current_range:
                    all_ranges.append(current_range)
                current_range = [t.vpage, t.vpage+t.size, [t]]
                current_group = t.mapping_group
            elif t.vpage <= current_range[1]:
                current_range[1] = max(current_range[1], t.vpage+t.size)
                current_range[2].append(t)
            else:
                all_ranges.append(current_range)
                current_range = [t.vpage, t.vpage+t.size, [t]]
        if current_range:
            all_ranges.append(current_range)
        self.all_ranges = all_ranges
    def get_map(self, vaddr, size, mappings, verbose=False):
        # Get the list of mappings that overlap the proposed (vaddr,size)
        overlap = []
        for t in mappings:
            if t.vpage + t.size <= vaddr:
                continue
            if t.vpage >= vaddr + size:
                continue
            overlap.append(t)
        if not overlap:
            raise Exception('Internal error in image builder -- hole found in range')
        if verbose:
            print 'XML info:'
            print self.xml.toxml('utf-8')
            print 'Conflicting mapping requests:'
            for t in overlap:
                print '*** %s' % t
        from copy import copy
        proposed = copy(overlap[0])
        proposed.ppage += (vaddr - proposed.vpage)
        proposed.vpage = vaddr
        proposed.size = size
        for t in overlap[1:]:
            overs = sorted([proposed.overlay, t.overlay])
            if overs[1] == 1 and overs[0] < 1:
                return False
            if max(overs) == 1:
                continue                 # All overlays are compatible with each other
            if proposed.vpage - proposed.ppage != t.vpage - t.ppage:
                return False
            if proposed.physpool != t.physpool:
                if t.physpool == '*':
                    pass
                elif proposed.physpool == '*':
                    proposed.physpool = t.physpool
                else:
                    return False
            if proposed.G != t.G:
                return False
            if proposed.A1A0 != t.A1A0:
                return False
            if proposed.ASID != t.ASID:
                return False
            if proposed.R != t.R:
                return False
            if proposed.C >= 0 and t.C >= 0 and proposed.U != t.U:
                return False
            if proposed.C >= 0 and t.C >= 0 and proposed.C != t.C:
                return False
            if proposed.locked != t.locked:
                pass       # Allow mixing locked and unlocked for now
            if proposed.mapping_group != t.mapping_group:
                return False
            if proposed.island != t.island:
                return False
            if proposed.reloc != t.reloc:
                return False
            if self.allow_map_wx.value != 'true':
                if proposed.X and t.W:
                    return False
                if proposed.W and t.X:
                    return False
            if t.W:
                proposed.W = True
            if t.X:
                proposed.X = True
            if t.locked:
                proposed.locked = True
            if not t.reclaim:
                proposed.reclaim = False
            proposed.C = max(proposed.C, t.C)
            proposed.overlay = max(overs)
        return proposed
    def get_opt_mappings(self):
        for a in self.all_ranges:
            base = a[0]
            nsize = a[1]-a[0]
            while nsize:
                best_map = None
                sz_lo = 0
                sz_hi = nsize
                # Do binary search on the size to find the biggest size
                #  that actually works
                while sz_hi > sz_lo:
                    tmp = (sz_hi + sz_lo + 1) / 2
                    v = self.get_map(base, tmp, a[2])
                    if v:
                        best_map = v
                        sz_lo = tmp
                    else:
                        sz_hi = tmp - 1
                if best_map == None:
                    self.get_map(base, tmp, a[2], verbose=True)
                    ex_str = 'Cannot find compatible mapping for 4K page at virtual address %X' % (base << 12)
                    raise Exception(ex_str)
                if best_map.locked or best_map.island:
                    # Locked mappings must be actually achievable in hardware
                    tmp = 0x1000
                    if best_map.reloc:
                        tmp = self.relocatable_image.align
                    while True:
                        if tmp <= sz_lo:
                            if (best_map.vpage & (tmp-1)) == 0:
                                if (best_map.ppage & (tmp-1)) == 0:
                                    break
                                if best_map.island:
                                    # If physical page is wrong on an island mapping now, it'll get fixed later
                                    #  when we actually allocate space for it in the island.
                                    break
                        tmp = tmp/4
                    sz_lo = tmp                    
                    best_map.size = tmp
                    best_map = self.get_map(best_map.vpage, best_map.size, a[2])
                    best_map.mapping_group = a[2][0].mapping_group
                base += sz_lo
                nsize -= sz_lo
                self.opt_mappings.append(best_map)
    def lookup_vma_to_lma(self, vaddr):
        for p in self.phdr:
            if vaddr >= p.p_vaddr and vaddr < p.p_vaddr+p.p_memsz:
                ret = p.p_paddr + (vaddr - p.p_vaddr)
                return ret
        raise Exception('Error finding LMA for VMA %X' % vaddr)
    def move_pool_mappings(self):
        w = []
        page_avail = 0
        for t in self.opt_mappings:
            if t.physpool and t.physpool != '*':
                w.append(t)
        for t in w:
            tmp = (t.vpage | t.size)
            tmp = (tmp & -tmp)
            t.sortkey = (-tmp, t.vpage)
        w.sort(key=lambda x: x.sortkey)
        self.island_pool_dynamic_usage = 0
        for t in w:
            pp = [p for p in self.pools if p.name == t.physpool]
            if len(pp) != 1:
                raise Exception('Error assigning mapping to pool %s' % t.physpool)
            p = pp[0]

            #Special handling for Island Sections
            if t.island:
                if not True: #This is to make sure that all the island mappings are processed.
				             #We don't use dynamic allocation anymore for user PD RW mappings.
                    self.island_pool_dynamic_usage += t.size
                    continue

            t.reloc, t.ppage, t.C = p.alloc_pages(t.size, t.size, t.range_idx)
            orgaddr = self.lookup_vma_to_lma(t.vpage << 12) + (self.virt_base - self.phys_base)
            if t.reloc == True:
                newpage = (t.ppage << 12) + self.virt_base + 1
                t.ppage += (self.phys_base >> 12)
            else:
                newpage = t.ppage << 12
            self.pool_moves.append([orgaddr, newpage, t.size << 12, t.copybytes])

    def clade_uncomp_offset(self):
        for i in self.infiles:
            for p in i.phdr:
                if p.temp and len(p.section_list):
                    p.p_paddr = 0
                    for sect in p.section_list:
                        if hasattr(sect, 's_internal'):
                            continue
                        if hasattr(sect, 'clade_reg'):
                            re_result = re.match("clade_region_(high|low)_pd(0|1|2|3)",sect.clade_reg)
                            if not re_result:
                                re_result = re.match("clade_region_(high|low)_pd(0|1|2|3)",sect.clade_reg+'_pd0')
                            if re_result:
                                isLow = 0
                                if re_result.group(1) == "low":
                                    isLow = 1
                                pd = int(re_result.group(2))
                                p.p_paddr = self.clade_base_paddr.value + (0x04000000*((2*pd)+isLow))
                                break
                    if p.p_paddr == 0:
                        raise Exception('PT_NULL segment exists without valid clade section')	

    def clade_reg_writes(self):
        addr = None
        clade_struct=["clade_comp","clade_exception_high","clade_exception_low_small","clade_exception_low_large"]
        for i in self.infiles:
            for s in i.shdr:
                if hasattr(s, 's_internal'):
                    continue
                if hasattr(s, 'clade_reg') and hasattr(s, 'phdr_link'):
                    if s.clade_reg in clade_struct:
                        if hasattr(s, 'physpool'):
                            for pool in self.pools:
                                if pool.name == getattr(s, 'physpool'):
                                    self.clade_regs[clade_struct.index(s.clade_reg)] = (pool.ranges[0][0] << 12)
                        else:
                            self.clade_regs[clade_struct.index(s.clade_reg)] = self.lookup_vma_to_lma(s.sh_addr)
        if self.clade_base_paddr.value != 0:
            self.clade_regs[len(self.clade_regs)-1] = self.clade_base_paddr.value
            clade_struct.append("clade_base_paddr")
        #Fail if clade config is not all or nothing
        if ((self.clade_regs.count(None) != 0) and 
            (self.clade_regs.count(None) != len(self.clade_regs))):
            n = 0
            for c in self.clade_regs:
                if c:
                    print "%s: %X" % (clade_struct[n], c)
                else:
                    print "%s: none" % clade_struct[n]
                    self.clade_regs[n] = 0
                n+=1
            print 'Clade configuration is incomplete.'

    def phys_island_report(self):
        pp = [p for p in self.pools if "ISLAND" in p.name.upper()]
        if not pp:
            return
        print 'BOOT island copies'
        for p in pp:
            total_bytes_copied = 0
            total_remaining_region = 0
            for r in p.ranges:
                if r[1] < 0:
                    raise Exception('Problem with fitting all island sections into the island: Exceeded region space')
                total_remaining_region += r[1]
                total_bytes_copied += r[2] - r[1] #+1

            total_remaining_region -= self.island_pool_dynamic_usage
            print '*' * 60
            print 'Total island space copied at boot:      %4dK (%d bytes)' % (total_bytes_copied * 4, total_bytes_copied * 4096)
            print 'Total island space used for spawn:      %4dK (%d bytes)' % (self.island_pool_dynamic_usage * 4, self.island_pool_dynamic_usage * 4096)
            print 'Total island space left in island pool: %4dK (%d bytes)' % (total_remaining_region * 4, total_remaining_region * 4096)
            print '*' * 60
            if total_remaining_region < 0:
                raise Exception('Problem with fitting all island sections into the island')
            if total_remaining_region*4096 < self.free_uimg:
                raise Exception('Failing build due to assert_free_microimage failure\nFree space is less than the requested %s bytes' % self.free_uimg)
    def can_be_reclaimed(self, map):
        # Return false if the map overlaps with any non-reclaim mappings
        # Return true otherwise
        for t in self.all_mappings:
            if t.overlaps(map):
                if not t.reclaim:
                    if t.C >= 0:
                        return False
        return True
    def make_restart_copies(self):
        #
        #  For restartable user ELFs, allocate space for the R/W copies
        #
        restart_source = ''
        restart_sections = []
        for i in self.infiles[1:]:
            if i.restart:
                for s in i.shdr:
                    if (s.sh_flags & s.SHF_ALLOC) != 0 and \
                       (s.sh_flags & s.SHF_WRITE) != 0 and \
                       (s.sh_type == s.SHT_PROGBITS) and \
                       (s.sh_size > 0):
                        while (len(restart_source) ^ s.sh_addr) & 7:
                            restart_source += '\0'
                        restart_sections.append((i,s,len(restart_source)))
                        restart_source += s.contents().read()
        for range in self.physpool_space.ranges:
            print range
        cur_offset = 0
        while cur_offset < len(restart_source):
            #
            #  Allocate first fit, at least 4K, but not more than we need
            #
            maxsize = alignup(len(restart_source) - cur_offset, 0x1000)
            prange = self.physpool_space.alloc_first_fit(minsize = 0x1000,
                                                         maxsize = maxsize,
                                                         extra_instructions=True)
            vrange = self.vaddr_space.find_optimal_match(prange.size,
                                                         match_addr=prange.addr,
                                                         extra_instructions=True)
            init_data = restart_source[cur_offset:(cur_offset+prange.size)]
            import StringIO
            f = StringIO.StringIO(init_data)
            self.add_segment(vrange.addr, prange.addr, len(init_data), f)
            map = HexagonMapping()
            map.size = prange.size >> 12
            map.vpage = vrange.addr >> 12
            map.ppage = prange.addr >> 12
            map.locked = False
            map.reloc = self.relocatable_image.value
            map.mapping_group = UniqueString()
            map.G = 1
            map.A1A0 = 0
            map.ASID = 0
            map.X = 0
            map.W = 0
            map.R = 1
            map.U = 1
            map.C = 7
            self.extra_mappings.extend(map.fix_to_TLB_entries(maxsize = self.max_elf_tlb.value/0x1000))
            for i,s,offset in restart_sections:
                if offset + s.sh_size <= cur_offset:
                    #
                    #  Entire restart for this section comes before this data
                    #
                    continue
                if offset >= cur_offset + len(init_data):
                    #
                    #  Entire restart for this section comes after this data
                    #
                    continue
                start_offset_in_this_block = max(0, offset-cur_offset)
                end_offset_in_this_block = min(len(init_data), (offset+s.sh_size)-cur_offset)
                i.restart_copies.append((s.sh_addr + (cur_offset+start_offset_in_this_block-offset),
                                         vrange.addr+start_offset_in_this_block,
                                         (end_offset_in_this_block-start_offset_in_this_block)))
            cur_offset += len(init_data)

    def ramfs_populate(self):
        if not self.ramfs:
            return
        from os.path import getsize, basename
        from struct import pack
        fsnames = ''
        for x in self.ramfs:
            if '=' in x[0]:
                runtime_file_name, build_file_name = x[0].split('=',1)
            else:
                runtime_file_name, build_file_name = (basename(x[0]), x[0])
            sz = getsize(x[0])
            f = open(x[0],'rb')
            paddr = self.physpool_space.find_optimal(sz,extra_instructions=True).addr
            vaddr = self.vaddr_space.find_optimal(sz,extra_instructions=True).addr
            self.add_segment(vaddr, paddr, sz, f);
            self.ramfs_info.append([len(fsnames), vaddr, sz, 1])
            fsnames += runtime_file_name + '\0'
            map = HexagonMapping()
            map.size = alignup(sz,0x1000) >> 12
            map.vpage = vaddr >> 12
            map.ppage = paddr >> 12
            map.locked = False
            map.reloc = self.relocatable_image.value
            map.mapping_group = UniqueString()
            map.G = 1
            map.A1A0 = 0
            map.ASID = 0
            map.X = 0
            map.W = 0
            map.R = 1
            map.U = 1
            map.C = 7
            self.extra_mappings.extend(map.fix_to_TLB_entries(maxsize = self.max_elf_tlb.value/0x1000))
        #
        #  Each directory entry takes 16 bytes:
        #   Name (stored as offset into fsnames, replaced with pointer at run time)
        #   Virtaddr (the pagetable basically *is* our "FAT" equivalent)
        #   Size (number of bytes)
        #   Flags (1 means a valid file; 0 means end of list)
        #
        ramfs_dir = ''
        for x in self.ramfs_info:
            x[0] += 16*(1+len(self.ramfs_info))
        for x in self.ramfs_info:
            ramfs_dir += pack('<LLLL', *x)
        ramfs_dir += pack('<LLLL', 0, 0, 0, 0)
        ramfs_dir += fsnames
        sz = len(ramfs_dir)
        paddr = self.physpool_space.find_optimal(sz, extra_instructions=True).addr
        vaddr = self.vaddr_space.find_optimal(sz, extra_instructions=True).addr
        import StringIO
        f = StringIO.StringIO(ramfs_dir)
        self.add_segment(vaddr, paddr, sz, f)
        map = HexagonMapping()
        map.size = alignup(sz,0x1000) >> 12
        map.vpage = vaddr >> 12
        map.ppage = paddr >> 12
        map.locked = False
        map.reloc = self.relocatable_image.value
        map.mapping_group = UniqueString()
        map.G = 1
        map.A1A0 = 0
        map.ASID = 0
        map.X = 0
        map.W = 0
        map.R = 1
        map.U = 1
        map.C = 7
        print 'vaddress = %X' % vaddr
        self.extra_mappings.extend(map.fix_to_TLB_entries(maxsize = self.max_elf_tlb.value/0x1000))
        if self.qurtos_ramfs_directory:
            self[(self.qurtos_ramfs_directory, 4)] = ('<L', vaddr)
    def shrink_to_default_physpool(self):
        self.physpool_space += AllocatedRange(left = -QurtInfinity,
                                              right = self.physaddr)
        for pool in self.pools:
            if pool.name == "DEFAULT_PHYSPOOL":
                self.physaddr_end = self.physaddr + int(self.total_memsize,0)
                self.physpool_space += AllocatedRange(left = self.physaddr + int(self.effective_memsize,0),
                                                      right = QurtInfinity)
    def gap_recovery(self):
        for t in self.opt_mappings:
            if t.C < 0:
                continue
            if not t.locked:
                continue
            perms = dict()
            if t.R:
                perms['R'] = True
            if t.W:
                perms['W'] = True
            if t.X:
                perms['X'] = True
            if t.U:
                perms['U'] = True
            self.vaddr_space += AllocatedRange(addr = t.vpage << 12,
                                               size = t.size << 12,
                                               reserve_only = True)
            self.physpool_space += QurtAddressRange(addr=t.ppage << 12,
                                                    size=t.size << 12,
                                                    **perms)
        for x in self.memsectiongaps:
            x.process()
    def finish_extra_mappings(self):
        if not self.extra_mappings:
            return
        #
        # Generate the list of instructions.
        # This will be a list of relocatable mappings followed by a zero entry,
        #  followed by a list of absolute mappings followed by a zero entry.
        # Finally, after those, there is one more word, which is the total
        #  size of the mapping used for the instructions, in bytes.  This will
        #  usually be 4K, but might be larger in the future.
        #
        import struct
        x = ''
        physadjust = (self.physaddr >> 11)
        for e in self.extra_mappings:
            if not e.locked and e.reloc:
                x += struct.pack('<Q', e.get_TLB_entry() - physadjust)
        x += struct.pack('<Q', 0)
        for e in self.extra_mappings:
            if not e.locked and not e.reloc:
                x += struct.pack('<Q', e.get_TLB_entry())
        x += struct.pack('<Q', 0)
        sz = 0x1000
        while sz<len(x)+4:
            sz *= 4
        x += struct.pack('<L', sz)
        vaddr = self.vaddr_space.find(sz,sz,extra_instructions=True).addr
        paddr = self.physpool_space.find(sz,sz,extra_instructions=True).addr
        p = self.phdr[0].copy()
        p.p_type = p.PT_LOAD
        p.p_offset = 0
        p.p_vaddr = vaddr
        p.p_paddr = paddr
        p.p_filesz = len(x)
        p.p_memsz = len(x)
        p.p_flags = 4
        p.p_align = sz
        s = self.shdr[0].copy()
        s.name = '.qibextra'
        s.sh_type = s.SHT_PROGBITS
        s.sh_flags = s.SHF_ALLOC
        s.sh_addr = vaddr
        s.sh_offset = 0
        s.sh_size = len(x)
        s.sh_link = s.SHN_UNDEF
        s.sh_info = 0
        s.sh_addralign = 8
        s.sh_entsize = 0
        s.user_mode = False
        s.cache_policy = 7
        s.locked = True
        s.reclaim = True
        s.overlay = 0
        s.physpool = None
        p.section_list = [s]
        self.phdr.append(p)
        self.shdr.append(s)
        import StringIO
        p.f = StringIO.StringIO(x)
        #
        #  Start slightly hacky code.
        #
        self.add_elf_mapping(s,vaddr,sz,paddr,0,0,0,0,0)
        self.opt_mappings.append(self.all_mappings.pop())
        if self.qurtos_extra_instructions == 0:
            raise Exception('Cannot complete advanced memory instructions -- missing symbol')
        self[(self.qurtos_extra_instructions, 4)] = ('<L', vaddr)

    def populate_output_file(self):
        infiles = self.infiles
        firstfile = infiles[0]
        self.phdr = []
        for i in infiles:
            for p in i.phdr:
                np = p.copy()
                p.outcopy = np
                if np.p_paddr >= i.vmem_bot and np.p_paddr <= i.vmem_top and np.p_type != Elf32ProgramHeader.PT_NULL:
                    np.p_paddr = i.output_physaddr + (np.p_paddr - i.vmem_bot)
                if not getattr(np, 'temp', False):
                    self.physpool_space += AllocatedRange(addr=np.p_paddr,
                                                          size=np.p_memsz,
                                                          phdr=np)
                self.phdr.append(np)
                for s in np.section_list:
                    s.phdr_link = np
                np.section_list = []
        self.shdr = []
        for i in infiles:
            for s in i.shdr:
                if s.sh_type != s.SHT_NULL:
                    if (s.sh_flags & s.SHF_ALLOC) != 0 or s.name == '.comment':
                        sp = s.copy()
                        self.shdr.append(sp)
                        if not i is firstfile:
                            sp.name = s.name+'.'+i.appname
                            sp.shindex = 0
                        if getattr(sp,'phdr_link',None):
                            sp.phdr_link.section_list.append(sp)
        self.ehdr = firstfile.ehdr.copy()
        self.ehdr.e_entry = firstfile.output_physaddr
    def set_root_prog_no(self):
        if self.QURTK_root_prog_no:
            current_value = self.unpack_from('<i', self.QURTK_root_prog_no)[0]
            if current_value < -1:
                new_value = self.program_maps.get(self.infiles[0].appname,-1)
                self[(self.QURTK_root_prog_no, 4)] = ('<i', new_value)
    def ProcessFile(self):
        infiles = self.infiles
        physaddr = self.physaddr
        self.phys_base = physaddr
        # Find the lowest virtual address in the os image
        base_addr = infiles[0].start_addr
        self.virt_base = base_addr
        if self.CONFIG_ADVANCED_MEMORY:
            self.read_boot_mappings()
            self.read_tlb_dump()
            self.read_mmap_table()
            self.read_pool_configs()
            self.patch_mmap_table()
            for t in self.vrlist:
                self.vaddr_space.alloc(left=(t[0] << 12),
                                       right=((t[1]+1) << 12),
                                       reserve_only=True)
            for t in self.boot_mappings:
                self.vaddr_space.alloc(left=t[1],
                                       right=t[2]-1,
                                       reserve_only=True,
                                       boot_mapping=t[0])
            for pool in self.pools:
                for r in pool.ranges:
                    if pool.name != "DEFAULT_PHYSPOOL":
                        if (r[1] << 12) < 0x40000000:
                            self.vaddr_space.alloc(addr=(r[0] << 12),
                                                   size=(r[1] << 12),
                                                   reserve_only=True,
                                                   idempotent_avoidance=True)
            #self.do_section_overrides(infiles)
            self.synthesize_elf_mappings(infiles)
            self.sort_mappings()
            self.get_ranges()
            self.get_opt_mappings()
            self.clade_reg_writes()
            self.move_pool_mappings()
            self.shrink_to_default_physpool()
            self.ramfs_populate()
            self.make_restart_copies()
            self.gap_recovery()
            self.map_trace_buffer()
            self.finish_extra_mappings()
            reclaim_mask = 0L
            reclaim_index = 1L
            v = []
            vn = []
            print '*' * 60
            print 'BOOT TLB mappings'
            for t in self.opt_mappings:
                if t.locked and t.reloc and t.overlay != 1 and t.C >= 0:
                    reclaim = self.can_be_reclaimed(t)
                    reclaim_msg = ('',' unlock')[reclaim]
                    t.ppage -= (self.phys_base >> 12)
                    print '%05X000 -> BASE  +  %05X000, length %05X000, perms X:%u W:%u R:%u U:%u C:%X%s' % (t.vpage, t.ppage, t.size,
                                                                                                             t.X, t.W, t.R, t.U, t.C,
                                                                                                             reclaim_msg)
                    v.append(t.get_TLB_entry())
                    if reclaim:
                        reclaim_mask += reclaim_index
                    reclaim_index *= 2
            for t in self.opt_mappings:
                if t.locked and not t.reloc and t.overlay != 1 and t.C >= 0:
                    reclaim = self.can_be_reclaimed(t)
                    reclaim_msg = ('',' unlock')[reclaim]
                    print '%05X000 -> Physical %05X000, length %05X000, perms X:%u W:%u R:%u U:%u C:%X%s' % (t.vpage, t.ppage, t.size,
                                                                                                             t.X, t.W, t.R, t.U, t.C,
                                                                                                             reclaim_msg)
                    vn.append(t.get_TLB_entry())
                    if reclaim:
                        reclaim_mask += reclaim_index
                    reclaim_index *= 2
                    if t.has_token( AddressPatchToken ):
                        index = len( v ) + len( vn ) - 1
                        self.eip.append( eip_q6cfg( index, eip_q6cfg.LOCKED ) )
            self.do_tlb_compare(v,vn)
            self.write_tlb_dump(v,vn)
            self.write_tlb_reclaim(reclaim_mask)
            print '*' * 60
            print 'reclaim_mask is %X' % reclaim_mask
            for x in self.pool_moves:
                print 'Vaddr %08X -> Paddr %08X, size %X' % (x[0], x[1], x[2])
            self.phys_island_report()
            self.write_quick_mmu(infiles[0])
        self.eip.writeback(None, infiles, self.pool_moves, self.clade_regs, self.opt_mappings)

    def last_fixups(self):
        if self.QURTK_hbi:
            if self.x__hexagon_bsp_init:
                hbi_physaddr = self.lookup_vma_to_lma(self.x__hexagon_bsp_init) - self.phys_base
                if True:
                    self[(self.QURTK_hbi,4)] = ('<L', int(hbi_physaddr)+1)

    def eip_build_filelike(self):
        import StringIO
        addr = self.QURT_EIP_BUILD
        while True:
            recordlen, recordtype = self.unpack_from('<HH',addr)
            if (recordlen, recordtype) == (0,0):
                return StringIO.StringIO(self[(self.QURT_EIP_BUILD, (addr+4)-self.QURT_EIP_BUILD)])
            if recordlen == 0:
                raise Exception()
            addr += recordlen

    def eip_build_records(self, matchtype, decode=''):
        import struct
        decodelen = struct.calcsize('<'+decode)
        f = self.eip_build_filelike()
        while True:
            try:
                recordlength, recordtype = struct.unpack('<HH', f.read(4))
            except struct.error:
                raise Exception('EIP build section is malformed')
            if (recordlength, recordtype) == (0,0):
                break
            payload_length = recordlength - 4
            if (payload_length & 3) != 0 or payload_length < 0:
                raise Exception('EIP build section is malformed')
            if payload_length == 0:
                payload = ''
            else:
                payload = f.read(payload_length)
                if len(payload) != payload_length:
                    raise Exception('EIP build section is malformed')
            if recordtype != matchtype:
                continue
            datalen = payload_length - decodelen
            if datalen < 0:
                raise Exception('EIP build section is malformed')
            yield struct.unpack('<%s%us' % (decode,datalen), payload)

    def eip_build(self):
        """Read and process EIP build records
        """
        # .patchable_mmaps is a dictionary of memory map index => address of
        # variable holding patched virtual address
        self.patchable_mmaps = {}
        try:
            addr = self.QURT_EIP_BUILD
            while True:
                hdr = self.unpack_from('<HH', addr)
                if hdr == (0,0):
                    break
                if not hdr[1] in [ 1, 5 ]:
                    # Not a copy OR Q6CFG record; skip it
                    addr += hdr[0]
                    continue

                if hdr[ 1 ] == 5:
                    variable_vaddr, tlb_size, mmap_idx = self.unpack_from( '<LHH', addr+4 )
                    self.patchable_mmaps[ mmap_idx ] = variable_vaddr
                elif hdr[ 1 ] == 1:
                    info = self.unpack_from('<LL', addr+4)
                    data = self[(addr+12, hdr[0]-12)]
                    if info[1] > len(data):
                        raise Exception()
                    if info[0]:
                        self[info] = data[:info[1]]

                addr += hdr[0]
        except:
            print 'QURT_EIP_BUILD failure'
            raise

    def run_alternate_script(self):
        #
        #  Check for whether an alternate image build script was requested.
        #  If so, go find it and run it, adding in the '-f' command line argument.
        #  The '-f' will tell that script not to check again for an alternate script.
        #
        if self.no_altscript:
            return
        if not self.altscript.value:
            return
        altscript = self.altscript.value
        import os
        if not os.path.isabs(altscript):        # Not absolute, treat it as relative to the location of this script
            altscript = os.path.join(os.path.dirname(__file__),altscript)
        af = open(altscript,'r')
        #
        #  The rest of this is just Python trickery to execute the new script as if it had been
        #   directly invoked from the command line with the -f option.
        #
        import __main__
        __main__.__file__ = altscript
        import sys
        sys.argv[0] = altscript
        sys.argv.insert(1,'-f')
        exec af in __main__.__dict__
        raise Exception("Alternate script %s did not exit!" % altscript)
    def main(self, argv):
        from lib.elf_info_patch import ElfInfoPatch
        self.parse_args(argv)
        try:
            parse_build_elements(*ElfInfoPatch().Parse(self, self.infiles[0].start_addr, self.infiles[0].start_size))
        except:
            parse_build_elements(self,[])
            self.altscript.value = 'qurt-image-build1.py'
        self.run_alternate_script()
        self.do_fixups()
        self.process_input_files()
        self.do_section_overrides()
        self.clade_uncomp_offset()
        self.populate_output_file()
        self.set_root_prog_no()
        self.eip_build()
        self.ProcessFile()
        self.last_fixups()
        for p in self.phdr:
            if getattr(p,'temp',False):
                continue
            for s in p.section_list:
                if getattr(s,'temp',False):
                    continue
                if hasattr(s,'s_internal'):
                    continue
                if s.sh_type == s.SHT_PROGBITS:
                    self.physpool_space += QurtAddressRange(addr = p.p_paddr + (s.sh_addr - p.p_vaddr),
                                                            size = s.sh_size,
                                                            boot_section = s.name)
        qiblog = QIBLog(self, self.qiblog_filename)
        for range in self.physpool_space.ranges:
            qiblog.add_range(range)
        qiblog.finish()
        self.finish()
        if self.write_reloc:
            for f in self.infiles:
                srcname = f.rawfile.name
                tmp = srcname.rsplit('.',1)
                tmp[0] += '_reloc'
                dstname = '.'.join(tmp)
                from shutil import copyfile
                copyfile(srcname,dstname)
        return 0

if __name__ == '__main__':
    from lib.qurt import run_script
    run_script(QurtElfPatch().main)
