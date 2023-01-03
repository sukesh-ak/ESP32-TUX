@echo off
del managed_components\espressif__fmt\.component_hash

echo Choose your device:
echo  [1] esp32 - WT32-SC01
echo  [2] esp32s3 - WT32-SC01-Plus / ESP32S3SPI35 / ESP32S335D

rem query user input for device selection
set /p choice=Enter your choice [1-2]: 
rmdir /s /q build

if "%choice%" == "1" (
  echo esp32 - WT32-SC01
  set target=esp32
) else if "%choice%" == "2" (
  echo esp32s3 - WT32-SC01-Plus / ESP32S3SPI35 / ESP32S335D
  set target=esp32s3
) else (
  echo Invalid choice
  exit 1
)

activate.bat && idf.py set-target %target% && del managed_components\espressif__fmt\.component_hash && ^
echo. && ^
echo "In the next menu, select your device in the menuconfig: ESP32-TUX Configuration -> Device Configuration -> Select the device" && ^
echo. && ^
pause && ^
start /wait cmd /c "activate.bat && idf.py menuconfig" && ^
echo. && ^
echo Now use build.bat or deploy.bat to build and deploy your project. && ^
echo.
