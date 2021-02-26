Folder structure & file description

    .
    ├── vam_sim
    │   ├── VASim_test.sh
              The script for running VA Simulation
    │   ├── VASim_test.config
              The config for running VA Simulation
    │   ├── engine_data
              Default VA engine data folder
    │   ├── sample_input
    │       ├── video_sample_*_0_0.idx
                  Ground truth files for video_sample_*.yuv
    	    ├── video_sample_*_0_0.jsonx
                  Ground truth files for video_sample_*.yuv
    	    ├── VASim_config.txt
                  Default VASim config
    	    ├── VASim_file_input_template.csv
                  Default file input list (tags, videos, ground truth) for VASim
    	    ├── VASim_rule_config*.json
                  Default JSON rule config for VASim
    	    └── video_sample_*.yuv
                  Default video sample - must push manually into this folder
    └── README
          This file.

# VASim automatic test

_NOTE: Be sure to have loaded engine(s) to the target prior to running the test_

* To load vam test engines, visit test_engines.
* To load vam built-in engines, visit vam-engines.
* To load a custom engine, push your own cross-compiled engine directly to /usr/lib64/VAMEngines

USAGE:

`$ adb shell`

`# cd usr/data/VAM`

`# ./VASim_test.sh [tag] `

  example:
  # ./VASim_test.sh first_test

  The VASim will start and load the sample video from sample_input/ folder and generate output in output/ folder

DESCRIPTION:

  To change input, output folder, please modify:
    VASim_test.config

  To change file input list, please modify:
    sample_input/VASim_file_input_template.csv

