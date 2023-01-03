del .\managed_components\espressif__fmt\.component_hash 2>nul
@echo off
REM Look for --port argument and if found then set it as ESPPORT so that idf.py can use it
:loop
if "%~1" == "--port" (
  set "ESPPORT=%~2"
  shift
  shift
) else if not "%~1" == "" (
  shift
)
if not "%~1" == "" goto loop
:end
@echo on
activate.bat && idf.py build && idf.py flash monitor