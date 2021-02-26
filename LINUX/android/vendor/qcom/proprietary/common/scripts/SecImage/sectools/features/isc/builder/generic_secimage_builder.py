# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from __secfile__ import DIR_PATH
from sectools.common.builder.builderutils import BuilderUtil
from sectools.features.isc.cfgparser import ConfigParser
from sectools.common.utils import c_path

SECTOOLS_DIR = c_path.join(DIR_PATH, "..", "..", "..", "..")
EWM_TYPE = "elf_wrapped_mbn"

class GenericSecImageBuilder(object):

    def __init__(self, env):
        self.env = env
        self.builderutil = BuilderUtil(env)

    def _generate_config_file(self, src_config, dest_config,
                              image_entry, relocatable):
        config = ConfigParser(src_config)

        image_type_list = config.root.parsegen.get_image_types_list()
        for image_type in image_type_list.get_image_type():
            if image_type.id == EWM_TYPE:
                #Override relocatable setting
                image_type.ewm_properties.relocatable = relocatable
                if image_entry is not None:
                    #Override image_entry
                    image_type.ewm_properties.image_entry = image_entry

        config.generate(dest_config)

    def build(self,
                target,
                source,
                sign_id,
                signer,
                qc_sign,
                jtag_id,
                soc_hw_version,
                soc_vers,
                app_id,
                config,
                target_base_dir,
                sec_image_policy,
                image_entry,
                relocatable):

        if jtag_id is not None and soc_hw_version is not None:
            self.builderutil.printinfo('both JTAG_ID and SOC_HW_VERSION are provided: jtag_id = ' + str(jtag_id) + ' soc_hw_version = ' + str(soc_hw_version))
            raise RuntimeError('please specify only one, JTAG_ID or SOC_HW_VERSION')

        self.builderutil.printinfo("sectools_builder: SECTOOLS_DIR = %s" % SECTOOLS_DIR)

        if sign_id.endswith("ewm"):
            c_path.create_dir(target_base_dir)
            generated_config = c_path.join(target_base_dir, "generated_config.xml")
            self._generate_config_file(config, generated_config, image_entry, relocatable)
            config = generated_config

        # Issuing sectools command
        cmds = ["python", c_path.join(SECTOOLS_DIR, "sectools.py"), "secimage", "-i", source,
                "-o", target_base_dir, "-g", sign_id, "-c", config]

        """
            FR 27556: Test keys to sign CRM builds
            if both jtag_id and soc_hw_version are provided, return error and exit;
            if one is provided, use that to sign the image;
            if neither is provided, use jtag_id=0 to sign the image.
        """
        if jtag_id is not None and soc_hw_version is None:
            jtag_id_arg = ["--cfg_msm_part", jtag_id]
            cmds = cmds + jtag_id_arg
        elif soc_hw_version is not None and jtag_id is None:
            soc_hw_version_arg = ["--cfg_soc_hw_version", soc_hw_version, "--cfg_in_use_soc_hw_version", "1"]
            cmds = cmds + soc_hw_version_arg

            """ FR34970: Signing software for multiple SOC HW Versions """
            if soc_vers is not None:
                soc_vers_arg = ["--cfg_soc_vers", soc_vers]
                cmds = cmds + soc_vers_arg

        """
            CR : support passing app_id for TZapps image signing
        """
        if app_id is not None:
            app_id_arg = ["--cfg_app_id", app_id]
            cmds += app_id_arg

        """
           8998 TZ QC signing with CASS
        """
        if signer:
            signer_arg = ["--cfg_selected_signer", signer]
            cmds = cmds + signer_arg
        if qc_sign:
            cmds = cmds + ["--qc_signing"]

        cmds.append(sec_image_policy.cmd_options)

        self.builderutil.printinfo("sectools_builder: %s" % " ".join(cmds))
        self.builderutil.execcmds(cmds, target=target)

        return self.builderutil.getReturnValueInBuilder(target)

