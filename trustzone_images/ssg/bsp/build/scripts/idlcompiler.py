import os
import sys

def exists(env):
  return True
  
def generate(env):
    if sys.platform.startswith("linux"):
      platform = "linux"
      ending = ""
    elif sys.platform.startswith("win"):
      platform = "win"
      ending = ".exe"
    bin_path = os.path.dirname(os.path.realpath(__file__))
    bin_path = os.path.join(bin_path, 'bin', platform)
    
    
    
    rootenv = env.get("ROOT_GLOBAL_ENV")
    
    
    minkidl_builder = env.Builder(generator = minkidl_generator,
                                 src_suffix = '.idl',
                                 suffix = '.h',
                                 emitter = minkidl_emitter)
    
    env['MINKIDL']          = '{}/minkidl{}'.format(bin_path,ending)
    env['MINKIDLCOM']       = '$MINKIDL -o $TARGET $SOURCE'
    env['MINKIDLCOMSTR']    = 'minkidl ${TARGET.basename}.h'
    
    rootenv['MINKIDL']          = '{}/minkidl{}'.format(bin_path,ending)
    rootenv['MINKIDLCOM']       = '$MINKIDL -o $TARGET $SOURCE'
    rootenv['MINKIDLCOMSTR']    = 'minkidl ${TARGET.basename}.h'
        
    env.Append(BUILDERS={'MINKIDL' : minkidl_builder})
    rootenv.Append(BUILDERS={'MINKIDL' : minkidl_builder})
    
    env.AddMethod(run_minkidl,'RunMinkidl')
    
    

def minkidl_generator(env, target, source, for_signature):
    cmds = []
    for t in target:
      cmd = env.subst('$MINKIDL -o {} {} -I ${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/idl ').format(t,source[0])
      if str(t).endswith('_invoke.h'):
        cmd += ' --skel'
      
      cmds.append(env.Action(cmd,'minkidl {}'.format(t)))
    return cmds

def minkidl_emitter(env, target, source):
    base, ext = os.path.splitext(str(target[0]))
    incl = base + '.h'
    t = [incl]
    if os.path.basename(str(source[0])).startswith('I'):
      skeleton = base + '_invoke.h'
      t.append(skeleton)
    env.Alias('minkidl_headers',t)
    return (t,source)


def run_minkidl(env):
  for dir, subdirs, files in os.walk(env.RealPath('${BUILD_ROOT}')):
    if dir.endswith(os.path.join('ssg','securemsm','trustzone','qsee','idl')):
      # idl files that live under ssg/securemsm/trustzone/qsee/idl are a special case
      # so we skip them
      continue
    for file in files:
      if file.endswith('.idl'):
        env.MINKIDL(os.path.join(dir,file))
        
  target_dir = '${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/include'
  for idl_file in env.Glob('${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/idl/*.idl'):
    target = os.path.join(target_dir, os.path.basename(os.path.splitext(str(idl_file))[0] + '.h'))
    env.MINKIDL(target=target, source=idl_file)
            
        
