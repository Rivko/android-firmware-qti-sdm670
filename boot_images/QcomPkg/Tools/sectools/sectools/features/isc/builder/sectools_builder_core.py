# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""

@author: coryf

"""

import copy
import os
import re
import subprocess

import sectools.common.builder.auto_gen_policy_parser as policy_parser
from __secfile__ import DIR_PATH
from sectools.common.utils import c_path
from sectools.common.utils.c_config import CoreConfig
from sectools.common.utils.c_data import remove_duplicates
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import is_python_27
from sectools.features.isc import SecImageCore
from sectools.features.isc.cfgparser import ConfigParser

SECTOOLS_DIR = c_path.join(DIR_PATH + '/../../../..')

EWM_TYPE = "elf_wrapped_mbn"

# Indices into SCons target output list
INTEGRITY = 0
SIGN = 1
ENCRYPT = 2
SIGN_AND_ENCRYPT = 3
INTEGRITY_SIGN_ATTR = 4

target_name_map = {
    INTEGRITY: "integrity",
    SIGN: "signed",
    ENCRYPT: "encrypted",
    SIGN_AND_ENCRYPT: "signed and encrypted",
    INTEGRITY_SIGN_ATTR: "integrity with signing attributes",
    None: None
}

target_map = {
    "-ta": INTEGRITY,
    "-sa": SIGN,
    "-ea": ENCRYPT,
    "-sea": SIGN_AND_ENCRYPT,
    "-tra": INTEGRITY_SIGN_ATTR
}


def validate_dir(path, var_name, error_list):
    if path:
        path = c_path.normalize(path)
        try:
            c_path.create_dir(path)
        except RuntimeError:
            error_list.append("Provided {0} \"{1}\" is not a valid path or does not have read/write access.".format(var_name, path))


def validate_file(path, var_name, error_list):
    if path:
        path = c_path.normalize(path)
        if not c_path.validate_file(path):
            error_list.append("Provided {0} \"{1}\" is not a valid file or does not have read access.".format(var_name, path))


def get_scons_targets(input):
    # initialize target lists
    integrity_targets = list()
    sign_targets = list()
    encrypt_targets = list()
    sign_and_encrypt_targets = list()
    scons_targets = [integrity_targets, sign_targets, encrypt_targets, sign_and_encrypt_targets]
    pilsplit_files_to_clean = list()
    # get chipset and filename of input file
    chipset = ConfigParser(input.config).root.metadata.get_chipset()
    path, filename = os.path.split(input.source)
    if input.build_policy:
        for policy in input.build_policy.sec_image_policies:
            target_list_to_append_to = scons_targets[target_map[policy.cmd_options]]
            # get path to uninstalled image
            uninstalled_target = c_path.normalize(c_path.join(input.target_base_dir, policy.id, chipset, input.sign_id, filename))
            target_list_to_append_to.append(uninstalled_target)
            logger.debug("Added file \"{0}\" to SCons target list".format(uninstalled_target))
            # get path to installed image
            if input.sectools_install_base_dir:
                for install_location in policy.install_locations:
                    if input.install_file_name:
                        target = c_path.join(install_location, input.install_file_name)
                    else:
                        target = c_path.join(install_location, filename)
                    logger.debug("Added to-be-installed file \"{0}\" to SCons target list".format(target))
                    target_list_to_append_to.append(target)
                    # determine pilsplit images to cleanup
                    if input.pilsplitter_target_base_dir:
                        if install_location != input.sectools_install_base_dir:
                            pilsplit_directory = c_path.join(input.pilsplitter_target_base_dir, install_location.replace(os.path.join(input.sectools_install_base_dir, "", ""), ""))
                        else:
                            pilsplit_directory = input.pilsplitter_target_base_dir
                        if c_path.validate_dir_write(pilsplit_directory):
                            pilsplit_filename = input.install_file_name.split(".")[0] if input.install_file_name else filename.split(".")[0]
                            # get all pil files in pilsplit target directory
                            regex = r"^" + re.escape(pilsplit_filename) + r"\.((mdt)|(b[0-9][0-9]))$"
                            pil_files = [c_path.join(pilsplit_directory, f) for f in os.listdir(pilsplit_directory) if re.match(regex, f)]
                            for pil_file in pil_files:
                                logger.debug("Added pilsplit file \"{0}\" to SCons clean list".format(pil_file))
                                pilsplit_files_to_clean.append(pil_file)
    # remove duplicates from lists
    for i, targets in enumerate(scons_targets):
        scons_targets[i] = remove_duplicates(targets)
    pilsplit_files_to_clean = remove_duplicates(pilsplit_files_to_clean)
    # clean pilsplit files
    input.environment.Clean(scons_targets, pilsplit_files_to_clean)
    # return target list
    return scons_targets


class SectoolsBuilderCore(object):

    def __init__(self, input_package):
        logger.debug("Constructing SectoolsBuilderCore")
        logger.debug("\n\nInput values received by Sectools Builder: \n" + str(input_package) + "\n")
        self.input = input_package

    def build(self):
        logger.debug("Executing SectoolsBuilderCore.build()")
        # construct build policy object based on selected build_policy_id
        if self.input.build_policy is None and self.input.build_policy_id is None:
            logger.error("A build policy was not provided. Sectools execution, installation, and pilsplitting will be skipped.")
        elif self.input.build_policy is None:
            logger.debug("Constructing BuildPolicy based on selected build_policy_id...")
            self.input.build_policy = BuildPolicy(self.input.policy_file, self.input.build_policy_id, self.input.sign_id, self.input.sectools_install_base_dir, self.input.additional_secimage_policy_ids)
            logger.debug("\n\nConstructed BuildPolicy:\n" + str(self.input.build_policy) + "\n")
        # either call sectools and proceed with installation and pilsplitting or return SCons callback to build method
        if self.input.execute_sectools:
            # update config file with relocatable and image_entry values
            self._generate_config_file()
            # validate files and paths
            errors = []
            validate_file(self.input.source, "source", errors)
            validate_dir(self.input.target_base_dir, "target_base_dir", errors)
            validate_dir(self.input.sectools_install_base_dir, "sectools_install_base_dir", errors)
            validate_dir(self.input.pilsplitter_target_base_dir, "pilsplitter_target_base_dir", errors)
            if errors:
                err_string = "\n".join(errors)
                logger.error(err_string)
                raise RuntimeError(err_string)
            # drive sign, install, and pilsplit sequence
            self._execute_sign()
        else:
            # return SCons callback to build method
            return self._scons_callback()

    def _generate_config_file(self):
        if self.input.sign_id.lower().endswith("ewm"):
            logger.debug("Generating new config files with provided values for relocatable and image_entry...")
            new_config_file = c_path.join(self.input.target_base_dir, "generated_config.xml")
            config = ConfigParser(self.input.config)

            image_type_list = config.root.parsegen.get_image_types_list()
            for image_type in image_type_list.get_image_type():
                if image_type.id == EWM_TYPE:
                    if self.input.relocatable is not None:
                        # Override relocatable setting
                        image_type.ewm_properties.relocatable = self.input.relocatable
                        logger.debug("Overrode config's relocatable value with {0}".format(self.input.relocatable))
                    if self.input.image_entry is not None:
                        # Override image_entry
                        image_type.ewm_properties.image_entry = self.input.image_entry
                        logger.debug("Overrode config's image_entry value with {0}".format(self.input.image_entry))

            config.generate(new_config_file)
            self.input.config = new_config_file
            logger.debug("New config file generated at \"{0}\"".format(new_config_file))

    def _execute_sign(self):
        # don't execute sectools, install, or pilsplit if no policy is provided
        if self.input.build_policy is None:
            return

        # create list containing command arguments to execute Sectools
        cmds = ["python", c_path.join(SECTOOLS_DIR, "sectools.py"), "secimage",
                "-i", self.input.source, "-g", self.input.sign_id, "-c", self.input.config]
        if self.input.app_id:
            cmds += ["--cfg_app_id", self.input.app_id]
        if self.input.jtag_id:
            cmds += ["--cfg_msm_part", self.input.jtag_id]
        if self.input.soc_hw_version:
            cmds += ["--cfg_soc_hw_version", self.input.soc_hw_version]
            cmds += ["--cfg_in_use_soc_hw_version", "1"]
            if self.input.soc_vers:
                cmds += ["--cfg_soc_vers", self.input.soc_vers]
        if self.input.signer:
            cmds += ["--cfg_selected_signer", self.input.signer]
        if self.input.qti_sign:
            cmds += ["--qti_signing"]
        if self.input.uie_qti:
            cmds += ["--cfg_UIE_capability", self.input.uie_qti]
            cmds += ["--cfg_UIE_key", "UIE_QTI"]
        if self.input.hash_table_algo:
            cmds += ["--cfg_segment_hash_algorithm", self.input.hash_table_algo]
        if self.input.signature_digest_algo:
            cmds += ["--cfg_hash_algorithm", self.input.signature_digest_algo]
        if self.input.num_root_certs:
            cmds += ["--cfg_num_root_certs", self.input.num_root_certs]
        if self.input.max_num_root_certs:
            cmds += ["--cfg_max_num_root_certs", self.input.max_num_root_certs]
        if self.input.selected_cert_config:
            cmds += ["--cfg_selected_cert_config", self.input.selected_cert_config]
        if self.input.client_id:
            cmds += ["--cfg_client_id", self.input.client_id]
        if self.input.lib_id:
            cmds += ["--cfg_lib_id", self.input.lib_id]
        if self.input.multi_serial_numbers:
            cmds += ["--cfg_multi_serial_numbers", self.input.multi_serial_numbers]

        # maintain verbosity level across sectools
        if logger.verbosity == logger.WARN:
            cmds += ["-z"]
        elif logger.verbosity == logger.ERROR:
            cmds += ["-zz"]
        elif logger.verbosity == logger.DEBUG:
            cmds += ["-v"]

        # execute Sectools for each sec_image_policy
        for policy in self.input.build_policy.sec_image_policies:
            _cmds = copy.deepcopy(cmds)
            _cmds += [policy.cmd_options]
            # set output directory based on policy
            target_base_dir = c_path.join(self.input.target_base_dir, policy.id)
            _cmds += ["-o", target_base_dir]
            try:
                logger.debug("\nExecuting Sectools...")

                # output Sectools's output as it is being generated
                process = subprocess.Popen(_cmds, stdout=subprocess.PIPE)
                for line in iter(process.stdout.readline, ''):
                    logger.info(line.rstrip())
            except Exception, e:
                logger.error("Sectools failed!")
                if is_python_27():
                    logger.debug("Python version 2.7 is supported.")
                else:
                    logger.debug("Python version 2.7 is not supported.")
                logger.error(e.message)
                raise RuntimeError(e.message)

            # install and pilsplit sectools's output file
            self._execute_install(policy)

    def _execute_install(self, policy):
        logger.debug("\nInstalling Sectools's output file...")
        if self.input.sectools_install_base_dir:
            path, filename = os.path.split(self.input.source)
            chipset = ConfigParser(self.input.config).root.metadata.get_chipset()
            src = c_path.normalize(c_path.join(self.input.target_base_dir, policy.id, chipset, self.input.sign_id, filename))
            for install_location in policy.install_locations:
                if self.input.install_file_name:
                    dest = c_path.join(install_location, self.input.install_file_name)
                else:
                    dest = c_path.join(install_location, filename)

                # Attempt installation 3 times before failing
                installation_successful = False
                for i in range(3):
                    try:
                        c_path.create_dir(install_location)
                        copy_successful, error = c_path.copyFile(src, dest, None, True)
                        if not copy_successful:
                            continue
                    except:
                        # Installation failed so retry installation
                        continue
                    installation_successful = True
                    logger.info("Installed \"{0}\" to \"{1}\"".format(src, dest))
                    break
                if not installation_successful:
                    error_message = "Failed to install \"{0}\" to \"{1}\"".format(src, dest)
                    logger.error(error_message)
                    raise RuntimeError(error_message)

                # pilsplit sectools's output file
                pilsplit_subdirectory = ""
                if install_location != self.input.sectools_install_base_dir:
                    pilsplit_subdirectory = install_location.replace(os.path.join(self.input.sectools_install_base_dir, "", ""), "")
                pilsplit_prefix = self.input.install_file_name.split(".")[0] if self.input.install_file_name else filename.split(".")[0]
                self._execute_pilsplit(dest, pilsplit_prefix, pilsplit_subdirectory)
        else:
            logger.info("Installation was skipped because a value for sectools_install_base_dir was not provided")
            if self.input.pilsplitter_target_base_dir:
                logger.info("Pilsplitting was skipped because a value for sectools_install_base_dir was not provided")

    def _execute_pilsplit(self, file_to_pilsplit, pilsplit_prefix, subdirectory):
        logger.debug("\nPilsplitting Sectools's output file...")
        if self.input.pilsplitter_target_base_dir:
            # Attempt pilsplit 3 times before failing
            pilsplit_successful = False
            for i in range(3):
                try:
                    if subdirectory:
                        subdir = c_path.join(self.input.pilsplitter_target_base_dir, subdirectory)
                        c_path.create_dir(subdir)
                        prefix = c_path.join(subdir, pilsplit_prefix)
                    else:
                        subdir = self.input.pilsplitter_target_base_dir
                        prefix = c_path.join(self.input.pilsplitter_target_base_dir, pilsplit_prefix)

                    SecImageCore.pil_split(file_to_pilsplit, prefix)
                except:
                    # Pilsplitting failed so retry pilsplit
                    continue
                pilsplit_successful = True
                logger.info("Pilsplitted \"{0}\" into directory \"{1}\"".format(file_to_pilsplit, subdir))
                break
            if not pilsplit_successful:
                error_message = "Failed to pilsplit \"{0}\"".format(file_to_pilsplit)
                logger.error(error_message)
                raise RuntimeError(error_message)
        else:
            logger.info("Pilsplit was skipped because a value for pilsplitter_target_base_dir was not provided")

    def _scons_callback(self):
        logger.debug("Creating SCons callback...")
        scons_targets = get_scons_targets(self.input)

        if self.input.target_image_type_filter is not None:
            logger.debug("Filtering SCons target list to only include {0} images...".format(target_name_map[self.input.target_image_type_filter]))
            logger.debug("SCons target list before filtering: {0}".format(scons_targets))
            scons_targets = scons_targets[self.input.target_image_type_filter]
            logger.debug("SCons target list after filtering: {0}".format(scons_targets))

        return self.input.environment.builder_function_handler(scons_targets, self.input.source, INPUT=self.input)


class BuildPolicy(object):

    def __init__(self, policy_file, build_policy_id, sign_id, install_base_dir, additional_secimage_policy_ids):
        root = CoreConfig(policy_parser, policy_file).root
        self.install_base_dir = install_base_dir

        # get all the supported build policies
        supported_build_policies = root.build_policy.policy_enable

        # loop through supported build policies and check that build_policy_id corresponds to one of the supported build policies
        self.policy = None
        supported_build_policy_ids = []

        # get build policy based on provided build_policy_id
        for build_policy in supported_build_policies:
            supported_build_policy_ids.append(build_policy.id)
            if build_policy.id == build_policy_id or build_policy.feature_flag.lower() == build_policy_id or "sec_policy_" + build_policy.id == build_policy_id:
                # found build policy matching specified policy
                self.policy = build_policy
        if self.policy is None:
            # raise error because the provided build_policy_id is not one of the ids specified in the sectools_policy.xml file
            error_message = "Provided build_policy_id \"{0}\" does not match one of the supported build policy ids: {1}".format(build_policy_id, ", ".join(supported_build_policy_ids))
            logger.error(error_message)
            raise RuntimeError(error_message)

        # get the sec image policies
        logger.debug("Getting secimage policies for \"{0}\" build policy...".format(build_policy_id))
        self.sec_image_policies, self.sec_image_policy_ids = BuildPolicy._get_policies(root.sec_image_policy.policy, self.policy.sec_image_policy)
        logger.debug("Secimage policies retrieved for \"{0}\" build policy: {1}".format(build_policy_id, ", ".join(self.sec_image_policy_ids)))

        # get the sec image policies specified by additional_secimage_policy_ids
        if additional_secimage_policy_ids:
            logger.debug("Adding additional secimage policies specified in the environment to the build policy...")
            additional_secimage_policies, additional_secimage_policy_ids = BuildPolicy._get_policies(root.sec_image_policy.policy, additional_secimage_policy_ids, self.sec_image_policy_ids)
            # add the additional sec image policies and their ids to the existing sec image lists
            if additional_secimage_policy_ids:
                logger.debug("Additional secimage policies added to \"{0}\" build policy: {1}".format(build_policy_id, ", ".join(additional_secimage_policy_ids)))
                self.sec_image_policies += additional_secimage_policies
                self.sec_image_policy_ids += additional_secimage_policy_ids

        # filter out sec image policies that specify an excluded sign_id that matches the provided sign_id
        BuildPolicy._remove_policies_for_excluded_sign_ids(self.sec_image_policies, self.sec_image_policy_ids, sign_id)

        # get the install policies
        logger.debug("Getting install policies for \"{0}\" build policy...".format(build_policy_id))
        self.install_policies, self.install_policy_ids = BuildPolicy._get_policies(root.install_policy.policy, self.policy.install_policy)
        logger.debug("Install policies retrieved for \"{0}\" build policy: {1}".format(build_policy_id, ", ".join(self.install_policy_ids)))

        # initialize install locations for sec image policies if installation should be performed
        if self.install_base_dir:
            BuildPolicy._set_install_locations_for_sec_image_policies(self.install_policies, self.sec_image_policies, self.install_base_dir)

    def __str__(self):

        def sec_policies_to_string(sec_policies):

            def exclude_to_string(complex_exclude_object):
                exclude_list = []
                for exclude in complex_exclude_object:
                    for sign_id in exclude.sign_id:
                        exclude_list.append(sign_id)
                return ", ".join(exclude_list)

            policy_string_list = []
            for policy in sec_policies:
                if policy.exclude:
                    if hasattr(policy, 'install_locations'):
                        policy_string_list.append("id: {0} excluded_sign_ids: {1} install_locations: {2} cmd_options: {3}".format(policy.id, exclude_to_string(policy.exclude), ", ".join(policy.install_locations), policy.cmd_options))
                    else:
                        policy_string_list.append("id: {0} excluded_sign_ids: {1} cmd_options: {2}".format(policy.id, exclude_to_string(policy.exclude), policy.cmd_options))
                else:
                    if hasattr(policy, 'install_locations'):
                        policy_string_list.append("id: {0} install_locations: {1} cmd_options: {2}".format(policy.id, ", ".join(policy.install_locations), policy.cmd_options))
                    else:
                        policy_string_list.append("id: {0} cmd_options: {1}".format(policy.id, policy.cmd_options))
            return "\n\t".join(policy_string_list)

        def install_policies_to_string(install_policies, install_base_dir):

            def directory_to_string(complex_directory_object, install_base_dir):
                directory_list = []
                for directory in complex_directory_object:
                    directory_list.append("[" + directory.sec_image_policy + ": " + c_path.normalize(directory.value[:].replace("${sectools_install_base_dir}", install_base_dir)) + "]")
                return ", ".join(directory_list)

            policy_string_list = []
            for policy in install_policies:
                policy_string_list.append("id: {0} sec_image_policy/install location pairs: {1}".format(policy.id, directory_to_string(policy.directory, install_base_dir)))
            return "\n\t".join(policy_string_list)

        return "~~~~~~~~~~~~~~~BuildPolicy~~~~~~~~~~~~~~~" + "\n" + \
            "id: " + self.policy.id + "\n" + \
            "sec_image_policies: \n\t" + sec_policies_to_string(self.sec_image_policies) + "\n" + \
            (("install_policies: \n\t" + install_policies_to_string(self.install_policies, self.install_base_dir) + "\n") if self.install_base_dir else "") + \
            "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

    @staticmethod
    def get_supported_secimage_policies(policy_file):
        logger.debug("Getting supported secimage policies...")
        root = CoreConfig(policy_parser, policy_file).root
        # get all the supported secimage policies
        secimage_policies = root.sec_image_policy.policy
        # loop through supported secimage policies and get their ids
        supported_secimage_policies = []
        for secimage_policy in secimage_policies:
            supported_secimage_policies.append(secimage_policy.id)
        return supported_secimage_policies

    @staticmethod
    def get_supported_build_policies(policy_file):
        logger.debug("Getting supported build policies...")
        root = CoreConfig(policy_parser, policy_file).root
        # get all the supported build policies
        build_policies = root.build_policy.policy_enable
        # loop through supported build policies and get their ids
        supported_build_policies = []
        for build_policy in build_policies:
            supported_build_policies.append(build_policy.id)
        return supported_build_policies

    @staticmethod
    def _get_policies(supported_policies, selected_policy_ids, policies_to_ignore=None):
        selected_policies = []
        selected_ids = []
        for policy in supported_policies:
            if policy.id in selected_policy_ids:
                if policies_to_ignore is None or (policies_to_ignore is not None and policy.id not in policies_to_ignore):
                    selected_policies.append(policy)
                    selected_ids.append(policy.id)
                else:
                    logger.debug("Secimage policy {0} is already included in the selected build policy".format(policy.id))
        return selected_policies, selected_ids

    @staticmethod
    def _remove_policies_for_excluded_sign_ids(policies_to_filter, policy_ids_to_filter, sign_id):
        logger.debug("Filtering out secimage policies for excluded sign ids...")
        ids_to_exclude = []
        for policy in policies_to_filter:
            for exclude in policy.exclude:
                if sign_id in exclude.sign_id:
                    ids_to_exclude.append(policy.id)
                    policies_to_filter.remove(policy)
                    logger.debug("Filtered out secimage policy {0}".format(policy.id))
        for policy_id in policy_ids_to_filter:
            if policy_id in ids_to_exclude:
                policy_ids_to_filter.remove(policy_id)

    @staticmethod
    def _set_install_locations_for_sec_image_policies(install_policies, sec_image_policies, install_base_dir):
        logger.debug("Getting the install policy install locations for the enabled secimage policies...")
        # loop though sec image policies for selected build policy
        for sec_image_policy in sec_image_policies:
            # initialize the sec_image_policy's install location list if the list doesn't exist yet
            if not hasattr(sec_image_policy, 'install_locations'):
                sec_image_policy.install_locations = []
            # loop through install policies for selected build policy
            for install_policy in install_policies:
                # loop through directories of each install policy
                for directory in install_policy.directory:
                    # if the directory's sec_image_policy matches the current sec_image_policy
                    # then add the directory's path value to the sec_image_policy's list of install locations
                    if directory.sec_image_policy == sec_image_policy.id:
                        # add the new install location to the sec_image_policy's list of locations substituting in the value for install_base_dir
                        if directory.value not in sec_image_policy.install_locations:
                            directory_path = c_path.normalize(directory.value[:].replace("${sectools_install_base_dir}", install_base_dir))
                            sec_image_policy.install_locations.append(directory_path)
                            logger.debug("Added {0} to the install locations for secimage policy {1}".format(c_path.normalize(directory.value[:].replace("${sectools_install_base_dir}", install_base_dir)), sec_image_policy.id))
