#! /bin/bash
set -e
. activate.sh
# hack to prevent an error
if [ -f ./managed_components/espressif__fmt/.component_hash ]; then
    rm ./managed_components/espressif__fmt/.component_hash
fi
echo "Choose your device:"
echo " [1] esp32 - WT32-SC01"
echo " [2] esp32c3 - WT32-SC01+"
echo " [3] esp32s3 - ESP32S3SPI35 / ESP32S335D"
# query user input for device selection
read -p "Enter your choice [1-3]: " choice
rm -rf build

case $choice in
    1) echo "esp32 - WT32-SC01"
       idf.py set-target esp32
       ;;
    2) echo "esp32c3 - WT32-SC01+"
       idf.py set-target esp32c3
       ;;
    3) echo "esp32s3 - ESP32S3SPI35 / ESP32S335D"
       idf.py set-target esp32s3
       ;;
    *) echo "Invalid choice"
    exit 1
       ;;
esac

if [ -f ./managed_components/espressif__fmt/.component_hash ]; then
    rm ./managed_components/espressif__fmt/.component_hash
fi
idf.py menuconfig

echo ""
echo "Configuration done, now run ./deploy.sh to build and flash the firmware"
