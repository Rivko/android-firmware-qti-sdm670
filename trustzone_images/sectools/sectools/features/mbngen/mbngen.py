# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================



import os
import sys
import traceback

from sectools.features.isc.defines import ALIGNED_IMAGE_SIZE
from sectools.common.parsegen.mbn.versions.v3 import MbnHdr40B
from sectools.common.parsegen.mbn.versions.v3 import MbnHdr80B
from sectools.common.parsegen.mbn.versions.v3 import MBN_HDR_VERSION_3
from sectools.common.parsegen.mbn.versions.v3.headers import MBN_80_MAGIC
from sectools.common.parsegen.mbn.versions.v3.parsegen_mbn import ParseGenMbnV3
from sectools.common.parsegen.mbn.versions.v6.headers import MbnHdrV6
from sectools.common.utils import c_path
from sectools.common.utils.c_base import CoreOptionParser
from sectools.common.utils.c_logging import logger
from sectools.features.mbngen.parsegen import auto_gen_xml_config, defines
from sectools.common.utils.c_config import CoreConfig

CMD_ARG_TOOL_NAME = 'mbngen'
MBN_GEN_TOOL_NAME = 'MBNGEN'
MBN_GEN_TOOL_VERSION = '1.0'

__version__ = MBN_GEN_TOOL_NAME + ' ' + MBN_GEN_TOOL_VERSION

MBN_80_CODEWORD = 0x844bdcd1

DEFAULT_OUTPUT_DIR = "mbngen_output"


class MbnGenOptionParser(CoreOptionParser):

    @property
    def c_description(self):
        return "mbngen validates existing files with MBN headers, generates new files with valid MBN headers, and dumps MBN headers from existing files in human readable form"

    @property
    def c_version(self):
        return __version__

    @property
    def c_epilog(self):
        return '''
Example usage:
sectools.py mbngen --validate --input_file in.mbn
sectools.py mbngen --validate --input_file in.mbn --config_file config.xml --config_tag_name tag_name
sectools.py mbngen --validate --input_file in.mbn --config_tag_name tag_name
sectools.py mbngen --validate --input_file in.mbn --image_id 27 --header_version 3 --image_src 40 --image_dest_ptr 0 --header_length 40

sectools.py mbngen --generate --input_file in.tar --config_file config.xml --config_tag_name tag_name --output_dir .
sectools.py mbngen --generate --input_file in.tar --config_tag_name tag_name --output_dir .\output
sectools.py mbngen --generate --input_file in.tar --image_id 27 --header_version 3 --image_src 40 --image_dest_ptr 0 --header_length 40 --output_dir c:\\tmp

sectools.py mbngen --dump --input_file in.mbn
'''

    def c_add_options(self):
        """Adds the command line args supported by MbnGen."""
        action_group = self.add_option_group('The different commands supported by mbngen')
        action_group.add_option('--validate', '-a',
                                action='store_true',
                                help='Verify a file to see if it has an existing MBN header. If provided with an mbn header config, it will also check to see if the given mbn file matches the given config')
        action_group.add_option('--generate', '-g',
                                action='store_true',
                                help='Generate a new file with MBN header from the given input file and mbn header configuration')
        action_group.add_option('--dump',
                                action='store_true',
                                help='Dump the mbn header of the given file in human readable form')

        config_source_group = self.add_option_group('Options to specify the MBN header values for the validator and generator')

        config_source_group.add_option('--config_file', '-c',
                               metavar='<file>',
                                help='Path to config file that specifies the different MBN header configurations. If not option is given, the default configuration provided by sectools will be used')
        config_source_group.add_option('--config_tag_name', '-t',
                                help='tag number for the config that specifies the MBN header values')

        in_out_group = self.add_option_group('Options to specify the input and output files')
        in_out_group.add_option('--output_dir', '-o',
                                metavar='<path>',
                                help='Output path for the MBN file generator. The output file will be input_file with \'.mbn\' appended')
        in_out_group.add_option('--input_file', '-i',
                                metavar='<file>',
                                help='Path to the input MBN file for validate and dump. Path to the input binary file for generate')

        config_options_group = self.add_option_group('Config options for specifying an MBN header')
        config_options_group.add_option('--image_id', type=int)
        config_options_group.add_option('--header_version', type=int)
        config_options_group.add_option('--image_src', type=int)
        config_options_group.add_option('--image_dest_ptr', type=int)
        config_options_group.add_option('--header_length', type=int)

    def validate_configs(self, args, is_validate):
        inputs = ['config_file', 'config_tag_name']
        num_inputs = 0
        for i in inputs:
            if getattr(args, i) is not None:
                num_inputs += 1

        configs = ['image_id', 'header_version', 'image_src', 'image_dest_ptr', 'header_length']
        num_configs = 0
        for config in configs:
            if getattr(args, config) is not None:
                num_configs += 1

        if num_inputs == 0 and num_configs == 0:
            # validate with no conf options simply checks if its a mbn or not
            if not is_validate:
                raise RuntimeError("Please specify a configuration source when using mbngen generate")

        if num_inputs > 0 and num_configs > 0:
            raise RuntimeError("Please do not specify mbn header configuration through both commandline arguments and configuration tag")

        if num_configs > 0 and num_configs < 5:
            raise RuntimeError("Please specify all 5 mbn header arguments when providing them via commandline")

        if args.config_file is not None and args.config_tag_name is None:
            raise RuntimeError("Please specify the config tag_name when utilizing mbngen configuration files")

    def validate_output_dir(self, args):
        if args.output_dir is None:
            raise RuntimeError("Please specify an output dir")

    def validate_input_file(self, args):
        if args.input_file is None:
            raise RuntimeError("Please specify an input file")
        if not c_path.validate_file(args.input_file):
            raise RuntimeError("input file {0} could not be opened for reading".format(args.input_file))
        logger.debug("input file {0} has been validated successfully".format(args.input_file))

    def set_config_vals(self, args):
        # args have already been verified, so no need to check if the 5 config options
        # were passed or not
        if args.config_tag_name is not None:
            conf_file_name = None
            if args.config_file is not None:
                conf_file_name = args.config_file
            else:
                conf_file_name = defines.MBNGEN_CONFIG_FILE
            config = CoreConfig(auto_gen_xml_config, conf_file_name)
            for conf in config.root.mbngen_conf.conf:
                if conf.tag_name == args.config_tag_name:
                    self.parsed_args.image_id = conf.image_id
                    self.parsed_args.header_version = conf.header_version
                    self.parsed_args.image_src = conf.image_src
                    self.parsed_args.image_dest_ptr = conf.image_dest_ptr
                    self.parsed_args.header_length = conf.header_length
                    supported_mbn_header_lengths = [MbnHdr40B().get_size(), MbnHdrV6().get_size(), MbnHdr80B().get_size()]
                    if conf.header_length not in supported_mbn_header_lengths:
                        raise RuntimeError("{0} is not a valid mbn header length".format(conf.header_length))
                    return
            raise RuntimeError("tag: {0} was not found in the mbngen configuration file".format(args.config_tag_name))

    def c_validate(self):

        args = self.parsed_args

        if args.dump is not None:
            dump_conflicts = ['validate', 'generate', 'config_file', 'config_tag_name', 'image_id', 'header_version', 'image_src', 'image_dest_ptr', 'output_dir', 'header_length']
            for conflict in dump_conflicts:
                if getattr(args, conflict) is not None:
                    raise RuntimeError("the argument {0} is not compatible with the mbngen dump".format(conflict))
            self.validate_input_file(args)

        elif args.generate is not None:
            generate_conflicts = ['dump', 'validate']
            for conflict in generate_conflicts:
                if getattr(args, conflict) is not None:
                    raise RuntimeError("the argument {0} is not compatible with the mbngen generate".format(conflict))
            self.validate_configs(args, False)
            self.validate_input_file(args)
            self.validate_output_dir(args)

        elif args.validate is not None:
            validate_conflicts = ['dump', 'generate', 'output_dir']
            for conflict in validate_conflicts:
                if getattr(args, conflict) is not None:
                    raise RuntimeError("the argument {0} is not compatible with the mbngen validate".format(conflict))
            self.validate_configs(args, True)
            self.validate_input_file(args)
        else:
            raise RuntimeError("Please specify an action for mbngen to perform")

        # slightly hacky, in that validate should really only validate, but I don't want MbnGen to have to validate
        # options that it grabs from config files.
        self.set_config_vals(args)
        logger.debug("mbngen input parameters have been successfully verified")

class MbnGen():

    def __init__(self):
        pass

    def dump(self, args):
        file = open(args.input_file, "rb")
        file_data = file.read()
        file.close()

        size = self.get_header_size(file_data, args.input_file)
        if size is None:
            raise RuntimeError("{0} does not appear to have a valid MBN header".format(args.input_file))
        else:
            mbn_handler = ParseGenMbnV3(
                data=file_data, header_size=size, version=MBN_HDR_VERSION_3)
            logger.info(mbn_handler)
        logger.debug("{0} has been successfully dumped".format(args.input_file))

    def validate(self, args):
        file = open(args.input_file, "rb")
        file_data = file.read()
        file.close()

        # if validate is given no arguments, then we just need to check if the file has an mbn header. that's it
        if self.is_mbn_file(file_data, args.input_file) and args.header_length is None:
            logger.info("\"{0}\" appears to have a valid MBN header".format(args.input_file))
            return

        mbn_handler = ParseGenMbnV3(
            data=file_data, header_size=args.header_length, version=MBN_HDR_VERSION_3)

        things_to_check = ["image_id", "image_dest_ptr", "image_src"]
        errors_found = 0

        for thing in things_to_check:
            if getattr(args, thing) != getattr(mbn_handler.header, thing):
                errors_found += 1
                logger.warning("{0} value: {1} provided by configuration does not match {0} value: "
                               "{2} from given mbn file".format(
                    thing, getattr(args, thing), getattr(mbn_handler.header, thing)))

        if args.header_length == 40:
            if args.header_version != mbn_handler.header.version:
                errors_found += 1
                logger.warning("header_version value: {0} provided by configuration does not match "
                               "header_version value: {1} from given mbn file".format(
                    args.header_version, mbn_handler.header.version))
        logger.info("MBN file {0} has {1}passed validation".format(args.input_file, "not " if errors_found > 0 else ""))

    def generate(self, args):
        infile = open(args.input_file, "rb")
        infile_data = infile.read()
        infile.close()

        if self.is_mbn_file(infile_data, args.input_file) is not None:
            logger.warning("WARNING! {0} appears to already have an MBN header!".format(args.input_file))

        # sectools will update these values when it inputs the cert chain and sig
        # but in this feature just set them to zero
        code_size = ALIGNED_IMAGE_SIZE(os.path.getsize(args.input_file))
        signature_size = 0
        signature_ptr = 0
        cert_chain_ptr = 0
        cert_chain_size = 0
        image_size = code_size + signature_size + cert_chain_size

        if args.header_version != MBN_HDR_VERSION_3:
            raise RuntimeError('Supported header version to generate is: ' + format(MBN_HDR_VERSION_3))

        header = None
        if args.header_length == 40:
            header = MbnHdr40B()
            header._unpack_data_list([args.image_id,
                                      args.header_version,
                                      args.image_src,
                                      args.image_dest_ptr,
                                      image_size,
                                      code_size,
                                      signature_ptr,
                                      signature_size,
                                      cert_chain_ptr,
                                      cert_chain_size])


        elif args.header_length == 80:
            header = MbnHdr80B()
            # todo: verify magic and codeword are correct
            header._unpack_data_list([MBN_80_CODEWORD,  # codeword I pulled from given file
                                      MBN_80_MAGIC,  # magic I pulled from given file
                                      args.image_id,
                                      0xffffffff,
                                      0xffffffff,
                                      args.image_src,
                                      args.image_dest_ptr,
                                      image_size,
                                      code_size,
                                      signature_ptr,
                                      signature_size,
                                      cert_chain_ptr,
                                      cert_chain_size,
                                      0,  # this has a value of 1 in a file I was given. what does it mean?
                                      0, 0, 0, 0, 0, 0])
        else:
            raise RuntimeError("received invalid MBN header length in generate. This should never happen")


        outfile_name = os.path.basename(args.input_file) + ".mbn"
        if not c_path.validate_dir_write(args.output_dir):
            os.makedirs(args.output_dir)

        outfile = open(c_path.join(args.output_dir, outfile_name), "wb")
        outfile.write(header.pack())
        outfile.write(infile_data)
        outfile.close()
        infile.close()
        logger.info("output \"{0}\" has been successfully generated".format(c_path.join(args.output_dir, outfile_name)))

    def is_mbn_file(self, file_data, file_name):
        return self.get_header_size(file_data, file_name)

    def get_header_size(self, file_data, file_name):
        try:
            return ParseGenMbnV3(data=file_data, header_size=40, version=MBN_HDR_VERSION_3).header_size
        except RuntimeError as e:
            logger.debug("{0} doesn't appear to have a valid 40 or 48 byte mbn header.\n".format(file_name))
            logger.debug("The following is the output from trying to open {0}".format(file_name))
            logger.debug(str(e))
            try:
                return ParseGenMbnV3(data=file_data, header_size=80, version=MBN_HDR_VERSION_3).header_size
            except:
                logger.debug("{0} doesn't appear to have a valid 80 byte mbn header.\n".format(file_name))
                logger.debug("The following is the output from trying to open {0}".format(file_name))
                logger.debug(str(e))
                return None

def main(args):
    """Parses the command line arguments, performs any basic operations based on
    the parsed arguments and starts processing using the mbn gen module.
    """
    logger.debug('\n\n    MbnGen launched as: "' + ' '.join(sys.argv) + '"\n')
    mbngen = MbnGen()
    if args.dump is not None:
        mbngen.dump(args)
    elif args.validate is not None:
        mbngen.validate(args)
    elif args.generate is not None:
        mbngen.generate(args)
    else:
        raise RuntimeError("No operation received to mbngen main. This should never happen")

def parse_args(argv):
    return MbnGenOptionParser(argv).parsed_args

if __name__ == '__main__':
    try:
        main(parse_args(sys.argv))
    except Exception:
        logger.debug(traceback.format_exc())
        logger.error(sys.exc_info()[1])
        sys.exit(1)

    except KeyboardInterrupt:
        print
        logger.error('Keyboard Interrupt Received. Exiting!')
        sys.exit(1)

