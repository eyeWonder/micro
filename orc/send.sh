
ino clean
ino build
until ino upload -m leonardo
do 
    echo "didn't upload"
done
osascript -e 'tell application "Terminal" to activate' -e 'tell application "System Events" to tell process "Terminal" to keystroke "t" using command down' -e 'tell application "Terminal" to do script "screen /dev/tty.usbmodem1411 57600" in selected tab of the front window'