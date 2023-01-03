@echo off
del managed_components\espressif__fmt\.component_hash

echo Choose your device:
echo  [1] esp32 - WT32-SC01
echo  [2] esp32c3 - WT32-SC01+
echo  [3] esp32s3 - ESP32S3SPI35 / ESP32S335D

rem query user input for device selection
set /p choice=Enter your choice [1-3]:
rmdir /s /q build

if "%choice%" == "1" (
  echo esp32 - WT32-SC01
  activate.bat && idf.py set-target esp32 && del managed_components\espressif__fmt\.component_hash && start cmd /k "activate.bat && idf.py menuconfig && exit"
) else if "%choice%" == "2" (
  echo esp32c3 - WT32-SC01+
  activate.bat && idf.py set-target esp32c3 && del managed_components\espressif__fmt\.component_hash && start cmd /k "activate.bat && idf.py menuconfig && exit"
) else if "%choice%" == "3" (
  echo esp32s3 - ESP32S3SPI35 / ESP32S335D
  activate.bat && idf.py set-target esp32s3 && del managed_components\espressif__fmt\.component_hash && start cmd /k "activate.bat && idf.py menuconfig && exit"
) else (
  echo Invalid choice
  exit 1
)
