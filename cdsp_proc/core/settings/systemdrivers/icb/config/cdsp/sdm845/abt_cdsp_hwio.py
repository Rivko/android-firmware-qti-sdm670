
# \\ben\corebsp_labdata_0001\sysdrv\hwio\HWIOGen.py --cfg=abt_hwio.py --flat=..\..\..\..\..\api\systemdrivers\hwio\sdm845\ARM_ADDRESS_FILE.FLAT
HWIO_REGISTER_FILES = [
  {
    'filename': 'ABTimeout_HWIO_CDSP.h',
    'modules': [
      'TURING_AHBE_TIME',
      'TCSR_TCSR_REGS',
    ],
    'module-filter-include': {
      'TURING_AHBE_TIME': ['BASE'],
      'TCSR_TCSR_REGS': ['TIMEOUT_SLAVE_GLB_EN', 'TCSR_TIMEOUT_INTR_STATUS',
         'TCSR_TIMEOUT_INTR_TURING_ENABLE' ],
    },
    'header': '''#include "msmhwiobase.h"
''',
  },
]

if __name__ == "__main__":
  from subprocess import Popen
  hwiogen = Popen([r"python", r"\\ben\corebsp_labdata_0001\sysdrv\hwio\HWIOGen.py", r"--cfg="+__file__, r"--flat=..\..\..\..\..\..\api\systemdrivers\hwio\sdm845\ARM_ADDRESS_FILE.FLAT"], shell=True)
  hwiogen.wait()

