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

class AutoImagePool:
    def __init__(self, name, size, **kw):
        self.tag = 'physical_pool'
        self.name = name
        self.region = (lambda: None)       # An object we can hang attributes on
        self.region.allocate = 'image'
        self.region.size = size
        for k,v in kw.items():
            setattr(self.region, k, v)
    def find_children(self, name):
        return [self.region]

class QurtXmlConfig:
    '''
    Implements a class object which can be passed around through
    the other libraries that make up the QuRT XML configuration
    utility.  This class provides a place to store "global"
    information about the configuration, and it provides some
    utility methods that can be used by the various parsers.
    '''

    def __init__(cfg):
        from genkernel import QurtVars
        QurtVars(cfg)

    def __getitem__(cfg, key):
        return cfg.template_dict[key]

    def __setitem__(cfg, key, value):
        cfg.template_dict[key] = value

    def add_post_func(cfg, function):
        cfg.post_funcs.append(function)

    def add_init_func(cfg, function_name):
        cfg.init_funcs.append(function_name)

    def add_phys_reloc(cfg, var):
        cfg.phys_relocs.append(var)

    def add_gen_reloc(cfg, var, delta):
        cfg.gen_relocs.append('{%s,%d}' % (var,delta))

    def add_image_pool(cfg, *ls, **kw):
        # Example:
        #  cfg.add_image_pool("AutoStackPool", 0x4000, align = 0x1000)
        cfg.parsed.children.append(AutoImagePool(*ls, **kw))

    def add_region_cache_policy(cfg, pool_num, region_num, cache_policy):
        cfg.region_cache_policies.extend([pool_num, region_num, cache_policy])

    def find_child(cfg, tagname):
        return cfg.parsed.find_child(tagname)

    def find_children(cfg, tagname):
        return cfg.parsed.find_children(tagname)

    def start_builder(cfg):
        import xml.dom.minidom
        cfg.sender_doc = xml.dom.minidom.parseString('<build_vars/>')

    def send_to_builder(cfg, **kw):
        #
        #  Send a piece of information to the image builder
        #
        if ('dict' in kw) and ('key' in kw):
            # If dict and key are set, don't send the values as individual values,
            #  send them as a single unit.
            el = cfg.sender_doc.createElement('add_to_dict')
            for key in kw.keys():
                el.setAttribute(key, str(kw.pop(key)))
            cfg.sender_doc.documentElement.appendChild(el)
            return

        for key, value in kw.items():
            el = cfg.sender_doc.createElement(str(key))
            el.setAttribute('value',str(value))
            cfg.sender_doc.documentElement.appendChild(el)

    def finish_builder(cfg):
        cfg.send_to_builder(region_cache_policies = ' '.join(map(str,cfg.region_cache_policies)))
        value, length = cfg.c_stringify(cfg.sender_doc.toxml('utf-8'), double=True)
        cfg['XMLSENDER'] = value
        cfg['XMLSENDERLENGTH'] = ('%u' % length)

    def alloc_slot(cfg, varname):
        ret = len(cfg.slots)
        cfg.slots.append(varname)
        return '%d' % ret

    def template_compile(cfg):
        from re import compile
        newdict = dict()
        for k,v in cfg.template_dict.items():
            key = '\\{:' + k + '\\}'
            key = compile(key)
            newdict[key] = v
        cfg.compiled_dict = newdict

    def main(cfg, options, args):
        import sys, os, re
        from parse_spec import QurtImageParse
        from machine_xml import collect_machine_element
        from kernel_xml import collect_kernel_element
        from program_xml import collect_program_element
        from physpool_xml import collect_physpool_element, collect_srm_resources
        from interrupt_xml import collect_interrupt_element
        from build_xml import collect_build_element
        from parse_build_params import parse_build_params
        from build_qurt_config import build_qurt_config

        for k,v in vars(options).items():
            setattr(cfg,k,v)

        #ignore_name does not seem to be used anywhere
        #cfg.ignore_name = re.compile(cfg.ignore_name)

        cfg.start_builder()
        cfg.spec_file = args[0]
        cfg.build_flags = []
        cfg.template_dict = dict()
        cfg.post_funcs = []
        cfg.init_funcs = []
        cfg.phys_relocs = []
        cfg.gen_relocs = []
        cfg.build_elements = []
        cfg.slots = []
        cfg.region_cache_policies = []
        if cfg.build_options:
            cfg.build_flags.extend(cfg.build_options.split())
        parse_build_params(cfg)

        QurtImageParse(cfg).do_parse()

        import qurt_consts
        for k,v in qurt_consts.__dict__.items():
            if not k.startswith('_'):
                cfg[k] = str(v)

        cfg['PHYSPOOLS'] = ''

        collect_machine_element(cfg)
        collect_kernel_element(cfg)
        collect_program_element(cfg)
        collect_physpool_element(cfg)
        collect_srm_resources(cfg)
        collect_interrupt_element(cfg)
        collect_build_element(cfg)
    
        if cfg.slots:
            xstr = ''
            for var in cfg.slots:
                xstr += 'extern const char %s[];\n' % var
            xstr += 'const char * const qurtos_slots[] = {'
            xstr += ','.join(cfg.slots)
            xstr += '};'
            cfg['CFGSLOTS'] = xstr
        else:
            cfg['CFGSLOTS'] = ''

        cfg['CFG_INIT_FUNC_PROTOS'] = '\n'.join(['INIT_FUNC_PROTO(%s);' % s for s in cfg.init_funcs])
        cfg['CFG_INIT_FUNCS'] = '{ ' + ', '.join(cfg.init_funcs+['0']) + ' }'
        cfg['PHYSRELOCS'] = '{ ' + ', '.join(cfg.phys_relocs+['0']) + ' }'
        cfg['GENRELOCS'] = '{ ' + ', '.join(cfg.gen_relocs+['{0}']) + ' }'

        # Call post_funcs -- these gives a last chance to check, modify, correct, or fail
        #  the configuration as it has been built.
        [fn(cfg) for fn in cfg.post_funcs]

        cfg.finish_builder()

        cfg.template_compile()

        # Read into c_code the file that contains our C template
        fi = open(os.path.join(os.path.dirname(sys.argv[0]),'Input','cust_config.c'))
        c_code = fi.read()
        fi.close()

        # Fill in the template
        for k,v in cfg.compiled_dict.items():
            c_code = re.sub(k, v, c_code)

        # If the object destination is a directory, add qurt_config.o
        if os.path.isdir(cfg.object_path):
            cfg.object_path = os.path.join(cfg.object_path, 'qurt_config.o')

        if cfg.src_only:
            cfg.cfile_path = cfg.object_path
        else:
            # C file goes the same place as the object, but named qurt_config.c
            cfg.cfile_path = os.path.join(os.path.dirname(cfg.object_path), 'qurt_config.c')

        # Write the filled-in template to the filesystem
        fo = open(cfg.cfile_path, 'w')
        fo.write(c_code)
        fo.write(getattr(cfg,'symtab_code',''))
        fo.write(getattr(cfg,'strtab_code',''))
        fo.close()

        if not cfg.src_only:
            build_qurt_config(cfg)

        return 0

def merge_cmd(args):
    '''
    Called when "qurt_config.py update ..." is executed.
    Creates a parser and runs the command line through it,
    then creates a QurtXmlConfig class instance and passes
    the parsed command line to its main() method.
    '''

    from optparse import OptionParser

    parser = OptionParser("%prog update [options] specfile", add_help_option=0)

    parser.add_option("-H", "--help", action="help")
    parser.add_option("-o", "--output", dest="object_path", metavar="FILE",
                      help="Specify the binary file output path")
    parser.add_option('-i', "--ignore", dest="ignore_name",
                      action="store", default="^$",
                      help="A regex specifying programs to be ignored.")
    parser.add_option('-S', "--string", dest="spec_is_string",
                      action="store_true",
                      help="Treat the specfile argument as an XML string.")
    parser.add_option('-B', "--build_options", dest="build_options",
                      help="Add further compiler options.")
    parser.add_option('-T', "--tools_path", dest="tools_path",
                      help="Specify the tools path")
    parser.add_option('-L', "--use_llvm", dest="use_llvm",
                      action="store_true",
                      help="Use LLVM tools rather than the default GCC")
    parser.add_option('-C', "--src_only", dest="src_only",
                      action="store_true",
                      help="Do not attempt to compile")

    return QurtXmlConfig().main(*parser.parse_args(args))
