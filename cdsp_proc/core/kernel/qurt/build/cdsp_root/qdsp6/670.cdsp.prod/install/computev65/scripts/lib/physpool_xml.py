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
Processing of the physpool XML element.
"""

from ezxml import Element, long_attr, str_attr, size_attr
from qurt import *

Region_el = Element("region",
                    allocate = (str_attr, "optional"),
                    section = (str_attr, "optional"),
                    align = (size_attr, "optional"),
                    base = (long_attr, "optional"),
                    padding = (size_attr, "optional"),
                    name = (str_attr, "optional"),
                    cache_policy = (str_attr, "optional"),
                    size = (size_attr, "required"),
                    type = (str_attr, "optional"))

SrmResources_el = Element("srm_resources",
                          Region_el)

PhysicalPool_el = Element("physical_pool",
                            Region_el,
                            name = (str_attr, "required"))

def verify_name(cfg, name):
    if name == '':
        raise Exception('Pool name must be non-empty')
    if len(name) > 31:
        raise Exception('Pool name is too long: %s' % name)
    if '\0' in name:
        raise Exception('Pool name contains embedded NUL byte: %s' % name)
    if getattr(cfg, 'POOLCHECK_' + name, False):
        raise Exception('Duplicate pool name: %s' % name)
    setattr(cfg, 'POOLCHECK_' + name, True)

def verify_region(cfg, base, size):
    if base == 0xFFFFFFFF000:               # image region, skip the checks
        return
    if (base & 0xFFF) != 0 or base < 0 or base >= 2**36:
        raise Exception('Illegal pool region address:  %X' % base)
    if (size & 0xFFF) != 0:
        print '*** WARNING -- QuRT XML pool region sizes should be multiples of 4K bytes ***'
    if size <= 0 or size > 2**36:
        raise Exception('Illegal pool region size:  %X' % size)
    if (base+size) > 2**36:
        raise Exception('Illegal pool region, extends beyond 64GB:  %X,%X' % (base,size))
    #
    # We used to flag pools which cross a 4GB boundary, but that
    #  should not be necessary any longer.
    #
    # Explicitly allow overlapping regions for now, as they are
    #  being used in some capacity.
    #
    pass

def add_physpool(cfg, name, regions=[]):
    if not hasattr(cfg, 'physpools'):
        cfg.physpools = ['']
        cfg.physpoolcount = 0
    cfg.physpools.append('    POOLSTART("%s")' % name)
    for i in range(len(regions)):
        r = regions[i]
        verify_region(cfg, r[0], r[1])          # Verify region in both the run-time configuration
        verify_region(cfg, r[0], r[1]+r[2])     #  and the build-time configuration
        cfg.physpools.append('        POOLREGION(0x%X,0x%X)' % (r[0] >> 12, r[1]+r[2]))
        if r[2]:
            cfg.add_gen_reloc('&pool_configs[%u].ranges[%u].size' % (cfg.physpoolcount, i), -r[2])
        if name == 'DEFAULT_PHYSPOOL' and i == 0:
            cfg.send_to_builder(effective_memsize = r[1])
            cfg.send_to_builder(total_memsize = r[1]+r[2])
        if r[3] != None:
            #
            # This region has a name.
            # Send the region name over to qurt-image-build along with the
            #  pool name and the index into the region list.
            #
            # In qurt-image-build, the region name info will be retrievable as:
            #    cfg.region_names[name].pool
            #    cfg.region_names[name].index
            #
            cfg.send_to_builder(dict='region_names', key=r[3], pool=name, index=i)
        if r[4] >= 0:
            # Mark the pool number, region number, and cache policy
            cfg.add_region_cache_policy(cfg.physpoolcount, i, r[4])
    cfg.physpools.append('    POOLEND')
    cfg.physpoolcount += 1
    if name == '':
        cfg.physpools.append('')

def handle_image_pools(cfg, physpool_el):
    codes = ['','']        # ASM CODE in first, C CODE in second
    pcount = 0
    count = 0
    swap_pool_defined = 0
    for el in physpool_el:
        rcount = 0
        for r in el.find_children('region'):
            if getattr(r,'allocate',None) == 'image':
                if getattr(r,'base',None) != None:
                    raise Exception('Pool region cannot specify allocate=image and a base address')
                r.base = 0xFFFFFFFF000       # Becomes 0xFFFFFFFF when we shift it
                r.size = (r.size + 0xFFF) & ~0xFFF
                size = r.size
                secname = getattr(r,'section','.pool.space')
                alignval = getattr(r,'align',0x1000)
                if alignval < 0x1000:
                    raise Exception('Pool region cannot specify an alignment less than 4K')
                symbol = 'pool_image_region_%u' % count
                asm_code = []
                asm_code.append('    .pushsection %s,"a",@nobits' % secname)
                asm_code.append('    .align %u' % alignval)
                asm_code.append('    .global %s' % symbol)
                asm_code.append('%s:' % symbol)
                asm_code.append('    .space %u' % size)
                asm_code.append('    .popsection')
                codes[0] += '\n'.join(asm_code)+'\n'
                c_code = []
                c_code.append('    __asm__("%%0 = ##%s" : "=r" (addr));' % symbol)
                c_code.append('    (*pfn)(pool_configs[%u].name,&pool_configs[%u].ranges[%u],addr);' % (pcount,pcount,rcount))
                if cfg['QURTKTCMDUMPTYPE'] == '2':
                    if el.name == cfg['QURTCFGTCMDUMPPOOL']:
                        swap_pool_defined = 1
                        c_code.append('    extern unsigned char %s;' %symbol)
                        c_code.append('    qurt_tcm_dump = &%s;\n' % symbol)
                codes[1] += '\n'.join(c_code)+'\n'
                count += 1
            rcount += 1
        pcount += 1
    if cfg['QURTKTCMDUMPTYPE'] == '2':
        if swap_pool_defined == 0:
            raise Exception('ERROR: TCM dump tag requires "%s" physpool that is not defined in the xml' % cfg['QURTCFGTCMDUMPPOOL'])
    if codes[0]:
        codes[0] = codes[0][:-1]
        codes[0] = codes[0].replace('"','\\"')
        codes[0] = codes[0].replace('\n','\\\\n"\n    "')
        codes[0] = '__asm__(\n    "' + codes[0] + '\\\\n"\n    );\n'
    if codes[1]:
        codes[1] = '\n    void *addr;\n\n' + codes[1]
    codes[1] = '{%s}\n\n' % codes[1]
    cfg['IMAGEPOOLINSERT'] = codes[1]+codes[0]

def handle_island_pools(cfg, physpool_el):
    default_physpool_el = None
    pcount = 0
    pool_relocs_str = ''
    for el in physpool_el:
        pool_reloc_ranges = 0
        rcount = 0
        # for the DEFAULT_PHYSPOOL save the region in below format:
        #   [0] = base address
        #   [1] = size in bytes
        #   [2] = number of bytes to be added to the pool descriptor
        #         in the memory image and then subtracted during bootup;
        #         this can either be from the 'padding' attribute or it
        #         can represent memory "stolen" from the end of the
        #         pool for other purposes, such as the image build utility.
        #
        if el.name == 'DEFAULT_PHYSPOOL':
            default_physpool_el = el
        for r in el.find_children('region'):
            if getattr(r,'allocate',None) == 'island':
                if getattr(r,'base',None) != None:
                    raise Exception('Pool region cannot specify allocate=island and a base address')
                if default_physpool_el:
                    default_physpool_region = [[x.base, x.size, getattr(x,'padding',0)] for x in default_physpool_el.find_children('region')]
                    default_physpool_region[0][1] -= r.size            # Take space from end of the first region
                    default_physpool_region[0][2] += r.size            #  and mark that we took it
                    #  island is taken from the default physpool,
                    #  the start address should be taken
                    #  relative to the physical base of the load image
                    r.base = default_physpool_region[0][1]
                    # Adjust the pool start address during bootup
                    cfg.add_phys_reloc('&pool_configs[%u].ranges[%u].start' % (pcount, rcount))
                    # Bitmask of range indexes per pool which have relocatable addresses
                    pool_reloc_ranges |= (1 << rcount)

                    #Adjust DEFAULT_PHYSPOOL 'padding/memory stolen' attribute here
                    setattr(default_physpool_el.find_children('region')[0], 'size', default_physpool_region[0][1])
                    setattr(default_physpool_el.find_children('region')[0], 'padding', default_physpool_region[0][2])
                else:
                    raise Exception('DEFAULT_PHYSPOOL region is empty')
            rcount += 1        
        if pool_reloc_ranges:
            #
            # This pool has regions with relocatable addresses.
            # Send a bitmap of relocatable region indexes over to qurt-image-build along with the
            #  pool name as a key
            #
            # In qurt-image-build, the bitmap of region indexes will be retrievable as:
            #    cfg.pool_relocs[pool_name].range_idx
            #
            cfg.send_to_builder(dict='pool_relocs', key=el.name, range_bitmap=pool_reloc_ranges)
        pcount += 1

def collect_physpool_element(cfg):
    """Collect the attributes of the machine element."""
    physpool_el = cfg.find_children("physical_pool")
    cfg['IMAGEPOOLINSERT'] = '{}'

    if not physpool_el:
        cfg['DOPOOLRELOC'] = '0'
        return 
   
    policydict = dict([(el.name, el.value) for el in cfg.find_child('machine').find_children('cache_policy')])
    policydict[None] = '-1'

    # Make a new list with DEFAULT_PHYSPOOL at the front
    #  and with everything else behind
    physpool_el = [el for el in physpool_el if el.name == 'DEFAULT_PHYSPOOL'] + \
                  [el for el in physpool_el if el.name != 'DEFAULT_PHYSPOOL']

    handle_image_pools(cfg, physpool_el)
    handle_island_pools(cfg, physpool_el)

    #pool_configs
    doreloc = '0'
    for el in physpool_el:
        verify_name(cfg, el.name)
        #
        # for each region:
        #   [0] = base address
        #   [1] = size in bytes
        #   [2] = number of bytes to be added to the pool descriptor
        #         in the memory image and then subtracted during bootup;
        #         this can either be from the 'padding' attribute or it
        #         can represent memory "stolen" from the end of the
        #         pool for other purposes, such as the image build utility.
        #   [3] = name of the region, or None if no name exists
        #   [4] = cache policy for the region (-1 if none specified)
        #
        r = [[x.base, x.size, getattr(x,'padding',0), getattr(x,'name',None),
              int(policydict[getattr(x,'cache_policy',None)],0)] for x in el.find_children('region')]
        add_physpool(cfg, el.name, r)

    add_physpool(cfg, '')
    cfg['DOPOOLRELOC'] = doreloc
    cfg['PHYSPOOLS'] = ' \\\n'.join(cfg.physpools)

def collect_srm_resources(cfg):
    #
    #  Collect the SRM resources from the XML configuration
    #

    if not ('-DCONFIG_SRM' in cfg.build_flags):
        #
        #  Not configured for SRM.  Just return.  Ignore any
        #   SRM resources configured in the XML.
        #
        cfg['SRMREJECT'] = ';'       # Make the function definition look like a prototype
        return

    regions = []

    for S in cfg.find_children('srm_resources'):
        for R in S.find_children('region'):
            regions.append(R)

    #
    #  Create a QurtAddressSpace object representing the entire 36-bit physical address space
    #

    A = QurtAddressSpace()
    A.alloc(left = -QurtInfinity, right = 0, reserve_only=True)
    A.alloc(left = 2**36, right = QurtInfinity, reserve_only=True)

    #
    #  Allocate all SRM resources out of the object
    #

    for R in regions:
        A.alloc(addr=R.base, size=R.size, reserve_only=True)

    #
    #  At this point, the remaining unallocated ranges in A consist of all of
    #   the memory that's NOT SRM protected.
    #

    valid_ranges = [r for r in A.ranges if not isinstance(r, AllocatedRange)]

    #
    #  The rest of this function builds a C function that returns 1 if
    #   a given (pageno,pgcnt) overlaps SRM protected resources, and
    #   returns 0 otherwise.
    #
    #  It uses a switch statement with a separate case for every legal
    #   (non-SRM) region in the address range.  If a range passed in
    #   fits entirely inside one of the legal regions, the function
    #   will return 0.  If a range does not fit into one of the legal
    #   regions, the function will return 1.
    #

    c_code = []

    c_code.append('{')
    c_code.append('   switch (pageno) {')

    for range in valid_ranges:
        left = range.left             # Starting address
        right = range.right           # Ending address + 1 (Start of invalid range)
        left = alignup(left, 4096)
        right = aligndown(right, 4096)
        if right > left:
            #
            #  A valid region exists here
            #
            left = left >> 12
            right = right >> 12
            c_code.append('   case 0x%X ... 0x%X: return (pgcnt > (0x%X-pageno));' % (left, right-1, right))

    c_code.append('   default: return 1;')
    c_code.append('   }')
    c_code.append('}')

    cfg['SRMREJECT'] = '\n'.join(c_code)
