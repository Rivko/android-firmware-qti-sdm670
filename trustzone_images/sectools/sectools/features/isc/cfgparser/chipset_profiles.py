# ===============================================================================
#
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


class ChipsetProfile(object):

    def __init__(self, secboot_version, in_use_soc_hw_version, msm_part,
                 soc_vers, soc_hw_version, platform_independent):
        self.secboot_version = secboot_version
        self.in_use_soc_hw_version = in_use_soc_hw_version
        self.msm_part = msm_part
        self.soc_vers = soc_vers if soc_vers else []
        self.soc_hw_version = soc_hw_version
        self.platform_independent = platform_independent


CHIPSET_PROFILES = {
    "9205": {
        "soc_vers": [
            "0x2013"
        ],
        "msm_part": [
            "0x001070E1"
        ]
    },
    "9206": {
        "msm_part": [
            "0x009730E1"
        ]
    },
    "806x": {
        "msm_part": [
            "0x008100E1"
        ]
    },
    "4020": {
        "msm_part": [
            "0x000160E1"
        ]
    },
    "8016": {
        "msm_part": [
            "0x007060E1"
        ]
    },
    "9x65": {
        "msm_part": [
            "0x0007F0E1"
        ]
    },
    "8940": {
        "msm_part": [
            "0x0006B0E1"
        ]
    },
    "sdm632": {
        "msm_part": [
            "0x000BA0E1",
            "0x000BB0E1"
        ]
    },
    "sdm630": {
        "soc_vers": [
            "0x3007"
        ],
        "msm_part": [
            "0x000AC0E1"
        ]
    },
    "sdm439": {
        "msm_part": [
            "0x000BF0E1",
            "0x000BE0E1"
        ]
    },
    "sdm670": {
        "soc_vers": [
            "0x6004",
            "0x6005",
            "0x6009",
        ],
        "msm_part": [
            "0x000910E1",
            "0x000930E1",  # SDA670
            "0x000DB0E1",  # SDM710
            "0x000AA0E1",  # QCS605
            "0x000ED0E1",  # SXR1120
            "0x000EA0E1",  # SXR1130
        ]
    },
    "8909": {
        "msm_part": [
            "0x009600E1"
        ]
    },
    "8905": {
        "msm_part": [
            "0x000940E1"
        ]
    },
    "sm8250": {
        "soc_vers": [
            "0x6008"
        ],
        "msm_part": [
            "0x000C30E1",
            "0x000CE0E1"
        ]
    },
    "sc8180x": {
        "soc_vers": [
            "0x6006"
        ],
        "msm_part": [
            "0x000B80E1"
        ]
    },
    "8064": {
        "msm_part": [
            "0x009600E1"
        ]
    },
    "sdm660": {
        "soc_vers": [
            "0x3006"
        ],
        "msm_part": [
            "0x0008C0E1"
        ]
    },
    "9x50": {
        "msm_part": [
            "0x0003A0E1",  # MDM9650
            "0x000390E1",  # MDM9350
            "0x000320E1",  # MDM9250
        ]
    },
    "9955": {
        "msm_part": [
            "0x0080B0E1"
        ]
    },
    "9x55": {
        "msm_part": [
            "0x0003B0E1",  # MDM9655
            "0x000340E1",  # MDM9255
        ]
    },
    "9206tx": {
        "msm_part": [
            "0x009730E1"
        ]
    },
    "6290": {
        "soc_vers": [
            "0x4004"
        ],
        "msm_part": [
            "0x000AB0E1"
        ]
    },
    "8053lat": {
        "msm_part": [
            "0x000660E1",  # APQ8053
        ]
    },
    "sdm845": {
        "soc_vers": [
            "0x6000"
        ],
        "msm_part": [
            "0x0008B0E1"
        ]
    },
    "8937": {
        "msm_part": [
            "0x0004F0E1",
            "0x000500E1",  # APQ8037
        ]
    },
    "8929": {
        "msm_part": [
            "0x0091B0E1"
        ]
    },
    "8939": {
        "msm_part": [
            "0x0090B0E1",
            "0x0090C0E1",  # APQ8036
            "0x150E10E1",  # New JTAG ID support for BBK on LA.2.1
        ]
    },
    "8976": {
        "msm_part": [
            "0x009B00E1",  # MSM8956
            "0x009900E1",  # MSM8976
        ]
    },
    "qcs405": {
        "soc_vers": [
            "0x2014"
        ],
        "msm_part": [
            "0x000E30E1",
            "0x000E40E1",
            "0x001040E1",
            "0x000AF0E1",
            "0x000EB0E1"
        ]
    },
    "2432": {
        "msm_part": [
            "0x009440E1"
        ]
    },
    "8039": {
        "msm_part": [
            "0x0090D0E1"
        ]
    },
    "8076": {
        "msm_part": [
            "0x009D00E1",  # APQ8076
            "0x009F00E1",  # APQ8056
        ]
    },
    "9x40": {
        "msm_part": [
            "0x009500E1"
        ]
    },
    "9x45": {
        "msm_part": [
            "0x009540E1"
        ]
    },
    "9x60": {
        "msm_part": [
            "0x0007D0E1"
        ]
    },
    "sm7150": {
        "soc_vers": [
            "0x600c"
        ],
        "msm_part": [
            "0x000E60E1",  # SM7150
            "0x000E70E1",  # SM7150p"
        ]
    },
    "9055": {
        "msm_part": [
            "0x0094B0E1"
        ]
    },
    "401x": {
        "msm_part": [
            "0x009790E1",  # IPQ4019
            "0x009780E1",  # IPQ4018
        ]
    },
    "9916": {
        "msm_part": [
            "0x008090E1"
        ]
    },
    "sdx24": {
        "soc_vers": [
            "0x6002"
        ],
        "msm_part": [
            "0x000960E1",  # SDX24
            "0x000970E1",  # SDX24M
            "0x0011d0E1"   # SDX24_AUTO
        ]
    },
    "8920": {
        "msm_part": [
            "0x000860E1"
        ]
    },
    "8996au": {
        "msm_part": [
            "0x0006F0E1",
            "0x400630E1",
        ]
    },
    "sm6150": {
        "soc_vers": [
            "0x6007"
        ],
        "msm_part": [
            "0x000950E1",  # SM6150
            "0x000EC0E1",  # SM6150p
            "0x000F50E1",  # SA6155/TalosAU
            "0x100EE0E1",  # SA6155P/TalosAU
            "0x1011C0E1",  #SM6150 IoT High
            "0x001290E1"   #SM6150 IoT Low
        ]
    },
    "807x": {
        "msm_part": [
            "0x0008A0E1"
        ]
    },
    "8909W": {
        "msm_part": [
            "0x000510E1",  # MSM8909W
            "0x000520E1",  # APQ8909W
        ]
    },
    "9607": {
        "msm_part": [
            "0x0004A0E1",
            "0x000480E1",  # MDM9207
        ]
    },
    "8009lat": {
        "msm_part": [
            "0x009680E1"
        ]
    },
    "sm8150": {
        "soc_vers": [
            "0x6003"
        ],
        "msm_part": [
            "0x000A50E1",  # sm8150
            "0x000A60E1",  # sm8150p
            "0x000E80E1",  # sa8155 (automotive)
            "0x000E90E1",  # sa8155p (automotive)
        ]
    },
    "8092": {
        "msm_part": [
            "0x009630E1"
        ]
    },
    "8084": {
        "msm_part": [
            "0x009000E1"
        ]
    },
    "8998": {
        "soc_vers": [
            "0x3002"
        ],
        "msm_part": [
            "0x000620E1",
            "0x0005E0E1"
        ]
    },
    "8952": {
        "msm_part": [
            "0x009720E1",
            "0x009770E1",  # APQ8052
        ]
    },
    "8953": {
        "msm_part": [
            "0x000460E1"
        ]
    },
    "8992": {
        "msm_part": [
            "0x009690E1"
        ]
    },
    "sdm450": {
        "msm_part": [
            "0x0009A0E1"
        ]
    },
    "8996": {
        "soc_vers": [
            "0x3000"
        ],
        "msm_part": [
            "0x009470E1"
        ]
    },
    "8994": {
        "msm_part": [
            "0x009400E1"
        ]
    },
    "9x35": {
        "msm_part": [
            "0x009210E1"
        ]
    },
    "8916": {
        "msm_part": [
            "0x007050E1"
        ]
    },
    "8917": {
        "msm_part": [
            "0x000560E1",
            "0x000550E1",  # APQ8017
            "0x0013D0E1"   # QCM2150
        ]
    },
    "qm215": {
        "msm_part": [
            "0x001060E1"
        ]
    },
    "sdx55": {
        "soc_vers": [
            "0x600b"
        ],
        "msm_part": [
            "0x000E50E1",
            "0x000CF0E1",
            "0x001250E1"
        ]
    },
    "nicobar": {
        "soc_vers": [
            "0x9001"
        ],
        "msm_part": [
            "0x0010A0E1",
            "0x001750E1",
            "0x001760E1"
        ]
    },
    "kamorta": {
        "soc_vers": [
            "0x9002"
        ],
        "msm_part": [
            "0x001360E1",
            "0x001370E1",
            "0x001730E1",
            "0x001740E1"
        ]
    },
    "sdx50m": {
        "msm_part": [
            "0x0007B0E1"
        ]
    },
    "fsm100xx": {
        "msm_part": [
            "0x001650E1",
            "0x001680E1",
            "0x001690E1",
            "0x0016A0E1",
            "0x001280E1",
            "0x0016B0E1"
        ]
    },
    "6390": {
        "msm_part": [
            "0x000D90E1",
            "0x001310E1",
            "0x0012E0E1",
            "0x0012F0E1",
            "0x0012D0E1"
        ],
        "soc_vers": [
            "0x400A",
            "0x400C"
        ]
    },
    "7605": {
        "msm_part": [
            "0x000D30E1",
            "0x000D50E1",
            "0x000D70E1",
        ],
        "soc_vers": [
            "0x400B"
        ]
    },
    "saipan": {
        "soc_vers": [
            "0x600D"
        ],
        "msm_part": [
            "0x0011E0E1"
        ]
    },
    "fraser": {
        "soc_vers": [
            "0x600D"
        ],
        "msm_part": [
            "0x0018A0E1"
        ]
    },
    "bitra": {
        "soc_vers": [
            "0x6012"
        ],
        "msm_part": [
            "0x0013F0E1",
            "0x001410E1"
        ]
    },
    "rennell": {
        "soc_vers": [
            "0x600E"
        ],
        "msm_part": [
            "0x0012A0E1",
            "0x0012B0E1",
            "0x001490E1"
        ]
    },
    "ipq6018": {
        "soc_vers": [
            "0x2017"
        ],
        "msm_part": [
            "0x001260E1"
        ]
    },
    "lahaina": {
        "soc_vers": [
            "0x600F"
        ],
        "msm_part": [
            "0x001350E1",
            "0x001420E1"
        ]
    },
    "qcn90xx": {
        "soc_vers": [
            "0x400D"
        ],
        "msm_part": [
            "0x0010B0E1",
            "0x0010C0E1",
            "0x001150E1",
            "0x0010D0E1",
            "0x0010E0E1",
            "0x0010F0E1",
            "0x001110E1",
            "0x001140E1"
        ]
    },
    "agatti": {
        "soc_vers": [
            "0x9003"
        ],
        "msm_part": [
            "0x001450E1",
            "0x0014F0E1",
            "0x001850E1",
            "0x001860E1"
        ]
    },
    "moselle": {
        "soc_vers": [
            "0x4014"
        ],
        "msm_part": [
            "0x001470E1"
        ]
    },
    "chitwan": {
        "soc_vers": [
            "0x6013"
        ],
        "msm_part": [
            "0x001440E1"
        ]
    },
    "olympic": {
        "soc_vers": [
            "0x6016"
        ],
        "msm_part": [
            "0x001610E1",
            "0x001720E1"
        ]
    },
    "makena": {
        "soc_vers": [
            "0x6014"
        ],
        "msm_part": [
            "0x0014A0E1",
            "0x0014B0E1",
            "0x0014C0E1"
        ]
    },
    "ipq5018": {
        "msm_part": [
            "0x001530E1",
            "0x001540E1",
            "0x001550E1"
        ]
    },
    "cedros": {
        "soc_vers": [
            "0x6017"
        ],
        "msm_part": [
            "0x001590E1"
        ]
    },
    "kodiak": {
        "soc_vers": [
            "0x6018"
        ],
        "msm_part": [
            "0x001920E1",
            "0x001930E1",
            "0x001940E1"
        ]
    },
    "mannar": {
        "soc_vers": [
            "0x9004"
        ],
        "msm_part": [
            "0x0016F0E1",
            "0x0016E0E1"
        ]
    },
    "sc7180": {
        "soc_vers": [
            "0x600E"
        ],
        "msm_part": [
            "0x0012C0E1"
        ]
    },
    "qtang2": {
        "soc_vers": [
            "0x7001"
        ],
        "msm_part": [
            "0x0018B0E1"
        ]
    }
}
