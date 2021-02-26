#=============================================================================
#
#                                    qurt_config.py
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
#!/usr/bin/env python

class qurt_config:
    def genheader_subcommand(self, arglist):
        from lib.genheader import genheader_cmd
        return genheader_cmd(arglist)
    def genkernel_subcommand(self, arglist):
        from lib.genkernel import genkernel_cmd
        return genkernel_cmd(arglist)
    def update_subcommand(self, arglist):
        from lib.merge import merge_cmd
        return merge_cmd(arglist)
    def usage(self):
        cmds = sorted([z.rsplit('_',1)[0] for z in dir(self) if z.endswith('_subcommand')])
        str = 'First argument must be one of:\n  ' + ', '.join(cmds)
        raise Exception(str)
    def run_command(self, argv):
        from traceback import format_exc as tbstr
        progname = argv[0]
        try:
            print ' '.join(argv)
            raw_args = argv[1:]
            args = [s for s in raw_args if not s == '--traceback']
            if args == raw_args:
                tbstr = None
            try:
                subfunc = getattr(self, '%s_subcommand' % args[0])
            except StandardError:
                self.usage()
            return subfunc(args[1:])
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception, err:
            import sys
            if tbstr:
                print >> sys.stderr, tbstr()
            print >> sys.stderr, '%s: Error:\n*** %s' % (progname, err)
        except:
            raise
        return 1
    def main(self):
        import sys
        sys.exit(self.run_command(sys.argv))

def attach_config_vars(cfg):
    #
    #  This function is called by the kernel build procedure
    #   to get access to the configuration variable database.
    #
    import os
    import imp
    lib = imp.load_module('lib',*imp.find_module('lib',[os.path.dirname(__file__)]))
    from lib.genkernel import QurtVars
    QurtVars(cfg)

if __name__ == '__main__':
    qurt_config().main()    # Never returns

# Signatures of the files that this depends on
# 0d4a45abb0f86b0671f2efeeec8a7036 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/cust_config_template.c
# d073c41e00df2ea7c34e2d8102556211 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/default_build_config.def
# 944ae4735c2a9878b74f170d673546cc /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/static_build_config.def
# 5bdb8cd95cf82507b4b103ed57bad354 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/qurt_tlb_unlock.xml
# 963f02d964059f1e185e101fdb3fbbe9 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/qurt_default.lcs
# f3b27684399aa6a8618b2c2de49222f9 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/qurt_whitelist.txt
# 82fc44b8836571a8c447b8f56aec59d9 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/__init__.py
# df95cfea7f7213272a0c81d88cce6d91 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/basefile.py
# fc96dd73348fba755e7a71b6bf8f3fdf /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/build_qurt_config.py
# 55dbbf6bfbb4d869adbf951edf632611 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/build_xml.py
# d954839d5141199dbaa932eb1588538a /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/cfgvars.py
# 6384f8979a24025e7f8c6cf2205c7e3c /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/elf_info_patch.py
# 857cfdbdbcb81b388444d7d47a4bd916 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/ezxml.py
# 9abba5ff04b9d874acb683fe26123b2b /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/genheader.py
# 4e79327e51e36fcd5df84df0eca744ce /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/genkernel.py
# c083132b08dbc9fe4f0e2ad37e4cab81 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/interrupt_xml.py
# d0d5b0e053b55897c3512fd3c6b55e87 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/kernel_xml.py
# 55c09ae7706c0f6ad9028428912822c4 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/l2cfg_xml.py
# 7867f0731f24bf8120367da03c42595e /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/machine_xml.py
# faa356cf7e46cd1096f7776d4b4e7aea /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/memsection_xml.py
# 231ce8ac26439d6bb934223c93dc296c /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/merge.py
# 3d5008ec9d0bc9b925dc044210fbdc25 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/parse_build_params.py
# afb36c8d258364d2f190f5bd2eaee94f /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/parse_spec.py
# baf42f087ff24df6851cc43c00894cda /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/physpool_xml.py
# f97cff1dc5b04c17867dd0c4f0dcf15b /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/program_xml.py
# ca6baa8f299f113b3f93a8cb4c1f45ec /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/qiblog.py
# 46a31eb420b1999d63d43a1df42e94ca /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/qurt.py
# df9d018c2174f3df3feedde67983ef42 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/StaticSyscfgComposer.py
# 04c16efe55ce34cc29ee0c66e36f73c4 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/build_params.txt
# 0d4a45abb0f86b0671f2efeeec8a7036 /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/cust_config.c
# 073cac593207dbf9bbba902915a4b7fb /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/qurt-image-build.py
# 46c88c26362dc546ca714bb6c3cf792a /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/lib/qurt_consts.py
# 97ea2b3ba55e3cf2cf0a6f1b718298ce /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/private_ukernel.lcs
# efad49cbc2d9d2d58ac3ca72fb92ac1e /local/mnt/workspace/CRMBuilds/CDSP.VT.2.0-00249-SDM710-2_20201030_052916/b/cdsp_proc/core/kernel/qurt/build/cdsp_root/qdsp6/670.cdsp.prod/install/computev65/scripts/Input/qurt_ukernel.o
