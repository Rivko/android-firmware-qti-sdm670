
##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2007, Open Kernel Labs, Inc.
# 
# All rights reserved. 
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by National ICT Australia, free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of National ICT Australia, nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on National ICT
# Australia or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of National ICT Australia or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.
"""
Processing of the program XML element.
"""

from ezxml import Element, long_attr, str_attr, size_attr
import re
import sys

Min_image_page_el = Element("min_image_page",
                    value = (size_attr, "optional"))

Osam_stack_fill_count_el = Element("osam_stack_fill_count",
                    value = (size_attr, "optional"))

Stack_size_el = Element("stack_size",
                    value = (size_attr, "required"))

App_heap_size_el = Element("heap_size",
                    name = (str_attr, "required"),
                    type = (str_attr, "optional"),
                    value = (size_attr, "required"))

App_dynamic_def_pool_size_el = Element("dynamic_default_pool_size",
                    name = (str_attr, "optional"),
                    type = (str_attr, "optional"),
                    value = (size_attr, "required"))

MainPrio_el = Element("main_priority",
                    value = (str_attr, "required"))

MainBitmask_el = Element("main_bitmask",
                    value = (str_attr, "optional"))

ReaperPrio_el = Element("reaper_priority",
                    value = (str_attr, "required"))

ReaperTcbPartition_el = Element("reaper_tcb_partition",
                    value = (str_attr, "required"))

ReaperStackSize_el = Element("reaper_stack_size",
                             value = (size_attr, "optional"))

MaxPiPrio_el = Element("max_pimutex_prio",
                    value = (str_attr, "optional"))

TlbRepl_el = Element("tlb_first_replaceable",
                    value = (str_attr, "optional"))
                    
Memsection_el = Element("memsection",
                    glob = (str_attr, "optional"),
                    tlb_lock = (str_attr, "optional"),
                    bus_attr = (str_attr, "optional"),
                    direct = (str_attr, "optional"),
                    attach = (str_attr, "required"),
                    physpool = (str_attr, "optional"),
                    virt_addr = (long_attr, "optional"),
                    phys_addr = (long_attr, "optional"),
                    user_mode = (str_attr, "optional"),
                    size = (size_attr, "required"),
                    cache_policy = (str_attr, "optional"))

from memsection_xml import *
Memsection_el = MemsectionWrapper(Memsection_el)

Irq_el = Element("irq",
                    num = (str_attr, "required"),
                    trigger = (str_attr, "required"),
                    island = (str_attr, "optional"),
                    polarity = (str_attr, "optional"),
                    l2base = (str_attr, "optional"),
                    l2trigger = (str_attr, "optional"),
                    l2polarity = (str_attr, "optional"))

Cachepart_el = Element("l2_partition",
                    attr = (str_attr, "required"))

Interrupt_el = Element("interrupt",
                    Irq_el)

CachePartition_el = Element("cache_partition",
                    Cachepart_el)

BusPriority_el = Element("internal_bus_priority",
                    enabled = (str_attr, "required"))

Isr_cb_el = Element("irq",
                    num = (str_attr, "required"),
                    callback = (str_attr, "required"))

Isr_el = Element("isr_callback",
                    Isr_cb_el)

class BootMapping:
    def __init__(self,el):
        self.name = 'boot_mapping'
        self.tag = self.name
        self.el = el
        pass
    def walkdom(self,el):
        return BootMapping(el)
    def extern_decls(self):
        return 'extern char %s[], %s[];' % (self.el.getAttribute('start_symbol'),
                                            self.el.getAttribute('end_symbol'))
    def map_definition(self):
        start_symbol = self.el.getAttribute('start_symbol')
        end_symbol = self.el.getAttribute('end_symbol')
        tlb_template = self.el.getAttribute('tlb_template')
        if not (start_symbol and end_symbol and tlb_template):
            raise Exception('boot_mapping: must define start_symbol, end_symbol, and tlb_template')

        # Default TLB template is for a global mapping, read-only, with L1 and L2 write-back caching,
        #  not accessible from user mode, and which is not locked into the TLB, and which is reclaimable.
        # The XML only needs to specify deviations from this default mapping.

        template_dict = {'G' : 1,
                         'A' : 0,
                         'ASID' : 0,
                         'X' : 0,
                         'W' : 0,
                         'R' : 1,
                         'U' : 0,
                         'C' : 7,
                         'RECLAIM' : 1,
                         'LOCK' : 0}

        ins = tlb_template.split('_')
        while ins:
            template_dict[ins[0].upper()] = int(ins[1])
            ins = ins[2:]

        # Build up the TLB template
        tlb_template_value = ((1                        << 63) |
                              (template_dict['G']       << 62) |
                              (template_dict['A']       << 59) |
                              (template_dict['ASID']    << 52) |
                              (template_dict['X']       << 31) |
                              (template_dict['W']       << 30) |
                              (template_dict['R']       << 29) |
                              (template_dict['U']       << 28) |
                              (template_dict['C']       << 24) |
                              (template_dict['RECLAIM'] <<  1) |
                              (template_dict['LOCK']    <<  0))

        return '\\\n   {0x%016XULL,%s,%s}' % (tlb_template_value, start_symbol, end_symbol)

Program_el = Element("program",
                    Min_image_page_el,
                    Osam_stack_fill_count_el,
                    Stack_size_el,
                    App_heap_size_el,
                    App_dynamic_def_pool_size_el,
                    MainPrio_el,
                    MainBitmask_el,
                    ReaperPrio_el,
                    ReaperTcbPartition_el,
                    ReaperStackSize_el,
                    Element("process_reaper_enable",        value = (str_attr, "optional")),
                    Element("process_reaper_priority",      value = (str_attr, "optional")),
                    Element("process_reaper_tcb_partition", value = (str_attr, "optional")),
                    Element("process_reaper_stack_size",    value = (str_attr, "optional")),
                    Element("file_match",                   value = (str_attr, "required")),
                    Element("secure",                       SID   = (str_attr, "required")),
                    MaxPiPrio_el,
                    TlbRepl_el,
                    Memsection_el,
                    BootMapping(None),
                    Interrupt_el,
                    CachePartition_el,
                    BusPriority_el,
                    Isr_el,
                    name = (str_attr, "required"))
                      
class MemSection:
    """
    Represents a memsection. Derives attributes from the parsed element.
    Consists of methods to perform consistency checks and obtain optional
    attributes.
    """
    VALID_PAGE_SIZES=[0x1000, 0x4000, 0x10000, 0x40000, 0x100000, 0x400000, 0x1000000]
    def __init__(self, memsec_el):
        self.reserved = None
        if 'virt_addr' not in memsec_el.__dict__.keys():
            raise Exception('memsection %s must define virt_addr' % str(memsec_el.__dict__))
        if self.ProcessReservedMemSection(memsec_el):
            return
        self.glob = '0'
        self.lock = '0'
        self.bus_attr = '00'
        self.direct = 'false'
        self.virt_addr = memsec_el.virt_addr
        self.user_mode = 'true'
        self.size = memsec_el.size
        self.attach = memsec_el.attach

        if not hasattr(memsec_el, 'glob') and not hasattr(memsec_el, 'user_mode'):
            # A memsection requesting no special handling for its mapping domain.
            # Make such memsections global and non-user-mode-accessible.
            # They used to be ASID=0, user-mode.
            self.glob = '1'
            self.user_mode = 'false'
        if hasattr (memsec_el, 'glob'):
            self.glob = memsec_el.glob
        if hasattr (memsec_el, 'tlb_lock'):
            self.lock = memsec_el.tlb_lock
        if hasattr (memsec_el, 'bus_attr'):
            self.bus_attr = memsec_el.bus_attr
        if hasattr (memsec_el, 'physpool'):
            self.pool= memsec_el.physpool
        if hasattr (memsec_el, 'direct'):
           self.direct = memsec_el.direct
        if hasattr (memsec_el, 'phys_addr'):
           self.phys_addr = memsec_el.phys_addr
        if hasattr (memsec_el, 'user_mode'):
           self.user_mode = memsec_el.user_mode
       
        if hasattr (memsec_el, 'cache_policy'):
           self.cache_policyname = memsec_el.cache_policy

    def ProcessReservedMemSection(self, memsec_el):
        self.reserved = None
        if memsec_el.attach.upper() == 'RESERVED':
            allowed = ['tag', 'children', 'attach', 'virt_addr', 'size']
            disallowed = [k for k in memsec_el.__dict__.keys() if not k in allowed]
            if disallowed:
                raise Exception('Reserved memsection with illegal key: %s' % disallowed[0])
            if (memsec_el.virt_addr & 0xFFF) != 0 or memsec_el.virt_addr < 0:
                raise Exception('Reserved memsection with illegal address: %s' % hex(int(memsec_el.virt_addr)))
            if (memsec_el.size & 0xFFF) != 0 or memsec_el.size <= 0:
                raise Exception('Reserved memsection with illegal size: %s' % hex(int(memsec_el.size)))
            if (memsec_el.virt_addr + memsec_el.size) > 2**32:
                raise Exception('Reserved memsection beyond 4GB: %s' % hex(int(memsec_el.virt_addr)))
            self.reserved = [memsec_el.virt_addr >> 12, ((memsec_el.virt_addr + memsec_el.size) >> 12) - 1]
        return self.reserved

    def setCachePolicy(self, policy_list):
        if hasattr (self, 'cache_policyname'):
            for (value, name) in policy_list:
                if name == self.cache_policyname:
                    self.cache_policy = value
                    break
        else:
            self.cache_policy = '0x7'

    def isTlbBootLock (self):
        return (self.lock.lower() == 'boot')

    def isTlbLock (self):
        if self.lock == '1':
            return True
        else:
            return self.isTlbBootLock()

    def queryBusAttr (self, allowed_range):
        try:
            bdigit = {'1': 1, '0': 0}
            tmp = self.bus_attr
            if len(tmp) == 2:
                ret = bdigit[tmp[0]]*2 + bdigit[tmp[1]]*1
                if ret in allowed_range:
                    return ret
        except Exception, err:
            pass
        raise Exception("bus_attr field of memsection is not a legal value")

    def queryUserMode (self):
        if self.user_mode == 'false':
            return '0'
        else:
            return '1'

    def setPhysAddr(self):
        if self.direct == 'true':
           self.phys_addr = self.virt_addr

    def isValidSize(self):
        if self.size in MemSection.VALID_PAGE_SIZES:
            return True
        else:
            return False

    def isValidAlign(self):
        phys_offset = self.phys_addr & (self.size - 1)
        virt_offset = self.virt_addr & (self.size - 1)
        if (phys_offset != 0)  or (virt_offset != 0):
            return False
        return True

    def isValidPhysPool(self, regions):
        for (base, size) in regions:
            if self.phys_addr >= base and self.phys_addr <= (base + size - 1) and (self.phys_addr + self.size - 1) >= base and (self.phys_addr + self.size - 1) <= (base + size - 1):
                return True
        return False

    def checkConsistency(self):
        if not hasattr (self, 'phys_addr'):
            print 'No phys_addr'
            return False

        if not hasattr (self, 'cache_policy'):
            print 'Invalid cache_policy'
            return False

        return True                     # Skip size and alignment checks.  We catch them later in tlb_split().
        if not self.isValidSize():
            print 'Invalid size'
            return False

        #import pdb; pdb.set_trace()
        if not self.isValidAlign():
            print "Improper alignment of phys addr: %s or virt addr: %s" % (hex(self.phys_addr), hex(self.virt_addr))
            return False

        # Every thing looks good for this memsection
        return True

    def getSize (self):
        return self.size

class CfgMemSection (MemSection):
    """Special kind-of MemSection that is used to map Q6 configuration register
       files (ie based off CFGBASE). TLB associated with this memsections is
       patched during build and run-time. Therefore certain build-time checks
       are ignored
    """

    # CFGBASE offsets (from Q6 System-Level Architecture Specification)
    CFGBASE_OFFSET_L2TCM   = 0x00
    CFGBASE_OFFSET_RSVD_04 = 0x04
    CFGBASE_OFFSET_SS      = 0x08
    CFGBASE_OFFSET_ETM     = 0x0C
    CFGBASE_OFFSET_L2      = 0x10
    CFGBASE_OFFSET_RSVD_14 = 0x14
    CFGBASE_OFFSET_RSVD_18 = 0x18 
    CFGBASE_OFFSET_AXIM2   = 0x1C
    CFGBASE_OFFSET_STREAM  = 0x20
    CFGBASE_OFFSET_CLADE   = 0x24

    ALLOWED_OFFSETS = (
        CFGBASE_OFFSET_ETM,
        CFGBASE_OFFSET_L2,
    )

    INVALID = -1

    @staticmethod
    def tuple_to_hex_string (t):
        return [ "0x%02X, " % i for i in t ]

    def __init__(self, varname, offset, **kwargs):

        if not offset in CfgMemSection.ALLOWED_OFFSETS:
            raise ValueError( "Offset 0x%02X disallowed. Allowed offsets are %s" % ( offset, CfgMemSection.tuple_to_hex_string( CfgMemSection.ALLOWED_OFFSETS ) ) )
        self.cfgbase_offset = offset

        self.mmap_tab_index = CfgMemSection.INVALID
        self.varname = varname

        elements = lambda: None

        elements.__dict__.update( kwargs )

        # Caller doesnt have control over .virt_addr. For CfgMemSection, it has
        # to start with a special value 0xBADXX000 where XX are hex digits for
        # CFGBASE offset for readability
        elements.virt_addr = 0xBAD00000 | ( offset << 12 )

        # CFGBASE offset is *ALSO* embedded in PPN for use at runtime. It is
        # shifted by 19 into paddr so that it is encoded in TLB[15:8]
        elements.phys_addr = offset << 19

        MemSection.__init__( self, elements )

    def name (self):
        """Return the variable name associated with this CfgMemSection
           instance
        """
        return self.varname

    def set_mmap_index (self, index):
        print 'Setting MMAP index for "%s" to %u' % ( self.name(), index )
        self.mmap_tab_index = index

    def get_mmap_index (self):
        if self.mmap_tab_index != CfgMemSection.INVALID:
            return self.mmap_tab_index
        raise ValueError( "Memory map table index for memsection \"%s\" is invalid" % self.name() )

def  checkMemOverlap (mem_objs):
    if len (mem_objs) < 2:
        return True

    # Check if overlapping virtual addresses and conflicting cache attributes exist
    for i in range(len(mem_objs) - 1):
        x = mem_objs[i]

        if isinstance( x, CfgMemSection ):
            # Ignore CfgMemSection instances in overlap check
            continue

        for j in range (i+1, len(mem_objs)):
            y = mem_objs[j]

            if isinstance( y, CfgMemSection ):
                # Ignore CfgMemSection instances in overlap check
                continue

            # Overlapping virtual addresses ?
            if x.virt_addr >= y.virt_addr and x.virt_addr <= (y.virt_addr + y.size - 1):
                print "Virt addr, size : (%s, %s) Virt addr, size:  (%s, %s) overlap" % (hex(x.virt_addr), hex(x.size), hex(y.virt_addr), hex(y.size))
                return False
            if x.virt_addr <= y.virt_addr and (x.virt_addr + x.size - 1) >= y.virt_addr:
                print "Virt addr, size : (%s, %s) Virt addr, size:  (%s, %s) overlap" % (hex(x.virt_addr), hex(x.size), hex(y.virt_addr), hex(y.size))
                return False

            # Conflicting cache attributes ?
            if x.phys_addr >= y.phys_addr and x.phys_addr <= (y.phys_addr + y.size - 1):
                if x.cache_policy != y.cache_policy:
                    print "Virt addr, cache policy : (%s, %s) Virt addr, cache policy:  (%s, %s) conflict" % (hex(x.virt_addr), hex(x.cache_policy), hex(y.virt_addr), hex(y.cache_policy))
                    return False
            if x.phys_addr <= y.phys_addr and (x.phys_addr + x.size - 1) >= y.phys_addr:
                if x.cache_policy != y.cache_policy:
                    print "Virt addr, cache policy : (%s, %s) Virt addr, cache policy:  (%s, %s) conflict" % (hex(x.virt_addr), hex(x.cache_policy), hex(y.virt_addr), hex(y.cache_policy))
                    return False

    return True

def collect_reserved_ranges(reserved_objs):
    # Passed a list of RESERVED memsection objects
    # Should return a string suitable for use in
    #  the C initializer
    # TBD:  Check the RESERVED memsections for overlap (fail?)
    #  and for adjacency (combine?)
    str = ''
    for obj in reserved_objs:
        str += '0x%05X, 0x%05X, ' % (obj.reserved[0], obj.reserved[1])
    return str

def collect_memsections (cfg):
    machine_el = cfg.find_child("machine")
    program_el = cfg.find_child("program")
    physpool_el = cfg.find_children("physical_pool")

    # Extract Cache policy definitions
    cache_policies = [] # [(value, name), ....]
    cache_policies = [(cache_el.value, cache_el.name) for cache_el in machine_el.find_children("cache_policy")]
    # Extract memsection information
    mem_sections = []
    mem_obj_list = []
    reserved_list = []
    for el in program_el.find_children("memsection"):
        mem_obj = MemSection(el)
        # Reserved memsections are special
        if mem_obj.reserved:
            reserved_list.append(mem_obj)
            continue
        # Set cache policy
        mem_obj.setCachePolicy(cache_policies)
        # Set Phys addr
        mem_obj.setPhysAddr()
        # Sanity check the object
        if not mem_obj.checkConsistency():
            sys.exit ("!!! Inconsistent memsection: \n %s \n" % (el))

        # Make the phys pool is valid
        if hasattr(mem_obj, 'pool'):
            pool_list = None
            for pool in physpool_el:
                if pool.name == mem_obj.pool:
                    pool_list = [(region_el.base, region_el.size) for region_el in pool.find_children("region")]
                    if not mem_obj.isValidPhysPool(pool_list):
                        sys.exit ("!!! Pool in memsection doesn't match phypool: \n %s \n" % (el))
                    break;
            if pool_list is None:
                sys.exit ("!!! Invalid Pool name in memsection: \n %s \n" % (el))

        mem_obj_list.append(mem_obj)

    # Create CfgMemSections
    cfgbase_memsections = []

    # Mapping for L2CFG
    # @warning Name *MUST* match the variable chosen in kernel/src/cfg_ops.c
    cfgbase_memsections.append(
        CfgMemSection(
            'QURTK_l2cfg_base_vaddr', 
            CfgMemSection.CFGBASE_OFFSET_L2,
            size = 64*1024,
            tlb_lock = "boot",
            cache_policy = "uncached",
            attach = "krn") )

    for mem_obj in cfgbase_memsections:
        mem_obj.setCachePolicy( cache_policies )
        mem_obj.setPhysAddr()
        if not mem_obj.checkConsistency():
            sys.exit( "!!! Inconsistent memsection: \n %s \n" % mem_obj.name() )
        mem_obj_list.append( mem_obj )

    if not checkMemOverlap(mem_obj_list):
         sys.exit ("!!! Memsections and Physpool fail in overall consistency \n")
    return mem_obj_list, reserved_list

def check_page_size(size):
    """Check if page size is valid page."""
    VALID_PAGE_SIZES=[0x1000, 0x4000, 0x10000, 0x40000, 0x100000, 0x400000, 0x1000000]
    if size in VALID_PAGE_SIZES:
        return True
    else:
        return False

def find_default(el, key, value):
    tmp = el.find_child(key)
    if tmp:
        value = tmp.value
    return value

def collect_program_element(cfg):
    """Collect the attributes of the program element."""

    asic = cfg.asic

    collect_secure_program_info(cfg)

    collect_program_interrupts(cfg)

    collect_program_cache_partition(cfg)

    collect_program_internal_bus_priority(cfg)

    program_el = cfg.find_child("program")

    pgsize = {}
    str_tlblock = ""
    str = ""
    tlb_entries=0
    non_tlb_entries=0

    mem_sections, reserved_list = collect_memsections(cfg)

    if asic == "v2" or asic == "v3":
        pgsize = {0x1000: "0", 0x4000: "1", 0x10000: "2", 0x40000: "3", 0x100000: "4", 0x400000: "5", 0x1000000: "6"}
    else:
        pgsize = {0x1000: "1", 0x4000: "2", 0x10000: "4", 0x40000: "8", 0x100000: "16", 0x400000: "32", 0x1000000: "64"}

    cfg["VIRT_RESERVE_RANGES"] = collect_reserved_ranges(reserved_list)

    str_unlock_after_boot = "{\\\n   "

    def tlb_split(obj,V,P,S):
        #
        #  Split the mapping from V -> P of size S
        #   into TLB entries
        #
        original_args = (V,P,S)
        valid_tlb = (2**24) | (2**22) | (2**20) | (2**18) | (2**16) | (2**14) | (2**12)
        max_tlb = (2**24)
        if isinstance( obj, CfgMemSection ):
            if valid_tlb & S != S:
                raise Exception( "Invalid TLB size of %u B for a CFBASE memsection" % S )
            yield ( V, P, S )
            return
        while S > 0:
            tmp = (V | P | max_tlb)     # Combine V, P, and MAX_TLB
            tmp = tmp & (-tmp)          # Isolate LSB
            while (tmp > S) or ((tmp & valid_tlb) == 0):
                # Either too big or not a valid TLB size
                tmp >>= 1
                if tmp == 0:
                    raise Exception('Illegal memsection request vaddr %X, paddr %X, size %X' % original_args)
            yield (V, P, tmp)
            V += tmp
            P += tmp
            S -= tmp

    for obj in mem_sections:
      for V,P,S in tlb_split(obj,obj.virt_addr, obj.phys_addr, obj.size):
        if obj.isTlbLock ():
            tlb_entries = tlb_entries + 1
            str_tlblock += "\\\n   MEMORY_MAP(" + obj.glob + ", 0, " + hex(V>>12) + ", "
            str_tlblock += obj.queryUserMode() + ", "
            str_tlblock += "%d, " % obj.queryBusAttr(range(4))
            str_tlblock += obj.attach.upper() + ", " + obj.cache_policy + ", " + pgsize[S] + ", MAIN, " + hex(P>>12) + "),"
            if obj.isTlbBootLock():
                str_unlock_after_boot += "{%#X,%#X},\\\n   " % (V>>12,S>>12)
            if isinstance( obj, CfgMemSection ):
                obj.set_mmap_index( tlb_entries - 1 )
        else:
            non_tlb_entries = non_tlb_entries + 1
            str += "\\\n   MEMORY_MAP(" + obj.glob + ", 0, " + hex(V>>12) + ", "
            str += obj.queryUserMode() + ", "
            str += "%d, " % obj.queryBusAttr(range(2))
            str += obj.attach.upper() + ", " + obj.cache_policy + ", " + pgsize[S] + ", MAIN, " + hex(P>>12) + "),"
            if isinstance( obj, CfgMemSection ):
                obj.set_mmap_index( non_tlb_entries - 1 )
      
    str_unlock_after_boot += "{0}\\\n}"

    # Instantiate EIP build records for Q6CFG regions. It must be done *AFTER*
    # build-time memory map table has been composed
    str_q6cfg = ""
    for obj in mem_sections:
        if isinstance( obj, CfgMemSection ):
            if obj.isTlbLock():
                mmap_index = obj.get_mmap_index()
            else:
                mmap_index = tlb_entries + obj.get_mmap_index()
            str_q6cfg += "DEFINE_Q6CFG_MMAP( %s, %u, %u );\n" % ( obj.name(), obj.getSize()/1024, mmap_index )
    cfg["Q6CFG"] = str_q6cfg

    if tlb_entries > 63:
        sys.exit("Number of tlb entries exceed the uper limit")
    str=str_tlblock+str
    cfg["UNLOCKAFTERBOOT"] = str_unlock_after_boot
    cfg["MEMORY_MAPS"] = str
    cfg["TLBLOCKNUM"] = repr(tlb_entries)

    bootmaps = program_el.find_children("boot_mapping")
    cfg["MAPEXTERNDECLS"] = '\n'.join([x.extern_decls() for x in bootmaps])
    cfg["BOOTMAPDEFINITIONS"] = ','.join([x.map_definition() for x in bootmaps] + ["{0}"])

    #QURTK_main_priority
    cfg["QURTKMAINPRIORITY"] = program_el.find_child("main_priority").value

    bitmask = hex (0xff)
    if hasattr(program_el, 'main_bitmask'):
        bitmask = program_el.find_child("main_bitmask").value

    #QURTK_main_bitmask
    cfg["QURTKMAINBITMASK"] = bitmask
   
    #QURT_reaper_priority
    param = program_el.find_child("reaper_priority")
    if param is None:
        cfg["QURTOS_REAPER_PRIORITY"] = "32"
    else:
        cfg["QURTOS_REAPER_PRIORITY"] = param.value

    #QURT_reaper_tcb_partition
    param = program_el.find_child("reaper_tcb_partition")
    if param is None:
        cfg["QURTOS_REAPER_TCB_PARTITION"] = "0"
    else:
        cfg["QURTOS_REAPER_TCB_PARTITION"] = param.value

    param = program_el.find_child("reaper_stack_size")
    if param is None:
        cfg["QURTOSREAPERSTACKSIZE"] = "4096"
    else:
        cfg["QURTOSREAPERSTACKSIZE"] = "%u" % param.value

    # Process reaper
    if int(find_default(program_el, 'process_reaper_enable', '1')):
        cfg.add_init_func('qurtos_process_reaper_generic_init')
        cfg['PROCESS_REAPER_ENABLE'] = '1'
        cfg['PROCESS_REAPER_PRIORITY'] = find_default(program_el,
                                                      'process_reaper_priority',
                                                      cfg['QURTOS_REAPER_PRIORITY'])
        cfg['PROCESS_REAPER_TCB_PARTITION'] = find_default(program_el,
                                                           'process_reaper_tcb_partition',
                                                           cfg['QURTOS_REAPER_TCB_PARTITION'])
        cfg['PROCESS_REAPER_STACK_SIZE'] = find_default(program_el,
                                                        'process_reaper_stack_size',
                                                        cfg['QURTOSREAPERSTACKSIZE'])
    else:
        cfg['PROCESS_REAPER_ENABLE'] = '0'

    #QURTK_max_pimutex_prio
    try:
        pimutex_prio = program_el.find_child("max_pimutex_prio").value
    except AttributeError:
        pimutex_prio = "0"
    cfg["MAXPIMUTEXPRIO"] = pimutex_prio
    
    try:
        first_repl = program_el.find_child("tlb_first_replaceable").value
        if (int(first_repl) > 128) or (int(first_repl) > 64 and asic != "v5"):
          sys.exit ("!!! First replaceable entry beyond limit \n")
    except AttributeError: 
        first_repl = "0"
        
    #QURTK_tlb_first_replaceable
    cfg["TLBFIRSTREPL"] = first_repl;
    
    try:
        osam_count = program_el.find_child("osam_stack_fill_count").value
    except AttributeError:
        osam_count = 128

    #qurt_osam_stack_fill_count
    cfg["OSAMSTACKFILLCOUNT"] = hex(osam_count)

    #QURTK_min_image_page
    try:
        min_image_pg = program_el.find_child("min_image_page").value
        if not check_page_size(min_image_pg):
            sys.exit("!!!min_page_size not a recognized page size \n")
    except AttributeError:
        min_image_pg = 0x40000

    #QURTK_min_image_page
    cfg["MINIMAGEPAGE"] = hex(min_image_pg)

    #QURTK_main_stack_size
    cfg["MAINSTACKSIZE"] = hex(program_el.find_child("stack_size").value)
    #QURTK_app_heap_size
    cfg["HEAPSIZE"] = hex(program_el.find_child("heap_size").value)
    #application heap type
    try:
        heap_type = 0
        heaptypedict = {'static':0, 'dynamic':1, 'aslr':2, 'appregion':3}
        heap_type = heaptypedict[program_el.find_child("heap_size").type.lower()]
    except Exception:
        pass
    cfg["QURTOSAPPHEAPTYPE"] = ("%d" % heap_type)


def collect_secure_program_info(cfg):
    progs = list(cfg.find_children('program'))
    for prog in progs:
        try:
            name = prog.name
            sid = prog.find_child('secure').SID
            match = prog.find_child('file_match').value
            index = progs.index(prog)
            #
            #  Send the information to qurt-image-build describing this program
            #
            cfg.send_to_builder(dict='secure_program_info',
                                key=prog.name,
                                sid=prog.find_child('secure').SID,
                                match=prog.find_child('file_match').value,
                                index=progs.index(prog))
        except AttributeError:
            pass
        except:
            raise

def collect_program_interrupts(cfg):
    """ Collect the interrupt attributes of the program element."""
       
    int_type = [0xffffffff for i in range(32) ]
    int_enable = [0 for i in range(32*16)]
    isr_data = [0 for i in range(16*16)]
    island_int_array = [0xffff for i in range(1025) ]
    
    program_count = 0
    program_names = ""
    str_isr = ""
    str_island_int = ""
    interrupt_scnt = 0
    max_int_num = 0
    int_privilege = 0
    island_int_num = 0

    for program_el in cfg.find_children("program"):
        if( program_count ==0 ):
            program_names += ( "\"" + program_el.name )
            program_names += ( "\"")
        else:
            program_names += (",\"" + program_el.name)
            program_names += ( "\"")
        program_count = program_count + 1

        for interrupt_el in program_el.find_children("interrupt"):
            interrupt_scnt = interrupt_scnt + 1
            for el in interrupt_el.find_children("irq"):
                el_no = int(el.num)
                element_no = el_no/32
                shift_no = el_no%32
                trigger = int_type[element_no]         
                irq_val = 1
                if el.trigger == "edge":
                    trigger = trigger | (irq_val << shift_no)
                else:
                    trigger = ~(~trigger | (irq_val << shift_no))          
                int_type[element_no] = trigger
                enable = int_enable[(program_count-1)*32 + element_no]         
                enable = enable | (irq_val << shift_no)        
                int_enable[(program_count-1)*32 + element_no] = enable
                if hasattr (el, "island"):
                    if(el.island == "true"):
                        island_int_array[island_int_num] = el_no
                        island_int_num = island_int_num + 1
                        # For now, island mode interupts are limited to max=6
                        if( island_int_num > 256):
                            sys.exit ("!!! Island mode interrupts are marked more than 256.  \n")
                if(el_no > max_int_num):
                    max_int_num = el_no

        element_no = 0
        isr_el = program_el.find_child("isr_callback")
        if isr_el is not None:
            for el in isr_el.find_children("irq"):
                el_no = int(el.num)
                if el.callback[:17] == "qurt_isr_callback":
                    isr_cb_no = int(el.callback[17:])
                else:
                    sys.exit ("!!! ISR callback function name is different from qurt_isr_callback.  \n")

                el_no = el_no | isr_cb_no << 10
                isr_data[(program_count-1)*16 + element_no] = el_no
                element_no = element_no + 1 


    if(interrupt_scnt > 0):
         machine_el = cfg.find_child("machine")
         if machine_el is not None:
             mint_el = machine_el.find_child("interrupt")
             if mint_el is not None:
                 sys.exit ("!!! Interrupt cannot be defined under both Machine and Program sections \n")

         int_privilege = 1
         str_type = ""
         str_type = hex(int_type[0])
         str_enable = ""
         str_enable = hex(int_enable[0])
         count = 1
         while (count < 32):
             str_type += ("," + hex(int_type[count]))
             str_enable += ("," + hex(int_enable[count]))
             count = count + 1
          
         while (count < 32*program_count):   
             str_enable += ("," + hex(int_enable[count]))
             count = count + 1
          
         str_isr = hex(isr_data[0])
         count = 1
         while (count < 16):
             str_isr += ("," + hex(isr_data[count]))
             count = count + 1
          
         while (count < 16*program_count):   
             str_isr += ("," + hex(isr_data[count]))
             count = count + 1

         str_island_int = hex(island_int_array[0])
         count = 1
         while (count < island_int_num):
             str_island_int += ("," + hex(island_int_array[count]))
             count = count + 1
          
         cfg["QURTKSIRCTYPE"] = str_type
         cfg["QURTKL2VICENABLE"] = str_enable
         cfg["MAXINT"] = repr(max_int_num)

    cfg["MAXPROGRAMS"] = repr(program_count)
    cfg["QURTKPROGNAMES"] = program_names
    cfg["INTPRIV"] = repr(int_privilege)
    cfg["ISRDATA"] = str_isr

    if(island_int_num > 0):
        cfg["INTISLANDDATA"] = str_island_int
        cfg["INTISLANDNUM"] = repr(island_int_num)


        
def collect_program_cache_partition(cfg):
    """ Collect the cache partition attributes in the program element."""
       
    # Maximum 16 PDs with default set to main partition
    cache_partition_data = 0x55555555 
    program_count = 0

    for program_el in cfg.find_children("program"):
        cache_partition_el = program_el.find_child("cache_partition")
        if cache_partition_el is not None:
            l2partition_el = cache_partition_el.find_child("l2_partition")
            if l2partition_el.attr == "cccc":
                partition_val = 0
            elif l2partition_el.attr == "main":
                partition_val = 1
            elif l2partition_el.attr == "aux":
                partition_val = 2
            elif l2partition_el.attr == "mini":
                partition_val = 3
            else:
                sys.exit ("!!! Error in Cache Partition Setting.  \n")

            cache_partition_data = cache_partition_data & (~(3 << (program_count*2)))
            cache_partition_data = cache_partition_data | (partition_val << (program_count*2))

        program_count = program_count + 1

    cfg["CACHEL2PARTDATA"] = hex(cache_partition_data)
  
          
def collect_program_internal_bus_priority(cfg):
    """ Collect internal_bus_priority attributes in the program element."""
       
    # Maximum 16 PDs with default set to bus_priority disabled
    bus_prio_data = 0x0 
    program_count = 0

    for program_el in cfg.find_children("program"):
        bus_prio_el = program_el.find_child("internal_bus_priority")
        if bus_prio_el is not None:
            if bus_prio_el.enabled == "true":
                bus_prio_val = 1
            elif bus_prio_el.enabled == "false":
                bus_prio_val = 0
            else:
                sys.exit ("!!! Error in internal_bus_priority Setting.  \n")

            bus_prio_data = bus_prio_data | (bus_prio_val << (program_count*2))


        program_count = program_count + 1

    cfg["INTERNALBUSPRIO"] = hex(bus_prio_data)
  
