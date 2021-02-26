#**Video Analytics**

This project allows you to run Video Analytics on the target device.

**Folder structure**

    ├── engine_generator
            Template for custom engine
    ├── test_engines
            Built-in engines to test basic features of VAM
    │   ├── GroundTruthEngine
    │   ├── snpe
    │   ├── TestEngine1
    │   └── TestEngine2
    ├── vam_lib
            Builds libVAManager.so
    ├── vam_sim
            Video Analytics Simulator
    └── vam_utils
        ├── JSON
                Builds libjson_apis.so
        └── VAMReport
                A program to compare metadata & ground truth

**LE Recipes structure**

* **vam-lib** - Compiles contents in *vam_lib* dir
* **vam-test** - Compiles contents in the rest dirs