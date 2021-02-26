#=============================================================================
#
#                                    kernel_xml.py
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
from ezxml import Element, str_attr, size_attr, long_attr
import re

'''    XML tag             AttrType   Req/Opt       C template regex       Default value (or None)    '''
kernel_db = [
    ["secured_kernel",     str_attr,  "optional", "SECUREDKERNEL",         "0"    ],
    ["hthread_mask",       str_attr,  "required", "HTHREADMASK",           None   ],
    ["heap_size",          size_attr, "optional", "KERNELHEAPSIZE",        0x1C000],
    ["heap_size_island",   size_attr, "optional", "KERNELHEAPSIZEISLAND",  0      ],
    ["heap_section_island",str_attr,  "optional", "ISLANDHEAPSECTION",     ".data.island"], 
    ["tlb_debug_level",    long_attr, "optional", "TLBDEBUGLEVEL",         0      ],
    ["dynamic_tlb_reservation",
                           long_attr, "optional", "DYNAMICTLBRESERVE",     -1     ],
    ["kernel_separation",  long_attr, "optional", "KERNELSEP",             0      ],
    ["split_virtual_pool", long_attr, "optional", "SPLITVIRTPOOL",         0      ],
    ["max_user_processes", long_attr, "optional", "MAXUSERPROCESSES",      8      ],
    ["max_threads",        str_attr,  "required", "MAXTHREADS",            None   ],
    ["max_threads_in_tcm", str_attr,  "optional", "MAXTHREADSINTCM",       "0"    ],
    ["tcm_utcb_from_stack",long_attr, "optional", "TCMUTCBFROMSTACK",      -1     ],
    ["max_futexes",        str_attr,  "required", "MAXFUTEXES",            None   ],
    ["qurtos_qdi_handles", str_attr,  "optional", "QURTOSQDIHANDLES",      "1024" ],
    ["qurtos_island_qdi_handles",
                           str_attr,  "optional", "QURTOSISLANDHANDLES",   "0"   ],
    ["trace_mask",         str_attr,  "required", "QURTKDEFAULTTRACEMASK", None   ],
    ["trace_size",         size_attr, "required", "TRACESIZE",             None   ],
    ["secure_process_sid", long_attr, "optional", "SPDSIDINFO",            0xffffffff],
    ["max_secure_processes", long_attr, "optional", "MAXSECUREPROCS",      2      ],
    ["max_mailboxes",      long_attr, "optional", "MAXMAILBOXES",          8      ],
    ["mailbox_bandwidth",  long_attr, "optional", "MAILBOXBANDW",          100    ],
    ["fastint_stack_size", size_attr, "optional", "FASTINTSTACKSIZE",      384    ],
    ["int_nest_allow_mask",size_attr, "optional", "L1NESTALLOW",           0      ],
    ["qurtos_stack_size",  size_attr, "optional", "QURTOSSTACKSIZE",       0      ],
    ["qurtos_island_stack_size",
                           size_attr, "optional", "QURTOSISLANDSTACKSIZE", 0      ],
    ["tcm_size",           size_attr, "optional", "TCMSIZE",               32     ],
    ["tcm_save_size",      size_attr, "optional", "TCMSAVESIZE",           0      ],
    ["QURTK_tcm_dump_type",size_attr, "optional", "QURTKTCMDUMPTYPE",      0      ],
    ["page_table_size",    size_attr, "optional", "PAGETABLESIZE",         "default"],  # Set in kernel_hook()
    ["debug_buf_size",     size_attr, "optional", "DEBUGBUFSIZE",          8192   ],
    ["isdb_debug",    size_attr, "optional", "QDSP6SSDBGCFGMASK",    0x20000000 ],
    ["isdb_debug_warmboot",    size_attr, "optional", "WBQDSP6SSDBGCFGMASK",    0x08000000 ],
	["osam_stack_usage_fill_count", size_attr, "optional", "OSAMSTACKUSAGEFILLCOUNT",    16 ],
    ["pager_total_pages",   size_attr, "optional", "PAGERTOTALPAGES", 0],
    ["pmu_stid_reserve_num", long_attr, "optional", "STIDRESNUM",          0      ]
]

#   user_processes      user_threads
object_cache_keys = '''
    memory_blocks
    virt_regions        phys_regions
    qurtos_threads      shmem_regions
'''

def object_cache_el():
    T = (long_attr, "optional")
    L = [(e,T) for e in object_cache_keys.split()]
    D = dict((k,v) for (k,v) in L)
    return Element("object_cache", **D)

def ramfs_scan_el():
    return Element("ramfs_scan",
                   paddr_start = (size_attr, "required"),
                   paddr_end = (size_attr, "required"),
                   paddr_step = (size_attr, "required"))

def object_cache_initializer(elems):
    L = [(e,'OBJCACHE_DEFAULT') for e in object_cache_keys.split()]
    D = dict((k,v) for (k,v) in L)
    for E in elems:
        D.update(E.__dict__)
    L = [".%s = %s" % (e,str(D[e])) for e in object_cache_keys.split()]
    S = ', \\\n   '.join(L)
    return S

# FIXME: support "fastint_stack size= " until cust_config.xml can move to "fastint_stack_size value= " 
Fastint_stack_el = Element("fastint_stack",
                      size = (str_attr,"optional"))

Tcmdump_el = Element("tcm_dump",
                      size = (size_attr, "optional"),
                      error = (str_attr, "optional"),
                      power = (str_attr, "optional"),
                      pool = (str_attr, "optional"))

VTLB_el = Element("vtlb",
                  entries = (size_attr, "optional"),
                  tree = (size_attr, "optional"),
                  section = (str_attr, "optional"))

Autostack_el = Element("autostack", 
                        pool_size = (size_attr, "optional"),
                        watermark_lo = (long_attr, "optional"),
                        watermark_hi = (long_attr, "optional"),
                        init_size = (size_attr, "optional"))

class _Mprotect:
    def __init__(self):
        self.ranges = []
    def __call__(self, cfg, *ls, **kw):
        for L1 in cfg.find_children("kernel"):
            for L2 in L1.find_children("mapping_protect"):
                for R in L2.find_children("region"):
                    if R.size > 0:
                        self.ranges.append((R.base, R.base+R.size-1))
        cfg.add_post_func(self.finish)
    def element(self):
        return Element("mapping_protect",
                       Element("region",
                               base=(long_attr, "required"),
                               size=(size_attr, "required")))
    def finish(self, cfg):
        # Convert each range to page numbers
        tmp = [(R[0] >> 12, R[1] >> 12) for R in self.ranges]

        # Sort ranges by starting page number
        tmp.sort()

        # Optionally do optimizations here
        # Leave this out for now, but get smarter in the future

        # Generate C code to enforce the ranges
        c_code = '__attribute__((section(".text.ukernel.island")))\n'
        c_code += 'unsigned QURTK_mprotect(unsigned val, unsigned lo, unsigned hi)\n{\n'
        for R in tmp:
            c_code += '  if (hi >= 0x%X && lo <= 0x%X) return 0;\n' % (R[0], R[1])
        c_code += '  return val;\n}\n'

        cfg['MPROTECT'] = c_code

Mprotect = _Mprotect()

Kernel_el = Element("kernel", object_cache_el(), ramfs_scan_el(), Fastint_stack_el, Mprotect.element(), Tcmdump_el, Autostack_el, VTLB_el, *[Element(e[0], value=(e[1],e[2])) for e in kernel_db]) 

def kernel_hook(cfg, name, value):
    if name == 'page_table_size':
        if value == 'default':
            if '-DCONFIG_VIRTUAL_TLB' in cfg.build_flags:
                # Default for VTLB is 40K
                value = size_attr("40K")
            else:
                # Default for non-VTLB is 144K
                value = size_attr("144K")
    if name == 'max_user_processes':
        if value > 0:
            # If max_user_processes is positive,
            #  add initialization functions to pull
            #  in user process support if CONFIG_MP is enabled
            if '-DCONFIG_MP' in cfg.build_flags:
                cfg.add_init_func('qurtos_user_client_init')
                cfg.add_init_func('qurtos_space_generic_init')
                cfg.add_init_func('qurtos_user_reaper_generic_init')
                cfg.add_init_func('qurtos_proxy_generic_init')
                cfg.add_init_func('qurtos_file_generic_init')
    ##collect all specials here            
    #total_specials = 0
    #if name == 'pager_total_pages':
    #    if value > 0:
    #        total_specials = total_specials +1
    ##autostack can be added here similarly
    #cfg['QURTKNUMSPECIALS'] = ('%u' % total_specials)
    #if name == 'autostack_total_pages':
    #    if value > 0:
     #       cfg.add_image_pool("AUTOSTACK_IMAGE_POOL", (value << 12), align = 0x1000)


    return value

def kernel_finish(cfg):
    ''' Do fixups after all information has been collected '''
    island_handles = int(cfg['QURTOSISLANDHANDLES'])
    main_handles = int(cfg['QURTOSQDIHANDLES'])

    secured_kernel = int(cfg['SECUREDKERNEL'])
    cfg.secured_kernel = (secured_kernel > 0)

    if '-DCONFIG_KERNEL_PAGETABLES' in cfg.build_flags:
        if int(cfg['DYNAMICTLBRESERVE'],0) < 0:
            cfg['DYNAMICTLBRESERVE'] = '16'

    if (island_handles == 0):
        # No island handle table
        main_handles = max(5, main_handles)
    elif (island_handles > 0) and (island_handles < main_handles) and (main_handles > 5):
        # Both island and main tables exist
        island_handles = max(5, island_handles)
        main_handles = main_handles - island_handles
    else:
        # No main handle table
        island_handles = max(5, max(island_handles, main_handles))
        main_handles = 0

    if island_handles > 4096 or main_handles > 4096:
        raise Exception('Too many QDI handles:  %u' % (island_handles+main_handles))

    cfg['QURTOSISLANDHANDLES'] = ('%u' % island_handles)
    cfg['QURTOSQDIHANDLES'] = ('%u' % main_handles)

    # Compute some info from the page table size

    v2page_bytes = (int(cfg['PAGETABLESIZE'], 0) + 63) & (~63)          # Number of bytes to set aside for page table
    v2page_bytes = min(v2page_bytes, 0x40000)                           # Silently truncate to 256K if it's too big
    cfg['V2PAGE_BLOCKS'] = ('%u' % (v2page_bytes / 64))                 # Number of 64-byte blocks in page table
    cfg['V2PAGE_ENTRIES'] = ('%u' % (v2page_bytes / 2))                 # Number of 2-byte entries in page table

''' Collect the attributes of the kernel element '''

def collect_kernel_element(cfg):
    k = cfg.find_child("kernel")
    if not k:
        return
    for e in kernel_db:
        try:
            v = k.find_child(e[0]).value
        except AttributeError:
            v = e[4]
            if v == None:
                print('Cannot parse kernel attribute ' + e[0])
                raise
        v = kernel_hook(cfg, e[0], v)
        if e[1] == size_attr or e[1] == long_attr:
            v = ('%#X' % v)
        cfg[e[3]] = v

    if '-DCONFIG_VIRTUAL_TLB' in cfg.build_flags:
        vtlb_elements = k.find_children('vtlb')
        if not vtlb_elements:
            #
            #  No VTLB was given.  Synthesize one
            #
            el = (lambda:None)                          # Object we can hang attributes on
            sz = int(cfg['PAGETABLESIZE'], 0)
            if sz > 0x40000:                            # Temp fix, if size is greater than 256K,
                sz = 0xA000                             #   set it to 40K
            el.entries = ((sz-224)+19)/20               # (sz-224)/20, rounded up
            el.entries = (el.entries+15)&(~15)          # Round up to multiple of 16
            #el.entries = min(el.entries, 32768)        # each index into vtlb entries array is a short (2^16) and odd ( /2)
            if el.entries > 32768:
                raise Exception('Cannot synthesize VTLB, pagetable size provided will exceed maximum of 32768 number of entries ... ')
            el.entries = max(el.entries,128)            # Force it to be at least 128
            el.tree = 8*el.entries
            vtlb_elements.append(el)
        if len(vtlb_elements) > 1:
            raise Exception('Multiple VTLB elements not supported yet')
        for el in vtlb_elements:
            if not hasattr(el,'entries'):
                raise Exception('VTLB element must specify a value for entries')
            if el.entries > 32768:
                raise Exception('number of VTLB entries provided exceeds maximum of 32768')
            if not hasattr(el,'tree'):
                raise Exception('VTLB element must specify a value for tree')
            if el.tree > (32768 *4):
                raise Exception('VTLB size of tree too large')
            cfg['VTLB_ENTRIES'] = '%u' % el.entries
            cfg['VTLB_TREE']    = '%u' % (((el.tree+31)&(~31))/2)       # Round up to multiple of 32 and divide by 2
            cfg['VTLB_SECTION'] = '"%s"' % el.__dict__.get('section', '.data.ukernel.main')

    try:
        if(k.find_child("max_user_processes").value):
            if '-DCONFIG_MP' not in cfg.build_flags:
                raise Exception('max_user_processes set > 0 when CONFIG_MP is disabled') 
    except AttributeError:
        if '-DCONFIG_MP' not in cfg.build_flags:
            cfg['MAXUSERPROCESSES'] = '0'

    # Support "tcm_size" until cust_config.xml can move to "tcm_dump"
    cfg['QURTKTCMDUMPTYPE'] = None
    try:
        tcm_dump_size = k.find_child("tcm_dump").size
        cfg['TCMSIZE'] = ('%#X' % tcm_dump_size)
    except AttributeError:
        cfg['QURTKTCMDUMPTYPE'] = '0'
    
    # If tcm_dump element had 'size' as an attribute, then do the below
    if cfg['QURTKTCMDUMPTYPE'] == None:
        try:
            tcm_dump_error_type = k.find_child("tcm_dump").error
        except AttributeError:
            tcm_dump_error_type = None
            
        # Currently, 'static' and 'user' are the only supported options for 'error'
        if tcm_dump_error_type != 'static' and tcm_dump_error_type != 'user' and tcm_dump_error_type != None:
            raise Exception('1. "error" attribute can only be "static" or "user" under tcm_dump tag in the cust_config xml file')
        
        try:
            tcm_dump_power_type = k.find_child("tcm_dump").power
        except AttributeError:
            tcm_dump_power_type = None
            
        # Currently, 'static', 'none' and 'user' are the only supported options for 'power'
        if tcm_dump_power_type != 'static' and tcm_dump_power_type != 'user' and tcm_dump_power_type != 'none' and tcm_dump_power_type != 'pool' and tcm_dump_power_type != None:
            raise Exception('"power" attribute can only be "static", "user", "none", "pool" under tcm_dump tag in the cust_config xml file')
            
        if tcm_dump_error_type == None and tcm_dump_power_type == None:
            tcm_dump_error_type = 'static'
            tcm_dump_power_type = 'static'
            
        if tcm_dump_error_type == None and tcm_dump_power_type == 'static':
            tcm_dump_error_type = 'static'
        if tcm_dump_error_type == None:
            if tcm_dump_power_type != 'static':
                raise Exception('2. "error" attribute needs to be specified under tcm_dump tag in the cust_config xml file')
        
        if tcm_dump_power_type == None and tcm_dump_error_type == 'static':
            tcm_dump_power_type = 'static'
        if tcm_dump_power_type == None:
            if tcm_dump_error_type != 'static':
                raise Exception('"power" attribute needs to be specified under tcm_dump tag in the cust_config xml file')

        # If "error" is "static", then irrespective of what is given for "power",
        # both error dump and PC will be stored at the static location in cust_config
        if tcm_dump_error_type == 'static' and tcm_dump_power_type == 'static':
            cfg['QURTKTCMDUMPTYPE'] = '0'
        if tcm_dump_error_type == 'static' and tcm_dump_power_type == 'user':
            cfg['QURTKTCMDUMPTYPE'] = '0'
        if tcm_dump_error_type == 'user' and tcm_dump_power_type == 'static':
            cfg['QURTKTCMDUMPTYPE'] = '0'
            
        # If "error" is "user", then "power" can have the following values:
        # "none": PC is not supported
        # "pool": TCM during pc will be saved at the pool provided in the tag
        # "user": TCM will be saved at "qurt_tcm_power_lcs_loc" provided in the
        # linker script by the user
        if tcm_dump_error_type == 'user' and tcm_dump_power_type == 'none':
            cfg['QURTKTCMDUMPTYPE'] = '1'
        if tcm_dump_error_type == 'static' and tcm_dump_power_type == 'none':
            cfg['QURTKTCMDUMPTYPE'] = '0'
        if tcm_dump_power_type == 'pool':
            try:
                tcm_swap_pool_name = k.find_child("tcm_dump").pool
                cfg['QURTCFGTCMDUMPPOOL'] = ('%s' % tcm_swap_pool_name)
                if tcm_dump_error_type == 'user':
                    cfg['QURTKTCMDUMPTYPE'] = '2'
                if tcm_dump_error_type == 'static':
                    cfg['QURTKTCMDUMPTYPE'] = '0'
            except AttributeError:
                # If "pool" is specified as the type for "power" then a pool name
                # has to be provided
                raise Exception('A pool name needs to be provided if "power"="pool" in tcm_dump tag in cust_config xml file')
        
        if tcm_dump_error_type == 'user' and tcm_dump_power_type == 'user':
            cfg['QURTKTCMDUMPTYPE'] = '3'

    # FIXME: support "fastint_stack size= " until cust_config.xml can move to "fastint_stack_size value= " 
    try:
        cfg['FASTINTSTACKSIZE'] = k.find_child("fastint_stack").size
    except AttributeError:
        pass
    Mprotect(cfg)
    cfg['OBJCACHECONFIG'] = object_cache_initializer(k.find_children('object_cache'))
    cfg['RAMFSSCAN'] = '0,0,0'
    try:
        x = k.find_child('ramfs_scan')
        cfg['RAMFSSCAN'] = '0x%X,0x%X,0x%X' % (x.paddr_start / 0x1000, x.paddr_end / 0x1000, x.paddr_step / 0x1000)
    except AttributeError:
        pass
    
    #collect autostack element
    try: 
        autostack_element = k.find_child("autostack")
        auto_pool_size = int(autostack_element.pool_size)
        cfg['AUTOSTACKTOTALPAGES'] = ('%d' % (auto_pool_size>>12)) 
        if (auto_pool_size >> 12) > 0:
            cfg.add_image_pool("AUTOSTACK_IMAGE_POOL", (auto_pool_size), align = 0x1000)
        else:
            raise Exception('Autostack pool needs to be at least 0x1000 bytes in size')

        #lo mark
        if not hasattr(autostack_element, 'watermark_lo'):
            lo_watermark = 0
        else:
            lo_watermark = int(autostack_element.watermark_lo)
        cfg['AUTOLO'] = ('%d' % (lo_watermark>>12))

        #hi mark
        if not hasattr(autostack_element, 'watermark_hi'):
            hi_watermark = int((auto_pool_size/2))
        else:
            hi_watermark = int(autostack_element.watermark_hi)
        cfg['AUTOHI'] = ('%d' % (hi_watermark>>12)) 
        
        #except on a couple error scenarios
        if (hi_watermark < lo_watermark):
            raise Exception('Autostack watermarks error: hi < lo...')
        if (hi_watermark == 0):
            raise Exception('Autostack watermarks error: hi == 0 ...')

        #initial stack size
        if not hasattr(autostack_element, 'init_size'):
            init_stack_size = int(4096)
        else:
            init_stack_size = int(autostack_element.init_size)
        cfg['AUTOINITSIZE'] = ('%d' % init_stack_size)

    except AttributeError:
         cfg['AUTOSTACKTOTALPAGES'] = '0'
         cfg['AUTOHI'] = '0'
         cfg['AUTOLO'] = '0'
         cfg['AUTOINITSIZE'] = '0'

    cfg.add_post_func(kernel_finish)
