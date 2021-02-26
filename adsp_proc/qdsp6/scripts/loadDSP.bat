adb shell "mount -t vfat -o remount,rw /dev/block/mmcblk0p1 /system/etc/firmware/misc"
adb shell "mount -o remount,rw /dev/block/mmcblkp012 /system"
adb shell rm /system/etc/firmware/q6.b00
adb shell rm /system/etc/firmware/q6.b01
adb shell rm /system/etc/firmware/q6.b02
adb shell rm /system/etc/firmware/q6.b03
adb shell rm /system/etc/firmware/q6.b04
adb shell rm /system/etc/firmware/q6.b05
adb shell rm /system/etc/firmware/q6.mdt
adb shell rm /system/etc/firmware/misc/image/q6.b00
adb shell rm /system/etc/firmware/misc/image/q6.b01
adb shell rm /system/etc/firmware/misc/image/q6.b02
adb shell rm /system/etc/firmware/misc/image/q6.b03
adb shell rm /system/etc/firmware/misc/image/q6.b04
adb shell rm /system/etc/firmware/misc/image/q6.b05
adb shell rm /system/etc/firmware/misc/image/q6.mdt
adb push %1\q6.b00 /system/etc/firmware/misc/image
adb push %1\q6.b01 /system/etc/firmware/misc/image
adb push %1\q6.b02 /system/etc/firmware/misc/image
adb push %1\q6.b03 /system/etc/firmware/misc/image
adb push %1\q6.b04 /system/etc/firmware/misc/image
adb push %1\q6.b05 /system/etc/firmware/misc/image
adb push %1\q6.mdt /system/etc/firmware/misc/image
adb push %1\q6.b00 /system/etc/firmware
adb push %1\q6.b01 /system/etc/firmware
adb push %1\q6.b02 /system/etc/firmware
adb push %1\q6.b03 /system/etc/firmware
adb push %1\q6.b04 /system/etc/firmware
adb push %1\q6.b05 /system/etc/firmware
adb push %1\q6.mdt /system/etc/firmware

