
@echo off
rem if _IN_ENVIRONMENT is not set
if "%_IN_ENVIRONMENT%"=="" (
    rem Set environmental variable _IN_ENVIRONMENT = 1
    "C:\Espressif\idf_cmd_init.bat" esp-idf-121ffdbe0b35e1365bcc6c7122ca4a7a
    set _IN_ENVIRONMENT=1
    set IDF_DISABLE_COLOR=1
    rem Set the current directory to the directory of the script
    cd %~dp0
) else (
    echo Already in environment
)


