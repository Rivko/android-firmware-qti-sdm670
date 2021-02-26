#=============================================================================
#
#                                    qurt_run_db.py
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
#
#  Python script for LLDB debugging
#
#  Can be invoked directly from the command line as follows:
#
#    python PATH/TO/qurt-lldb.py [args...]
#
#  Arguments include:
#
#    --program=XXX               XXX is the "boot" image in ELF format
#    --symbols=XXX               XXX is an ELF file whose symbols should be imported
#    --tools=XXX                 XXX is the base directory for the Hexagon tools to be used
#    --rtos=XXX                  XXX is the QuRT model file to be used
#    --cosim=XXX                 XXX is a cosim description (first word is path to a cosim, then args)
#    --cosimcfg=XXX              XXX is a file containing one cosim description per line
#    --dir=XXX                   XXX is the directory where temporary files should be created
#    --chdir=XXX                 XXX is the directory to change to before doing anything
#

tcp_port = 0

def get_free_tcp_port():
   ''' Read the output of netstat -a -n to find a free TCP port '''
   global tcp_port
   if tcp_port == 0:
      portdict = dict()
      try:
         netstat = subprocess.Popen(['/bin/netstat', '-a', '-n'], stdout=subprocess.PIPE)
         for line in netstat.stdout:
            z=line.split()
            if len(z) >= 4:
               if z[0] == 'tcp':
                  v=z[3].split(':')
                  portdict[v[len(v)-1]] = True
         netstat.stdout.close()
         netstat.wait()
      except Exception:
         pass
      tcp_port = 17000
      while str(tcp_port) in portdict:
         tcp_port += 1
   return tcp_port

class QurtRunDb:
    def create_osam_file(self):
        if not self.options.rtos:
            self.osam_filename = None
            return
        import atexit, os, tempfile
        (fd, self.osam_filename) = tempfile.mkstemp(suffix='.cfg', dir=self.options.dir, text=True)
        os.write(fd, self.options.rtos+'\n')
        os.close(fd)
        atexit.register(os.unlink, self.osam_filename)
    def create_cosim_file(self):
        cosims = self.options.cosim
        for fname in self.options.cosimcfg:
            f = open(fname, 'r')
            for line in f:
                s = line.strip()
                if s:
                    cosims.append(s)
            f.close()
        if not cosims:
            self.cosim_filename = None
            return
        import atexit, os, tempfile
        (fd, self.cosim_filename) = tempfile.mkstemp(suffix='.cfg', dir=self.options.dir, text=True)
        for s in cosims:
            os.write(fd, s+'\n')
        os.close(fd)
        atexit.register(os.unlink, self.cosim_filename)
    def create_gdb_cmd_file(self, dbgport):
        import atexit, os, tempfile
        (fd, self.gdb_cmd_filename) = tempfile.mkstemp(suffix='.ini', dir=self.options.dir, text=True)
        for name in self.options.symbols:
            os.write(fd, 'sym %s\n' % name)
        os.write(fd, 'set remotetimeout 900\n')
        os.write(fd, 'set tcp connect-timeout 900\n')
        os.write(fd, 'target extended-remote :%s\n' % dbgport)
        os.write(fd, 'set targetargs')
        if self.osam_filename:
            os.write(fd, ' --rtos %s' % self.osam_filename)
        if self.options.symbols:
            os.write(fd, ' --symfile %s' % self.options.symbols[0])
        if self.cosim_filename:
            os.write(fd, ' --cosim_file %s' % self.cosim_filename)
        os.write(fd, '\nbreak main\n')
        os.close(fd)
        atexit.register(os.unlink, self.gdb_cmd_filename)
    def run_gdb(self, gdb_path, sim_path):
        import subprocess
        dbgport = get_free_tcp_port()
        self.create_osam_file()
        self.create_cosim_file()
        self.create_gdb_cmd_file(dbgport)
        cmd = [sim_path, '-G', '%s' % dbgport]
        if self.osam_filename:
            cmd.extend(['--rtos', self.osam_filename])
        if self.options.symbols:
            cmd.extend(['--symfile', self.options.symbols[0]])
        if self.cosim_filename:
            cmd.extend(['--cosim_file', self.cosim_filename])
        cmd.extend([self.options.program])
        print ' '.join(cmd)
        simproc = subprocess.Popen(cmd)
        cmd = [gdb_path, self.options.program, '-x', self.gdb_cmd_filename]
        print ' '.join(cmd)
        ret = subprocess.call(cmd)
        simproc.wait()
        return ret
    def run_lldb(self, lldb_path, sim_path):
        import os, subprocess
        self.create_osam_file()
        self.create_cosim_file()
        os.environ['QURT_SIM_OSAM_FILE'] = '%s' % self.osam_filename
        os.environ['QURT_SIM_COSIM_FILE'] = '%s' % self.cosim_filename
        os.environ['QURT_SIM_SYMBOL_FILES'] = ' '.join(self.options.symbols)
        os.environ['QURT_SIM_TOOLS_DIR'] = '%s' % self.options.tools
        os.environ['PYTHONHOME'] = os.path.dirname(os.path.dirname(lldb_path))
        cmd = [lldb_path+'-3.5.0', '-o', 'command script import %s' % self.args[0], self.options.program,
               '--',
               '--rtos', str(self.osam_filename),
               '--symfile', ' '.join(self.options.symbols),
               '--cosim_file', str(self.cosim_filename)]
        print ' '.join(cmd)
        return subprocess.call(cmd)
    def run_debugger(self):
        import os
        gdb_path = os.path.join(self.options.tools, 'gnu', 'bin', 'hexagon-gdb')
        sim_path = os.path.join(self.options.tools, 'qc', 'bin', 'hexagon-sim')
        if os.path.exists(gdb_path):
            return self.run_gdb(gdb_path, sim_path)
        lldb_path = os.path.join(self.options.tools, 'Tools', 'bin', 'hexagon-lldb')
        sim_path = os.path.join(self.options.tools, 'Tools', 'bin', 'hexagon-sim')
        if os.path.exists(lldb_path):
            return self.run_lldb(lldb_path, sim_path)
        raise Exception('Cannot find a debugger')
    def main(self, args):
        import os, optparse
        parser = optparse.OptionParser()
        parser.add_option('--program')
        parser.add_option('--symbols', action='append', default=[])
        parser.add_option('--tools')
        parser.add_option('--rtos')
        parser.add_option('--cosim', action='append', default=[])
        parser.add_option('--cosimcfg', action='append', default=[])
        parser.add_option('--dir')
        parser.add_option('--chdir', default='.')
        (self.options, self.args) = parser.parse_args(args)
        os.chdir(self.options.chdir)
        self.run_debugger()

if __name__ == '__main__':
    # Being executed from the command line
    # Do the right thing
    import sys
    QurtRunDb().main(sys.argv)
else:
    from os import environ as __E
    try:
        __T = __E['QURT_SIM_TOOLS_DIR']+'/Tools/bin/'
        __F = open(__T+'hexagon_utils.py', 'r')
        __S = __F.read()
        __F.close()
        __E['HEXAGON_LLDB_HOME'] = __T
    except IOError:
        pass
    try:
        __T = __E['QURT_SIM_TOOLS_DIR']+'/Tools/lib/python2.7/'
        __F = open(__T+'hexagon_utils.py', 'r')
        __S = __F.read()
        __F.close()
        __E['HEXAGON_LLDB_HOME'] = __T
    except IOError:
        pass
    __NEWARGS = ['--rtos',       __E['QURT_SIM_OSAM_FILE'],
                 '--symfile',    __E['QURT_SIM_SYMBOL_FILES'],
                 '--cosim_file', __E['QURT_SIM_COSIM_FILE'],
                 '-G']
    __NEWARGS = ','.join(['"%s"' % __N for __N in __NEWARGS])
    __S = __S.replace('hexagon_utils','qurt_run_db')     # Point hexagon_utils references to our own module
    __S = __S.replace('"-G"', __NEWARGS)                 # Replace -G with __NEWARGS in hexagon-sim invocation
    exec(__S)
    del __T
    del __S
    del __F
    del __E
    __CAPTURE = __lldb_init_module
    def __lldb_init_module(debugger,dict):
        import os
        __CAPTURE(debugger,dict)
        s = 'target module add '+os.environ['QURT_SIM_SYMBOL_FILES']
        debugger.HandleCommand(s)
        s = 'target symbols add '+os.environ['QURT_SIM_SYMBOL_FILES']
        debugger.HandleCommand(s)
        s = 'breakpoint set -a QURTOS_init'
        debugger.HandleCommand(s)
