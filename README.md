#  Wireless Tag WT32-SC01 / Plus (3.5" TFT Touch Display)
Design adapts to different screen resolution without any code changes.   

## ESP32/ESP32-S3 + ESP-IDF + LVGL9.X + LOVYANGFX
> Wireless Tag WT32-SC01  
![device](datasheet/WT32-SC01.png)  

> Wireless Tag WT32-SC01 Plus  
![device](datasheet/WT32-SC01-Plus.png)

Some features included in the sample

- Support for updating UI from different tasks [lvgl_acquire/lvgl_release](/main/main.cpp#L119)
- UI code separation into [gui.hpp](/main/gui.hpp)
- Same UI code which adapts to different resolutions - [here](/main/gui.hpp#L101)
- Supports shared SPI bus for SD Card - [here](/main/helper_storage.hpp)
- This Readme below explains how to have same project target different ESP32 controller.
- Switch between devices using just a header file inclusion - [here](/main/main.cpp#L15)
- Add your own controller/display with just a header change - [here](/main/main.cpp#L15)
- Shows battery meter animation using timer - [here](/main/main.cpp#L141)
- Shows SD card status change with icon
- Switch theme between Light & Dark - [here](/main/gui.hpp#L227)
- Scrolling long message in footer - [here](/main/gui.hpp#L258)
- Rotate screen 180deg [here](/main/main.cpp#L106)

### Demo of LVGL Widgets using Wireless Tag WT32-SC01 / Plus with Capacitive Touch
> [Wireless Tag WT32-SC01 / Plus (3.5" TFT Touch Display)](https://www.alibaba.com/product-detail/esp32-development-board-WT32-SC01-3_62534911683.html)   

## Get Started
> Git clone and recursively update submodule
```cmake
# Clone repo and update submodules (LovyanGFX + LVGL)
git clone https://github.com/sukesh-ak/WT32-SC01-PLUS-LVGL-IDF.git
cd WT32-SC01-PLUS-LVGL-IDF
git submodule update --init --recursive

# Update submodules (LovyanGFX + LVGL)
git submodule foreach git pull
```

## Separate build folder for ESP32 & ESP32-S3
> Check settings in CMakeLists.txt [here](CMakeLists.txt#L8)
#### WT32-SC01 - ESP32
```cmake
# set target and build,flash,monitor
idf.py -B build-esp32 set-target esp32 build
idf.py -B build-esp32 -p COM6 flash monitor
```

#### WT32-SC01 Plus - ESP32-S3
```cmake
# set target and build,flash,monitor
idf.py -B build-esp32s3 set-target esp32s3 build
idf.py -B build-esp32s3 -p COM3 app-flash monitor
```

## Setup custom lvgl config - ESP-IDF  
> Check settings in CMakeLists.txt [here](CMakeLists.txt#L16)
```cmake
#LVGL custom config file setup
idf_build_set_property(COMPILE_OPTIONS "-DLV_CONF_INCLUDE_SIMPLE=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-I../main" APPEND)
```

## Display Compile Time Information
Check settings in CMakeLists.txt [here](CMakeLists.txt#L25)  
```cmake
# Display Compile Time Information
message(STATUS "--------------Compile Info------------")
message(STATUS "IDF_PATH = ${IDF_PATH}")
message(STATUS "IDF_TARGET = ${IDF_TARGET}")
message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
message(STATUS "PROJECT_DIR = ${PROJECT_DIR}")
message(STATUS "BUILD_DIR = ${BUILD_DIR}")
message(STATUS "SDKCONFIG = ${SDKCONFIG}")
message(STATUS "SDKCONFIG_DEFAULTS = ${SDKCONFIG_DEFAULTS}")
message(STATUS "CONFIG_LV_CONF_SKIP = ${CONFIG_LV_CONF_SKIP}")
message(STATUS "COMPILE_OPTIONS = ${COMPILE_OPTIONS}")
message(STATUS "---------------------------------------")
message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "---------------------------------------")
```
