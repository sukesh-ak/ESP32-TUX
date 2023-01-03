#! /bin/bash
. activate.sh
# hack to prevent an error
touch ./managed_components/espressif__fmt/.component_hash
rm ./managed_components/espressif__fmt/.component_hash
idf.py build
idf.py flash monitor
