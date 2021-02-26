#!/bin/sh

export Q6VERSION=v5


export Q6_TOOLS_ROOT=/pkg/qct/software/hexagon/releases/tools/5.1.04
export PATH=/pkg/python/2.7.1/bin:$PATH
export PATH=$PATH:/pkg/java/j2sdk1.4.2_09/bin/
export PATH="${Q6_TOOLS_ROOT}/qc/bin:${PATH}"
export PATH="${Q6_TOOLS_ROOT}/gnu/bin:${PATH}"
export PATH="/pkg/qct/gnu/software/make/bin:${PATH}"
export Q6_ROOT=${Q6_TOOLS_ROOT}
