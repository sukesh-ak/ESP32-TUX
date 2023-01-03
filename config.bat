@echo off
call activate.bat

rem hack to prevent an error
echo.>managed_components\espressif__fmt\.component_hash
del managed_components\espressif__fmt\.component_hash

echo Choose your device:
echo  [1] esp32 - WT32-SC01
echo  [2] esp32c3 - WT32-SC01+
echo  [3] esp32s3 - ESP32S3SPI35 / ESP32S335D

rem query user input for device selection
set /p choice=Enter your choice [1-3]: 

if "%choice%" == "1" (
  echo esp32 - WT32-SC01
  python idf.py set-target esp32
) else if "%choice%" == "2" (
  echo esp32c3 - WT32-SC01+
  python idf.py set-target esp32c3
) else if "%choice%" == "3" (
  echo esp32s3 - ESP32S3SPI35 / ESP32S335D
  python idf.py set-target esp32s3
) else (
  echo Invalid choice
  exit 1
)

echo.>managed_components\espressif__fmt\.component_hash
del managed_components\espressif__fmt\.component_hash
python idf.py menuconfig