#**VAM ML Unified Engine**

MLE combines SNPE and TFLite frameworks
####Steps to enable

**Prerequisites**
* LE environment
* [Download](https://developer.qualcomm.com/software/qualcomm-neural-processing-sdk) SNPE SDK

###**Enable SNPE compilation**


 * Navigate to *vam-test* recipe (poky/meta-qti-qmmf-prop/recipes/vam-test/vam-test.bb)
 and uncomment the following line:

    DISTRO_FEATURES += "snpe-enable"

 * Navigate to *Makefile.am* in this directory and specify SNPE_ROOT:

    SNPE_ROOT=<path_to_snpe_sdk>
 * rebake *vam-test*
 * build and flash images on device


**Push SNPE libraries on target:**

* Go to snpe sdk dir/lib/<desired_platform> (example:arm-linux-gcc4.8hf)
* Push both libraries in /usr/lib on device
* In case of using DSP runtime : go to snpe sdk dir/lib/dsp
* Push all libraries in /usr/lib on device


###**Verification**
* Make sure the engine is present on target:

   `adb shell ls -la /usr/lib/vam_engines`

* update the configuration file *va-ml-engine-library_config.json*

    "FrameworkType":0,  --> Framework type: Possible values:
    '0' --> SNPE;
    '1' --> TFLite;

    "EngineOutput":0,  --> Engine output: Possible values:
    '0' --> Single;
    '1' --> Multi;
    '2' --> SingleSSD;

    "InputFormat":3,  --> SNPE input format: Possible values:
    '0' --> RGB
    '1' --> BGR
    '2' --> RGB Float
    '3' --> BGR Float
    * Currently SNPE common engine supports only BGR Float

    "NetworkIO":0" --> Network IO: Possible values:
    '0' --> UserBufer
    '1' --> ITensor

    "ScaleWidth":,  --> ScaleWidth

    "ScaleHeight":, --> ScaleHeight

    "BlueMean":,   --> BlueMean
    "BlueSigma":,   --> Divisor of blue channel for norm
    "GreenMean":,   --> GreenMean
    "GreenSigma":,   --> Divisor of green channel for norm
    "RedMean":,   --> RedMean
    "RedSigma":,   --> Divisor of red channel for norm
    Mean Range [0 - 255]
    Sigma default value: 255

    "UseNorm":1,   --> Normalization
    '0' --> Do not use normalization
    '1' --> Use normalization

    "TargetFPS":30,   --> TargetFPS

    "ConfThreshold":0.0,   --> ConfThreshold
    Range [0.0 - 1.0]

    "MODEL_FILENAME":"<name>.dlc/tflite", --> dlc/tflite file name

    "LABELS_FILENAME":"<name>.txt", --> labels file name

    "InputLayers" --> Input layers

    "OutputLayers" --> Output layers - array of strings.
    Sample format: "OutputLayers":["add_6", "Postprocessor/BatchMultiClassNonMaxSuppression"]
    It is possible to have single entry

    "ResultLayers" --> Used for comparing the results.
    It is possible to have single entry.
    For ComplexEngine, use the following convention:
    '[<pscore> <pbox> <pclass>]'

    Note: Input, output and result layers are applicable for SNPE only

*Note: Dimensions and layers can be obtained from snpe-dlc-info*

    "Runtime":0,  --> Runtime: Possible vallues:
    '0' --> CPU;
    '1' --> DSP;

    Note: the following parameters are for TFLite only
    "USE_NNAPI" - TFLite runtime. Possible values:
    '0' --> CPU
    '1' --> DSP

    "NUM_THREADS" - number of threads



* push the configuration file in /data/misc/camera:
    `adb push <path_to_config_file>/va-snpe-engine-library_config.json`
* push the necessary *labels* and *.dlc* files in /data/misc/camera:

    `adb push <path_to_dlc_file>/tensorflow_mobilenet_ssd.dlc /data/misc/camera`

    `adb push <path_to_labels_file>/coco_labels.txt /data/misc/camera`


*Note: All files (dlc, labels and config) should be pushed in /data/misc/camera*

* Verify using *APK* or *custom scripts for vam*
