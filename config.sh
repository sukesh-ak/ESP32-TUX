#! /bin/bash
set -e
. activate.sh
# hack to prevent an error
if [ -f ./managed_components/espressif__fmt/.component_hash ]; then
    rm ./managed_components/espressif__fmt/.component_hash
fi
echo "Choose your device:"
echo " [1] esp32 - WT32-SC01"
echo " [2] esp32s3 - WT32-SC01-Plus / ESP32S3SPI35 / ESP32S335D"
# query user input for device selection
read -p "Enter your choice [1-2]: " choice
rm -rf build

case $choice in
    1) echo "esp32 - WT32-SC01"
       target=esp32
       ;;
    2) echo "esp32s3 - WT32-SC01-Plus / ESP32S3SPI35 / ESP32S335D"
       target=esp32s3
       ;;
    *) echo "Invalid choice"
    exit 1
       ;;
esac

idf.py set-target $target

if [ -f ./managed_components/espressif__fmt/.component_hash ]; then
    rm ./managed_components/espressif__fmt/.component_hash
fi

echo "In the next menu, select your device in the menuconfig: ESP32-TUX Configuration -> Device Configuration -> Select the device"
read -p "Press enter to continue..."
idf.py menuconfig

echo ""
echo "Configuration done, now run ./build.sh or ./deploy.sh to build and flash the firmware"
echo ""
