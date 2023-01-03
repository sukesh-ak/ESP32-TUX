#! /bin/bash
set -e
. activate.sh && ./build.sh && idf.py flash monitor
