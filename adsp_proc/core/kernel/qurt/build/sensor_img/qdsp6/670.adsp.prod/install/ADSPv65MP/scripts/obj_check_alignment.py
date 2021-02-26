from lib.qurt import *

class ObjCheckAlignment:
    def __init__(self):
        self.result_file = None
        self.stats_file = None
        self.out_file = None
        self.result = 'PASS'
        self.stats = ''
        self.out = ''
    def scan(self, fname):
        for libname, objname, fi in QurtElfContainer(fname):
            fi.seek(0)
            ehdr = Elf32Header(fi)
            fi.seek(ehdr.e_shoff)
            shdr = [Elf32SectionHeader(fi, loc=loc, f=fi) for loc in range(ehdr.e_shnum)]
            strtab_section = shdr[ehdr.e_shstrndx]
            strtab = strtab_section.contents().read()
            for s in shdr:
                s.name = strtab[s.sh_name:].split('\0')[0]
            for s in shdr:
                if s.sh_flags & s.SHF_ALLOC:
                    if s.sh_size > 1:
                        if s.sh_addralign == 1:
                            #
                            # It is a section with size > 1 and with alignment == 1
                            #
                            # This might be suspicious.
                            #
                            if (s.sh_flags & s.SHF_MERGE) and (s.sh_flags & s.SHF_STRINGS):
                                #
                                #  It's a mergeable section of strings.  These are okay.
                                #
                                continue
                            if libname:
                                msg = '%s:%s, section %s\n' % (libname, objname, s.name)
                            else:
                                msg = '%s, section %s\n' % (objname, s.name)
                            self.out += msg
                            if s.name.startswith('.rodata.'):
                                #
                                #  If it's .rodata.xxx, it's probably okay.  Let it be for now.
                                #
                                continue
                            #
                            # It's suspicious, and it's not .rodata.xxx.  Flag it to get fixed.
                            #
                            self.out += ' ^^^^ FIX THIS ^^^^\n'
                            self.result = 'FAIL'
            fi.close()
            if libname:
                self.stats += 'Checked %s:%s\n' % (libname, objname)
            else:
                self.stats += 'Checked %s\n' % (objname,)
    def main(self, argv):
        while argv[1:]:
            arg = argv.pop(1)
            if arg == '-r':
                self.result_file = argv.pop(1)
            elif arg == '-s':
                self.stats_file = argv.pop(1)
            elif arg == '-o':
                self.out_file = argv.pop(1)
            else:
                self.scan(arg)
        if self.result_file:
            f = open(self.result_file, 'w')
            f.write(self.result)
            f.close()
        if self.stats_file:
            f = open(self.stats_file, 'w')
            f.write(self.stats)
            f.close()
        if self.out_file:
            f = open(self.out_file, 'w')
            f.write(self.out)
            f.close()
        if self.result != 'PASS':
            return 1
        return 0

def get_objects(d):
    #
    #  Return the list of all .a and .o files that are under the given directory
    #
    import os
    ret = []
    for root, dirs, files in os.walk(d):
        for fname in files:
            if fname.endswith('.o') or fname.endswith('.o_orig') or fname.endswith('.a'):
                ret.append(os.path.join(root,fname))
    return ret

def CheckAction(target, source, env):
    return ObjCheckAlignment().main(['CheckAction',
                                     '-r', str(target[0]),
                                     '-s', str(target[1]),
                                     '-o', str(target[2])] + [str(s) for s in source])

def CheckEmitter(target, source, env):
    return target, source

def Checker(env, *ls):
    return env.ObjCheckBuilder(['$BUILD_DIR/result',
                                '$BUILD_DIR/stats.txt',
                                '$BUILD_DIR/out'], env.Flatten([get_objects(str(d)) for d in ls]))

def exists(env):
    return env.Detect('obj_check_alignment')

def generate(env):
    env.AddMethod(Checker, 'ObjCheckAlignment')
    env['BUILDERS']['ObjCheckBuilder'] = env.Builder(action = env.Action(CheckAction, cmdstr = 'Checking object alignment'),
                                                     emitter = CheckEmitter)

if __name__ == '__main__':
    from lib.qurt import run_script
    run_script(ObjCheckAlignment().main)


# Signatures of the files that this depends on
# 46a31eb420b1999d63d43a1df42e94ca /local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/core/kernel/qurt/build/sensor_img/qdsp6/670.adsp.prod/install/ADSPv65MP/scripts/lib/qurt.py
