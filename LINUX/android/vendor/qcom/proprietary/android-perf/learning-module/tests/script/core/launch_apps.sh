adb shell input keyevent KEYCODE_BACK
for i in `seq 1 5`; do
    adb shell am start -a android.intent.action.MAIN -n com.android.settings/.Settings
    sleep 3
    adb shell input keyevent KEYCODE_BACK
    sleep 3
    adb shell am start -a android.intent.action.MAIN -n com.android.mms/.ui.ConversationList
    sleep 3
    adb shell input keyevent KEYCODE_BACK
    sleep 3
    adb shell am start -a android.intent.action.MAIN -n com.android.dialer/.app.DialtactsActivity
    sleep 3
    adb shell input keyevent KEYCODE_BACK
    sleep 3
done
adb shell lmtests idle
