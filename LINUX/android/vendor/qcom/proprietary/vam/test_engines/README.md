#**VAM test engines**

Provides sample test engines for VAM

**Folder structure**

    ├── common
            Common utilities
    ├── GroundTruthEngine
            Ground truth object detection, displays object name and location
    ├── snpe
            Sample reference SNPE engine
    ├── TestEngine1
            Detects objects, counts objects, and generates an event
    └── TestEngine2
            Counts and classifies objects, depends on object detection

**Testing**

* engines libraries should be in **/usr/lib/vam_engines** folder on target device