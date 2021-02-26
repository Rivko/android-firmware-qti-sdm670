"""Collection of general-purpose tools/methods for use in SSG SConscripts"""

import hashlib
import SCons

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('ssg_environment')


def generate(env):
    env.AddMethod(feature_flag, "DeclareFeature")
    env.AddMethod(get_scons_abspath, "SConsAbsPath")
    env.AddMethod(get_app_id_by_image_name, "GetAppIDByImageName")
    env.AddMethod(offtarget_testing, "OfftargetTesting")
    env.AddMethod(is_64_bit_image, "Is64BitImage")

    # now use DeclareFeature to declare this script itself
    env.DeclareFeature("SSG_BUILD_ENVIRONMENT", uses_prefix=True)


def feature_flag(env, flag, uses_prefix=False):
    """Adds a feature-definition flag to the environment in a way that is
       compatible with both QC SCons (USES_FLAGS) & Open Source SCons. Either
       implementation can be checked in SConscripts with 'if <flag> in env'.
       If 'uses_prefix' is True, also declares a USES_<flag> version of the
       flag, even if QC SCons is not available (useful for tools/features loaded
       globally before QC SCons)."""
    try:
        env.AddUsesFlags(flag)
    except AttributeError:
        env[flag] = True

    # most CoreBSP scripts expect that uses flags will always begin with
    # USES_* for consistency. If this feature flag does not already do that,
    # define an additional flag for their benefit.
    if flag.startswith("USES_"):
        return

    try:
        env.AddUsesFlags("USES_" + flag)
    except AttributeError:
        if uses_prefix:
            env["USES_" + flag] = True


def get_scons_abspath(env, item):
    """Returns the absolute path of the given string or SCons Node"""
    ret = None
    if type(item) == str:
      ret = env.File(item).get_abspath()
    elif isinstance(item, SCons.Node.Node):
      ret = item.get_abspath()
    elif isinstance(item, SCons.Node.Alias.Alias):
      ret = item.get_abspath()

    assert ret != None, "Unexpected item type '{}'".format(str(type(item)))
    return ret

def get_app_id_by_image_name(env, image_name):
    """Return app id by calculating hash value with image name

       On MBNv6, app id has 4 byte length (8 hex characters) while previous 
       version supported 8 byte, 16 hex characters. So, we limit it as 4 byte
       with exception (sdm670/sdm845) due to potential regression
       """

    if env["CHIPSET"] in ['sdm670', 'sdm845']:
        app_id_hexstr_len = 16
    else:
        app_id_hexstr_len = 8

    return '0x' + hashlib.sha256(image_name).hexdigest()[-app_id_hexstr_len:]

def offtarget_testing(env):
    return env.get('OFF_TARGET')
    
def is_64_bit_image(env):
    return env['PROC'] in ['A53_64', 'x86-64']