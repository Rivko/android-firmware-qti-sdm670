@echo off
set ERRORLEVEL=1
set loopcount=1
adb shell chmod 777 /data/media
if %ERRORLEVEL% LEQ 0 goto bad
Adb push  \\haagen\MM_tech_test\TestPlan_Clips\V2.6\Clipsforautomation\au4.mp3  /data/media
if %ERRORLEVEL% LEQ 0 goto bad
Adb push  "\\kevwang\Dropbox\test media\audio\01_ENTERSANDMAN_pcm_16bit_mono_32KHz_.wav" /data/media
if %ERRORLEVEL% LEQ 0 goto bad
:start
        adb shell rm /data/audio_test
        if %ERRORLEVEL% LEQ 0 goto bad
        adb shell "./system/bin/mm-audio-native-test & echo \"Playing audio\"; sleep 1; echo \"playpcm /data/media/01_ENTERSANDMAN_pcm_16bit_mono_32KHz_.wav -id=1 -dev=/dev/msm_pcm_out\" > /data/audio_test; sleep 30"
        if %ERRORLEVEL% LEQ 0 goto bad
        adb shell "echo \"sleeping 30sec\"; sleep 5"
        if %ERRORLEVEL% LEQ 0 goto bad
        echo "loop count = %loopcount%"
        set /a loopcount="%loopcount%+1"
if %loopcount% LEQ %1 goto start
goto ok

:bad
echo "Problem at loop %loopcount%"
goto done

:ok
echo "Done loopcount = %loopcount%"

:done

