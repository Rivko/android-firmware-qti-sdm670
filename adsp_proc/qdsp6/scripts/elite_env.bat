REM *********************************
REM Set all paths needed by Makefiles
REM *********************************
set Q6_TOOLS_ROOT=C:/Qualcomm/HEXAGON_Tools/5.1.04
set PATH=%Q6_TOOLS_ROOT%/qc/bin;%PATH%
set PATH=%Q6_TOOLS_ROOT%/gnu/bin;%PATH%
set PATH=%Q6_TOOLS_ROOT%/qc/lib/iss;%PATH%
set Q6_ROOT=%Q6_TOOLS_ROOT%


copy qurt\osam\bin\T32\v5\qurt_model.* C:\t32\EDK\QDSP6\
cp %Q6_TOOLS_ROOT%/examples/trace32/win.* C:/t32/
cp %Q6_TOOLS_ROOT%/qc/lib/iss/T32_MCD.dll C:/t32/
