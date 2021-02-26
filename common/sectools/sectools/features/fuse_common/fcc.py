# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
:Authors: sectools

Main interface to the fuseblower functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""


from collections import namedtuple
import os
import shutil

from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import load_data_from_file, \
    PROGRESS_CB_PYPASS, ProgressNotifier
from sectools.common.utils import c_path
from sectools.features.fuse_common import datamodel
from sectools.features.fuse_common import defines
from sectools.features.fuse_common.parsegen.secdat import SecdatParseGen as secdat_pg
from sectools.features.fuse_common.validator.secdat_validator import SecDatValidator
from sectools.features.fuse_common.debugrepr import dump_debug_data_model, dump_debug_secdat


class FuseCommonCore(object):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).

    User can select the config files and output dir for secdat generation using:

    - :meth:`workspace` and :meth:`chipset`
    - :meth:`oem_config_path`, :meth:`qti_config_path`, :meth:`ui_config_path` and :meth:`output_dir`

    User can specify secdat file for validation using:

    - :meth:`secdat`

    User can finally perform the following operations:

    - :meth:`verify_setup`
    - :meth:`generate`
    - :meth:`validate`

    User can also perform multiple operations using :meth:`process`

    :param str config_dir: Path to the directory containing the config files
        for the supported chipsets.
    :param bool debug: If all the underlying modules should work in debug mode
        (save debug logs, etc).

    .. data:: debug

        (bool) Is debug mode enabled. The debug logs will be stored in :meth:`output_dir`

    """

    def __init__(self, config_dir=defines.DEFAULT_CONFIG_DIR, debug=False):
        """Initializes the internal properties."""
        config_dir = c_path.normalize(config_dir)

        # Object representing the workspace, when set.
        # type: :class:`fb_config.WorkspaceDir`
        self._workspace_dir_obj = None

        # Object representing the config directory.
        # type: :class:`fb_config.ConfigDir`
        self._config_dir_obj = self.CONFIG_MODULE.ConfigDir(config_dir)

        # Object representing the config files being used.
        # type: :class:`fb_config.ConfigParser`
        self._config_parser = self.CONFIG_MODULE.ConfigParser()

        # Path of the output directory
        self._output_dir = None

        # Path of the secdat file for validation
        self._secdat = None

        # Public properties
        self.debug = debug

    @property
    def config_dir(self):
        """(str) Path to the config directory provided during initialization of
        the class.
        Cannot be set.
        """
        return self._config_dir_obj.config_dir

    @config_dir.setter
    def config_dir(self, config_dir):
        raise RuntimeError('Config dir cannot be set.')

    @property
    def workspace(self):
        """(str) Path to the workspace directory."""
        if self._workspace_dir_obj is None:
            raise RuntimeError('Workspace is not set.')
        return self._workspace_dir_obj

    @workspace.setter
    def workspace(self, workspace_path):
        workspace_path = c_path.normalize(workspace_path)
        self._workspace_dir_obj = self.CONFIG_MODULE.WorkspaceDir(workspace_path)
        logger.info('Workspace is set to: ' + workspace_path)

    @property
    def available_owners(self):
        """(list[str]) List of owners for which the data model is available.
        Cannot be set.
        """
        return datamodel.defines.SUPPORTED_OWNERS

    @available_owners.setter
    def available_owners(self, owners_list):
        raise RuntimeError('Available owners cannot be set.')

    @property
    def available_operations(self):
        """(list[str]) List of operations which are available to be performed
        on the fuses.
        Cannot be set.
        """
        return datamodel.defines.SUPPORTED_OPERATIONS

    @available_operations.setter
    def available_operations(self, operation_list):
        raise RuntimeError('Available operations cannot be set.')

    @property
    def available_chipsets(self):
        """(list[str]) List of chipsets for which config files are available
        in :meth:`workspace` and :meth:`config_dir`.
        Cannot be set.
        """
        if self._workspace_dir_obj is not None:
            workspace_chipsets = set(self._workspace_dir_obj.chipsets)
        else:
            workspace_chipsets = set()
        config_dir_chipsets = set(self._config_dir_obj.chipsets)
        return list(workspace_chipsets.union(config_dir_chipsets))

    @available_chipsets.setter
    def available_chipsets(self, chipset_list):
        raise RuntimeError('Available chipsets cannot be set.')

    @property
    def chipset(self):
        """(str) Chipset that is selected for the current run. Chipset can only
        be set after :meth:`workspace` is specified. Setting the chipset will
        update the :meth:`oem_config_path`, :meth:`qti_config_path`,
        :meth:`ui_config_path` and :meth:`output_dir`
        """
        if self._config_parser.chipset is not None:
            return self._config_parser.chipset
        else:
            raise RuntimeError('Config paths or chipset is not set.')

    @chipset.setter
    def chipset(self, chipset):
        # Check that the workspace is set
        if self._workspace_dir_obj is None:
            raise RuntimeError('Please set workspace before setting the chipset.')

        # Log if the chipset is changed
        try:
            selected_chipset = self.chipset
            if selected_chipset:
                logger.note('Switching chipset from "' + selected_chipset + '" to "' + chipset + '"')
        except Exception:
            pass

        # Update the workspace
        self._update_workspace(chipset)

        # Set the config paths
        self.set_config_paths(*self._workspace_dir_obj.get_chipset_config_paths(chipset))

        # Set the ouput dir
        self.output_dir = c_path.join(os.path.dirname(self._workspace_dir_obj.config_dir),
                                      'secdat/' + chipset)

        # Add the existing secdat
        secdat = c_path.join(self.output_dir, 'sec.dat')
        if c_path.validate_file(secdat):
            self.secdat = secdat
        else:
            self.secdat = None

    def _update_workspace(self, chipset):
        """Checks if the :meth:`workspace` contains the files for the given
        chipset. If the files dont exist in the workspace, :meth:`config_dir` is
        searched for the files for the chipset. If the files are found in the
        config_dir, they are copied to the workspace, otherwise an exception
        is raised.

        :param str chipset: The chipset to find in the workspace.
        :raise RuntimeError: If files for the chipset are not found in the
            workspace or the config_dir
        """
        # Check if workspace is missing any of the config files for the chipset
        has_oem, has_qti, has_ui, has_user = self._workspace_dir_obj.has_configs(chipset)

        # If any of the files are missing, update the workspace
        if not (has_oem and has_qti and has_ui and has_user):
            # Put these files in the workspace
            self.reset_workspace(chipset, has_oem, has_qti, has_ui, has_user)

    def reset_workspace(self, chipset=None, oem=True, qti=True, ui=True, user=True):
        """Resets the :meth:`workspace` for the chipset that is provided.
        :meth:`config_dir` is searched for the files for the chipset. If the
        files are found in the config_dir, they are copied to the workspace,
        otherwise an exception is raised.

        :param str chipset: The chipset to reset in the workspace.
        :param bool oem: If the oem file should be updated.
        :param bool qti: If the qti file should be updated.
        :param bool ui: If the ui file should be updated.
        :param bool user: If the user file should be updated.
        :raise RuntimeError: If files for the chipset are not found in the
            the config_dir
        """
        if chipset is None:
            chipset = self.chipset

        # Look for the config files in the config_dir
        try:
            template_oem, template_qti, template_ui, template_user = self._config_dir_obj.get_chipset_config_paths(chipset)
        except Exception as e:
            logger.warning('Template config files not found for copying to workspace for chipset: ' + chipset + '\n'
                           '    ' + 'Error: ' + str(e))
            raise

        # Create the directory in the workspace for this chipset
        self._workspace_dir_obj.create_chipset_dir(chipset)
        workspace_chipset_dir = self._workspace_dir_obj.get_chipset_dir(chipset)

        if not oem:
            # Copy the OEM template file
            logger.info('Copying template OEM config file to workspace for chipset: ' + chipset)
            shutil.copy(template_oem, workspace_chipset_dir)
            logger.note('OEM config file created at: ' + workspace_chipset_dir)

        if not qti:
            # Copy the QTI template file
            logger.info('Copying template QTI config file to workspace for chipset: ' + chipset)
            shutil.copy(template_qti, workspace_chipset_dir)
            logger.note('QTI config file created at: ' + workspace_chipset_dir)

        if not ui:
            # Copy the UI template file
            logger.info('Copying template UI config file to workspace for chipset: ' + chipset)
            shutil.copy(template_ui, workspace_chipset_dir)
            logger.note('UI config file created at: ' + workspace_chipset_dir)

        if not user:
            # Copy the USER template file
            logger.info('Copying template USER config file to workspace for chipset: ' + chipset)
            shutil.copy(template_user, workspace_chipset_dir)
            logger.note('USER config file created at: ' + workspace_chipset_dir)

    @property
    def is_workspace_resettable(self):
        """(bool) Is the workspace resettable.
        """
        resettable = True
        try:
            self.workspace
            self.chipset
        except Exception:
            resettable = False
        return resettable

    @property
    def user_config_path(self):
        """(str) OEM config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.user_config_path

    @user_config_path.setter
    def user_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def oem_config_path(self):
        """(str) OEM config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.oem_config_path

    @oem_config_path.setter
    def oem_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def qti_config_path(self):
        """(str) QTI config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.qti_config_path

    @qti_config_path.setter
    def qti_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    @property
    def ui_config_path(self):
        """(str) UI config path that is selected for the current run.
        Cannot be set directly. Use :meth:`set_config_paths`.
        """
        return self._config_parser.ui_config_path

    @ui_config_path.setter
    def ui_config_path(self, config_path):
        raise RuntimeError('Cannot be set')

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None):
        """Sets the config paths to the paths provided.

        :param str oem_config_path: Path to the OEM config file
        :param str qti_config_path: Path to the QTI config file
        :param str ui_config_path: Path to the UI config file
        """
        # Normalize all paths
        oem_config_path = c_path.normalize(oem_config_path) if oem_config_path is not None else None
        qti_config_path = c_path.normalize(qti_config_path) if qti_config_path is not None else None
        ui_config_path = c_path.normalize(ui_config_path) if ui_config_path is not None else None
        user_config_path = c_path.normalize(user_config_path) if user_config_path is not None else None

        # Set the paths on the config_parser
        self._config_parser.read(oem_config_path,
                                 qti_config_path,
                                 ui_config_path,
                                 user_config_path)

    @property
    def output_dir(self):
        """(str) Path to the output directory to use for saving logs, output
        files, debug_files.
        """
        if self._output_dir is not None:
            return self._output_dir
        else:
            raise RuntimeError('Output directory is not set')

    @output_dir.setter
    def output_dir(self, output_dir):
        if output_dir:
            output_dir = c_path.normalize(output_dir)
            try:
                c_path.create_dir(output_dir)
            except Exception as e:
                raise RuntimeError('Cannot write in the given output_dir: ' + output_dir + '\n'
                                   '    ' + 'Error: ' + str(e))
        self._output_dir = output_dir
        logger.info('Output dir is set to: ' + output_dir)

    @property
    def secdat(self):
        """(str) Path to the secdat file to be validated."""
        if self._secdat is not None:
            return self._secdat
        else:
            raise RuntimeError('Secdat path is not set')

    @secdat.setter
    def secdat(self, secdat_path):
        if secdat_path:
            secdat_path = c_path.normalize(secdat_path)
            if not c_path.validate_file(secdat_path):
                raise RuntimeError('Cannot access the secdat file: ' + secdat_path)
        self._secdat = secdat_path
        logger.info('Secdat path is set to: ' + str(secdat_path))

    @property
    def data_model(self):
        """(obj) Handle to the data model that is being worked with.
        Cannot be set.
        """
        return self._config_parser.data_model

    @data_model.setter
    def data_model(self, data_model):
        raise RuntimeError('Data model cannot be set.')

    def get_data_model_by_owner(self, owner):
        """Returns a handle to the data model filtered by the given owner.

        :param str owner: The owner to filter the data model with.
        :returns obj: datamodel
        """
        return datamodel.auto_gen_model.filter_by_owner(self._config_parser.data_model, owner)

    def update_data_model(self, data_model):
        """Updates the internal data model with the one given.

        :param obj data_model: The data model to use to update the internal data
            model
        """
        self._config_parser.data_model += data_model

    def save_workspace(self):
        """Saves the current :meth:`data_model` in the workspace by
        updating the config files for the selected chipset in the workspace.
        This API works only if the :meth:`workspace` and :meth:`chipset` is set.
        """
        if self._workspace_dir_obj is None:
            raise RuntimeError('Workspace is not set.')

        self._config_parser.write()
        self.generate()

    def get_supported_validations(self):
        # TODO: Hook up to the validator
        pass

    def set_validations(self):
        # TODO: Hook up to the validator
        pass

    def _verify_generate(self):
        """Verify that the current arguments are sufficient for generation of
        secdat.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        if not (self.oem_config_path or self.qti_config_path):
            return False, 'generation of sec.dat: Missing at least one required config file'
        elif not (self.output_dir):
            return False, 'generation of sec.dat: Missing the output directory'
        else:
            return True, ''

    def _verify_validate(self):
        """Verify that the current arguments are sufficient for validation of
        secdat.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        if not (self.oem_config_path or self.qti_config_path):
            return False, 'validation of sec.dat: Missing at least one required config file'
        elif not (self.secdat):
            return False, 'validation of sec.dat: Missing sec.dat path'
        else:
            return True, ''

    @property
    def is_validate_enabled(self):
        """(bool) If validation is enabled"""
        return self._verify_validate()[0]

    def verify_setup(self, generate, validate):
        """Verify that the current arguments are sufficient for validation of
        secdat.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        # Ensure the correct set of operations is provided
        if not (generate or validate):
            raise RuntimeError('Please specify one or more operations to perform.')

        retval_gen, reterr_gen = True, ''
        retval_val, reterr_val = True, ''
        if generate:
            retval_gen, reterr_gen = self._verify_generate()
        if validate:
            retval_val, reterr_val = self._verify_validate()
        if not (retval_gen and retval_val):
            raise RuntimeError('Setup is not complete. Based on the operation, setup parameters for one of the two:' + '\n'
                               '    ' + reterr_gen + '\n'
                               '    ' + reterr_val)

    def get_version_dir(self, version):
        return 'v' + str(version)

    def generate(self):
        """Generate the secdat file based on the configs which have been setup.

        :param str secdat: the path to the secdat file to be generated.
        """
        retval_gen, reterr_gen = self._verify_generate()
        if not retval_gen:
            raise RuntimeError(reterr_gen)

        # Get the version from config file
        ucp = self._config_parser
        ucp_file_version = ucp.get_secdat_file_version()
        ucp_fuse_version = ucp.get_secdat_fuse_version()
        ucp_file_info = ucp.get_secdat_file_info()
        version_dir = self.get_version_dir(ucp_file_version)

        #--------------------------------------------------------------------------------------------------------
        # Setting input_secdat path:
        #            @ if input given as -s
        #            @ if sec.dat exists in the keyprovision_output
        #--------------------------------------------------------------------------------------------------------
        in_secdat = None

        # Checking self secdat
        if in_secdat is None:
            try:
                in_secdat = self.secdat
            except Exception:
                pass

        # Checking common secdat
        if in_secdat is None:
            common_secdat = c_path.join(self.output_dir, defines.COMMON_DIR, version_dir, defines.SEC_DAT)
            if os.path.isfile(common_secdat):
                in_secdat = common_secdat

        # Create list of output secdat
        out_secdats = []
        IO = namedtuple('IO', 'in_data out_path')

        out_secdats.append(IO(None, c_path.join(self.output_dir, self.FEATURE_DIR, version_dir, defines.SEC_DAT)))
        out_secdats.append(IO(load_data_from_file(in_secdat) if in_secdat else None,
                              c_path.join(self.output_dir, defines.COMMON_DIR, version_dir, defines.SEC_DAT)))

        # Generate the secdats:
        for secdat in out_secdats:
            out_dir = os.path.dirname(secdat.out_path)
            c_path.create_dir(out_dir)
            secdat_obj = secdat_pg(self.SECDAT_SEGMENT_TYPE, secdat.in_data)
            secdat_obj.data_model = self.data_model
            secdat_obj.write(secdat.out_path, ucp_file_version, ucp_fuse_version, ucp_file_info)
            dump_debug_data_model(self.data_model, out_dir, self.debug, self.TOOL_NAME)
            dump_debug_secdat(secdat_obj, out_dir, self.debug)
            logger.info('Secdat generated at: ' + secdat.out_path)

        self.secdat = c_path.join(self.output_dir, self.FEATURE_DIR, version_dir, defines.SEC_DAT)

    def validate(self, secdat):
        """Validate the given secdat file.

        :param str secdat: the path to the secdat file to be validated.
        """
        logger.info('\n')
        secdat = c_path.normalize(secdat)
        retval_val, reterr_val = self._verify_validate()
        if not retval_val:
            raise RuntimeError(reterr_val)

        validator = SecDatValidator(oem_config_path=self.oem_config_path,
                                    qti_config_path=self.qti_config_path,
                                    user_config_path=self.user_config_path,
                                    ui_config_path=self.ui_config_path,
                                    sec_dat_path=self.secdat,
                                    config=self.CONFIG_MODULE,
                                    feature=self.SECDAT_SEGMENT_TYPE,
                                    output_dir = c_path.join(self.output_dir, self.FEATURE_DIR),
                                    debug = self.debug,
                                    TOOL_NAME = self.TOOL_NAME)

        validator.validate()

    def process(self, verify_setup=False, generate=False, validate=False,
                progress_cb=PROGRESS_CB_PYPASS):
        """Performs the fuse blowing operations specified from the params.

        :param bool verify_setup: Verify that the configuration of the object
            is correct and return. This will ignore any other params.
        :param bool generate: Generate the sec.dat using the config files.
        :param bool validate: Validate the sec.dat
        :param cb progress_cb: Callback method to get a status update during
            processing.

            Callback method should have this format:
            ::
                def progress_cb(status_string, progress_percent):
                    \"""
                    :param str status_string: the current status.
                    :param int progress_percent: the progress (in percent)
                    \"""
                    ...

        """
        # Verify setup
        if verify_setup:
            self.verify_setup(generate, validate)
            logger.note('The inputs provided (config, cmd args) are valid.')
            return

        # Total number of stages. This is used for progress notification via the
        # callback.
        _PROGRESS_TOTAL_STAGES = 3

        # Start processing images
        progress = ProgressNotifier(1, progress_cb, _PROGRESS_TOTAL_STAGES)

        # Send a progress notification to the toplevel
        progress.status = 'Processing..'
        progress.cur_stage = 0
        progress.push()

        # Generate the secdat
        if generate:
            self.generate()
        progress.push()

        # Validate the secdat
        if validate:
            self.validate(self.secdat)
        progress.push()

        # End processing
        progress.complete()
